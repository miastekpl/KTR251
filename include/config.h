/**
 * @file config.h
 * @brief Główna konfiguracja systemu KTR251 - Komputer do malowania pasów drogowych
 * @version 1.1.0
 * @date 2026-02-04
 *
 * UWAGA: Konfiguracja pinów GPIO znajduje się w pliku pins_config.h
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "pins_config.h"    // Konfiguracja pinów GPIO

// =============================================================================
// WERSJA SYSTEMU
// =============================================================================
#define FIRMWARE_VERSION    "1.1.0"
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
// KONFIGURACJA ENKODERA
// =============================================================================
// Piny enkodera zdefiniowane w pins_config.h:
// PIN_ENCODER_CLK, PIN_ENCODER_DT, PIN_ENCODER_SW

// Parametry enkodera
#define ENCODER_PPR         600     // Impulsy na obrót (Pulses Per Revolution)
#define ENCODER_DEBOUNCE_US 100     // Czas debouncingu w mikrosekundach

// Parametry koła pomiarowego
#define WHEEL_DIAMETER_MM   200.0f  // Średnica koła pomiarowego w mm
#define WHEEL_CIRCUMFERENCE (WHEEL_DIAMETER_MM * PI)  // Obwód koła

// Aliasy dla kompatybilności wstecznej
#define ENCODER_PIN_CLK     PIN_ENCODER_CLK
#define ENCODER_PIN_DT      PIN_ENCODER_DT
#define ENCODER_PIN_SW      PIN_ENCODER_SW

// =============================================================================
// KONFIGURACJA PRZEKAŹNIKÓW (PISTOLETÓW)
// =============================================================================
// Piny i liczba przekaźników zdefiniowane w pins_config.h:
// NUM_RELAYS, PIN_RELAY_1 - PIN_RELAY_6, RELAY_PINS[]

// Logika przekaźników (true = aktywny stan niski)
#define RELAY_ACTIVE_LOW    true

// =============================================================================
// KONFIGURACJA WYŚWIETLACZA ILI9341
// =============================================================================
// Piny wyświetlacza zdefiniowane w pins_config.h:
// PIN_TFT_BL, PIN_TFT_RST, PIN_TFT_DC, PIN_TFT_CS, PIN_TFT_MOSI, PIN_TFT_SCLK, PIN_TFT_MISO

#define TFT_WIDTH           240
#define TFT_HEIGHT          320

// Ustawienia wyświetlacza
#define TFT_ROTATION        1       // Orientacja (0-3)
#define TFT_BL_PWM_CHANNEL  0       // Kanał PWM dla podświetlenia
#define TFT_BL_PWM_FREQ     5000    // Częstotliwość PWM (Hz)
#define TFT_BL_PWM_RES      8       // Rozdzielczość PWM (8-bit = 0-255)
#define TFT_BL_DEFAULT      200     // Domyślna jasność (0-255)

// Aliasy dla kompatybilności wstecznej
#define TFT_BL              PIN_TFT_BL
#define TFT_RST             PIN_TFT_RST
#define TFT_DC              PIN_TFT_DC
#define TFT_CS              PIN_TFT_CS
#define TFT_MOSI            PIN_TFT_MOSI
#define TFT_SCLK            PIN_TFT_SCLK
#define TFT_MISO            PIN_TFT_MISO

// =============================================================================
// KONFIGURACJA MALOWANIA - DOMYŚLNE WARTOŚCI
// =============================================================================
// Domyślny wzorzec: P-1a (4m linia, 8m przerwa, 12cm szerokość)
#define DEFAULT_LINE_LENGTH_MM      4000    // Długość pasa (4m) - P-1a
#define DEFAULT_GAP_LENGTH_MM       8000    // Długość przerwy (8m) - P-1a
#define DEFAULT_LINE_WIDTH_MM       120     // Szerokość pasa (12cm)

// Limity wartości
#define MIN_LINE_LENGTH_MM          100     // Minimalna długość pasa
#define MAX_LINE_LENGTH_MM          50000   // Maksymalna długość pasa (50m)
#define MIN_GAP_LENGTH_MM           100     // Minimalna przerwa
#define MAX_GAP_LENGTH_MM           100000  // Maksymalna przerwa (100m)

// =============================================================================
// KONFIGURACJA SYSTEMU - INTERWAŁY
// =============================================================================
#define DISPLAY_UPDATE_INTERVAL     100     // Odświeżanie wyświetlacza (ms)
#define WEB_UPDATE_INTERVAL         200     // Aktualizacja WebSocket (ms)
#define ENCODER_PROCESS_INTERVAL    10      // Przetwarzanie enkodera (ms)
#define KEYPAD_SCAN_INTERVAL        20      // Skanowanie klawiatury (ms)

// =============================================================================
// KONFIGURACJA SYSTEMU - BUFORY
// =============================================================================
#define JSON_BUFFER_SIZE            2048
#define WEB_SOCKET_BUFFER_SIZE      1024

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
