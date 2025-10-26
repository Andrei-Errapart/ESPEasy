#include "_Plugin_Helper.h"
#ifdef USES_P152

// #######################################################################################################
// ######################### Plugin 152: INA232 DC Voltage/Current sensor ################################
// #######################################################################################################

/** Changelog:
 * 2025-01-24 Initial implementation based on P027 (INA219)
 *            INA232 is a 48V, 16-bit current/voltage/power monitor
 *************************************************************************************************/

# include "src/PluginStructs/P152_data_struct.h"

# define PLUGIN_152
# define PLUGIN_ID_152         152
# define PLUGIN_NAME_152       "Energy (DC) - INA232"
# define PLUGIN_VALUENAME1_152 "Voltage"
# define PLUGIN_VALUENAME2_152 "Current"
# define PLUGIN_VALUENAME3_152 "Power"

# define P152_I2C_ADDR    (uint8_t)PCONFIG(1)

boolean Plugin_152(uint8_t function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
    {
      Device[++deviceCount].Number           = PLUGIN_ID_152;
      Device[deviceCount].Type               = DEVICE_TYPE_I2C;
      Device[deviceCount].VType              = Sensor_VType::SENSOR_TYPE_TRIPLE;
      Device[deviceCount].Ports              = 0;
      Device[deviceCount].PullUpOption       = false;
      Device[deviceCount].InverseLogicOption = false;
      Device[deviceCount].FormulaOption      = true;
      Device[deviceCount].ValueCount         = 3;
      Device[deviceCount].SendDataOption     = true;
      Device[deviceCount].TimerOption        = true;
      Device[deviceCount].GlobalSyncOption   = true;
      Device[deviceCount].PluginStats        = true;
      break;
    }

    case PLUGIN_GET_DEVICENAME:
    {
      string = F(PLUGIN_NAME_152);
      break;
    }

    case PLUGIN_GET_DEVICEVALUENAMES:
    {
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_152));
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_152));
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_152));
      break;
    }

    case PLUGIN_I2C_HAS_ADDRESS:
    case PLUGIN_WEBFORM_SHOW_I2C_PARAMS:
    {
      // INA232 I2C address is configurable via A0 pin
      // A0=GND: 0x40, A0=VS: 0x41, A0=SDA: 0x42, A0=SCL: 0x43
      const uint8_t i2cAddressValues[] = { 0x40, 0x41, 0x42, 0x43 };

      if (function == PLUGIN_WEBFORM_SHOW_I2C_PARAMS) {
        addFormSelectorI2C(F("i2c_addr"), 4, i2cAddressValues, P152_I2C_ADDR);
        addFormNote(F("A0: GND=0x40, VS=0x41, SDA=0x42, SCL=0x43"));
      } else {
        success = intArrayContains(4, i2cAddressValues, event->Par1);
      }
      break;
    }

    # if FEATURE_I2C_GET_ADDRESS
    case PLUGIN_I2C_GET_ADDRESS:
    {
      event->Par1 = P152_I2C_ADDR;
      success     = true;
      break;
    }
    # endif // if FEATURE_I2C_GET_ADDRESS

    case PLUGIN_WEBFORM_LOAD:
    {
      {
        // INA232 supports up to 48V with 16-bit resolution
        const __FlashStringHelper *optionsMode[] = { F("48V, 5A"), F("48V, 2A"), F("24V, 5A"), F("12V, 5A") };
        const int optionValuesMode[]             = { 0, 1, 2, 3 };
        addFormSelector(F("Measure range"), F("range"), 4, optionsMode, optionValuesMode, PCONFIG(0));
      }
      {
        const __FlashStringHelper *options[] = { F("Voltage"), F("Current"), F("Power"), F("Voltage/Current/Power") };
        addFormSelector(F("Measurement Type"), F("measuretype"), 4, options, nullptr, PCONFIG(2));
      }

      success = true;
      break;
    }

    case PLUGIN_WEBFORM_SAVE:
    {
      PCONFIG(0) = getFormItemInt(F("range"));
      PCONFIG(1) = getFormItemInt(F("i2c_addr"));
      PCONFIG(2) = getFormItemInt(F("measuretype"));
      success    = true;
      break;
    }

    case PLUGIN_INIT:
    {
      const uint8_t i2caddr =  P152_I2C_ADDR;

      initPluginTaskData(event->TaskIndex, new (std::nothrow) P152_data_struct(i2caddr));
      P152_data_struct *P152_data =
        static_cast<P152_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P152_data) {
        // Check if device is connected
        if (!P152_data->isConnected()) {
          addLog(LOG_LEVEL_ERROR, F("INA232: Device not found on I2C bus"));
          success = false;
          break;
        }
        
        const bool mustLog = loglevelActiveFor(LOG_LEVEL_INFO);
        String     log;

        if (mustLog) {
          log  = formatToHex(i2caddr, F("INA232 0x"), 2);
          log += F(" setting Range to: ");
        }

        switch (PCONFIG(0)) {
          case 0:
          {
            if (mustLog) {
              log += F("48V, 5A");
            }
            P152_data->setCalibration_48V_5A();
            break;
          }
          case 1:
          {
            if (mustLog) {
              log += F("48V, 2A");
            }
            P152_data->setCalibration_48V_2A();
            break;
          }
          case 2:
          {
            if (mustLog) {
              log += F("24V, 5A");
            }
            P152_data->setCalibration_24V_5A();
            break;
          }
          case 3:
          {
            if (mustLog) {
              log += F("12V, 5A");
            }
            P152_data->setCalibration_12V_5A();
            break;
          }
        }

        if (mustLog) {
          addLogMove(LOG_LEVEL_INFO, log);
        }
        success = true;
      }
      break;
    }

    case PLUGIN_READ:
    {
      P152_data_struct *P152_data =
        static_cast<P152_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P152_data) {
        // Check if device is still connected before reading
        if (!P152_data->isConnected()) {
          addLog(LOG_LEVEL_ERROR, F("INA232: Device not responding, attempting recovery"));
          P152_data->tryRecoverDevice();
          // Try one more time after recovery
          if (!P152_data->isConnected()) {
            success = false;
            break;
          }
        }
        
        float voltage = P152_data->getBusVoltage_V();
        float current = P152_data->getCurrent_mA() / 1000;
        float power   = P152_data->getPower_mW() / 1000;
        
        // Add debug logging to check raw values
        if (loglevelActiveFor(LOG_LEVEL_DEBUG)) {
          P152_data->debugReadAllRegisters();
          String log = F("INA232 Raw values - Voltage: ");
          log += String(voltage, 3);
          log += F("V, Current: ");
          log += String(current, 3);
          log += F("A, Power: ");
          log += String(power, 3);
          log += F("W, Calculated Power: ");
          log += String(voltage * current, 6);
          log += F("W");
          addLogMove(LOG_LEVEL_DEBUG, log);
        }

        UserVar[event->BaseVarIndex]     = voltage;
        UserVar[event->BaseVarIndex + 1] = current;
        UserVar[event->BaseVarIndex + 2] = power;

        const bool mustLog = loglevelActiveFor(LOG_LEVEL_INFO);
        String     log;

        if (mustLog) {
          log = formatToHex(P152_I2C_ADDR, F("INA232 0x"), 2);
        }

        // for backward compability we allow the user to select if only one measurement should be returned
        // or all 3 measurements at once
        switch (PCONFIG(2)) {
          case 0:
          {
            event->sensorType            = Sensor_VType::SENSOR_TYPE_SINGLE;
            UserVar[event->BaseVarIndex] = voltage;

            if (mustLog) {
              log += F(": Voltage: ");
              log += String(voltage, 3);
              log += F("V (");
              log += String(voltage * 1000, 1);
              log += F("mV)");
            }
            break;
          }
          case 1:
          {
            event->sensorType            = Sensor_VType::SENSOR_TYPE_SINGLE;
            UserVar[event->BaseVarIndex] = current;

            if (mustLog) {
              log += F(": Current: ");
              log += String(current, 3);
              log += F("A (");
              log += String(current * 1000, 1);
              log += F("mA)");
            }
            break;
          }
          case 2:
          {
            event->sensorType            = Sensor_VType::SENSOR_TYPE_SINGLE;
            UserVar[event->BaseVarIndex] = power;

            if (mustLog) {
              log += F(": Power: ");
              log += String(power, 3);
              log += F("W (");
              log += String(power * 1000, 1);
              log += F("mW)");
            }
            break;
          }
          case 3:
          {
            event->sensorType                = Sensor_VType::SENSOR_TYPE_TRIPLE;
            UserVar[event->BaseVarIndex]     = voltage;
            UserVar[event->BaseVarIndex + 1] = current;
            UserVar[event->BaseVarIndex + 2] = power;

            if (mustLog) {
              log += F(": Voltage: ");
              log += String(voltage, 3);
              log += F("V (");
              log += String(voltage * 1000, 1);
              log += F("mV) Current: ");
              log += String(current, 3);
              log += F("A (");
              log += String(current * 1000, 1);
              log += F("mA) Power: ");
              log += String(power, 3);
              log += F("W");
            }
            break;
          }
        }

        if (mustLog) {
          addLogMove(LOG_LEVEL_INFO, log);
        }
        success = true;
      }
      break;
    }
  }
  return success;
}

#endif // USES_P152
