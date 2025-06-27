

# Struct ns\_model\_state\_t



[**ClassList**](annotated.md) **>** [**ns\_model\_state\_t**](structns__model__state__t.md)


























## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint8\_t \* | [**arena**](#variable-arena)  <br>_Tensor Arena._  |
|  uint32\_t | [**arena\_size**](#variable-arena_size)  <br>_Size of tensor arena, in bytes._  |
|  uint32\_t | [**computed\_arena\_size**](#variable-computed_arena_size)  <br> |
|  tflite::ErrorReporter \* | [**error\_reporter**](#variable-error_reporter)  <br>_Error reporter, initialized during init._  |
|  tflite::MicroInterpreter \* | [**interpreter**](#variable-interpreter)  <br>_Interpreter, initialized during init._  |
|  void \* | [**mac\_estimate**](#variable-mac_estimate)  <br> |
|  const tflite::Model \* | [**model**](#variable-model)  <br>_Model structure, initialized during init._  |
|  const unsigned char \* | [**model\_array**](#variable-model_array)  <br> |
|  TfLiteTensor \* | [**model\_input**](#variable-model_input)  <br>_Input tensors, initialized during init._  |
|  TfLiteTensor \* | [**model\_output**](#variable-model_output)  <br>_Output tensors, initialized during init._  |
|  uint32\_t | [**numInputTensors**](#variable-numinputtensors)  <br>_Number of input tensors._  |
|  uint32\_t | [**numOutputTensors**](#variable-numoutputtensors)  <br>_Number of output tensors._  |
|  void \* | [**pmu**](#variable-pmu)  <br> |
|  tflite::MicroProfiler \* | [**profiler**](#variable-profiler)  <br>_Profiler, initialized during init._  |
|  ns\_model\_runtime\_e | [**runtime**](#variable-runtime)  <br>_Future use._  |
|  uint8\_t \* | [**rv\_arena**](#variable-rv_arena)  <br>_ResourceVariable Arena._  |
|  uint32\_t | [**rv\_arena\_size**](#variable-rv_arena_size)  <br>_Size of RV arena, in bytes._  |
|  uint32\_t | [**rv\_count**](#variable-rv_count)  <br>_Number of resource variables._  |
|  ns\_model\_states\_e | [**state**](#variable-state)  <br> |
|  void \* | [**tickTimer**](#variable-ticktimer)  <br> |












































## Public Attributes Documentation




### variable arena 

_Tensor Arena._ 
```C++
uint8_t* ns_model_state_t::arena;
```




<hr>



### variable arena\_size 

_Size of tensor arena, in bytes._ 
```C++
uint32_t ns_model_state_t::arena_size;
```




<hr>



### variable computed\_arena\_size 

```C++
uint32_t ns_model_state_t::computed_arena_size;
```




<hr>



### variable error\_reporter 

_Error reporter, initialized during init._ 
```C++
tflite::ErrorReporter* ns_model_state_t::error_reporter;
```




<hr>



### variable interpreter 

_Interpreter, initialized during init._ 
```C++
tflite::MicroInterpreter* ns_model_state_t::interpreter;
```




<hr>



### variable mac\_estimate 

```C++
void* ns_model_state_t::mac_estimate;
```




<hr>



### variable model 

_Model structure, initialized during init._ 
```C++
const tflite::Model* ns_model_state_t::model;
```




<hr>



### variable model\_array 

```C++
const unsigned char* ns_model_state_t::model_array;
```




<hr>



### variable model\_input 

_Input tensors, initialized during init._ 
```C++
TfLiteTensor* ns_model_state_t::model_input[NS_MAX_INPUT_TENSORS];
```




<hr>



### variable model\_output 

_Output tensors, initialized during init._ 
```C++
TfLiteTensor* ns_model_state_t::model_output[NS_MAX_OUTPUT_TENSORS];
```




<hr>



### variable numInputTensors 

_Number of input tensors._ 
```C++
uint32_t ns_model_state_t::numInputTensors;
```




<hr>



### variable numOutputTensors 

_Number of output tensors._ 
```C++
uint32_t ns_model_state_t::numOutputTensors;
```




<hr>



### variable pmu 

```C++
void* ns_model_state_t::pmu;
```




<hr>



### variable profiler 

_Profiler, initialized during init._ 
```C++
tflite::MicroProfiler* ns_model_state_t::profiler;
```




<hr>



### variable runtime 

_Future use._ 
```C++
ns_model_runtime_e ns_model_state_t::runtime;
```




<hr>



### variable rv\_arena 

_ResourceVariable Arena._ 
```C++
uint8_t* ns_model_state_t::rv_arena;
```




<hr>



### variable rv\_arena\_size 

_Size of RV arena, in bytes._ 
```C++
uint32_t ns_model_state_t::rv_arena_size;
```




<hr>



### variable rv\_count 

_Number of resource variables._ 
```C++
uint32_t ns_model_state_t::rv_count;
```




<hr>



### variable state 

```C++
ns_model_states_e ns_model_state_t::state;
```




<hr>



### variable tickTimer 

```C++
void* ns_model_state_t::tickTimer;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-model/includes-api/ns_model.h`

