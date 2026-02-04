/**
 * @file pins_config.h
 * @brief Dedykowana konfiguracja pinów GPIO dla ESP32-S3 N16R8
 * @version 1.1.0
 * @date 2026-02-04
 *
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║                    ESP32-S3 N16R8 - MAPA PINÓW                           ║
 * ╠══════════════════════════════════════════════════════════════════════════╣
 * ║  PINY NIEDOSTĘPNE (pamięć Octal):                                        ║
 * ║  - GPIO 26-32: Octal PSRAM (NIEDOSTĘPNE!)                                ║
 * ║  - GPIO 33-37: Octal Flash (NIEDOSTĘPNE!)                                ║
 * ║                                                                          ║
 * ║  PINY STRAPPING (unikać przy starcie):                                   ║
 * ║  - GPIO 0:  Boot mode select                                             ║
 * ║  - GPIO 3:  JTAG signal                                                  ║
 * ║  - GPIO 45: VDD_SPI voltage select                                       ║
 * ║  - GPIO 46: Boot mode / Log output                                       ║
 * ║                                                                          ║
 * ║  PINY SPECJALNE:                                                         ║
 * ║  - GPIO 19, 20: USB D-, D+ (używane przez USB)                           ║
 * ║  - GPIO 43, 44: UART0 TX/RX (używane przez Serial)                       ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 *
 * DOSTĘPNE PINY GPIO dla użytkownika:
 * GPIO 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 21,
 * GPIO 38, 39, 40, 41, 42, 47, 48
 *
 * UWAGA: Przed zmianą pinów sprawdź dokumentację ESP32-S3!
 */

#ifndef PINS_CONFIG_H
#define PINS_CONFIG_H

#include <Arduino.h>

// =============================================================================
// █████████████████████████████████████████████████████████████████████████████
// █                           ENKODER KY-040                                  █
// █████████████████████████████████████████████████████████████████████████████
// =============================================================================
/**
 * Enkoder obrotowy do pomiaru dystansu
 * - CLK: Sygnał A (z przerwaniem)
 * - DT:  Sygnał B (kierunek)
 * - SW:  Przycisk (opcjonalny)
 *
 * Wszystkie piny z wewnętrznym pull-up
 */
#define PIN_ENCODER_CLK         4       // GPIO4  - Sygnał CLK (przerwanie)
#define PIN_ENCODER_DT          5       // GPIO5  - Sygnał DT (kierunek)
#define PIN_ENCODER_SW          6       // GPIO6  - Przycisk enkodera

// =============================================================================
// █████████████████████████████████████████████████████████████████████████████
// █                        WYŚWIETLACZ ILI9341 (SPI)                          █
// █████████████████████████████████████████████████████████████████████████████
// =============================================================================
/**
 * Wyświetlacz TFT 2.8" ILI9341 320x240
 * Interfejs: SPI @ 40MHz
 *
 * UWAGA: Te same piny muszą być w User_Setup.h dla TFT_eSPI!
 */
#define PIN_TFT_BL              7       // GPIO7  - Backlight (PWM)
#define PIN_TFT_RST             8       // GPIO8  - Reset
#define PIN_TFT_DC              9       // GPIO9  - Data/Command
#define PIN_TFT_CS              10      // GPIO10 - Chip Select
#define PIN_TFT_MOSI            11      // GPIO11 - SPI MOSI
#define PIN_TFT_SCLK            12      // GPIO12 - SPI Clock
#define PIN_TFT_MISO            13      // GPIO13 - SPI MISO (opcjonalny)

// =============================================================================
// █████████████████████████████████████████████████████████████████████████████
// █                      PRZEKAŹNIKI (6x PISTOLETY)                           █
// █████████████████████████████████████████████████████████████████████████████
// =============================================================================
/**
 * Moduł 6 przekaźników do sterowania pistoletami
 * Logika: ACTIVE LOW (HIGH = wyłączony, LOW = włączony)
 *
 * Rozmieszczenie:
 *   [1] [2] [3]  <- Lewa strona
 *   [4] [5] [6]  <- Prawa strona
 */
#define NUM_RELAYS              6

#define PIN_RELAY_1             15      // GPIO15 - Pistolet 1 (lewy skrajny)
#define PIN_RELAY_2             16      // GPIO16 - Pistolet 2 (lewy środkowy)
#define PIN_RELAY_3             17      // GPIO17 - Pistolet 3 (środkowy L)
#define PIN_RELAY_4             18      // GPIO18 - Pistolet 4 (środkowy P)
#define PIN_RELAY_5             21      // GPIO21 - Pistolet 5 (prawy środkowy)
#define PIN_RELAY_6             47      // GPIO47 - Pistolet 6 (prawy skrajny)

// Tablica pinów przekaźników (dla pętli)
static const uint8_t RELAY_PINS[NUM_RELAYS] = {
    PIN_RELAY_1, PIN_RELAY_2, PIN_RELAY_3,
    PIN_RELAY_4, PIN_RELAY_5, PIN_RELAY_6
};

