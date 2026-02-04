/**
 * @file patterns_module.cpp
 * @brief Implementacja modułu wzorców malowania
 * @version 1.1.0
 * @date 2026-02-04
 */

#include "patterns_module.h"
#include "config.h"

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

PatternsModule Patterns;

// =============================================================================
// PREDEFINIOWANE WZORCE MALOWANIA
// =============================================================================
/**
 * Tabela wzorców zgodna z polskimi przepisami:
 *
 * ┌────────┬─────────┬──────────┬───────────┬───────────────────────────┐
 * │ Wzorzec│ Linia(m)│ Przerwa  │ Szerokość │ Opis                      │
 * │        │         │   (m)    │   (cm)    │                           │
 * ├────────┼─────────┼──────────┼───────────┼───────────────────────────┤
 * │ P-1a   │   4.0   │   8.0    │    12     │ Przerywana długa          │
 * │ P-1b   │   2.0   │   4.0    │    12     │ Przerywana krótka         │
 * │ P-1c   │   2.0   │   2.0    │    12     │ Wydzielająca              │
 * │ P-1d   │   1.0   │   1.0    │    12     │ Prowadząca wąska          │
 * │ P-1e   │   1.0   │   1.0    │    24     │ Prowadząca szeroka        │
 * ├────────┼─────────┼──────────┼───────────┼───────────────────────────┤
 * │ P-2a   │  Ciągła │    -     │    12     │ Ciągła wąska              │
 * │ P-2b   │  Ciągła │    -     │    24     │ Ciągła szeroka            │
 * ├────────┼─────────┼──────────┼───────────┼───────────────────────────┤
 * │ P-3a   │   4.0   │   2.0    │    12     │ Przekraczalna długa       │
 * │ P-3b   │   1.0   │   1.0    │    12     │ Przekraczalna krótka      │
 * │ P-4    │  Ciągła │    -     │    24     │ Podwójna ciągła           │
 * ├────────┼─────────┼──────────┼───────────┼───────────────────────────┤
 * │ P-6    │   4.0   │   2.0    │    12     │ Ostrzegawcza              │
 * ├────────┼─────────┼──────────┼───────────┼───────────────────────────┤
 * │ P-7a   │   1.0   │   1.0    │    24     │ Krawędziowa przeryw. szer.│
 * │ P-7b   │  Ciągła │    -     │    24     │ Krawędziowa ciągła szer.  │
 * │ P-7c   │   1.0   │   1.0    │    12     │ Krawędziowa przeryw. wąska│
 * │ P-7d   │  Ciągła │    -     │    12     │ Krawędziowa ciągła wąska  │
 * └────────┴─────────┴──────────┴───────────┴───────────────────────────┘
 */

