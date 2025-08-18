#include "BME280_LITE.h"

bool BME280_LITE::write(uint8_t dev_addr, uint8_t reg_addr, uint8_t reg_val) {
  Wire.setWireTimeout(10000, true);

  Wire.beginTransmission(dev_addr);
  Wire.write(reg_addr);
  Wire.write(reg_val);
  uint8_t result = Wire.endTransmission();

  if (result == 0) {
    return true;
  }
  return false;
}

void BME280_LITE::read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_buf, uint8_t read_len) {
  Wire.setWireTimeout(10000, true);

  Wire.beginTransmission(dev_addr);
  Wire.write(reg_addr);
  Wire.endTransmission(false);

  Wire.requestFrom(dev_addr, read_len);
  for (uint8_t i = 0; i < read_len; i++) {
    data_buf[i] = Wire.read();
  }
}

bool BME280_LITE::initialize(uint8_t dev_addr, uint8_t osrs_H, uint8_t osrs_T, uint8_t osrs_P, uint8_t bme280_mode, uint8_t bme280_stby, uint8_t bme280_iir) {
  Wire.begin();

  uint8_t valCTRL_MEAS = ((osrs_T << 5) | (osrs_P << 2) | bme280_mode);
  uint8_t valCONFIG = ((bme280_stby << 5) | (bme280_iir << 2) | 00);
  
  if ((write(dev_addr, CTRL_HUM, osrs_H)) && (write(dev_addr, CTRL_MEAS, valCTRL_MEAS)) && (write(dev_addr, CONFIG, valCONFIG))) {
    return true;
  }
  return false;
}

void BME280_LITE::calibrate(uint8_t dev_addr) {
  uint8_t data[24];
  
  read(dev_addr, 0x88, data, 6);
  T1 = (uint16_t)(data[1] << 8 | data[0]);
  T2 = (int16_t)(data[3] << 8 | data[2]);
  T3 = (int16_t)(data[5] << 8 | data[4]);

  read(dev_addr, 0x8E, data, 18);
  P1 = (uint16_t)(data[1] << 8 | data[0]);
  P2 = (int16_t)(data[3] << 8 | data[2]);
  P3 = (int16_t)(data[5] << 8 | data[4]);
  P4 = (int16_t)(data[7] << 8 | data[6]);
  P5 = (int16_t)(data[9] << 8 | data[8]);
  P6 = (int16_t)(data[11] << 8 | data[10]);
  P7 = (int16_t)(data[13] << 8 | data[12]);
  P8 = (int16_t)(data[15] << 8 | data[14]);
  P9 = (int16_t)(data[17] << 8 | data[16]);

  read(dev_addr, 0xA1, data, 1);
  H1 = data[0];
  
  read(dev_addr, 0xE1, data, 7);
  H2 = (int16_t)(data[1] << 8 | data[0]);
  H3 = data[2];
  
  H4 = (int16_t)(((int8_t)data[3] << 4) | (data[4] & 0x0F));
  H5 = (int16_t)(((int8_t)data[5] << 4) | (data[4] >> 4));
  
  H6 = (int8_t)data[6];
}

float BME280_LITE::readTemperature(uint8_t dev_addr) {
  uint8_t data[3];

  read(dev_addr, TEMP_MSB, data, 3);

  int32_t adc_T = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);

  int32_t var1 = ((((adc_T >> 3) - (T1 << 1))) * ((int32_t)T2)) >> 11;
  int32_t var2 = ((((adc_T >> 4) - T1) * ((adc_T >> 4) - (int32_t)T1)) >> 12) * (((int32_t)T3) >> 14);
  
  t_fine = var1 + var2;

  int32_t temp_int = (t_fine * 5 + 128) >> 8;
  
  return (float)temp_int / 100.0f;
}

float BME280_LITE::readPressure(uint8_t dev_addr) {
  uint8_t data[3];
  read(dev_addr, PRESS_MSB, data, 3);
  
  int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
  
  int64_t var1, var2, p;
  
  var1 = (int64_t)t_fine - 128000;
  var2 = var1 * var1 * (int64_t)P6;
  var2 = var2 + ((var1 * (int64_t)P5) << 17);
  var2 = var2 + (((int64_t)P4) << 35);
  var1 = ((var1 * var1 * (int64_t)P3) >> 8) + ((var1 * (int64_t)P2) << 12);
  var1 = ((((int64_t)1 << 47) + var1) * (int64_t)P1) >> 33;

  if (var1 == 0) {
      return 0.0f;
  }
  
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = ((int64_t)P9 * (p >> 13) * (p >> 13)) >> 25;
  var2 = ((int64_t)P8 * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)P7) << 4);
  
  return (float)p / 25600.0f;
}

float BME280_LITE::readHumidity(uint8_t dev_addr) {
    uint8_t data[2];
    read(dev_addr, HUM_MSB, data, 2);
    int32_t adc_H = (data[0] << 8) | data[1];
    
    int32_t v_x1_u32r = t_fine - 76800;
    
    int32_t part1 = adc_H << 14;
    int32_t part2 = (int32_t)H4 << 20;
    int32_t part3 = (int32_t)H5 * v_x1_u32r;
    v_x1_u32r = (part1 - part2 - part3 + 16384);
    v_x1_u32r >>= 15;

    int32_t var1 = (v_x1_u32r * (int32_t)H6) >> 10;
    int32_t var2 = (v_x1_u32r * (int32_t)H3) >> 11;
    var1 = var1 * (var2 + 32768);
    var1 >>= 10;
    var1 += 2097152;
    var1 *= (int32_t)H2;
    var1 += 8192;
    var1 >>= 14;

    v_x1_u32r *= var1;

    int32_t adjust = v_x1_u32r >> 15;
    adjust *= adjust;
    adjust >>= 7;
    adjust *= (int32_t)H1;
    adjust >>= 4;
    
    v_x1_u32r -= adjust;

    if (v_x1_u32r < 0) v_x1_u32r = 0;
    if (v_x1_u32r > 419430400) v_x1_u32r = 419430400;

    return (float)(v_x1_u32r >> 12) / 1024.0f;
}

float BME280_LITE::readAltitude(uint8_t dev_addr, float current_sea_level_pressure) {
  float pressure = readPressure(dev_addr);
  return 44330.0f * (1.0f - powf(pressure / current_sea_level_pressure, 0.1903f));
}