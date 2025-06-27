

# File ns\_ble\_tests.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-ble**](dir_ec3c5c5ea2d338d436d6fa61f38fc381.md) **>** [**tests**](dir_8983aa85a9e91ac6633429bf8b0fa928.md) **>** [**ns\_ble\_tests.c**](ns__ble__tests_8c.md)

[Go to the source code of this file](ns__ble__tests_8c_source.md)



* `#include "unity/unity.h"`
* `#include "ns_ble.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint16\_t | [**humidity**](#variable-humidity)   = `2`<br> |
|  float | [**temperature**](#variable-temperature)   = `0.0`<br> |
|  size\_t const | [**ucHeapSize**](#variable-ucheapsize)   = `configTOTAL\_HEAP\_SIZE`<br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webbleAccel**](#variable-webbleaccel)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webbleBsec**](#variable-webblebsec)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webbleCo2**](#variable-webbleco2)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webbleGas**](#variable-webblegas)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webbleGyro**](#variable-webblegyro)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webbleHumidity**](#variable-webblehumidity)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webblePressure**](#variable-webblepressure)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webbleQuat**](#variable-webblequat)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webbleRgb**](#variable-webblergb)  <br> |
|  [**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) | [**webbleService**](#variable-webbleservice)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) | [**webbleTemperature**](#variable-webbletemperature)  <br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  wsfBufPoolDesc\_t | [**webbleBufferDescriptors**](#variable-webblebufferdescriptors)   = `/* multi line expression */`<br> |
|  uint32\_t | [**webbleWSFBufferPool**](#variable-webblewsfbufferpool)  <br> |
|  [**ns\_ble\_pool\_config\_t**](structns__ble__pool__config__t.md) | [**webbleWsfBuffers**](#variable-webblewsfbuffers)   = `/* multi line expression */`<br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  uint8\_t ucHeap[NS\_MALLOC\_HEAP\_SIZE\_IN\_K \*1024] | [**\_\_attribute\_\_**](#function-__attribute__) ((aligned(4))) <br> |
|  void | [**ns\_ble\_characteristic\_test**](#function-ns_ble_characteristic_test) () <br> |
|  void | [**ns\_ble\_create\_different\_service\_test**](#function-ns_ble_create_different_service_test) () <br> |
|  void | [**ns\_ble\_create\_null\_service\_test**](#function-ns_ble_create_null_service_test) () <br> |
|  void | [**ns\_ble\_create\_service\_test**](#function-ns_ble_create_service_test) () <br> |
|  void | [**ns\_ble\_create\_service\_test\_no\_characteristics**](#function-ns_ble_create_service_test_no_characteristics) () <br> |
|  void | [**ns\_ble\_empty\_service\_add\_characteristic\_test**](#function-ns_ble_empty_service_add_characteristic_test) () <br> |
|  void | [**ns\_ble\_multiple\_characteristics\_fail\_test**](#function-ns_ble_multiple_characteristics_fail_test) () <br> |
|  void | [**ns\_ble\_multiple\_characteristics\_test**](#function-ns_ble_multiple_characteristics_test) () <br> |
|  void | [**ns\_ble\_negative\_attribute\_test**](#function-ns_ble_negative_attribute_test) () <br> |
|  void | [**ns\_ble\_start\_service\_test**](#function-ns_ble_start_service_test) () <br> |
|  void | [**ns\_ble\_tests\_post\_test\_hook**](#function-ns_ble_tests_post_test_hook) () <br> |
|  void | [**ns\_ble\_tests\_pre\_test\_hook**](#function-ns_ble_tests_pre_test_hook) () <br> |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  int | [**webbleReadHandler**](#function-webblereadhandler) ([**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* s, struct [**ns\_ble\_characteristic**](structns__ble__characteristic.md) \* c, void \* dest) <br> |
|  int | [**webbleWriteHandler**](#function-webblewritehandler) ([**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* s, struct [**ns\_ble\_characteristic**](structns__ble__characteristic.md) \* c, void \* src) <br> |

























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**WEBBLE\_WSF\_BUFFER\_POOLS**](ns__ble__tests_8c.md#define-webble_wsf_buffer_pools)  `4`<br> |
| define  | [**WEBBLE\_WSF\_BUFFER\_SIZE**](ns__ble__tests_8c.md#define-webble_wsf_buffer_size)  `(WEBBLE\_WSF\_BUFFER\_POOLS \* 16 + 16 \* 8 + 32 \* 4 + 64 \* 6 + 280 \* 14) / sizeof(uint32\_t)`<br> |
| define  | [**configTOTAL\_HEAP\_SIZE**](ns__ble__tests_8c.md#define-configtotal_heap_size)  `NS\_MALLOC\_HEAP\_SIZE\_IN\_K \* 1024`<br> |
| define  | [**webbleUuid**](ns__ble__tests_8c.md#define-webbleuuid) (uuid) `"19b10000" uuid "537e4f6cd104768a1214"`<br> |

## Public Attributes Documentation




### variable humidity 

```C++
uint16_t humidity;
```




<hr>



### variable temperature 

```C++
float temperature;
```




<hr>



### variable ucHeapSize 

```C++
size_t const ucHeapSize;
```




<hr>



### variable webbleAccel 

```C++
ns_ble_characteristic_t webbleAccel;
```




<hr>



### variable webbleBsec 

```C++
ns_ble_characteristic_t webbleBsec;
```




<hr>



### variable webbleCo2 

```C++
ns_ble_characteristic_t webbleCo2;
```




<hr>



### variable webbleGas 

```C++
ns_ble_characteristic_t webbleGas;
```




<hr>



### variable webbleGyro 

```C++
ns_ble_characteristic_t webbleGyro;
```




<hr>



### variable webbleHumidity 

```C++
ns_ble_characteristic_t webbleHumidity;
```




<hr>



### variable webblePressure 

```C++
ns_ble_characteristic_t webblePressure;
```




<hr>



### variable webbleQuat 

```C++
ns_ble_characteristic_t webbleQuat;
```




<hr>



### variable webbleRgb 

```C++
ns_ble_characteristic_t webbleRgb;
```




<hr>



### variable webbleService 

```C++
ns_ble_service_t webbleService;
```




<hr>



### variable webbleTemperature 

```C++
ns_ble_characteristic_t webbleTemperature;
```




<hr>
## Public Static Attributes Documentation




### variable webbleBufferDescriptors 

```C++
wsfBufPoolDesc_t webbleBufferDescriptors[WEBBLE_WSF_BUFFER_POOLS];
```




<hr>



### variable webbleWSFBufferPool 

```C++
uint32_t webbleWSFBufferPool[WEBBLE_WSF_BUFFER_SIZE];
```




<hr>



### variable webbleWsfBuffers 

```C++
ns_ble_pool_config_t webbleWsfBuffers;
```




<hr>
## Public Functions Documentation




### function \_\_attribute\_\_ 

```C++
uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K *1024] __attribute__ (
    (aligned(4))
) 
```




<hr>



### function ns\_ble\_characteristic\_test 

```C++
void ns_ble_characteristic_test () 
```




<hr>



### function ns\_ble\_create\_different\_service\_test 

```C++
void ns_ble_create_different_service_test () 
```




<hr>



### function ns\_ble\_create\_null\_service\_test 

```C++
void ns_ble_create_null_service_test () 
```




<hr>



### function ns\_ble\_create\_service\_test 

```C++
void ns_ble_create_service_test () 
```




<hr>



### function ns\_ble\_create\_service\_test\_no\_characteristics 

```C++
void ns_ble_create_service_test_no_characteristics () 
```




<hr>



### function ns\_ble\_empty\_service\_add\_characteristic\_test 

```C++
void ns_ble_empty_service_add_characteristic_test () 
```




<hr>



### function ns\_ble\_multiple\_characteristics\_fail\_test 

```C++
void ns_ble_multiple_characteristics_fail_test () 
```




<hr>



### function ns\_ble\_multiple\_characteristics\_test 

```C++
void ns_ble_multiple_characteristics_test () 
```




<hr>



### function ns\_ble\_negative\_attribute\_test 

```C++
void ns_ble_negative_attribute_test () 
```




<hr>



### function ns\_ble\_start\_service\_test 

```C++
void ns_ble_start_service_test () 
```




<hr>



### function ns\_ble\_tests\_post\_test\_hook 

```C++
void ns_ble_tests_post_test_hook () 
```




<hr>



### function ns\_ble\_tests\_pre\_test\_hook 

```C++
void ns_ble_tests_pre_test_hook () 
```




<hr>
## Public Static Functions Documentation




### function webbleReadHandler 

```C++
static int webbleReadHandler (
    ns_ble_service_t * s,
    struct ns_ble_characteristic * c,
    void * dest
) 
```




<hr>



### function webbleWriteHandler 

```C++
static int webbleWriteHandler (
    ns_ble_service_t * s,
    struct ns_ble_characteristic * c,
    void * src
) 
```




<hr>
## Macro Definition Documentation





### define WEBBLE\_WSF\_BUFFER\_POOLS 

```C++
#define WEBBLE_WSF_BUFFER_POOLS `4`
```




<hr>



### define WEBBLE\_WSF\_BUFFER\_SIZE 

```C++
#define WEBBLE_WSF_BUFFER_SIZE `(WEBBLE_WSF_BUFFER_POOLS * 16 + 16 * 8 + 32 * 4 + 64 * 6 + 280 * 14) / sizeof(uint32_t)`
```




<hr>



### define configTOTAL\_HEAP\_SIZE 

```C++
#define configTOTAL_HEAP_SIZE `NS_MALLOC_HEAP_SIZE_IN_K * 1024`
```




<hr>



### define webbleUuid 

```C++
#define webbleUuid (
    uuid
) `"19b10000" uuid "537e4f6cd104768a1214"`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-ble/tests/ns_ble_tests.c`

