

# Group ns-usb



[**Modules**](modules.md) **>** [**ns-usb**](group__ns-usb.md)




















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**TU\_ATTR\_PACKED**](struct_t_u___a_t_t_r___p_a_c_k_e_d.md) <br>_URL buffer Stucture._  |
| struct | [**ns\_usb\_config\_t**](structns__usb__config__t.md) <br>_USB Configuration Struct._  |
| struct | [**ns\_usb\_transaction\_t**](structns__usb__transaction__t.md) <br>_USB Transaction Control Stucture._  |


## Public Types

| Type | Name |
| ---: | :--- |
| typedef struct [**TU\_ATTR\_PACKED**](struct_t_u___a_t_t_r___p_a_c_k_e_d.md) | [**ns\_tusb\_desc\_webusb\_url\_t**](#typedef-ns_tusb_desc_webusb_url_t)  <br>_URL buffer Stucture._  |
| enum  | [**ns\_usb\_device\_type\_e**](#enum-ns_usb_device_type_e)  <br>_USB Device Type._  |
| typedef void(\* | [**ns\_usb\_rx\_cb**](#typedef-ns_usb_rx_cb)  <br> |
| typedef void(\* | [**ns\_usb\_service\_cb**](#typedef-ns_usb_service_cb)  <br> |
| typedef void(\* | [**ns\_usb\_tx\_cb**](#typedef-ns_usb_tx_cb)  <br> |
| typedef void \* | [**usb\_handle\_t**](#typedef-usb_handle_t)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_usb\_V0\_0\_1**](#variable-ns_usb_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_usb\_V1\_0\_0**](#variable-ns_usb_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_usb\_current\_version**](#variable-ns_usb_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_usb\_oldest\_supported\_version**](#variable-ns_usb_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_get\_cdc\_rx\_bufferLength**](#function-ns_get_cdc_rx_bufferlength) () <br> |
|  uint32\_t | [**ns\_get\_cdc\_tx\_bufferLength**](#function-ns_get_cdc_tx_bufferlength) () <br> |
|  [**ns\_tusb\_desc\_webusb\_url\_t**](ns__usb_8h.md#typedef-ns_tusb_desc_webusb_url_t) \* | [**ns\_get\_desc\_url**](#function-ns_get_desc_url) () <br> |
|  bool | [**ns\_usb\_data\_available**](#function-ns_usb_data_available) (usb\_handle\_t handle) <br> |
|  uint8\_t \* | [**ns\_usb\_get\_rx\_buffer**](#function-ns_usb_get_rx_buffer) () <br> |
|  uint8\_t \* | [**ns\_usb\_get\_tx\_buffer**](#function-ns_usb_get_tx_buffer) () <br> |
|  void | [**ns\_usb\_handle\_read\_error**](#function-ns_usb_handle_read_error) (usb\_handle\_t h) <br>_Flushes the USB RX fifo after a delay, resets ns\_usb rx state._  |
|  uint32\_t | [**ns\_usb\_init**](#function-ns_usb_init) ([**ns\_usb\_config\_t**](structns__usb__config__t.md) \* cfg, usb\_handle\_t \* h) <br>_Initialize the USB system._  |
|  uint32\_t | [**ns\_usb\_recieve\_data**](#function-ns_usb_recieve_data) (usb\_handle\_t handle, void \* buffer, uint32\_t bufsize) <br>_Blocking USB Receive Data._  |
|  void | [**ns\_usb\_register\_callbacks**](#function-ns_usb_register_callbacks) (usb\_handle\_t handle, ns\_usb\_rx\_cb rxcb, ns\_usb\_tx\_cb txcb) <br>_Register callbacks for USB events, should be called after ns\_usb\_init._  |
|  uint32\_t | [**ns\_usb\_send\_data**](#function-ns_usb_send_data) (usb\_handle\_t handle, void \* buffer, uint32\_t bufsize) <br>_Blocking USB Send Data._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**MAX\_URL\_LENGTH**](ns__usb_8h.md#define-max_url_length)  100<br> |
| define  | [**NS\_USB\_API\_ID**](ns__usb_8h.md#define-ns_usb_api_id)  0xCA0006<br> |
| define  | [**NS\_USB\_CURRENT\_VERSION**](ns__usb_8h.md#define-ns_usb_current_version)  NS\_USB\_V1\_0\_0<br> |
| define  | [**NS\_USB\_OLDEST\_SUPPORTED\_VERSION**](ns__usb_8h.md#define-ns_usb_oldest_supported_version)  NS\_USB\_V0\_0\_1<br> |
| define  | [**NS\_USB\_V0\_0\_1**](ns__usb_8h.md#define-ns_usb_v0_0_1)          { .major = 0, .minor = 0, .revision = 1 }<br> |
| define  | [**NS\_USB\_V1\_0\_0**](ns__usb_8h.md#define-ns_usb_v1_0_0)          { .major = 1, .minor = 0, .revision = 0 }<br> |

## Public Types Documentation




### typedef ns\_tusb\_desc\_webusb\_url\_t 

```C++
typedef struct TU_ATTR_PACKED ns_tusb_desc_webusb_url_t;
```






### enum ns\_usb\_device\_type\_e 

```C++
enum ns_usb_device_type_e {
    NS_USB_CDC_DEVICE,
    NS_USB_HID_DEVICE,
    NS_USB_MSC_DEVICE,
    NS_USB_VENDOR_DEVICE
};
```






### typedef ns\_usb\_rx\_cb 

```C++
typedef void(* ns_usb_rx_cb) (ns_usb_transaction_t *);
```






### typedef ns\_usb\_service\_cb 

```C++
typedef void(* ns_usb_service_cb) (uint8_t);
```






### typedef ns\_usb\_tx\_cb 

```C++
typedef void(* ns_usb_tx_cb) (ns_usb_transaction_t *);
```






### typedef usb\_handle\_t 

```C++
typedef void* usb_handle_t;
```



## Public Attributes Documentation




### variable ns\_usb\_V0\_0\_1 

```C++
const ns_core_api_t ns_usb_V0_0_1;
```






### variable ns\_usb\_V1\_0\_0 

```C++
const ns_core_api_t ns_usb_V1_0_0;
```






### variable ns\_usb\_current\_version 

```C++
const ns_core_api_t ns_usb_current_version;
```






### variable ns\_usb\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_usb_oldest_supported_version;
```



## Public Functions Documentation




### function ns\_get\_cdc\_rx\_bufferLength 

```C++
uint32_t ns_get_cdc_rx_bufferLength () 
```






### function ns\_get\_cdc\_tx\_bufferLength 

```C++
uint32_t ns_get_cdc_tx_bufferLength () 
```






### function ns\_get\_desc\_url 

```C++
ns_tusb_desc_webusb_url_t * ns_get_desc_url () 
```






### function ns\_usb\_data\_available 

```C++
bool ns_usb_data_available (
    usb_handle_t handle
) 
```






### function ns\_usb\_get\_rx\_buffer 

```C++
uint8_t * ns_usb_get_rx_buffer () 
```






### function ns\_usb\_get\_tx\_buffer 

```C++
uint8_t * ns_usb_get_tx_buffer () 
```






### function ns\_usb\_handle\_read\_error 

_Flushes the USB RX fifo after a delay, resets ns\_usb rx state._ 
```C++
void ns_usb_handle_read_error (
    usb_handle_t h
) 
```





**Parameters:**


* `h` handle 




        



### function ns\_usb\_init 

_Initialize the USB system._ 
```C++
uint32_t ns_usb_init (
    ns_usb_config_t * cfg,
    usb_handle_t * h
) 
```





**Parameters:**


* `cfg` 
* `handle` 



**Returns:**

uint32\_t Status 





        



### function ns\_usb\_recieve\_data 

_Blocking USB Receive Data._ 
```C++
uint32_t ns_usb_recieve_data (
    usb_handle_t handle,
    void * buffer,
    uint32_t bufsize
) 
```





**Parameters:**


* `handle` USB handle 
* `buffer` Pointer to buffer where data will be placed 
* `bufsize` Requested number of bytes 



**Returns:**

uint32\_t 





        



### function ns\_usb\_register\_callbacks 

_Register callbacks for USB events, should be called after ns\_usb\_init._ 
```C++
void ns_usb_register_callbacks (
    usb_handle_t handle,
    ns_usb_rx_cb rxcb,
    ns_usb_tx_cb txcb
) 
```





**Parameters:**


* `handle` 
* `rx_cb` 
* `tx_cb` 




        



### function ns\_usb\_send\_data 

_Blocking USB Send Data._ 
```C++
uint32_t ns_usb_send_data (
    usb_handle_t handle,
    void * buffer,
    uint32_t bufsize
) 
```





**Parameters:**


* `handle` USB handle 
* `buffer` Pointer to buffer with data to be sent 
* `bufsize` Requested number of bytes 



**Returns:**

uint32\_t 





        
## Macro Definition Documentation





### define MAX\_URL\_LENGTH 

```C++
#define MAX_URL_LENGTH 100
```






### define NS\_USB\_API\_ID 

```C++
#define NS_USB_API_ID 0xCA0006
```






### define NS\_USB\_CURRENT\_VERSION 

```C++
#define NS_USB_CURRENT_VERSION NS_USB_V1_0_0
```






### define NS\_USB\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_USB_OLDEST_SUPPORTED_VERSION NS_USB_V0_0_1
```






### define NS\_USB\_V0\_0\_1 

```C++
#define NS_USB_V0_0_1 { .major = 0, .minor = 0, .revision = 1 }
```






### define NS\_USB\_V1\_0\_0 

```C++
#define NS_USB_V1_0_0 { .major = 1, .minor = 0, .revision = 0 }
```




------------------------------


