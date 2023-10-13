

# Group ns-ble



[**Modules**](modules.md) **>** [**ns-ble**](group__ns-ble.md)




















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_ble\_characteristic**](structns__ble__characteristic.md) <br> |
| struct | [**ns\_ble\_connection\_t**](structns__ble__connection__t.md) <br>_BLE Connection control block._  |
| struct | [**ns\_ble\_control**](structns__ble__control.md) <br>_BLE Control block._  |
| struct | [**ns\_ble\_pool\_config\_t**](structns__ble__pool__config__t.md) <br>_WSF Buffer Pool Configuration._  |
| struct | [**ns\_ble\_service**](structns__ble__service.md) <br>_BLE Service Configuration._  |
| struct | [**ns\_ble\_service\_control**](structns__ble__service__control.md) <br>_BLE Service control block._  |
| struct | [**ns\_ble\_uuid128\_t**](structns__ble__uuid128__t.md) <br>_BLE UUID (128-bit)_  |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**@0**](#enum-@0)  <br> |
| typedef int(\* | [**ns\_ble\_characteristic\_notify\_handler\_t**](#typedef-ns_ble_characteristic_notify_handler_t)  <br> |
| typedef int(\* | [**ns\_ble\_characteristic\_read\_handler\_t**](#typedef-ns_ble_characteristic_read_handler_t)  <br> |
| typedef struct [**ns\_ble\_characteristic**](structns__ble__characteristic.md) | [**ns\_ble\_characteristic\_t**](#typedef-ns_ble_characteristic_t)  <br> |
| typedef int(\* | [**ns\_ble\_characteristic\_write\_handler\_t**](#typedef-ns_ble_characteristic_write_handler_t)  <br> |
| typedef struct [**ns\_ble\_control**](structns__ble__control.md) | [**ns\_ble\_control\_t**](#typedef-ns_ble_control_t)  <br>_BLE Control block._  |
| union  | [**ns\_ble\_msg\_t**](#union-ns_ble_msg_t)  <br> |
| typedef struct [**ns\_ble\_service\_control**](structns__ble__service__control.md) | [**ns\_ble\_service\_control\_t**](#typedef-ns_ble_service_control_t)  <br>_BLE Service control block._  |
| typedef void(\* | [**ns\_ble\_service\_specific\_handler\_cb**](#typedef-ns_ble_service_specific_handler_cb)  <br> |
| typedef void(\* | [**ns\_ble\_service\_specific\_handler\_init\_cb**](#typedef-ns_ble_service_specific_handler_init_cb)  <br> |
| typedef void(\* | [**ns\_ble\_service\_specific\_init\_cb**](#typedef-ns_ble_service_specific_init_cb)  <br> |
| typedef bool(\* | [**ns\_ble\_service\_specific\_procMsg\_cb**](#typedef-ns_ble_service_specific_procmsg_cb)  <br> |
| typedef struct [**ns\_ble\_service**](structns__ble__service.md) | [**ns\_ble\_service\_t**](#typedef-ns_ble_service_t)  <br>_BLE Service Configuration._  |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_ble\_V0\_0\_1**](#variable-ns_ble_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_ble\_current\_version**](#variable-ns_ble_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_ble\_oldest\_supported\_version**](#variable-ns_ble_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  int | [**ns\_ble\_add\_characteristic**](#function-ns_ble_add_characteristic) ([**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* s, [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) \* c) <br>_Add a characteristic to a service. This function should be called after all characteristics have been defined using ns\_ble\_create\_characteristic._  |
|  int | [**ns\_ble\_char2uuid**](#function-ns_ble_char2uuid) (const char uuidString, [**ns\_ble\_uuid128\_t**](structns__ble__uuid128__t.md) \* uuid128) <br>_Helper function to convert a 16-byte UUID string to a 128-bit UUID byte array. UUID string must be exactly 16 characters long and only contain hex characters._  |
|  int | [**ns\_ble\_create\_characteristic**](#function-ns_ble_create_characteristic) ([**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) \* c, char const \* uuidString, void \* applicationValue, uint16\_t valueLength, uint16\_t properties, ns\_ble\_characteristic\_read\_handler\_t readHandlerCb, ns\_ble\_characteristic\_write\_handler\_t writeHandlerCb, ns\_ble\_characteristic\_notify\_handler\_t notifyHandlerCb, uint16\_t periodMs, uint16\_t \* attributeCount) <br>_Define a characteristic for subsequent addition to a service._  |
|  int | [**ns\_ble\_create\_service**](#function-ns_ble_create_service) ([**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* s) <br>_Create a BLE service based on the given configuration._  |
|  void | [**ns\_ble\_pre\_init**](#function-ns_ble_pre_init) (void) <br>_Call this function from the setup\_task, prior to creating the RadioTask All this does is set up NVIC priorities._  |
|  int | [**ns\_ble\_start\_service**](#function-ns_ble_start_service) ([**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* s) <br>_Invoked by RadioTask to start the service. This function should be called after all characteristics have been added._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_BLE\_API\_ID**](ns__ble_8h.md#define-ns_ble_api_id)  0xCA0008<br> |
| define  | [**NS\_BLE\_ATT\_UUID\_BUILD**](ns__ble_8h.md#define-ns_ble_att_uuid_build) (part)         NS\_BLE\_ATT\_UUID\_POSTAMBLE, UINT16\_TO\_BYTES(part), [**NS\_BLE\_ATT\_UUID\_PREAMBLE**](ns__ble_8h.md#define-ns_ble_att_uuid_preamble)<br> |
| define  | [**NS\_BLE\_ATT\_UUID\_POSTAMBLE**](ns__ble_8h.md#define-ns_ble_att_uuid_postamble)  0x14, 0x12, 0x8a, 0x76, 0x04, 0xd1, 0x6c, 0x4f, 0x7e, 0x53<br> |
| define  | [**NS\_BLE\_ATT\_UUID\_PREAMBLE**](ns__ble_8h.md#define-ns_ble_att_uuid_preamble)  0x00, 0x00, 0xb1, 0x19<br> |
| define  | [**NS\_BLE\_CCC\_DECL**](ns__ble_8h.md#define-ns_ble_ccc_decl) (\_prop) <br> |
| define  | [**NS\_BLE\_CHAR\_DECL**](ns__ble_8h.md#define-ns_ble_char_decl) (\_char) <br> |
| define  | [**NS\_BLE\_CHAR\_VAL**](ns__ble_8h.md#define-ns_ble_char_val) (\_prop, \_settings, \_permissions) <br> |
| define  | [**NS\_BLE\_CONN\_MAX**](ns__ble_8h.md#define-ns_ble_conn_max)  1<br> |
| define  | [**NS\_BLE\_CURRENT\_VERSION**](ns__ble_8h.md#define-ns_ble_current_version)  NS\_BLE\_V0\_0\_1<br> |
| define  | [**NS\_BLE\_MAX\_SERVICES**](ns__ble_8h.md#define-ns_ble_max_services)  1<br> |
| define  | [**NS\_BLE\_OLDEST\_SUPPORTED\_VERSION**](ns__ble_8h.md#define-ns_ble_oldest_supported_version)  NS\_BLE\_V0\_0\_1<br> |
| define  | [**NS\_BLE\_V0\_0\_1**](ns__ble_8h.md#define-ns_ble_v0_0_1)          { .major = 0, .minor = 0, .revision = 1 }<br> |

## Public Types Documentation




### enum @0 

```C++
enum @0 {
    NS_BLE_READ = 1,
    NS_BLE_WRITE = 2,
    NS_BLE_NOTIFY = 4,
    NS_BLE_PROP_MAX
};
```






### typedef ns\_ble\_characteristic\_notify\_handler\_t 

```C++
typedef int(* ns_ble_characteristic_notify_handler_t) (ns_ble_service_t *, ns_ble_characteristic_t *);
```






### typedef ns\_ble\_characteristic\_read\_handler\_t 

```C++
typedef int(* ns_ble_characteristic_read_handler_t) (ns_ble_service_t *, ns_ble_characteristic_t *, void *);
```






### typedef ns\_ble\_characteristic\_t 

```C++
typedef struct ns_ble_characteristic ns_ble_characteristic_t;
```






### typedef ns\_ble\_characteristic\_write\_handler\_t 

```C++
typedef int(* ns_ble_characteristic_write_handler_t) (ns_ble_service_t *, ns_ble_characteristic_t *, void *);
```






### typedef ns\_ble\_control\_t 

```C++
typedef struct ns_ble_control ns_ble_control_t;
```






### union ns\_ble\_msg\_t 

```C++

```






### typedef ns\_ble\_service\_control\_t 

```C++
typedef struct ns_ble_service_control ns_ble_service_control_t;
```






### typedef ns\_ble\_service\_specific\_handler\_cb 

```C++
typedef void(* ns_ble_service_specific_handler_cb) (wsfEventMask_t event, wsfMsgHdr_t *pMsg);
```






### typedef ns\_ble\_service\_specific\_handler\_init\_cb 

```C++
typedef void(* ns_ble_service_specific_handler_init_cb) (wsfHandlerId_t handlerId);
```






### typedef ns\_ble\_service\_specific\_init\_cb 

```C++
typedef void(* ns_ble_service_specific_init_cb) (wsfHandlerId_t handlerId, struct ns_ble_control *generic_cfg, struct ns_ble_service_control *specific_cfg);
```






### typedef ns\_ble\_service\_specific\_procMsg\_cb 

```C++
typedef bool(* ns_ble_service_specific_procMsg_cb) (ns_ble_msg_t *pMsg);
```






### typedef ns\_ble\_service\_t 

```C++
typedef struct ns_ble_service ns_ble_service_t;
```



## Public Attributes Documentation




### variable ns\_ble\_V0\_0\_1 

```C++
const ns_core_api_t ns_ble_V0_0_1;
```






### variable ns\_ble\_current\_version 

```C++
const ns_core_api_t ns_ble_current_version;
```






### variable ns\_ble\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_ble_oldest_supported_version;
```



## Public Functions Documentation




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





        



### function ns\_ble\_pre\_init 

```C++
void ns_ble_pre_init (
    void
) 
```






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





        
## Macro Definition Documentation





### define NS\_BLE\_API\_ID 

```C++
#define NS_BLE_API_ID 0xCA0008
```






### define NS\_BLE\_ATT\_UUID\_BUILD 

```C++
#define NS_BLE_ATT_UUID_BUILD (
    part
) NS_BLE_ATT_UUID_POSTAMBLE, UINT16_TO_BYTES(part), NS_BLE_ATT_UUID_PREAMBLE
```






### define NS\_BLE\_ATT\_UUID\_POSTAMBLE 

```C++
#define NS_BLE_ATT_UUID_POSTAMBLE 0x14, 0x12, 0x8a, 0x76, 0x04, 0xd1, 0x6c, 0x4f, 0x7e, 0x53
```






### define NS\_BLE\_ATT\_UUID\_PREAMBLE 


```C++
#define NS_BLE_ATT_UUID_PREAMBLE 0x00, 0x00, 0xb1, 0x19
```



Base UUID: 00002760-08C2-11E1-9073-0E8AC72EXXXX


Base UUID: 00002760-08C2-11E1-9073-0E8AC72EXXXX 


        



### define NS\_BLE\_CCC\_DECL 

```C++
#define NS_BLE_CCC_DECL (
    _prop
) {                                                                                          \
            .pUuid = attCliChCfgUuid, .pValue = (uint8_t *)_prop##ChCcc,                           \
            .pLen = (uint16_t *)&_prop##ChCccLen, .maxLen = sizeof(_prop##ChCcc),                  \
            .settings = ATTS_SET_CCC, .permissions = (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE),       \
        }
```






### define NS\_BLE\_CHAR\_DECL 

```C++
#define NS_BLE_CHAR_DECL (
    _char
) {                                                                                          \
            .pUuid = attChUuid, .pValue = (uint8_t *)_char##Ch, .pLen = (uint16_t *)&_char##ChLen, \
            .maxLen = sizeof(_char##Ch), .settings = 0, .permissions = ATTS_PERMIT_READ,           \
        }
```






### define NS\_BLE\_CHAR\_VAL 

```C++
#define NS_BLE_CHAR_VAL (
    _prop,
    _settings,
    _permissions
) {                                                                                          \
            .pUuid = _prop##ChUuid, .pValue = (uint8_t *)_prop##ChData,                            \
            .pLen = (uint16_t *)&_prop##ChDataLen, .maxLen = sizeof(_prop##ChData),                \
            .settings = _settings, .permissions = _permissions,                                    \
        }
```






### define NS\_BLE\_CONN\_MAX 

```C++
#define NS_BLE_CONN_MAX 1
```






### define NS\_BLE\_CURRENT\_VERSION 

```C++
#define NS_BLE_CURRENT_VERSION NS_BLE_V0_0_1
```






### define NS\_BLE\_MAX\_SERVICES 

```C++
#define NS_BLE_MAX_SERVICES 1
```






### define NS\_BLE\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_BLE_OLDEST_SUPPORTED_VERSION NS_BLE_V0_0_1
```






### define NS\_BLE\_V0\_0\_1 

```C++
#define NS_BLE_V0_0_1 { .major = 0, .minor = 0, .revision = 1 }
```




------------------------------


