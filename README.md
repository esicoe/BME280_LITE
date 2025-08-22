# BME280_LITE Library

This is the BME280_LITE library, designed to be lightweight for memory. This library supports **I2C communication only**.

---

## Functions

### `begin()`

This function writes the registers in the BME280 responsible for the initialization of the sensor, and must be called once in `setup()`. The registers configured are:

- **CTRL_HUM** (humidity control)
- **CTRL_MEAS** (measure control) 
- **CONFIG** (configuration)

#### Configuration Options

The initialization function configures some settings like:

- **Oversampling**: Controls how many times a measurement is taken and averaged for the final result
- **Mode**: 
  - `forced` - runs once
  - `forced 2` - runs twice
  - `sleep` - sensor sleeps
  - `normal` - repeated sampling
- **Standby times**: The pause between measurements (available only in normal mode)
- **IIR filter**: Built-in noise reduction filter (useful for filtering environmental disturbances like opening windows or wind gusts)

> **Note**: Higher oversampling settings take more time but are still quite fast (under 10ms)

#### Parameters

```cpp
bme.initialize(BMEaddress, Humidity_oversampling, Temperature_oversampling, Pressure_oversampling, BME_mode, BME_standby, BME_IIR_filter)
```

| Parameter | Values | Description |
|-----------|--------|-------------|
| `BMEaddress` | `0x76` (primary), `0x77` (alternate) | I²C device address |
| `Humidity oversampling` | `BME_H_SKIP=0`, `BME_H_X1=1`, `BME_H_X2=2`, `BME_H_X4=3`, `BME_H_X8=4`, `BME_H_X16=5` | Humidity measurement oversampling |
| `Temperature oversampling` | `BME_T_SKIP=0`, `BME_T_X1=1`, `BME_T_X2=2`, `BME_T_X4=3`, `BME_T_X8=4`, `BME_T_X16=5` | Temperature measurement oversampling |
| `Pressure oversampling` | `BME_P_SKIP=0`, `BME_P_X1=1`, `BME_P_X2=2`, `BME_P_X4=3`, `BME_P_X8=4`, `BME_P_X16=5` | Pressure measurement oversampling |
| `BME mode` | `BME_SLEEP=0`, `BME_FORCED1=1`, `BME_FORCED2=2`, `BME_NORMAL=3` | Operating mode |
| `BME standby` | `BME_TSB_0_5MS=0`, `BME_TSB_62_5MS=1`, `BME_TSB_125MS=2`, `BME_TSB_250MS=3`, `BME_TSB_500MS=4`, `BME_TSB_1000MS=5`, `BME_TSB_10MS=6`, `BME_TSB_20MS=7` | Standby time between measurements |
| `BME IIR filter` | `BME_FILTER_OFF=0`, `BME_FILTER_2=1`, `BME_FILTER_4=2`, `BME_FILTER_8=3`, `BME_FILTER_16=4` | IIR filter coefficient |

**Returns**: `boolean` - `true` if initialization was successful, `false` otherwise

**Example**:
```cpp
bme.initialize(BME_ADDR, BME_H_X1, BME_T_X1, BME_P_X2, BME_NORMAL, BME_TSB_0_5MS, BME_FILTER_2);
```

---

### `calibrate()`

Reads the factory calibration registers. These registers contain fixed values written at the factory for each BME280 chip. This function must be called once in `setup()` to ensure proper measurements.

#### Parameters

```cpp
bme.calibrate(BMEaddress)
```

| Parameter | Values | Description |
|-----------|--------|-------------|
| `BMEaddress` | `0x76` (primary), `0x77` (alternate) | I²C device address |

**Returns**: `boolean` - `true` if initialization was successful, `false` otherwise

---

## Measurement Functions

### `readTemperature()`

Reads the temperature from the sensor.

#### Parameters

```cpp
bme.readTemperature(BMEaddress)
```

| Parameter | Values | Description |
|-----------|--------|-------------|
| `BMEaddress` | `0x76` (primary), `0x77` (alternate) | I²C device address |

**Returns**: Struct BME_SensorData where bool isValid t/f based on success and data `float` - Temperature in degrees Celsius

---

### `readHumidity()`

Reads the relative humidity from the sensor.

#### Parameters

```cpp
bme.readHumidity(BMEaddress)
```

| Parameter | Values | Description |
|-----------|--------|-------------|
| `BMEaddress` | `0x76` (primary), `0x77` (alternate) | I²C device address |

**Returns**: `float` - Relative humidity in %RH

---

### `readPressure()`

Reads the atmospheric pressure from the sensor.

#### Parameters

```cpp
bme.readPressure(BMEaddress)
```

| Parameter | Values | Description |
|-----------|--------|-------------|
| `BMEaddress` | `0x76` (primary), `0x77` (alternate) | I²C device address |

**Returns**: `float` - Atmospheric pressure in Pascals (Pa)

---

### `readAltitude()`

Calculates altitude based on atmospheric pressure readings.

#### Parameters

```cpp
bme.readAltitude(BMEaddress)
```

| Parameter | Values | Description |
|-----------|--------|-------------|
| `BMEaddress` | `0x76` (primary), `0x77` (alternate) | I²C device address |

**Returns**: `float` - Altitude in meters
