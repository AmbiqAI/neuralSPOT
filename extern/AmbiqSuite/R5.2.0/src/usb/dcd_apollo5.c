/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Ambiq
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "tusb_option.h"

#if CFG_TUD_ENABLED && CFG_TUSB_MCU == OPT_MCU_APOLLO5

#include "device/dcd.h"
#include "am_mcu_apollo.h"
#include "am_util_delay.h"
#include "am_bsp.h"         // Declare BSP functions am_bsp_external_vddusb33_switch() and am_bsp_external_vddusb0p9_switch()

#ifdef CFG_TUD_SPEED_HS2FS_CONVER
extern tusb_speed_t tusb_request_speed_get(void);
#endif

/*------------------------------------------------------------------*/
/* MACRO TYPEDEF CONSTANT ENUM
 *------------------------------------------------------------------*/
static void *pUSBHandle = NULL;

static void dcd_usb_dev_evt_callback(am_hal_usb_dev_event_e eDevState) ;

#if defined(AM_PART_APOLLO5A)
static void dcd_usb_ep_xfer_complete_callback(const uint8_t ep_addr,
                                              const uint16_t xfer_len,
                                              am_hal_usb_xfer_code_e code,
                                              void *param) ;
#else
static void dcd_usb_ep_xfer_complete_callback(const uint8_t ep_addr,
                                              const uint32_t xfer_len,
                                              am_hal_usb_xfer_code_e code,
                                              void *param) ;
#endif
static void dcd_usb_ep0_setup_callback(uint8_t *setup) ;
static inline void dcd_ep_dbuf_configure(void);


//*****************************************************************************
//! @brief  Mapping&injecting the ambiq USB HAL event to TinyUSB USB stack
//!
//! @param eDevState
//*****************************************************************************
static void
dcd_usb_dev_evt_callback(am_hal_usb_dev_event_e eDevState)
{
  switch(eDevState)
  {
    case AM_HAL_USB_DEV_EVT_BUS_RESET:
      am_hal_usb_intr_usb_enable(pUSBHandle, USB_CFG2_SOFE_Msk|USB_CFG2_ResumeE_Msk|USB_CFG2_SuspendE_Msk|USB_CFG2_ResetE_Msk);
      am_hal_usb_ep_init(pUSBHandle, 0, 0, 64);
#ifdef CFG_TUD_SPEED_HS2FS_CONVER

      if (tusb_request_speed_get() == TUSB_SPEED_HIGH)
      {
          dcd_event_bus_reset(0, TUSB_SPEED_HIGH, true);
      }
      else
      {
          am_hal_usb_set_dev_speed(pUSBHandle, AM_HAL_USB_SPEED_FULL);
          dcd_event_bus_reset(0, TUSB_SPEED_FULL, true);
      }
#else //CFG_TUD_SPEED_HS2FS_CONVER
#if BOARD_DEVICE_RHPORT_SPEED == OPT_MODE_FULL_SPEED
      am_hal_usb_set_dev_speed(pUSBHandle, AM_HAL_USB_SPEED_FULL);
      dcd_event_bus_reset(0, TUSB_SPEED_FULL, true);
#else
      dcd_event_bus_reset(0, TUSB_SPEED_HIGH, true);
#endif
#endif  //CFG_TUD_SPEED_HS2FS_CONVER
      break;
    case AM_HAL_USB_DEV_EVT_RESUME:
      dcd_event_bus_signal(0, DCD_EVENT_RESUME, true);
      // Do something for resuming
      // then set the device state to active
      am_hal_usb_set_dev_state(pUSBHandle, AM_HAL_USB_DEV_STATE_ACTIVE);
      break;
    case AM_HAL_USB_DEV_EVT_SOF:
      dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
      break;
    case AM_HAL_USB_DEV_EVT_SUSPEND:
      dcd_event_bus_signal(0, DCD_EVENT_SUSPEND, true);
      // Do something for suspending
      // then set the device state to suspended
      am_hal_usb_set_dev_state(pUSBHandle, AM_HAL_USB_DEV_STATE_SUSPENDED);
      break;
    default:
      // Not reachable case
      // add to suppress the compiling warning
      break;
  }
}

