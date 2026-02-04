/**
 * @file encoder_module.cpp
 * @brief Implementacja modułu obsługi enkodera KY-040
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "encoder_module.h"

// =============================================================================
// ZMIENNE STATYCZNE
// =============================================================================

EncoderModule* EncoderModule::_instance = nullptr;
EncoderModule Encoder;

// =============================================================================
// KONSTRUKTOR
// =============================================================================

EncoderModule::EncoderModule() {
    _instance = this;

    // Inicjalizacja danych
    memset(&_data, 0, sizeof(_data));

    // Domyślna konfiguracja
    _config.pulsesPerRevolution = ENCODER_PPR;
    _config.wheelDiameterMM = WHEEL_DIAMETER_MM;
    _config.wheelCircumferenceMM = WHEEL_CIRCUMFERENCE;
    _config.debounceTimeUS = ENCODER_DEBOUNCE_US;

    _lastSpeedCalcTime = 0;
    _lastSpeedPulseCount = 0;
    _lastInterruptTime = 0;
}

// =============================================================================
// INICJALIZACJA
// =============================================================================

bool EncoderModule::begin() {
    DEBUG_PRINTLN(F("[ENCODER] Inicjalizacja modulu enkodera..."));

    // Konfiguracja pinów
    pinMode(ENCODER_PIN_CLK, INPUT_PULLUP);
    pinMode(ENCODER_PIN_DT, INPUT_PULLUP);
    pinMode(ENCODER_PIN_SW, INPUT_PULLUP);

    // Podłączenie przerwania na CLK
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_CLK),
                    EncoderModule::handleInterrupt,
                    FALLING);

    // Inicjalizacja czasu
    _data.lastUpdateTime = millis();
    _lastSpeedCalcTime = millis();

    DEBUG_PRINTF("[ENCODER] PPR: %d, Obwod kola: %.2f mm\n",
                 _config.pulsesPerRevolution,
                 _config.wheelCircumferenceMM);
    DEBUG_PRINTLN(F("[ENCODER] Modul zainicjalizowany pomyslnie"));

    return true;
}

// =============================================================================
// OBSŁUGA PRZERWANIA (ISR)
// =============================================================================

void IRAM_ATTR EncoderModule::handleInterrupt() {
    if (_instance == nullptr) return;

    // Debouncing sprzętowy
    uint32_t currentTime = micros();
    if (currentTime - _instance->_lastInterruptTime < _instance->_config.debounceTimeUS) {
        return;
    }
    _instance->_lastInterruptTime = currentTime;

    // Odczyt kierunku z pinu DT
    if (digitalRead(ENCODER_PIN_DT)) {
        _instance->_data.pulseCount++;
        _instance->_data.direction = 1; // Przód
    } else {
        _instance->_data.pulseCount--;
        _instance->_data.direction = 2; // Tył
    }
}

// =============================================================================
// AKTUALIZACJA (WYWOŁYWAĆ W LOOP)
// =============================================================================

void EncoderModule::update() {
    uint32_t currentTime = millis();

    // Oblicz dystans
    calculateDistance();

    // Oblicz prędkość co 100ms
    if (currentTime - _lastSpeedCalcTime >= 100) {
        calculateSpeed();
        _lastSpeedCalcTime = currentTime;
    }

    // Aktualizacja stanu przycisku
    updateButton();

    // Detekcja zatrzymania (brak impulsów przez 200ms)
    if (_data.pulseCount == _data.lastPulseCount &&
        currentTime - _data.lastUpdateTime > 200) {
        _data.direction = 0; // Stop
        _data.speedMPS = 0;
        _data.speedKMH = 0;
    }

    if (_data.pulseCount != _data.lastPulseCount) {
        _data.lastUpdateTime = currentTime;
        _data.lastPulseCount = _data.pulseCount;
    }
}

// =============================================================================
// OBLICZENIA
// =============================================================================

void EncoderModule::calculateDistance() {
    // Dystans = (impulsy / PPR) * obwód_koła
    float revolutions = (float)abs(_data.pulseCount) / (float)_config.pulsesPerRevolution;
    _data.distanceMM = revolutions * _config.wheelCircumferenceMM;
    _data.distanceM = _data.distanceMM / 1000.0f;
}

void EncoderModule::calculateSpeed() {
    // Różnica impulsów od ostatniego pomiaru
    int64_t pulseDiff = _data.pulseCount - _lastSpeedPulseCount;
    _lastSpeedPulseCount = _data.pulseCount;

    // Czas od ostatniego pomiaru (100ms = 0.1s)
    float timeSec = 0.1f;

    // Oblicz prędkość
    float revolutions = (float)abs(pulseDiff) / (float)_config.pulsesPerRevolution;
    float distanceMM = revolutions * _config.wheelCircumferenceMM;
    float distanceM = distanceMM / 1000.0f;

    _data.speedMPS = distanceM / timeSec;
    _data.speedKMH = _data.speedMPS * 3.6f;
}

void EncoderModule::updateButton() {
    static bool lastButtonState = true;
    bool currentState = digitalRead(ENCODER_PIN_SW);

    _data.buttonPressed = !currentState; // Aktywny stan niski

    // Detekcja zwolnienia przycisku
    if (!lastButtonState && currentState) {
        _data.buttonReleased = true;
    }

    lastButtonState = currentState;
}

// =============================================================================
// GETTERY
// =============================================================================

const EncoderData& EncoderModule::getData() const {
    return _data;
}

float EncoderModule::getDistanceMM() const {
    return _data.distanceMM;
}

float EncoderModule::getDistanceM() const {
    return _data.distanceM;
}

float EncoderModule::getSpeedKMH() const {
    return _data.speedKMH;
}

float EncoderModule::getSpeedMPS() const {
    return _data.speedMPS;
}

int64_t EncoderModule::getPulseCount() const {
    return _data.pulseCount;
}

bool EncoderModule::isButtonPressed() const {
    return _data.buttonPressed;
}

bool EncoderModule::wasButtonReleased() {
    if (_data.buttonReleased) {
        _data.buttonReleased = false;
        return true;
    }
    return false;
}

const EncoderConfig& EncoderModule::getConfig() const {
    return _config;
}

// =============================================================================
// SETTERY
// =============================================================================

void EncoderModule::setWheelDiameter(float diameterMM) {
    _config.wheelDiameterMM = diameterMM;
    _config.wheelCircumferenceMM = diameterMM * PI;
    DEBUG_PRINTF("[ENCODER] Nowa srednica kola: %.2f mm, obwod: %.2f mm\n",
                 diameterMM, _config.wheelCircumferenceMM);
}

void EncoderModule::setPPR(uint16_t ppr) {
    _config.pulsesPerRevolution = ppr;
    DEBUG_PRINTF("[ENCODER] Nowe PPR: %d\n", ppr);
}

// =============================================================================
// RESET
// =============================================================================

void EncoderModule::resetDistance() {
    noInterrupts();
    _data.pulseCount = 0;
    _data.lastPulseCount = 0;
    _lastSpeedPulseCount = 0;
    interrupts();

    _data.distanceMM = 0;
    _data.distanceM = 0;
    _data.speedMPS = 0;
    _data.speedKMH = 0;

    DEBUG_PRINTLN(F("[ENCODER] Dystans zresetowany"));
}

void EncoderModule::resetPulseCount() {
    noInterrupts();
    _data.pulseCount = 0;
    _data.lastPulseCount = 0;
    _lastSpeedPulseCount = 0;
    interrupts();

    DEBUG_PRINTLN(F("[ENCODER] Licznik impulsow zresetowany"));
}
