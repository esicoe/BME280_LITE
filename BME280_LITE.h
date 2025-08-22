#ifndef BME280_LITE_H
#define BME280_LITE_H

#include <Arduino.h>
#include <Wire.h>

// BME280 REGISTERS
  #define CTRL_HUM    0xF2
  #define CTRL_MEAS   0xF4
  #define CONFIG      0xF5

  #define DIG_T1_LSB  0x88
  #define DIG_H2_LSB  0xE1

  #define PRESS_MSB     0xF7
  #define PRESS_LSB     0xF8
  #define PRESS_XLSB    0xF9
  #define TEMP_MSB      0xFA
  #define TEMP_LSB      0xFB 
  #define TEMP_XLSB     0xFC
  #define HUM_MSB       0xFD 
  #define HUM_LSB       0xFE 

// BME280 SETTINGS
  #define BME_H_SKIP 0
  #define BME_H_X1   1
  #define BME_H_X2   2
  #define BME_H_X4   3
  #define BME_H_X8   4
  #define BME_H_X16  5

  #define BME_T_SKIP 0
  #define BME_T_X1   1
  #define BME_T_X2   2
  #define BME_T_X4   3
  #define BME_T_X8   4
  #define BME_T_X16  5

  #define BME_P_SKIP 0
  #define BME_P_X1   1
  #define BME_P_X2   2
  #define BME_P_X4   3
  #define BME_P_X8   4
  #define BME_P_X16  5

  #define BME_SLEEP   0
  #define BME_FORCED1 1
  #define BME_FORCED2 2
  #define BME_NORMAL  3

  #define BME_TSB_0_5MS   0
  #define BME_TSB_62_5MS  1
  #define BME_TSB_125MS   2
  #define BME_TSB_250MS   3
  #define BME_TSB_500MS   4
  #define BME_TSB_1000MS  5
  #define BME_TSB_10MS    6
  #define BME_TSB_20MS    7

  #define BME_FILTER_OFF 0
  #define BME_FILTER_2   1
  #define BME_FILTER_4   2
  #define BME_FILTER_8   3
  #define BME_FILTER_16  4

struct BME_RegisterData {
  bool isValid;
  uint8_t data[32];
};

struct BME_SensorData {
  bool isValid;
  float data;
};

class BME280_LITE {
  public:
    bool begin(uint8_t BMEaddress, uint8_t BMEosrs_H, uint8_t BMEosrs_T, uint8_t BMEosrs_P, uint8_t BMEmode, uint8_t BMEstby, uint8_t BMEIIR);
    bool calibrate(uint8_t BMEaddress);
    BME_SensorData readTemperature(uint8_t BMEaddress);
    BME_SensorData readPressure(uint8_t BMEaddress);
    BME_SensorData readHumidity(uint8_t BMEaddress);
    BME_SensorData readAltitude(uint8_t BMEaddress, float seaLevelPressure);

  private:
    BME_RegisterData read(uint8_t BMEaddress, uint8_t BMEregAddress, uint8_t BMEnumBytes);
    bool write(uint8_t BMEaddress, uint8_t BMEregAddress, uint8_t BMEwriteData);

    uint16_t T1;
    int16_t T2, T3;
    uint16_t P1;
    int16_t P2, P3, P4, P5, P6, P7, P8, P9;
    uint8_t H1;
    int16_t H2;
    uint8_t H3;
    int16_t H4, H5;
    int8_t H6;
    int32_t t_fine;
};
#endif
