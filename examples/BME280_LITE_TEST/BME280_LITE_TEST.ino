/*
  This is the test for the BME280_LITE library.
  This library only works with I2C. The device's address is 0x76 or 0x77.
  The objective of the library is to be as simple and as lightweight as possible.
  Version = 2.0.0 (changes include more error handling by setting function output as structs)
  
  Written by Edward Sicoe.
*/

#include <BME280_LITE.h>

#define BME_ADDR    0x76 // Primary address (0x77 is the alternate)
#define SEA_LEVEL_PRES  1017.8 // sea level pressure in hPa to use in the readAltitude funciton.

BME280_LITE bme;

void setup() {
  bme.begin(BME_ADDR, // returns a T/F based on initialization.
            BME_H_X1, // All settings can be found in the docs.
            BME_T_X1, 
            BME_P_X16, 
            BME_NORMAL, 
            BME_TSB_0_5MS, 
            BME_FILTER_2);
  bme.calibrate(BME_ADDR); // returns T/F based on calibration status. You can read it if you wish.
  Serial.begin(9600);
}

void loop() {
/* 
Note: all read functions return a struct with a "isValid" boolean followed by a float "data".
The struct BME_SensorData is used since a failed I2C transaction means that "isValid" is false.
Reading when isValid is false means the read function is simply reading noise from the pin and is outputing false data.
*/

  BME_SensorData temperature = bme.readTemperature(BME_ADDR); 
  if (!temperature.isValid) {
    Serial.println("TEMP READ FAIL");
  } else {
    Serial.print("T: ");
    Serial.println(temperature.data);
  }
  
  BME_SensorData pressure = bme.readPressure(BME_ADDR);
  if (!pressure.isValid) {
    Serial.println("PRESSURE READ FAIL");
  } else {
    Serial.print("P: ");
    Serial.println(pressure.data);
  }

  BME_SensorData humidity = bme.readHumidity(BME_ADDR);
  if (!humidity.isValid) {
    Serial.println("HUMIDITY READ FAIL");
  } else {
    Serial.print("H: ");
    Serial.println(humidity.data);
  }
  
  BME_SensorData altitude = bme.readAltitude(BME_ADDR, SEA_LEVEL_PRES);
  if (!altitude.isValid) {
    Serial.println("ALTITUDE READ FAIL");
  } else {
    Serial.print("ALT: ");
    Serial.println(altitude.data);
  }

  delay(1000);
}
