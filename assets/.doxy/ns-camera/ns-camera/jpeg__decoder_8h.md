

# File jpeg\_decoder.h



[**FileList**](files.md) **>** [**jpeg-decoder**](dir_09e5e1a913ce3e01b48c441fa9bab0f2.md) **>** [**jpeg\_decoder.h**](jpeg__decoder_8h.md)

[Go to the source code of this file](jpeg__decoder_8h_source.md)



* `#include "picojpeg.h"`
* `#include <stdint.h>`
* `#include <stdio.h>`
* `#include <string.h>`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) <br> |






















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**jpeg\_decoder\_abort**](#function-jpeg_decoder_abort) ([**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) \* ctx) <br> |
|  int | [**jpeg\_decoder\_init**](#function-jpeg_decoder_init) ([**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) \* ctx, const uint8\_t \* array, uint32\_t array\_size) <br> |
|  int | [**jpeg\_decoder\_read**](#function-jpeg_decoder_read) ([**jpeg\_decoder\_context\_t**](structjpeg__decoder__context__t.md) \* ctx) <br> |
|  unsigned char | [**pjpeg\_callback**](#function-pjpeg_callback) (unsigned char \* pBuf, unsigned char buf\_size, unsigned char \* pBytes\_actually\_read, void \* pCallback\_data) <br> |




























## Public Functions Documentation




### function jpeg\_decoder\_abort 

```C++
void jpeg_decoder_abort (
    jpeg_decoder_context_t * ctx
) 
```




<hr>



### function jpeg\_decoder\_init 

```C++
int jpeg_decoder_init (
    jpeg_decoder_context_t * ctx,
    const uint8_t * array,
    uint32_t array_size
) 
```




<hr>



### function jpeg\_decoder\_read 

```C++
int jpeg_decoder_read (
    jpeg_decoder_context_t * ctx
) 
```




<hr>



### function pjpeg\_callback 

```C++
unsigned char pjpeg_callback (
    unsigned char * pBuf,
    unsigned char buf_size,
    unsigned char * pBytes_actually_read,
    void * pCallback_data
) 
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-camera/src/jpeg-decoder/jpeg_decoder.h`

