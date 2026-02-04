/**
 * @file User_Setup.h
 * @brief Konfiguracja biblioteki TFT_eSPI dla ESP32-S3 N16R8 + ILI9341
 * @version 1.0.0
 *
 * UWAGA: Ten plik musi zostać skopiowany do katalogu biblioteki TFT_eSPI
 * lub zdefiniować USER_SETUP_LOADED przed includowaniem TFT_eSPI
 */

#ifndef USER_SETUP_H
#define USER_SETUP_H

// =============================================================================
// STEROWNIK WYŚWIETLACZA
// =============================================================================
#define ILI9341_DRIVER

// =============================================================================
// ROZMIAR WYŚWIETLACZA
// =============================================================================
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// =============================================================================
// PINY ESP32-S3 N16R8 DLA ILI9341
// =============================================================================
// Bezpieczne piny, bez konfliktów z PSRAM/Flash

#define TFT_MOSI    11      // GPIO11 - Master Out Slave In
#define TFT_MISO    13      // GPIO13 - Master In Slave Out
#define TFT_SCLK    12      // GPIO12 - Serial Clock
#define TFT_CS      10      // GPIO10 - Chip Select
#define TFT_DC       9      // GPIO9  - Data/Command
#define TFT_RST      8      // GPIO8  - Reset

// Podświetlenie kontrolowane osobno przez PWM
#define TFT_BL       7      // GPIO7  - Backlight (nie używane przez TFT_eSPI)

// =============================================================================
// KONFIGURACJA SPI
// =============================================================================
// ESP32-S3 obsługuje SPI do 80MHz, ale 40MHz jest bezpieczniejsze
#define SPI_FREQUENCY       40000000    // 40 MHz
#define SPI_READ_FREQUENCY  20000000    // 20 MHz dla odczytu
#define SPI_TOUCH_FREQUENCY  2500000    // 2.5 MHz dla dotyku (jeśli używany)

// =============================================================================
// OPCJE WYŚWIETLACZA
// =============================================================================
// Włącz obsługę fontów
#define LOAD_GLCD   // Font 1: Adafruit 8 pixel
#define LOAD_FONT2  // Font 2: 16 pixel
#define LOAD_FONT4  // Font 4: 26 pixel
#define LOAD_FONT6  // Font 6: 48 pixel numeryczny
#define LOAD_FONT7  // Font 7: 48 pixel 7-segmentowy
#define LOAD_FONT8  // Font 8: 75 pixel numeryczny
#define LOAD_GFXFF  // FreeFonts

// Włącz smooth fonts
#define SMOOTH_FONT

// =============================================================================
// OPTYMALIZACJE DLA ESP32-S3
// =============================================================================
// DMA dla szybszego transferu (ESP32-S3 wspiera)
#define USE_HSPI_PORT

// Transakcje SPI
#define SUPPORT_TRANSACTIONS

#endif // USER_SETUP_H
