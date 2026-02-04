/**
 * @file main.cpp
 * @brief Główny plik programu KTR251 - Komputer do malowania pasów drogowych
 * @version 1.0.0
 * @date 2026-02-04
 *
 * System sterowania malowaniem pasów drogowych
 * - Enkoder KY-040 do pomiaru dystansu
 * - 6 przekaźników (pistoletów)
 * - Wyświetlacz ILI9341
 * - Serwer WWW z WebSocket
 *
 * Platforma: ESP32-S3 N16R8
 */

#include <Arduino.h>
#include "config.h"

// Moduły
#include "modules/encoder/encoder_module.h"
#include "modules/relays/relay_module.h"
#include "modules/display/display_module.h"
#include "modules/webserver/webserver_module.h"

// =============================================================================
// ZMIENNE GLOBALNE
// =============================================================================

// Timery dla zadań
uint32_t lastEncoderUpdate = 0;
uint32_t lastDisplayUpdate = 0;
uint32_t lastWebUpdate = 0;

// Flagi stanu
bool systemInitialized = false;

// =============================================================================
// DEKLARACJE FUNKCJI
// =============================================================================

void initializeSystem();
void updateEncoder();
void updateDisplay();
void updateWeb();
void handleWebCommand(const char* command, JsonDocument& params);
void handleEncoderButton();

// =============================================================================
// SETUP
// =============================================================================

