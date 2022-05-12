/**************************************************************************/
/*!
    @file     Adafruit_BLEBatterry.h
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

#ifndef _BLUEDRO_
#define _BLUEDRO_

#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BLEGatt.h"

class BlueDRO {
  private:
    static uint8_t serviceUuid[16];
    static uint8_t positionCharacteristicUuid[16];
    static uint8_t numeratorCharacteristicUuid[16];
    static uint8_t denominatorCharacteristicUuid[16];
    static uint8_t diameterModeCharacteristicUuid[16];
    static uint8_t reverseCharacteristicUuid[16];
    static uint8_t setPositionCharacteristicUuid[16];

    uint8_t serviceId;
    uint8_t positionCharacteristicId;
    uint8_t numeratorCharacteristicId;
    uint8_t denominatorCharacteristicId;
    uint8_t diameterModeCharacteristicId;
    uint8_t reverseCharacteristicId;
    uint8_t setPositionCharacteristicId;
    
    Adafruit_BLE& _ble;
    Adafruit_BLEGatt _gatt;

  public:
    BlueDRO(Adafruit_BLE& ble);

    bool begin(bool reset = true);
    bool end(bool reset = true);

    bool update(int32_t counter);

    int32_t position;
    int32_t numerator;
    int32_t denominator;
    bool diameterMode;
    bool reverse;
};

#endif /* _BLUEDRO_ */
