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

#ifndef _SIYKA_BLE_DRO_H_
#define _SIYKA_BLE_DRO_H_

#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BLEGatt.h"

class Siyka_BLE_DRO
{
  private:
    static uint8_t serviceUuid[16];
    static uint8_t characteristicUuid0[16];
    static uint8_t characteristicUuid1[16];
    static uint8_t characteristicUuid2[16];
    static uint8_t characteristicUuid3[16];
    static uint8_t characteristicUuid4[16];

    uint8_t serviceId;
    uint8_t characteristicId0;
    uint8_t characteristicId1;
    uint8_t characteristicId2;
    uint8_t characteristicId3;
    uint8_t characteristicId4;

    struct data_t
    {
      float position;
      uint8_t units;
      uint8_t unused;
    } data;
    
    Adafruit_BLE& _ble;
    Adafruit_BLEGatt _gatt;

  public:
    Siyka_BLE_DRO(Adafruit_BLE& ble);

    bool begin(bool reset = true);
    bool stop (bool reset = true);

    bool update(float position);
};

#endif /* _SIYKA_BLE_DRO_H_ */
