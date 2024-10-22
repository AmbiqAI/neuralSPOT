

# File ns\_spi.h

[**File List**](files.md) **>** [**includes-api**](dir_afaa7171ab94fb931f028cea1f13c04e.md) **>** [**ns\_spi.h**](ns__spi_8h.md)

[Go to the documentation of this file](ns__spi_8h.md)

```C++

#ifndef NS_SPI
#define NS_SPI
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

uint32_t ns_spi_interface_init(ns_spi_config_t *cfg, uint32_t speed, am_hal_iom_spi_mode_e mode);

uint32_t ns_spi_read(
    ns_spi_config_t *cfg, const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen,
    uint32_t csPin);

uint32_t ns_spi_write(
    ns_spi_config_t *cfg, const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen,
    uint32_t csPin);

uint32_t ns_spi_transfer(
    ns_spi_config_t *cfg, const void *txBuf, const void *rxBuf, uint32_t size, uint32_t csPin);

uint32_t ns_spi_read_dma(
    ns_spi_config_t *cfg, const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen,
    uint32_t csPin);

#ifdef __cplusplus
}
#endif

#endif // NS_SPI

```

