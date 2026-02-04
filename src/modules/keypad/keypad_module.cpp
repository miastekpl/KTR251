/**
 * @file keypad_module.cpp
 * @brief Implementacja modułu klawiatury matrycowej 4x4
 * @version 1.1.0
 * @date 2026-02-04
 */

#include "keypad_module.h"
#include "config.h"

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

KeypadModule Keypad;

// =============================================================================
// KONSTRUKTOR
// =============================================================================

KeypadModule::KeypadModule() {
    memset(&_state, 0, sizeof(_state));
    _state.lastKey = KeyCode::KEY_NONE;
    _state.currentKey = KeyCode::KEY_NONE;

    _onKeyPressCallback = nullptr;
    _lastScanTime = 0;

    memset(_rawKeys, 0xFF, sizeof(_rawKeys));
    memset(_debouncedKeys, 0xFF, sizeof(_debouncedKeys));
    memset(_lastDebouncedKeys, 0xFF, sizeof(_lastDebouncedKeys));
}

// =============================================================================
// INICJALIZACJA
// =============================================================================

bool KeypadModule::begin() {
    DEBUG_PRINTLN(F("[KEYPAD] Inicjalizacja klawiatury matrycowej 4x4..."));

    // Konfiguracja pinów wierszy jako OUTPUT (domyślnie HIGH)
    for (uint8_t i = 0; i < KEYPAD_ROWS; i++) {
        pinMode(KEYPAD_ROW_PINS[i], OUTPUT);
        digitalWrite(KEYPAD_ROW_PINS[i], HIGH);
        DEBUG_PRINTF("[KEYPAD] Wiersz %d: GPIO%d\n", i + 1, KEYPAD_ROW_PINS[i]);
    }

    // Konfiguracja pinów kolumn jako INPUT_PULLUP
    for (uint8_t i = 0; i < KEYPAD_COLS; i++) {
        pinMode(KEYPAD_COL_PINS[i], INPUT_PULLUP);
        DEBUG_PRINTF("[KEYPAD] Kolumna %d: GPIO%d\n", i + 1, KEYPAD_COL_PINS[i]);
    }

    _lastScanTime = millis();

    DEBUG_PRINTLN(F("[KEYPAD] Uklad klawiatury:"));
    DEBUG_PRINTLN(F("[KEYPAD]   P-1a P-1b P-1c P-1d"));
    DEBUG_PRINTLN(F("[KEYPAD]   P-1e P-2a P-2b P-3a"));
    DEBUG_PRINTLN(F("[KEYPAD]   P-3b P-4  P-6  P-7a"));
    DEBUG_PRINTLN(F("[KEYPAD]   P-7b P-7c P-7d START"));
    DEBUG_PRINTLN(F("[KEYPAD] Modul zainicjalizowany pomyslnie"));

    return true;
}

// =============================================================================
// AKTUALIZACJA
// =============================================================================

void KeypadModule::update() {
    uint32_t currentTime = millis();

    // Skanuj co KEYPAD_SCAN_INTERVAL_MS
    if (currentTime - _lastScanTime < KEYPAD_SCAN_INTERVAL_MS) {
        return;
    }
    _lastScanTime = currentTime;

    // Zachowaj poprzedni stan
    memcpy(_lastDebouncedKeys, _debouncedKeys, sizeof(_debouncedKeys));

    // Skanuj matrycę
    scanMatrix();

    // Debouncing - prosty: akceptuj zmianę tylko jeśli stan się ustabilizował
    static uint8_t stableCount[KEYPAD_ROWS] = {0};
    for (uint8_t row = 0; row < KEYPAD_ROWS; row++) {
        if (_rawKeys[row] == _debouncedKeys[row]) {
            stableCount[row] = 0;
        } else {
            stableCount[row]++;
            if (stableCount[row] >= (KEYPAD_DEBOUNCE_MS / KEYPAD_SCAN_INTERVAL_MS)) {
                _debouncedKeys[row] = _rawKeys[row];
                stableCount[row] = 0;
            }
        }
    }

    // Sprawdź zmiany
    processKeyChange();
}

