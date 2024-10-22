

# File ns\_ble.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-ble**](dir_ec3c5c5ea2d338d436d6fa61f38fc381.md) **>** [**src**](dir_cf8bc0902f5dfb1bbd89749c3ff54123.md) **>** [**ns\_ble.c**](ns__ble_8c.md)

[Go to the source code of this file](ns__ble_8c_source.md)

_Generic BLE Wrapper._ [More...](#detailed-description)

* `#include "ns_ble.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_ble\_control\_t**](ns__ble_8h.md#typedef-ns_ble_control_t) | [**g\_ns\_ble\_control**](#variable-g_ns_ble_control)  <br> |
|  const ns\_core\_api\_t | [**ns\_ble\_V0\_0\_1**](#variable-ns_ble_v0_0_1)  <br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  appAdvCfg\_t | [**ns\_ble\_default\_AdvCfg**](#variable-ns_ble_default_advcfg)   = = {
    {60000, 0, 0}, 
    {96, 96, 0}    
}<br> |
|  appSecCfg\_t | [**ns\_ble\_default\_SecCfg**](#variable-ns_ble_default_seccfg)   = = {
    DM\_AUTH\_BOND\_FLAG \| DM\_AUTH\_SC\_FLAG,
     
    0,                                      
    DM\_KEY\_DIST\_LTK,                        
    FALSE,                                  
    FALSE                                   
}<br> |
|  appSlaveCfg\_t | [**ns\_ble\_default\_SlaveCfg**](#variable-ns_ble_default_slavecfg)   = = {
    NS\_BLE\_CONN\_MAX, 
}<br> |
|  smpCfg\_t | [**ns\_ble\_default\_SmpCfg**](#variable-ns_ble_default_smpcfg)   = = {
    
    3200,                
    SMP\_IO\_NO\_IN\_NO\_OUT, 
    7,                   
    16,                  
    3,                   
    0,                   
}<br> |
|  appUpdateCfg\_t | [**ns\_ble\_default\_UpdateCfg**](#variable-ns_ble_default_updatecfg)   = = {
    3100, 
    
    8,   
    18,  
    0,   
    600, 
    5    
}<br> |
|  const uint8\_t | [**ns\_ble\_generic\_data\_disc**](#variable-ns_ble_generic_data_disc)  <br> |
|  const uint8\_t | [**ns\_ble\_generic\_scan\_data\_disc**](#variable-ns_ble_generic_scan_data_disc)  <br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**am\_cooper\_irq\_isr**](#function-am_cooper_irq_isr) (void) <br> |
|  void | [**am\_uart\_isr**](#function-am_uart_isr) (void) <br> |
|  int | [**ns\_ble\_add\_characteristic**](#function-ns_ble_add_characteristic) ([**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* s, [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) \* c) <br>_Add a characteristic to a service. This function should be called after all characteristics have been defined using ns\_ble\_create\_characteristic._  |
|  int | [**ns\_ble\_char2uuid**](#function-ns_ble_char2uuid) (const char uuidString, [**ns\_ble\_uuid128\_t**](structns__ble__uuid128__t.md) \* uuid128) <br>_Helper function to convert a 16-byte UUID string to a 128-bit UUID byte array. UUID string must be exactly 16 characters long and only contain hex characters._  |
|  int | [**ns\_ble\_create\_characteristic**](#function-ns_ble_create_characteristic) ([**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) \* c, char const \* uuidString, void \* applicationValue, uint16\_t valueLength, uint16\_t properties, ns\_ble\_characteristic\_read\_handler\_t readHandlerCb, ns\_ble\_characteristic\_write\_handler\_t writeHandlerCb, ns\_ble\_characteristic\_notify\_handler\_t notifyHandlerCb, uint16\_t periodMs, uint8\_t async, uint16\_t \* attributeCount) <br>_Define a characteristic for subsequent addition to a service._  |
|  int | [**ns\_ble\_create\_service**](#function-ns_ble_create_service) ([**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* s) <br>_Create a BLE service based on the given configuration._  |
|  void | [**ns\_ble\_generic\_handler**](#function-ns_ble_generic_handler) (wsfEventMask\_t event, wsfMsgHdr\_t \* pMsg) <br> |
|  void | [**ns\_ble\_generic\_handlerInit**](#function-ns_ble_generic_handlerinit) (wsfHandlerId\_t handlerId, [**ns\_ble\_service\_control\_t**](ns__ble_8h.md#typedef-ns_ble_service_control_t) \* cfg) <br> |
|  void | [**ns\_ble\_generic\_init**](#function-ns_ble_generic_init) (bool useDefault, [**ns\_ble\_control\_t**](ns__ble_8h.md#typedef-ns_ble_control_t) \* generic\_cfg, [**ns\_ble\_service\_control\_t**](ns__ble_8h.md#typedef-ns_ble_service_control_t) \* service\_cfg) <br> |
|  uint8\_t | [**ns\_ble\_generic\_read\_cback**](#function-ns_ble_generic_read_cback) (dmConnId\_t connId, uint16\_t handle, uint8\_t operation, uint16\_t offset, attsAttr\_t \* pAttr) <br> |
|  uint8\_t | [**ns\_ble\_generic\_write\_cback**](#function-ns_ble_generic_write_cback) (dmConnId\_t connId, uint16\_t handle, uint8\_t operation, uint16\_t offset, uint16\_t len, uint8\_t \* pValue, attsAttr\_t \* pAttr) <br> |
|  uint16\_t | [**ns\_ble\_get\_next\_handle\_id**](#function-ns_ble_get_next_handle_id) ([**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* service) <br> |
|  void | [**ns\_ble\_new\_handler**](#function-ns_ble_new_handler) (wsfEventMask\_t event, wsfMsgHdr\_t \* pMsg) <br> |
|  void | [**ns\_ble\_new\_handler\_init**](#function-ns_ble_new_handler_init) (wsfHandlerId\_t handlerId) <br> |
|  bool | [**ns\_ble\_new\_proc\_msg**](#function-ns_ble_new_proc_msg) ([**ns\_ble\_msg\_t**](unionns__ble__msg__t.md) \* pMsg) <br> |
|  void | [**ns\_ble\_pre\_init**](#function-ns_ble_pre_init) (void) <br>_Call this function from the setup\_task, prior to creating the RadioTask All this does is set up NVIC priorities._  |
|  void | [**ns\_ble\_send\_value**](#function-ns_ble_send_value) ([**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) \* c, attEvt\_t \* pMsg) <br> |
|  int | [**ns\_ble\_set\_tx\_power**](#function-ns_ble_set_tx_power) (txPowerLevel\_t power) <br>_Function to set the BLE TX power._  |
|  int | [**ns\_ble\_start\_service**](#function-ns_ble_start_service) ([**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* s) <br>_Invoked by RadioTask to start the service. This function should be called after all characteristics have been added._  |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_ble\_generic\_AttCback**](#function-ns_ble_generic_attcback) (attEvt\_t \* pEvt) <br> |
|  void | [**ns\_ble\_generic\_CccCback**](#function-ns_ble_generic_ccccback) (attsCccEvt\_t \* pEvt) <br> |
|  void | [**ns\_ble\_generic\_DmCback**](#function-ns_ble_generic_dmcback) (dmEvt\_t \* pDmEvt) <br> |
|  void | [**ns\_ble\_generic\_advSetup**](#function-ns_ble_generic_advsetup) ([**ns\_ble\_msg\_t**](unionns__ble__msg__t.md) \* pMsg) <br> |
|  void | [**ns\_ble\_generic\_conn\_open**](#function-ns_ble_generic_conn_open) (dmEvt\_t \* pMsg) <br> |
|  void | [**ns\_ble\_generic\_conn\_update**](#function-ns_ble_generic_conn_update) (dmEvt\_t \* pMsg) <br> |
|  void | [**ns\_ble\_generic\_new\_handle\_cnf**](#function-ns_ble_generic_new_handle_cnf) (attEvt\_t \* pMsg) <br> |
|  void | [**ns\_ble\_generic\_procMsg**](#function-ns_ble_generic_procmsg) ([**ns\_ble\_msg\_t**](unionns__ble__msg__t.md) \* pMsg) <br> |
|  bool | [**ns\_ble\_handle\_indication\_timer\_expired**](#function-ns_ble_handle_indication_timer_expired) ([**ns\_ble\_msg\_t**](unionns__ble__msg__t.md) \* pMsg) <br> |
|  void | [**ns\_ble\_process\_ccc\_state**](#function-ns_ble_process_ccc_state) (attsCccEvt\_t \* pMsg) <br> |


























# Detailed Description




**Author:**

Ambiq Team 




**Version:**

0.1 




**Date:**

2023-06-08




**Copyright:**

Copyright (c) 2023 





    
## Public Attributes Documentation




### variable g\_ns\_ble\_control 

```C++
ns_ble_control_t g_ns_ble_control;
```






### variable ns\_ble\_V0\_0\_1 

```C++
const ns_core_api_t ns_ble_V0_0_1;
```



## Public Static Attributes Documentation




### variable ns\_ble\_default\_AdvCfg 


```C++
appAdvCfg_t ns_ble_default_AdvCfg;
```



configurable parameters for advertising 


        



### variable ns\_ble\_default\_SecCfg 


```C++
appSecCfg_t ns_ble_default_SecCfg;
```



configurable parameters for security 


        



### variable ns\_ble\_default\_SlaveCfg 


```C++
appSlaveCfg_t ns_ble_default_SlaveCfg;
```



configurable parameters for slave 


        



### variable ns\_ble\_default\_SmpCfg 


```C++
smpCfg_t ns_ble_default_SmpCfg;
```



SMP security parameter configuration 


        



### variable ns\_ble\_default\_UpdateCfg 


```C++
appUpdateCfg_t ns_ble_default_UpdateCfg;
```



configurable parameters for connection parameter update 


        



### variable ns\_ble\_generic\_data\_disc 


```C++
const uint8_t ns_ble_generic_data_disc[];
```



advertising data, discoverable mode 


        



### variable ns\_ble\_generic\_scan\_data\_disc 


```C++
const uint8_t ns_ble_generic_scan_data_disc[];
```



scan data, discoverable mode 


        
## Public Functions Documentation




### function am\_cooper\_irq\_isr 

```C++
void am_cooper_irq_isr (
    void
) 
```






### function am\_uart\_isr 

```C++
void am_uart_isr (
    void
) 
```






### function ns\_ble\_add\_characteristic 

_Add a characteristic to a service. This function should be called after all characteristics have been defined using ns\_ble\_create\_characteristic._ 
```C++
int ns_ble_add_characteristic (
    ns_ble_service_t * s,
    ns_ble_characteristic_t * c
) 
```





**Parameters:**


* `s` - a pointer to the service to which the characteristic should be added 
* `c` - a pointer to the characteristic to be added



**Returns:**

int 





        



### function ns\_ble\_char2uuid 

_Helper function to convert a 16-byte UUID string to a 128-bit UUID byte array. UUID string must be exactly 16 characters long and only contain hex characters._ 
```C++
int ns_ble_char2uuid (
    const char uuidString,
    ns_ble_uuid128_t * uuid128
) 
```





**Parameters:**


* `uuidString` - a 16-byte UUID string 
* `uuid128` - a pointer to a 128-bit UUID byte array (uuid128.array) 



**Returns:**

int 





        



### function ns\_ble\_create\_characteristic 

_Define a characteristic for subsequent addition to a service._ 
```C++
int ns_ble_create_characteristic (
    ns_ble_characteristic_t * c,
    char const * uuidString,
    void * applicationValue,
    uint16_t valueLength,
    uint16_t properties,
    ns_ble_characteristic_read_handler_t readHandlerCb,
    ns_ble_characteristic_write_handler_t writeHandlerCb,
    ns_ble_characteristic_notify_handler_t notifyHandlerCb,
    uint16_t periodMs,
    uint8_t async,
    uint16_t * attributeCount
) 
```





**Parameters:**


* `c` - config struct, populated by this function 
* `uuidString` - a 16-byte UUID string 
* `applicationValue` - a pointer to the application's value store 
* `valueLength` - the length of the value store, in bytes 
* `properties` - a bitmask of properties for the characteristic, from the enum above 
* `readHandlerCb` - a callback function for read requests if the characteristic is readable 
* `writeHandlerCb` - a callback function for write requests if the characteristic is writable 
* `notifyHandlerCb` - a callback function for notify requests if the characteristic is "subscribe-able" 
* `periodMs` - the period of the notify timer, in milliseconds 
* `attributeCount` - a pointer to the service's attribute count. This is incremented by the function. 



**Returns:**

int 





        



### function ns\_ble\_create\_service 

_Create a BLE service based on the given configuration._ 
```C++
int ns_ble_create_service (
    ns_ble_service_t * s
) 
```





**Parameters:**


* `s` - a pointer to a service configuration struct. Relevant fields are:
  * uuid128: a 128-bit UUID for the service
  * name: a string name for the service, up to 31 characters
  * nameLen: the length of the name string
  * baseHandle: the first handle ID of the service. This is used by BLE to identify attributes of the service
  * numAttributes: keeps track of the number of attributes in the service. Initialize to 0.
  * poolConfig: a pointer to a WSF pool configuration struct. 





**Returns:**

int 





        



### function ns\_ble\_generic\_handler 

```C++
void ns_ble_generic_handler (
    wsfEventMask_t event,
    wsfMsgHdr_t * pMsg
) 
```






### function ns\_ble\_generic\_handlerInit 

```C++
void ns_ble_generic_handlerInit (
    wsfHandlerId_t handlerId,
    ns_ble_service_control_t * cfg
) 
```






### function ns\_ble\_generic\_init 

```C++
void ns_ble_generic_init (
    bool useDefault,
    ns_ble_control_t * generic_cfg,
    ns_ble_service_control_t * service_cfg
) 
```






### function ns\_ble\_generic\_read\_cback 

```C++
uint8_t ns_ble_generic_read_cback (
    dmConnId_t connId,
    uint16_t handle,
    uint8_t operation,
    uint16_t offset,
    attsAttr_t * pAttr
) 
```






### function ns\_ble\_generic\_write\_cback 

```C++
uint8_t ns_ble_generic_write_cback (
    dmConnId_t connId,
    uint16_t handle,
    uint8_t operation,
    uint16_t offset,
    uint16_t len,
    uint8_t * pValue,
    attsAttr_t * pAttr
) 
```






### function ns\_ble\_get\_next\_handle\_id 

```C++
uint16_t ns_ble_get_next_handle_id (
    ns_ble_service_t * service
) 
```






### function ns\_ble\_new\_handler 

```C++
void ns_ble_new_handler (
    wsfEventMask_t event,
    wsfMsgHdr_t * pMsg
) 
```






### function ns\_ble\_new\_handler\_init 

```C++
void ns_ble_new_handler_init (
    wsfHandlerId_t handlerId
) 
```






### function ns\_ble\_new\_proc\_msg 

```C++
bool ns_ble_new_proc_msg (
    ns_ble_msg_t * pMsg
) 
```






### function ns\_ble\_pre\_init 

```C++
void ns_ble_pre_init (
    void
) 
```






### function ns\_ble\_send\_value 

```C++
void ns_ble_send_value (
    ns_ble_characteristic_t * c,
    attEvt_t * pMsg
) 
```






### function ns\_ble\_set\_tx\_power 

_Function to set the BLE TX power._ 
```C++
int ns_ble_set_tx_power (
    txPowerLevel_t power
) 
```





**Parameters:**


* `power` - tx power level in dBm. 



**Returns:**

bool 





        



### function ns\_ble\_start\_service 

_Invoked by RadioTask to start the service. This function should be called after all characteristics have been added._ 
```C++
int ns_ble_start_service (
    ns_ble_service_t * s
) 
```





**Parameters:**


* `s` - a pointer to the service to be started 



**Returns:**

int 





        
## Public Static Functions Documentation




### function ns\_ble\_generic\_AttCback 

```C++
static void ns_ble_generic_AttCback (
    attEvt_t * pEvt
) 
```






### function ns\_ble\_generic\_CccCback 

```C++
static void ns_ble_generic_CccCback (
    attsCccEvt_t * pEvt
) 
```






### function ns\_ble\_generic\_DmCback 

```C++
static void ns_ble_generic_DmCback (
    dmEvt_t * pDmEvt
) 
```






### function ns\_ble\_generic\_advSetup 

```C++
static void ns_ble_generic_advSetup (
    ns_ble_msg_t * pMsg
) 
```






### function ns\_ble\_generic\_conn\_open 

```C++
static void ns_ble_generic_conn_open (
    dmEvt_t * pMsg
) 
```






### function ns\_ble\_generic\_conn\_update 

```C++
static void ns_ble_generic_conn_update (
    dmEvt_t * pMsg
) 
```






### function ns\_ble\_generic\_new\_handle\_cnf 

```C++
static void ns_ble_generic_new_handle_cnf (
    attEvt_t * pMsg
) 
```






### function ns\_ble\_generic\_procMsg 

```C++
static void ns_ble_generic_procMsg (
    ns_ble_msg_t * pMsg
) 
```






### function ns\_ble\_handle\_indication\_timer\_expired 

```C++
static bool ns_ble_handle_indication_timer_expired (
    ns_ble_msg_t * pMsg
) 
```






### function ns\_ble\_process\_ccc\_state 

```C++
static void ns_ble_process_ccc_state (
    attsCccEvt_t * pMsg
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-ble/src/ns_ble.c`

