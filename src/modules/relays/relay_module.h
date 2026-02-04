/**
 * @file relay_module.h
 * @brief Moduł obsługi przekaźników (pistoletów do malowania)
 * @version 1.0.0
 * @date 2026-02-04
 *
 * Moduł odpowiada za:
 * - Sterowanie 6 przekaźnikami (pistoletami)
 * - Automatyczne malowanie wzorów (linia/przerwa)
 * - Zarządzanie wzorami malowania
 * - Synchronizacja z enkoderem
 */

#ifndef RELAY_MODULE_H
#define RELAY_MODULE_H

#include <Arduino.h>
#include "config.h"

// =============================================================================
// TYPY I STAŁE
// =============================================================================

/**
 * @brief Tryby pracy systemu malowania
 */
enum class PaintingMode : uint8_t {
    MANUAL = 0,         // Ręczne sterowanie
    AUTO_LINE,          // Automatyczne malowanie linii przerywanej
    AUTO_SOLID,         // Automatyczne malowanie linii ciągłej
    AUTO_DOUBLE,        // Podwójna linia
    STOPPED             // Zatrzymany
};

/**
 * @brief Stan cyklu malowania
 */
enum class PaintingState : uint8_t {
    IDLE = 0,           // Bezczynny
    PAINTING,           // Malowanie (pistolet aktywny)
    GAP                 // Przerwa (pistolet nieaktywny)
};

// =============================================================================
// STRUKTURY DANYCH
// =============================================================================

/**
 * @brief Konfiguracja wzoru malowania
 */
struct PaintingPattern {
    uint32_t lineLengthMM;      // Długość malowanego odcinka (mm)
    uint32_t gapLengthMM;       // Długość przerwy (mm)
    uint32_t lineWidthMM;       // Szerokość linii (mm) - informacyjne
    bool activePistols[NUM_RELAYS]; // Które pistolety są aktywne
};

/**
 * @brief Stan pojedynczego przekaźnika
 */
struct RelayState {
    bool isActive;              // Czy przekaźnik jest włączony
    bool isEnabled;             // Czy przekaźnik jest dozwolony do użycia
    uint32_t activationCount;   // Licznik aktywacji
    uint32_t totalActiveTimeMs; // Całkowity czas aktywności (ms)
};

/**
 * @brief Dane modułu przekaźników
 */
struct RelayModuleData {
    PaintingMode mode;          // Aktualny tryb pracy
    PaintingState state;        // Aktualny stan cyklu
    PaintingPattern pattern;    // Aktualny wzór
    RelayState relays[NUM_RELAYS]; // Stany przekaźników

    float currentPositionMM;    // Aktualna pozycja od początku cyklu
    float cycleStartMM;         // Pozycja początkowa cyklu
    uint32_t cycleCount;        // Licznik cykli (linia+przerwa)
    uint32_t totalPaintedMM;    // Całkowita długość namalowana (mm)

    bool isRunning;             // Czy system jest uruchomiony
    bool isPaused;              // Czy system jest wstrzymany
};

// =============================================================================
// KLASA MODUŁU PRZEKAŹNIKÓW
// =============================================================================

class RelayModule {
public:
    /**
     * @brief Konstruktor domyślny
     */
    RelayModule();

    /**
     * @brief Inicjalizacja modułu przekaźników
     * @return true jeśli sukces
     */
    bool begin();

    /**
     * @brief Aktualizacja stanu (wywoływać w loop z aktualnym dystansem)
     * @param currentDistanceMM Aktualny dystans z enkodera w mm
     */
    void update(float currentDistanceMM);

    // =========================================================================
    // STEROWANIE TRYBEM PRACY
    // =========================================================================

    /**
     * @brief Ustawienie trybu pracy
     * @param mode Nowy tryb pracy
     */
    void setMode(PaintingMode mode);

    /**
     * @brief Pobranie aktualnego trybu pracy
     * @return Aktualny tryb
     */
    PaintingMode getMode() const;

    /**
     * @brief Start automatycznego malowania
     */
    void start();

