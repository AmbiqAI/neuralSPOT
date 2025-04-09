# Ambiq TinyUSB User Guide

This user guide explains the TinyUSB porting layer for Apollo5 family, `dcd_apollo5`, and the customizations available to application developers.

## Table of Content

- [USB PHY Clock Source Selection](#usb-phy-clock-source-selection)
- [Non-Control Transfer Modes](#non-control-transfer-modes)
    - [PIO Mode](#pio-mode)
    - [DMA0 Mode](#dma0-mode)
    - [DMA1 Mode](#dma1-mode)
- [Double Endpoint Buffer acceleration](#double-endpoint-buffer-acceleration)
    - [Double Endpoint Buffer Limitation by Total FIFO Size](#double-endpoint-buffer-limitation-by-total-fifo-size)
        - [Example FIFO Buffer Size Calculation](#example-fifo-buffer-size-calculation)
    - [Double Endpoint Buffer Configuration for Ambiq TinyUSB Porting](#double-endpoint-buffer-configuration-for-ambiq-tinyusb-porting)
- [Cache Coherency Handling](#cache-coherency-handling)
    - [Cache Coherency Handlings in Ambiq TinyUSB Port](#cache-coherency-handlings-in-ambiq-tinyusb-port)
- [CDC Throughput Optimization](#cdc-throughput-optimization)
- [Control Transfer STATUS ACK Optimization](#control-transfer-status-ack-optimization)

## USB PHY Clock Source Selection

<a name="usb-phy-clock-source-selection"></a>

For Apollo5, a numbers of clock can be selected to be the reference clock for USB PHY, which would be used by USB PLL to generate its interface clocks. In Ambiq USB HAL, the clock selection defaults to `AM_HAL_USB_PHYCLKSRC_DEFAULT` upon power up.

For Ambiq TinyUSB porting, to change the selection, define `CFG_TUD_AM_USBPHY_CLK_SRC` with the desired clock source. Once `CFG_TUD_AM_USBPHY_CLK_SRC` is defined, `dcd_apollo5` will overwrite the HAL default with the selected source during `dcd_power_up()`.

Followings are the Clock sources available in Ambiq TinyUSB port:

| USB PHY Clock Source                  | Description                               |
| ------------------------------------- | ----------------------------------------- |
| AM_HAL_USB_PHYCLKSRC_DEFAULT          | [HS Mode] SYSPLL </br>[FS Mode] HFRC_24M  |
| AM_HAL_USB_PHYCLKSRC_HFRC_24M         | [HS & FS Mode] HFRC_24M Mode              |
| AM_HAL_USB_PHYCLKSRC_EXTREFCLK        | [HS & FS Mode] EXTREF_CLK                 |
| AM_HAL_USB_PHYCLKSRC_EXTREFCLK_DIV2   | [HS & FS Mode] EXTREF_CLK / 2             |
| AM_HAL_USB_PHYCLKSRC_XTAL_HS          | [HS & FS Mode] XTAL_HS                    |
| AM_HAL_USB_PHYCLKSRC_XTAL_HS_DIV2     | [HS & FS Mode] XTAL_HS / 2                |
| AM_HAL_USB_PHYCLKSRC_PLL              | [HS & FS Mode] SYSPLL                     |

## Non-Control Transfer Modes

<a name="non-control-transfer-modes"></a>

There are 3 transfer modes available for non-control endpoint for Apollo5: PIO, DMA0 and DMA1.

In Ambiq's TinyUSB porting, the transfer mode for non-control endpoint is controlled by 2 flags:

- `AM_CFG_USB_DMA_MODE_0`
- `AM_CFG_USB_DMA_MODE_1`

DMA1 provides the highest throughput followed by DMA0, and PIO mode. However, there are pros and cons for each of the modes as described in the sub-sections below. Application developer should weigh on the modes available and select the transfer mode that best suit the application.

### PIO Mode

<a name="pio-mode"></a>

PIO mode is in force when both `AM_CFG_USB_DMA_MODE_0` and `AM_CFG_USB_DMA_MODE_1` are NOT defined. In PIO mode, USB HAL actively loads/unloads USB FIFO to transmit/receive data without utilizing USB DMA engines.

Pros :

- Can be used with all Endpoint types, i.e.: Interrupt, Bulk, Isochronous
- No cache coherency handling is needed for USB interface since CPU is the bus master to load/unload USB FIFO.

Cons:

- PIO Mode is a CPU intensive mode, hence it would be suboptimal for high data throughput applications.

### DMA0 Mode

<a name="dma0-mode"></a>

DMA0 mode is in force when `AM_CFG_USB_DMA_MODE_0` is defined, regardless whether `AM_CFG_USB_DMA_MODE_1` is defined.

In DMA0 mode, DMA request is to be issued to DMA engine for each USB packets to be transmitted/received. Hence, USB HAL would need to generate DMA request and service DMA completion interrupt individually for each USB packet. Which is the main reason that the throughput is not as great as DMA1 mode.

Pros :

- Can be used with all Endpoint types, i.e.: Interrupt, Bulk, Isochronous
- MCU is not occupied when loading/unloading of USB FIFO.

Cons:

- Cache coherency handling is needed if endpoint buffer is located in cacheable memory region, as there are 2 memory bus masters involved, i.e. CPU and DMA.

### DMA1 Mode

<a name="dma1-mode"></a>

DMA1 mode is in force when `AM_CFG_USB_DMA_MODE_1` is defined, and `AM_CFG_USB_DMA_MODE_0` is not defined.

In DMA1 Mode, DMA request can be issued to DMA engine to move multiple packets at once, hence giving the highest throughput among all the modes available. However, there are few drawbacks for DMA1 mode as outlined below.

Pros :

- Highest throughput among the 3 transfer modes available.

Cons:

- DMA1 mode cannot be selected when any one of the endpoint is of type: Asynchronous.
- In DMA1 mode, USB HAL needs to actively poll and check whether USB host has stopped sending data, which introduces a load to CPU when USB is idle.
- Cache coherency handling is needed if endpoint buffer is located in cacheable memory region, as there are 2 memory bus masters involved, i.e. CPU and DMA.

## Double Endpoint Buffer acceleration

<a name="double-endpoint-buffer-acceleration"></a>

In additional to DMA, Apollo 5 offers further performance optimization through a feature called Double Endpoint Buffer (DEB). When this feature is enabled, it creates a ping-pong endpoint FIFO that allows USB hardware to continue data transfer back to back, without the need to wait for DMA/CPU to complete FIFO loading/unloading.

DEB can be configured for each transfer direction of each endpoint separately. For clarity, we will refer to a transfer direction of a specific endpoint as endpoint-direction.

### Double Endpoint Buffer Limitation by Total FIFO Size

<a name="double-endpoint-buffer-limitation-by-total-fifo-size"></a>

The numbers of endpoint-direction that could enable DEB is limited by total Endpoint FIFO size available. For Apollo5, the total endpoint FIFO available for allocation is 4KB.

Application developer should populate the DEB configuration carefully based on the actual use case and make sure that the 4KB limit is not exceeded. Follow the following steps to evaluate whether DEB config is correct in your TinyUSB application:

1. Reserve 128 Bytes for Control Endpoint (EP0), i.e. 64 bytes for EP0-IN, and 64 bytes for EP0-OUT.
1. Find endpoint buffer size from `desc_hs_configuration` / `desc_fs_configuration` according to USB speed configured by `BOARD_DEVICE_RHPORT_SPEED`.
    - For endpoint-direction which DEB is to be enabled, the endpoint FIFO size reserved is 2 times of wMaxPacketSize field.
    - For endpoint-direction which DEB is to be disabled, the endpoint FIFO size reserved is equal to wMaxPacketSize field.
1. Take note that only the following lengths are available for FIFO size configuration for an endpoint-direction. Hence, do make sure that the wMaxPacketSize is one of the values:8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096.
1. Add all of the allocated buffer size from step 1 and 2. Make sure that the total allocated endpoint FIFO size doesn't exceed 4096 bytes.

#### Example FIFO Buffer Size Calculation

<a name="example-fifo-buffer-size-calculation"></a>

For Example:
The following `desc_hs_configuration` is used, and DEB is to be enabled for both IN and OUT endpoint for MSC interface.

``` c
#define EPNUM_CDC_NOTIF   0x81
#define EPNUM_CDC_OUT     0x02
#define EPNUM_CDC_IN      0x83
#define EPNUM_MSC_OUT     0x04
#define EPNUM_MSC_IN      0x85

uint8_t const desc_hs_configuration[] =
{
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 512),

    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 5, EPNUM_MSC_OUT, EPNUM_MSC_IN, 512),
};
```

Amounts to 3208 bytes of USB endpoint FIFO usage, which is lesser than 4096 bytes. Hence, this DEB configuration is good for use.

| Endpoint-Direction | wMaxPacketSize | DEB Enabled? | FIFO Size |
| ------------------ | -------------- | ------------ | --------- |
| EP0-IN             | 64             | N/A          | 64        |
| EP0-OUT            | 64             | N/A          | 64        |
| EP1-IN (0x81)      | 8              | NO           | 8         |
| EP2-OUT (0x02)     | 512            | NO           | 512       |
| EP3-IN  (0x83)     | 512            | NO           | 512       |
| EP4-OUT (0x04)     | 512            | YES          | 1024      |
| EP5-IN  (0x85)     | 512            | YES          | 1024      |
|                    |                | **TOTAL**    | **3208**  |

### Double Endpoint Buffer Configuration for Ambiq TinyUSB Porting

<a name="double-endpoint-buffer-configuration-for-ambiq-tinyusb-porting"></a>

To enable Double Buffer for an IN endpoint, define the following flag, with *<ep_num>* replaced with the endpoint for DEB to be enabled.

``` c
AM_CFG_USB_EP<ep_num>_IN_DBUF_ENABLE
```

To enable Double Buffer for an OUT endpoint, define the following flag, with *<ep_num>* replaced with the endpoint for DEB to be enabled.

``` c
AM_CFG_USB_EP<ep_num>_OUT_DBUF_ENABLE
```

For example, to enable DEB at EP4-OUT and EP5-IN the following flags should be enabled:

``` c
AM_CFG_USB_EP4_OUT_DBUF_ENABLE
AM_CFG_USB_EP5_IN_DBUF_ENABLE
```

## Cache Coherency Handling

<a name="cache-coherency-handling"></a>

Cache coherency handling is needed when either DMA mode is selected for non-control Endpoints as described in [Non-Control Transfer Modes](#non-control-transfer-modes). For Ambiq TinyUSB porting, cache coherency handling is enabled automatically at the DCD porting layer, `dcd_apollo5.c`, when either of the DMA mode is selected.

Application developer should make sure that the TinyUSB config: `CFG_TUSB_MEM_ALIGN` is defined as `__attribute__ ((aligned(32)))` when any of the condition is met:

1. DMA mode is selected, and the endpoint buffer locations are cacheable.
1. Other Module is using the endpoint buffer directly in their DMA operation, and the endpoint buffer locations are cacheable. An example for this case is `tinyusb_cdc_msc_emmc` example where eMMC is using USB Endpoint buffer as its DMA buffer directly.

### Cache Coherency Handlings in Ambiq TinyUSB Port

<a name="cache-coherency-handlings-in-ambiq-tinyusb-port"></a>

The cache coherency handling resides in `dcd_edpt_xfer` API.

Due to the fact that `CFG_TUSB_MEM_ALIGN` is not applied on control endpoint buffers in TinyUSB by default, cache coherency handling is only done on non-EP0 endpoint buffers.

For USB IN transaction, where CM55 is assigning a buffer to be transmitted to USB Host, data-cache of the buffer is cleaned before `am_hal_usb_ep_xfer()` is used to initiate the transfer. This is to ensure that that latest data in cache is flushed into memory module where DMA accesses the data from.

For USB OUT transaction, where CM55 is assigning a buffer to hold upcoming data received from USB Host, data-cache of the buffer is invalidated before `am_hal_usb_ep_xfer()` API is used to initiate the transfer. This is to make sure that new data moved in by DMA is not wrongly overwritten by stale data in data cache.

It is expected that once a buffer is passed into `dcd_edpt_xfer` API for transaction, is is not accessed by CPU until transfer complete callback or USB reset event is received.

## CDC Throughput Optimization

<a name="cdc-throughput-optimization"></a>

The CDC driver in TinyUSB incorporates a FIFO as buffer between application data and endpoint buffers, which simplifies the sending and receiving of data to the USB CDC interfaces. However, this handling isn't throughput friendly as the data has to move through multiple layers of buffer from application to USB HAL.

In Ambiq's TinyUSB port, an alternate buffering mechanism, that allows application direct access to USB HAL, is introduce for applications that favor throughput over convenience. It can be turned on by adding compiler flag: `AM_CDC_USE_APP_BUF`.

With `AM_CDC_USE_APP_BUF` enabled:

- Application buffer passed into cdc_driver will be used directly as endpoint buffer, hence removing the throughput losses caused by the copying of data between buffers.
- With AM_CDC_USE_APP_BUF, buffer with size >512 bytes can be assigned into USB HAL, allowing a big boost in USB CDC throughput when used together with [DMA1 Mode](#dma1-mode).

Highest throughput for CDC is showcased using example `tinyusb_cdc`, with the following configurations:

- `AM_CDC_USE_APP_BUF` enabled
- `AM_CFG_USB_DMA_MODE_1` selected
- `AM_CFG_USB_EP2_OUT_DBUF_ENABLE` and `AM_CFG_USB_EP3_IN_DBUF_ENABLE` defined to enable double buffer for both CDC bulk endpoints.
- When both `AM_CDC_USE_APP_BUF` and `AM_CFG_USB_DMA_MODE1` are defined, macro within `tinyusb_cdc.c` will increase endpoint buffer to 32KB each in order make full use of DMA1 capability.

However, it is worth noting that the cdc_device driver interface changes when `AM_CDC_USE_APP_BUF` is enabled. Application will then need to implement its own buffer management handling. Refer to example `tinyusb_cdc` for reference on the usage.

## Control Transfer STATUS ACK Optimization

<a name="control-transfer-status-ack-optimization"></a>

Ambiq USB controller features an integrated state machine to govern Control Transfer stages for EP0. The state machine features an optimization where STATUS stage is handled automatically once the previous stage is completed. While this feature improves efficiency of Control Transfer, it isn't always compatible with the USB stacks used. This is because some USB stacks are hardcoded to trigger Zero-Length-Packet (ZLP) to USB HAL to complete Control Transfer STATUS stage. TinyUSB is one of the stack that doesn't work out of the box with the feature.

By default, Ambiq USB HAL will wait for ZLP from USB stack in Status Stage before proceeding to wait for the next SETUP packet. In cases where Ambiq USB HAL is to be ported to USB stacks that supports this optimization, the HAL can be recompiled with flag `AM_HAL_USB_CTRL_XFR_AUTO_STATUS_STATE_ACK` enabled.
