

# File usb\_descriptors.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-usb**](dir_450d7ce7daa9d29b9b3b5cb7d00f16f9.md) **>** [**src**](dir_f255d6a194767e484966de6b2584c9a6.md) **>** [**overrides**](dir_9c7a8f6708b262eb1b0dd6b85c35f66f.md) **>** [**usb\_descriptors.c**](usb__descriptors_8c.md)

[Go to the source code of this file](usb__descriptors_8c_source.md)



* `#include "tusb.h"`
* `#include "usb_descriptors.h"`

















## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**@2**](#enum-@2)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint8\_t const | [**desc\_bos**](#variable-desc_bos)   = = {
    
    TUD\_BOS\_DESCRIPTOR(BOS\_TOTAL\_LEN, 2),

    
    TUD\_BOS\_WEBUSB\_DESCRIPTOR(VENDOR\_REQUEST\_WEBUSB, 1),

    
    TUD\_BOS\_MS\_OS\_20\_DESCRIPTOR(MS\_OS\_20\_DESC\_LEN, VENDOR\_REQUEST\_MICROSOFT)}<br> |
|  tusb\_desc\_device\_t const | [**desc\_device**](#variable-desc_device)   = = {
    .bLength = sizeof(tusb\_desc\_device\_t),
    .bDescriptorType = TUSB\_DESC\_DEVICE,
    .bcdUSB = 0x0210, 

    
    
    
    .bDeviceClass = TUSB\_CLASS\_MISC,
    .bDeviceSubClass = MISC\_SUBCLASS\_COMMON,
    .bDeviceProtocol = MISC\_PROTOCOL\_IAD,
    .bMaxPacketSize0 = CFG\_TUD\_ENDPOINT0\_SIZE,

    .idVendor = 0xCafe,
    .idProduct = USB\_PID,
    .bcdDevice = 0x0100,

    .iManufacturer = USB\_DESCRIPTOR\_MANUFACTURER,
    .iProduct = USB\_DESCRIPTOR\_PRODUCT,
    .iSerialNumber = USB\_DESCRIPTOR\_SERIAL,

    .bNumConfigurations = 0x01}<br> |
