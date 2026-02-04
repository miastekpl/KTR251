/**
 * @file encoder_module.h
 * @brief Moduł obsługi enkodera KY-040 dla pomiaru dystansu
 * @version 1.0.0
 * @date 2026-02-04
 *
 * Moduł odpowiada za:
 * - Obsługę enkodera obrotowego KY-040
 * - Pomiar przebytego dystansu w mm
 * - Obliczanie prędkości w m/s i km/h
 * - Obsługę przycisku enkodera
 */

#ifndef ENCODER_MODULE_H
#define ENCODER_MODULE_H

#include <Arduino.h>
#include "config.h"

// =============================================================================
// STRUKTURY DANYCH
// =============================================================================

/**
 * @brief Struktura przechowująca dane enkodera
 */
struct EncoderData {
    volatile int64_t pulseCount;    // Licznik impulsów (może być ujemny)
    volatile int64_t lastPulseCount; // Poprzednia wartość licznika
    float distanceMM;               // Przebyty dystans w mm
    float distanceM;                // Przebyty dystans w m
    float speedMPS;                 // Prędkość w m/s
    float speedKMH;                 // Prędkość w km/h
    uint32_t lastUpdateTime;        // Czas ostatniej aktualizacji (ms)
    bool buttonPressed;             // Stan przycisku
    bool buttonReleased;            // Flaga zwolnienia przycisku
    uint8_t direction;              // Kierunek: 0=stop, 1=przód, 2=tył
};

/**
 * @brief Konfiguracja enkodera
 */
struct EncoderConfig {
    uint16_t pulsesPerRevolution;   // Impulsy na obrót
    float wheelDiameterMM;          // Średnica koła w mm
    float wheelCircumferenceMM;     // Obwód koła w mm
    uint16_t debounceTimeUS;        // Czas debouncingu w us
};

// =============================================================================
// KLASA MODUŁU ENKODERA
// =============================================================================

class EncoderModule {
public:
    /**
     * @brief Konstruktor domyślny
     */
    EncoderModule();

    /**
     * @brief Inicjalizacja modułu enkodera
     * @return true jeśli sukces, false w przypadku błędu
     */
    bool begin();

    /**
     * @brief Aktualizacja stanu enkodera (wywoływać w loop)
     */
    void update();

    /**
     * @brief Resetowanie liczników dystansu
     */
    void resetDistance();

    /**
     * @brief Resetowanie tylko licznika impulsów
     */
    void resetPulseCount();

    /**
     * @brief Pobranie aktualnych danych enkodera
     * @return Referencja do struktury danych
     */
    const EncoderData& getData() const;

    /**
     * @brief Pobranie dystansu w mm
     * @return Dystans w milimetrach
     */
    float getDistanceMM() const;

    /**
     * @brief Pobranie dystansu w metrach
     * @return Dystans w metrach
     */
    float getDistanceM() const;

    /**
     * @brief Pobranie prędkości w km/h
     * @return Prędkość w km/h
     */
    float getSpeedKMH() const;

    /**
     * @brief Pobranie prędkości w m/s
     * @return Prędkość w m/s
     */
    float getSpeedMPS() const;

    /**
     * @brief Pobranie liczby impulsów
     * @return Liczba impulsów
     */
    int64_t getPulseCount() const;

    /**
     * @brief Sprawdzenie czy przycisk został naciśnięty
     * @return true jeśli przycisk naciśnięty
     */
    bool isButtonPressed() const;

    /**
     * @brief Sprawdzenie czy przycisk został zwolniony (jednorazowe)
     * @return true jeśli przycisk właśnie zwolniony
     */
    bool wasButtonReleased();

    /**
     * @brief Ustawienie parametrów koła
     * @param diameterMM Średnica koła w mm
     */
    void setWheelDiameter(float diameterMM);

    /**
     * @brief Ustawienie PPR enkodera
     * @param ppr Impulsy na obrót
     */
    void setPPR(uint16_t ppr);

    /**
     * @brief Pobranie konfiguracji enkodera
     * @return Referencja do struktury konfiguracji
     */
    const EncoderConfig& getConfig() const;

    /**
     * @brief Statyczna funkcja obsługi przerwania (ISR)
     */
    static void IRAM_ATTR handleInterrupt();

private:
    EncoderData _data;
    EncoderConfig _config;

    static EncoderModule* _instance;

    uint32_t _lastSpeedCalcTime;
    int64_t _lastSpeedPulseCount;

    volatile uint32_t _lastInterruptTime;

    /**
     * @brief Przeliczenie impulsów na dystans
     */
    void calculateDistance();

    /**
     * @brief Obliczenie prędkości
     */
    void calculateSpeed();

    /**
     * @brief Aktualizacja stanu przycisku
     */
    void updateButton();
};

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

extern EncoderModule Encoder;

#endif // ENCODER_MODULE_H
