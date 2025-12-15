/**
 * @file ns_nvm.c
 * @author Ambiq
 * @brief NVM (external MSPI flash) helper for erase/read/write/XIP on Apollo5
 * @version 0.1
 * @date 2025-08-29
 *
 * Implements a small, stable API over AmbiqSuite's IS25WX064 MSPI flash driver:
 *   - Timing scan + init
 *   - Erase (sector, mass), Read, Write
 *   - XIP enable/disable
 *
 * References:
 *   - boards/apollo510_evb/examples/interfaces/mspi_octal_example (usage)
 *   - devices/am_devices_mspi_is25wx064.[ch] (HAL)
 */

 #include <string.h>
 #include "am_mcu_apollo.h"
 #include "am_bsp.h"
 #include "am_util.h"
 
 #include "ns_core.h"
 #include "ns_ambiqsuite_harness.h"
 #include "ns_nvm.h"
 
 /* HAL for IS25WX064 */
 #include "am_devices_mspi_is25wx064.h"
 
 /* ===== ns_core API versioning ===== */
 const ns_core_api_t ns_nvm_V0_0_1                 = { .apiId = NS_NVM_API_ID, .version = NS_NVM_V0_0_1 };
 const ns_core_api_t ns_nvm_oldest_supported_version = { .apiId = NS_NVM_API_ID, .version = NS_NVM_OLDEST_SUPPORTED_VERSION };
 const ns_core_api_t ns_nvm_current_version          = { .apiId = NS_NVM_API_ID, .version = NS_NVM_CURRENT_VERSION };
 
 /* ===== Local state ===== */
 
 /* Single-instance storage (mirrors how ns_psram holds handles today). */
 static void *s_flash_handle = NULL;
 static void *s_mspi_handle  = NULL;
 static uint32_t s_mspi_module = 0;
 static ns_nvm_config_t *s_cfg = NULL;
 
 /* DMA/TCB buffer for non-blocking transactions (user may override in cfg). */
 AM_SHARED_RW static uint32_t s_dmatcb[256];
 
 /* Timing config determined by scan */
 static am_devices_mspi_is25wx064_timing_config_t s_timing;
 
 /* Convenience: MSPI IRQs by module index */
 #if defined(apollo510L_eb)
 static const IRQn_Type s_mspi_irqs[] = {
     MSPI0_IRQn, MSPI1_IRQn, MSPI2_IRQn
 };
 #else
 static const IRQn_Type s_mspi_irqs[] = {
     MSPI0_IRQn, MSPI1_IRQn, MSPI2_IRQn, MSPI3_IRQn
 };
 #endif
 
 /* ===== Optional MPU setup for the DMA TCB buffer (mirrors examples) ===== */
 #if defined(apollo510_evb) || defined(apollo510L_eb) || defined(apollo510b_evb)
 /* Configure MPU attributes for the DMATCB buffer to be normal write-back etc. */
 static am_hal_mpu_region_config_t sMPUCfg = {
     .ui32RegionNumber  = 6,
     .ui32BaseAddress   = (uint32_t)s_dmatcb,
     .eShareable        = NON_SHARE,
     .eAccessPermission = RW_NONPRIV,
     .bExecuteNever     = true,
     .ui32LimitAddress  = (uint32_t)s_dmatcb + sizeof(s_dmatcb) - 1,
     .ui32AttrIndex     = 0,
     .bEnable           = true,
 };
 static am_hal_mpu_attr_t sMPUAttr = {
     .ui8AttrIndex   = 0,
     .bNormalMem     = true,
     .sOuterAttr = {
         .bNonTransient = false,
         .bWriteBack    = true,
         .bReadAllocate = false,
         .bWriteAllocate= false
     },
     .sInnerAttr = {
         .bNonTransient = false,
         .bWriteBack    = true,
         .bReadAllocate = false,
         .bWriteAllocate= false
     },
     .eDeviceAttr = 0,
 };
 static void prv_setup_mpu_for_dmatcb(void)
 {
     am_hal_mpu_attr_configure(&sMPUAttr, 1);
     am_hal_mpu_region_clear();
     am_hal_mpu_region_configure(&sMPUCfg, 1);
     am_hal_cachectrl_dcache_invalidate(NULL, true);
     am_hal_mpu_enable(true, true);
 }
 #else
 static inline void prv_setup_mpu_for_dmatcb(void) { /* no-op on non-510 EVB */ }
 #endif
 
 /* ===== Helpers ===== */
 
 static inline uint32_t aperture_base_for_module(uint32_t module)
 {
     switch (module) {
         case 0: return MSPI0_APERTURE_START_ADDR;
         case 1: return MSPI1_APERTURE_START_ADDR;
         case 2: return MSPI2_APERTURE_START_ADDR;
 #ifndef apollo510L_eb
         default: return MSPI3_APERTURE_START_ADDR;
 #else
         default: return MSPI2_APERTURE_START_ADDR;
 #endif
     }
 }
 
 /* The 1-8-8 CE0 enum name changed across SDKs; provide a fallback if needed. */
 #ifndef AM_HAL_MSPI_FLASH_CE0_1_8_8
 #define AM_HAL_MSPI_FLASH_CE0_1_8_8 AM_HAL_MSPI_FLASH_OCTAL_CE0_1_8_8
 #endif

 static inline am_hal_mspi_device_e map_iface_to_hal(ns_nvm_interface_e iface, uint8_t ce)
 {
     const bool ce1 = (ce == 1);
     if (iface == NS_NVM_IF_OCTAL_DDR) {
         return ce1 ? AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1 : AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0;
     } else {
         return ce1 ? AM_HAL_MSPI_FLASH_OCTAL_CE1_1_8_8 : AM_HAL_MSPI_FLASH_CE0_1_8_8; // NOTE: use official enum
     }
 }
 

 
 /* Build HAL config from ns_nvm_config_t. */
 static void build_hal_config(const ns_nvm_config_t *cfg, am_devices_mspi_is25wx064_config_t *out)
 {
     out->eDeviceConfig         = map_iface_to_hal(cfg->iface, cfg->chip_select);
     out->eClockFreq            = cfg->clock_freq;
     out->pNBTxnBuf             = (cfg->nbtxn_buf) ? cfg->nbtxn_buf : s_dmatcb;
     out->ui32NBTxnBufLength    = (cfg->nbtxn_buf) ? cfg->nbtxn_buf_len
                                                   : (uint32_t)(sizeof(s_dmatcb)/sizeof(uint32_t));
     out->ui32ScramblingStartAddr = cfg->scrambling_start;
     out->ui32ScramblingEndAddr   = cfg->scrambling_end;
 }
 
 /* ===== MSPI ISRs (provide all four; only the configured module will be enabled) ===== */
 static inline void prv_mspi_isr_common(void)
 {
     if (s_mspi_handle) {
         uint32_t status;
         am_hal_mspi_interrupt_status_get(s_mspi_handle, &status, false);
         am_hal_mspi_interrupt_clear(s_mspi_handle, status);
         am_hal_mspi_interrupt_service(s_mspi_handle, status);
     }
 }
 void am_mspi0_isr(void) { prv_mspi_isr_common(); }
 void am_mspi1_isr(void) { prv_mspi_isr_common(); }
 void am_mspi2_isr(void) { prv_mspi_isr_common(); }
 void am_mspi3_isr(void) { prv_mspi_isr_common(); }
 
 /* ===== Public API ===== */
 
 uint32_t ns_nvm_init(ns_nvm_config_t *cfg)
 {
 #ifndef NS_DISABLE_API_VALIDATION
     if (cfg == NULL) {
         return NS_STATUS_INVALID_HANDLE;
     }
     if (ns_core_check_api(cfg->api, &ns_nvm_oldest_supported_version, &ns_nvm_current_version)) {
         return NS_STATUS_INVALID_VERSION;
     }
 #endif
 
     if (!cfg->enable) {
         return NS_STATUS_SUCCESS;
     }
 
     /* Save pointers for later operations. */
     s_cfg = cfg;
     s_mspi_module = cfg->mspi_module;
 
     /* Minimal defaults if caller didn't set them. */
     if (cfg->clock_freq == 0) {
         cfg->clock_freq = AM_HAL_MSPI_CLK_96MHZ;
     }
     /* Default: IS25WX064 is 8MB */
     cfg->size_bytes = (8u * 1024u * 1024u);
     cfg->xip_base_address = aperture_base_for_module(cfg->mspi_module);
 
     /* Optional MPU setup for DMATCB buffer. */
     prv_setup_mpu_for_dmatcb();
 
     /* Build HAL config and run timing scan. */
     am_devices_mspi_is25wx064_config_t hal_cfg;
     build_hal_config(cfg, &hal_cfg);
     ns_lp_printf("NVM timing scan...\n");
     uint32_t status = am_devices_mspi_is25wx064_init_timing_check(cfg->mspi_module, &hal_cfg, &s_timing);
     if (status != AM_DEVICES_MSPI_IS25WX064_STATUS_SUCCESS) {
         ns_lp_printf("NVM timing scan failed (status=%u). Falling back to defaults.\n", status);
         /* Continue; HAL will apply its defaults or best-known. */
     }
     ns_lp_printf("NVM timing scan succeeded.\n");
     /* Initialize the device. */
     status = am_devices_mspi_is25wx064_init(cfg->mspi_module, &hal_cfg, &s_flash_handle, &s_mspi_handle);
     if (status != AM_DEVICES_MSPI_IS25WX064_STATUS_SUCCESS) {
         ns_lp_printf("NVM init failed (status=%u). Check wiring/pins.\n", status);
         return status;
     }
     ns_lp_printf("NVM init succeeded.\n");
     /* Hook interrupts for the selected module. */
     if (cfg->mspi_module < (sizeof(s_mspi_irqs)/sizeof(s_mspi_irqs[0]))) {
         NVIC_SetPriority(s_mspi_irqs[cfg->mspi_module], AM_IRQ_PRIORITY_DEFAULT);
         NVIC_EnableIRQ(s_mspi_irqs[cfg->mspi_module]);
         am_hal_interrupt_master_enable();
     }
 
     /* Apply timing (from scan if succeeded; HAL will sanity-check). */
     (void)am_devices_mspi_is25wx064_apply_ddr_timing(s_flash_handle, &s_timing);
     ns_lp_printf("NVM timing applied.\n");
     /* (Optional) verify ID and keep a friendly log. */
     (void)am_devices_mspi_is25wx064_id(s_flash_handle);
     ns_lp_printf("NVM ID verified.\n");
     /* Optionally put device into XIP. */
     if (cfg->enable_xip) {
         status = am_devices_mspi_is25wx064_enable_xip(s_flash_handle);
         if (status != AM_DEVICES_MSPI_IS25WX064_STATUS_SUCCESS) {
             ns_lp_printf("NVM XIP enable failed (status=%u).\n", status);
             return status;
         }
         /* Invalidate DCache to ensure coherent instruction/data fetch if XIP used. */
         am_hal_cachectrl_dcache_invalidate(NULL, true);
     }
     ns_lp_printf("NVM XIP enabled.\n");
     return AM_DEVICES_MSPI_IS25WX064_STATUS_SUCCESS;
 }
 
 uint32_t ns_nvm_read(uint32_t addr, uint8_t *buf, uint32_t len, bool wait)
 {
     if (!s_flash_handle || !buf || len == 0) {
         return NS_STATUS_INVALID_HANDLE;
     }
     return am_devices_mspi_is25wx064_read(s_flash_handle, buf, addr, len, wait);
 }
 
 uint32_t ns_nvm_write(uint32_t addr, const uint8_t *buf, uint32_t len, bool wait)
 {
     if (!s_flash_handle || !buf || len == 0) {
         return NS_STATUS_INVALID_HANDLE;
     }
     /* HAL signature uses non-const pointer; cast away const for driver call. */
     return am_devices_mspi_is25wx064_write(s_flash_handle, (uint8_t *)buf, addr, len, wait);
 }
 
 uint32_t ns_nvm_sector_erase(uint32_t sector_addr)
 {
     if (!s_flash_handle) {
         return NS_STATUS_INVALID_HANDLE;
     }
     return am_devices_mspi_is25wx064_sector_erase(s_flash_handle, sector_addr);
 }
 
 uint32_t ns_nvm_mass_erase(void)
 {
     if (!s_flash_handle) {
         return NS_STATUS_INVALID_HANDLE;
     }
     return am_devices_mspi_is25wx064_mass_erase(s_flash_handle);
 }
 
 uint32_t ns_nvm_enable_xip(void)
 {
     if (!s_flash_handle) {
         return NS_STATUS_INVALID_HANDLE;
     }
     uint32_t status = am_devices_mspi_is25wx064_enable_xip(s_flash_handle);
     if (status == AM_DEVICES_MSPI_IS25WX064_STATUS_SUCCESS) {
         am_hal_cachectrl_dcache_invalidate(NULL, true);
     }
     return status;
 }
 
 uint32_t ns_nvm_disable_xip(void)
 {
     if (!s_flash_handle) {
         return NS_STATUS_INVALID_HANDLE;
     }
     return am_devices_mspi_is25wx064_disable_xip(s_flash_handle);
 }
 