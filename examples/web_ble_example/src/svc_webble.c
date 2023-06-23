

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************
#include "svc_webble.h"
#include "att_api.h"
#include "bstream.h"
#include "svc_cfg.h"
#include "svc_ch.h"
#include "wsf_trace.h"
#include "wsf_types.h"

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/* AMDTP service declaration */
static const uint8_t webbleSvc[] = {ATT_UUID_WEBBLE_SERVICE};
static const uint16_t webbleSvcLen = sizeof(webbleSvc);

// Charateristic Property Declaration
static const uint8_t webbleTemperatureCh[] = {
    ATT_PROP_READ,
    UINT16_TO_BYTES(WEBBLE_TEMPERATURE_HDL),
    ATT_UUID_WEBBLE_TEMPERATURE,
};
static const uint16_t webbleTemperatureChLen = sizeof(webbleTemperatureCh);
static const uint8_t webbleTemperatureChUuid[] = {ATT_UUID_WEBBLE_TEMPERATURE};
static float webbleTemperatureChData[] = {0.0};
static const uint16_t webbleTemperatureChDataLen = sizeof(webbleTemperatureChData);

static const uint8_t webbleHumidityCh[] = {
    ATT_PROP_READ,
    UINT16_TO_BYTES(WEBBLE_HUMIDITY_HDL),
    ATT_UUID_WEBBLE_HUMIDITY,
};
static const uint16_t webbleHumidityChLen = sizeof(webbleHumidityCh);
static const uint8_t webbleHumidityChUuid[] = {ATT_UUID_WEBBLE_HUMIDITY};
static uint16_t webbleHumidityChData[] = {0};
static const uint16_t webbleHumidityChDataLen = sizeof(webbleHumidityChData);

static const uint8_t webblePressureCh[] = {
    ATT_PROP_READ,
    UINT16_TO_BYTES(WEBBLE_PRESSURE_HDL),
    ATT_UUID_WEBBLE_PRESSURE,
};
static const uint16_t webblePressureChLen = sizeof(webblePressureCh);
static const uint8_t webblePressureChUuid[] = {ATT_UUID_WEBBLE_PRESSURE};
static float webblePressureChData[] = {0.0};
static const uint16_t webblePressureChDataLen = sizeof(webblePressureChData);

static const uint8_t webbleAccelerometerCh[] = {
    ATT_PROP_READ | ATT_PROP_NOTIFY,
    UINT16_TO_BYTES(WEBBLE_ACCELEROMETER_HDL),
    ATT_UUID_WEBBLE_ACCELEROMETER,
};
static const uint16_t webbleAccelerometerChLen = sizeof(webbleAccelerometerCh);
static const uint8_t webbleAccelerometerChUuid[] = {ATT_UUID_WEBBLE_ACCELEROMETER};
static float webbleAccelerometerChData[] = {0.0, 0.0, 0.0};
static const uint16_t webbleAccelerometerChDataLen = sizeof(webbleAccelerometerChData);
static uint8_t webbleAccelerometerChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t webbleAccelerometerChCccLen = sizeof(webbleAccelerometerChCcc);

static const uint8_t webbleGyroscopeCh[] = {
    ATT_PROP_READ | ATT_PROP_NOTIFY,
    UINT16_TO_BYTES(WEBBLE_GYROSCOPE_HDL),
    ATT_UUID_WEBBLE_GYROSCOPE,
};
static const uint16_t webbleGyroscopeChLen = sizeof(webbleGyroscopeCh);
static const uint8_t webbleGyroscopeChUuid[] = {ATT_UUID_WEBBLE_GYROSCOPE};
static float webbleGyroscopeChData[] = {0.0, 0.0, 0.0};
static const uint16_t webbleGyroscopeChDataLen = sizeof(webbleGyroscopeChData);
static uint8_t webbleGyroscopeChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t webbleGyroscopeChCccLen = sizeof(webbleGyroscopeChCcc);

static const uint8_t webbleQuaternionCh[] = {
    ATT_PROP_READ | ATT_PROP_NOTIFY,
    UINT16_TO_BYTES(WEBBLE_QUATERNION_HDL),
    ATT_UUID_WEBBLE_QUATERNION,
};
static const uint16_t webbleQuaternionChLen = sizeof(webbleQuaternionCh);
static const uint8_t webbleQuaternionChUuid[] = {ATT_UUID_WEBBLE_QUATERNION};
static float webbleQuaternionChData[] = {0.0, 0.0, 0.0, 0.0};
static const uint16_t webbleQuaternionChDataLen = sizeof(webbleQuaternionChData);
static uint8_t webbleQuaternionChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t webbleQuaternionChCccLen = sizeof(webbleQuaternionChCcc);

static const uint8_t webbleRgbLedCh[] = {
    ATT_PROP_READ | ATT_PROP_WRITE,
    UINT16_TO_BYTES(WEBBLE_RGBLED_HDL),
    ATT_UUID_WEBBLE_RGBLED,
};
static const uint16_t webbleRgbLedChLen = sizeof(webbleRgbLedCh);
static const uint8_t webbleRgbLedChUuid[] = {ATT_UUID_WEBBLE_RGBLED};
static uint8_t webbleRgbLedChData[] = {0, 0, 0};
static const uint16_t webbleRgbLedChDataLen = sizeof(webbleRgbLedChData);

