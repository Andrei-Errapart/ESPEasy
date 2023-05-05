#include "../PluginStructs/P027_data_struct.h"

#ifdef USES_P027


# define INA232_READ                            (0x01)
# define INA232_REG_CONFIG                      (0x00)
# define INA232_CONFIG_RESET                    (0x8000) // Reset Bit

# define INA232_CONFIG_ADCRANGE_MASK            (0x1000) // Bus Voltage Range Mask Bit 12
# define INA232_CONFIG_ADCRANGE_81              (0x0000) // 81.92 mV Range
# define INA232_CONFIG_ADCRANGE_20              (0x1000) // 20.48 mV Range

# define INA232_CONFIG_AVG_MASK                 (0x0E00) // Sets the number of ADC conversion results to be averaged. The readback registers are updated after averaging is completed
# define INA232_CONFIG_AVG_1                    (0x0000) // 000b = 1
# define INA232_CONFIG_AVG_4                    (0x0200) // 001b = 4
# define INA232_CONFIG_AVG_16                   (0x0400) // 010b = 16
# define INA232_CONFIG_AVG_64                   (0x0600) // 011b = 64
# define INA232_CONFIG_AVG_128                  (0x0800) // 100b = 128
# define INA232_CONFIG_AVG_256                  (0x0A00) // 101b = 256
# define INA232_CONFIG_AVG_512                  (0x0C00) // 110b = 512
# define INA232_CONFIG_AVG_1024                 (0x0E00) // 111b = 1024

# define INA232_CONFIG_VBUSCT_MASK              (0x01C0) // Sets the conversion time of the VBUS measurement
# define INA232_CONFIG_VBUSCT_140US             (0x0000) // 000b = 140 μs
# define INA232_CONFIG_VBUSCT_205US             (0x0040) // 001b = 204 μs
# define INA232_CONFIG_VBUSCT_332US             (0x0080) // 010b = 332 μs
# define INA232_CONFIG_VBUSCT_588US             (0x00C0) // 011b = 588 μs
# define INA232_CONFIG_VBUSCT_1100US            (0x0100) // 100b = 1100 μs
# define INA232_CONFIG_VBUSCT_2116US            (0x0140) // 101b = 2116 μs
# define INA232_CONFIG_VBUSCT_4156US            (0x0180) // 110b = 4156 μs
# define INA232_CONFIG_VBUSCT_8244US            (0x01C0) // 111b = 8244 μs

# define INA232_CONFIG_VSHCT_MASK               (0x0038) // Sets the conversion time of the SHUNT measurement
# define INA232_CONFIG_VSHCT_140US              (0x0000) // 000b = 140 μs
# define INA232_CONFIG_VSHCT_204US              (0x0008) // 001b = 204 μs
# define INA232_CONFIG_VSHCT_332US              (0x0010) // 010b = 332 μs
# define INA232_CONFIG_VSHCT_588US              (0x0018) // 011b = 588 μs
# define INA232_CONFIG_VSHCT_1100US             (0x0020) // 100b = 1100 μs
# define INA232_CONFIG_VSHCT_2116US             (0x0028) // 101b = 2116 μs
# define INA232_CONFIG_VSHCT_4156US             (0x0030) // 110b = 4156 μs
# define INA232_CONFIG_VSHCT_8244US             (0x0038) // 111b = 8244 μs

# define INA232_CONFIG_MODE_MASK                (0x0007) // Operating Mode Mask
# define INA232_CONFIG_MODE_POWERDOWN           (0x0000)
# define INA232_CONFIG_MODE_SVOLT_TRIGGERED     (0x0001) // 001b = Shunt Voltage triggered, single shot
# define INA232_CONFIG_MODE_BVOLT_TRIGGERED     (0x0002) // 010b = Bus Voltage triggered, single shot
# define INA232_CONFIG_MODE_SANDBVOLT_TRIGGERED (0x0003) // 011b = Shunt voltage and Bus voltage triggered, single shot
# define INA232_CONFIG_MODE_ADCOFF              (0x0004)
# define INA232_CONFIG_MODE_SVOLT_CONTINUOUS    (0x0005)
# define INA232_CONFIG_MODE_BVOLT_CONTINUOUS    (0x0006)
# define INA232_CONFIG_MODE_SANDBVOLT_CONTINUOUS (0x0007)

# define INA232_REG_SHUNTVOLTAGE                (0x01)
# define INA232_REG_BUSVOLTAGE                  (0x02)
# define INA232_REG_POWER                       (0x03)
# define INA232_REG_CURRENT                     (0x04)
# define INA232_REG_CALIBRATION                 (0x05)


