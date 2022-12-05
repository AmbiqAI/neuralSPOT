#ifndef NS_I2C
#define NS_I2C

/**
 * @file ns_i2c.h
 * @author Adam Page
 * @brief Generic I2C driver
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) 2022
 *  \addtogroup NeuralSPOT-i2c
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

typedef enum { NS_I2C_STATUS_SUCCESS = 0, NS_I2C_STATUS_ERROR = 1 } ns_i2c_status_e;

// I2C Driver Configration
typedef struct {
    int8_t iom; // Apollo4 IOM port
    // Internal state
    void *iomHandle;             // AmbiqSuite IOM handle
    am_hal_iom_config_t sIomCfg; //  AmbiqSuite IOM config
} ns_i2c_config_t;

// I2C Transfer Flags
typedef enum { NS_I2C_XFER_WR = 0, NS_I2C_XFER_RD = (1u << 0) } ns_i2c_xfer_flag_e;

// I2C Transfer Message
typedef struct {
    uint16_t addr;
    uint16_t flags;
    uint16_t len;
    uint8_t *buf;
} ns_i2c_msg_t;

/**
 * @brief Initialize I2C on one of the IOM (IO managers)
 * @param cfg I2C configuration
 * @param speed I2C speed in Hz
 * @return uint32_t status
 */
uint32_t
ns_i2c_interface_init(ns_i2c_config_t *cfg, uint32_t speed);

/**
 * @brief Perform low-level I2C read using IOM transfer
 * @param cfg I2C configuration
 * @param buf Buffer to store read bytes
 * @param size Number of bytes to read
 * @param addr I2C device address
 */
uint32_t
ns_i2c_read(ns_i2c_config_t *cfg, const void *buf, uint32_t size, uint16_t addr);

/**
 * @brief Perform low-level I2C write using IOM transfer
 * @param cfg I2C configuration
 * @param buf Buffer of bytes to write
 * @param size Number of bytes to write
 * @param addr I2C device address
 */
uint32_t
ns_i2c_write(ns_i2c_config_t *cfg, const void *buf, uint32_t size, uint16_t addr);

/**
 * @brief Perform low-level I2C write followed by immediate read
 * @param cfg I2C configuration
 * @param writeBuf Write buffer
 * @param numWrite Number of bytes to write
 * @param readBuf Read buffer
 * @param numRead Number of bytes to read
 * @param addr I2C device address
 */
uint32_t
ns_i2c_write_read(ns_i2c_config_t *cfg, uint16_t addr, const void *writeBuf, size_t numWrite,
                  void *readBuf, size_t numRead);

/**
 * @brief Perform sequence of low-level I2C transfers (similar to Linux)
 * @param cfg I2C configuration
 * @param msgs I2C messages to transfer
 * @param numMsgs Number of I2C messsages
 */
uint32_t
ns_i2c_transfer(ns_i2c_config_t *cfg, ns_i2c_msg_t *msgs, size_t numMsgs);

#ifdef __cplusplus
}
#endif

#endif // NS_IO_I2C
