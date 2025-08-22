#include "BME280_LITE.h"

BME_RegisterData BME280_LITE::read(uint8_t BMEaddress, uint8_t BMEregAddress, uint8_t BMEnumBytes) {
  BME_RegisterData status = {false, {0}};

  #if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(10000, true);
  #endif

  Wire.beginTransmission(BMEaddress);
  Wire.write(BMEregAddress);
  if (Wire.endTransmission(false) != 0) return status;

  if (Wire.requestFrom(BMEaddress, BMEnumBytes, true) != BMEnumBytes) return status;

  for (uint8_t i = 0; i<BMEnumBytes; i++) {
    status.data[i] = Wire.read();
  }
  status.isValid = true;
  return status;
}

bool BME280_LITE::write(uint8_t BMEaddress, uint8_t BMEregAddress, uint8_t BMEwriteData) {
  #if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(10000, true);
  #endif

  Wire.beginTransmission(BMEaddress);
  Wire.write(BMEregAddress);
  Wire.write(BMEwriteData);
  if (Wire.endTransmission() != 0) return false;
  return true; 
}

bool BME280_LITE::begin(uint8_t BMEaddress, uint8_t BMEosrs_H, uint8_t BMEosrs_T, uint8_t BMEosrs_P, uint8_t BMEmode, uint8_t BMEstby, uint8_t BMEIIR) {
  Wire.begin();

  if (write(BMEaddress, CTRL_HUM, BMEosrs_H) && 
      write(BMEaddress, CTRL_MEAS, (BMEosrs_T << 5) | (BMEosrs_P << 2) | BMEmode) &&
      write(BMEaddress, CONFIG, (BMEstby << 5) | (BMEIIR << 2))) {
        return true;
      }
  return false;
}

bool BME280_LITE::calibrate(uint8_t BMEaddress) {
  BME_RegisterData Calib1 = read(BMEaddress, DIG_T1_LSB, 26);
  if (!Calib1.isValid) return false;

  T1 = (uint16_t)(Calib1.data[1] << 8 | Calib1.data[0]);
  T2 = (int16_t)(Calib1.data[3] << 8 | Calib1.data[2]);
  T3 = (int16_t)(Calib1.data[5] << 8 | Calib1.data[4]);

  P1 = (uint16_t)(Calib1.data[7]  << 8 | Calib1.data[6]);
  P2 = (int16_t)(Calib1.data[9]  << 8 | Calib1.data[8]);
  P3 = (int16_t)(Calib1.data[11] << 8 | Calib1.data[10]);
  P4 = (int16_t)(Calib1.data[13] << 8 | Calib1.data[12]);
  P5 = (int16_t)(Calib1.data[15] << 8 | Calib1.data[14]);
  P6 = (int16_t)(Calib1.data[17] << 8 | Calib1.data[16]);
  P7 = (int16_t)(Calib1.data[19] << 8 | Calib1.data[18]);
  P8 = (int16_t)(Calib1.data[21] << 8 | Calib1.data[20]);
  P9 = (int16_t)(Calib1.data[23] << 8 | Calib1.data[22]);

  H1 = Calib1.data[25];

  BME_RegisterData Calib2 = read(BMEaddress, DIG_H2_LSB, 8);
  if (!Calib2.isValid) return false;
  
  H2 = (int16_t)(Calib2.data[1] << 8 | Calib2.data[0]);
  H3 = Calib2.data[2];
  H4 = (int16_t)((Calib2.data[3] << 4) | (Calib2.data[4] & 0x0F));
  H5 = (int16_t)((Calib2.data[5] << 4) | (Calib2.data[4] >> 4));
  H6 = (int8_t)Calib2.data[6];

  return true;
}

BME_SensorData BME280_LITE::readTemperature(uint8_t BMEaddress) {
  BME_SensorData result = {false, 0.0f};

  BME_RegisterData raw = read(BMEaddress, TEMP_MSB, 3);
  if (!raw.isValid) return result;

  int32_t adc_T = ((int32_t)raw.data[0] << 12) |
                  ((int32_t)raw.data[1] << 4)  |
                  ((raw.data[2] >> 4) & 0x0F);

  int32_t var1 = ((((adc_T >> 3) - ((int32_t)T1 << 1))) * ((int32_t)T2)) >> 11;
  int32_t var2 = (((((adc_T >> 4) - (int32_t)T1) *
                    ((adc_T >> 4) - (int32_t)T1)) >> 12) *
                    (int32_t)T3) >> 14;

  t_fine = var1 + var2;

  int32_t temp_int = (t_fine * 5 + 128) >> 8;

  result.isValid = true;
  result.data = (float)temp_int / 100.0f;

  return result;
}

