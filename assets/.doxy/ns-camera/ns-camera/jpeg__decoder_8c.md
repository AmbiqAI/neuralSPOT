

# File jpeg\_decoder.c



[**FileList**](files.md) **>** [**jpeg-decoder**](dir_09e5e1a913ce3e01b48c441fa9bab0f2.md) **>** [**jpeg\_decoder.c**](jpeg__decoder_8c.md)

[Go to the source code of this file](jpeg__decoder_8c_source.md)



* `#include "jpeg_decoder.h"`
* `#include "picojpeg.h"`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**jpeg\_decoder\_abort**](#function-jpeg_decoder_abort) ([**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) \* ctx) <br> |
|  int | [**jpeg\_decoder\_decode\_mcu**](#function-jpeg_decoder_decode_mcu) ([**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) \* ctx) <br> |
|  int | [**jpeg\_decoder\_init**](#function-jpeg_decoder_init) ([**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) \* ctx, const uint8\_t \* array, uint32\_t array\_size) <br> |
|  int | [**jpeg\_decoder\_read**](#function-jpeg_decoder_read) ([**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) \* ctx) <br> |
|  int | [**jpeg\_decoder\_read\_swapped\_bytes**](#function-jpeg_decoder_read_swapped_bytes) ([**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) \* ctx) <br> |
|  unsigned char | [**pjpeg\_callback**](#function-pjpeg_callback) (unsigned char \* pBuf, unsigned char buf\_size, unsigned char \* pBytes\_actually\_read, void \* pCallback\_data) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**jpg\_min**](jpeg__decoder_8c.md#define-jpg_min) (a, b) (((a) &lt; (b)) ? (a) : (b))<br> |
| define  | [**min**](jpeg__decoder_8c.md#define-min) (a, b) (((a) &lt; (b)) ? (a) : (b))<br> |

## Public Functions Documentation




### function jpeg\_decoder\_abort 

```C++
void jpeg_decoder_abort (
    jpeg_decoder_context_t * ctx
) 
```






### function jpeg\_decoder\_decode\_mcu 

```C++
int jpeg_decoder_decode_mcu (
    jpeg_decoder_context_t * ctx
) 
```






### function jpeg\_decoder\_init 

```C++
int jpeg_decoder_init (
    jpeg_decoder_context_t * ctx,
    const uint8_t * array,
    uint32_t array_size
) 
```






### function jpeg\_decoder\_read 

```C++
int jpeg_decoder_read (
    jpeg_decoder_context_t * ctx
) 
```






### function jpeg\_decoder\_read\_swapped\_bytes 

```C++
int jpeg_decoder_read_swapped_bytes (
    jpeg_decoder_context_t * ctx
) 
```






### function pjpeg\_callback 

```C++
unsigned char pjpeg_callback (
    unsigned char * pBuf,
    unsigned char buf_size,
    unsigned char * pBytes_actually_read,
    void * pCallback_data
) 
```



## Macro Definition Documentation





### define jpg\_min 

```C++
#define jpg_min (
    a,
    b
) (((a) < (b)) ? (a) : (b))
```






### define min 

```C++
#define min (
    a,
    b
) (((a) < (b)) ? (a) : (b))
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/src/jpeg-decoder/jpeg_decoder.c`

