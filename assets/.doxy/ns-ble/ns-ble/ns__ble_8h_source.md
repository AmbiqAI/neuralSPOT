

# File ns\_ble.h

[**File List**](files.md) **>** [**includes-api**](dir_4e48286ed5350b74cbe32681da7dff10.md) **>** [**ns\_ble.h**](ns__ble_8h.md)

[Go to the documentation of this file](ns__ble_8h.md)

```C++


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
    #define NS_BLE_V0_0_1                                                                          \
        { .major = 0, .minor = 0, .revision = 1 }

    #define NS_BLE_OLDEST_SUPPORTED_VERSION NS_BLE_V0_0_1
    #define NS_BLE_CURRENT_VERSION NS_BLE_V0_0_1
    #define NS_BLE_API_ID 0xCA0008

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

    // #define NS_BLE_ATT_UUID_BASE
    //     0x2E, 0xC7, 0x8A, 0x0E, 0x73, 0x90, 0xE1, 0x11, 0xC2, 0x08, 0x60, 0x27, 0x00, 0x00
    // #define NS_BLE_ATT_UUID_BUILD(part) UINT16_TO_BYTES(part), NS_BLE_ATT_UUID_BASE

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

typedef struct {
    appDbHdl_t dbHdl; 
    uint8_t addrType; 
    bdAddr_t addr;    
    bool_t doConnect; 
} ns_ble_connection_t;

typedef struct ns_ble_service_control {
    // State
    wsfHandlerId_t handlerId;        
    bool_t scanning;                 
    bool_t autoConnect;              
    uint8_t discState;               
    uint8_t hdlListLen;              
    ns_ble_connection_t *connection; 
    // Service Config
    // WSF Service Metadata
    attsGroup_t *group;   
    attsAttr_t *list;     
    attsCccSet_t *cccSet; 
    uint32_t cccCount;    
    // Advertising Data
    uint8_t *advData;    
    uint8_t advDataLen;  
    uint8_t *scanData;   
    uint8_t scanDataLen; 
    // Service callbacks
    // attsReadCback_t read_cb; /*! Read callback */

    ns_ble_service_specific_handler_cb handler_cb; 
    ns_ble_service_specific_handler_init_cb handler_init_cb; 
    ns_ble_service_specific_init_cb service_init_cb;         
    ns_ble_service_specific_procMsg_cb procMsg_cb;           
    // WSF Buffers
    uint8_t wsfBufCount;     
    uint32_t *bufferPool;    
    uint32_t bufferPoolSize; 
    wsfBufPoolDesc_t
        *bufferDescriptors;         
    uint32_t bufferDescriptorsSize; 
} ns_ble_service_control_t;

typedef struct ns_ble_control {
    const ns_core_api_t *api; 

    // State
    // uint16_t hdlList[APP_DB_HDL_LIST_LEN]; /*! Cached handle list */ (needed?)
    wsfHandlerId_t handlerId; 
    bool_t scanning;          
    bool_t autoConnect;       
    uint8_t discState;        
    uint8_t hdlListLen;       
    uint16_t nextHandleId;    
    // Config
    bool secureConnections; 
    // WSF Config Structs
    appAdvCfg_t *advCfg;       
    appSlaveCfg_t *slaveCfg;   
    appSecCfg_t *secCfg;       
    appUpdateCfg_t *updateCfg; 
    smpCfg_t *smpCfg;          
    hciConnSpec_t *connCfg;    
    appDiscCfg_t *appDiscCfg;  
    appCfg_t *appCfg;          
    // Service Config
    ns_ble_service_control_t *service_config; 
    // New API ------
    ns_ble_service_t *services[NS_BLE_MAX_SERVICES]; 
    uint16_t numServices;                            
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
    ns_ble_uuid128_t uuid128;         
    char name[31];                    
    uint8_t nameLen;                  
    uint16_t baseHandle;              
    uint16_t numAttributes;           
    uint16_t numCharacteristics;      
    ns_ble_pool_config_t *poolConfig; 

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
    wsfTimer_t indicationTimer; 
    uint32_t indicationPeriod;  
    // Internals
    uint16_t handleId;

} ns_ble_characteristic_t;

enum {
    NS_BLE_READ = 1,
    NS_BLE_WRITE = 2,
    NS_BLE_NOTIFY = 4,
    NS_BLE_PROP_MAX,
};

extern void ns_ble_pre_init(void);

extern int ns_ble_create_service(ns_ble_service_t *s);

extern void ns_ble_send_value(ns_ble_characteristic_t *c, attEvt_t *pMsg);

extern int ns_ble_create_characteristic(
    ns_ble_characteristic_t *c, char const *uuidString, void *applicationValue,
    uint16_t valueLength, uint16_t properties, ns_ble_characteristic_read_handler_t readHandlerCb,
    ns_ble_characteristic_write_handler_t writeHandlerCb,
    ns_ble_characteristic_notify_handler_t notifyHandlerCb, uint16_t periodMs,
    uint16_t *attributeCount);

extern int ns_ble_add_characteristic(ns_ble_service_t *s, ns_ble_characteristic_t *c);

extern int ns_ble_char2uuid(const char uuidString[16], ns_ble_uuid128_t *uuid128);

extern int ns_ble_start_service(ns_ble_service_t *s);

    #ifdef __cplusplus
}
    #endif
#endif // NS_BLE

```

