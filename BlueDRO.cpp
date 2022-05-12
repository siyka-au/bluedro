#include "BlueDRO.h"

/******************************************************************************/
/*!
    @brief Constructor
*/
/******************************************************************************/

uint8_t BlueDRO::serviceUuid[16]                    = {0xd7, 0x57, 0x8c, 0xaf, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::positionCharacteristicUuid[16]     = {0xd7, 0x57, 0xfc, 0xb0, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::numeratorCharacteristicUuid[16]    = {0xd7, 0x57, 0xfc, 0xb1, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::denominatorCharacteristicUuid[16]  = {0xd7, 0x57, 0xfc, 0xb2, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::diameterModeCharacteristicUuid[16] = {0xd7, 0x57, 0xfc, 0xb3, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::reverseCharacteristicUuid[16]      = {0xd7, 0x57, 0xfc, 0xb4, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::setPositionCharacteristicUuid[16]  = {0xd7, 0x57, 0xfc, 0xb5, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};

BlueDRO::BlueDRO(Adafruit_BLE& ble) : _ble(ble), _gatt(ble) {
  position = 0;
  numerator = 1;
  denominator = 1;
  diameterMode = false;
  reverse = false;
  int32_t storedNumerator;
  int32_t storedDenominator;
  
//  if (_ble.readNVM(0, storedNumerator)) {
//    if (storedNumerator != 0) {
//      numerator = storedNumerator;
//    } else {
//      _ble.writeNVM(0, numerator);
//    }
//  }
//
//  if (_ble.readNVM(4, storedDenominator)) {
//    if (storedDenominator != 0) {
//      denominator = storedDenominator;
//    } else {
//      _ble.writeNVM(4, denominator);
//    }
//  }
}

/******************************************************************************/
/*!
    @brief Enable DRO service if not already enabled
    @param reset true will reset Bluefruit
*/
/******************************************************************************/
bool BlueDRO::begin(bool reset) {
  serviceId = _gatt.addService(serviceUuid);

  struct GattPresentationFormat millimetrePresentationFormat = {
    GATT_PRESENT_FORMAT_SINT32,     // presentation format
    -6,                             // exponent
    GATT_PRESENT_UNIT_LENGTH_METRE, // unit
    0x01,                           // namespace, Bluetooth SIG
    0x0000                          // Description enumerator
  };

  struct GattPresentationFormat scalingFactorPresentationFormat = {
    GATT_PRESENT_FORMAT_SINT32,     // presentation format
    0,                              // exponent
    GATT_PRESENT_UNIT_NONE,         // unit
    0x01,                           // namespace, Bluetooth SIG
    0x0000                          // Description enumerator
  };

  struct GattPresentationFormat booleanPresentationFormat = {
    GATT_PRESENT_FORMAT_BOOLEAN,    // presentation format
    0,                              // exponent
    GATT_PRESENT_UNIT_NONE,         // unit
    0x01,                           // namespace, Bluetooth SIG
    0x0000                          // Description enumerator
  };
  
  positionCharacteristicId     = _gatt.addCharacteristic(positionCharacteristicUuid,      GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_NOTIFY, 4, 4, BLE_DATATYPE_BYTEARRAY, "Current position", &millimetrePresentationFormat);
  numeratorCharacteristicId    = _gatt.addCharacteristic(numeratorCharacteristicUuid,     GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE,  4, 4, BLE_DATATYPE_BYTEARRAY, "Scaling numerator", &scalingFactorPresentationFormat);
  denominatorCharacteristicId  = _gatt.addCharacteristic(denominatorCharacteristicUuid,   GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE,  4, 4, BLE_DATATYPE_BYTEARRAY, "Scaling denominator", &scalingFactorPresentationFormat);
  diameterModeCharacteristicId = _gatt.addCharacteristic(diameterModeCharacteristicUuid,  GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE,  1, 1, BLE_DATATYPE_INTEGER,   "Diameter mode", &booleanPresentationFormat);
  reverseCharacteristicId      = _gatt.addCharacteristic(reverseCharacteristicUuid,       GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE,  1, 1, BLE_DATATYPE_INTEGER,   "Reverse direction", &booleanPresentationFormat);
  setPositionCharacteristicId  = _gatt.addCharacteristic(setPositionCharacteristicUuid,   GATT_CHARS_PROPERTIES_WRITE,                               4, 4, BLE_DATATYPE_BYTEARRAY, "Arbitrarily set position", &millimetrePresentationFormat);  
  
  // Perform Bluefruit reset if needed
  if (reset) _ble.reset();

  _gatt.setChar(positionCharacteristicId, position);
  _gatt.setChar(numeratorCharacteristicId, numerator);
  _gatt.setChar(denominatorCharacteristicId, denominator);
  _gatt.setChar(diameterModeCharacteristicId, diameterMode);
  _gatt.setChar(reverseCharacteristicId, reverse);

  return true;
}

/******************************************************************************/
/*!
    @brief Stop DRO service if it is enabled
    @param reset true will reset Bluefruit
*/
/******************************************************************************/
bool BlueDRO::end(bool reset) {
  // Perform Bluefruit reset if needed
  if (reset) _ble.reset();

  return true;
}

/******************************************************************************/
/*!
    @brief Update DRO position value
    @param position Position value in millimetres
*/
/******************************************************************************/
bool BlueDRO::update(int32_t counter) {
  position = counter * numerator / denominator;
//  if (diameterMode) position *= 2;
  return _gatt.setChar(positionCharacteristicId, position);
}
