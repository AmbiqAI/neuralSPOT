

# File ns\_i2c\_register\_driver.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-i2c**](dir_cc1f492d5d9f73ec0b0ac0581cc456e0.md) **>** [**src**](dir_d6c56226b0c7cd8d6baff594b5865597.md) **>** [**ns\_i2c\_register\_driver.c**](ns__i2c__register__driver_8c.md)

[Go to the documentation of this file](ns__i2c__register__driver_8c.md)


```C++

#include "ns_i2c_register_driver.h"
#include "am_hal_iom.h"
#include "ns_i2c.h"

uint32_t ns_i2c_read_sequential_regs(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint32_t regAddr, void *buf, uint32_t size) {
    am_hal_iom_transfer_t Transaction;
    Transaction.ui8Priority = 1;
    Transaction.ui32InstrLen = 1;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L) || defined(AM_PART_APOLLO5A) || defined(AM_PART_APOLLO5B)
    Transaction.ui64Instr = regAddr;
#else
    Transaction.ui32Instr = (uint32_t)regAddr;
#endif
    Transaction.eDirection = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes = size;
    Transaction.pui32RxBuffer = (uint32_t *)buf;
    Transaction.bContinue = false;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.uPeerInfo.ui32I2CDevAddr = devAddr;
    if (am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction)) {
        return NS_I2C_STATUS_ERROR;
    }
    return NS_I2C_STATUS_SUCCESS;
}

uint32_t ns_i2c_write_sequential_regs(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint32_t regAddr, void *buf, uint32_t size) {
    am_hal_iom_transfer_t Transaction;
    Transaction.ui8Priority = 1;
    Transaction.ui32InstrLen = 1;
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L) || defined(AM_PART_APOLLO5A) || defined(AM_PART_APOLLO5B)
    Transaction.ui64Instr = regAddr;
#else
    Transaction.ui32Instr = (uint32_t)regAddr;
#endif
    Transaction.eDirection = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes = size;
    Transaction.pui32TxBuffer = (uint32_t *)buf;
    Transaction.bContinue = false;
    Transaction.ui8RepeatCount = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;
    Transaction.uPeerInfo.ui32I2CDevAddr = devAddr;
    if (am_hal_iom_blocking_transfer(cfg->iomHandle, &Transaction) != AM_HAL_STATUS_SUCCESS) {
        return NS_I2C_STATUS_ERROR;
    }
    return NS_I2C_STATUS_SUCCESS;
}

uint32_t ns_i2c_read_reg(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t regAddr, uint8_t *value, uint8_t mask) {
    uint32_t regValue;
    if (ns_i2c_read_sequential_regs(cfg, devAddr, regAddr, &regValue, 1)) {
        return NS_I2C_STATUS_ERROR;
    }
    if (mask != 0xFF) {
        regValue = regValue & mask;
    }
    (*value) = (uint8_t)regValue;
    return NS_I2C_STATUS_SUCCESS;
}

uint32_t ns_i2c_write_reg(
    ns_i2c_config_t *cfg, uint32_t devAddr, uint8_t regAddr, uint8_t value, uint8_t mask) {
    uint32_t regValue = value;
    if (mask != 0xFF) {
        if (ns_i2c_read_sequential_regs(cfg, devAddr, regAddr, &regValue, 1)) {
            return NS_I2C_STATUS_ERROR;
        }
        regValue = (regValue & ~mask) | (value & mask);
    }
    if (ns_i2c_write_sequential_regs(cfg, devAddr, regAddr, &regValue, 1)) {
        return NS_I2C_STATUS_ERROR;
    }
    return NS_I2C_STATUS_SUCCESS;
}
```