void setup() {
    // Inicjalizacja portu szeregowego
    Serial.begin(DEBUG_BAUD_RATE);
    delay(100);

    DEBUG_PRINTLN(F("\n========================================"));
    DEBUG_PRINTLN(F("   KTR251 - Road Marking Controller"));
    DEBUG_PRINTF("   Firmware: v%s\n", FIRMWARE_VERSION);
    DEBUG_PRINTF("   Data: %s\n", FIRMWARE_DATE);
    DEBUG_PRINTLN(F("========================================\n"));

    // Informacje o ESP32-S3
    DEBUG_PRINTF("[SYSTEM] Chip: %s Rev %d\n",
                 ESP.getChipModel(), ESP.getChipRevision());
    DEBUG_PRINTF("[SYSTEM] CPU: %d MHz\n", ESP.getCpuFreqMHz());
    DEBUG_PRINTF("[SYSTEM] Flash: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    DEBUG_PRINTF("[SYSTEM] PSRAM: %d KB\n", ESP.getPsramSize() / 1024);
    DEBUG_PRINTF("[SYSTEM] Free heap: %d KB\n", ESP.getFreeHeap() / 1024);
    DEBUG_PRINTLN();

    // Inicjalizacja systemu
    initializeSystem();
}

// =============================================================================
// GŁÓWNA PĘTLA
// =============================================================================

void loop() {
    if (!systemInitialized) {
        delay(100);
        return;
    }

    uint32_t currentTime = millis();

    // Aktualizacja enkodera (wysoka częstotliwość)
    if (currentTime - lastEncoderUpdate >= ENCODER_PROCESS_INTERVAL) {
        updateEncoder();
        lastEncoderUpdate = currentTime;
    }

    // Aktualizacja wyświetlacza
    if (currentTime - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
        updateDisplay();
        lastDisplayUpdate = currentTime;
    }

    // Aktualizacja WebSocket
    if (currentTime - lastWebUpdate >= WEB_UPDATE_INTERVAL) {
        updateWeb();
        lastWebUpdate = currentTime;
    }

    // Obsługa przycisku enkodera
    handleEncoderButton();

    // Aktualizacja serwera WWW
    WebServer.update();

    // Krótka pauza dla stabilności
    yield();
}

// =============================================================================
// INICJALIZACJA SYSTEMU
// =============================================================================

void initializeSystem() {
    DEBUG_PRINTLN(F("[SYSTEM] Inicjalizacja modulow..."));

    // 1. Wyświetlacz (pokazuje splash screen)
    if (!Display.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac wyswietlacza!"));
        return;
    }

    // 2. Enkoder
    if (!Encoder.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac enkodera!"));
        Display.showMessage("BLAD", "Enkoder", COLOR_ERROR);
        return;
    }

    // 3. Przekaźniki
    if (!Relays.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac przekaznikow!"));
        Display.showMessage("BLAD", "Przekazniki", COLOR_ERROR);
        return;
    }

    // 4. Serwer WWW
    if (!WebServer.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac serwera WWW!"));
        Display.showMessage("BLAD", "WiFi/WWW", COLOR_ERROR);
        return;
    }

    // Ustawienie callbacka dla komend WWW
    WebServer.setCommandCallback(handleWebCommand);

    // Aktualizacja wyświetlacza z adresem IP
    Display.setIPAddress(WebServer.getIPAddress().c_str());
    Display.setStatusText("Gotowy");

    // Odśwież ekran główny
    Display.refresh();

    // System zainicjalizowany
    systemInitialized = true;

    DEBUG_PRINTLN(F("\n[SYSTEM] ============================"));
    DEBUG_PRINTLN(F("[SYSTEM] System zainicjalizowany!"));
    DEBUG_PRINTF("[SYSTEM] WiFi AP: %s\n", WIFI_AP_SSID);
    DEBUG_PRINTF("[SYSTEM] Haslo: %s\n", WIFI_AP_PASSWORD);
    DEBUG_PRINTF("[SYSTEM] WWW: http://%s\n", WebServer.getIPAddress().c_str());
    DEBUG_PRINTLN(F("[SYSTEM] ============================\n"));
}

// =============================================================================
// AKTUALIZACJA ENKODERA
// =============================================================================

void updateEncoder() {
    // Aktualizacja modułu enkodera
    Encoder.update();

    // Aktualizacja modułu przekaźników z aktualnym dystansem
    Relays.update(Encoder.getDistanceMM());
}

// =============================================================================
// AKTUALIZACJA WYŚWIETLACZA
// =============================================================================

void updateDisplay() {
    // Przygotuj dane dla wyświetlacza
    DisplayData displayData;
    displayData.distanceM = Encoder.getDistanceM();
    displayData.speedKMH = Encoder.getSpeedKMH();
    displayData.cycleCount = Relays.getCycleCount();
    displayData.totalPaintedM = Relays.getTotalPaintedMM() / 1000;
    displayData.isRunning = Relays.isRunning();
    displayData.isPaused = Relays.getData().isPaused;
    displayData.activePistols = Relays.getActiveRelayMask();
    displayData.mode = (uint8_t)Relays.getMode();

    // Status tekstowy
    if (Relays.isRunning()) {
        if (Relays.getData().isPaused) {
            strcpy(displayData.statusText, "PAUZA");
        } else if (Relays.getData().state == PaintingState::PAINTING) {
            strcpy(displayData.statusText, "MALOWANIE");
        } else {
            strcpy(displayData.statusText, "PRZERWA");
        }
    } else {
        strcpy(displayData.statusText, "Gotowy");
    }

    // IP
    strncpy(displayData.ipAddress, WebServer.getIPAddress().c_str(),
            sizeof(displayData.ipAddress) - 1);

    // Aktualizuj wyświetlacz
    Display.setData(displayData);
    Display.update();
}

// =============================================================================
// AKTUALIZACJA WWW
// =============================================================================

void updateWeb() {
    // Przygotuj dane dla WebSocket
    WebServerData webData;
    webData.distanceM = Encoder.getDistanceM();
    webData.speedKMH = Encoder.getSpeedKMH();
    webData.cycleCount = Relays.getCycleCount();
    webData.totalPaintedM = Relays.getTotalPaintedMM();
    webData.isRunning = Relays.isRunning();
    webData.isPaused = Relays.getData().isPaused;
    webData.activePistols = Relays.getActiveRelayMask();
    webData.mode = (uint8_t)Relays.getMode();
    webData.pulseCount = Encoder.getPulseCount();
    webData.lineLength = Relays.getPattern().lineLengthMM;
    webData.gapLength = Relays.getPattern().gapLengthMM;

    // Aktualizuj dane wzoru w WebServer
    WebServer.setPatternData(webData.lineLength, webData.gapLength);

    // Wyślij do klientów
    WebServer.broadcastData(webData);
}

// =============================================================================
// OBSŁUGA KOMEND Z WWW
// =============================================================================

void handleWebCommand(const char* command, JsonDocument& params) {
    DEBUG_PRINTF("[CMD] Otrzymano komende: %s\n", command);

    if (strcmp(command, "start") == 0) {
        Relays.start();
        Display.setStatusText("MALOWANIE");
    }
    else if (strcmp(command, "stop") == 0) {
        Relays.stop();
        Display.setStatusText("Gotowy");
    }
    else if (strcmp(command, "pause") == 0) {
        Relays.pause();
        Display.setStatusText("PAUZA");
    }
    else if (strcmp(command, "resume") == 0) {
        Relays.resume();
    }
    else if (strcmp(command, "reset") == 0) {
        Relays.reset();
        Encoder.resetDistance();
        Display.setStatusText("Reset");
    }
    else if (strcmp(command, "setMode") == 0) {
        int mode = params["mode"] | 0;
        Relays.setMode((PaintingMode)mode);
        DEBUG_PRINTF("[CMD] Tryb zmieniony na: %d\n", mode);
    }
    else if (strcmp(command, "setPattern") == 0) {
        uint32_t lineLen = params["lineLen"] | DEFAULT_LINE_LENGTH_MM;
        uint32_t gapLen = params["gapLen"] | DEFAULT_GAP_LENGTH_MM;
        Relays.setLineLength(lineLen);
        Relays.setGapLength(gapLen);
        DEBUG_PRINTF("[CMD] Wzor: linia=%d mm, przerwa=%d mm\n", lineLen, gapLen);
    }
    else if (strcmp(command, "togglePistol") == 0) {
        int id = params["id"] | -1;
        if (id >= 0 && id < NUM_RELAYS) {
            Relays.toggleRelay(id);
        }
    }
    else if (strcmp(command, "activateRelay") == 0) {
        int id = params["id"] | -1;
        if (id >= 0 && id < NUM_RELAYS) {
            Relays.activateRelay(id);
        }
    }
    else if (strcmp(command, "deactivateRelay") == 0) {
        int id = params["id"] | -1;
        if (id >= 0 && id < NUM_RELAYS) {
            Relays.deactivateRelay(id);
        }
    }
    else if (strcmp(command, "setWheelDiameter") == 0) {
        float diameter = params["diameter"] | WHEEL_DIAMETER_MM;
        Encoder.setWheelDiameter(diameter);
    }
    else if (strcmp(command, "setPPR") == 0) {
        int ppr = params["ppr"] | ENCODER_PPR;
        Encoder.setPPR(ppr);
    }
    else if (strcmp(command, "setBrightness") == 0) {
        int brightness = params["value"] | TFT_BL_DEFAULT;
        Display.setBrightness(brightness);
    }
    else {
        DEBUG_PRINTF("[CMD] Nieznana komenda: %s\n", command);
    }
}

// =============================================================================
// OBSŁUGA PRZYCISKU ENKODERA
// =============================================================================

void handleEncoderButton() {
    if (Encoder.wasButtonReleased()) {
        DEBUG_PRINTLN(F("[BUTTON] Przycisk enkodera nacisniety"));

        // Przełącz między ekranami
        Display.nextScreen();

        // Lub: Start/Stop malowania
        // if (Relays.isRunning()) {
        //     Relays.stop();
        // } else {
        //     Relays.start();
        // }
    }
}
