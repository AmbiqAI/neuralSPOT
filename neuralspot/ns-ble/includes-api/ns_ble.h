/**
 * @file ns_ble.h
 * @author Ambiq Team
 * @brief Generic BLE Wrapper
 * @version 0.1
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 * \addtogroup ns-ble
 * @{
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
    #if defined(AM_PART_APOLLO3P) || defined(AM_PART_APOLLO3)
        #include "hci_drv_apollo3.h"
    #else
        #include "hci_drv_cooper.h"
    #endif
    #include "hci_handler.h"

    // *** Versions
    #define NS_BLE_V0_0_1                                                                          \
        { .major = 0, .minor = 0, .revision = 1 }

    #define NS_BLE_OLDEST_SUPPORTED_VERSION NS_BLE_V0_0_1
    #define NS_BLE_CURRENT_VERSION NS_BLE_V0_0_1
    #define NS_BLE_API_ID 0xCA0008

    #define NS_BLE_DEFAULT_MALLOC_K 8

extern const ns_core_api_t ns_ble_V0_0_1;
extern const ns_core_api_t ns_ble_oldest_supported_version;
extern const ns_core_api_t ns_ble_current_version;

    // *** Helper Macros
    #define NS_BLE_CHAR_DECL(_char)                                                                \
        {                                                                                          \
            .pUuid = attChUuid, .pValue = (uint8_t *)_char##Ch, .pLen = (uint16_t *)&_char##ChLen, \
            .maxLen = sizeof(_char##Ch), .settings = 0, .permissions = ATTS_PERMIT_READ,           \
        }

    #define NS_BLE_CHAR_VAL(_prop, _settings, _permissions)                                        \
        {                                                                                          \
            .pUuid = _prop##ChUuid, .pValue = (uint8_t *)_prop##ChData,                            \
            .pLen = (uint16_t *)&_prop##ChDataLen, .maxLen = sizeof(_prop##ChData),                \
            .settings = _settings, .permissions = _permissions,                                    \
        }

    #define NS_BLE_CCC_DECL(_prop)                                                                 \
        {                                                                                          \
            .pUuid = attCliChCfgUuid, .pValue = (uint8_t *)_prop##ChCcc,                           \
            .pLen = (uint16_t *)&_prop##ChCccLen, .maxLen = sizeof(_prop##ChCcc),                  \
            .settings = ATTS_SET_CCC, .permissions = (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE),       \
        }

    /*! Base UUID:  00002760-08C2-11E1-9073-0E8AC72EXXXX */
    // #define NS_BLE_ATT_UUID_BASE
    //     0x2E, 0xC7, 0x8A, 0x0E, 0x73, 0x90, 0xE1, 0x11, 0xC2, 0x08, 0x60, 0x27, 0x00, 0x00
    // #define NS_BLE_ATT_UUID_BUILD(part) UINT16_TO_BYTES(part), NS_BLE_ATT_UUID_BASE

    /*! Base UUID:  00002760-08C2-11E1-9073-0E8AC72EXXXX */
    #define NS_BLE_ATT_UUID_PREAMBLE 0x00, 0x00, 0xb1, 0x19
    #define NS_BLE_ATT_UUID_POSTAMBLE 0x14, 0x12, 0x8a, 0x76, 0x04, 0xd1, 0x6c, 0x4f, 0x7e, 0x53
    #define NS_BLE_ATT_UUID_BUILD(part)                                                            \
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

/**
 * @brief BLE Connection control block
 */
typedef struct {
    appDbHdl_t dbHdl; /*! Device database record handle type */
    uint8_t addrType; /*! Type of address of device to connect to */
    bdAddr_t addr;    /*! Address of device to connect to */
    bool_t doConnect; /*! TRUE to issue connect on scan complete */
} ns_ble_connection_t;