const PaintPattern PatternsModule::_patterns[NUM_PATTERNS] = {
    // P-1: Linie przerywane
    {
        PatternId::P_1A,
        "P-1a",
        "Przerywana dluga",
        LineType::DASHED,
        4000,   // 4.0m linia
        8000,   // 8.0m przerwa
        120,    // 12cm szerokość
        false
    },
    {
        PatternId::P_1B,
        "P-1b",
        "Przerywana krotka",
        LineType::DASHED,
        2000,   // 2.0m linia
        4000,   // 4.0m przerwa
        120,    // 12cm szerokość
        false
    },
    {
        PatternId::P_1C,
        "P-1c",
        "Wydzielajaca",
        LineType::DASHED,
        2000,   // 2.0m linia
        2000,   // 2.0m przerwa
        120,    // 12cm szerokość
        false
    },
    {
        PatternId::P_1D,
        "P-1d",
        "Prowadzaca waska",
        LineType::DASHED,
        1000,   // 1.0m linia
        1000,   // 1.0m przerwa
        120,    // 12cm szerokość
        false
    },
    {
        PatternId::P_1E,
        "P-1e",
        "Prowadzaca szeroka",
        LineType::DASHED,
        1000,   // 1.0m linia
        1000,   // 1.0m przerwa
        240,    // 24cm szerokość
        false
    },

    // P-2: Linie ciągłe
    {
        PatternId::P_2A,
        "P-2a",
        "Ciagla waska",
        LineType::SOLID,
        0,      // ciągła
        0,      // ciągła
        120,    // 12cm szerokość
        false
    },
    {
        PatternId::P_2B,
        "P-2b",
        "Ciagla szeroka",
        LineType::SOLID,
        0,      // ciągła
        0,      // ciągła
        240,    // 24cm szerokość
        false
    },

    // P-3: Linie przekraczalne
    {
        PatternId::P_3A,
        "P-3a",
        "Przekraczalna dluga",
        LineType::DASHED,
        4000,   // 4.0m linia
        2000,   // 2.0m przerwa
        120,    // 12cm szerokość
        false
    },
    {
        PatternId::P_3B,
        "P-3b",
        "Przekraczalna krotka",
        LineType::DASHED,
        1000,   // 1.0m linia
        1000,   // 1.0m przerwa
        120,    // 12cm szerokość
        false
    },

    // P-4: Podwójna ciągła
    {
        PatternId::P_4,
        "P-4",
        "Podwojna ciagla",
        LineType::DOUBLE_SOLID,
        0,      // ciągła
        0,      // ciągła
        240,    // 24cm szerokość (2x12cm)
        true    // podwójna
    },

    // P-6: Ostrzegawcza
    {
        PatternId::P_6,
        "P-6",
        "Ostrzegawcza",
        LineType::DASHED,
        4000,   // 4.0m linia
        2000,   // 2.0m przerwa
        120,    // 12cm szerokość
        false
    },

    // P-7: Linie krawędziowe
    {
        PatternId::P_7A,
        "P-7a",
        "Krawedz. przeryw. szer.",
        LineType::DASHED,
        1000,   // 1.0m linia
        1000,   // 1.0m przerwa
        240,    // 24cm szerokość
        false
    },
    {
        PatternId::P_7B,
        "P-7b",
        "Krawedz. ciagla szer.",
        LineType::SOLID,
        0,      // ciągła
        0,      // ciągła
        240,    // 24cm szerokość
        false
    },
    {
        PatternId::P_7C,
        "P-7c",
        "Krawedz. przeryw. waska",
        LineType::DASHED,
        1000,   // 1.0m linia
        1000,   // 1.0m przerwa
        120,    // 12cm szerokość
        false
    },
    {
        PatternId::P_7D,
        "P-7d",
        "Krawedz. ciagla waska",
        LineType::SOLID,
        0,      // ciągła
        0,      // ciągła
        120,    // 12cm szerokość
        false
    }
};

// =============================================================================
// KONSTRUKTOR
// =============================================================================

PatternsModule::PatternsModule() {
    _activeIndex = 0;  // Domyślnie P-1a
    _onChangeCallback = nullptr;
}

// =============================================================================
// INICJALIZACJA
// =============================================================================

bool PatternsModule::begin() {
    DEBUG_PRINTLN(F("[PATTERNS] Inicjalizacja modulu wzorcow..."));
    DEBUG_PRINTF("[PATTERNS] Zaladowano %d wzorcow\n", NUM_PATTERNS);

    // Wypisz wszystkie wzorce
    for (uint8_t i = 0; i < NUM_PATTERNS; i++) {
        DEBUG_PRINTF("[PATTERNS]   %s: %s (L=%dmm, G=%dmm, W=%dmm)\n",
            _patterns[i].name,
            _patterns[i].description,
            _patterns[i].lineLengthMM,
            _patterns[i].gapLengthMM,
            _patterns[i].lineWidthMM
        );
    }

    DEBUG_PRINTF("[PATTERNS] Aktywny wzorzec: %s\n", _patterns[_activeIndex].name);
    DEBUG_PRINTLN(F("[PATTERNS] Modul zainicjalizowany pomyslnie"));

    return true;
}

