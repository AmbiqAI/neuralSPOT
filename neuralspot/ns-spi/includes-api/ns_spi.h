#ifndef NS_SPI
#define NS_SPI
/**
 * @file ns_spi.h
 * @author Adam Page
 * @brief Generic SPI driver
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) 2022
 *  \addtogroup NeuralSPOT-spi
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

typedef enum { NS_SPI_STATUS_SUCCESS = 0, NS_SPI_STATUS_ERROR = 1 } ns_spi_status_e;
struct ns_spi_cfg;
typedef void (*ns_spi_cb)(struct ns_spi_cfg *);

// SPI Driver Configuration
typedef struct ns_spi_cfg {
    int8_t iom; // Apollo4 IOM port
    // Internal state
    void *iomHandle;             // AmbiqSuite IOM handle
    am_hal_iom_config_t sIomCfg; //  AmbiqSuite IOM config
    ns_spi_cb cb;                // Callback
} ns_spi_config_t;

/**
 * @brief Initialize the SPI interface
 *
 * @param cfg SPI Configuration
 * @param speed Bus speed in Hz
 * @param mode SPI mode (CPOL, CPHA)
 * @return uint32_t
 */
uint32_t ns_spi_interface_init(ns_spi_config_t *cfg, uint32_t speed, am_hal_iom_spi_mode_e mode);

/**
 * @brief Read from a SPI device. The SPI device is selected by the csPin parameter.
 *
 * @param cfg SPI Configuration
 * @param buf Buffer to read into
 * @param bufLen Buffer length
 * @param reg Register to read from
 * @param regLen Register length
 * @param csPin CS pin number
 * @return uint32_t
 */
uint32_t ns_spi_read(
    ns_spi_config_t *cfg, const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen,
    uint32_t csPin);

/**
 * @brief Write to a SPI device. The SPI device is selected by the csPin parameter.
 *
 * @param cfg SPI Configuration
 * @param buf Buffer to write from
 * @param bufLen Buffer length
 * @param reg Register to write to
 * @param regLen Register length
 * @param csPin CS pin number
 * @return uint32_t
 */
uint32_t ns_spi_write(
    ns_spi_config_t *cfg, const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen,
    uint32_t csPin);

/**
 * @brief Transfer data to/from a SPI device. The SPI device is selected by the csPin parameter.
 *
 * @param cfg SPI Configuration
 * @param txBuf Transmit buffer
 * @param rxBuf Receive buffer
 * @param size Size of the transfer
 * @param csPin CS pin number
 * @return uint32_t
 */
uint32_t ns_spi_transfer(
    ns_spi_config_t *cfg, const void *txBuf, const void *rxBuf, uint32_t size, uint32_t csPin);

/**
 * @brief Issure DMA read, the cfg->callback will be called when the transfer is complete
 *
 * @param cfg
 * @param buf
 * @param bufLen
 * @param reg
 * @param regLen
 * @param csPin
 * @return uint32_t
 */
uint32_t ns_spi_read_dma(
    ns_spi_config_t *cfg, const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen,
    uint32_t csPin);

#ifdef __cplusplus
}
#endif

#endif // NS_SPI
