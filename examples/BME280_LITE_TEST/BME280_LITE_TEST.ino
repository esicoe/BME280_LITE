/*
  This is the test for the BME280_LITE library.
  This library only works with I2C. The device's address is 0x76 or 0x77.
  The objective of the library is to be as simple and as lightweight as possible.
  Version = 2.1.0 (added support for custom TwoWire objects for multiple I2C buses)
  
  Written by Edward Sicoe.
*/

#include <BME280_LITE.h>

#define BME_ADDR    0x76 // Primary address (0x77 is the alternate)
#define SEA_LEVEL_PRES  1017.8 // sea level pressure in hPa to use in the readAltitude function.

BME280_LITE bme;

void setup() {
  Serial.begin(9600);
  
  // Option 1: Use default Wire (backwards compatible)
  if (!bme.begin(BME_ADDR, 
                 BME_H_X1, 
                 BME_T_X1, 
                 BME_P_X16, 
                 BME_NORMAL, 
                 BME_TSB_0_5MS, 
                 BME_FILTER_2)) {
    Serial.println("BME280 initialization failed!");
    while(1);
  }
  
  /* Option 2: Use custom I2C pins (ESP32 example)
  #define SDA_PIN 21
  #define SCL_PIN 22
  
  Wire.begin(SDA_PIN, SCL_PIN);  // Initialize with custom pins
  
  if (!bme.begin(&Wire,          // Pass the Wire object
                 BME_ADDR, 
                 BME_H_X1, 
                 BME_T_X1, 
                 BME_P_X16, 
                 BME_NORMAL, 
                 BME_TSB_0_5MS, 
                 BME_FILTER_2)) {
    Serial.println("BME280 initialization failed!");
    while(1);
  }
  */
  
  if (!bme.calibrate(BME_ADDR)) {
    Serial.println("BME280 calibration failed!");
    while(1);
  }
  
  Serial.println("BME280 initialized successfully!");
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
