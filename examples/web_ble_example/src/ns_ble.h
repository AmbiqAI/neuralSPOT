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

#include "ns_ambiqsuite_harness.h"
#include "ns_malloc.h"
#include "wsf_types.h"
#include "wsf_buf.h"
#include "wsf_msg.h"
#include "wsf_trace.h"

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
#include "smp_api.h"
#include "smp_handler.h"
#include "svc_ch.h"
#include "svc_core.h"
#include "svc_dis.h"

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

// *** Helper Macros
#define NS_BLE_CHAR_DECL(_char)                                                                    \
    {                                                                                              \
        .pUuid = attChUuid, .pValue = (uint8_t *)_char##Ch, .pLen = (uint16_t *)&_char##ChLen,     \
        .maxLen = sizeof(_char##Ch), .settings = 0, .permissions = ATTS_PERMIT_READ,               \
    }

#define NS_BLE_CHAR_VAL(_prop, _settings, _permissions)                                            \
    {                                                                                              \
        .pUuid = _prop##ChUuid, .pValue = (uint8_t *)_prop##ChData,                                \
        .pLen = (uint16_t *)&_prop##ChDataLen, .maxLen = sizeof(_prop##ChData),                    \
        .settings = _settings, .permissions = _permissions,                                        \
    }

#define NS_BLE_CCC_DECL(_prop)                                                                     \
    {                                                                                              \
        .pUuid = attCliChCfgUuid, .pValue = (uint8_t *)_prop##ChCcc,                               \
        .pLen = (uint16_t *)&_prop##ChCccLen, .maxLen = sizeof(_prop##ChCcc),                      \
        .settings = ATTS_SET_CCC, .permissions = (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE),           \
    }

/*! Base UUID:  00002760-08C2-11E1-9073-0E8AC72EXXXX */
// #define NS_BLE_ATT_UUID_BASE
//     0x2E, 0xC7, 0x8A, 0x0E, 0x73, 0x90, 0xE1, 0x11, 0xC2, 0x08, 0x60, 0x27, 0x00, 0x00
// #define NS_BLE_ATT_UUID_BUILD(part) UINT16_TO_BYTES(part), NS_BLE_ATT_UUID_BASE

/*! Base UUID:  00002760-08C2-11E1-9073-0E8AC72EXXXX */
#define NS_BLE_ATT_UUID_PREAMBLE 0x00, 0x00, 0xb1, 0x19
#define NS_BLE_ATT_UUID_POSTAMBLE 0x14, 0x12, 0x8a, 0x76, 0x04, 0xd1, 0x6c, 0x4f, 0x7e, 0x53
#define NS_BLE_ATT_UUID_BUILD(part)                                                                \
    NS_BLE_ATT_UUID_POSTAMBLE, UINT16_TO_BYTES(part), NS_BLE_ATT_UUID_PREAMBLE

#ifndef NS_BLE_CONN_MAX
    #define NS_BLE_CONN_MAX 1
#endif

#define NS_BLE_MAX_SERVICES 1

// *** Typedefs Prototypes (for callbacks)
typedef struct ns_ble_control ns_ble_control_t;
typedef struct ns_ble_service_control ns_ble_service_control_t;
typedef struct ns_ble_service ns_ble_service_t;
typedef struct ns_ble_characteristic ns_ble_characteristic_t;

typedef union {
    wsfMsgHdr_t hdr;
    dmEvt_t dm;
    attsCccEvt_t ccc;
    attEvt_t att;
} ns_ble_msg_t;

typedef void (*ns_ble_service_specific_handler_cb)(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
typedef void (*ns_ble_service_specific_handler_init_cb)(wsfHandlerId_t handlerId);
typedef void (*ns_ble_service_specific_init_cb)(
    wsfHandlerId_t handlerId, struct ns_ble_control *generic_cfg,
    struct ns_ble_service_control *specific_cfg);
typedef bool (*ns_ble_service_specific_procMsg_cb)(ns_ble_msg_t *pMsg);

/*! Application message type */

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
    attsCccSet_t *cccSet;
    uint32_t cccCount;

    // Advertising Data
    uint8_t *advData;
    uint8_t advDataLen;
    uint8_t *scanData;
    uint8_t scanDataLen;

    // Service callbacks
    // attsReadCback_t read_cb; /*! Read callback */

    ns_ble_service_specific_handler_cb handler_cb; /*! Called after generic event handling */
    ns_ble_service_specific_handler_init_cb handler_init_cb; /*! Service-specific handler init */
    ns_ble_service_specific_init_cb service_init_cb;         /*! Service-specific init */
    ns_ble_service_specific_procMsg_cb procMsg_cb;           /*! Service-specific message handler */

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
    uint16_t nextHandleId;    /*! Next handle ID to allocate */

    // Config
    bool secureConnections; /*! TRUE to use secure connections */

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

    // New API ------
    ns_ble_service_t *services[NS_BLE_MAX_SERVICES]; /*! Array of services */
    uint16_t numServices;                            /*! Number of services */

} ns_ble_control_t;

// ----------------------------------------------------------------------------
// NS BLE API
// ----------------------------------------------------------------------------
typedef struct {
    uint8_t array[16];
} ns_ble_uuid128_t;

typedef struct {
    uint32_t *pool;
    uint32_t poolSize;
    wsfBufPoolDesc_t *desc;
    uint32_t descNum;
} ns_ble_pool_config_t;

typedef struct ns_ble_service {
    // Config
    ns_ble_uuid128_t uuid128; //! char array of 128b UUID
    char name[31];
    uint8_t nameLen;
    uint16_t baseHandle; //! initialize to first handle of service
    uint16_t numAttributes;
    uint16_t numCharacteristics;
    ns_ble_pool_config_t *poolConfig;

    // Internals
    uint16_t handleId;
    ns_ble_service_control_t *control;
    ns_ble_characteristic_t **characteristics; //! array of characteristics
    uint16_t nextCharacteristicIndex;
    attsCccSet_t *cccSet;
    uint16_t nextCccIndex;
    uint16_t nextCccIndicationHandle;

    attsGroup_t group;      //! attribute group for Cordio
    attsAttr_t *attributes; //! array of attributes for Cordio
    uint16_t nextHandleId;
    uint16_t maxHandle;
    uint16_t nextAttributeIndex;

    // Advertising Data
    uint8_t *advData;
    uint8_t advDataLen;
    uint8_t *scanData;
    uint8_t scanDataLen;

    // Attributes
    uint16_t primaryAttributeLen;
    attsAttr_t primaryAttribute;
    attsAttr_t versionAttribute;
    attsAttr_t nameAttribute;

    // Cordio Callbacks
    attsReadCback_t readCback;   /*!< \brief Read callback function */
    attsWriteCback_t writeCback; /*!< \brief Write callback function */
} ns_ble_service_t;

typedef int (*ns_ble_characteristic_read_handler_t)(
    ns_ble_service_t *, ns_ble_characteristic_t *, void *);
typedef int (*ns_ble_characteristic_write_handler_t)(
    ns_ble_service_t *, ns_ble_characteristic_t *, void *);
typedef int (*ns_ble_characteristic_notify_handler_t)(
    ns_ble_service_t *, ns_ble_characteristic_t *);

typedef struct ns_ble_characteristic {
    // Config
    ns_ble_uuid128_t uuid128; //! char array of 128b UUID
    // uint16_t properties;
    // uint16_t permissions;
    // uint16_t maxLen;
    // uint16_t initLen;

    void *applicationValue; //! pointer to application's value store

    // Handlers
    ns_ble_characteristic_read_handler_t readHandlerCb;
    ns_ble_characteristic_write_handler_t writeHandlerCb;
    ns_ble_characteristic_notify_handler_t notifyHandlerCb;

    // Attributes
    // Declaration
    uint8_t declarationProperties[19];
    uint16_t declarationLen;
    attsAttr_t declaration;
    uint16_t declarationHandle;

    // Value
    uint8_t *pValue;
    uint16_t valueLen;
    attsAttr_t value;
    uint16_t valueHandle;

    // CCC (only populated for notify characteristics)
    uint8_t cccArray[2];
    uint16_t cccLen;
    attsAttr_t ccc;
    uint16_t cccHandle;
    uint16_t cccIndex;
    uint16_t cccIndicationHandle;
    wsfTimer_t indicationTimer; /*! \brief periodic measurement timer */
    uint32_t indicationPeriod;  /*! \brief periodic measurement period in ms */

    // Internals
    uint16_t handleId;

} ns_ble_characteristic_t;

extern ns_ble_control_t g_ns_ble_control;

extern void ns_ble_generic_conn_open(dmEvt_t *pMsg);
extern void ns_ble_generic_conn_update(dmEvt_t *pMsg);
extern void ns_ble_generic_CccCback(attsCccEvt_t *pEvt);
extern void ns_ble_generic_AttCback(attEvt_t *pEvt);
extern void ns_ble_generic_DmCback(dmEvt_t *pDmEvt);
extern void ns_ble_generic_advSetup(ns_ble_msg_t *pMsg);

extern void ns_ble_generic_handlerInit(wsfHandlerId_t handlerId, ns_ble_service_control_t *cfg);
extern void ns_ble_generic_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);
extern void ns_ble_pre_init(void);
extern void ns_ble_generic_init(
    bool useDefault, ns_ble_control_t *generic_cfg, ns_ble_service_control_t *service_cfg);

enum {
    NS_BLE_READ = 1,
    NS_BLE_WRITE = 2,
    NS_BLE_NOTIFY = 4,
    NS_BLE_PROP_MAX,
};

extern int ns_ble_create_service(ns_ble_service_t *);
extern int ns_ble_create_characteristic(
    ns_ble_characteristic_t *c, char const *uuidString, void *applicationValue,
    uint16_t valueLength, uint16_t properties, ns_ble_characteristic_read_handler_t readHandlerCb,
    ns_ble_characteristic_write_handler_t writeHandlerCb,
    ns_ble_characteristic_notify_handler_t notifyHandlerCb, uint16_t periodMs,
    uint16_t *attributeCount);

extern int ns_ble_add_characteristic(ns_ble_service_t *, ns_ble_characteristic_t *);
extern int ns_ble_char2uuid(const char uuidString[16], ns_ble_uuid128_t *uuid128);
extern int ns_ble_start_service(ns_ble_service_t *);

#ifdef __cplusplus
}
#endif
/** @}*/
#endif // NS_BLE
