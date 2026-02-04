/**
 * @file main.cpp
 * @brief Główny plik programu KTR251 v1.1.0 - Komputer do malowania pasów drogowych
 * @version 1.1.0
 * @date 2026-02-04
 *
 * Platforma: ESP32-S3 N16R8
 *
 * Moduły:
 * - encoder_module: Pomiar dystansu i prędkości
 * - relay_module: Sterowanie 6 pistoletami
 * - display_module: Wyświetlacz TFT ILI9341
 * - webserver_module: WiFi AP + panel WWW
 * - patterns_module: Predefiniowane wzorce malowania (P-1 do P-7)
 * - keypad_module: Klawiatura matrycowa 4x4 do wyboru wzorców
 */

#include <Arduino.h>
#include "config.h"

// Moduły
#include "modules/encoder/encoder_module.h"
#include "modules/relays/relay_module.h"
#include "modules/display/display_module.h"
#include "modules/webserver/webserver_module.h"
#include "modules/patterns/patterns_module.h"
#include "modules/keypad/keypad_module.h"

// =============================================================================
// ZMIENNE GLOBALNE
// =============================================================================

uint32_t lastEncoderUpdate = 0;
uint32_t lastDisplayUpdate = 0;
uint32_t lastWebUpdate = 0;
uint32_t lastKeypadUpdate = 0;

bool systemInitialized = false;

// =============================================================================
// DEKLARACJE FUNKCJI
// =============================================================================

void initializeSystem();
void updateEncoder();
void updateDisplay();
void updateWeb();
void updateKeypad();
void handleWebCommand(const char* command, JsonDocument& params);
void handleEncoderButton();
void handleKeypadPress(KeyCode key);
void onPatternChange(PatternId newPattern, const PaintPattern* pattern);
void applyPattern(const PaintPattern* pattern);

// =============================================================================
// SETUP
// =============================================================================

