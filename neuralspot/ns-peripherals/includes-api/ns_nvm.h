/**
 * @file ns_nvm.h
 * @author Ambiq
 * @brief NeuralSPOT NVM (external MSPI flash) helper API
 * @version 0.1
 * @date 2025-08-29
 *
 * This API provides a thin, stable wrapper over AmbiqSuite MSPI flash HAL
 * to support erase/read/write and XIP enable/disable, similar in spirit to
 * ns_psram. It validates the ns_core API prefix and exposes a small config.
 *
 * Typical flow:
 *   1) Fill ns_nvm_config_t (module, CE, mode, freq).
 *   2) Call ns_nvm_init(&cfg). This performs timing scan, init, and (optionally) XIP.
 *   3) Use ns_nvm_read / ns_nvm_write / ns_nvm_sector_erase / ns_nvm_mass_erase.
 *   4) Optionally toggle XIP via ns_nvm_enable_xip / ns_nvm_disable_xip.
 *
 * The current implementation targets IS25WX064 (64 Mbit) by default.
 *
 * Copyright (c) 2025 Ambiq
 */

 #ifndef NS_NVM_H
 #define NS_NVM_H
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 #include "ns_core.h"
 #include "am_mcu_apollo.h"   // For am_hal_mspi_* enums
 #include <stdbool.h>
 #include <stdint.h>
 
 /* ===== Versioning (ns_core-style) ===== */
 #define NS_NVM_V0_0_1         { .major = 0, .minor = 0, .revision = 1 }
 #define NS_NVM_V1_0_0         { .major = 1, .minor = 0, .revision = 0 }
 #define NS_NVM_OLDEST_SUPPORTED_VERSION NS_NVM_V0_0_1
 #define NS_NVM_CURRENT_VERSION          NS_NVM_V1_0_0
 
 #define NS_NVM_API_ID 0xCA000B
 
 extern const ns_core_api_t ns_nvm_V0_0_1;
 extern const ns_core_api_t ns_nvm_oldest_supported_version;
 extern const ns_core_api_t ns_nvm_current_version;
 
 /* ===== Public types ===== */
 
 typedef enum {
     NS_NVM_IF_OCTAL_DDR = 0,   ///< AM_HAL_MSPI_FLASH_OCTAL_DDR_CE{0,1}
     NS_NVM_IF_OCTAL_1_8_8 = 1  ///< AM_HAL_MSPI_FLASH_OCTAL_CE{0,1}_1_8_8 (SDR)
 } ns_nvm_interface_e;
 
 /**
  * @brief NVM configuration
  */
 typedef struct {
     /* ns_core api prefix */
     ns_core_api_t const *api;      ///< API prefix (use &ns_nvm_current_version)
 
     /* Features */
     bool enable;                   ///< Set true to enable/init the device
     bool enable_xip;               ///< Enable XIP at the end of init (optional)
 
     /* MSPI/Device selection */
     uint8_t mspi_module;           ///< 0..3 (depends on part/package)
     uint8_t chip_select;           ///< 0 -> CE0, 1 -> CE1
     ns_nvm_interface_e iface;      ///< Octal DDR vs. 1-8-8
     am_hal_mspi_clock_e clock_freq;///< e.g. AM_HAL_MSPI_CLK_96MHZ, _125MHZ, etc.
 
     /* Optional non-blocking TCB buffer (if NULL, an internal one is used) */
     uint32_t *nbtxn_buf;           ///< Pointer to DMA/TCB buffer (uint32_t)
     uint32_t nbtxn_buf_len;        ///< Length in uint32_t elements
 
     /* Optional MSPI scrambling window (0 disables) */
     uint32_t scrambling_start;
     uint32_t scrambling_end;
 
     /* Out params filled on init */
     uint32_t xip_base_address;     ///< XIP aperture base for selected module
     uint32_t size_bytes;           ///< Device capacity in bytes (8MB for IS25WX064)
 } ns_nvm_config_t;
 
 /* ===== Public API ===== */
 
 /**
  * @brief Initialize NVM per the configuration struct (timing scan + init).
  *
  * @param cfg Config struct (must remain valid during operation).
  * @return 0 on success, ns_core error codes on validation errors,
  *         or HAL device status (AM_DEVICES_MSPI_IS25WX064_STATUS_*) on HAL error.
  */
 uint32_t ns_nvm_init(ns_nvm_config_t *cfg);
 
 /**
  * @brief Read from NVM into a buffer.
  *
  * @param addr   Byte address in NVM.
  * @param buf    Destination buffer in SRAM.
  * @param len    Number of bytes to read.
  * @param wait   If true, block until completion.
  *
  * @return HAL device status code.
  */
 uint32_t ns_nvm_read(uint32_t addr, uint8_t *buf, uint32_t len, bool wait);
 
 /**
  * @brief Write to NVM from a buffer. Caller must ensure erase has been done.
  *
  * @param addr   Byte address in NVM (page boundaries handled by HAL).
  * @param buf    Source buffer in SRAM.
  * @param len    Number of bytes to write.
  * @param wait   If true, block until completion.
  *
  * @return HAL device status code.
  */
 uint32_t ns_nvm_write(uint32_t addr, const uint8_t *buf, uint32_t len, bool wait);
 
 /**
  * @brief Erase a single sector containing the given address.
  *        Sector size for IS25WX064 is 128KB.
  *
  * @param sector_addr Sector-aligned (or any address within sector).
  * @return HAL device status code.
  */
 uint32_t ns_nvm_sector_erase(uint32_t sector_addr);
 
 /**
  * @brief Full chip erase. Use with care.
  * @return HAL device status code.
  */
 uint32_t ns_nvm_mass_erase(void);
 
 /**
  * @brief Enable XIP on the MSPI instance configured during init.
  * @return HAL device status code.
  */
 uint32_t ns_nvm_enable_xip(void);
 
 /**
  * @brief Disable XIP on the MSPI instance configured during init.
  * @return HAL device status code.
  */
 uint32_t ns_nvm_disable_xip(void);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NS_NVM_H */ 