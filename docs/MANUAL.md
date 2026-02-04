# KTR251 - Instrukcja obsługi

## Komputer do sterowania procesem malowania pasów drogowych

**Wersja firmware:** 1.0.0
**Data:** 2026-02-04

---

## Spis treści

1. [Wprowadzenie](#1-wprowadzenie)
2. [Uruchomienie systemu](#2-uruchomienie-systemu)
3. [Interfejs wyświetlacza](#3-interfejs-wyświetlacza)
4. [Panel sterowania WWW](#4-panel-sterowania-www)
5. [Tryby pracy](#5-tryby-pracy)
6. [Konfiguracja wzoru malowania](#6-konfiguracja-wzoru-malowania)
7. [Sterowanie pistoletami](#7-sterowanie-pistoletami)
8. [Kalibracja](#8-kalibracja)
9. [Rozwiązywanie problemów](#9-rozwiązywanie-problemów)
10. [Specyfikacja techniczna](#10-specyfikacja-techniczna)

---

## 1. Wprowadzenie

KTR251 to zaawansowany komputer pokładowy do sterowania procesem malowania pasów drogowych. System automatycznie kontroluje włączanie i wyłączanie pistoletów na podstawie przebytego dystansu mierzonego enkoderem.

### 1.1. Główne funkcje

- Automatyczne malowanie linii przerywanych (konfigurowalna długość linii i przerwy)
- Automatyczne malowanie linii ciągłych
- Sterowanie ręczne 6 pistoletami
- Pomiar dystansu i prędkości w czasie rzeczywistym
- Wyświetlacz TFT z bieżącymi danymi
- Bezprzewodowy panel sterowania (WiFi + WWW)
- Statystyki pracy (liczba cykli, namalowany metraż)

---

## 2. Uruchomienie systemu

### 2.1. Włączenie zasilania

1. Podłącz zasilanie 12-24V DC
2. Poczekaj na ekran powitalny (ok. 2 sekundy)
3. System automatycznie uruchomi punkt dostępowy WiFi

### 2.2. Parametry WiFi (domyślne)

| Parametr  | Wartość             |
|-----------|---------------------|
| Sieć      | `KTR251-Controller` |
| Hasło     | `roadmarking2026`   |
| Adres IP  | `192.168.4.1`       |

### 2.3. Połączenie z panelem WWW

1. Na telefonie/tablecie połącz się z siecią `KTR251-Controller`
2. Wprowadź hasło: `roadmarking2026`
3. Otwórz przeglądarkę i wpisz: `http://192.168.4.1`
4. Panel sterowania załaduje się automatycznie

---

## 3. Interfejs wyświetlacza

### 3.1. Ekran główny

```
┌────────────────────────────────┐
│  KTR251              AUTO-L   │  ← Nagłówek z trybem
├────────────────────────────────┤
│  DYSTANS                       │
│         123.4 m               │  ← Przebyty dystans
├────────────────────────────────┤
│  PRĘDKOŚĆ         5.2 km/h    │  ← Aktualna prędkość
├────────────────────────────────┤
│  PISTOLETY                     │
│  [1] [2] [3] [4] [5] [6]      │  ← Status pistoletów
├────────────────────────────────┤
│  Cykle: 15    Namalowano: 90m │  ← Statystyki
├────────────────────────────────┤
│  MALOWANIE        192.168.4.1 │  ← Status i IP
└────────────────────────────────┘
```

### 3.2. Wskaźniki pistoletów

| Kolor   | Znaczenie                        |
|---------|----------------------------------|
| Zielony | Pistolet aktywny (maluje)        |
| Szary   | Pistolet nieaktywny              |

### 3.3. Statusy systemu

| Status     | Opis                              |
|------------|-----------------------------------|
| Gotowy     | System gotowy do pracy            |
| MALOWANIE  | Trwa malowanie (pistolety aktywne)|
| PRZERWA    | Przerwa między liniami            |
| PAUZA      | System wstrzymany                 |
| Reset      | Wykonano reset                    |

### 3.4. Przycisk enkodera

- Naciśnij przycisk na enkoderze, aby przełączać ekrany
- Dostępne ekrany: Główny → Ustawienia → Wzór

---

## 4. Panel sterowania WWW

### 4.1. Układ panelu

Panel WWW podzielony jest na sekcje:

1. **Dane** - dystans, prędkość, statystyki (aktualizowane na żywo)
2. **Pistolety** - włączanie/wyłączanie indywidualnych pistoletów
3. **Wzór malowania** - konfiguracja długości linii i przerwy
4. **Tryb pracy** - wybór trybu automatycznego/ręcznego
5. **Kontrola** - przyciski START, STOP, RESET

### 4.2. Przyciski kontrolne

| Przycisk | Funkcja                                      |
|----------|----------------------------------------------|
| START    | Uruchomienie automatycznego malowania        |
| STOP     | Zatrzymanie malowania                        |
| RESET    | Reset dystansu i statystyk                   |

---

## 5. Tryby pracy

### 5.1. Tryb ręczny (MANUAL)

- Pistolety kontrolowane pojedynczo przez panel WWW
- Każdy pistolet można włączyć/wyłączyć niezależnie
- Brak automatycznego cyklu malowania

**Zastosowanie:** Testy, precyzyjne prace, malowanie elementów specjalnych

### 5.2. Tryb automatyczny - linia przerywana (AUTO-L)

- System automatycznie włącza/wyłącza pistolety
- Cykl: malowanie → przerwa → malowanie → przerwa...
- Konfigurowalna długość linii i przerwy

**Zastosowanie:** Standardowe malowanie pasów P-1 (linia przerywana)

### 5.3. Tryb automatyczny - linia ciągła (AUTO-S)

- Pistolety włączone przez cały czas ruchu
- Brak przerw w malowaniu

**Zastosowanie:** Malowanie linii ciągłych, linii krawędziowych

---

## 6. Konfiguracja wzoru malowania

### 6.1. Parametry wzoru

| Parametr         | Domyślna wartość | Zakres           |
|------------------|------------------|------------------|
| Długość linii    | 6000 mm (6m)     | 100 - 50000 mm   |
| Długość przerwy  | 12000 mm (12m)   | 100 - 100000 mm  |

### 6.2. Standardowe wzory według przepisów

| Typ pasa      | Linia  | Przerwa | Stosunek |
|---------------|--------|---------|----------|
| P-1 (6/12)    | 6m     | 12m     | 1:2      |
| P-1 (6/6)     | 6m     | 6m      | 1:1      |
| P-1 (3/6)     | 3m     | 6m      | 1:2      |
| P-2 (linia)   | ciągła | -       | -        |

### 6.3. Zmiana wzoru

1. W panelu WWW znajdź sekcję "Wzór malowania"
2. Wpisz długość linii w mm
3. Wpisz długość przerwy w mm
4. Kliknij "Ustaw wzór"
5. Potwierdź zmianę na wyświetlaczu

---

## 7. Sterowanie pistoletami

### 7.1. Rozmieszczenie pistoletów

```
Kierunek jazdy →

    Pistolet 1    Pistolet 2    Pistolet 3
    (lewy)        (środek L)    (środek)
         ↓             ↓             ↓
    ─────────────────────────────────────

    Pistolet 4    Pistolet 5    Pistolet 6
    (środek)      (środek P)    (prawy)
         ↓             ↓             ↓
    ─────────────────────────────────────
```

### 7.2. Aktywacja/deaktywacja pistoletów

- Kliknij na numer pistoletu w panelu WWW
- Zielony = aktywny (będzie malował w trybie AUTO)
- Szary = nieaktywny (wyłączony)

### 7.3. Konfiguracje typowe

| Konfiguracja        | Aktywne pistolety |
|---------------------|-------------------|
| Pełna szerokość     | 1, 2, 3, 4, 5, 6  |
| Linia środkowa      | 3, 4              |
| Linia krawędziowa L | 1, 2              |
| Linia krawędziowa P | 5, 6              |

---

## 8. Kalibracja

### 8.1. Kalibracja enkodera

Domyślne parametry:
- PPR (impulsy/obrót): 600
- Średnica koła: 200 mm
- Obwód koła: 628.32 mm

**Zmiana parametrów:**

Wyślij komendę przez WebSocket:
```json
{"cmd": "setWheelDiameter", "diameter": 200.0}
{"cmd": "setPPR", "ppr": 600}
```

### 8.2. Weryfikacja kalibracji

1. Odmierz odcinek 10 metrów
2. Zresetuj licznik dystansu
3. Przejedź odmierzony odcinek
4. Sprawdź wskazanie na wyświetlaczu
5. W razie różnicy, skoryguj średnicę koła

**Wzór korekty:**
```
Nowa_średnica = Aktualna_średnica × (Dystans_rzeczywisty / Dystans_wskazany)
```

---

## 9. Rozwiązywanie problemów

### 9.1. Brak wyświetlania

| Problem                    | Rozwiązanie                          |
|----------------------------|--------------------------------------|
| Czarny ekran               | Sprawdź zasilanie, przewody SPI      |
| Biały ekran                | Sprawdź połączenie RESET, DC         |
| Przekłamania kolorów       | Sprawdź połączenie MOSI, SCLK        |

### 9.2. Brak odczytu enkodera

| Problem                    | Rozwiązanie                          |
|----------------------------|--------------------------------------|
| Dystans = 0                | Sprawdź przewody CLK, DT             |
| Dystans rośnie przy postoju| Sprawdź masę, ekranowanie            |
| Nieprawidłowa wartość      | Skalibruj średnicę koła              |

### 9.3. Przekaźniki nie działają

| Problem                    | Rozwiązanie                          |
|----------------------------|--------------------------------------|
| Brak kliknięcia            | Sprawdź zasilanie 5V modułu          |
| Ciągłe załączenie          | Sprawdź logikę (active low)          |
| Losowe włączanie           | Sprawdź przewody, dodaj pull-up      |

### 9.4. Brak połączenia WiFi

| Problem                    | Rozwiązanie                          |
|----------------------------|--------------------------------------|
| Sieć niewidoczna           | Zrestartuj urządzenie                |
| Brak połączenia            | Sprawdź hasło                        |
| Strona się nie ładuje      | Użyj IP: 192.168.4.1                 |

---

## 10. Specyfikacja techniczna

### 10.1. Parametry systemu

| Parametr                   | Wartość                |
|----------------------------|------------------------|
| Napięcie zasilania         | 12-24V DC              |
| Pobór prądu (typowy)       | ~200mA @ 5V            |
| Pobór prądu (przekaźniki)  | +80mA/przekaźnik       |
| Temperatura pracy          | -20°C do +60°C         |
| Stopień ochrony            | Zależy od obudowy      |

### 10.2. Parametry wyświetlacza

| Parametr                   | Wartość                |
|----------------------------|------------------------|
| Typ                        | TFT ILI9341            |
| Rozdzielczość              | 320 × 240 px           |
| Przekątna                  | 2.8"                   |
| Interfejs                  | SPI @ 40 MHz           |

### 10.3. Parametry enkodera

| Parametr                   | Wartość                |
|----------------------------|------------------------|
| Typ                        | Inkrementalny AB       |
| PPR                        | Konfigurowalne         |
| Rozdzielczość              | ~1 mm (zależy od koła) |
| Napięcie                   | 3.3V                   |

### 10.4. Parametry przekaźników

| Parametr                   | Wartość                |
|----------------------------|------------------------|
| Liczba kanałów             | 6                      |
| Obciążalność               | 10A @ 250V AC          |
| Izolacja                   | Optyczna               |
| Sterowanie                 | Active LOW             |

---

## Kontakt i wsparcie

W razie pytań lub problemów skontaktuj się z producentem.

---

*Dokument opracowany dla projektu KTR251 v1.0.0*
*Ostatnia aktualizacja: 2026-02-04*
