# CLAUDE.md - AI Assistant Guide for KTR251

> This document provides context and guidelines for AI assistants working with this repository.

## Project Overview

**Repository:** KTR251 - Road Marking Controller
**Status:** Production Ready v1.0.0
**Platform:** ESP32-S3 N16R8
**Language:** C++ (Arduino Framework)
**Last Updated:** 2026-02-04

KTR251 is an advanced embedded system for controlling road marking machines. It automatically controls paint guns based on distance measured by an encoder, ensuring precise and repeatable line painting.

## Repository Structure

```
KTR251/
├── src/
│   ├── main.cpp                    # Main program - module integration
│   └── modules/
│       ├── encoder/                # Encoder module (distance/speed)
│       │   ├── encoder_module.h
│       │   └── encoder_module.cpp
│       ├── relays/                 # Relay module (6 paint guns)
│       │   ├── relay_module.h
│       │   └── relay_module.cpp
│       ├── display/                # Display module (ILI9341 TFT)
│       │   ├── display_module.h
│       │   └── display_module.cpp
│       └── webserver/              # WebServer module (WiFi AP + WWW)
│           ├── webserver_module.h
│           └── webserver_module.cpp
├── include/
│   ├── config.h                    # System configuration & pin definitions
│   └── User_Setup.h                # TFT_eSPI library configuration
├── docs/
│   ├── SCHEMATIC.md                # Electrical schematics
│   └── MANUAL.md                   # User manual (Polish)
├── releases/                       # ZIP releases
├── platformio.ini                  # PlatformIO configuration
├── CHANGELOG.md                    # Version history
├── README.md                       # Project documentation
└── CLAUDE.md                       # This file
```

### Module Architecture

| Module | Purpose | Key Files |
|--------|---------|-----------|
| **encoder** | Distance & speed measurement | `encoder_module.h/cpp` |
| **relays** | Paint gun control (6 channels) | `relay_module.h/cpp` |
| **display** | TFT screen with real-time data | `display_module.h/cpp` |
| **webserver** | WiFi AP + web control panel | `webserver_module.h/cpp` |

## Hardware Configuration

### ESP32-S3 N16R8 Pin Restrictions

**UNAVAILABLE PINS (occupied by memory):**
- GPIO 26-32: Octal PSRAM
- GPIO 33-37: Octal Flash

**STRAPPING PINS (avoid):**
- GPIO 0, 3, 45, 46

### Current Pin Assignment

| GPIO | Function | Module |
|------|----------|--------|
| 4 | ENCODER_CLK | Encoder |
| 5 | ENCODER_DT | Encoder |
| 6 | ENCODER_SW | Encoder (button) |
| 7 | TFT_BL | Display (backlight PWM) |
| 8 | TFT_RST | Display |
| 9 | TFT_DC | Display |
| 10 | TFT_CS | Display |
| 11 | TFT_MOSI | Display (SPI) |
| 12 | TFT_SCLK | Display (SPI) |
| 13 | TFT_MISO | Display (SPI) |
| 15 | RELAY_1 | Relays (Gun 1) |
| 16 | RELAY_2 | Relays (Gun 2) |
| 17 | RELAY_3 | Relays (Gun 3) |
| 18 | RELAY_4 | Relays (Gun 4) |
| 21 | RELAY_5 | Relays (Gun 5) |
| 47 | RELAY_6 | Relays (Gun 6) |

**Free pins for expansion:** GPIO 1, 2, 14, 38, 39, 40, 41, 42, 48

## Build & Run

### Prerequisites

- PlatformIO CLI or VS Code with PlatformIO extension
- USB cable for ESP32-S3

### Commands

```bash
# Build
pio run

# Upload to device
pio run --target upload

# Serial monitor
pio device monitor --baud 115200

# Clean build
pio run --target clean
```

### WiFi Access (default)

- **SSID:** `KTR251-Controller`
- **Password:** `roadmarking2026`
- **Web panel:** `http://192.168.4.1`

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| TFT_eSPI | ^2.5.43 | ILI9341 display driver |
| ArduinoJson | ^7.0.0 | JSON parsing for WebSocket |
| ESPAsyncWebServer | ^1.2.3 | Async web server |
| AsyncTCP | ^1.1.1 | TCP for async web server |

## Development Guidelines

### Code Style

- **Language:** C++ with Arduino framework
- **Naming:**
  - Classes: PascalCase (`EncoderModule`)
  - Functions: camelCase (`getDistanceMM()`)
  - Constants: UPPER_SNAKE_CASE (`ENCODER_PPR`)
  - Private members: prefix `_` (`_data`)
- **Comments:** Polish for user-facing, English for technical
- **Indentation:** 4 spaces

### Module Pattern

Each module follows this structure:

```cpp
// Header (.h)
class ModuleName {
public:
    bool begin();           // Initialize module
    void update();          // Call in loop()
    // ... getters/setters
private:
    ModuleData _data;
    // ... internal methods
};
extern ModuleName ModuleInstance;

// Implementation (.cpp)
ModuleName ModuleInstance;
// ... implementation
```

### Adding New Modules

1. Create folder in `src/modules/<module_name>/`
2. Add `<module_name>_module.h` and `.cpp`
3. Follow existing module pattern
4. Include in `main.cpp`
5. Update `config.h` if new pins needed

### Important Notes

- **Relay logic:** Active LOW (set HIGH = relay OFF)
- **Encoder:** Uses hardware interrupts on GPIO 4
- **Display:** Requires `User_Setup.h` copied to TFT_eSPI library folder
- **WebSocket path:** `/ws`

## AI Assistant Instructions

### Before Making Changes

1. Read `config.h` for pin assignments and system parameters
2. Check existing module interfaces before adding new features
3. Verify GPIO availability (see pin restrictions above)
4. Review `CHANGELOG.md` for recent changes

### When Modifying Code

1. Maintain modular structure - changes should be in appropriate module
2. Update `config.h` for any new configurable parameters
3. Test with all modules enabled
4. Update documentation if interface changes

### When Adding Features

1. Prefer extending existing modules over creating new ones
2. Use existing patterns (callbacks, data structures)
3. Keep WebSocket API consistent with existing commands
4. Add new commands to `handleWebCommand()` in `main.cpp`

### Git Commits

Format: `<type>: <description in English>`

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `chore`

Example: `feat: Add temperature sensor support`

---

**Version:** 1.0.0
**Last reviewed:** 2026-02-04