BME_SensorData BME280_LITE::readPressure(uint8_t BMEaddress) {
  BME_SensorData result = {false, 0.0f};

  BME_RegisterData tfine = read(BMEaddress, TEMP_MSB, 3);
  if (!tfine.isValid) return result;

  int32_t adc_T = ((int32_t)tfine.data[0] << 12) |
                  ((int32_t)tfine.data[1] << 4)  |
                  (tfine.data[2] >> 4);

  int32_t var1 = ((((adc_T >> 3) - ((int32_t)T1 << 1))) * ((int32_t)T2)) >> 11;
  int32_t var2 = (((((adc_T >> 4) - (int32_t)T1) *
                    ((adc_T >> 4) - (int32_t)T1)) >> 12) *
                    (int32_t)T3) >> 14;
  t_fine = var1 + var2;

  BME_RegisterData raw = read(BMEaddress, PRESS_MSB, 3);
  if (!raw.isValid) return result;

  int32_t adc_P = ((int32_t)raw.data[0] << 12) |
                  ((int32_t)raw.data[1] << 4)  |
                  (raw.data[2] >> 4);

  int64_t var1_64 = (int64_t)t_fine - 128000;
  int64_t var2_64 = var1_64 * var1_64 * (int64_t)P6;
  var2_64 = var2_64 + ((var1_64 * (int64_t)P5) << 17);
  var2_64 = var2_64 + (((int64_t)P4) << 35);
  var1_64 = ((var1_64 * var1_64 * (int64_t)P3) >> 8) + ((var1_64 * (int64_t)P2) << 12);
  var1_64 = ((((int64_t)1 << 47) + var1_64) * (int64_t)P1) >> 33;

  if (var1_64 == 0) return result;

  int64_t p = 1048576 - adc_P;
  p = (((p << 31) - var2_64) * 3125) / var1_64;
  var1_64 = ((int64_t)P9 * (p >> 13) * (p >> 13)) >> 25;
  var2_64 = ((int64_t)P8 * p) >> 19;
  p = ((p + var1_64 + var2_64) >> 8) + (((int64_t)P7) << 4);

  result.isValid = true;
  result.data = (float)p / 25600.0f;

  return result;
}

BME_SensorData BME280_LITE::readHumidity(uint8_t BMEaddress) {
  BME_SensorData result = {false, 0.0f};

  BME_RegisterData tfine = read(BMEaddress, TEMP_MSB, 3);
  if (!tfine.isValid) return result;

  int32_t adc_T = ((int32_t)tfine.data[0] << 12) |
                  ((int32_t)tfine.data[1] << 4)  |
                  (tfine.data[2] >> 4);

  int32_t var1 = ((((adc_T >> 3) - ((int32_t)T1 << 1))) * ((int32_t)T2)) >> 11;
  int32_t var2 = (((((adc_T >> 4) - (int32_t)T1) *
                    ((adc_T >> 4) - (int32_t)T1)) >> 12) *
                    (int32_t)T3) >> 14;
  t_fine = var1 + var2;

  BME_RegisterData raw = read(BMEaddress, HUM_MSB, 2);
  if (!raw.isValid) return result;

  int32_t adc_H = ((int32_t)raw.data[0] << 8) | raw.data[1];

  int32_t v_x1_u32r = t_fine - 76800;
  int32_t part1 = adc_H << 14;
  int32_t part2 = (int32_t)H4 << 20;
  int32_t part3 = (int32_t)H5 * v_x1_u32r;
  v_x1_u32r = (part1 - part2 - part3 + 16384) >> 15;

  var1 = (v_x1_u32r * (int32_t)H6) >> 10;
  var2 = (v_x1_u32r * (int32_t)H3) >> 11;
  var1 = (var1 * (var2 + 32768)) >> 10;
  var1 = (var1 + 2097152) * (int32_t)H2 + 8192;
  var1 >>= 14;

  v_x1_u32r = v_x1_u32r * var1;
  int32_t adjust = (v_x1_u32r >> 15) * (v_x1_u32r >> 15) >> 7;
  adjust = (adjust * (int32_t)H1) >> 4;
  v_x1_u32r -= adjust;

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;

  result.isValid = true;
  result.data = (float)(v_x1_u32r >> 12) / 1024.0f;

  return result;
}

BME_SensorData BME280_LITE::readAltitude(uint8_t BMEaddress, float seaLevelPressure) {
  BME_SensorData pressure = readPressure(BMEaddress);
  if (!pressure.isValid) return pressure;

  BME_SensorData result = {true, 0.0f};
  result.data = 44330.0f * (1.0f - powf(pressure.data / seaLevelPressure, 0.1903f));
  return result;
}
