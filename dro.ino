/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include "ls7366r.h"
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BLEBattery.h"
#include "Siyka_BLE_DRO.h"
#include "BluefruitConfig.h"

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
    #define FACTORYRESET_ENABLE      1

    #define ENCODER_CHIPSEL 11
/*=========================================================================*/

const ls7366r_init_t encoder_config =
{
    /* mode 1 */
    LS7366R_MODE1_QUAD_X4 |
    LS7366R_MODE1_COUNTER_FREERUN |
    LS7366R_MODE1_INDEX_RESET_COUNTER |
    LS7366R_MODE1_FCLK_DIV1 |
    LS7366R_MODE1_INDEXSYNC_SYNC,

    /* mode 2 */
    LS7366R_MODE2_CTRLEN_4B |
    LS7366R_MODE2_CTR_ENABLE |
    LS7366R_MODE2_FLAG_NONE
};

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Adafruit_BLEBattery battery(ble);
Siyka_BLE_DRO dro(ble);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
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

  // Enable Battery service and reset Bluefruit
  battery.begin();
  dro.begin(false);

  ble.reset();

    pinMode(ENCODER_CHIPSEL, OUTPUT);
  digitalWrite(ENCODER_CHIPSEL, HIGH);

  Serial.print(F("Initialising the LS7366R"));
  ls7366r_init(ENCODER_CHIPSEL, &encoder_config);
//  SPI.begin();
  battery.update(50);
}

uint32_t counterVal = 0;

void loop(void) {
  counterVal = ls7366r_get_counter_4b(ENCODER_CHIPSEL);
  Serial.print("Position read = ");
  Serial.println(counterVal);
  
  dro.update(counterVal);
  delay(500);
}

inline void _ls7366r_chip_sel( uint8_t chip_sel ) {
  digitalWrite(chip_sel, LOW); 
}

inline void _ls7366r_chip_desel( uint8_t chip_sel ) {
  digitalWrite(chip_sel, HIGH); 
}

inline uint8_t _ls7366r_spi_transfer( uint8_t chip_sel, uint8_t out ) {
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  uint8_t s = SPI.transfer(out);
  SPI.endTransaction();
  return s;
}
