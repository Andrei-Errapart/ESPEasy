#include "../PluginStructs/P152_data_struct.h"

#ifdef USES_P152


// INA232 Register Addresses
# define INA232_REG_CONFIG                      (0x00)
# define INA232_REG_SHUNTVOLTAGE                (0x01)
# define INA232_REG_BUSVOLTAGE                  (0x02)
# define INA232_REG_POWER                       (0x03)
# define INA232_REG_CURRENT                     (0x04)
# define INA232_REG_CALIBRATION                 (0x05)
# define INA232_REG_MASK_ENABLE                 (0x06)
# define INA232_REG_ALERT_LIMIT                 (0x07)
# define INA232_REG_MANUFACTURER_ID             (0x3E)
# define INA232_REG_DIE_ID                      (0x3F)

// INA232 Configuration Register Bits
# define INA232_CONFIG_RESET                    (0x8000) // Reset Bit

# define INA232_CONFIG_ADCRANGE_MASK            (0x4000) // ADC Range Mask
# define INA232_CONFIG_ADCRANGE_163_84MV        (0x0000) // ±163.84 mV (actually ±81.92mV per datasheet)
# define INA232_CONFIG_ADCRANGE_40_96MV         (0x4000) // ±40.96 mV (actually ±20.48mV per datasheet)

# define INA232_CONFIG_AVG_MASK                 (0x0E00) // Averaging Mode Mask
# define INA232_CONFIG_AVG_1                    (0x0000) // 1 sample
# define INA232_CONFIG_AVG_4                    (0x0200) // 4 samples
# define INA232_CONFIG_AVG_16                   (0x0400) // 16 samples
# define INA232_CONFIG_AVG_64                   (0x0600) // 64 samples
# define INA232_CONFIG_AVG_128                  (0x0800) // 128 samples
# define INA232_CONFIG_AVG_256                  (0x0A00) // 256 samples
# define INA232_CONFIG_AVG_512                  (0x0C00) // 512 samples
# define INA232_CONFIG_AVG_1024                 (0x0E00) // 1024 samples

# define INA232_CONFIG_VBUSCT_MASK              (0x01C0) // Bus Voltage Conversion Time Mask
# define INA232_CONFIG_VBUSCT_140US             (0x0000) // 140 µs
# define INA232_CONFIG_VBUSCT_204US             (0x0040) // 204 µs
# define INA232_CONFIG_VBUSCT_332US             (0x0080) // 332 µs
# define INA232_CONFIG_VBUSCT_588US             (0x00C0) // 588 µs
# define INA232_CONFIG_VBUSCT_1100US            (0x0100) // 1.1 ms
# define INA232_CONFIG_VBUSCT_2116US            (0x0140) // 2.116 ms
# define INA232_CONFIG_VBUSCT_4156US            (0x0180) // 4.156 ms
# define INA232_CONFIG_VBUSCT_8244US            (0x01C0) // 8.244 ms

# define INA232_CONFIG_VSHCT_MASK               (0x0038) // Shunt Voltage Conversion Time Mask
# define INA232_CONFIG_VSHCT_140US              (0x0000) // 140 µs
# define INA232_CONFIG_VSHCT_204US              (0x0008) // 204 µs
# define INA232_CONFIG_VSHCT_332US              (0x0010) // 332 µs
# define INA232_CONFIG_VSHCT_588US              (0x0018) // 588 µs
# define INA232_CONFIG_VSHCT_1100US             (0x0020) // 1.1 ms
# define INA232_CONFIG_VSHCT_2116US             (0x0028) // 2.116 ms
# define INA232_CONFIG_VSHCT_4156US             (0x0030) // 4.156 ms
# define INA232_CONFIG_VSHCT_8244US             (0x0038) // 8.244 ms

# define INA232_CONFIG_MODE_MASK                (0x0007) // Operating Mode Mask
# define INA232_CONFIG_MODE_SHUTDOWN            (0x0000) // Shutdown
# define INA232_CONFIG_MODE_SHUNT_TRIG          (0x0001) // Shunt voltage, triggered
# define INA232_CONFIG_MODE_BUS_TRIG            (0x0002) // Bus voltage, triggered
# define INA232_CONFIG_MODE_SHUNT_BUS_TRIG      (0x0003) // Shunt and bus, triggered
# define INA232_CONFIG_MODE_TEMP_TRIG           (0x0004) // Temperature, triggered
# define INA232_CONFIG_MODE_SHUNT_CONT          (0x0005) // Shunt voltage, continuous
# define INA232_CONFIG_MODE_BUS_CONT            (0x0006) // Bus voltage, continuous
# define INA232_CONFIG_MODE_SHUNT_BUS_CONT      (0x0007) // Shunt and bus, continuous


P152_data_struct::P152_data_struct(uint8_t i2c_addr) : i2caddr(i2c_addr) {}

