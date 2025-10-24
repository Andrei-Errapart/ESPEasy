#ifndef PLUGINSTRUCTS_P152_DATA_STRUCT_H
#define PLUGINSTRUCTS_P152_DATA_STRUCT_H

#include "../../_Plugin_Helper.h"
#ifdef USES_P152


struct P152_data_struct : public PluginTaskData_base {
public:

  P152_data_struct(uint8_t i2c_addr);

  P152_data_struct() = delete;
  virtual ~P152_data_struct() = default;


  // **************************************************************************/
  // Configures INA232 to measure up to 48V and 5A
  // **************************************************************************/
  void setCalibration_48V_5A();

  // **************************************************************************/
  // Configures INA232 to measure up to 48V and 2A
  // **************************************************************************/
  void setCalibration_48V_2A();

  // **************************************************************************/
  // Configures INA232 to measure up to 24V and 5A
  // **************************************************************************/
  void setCalibration_24V_5A();

  // **************************************************************************/
  // Configures INA232 to measure up to 12V and 5A
  // **************************************************************************/
  void setCalibration_12V_5A();

private:

  // **************************************************************************/
  // Gets the raw bus voltage (16-bit signed integer)
  // **************************************************************************/
  int16_t getBusVoltage_raw();

  // **************************************************************************/
  // Gets the raw shunt voltage (16-bit signed integer)
  // **************************************************************************/
  int16_t getShuntVoltage_raw();

  // **************************************************************************/
  // Gets the raw current value (16-bit signed integer)
  // **************************************************************************/
  int16_t getCurrent_raw();

  // **************************************************************************/
  // Gets the raw power value (24-bit unsigned integer)
  // **************************************************************************/
  uint32_t getPower_raw();

public:

  // **************************************************************************/
  // Gets the shunt voltage in mV
  // **************************************************************************/
  float getShuntVoltage_mV();

  // **************************************************************************/
  // Gets the bus voltage in volts
  // **************************************************************************/
  float getBusVoltage_V();

  // **************************************************************************/
  // Gets the current value in mA, taking into account the
  // config settings and current LSB
  // **************************************************************************/
  float getCurrent_mA();

  // **************************************************************************/
  // Gets the power value in mW
  // **************************************************************************/
  float getPower_mW();

private:

  // **************************************************************************/
  // Sends a command byte and 16-bit value over I2C
  // **************************************************************************/
  void wireWriteRegister(uint8_t  reg,
                         uint16_t value);

  // **************************************************************************/
  // Reads a 16-bit value over I2C
  // **************************************************************************/
  void wireReadRegister(uint8_t   reg,
                        uint16_t *value);

  // **************************************************************************/
  // Reads a 24-bit value over I2C (for power register)
  // **************************************************************************/
  void wireReadRegister24(uint8_t   reg,
                          uint32_t *value);


  uint32_t calValue = 0;

  // The following multipliers are used to convert raw current and power
  // values to mA and mW, taking into account the current config settings
  float currentLSB_mA = 0;
  float powerLSB_mW   = 0;
  float shuntVoltageLSB_uV = 0;

  uint8_t i2caddr;
};
#endif // ifdef USES_P152
#endif // ifndef PLUGINSTRUCTS_P152_DATA_STRUCT_H
