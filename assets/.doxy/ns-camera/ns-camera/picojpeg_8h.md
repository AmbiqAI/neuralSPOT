

# File picojpeg.h



[**FileList**](files.md) **>** [**jpeg-decoder**](dir_09e5e1a913ce3e01b48c441fa9bab0f2.md) **>** [**picojpeg.h**](picojpeg_8h.md)

[Go to the source code of this file](picojpeg_8h_source.md)


















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**pjpeg\_image\_info\_t**](structpjpeg__image__info__t.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**@1**](#enum-@1)  <br> |
| typedef unsigned char(\* | [**pjpeg\_need\_bytes\_callback\_t**](#typedef-pjpeg_need_bytes_callback_t)  <br> |
| enum  | [**pjpeg\_scan\_type\_t**](#enum-pjpeg_scan_type_t)  <br> |




















## Public Functions

| Type | Name |
| ---: | :--- |
|  unsigned char | [**pjpeg\_decode\_init**](#function-pjpeg_decode_init) ([**pjpeg\_image\_info\_t**](structpjpeg__image__info__t.md) \* pInfo, pjpeg\_need\_bytes\_callback\_t pNeed\_bytes\_callback, void \* pCallback\_data, unsigned char reduce) <br> |
|  unsigned char | [**pjpeg\_decode\_mcu**](#function-pjpeg_decode_mcu) (void) <br> |




























## Public Types Documentation




### enum @1 

```C++
enum @1 {
    PJPG_NO_MORE_BLOCKS = 1,
    PJPG_BAD_DHT_COUNTS,
    PJPG_BAD_DHT_INDEX,
    PJPG_BAD_DHT_MARKER,
    PJPG_BAD_DQT_MARKER,
    PJPG_BAD_DQT_TABLE,
    PJPG_BAD_PRECISION,
    PJPG_BAD_HEIGHT,
    PJPG_BAD_WIDTH,
    PJPG_TOO_MANY_COMPONENTS,
    PJPG_BAD_SOF_LENGTH,
    PJPG_BAD_VARIABLE_MARKER,
    PJPG_BAD_DRI_LENGTH,
    PJPG_BAD_SOS_LENGTH,
    PJPG_BAD_SOS_COMP_ID,
    PJPG_W_EXTRA_BYTES_BEFORE_MARKER,
    PJPG_NO_ARITHMITIC_SUPPORT,
    PJPG_UNEXPECTED_MARKER,
    PJPG_NOT_JPEG,
    PJPG_UNSUPPORTED_MARKER,
    PJPG_BAD_DQT_LENGTH,
    PJPG_TOO_MANY_BLOCKS,
    PJPG_UNDEFINED_QUANT_TABLE,
    PJPG_UNDEFINED_HUFF_TABLE,
    PJPG_NOT_SINGLE_SCAN,
    PJPG_UNSUPPORTED_COLORSPACE,
    PJPG_UNSUPPORTED_SAMP_FACTORS,
    PJPG_DECODE_ERROR,
    PJPG_BAD_RESTART_MARKER,
    PJPG_ASSERTION_ERROR,
    PJPG_BAD_SOS_SPECTRAL,
    PJPG_BAD_SOS_SUCCESSIVE,
    PJPG_STREAM_READ_ERROR,
    PJPG_NOTENOUGHMEM,
    PJPG_UNSUPPORTED_COMP_IDENT,
    PJPG_UNSUPPORTED_QUANT_TABLE,
    PJPG_UNSUPPORTED_MODE
};
```




<hr>



### typedef pjpeg\_need\_bytes\_callback\_t 

```C++
typedef unsigned char(* pjpeg_need_bytes_callback_t) (unsigned char *pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
```




<hr>



### enum pjpeg\_scan\_type\_t 

```C++
enum pjpeg_scan_type_t {
    PJPG_GRAYSCALE,
    PJPG_YH1V1,
    PJPG_YH2V1,
    PJPG_YH1V2,
    PJPG_YH2V2
};
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

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/src/jpeg-decoder/picojpeg.h`

