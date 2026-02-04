/**
 * @file keypad_module.h
 * @brief Moduł obsługi klawiatury matrycowej 4x4 do wyboru wzorców
 * @version 1.1.0
 * @date 2026-02-04
 *
 * Układ klawiatury (16 przycisków):
 *        C1    C2    C3    C4
 *       ┌─────┬─────┬─────┬─────┐
 *   R1  │P-1a │P-1b │P-1c │P-1d │
 *       ├─────┼─────┼─────┼─────┤
 *   R2  │P-1e │P-2a │P-2b │P-3a │
 *       ├─────┼─────┼─────┼─────┤
 *   R3  │P-3b │ P-4 │ P-6 │P-7a │
 *       ├─────┼─────┼─────┼─────┤
 *   R4  │P-7b │P-7c │P-7d │START│
 *       └─────┴─────┴─────┴─────┘
 */

#ifndef KEYPAD_MODULE_H
#define KEYPAD_MODULE_H

#include <Arduino.h>
#include "pins_config.h"

// =============================================================================
// STAŁE
// =============================================================================

/**
 * @brief Czas debouncingu w ms
 */
#define KEYPAD_DEBOUNCE_MS      50

/**
 * @brief Interwał skanowania klawiatury w ms
 */
#define KEYPAD_SCAN_INTERVAL_MS 20

/**
 * @brief Kody klawiszy (indeks w tablicy 0-15)
 */
enum class KeyCode : uint8_t {
    KEY_P1A = 0,    // Wiersz 1
    KEY_P1B = 1,
    KEY_P1C = 2,
    KEY_P1D = 3,
    KEY_P1E = 4,    // Wiersz 2
    KEY_P2A = 5,
    KEY_P2B = 6,
    KEY_P3A = 7,
    KEY_P3B = 8,    // Wiersz 3
    KEY_P4  = 9,
    KEY_P6  = 10,
    KEY_P7A = 11,
    KEY_P7B = 12,   // Wiersz 4
    KEY_P7C = 13,
    KEY_P7D = 14,
    KEY_START = 15, // Przycisk START/STOP
    KEY_NONE = 255
};

// =============================================================================
// STRUKTURA DANYCH
// =============================================================================

/**
 * @brief Stan klawiatury
 */
struct KeypadState {
    KeyCode lastKey;            // Ostatnio naciśnięty klawisz
    KeyCode currentKey;         // Aktualnie naciśnięty klawisz
    bool keyPressed;            // Flaga nowego naciśnięcia
    bool keyReleased;           // Flaga zwolnienia
    uint32_t lastPressTime;     // Czas ostatniego naciśnięcia
    uint32_t pressCount;        // Licznik naciśnięć
};

// =============================================================================
// KLASA MODUŁU KLAWIATURY
// =============================================================================

class KeypadModule {
public:
    /**
     * @brief Konstruktor
     */
    KeypadModule();

    /**
     * @brief Inicjalizacja modułu
     * @return true jeśli sukces
     */
    bool begin();

    /**
     * @brief Aktualizacja stanu klawiatury (wywoływać w loop)
     */
    void update();

    // =========================================================================
    // ODCZYT STANU
    // =========================================================================

    /**
     * @brief Sprawdzenie czy klawisz został naciśnięty (jednorazowe)
     * @return true jeśli nowe naciśnięcie
     */
    bool isKeyPressed();

    /**
     * @brief Sprawdzenie czy klawisz został zwolniony (jednorazowe)
     * @return true jeśli zwolniony
     */
    bool isKeyReleased();

    /**
     * @brief Pobranie kodu ostatnio naciśniętego klawisza
     * @return Kod klawisza lub KEY_NONE
     */
    KeyCode getLastKey() const;

    /**
     * @brief Pobranie kodu aktualnie naciśniętego klawisza
     * @return Kod klawisza lub KEY_NONE
     */
    KeyCode getCurrentKey() const;

    /**
     * @brief Sprawdzenie czy konkretny klawisz jest naciśnięty
     * @param key Kod klawisza
     * @return true jeśli naciśnięty
     */
    bool isKeyDown(KeyCode key) const;

    /**
     * @brief Pobranie indeksu wzorca dla klawisza (0-14)
     * @param key Kod klawisza
     * @return Indeks wzorca lub 255 dla KEY_START/KEY_NONE
     */
    uint8_t keyToPatternIndex(KeyCode key) const;

    /**
     * @brief Sprawdzenie czy klawisz to przycisk START
     * @param key Kod klawisza
     * @return true jeśli START
     */
    bool isStartKey(KeyCode key) const;

    /**
     * @brief Pobranie pełnego stanu klawiatury
     * @return Referencja do struktury stanu
     */
    const KeypadState& getState() const;

    // =========================================================================
    // CALLBACK
    // =========================================================================

    /**
     * @brief Typ callbacku dla naciśnięcia klawisza
     */
    typedef void (*KeyPressCallback)(KeyCode key);

    /**
     * @brief Ustawienie callbacku naciśnięcia
     * @param callback Funkcja callback
     */
    void setOnKeyPress(KeyPressCallback callback);

private:
    KeypadState _state;
    KeyPressCallback _onKeyPressCallback;

    uint32_t _lastScanTime;
    uint8_t _rawKeys[KEYPAD_ROWS];          // Surowy stan wierszy
    uint8_t _debouncedKeys[KEYPAD_ROWS];    // Stan po debouncingu
    uint8_t _lastDebouncedKeys[KEYPAD_ROWS]; // Poprzedni stan

    /**
     * @brief Skanowanie matrycy klawiatury
     */
    void scanMatrix();

    /**
     * @brief Przetwarzanie zmiany stanu
     */
    void processKeyChange();

    /**
     * @brief Konwersja pozycji w matrycy na kod klawisza
     * @param row Wiersz (0-3)
     * @param col Kolumna (0-3)
     * @return Kod klawisza
     */
    KeyCode matrixToKeyCode(uint8_t row, uint8_t col) const;
};

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

extern KeypadModule Keypad;

#endif // KEYPAD_MODULE_H
