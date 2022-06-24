#include <Arduino.h>
#include <SPI.h>
#include <HardwareSerial.h>

// Adafruit BluefruitLE nRF51
// https://github.com/adafruit/Adafruit_BluefruitLE_nRF51
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BLEBattery.h"
#include "BlueDRO.h"

// LSI 
// https://github.com/siyka-au/LS7366
#include "LS7366.h"

constexpr auto kBufferSize = 128;   // Size of the read buffer for incoming data
constexpr auto kVerboseMode = false; // If set to 'true' enables debug output
constexpr auto kFactoryResetEnable = false;

constexpr auto kBluefruitSpiChipSel = 8;
constexpr auto kBluefruitSpiIrq = 7;
constexpr auto kBluefruitSpiReset = 4;
constexpr auto kVbatPin = A9;
constexpr auto kCounterChipSel = 11;

Adafruit_BluefruitLE_SPI ble(kBluefruitSpiChipSel, kBluefruitSpiIrq, kBluefruitSpiReset);
Adafruit_BLEBattery battery(ble);
BlueDRO dro(ble);
LS7366 counter(kCounterChipSel);
uint8_t lastBatteryPercentage = 0;
int32_t lastCount = 0;

// A small helper
void error(const __FlashStringHelper *err)
{
  Serial.println(err);
  while (1);
}

void setup(void)
{
  Serial.begin(115200);

  Serial.print(F("Initialising the Bluefruit LE module: "));
  if (!ble.begin(kVerboseMode)) {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if constexpr (kFactoryResetEnable) {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  ble.setDevName("BlueDRO");

  // Enable Battery and DRO service then reset Bluefruit
  battery.begin();
  dro.begin();

  ble.info();
  
  Serial.println(F("Initialising the LS7366R"));

  counter.write_mode_register_0(FILTER_1 | DISABLE_INDX | FREE_RUN | QUADRX4);
  counter.write_mode_register_1(NO_FLAGS | EN_CNTR | BYTE_4 );
  counter.clear_counter();
  counter.clear_status_register();

  Serial.println(F("Loading DRO data from NVM"));
  dro.loadData();

  Serial.println(F("Running"));
}

void loop(void)
{
  // Counter handling
  int32_t count = counter.read_counter();

  if (count != lastCount) {
    dro.update(count);
    
    //Serial.print(F("Count: "));
    //Serial.print(count);
    //Serial.print(F(" - Position: "));
    //Serial.println(dro.position());
  }

  lastCount = count;

  // Battery handling
  float vBatMeasured = analogRead(kVbatPin) * 2 * 3.3 / 1024;
  uint8_t batteryPercentage = 123 - 123 / (pow(1 + pow(vBatMeasured / 3.7, 80), 0.165));
  
  if (batteryPercentage > 100)
    batteryPercentage = 100;
    
  if (batteryPercentage < 0)
    batteryPercentage = 0;

  if (batteryPercentage != lastBatteryPercentage) {
    battery.update(batteryPercentage);
    
    //Serial.print(F("Battery Voltage: "));
    //Serial.print(vBatMeasured);
    //Serial.print(F(" - Battery %: "));
    //Serial.println(batteryPercentage);
  }

  lastBatteryPercentage = batteryPercentage;

  // Serial command interface for testing
  if (Serial.available()) {
    int z = Serial.read();
    char command = (char)z;
    switch (command) {       
      case 'C': {
        Serial.println(F("Clearing counter"));
        counter.clear_counter();
        break;
      }

      case 'd': {
        int32_t readValue;
        if (ble.readNVM(0, &readValue)) {
          Serial.print(F("Numerator (NVM) = "));
          Serial.println(readValue);
        } else {
          Serial.println(F("Failed to read numerator from NVM"));
        }
        break;
      }

      case 'e': {
        int32_t readValue;
        if (ble.readNVM(4, &readValue)) {
          Serial.print(F("Denominator (NVM) = "));
          Serial.println(readValue);
        } else {
          Serial.println(F("Failed to read denominator from NVM"));
        }
        break;
      }

      case 'f': {
        Serial.print(F("Offset = "));
        Serial.println(dro.offset());
        break;
      }

      case 'D': {
        constexpr uint16_t num = 5;
        if (ble.writeNVM(0, num)) {
          dro.numerator(num);
          Serial.println(F("Writing 5 to numerator in NVM"));
        } else {
          Serial.println(F("Failed to set numerator to 5 in NVM"));
        }
        break;
      }

      case 'E': {
        constexpr uint16_t denom = 10;
        if (ble.writeNVM(0, denom)) {
          dro.numerator(denom);
          Serial.println(F("Writing 10 to numerator in NVM"));
        } else {
          Serial.println(F("Failed to set numerator to 10 in NVM"));
        }
        break;
      }
  
      case 'n': {
        dro.position(125);
        Serial.println(F("Setting position to 125"));
        break;
      }

      case 'm': {
        dro.position(250);
        Serial.println(F("Setting position to 250"));
        break;
      }

      case 'x': {
        char buf[32];
        constexpr uint16_t buf_size = sizeof(buf);
        Serial.print(F("Device Name: "));
        if (ble.getDevName(buf, buf_size)) {
          Serial.println(buf);
        } else {
          Serial.println(F("Error"));
        }
        break;
      }

      case 'y': {
        Serial.print(F("Is connectable? "));
        Serial.println(ble.isConnectable() ? F("Yes") : F("No"));        
        break;
      }

      case 'z': {
        Serial.print(F("Deleting bonds... "));
        if (ble.deleteBonds()) {
          Serial.println(F("OK"));
        } else {
          Serial.println(F("Failed"));
        }
        
        break;
      }

      default: {
        Serial.print(command);
        Serial.println(F(" not supported"));
        break;
      }
    }
  }

  ble.update(100);

}