    /**
     * @brief Stop automatycznego malowania
     */
    void stop();

    /**
     * @brief Pauza automatycznego malowania
     */
    void pause();

    /**
     * @brief Wznowienie po pauzie
     */
    void resume();

    /**
     * @brief Reset liczników i pozycji
     */
    void reset();

    // =========================================================================
    // STEROWANIE RĘCZNE PRZEKAŹNIKAMI
    // =========================================================================

    /**
     * @brief Włączenie pojedynczego przekaźnika
     * @param index Indeks przekaźnika (0-5)
     */
    void activateRelay(uint8_t index);

    /**
     * @brief Wyłączenie pojedynczego przekaźnika
     * @param index Indeks przekaźnika (0-5)
     */
    void deactivateRelay(uint8_t index);

    /**
     * @brief Przełączenie stanu przekaźnika
     * @param index Indeks przekaźnika (0-5)
     */
    void toggleRelay(uint8_t index);

    /**
     * @brief Włączenie wszystkich aktywnych przekaźników
     */
    void activateAll();

    /**
     * @brief Wyłączenie wszystkich przekaźników
     */
    void deactivateAll();

    /**
     * @brief Ustawienie które pistolety są aktywne
     * @param pistolMask Maska bitowa (bit 0 = pistolet 1, itd.)
     */
    void setActivePistols(uint8_t pistolMask);

    /**
     * @brief Włączenie/wyłączenie pojedynczego pistoletu w masce
     * @param index Indeks pistoletu (0-5)
     * @param enabled Czy włączony
     */
    void enablePistol(uint8_t index, bool enabled);

    // =========================================================================
    // KONFIGURACJA WZORU
    // =========================================================================

    /**
     * @brief Ustawienie długości malowanego odcinka
     * @param lengthMM Długość w mm
     */
    void setLineLength(uint32_t lengthMM);

    /**
     * @brief Ustawienie długości przerwy
     * @param lengthMM Długość w mm
     */
    void setGapLength(uint32_t lengthMM);

    /**
     * @brief Ustawienie wzoru malowania
     * @param pattern Struktura wzoru
     */
    void setPattern(const PaintingPattern& pattern);

    /**
     * @brief Pobranie aktualnego wzoru
     * @return Referencja do wzoru
     */
    const PaintingPattern& getPattern() const;

    // =========================================================================
    // GETTERY
    // =========================================================================

    /**
     * @brief Pobranie pełnych danych modułu
     * @return Referencja do danych
     */
    const RelayModuleData& getData() const;

    /**
     * @brief Sprawdzenie czy system działa
     * @return true jeśli działa
     */
    bool isRunning() const;

    /**
     * @brief Sprawdzenie stanu przekaźnika
     * @param index Indeks przekaźnika (0-5)
     * @return true jeśli aktywny
     */
    bool isRelayActive(uint8_t index) const;

    /**
     * @brief Pobranie liczby cykli
     * @return Liczba ukończonych cykli
     */
    uint32_t getCycleCount() const;

    /**
     * @brief Pobranie całkowitej długości namalowanej
     * @return Długość w mm
     */
    uint32_t getTotalPaintedMM() const;

    /**
     * @brief Pobranie maski aktywnych przekaźników
     * @return Maska bitowa
     */
    uint8_t getActiveRelayMask() const;

private:
    RelayModuleData _data;

    uint32_t _lastUpdateTime;
    float _lastDistanceMM;

    /**
     * @brief Fizyczne ustawienie stanu przekaźnika
     * @param index Indeks przekaźnika
     * @param state Stan (true = włączony)
     */
    void setRelayState(uint8_t index, bool state);

    /**
     * @brief Logika automatycznego malowania
     * @param distanceDelta Różnica dystansu od ostatniej aktualizacji
     */
    void processAutoMode(float distanceDelta);

    /**
     * @brief Aktualizacja statystyk przekaźników
     */
    void updateRelayStats();
};

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

extern RelayModule Relays;

#endif // RELAY_MODULE_H