// =============================================================================
// SKANOWANIE MATRYCY
// =============================================================================

void KeypadModule::scanMatrix() {
    for (uint8_t row = 0; row < KEYPAD_ROWS; row++) {
        // Ustaw aktualny wiersz na LOW
        digitalWrite(KEYPAD_ROW_PINS[row], LOW);

        // Krótkie opóźnienie na ustabilizowanie
        delayMicroseconds(10);

        // Odczytaj kolumny
        uint8_t colState = 0;
        for (uint8_t col = 0; col < KEYPAD_COLS; col++) {
            if (digitalRead(KEYPAD_COL_PINS[col]) == LOW) {
                colState |= (1 << col);  // Klawisz naciśnięty
            }
        }

        _rawKeys[row] = colState;

        // Przywróć wiersz na HIGH
        digitalWrite(KEYPAD_ROW_PINS[row], HIGH);
    }
}

// =============================================================================
// PRZETWARZANIE ZMIAN
// =============================================================================

void KeypadModule::processKeyChange() {
    // Znajdź naciśnięty klawisz
    KeyCode newKey = KeyCode::KEY_NONE;

    for (uint8_t row = 0; row < KEYPAD_ROWS; row++) {
        if (_debouncedKeys[row] != 0) {
            for (uint8_t col = 0; col < KEYPAD_COLS; col++) {
                if (_debouncedKeys[row] & (1 << col)) {
                    newKey = matrixToKeyCode(row, col);
                    break;
                }
            }
            if (newKey != KeyCode::KEY_NONE) break;
        }
    }

    // Wykryj nowe naciśnięcie
    if (newKey != KeyCode::KEY_NONE && _state.currentKey == KeyCode::KEY_NONE) {
        // Nowe naciśnięcie
        _state.keyPressed = true;
        _state.lastKey = newKey;
        _state.lastPressTime = millis();
        _state.pressCount++;

        DEBUG_PRINTF("[KEYPAD] Klawisz nacisniety: %d\n", (int)newKey);

        // Wywołaj callback
        if (_onKeyPressCallback) {
            _onKeyPressCallback(newKey);
        }
    }

    // Wykryj zwolnienie
    if (newKey == KeyCode::KEY_NONE && _state.currentKey != KeyCode::KEY_NONE) {
        _state.keyReleased = true;
        DEBUG_PRINTF("[KEYPAD] Klawisz zwolniony: %d\n", (int)_state.currentKey);
    }

    _state.currentKey = newKey;
}

// =============================================================================
// KONWERSJA
// =============================================================================

KeyCode KeypadModule::matrixToKeyCode(uint8_t row, uint8_t col) const {
    // Oblicz indeks (row * 4 + col)
    uint8_t index = row * KEYPAD_COLS + col;
    if (index > 15) return KeyCode::KEY_NONE;
    return (KeyCode)index;
}

// =============================================================================
// ODCZYT STANU
// =============================================================================

bool KeypadModule::isKeyPressed() {
    if (_state.keyPressed) {
        _state.keyPressed = false;
        return true;
    }
    return false;
}

bool KeypadModule::isKeyReleased() {
    if (_state.keyReleased) {
        _state.keyReleased = false;
        return true;
    }
    return false;
}

KeyCode KeypadModule::getLastKey() const {
    return _state.lastKey;
}

KeyCode KeypadModule::getCurrentKey() const {
    return _state.currentKey;
}

bool KeypadModule::isKeyDown(KeyCode key) const {
    return _state.currentKey == key;
}

uint8_t KeypadModule::keyToPatternIndex(KeyCode key) const {
    uint8_t k = (uint8_t)key;
    // Klawisze 0-14 odpowiadają wzorcom 0-14
    // Klawisz 15 (START) nie ma wzorca
    if (k < 15) return k;
    return 255;
}

bool KeypadModule::isStartKey(KeyCode key) const {
    return key == KeyCode::KEY_START;
}

const KeypadState& KeypadModule::getState() const {
    return _state;
}

// =============================================================================
// CALLBACK
// =============================================================================

void KeypadModule::setOnKeyPress(KeyPressCallback callback) {
    _onKeyPressCallback = callback;
}