// =============================================================================
// █████████████████████████████████████████████████████████████████████████████
// █                    KLAWIATURA MATRYCOWA 4x4                               █
// █████████████████████████████████████████████████████████████████████████████
// =============================================================================
/**
 * Klawiatura membranowa 4x4 do wyboru wzorców malowania
 * 16 przycisków = 15 wzorców + 1 funkcyjny
 *
 * Układ klawiatury:
 *        C1    C2    C3    C4
 *       ┌─────┬─────┬─────┬─────┐
 *   R1  │P-1a │P-1b │P-1c │P-1d │
 *       ├─────┼─────┼─────┼─────┤
 *   R2  │P-1e │P-2a │P-2b │P-3a │
 *       ├─────┼─────┼─────┼─────┤
 *   R3  │P-3b │ P-4 │ P-6 │P-7a │
 *       ├─────┼─────┼─────┼─────┤
 *   R4  │P-7b │P-7c │P-7d │START│
 *       └─────┴─────┴─────┴─────┘
 */
#define KEYPAD_ROWS             4
#define KEYPAD_COLS             4

// Piny wierszy (OUTPUT podczas skanowania)
#define PIN_KEYPAD_ROW1         1       // GPIO1  - Wiersz 1
#define PIN_KEYPAD_ROW2         2       // GPIO2  - Wiersz 2
#define PIN_KEYPAD_ROW3         14      // GPIO14 - Wiersz 3
#define PIN_KEYPAD_ROW4         38      // GPIO38 - Wiersz 4

// Piny kolumn (INPUT_PULLUP podczas skanowania)
#define PIN_KEYPAD_COL1         39      // GPIO39 - Kolumna 1
#define PIN_KEYPAD_COL2         40      // GPIO40 - Kolumna 2
#define PIN_KEYPAD_COL3         41      // GPIO41 - Kolumna 3
#define PIN_KEYPAD_COL4         42      // GPIO42 - Kolumna 4

// Tablice pinów klawiatury
static const uint8_t KEYPAD_ROW_PINS[KEYPAD_ROWS] = {
    PIN_KEYPAD_ROW1, PIN_KEYPAD_ROW2, PIN_KEYPAD_ROW3, PIN_KEYPAD_ROW4
};

static const uint8_t KEYPAD_COL_PINS[KEYPAD_COLS] = {
    PIN_KEYPAD_COL1, PIN_KEYPAD_COL2, PIN_KEYPAD_COL3, PIN_KEYPAD_COL4
};

// =============================================================================
// █████████████████████████████████████████████████████████████████████████████
// █                         WOLNE PINY (REZERWOWE)                            █
// █████████████████████████████████████████████████████████████████████████████
// =============================================================================
/**
 * Piny dostępne do przyszłej rozbudowy:
 * - Czujnik poziomu farby
 * - Czujnik temperatury
 * - Buzzer / sygnalizacja dźwiękowa
 * - LED status
 * - Dodatkowe wejścia/wyjścia
 */
#define PIN_SPARE_1             48      // GPIO48 - Wolny (np. buzzer)

// =============================================================================
// █████████████████████████████████████████████████████████████████████████████
// █                      PODSUMOWANIE UŻYCIA PINÓW                            █
// █████████████████████████████████████████████████████████████████████████████
// =============================================================================
/**
 * ┌──────────┬────────────────────────────────────────────────────────────────┐
 * │   GPIO   │                        FUNKCJA                                 │
 * ├──────────┼────────────────────────────────────────────────────────────────┤
 * │    1     │ Klawiatura - Wiersz 1                                          │
 * │    2     │ Klawiatura - Wiersz 2                                          │
 * │    4     │ Enkoder - CLK (przerwanie)                                     │
 * │    5     │ Enkoder - DT (kierunek)                                        │
 * │    6     │ Enkoder - SW (przycisk)                                        │
 * │    7     │ TFT - Backlight (PWM)                                          │
 * │    8     │ TFT - Reset                                                    │
 * │    9     │ TFT - DC                                                       │
 * │   10     │ TFT - CS                                                       │
 * │   11     │ TFT - MOSI                                                     │
 * │   12     │ TFT - SCLK                                                     │
 * │   13     │ TFT - MISO                                                     │
 * │   14     │ Klawiatura - Wiersz 3                                          │
 * │   15     │ Przekaźnik 1 (Pistolet 1)                                      │
 * │   16     │ Przekaźnik 2 (Pistolet 2)                                      │
 * │   17     │ Przekaźnik 3 (Pistolet 3)                                      │
 * │   18     │ Przekaźnik 4 (Pistolet 4)                                      │
 * │   21     │ Przekaźnik 5 (Pistolet 5)                                      │
 * │   38     │ Klawiatura - Wiersz 4                                          │
 * │   39     │ Klawiatura - Kolumna 1                                         │
 * │   40     │ Klawiatura - Kolumna 2                                         │
 * │   41     │ Klawiatura - Kolumna 3                                         │
 * │   42     │ Klawiatura - Kolumna 4                                         │
 * │   47     │ Przekaźnik 6 (Pistolet 6)                                      │
 * │   48     │ Wolny (rezerwowy)                                              │
 * ├──────────┼────────────────────────────────────────────────────────────────┤
 * │  26-32   │ NIEDOSTĘPNE - Octal PSRAM                                      │
 * │  33-37   │ NIEDOSTĘPNE - Octal Flash                                      │
 * │  0,3,45,46│ STRAPPING - nie używać                                        │
 * │  19,20   │ USB - zarezerwowane                                            │
 * │  43,44   │ UART0 - Serial monitor                                         │
 * └──────────┴────────────────────────────────────────────────────────────────┘
 */

#endif // PINS_CONFIG_H
