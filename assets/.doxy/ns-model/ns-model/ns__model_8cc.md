

# File ns\_model.cc



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-model**](dir_0e0d7b58960104054b7f7c1d73877dc4.md) **>** [**src**](dir_788645d5adb2eb568128006739007a6c.md) **>** [**ns\_model.cc**](ns__model_8cc.md)

[Go to the source code of this file](ns__model_8cc_source.md)



* `#include "ns_model.h"`
* `#include "ns_ambiqsuite_harness.h"`
* `#include "ns_debug_log.h"`
* `#include "tensorflow/lite/micro/kernels/micro_ops.h"`
* `#include "tensorflow/lite/micro/micro_interpreter.h"`
* `#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"`
* `#include "tensorflow/lite/micro/micro_profiler.h"`
* `#include "tensorflow/lite/micro/system_setup.h"`
* `#include "tensorflow/lite/schema/schema_generated.h"`
* `#include "tensorflow/lite/micro/micro_error_reporter.h"`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  int | [**ns\_model\_init**](#function-ns_model_init) ([**ns\_model\_state\_t**](structns__model__state__t.md) \* ms) <br>_Initialize the model._  |
|  TfLiteTensor \* | [**ns\_tf\_get\_input\_tensor**](#function-ns_tf_get_input_tensor) ([**ns\_model\_state\_t**](structns__model__state__t.md) \* ms, uint32\_t index) <br> |
|  uint32\_t | [**ns\_tf\_get\_num\_input\_tensors**](#function-ns_tf_get_num_input_tensors) ([**ns\_model\_state\_t**](structns__model__state__t.md) \* ms) <br> |
|  uint32\_t | [**ns\_tf\_get\_num\_output\_tensors**](#function-ns_tf_get_num_output_tensors) ([**ns\_model\_state\_t**](structns__model__state__t.md) \* ms) <br> |
|  TfLiteTensor \* | [**ns\_tf\_get\_output\_tensor**](#function-ns_tf_get_output_tensor) ([**ns\_model\_state\_t**](structns__model__state__t.md) \* ms, uint32\_t index) <br> |




























## Public Functions Documentation




### function ns\_model\_init 

_Initialize the model._ 
```C++
int ns_model_init (
    ns_model_state_t * ms
) 
```





**Parameters:**


* `ms` Model state and configuration struct 



**Returns:**

int status


Initialize the model.


This code is fairly common across most TF-based models. The major differences relate to input and output tensor handling. 


        



### function ns\_tf\_get\_input\_tensor 

```C++
TfLiteTensor * ns_tf_get_input_tensor (
    ns_model_state_t * ms,
    uint32_t index
) 
```






### function ns\_tf\_get\_num\_input\_tensors 

```C++
uint32_t ns_tf_get_num_input_tensors (
    ns_model_state_t * ms
) 
```






### function ns\_tf\_get\_num\_output\_tensors 

```C++
uint32_t ns_tf_get_num_output_tensors (
    ns_model_state_t * ms
) 
```






### function ns\_tf\_get\_output\_tensor 

```C++
TfLiteTensor * ns_tf_get_output_tensor (
    ns_model_state_t * ms,
    uint32_t index
) 
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-model/src/ns_model.cc`

