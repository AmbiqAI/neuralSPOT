

# Struct ns\_pdm\_cfg\_t



[**ClassList**](annotated.md) **>** [**ns\_pdm\_cfg\_t**](structns__pdm__cfg__t.md)



_PDM Configuration._ 

* `#include <ns_audio.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  [**ns\_audio\_clksel\_e**](ns__audio_8h.md#enum-ns_audio_clksel_e) | [**clock**](#variable-clock)  <br> |
|  [**ns\_audio\_pdm\_clock\_e**](ns__audio_8h.md#enum-ns_audio_pdm_clock_e) | [**clock\_freq**](#variable-clock_freq)  <br> |
|  am\_hal\_pdm\_gain\_e | [**left\_gain**](#variable-left_gain)  <br> |
|  [**ns\_audio\_pdm\_micsel\_e**](ns__audio_8h.md#enum-ns_audio_pdm_micsel_e) | [**mic**](#variable-mic)  <br>_VoS Kit breakout board PDM mic slot._  |
|  uint8\_t | [**numBytes**](#variable-numbytes)  <br> |
|  am\_hal\_pdm\_gain\_e | [**right\_gain**](#variable-right_gain)  <br> |












































## Public Attributes Documentation




### variable clock 

```C++
ns_audio_clksel_e ns_pdm_cfg_t::clock;
```




<hr>



### variable clock\_freq 

```C++
ns_audio_pdm_clock_e ns_pdm_cfg_t::clock_freq;
```




<hr>



### variable left\_gain 

```C++
am_hal_pdm_gain_e ns_pdm_cfg_t::left_gain;
```




<hr>



### variable mic 

_VoS Kit breakout board PDM mic slot._ 
```C++
ns_audio_pdm_micsel_e ns_pdm_cfg_t::mic;
```




<hr>



### variable numBytes 

```C++
uint8_t ns_pdm_cfg_t::numBytes;
```




<hr>



### variable right\_gain 

```C++
am_hal_pdm_gain_e ns_pdm_cfg_t::right_gain;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/includes-api/ns_audio.h`

