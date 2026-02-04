/**
 * @file patterns_module.h
 * @brief Moduł predefiniowanych wzorców malowania pasów drogowych
 * @version 1.1.0
 * @date 2026-02-04
 *
 * Wzorce zgodne z polskimi przepisami drogowymi:
 * - P-1: Linie przerywane (a-e)
 * - P-2: Linie ciągłe (a-b)
 * - P-3: Linie przekraczalne (a-b)
 * - P-4: Podwójna ciągła
 * - P-6: Ostrzegawcza
 * - P-7: Krawędziowe (a-d)
 */

#ifndef PATTERNS_MODULE_H
#define PATTERNS_MODULE_H

#include <Arduino.h>

// =============================================================================
// STAŁE I TYPY
// =============================================================================

/**
 * @brief Liczba predefiniowanych wzorców
 */
#define NUM_PATTERNS            15

/**
 * @brief Maksymalna długość nazwy wzorca
 */
#define PATTERN_NAME_MAX_LEN    8

/**
 * @brief Maksymalna długość opisu wzorca
 */
#define PATTERN_DESC_MAX_LEN    32

/**
 * @brief Identyfikatory wzorców (kolejność jak na klawiaturze)
 */
enum class PatternId : uint8_t {
    P_1A = 0,       // Przerywana długa
    P_1B,           // Przerywana krótka
    P_1C,           // Wydzielająca
    P_1D,           // Prowadząca wąska
    P_1E,           // Prowadząca szeroka
    P_2A,           // Ciągła wąska
    P_2B,           // Ciągła szeroka
    P_3A,           // Przekraczalna długa
    P_3B,           // Przekraczalna krótka
    P_4,            // Podwójna ciągła
    P_6,            // Ostrzegawcza
    P_7A,           // Krawędziowa przerywana szeroka
    P_7B,           // Krawędziowa ciągła szeroka
    P_7C,           // Krawędziowa przerywana wąska
    P_7D,           // Krawędziowa ciągła wąska
    PATTERN_NONE = 255  // Brak wzorca (tryb ręczny)
};

/**
 * @brief Typ linii
 */
enum class LineType : uint8_t {
    DASHED = 0,     // Przerywana
    SOLID,          // Ciągła
    DOUBLE_SOLID    // Podwójna ciągła
};

// =============================================================================
// STRUKTURA WZORCA
// =============================================================================

/**
 * @brief Definicja pojedynczego wzorca malowania
 */
struct PaintPattern {
    PatternId id;                           // Identyfikator wzorca
    char name[PATTERN_NAME_MAX_LEN];        // Nazwa (np. "P-1a")
    char description[PATTERN_DESC_MAX_LEN]; // Opis (np. "Przerywana długa")
    LineType lineType;                      // Typ linii
    uint32_t lineLengthMM;                  // Długość linii w mm (0 dla ciągłej)
    uint32_t gapLengthMM;                   // Długość przerwy w mm (0 dla ciągłej)
    uint16_t lineWidthMM;                   // Szerokość linii w mm
    bool isDouble;                          // Czy podwójna linia
};

// =============================================================================
// KLASA MODUŁU WZORCÓW
// =============================================================================

/**
 * @brief Klasa zarządzająca wzorcami malowania
 *
 * Wzorce są predefiniowane i nie można ich modyfikować w runtime.
 * Można tylko wybrać aktywny wzorzec.
 */
class PatternsModule {
public:
    /**
     * @brief Konstruktor
     */
    PatternsModule();

    /**
     * @brief Inicjalizacja modułu
     * @return true jeśli sukces
     */
    bool begin();

    // =========================================================================
    // WYBÓR WZORCA
    // =========================================================================

    /**
     * @brief Ustawienie aktywnego wzorca po ID
     * @param id Identyfikator wzorca
     * @return true jeśli wzorzec istnieje
     */
    bool setPattern(PatternId id);

