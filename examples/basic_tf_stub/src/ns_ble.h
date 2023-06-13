/**
 * @file ns_ble.h
 * @author Ambiq Team
 * @brief Generic BLE Wrapper
 * @version 0.1
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef NS_BLE
#define NS_BLE

#ifdef __cplusplus
extern "C" {
#endif

#include "app_api.h"
#include "app_db.h"
#include "app_hw.h"
#include "app_ui.h"
#include "att_api.h"
#include "att_handler.h"
#include "bstream.h"
#include "dm_api.h"
#include "dm_handler.h"
#include "gatt_api.h"
#include "hci_api.h"
#include "l2c_api.h"
#include "l2c_handler.h"
#include "ns_ambiqsuite_harness.h"
#include "smp_api.h"
#include "smp_handler.h"
#include "svc_ch.h"
#include "svc_core.h"
#include "svc_dis.h"
#include "wsf_buf.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "wsf_types.h"
#include <stdbool.h>
#include <string.h>

#include "att_api.h"
#include "hci_core.h"
#include "hci_drv_apollo.h"
#include "hci_drv_cooper.h"
#include "hci_handler.h"

// *** Versions
#define NS_BLE_V0_0_1                                                                              \
    { .major = 0, .minor = 0, .revision = 1 }

#define NS_BLE_OLDEST_SUPPORTED_VERSION NS_BLE_V0_0_1
#define NS_BLE_CURRENT_VERSION NS_BLE_V0_0_1
#define NS_BLE_API_ID 0xCA0008

extern const ns_core_api_t ns_ble_V0_0_1;
extern const ns_core_api_t ns_ble_oldest_supported_version;
extern const ns_core_api_t ns_ble_current_version;

// *** Macros
/*! Base UUID:  00002760-08C2-11E1-9073-0E8AC72EXXXX */
#define NS_ATT_UUID_BASE                                                                           \
    0x2E, 0xC7, 0x8A, 0x0E, 0x73, 0x90, 0xE1, 0x11, 0xC2, 0x08, 0x60, 0x27, 0x00, 0x00
#define NS_ATT_UUID_BUILD(part) UINT16_TO_BYTES(part), NS_ATT_UUID_BASE

#ifndef NS_BLE_CONN_MAX
    #define NS_BLE_CONN_MAX 1
#endif

// *** Typedefs
struct ns_ble_control;
struct ns_ble_service_control;

typedef void (*ns_ble_service_specific_handler_cb)(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
typedef void (*ns_ble_service_specific_handler_init_cb)(wsfHandlerId_t handlerId);
typedef void (*ns_ble_service_specific_init_cb)(wsfHandlerId_t handlerId,
                                                struct ns_ble_control *generic_cfg,
                                                struct ns_ble_service_control *specific_cfg);

/*! connection control block */
typedef struct {
    appDbHdl_t dbHdl; /*! Device database record handle type */
    uint8_t addrType; /*! Type of address of device to connect to */
    bdAddr_t addr;    /*! Address of device to connect to */
    bool_t doConnect; /*! TRUE to issue connect on scan complete */
} ns_ble_connection_t;

typedef struct ns_ble_service_control {
    // State
    wsfHandlerId_t handlerId;        /*! WSF handler ID */
    bool_t scanning;                 /*! TRUE if scanning */
    bool_t autoConnect;              /*! TRUE if auto-connecting */
    uint8_t discState;               /*! Service discovery state */
    uint8_t hdlListLen;              /*! Cached handle list length */
    ns_ble_connection_t *connection; /*! Connection control block */

    // Service Config
    // WSF Service Metadata
    attsGroup_t *group; /*! Attribute group */
    attsAttr_t *list;   /*! Attribute list */
    attsCccSet_t const *cccSet;
    uint32_t cccCount;

    // Service callbacks
    attsReadCback_t read_cb; /*! Read callback */

    ns_ble_service_specific_handler_cb handler_cb; /*! Called after generic event handling */
    ns_ble_service_specific_handler_init_cb handler_init_cb; /*! Service-specific handler init */
    ns_ble_service_specific_init_cb service_init_cb;         /*! Service-specific init */

    // Internals
    // WSF Buffers
    uint8_t wsfBufCount;
    uint32_t *bufferPool;
    uint32_t bufferPoolSize; // in bytes
    wsfBufPoolDesc_t *bufferDescriptors;
    uint32_t bufferDescriptorsSize; // in bytes
} ns_ble_service_control_t;

typedef struct ns_ble_control {
    const ns_core_api_t *api; ///< API prefix

    // State
    // uint16_t hdlList[APP_DB_HDL_LIST_LEN]; /*! Cached handle list */ (needed?)
    wsfHandlerId_t handlerId; /*! WSF hander ID */
    bool_t scanning;          /*! TRUE if scanning */
    bool_t autoConnect;       /*! TRUE if auto-connecting */
    uint8_t discState;        /*! Service discovery state */
    uint8_t hdlListLen;       /*! Cached handle list length */

    // Config
    // WSF Config Structs
    appAdvCfg_t *advCfg;       /*! Advertising configuration */
    appSlaveCfg_t *slaveCfg;   /*! Master configuration */
    appSecCfg_t *secCfg;       /*! Security configuration */
    appUpdateCfg_t *updateCfg; /*! Connection update configuration */
    smpCfg_t *smpCfg;          /*! SMP configuration */
    hciConnSpec_t *connCfg;    /*! Connection parameters */
    appDiscCfg_t *appDiscCfg;  /*! Discovery configuration */
    appCfg_t *appCfg;          /*! Application configuration */

    // Service Config
    ns_ble_service_control_t *service_config; /*! Service configuration */

} ns_ble_control_t;

extern ns_ble_control_t g_ns_ble_control;

extern void
ns_ble_generic_handlerInit(wsfHandlerId_t handlerId, ns_ble_service_control_t *cfg);
extern void
ns_ble_generic_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
extern void
ns_ble_generic_init(bool useDefault, ns_ble_control_t *generic_cfg,
                    ns_ble_service_control_t *service_cfg);

#ifdef __cplusplus
}
#endif
/** @}*/
#endif // NS_BLE
