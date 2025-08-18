#ifndef BME280_LITE_H
#define BME280_LITE_H

#include <Arduino.h>
#include <Wire.h>

// TELEMETRY REGISTERS -- READ
  #define PRESS_MSB     0xF7
  #define PRESS_LSB     0xF8
  #define PRESS_XLSB    0xF9
  #define TEMP_MSB      0xFA
  #define TEMP_LSB      0xFB 
  #define TEMP_XLSB     0xFC
  #define HUM_MSB       0xFD 
  #define HUM_LSB       0xFE 

// CONTROL REGISTERS -- READ && WRITE
  #define CTRL_HUM      0xF2 
  #define CTRL_MEAS     0xF4 
  #define CONFIG        0xF5  

// DIGITAL CALIBRATION REGISTERS -- READ
  #define DIG_T1_LSB    0x88  
  #define DIG_T1_MSB    0x89 
  #define DIG_T2_LSB    0x8A 
  #define DIG_T2_MSB    0x8B  
  #define DIG_T3_LSB    0x8C 
  #define DIG_T3_MSB    0x8D 

  #define DIG_P1_LSB    0x8E 
  #define DIG_P1_MSB    0x8F 
  #define DIG_P2_LSB    0x90 
  #define DIG_P2_MSB    0x91 
  #define DIG_P3_LSB    0x92 
  #define DIG_P3_MSB    0x93
  #define DIG_P4_LSB    0x94
  #define DIG_P4_MSB    0x95
  #define DIG_P5_LSB    0x96
  #define DIG_P5_MSB    0x97 
  #define DIG_P6_LSB    0x98
  #define DIG_P6_MSB    0x99
  #define DIG_P7_LSB    0x9A
  #define DIG_P7_MSB    0x9B
  #define DIG_P8_LSB    0x9C
  #define DIG_P8_MSB    0x9D
  #define DIG_P9_LSB    0x9E
  #define DIG_P9_MSB    0x9F

  #define DIG_H1        0xA1
  #define DIG_H2_LSB    0xE1
  #define DIG_H2_MSB    0xE2
  #define DIG_H3        0xE3
  #define DIG_H4_H5_1   0xE4  
  #define DIG_H4_H5_2   0xE5  
  #define DIG_H5_MSB    0xE6
  #define DIG_H6        0xE7

enum bme280_settings : uint8_t {
  BME_H_SKIP = 0,
  BME_H_X1   = 1,
  BME_H_X2   = 2,
  BME_H_X4   = 3,
  BME_H_X8   = 4,
  BME_H_X16  = 5,  

  // CTRL_MEAS
  BME_T_SKIP      = 0, 
  BME_T_X1        = 1, 
  BME_T_X2        = 2, 
  BME_T_X4        = 3, 
  BME_T_X8        = 4, 
  BME_T_X16       = 5, 
  BME_P_SKIP      = 0, 
  BME_P_X1        = 1, 
  BME_P_X2        = 2, 
  BME_P_X4        = 3, 
  BME_P_X8        = 4, 
  BME_P_X16       = 5, 

  // CONFIG
  BME_SLEEP       = 0, 
  BME_FORCED1     = 1, 
  BME_FORCED2     = 2, 
  BME_NORMAL      = 3, 
  BME_TSB_0_5MS   = 0, 
  BME_TSB_62_5MS  = 1, 
  BME_TSB_125MS   = 2, 
  BME_TSB_250MS   = 3, 
  BME_TSB_500MS   = 4, 
  BME_TSB_1000MS  = 5, 
  BME_TSB_10MS    = 6, 
  BME_TSB_20MS    = 7, 
  BME_FILTER_OFF  = 0, 
  BME_FILTER_2    = 1, 
  BME_FILTER_4    = 2, 
  BME_FILTER_8    = 3, 
  BME_FILTER_16   = 4, 
};

class BME280_LITE {
  public:
    bool initialize(uint8_t dev_addr, uint8_t osrs_H, uint8_t osrs_T, uint8_t osrs_P, uint8_t bme280_mode, uint8_t bme280_stby, uint8_t bme280_iir);
    void calibrate(uint8_t dev_addr);
    float readTemperature(uint8_t dev_addr);
    float readPressure(uint8_t dev_addr);
    float readHumidity(uint8_t dev_addr);
    float readAltitude(uint8_t dev_addr, float current_sea_level_pressure);

  private:
    bool write(uint8_t dev_addr, uint8_t reg_addr, uint8_t reg_val);
    void read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_buf, uint8_t read_len);

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