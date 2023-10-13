

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
|  tflite::MicroProfiler \* | [**profiler**](#variable-profiler)  <br>_Profiler, initialized during init._  |
|  ns\_model\_runtime\_e | [**runtime**](#variable-runtime)  <br>_Future use._  |
|  uint8\_t \* | [**rv\_arena**](#variable-rv_arena)  <br>_ResourceVariable Arena._  |
|  uint32\_t | [**rv\_arena\_size**](#variable-rv_arena_size)  <br>_Size of RV arena, in bytes._  |
|  uint32\_t | [**rv\_count**](#variable-rv_count)  <br>_Number of resource variables._  |
|  ns\_model\_states\_e | [**state**](#variable-state)  <br> |
|  void \* | [**tickTimer**](#variable-ticktimer)  <br> |












































## Public Attributes Documentation




### variable arena 

```C++
uint8_t* ns_model_state_t::arena;
```






### variable arena\_size 

```C++
uint32_t ns_model_state_t::arena_size;
```






### variable computed\_arena\_size 

```C++
uint32_t ns_model_state_t::computed_arena_size;
```






### variable error\_reporter 

```C++
tflite::ErrorReporter* ns_model_state_t::error_reporter;
```






### variable interpreter 

```C++
tflite::MicroInterpreter* ns_model_state_t::interpreter;
```






### variable mac\_estimate 

```C++
void* ns_model_state_t::mac_estimate;
```






### variable model 

```C++
const tflite::Model* ns_model_state_t::model;
```






### variable model\_array 

```C++
const unsigned char* ns_model_state_t::model_array;
```






### variable model\_input 

```C++
TfLiteTensor* ns_model_state_t::model_input[NS_MAX_INPUT_TENSORS];
```






### variable model\_output 

```C++
TfLiteTensor* ns_model_state_t::model_output[NS_MAX_OUTPUT_TENSORS];
```






### variable numInputTensors 

```C++
uint32_t ns_model_state_t::numInputTensors;
```






### variable numOutputTensors 

```C++
uint32_t ns_model_state_t::numOutputTensors;
```






### variable profiler 

```C++
tflite::MicroProfiler* ns_model_state_t::profiler;
```






### variable runtime 

```C++
ns_model_runtime_e ns_model_state_t::runtime;
```






### variable rv\_arena 

```C++
uint8_t* ns_model_state_t::rv_arena;
```






### variable rv\_arena\_size 

```C++
uint32_t ns_model_state_t::rv_arena_size;
```






### variable rv\_count 

```C++
uint32_t ns_model_state_t::rv_count;
```






### variable state 

```C++
ns_model_states_e ns_model_state_t::state;
```






### variable tickTimer 

```C++
void* ns_model_state_t::tickTimer;
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-model/includes-api/ns_model.h`