/**
 * @brief BLE Service control block
 */
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
    attsGroup_t *group;   /*! Attribute group */
    attsAttr_t *list;     /*! Attribute list */
    attsCccSet_t *cccSet; /*! CCC descriptor list */
    uint32_t cccCount;    /*! Number of CCC descriptors */

    // Advertising Data
    uint8_t *advData;    /*! Advertising data */
    uint8_t advDataLen;  /*! Advertising data length */
    uint8_t *scanData;   /*! Scan data */
    uint8_t scanDataLen; /*! Scan data length */

    // Service callbacks
    // attsReadCback_t read_cb; /*! Read callback */

    ns_ble_service_specific_handler_cb handler_cb; /*! Called after generic event handling */
    ns_ble_service_specific_handler_init_cb handler_init_cb; /*! Service-specific handler init */
    ns_ble_service_specific_init_cb service_init_cb;         /*! Service-specific init */
    ns_ble_service_specific_procMsg_cb procMsg_cb;           /*! Service-specific message handler */

    // WSF Buffers
    uint8_t wsfBufCount;     ///< Number of WSF buffers
    uint32_t *bufferPool;    ///< Pointer to WSF buffer pool (allocated by caller)
    uint32_t bufferPoolSize; ///< in bytes
    wsfBufPoolDesc_t
        *bufferDescriptors;         ///< Pointer to WSF buffer descriptors (allocated by caller)
    uint32_t bufferDescriptorsSize; ///< in bytes
} ns_ble_service_control_t;

/**
 * @brief BLE Control block
 */
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

/**
 * @brief BLE UUID (128-bit)
 *
 */
typedef struct {
    uint8_t array[16];
} ns_ble_uuid128_t;

/**
 * @brief WSF Buffer Pool Configuration
 *
 */
typedef struct {
    uint32_t *pool;
    uint32_t poolSize;
    wsfBufPoolDesc_t *desc;
    uint32_t descNum;
} ns_ble_pool_config_t;

/**
 * @brief BLE Service Configuration
 *
 */
typedef struct ns_ble_service {
    // Config
    ns_ble_uuid128_t uuid128;         //! char array of 128b UUID
    char name[31];                    //! string name of service
    uint8_t nameLen;                  //! length of name string
    uint16_t baseHandle;              //! initialize to first handle of service
    uint16_t numAttributes;           //! Number of attributes in service (initialize to 0)
    uint16_t numCharacteristics;      //! Number of characteristics in service
    ns_ble_pool_config_t *poolConfig; //! pointer to WSF pool configuration struct

    // Internals - the following is initialized internally
    uint16_t handleId;
    ns_ble_service_control_t *control;
    ns_ble_characteristic_t **characteristics; // array of characteristics
    uint16_t nextCharacteristicIndex;
    attsCccSet_t *cccSet;
    uint16_t nextCccIndex;
    uint16_t nextCccIndicationHandle;

    attsGroup_t group;      // attribute group for Cordio
    attsAttr_t *attributes; // array of attributes for Cordio
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
    attsReadCback_t readCback;   /* Read callback function */
    attsWriteCback_t writeCback; /* Write callback function */
} ns_ble_service_t;

typedef int (*ns_ble_characteristic_read_handler_t)(
    ns_ble_service_t *, ns_ble_characteristic_t *, void *);
typedef int (*ns_ble_characteristic_write_handler_t)(
    ns_ble_service_t *, ns_ble_characteristic_t *, void *);
typedef int (*ns_ble_characteristic_notify_handler_t)(
    ns_ble_service_t *, ns_ble_characteristic_t *);

// Characteristic Configuration
typedef struct ns_ble_characteristic {
    // Config
    ns_ble_uuid128_t uuid128; // Characteristic UUID

    void *applicationValue; // pointer to application's value store
    uint16_t valueLen;      // In bytes

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
    attsAttr_t value;
    uint16_t valueHandle;

    // CCC (only populated for notify characteristics)
    uint8_t cccArray[2];
    uint16_t cccLen;
    attsAttr_t ccc;
    uint16_t cccHandle;
    uint16_t cccIndex;
    uint16_t cccIndicationHandle;
    wsfTimer_t indicationTimer;       /*! \brief periodic measurement timer */
    uint32_t indicationPeriod;        /*! \brief periodic measurement period in ms */
    uint8_t indicationIsAsynchronous; /*! \brief TRUE if indication is asynchronous */

    // Internals
    uint16_t handleId;

} ns_ble_characteristic_t;

