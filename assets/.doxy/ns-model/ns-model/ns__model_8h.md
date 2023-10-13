

# File ns\_model.h



[**FileList**](files.md) **>** [**includes-api**](dir_4cff3ad1ea11578e3df8271aa3d25e93.md) **>** [**ns\_model.h**](ns__model_8h.md)

[Go to the source code of this file](ns__model_8h_source.md)


















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_model\_state\_t**](structns__model__state__t.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**ns\_model\_runtime\_e**](#enum-ns_model_runtime_e)  <br> |
| enum  | [**ns\_model\_states\_e**](#enum-ns_model_states_e)  <br> |




















## Public Functions

| Type | Name |
| ---: | :--- |
|  int | [**ns\_model\_init**](#function-ns_model_init) ([**ns\_model\_state\_t**](structns__model__state__t.md) \* ms) <br>_Initialize the model._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_MAX\_INPUT\_TENSORS**](ns__model_8h.md#define-ns_max_input_tensors)  3<br> |
| define  | [**NS\_MAX\_OUTPUT\_TENSORS**](ns__model_8h.md#define-ns_max_output_tensors)  3<br> |

## Public Types Documentation




### enum ns\_model\_runtime\_e 

```C++
enum ns_model_runtime_e {
    TFLM
};
```






### enum ns\_model\_states\_e 

```C++
enum ns_model_states_e {
    READY,
    NOT_READY,
    ERROR
};
```



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


        
## Macro Definition Documentation





### define NS\_MAX\_INPUT\_TENSORS 

```C++
#define NS_MAX_INPUT_TENSORS 3
```






### define NS\_MAX\_OUTPUT\_TENSORS 

```C++
#define NS_MAX_OUTPUT_TENSORS 3
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-model/includes-api/ns_model.h`

