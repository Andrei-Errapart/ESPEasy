# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESPEasy is firmware for ESP8266/ESP8285/ESP32/ESP32-S2 microcontrollers that enables easy IoT deployment of sensors and actuators. It uses a plugin-based architecture with 150+ sensor/actuator plugins and supports multiple home automation controllers.

**Main branch**: `mega` (development and stable branch)

## Build System

The project uses PlatformIO with a multi-file configuration system:

- `platformio.ini` - Main configuration (includes other .ini files)
- `platformio_esp82xx_envs.ini` - ESP8266/ESP8285 build environments
- `platformio_esp32_envs.ini` - ESP32 build environments
- `platformio_esp32s2_envs.ini` - ESP32-S2 build environments
- `platformio_core_defs.ini` - Core build flags and definitions

### Common Build Commands

```bash
# Build default environment (see default_envs in platformio.ini)
pio run

# Build specific environment
pio run -e normal_ESP8266_4M1M
pio run -e max_ESP32_16M8M_LittleFS

# Upload to device
pio run -e <environment> -t upload

# Clean build
pio run -t clean

# List all available environments
pio run --list-targets
```

### Build Scripts

The build process uses Python scripts in `tools/pio/`:
- `generate-compiletime-defines.py` - Generates compile-time version info
- `concat_cpp_files.py` - Pre-build concatenation for ESP8266
- `gzip-firmware.py` - Post-build compression
- `set-ci-defines.py` - CI/CD build configuration

## Code Architecture

### Directory Structure

```
src/
├── ESPEasy.ino              # Main entry point (setup/loop)
├── _Pxxx_*.ino              # Plugin files (150+ plugins)
├── _Cxxx.cpp                # Controller files (MQTT, HTTP, etc.)
├── _Nxxx_*.cpp              # Notification plugins
└── src/                     # Modular C++ code
    ├── Commands/            # Command handlers (GPIO, WiFi, System, etc.)
    ├── ControllerQueue/     # Controller queue implementations
    ├── DataStructs/         # Data structures and classes
    ├── DataTypes/           # Enums and type definitions
    ├── ESPEasyCore/         # Core setup/loop logic
    ├── Globals/             # Global variables
    ├── Helpers/             # Helper functions
    ├── PluginStructs/       # Plugin-specific data structures
    └── WebServer/           # Web interface handlers
```

### Plugin Architecture

Plugins are the core of ESPEasy's functionality. Each plugin:

- **File naming**: `_Pxxx_Name.ino` where xxx is the plugin ID (e.g., `_P001_Switch.ino`)
- **Function signature**: `boolean Plugin_xxx(uint8_t function, struct EventStruct *event, String& string)`
- **Lifecycle functions**:
  - `PLUGIN_DEVICE_ADD` - Define device characteristics
  - `PLUGIN_INIT` - Initialize hardware
  - `PLUGIN_READ` - Read sensor data
  - `PLUGIN_WRITE` - Handle commands
  - `PLUGIN_WEBFORM_LOAD` - Build configuration UI
  - `PLUGIN_WEBFORM_SAVE` - Save configuration
  - `PLUGIN_EXIT` - Cleanup/free memory

**Plugin Development Lifecycle**:
1. Development - Wrap with `#ifdef PLUGIN_BUILD_DEV`, add [DEVELOPMENT] to name
2. Testing - Wrap with `#ifdef PLUGIN_BUILD_TESTING`, add [TESTING] to name
3. Stable - Remove ifdefs and tags

**Configuration Storage**:
- `CONFIG_PIN1/2/3` - GPIO pin assignments
- `CONFIG_PORT` - Port selection
- `PCONFIG(x)` - Custom config (x = 1-8, stores 16-bit values)
- `PCONFIG_LONG(x)` - Long values
- Plugin template: `src/_Pxxx_PluginTemplate.ino`

### Controller Architecture

Controllers (files starting with `_C`) handle communication with home automation systems:
- Domoticz (HTTP/MQTT)
- Home Assistant
- OpenHAB
- Generic MQTT
- Custom protocols

Each controller implements queuing for reliable delivery.

### Code Organization Principles

1. **Modular Structure**: Core logic is in `src/src/`, organized by function (Commands, Helpers, DataStructs)
2. **Plugin Isolation**: Plugins should be self-contained and not modify framework code
3. **Helper Macros**: Use macros from `_Plugin_Helper.h` for cleaner code
4. **Memory Management**: Always free memory in `PLUGIN_EXIT`

## Testing

Integration tests are in `test/`:
- Python-based test framework
- Tests communicate with ESP device over serial/network
- Run with: `./test/testall` (requires connected device)

## Documentation

- ReadTheDocs: https://espeasy.readthedocs.io/en/latest/
- Source files: `docs/source/` (Sphinx/RST format)
- Build docs: `cd docs && pip install -r requirements.txt && make html`

## Contributing Guidelines

From `.github/CONTRIBUTING.md`:

1. **Code Quality**:
   - Ensure code compiles without warnings (warnings treated as errors in CI)
   - Use the mega branch for new features
   - Use separate PRs for different features/fixes

2. **Plugin Development**:
   - New plugins start with `PLUGIN_BUILD_DEV` ifdef
   - Move to `PLUGIN_BUILD_TESTING` after initial validation
   - Do NOT modify variables in `PLUGIN_WRITE` handlers
   - Prefix log entries with plugin ID: `[Pxxx] message`

3. **Custom Builds**:
   - Override defaults using `Custom.h` (see `Custom-sample.h`)
   - Enable with `-D USE_CUSTOM_H` build flag

## Platform-Specific Notes

### ESP8266/ESP8285
- Uses lwIP 2 Low Memory mode
- Supports 1M-16M flash configurations
- Files may be concatenated during build (`concat_cpp_files.py`)

### ESP32/ESP32-S2
- Larger flash/RAM allows "max" builds with all plugins
- Supports Ethernet (ETH builds)
- Different partition schemes for various flash sizes (see `esp32_partition_*.csv`)

## Important Build Flags

From `platformio_core_defs.ini`:
- `-D USE_CUSTOM_H` - Use Custom.h for overrides
- `-D FEATURE_ADC_VCC=1` - Measure ESP8266 VCC
- `-D PLUGIN_BUILD_DEV` - Include development plugins
- `-D PLUGIN_BUILD_TESTING` - Include testing plugins

## Firmware Naming Convention

Binary releases follow this pattern:
```
ESPEasy_mega_[date]_[build-type]_[hardware]_[flash-size]_[features].bin
```

Build types: normal, max, collection_A-F, energy, display, minimal, custom

## CI/CD

GitHub Actions workflows (`.github/workflows/`):
- `build.yml` - Build all environments on PR/push to mega
- `release.yml` - Create release binaries

The CI uses matrix builds generated by `tools/ci/generate-matrix.py`.
