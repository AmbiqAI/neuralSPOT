/**
 * @file ns_peripherals_psram.h
 * @author Ambiq
 * @brief Enable PSRAM
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef NS_PERIPHERAL_PSRAM
#define NS_PERIPHERAL_PSRAM

#ifdef __cplusplus
extern "C" {
#endif
#include "ns_core.h"

#define NS_PSRAM_V0_0_1                                                                           \
    { .major = 0, .minor = 0, .revision = 1 }
#define NS_PSRAM_V1_0_0                                                                           \
    { .major = 1, .minor = 0, .revision = 0 }
#define NS_PSRAM_OLDEST_SUPPORTED_VERSION NS_PSRAM_V0_0_1
#define NS_PSRAM_CURRENT_VERSION NS_PSRAM_V1_0_0

extern const ns_core_api_t ns_psram_V0_0_1;
// extern const ns_core_api_t ns_psram_V1_0_0;
extern const ns_core_api_t ns_psram_oldest_supported_version;
extern const ns_core_api_t ns_psram_current_version;
#define NS_PSRAM_API_ID 0xCA000A

typedef enum {
    PSRAM_TYPE_HEX = 0,
    PSRAM_TYPE_OCT = 1,
} psram_type_e;

typedef struct {
    ns_core_api_t const *api;              ///< API prefix
    bool psram_enable;                     ///< Enable PSRAM
    psram_type_e psram_type;               ///< PSRAM type
    uint8_t psram_block;                   ///< PSRAM block
    uint32_t psram_size;                   ///< PSRAM size, filled in by init
    uint32_t psram_base_address;           ///< PSRAM base address, filled in by init
} ns_psram_config_t;

/**
 * @brief Initialize PSRAM per the configuration struct
 *
 * @param ns_psram_config_t* : psram configuration struct
 *
 * @return uint32_t
 */
extern uint32_t ns_psram_init(ns_psram_config_t *);

#ifdef __cplusplus
}
#endif

#endif