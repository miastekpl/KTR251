/**
 * @file display_module.cpp
 * @brief Implementacja modułu obsługi wyświetlacza ILI9341
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "display_module.h"

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

DisplayModule Display;

// =============================================================================
// KONSTRUKTOR
// =============================================================================

DisplayModule::DisplayModule() : _tft(TFT_eSPI()) {
    memset(&_data, 0, sizeof(_data));
    _currentScreen = DisplayScreen::MAIN;
    _brightness = TFT_BL_DEFAULT;
    _lastUpdateTime = 0;
    _needsFullRedraw = true;

    // Poprzednie wartości
    _prevDistanceM = -1;
    _prevSpeedKMH = -1;
    _prevCycleCount = 0xFFFFFFFF;
    _prevActivePistols = 0xFF;
    _prevIsRunning = false;

    // Domyślne wartości
    strcpy(_data.statusText, "Gotowy");
    strcpy(_data.ipAddress, "0.0.0.0");
}

// =============================================================================
// INICJALIZACJA
// =============================================================================

bool DisplayModule::begin() {
    DEBUG_PRINTLN(F("[DISPLAY] Inicjalizacja wyswietlacza ILI9341..."));

    // Inicjalizacja TFT
    _tft.init();
    _tft.setRotation(TFT_ROTATION);
    _tft.fillScreen(COLOR_BACKGROUND);

    // Inicjalizacja podświetlenia
    initBacklight();

    // Ekran powitalny
    showSplash();
    delay(1500);

    // Przejdź do ekranu głównego
    _needsFullRedraw = true;
    _lastUpdateTime = millis();

    DEBUG_PRINTF("[DISPLAY] Rozdzielczosc: %dx%d\n", _tft.width(), _tft.height());
    DEBUG_PRINTLN(F("[DISPLAY] Modul zainicjalizowany pomyslnie"));

    return true;
}

void DisplayModule::initBacklight() {
    // Konfiguracja PWM dla podświetlenia
    ledcSetup(TFT_BL_PWM_CHANNEL, TFT_BL_PWM_FREQ, TFT_BL_PWM_RES);
    ledcAttachPin(TFT_BL, TFT_BL_PWM_CHANNEL);
    ledcWrite(TFT_BL_PWM_CHANNEL, _brightness);

    DEBUG_PRINTF("[DISPLAY] Podswietlenie: %d/255\n", _brightness);
}

// =============================================================================
// EKRAN POWITALNY
// =============================================================================

void DisplayModule::showSplash() {
    _tft.fillScreen(COLOR_BACKGROUND);

    // Logo/Tytuł
    _tft.setTextColor(COLOR_ACCENT);
    _tft.setTextDatum(MC_DATUM);
    _tft.setTextSize(1);

    // Duży tytuł
    _tft.setFreeFont(&FreeSansBold18pt7b);
    _tft.drawString("KTR251", _tft.width() / 2, 80);

    // Podtytuł
    _tft.setTextColor(COLOR_WHITE);
    _tft.setFreeFont(&FreeSans12pt7b);
    _tft.drawString("Road Marking", _tft.width() / 2, 130);
    _tft.drawString("Controller", _tft.width() / 2, 160);

    // Wersja
    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.setFreeFont(&FreeSans9pt7b);
    char versionStr[32];
    snprintf(versionStr, sizeof(versionStr), "v%s", FIRMWARE_VERSION);
    _tft.drawString(versionStr, _tft.width() / 2, 220);

    // Pasek ładowania
    int barWidth = 200;
    int barHeight = 8;
    int barX = (_tft.width() - barWidth) / 2;
    int barY = 260;

    _tft.drawRect(barX, barY, barWidth, barHeight, COLOR_GRAY);

    for (int i = 0; i < barWidth - 4; i += 4) {
        _tft.fillRect(barX + 2, barY + 2, i, barHeight - 4, COLOR_ACCENT);
        delay(15);
    }
}

// =============================================================================
// AKTUALIZACJA
// =============================================================================

void DisplayModule::update() {
    uint32_t currentTime = millis();

    // Ogranicz częstotliwość odświeżania
    if (currentTime - _lastUpdateTime < DISPLAY_UPDATE_INTERVAL && !_needsFullRedraw) {
        return;
    }
    _lastUpdateTime = currentTime;

    // Rysuj odpowiedni ekran
    switch (_currentScreen) {
        case DisplayScreen::MAIN:
            drawMainScreen();
            break;
        case DisplayScreen::SETTINGS:
            drawSettingsScreen();
            break;
        case DisplayScreen::PATTERN:
            drawPatternScreen();
            break;
        default:
            drawMainScreen();
            break;
    }

    _needsFullRedraw = false;
}

void DisplayModule::refresh() {
    _needsFullRedraw = true;
    _prevDistanceM = -1;
    _prevSpeedKMH = -1;
    _prevCycleCount = 0xFFFFFFFF;
    _prevActivePistols = 0xFF;
}

// =============================================================================
// EKRAN GŁÓWNY
// =============================================================================

void DisplayModule::drawMainScreen() {
    if (_needsFullRedraw) {
        _tft.fillScreen(COLOR_BACKGROUND);
        drawHeader();
    }

    // Aktualizuj tylko zmienione elementy
    drawDistance();
    drawSpeed();
    drawPistolStatus();
    drawStats();
    drawStatusBar();
}

void DisplayModule::drawHeader() {
    // Nagłówek
    _tft.fillRect(0, 0, _tft.width(), 30, COLOR_HEADER_BG);

    _tft.setTextColor(COLOR_WHITE);
    _tft.setTextDatum(ML_DATUM);
    _tft.setFreeFont(&FreeSans9pt7b);
    _tft.drawString("KTR251", 10, 15);

    // Status połączenia/tryb
    _tft.setTextDatum(MR_DATUM);
    const char* modeStr = "MANUAL";
    switch (_data.mode) {
        case 1: modeStr = "AUTO-L"; break;
        case 2: modeStr = "AUTO-S"; break;
        case 3: modeStr = "AUTO-D"; break;
    }
    _tft.drawString(modeStr, _tft.width() - 10, 15);
}

void DisplayModule::drawDistance() {
    // Sprawdź czy wartość się zmieniła (z tolerancją)
    if (!_needsFullRedraw && fabs(_data.distanceM - _prevDistanceM) < 0.001f) {
        return;
    }
    _prevDistanceM = _data.distanceM;

    // Wyczyść obszar
    _tft.fillRect(0, 40, _tft.width(), 70, COLOR_BACKGROUND);

    // Etykieta
    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.setTextDatum(TL_DATUM);
    _tft.setFreeFont(&FreeSans9pt7b);
    _tft.drawString("DYSTANS", 10, 45);

    // Wartość
    _tft.setTextColor(COLOR_WHITE);
    _tft.setFreeFont(&FreeSansBold24pt7b);
    _tft.setTextDatum(TR_DATUM);

    char distStr[16];
    if (_data.distanceM < 1000) {
        snprintf(distStr, sizeof(distStr), "%.1f", _data.distanceM);
    } else {
        snprintf(distStr, sizeof(distStr), "%.2f", _data.distanceM / 1000.0f);
    }
    _tft.drawString(distStr, _tft.width() - 50, 55);

    // Jednostka
    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.setFreeFont(&FreeSans9pt7b);
    _tft.setTextDatum(TR_DATUM);
    _tft.drawString(_data.distanceM < 1000 ? "m" : "km", _tft.width() - 10, 75);
}

void DisplayModule::drawSpeed() {
    // Sprawdź czy wartość się zmieniła
    if (!_needsFullRedraw && fabs(_data.speedKMH - _prevSpeedKMH) < 0.1f) {
        return;
    }
    _prevSpeedKMH = _data.speedKMH;

    // Wyczyść obszar
    _tft.fillRect(0, 115, _tft.width(), 50, COLOR_BACKGROUND);

    // Etykieta
    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.setTextDatum(TL_DATUM);
    _tft.setFreeFont(&FreeSans9pt7b);
    _tft.drawString("PREDKOSC", 10, 120);

    // Wartość
    _tft.setTextColor(COLOR_ACCENT);
    _tft.setFreeFont(&FreeSansBold18pt7b);
    _tft.setTextDatum(TR_DATUM);

    char speedStr[16];
    snprintf(speedStr, sizeof(speedStr), "%.1f", _data.speedKMH);
    _tft.drawString(speedStr, _tft.width() - 70, 120);

    // Jednostka
    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.setFreeFont(&FreeSans9pt7b);
    _tft.drawString("km/h", _tft.width() - 10, 130);
}

void DisplayModule::drawPistolStatus() {
    // Sprawdź czy się zmieniło
    if (!_needsFullRedraw && _data.activePistols == _prevActivePistols &&
        _data.isRunning == _prevIsRunning) {
        return;
    }
    _prevActivePistols = _data.activePistols;
    _prevIsRunning = _data.isRunning;

    // Wyczyść obszar
    _tft.fillRect(0, 170, _tft.width(), 50, COLOR_BACKGROUND);

    // Etykieta
    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.setTextDatum(TL_DATUM);
    _tft.setFreeFont(&FreeSans9pt7b);
    _tft.drawString("PISTOLETY", 10, 175);

    // Rysuj wskaźniki pistoletów
    int startX = 20;
    int y = 200;
    int size = 30;
    int spacing = 8;

    for (int i = 0; i < NUM_RELAYS; i++) {
        int x = startX + i * (size + spacing);
        bool isActive = (_data.activePistols & (1 << i)) != 0;

        // Tło
        uint16_t color = isActive ? COLOR_ACTIVE : COLOR_INACTIVE;
        _tft.fillRoundRect(x, y, size, size, 4, color);

        // Numer
        _tft.setTextColor(isActive ? COLOR_BLACK : COLOR_GRAY);
        _tft.setTextDatum(MC_DATUM);
        _tft.setFreeFont(&FreeSansBold9pt7b);
        char numStr[2];
        snprintf(numStr, sizeof(numStr), "%d", i + 1);
        _tft.drawString(numStr, x + size / 2, y + size / 2);
    }
}

void DisplayModule::drawStats() {
    // Sprawdź czy się zmieniło
    if (!_needsFullRedraw && _data.cycleCount == _prevCycleCount) {
        return;
    }
    _prevCycleCount = _data.cycleCount;

    // Wyczyść obszar
    _tft.fillRect(0, 240, _tft.width(), 40, COLOR_BACKGROUND);

    // Cykle
    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.setTextDatum(TL_DATUM);
    _tft.setFreeFont(&FreeSans9pt7b);
    _tft.drawString("Cykle:", 10, 245);

    _tft.setTextColor(COLOR_WHITE);
    char cycleStr[16];
    snprintf(cycleStr, sizeof(cycleStr), "%lu", (unsigned long)_data.cycleCount);
    _tft.drawString(cycleStr, 70, 245);

    // Namalowano
    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.drawString("Namalowano:", 120, 245);

    _tft.setTextColor(COLOR_WHITE);
    char paintedStr[16];
    snprintf(paintedStr, sizeof(paintedStr), "%.1f m", (float)_data.totalPaintedM);
    _tft.drawString(paintedStr, 220, 245);
}

void DisplayModule::drawStatusBar() {
    // Pasek statusu na dole
    int y = _tft.height() - 25;

    _tft.fillRect(0, y, _tft.width(), 25, COLOR_DARK_GRAY);

    // Status
    _tft.setTextColor(_data.isRunning ? COLOR_ACTIVE : COLOR_TEXT_PRIMARY);
    _tft.setTextDatum(ML_DATUM);
    _tft.setFreeFont(&FreeSans9pt7b);
    _tft.drawString(_data.statusText, 10, y + 12);

    // IP
    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.setTextDatum(MR_DATUM);
    _tft.drawString(_data.ipAddress, _tft.width() - 10, y + 12);
}

// =============================================================================
// EKRANY POMOCNICZE
// =============================================================================

void DisplayModule::drawSettingsScreen() {
    if (!_needsFullRedraw) return;

    _tft.fillScreen(COLOR_BACKGROUND);
    drawHeader();

    _tft.setTextColor(COLOR_WHITE);
    _tft.setTextDatum(MC_DATUM);
    _tft.setFreeFont(&FreeSans12pt7b);
    _tft.drawString("Ustawienia", _tft.width() / 2, 100);

    _tft.setTextColor(COLOR_TEXT_SECONDARY);
    _tft.setFreeFont(&FreeSans9pt7b);
    _tft.drawString("Dostep przez WWW", _tft.width() / 2, 150);
    _tft.drawString(_data.ipAddress, _tft.width() / 2, 180);
}

void DisplayModule::drawPatternScreen() {
    if (!_needsFullRedraw) return;

    _tft.fillScreen(COLOR_BACKGROUND);
    drawHeader();

    _tft.setTextColor(COLOR_WHITE);
    _tft.setTextDatum(MC_DATUM);
    _tft.setFreeFont(&FreeSans12pt7b);
    _tft.drawString("Wzor malowania", _tft.width() / 2, 100);
}

// =============================================================================
// STEROWANIE EKRANEM
// =============================================================================

void DisplayModule::setScreen(DisplayScreen screen) {
    if (_currentScreen != screen) {
        _currentScreen = screen;
        _needsFullRedraw = true;
    }
}

DisplayScreen DisplayModule::getScreen() const {
    return _currentScreen;
}

void DisplayModule::nextScreen() {
    int next = (int)_currentScreen + 1;
    if (next > (int)DisplayScreen::DIAGNOSTIC) {
        next = 0;
    }
    setScreen((DisplayScreen)next);
}

void DisplayModule::prevScreen() {
    int prev = (int)_currentScreen - 1;
    if (prev < 0) {
        prev = (int)DisplayScreen::DIAGNOSTIC;
    }
    setScreen((DisplayScreen)prev);
}

// =============================================================================
// SETTERY
// =============================================================================

void DisplayModule::setData(const DisplayData& data) {
    _data = data;
}

void DisplayModule::setStatusText(const char* text) {
    strncpy(_data.statusText, text, sizeof(_data.statusText) - 1);
    _data.statusText[sizeof(_data.statusText) - 1] = '\0';
}

void DisplayModule::setIPAddress(const char* ip) {
    strncpy(_data.ipAddress, ip, sizeof(_data.ipAddress) - 1);
    _data.ipAddress[sizeof(_data.ipAddress) - 1] = '\0';
}

void DisplayModule::setBrightness(uint8_t brightness) {
    _brightness = brightness;
    ledcWrite(TFT_BL_PWM_CHANNEL, _brightness);
    DEBUG_PRINTF("[DISPLAY] Jasnosc: %d/255\n", _brightness);
}

uint8_t DisplayModule::getBrightness() const {
    return _brightness;
}

void DisplayModule::showMessage(const char* title, const char* message, uint16_t color) {
    _tft.fillScreen(COLOR_BACKGROUND);
    _tft.fillRect(0, 0, _tft.width(), 40, color);

    _tft.setTextColor(COLOR_WHITE);
    _tft.setTextDatum(MC_DATUM);
    _tft.setFreeFont(&FreeSansBold12pt7b);
    _tft.drawString(title, _tft.width() / 2, 20);

    _tft.setFreeFont(&FreeSans12pt7b);
    _tft.drawString(message, _tft.width() / 2, _tft.height() / 2);
}

TFT_eSPI& DisplayModule::getTFT() {
    return _tft;
}