//*****************************************************************************
//! @brief  Setup request is received and pass it to upper layer TinyUSB
//!         stack to handle
//!
//! @param setup
//*****************************************************************************
static void
dcd_usb_ep0_setup_callback(uint8_t *setup)
{
  dcd_event_setup_received(0, setup, true);
}

//*****************************************************************************
//! @brief
//!
//! @param ep_addr
//! @param xfer_len
//! @param code
//! @param param
//*****************************************************************************
#if defined(AM_PART_APOLLO5A)
static void dcd_usb_ep_xfer_complete_callback(const uint8_t ep_addr,
                                              const uint16_t xfer_len,
                                              am_hal_usb_xfer_code_e code,
                                              void *param)
#else
static void
dcd_usb_ep_xfer_complete_callback(const uint8_t ep_addr,
                                              const uint32_t xfer_len,
                                              am_hal_usb_xfer_code_e code,
                                              void *param)
#endif
{
    switch (code)
    {
        case USB_XFER_DONE:
            dcd_event_xfer_complete(0, ep_addr, xfer_len, XFER_RESULT_SUCCESS, true);
            break;
        case USB_XFER_STALL:
            dcd_event_xfer_complete(0, ep_addr, xfer_len, XFER_RESULT_STALLED, true);
            break;
        case USB_XFER_ABORT:
            break;
        case USB_XFER_DATA:
        case USB_XFER_UNSTALL:
        case USB_XFER_RESET:
        case USB_XFER_ERROR:
        default:
            if (xfer_len)
            {
                dcd_event_xfer_complete(0, ep_addr, xfer_len, XFER_RESULT_FAILED, true);
            }
            break;
    }
}

#undef AM_USB_CHARGER_DETECT
#ifdef AM_USB_CHARGER_DETECT

//*****************************************************************************
//! @brief
//!
//! @details The hardware detects a voltage higher than 4V on the VBUS
//! (USB power supply pin).
//! The hardware should provide a mechanism to make sure the VBUS voltage
//! can be compared against 4.0V.
//! This function is called only after the VBUS is more than 4.0V.
//! For example, the VBUS may come as the output of a comparator
//! (via interrupt or polling)
//!
//! @return am_hal_usb_charger_type_e
//*****************************************************************************
am_hal_usb_charger_type_e
am_hal_usb_BC1_2_good_battery_detection(void)
{
    int c, i;

    am_hal_usb_hardware_reset();

    //
    // Check VBUS voltage(USB power supply) is more than 4Volt.
    //
    am_util_delay_ms(10);

    am_hal_usb_charger_enable_data_pin_contact_detection();

    //
    // Check the data pin contact detection (DCD) for 900msec
    //
    c = 90;
    i = 2;
    while(c > 0)
    {
        //
        // Upon detecting VBUS greater than its VOTG_SESS_VLD threshold, a PD shall start a timer with a timeout
        // value of TDCD_TIMEOUT. A PD that supports DCD is allowed to enable its IDP_SRC and monitor for Dp being
        // at VLGC_LOW for TDCD_DBNC.
        // If the DCD timer expires before the Dp or ID conditions are detected, the PD shall proceed to Primary Detection.
        //
        c--;
        am_util_delay_ms(10);
        if( am_hal_usb_charger_data_pin_connection_status() == AM_HAL_USB_CHARGER_DATA_PIN_CONNECTED)
        {
            i--;

            //
            // Debouncing for data pins(Dm and Dp);T_DCD_DBNC(10msec) Data Contact Detect debounce
            //
            if(i == 0)
                break;
        }
        else
        {
            i = 2;
        }
    }

    //
    // Primary detection for good battery:
    //
    am_hal_usb_charger_enable_primary_detection();

    //
    // Start a timer for 100usec
    //
    c = 10;
    i = 3;
    am_util_delay_us(100);
    while(c > 0)
    {
        c--;
        am_util_delay_us(10);
        if (am_hal_usb_charger_sdp_connection_status() == AM_HAL_USB_CHARGER_SDP)
        {
            i--;
            if (i == 0)
            {
                //
                // pull-up the Dp pin to keep the current to 100mA(else the current may be reduced to 2.5mA)
                // or immidiately starts enumeration.
                // else the current can only be 2.5mA
                // "The SDP is detected"
                //
                return AM_HAL_USB_CHARGER_SDP;
            }
        }
    }

    //
    // Secondary detection for good battery:
    // If a PD Detects that it is attached to either a DCP or CDP during Primary Detection, and it is ready to be enumerated,
    // then it is allowed to take the branch where it connects. If a PD is not ready to be enumerated, then it is required
    // to do Secondary Detection.
    //
    am_hal_usb_charger_enable_secondary_detection();

    //
    // Start a timer for 100usec for analog circuit to settle down
    //
    am_util_delay_us(100);
    if (am_hal_usb_charger_cdp_or_dcp_connection_status() == AM_HAL_USB_CHARGER_CDP)
    {
        //
        // If a PD detects that Dp is less than VDAT_REF, it knows that it is attached to a CDP.
        // It is then required to turn off VDP_SRC and VDM_SRC, as shown in the Good Battery
        // Algorithm in Section 3.3.2, and is allowed to draw IDEV_CHG.
        //
        am_hal_usb_charger_enable_cdp_charger();

        //
        // "the CDP is detected"
        //
        return AM_HAL_USB_CHARGER_CDP;
    }

    if (am_hal_usb_charger_cdp_or_dcp_connection_status() == AM_HAL_USB_CHARGER_DCP)
    {
        //
        // If a PD detects that Dp is greater than VDAT_REF, it knows that it is attached to a DCP.
        // It is then required to enable VDP_SRC or pull Dp to VDP_UP through RDP_UP, as defined in
        // the Good Battery Algorithm in Section 3.3.2.
        //
        am_hal_usb_charger_enable_dcp_charger();
        // "the DCP is detected".
        return AM_HAL_USB_CHARGER_DCP;
    }

    return AM_HAL_USB_CHARGER_NO_CHARGER;
}

