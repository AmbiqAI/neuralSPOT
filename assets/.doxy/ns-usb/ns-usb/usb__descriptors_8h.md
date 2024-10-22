

# File usb\_descriptors.h



[**FileList**](files.md) **>** [**includes-api**](dir_a2a61ebab6bf7d344fb1c36a75cc33e9.md) **>** [**usb\_descriptors.h**](usb__descriptors_8h.md)

[Go to the source code of this file](usb__descriptors_8h_source.md)




















## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**@0**](#enum-@0)  <br> |
| enum  | [**@1**](#enum-@1)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint8\_t const | [**desc\_ms\_os\_20**](#variable-desc_ms_os_20)  <br> |
|  char const  \* | [**usb\_string\_desc\_arr**](#variable-usb_string_desc_arr)  <br> |












































## Public Types Documentation




### enum @0 

```C++
enum @0 {
    VENDOR_REQUEST_WEBUSB = 1,
    VENDOR_REQUEST_MICROSOFT = 2
};
```






### enum @1 

```C++
enum @1 {
    USB_DESCRIPTOR_MANUFACTURER = 0x01,
    USB_DESCRIPTOR_PRODUCT = 0x02,
    USB_DESCRIPTOR_SERIAL = 0x03
};
```



## Public Attributes Documentation




### variable desc\_ms\_os\_20 

```C++
uint8_t const desc_ms_os_20[];
```






### variable usb\_string\_desc\_arr 

```C++
char const* usb_string_desc_arr[6];
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-usb/includes-api/usb_descriptors.h`

