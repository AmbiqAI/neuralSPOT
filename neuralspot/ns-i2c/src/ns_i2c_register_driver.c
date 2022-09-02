/**
 * @file ns_i2c_register_driver.c
 * @author Carlos Morales
 * @brief Generic i2c driver for register-based i2c devices
 * @version 0.1
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "am_hal_iom.h"
#include "ns_i2c_register_driver.h"

#define NS_I2C_MAX_DEVICE 1
static ns_i2cDriverConfig_t ns_i2cDriverConfig[NS_I2C_MAX_DEVICE+1]; 

uint32_t
ns_i2c_reg_read(ns_i2cDriverConfig_t *h, uint32_t regAddr, void *pBuf, uint32_t size)
{
    am_hal_iom_transfer_t       Transaction;

    Transaction.ui32InstrLen    = 1;
    Transaction.ui64Instr       = regAddr;
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes    = size;
    Transaction.pui32RxBuffer   = (uint32_t*)pBuf;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    Transaction.uPeerInfo.ui32I2CDevAddr = h->address;
    if (am_hal_iom_blocking_transfer(h->iom_handle, &Transaction))
    {
         return NS_I2C_STATUS_ERROR;
    }

    return NS_I2C_STATUS_SUCCESS;
}

uint32_t
ns_i2c_reg_write(ns_i2cDriverConfig_t *h, uint32_t regAddr, void *pBuf, uint32_t size)
{
    am_hal_iom_transfer_t       Transaction;

    Transaction.ui8Priority     = 1;
    Transaction.ui32InstrLen    = 1;
    Transaction.ui64Instr       = regAddr;
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes    = size;
    Transaction.pui32TxBuffer   = (uint32_t*)pBuf;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    Transaction.uPeerInfo.ui32I2CDevAddr = h->address;
    if (am_hal_iom_blocking_transfer(h->iom_handle, &Transaction) != AM_HAL_STATUS_SUCCESS)
    {
         return NS_I2C_STATUS_ERROR;
    }

    return NS_I2C_STATUS_SUCCESS;
}

uint32_t
ns_i2c_write_bits(ns_i2cDriverConfig_t *h, uint8_t regAddr, uint8_t val, uint8_t bitOffset, uint8_t len) {
	uint32_t regVal;
    //uint8_t finalRegVal;
    if(ns_i2c_reg_read(h, regAddr, (uint8_t*)&regVal, 1)) {
        return NS_I2C_STATUS_ERROR;
    }

	int i = len - 1;
	uint16_t curr;
	while (i >= 0) {
	    curr = val & 1;
	    val = val >> 1;
	    regVal = regVal & (0xff & ~(1<<(bitOffset - i)));
	    regVal = regVal | (curr<<(bitOffset - i));
	    i -= 1;
        }

    return NS_I2C_STATUS_SUCCESS;
}

uint32_t
ns_i2c_interface_init(uint32_t ui32Module, uint8_t devAddress, void **pIomHandle)
{
    if ( ui32Module > NS_I2C_MAX_DEVICE )
    {
        return NS_I2C_STATUS_ERROR;
    }

    // Initialize local state
    ns_i2cDriverConfig[ui32Module].address = devAddress;
    ns_i2cDriverConfig[ui32Module].iom = ui32Module;
    ns_i2cDriverConfig[ui32Module].sIomCfg.eInterfaceMode = AM_HAL_IOM_I2C_MODE;
    ns_i2cDriverConfig[ui32Module].sIomCfg.ui32ClockFreq  = AM_HAL_IOM_100KHZ;
    ns_i2cDriverConfig[ui32Module].sIomCfg.pNBTxnBuf = NULL;
    ns_i2cDriverConfig[ui32Module].sIomCfg.ui32NBTxnBufLength = 0; 

    am_hal_pwrctrl_periph_enable((am_hal_pwrctrl_periph_e)(AM_HAL_PWRCTRL_PERIPH_IOM0 + ui32Module));

    // Initialize the IOM instance.
    // Enable power to the IOM instance.
    // Configure the IOM for Serial operation during initialization.
    // Enable the IOM.
    // HAL Success return is 0
    //
    if (am_hal_iom_initialize(ui32Module, &(ns_i2cDriverConfig[ui32Module].iom_handle)) ||
        am_hal_iom_power_ctrl(ns_i2cDriverConfig[ui32Module].iom_handle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(ns_i2cDriverConfig[ui32Module].iom_handle, &(ns_i2cDriverConfig[ui32Module].sIomCfg)) ||
        am_hal_iom_enable(ns_i2cDriverConfig[ui32Module].iom_handle))
    {
        return NS_I2C_STATUS_ERROR;
    }

    //
    // Configure the IOM pins.
    //
    am_bsp_iom_pins_enable(ui32Module, AM_HAL_IOM_I2C_MODE);

    *pIomHandle = &(ns_i2cDriverConfig[ui32Module]);
    return NS_I2C_STATUS_SUCCESS;
}