static const uint8_t webbleBsecCh[] = {
    ATT_PROP_READ,
    UINT16_TO_BYTES(WEBBLE_BSEC_HDL),
    ATT_UUID_WEBBLE_BSEC,
};
static const uint16_t webbleBsecChLen = sizeof(webbleBsecCh);
static const uint8_t webbleBsecChUuid[] = {ATT_UUID_WEBBLE_BSEC};
static float webbleBsecChData[] = {0.0};
static const uint16_t webbleBsecChDataLen = sizeof(webbleBsecChData);

static const uint8_t webbleCo2Ch[] = {
    ATT_PROP_READ,
    UINT16_TO_BYTES(WEBBLE_CO2_HDL),
    ATT_UUID_WEBBLE_CO2,
};
static const uint16_t webbleCo2ChLen = sizeof(webbleCo2Ch);
static const uint8_t webbleCo2ChUuid[] = {ATT_UUID_WEBBLE_CO2};
static int16_t webbleCo2ChData[] = {0};
static const uint16_t webbleCo2ChDataLen = sizeof(webbleCo2ChData);

static const uint8_t webbleGasCh[] = {
    ATT_PROP_READ,
    UINT16_TO_BYTES(WEBBLE_GAS_HDL),
    ATT_UUID_WEBBLE_GAS,
};
static const uint16_t webbleGasChLen = sizeof(webbleGasCh);
static const uint8_t webbleGasChUuid[] = {ATT_UUID_WEBBLE_GAS};
static uint16_t webbleGasChData[] = {0};
static const uint16_t webbleGasChDataLen = sizeof(webbleGasChData);

// Build the attribute list
static const attsAttr_t webbleAttributeList[] = {
    {
        .pUuid = attPrimSvcUuid,
        .pValue = (uint8_t *)webbleSvc,
        .pLen = (uint16_t *)&webbleSvcLen,
        .maxLen = sizeof(webbleSvc),
        .settings = 0,
        .permissions = ATTS_PERMIT_READ,
    },
    /* Temperature, Humidity, and Pressure are read-only characteristics */
    NS_BLE_CHAR_DECL(webbleTemperature),
    NS_BLE_CHAR_VAL(
        webbleTemperature, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_READ_CBACK),
        ATTS_PERMIT_READ),

    NS_BLE_CHAR_DECL(webbleHumidity),
    NS_BLE_CHAR_VAL(
        webbleHumidity, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_READ_CBACK),
        ATTS_PERMIT_READ),

    NS_BLE_CHAR_DECL(webblePressure),
    NS_BLE_CHAR_VAL(
        webblePressure, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_READ_CBACK),
        ATTS_PERMIT_READ),

    /* Accelerometer, Gyroscope and Quaternion are read with Notify (so we declare CCC too)*/
    NS_BLE_CHAR_DECL(webbleAccelerometer),
    NS_BLE_CHAR_VAL(
        webbleAccelerometer, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN), ATTS_PERMIT_READ),
    NS_BLE_CCC_DECL(webbleAccelerometer),

    NS_BLE_CHAR_DECL(webbleGyroscope),
    NS_BLE_CHAR_VAL(webbleGyroscope, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN), ATTS_PERMIT_READ),
    NS_BLE_CCC_DECL(webbleGyroscope),

    NS_BLE_CHAR_DECL(webbleQuaternion),
    NS_BLE_CHAR_VAL(
        webbleQuaternion, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN), ATTS_PERMIT_READ),
    NS_BLE_CCC_DECL(webbleQuaternion),

    /* RGB is read/write */
    NS_BLE_CHAR_DECL(webbleRgbLed),
    NS_BLE_CHAR_VAL(
        webbleRgbLed, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_READ_CBACK),
        (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)),

    /* BSEC, CO2 and Gas read-only */
    NS_BLE_CHAR_DECL(webbleBsec),
    NS_BLE_CHAR_VAL(
        webbleBsec, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_READ_CBACK),
        ATTS_PERMIT_READ),

    NS_BLE_CHAR_DECL(webbleCo2),
    NS_BLE_CHAR_VAL(
        webbleCo2, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_READ_CBACK),
        ATTS_PERMIT_READ),

    NS_BLE_CHAR_DECL(webbleGas),
    NS_BLE_CHAR_VAL(
        webbleGas, (ATTS_SET_UUID_128 | ATTS_SET_VARIABLE_LEN | ATTS_SET_READ_CBACK),
        ATTS_PERMIT_READ),
};

static attsGroup_t webbleGroup = {
    NULL, (attsAttr_t *)webbleAttributeList, NULL, NULL, WEBBLE_START_HDL, WEBBLE_END_HDL};

/*************************************************************************************************/
/*!
 *  \fn     SvcWebbleAddGroup
 *
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcWebbleAddGroup(void) { AttsAddGroup(&webbleGroup); }

/*************************************************************************************************/
/*!
 *  \fn     SvcWebbleRemoveGroup
 *
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcWebbleRemoveGroup(void) { AttsRemoveGroup(WEBBLE_START_HDL); }

/*************************************************************************************************/
/*!
 *  \fn     SvcWebbleCbackRegister
 *
 *  \brief  Register callbacks for the service.
 *
 *  \param  readCback   Read callback function.
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcWebbleCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback) {
    webbleGroup.readCback = readCback;
    webbleGroup.writeCback = writeCback;
}