void P152_data_struct::setCalibration_48V_5A() {
  // For 5A max current with 0.01 ohm shunt resistor
  // Max shunt voltage: 5A * 0.01Ω = 50mV
  // Use ±81.92mV range (ADCRANGE=0), LSB = 2.5 µV
  // Current LSB = max current / 2^15 = 5A / 32768 = 152.59 µA
  // We'll use 200 µA for easier calculation
  // Calibration = 0.00512 / (Current_LSB * Rshunt)
  // Calibration = 0.00512 / (0.0002 * 0.01) = 2560

  currentLSB_mA = 0.2f;  // 200 µA = 0.2 mA
  shuntVoltageLSB_uV = 2.5f;  // 2.5 µV for ±81.92mV range
  powerLSB_mW = 3.2f * currentLSB_mA;  // Power LSB = 3.2 * Current LSB

  calValue = 2560;

  // Set Calibration register
  wireWriteRegister(INA232_REG_CALIBRATION, calValue);

  // Set Config register
  // ±81.92mV range, 128 averages, 1.1ms conversion time for both, continuous mode
  uint16_t config = INA232_CONFIG_ADCRANGE_163_84MV |
                    INA232_CONFIG_AVG_128 |
                    INA232_CONFIG_VBUSCT_1100US |
                    INA232_CONFIG_VSHCT_1100US |
                    INA232_CONFIG_MODE_SHUNT_BUS_CONT;

  wireWriteRegister(INA232_REG_CONFIG, config);
}

void P152_data_struct::setCalibration_48V_2A() {
  // For 2A max current with 0.01 ohm shunt resistor
  // Max shunt voltage: 2A * 0.01Ω = 20mV
  // Use ±20.48mV range (ADCRANGE=1), LSB = 0.3125 µV
  // Current LSB = 2A / 32768 = 61.04 µA
  // We'll use 100 µA for easier calculation
  // Calibration = 0.00512 / (Current_LSB * Rshunt)
  // Calibration = 0.00512 / (0.0001 * 0.01) = 5120

  currentLSB_mA = 0.1f;  // 100 µA = 0.1 mA
  shuntVoltageLSB_uV = 0.3125f;  // 0.3125 µV for ±20.48mV range
  powerLSB_mW = 3.2f * currentLSB_mA;  // Power LSB = 3.2 * Current LSB

  calValue = 5120;

  // Set Calibration register
  wireWriteRegister(INA232_REG_CALIBRATION, calValue);

  // Set Config register
  // ±20.48mV range, 128 averages, 1.1ms conversion time for both, continuous mode
  uint16_t config = INA232_CONFIG_ADCRANGE_40_96MV |
                    INA232_CONFIG_AVG_128 |
                    INA232_CONFIG_VBUSCT_1100US |
                    INA232_CONFIG_VSHCT_1100US |
                    INA232_CONFIG_MODE_SHUNT_BUS_CONT;

  wireWriteRegister(INA232_REG_CONFIG, config);
}

void P152_data_struct::setCalibration_24V_5A() {
  // Same as 48V_5A in terms of current measurement
  // Just different voltage range, but INA232 handles up to 48V anyway
  setCalibration_48V_5A();
}

void P152_data_struct::setCalibration_12V_5A() {
  // Same as 48V_5A in terms of current measurement
  setCalibration_48V_5A();
}

int16_t P152_data_struct::getBusVoltage_raw() {
  uint16_t value;
  wireReadRegister(INA232_REG_BUSVOLTAGE, &value);
  return (int16_t)value;
}

int16_t P152_data_struct::getShuntVoltage_raw() {
  uint16_t value;
  wireReadRegister(INA232_REG_SHUNTVOLTAGE, &value);
  return (int16_t)value;
}

int16_t P152_data_struct::getCurrent_raw() {
  uint16_t value;

  // Sometimes a sharp load will reset the INA232, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  wireWriteRegister(INA232_REG_CALIBRATION, calValue);

  // Now we can safely read the CURRENT register!
  wireReadRegister(INA232_REG_CURRENT, &value);

  return (int16_t)value;
}

uint32_t P152_data_struct::getPower_raw() {
  uint32_t value;
  wireReadRegister24(INA232_REG_POWER, &value);
  return value;
}

float P152_data_struct::getShuntVoltage_mV() {
  int16_t value = getShuntVoltage_raw();
  // Convert to mV using the appropriate LSB (in µV)
  return value * shuntVoltageLSB_uV / 1000.0f;
}

float P152_data_struct::getBusVoltage_V() {
  int16_t value = getBusVoltage_raw();
  // INA232 bus voltage LSB = 1.25 mV
  return value * 0.00125f;
}

float P152_data_struct::getCurrent_mA() {
  int16_t value = getCurrent_raw();
  return value * currentLSB_mA;
}

float P152_data_struct::getPower_mW() {
  uint32_t value = getPower_raw();
  return value * powerLSB_mW;
}

void P152_data_struct::wireWriteRegister(uint8_t reg, uint16_t value)
{
  I2C_write16_reg(i2caddr, reg, value);
}

void P152_data_struct::wireReadRegister(uint8_t reg, uint16_t *value)
{
  Wire.beginTransmission(i2caddr);
  Wire.write(reg);
  Wire.endTransmission();

  delay(1);  // Max conversion time is ~8ms with averaging

  Wire.requestFrom(i2caddr, (uint8_t)2);

  // Shift values to create properly formed integer
  *value = ((Wire.read() << 8) | Wire.read());
}

void P152_data_struct::wireReadRegister24(uint8_t reg, uint32_t *value)
{
  Wire.beginTransmission(i2caddr);
  Wire.write(reg);
  Wire.endTransmission();

  delay(1);

  Wire.requestFrom(i2caddr, (uint8_t)3);

  // Read 24-bit value (3 bytes)
  *value = ((uint32_t)Wire.read() << 16) |
           ((uint32_t)Wire.read() << 8) |
           Wire.read();
}

#endif // ifdef USES_P152
