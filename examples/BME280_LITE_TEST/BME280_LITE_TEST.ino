/*
  This is the test for the BME280_LITE library.
  This library only works with I2C. The device's address is 0x76 or 0x77.
  The objective of the library is to be as simple and as lightweight as possible.
  
  Written by Edward Sicoe.
*/

#include <Wire.h>
#include <BME280_LITE.h>

#define BME_ADDR 0x76 // Default BME280 I2C address.
#define BME_XADDR 0x77 // Auxilliary BME280 I2C address, useful if you have two of these.
#define PRESSURE_AT_SEA_LEVEL_hPa 1013.25 // Add the sea level pressure in your location. Improves accuracy big time.

BME280_LITE bme; // Constructor for the BME280-LITE class.

void setup() {
  Serial.begin(9600);
  bool status = bme.initialize(BME_ADDR, // Specified I2C address from the preprocessor directive
                BME_H_X1, // Oversampling for humidity. Check docs for more info.
                BME_T_X1, // Oversampling for temperature. Check docs for more info.
                BME_P_X2, // Oversampling for pressure. Check docs for more info.
                BME_NORMAL, // Operating mode for the device. Check docs for more info.
                BME_TSB_0_5MS, // Standby time, available only in normal mode. Check docs for more info.
                BME_FILTER_2); // IIR filter coefficient. Check docs for more info.

  if (!status) {
  Serial.println("FAILED BME280 INITALIZATION"); // bme.initalize retuns a bool type given it's success.
  // FALSE = failed to initalize, TRUE = I2C OK.
  }

  bme.calibrate(BME_ADDR); // Assigns important calibration values from the BME280. More info in docs. Returns nothing.
}

void loop() {
  float temperature = bme.readTemperature(BME_ADDR); // Temperature is in celsius (float).
  float humidity = bme.readHumidity(BME_ADDR); // Humidity in %RH (float).
  float pressure = bme.readPressure(BME_ADDR); // Pressure is in hPa (float).
  float altitude = bme.readAltitude(BME_ADDR, PRESSURE_AT_SEA_LEVEL_hPa); // Elevation is in meters (float).

  Serial.print("T: "); Serial.println(temperature);
  Serial.print("H: "); Serial.println(humidity);
  Serial.print("P: "); Serial.println(pressure);
  Serial.print("ALT: "); Serial.println(altitude);

  delay(1000); // 1s delay.
}