//*****************************************************************************
//! @brief
//!
//! @param bValid
//! @return am_hal_usb_charger_type_e
//*****************************************************************************
am_hal_usb_charger_type_e
dcd_usb_vbus_session(uint8_t rhport, bool bValid)
{
    (void) rhport;

    am_hal_usb_charger_type_e eChgType = AM_HAL_USB_CHARGER_NO_CHARGER;
    if (bValid == true)
    {
        //
        // VBUS session is valid
        //
        am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_WAKE, false);

        am_hal_usb_enable_phy_reset_override();

        eChgType = am_hal_usb_BC1_2_good_battery_detection();

        am_hal_usb_hardware_unreset();

        am_hal_usb_disable_phy_reset_override();

        if (eChgType == AM_HAL_USB_CHARGER_SDP)
        {

            //
            // Attach to USB host to start the enumeration
            //
            am_hal_usb_intr_usb_enable(pUSBHandle, USB_INTRUSB_Reset_Msk);
            dcd_connect(rhport);
        }
        else
        {
            //
            // Power off the USB peripheral for only power charging case
            //
            am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_NORMALSLEEP, false);
        }
    }
    else
    {
        //
        // VBUS session is invalid
        //
        dcd_disconnect(rhport);
        am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_NORMALSLEEP, false);
    }

    return eChgType;
}

static am_hal_usb_charger_type_e dcd_usb_charger_type;
#endif

/*------------------------------------------------------------------*/

#ifndef ENABLE_EXT_USB_PWR_RAILS

#if defined(apollo5_eb) || defined(apollo5b_eb_revb)

#define ENABLE_EXT_USB_PWR_RAILS

#endif
#endif // ENABLE_EXT_USB_PWR_RAILS


//*****************************************************************************
// This is called for a graceful USB power-up
//*****************************************************************************
void
dcd_power_up(uint8_t rhport)
{
    (void) rhport;

    //
    // enable internal power rail
    //
    am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_WAKE, false);

    //
    // clear USB PHY reset in MCU control registers
    //
    am_hal_usb_enable_phy_reset_override();


#ifdef ENABLE_EXT_USB_PWR_RAILS

    //
    // Enable the USB power rails
    //
    am_bsp_external_vddusb33_switch(true);
    am_bsp_external_vddusb0p9_switch(true);
    am_util_delay_ms(50);
