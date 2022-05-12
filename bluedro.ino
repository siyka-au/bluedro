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
#include "/home/malleblas/workspaces/siyka/LS7366/LS7366.h"

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE     Perform a factory reset when running this sketch
   
                            Enabling this will put your Bluefruit LE module
                            in a 'known good' state and clear any config
                            data set in previous sketches or projects, so
                            running this at least once is a good idea.
   
                            When deploying your project, however, you will
                            want to disable factory reset by setting this
                            value to 0.  If you are making changes to your
                            Bluefruit LE device via AT commands, and those
                            changes aren't persisting across resets, this
                            is the reason why.  Factory reset will erase
                            the non-volatile memory where config data is
                            stored, setting it back to factory default
                            values.
       
                            Some sketches that require you to bond to a
                            central device (HID mouse, keyboard, etc.)
                            won't work at all with this feature enabled
                            since the factory reset will clear all of the
                            bonding data stored on the chip, meaning the
                            central device won't be able to reconnect.
-----------------------------------------------------------------------*/
#define BUFSIZE                        128   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   false  // If set to 'true' enables debug output
#define FACTORYRESET_ENABLE            1

#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4    // Optional but recommended, set to -1 if unused
#define VBATPIN A9
#define COUNTER_CHIPSEL 11

/*=========================================================================*/

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Adafruit_BLEBattery battery(ble);
BlueDRO dro(ble);

LS7366 counter(COUNTER_CHIPSEL);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void connected(void) {
  Serial.println(F("BLE Connected"));
}

void disconnected(void)
{
  Serial.println(F("BLE Disconnected"));
}

void BleGattRX(int32_t chars_id, uint8_t data[], uint16_t len)
{
  Serial.print(F("[BLE GATT RX] (" ));
  Serial.print(chars_id);
  Serial.println(")");
  
//  int32_t val;
//  memcpy(&val, data, len);
//  Serial.println(val);
}

void setup(void) {
  Serial.begin(115200);

  Serial.print(F("Initialising the Bluefruit LE module: "));
  if ( !ble.begin(VERBOSE_MODE) ) {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE ) {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  // Enable Battery and DRO service then reset Bluefruit
  battery.begin();
  dro.begin();
  ble.reset();

  ble.info();
  
  /* Set callbacks */
  ble.setConnectCallback(connected);
  ble.setDisconnectCallback(disconnected);
  ble.setBleGattRxCallback(1, BleGattRX);
  ble.setBleGattRxCallback(2, BleGattRX);
  ble.setBleGattRxCallback(3, BleGattRX);
  ble.setBleGattRxCallback(4, BleGattRX);

  Serial.println(F("Initialising the LS7366R"));

  counter.write_mode_register_0(FILTER_1 | DISABLE_INDX | FREE_RUN | QUADRX4);
  counter.write_mode_register_1(NO_FLAGS | EN_CNTR | BYTE_4 );
  counter.clear_counter();
  counter.clear_status_register();

  Serial.println(F("Running"));
}

int32_t lastCount = 0;

void loop(void) {
  int32_t count = counter.read_counter();

  if (count != lastCount) {
    Serial.print("Count = ");
    Serial.print(count);

    dro.update(count);

    Serial.print(" - Position = ");
    Serial.println(dro.position);
  }

  lastCount = count;
  
  float vBatMeasured = analogRead(VBATPIN) * 2 * 3.3 / 1024;
  float batteryPercentage = 123 - 123 / (pow(1 + pow(vBatMeasured / 3.7, 80), 0.165));
  
  if ( batteryPercentage > 100)
    batteryPercentage = 100;
    
  if (batteryPercentage < 0)
    batteryPercentage = 0;
  
//  Serial.print("Vbat: " );
//  Serial.println(vBatMeasured);
//  Serial.print("Battery %: " );
//  Serial.println(batteryPercentage);
  battery.update(batteryPercentage);

  // Serial command interface for testing
  if (Serial.available()) {
    char command = (char)Serial.read();
    switch (command) {      
      case 'c':
        Serial.println("Clearing counter");
        counter.clear_counter();
        break;

      case 'd':
        if (ble.writeNVM(0, 1)) {
          dro.numerator = 1;
          Serial.println("Writing 1 to numerator in NVM");
        } else {
          Serial.println("Failed to set numerator to 1 in NVM");
        }
        break;

      case 'e':
        if (ble.writeNVM(0, 10)) {
          dro.numerator = 10;
          Serial.println("Writing 10 to numerator in NVM");
        } else {
          Serial.println("Failed to set numerator to 10 in NVM");
        }
        break;

      case 'f':
        int32_t nvmContent;
        if (ble.readNVM(0, &nvmContent)) {
          Serial.print("NVM = ");
          Serial.println(nvmContent);
        } else {
          Serial.println("Failed to read NVM");
        }
        break;
  
      case 'm':
        Serial.println("Setting counter to 1000");
        counter.write_data_register(1000);
        counter.load_counter();
        break;

      case 'o':
        uint32_t otr = counter.read_OTR();
        Serial.print("OTR: ");
        Serial.println(otr);
        break;
        
      default:
        Serial.print(command);
        Serial.println(" not supported");
        break;
    }
  }

  ble.update(200);
  
}