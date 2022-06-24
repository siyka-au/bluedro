#include "BlueDRO.h"

uint8_t BlueDRO::serviceUuid[16]                    = {0xd7, 0x57, 0x8c, 0xaf, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::positionCharacteristicUuid[16]     = {0xd7, 0x57, 0xfc, 0xb0, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::numeratorCharacteristicUuid[16]    = {0xd7, 0x57, 0xfc, 0xb1, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::denominatorCharacteristicUuid[16]  = {0xd7, 0x57, 0xfc, 0xb2, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::reverseCharacteristicUuid[16]      = {0xd7, 0x57, 0xfc, 0xb3, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t BlueDRO::setPositionCharacteristicUuid[16]  = {0xd7, 0x57, 0xfc, 0xb4, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};

constexpr uint16_t numeratorNVMOffset = 0;
constexpr uint16_t denominatorNVMOffset = 4;

/******************************************************************************/
/*!
    @brief Constructor
*/
/******************************************************************************/
BlueDRO::BlueDRO(Adafruit_BLE& ble) : _ble(ble), _gatt(ble)
{
  position_ = 0;
  numerator_ = 1;
  denominator_ = 1;
  offset_ = 0;
  reverse_ = false;
  direction_factor_ = 1;
}

/******************************************************************************/
/*!
    @brief Load data from NVM
*/
/******************************************************************************/
void BlueDRO::loadData()
{
  int32_t storedValue;
  
  if (_ble.readNVM(0, &storedValue)) {
    if (storedValue != 0) {
      numerator_ = storedValue;
    } else {
      _ble.writeNVM(numeratorNVMOffset, numerator_);
    }
  }

  if (_ble.readNVM(4, &storedValue)) {
    if (storedValue != 0) {
      denominator_ = storedValue;
    } else {
      _ble.writeNVM(denominatorNVMOffset, denominator_);
    }
  }
}

/******************************************************************************/
/*!
    @brief Enable DRO service if not already enabled
    @param reset true will reset Bluefruit
*/
/******************************************************************************/
bool BlueDRO::begin(bool reset)
{
  _gatt.clear();
  serviceId = _gatt.addService(serviceUuid);

  struct GattPresentationFormat millimetrePresentationFormat = {
    GATT_PRESENT_FORMAT_SINT32,     // presentation format
    -6,                             // exponent
    GATT_PRESENT_UNIT_LENGTH_METRE, // unit
    0x01,                           // namespace, Bluetooth SIG
    0x0000                          // Description enumerator
  };

  struct GattPresentationFormat scalingFactorPresentationFormat = {
    GATT_PRESENT_FORMAT_UINT16,     // presentation format
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
  
  positionCharacteristicId    = _gatt.addCharacteristic(positionCharacteristicUuid,    GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_NOTIFY, 4, 4, BLE_DATATYPE_INTEGER, "Current position", &millimetrePresentationFormat);
  numeratorCharacteristicId   = _gatt.addCharacteristic(numeratorCharacteristicUuid,   GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE,  2, 2, BLE_DATATYPE_INTEGER, "Scaling numerator", &scalingFactorPresentationFormat);
  denominatorCharacteristicId = _gatt.addCharacteristic(denominatorCharacteristicUuid, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE,  2, 2, BLE_DATATYPE_INTEGER, "Scaling denominator", &scalingFactorPresentationFormat);
  reverseCharacteristicId     = _gatt.addCharacteristic(reverseCharacteristicUuid,     GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE,  1, 1, BLE_DATATYPE_INTEGER, "Reverse direction", &booleanPresentationFormat);
  setPositionCharacteristicId = _gatt.addCharacteristic(setPositionCharacteristicUuid, GATT_CHARS_PROPERTIES_WRITE,                               4, 4, BLE_DATATYPE_INTEGER, "Arbitrarily set position", &millimetrePresentationFormat);  
  
  // Perform Bluefruit reset if needed
  if (reset) _ble.reset();

  _gatt.setChar(positionCharacteristicId, position_);
  _gatt.setChar(numeratorCharacteristicId, numerator_);
  _gatt.setChar(denominatorCharacteristicId, denominator_);
  _gatt.setChar(reverseCharacteristicId, reverse_);

  _ble.setBleGattRxListener(numeratorCharacteristicId, this);
  _ble.setBleGattRxListener(denominatorCharacteristicId, this);
  _ble.setBleGattRxListener(reverseCharacteristicId, this);
  _ble.setBleGattRxListener(setPositionCharacteristicId, this);

  return true;
}

/******************************************************************************/
/*!
    @brief Stop DRO service if it is enabled
    @param reset true will reset Bluefruit
*/
/******************************************************************************/
bool BlueDRO::end(bool reset)
{
  // Perform Bluefruit reset if needed
  if (reset) _ble.reset();

  return true;
}

/******************************************************************************/
/*!
    @brief Update DRO position value
    @param counter Encoder counts
*/
/******************************************************************************/
bool BlueDRO::update(int32_t counter)
{
  counter_ = counter;
  updatePosition();
}

/******************************************************************************/
/*!
    @brief Set the position
    @param position position
*/
/******************************************************************************/
void BlueDRO::position(int32_t position)
{
  offset_ = position - position_ + offset_;
  updatePosition();
}

void BlueDRO::updatePosition(void)
{
  position_ = counter_ * direction_factor_ * numerator_ / denominator_ + offset_;
  _gatt.setChar(positionCharacteristicId, position_);
}

/******************************************************************************/
/*!
    @brief Set the scaling numerator and save it to NVM
    @param numerator numerator
*/
/******************************************************************************/
void BlueDRO::numerator(uint16_t numerator)
{
  if (numerator > 0) {
    numerator_ = numerator;
    _ble.writeNVM(numeratorNVMOffset, numerator_);
    _gatt.setChar(numeratorCharacteristicId, numerator_);
    updatePosition();
  }
}

/******************************************************************************/
/*!
    @brief Set the scaling denominator and save it to NVM
    @param denominator denominator
*/
/******************************************************************************/
void BlueDRO::denominator(uint16_t denominator)
{
  if (denominator > 0) {
    denominator_ = denominator;
    _ble.writeNVM(denominatorNVMOffset, denominator_);
    _gatt.setChar(denominatorCharacteristicId, denominator_);
    updatePosition();
  }
}

/******************************************************************************/
/*!
    @brief Reverses the direction
*/
/******************************************************************************/
void BlueDRO::reverseDirection(void)
{
  reverse_ = !reverse_;
  _gatt.setChar(reverseCharacteristicId, reverse_);
  direction_factor_ = reverse_ ? -1 : 1;
  updatePosition();
}

/******************************************************************************/
/*!
    @brief Called on BLE connection
*/
/******************************************************************************/
void BlueDRO::onBLEConnect()
{
  
}

/******************************************************************************/
/*!
    @brief Called on BLE disconnection
*/
/******************************************************************************/
void BlueDRO::onBLEDisconnect()
{

}

/******************************************************************************/
/*!
    @brief Enable DRO service if not already enabled
    @param characteristicId characteristic index of the received data
    @param data the received data
    @param dataLength the length of received data
*/
/******************************************************************************/
void BlueDRO::onBLEGattRx(int32_t characteristicId, uint8_t data[], uint16_t dataLength)
{
  Serial.print(F("[BLE GATT RX] (" ));
  Serial.print(characteristicId);
  Serial.println(")");

  if (characteristicId == this->numeratorCharacteristicId)
  {
  }
  else if (characteristicId == this->denominatorCharacteristicId)
  {
  }
  else if (characteristicId == this->reverseCharacteristicId)
  {
    reverseDirection();
    Serial.print("Setting direction = ");
    Serial.println(reverse_);
  }
  else if (characteristicId == this->setPositionCharacteristicId)
  {
    int32_t setPosition = _gatt.getCharInt32(setPositionCharacteristicId);
    this->position(setPosition);
    Serial.print("Setting position = ");
    Serial.println(setPosition);
  }
}