#endif // ENABLE_EXT_USB_PWR_RAILS


#ifdef AM_USB_CHARGER_DETECT
    //
    // Simulate an PMIC or GPIO VBUS session interrupt
    //
    dcd_usb_charger_type = dcd_usb_vbus_session(rhport, true);

#else // !AM_USB_CHARGER_DETECT

    //
    // disable BC detection voltage source
    //
    am_hal_usb_hardware_unreset();

    //
    // set USB PHY reset disable
    //
    am_hal_usb_disable_phy_reset_override();

#ifdef CFG_TUD_SPEED_HS2FS_CONVER
    am_hal_usb_dev_speed_e eUsbSpeed ;

    if (tusb_request_speed_get()  == TUSB_SPEED_HIGH){
        eUsbSpeed = AM_HAL_USB_SPEED_HIGH;
    }
    else
    {
        eUsbSpeed = AM_HAL_USB_SPEED_FULL;
    }
#else //CFG_TUD_SPEED_HS2FS_CONVER
#if BOARD_DEVICE_RHPORT_SPEED == OPT_MODE_FULL_SPEED
    am_hal_usb_dev_speed_e eUsbSpeed = AM_HAL_USB_SPEED_FULL;
#else
    am_hal_usb_dev_speed_e eUsbSpeed = AM_HAL_USB_SPEED_HIGH;
#endif
#endif //CFG_TUD_SPEED_HS2FS_CONVER
    //
    // Override USB PHY Clock Selection if requested
    //
#if (defined(CFG_TUD_AM_USBPHY_CLK_SRC))
    #if defined(AM_PART_APOLLO5B)
    am_hal_usb_set_phy_clk_source(pUSBHandle, CFG_TUD_AM_USBPHY_CLK_SRC);
    #elif defined(AM_PART_APOLLO510L) && defined(CFG_TUD_AM_USBPHY_CLK_DIV)
    am_hal_usb_set_phy_clk_source(pUSBHandle, CFG_TUD_AM_USBPHY_CLK_SRC, CFG_TUD_AM_USBPHY_CLK_DIV);
    #endif
#endif

#if ((defined(AM_PART_APOLLO5B) && !defined(APOLLO5_FPGA)) || defined(AM_PART_APOLLO510L))
    am_hal_usb_phy_clock_enable(pUSBHandle, true, eUsbSpeed);
#endif

    am_hal_usb_set_dev_speed(pUSBHandle, eUsbSpeed);

    am_hal_usb_intr_usb_enable(pUSBHandle, USB_INTRUSB_Reset_Msk);

#endif // END AM_USB_CHARGER_DETECT

}

//*****************************************************************************
// This is called for a graceful USB powerdown
//*****************************************************************************
void dcd_power_down(uint8_t rhport)
{
  (void) rhport;
  //
  // disable USB interrupts
  //
  am_hal_usb_intr_usb_disable(pUSBHandle,
                              (USB_INTRUSB_SOF_Msk |
                                USB_INTRUSB_Reset_Msk |
                                USB_INTRUSB_Resume_Msk |
                                USB_INTRUSB_Suspend_Msk));

  //
  // disable high speed clock
  //
  am_hal_usb_dev_speed_e eUsbSpeed = AM_HAL_USB_SPEED_FULL;
  am_hal_usb_set_dev_speed(pUSBHandle, eUsbSpeed);

#if (defined(AM_PART_APOLLO5B) && !defined(APOLLO5_FPGA))
  am_hal_usb_phy_clock_enable(pUSBHandle, false, eUsbSpeed);
#endif

  am_hal_usb_enable_phy_reset_override();

#ifdef ENABLE_EXT_USB_PWR_RAILS

  //
  // Disable the USB power rails
  //
  am_bsp_external_vddusb33_switch(false);
  am_bsp_external_vddusb0p9_switch(false);
  am_util_delay_ms(50);
#endif // ENABLE_EXT_USB_PWR_RAILS

  am_hal_usb_power_control(pUSBHandle, AM_HAL_SYSCTRL_DEEPSLEEP, false);
}