// =============================================================================
// WYBÓR WZORCA
// =============================================================================

bool PatternsModule::setPattern(PatternId id) {
    // Znajdź indeks dla danego ID
    for (uint8_t i = 0; i < NUM_PATTERNS; i++) {
        if (_patterns[i].id == id) {
            return setPatternByIndex(i);
        }
    }
    return false;
}

bool PatternsModule::setPatternByIndex(uint8_t index) {
    if (index >= NUM_PATTERNS) {
        return false;
    }

    if (_activeIndex != index) {
        _activeIndex = index;
        DEBUG_PRINTF("[PATTERNS] Zmiana wzorca na: %s (%s)\n",
            _patterns[_activeIndex].name,
            _patterns[_activeIndex].description
        );
        notifyPatternChange();
    }

    return true;
}

bool PatternsModule::setPatternByName(const char* name) {
    if (!name) return false;

    for (uint8_t i = 0; i < NUM_PATTERNS; i++) {
        if (strcasecmp(_patterns[i].name, name) == 0) {
            return setPatternByIndex(i);
        }
    }
    return false;
}

void PatternsModule::nextPattern() {
    uint8_t next = _activeIndex + 1;
    if (next >= NUM_PATTERNS) {
        next = 0;
    }
    setPatternByIndex(next);
}

void PatternsModule::prevPattern() {
    uint8_t prev = (_activeIndex == 0) ? NUM_PATTERNS - 1 : _activeIndex - 1;
    setPatternByIndex(prev);
}

// =============================================================================
// GETTERY AKTYWNEGO WZORCA
// =============================================================================

const PaintPattern* PatternsModule::getActivePattern() const {
    return &_patterns[_activeIndex];
}

PatternId PatternsModule::getActivePatternId() const {
    return _patterns[_activeIndex].id;
}

uint8_t PatternsModule::getActivePatternIndex() const {
    return _activeIndex;
}

const char* PatternsModule::getActivePatternName() const {
    return _patterns[_activeIndex].name;
}

const char* PatternsModule::getActivePatternDescription() const {
    return _patterns[_activeIndex].description;
}

bool PatternsModule::isActiveSolid() const {
    return _patterns[_activeIndex].lineType != LineType::DASHED;
}

uint32_t PatternsModule::getActiveLineLength() const {
    return _patterns[_activeIndex].lineLengthMM;
}

uint32_t PatternsModule::getActiveGapLength() const {
    return _patterns[_activeIndex].gapLengthMM;
}

uint16_t PatternsModule::getActiveLineWidth() const {
    return _patterns[_activeIndex].lineWidthMM;
}

// =============================================================================
// DOSTĘP DO WSZYSTKICH WZORCÓW
// =============================================================================

const PaintPattern* PatternsModule::getPatternByIndex(uint8_t index) const {
    if (index >= NUM_PATTERNS) return nullptr;
    return &_patterns[index];
}

const PaintPattern* PatternsModule::getPatternById(PatternId id) const {
    for (uint8_t i = 0; i < NUM_PATTERNS; i++) {
        if (_patterns[i].id == id) {
            return &_patterns[i];
        }
    }
    return nullptr;
}

uint8_t PatternsModule::getPatternCount() const {
    return NUM_PATTERNS;
}

const PaintPattern* PatternsModule::getAllPatterns() const {
    return _patterns;
}

// =============================================================================
// CALLBACK
// =============================================================================

void PatternsModule::setOnPatternChange(PatternChangeCallback callback) {
    _onChangeCallback = callback;
}

void PatternsModule::notifyPatternChange() {
    if (_onChangeCallback) {
        _onChangeCallback(_patterns[_activeIndex].id, &_patterns[_activeIndex]);
    }
}
