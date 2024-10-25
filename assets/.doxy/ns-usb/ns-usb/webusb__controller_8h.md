

# File webusb\_controller.h



[**FileList**](files.md) **>** [**includes-api**](dir_a2a61ebab6bf7d344fb1c36a75cc33e9.md) **>** [**webusb\_controller.h**](webusb__controller_8h.md)

[Go to the source code of this file](webusb__controller_8h_source.md)

_Connect MCU and Audio Tool via WebUSB._ 

* `#include <stdint.h>`

















## Public Types

| Type | Name |
| ---: | :--- |
| typedef void(\* | [**webusb\_rx\_cb**](#typedef-webusb_rx_cb)  <br> |




















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**webusb\_init**](#function-webusb_init) (void) <br>_Init tinyusb._  |
|  void | [**webusb\_register\_msg\_cb**](#function-webusb_register_msg_cb) (webusb\_rx\_cb cb, void \* param) <br>_Register message callback function and pass to callback pointer._  |
|  void | [**webusb\_register\_raw\_cb**](#function-webusb_register_raw_cb) (webusb\_rx\_cb cb, void \* param) <br>_Register raw callback function and pass to callback pointer._  |
|  uint32\_t | [**webusb\_send\_data**](#function-webusb_send_data) (uint8\_t \* buf, uint32\_t bufsize) <br>_Blocking USB Send Data._  |
|  void | [**webusb\_task**](#function-webusb_task) (void) <br>_tinyusb device task._  |




























## Public Types Documentation




### typedef webusb\_rx\_cb 

```C++
typedef void(* webusb_rx_cb) (const uint8_t *buf, uint32_t buf_len, void *param);
```



## Public Functions Documentation




### function webusb\_init 

```C++
void webusb_init (
    void
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





        



### function webusb\_task 

```C++
void webusb_task (
    void
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-usb/includes-api/webusb_controller.h`