P027_data_struct::P027_data_struct(uint8_t i2c_addr) : i2caddr(i2c_addr) {}

void P027_data_struct::setCalibration_32V_2A() {
  calValue = 4027;

  // Set multipliers to convert raw current/power values
  currentDivider_mA = 10; // Current LSB = 100uA per bit (1000/100 = 10)

  // Set Calibration register to 'Cal' calculated above
  wireWriteRegister(INA232_REG_CALIBRATION, calValue);

  // Set Config register to take into account the settings above
  uint16_t config = INA232_CONFIG_AVG_1 |
                    INA232_CONFIG_VBUSCT_1100US |
                    INA232_CONFIG_VSHCT_1100US |
                    INA232_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

  wireWriteRegister(INA232_REG_CONFIG, config);
}

void P027_data_struct::setCalibration_32V_1A() {
  calValue = 10240;

  // Set multipliers to convert raw current/power values
  currentDivider_mA = 25; // Current LSB = 40uA per bit (1000/40 = 25)

  // Set Calibration register to 'Cal' calculated above
  wireWriteRegister(INA232_REG_CALIBRATION, calValue);

  // Set Config register to take into account the settings above
  uint16_t config = INA232_CONFIG_AVG_1 |
                    INA232_CONFIG_VBUSCT_1100US |
                    INA232_CONFIG_VSHCT_1100US |
                    INA232_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
    wireWriteRegister(INA232_REG_CONFIG, config);
}

void P027_data_struct::setCalibration_16V_400mA() {
  calValue = 8192;

  // Set multipliers to convert raw current/power values
  currentDivider_mA = 20; // Current LSB = 50uA per bit (1000/50 = 20)

  // Set Calibration register to 'Cal' calculated above
  wireWriteRegister(INA232_REG_CALIBRATION, calValue);

  // Set Config register to take into account the settings above
  uint16_t config = INA232_CONFIG_AVG_1 |
                    INA232_CONFIG_VBUSCT_1100US |
                    INA232_CONFIG_VSHCT_1100US |
                    INA232_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

  wireWriteRegister(INA232_REG_CONFIG, config);
}

int16_t P027_data_struct::getBusVoltage_raw() {
  uint16_t value;

  wireReadRegister(INA232_REG_BUSVOLTAGE, &value);

  // Shift to the right 3 to drop CNVR and OVF and multiply by LSB
  return (int16_t)((value >> 3) * 4);
}

int16_t P027_data_struct::getShuntVoltage_raw() {
  uint16_t value;

  wireReadRegister(INA232_REG_SHUNTVOLTAGE, &value);
  return (int16_t)value;
}

int16_t P027_data_struct::getPower_raw() {
  uint16_t value;

  wireReadRegister(INA232_REG_POWER, &value);
  return (int16_t)value;
}

int16_t P027_data_struct::getCurrent_raw() {
  uint16_t value;

  // Sometimes a sharp load will reset the INA232, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  // wireWriteRegister(INA232_R_CALIBRATION, calValue);

  // Now we can safely read the CURRENT register!
  wireReadRegister(INA232_REG_CURRENT, &value);

  return (int16_t)value;
}

float P027_data_struct::getShuntVoltage_mV() {
  int16_t value;

  value = P027_data_struct::getShuntVoltage_raw();
  return value * 0.01f;
}

float P027_data_struct::getBusVoltage_V() {
  int16_t value = getBusVoltage_raw();

  return value * 0.001f;
}

float P027_data_struct::getCurrent_mA() {
  float valueDec = getCurrent_raw();

  valueDec /= currentDivider_mA;
  return valueDec;
}

float P027_data_struct::getPower_W() {
  float valueDec = getPower_raw();

  return valueDec;
}

void P027_data_struct::wireWriteRegister(uint8_t reg, uint16_t value)
{
  I2C_write16_reg(i2caddr, reg, value);
}

void P027_data_struct::wireReadRegister(uint8_t reg, uint16_t *value)
{
  // FIXME TD-er: Must add a function in I2C_access to allow for some delay between calls
  Wire.beginTransmission(i2caddr);
  Wire.write(reg); // Register
  Wire.endTransmission();

  delay(2);        // Max 12-bit conversion t<9ms

  Wire.requestFrom(i2caddr, (uint8_t)2);

  // Shift values to create properly formed integer
  *value = ((Wire.read() << 8) | Wire.read());
}

#endif // ifdef USES_P027