|  uint8\_t const | [**desc\_ms\_os\_20**](#variable-desc_ms_os_20)  <br> |
|  char const  \* | [**usb\_string\_desc\_arr**](#variable-usb_string_desc_arr)   = = {
    (const char[]){0x09, 0x04}, 
    "TinyUSB",                  
    "TinyUSB Device",           
    "123456",                   
    "TinyUSB CDC",              
                                
    "TinyUSB WebUSB"            
}<br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  uint16\_t | [**\_desc\_str**](#variable-_desc_str)  <br> |
|  uint8\_t const | [**desc\_fs\_configuration**](#variable-desc_fs_configuration)   = = {
    
    
    
    
    TUD\_CONFIG\_DESCRIPTOR(
        1, ITF\_NUM\_TOTAL, 0, CONFIG\_TOTAL\_LEN, TUSB\_DESC\_CONFIG\_ATT\_REMOTE\_WAKEUP, POWER),

    
    
    
    
    TUD\_CDC\_DESCRIPTOR(
        ITF\_NUM\_CDC, 4, EPNUM\_CDC\_NOTIF, 8, EPNUM\_CDC\_OUT, EPNUM\_CDC\_IN, CDC\_FS\_EP\_LEN),

    
    TUD\_VENDOR\_DESCRIPTOR(ITF\_NUM\_VENDOR, 5, EPNUM\_VENDOR\_OUT, EPNUM\_VENDOR\_IN, 64),
}<br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|   | [**TU\_VERIFY\_STATIC**](#function-tu_verify_static) (sizeof(desc\_ms\_os\_20)==MS\_OS\_20\_DESC\_LEN, "Incorrect size") <br> |
|  uint8\_t const \* | [**tud\_descriptor\_bos\_cb**](#function-tud_descriptor_bos_cb) (void) <br> |
|  uint8\_t const \* | [**tud\_descriptor\_configuration\_cb**](#function-tud_descriptor_configuration_cb) (uint8\_t index) <br>_Invoked when received GET CONFIGURATION DESCRIPTOR._  |
|  uint8\_t const \* | [**tud\_descriptor\_device\_cb**](#function-tud_descriptor_device_cb) (void) <br>_Invoked when received GET DEVICE DESCRIPTOR._  |
|  uint16\_t const \* | [**tud\_descriptor\_string\_cb**](#function-tud_descriptor_string_cb) (uint8\_t index, uint16\_t langid) <br>_Invoked when received GET STRING DESCRIPTOR request._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**BOS\_TOTAL\_LEN**](usb__descriptors_8c.md#define-bos_total_len)  (TUD\_BOS\_DESC\_LEN + TUD\_BOS\_WEBUSB\_DESC\_LEN + TUD\_BOS\_MICROSOFT\_OS\_DESC\_LEN)<br> |
| define  | [**CDC\_FS\_EP\_LEN**](usb__descriptors_8c.md#define-cdc_fs_ep_len)  0x40<br> |
| define  | [**CDC\_HS\_EP\_LEN**](usb__descriptors_8c.md#define-cdc_hs_ep_len)  0x200<br> |
| define  | [**CONFIG\_TOTAL\_LEN**](usb__descriptors_8c.md#define-config_total_len)  (TUD\_CONFIG\_DESC\_LEN + TUD\_CDC\_DESC\_LEN + TUD\_VENDOR\_DESC\_LEN)<br> |
| define  | [**EPNUM\_CDC\_IN**](usb__descriptors_8c.md#define-epnum_cdc_in)  0x82<br> |
| define  | [**EPNUM\_CDC\_NOTIF**](usb__descriptors_8c.md#define-epnum_cdc_notif)  0x81<br> |
| define  | [**EPNUM\_CDC\_OUT**](usb__descriptors_8c.md#define-epnum_cdc_out)  0x02<br> |
| define  | [**EPNUM\_VENDOR\_IN**](usb__descriptors_8c.md#define-epnum_vendor_in)  0x83<br> |
| define  | [**EPNUM\_VENDOR\_OUT**](usb__descriptors_8c.md#define-epnum_vendor_out)  0x03<br> |
| define  | [**MS\_OS\_20\_DESC\_LEN**](usb__descriptors_8c.md#define-ms_os_20_desc_len)  0xB2<br> |
| define  | [**POWER**](usb__descriptors_8c.md#define-power)  100<br> |
| define  | [**USB\_PID**](usb__descriptors_8c.md#define-usb_pid)  <br> |
| define  | [**\_PID\_MAP**](usb__descriptors_8c.md#define-_pid_map) (itf, n) ((CFG\_TUD\_##itf) &lt;&lt; (n))<br> |

## Public Types Documentation




### enum @2 

```C++
enum @2 {
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
    ITF_NUM_VENDOR,
    ITF_NUM_TOTAL
};
```



## Public Attributes Documentation




### variable desc\_bos 

```C++
uint8_t const desc_bos[];
```






### variable desc\_device 

```C++
tusb_desc_device_t const desc_device;
```






### variable desc\_ms\_os\_20 

```C++
uint8_t const desc_ms_os_20[];
```






### variable usb\_string\_desc\_arr 

```C++
char const* usb_string_desc_arr[6];
```



## Public Static Attributes Documentation




### variable \_desc\_str 

```C++
uint16_t _desc_str[32];
```






### variable desc\_fs\_configuration 

```C++
uint8_t const desc_fs_configuration[];
```



## Public Functions Documentation




### function TU\_VERIFY\_STATIC 

```C++
TU_VERIFY_STATIC (
    sizeof(desc_ms_os_20)==MS_OS_20_DESC_LEN,
    "Incorrect size"
) 
```






### function tud\_descriptor\_bos\_cb 

```C++
uint8_t const * tud_descriptor_bos_cb (
    void
) 
```






### function tud\_descriptor\_configuration\_cb 

_Invoked when received GET CONFIGURATION DESCRIPTOR._ 
```C++
uint8_t const * tud_descriptor_configuration_cb (
    uint8_t index
) 
```





**Note:**

Descriptor contents must exist long enough for transfer to complete




**Parameters:**


* `index` 



**Returns:**

Application return pointer to descriptor. 





        



### function tud\_descriptor\_device\_cb 

_Invoked when received GET DEVICE DESCRIPTOR._ 
```C++
uint8_t const * tud_descriptor_device_cb (
    void
) 
```





**Returns:**

Application return pointer to descriptor 





        



### function tud\_descriptor\_string\_cb 

_Invoked when received GET STRING DESCRIPTOR request._ 
```C++
uint16_t const * tud_descriptor_string_cb (
    uint8_t index,
    uint16_t langid
) 
```





**Returns:**

Application return pointer to descriptor. 





        
## Macro Definition Documentation





### define BOS\_TOTAL\_LEN 

```C++
#define BOS_TOTAL_LEN (TUD_BOS_DESC_LEN + TUD_BOS_WEBUSB_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)
```






### define CDC\_FS\_EP\_LEN 

```C++
#define CDC_FS_EP_LEN 0x40
```






### define CDC\_HS\_EP\_LEN 

```C++
#define CDC_HS_EP_LEN 0x200
```






### define CONFIG\_TOTAL\_LEN 

```C++
#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_VENDOR_DESC_LEN)
```






### define EPNUM\_CDC\_IN 

```C++
#define EPNUM_CDC_IN 0x82
```






### define EPNUM\_CDC\_NOTIF 

```C++
#define EPNUM_CDC_NOTIF 0x81
```






### define EPNUM\_CDC\_OUT 

```C++
#define EPNUM_CDC_OUT 0x02
```






### define EPNUM\_VENDOR\_IN 

```C++
#define EPNUM_VENDOR_IN 0x83
```






### define EPNUM\_VENDOR\_OUT 

```C++
#define EPNUM_VENDOR_OUT 0x03
```






### define MS\_OS\_20\_DESC\_LEN 

```C++
#define MS_OS_20_DESC_LEN 0xB2
```






### define POWER 

```C++
#define POWER 100
```






### define USB\_PID 

```C++
#define USB_PID (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | _PID_MAP(MIDI, 3) |         \
     _PID_MAP(VENDOR, 4))
```






### define \_PID\_MAP 

```C++
#define _PID_MAP (
    itf,
    n
) ((CFG_TUD_##itf) << (n))
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-usb/src/overrides/usb_descriptors.c`

