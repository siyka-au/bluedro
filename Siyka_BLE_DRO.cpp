/**************************************************************************/
/*!
    @file     Adafruit_BLEBatterry.cpp
    @author   hathach

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2016, Adafruit Industries (adafruit.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include "Siyka_BLE_DRO.h"

/******************************************************************************/
/*!
    @brief Constructor
*/
/******************************************************************************/

uint8_t Siyka_BLE_DRO::serviceUuid[16] = {0xd7, 0x57, 0x8c, 0xaf, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t Siyka_BLE_DRO::characteristicUuid0[16] = {0xd7, 0x57, 0xfc, 0xb0, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t Siyka_BLE_DRO::characteristicUuid1[16] = {0xd7, 0x57, 0xfc, 0xb1, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t Siyka_BLE_DRO::characteristicUuid2[16] = {0xd7, 0x57, 0xfc, 0xb2, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t Siyka_BLE_DRO::characteristicUuid3[16] = {0xd7, 0x57, 0xfc, 0xb3, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};
uint8_t Siyka_BLE_DRO::characteristicUuid4[16] = {0xd7, 0x57, 0xfc, 0xb4, 0x68, 0x6d, 0x42, 0x16, 0xba, 0x8e, 0xa3, 0x70, 0x3f, 0x15, 0x90, 0xfc};

Siyka_BLE_DRO::Siyka_BLE_DRO(Adafruit_BLE& ble) : _ble(ble), _gatt(ble)
{
  data.position = 0;
  data.units = 0;
  data.unused = 0;
}

/******************************************************************************/
/*!
    @brief Enable DRO service if not already enabled
    @param reset true will reset Bluefruit
*/
/******************************************************************************/
bool Siyka_BLE_DRO::begin(bool reset)
{
  serviceId = _gatt.addService(serviceUuid);
  characteristicId0 = _gatt.addCharacteristic(characteristicUuid0, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE | GATT_CHARS_PROPERTIES_NOTIFY, 6, 6, BLE_DATATYPE_BYTEARRAY);
  characteristicId1 = _gatt.addCharacteristic(characteristicUuid1, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE | GATT_CHARS_PROPERTIES_NOTIFY, 1, 1, BLE_DATATYPE_BYTEARRAY);
  characteristicId3 = _gatt.addCharacteristic(characteristicUuid3, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE | GATT_CHARS_PROPERTIES_NOTIFY, 1, 1, BLE_DATATYPE_BYTEARRAY);
  characteristicId2 = _gatt.addCharacteristic(characteristicUuid2, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE | GATT_CHARS_PROPERTIES_NOTIFY, 1, 1, BLE_DATATYPE_BYTEARRAY);  
  characteristicId4 = _gatt.addCharacteristic(characteristicUuid4, GATT_CHARS_PROPERTIES_READ | GATT_CHARS_PROPERTIES_WRITE | GATT_CHARS_PROPERTIES_NOTIFY, 1, 1, BLE_DATATYPE_BYTEARRAY);
  // Perform Bluefruit reset if needed
  if (reset) _ble.reset();

  _gatt.setChar(characteristicId3, (uint8_t) 8);

  return true;
}

/******************************************************************************/
/*!
    @brief Stop DRO service if it is enabled
    @param reset true will reset Bluefruit
*/
/******************************************************************************/
bool Siyka_BLE_DRO::stop(bool reset)
{
  // Perform Bluefruit reset if needed
  if (reset) _ble.reset();

  return true;
}

/******************************************************************************/
/*!
    @brief Update DRO position value
    @param percent Battery value in percentage 0-100
*/
/******************************************************************************/
bool Siyka_BLE_DRO::update(float position)
{
  this->data.position = position;
  _gatt.setChar(characteristicId0, (uint8_t*) &data, 6);
  return true;
}
