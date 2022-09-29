/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem Module  R0.14                               /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2020, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "diskio.h"
#include "mmc_apollo4.h"

static volatile DSTATUS Stat = STA_NOINIT;    /* Disk status */
static volatile bool bAsyncWriteIsDone = false;
static volatile bool bAsyncReadIsDone  = false;
am_hal_card_t eMMCard;
am_hal_card_host_t *pSdhcCardHost = NULL;
DWORD sec_count = 0;


/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

//*****************************************************************************
//
// Interrupt handler for SDIO
//
//*****************************************************************************
void am_sdio_isr(void)
{
    uint32_t ui32IntStatus;

    am_hal_sdhc_intr_status_get(pSdhcCardHost->pHandle, &ui32IntStatus, true);
    am_hal_sdhc_intr_status_clear(pSdhcCardHost->pHandle, ui32IntStatus);
    am_hal_sdhc_interrupt_service(pSdhcCardHost->pHandle, ui32IntStatus);
}

//*****************************************************************************
//
// Card event call back function
//
//*****************************************************************************
void am_hal_card_event_test_cb(am_hal_host_evt_t *pEvt)
{
    am_hal_card_host_t *pHost = (am_hal_card_host_t *)pEvt->pCtx;

    if (AM_HAL_EVT_XFER_COMPLETE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_READ)
    {
        bAsyncReadIsDone = true;
        am_util_debug_printf("Last Read Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_XFER_COMPLETE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_WRITE)
    {
        bAsyncWriteIsDone = true;
        am_util_debug_printf("Last Write Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_SDMA_DONE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_READ)
    {
        am_util_debug_printf("SDMA Read Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_SDMA_DONE == pEvt->eType &&
        pHost->AsyncCmdData.dir == AM_HAL_DATA_DIR_WRITE)
    {
        am_util_debug_printf("SDMA Write Xfered block %d\n", pEvt->ui32BlkCnt);
    }

    if (AM_HAL_EVT_CARD_PRESENT == pEvt->eType)
    {
        am_util_debug_printf("A card is inserted\n");
    }
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS mmc_disk_initialize (void)
{
    DWORD i = 0;

    if ( Stat == STA_NOINIT )
    {
        //
        // Get the SDHC card host instance
        //
        pSdhcCardHost = am_hal_get_card_host(AM_HAL_SDHC_CARD_HOST, true);

        if (pSdhcCardHost == NULL)
        {
            am_util_debug_printf("Error: No card host found!\n");
            Stat |= STA_NOINIT;
            return Stat;
        }

        am_util_debug_printf("card host is found\n");

        //
        // check if card is present
        //
        while (am_hal_card_host_find_card(pSdhcCardHost, &eMMCard) != AM_HAL_STATUS_SUCCESS)
        {
            am_util_debug_printf("No card is present now\n");
            am_util_delay_ms(10);
            am_util_debug_printf("Checking if card is available again\n");
            i++;
            if ( i ==  100 )
            {
                Stat |= STA_NOINIT;
                return Stat;
            }
        }

        //
        // Initialize the card
        //
        i = 0;
        while (am_hal_card_init(&eMMCard, NULL, AM_HAL_CARD_PWR_CTRL_SDHC_OFF) != AM_HAL_STATUS_SUCCESS)
        {
            am_util_delay_ms(10);
            am_util_debug_printf("card and host is not ready, try again\n");
            i++;
            if ( i ==  100 )
            {
                Stat |= STA_NOINIT;
                return Stat;
            }
        }

        //
        // Set ADMA transfer mode
        //
        am_hal_card_host_set_xfer_mode(pSdhcCardHost, AM_HAL_HOST_XFER_ADMA);

        //
        // async read & write, card insert & remove needs a callback function
        //
        am_hal_card_register_evt_callback(&eMMCard, am_hal_card_event_test_cb);

        //
        // Get the sector count from eMMC
        //
        sec_count = am_hal_card_get_ext_csd_field(&eMMCard, 212, 4);
        if ( sec_count == 0 )
        {
            am_util_debug_printf("Fail to get the sector counter of eMMC\n");
            Stat |= STA_NOINIT;
            return Stat;
        }
        else
        {
            am_util_debug_printf("eMMC EXT CSD Sector Count is: 0x%x\n", sec_count);
        }

        //
        // Configure the card
        //
        if ( am_hal_card_cfg_set(&eMMCard, AM_HAL_CARD_TYPE_EMMC,
                AM_HAL_HOST_BUS_WIDTH_8, 48000000, AM_HAL_HOST_BUS_VOLTAGE_1_8,
                AM_HAL_HOST_UHS_SDR50) != AM_HAL_STATUS_SUCCESS )
        {
            am_util_debug_printf("Fail to configure the card\n");
            Stat |= STA_NOINIT;
            return Stat;
        }

#ifdef ENABLE_SDIO_POWER_SAVE
        //
        // power down SDIO peripheral
        //
        if ( am_hal_card_pwrctrl_sleep(&eMMCard) != AM_HAL_STATUS_SUCCESS )
        {
            am_util_debug_printf("Fail to power down SDIO peripheral\n");
            Stat |= STA_NOINIT;
            return Stat;
        }
        else
        {
            am_util_debug_printf("Power down SDIO peripheral\n");
        }
#endif

        Stat &= ~STA_NOINIT;
    }

    return Stat;
}


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS mmc_disk_status (void)
{
    return Stat;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT mmc_disk_read (
    BYTE *buff,            /* Pointer to the data buffer to store read data */
    LBA_t sector,                /* Start sector number (LBA) */
    UINT count            /* Sector count (1..128) */
)
{
    DWORD i = 0;
    DWORD ui32Status;

    if ( !count )
    {
        return RES_PARERR;
    }

    if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

#ifdef ENABLE_SDIO_POWER_SAVE
    //
    // power up SDIO peripheral
    //
    if ( am_hal_card_pwrctrl_wakeup(&eMMCard) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_debug_printf("Fail to power up SDIO peripheral\n");
        return RES_ERROR;
    }
    else
    {
        am_util_debug_printf("Power up SDIO peripheral\n");
    }
#endif

    am_util_debug_printf("Read Data\n");
    am_util_debug_printf("count = %d  sector= %d\n", count, sector);

    //
    // Calling Read API
    //
    bAsyncReadIsDone = false;
    ui32Status = am_hal_card_block_read_async(&eMMCard, sector, count, (uint8_t *)buff);
    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        am_util_debug_printf("Fail to call read API. Read Status = %d\n", ui32Status);
        return RES_ERROR;
    }

    //
    // wait until the async read is done
    //
    while (!bAsyncReadIsDone)
    {
        am_util_delay_ms(1);
        i++;
        if ( i == 1000 )
        {
            am_util_debug_printf("Read Timeout\n");
            return RES_ERROR;
        }
    }

    am_util_debug_printf("Read Completed \n");

#ifdef ENABLE_SDIO_POWER_SAVE
    //
    // power down SDIO peripheral
    //
    if ( am_hal_card_pwrctrl_sleep(&eMMCard) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_debug_printf("Fail to power down SDIO peripheral\n");
        return RES_ERROR;
    }
    else
    {
        am_util_debug_printf("Power down SDIO peripheral\n");
    }
#endif

    return RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT mmc_disk_write (
    const BYTE *buff,    /* Pointer to the data to be written */
    LBA_t sector,        /* Start sector number (LBA) */
    UINT count        /* Sector count (1..128) */
)
{
    BYTE * wr_buf;
    DWORD i = 0;
    DWORD ui32Status;

    if ( !count )
    {
        return RES_PARERR;
    }

    if ( Stat & STA_NOINIT )
    {
        return RES_NOTRDY;
    }

#ifdef ENABLE_SDIO_POWER_SAVE
    //
    // power up SDIO peripheral
    //
    if ( am_hal_card_pwrctrl_wakeup(&eMMCard) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_debug_printf("Fail to power up SDIO peripheral\n");
        return RES_ERROR;
    }
    else
    {
        am_util_debug_printf("Power up SDIO peripheral\n");
    }
#endif

    am_util_debug_printf("Write Data\n");
    am_util_debug_printf("count = %d  sector= %d\n", count, sector);

    wr_buf = (BYTE *)buff;

    //
    // Calling Write API
    //
    bAsyncWriteIsDone = false;
    ui32Status = am_hal_card_block_write_async(&eMMCard, sector, count, (uint8_t *)wr_buf);
    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        am_util_debug_printf("Fail to call write API. Write Status = %d\n", ui32Status);
        return RES_ERROR;
    }

    //
    // wait until the async write is done
    //
    while (!bAsyncWriteIsDone)
    {
        am_util_delay_ms(1);
        i++;
        if ( i == 1000 )
        {
            am_util_debug_printf("Write Timeout\n");
            return RES_ERROR;
        }
    }

    am_util_debug_printf("Write Completed \n");

#ifdef ENABLE_SDIO_POWER_SAVE
    //
    // power down SDIO peripheral
    //
    if ( am_hal_card_pwrctrl_sleep(&eMMCard) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_debug_printf("Fail to power down SDIO peripheral\n");
        return RES_ERROR;
    }
    else
    {
        am_util_debug_printf("Power down SDIO peripheral\n");
    }
#endif

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT mmc_disk_ioctl (
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
       DRESULT res = RES_ERROR;

#if FF_USE_TRIM
    LBA_t *range;
    DWORD st, ed;
#endif
#if _USE_ISDIO
    SDIO_CTRL *sdi;
    BYTE rc, *bp;
    UINT dc;
#endif

    if (Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

    switch (cmd)
    {
        /* Make sure that no pending write process */
        case CTRL_SYNC :
            res = RES_OK;
            break;

        /* Get number of sectors on the disk (DWORD) */
        case GET_SECTOR_COUNT :
            *(DWORD*)buff = sec_count;
            res = RES_OK;
            break;

        /* Get erase block size in unit of sector (DWORD) */
        case GET_BLOCK_SIZE :
            *(DWORD*)buff = 1;
            res = RES_OK;
            break;

        default:
            res = RES_PARERR;
    }

    return res;
}