    /**
     * @brief Ustawienie aktywnego wzorca po indeksie (0-14)
     * @param index Indeks wzorca
     * @return true jeśli indeks poprawny
     */
    bool setPatternByIndex(uint8_t index);

    /**
     * @brief Ustawienie aktywnego wzorca po nazwie
     * @param name Nazwa wzorca (np. "P-1a")
     * @return true jeśli wzorzec znaleziony
     */
    bool setPatternByName(const char* name);

    /**
     * @brief Następny wzorzec (cyklicznie)
     */
    void nextPattern();

    /**
     * @brief Poprzedni wzorzec (cyklicznie)
     */
    void prevPattern();

    // =========================================================================
    // GETTERY AKTYWNEGO WZORCA
    // =========================================================================

    /**
     * @brief Pobranie aktywnego wzorca
     * @return Wskaźnik do aktywnego wzorca (lub nullptr)
     */
    const PaintPattern* getActivePattern() const;

    /**
     * @brief Pobranie ID aktywnego wzorca
     * @return ID wzorca
     */
    PatternId getActivePatternId() const;

    /**
     * @brief Pobranie indeksu aktywnego wzorca
     * @return Indeks (0-14) lub 255 dla NONE
     */
    uint8_t getActivePatternIndex() const;

    /**
     * @brief Pobranie nazwy aktywnego wzorca
     * @return Nazwa (np. "P-1a")
     */
    const char* getActivePatternName() const;

    /**
     * @brief Pobranie opisu aktywnego wzorca
     * @return Opis
     */
    const char* getActivePatternDescription() const;

    /**
     * @brief Czy aktywny wzorzec to linia ciągła
     * @return true jeśli ciągła
     */
    bool isActiveSolid() const;

    /**
     * @brief Pobranie długości linii aktywnego wzorca
     * @return Długość w mm (0 dla ciągłej)
     */
    uint32_t getActiveLineLength() const;

    /**
     * @brief Pobranie długości przerwy aktywnego wzorca
     * @return Długość w mm (0 dla ciągłej)
     */
    uint32_t getActiveGapLength() const;

    /**
     * @brief Pobranie szerokości linii aktywnego wzorca
     * @return Szerokość w mm
     */
    uint16_t getActiveLineWidth() const;

    // =========================================================================
    // DOSTĘP DO WSZYSTKICH WZORCÓW
    // =========================================================================

    /**
     * @brief Pobranie wzorca po indeksie
     * @param index Indeks (0-14)
     * @return Wskaźnik do wzorca lub nullptr
     */
    const PaintPattern* getPatternByIndex(uint8_t index) const;

    /**
     * @brief Pobranie wzorca po ID
     * @param id Identyfikator wzorca
     * @return Wskaźnik do wzorca lub nullptr
     */
    const PaintPattern* getPatternById(PatternId id) const;

    /**
     * @brief Liczba dostępnych wzorców
     * @return NUM_PATTERNS (15)
     */
    uint8_t getPatternCount() const;

    /**
     * @brief Pobranie tablicy wszystkich wzorców
     * @return Wskaźnik do tablicy
     */
    const PaintPattern* getAllPatterns() const;

    // =========================================================================
    // CALLBACK DLA ZMIANY WZORCA
    // =========================================================================

    /**
     * @brief Typ callbacku wywoływanego przy zmianie wzorca
     */
    typedef void (*PatternChangeCallback)(PatternId newPattern, const PaintPattern* pattern);

    /**
     * @brief Ustawienie callbacku zmiany wzorca
     * @param callback Funkcja callback
     */
    void setOnPatternChange(PatternChangeCallback callback);

private:
    static const PaintPattern _patterns[NUM_PATTERNS];
    uint8_t _activeIndex;
    PatternChangeCallback _onChangeCallback;

    /**
     * @brief Wywołanie callbacku zmiany
     */
    void notifyPatternChange();
};

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

extern PatternsModule Patterns;

#endif // PATTERNS_MODULE_H
