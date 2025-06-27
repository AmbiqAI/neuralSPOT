

# File ns\_power\_profile.c



[**FileList**](files.md) **>** [**apollo5**](dir_4b586f9555feab2a00d1b63aa502548f.md) **>** [**ns\_power\_profile.c**](apollo5_2ns__power__profile_8c.md)

[Go to the source code of this file](apollo5_2ns__power__profile_8c_source.md)



* `#include <stdio.h>`
* `#include <stdlib.h>`
* `#include <stdarg.h>`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_power_profile.h"`
* `#include "am_util_debug.h"`
* `#include "am_bsp.h"`
* `#include "ns_ambiqsuite_harness.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**regmap\_t**](structregmap__t.md) <br> |








## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  const [**regmap\_t**](structregmap__t.md) | [**g\_regmap**](#variable-g_regmap)  <br> |
|  bool | [**g\_snapshotValid**](#variable-g_snapshotvalid)   = `{false, false, false, false, false}`<br> |
|  uint32\_t | [**g\_snapshots**](#variable-g_snapshots)  <br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**capture\_snapshot**](#function-capture_snapshot) (int snapshotIndex) <br> |
|  void | [**print\_snapshot**](#function-print_snapshot) (int snapshotIndex, bool bJson) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**MCUCTRL\_CPUDCACHETRIM**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_cpudcachetrim)  `(\*(volatile uint32\_t\*)0x40020428)`<br> |
| define  | [**MCUCTRL\_CPUICACHETRIM**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_cpuicachetrim)  `(\*(volatile uint32\_t\*)0x40020424)`<br> |
| define  | [**MCUCTRL\_HFRC**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_hfrc)  `(\*(volatile uint32\_t\*)0x400200C0)`<br> |
| define  | [**MCUCTRL\_HFRC2**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_hfrc2)  `(\*(volatile uint32\_t\*)0x400200C4)`<br> |
| define  | [**MCUCTRL\_MISCPWRCTRL**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_miscpwrctrl)  `(\*(volatile uint32\_t\*)0x40020198)`<br> |
| define  | [**MCUCTRL\_SRAMTRIM**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_sramtrim)  `(\*(volatile uint32\_t\*)0x40020334)`<br> |
| define  | [**MCUCTRL\_SRAMTRIMHP**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_sramtrimhp)  `(\*(volatile uint32\_t\*)0x40020330)`<br> |
| define  | [**MCUCTRL\_SSRAMTRIM**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_ssramtrim)  `(\*(volatile uint32\_t\*)0x4002042C)`<br> |
| define  | [**MCUCTRL\_VREFGEN**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_vrefgen)  `(\*(volatile uint32\_t\*)0x40020040)`<br> |
| define  | [**MCUCTRL\_VREFGEN3**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_vrefgen3)  `(\*(volatile uint32\_t\*)0x40020048)`<br> |
| define  | [**MCUCTRL\_VREFGEN5**](apollo5_2ns__power__profile_8c.md#define-mcuctrl_vrefgen5)  `(\*(volatile uint32\_t\*)0x40020050)`<br> |
| define  | [**REGMAP\_SIZE**](apollo5_2ns__power__profile_8c.md#define-regmap_size)  `(sizeof(g\_regmap)/sizeof(g\_regmap[0]))`<br> |

## Public Static Attributes Documentation




### variable g\_regmap 

```C++
const regmap_t g_regmap[];
```




<hr>



### variable g\_snapshotValid 

```C++
bool g_snapshotValid[5];
```




<hr>



### variable g\_snapshots 

```C++
uint32_t g_snapshots[5][REGMAP_SIZE];
```




<hr>
## Public Functions Documentation




### function capture\_snapshot 

```C++
void capture_snapshot (
    int snapshotIndex
) 
```




<hr>



### function print\_snapshot 

```C++
void print_snapshot (
    int snapshotIndex,
    bool bJson
) 
```




<hr>
## Macro Definition Documentation





### define MCUCTRL\_CPUDCACHETRIM 

```C++
#define MCUCTRL_CPUDCACHETRIM `(*(volatile uint32_t*)0x40020428)`
```




<hr>



### define MCUCTRL\_CPUICACHETRIM 

```C++
#define MCUCTRL_CPUICACHETRIM `(*(volatile uint32_t*)0x40020424)`
```




<hr>



### define MCUCTRL\_HFRC 

```C++
#define MCUCTRL_HFRC `(*(volatile uint32_t*)0x400200C0)`
```




<hr>



### define MCUCTRL\_HFRC2 

```C++
#define MCUCTRL_HFRC2 `(*(volatile uint32_t*)0x400200C4)`
```




<hr>



### define MCUCTRL\_MISCPWRCTRL 

```C++
#define MCUCTRL_MISCPWRCTRL `(*(volatile uint32_t*)0x40020198)`
```




<hr>



### define MCUCTRL\_SRAMTRIM 

```C++
#define MCUCTRL_SRAMTRIM `(*(volatile uint32_t*)0x40020334)`
```




<hr>



### define MCUCTRL\_SRAMTRIMHP 

```C++
#define MCUCTRL_SRAMTRIMHP `(*(volatile uint32_t*)0x40020330)`
```




<hr>



### define MCUCTRL\_SSRAMTRIM 

```C++
#define MCUCTRL_SSRAMTRIM `(*(volatile uint32_t*)0x4002042C)`
```




<hr>



### define MCUCTRL\_VREFGEN 

```C++
#define MCUCTRL_VREFGEN `(*(volatile uint32_t*)0x40020040)`
```




<hr>



### define MCUCTRL\_VREFGEN3 

```C++
#define MCUCTRL_VREFGEN3 `(*(volatile uint32_t*)0x40020048)`
```




<hr>



### define MCUCTRL\_VREFGEN5 

```C++
#define MCUCTRL_VREFGEN5 `(*(volatile uint32_t*)0x40020050)`
```




<hr>



### define REGMAP\_SIZE 

```C++
#define REGMAP_SIZE `(sizeof(g_regmap)/sizeof(g_regmap[0]))`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/apollo5/ns_power_profile.c`

