/**
 * @file config.h
 * @brief Główna konfiguracja systemu KTR251 - Komputer do malowania pasów drogowych
 * @version 1.0.0
 * @date 2026-02-04
 *
 * UWAGA: ESP32-S3 N16R8 ma ograniczenia pinów:
 * - GPIO 26-32: Zajęte przez Octal PSRAM (NIEDOSTĘPNE)
 * - GPIO 33-37: Zajęte przez Octal Flash (NIEDOSTĘPNE)
 * - GPIO 0, 3, 45, 46: Strapping pins (unikać)
 * - GPIO 19, 20: USB (D-, D+)
 * - GPIO 43, 44: UART0 (TX, RX)
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =============================================================================
// WERSJA SYSTEMU
// =============================================================================
#define FIRMWARE_VERSION    "1.0.0"
#define FIRMWARE_NAME       "KTR251 Road Marking Controller"
#define FIRMWARE_DATE       "2026-02-04"

// =============================================================================
// KONFIGURACJA WIFI
// =============================================================================
#define WIFI_AP_SSID        "KTR251-Controller"
#define WIFI_AP_PASSWORD    "roadmarking2026"
#define WIFI_AP_CHANNEL     1
#define WIFI_AP_MAX_CONN    4

// Opcjonalne połączenie do istniejącej sieci
#define WIFI_STA_ENABLED    false
#define WIFI_STA_SSID       ""
#define WIFI_STA_PASSWORD   ""

// =============================================================================
// KONFIGURACJA SERWERA WWW
// =============================================================================
#define WEB_SERVER_PORT     80
#define WEBSOCKET_PATH      "/ws"

// =============================================================================
// KONFIGURACJA ENKODERA KY-040
// =============================================================================
// Piny enkodera - używamy przerwań
#define ENCODER_PIN_CLK     4       // GPIO4 - sygnał CLK (A)
#define ENCODER_PIN_DT      5       // GPIO5 - sygnał DT (B)
#define ENCODER_PIN_SW      6       // GPIO6 - przycisk (opcjonalny)

// Parametry enkodera
#define ENCODER_PPR         600     // Impulsy na obrót (Pulses Per Revolution)
#define ENCODER_DEBOUNCE_US 100     // Czas debouncingu w mikrosekundach

// Parametry koła pomiarowego
#define WHEEL_DIAMETER_MM   200.0f  // Średnica koła pomiarowego w mm
#define WHEEL_CIRCUMFERENCE (WHEEL_DIAMETER_MM * PI)  // Obwód koła

// =============================================================================
// KONFIGURACJA PRZEKAŹNIKÓW (PISTOLETÓW)
// =============================================================================
#define NUM_RELAYS          6       // Liczba przekaźników/pistoletów

// Piny przekaźników
#define RELAY_PIN_1         15      // GPIO15 - Pistolet 1 (lewy skrajny)
#define RELAY_PIN_2         16      // GPIO16 - Pistolet 2
#define RELAY_PIN_3         17      // GPIO17 - Pistolet 3
#define RELAY_PIN_4         18      // GPIO18 - Pistolet 4
#define RELAY_PIN_5         21      // GPIO21 - Pistolet 5
#define RELAY_PIN_6         47      // GPIO47 - Pistolet 6 (prawy skrajny)

// Logika przekaźników (true = aktywny stan niski)
#define RELAY_ACTIVE_LOW    true

// Tablica pinów przekaźników
static const uint8_t RELAY_PINS[NUM_RELAYS] = {
    RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3,
    RELAY_PIN_4, RELAY_PIN_5, RELAY_PIN_6
};

// =============================================================================
// KONFIGURACJA WYŚWIETLACZA ILI9341
// =============================================================================
// UWAGA: TFT_eSPI wymaga konfiguracji w pliku User_Setup.h
// Poniższe definicje są dla dokumentacji i innych modułów

#define TFT_WIDTH           240
#define TFT_HEIGHT          320

// Piny SPI dla ILI9341 (HSPI)
#define TFT_MOSI            11      // GPIO11 - MOSI
#define TFT_MISO            13      // GPIO13 - MISO (opcjonalny)
#define TFT_SCLK            12      // GPIO12 - SCK
#define TFT_CS              10      // GPIO10 - Chip Select
#define TFT_DC              9       // GPIO9  - Data/Command
#define TFT_RST             8       // GPIO8  - Reset
#define TFT_BL              7       // GPIO7  - Backlight

// Ustawienia wyświetlacza
#define TFT_ROTATION        1       // Orientacja (0-3)
#define TFT_BL_PWM_CHANNEL  0       // Kanał PWM dla podświetlenia
#define TFT_BL_PWM_FREQ     5000    // Częstotliwość PWM (Hz)
#define TFT_BL_PWM_RES      8       // Rozdzielczość PWM (8-bit = 0-255)
#define TFT_BL_DEFAULT      200     // Domyślna jasność (0-255)

// =============================================================================
// KONFIGURACJA MALOWANIA
// =============================================================================
// Domyślne wartości dla wzorów malowania
#define DEFAULT_LINE_LENGTH_MM      6000    // Długość pasa (6m)
#define DEFAULT_GAP_LENGTH_MM       12000   // Długość przerwy (12m)
#define DEFAULT_LINE_WIDTH_MM       120     // Szerokość pasa (12cm)

// Limity wartości
#define MIN_LINE_LENGTH_MM          100     // Minimalna długość pasa
#define MAX_LINE_LENGTH_MM          50000   // Maksymalna długość pasa
#define MIN_GAP_LENGTH_MM           100     // Minimalna przerwa
#define MAX_GAP_LENGTH_MM           100000  // Maksymalna przerwa

// =============================================================================
// KONFIGURACJA SYSTEMU
// =============================================================================
// Interwały zadań (ms)
#define DISPLAY_UPDATE_INTERVAL     100     // Odświeżanie wyświetlacza
#define WEB_UPDATE_INTERVAL         200     // Aktualizacja WebSocket
#define ENCODER_PROCESS_INTERVAL    10      // Przetwarzanie enkodera

// Rozmiary buforów
#define JSON_BUFFER_SIZE            2048
#define WEB_SOCKET_BUFFER_SIZE      1024

// =============================================================================
// WOLNE PINY (DO PRZYSZŁEJ ROZBUDOWY)
// =============================================================================
// GPIO 1, 2, 14, 38, 39, 40, 41, 42, 48 - dostępne dla przyszłych modułów
#define SPARE_PIN_1         1
#define SPARE_PIN_2         2
#define SPARE_PIN_3         14
#define SPARE_PIN_4         38
#define SPARE_PIN_5         39
#define SPARE_PIN_6         40
#define SPARE_PIN_7         41
#define SPARE_PIN_8         42
#define SPARE_PIN_9         48

// =============================================================================
// DEBUG
// =============================================================================
#define DEBUG_SERIAL        true
#define DEBUG_BAUD_RATE     115200

#if DEBUG_SERIAL
    #define DEBUG_PRINT(x)      Serial.print(x)
    #define DEBUG_PRINTLN(x)    Serial.println(x)
    #define DEBUG_PRINTF(...)   Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(...)
#endif

#endif // CONFIG_H
