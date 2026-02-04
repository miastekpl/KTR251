/**
 * @file relay_module.cpp
 * @brief Implementacja modułu obsługi przekaźników
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "relay_module.h"

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

RelayModule Relays;

// =============================================================================
// KONSTRUKTOR
// =============================================================================

RelayModule::RelayModule() {
    memset(&_data, 0, sizeof(_data));

    // Domyślna konfiguracja wzoru
    _data.pattern.lineLengthMM = DEFAULT_LINE_LENGTH_MM;
    _data.pattern.gapLengthMM = DEFAULT_GAP_LENGTH_MM;
    _data.pattern.lineWidthMM = DEFAULT_LINE_WIDTH_MM;

    // Domyślnie wszystkie pistolety aktywne
    for (int i = 0; i < NUM_RELAYS; i++) {
        _data.pattern.activePistols[i] = true;
        _data.relays[i].isEnabled = true;
    }

    _data.mode = PaintingMode::MANUAL;
    _data.state = PaintingState::IDLE;
    _data.isRunning = false;
    _data.isPaused = false;

    _lastUpdateTime = 0;
    _lastDistanceMM = 0;
}

// =============================================================================
// INICJALIZACJA
// =============================================================================

bool RelayModule::begin() {
    DEBUG_PRINTLN(F("[RELAY] Inicjalizacja modulu przekaznikow..."));

    // Konfiguracja pinów przekaźników
    for (int i = 0; i < NUM_RELAYS; i++) {
        pinMode(RELAY_PINS[i], OUTPUT);

        // Początkowy stan - wyłączone
        if (RELAY_ACTIVE_LOW) {
            digitalWrite(RELAY_PINS[i], HIGH);
        } else {
            digitalWrite(RELAY_PINS[i], LOW);
        }

        DEBUG_PRINTF("[RELAY] Przekaznik %d na GPIO%d - skonfigurowany\n",
                     i + 1, RELAY_PINS[i]);
    }

    _lastUpdateTime = millis();
    DEBUG_PRINTLN(F("[RELAY] Modul zainicjalizowany pomyslnie"));

    return true;
}

// =============================================================================
// AKTUALIZACJA
// =============================================================================

void RelayModule::update(float currentDistanceMM) {
    uint32_t currentTime = millis();

    // Oblicz przebytą odległość od ostatniej aktualizacji
    float distanceDelta = currentDistanceMM - _lastDistanceMM;
    _lastDistanceMM = currentDistanceMM;

    // Jeśli poruszamy się do tyłu, ignoruj (lub obsłuż inaczej)
    if (distanceDelta < 0) {
        distanceDelta = 0;
    }

    // Przetwarzaj tryb automatyczny
    if (_data.isRunning && !_data.isPaused) {
        if (_data.mode == PaintingMode::AUTO_LINE ||
            _data.mode == PaintingMode::AUTO_SOLID ||
            _data.mode == PaintingMode::AUTO_DOUBLE) {
            processAutoMode(distanceDelta);
        }
    }

    // Aktualizacja statystyk
    if (currentTime - _lastUpdateTime >= 100) {
        updateRelayStats();
        _lastUpdateTime = currentTime;
    }
}

// =============================================================================
// LOGIKA AUTOMATYCZNEGO MALOWANIA
// =============================================================================

void RelayModule::processAutoMode(float distanceDelta) {
    // Aktualizuj pozycję w cyklu
    _data.currentPositionMM += distanceDelta;

    // Linia ciągła - zawsze maluj
    if (_data.mode == PaintingMode::AUTO_SOLID) {
        if (_data.state != PaintingState::PAINTING) {
            _data.state = PaintingState::PAINTING;
            activateAll();
        }
        _data.totalPaintedMM += (uint32_t)distanceDelta;
        return;
    }

    // Linia przerywana - cykl linia/przerwa
    float cycleLength = _data.pattern.lineLengthMM + _data.pattern.gapLengthMM;

    // Pozycja w aktualnym cyklu
    float positionInCycle = fmod(_data.currentPositionMM, cycleLength);

    // Sprawdź czy jesteśmy w fazie malowania czy przerwy
    if (positionInCycle < _data.pattern.lineLengthMM) {
        // Faza malowania
        if (_data.state != PaintingState::PAINTING) {
            _data.state = PaintingState::PAINTING;
            activateAll();
            DEBUG_PRINTF("[RELAY] Rozpoczecie malowania - cykl %d\n",
                         _data.cycleCount + 1);
        }
        _data.totalPaintedMM += (uint32_t)distanceDelta;
    } else {
        // Faza przerwy
        if (_data.state != PaintingState::GAP) {
            _data.state = PaintingState::GAP;
            deactivateAll();

            // Zwiększ licznik cykli przy przejściu do przerwy
            _data.cycleCount++;
            DEBUG_PRINTF("[RELAY] Przerwa - zakonczono cykl %d\n",
                         _data.cycleCount);
        }
    }
}

// =============================================================================
// STEROWANIE TRYBEM PRACY
// =============================================================================

void RelayModule::setMode(PaintingMode mode) {
    _data.mode = mode;
    DEBUG_PRINTF("[RELAY] Zmiana trybu na: %d\n", (int)mode);

    // Przy zmianie trybu zatrzymaj
    if (_data.isRunning) {
        stop();
    }
}

PaintingMode RelayModule::getMode() const {
    return _data.mode;
}

void RelayModule::start() {
    if (_data.mode == PaintingMode::MANUAL) {
        DEBUG_PRINTLN(F("[RELAY] Nie mozna uruchomic w trybie recznym"));
        return;
    }

    _data.isRunning = true;
    _data.isPaused = false;
    _data.state = PaintingState::PAINTING;
    _data.currentPositionMM = 0;
    _data.cycleStartMM = _lastDistanceMM;

    // Rozpocznij malowanie
    activateAll();

    DEBUG_PRINTF("[RELAY] Start malowania - tryb %d\n", (int)_data.mode);
}

void RelayModule::stop() {
    _data.isRunning = false;
    _data.isPaused = false;
    _data.state = PaintingState::IDLE;

    // Wyłącz wszystkie przekaźniki
    deactivateAll();

    DEBUG_PRINTLN(F("[RELAY] Stop malowania"));
}

void RelayModule::pause() {
    if (_data.isRunning) {
        _data.isPaused = true;
        deactivateAll();
        DEBUG_PRINTLN(F("[RELAY] Pauza"));
    }
}

void RelayModule::resume() {
    if (_data.isRunning && _data.isPaused) {
        _data.isPaused = false;
        DEBUG_PRINTLN(F("[RELAY] Wznowienie"));
    }
}

void RelayModule::reset() {
    stop();
    _data.cycleCount = 0;
    _data.totalPaintedMM = 0;
    _data.currentPositionMM = 0;
    _lastDistanceMM = 0;

    // Reset statystyk przekaźników
    for (int i = 0; i < NUM_RELAYS; i++) {
        _data.relays[i].activationCount = 0;
        _data.relays[i].totalActiveTimeMs = 0;
    }

    DEBUG_PRINTLN(F("[RELAY] Reset systemu"));
}

// =============================================================================
// STEROWANIE RĘCZNE PRZEKAŹNIKAMI
// =============================================================================

void RelayModule::setRelayState(uint8_t index, bool state) {
    if (index >= NUM_RELAYS) return;

    bool physicalState = RELAY_ACTIVE_LOW ? !state : state;
    digitalWrite(RELAY_PINS[index], physicalState);
    _data.relays[index].isActive = state;

    if (state) {
        _data.relays[index].activationCount++;
    }
}

void RelayModule::activateRelay(uint8_t index) {
    if (index >= NUM_RELAYS) return;
    if (!_data.relays[index].isEnabled) return;

    setRelayState(index, true);
    DEBUG_PRINTF("[RELAY] Przekaznik %d WLACZONY\n", index + 1);
}

void RelayModule::deactivateRelay(uint8_t index) {
    if (index >= NUM_RELAYS) return;

    setRelayState(index, false);
    DEBUG_PRINTF("[RELAY] Przekaznik %d WYLACZONY\n", index + 1);
}

void RelayModule::toggleRelay(uint8_t index) {
    if (index >= NUM_RELAYS) return;

    if (_data.relays[index].isActive) {
        deactivateRelay(index);
    } else {
        activateRelay(index);
    }
}

void RelayModule::activateAll() {
    for (int i = 0; i < NUM_RELAYS; i++) {
        if (_data.pattern.activePistols[i] && _data.relays[i].isEnabled) {
            setRelayState(i, true);
        }
    }
}

void RelayModule::deactivateAll() {
    for (int i = 0; i < NUM_RELAYS; i++) {
        setRelayState(i, false);
    }
}

void RelayModule::setActivePistols(uint8_t pistolMask) {
    for (int i = 0; i < NUM_RELAYS; i++) {
        _data.pattern.activePistols[i] = (pistolMask & (1 << i)) != 0;
    }
    DEBUG_PRINTF("[RELAY] Nowa maska pistoletow: 0x%02X\n", pistolMask);
}

void RelayModule::enablePistol(uint8_t index, bool enabled) {
    if (index >= NUM_RELAYS) return;

    _data.pattern.activePistols[index] = enabled;
    _data.relays[index].isEnabled = enabled;

    // Wyłącz jeśli został dezaktywowany
    if (!enabled && _data.relays[index].isActive) {
        deactivateRelay(index);
    }

    DEBUG_PRINTF("[RELAY] Pistolet %d: %s\n", index + 1,
                 enabled ? "AKTYWNY" : "NIEAKTYWNY");
}

// =============================================================================
// KONFIGURACJA WZORU
// =============================================================================

void RelayModule::setLineLength(uint32_t lengthMM) {
    if (lengthMM < MIN_LINE_LENGTH_MM) lengthMM = MIN_LINE_LENGTH_MM;
    if (lengthMM > MAX_LINE_LENGTH_MM) lengthMM = MAX_LINE_LENGTH_MM;

    _data.pattern.lineLengthMM = lengthMM;
    DEBUG_PRINTF("[RELAY] Dlugosc linii: %d mm\n", lengthMM);
}

void RelayModule::setGapLength(uint32_t lengthMM) {
    if (lengthMM < MIN_GAP_LENGTH_MM) lengthMM = MIN_GAP_LENGTH_MM;
    if (lengthMM > MAX_GAP_LENGTH_MM) lengthMM = MAX_GAP_LENGTH_MM;

    _data.pattern.gapLengthMM = lengthMM;
    DEBUG_PRINTF("[RELAY] Dlugosc przerwy: %d mm\n", lengthMM);
}

void RelayModule::setPattern(const PaintingPattern& pattern) {
    _data.pattern = pattern;
    DEBUG_PRINTLN(F("[RELAY] Wzor zaktualizowany"));
}

const PaintingPattern& RelayModule::getPattern() const {
    return _data.pattern;
}

// =============================================================================
// GETTERY
// =============================================================================

const RelayModuleData& RelayModule::getData() const {
    return _data;
}

bool RelayModule::isRunning() const {
    return _data.isRunning;
}

bool RelayModule::isRelayActive(uint8_t index) const {
    if (index >= NUM_RELAYS) return false;
    return _data.relays[index].isActive;
}

uint32_t RelayModule::getCycleCount() const {
    return _data.cycleCount;
}

uint32_t RelayModule::getTotalPaintedMM() const {
    return _data.totalPaintedMM;
}

uint8_t RelayModule::getActiveRelayMask() const {
    uint8_t mask = 0;
    for (int i = 0; i < NUM_RELAYS; i++) {
        if (_data.relays[i].isActive) {
            mask |= (1 << i);
        }
    }
    return mask;
}

// =============================================================================
// FUNKCJE PRYWATNE
// =============================================================================

void RelayModule::updateRelayStats() {
    // Aktualizacja czasu aktywności przekaźników
    for (int i = 0; i < NUM_RELAYS; i++) {
        if (_data.relays[i].isActive) {
            _data.relays[i].totalActiveTimeMs += 100; // Interwał aktualizacji
        }
    }
}
