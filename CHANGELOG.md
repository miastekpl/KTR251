# Changelog

Wszystkie istotne zmiany w projekcie KTR251 będą dokumentowane w tym pliku.

Format oparty na [Keep a Changelog](https://keepachangelog.com/pl/1.0.0/),
projekt stosuje [Semantic Versioning](https://semver.org/lang/pl/).

---

## [1.1.0] - 2026-02-04

### Dodano

#### Moduł wzorców malowania (patterns_module)
- **15 predefiniowanych wzorców** zgodnych z polskimi przepisami drogowymi:
  - P-1a do P-1e: Linie przerywane (różne proporcje linia/przerwa)
  - P-2a, P-2b: Linie ciągłe (wąska 12cm, szeroka 24cm)
  - P-3a, P-3b: Linie przekraczalne
  - P-4: Podwójna linia ciągła
  - P-6: Linia ostrzegawcza
  - P-7a do P-7d: Linie krawędziowe
- Natychmiastowa zmiana wzorca podczas malowania (on-the-fly)
- Callback powiadamiający o zmianie wzorca

#### Moduł klawiatury matrycowej (keypad_module)
- Obsługa klawiatury membranowej **4x4 (16 przycisków)**
- 15 przycisków dedykowanych wzorcom (P-1a do P-7d)
- 1 przycisk START/STOP
- Skanowanie matrycy z debouncingiem
- Callback dla naciśnięcia klawisza

#### Konfiguracja pinów (pins_config.h)
- **Nowy plik** z dedykowaną konfiguracją wszystkich pinów GPIO
- Szczegółowa dokumentacja ograniczeń ESP32-S3 N16R8
- Wizualna mapa pinów z funkcjami
- Tablice pinów dla pętli (przekaźniki, klawiatura)

#### Interfejs WWW
- **15 przycisków wyboru wzorców** w panelu webowym
- Podświetlenie aktywnego wzorca
- Wyświetlanie nazwy aktywnego wzorca
- Przyciski nawigacji (Poprzedni/Następny wzorzec)

### Zmieniono
- `config.h` - teraz dołącza `pins_config.h` dla konfiguracji pinów
- `main.cpp` - integracja modułów patterns i keypad
- `webserver_module` - rozbudowa o obsługę wzorców
- Wersja firmware zaktualizowana do 1.1.0

### Przypisanie pinów (v1.1.0)
| GPIO | Funkcja              |
|------|----------------------|
| 1    | Klawiatura - Wiersz 1|
| 2    | Klawiatura - Wiersz 2|
| 4    | Enkoder CLK          |
| 5    | Enkoder DT           |
| 6    | Enkoder SW           |
| 7-13 | TFT ILI9341 (SPI)    |
| 14   | Klawiatura - Wiersz 3|
| 15-18,21,47 | Przekaźniki 1-6|
| 38   | Klawiatura - Wiersz 4|
| 39-42| Klawiatura - Kolumny |
| 48   | Wolny (rezerwowy)    |

---

## [1.0.0] - 2026-02-04

### Dodano

#### Moduły firmware
- **Moduł enkodera (encoder_module)** - obsługa enkodera KY-040/przemysłowego
  - Pomiar dystansu w mm/m z rozdzielczością ~1mm
  - Pomiar prędkości w m/s i km/h
  - Detekcja kierunku ruchu (przód/tył)
  - Obsługa przycisku enkodera
  - Konfigurowalne PPR i średnica koła
  - Debouncing sprzętowy i programowy

- **Moduł przekaźników (relay_module)** - sterowanie 6 pistoletami
  - Sterowanie ręczne każdym przekaźnikiem
  - Tryb automatyczny - linia przerywana (AUTO_LINE)
  - Tryb automatyczny - linia ciągła (AUTO_SOLID)
  - Konfigurowalna długość linii i przerwy
  - Statystyki: liczba cykli, całkowity metraż
  - Obsługa pauzy i wznowienia

- **Moduł wyświetlacza (display_module)** - obsługa TFT ILI9341
  - Ekran główny z bieżącymi danymi
  - Wyświetlanie dystansu, prędkości, statusu
  - Wizualizacja stanu pistoletów
  - Regulacja jasności podświetlenia (PWM)
  - Ekran powitalny z paskiem postępu
  - Przełączanie między ekranami

- **Moduł serwera WWW (webserver_module)** - panel zdalnego sterowania
  - Punkt dostępowy WiFi (AP mode)
  - Responsywny interfejs WWW
  - Komunikacja WebSocket w czasie rzeczywistym
  - Sterowanie trybami pracy
  - Konfiguracja wzoru malowania
  - Sterowanie pojedynczymi pistoletami

#### Konfiguracja
- Plik `config.h` z pełną konfiguracją pinów
- Plik `User_Setup.h` dla biblioteki TFT_eSPI
- Konfiguracja PlatformIO (`platformio.ini`)

#### Dokumentacja
- `docs/SCHEMATIC.md` - schematy połączeń elektrycznych
- `docs/MANUAL.md` - instrukcja obsługi w języku polskim
- `CHANGELOG.md` - historia zmian
- `README.md` - opis projektu

#### Funkcje bezpieczeństwa
- Wszystkie piny GPIO zweryfikowane pod kątem ESP32-S3 N16R8
- Unikanie pinów zajętych przez PSRAM/Flash
- Unikanie pinów strapping
- Logika przekaźników active-low dla bezpieczeństwa

### Specyfikacja techniczna

#### Platforma
- ESP32-S3 N16R8 (16MB Flash, 8MB PSRAM)
- CPU: 240MHz Dual Core
- WiFi: 802.11 b/g/n

#### Przypisanie pinów
| GPIO | Funkcja        |
|------|----------------|
| 4    | Enkoder CLK    |
| 5    | Enkoder DT     |
| 6    | Enkoder SW     |
| 7    | TFT Backlight  |
| 8    | TFT Reset      |
| 9    | TFT DC         |
| 10   | TFT CS         |
| 11   | TFT MOSI       |
| 12   | TFT SCLK       |
| 13   | TFT MISO       |
| 15   | Przekaźnik 1   |
| 16   | Przekaźnik 2   |
| 17   | Przekaźnik 3   |
| 18   | Przekaźnik 4   |
| 21   | Przekaźnik 5   |
| 47   | Przekaźnik 6   |

#### Biblioteki
- TFT_eSPI v2.5.43
- ArduinoJson v7.0.0
- ESPAsyncWebServer v1.2.3
- AsyncTCP v1.1.1

---

## [Planowane] - Przyszłe wersje

### Do zrobienia
- [ ] Zapisywanie konfiguracji w pamięci Flash (Preferences)
- [ ] Tryb podwójnej linii (AUTO_DOUBLE)
- [ ] Obsługa ekranu dotykowego
- [ ] Eksport statystyk do pliku CSV
- [ ] Aktualizacja OTA przez WiFi
- [ ] Tryb klienta WiFi (połączenie do istniejącej sieci)
- [ ] Obsługa czujnika poziomu farby
- [ ] Dźwiękowa sygnalizacja (buzzer)
- [ ] GPS do logowania pozycji

---

*Projekt: KTR251 - Komputer do malowania pasów drogowych*