//*****************************************************************************
// @brief  Controller API
//
// @param rhport
//*****************************************************************************
void
dcd_init (uint8_t rhport)
{
#if defined(AM_PART_APOLLO5A)
    //
    // Enable HFRC ADJ
    //
    uint32_t       ui32Regval =
                    _VAL2FLD(CLKGEN_HFADJ_HFADJMAXDELTA, CLKGEN_HFADJ_HFADJMAXDELTA_DISABLED) |
                    _VAL2FLD(CLKGEN_HFADJ_HFADJGAIN, CLKGEN_HFADJ_HFADJGAIN_Gain_of_1_in_2) |
                    _VAL2FLD(CLKGEN_HFADJ_HFWARMUP, CLKGEN_HFADJ_HFWARMUP_1SEC)             |
                    _VAL2FLD(CLKGEN_HFADJ_HFXTADJ, 0x5B8)                                   |
                    _VAL2FLD(CLKGEN_HFADJ_HFADJCK, CLKGEN_HFADJ_HFADJCK_4SEC)               |
                    _VAL2FLD(CLKGEN_HFADJ_HFADJEN, CLKGEN_HFADJ_HFADJEN_EN);

    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_HFADJ_ENABLE, &ui32Regval);

    //
    // Delay 10ms for HFRC ADJ to complete.
    //
    am_util_delay_ms(10);
#endif

    //
    // Powerup Sequence
    // see Apollo4 Errata ERR041: Induced D+ output pulse may cause
    // unintended disconnect.
    //

    uint32_t initStat = am_hal_usb_initialize(0, (void *) &pUSBHandle);
    if (initStat != AM_HAL_STATUS_SUCCESS) return;

    //
    // Register the callback functions
    //
    am_hal_usb_register_dev_evt_callback(pUSBHandle, dcd_usb_dev_evt_callback);
    am_hal_usb_register_ep0_setup_received_callback(pUSBHandle, dcd_usb_ep0_setup_callback);
    am_hal_usb_register_ep_xfer_complete_callback(pUSBHandle, dcd_usb_ep_xfer_complete_callback);


#if defined (AM_PART_APOLLO5B)
#if defined (AM_CFG_USB_DMA_MODE_0)
  am_hal_usb_set_xfer_mode(pUSBHandle, AM_HAL_USB_OUT_DMA_MODE_0);
  am_hal_usb_set_xfer_mode(pUSBHandle, AM_HAL_USB_IN_DMA_MODE_0);
#elif defined (AM_CFG_USB_DMA_MODE_1)
  am_hal_usb_set_xfer_mode(pUSBHandle, AM_HAL_USB_OUT_DMA_MODE_1);
  am_hal_usb_set_xfer_mode(pUSBHandle, AM_HAL_USB_IN_DMA_MODE_1);
#endif
#endif

#if defined(AM_PART_APOLLO5B)
   dcd_ep_dbuf_configure();
#endif

    dcd_power_up(rhport);
    dcd_connect(rhport);
}

//*****************************************************************************
//
//! @brief shutdown and powerdown the usb
//!
//! @param rhport
//
//*****************************************************************************
void
dcd_deinit(uint8_t rhport)
{
  //
  // detach from the host
  //
  dcd_disconnect(rhport);
  //
  // wait for any interrupts to happen and be processed
  //
  am_util_delay_ms(50);

  dcd_power_down(rhport);
  am_hal_usb_deinitialize(pUSBHandle);
  pUSBHandle = NULL;
}

//*****************************************************************************
// @brief enable device interrupt
//
//*****************************************************************************
void
dcd_int_enable(uint8_t rhport)
{
  (void) rhport;
  NVIC_SetPriority(USB0_IRQn, AM_IRQ_PRIORITY_DEFAULT);
  NVIC_EnableIRQ(USB0_IRQn);
}

//*****************************************************************************
// @brief disable device interrupt
//
//*****************************************************************************
void
dcd_int_disable(uint8_t rhport)
{
  (void) rhport;
  NVIC_DisableIRQ(USB0_IRQn);
}

