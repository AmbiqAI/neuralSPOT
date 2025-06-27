

# File picojpeg.c



[**FileList**](files.md) **>** [**jpeg-decoder**](dir_09e5e1a913ce3e01b48c441fa9bab0f2.md) **>** [**picojpeg.c**](picojpeg_8c.md)

[Go to the source code of this file](picojpeg_8c_source.md)



* `#include "picojpeg.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**HuffTableT**](struct_huff_table_t.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| typedef struct [**HuffTableT**](struct_huff_table_t.md) | [**HuffTable**](#typedef-hufftable)  <br> |
| enum  | [**JPEG\_MARKER**](#enum-jpeg_marker)  <br> |
| typedef signed short | [**int16**](#typedef-int16)  <br> |
| typedef signed char | [**int8**](#typedef-int8)  <br> |
| typedef unsigned short | [**uint16**](#typedef-uint16)  <br> |
| typedef unsigned char | [**uint8**](#typedef-uint8)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  const uint8 | [**gWinogradQuant**](#variable-gwinogradquant)   = `/* multi line expression */`<br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  const int8 | [**ZAG**](#variable-zag)   = `/* multi line expression */`<br> |
|  uint16 | [**gBitBuf**](#variable-gbitbuf)  <br> |
|  uint8 | [**gBitsLeft**](#variable-gbitsleft)  <br> |
|  uint8 | [**gCallbackStatus**](#variable-gcallbackstatus)  <br> |
|  int16 | [**gCoeffBuf**](#variable-gcoeffbuf)  <br> |
|  uint8 | [**gCompACTab**](#variable-gcompactab)  <br> |
|  uint8 | [**gCompDCTab**](#variable-gcompdctab)  <br> |
|  uint8 | [**gCompHSamp**](#variable-gcomphsamp)  <br> |
|  uint8 | [**gCompIdent**](#variable-gcompident)  <br> |
|  uint8 | [**gCompList**](#variable-gcomplist)  <br> |
|  uint8 | [**gCompQuant**](#variable-gcompquant)  <br> |
|  uint8 | [**gCompVSamp**](#variable-gcompvsamp)  <br> |
|  uint8 | [**gCompsInFrame**](#variable-gcompsinframe)  <br> |
|  uint8 | [**gCompsInScan**](#variable-gcompsinscan)  <br> |
|  [**HuffTable**](struct_huff_table_t.md) | [**gHuffTab0**](#variable-ghufftab0)  <br> |
|  [**HuffTable**](struct_huff_table_t.md) | [**gHuffTab1**](#variable-ghufftab1)  <br> |
|  [**HuffTable**](struct_huff_table_t.md) | [**gHuffTab2**](#variable-ghufftab2)  <br> |
|  [**HuffTable**](struct_huff_table_t.md) | [**gHuffTab3**](#variable-ghufftab3)  <br> |
|  uint8 | [**gHuffVal0**](#variable-ghuffval0)  <br> |
|  uint8 | [**gHuffVal1**](#variable-ghuffval1)  <br> |
|  uint8 | [**gHuffVal2**](#variable-ghuffval2)  <br> |
|  uint8 | [**gHuffVal3**](#variable-ghuffval3)  <br> |
|  uint16 | [**gImageXSize**](#variable-gimagexsize)  <br> |
|  uint16 | [**gImageYSize**](#variable-gimageysize)  <br> |
|  uint8 | [**gInBuf**](#variable-ginbuf)  <br> |
|  uint8 | [**gInBufLeft**](#variable-ginbufleft)  <br> |
|  uint8 | [**gInBufOfs**](#variable-ginbufofs)  <br> |
|  int16 | [**gLastDC**](#variable-glastdc)  <br> |
|  uint8 | [**gMCUBufB**](#variable-gmcubufb)  <br> |
|  uint8 | [**gMCUBufG**](#variable-gmcubufg)  <br> |
|  uint8 | [**gMCUBufR**](#variable-gmcubufr)  <br> |
|  uint8 | [**gMCUOrg**](#variable-gmcuorg)  <br> |
|  uint8 | [**gMaxBlocksPerMCU**](#variable-gmaxblockspermcu)  <br> |
|  uint16 | [**gMaxMCUSPerCol**](#variable-gmaxmcuspercol)  <br> |
|  uint16 | [**gMaxMCUSPerRow**](#variable-gmaxmcusperrow)  <br> |
|  uint8 | [**gMaxMCUXSize**](#variable-gmaxmcuxsize)  <br> |
|  uint8 | [**gMaxMCUYSize**](#variable-gmaxmcuysize)  <br> |
|  uint16 | [**gNextRestartNum**](#variable-gnextrestartnum)  <br> |
|  uint16 | [**gNumMCUSRemainingX**](#variable-gnummcusremainingx)  <br> |
|  uint16 | [**gNumMCUSRemainingY**](#variable-gnummcusremainingy)  <br> |
|  int16 | [**gQuant0**](#variable-gquant0)  <br> |
|  int16 | [**gQuant1**](#variable-gquant1)  <br> |
|  uint8 | [**gReduce**](#variable-greduce)  <br> |
|  uint16 | [**gRestartInterval**](#variable-grestartinterval)  <br> |
|  uint16 | [**gRestartsLeft**](#variable-grestartsleft)  <br> |
|  pjpeg\_scan\_type\_t | [**gScanType**](#variable-gscantype)  <br> |
|  uint8 | [**gTemFlag**](#variable-gtemflag)  <br> |
|  uint8 | [**gValidHuffTables**](#variable-gvalidhufftables)  <br> |
|  uint8 | [**gValidQuantTables**](#variable-gvalidquanttables)  <br> |
|  void \* | [**g\_pCallback\_data**](#variable-g_pcallback_data)  <br> |
|  pjpeg\_need\_bytes\_callback\_t | [**g\_pNeedBytesCallback**](#variable-g_pneedbytescallback)  <br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  unsigned char | [**pjpeg\_decode\_init**](#function-pjpeg_decode_init) ([**pjpeg\_image\_info\_t**](structpjpeg__image__info__t.md) \* pInfo, pjpeg\_need\_bytes\_callback\_t pNeed\_bytes\_callback, void \* pCallback\_data, unsigned char reduce) <br> |
|  unsigned char | [**pjpeg\_decode\_mcu**](#function-pjpeg_decode_mcu) (void) <br> |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  PJPG\_INLINE uint8 | [**addAndClamp**](#function-addandclamp) (uint8 a, int16 b) <br> |
|  uint8 | [**checkHuffTables**](#function-checkhufftables) (void) <br> |
|  uint8 | [**checkQuantTables**](#function-checkquanttables) (void) <br> |
|  PJPG\_INLINE uint8 | [**clamp**](#function-clamp) (int16 s) <br> |
|  void | [**convertCb**](#function-convertcb) (uint8 dstOfs) <br> |
|  void | [**convertCr**](#function-convertcr) (uint8 dstOfs) <br> |
|  void | [**copyY**](#function-copyy) (uint8 dstOfs) <br> |
|  void | [**createWinogradQuant**](#function-createwinogradquant) (int16 \* pQuant) <br> |
|  uint8 | [**decodeNextMCU**](#function-decodenextmcu) (void) <br> |
|  void | [**fillInBuf**](#function-fillinbuf) (void) <br> |
|  uint8 | [**findEOI**](#function-findeoi) (void) <br> |
|  void | [**fixInBuffer**](#function-fixinbuffer) (void) <br> |
|  PJPG\_INLINE uint8 | [**getBit**](#function-getbit) (void) <br> |
|  uint16 | [**getBits**](#function-getbits) (uint8 numBits, uint8 FFCheck) <br> |
|  PJPG\_INLINE uint16 | [**getBits1**](#function-getbits1) (uint8 numBits) <br> |
|  PJPG\_INLINE uint16 | [**getBits2**](#function-getbits2) (uint8 numBits) <br> |
|  PJPG\_INLINE uint8 | [**getChar**](#function-getchar) (void) <br> |
|  int16 | [**getExtendOffset**](#function-getextendoffset) (uint8 i) <br> |
|  uint16 | [**getExtendTest**](#function-getextendtest) (uint8 i) <br> |
|  [**HuffTable**](struct_huff_table_t.md) \* | [**getHuffTable**](#function-gethufftable) (uint8 index) <br> |
|  uint8 \* | [**getHuffVal**](#function-gethuffval) (uint8 index) <br> |
|  uint16 | [**getMaxHuffCodes**](#function-getmaxhuffcodes) (uint8 index) <br> |
|  PJPG\_INLINE uint8 | [**getOctet**](#function-getoctet) (uint8 FFCheck) <br> |
|  void | [**huffCreate**](#function-huffcreate) (const uint8 \* pBits, [**HuffTable**](struct_huff_table_t.md) \* pHuffTable) <br> |
|  PJPG\_INLINE uint8 | [**huffDecode**](#function-huffdecode) (const [**HuffTable**](struct_huff_table_t.md) \* pHuffTable, const uint8 \* pHuffVal) <br> |
|  PJPG\_INLINE int16 | [**huffExtend**](#function-huffextend) (uint16 x, uint8 s) <br> |
|  void | [**idctCols**](#function-idctcols) (void) <br> |
|  void | [**idctRows**](#function-idctrows) (void) <br> |
|  PJPG\_INLINE int16 | [**imul\_b1\_b3**](#function-imul_b1_b3) (int16 w) <br> |
|  PJPG\_INLINE int16 | [**imul\_b2**](#function-imul_b2) (int16 w) <br> |
|  PJPG\_INLINE int16 | [**imul\_b4**](#function-imul_b4) (int16 w) <br> |
|  PJPG\_INLINE int16 | [**imul\_b5**](#function-imul_b5) (int16 w) <br> |
|  uint8 | [**init**](#function-init) (void) <br> |
|  uint8 | [**initFrame**](#function-initframe) (void) <br> |
|  uint8 | [**initScan**](#function-initscan) (void) <br> |
|  uint8 | [**locateSOFMarker**](#function-locatesofmarker) (void) <br> |
|  uint8 | [**locateSOIMarker**](#function-locatesoimarker) (void) <br> |
|  uint8 | [**locateSOSMarker**](#function-locatesosmarker) (uint8 \* pFoundEOI) <br> |
|  uint8 | [**nextMarker**](#function-nextmarker) (void) <br> |
|  uint8 | [**processMarkers**](#function-processmarkers) (uint8 \* pMarker) <br> |
|  uint8 | [**processRestart**](#function-processrestart) (void) <br> |
|  uint8 | [**readDHTMarker**](#function-readdhtmarker) (void) <br> |
|  uint8 | [**readDQTMarker**](#function-readdqtmarker) (void) <br> |
|  uint8 | [**readDRIMarker**](#function-readdrimarker) (void) <br> |
|  uint8 | [**readSOFMarker**](#function-readsofmarker) (void) <br> |
|  uint8 | [**readSOSMarker**](#function-readsosmarker) (void) <br> |
|  uint8 | [**skipVariableMarker**](#function-skipvariablemarker) (void) <br> |
|  PJPG\_INLINE void | [**stuffChar**](#function-stuffchar) (uint8 i) <br> |
|  PJPG\_INLINE uint8 | [**subAndClamp**](#function-subandclamp) (uint8 a, int16 b) <br> |
|  void | [**transformBlock**](#function-transformblock) (uint8 mcuBlock) <br> |
|  void | [**transformBlockReduce**](#function-transformblockreduce) (uint8 mcuBlock) <br> |
|  void | [**upsampleCb**](#function-upsamplecb) (uint8 srcOfs, uint8 dstOfs) <br> |
|  void | [**upsampleCbH**](#function-upsamplecbh) (uint8 srcOfs, uint8 dstOfs) <br> |
|  void | [**upsampleCbV**](#function-upsamplecbv) (uint8 srcOfs, uint8 dstOfs) <br> |
|  void | [**upsampleCr**](#function-upsamplecr) (uint8 srcOfs, uint8 dstOfs) <br> |
|  void | [**upsampleCrH**](#function-upsamplecrh) (uint8 srcOfs, uint8 dstOfs) <br> |
|  void | [**upsampleCrV**](#function-upsamplecrv) (uint8 srcOfs, uint8 dstOfs) <br> |

























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**PJPG\_ARITH\_SHIFT\_RIGHT\_8\_L**](picojpeg_8c.md#define-pjpg_arith_shift_right_8_l) (x) `((x) &gt;&gt; 8)`<br> |
| define  | [**PJPG\_ARITH\_SHIFT\_RIGHT\_N\_16**](picojpeg_8c.md#define-pjpg_arith_shift_right_n_16) (x, n) `((x) &gt;&gt; (n))`<br> |
| define  | [**PJPG\_DCT\_SCALE**](picojpeg_8c.md#define-pjpg_dct_scale)  `(1U &lt;&lt; PJPG\_DCT\_SCALE\_BITS)`<br> |
| define  | [**PJPG\_DCT\_SCALE\_BITS**](picojpeg_8c.md#define-pjpg_dct_scale_bits)  `7`<br> |
| define  | [**PJPG\_DESCALE**](picojpeg_8c.md#define-pjpg_descale) (x) `PJPG\_ARITH\_SHIFT\_RIGHT\_N\_16(((x) + (1 &lt;&lt; (PJPG\_DCT\_SCALE\_BITS - 1))), PJPG\_DCT\_SCALE\_BITS)`<br> |
| define  | [**PJPG\_INLINE**](picojpeg_8c.md#define-pjpg_inline)  <br> |
| define  | [**PJPG\_MAXCOMPSINSCAN**](picojpeg_8c.md#define-pjpg_maxcompsinscan)  `3`<br> |
| define  | [**PJPG\_MAX\_HEIGHT**](picojpeg_8c.md#define-pjpg_max_height)  `16384`<br> |
| define  | [**PJPG\_MAX\_IN\_BUF\_SIZE**](picojpeg_8c.md#define-pjpg_max_in_buf_size)  `256`<br> |
| define  | [**PJPG\_MAX\_WIDTH**](picojpeg_8c.md#define-pjpg_max_width)  `16384`<br> |
| define  | [**PJPG\_RIGHT\_SHIFT\_IS\_ALWAYS\_UNSIGNED**](picojpeg_8c.md#define-pjpg_right_shift_is_always_unsigned)  `0`<br> |
| define  | [**PJPG\_WFIX**](picojpeg_8c.md#define-pjpg_wfix) (x) `((x)\*PJPG\_DCT\_SCALE + 0.5f)`<br> |
| define  | [**PJPG\_WINOGRAD\_QUANT\_SCALE\_BITS**](picojpeg_8c.md#define-pjpg_winograd_quant_scale_bits)  `10`<br> |

## Public Types Documentation




### typedef HuffTable 

```C++
typedef struct HuffTableT HuffTable;
```




<hr>



### enum JPEG\_MARKER 

```C++
enum JPEG_MARKER {
    M_SOF0 = 0xC0,
    M_SOF1 = 0xC1,
    M_SOF2 = 0xC2,
    M_SOF3 = 0xC3,
    M_SOF5 = 0xC5,
    M_SOF6 = 0xC6,
    M_SOF7 = 0xC7,
    M_JPG = 0xC8,
    M_SOF9 = 0xC9,
    M_SOF10 = 0xCA,
    M_SOF11 = 0xCB,
    M_SOF13 = 0xCD,
    M_SOF14 = 0xCE,
    M_SOF15 = 0xCF,
    M_DHT = 0xC4,
    M_DAC = 0xCC,
    M_RST0 = 0xD0,
    M_RST1 = 0xD1,
    M_RST2 = 0xD2,
    M_RST3 = 0xD3,
    M_RST4 = 0xD4,
    M_RST5 = 0xD5,
    M_RST6 = 0xD6,
    M_RST7 = 0xD7,
    M_SOI = 0xD8,
    M_EOI = 0xD9,
    M_SOS = 0xDA,
    M_DQT = 0xDB,
    M_DNL = 0xDC,
    M_DRI = 0xDD,
    M_DHP = 0xDE,
    M_EXP = 0xDF,
    M_APP0 = 0xE0,
    M_APP15 = 0xEF,
    M_JPG0 = 0xF0,
    M_JPG13 = 0xFD,
    M_COM = 0xFE,
    M_TEM = 0x01,
    M_ERROR = 0x100,
    RST0 = 0xD0
};
```




<hr>



### typedef int16 

```C++
typedef signed short int16;
```




<hr>



### typedef int8 

```C++
typedef signed char int8;
```




<hr>



### typedef uint16 

```C++
typedef unsigned short uint16;
```




<hr>



### typedef uint8 

```C++
typedef unsigned char uint8;
```




<hr>
## Public Attributes Documentation




### variable gWinogradQuant 

```C++
const uint8 gWinogradQuant[];
```




<hr>
## Public Static Attributes Documentation




### variable ZAG 

```C++
const int8 ZAG[];
```




<hr>



### variable gBitBuf 

```C++
uint16 gBitBuf;
```




<hr>



### variable gBitsLeft 

```C++
uint8 gBitsLeft;
```




<hr>



### variable gCallbackStatus 

```C++
uint8 gCallbackStatus;
```




<hr>



### variable gCoeffBuf 

```C++
int16 gCoeffBuf[8 *8];
```




<hr>



### variable gCompACTab 

```C++
uint8 gCompACTab[3];
```




<hr>



### variable gCompDCTab 

```C++
uint8 gCompDCTab[3];
```




<hr>



### variable gCompHSamp 

```C++
uint8 gCompHSamp[3];
```




<hr>



### variable gCompIdent 

```C++
uint8 gCompIdent[3];
```




<hr>



### variable gCompList 

```C++
uint8 gCompList[3];
```




<hr>



### variable gCompQuant 

```C++
uint8 gCompQuant[3];
```




<hr>



### variable gCompVSamp 

```C++
uint8 gCompVSamp[3];
```




<hr>



### variable gCompsInFrame 

```C++
uint8 gCompsInFrame;
```




<hr>



### variable gCompsInScan 

```C++
uint8 gCompsInScan;
```




<hr>



### variable gHuffTab0 

```C++
HuffTable gHuffTab0;
```




<hr>



### variable gHuffTab1 

```C++
HuffTable gHuffTab1;
```




<hr>



### variable gHuffTab2 

```C++
HuffTable gHuffTab2;
```




<hr>



### variable gHuffTab3 

```C++
HuffTable gHuffTab3;
```




<hr>



### variable gHuffVal0 

```C++
uint8 gHuffVal0[16];
```




<hr>



### variable gHuffVal1 

```C++
uint8 gHuffVal1[16];
```




<hr>



### variable gHuffVal2 

```C++
uint8 gHuffVal2[256];
```




<hr>



### variable gHuffVal3 

```C++
uint8 gHuffVal3[256];
```




<hr>



### variable gImageXSize 

```C++
uint16 gImageXSize;
```




<hr>



### variable gImageYSize 

```C++
uint16 gImageYSize;
```




<hr>



### variable gInBuf 

```C++
uint8 gInBuf[PJPG_MAX_IN_BUF_SIZE];
```




<hr>



### variable gInBufLeft 

```C++
uint8 gInBufLeft;
```




<hr>



### variable gInBufOfs 

```C++
uint8 gInBufOfs;
```




<hr>



### variable gLastDC 

```C++
int16 gLastDC[3];
```




<hr>



### variable gMCUBufB 

```C++
uint8 gMCUBufB[256];
```




<hr>



### variable gMCUBufG 

```C++
uint8 gMCUBufG[256];
```




<hr>



### variable gMCUBufR 

```C++
uint8 gMCUBufR[256];
```




<hr>



### variable gMCUOrg 

```C++
uint8 gMCUOrg[6];
```




<hr>



### variable gMaxBlocksPerMCU 

```C++
uint8 gMaxBlocksPerMCU;
```




<hr>



### variable gMaxMCUSPerCol 

```C++
uint16 gMaxMCUSPerCol;
```




<hr>



### variable gMaxMCUSPerRow 

```C++
uint16 gMaxMCUSPerRow;
```




<hr>



### variable gMaxMCUXSize 

```C++
uint8 gMaxMCUXSize;
```




<hr>



### variable gMaxMCUYSize 

```C++
uint8 gMaxMCUYSize;
```




<hr>



### variable gNextRestartNum 

```C++
uint16 gNextRestartNum;
```




<hr>



### variable gNumMCUSRemainingX 

```C++
uint16 gNumMCUSRemainingX;
```




<hr>



### variable gNumMCUSRemainingY 

```C++
uint16 gNumMCUSRemainingY;
```




<hr>



### variable gQuant0 

```C++
int16 gQuant0[8 *8];
```




<hr>



### variable gQuant1 

```C++
int16 gQuant1[8 *8];
```




<hr>



### variable gReduce 

```C++
uint8 gReduce;
```




<hr>



### variable gRestartInterval 

```C++
uint16 gRestartInterval;
```




<hr>



### variable gRestartsLeft 

```C++
uint16 gRestartsLeft;
```




<hr>



### variable gScanType 

```C++
pjpeg_scan_type_t gScanType;
```




<hr>



### variable gTemFlag 

```C++
uint8 gTemFlag;
```




<hr>



### variable gValidHuffTables 

```C++
uint8 gValidHuffTables;
```




<hr>



### variable gValidQuantTables 

```C++
uint8 gValidQuantTables;
```




<hr>



### variable g\_pCallback\_data 

```C++
void* g_pCallback_data;
```




<hr>



### variable g\_pNeedBytesCallback 

```C++
pjpeg_need_bytes_callback_t g_pNeedBytesCallback;
```




<hr>
## Public Functions Documentation




### function pjpeg\_decode\_init 

```C++
unsigned char pjpeg_decode_init (
    pjpeg_image_info_t * pInfo,
    pjpeg_need_bytes_callback_t pNeed_bytes_callback,
    void * pCallback_data,
    unsigned char reduce
) 
```




<hr>



### function pjpeg\_decode\_mcu 

```C++
unsigned char pjpeg_decode_mcu (
    void
) 
```




<hr>
## Public Static Functions Documentation




### function addAndClamp 

```C++
static PJPG_INLINE uint8 addAndClamp (
    uint8 a,
    int16 b
) 
```




<hr>



### function checkHuffTables 

```C++
static uint8 checkHuffTables (
    void
) 
```




<hr>



### function checkQuantTables 

```C++
static uint8 checkQuantTables (
    void
) 
```




<hr>



### function clamp 

```C++
static PJPG_INLINE uint8 clamp (
    int16 s
) 
```




<hr>



### function convertCb 

```C++
static void convertCb (
    uint8 dstOfs
) 
```




<hr>



### function convertCr 

```C++
static void convertCr (
    uint8 dstOfs
) 
```




<hr>



### function copyY 

```C++
static void copyY (
    uint8 dstOfs
) 
```




<hr>



### function createWinogradQuant 

```C++
static void createWinogradQuant (
    int16 * pQuant
) 
```




<hr>



### function decodeNextMCU 

```C++
static uint8 decodeNextMCU (
    void
) 
```




<hr>



### function fillInBuf 

```C++
static void fillInBuf (
    void
) 
```




<hr>



### function findEOI 

```C++
static uint8 findEOI (
    void
) 
```




<hr>



### function fixInBuffer 

```C++
static void fixInBuffer (
    void
) 
```




<hr>



### function getBit 

```C++
static PJPG_INLINE uint8 getBit (
    void
) 
```




<hr>



### function getBits 

```C++
static uint16 getBits (
    uint8 numBits,
    uint8 FFCheck
) 
```




<hr>



### function getBits1 

```C++
static PJPG_INLINE uint16 getBits1 (
    uint8 numBits
) 
```




<hr>



### function getBits2 

```C++
static PJPG_INLINE uint16 getBits2 (
    uint8 numBits
) 
```




<hr>



### function getChar 

```C++
static PJPG_INLINE uint8 getChar (
    void
) 
```




<hr>



### function getExtendOffset 

```C++
static int16 getExtendOffset (
    uint8 i
) 
```




<hr>



### function getExtendTest 

```C++
static uint16 getExtendTest (
    uint8 i
) 
```




<hr>



### function getHuffTable 

```C++
static HuffTable * getHuffTable (
    uint8 index
) 
```




<hr>



### function getHuffVal 

```C++
static uint8 * getHuffVal (
    uint8 index
) 
```




<hr>



### function getMaxHuffCodes 

```C++
static uint16 getMaxHuffCodes (
    uint8 index
) 
```




<hr>



### function getOctet 

```C++
static PJPG_INLINE uint8 getOctet (
    uint8 FFCheck
) 
```




<hr>



### function huffCreate 

```C++
static void huffCreate (
    const uint8 * pBits,
    HuffTable * pHuffTable
) 
```




<hr>



### function huffDecode 

```C++
static PJPG_INLINE uint8 huffDecode (
    const HuffTable * pHuffTable,
    const uint8 * pHuffVal
) 
```




<hr>



### function huffExtend 

```C++
static PJPG_INLINE int16 huffExtend (
    uint16 x,
    uint8 s
) 
```




<hr>



### function idctCols 

```C++
static void idctCols (
    void
) 
```




<hr>



### function idctRows 

```C++
static void idctRows (
    void
) 
```




<hr>



### function imul\_b1\_b3 

```C++
static PJPG_INLINE int16 imul_b1_b3 (
    int16 w
) 
```




<hr>



### function imul\_b2 

```C++
static PJPG_INLINE int16 imul_b2 (
    int16 w
) 
```




<hr>



### function imul\_b4 

```C++
static PJPG_INLINE int16 imul_b4 (
    int16 w
) 
```




<hr>



### function imul\_b5 

```C++
static PJPG_INLINE int16 imul_b5 (
    int16 w
) 
```




<hr>



### function init 

```C++
static uint8 init (
    void
) 
```




<hr>



### function initFrame 

```C++
static uint8 initFrame (
    void
) 
```




<hr>



### function initScan 

```C++
static uint8 initScan (
    void
) 
```




<hr>



### function locateSOFMarker 

```C++
static uint8 locateSOFMarker (
    void
) 
```




<hr>



### function locateSOIMarker 

```C++
static uint8 locateSOIMarker (
    void
) 
```




<hr>



### function locateSOSMarker 

```C++
static uint8 locateSOSMarker (
    uint8 * pFoundEOI
) 
```




<hr>



### function nextMarker 

```C++
static uint8 nextMarker (
    void
) 
```




<hr>



### function processMarkers 

```C++
static uint8 processMarkers (
    uint8 * pMarker
) 
```




<hr>



### function processRestart 

```C++
static uint8 processRestart (
    void
) 
```




<hr>



### function readDHTMarker 

```C++
static uint8 readDHTMarker (
    void
) 
```




<hr>



### function readDQTMarker 

```C++
static uint8 readDQTMarker (
    void
) 
```




<hr>



### function readDRIMarker 

```C++
static uint8 readDRIMarker (
    void
) 
```




<hr>



### function readSOFMarker 

```C++
static uint8 readSOFMarker (
    void
) 
```




<hr>



### function readSOSMarker 

```C++
static uint8 readSOSMarker (
    void
) 
```




<hr>



### function skipVariableMarker 

```C++
static uint8 skipVariableMarker (
    void
) 
```




<hr>



### function stuffChar 

```C++
static PJPG_INLINE void stuffChar (
    uint8 i
) 
```




<hr>



### function subAndClamp 

```C++
static PJPG_INLINE uint8 subAndClamp (
    uint8 a,
    int16 b
) 
```




<hr>



### function transformBlock 

```C++
static void transformBlock (
    uint8 mcuBlock
) 
```




<hr>



### function transformBlockReduce 

```C++
static void transformBlockReduce (
    uint8 mcuBlock
) 
```




<hr>



### function upsampleCb 

```C++
static void upsampleCb (
    uint8 srcOfs,
    uint8 dstOfs
) 
```




<hr>



### function upsampleCbH 

```C++
static void upsampleCbH (
    uint8 srcOfs,
    uint8 dstOfs
) 
```




<hr>



### function upsampleCbV 

```C++
static void upsampleCbV (
    uint8 srcOfs,
    uint8 dstOfs
) 
```




<hr>



### function upsampleCr 

```C++
static void upsampleCr (
    uint8 srcOfs,
    uint8 dstOfs
) 
```




<hr>



### function upsampleCrH 

```C++
static void upsampleCrH (
    uint8 srcOfs,
    uint8 dstOfs
) 
```




<hr>



### function upsampleCrV 

```C++
static void upsampleCrV (
    uint8 srcOfs,
    uint8 dstOfs
) 
```




<hr>
## Macro Definition Documentation





### define PJPG\_ARITH\_SHIFT\_RIGHT\_8\_L 

```C++
#define PJPG_ARITH_SHIFT_RIGHT_8_L (
    x
) `((x) >> 8)`
```




<hr>



### define PJPG\_ARITH\_SHIFT\_RIGHT\_N\_16 

```C++
#define PJPG_ARITH_SHIFT_RIGHT_N_16 (
    x,
    n
) `((x) >> (n))`
```




<hr>



### define PJPG\_DCT\_SCALE 

```C++
#define PJPG_DCT_SCALE `(1U << PJPG_DCT_SCALE_BITS)`
```




<hr>



### define PJPG\_DCT\_SCALE\_BITS 

```C++
#define PJPG_DCT_SCALE_BITS `7`
```




<hr>



### define PJPG\_DESCALE 

```C++
#define PJPG_DESCALE (
    x
) `PJPG_ARITH_SHIFT_RIGHT_N_16(((x) + (1 << (PJPG_DCT_SCALE_BITS - 1))), PJPG_DCT_SCALE_BITS)`
```




<hr>



### define PJPG\_INLINE 

```C++
#define PJPG_INLINE 
```




<hr>



### define PJPG\_MAXCOMPSINSCAN 

```C++
#define PJPG_MAXCOMPSINSCAN `3`
```




<hr>



### define PJPG\_MAX\_HEIGHT 

```C++
#define PJPG_MAX_HEIGHT `16384`
```




<hr>



### define PJPG\_MAX\_IN\_BUF\_SIZE 

```C++
#define PJPG_MAX_IN_BUF_SIZE `256`
```




<hr>



### define PJPG\_MAX\_WIDTH 

```C++
#define PJPG_MAX_WIDTH `16384`
```




<hr>



### define PJPG\_RIGHT\_SHIFT\_IS\_ALWAYS\_UNSIGNED 

```C++
#define PJPG_RIGHT_SHIFT_IS_ALWAYS_UNSIGNED `0`
```




<hr>



### define PJPG\_WFIX 

```C++
#define PJPG_WFIX (
    x
) `((x)*PJPG_DCT_SCALE + 0.5f)`
```




<hr>



### define PJPG\_WINOGRAD\_QUANT\_SCALE\_BITS 

```C++
#define PJPG_WINOGRAD_QUANT_SCALE_BITS `10`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/src/jpeg-decoder/picojpeg.c`

