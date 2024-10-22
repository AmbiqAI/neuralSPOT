

# File webusb\_controller.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-usb**](dir_450d7ce7daa9d29b9b3b5cb7d00f16f9.md) **>** [**src**](dir_f255d6a194767e484966de6b2584c9a6.md) **>** [**overrides**](dir_9c7a8f6708b262eb1b0dd6b85c35f66f.md) **>** [**webusb\_controller.c**](webusb__controller_8c.md)

[Go to the source code of this file](webusb__controller_8c_source.md)

_web usb controller._ [More...](#detailed-description)

* `#include <stdlib.h>`
* `#include <stdio.h>`
* `#include <string.h>`
* `#include "webusb_controller.h"`
* `#include "usb_descriptors.h"`
* `#include "ns_usb.h"`
* `#include "tusb.h"`
* `#include "am_util_debug.h"`
* `#include "ns_ambiqsuite_harness.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**webusb\_parameter\_t**](structwebusb__parameter__t.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**@3**](#enum-@3)  <br> |






## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  uint8\_t | [**rx\_buf**](#variable-rx_buf)  <br> |
|  bool | [**webusb\_connected**](#variable-webusb_connected)   = = false<br> |
|  [**webusb\_parameter\_t**](structwebusb__parameter__t.md) | [**webusb\_parameter**](#variable-webusb_parameter)   = = {
    .rx\_msg\_cb = NULL,
    .rx\_raw\_cb = NULL,
    .rx\_msg\_param = NULL,
    .rx\_raw\_param = NULL,
}<br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**tud\_mount\_cb**](#function-tud_mount_cb) (void) <br> |
|  void | [**tud\_resume\_cb**](#function-tud_resume_cb) (void) <br> |
|  void | [**tud\_suspend\_cb**](#function-tud_suspend_cb) (bool remote\_wakeup\_en) <br> |
|  void | [**tud\_umount\_cb**](#function-tud_umount_cb) (void) <br> |
|  bool | [**tud\_vendor\_control\_xfer\_cb**](#function-tud_vendor_control_xfer_cb) (uint8\_t rhport, uint8\_t stage, tusb\_control\_request\_t const \* request) <br> |
|  void | [**tud\_vendor\_rx\_cb**](#function-tud_vendor_rx_cb) (uint8\_t itf) <br> |
|  void | [**webusb\_register\_msg\_cb**](#function-webusb_register_msg_cb) (webusb\_rx\_cb cb, void \* param) <br>_Register message callback function and pass to callback pointer._  |
|  void | [**webusb\_register\_raw\_cb**](#function-webusb_register_raw_cb) (webusb\_rx\_cb cb, void \* param) <br>_Register raw callback function and pass to callback pointer._  |
|  uint32\_t | [**webusb\_send\_data**](#function-webusb_send_data) (uint8\_t \* buf, uint32\_t bufsize) <br>_Blocking USB Send Data._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**BSTREAM\_TO\_UINT16**](webusb__controller_8c.md#define-bstream_to_uint16) (n, p) <br> |
| define  | [**BYTES\_TO\_UINT16**](webusb__controller_8c.md#define-bytes_to_uint16) (n, p)     { n = (((uint16\_t)(p)[0] &lt;&lt; 8) + (uint16\_t)(p)[1]); }<br> |
| define  | [**DESC\_MS\_OS\_20**](webusb__controller_8c.md#define-desc_ms_os_20)  7<br> |
| define  | [**REWRIT\_NUMBER**](webusb__controller_8c.md#define-rewrit_number)  40<br> |
| define  | [**WEBUSB\_REQUEST\_SET\_CONTROL\_LINE\_STATE**](webusb__controller_8c.md#define-webusb_request_set_control_line_state)  0x22<br> |

# Detailed Description


This example demonstrates how to use the USB CDC-ACM device class. it will echo back the the input from the terminal tool. 


    
## Public Types Documentation




### enum @3 

```C++
enum @3 {
    RX_INVALID = 0,
    RX_ACORE_MSG = 1,
    RX_RAW_DATA = 2
};
```



## Public Static Attributes Documentation




### variable rx\_buf 

```C++
uint8_t rx_buf[512];
```






### variable webusb\_connected 

```C++
bool webusb_connected;
```






### variable webusb\_parameter 

```C++
webusb_parameter_t webusb_parameter;
```



## Public Functions Documentation




### function tud\_mount\_cb 


```C++
void tud_mount_cb (
    void
) 
```



The functions in the array below are prototyped as TU\_ATTR\_WEAK in Tinyusb. The linkers were not always including these functions in the executable. Using this array forces the linker to keep the function.


In summary,the Tinyusb functions prototyped as weak (macro TU\_ATTR\_WEAK) and implemented in the user code should be added to the array below. 


        



### function tud\_resume\_cb 

```C++
void tud_resume_cb (
    void
) 
```






### function tud\_suspend\_cb 

```C++
void tud_suspend_cb (
    bool remote_wakeup_en
) 
```






### function tud\_umount\_cb 

```C++
void tud_umount_cb (
    void
) 
```






### function tud\_vendor\_control\_xfer\_cb 

```C++
bool tud_vendor_control_xfer_cb (
    uint8_t rhport,
    uint8_t stage,
    tusb_control_request_t const * request
) 
```






### function tud\_vendor\_rx\_cb 

```C++
void tud_vendor_rx_cb (
    uint8_t itf
) 
```






### function webusb\_register\_msg\_cb 

_Register message callback function and pass to callback pointer._ 
```C++
void webusb_register_msg_cb (
    webusb_rx_cb cb,
    void * param
) 
```





**Parameters:**


* `cd` Callback function 
* `param` Pointer to user parameter which will be passed to callback 




        



### function webusb\_register\_raw\_cb 

_Register raw callback function and pass to callback pointer._ 
```C++
void webusb_register_raw_cb (
    webusb_rx_cb cb,
    void * param
) 
```





**Parameters:**


* `cd` Callback function 
* `param` Pointer to user parameter which will be passed to callback 




        



### function webusb\_send\_data 

_Blocking USB Send Data._ 
```C++
uint32_t webusb_send_data (
    uint8_t * buf,
    uint32_t bufsize
) 
```





**Parameters:**


* `buf` Pointer to buffer with data to be sent 
* `bufsize` Requested number of bytes 



**Returns:**

uint32\_t The amount of data written to the ring-buffer. 





        
## Macro Definition Documentation





### define BSTREAM\_TO\_UINT16 

```C++
#define BSTREAM_TO_UINT16 (
    n,
    p
) {                                                                                              \
        BYTES_TO_UINT16(n, p);                                                                     \
        p += 2;                                                                                    \
    }
```






### define BYTES\_TO\_UINT16 

```C++
#define BYTES_TO_UINT16 (
    n,
    p
) { n = (((uint16_t)(p)[0] << 8) + (uint16_t)(p)[1]); }
```






### define DESC\_MS\_OS\_20 

```C++
#define DESC_MS_OS_20 7
```






### define REWRIT\_NUMBER 

```C++
#define REWRIT_NUMBER 40
```






### define WEBUSB\_REQUEST\_SET\_CONTROL\_LINE\_STATE 

```C++
#define WEBUSB_REQUEST_SET_CONTROL_LINE_STATE 0x22
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-usb/src/overrides/webusb_controller.c`

