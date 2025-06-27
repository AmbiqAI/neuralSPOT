# neuralSPOT SPI Driver Library

A lightweight, generic SPI driver for Ambiq Apollo MCUs, built on top of the AmbiqSuite IOM (I/O Master) interface. Provides blocking and non-blocking (DMA) transfer routines, and easy-to-use callback support.

---

## Table of Contents

1. [Features](#features)
3. [Quick Start](#quick-start)
4. [Configuration Options](#configuration-options)
5. [APIs](#apis)
6. [Code Examples](#code-examples)
   * [Basic Initialization and Transfer](#basic-initialization-and-transfer)
   * [DMA Read with Callback](#dma-read-with-callback)


---

## Features

* **Generic SPI Interface**: Support for any SPI peripheral on Ambiq Apollo microcontrollers.
* **Blocking Transfers**: Simple read, write, and full-duplex operations.
* **Non-blocking (DMA) Transfers**: Issue DMA read requests with completion callbacks.
* **Interrupt Service Routine**: Central ISR handler for IOM interrupts.
* **Callback Support**: Pass a user-defined callback for DMA completion or custom handling.

---

## Quick Start

1. **Include the API Header**:

   ```c
   #include "ns_spi.h"
   ```

2. **Declare and Configure** `ns_spi_config_t`:

   ```c
   ns_spi_config_t spi_cfg = {
       .iom     = 1,            // IOM port number (e.g., 0, 1, ...)
       .iomHandle = NULL,       // (set by init)
       .cb      = NULL          // No callback for blocking transfers
   };
   ```

3. **Initialize the Interface**:

   ```c
   uint32_t status = ns_spi_interface_init(
       &spi_cfg,
       AM_HAL_IOM_8MHZ,         // 8 MHz SPI clock
       AM_HAL_IOM_SPI_MODE_3    // CPOL=1, CPHA=1
   );
   if (status != NS_SPI_STATUS_SUCCESS) {
       // Handle error...
   }
   ```

4. **Perform Transfers**:

   ```c
   uint8_t tx_buf[2] = {0x9F, 0x00}; // Example command
   uint8_t rx_buf[2];

   // Read 2 bytes from register 0x9F
   ns_spi_read(
       &spi_cfg,
       rx_buf,
       sizeof(rx_buf),
       0x9F,                    // Register address
       1,                       // Register length in bytes
       AM_BSP_GPIO_IOM1_CS      // Chip-select pin
   );
   ```

---

## Configuration Options

| Field       | Description                                        |
| ----------- | -------------------------------------------------- |
| `iom`       | IOM port number (e.g., `0`, `1`)                   |
| `iomHandle` | Handle assigned by AmbiqSuite after initialization |
| `sIomCfg`   | Internal AmbiqSuite IOM configuration structure    |
| `cb`        | Callback invoked on DMA-read completion (if set)   |

---

## APIs

```c
// ISR Handler: call from your IOM IRQ handler
void ns_spi_handle_iom_isr(void);

// Initialize SPI interface
uint32_t ns_spi_interface_init(
    ns_spi_config_t *cfg,
    uint32_t speed,
    am_hal_iom_spi_mode_e mode
);

// Blocking Read from SPI device
uint32_t ns_spi_read(
    ns_spi_config_t *cfg,
    const void *buf,
    uint32_t bufLen,
    uint64_t reg,
    uint32_t regLen,
    uint32_t csPin
);

// Blocking Write to SPI device
uint32_t ns_spi_write(
    ns_spi_config_t *cfg,
    const void *buf,
    uint32_t bufLen,
    uint64_t reg,
    uint32_t regLen,
    uint32_t csPin
);

// Non-blocking (DMA) Read with callback
uint32_t ns_spi_read_dma(
    ns_spi_config_t *cfg,
    const void *buf,
    uint32_t bufLen,
    uint64_t reg,
    uint32_t regLen,
    uint32_t csPin
);
```

---

## Code Examples

### Basic Initialization and Transfer

```c
#include "ns_spi.h"

int main(void) {
    ns_spi_config_t spi_cfg = { .iom = 1, .cb = NULL };
    if (ns_spi_interface_init(&spi_cfg, AM_HAL_IOM_4MHZ, AM_HAL_IOM_SPI_MODE_0)
        != NS_SPI_STATUS_SUCCESS) {
        return -1; // init failed
    }

    uint8_t data[4];
    // Read 4 bytes starting at register 0x10
    ns_spi_read(&spi_cfg, data, 4, 0x10, 1, AM_BSP_GPIO_IOM1_CS);

    // Process data...
    while (1);
}
```

### DMA Read with Callback

```c
#include "ns_spi.h"
#include <stdio.h>

void dma_complete_cb(struct ns_spi_cfg *cfg) {
    printf("DMA transfer complete\n");
    // You can re-schedule another dma_read, or process buffer
}

int main(void) {
    ns_spi_config_t spi_cfg = { .iom = 1, .cb = dma_complete_cb };
    ns_spi_interface_init(&spi_cfg, AM_HAL_IOM_8MHZ, AM_HAL_IOM_SPI_MODE_3);

    uint8_t buffer[16];
    // Kick off a DMA read of 16 bytes from reg 0x20
    ns_spi_read_dma(&spi_cfg, buffer, 16, 0x20, 1, AM_BSP_GPIO_IOM1_CS);

    // The dma_complete_cb will be invoked upon completion
    while (1) {
        // sleep or handle other tasks
    }
}
```

---

