/**
 * @file display_module.h
 * @brief Moduł obsługi wyświetlacza ILI9341
 * @version 1.0.0
 * @date 2026-02-04
 *
 * Moduł odpowiada za:
 * - Inicjalizację i obsługę wyświetlacza TFT
 * - Wyświetlanie ekranu głównego z danymi
 * - Zarządzanie jasnością podświetlenia
 * - Wyświetlanie menu i statusu
 */

#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"

// =============================================================================
// KOLORY (format RGB565)
// =============================================================================

#define COLOR_BLACK         0x0000
#define COLOR_WHITE         0xFFFF
#define COLOR_RED           0xF800
#define COLOR_GREEN         0x07E0
#define COLOR_BLUE          0x001F
#define COLOR_YELLOW        0xFFE0
#define COLOR_CYAN          0x07FF
#define COLOR_MAGENTA       0xF81F
#define COLOR_ORANGE        0xFD20
#define COLOR_GRAY          0x8410
#define COLOR_DARK_GRAY     0x4208
#define COLOR_LIGHT_GRAY    0xC618

// Kolory interfejsu
#define COLOR_BACKGROUND    0x0000      // Czarne tło
#define COLOR_HEADER_BG     0x000F      // Ciemnoniebieski nagłówek
#define COLOR_TEXT_PRIMARY  0xFFFF      // Biały tekst główny
#define COLOR_TEXT_SECONDARY 0x8410     // Szary tekst pomocniczy
#define COLOR_ACCENT        0x07E0      // Zielony akcent
#define COLOR_WARNING       0xFD20      // Pomarańczowe ostrzeżenie
#define COLOR_ERROR         0xF800      // Czerwony błąd
#define COLOR_ACTIVE        0x07E0      // Zielony aktywny
#define COLOR_INACTIVE      0x4208      // Szary nieaktywny

// =============================================================================
// TYPY I STAŁE
// =============================================================================

/**
 * @brief Ekrany wyświetlacza
 */
enum class DisplayScreen : uint8_t {
    MAIN = 0,           // Ekran główny z danymi
    SETTINGS,           // Ustawienia
    PATTERN,            // Konfiguracja wzoru
    STATUS,             // Status systemu
    DIAGNOSTIC          // Diagnostyka
};

/**
 * @brief Dane do wyświetlenia
 */
struct DisplayData {
    float distanceM;            // Dystans w metrach
    float speedKMH;             // Prędkość w km/h
    uint32_t cycleCount;        // Liczba cykli
    uint32_t totalPaintedM;     // Namalowano (metry)
    bool isRunning;             // Czy system działa
    bool isPaused;              // Czy pauza
    uint8_t activePistols;      // Maska aktywnych pistoletów
    uint8_t mode;               // Tryb pracy
    char statusText[32];        // Tekst statusu
    char ipAddress[16];         // Adres IP
};

// =============================================================================
// KLASA MODUŁU WYŚWIETLACZA
// =============================================================================

class DisplayModule {
public:
    /**
     * @brief Konstruktor
     */
    DisplayModule();

    /**
     * @brief Inicjalizacja wyświetlacza
     * @return true jeśli sukces
     */
    bool begin();

    /**
     * @brief Aktualizacja wyświetlacza (wywoływać w loop)
     */
    void update();

    /**
     * @brief Wymuszenie odświeżenia całego ekranu
     */
    void refresh();

    /**
     * @brief Ustawienie aktualnego ekranu
     * @param screen Ekran do wyświetlenia
     */
    void setScreen(DisplayScreen screen);

    /**
     * @brief Pobranie aktualnego ekranu
     * @return Aktualny ekran
     */
    DisplayScreen getScreen() const;

    /**
     * @brief Następny ekran
     */
    void nextScreen();

    /**
     * @brief Poprzedni ekran
     */
    void prevScreen();

    /**
     * @brief Aktualizacja danych do wyświetlenia
     * @param data Struktura z danymi
     */
    void setData(const DisplayData& data);

    /**
     * @brief Ustawienie tekstu statusu
     * @param text Tekst do wyświetlenia
     */
    void setStatusText(const char* text);

    /**
     * @brief Ustawienie adresu IP
     * @param ip Adres IP
     */
    void setIPAddress(const char* ip);

    /**
     * @brief Ustawienie jasności podświetlenia
     * @param brightness Jasność (0-255)
     */
    void setBrightness(uint8_t brightness);

    /**
     * @brief Pobranie jasności
     * @return Aktualna jasność (0-255)
     */
    uint8_t getBrightness() const;

    /**
     * @brief Wyświetlenie komunikatu
     * @param title Tytuł
     * @param message Treść
     * @param color Kolor tła
     */
    void showMessage(const char* title, const char* message, uint16_t color = COLOR_HEADER_BG);

    /**
     * @brief Wyświetlenie ekranu powitalnego
     */
    void showSplash();

    /**
     * @brief Dostęp do obiektu TFT
     * @return Referencja do TFT_eSPI
     */
    TFT_eSPI& getTFT();

private:
    TFT_eSPI _tft;
    DisplayData _data;
    DisplayScreen _currentScreen;
    uint8_t _brightness;

    uint32_t _lastUpdateTime;
    bool _needsFullRedraw;

    // Poprzednie wartości do optymalizacji rysowania
    float _prevDistanceM;
    float _prevSpeedKMH;
    uint32_t _prevCycleCount;
    uint8_t _prevActivePistols;
    bool _prevIsRunning;

    /**
     * @brief Inicjalizacja PWM dla podświetlenia
     */
    void initBacklight();

    /**
     * @brief Rysowanie ekranu głównego
     */
    void drawMainScreen();

    /**
     * @brief Rysowanie nagłówka
     */
    void drawHeader();

    /**
     * @brief Rysowanie wartości dystansu
     */
    void drawDistance();

    /**
     * @brief Rysowanie wartości prędkości
     */
    void drawSpeed();

    /**
     * @brief Rysowanie statusu pistoletów
     */
    void drawPistolStatus();

    /**
     * @brief Rysowanie statystyk
     */
    void drawStats();

    /**
     * @brief Rysowanie paska statusu
     */
    void drawStatusBar();

    /**
     * @brief Rysowanie ekranu ustawień
     */
    void drawSettingsScreen();

    /**
     * @brief Rysowanie ekranu wzoru
     */
    void drawPatternScreen();

    /**
     * @brief Pomocnicza funkcja do rysowania wartości z jednostką
     */
    void drawValueWithUnit(int x, int y, float value, const char* unit,
                           uint16_t valueColor, uint8_t valueFont,
                           uint16_t unitColor, uint8_t unitFont);
};

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

extern DisplayModule Display;

#endif // DISPLAY_MODULE_H