void setup() {
    Serial.begin(DEBUG_BAUD_RATE);
    delay(100);

    DEBUG_PRINTLN(F("\n========================================"));
    DEBUG_PRINTLN(F("   KTR251 - Road Marking Controller"));
    DEBUG_PRINTF("   Firmware: v%s\n", FIRMWARE_VERSION);
    DEBUG_PRINTF("   Data: %s\n", FIRMWARE_DATE);
    DEBUG_PRINTLN(F("========================================\n"));

    DEBUG_PRINTF("[SYSTEM] Chip: %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    DEBUG_PRINTF("[SYSTEM] CPU: %d MHz\n", ESP.getCpuFreqMHz());
    DEBUG_PRINTF("[SYSTEM] Flash: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    DEBUG_PRINTF("[SYSTEM] PSRAM: %d KB\n", ESP.getPsramSize() / 1024);
    DEBUG_PRINTF("[SYSTEM] Free heap: %d KB\n", ESP.getFreeHeap() / 1024);
    DEBUG_PRINTLN();

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

    // Aktualizacja enkodera (10ms)
    if (currentTime - lastEncoderUpdate >= ENCODER_PROCESS_INTERVAL) {
        updateEncoder();
        lastEncoderUpdate = currentTime;
    }

    // Aktualizacja klawiatury (20ms)
    if (currentTime - lastKeypadUpdate >= KEYPAD_SCAN_INTERVAL) {
        updateKeypad();
        lastKeypadUpdate = currentTime;
    }

    // Aktualizacja wyświetlacza (100ms)
    if (currentTime - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
        updateDisplay();
        lastDisplayUpdate = currentTime;
    }

    // Aktualizacja WebSocket (200ms)
    if (currentTime - lastWebUpdate >= WEB_UPDATE_INTERVAL) {
        updateWeb();
        lastWebUpdate = currentTime;
    }

    // Obsługa przycisku enkodera
    handleEncoderButton();

    // Aktualizacja serwera WWW
    WebServer.update();

    yield();
}

// =============================================================================
// INICJALIZACJA SYSTEMU
// =============================================================================

void initializeSystem() {
    DEBUG_PRINTLN(F("[SYSTEM] Inicjalizacja modulow..."));

    // 1. Wzorce malowania
    if (!Patterns.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac wzorcow!"));
        return;
    }
    Patterns.setOnPatternChange(onPatternChange);

    // 2. Wyświetlacz
    if (!Display.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac wyswietlacza!"));
        return;
    }

    // 3. Enkoder
    if (!Encoder.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac enkodera!"));
        Display.showMessage("BLAD", "Enkoder", COLOR_ERROR);
        return;
    }

    // 4. Przekaźniki
    if (!Relays.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac przekaznikow!"));
        Display.showMessage("BLAD", "Przekazniki", COLOR_ERROR);
        return;
    }

    // 5. Klawiatura
    if (!Keypad.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac klawiatury!"));
        Display.showMessage("BLAD", "Klawiatura", COLOR_ERROR);
        return;
    }
    Keypad.setOnKeyPress(handleKeypadPress);

    // 6. Serwer WWW
    if (!WebServer.begin()) {
        DEBUG_PRINTLN(F("[SYSTEM] BLAD: Nie mozna zainicjalizowac serwera WWW!"));
        Display.showMessage("BLAD", "WiFi/WWW", COLOR_ERROR);
        return;
    }
    WebServer.setCommandCallback(handleWebCommand);

    // Zastosuj domyślny wzorzec (P-1a)
    applyPattern(Patterns.getActivePattern());

    // Aktualizacja wyświetlacza
    Display.setIPAddress(WebServer.getIPAddress().c_str());
    Display.setStatusText("Gotowy");
    Display.refresh();

    systemInitialized = true;

    DEBUG_PRINTLN(F("\n[SYSTEM] ============================"));
    DEBUG_PRINTLN(F("[SYSTEM] System zainicjalizowany!"));
    DEBUG_PRINTF("[SYSTEM] WiFi AP: %s\n", WIFI_AP_SSID);
    DEBUG_PRINTF("[SYSTEM] Haslo: %s\n", WIFI_AP_PASSWORD);
    DEBUG_PRINTF("[SYSTEM] WWW: http://%s\n", WebServer.getIPAddress().c_str());
    DEBUG_PRINTF("[SYSTEM] Aktywny wzorzec: %s\n", Patterns.getActivePatternName());
    DEBUG_PRINTLN(F("[SYSTEM] ============================\n"));
}

// =============================================================================
// AKTUALIZACJA ENKODERA
// =============================================================================

void updateEncoder() {
    Encoder.update();
    Relays.update(Encoder.getDistanceMM());
}

// =============================================================================
// AKTUALIZACJA KLAWIATURY
// =============================================================================

void updateKeypad() {
    Keypad.update();
}

// =============================================================================
// OBSŁUGA NACIŚNIĘCIA KLAWISZA
// =============================================================================

void handleKeypadPress(KeyCode key) {
    DEBUG_PRINTF("[KEYPAD] Klawisz: %d\n", (int)key);

    if (Keypad.isStartKey(key)) {
        // Przycisk START/STOP
        if (Relays.isRunning()) {
            Relays.stop();
            Display.setStatusText("STOP");
        } else {
            // Ustaw tryb automatyczny i start
            if (Patterns.isActiveSolid()) {
                Relays.setMode(PaintingMode::AUTO_SOLID);
            } else {
                Relays.setMode(PaintingMode::AUTO_LINE);
            }
            Relays.start();
            Display.setStatusText("MALOWANIE");
        }
    } else {
        // Przycisk wzorca (0-14)
        uint8_t patternIndex = Keypad.keyToPatternIndex(key);
        if (patternIndex < NUM_PATTERNS) {
            Patterns.setPatternByIndex(patternIndex);
            // Wzorzec zostanie zastosowany przez callback onPatternChange
        }
    }
}

// =============================================================================
// CALLBACK ZMIANY WZORCA
// =============================================================================

void onPatternChange(PatternId newPattern, const PaintPattern* pattern) {
    DEBUG_PRINTF("[PATTERN] Zmiana wzorca na: %s\n", pattern->name);
    applyPattern(pattern);
}

// =============================================================================
// ZASTOSOWANIE WZORCA
// =============================================================================

void applyPattern(const PaintPattern* pattern) {
    if (!pattern) return;

    // Ustaw parametry w module przekaźników
    Relays.setLineLength(pattern->lineLengthMM);
    Relays.setGapLength(pattern->gapLengthMM);

    // Ustaw tryb w zależności od typu linii
    if (pattern->lineType == LineType::SOLID ||
        pattern->lineType == LineType::DOUBLE_SOLID) {
        Relays.setMode(PaintingMode::AUTO_SOLID);
    } else {
        Relays.setMode(PaintingMode::AUTO_LINE);
    }

    // Aktualizuj WebServer
    WebServer.setPatternData(
        pattern->lineLengthMM,
        pattern->gapLengthMM,
        Patterns.getActivePatternIndex(),
        pattern->name
    );

    DEBUG_PRINTF("[PATTERN] Zastosowano: %s (L=%dmm, G=%dmm, W=%dmm)\n",
        pattern->name,
        pattern->lineLengthMM,
        pattern->gapLengthMM,
        pattern->lineWidthMM
    );
}

// =============================================================================
// AKTUALIZACJA WYŚWIETLACZA
// =============================================================================

void updateDisplay() {
    DisplayData displayData;
    displayData.distanceM = Encoder.getDistanceM();
    displayData.speedKMH = Encoder.getSpeedKMH();
    displayData.cycleCount = Relays.getCycleCount();
    displayData.totalPaintedM = Relays.getTotalPaintedMM() / 1000;
    displayData.isRunning = Relays.isRunning();
    displayData.isPaused = Relays.getData().isPaused;
    displayData.activePistols = Relays.getActiveRelayMask();
    displayData.mode = (uint8_t)Relays.getMode();

    // Status tekstowy z nazwą wzorca
    if (Relays.isRunning()) {
        if (Relays.getData().isPaused) {
            snprintf(displayData.statusText, sizeof(displayData.statusText),
                     "PAUZA [%s]", Patterns.getActivePatternName());
        } else if (Relays.getData().state == PaintingState::PAINTING) {
            snprintf(displayData.statusText, sizeof(displayData.statusText),
                     "%s", Patterns.getActivePatternName());
        } else {
            snprintf(displayData.statusText, sizeof(displayData.statusText),
                     "PRZERWA [%s]", Patterns.getActivePatternName());
        }
    } else {
        snprintf(displayData.statusText, sizeof(displayData.statusText),
                 "%s", Patterns.getActivePatternName());
    }

    strncpy(displayData.ipAddress, WebServer.getIPAddress().c_str(),
            sizeof(displayData.ipAddress) - 1);

    Display.setData(displayData);
    Display.update();
}

// =============================================================================
// AKTUALIZACJA WWW
// =============================================================================

void updateWeb() {
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
    webData.lineLength = Patterns.getActiveLineLength();
    webData.gapLength = Patterns.getActiveGapLength();

    WebServer.broadcastData(webData);
}

// =============================================================================
// OBSŁUGA KOMEND Z WWW
// =============================================================================

void handleWebCommand(const char* command, JsonDocument& params) {
    DEBUG_PRINTF("[CMD] Komenda: %s\n", command);

    if (strcmp(command, "start") == 0) {
        if (Patterns.isActiveSolid()) {
            Relays.setMode(PaintingMode::AUTO_SOLID);
        } else {
            Relays.setMode(PaintingMode::AUTO_LINE);
        }
        Relays.start();
        Display.setStatusText("MALOWANIE");
    }
    else if (strcmp(command, "stop") == 0) {
        Relays.stop();
        Display.setStatusText("STOP");
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
    else if (strcmp(command, "setPatternByIndex") == 0) {
        int index = params["index"] | -1;
        if (index >= 0 && index < NUM_PATTERNS) {
            Patterns.setPatternByIndex(index);
        }
    }
    else if (strcmp(command, "setPatternByName") == 0) {
        const char* name = params["name"];
        if (name) {
            Patterns.setPatternByName(name);
        }
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
    else if (strcmp(command, "nextPattern") == 0) {
        Patterns.nextPattern();
    }
    else if (strcmp(command, "prevPattern") == 0) {
        Patterns.prevPattern();
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
        DEBUG_PRINTLN(F("[BUTTON] Przycisk enkodera"));
        // Przełącz następny wzorzec
        Patterns.nextPattern();
    }
}
