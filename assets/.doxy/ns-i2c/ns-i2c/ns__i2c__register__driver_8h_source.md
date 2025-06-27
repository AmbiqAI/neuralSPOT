

# File ns\_i2c\_register\_driver.h

[**File List**](files.md) **>** [**includes-api**](dir_ead24bed0d4d5fb1b23b4266f13c1ec5.md) **>** [**ns\_i2c\_register\_driver.h**](ns__i2c__register__driver_8h.md)

[Go to the documentation of this file](ns__i2c__register__driver_8h.md)


```C++

#ifndef NS_I2C_REGISTER_DRIVER
#define NS_I2C_REGISTER_DRIVER

#ifdef __cplusplus
extern "C" {
#endif

#include "ns_i2c.h"

uint32_t ns_i2c_read_reg(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t regAddr, uint8_t *value, uint8_t mask);

uint32_t ns_i2c_write_reg(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t regAddr, uint8_t value, uint8_t mask);

uint32_t ns_i2c_read_sequential_regs(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint32_t regAddr, void *buf, uint32_t size);

uint32_t ns_i2c_write_sequential_regs(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint32_t regAddr, void *buf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif // NS_I2C_REGISTER_DRIVER
```


