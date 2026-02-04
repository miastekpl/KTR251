# KTR251 - Road Marking Controller

**Komputer pokładowy do sterowania procesem malowania pasów drogowych**

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)]()
[![Platform](https://img.shields.io/badge/platform-ESP32--S3-green.svg)]()
[![License](https://img.shields.io/badge/license-MIT-orange.svg)]()

---

## Opis projektu

KTR251 to zaawansowany system sterowania dla maszyn do malowania pasów drogowych. System automatycznie kontroluje włączanie i wyłączanie pistoletów malarskich na podstawie dystansu mierzonego enkoderem, zapewniając precyzyjne i powtarzalne malowanie linii na drogach.

### Główne funkcje

- **Automatyczne malowanie** linii przerywanych z konfigurowalnymi parametrami
- **Sterowanie 6 pistoletami** - niezależna kontrola każdego pistoletu
- **Pomiar dystansu i prędkości** w czasie rzeczywistym
- **Wyświetlacz TFT 2.8"** z bieżącymi danymi operacyjnymi
- **Bezprzewodowy panel sterowania** - dostęp przez WiFi i przeglądarkę WWW
- **Budowa modułowa** - łatwa rozbudowa i utrzymanie kodu

---

## Wymagania sprzętowe

| Komponent | Specyfikacja |
|-----------|--------------|
| Mikrokontroler | ESP32-S3 N16R8 (16MB Flash, 8MB PSRAM) |
| Wyświetlacz | ILI9341 TFT 2.8" (320x240, SPI) |
| Enkoder | KY-040 lub przemysłowy (600 PPR) |
| Przekaźniki | Moduł 6-kanałowy, 5V, active low |
| Zasilanie | 12-24V DC |

---

## Struktura projektu

```
KTR251/
├── src/
│   ├── main.cpp                    # Główny plik programu
│   └── modules/
│       ├── encoder/                # Moduł enkodera
│       │   ├── encoder_module.h
│       │   └── encoder_module.cpp
│       ├── relays/                 # Moduł przekaźników
│       │   ├── relay_module.h
│       │   └── relay_module.cpp
│       ├── display/                # Moduł wyświetlacza
│       │   ├── display_module.h
│       │   └── display_module.cpp
│       └── webserver/              # Moduł serwera WWW
│           ├── webserver_module.h
│           └── webserver_module.cpp
├── include/
│   ├── config.h                    # Konfiguracja systemu
│   └── User_Setup.h                # Konfiguracja TFT_eSPI
├── docs/
│   ├── SCHEMATIC.md                # Schematy połączeń
│   └── MANUAL.md                   # Instrukcja obsługi
├── releases/                       # Wydania ZIP
├── platformio.ini                  # Konfiguracja PlatformIO
├── CHANGELOG.md                    # Historia zmian
└── README.md                       # Ten plik
```

---

## Instalacja

### Wymagania

- [PlatformIO](https://platformio.org/) (zalecane) lub Arduino IDE
- Biblioteki (instalowane automatycznie przez PlatformIO):
  - TFT_eSPI v2.5.43
  - ArduinoJson v7.0.0
  - ESPAsyncWebServer v1.2.3
  - AsyncTCP v1.1.1

### Kompilacja i wgranie

```bash
# Klonowanie repozytorium
git clone https://github.com/miastekpl/KTR251.git
cd KTR251

# Kompilacja
pio run

# Wgranie na ESP32-S3
pio run --target upload

# Monitor szeregowy
pio device monitor
```

---

## Podłączenie

### Przypisanie pinów

| GPIO | Funkcja | Opis |
|------|---------|------|
| 4 | ENCODER_CLK | Sygnał A enkodera |
| 5 | ENCODER_DT | Sygnał B enkodera |
| 6 | ENCODER_SW | Przycisk enkodera |
| 7 | TFT_BL | Podświetlenie (PWM) |
| 8 | TFT_RST | Reset wyświetlacza |
| 9 | TFT_DC | Data/Command |
| 10 | TFT_CS | Chip Select |
| 11 | TFT_MOSI | SPI MOSI |
| 12 | TFT_SCLK | SPI Clock |
| 13 | TFT_MISO | SPI MISO |
| 15-18, 21, 47 | RELAY_1-6 | Przekaźniki pistoletów |

Szczegółowe schematy: [docs/SCHEMATIC.md](docs/SCHEMATIC.md)

---

## Użytkowanie

### Szybki start

1. Włącz zasilanie - poczekaj na ekran powitalny
2. Połącz się z siecią WiFi: `KTR251-Controller` (hasło: `roadmarking2026`)
3. Otwórz przeglądarkę: `http://192.168.4.1`
4. Skonfiguruj wzór malowania i wybierz tryb pracy
5. Naciśnij START

### Tryby pracy

| Tryb | Opis |
|------|------|
| **Ręczny** | Sterowanie każdym pistoletem niezależnie |
| **Auto Linia** | Automatyczne malowanie linii przerywanych |
| **Auto Ciągła** | Malowanie linii ciągłej |

### Domyślne parametry

- Długość linii: 6000 mm (6 m)
- Długość przerwy: 12000 mm (12 m)
- PPR enkodera: 600
- Średnica koła: 200 mm

Pełna instrukcja: [docs/MANUAL.md](docs/MANUAL.md)

---

## API WebSocket

System udostępnia API przez WebSocket (`ws://192.168.4.1/ws`):

### Komendy

```json
{"cmd": "start"}                              // Uruchomienie
{"cmd": "stop"}                               // Zatrzymanie
{"cmd": "reset"}                              // Reset liczników
{"cmd": "setMode", "mode": 1}                 // Zmiana trybu (0-2)
{"cmd": "setPattern", "lineLen": 6000, "gapLen": 12000}  // Wzór
{"cmd": "togglePistol", "id": 0}              // Przełączenie pistoletu
```

### Dane (broadcast)

```json
{
  "type": "data",
  "distance": 123.45,
  "speed": 5.2,
  "cycles": 15,
  "painted": 90000,
  "running": true,
  "pistols": 63,
  "mode": 1
}
```

---

## Changelog

Zobacz [CHANGELOG.md](CHANGELOG.md) dla pełnej historii zmian.

### Wersja 1.0.0 (2026-02-04)

- Pierwsza wersja produkcyjna
- Modułowa architektura firmware
- Kompletny interfejs WWW z WebSocket
- Dokumentacja techniczna i instrukcja obsługi

---

## Licencja

MIT License - szczegóły w pliku LICENSE.

---

## Autor

Projekt KTR251 stworzony z wykorzystaniem Claude AI.

---

*Komputer do malowania pasów drogowych - KTR251 v1.0.0*