//*****************************************************************************
//
// @brief Receive Set Address request, mcu port must also include status IN response
//
//
//*****************************************************************************
void
dcd_set_address (uint8_t rhport, uint8_t dev_addr)
{
  // Response with status first before changing device address
  dcd_edpt_xfer(rhport, tu_edpt_addr(0, TUSB_DIR_IN), NULL, 0);
  am_hal_usb_set_addr(pUSBHandle, dev_addr);
  am_hal_usb_set_dev_state(pUSBHandle, AM_HAL_USB_DEV_STATE_ADDRESSED);
}

//*****************************************************************************
// @brief
//
//*****************************************************************************
void
dcd_set_config (uint8_t rhport, uint8_t config_num)
{
  (void) rhport;
  (void) config_num;
  am_hal_usb_set_dev_state(pUSBHandle, AM_HAL_USB_DEV_STATE_CONFIGED);
}

//*****************************************************************************
//
// @brief Wake up host
//
//*****************************************************************************
void
dcd_remote_wakeup(uint8_t rhport)
{
  (void) rhport;
  am_hal_usb_start_remote_wakeup(pUSBHandle);
}

/*------------------------------------------------------------------*/
/* DCD Endpoint port
 *------------------------------------------------------------------*/


//*****************************************************************************
// @brief Configure endpoint's registers according to descriptor
//
//*****************************************************************************
bool
dcd_edpt_open (uint8_t rhport, tusb_desc_endpoint_t const * desc_edpt)
{
  (void) rhport;

  return am_hal_usb_ep_init((void *)pUSBHandle,
                            desc_edpt->bEndpointAddress,
                            (uint8_t)(desc_edpt->bmAttributes.xfer),
                            (uint16_t)(desc_edpt->wMaxPacketSize)) == AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// @brief Close all non-control endpoints, cancel all pending transfers if any.
// @note Invoked when switching from a non-zero Configuration by SET_CONFIGURE
// therefore required for multiple configuration support.
//
//*****************************************************************************
void
dcd_edpt_close_all (uint8_t rhport)
{
  (void) rhport;
}

//*****************************************************************************
//
// @brief Close an endpoint.
//
//*****************************************************************************
void
dcd_edpt_close(uint8_t rhport, uint8_t ep_addr)
{
    (void) rhport;
    (void) ep_addr;
}

//*****************************************************************************
//
// @brief Submit a transfer, When complete dcd_event_xfer_complete()
// is invoked to notify the stack
//
//*****************************************************************************
#if defined (AM_PART_APOLLO5B)
bool
dcd_edpt_xfer (uint8_t rhport,
               uint8_t ep_addr,
               uint8_t * buffer,
               uint32_t total_bytes)
#else
bool
dcd_edpt_xfer (uint8_t rhport,
               uint8_t ep_addr,
               uint8_t * buffer,
               uint16_t total_bytes)
#endif
{
  (void) rhport;


  #if ((!defined(SSRAM_NON_CACHEABLE)) && ((defined(AM_CFG_USB_DMA_MODE_0) || defined(AM_CFG_USB_DMA_MODE_1))))
  if (buffer && total_bytes && (tu_edpt_number(ep_addr) != 0))
  {
    am_hal_cachectrl_range_t sRange;
    sRange.ui32StartAddr = (uint32_t)buffer;
    sRange.ui32Size = total_bytes;
    if(tu_edpt_dir(ep_addr) == TUSB_DIR_IN)
    {
      am_hal_cachectrl_dcache_clean(&sRange);
    }
    else
    {
      am_hal_cachectrl_dcache_invalidate(&sRange, false);
    }
  }
  #endif

  return am_hal_usb_ep_xfer(pUSBHandle,
                            ep_addr,
                            buffer,
                            total_bytes) == AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// @brief Stall endpoint, any queuing transfer should be removed from endpoint
//
//*****************************************************************************
void
dcd_edpt_stall (uint8_t rhport, uint8_t ep_addr)
{
  (void) rhport;

  am_hal_usb_ep_stall(pUSBHandle, ep_addr);
}

//*****************************************************************************
//
// @brief  clear stall, data toggle is also reset to DATA0
//
//*****************************************************************************
void
dcd_edpt_clear_stall (uint8_t rhport, uint8_t ep_addr)
{
  am_hal_usb_ep_clear_stall(pUSBHandle, ep_addr);
}

//*****************************************************************************
// @brief  Enable/Disable Start-of-frame interrupt. Default is disabled
//*****************************************************************************
void dcd_sof_enable(uint8_t rhport, bool en)
{
    (void) rhport;

    if(en)
    {
        am_hal_usb_enable_sof_intr(pUSBHandle);
    }
    else
    {
        am_hal_usb_disable_sof_intr(pUSBHandle);
    }
}

//*****************************************************************************
// @brief Connect by enabling internal pull-up resistor on D+/D-
//
//*****************************************************************************
void
dcd_connect(uint8_t rhport)
{
    (void) rhport;
    am_hal_usb_attach(pUSBHandle);
}

//*****************************************************************************
// @brief Disconnect by disabling internal pull-up resistor on D+/D-
//
//*****************************************************************************
TU_ATTR_USED  void
dcd_disconnect(uint8_t rhport)
{
    (void) rhport;
    am_hal_usb_detach(pUSBHandle);
#ifdef CFG_TUD_SPEED_HS2FS_CONVER
    dcd_event_bus_signal(0, DCD_EVENT_UNPLUGGED, false);
#endif
}

#if defined(AM_PART_APOLLO5B)
//*****************************************************************************
//
//! @brief Configure Double-Buffer settings to HAL
//
//*****************************************************************************
static inline void
dcd_ep_dbuf_configure(void)
{
    #ifdef AM_CFG_USB_EP1_IN_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 1, AM_HAL_USB_IN_DIR, true);
    #endif
    #ifdef AM_CFG_USB_EP1_OUT_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 1, AM_HAL_USB_OUT_DIR, true);
    #endif
    #ifdef AM_CFG_USB_EP2_IN_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 2, AM_HAL_USB_IN_DIR, true);
    #endif
    #ifdef AM_CFG_USB_EP2_OUT_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 2, AM_HAL_USB_OUT_DIR, true);
    #endif
    #ifdef AM_CFG_USB_EP3_IN_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 3, AM_HAL_USB_IN_DIR, true);
    #endif
    #ifdef AM_CFG_USB_EP3_OUT_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 3, AM_HAL_USB_OUT_DIR, true);
    #endif
    #ifdef AM_CFG_USB_EP4_IN_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 4, AM_HAL_USB_IN_DIR, true);
    #endif
    #ifdef AM_CFG_USB_EP4_OUT_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 4, AM_HAL_USB_OUT_DIR, true);
    #endif
    #ifdef AM_CFG_USB_EP5_IN_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 5, AM_HAL_USB_IN_DIR, true);
    #endif
    #ifdef AM_CFG_USB_EP5_OUT_DBUF_ENABLE
    am_hal_usb_enable_ep_double_buffer(pUSBHandle, 5, AM_HAL_USB_OUT_DIR, true);
    #endif
}
#endif

//*****************************************************************************
//
// the usb isr
//
//*****************************************************************************
void
am_usb_isr(void)
{

    #if defined(AM_PART_APOLLO5A)
    uint32_t ui32IntStatus[3];
    am_hal_usb_intr_status_get(pUSBHandle,
                               &ui32IntStatus[0],
                               &ui32IntStatus[1],
                               &ui32IntStatus[2]);
    am_hal_usb_interrupt_service(pUSBHandle,
                                 ui32IntStatus[0],
                                 ui32IntStatus[1],
                                 ui32IntStatus[2]);

    #else
    uint32_t ui32IntStatus[5];
    am_hal_usb_intr_status_get(pUSBHandle,
                               &ui32IntStatus[0],
                               &ui32IntStatus[1],
                               &ui32IntStatus[2],
                               &ui32IntStatus[3],
                               &ui32IntStatus[4]);
    am_hal_usb_interrupt_service(pUSBHandle,
                                 ui32IntStatus[0],
                                 ui32IntStatus[1],
                                 ui32IntStatus[2],
                                 ui32IntStatus[3],
                                 ui32IntStatus[4]);
    #endif
}

#endif // CFG_TUD_ENABLED && CFG_TUSB_MCU == OPT_MCU_APOLLO5