enum {
    NS_BLE_READ = 1,
    NS_BLE_WRITE = 2,
    NS_BLE_NOTIFY = 4,
    NS_BLE_PROP_MAX,
};

/**
 * @brief Call this function from the setup_task, prior to creating the RadioTask
 * All this does is set up NVIC priorities
 */
extern void ns_ble_pre_init(void);

/**
 * @brief Create a BLE service based on the given configuration.
 *
 * @param s - a pointer to a service configuration struct. Relevant fields are:
 *  - uuid128: a 128-bit UUID for the service
 *  - name: a string name for the service, up to 31 characters
 *  - nameLen: the length of the name string
 *  - baseHandle: the first handle ID of the service. This is used by BLE to identify attributes of
 * the service
 *  - numAttributes: keeps track of the number of attributes in the service. Initialize to 0.
 *  - poolConfig: a pointer to a WSF pool configuration struct.
 * @return int
 */
extern int ns_ble_create_service(ns_ble_service_t *s);

extern void ns_ble_send_value(ns_ble_characteristic_t *c, attEvt_t *pMsg);

/**
 * @brief Define a characteristic for subsequent addition to a service.
 *
 * @param c - config struct, populated by this function
 * @param uuidString - a 16-byte UUID string
 * @param applicationValue - a pointer to the application's value store
 * @param valueLength - the length of the value store, in bytes
 * @param properties - a bitmask of properties for the characteristic, from the enum above
 * @param readHandlerCb - a callback function for read requests if the characteristic is readable
 * @param writeHandlerCb - a callback function for write requests if the characteristic is writable
 * @param notifyHandlerCb - a callback function for notify requests if the characteristic is
 * "subscribe-able"
 * @param periodMs - the period of the notify timer, in milliseconds
 * @param attributeCount - a pointer to the service's attribute count. This is incremented by the
 * function.
 * @return int
 */
extern int ns_ble_create_characteristic(
    ns_ble_characteristic_t *c, char const *uuidString, void *applicationValue,
    uint16_t valueLength, uint16_t properties, ns_ble_characteristic_read_handler_t readHandlerCb,
    ns_ble_characteristic_write_handler_t writeHandlerCb,
    ns_ble_characteristic_notify_handler_t notifyHandlerCb, uint16_t periodMs, uint8_t async,
    uint16_t *attributeCount);

/**
 * @brief Add a characteristic to a service. This function should be called after all
 * characteristics have been defined using ns_ble_create_characteristic.
 *
 * @param s - a pointer to the service to which the characteristic should be added
 * @param c - a pointer to the characteristic to be added
 *
 * @return int
 */
extern int ns_ble_add_characteristic(ns_ble_service_t *s, ns_ble_characteristic_t *c);

/**
 * @brief Helper function to convert a 16-byte UUID string to a 128-bit UUID byte array.
 * UUID string must be exactly 16 characters long and only contain hex characters.
 *
 * @param uuidString - a 16-byte UUID string
 * @param uuid128 - a pointer to a 128-bit UUID byte array (uuid128.array)
 * @return int
 */
extern int ns_ble_char2uuid(const char uuidString[16], ns_ble_uuid128_t *uuid128);

/**
 * @brief Invoked by RadioTask to start the service. This function should be called after all
 * characteristics have been added.
 *
 * @param s - a pointer to the service to be started
 * @return int
 */
extern int ns_ble_start_service(ns_ble_service_t *s);

    #ifdef __cplusplus
}
    #endif
#endif // NS_BLE
       /** @}*/
