# KTR251 - Schematy połączeń

## Wersja dokumentu: 1.0.0
## Data: 2026-02-04

---

## 1. Przegląd systemu

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        KTR251 - SYSTEM OVERVIEW                         │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│    ┌──────────────┐      ┌──────────────────────────────────┐           │
│    │   ENKODER    │      │         ESP32-S3 N16R8           │           │
│    │   KY-040     │──────│                                  │           │
│    │  (pomiar)    │      │   CPU: 240MHz Dual Core          │           │
│    └──────────────┘      │   Flash: 16MB                    │           │
│                          │   PSRAM: 8MB                     │           │
│    ┌──────────────┐      │                                  │           │
│    │ WYŚWIETLACZ  │      │                                  │           │
│    │  ILI9341     │──────│                                  │           │
│    │  320x240     │      │                                  │           │
│    └──────────────┘      │                                  │           │
│                          │                                  │           │
│    ┌──────────────┐      │                                  │           │
│    │ PRZEKAŹNIKI  │      │                                  │           │
│    │   6x RELAY   │──────│                                  │           │
│    │ (pistolety)  │      │                                  │           │
│    └──────────────┘      └──────────────────────────────────┘           │
│                                        │                                 │
│                                        │ WiFi AP                         │
│                                        ▼                                 │
│                          ┌──────────────────────────────────┐           │
│                          │      PANEL STEROWANIA            │           │
│                          │      (przeglądarka WWW)          │           │
│                          └──────────────────────────────────┘           │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Przypisanie pinów ESP32-S3 N16R8

### 2.1. Ważne ograniczenia ESP32-S3 N16R8

```
⚠️  PINY NIEDOSTĘPNE (zajęte przez pamięć):
    - GPIO 26-32: Octal PSRAM
    - GPIO 33-37: Octal Flash

⚠️  PINY STRAPPING (unikać):
    - GPIO 0:  Boot mode
    - GPIO 3:  JTAG
    - GPIO 45: VDD_SPI voltage
    - GPIO 46: Boot mode

⚠️  PINY SPECJALNE:
    - GPIO 19, 20: USB D-, D+
    - GPIO 43, 44: UART0 TX, RX
```

### 2.2. Tabela przypisania pinów

| GPIO | Funkcja          | Moduł        | Uwagi                    |
|------|------------------|--------------|--------------------------|
| 4    | ENCODER_CLK      | Enkoder      | Z przerwaniem            |
| 5    | ENCODER_DT       | Enkoder      | Kierunek                 |
| 6    | ENCODER_SW       | Enkoder      | Przycisk (opcjonalny)    |
| 7    | TFT_BL           | Wyświetlacz  | PWM podświetlenie        |
| 8    | TFT_RST          | Wyświetlacz  | Reset                    |
| 9    | TFT_DC           | Wyświetlacz  | Data/Command             |
| 10   | TFT_CS           | Wyświetlacz  | Chip Select              |
| 11   | TFT_MOSI         | Wyświetlacz  | SPI MOSI                 |
| 12   | TFT_SCLK         | Wyświetlacz  | SPI Clock                |
| 13   | TFT_MISO         | Wyświetlacz  | SPI MISO (opcjonalny)    |
| 15   | RELAY_1          | Przekaźniki  | Pistolet 1               |
| 16   | RELAY_2          | Przekaźniki  | Pistolet 2               |
| 17   | RELAY_3          | Przekaźniki  | Pistolet 3               |
| 18   | RELAY_4          | Przekaźniki  | Pistolet 4               |
| 21   | RELAY_5          | Przekaźniki  | Pistolet 5               |
| 47   | RELAY_6          | Przekaźniki  | Pistolet 6               |

### 2.3. Wolne piny (do rozbudowy)

| GPIO | Status    | Sugerowane użycie           |
|------|-----------|----------------------------|
| 1    | Wolny     | Czujnik poziomu farby      |
| 2    | Wolny     | Czujnik temperatury        |
| 14   | Wolny     | Dodatkowy enkoder          |
| 38   | Wolny     | Wejście analogowe          |
| 39   | Wolny     | Wejście analogowe          |
| 40   | Wolny     | I2C SDA (rozszerzenie)     |
| 41   | Wolny     | I2C SCL (rozszerzenie)     |
| 42   | Wolny     | Buzzer / sygnalizacja      |
| 48   | Wolny     | LED status                 |

---

## 3. Schemat połączeń - Enkoder KY-040

```
                    KY-040
                 ┌─────────┐
                 │    +    │───────── 3.3V
                 │   GND   │───────── GND
                 │   SW    │───────── GPIO6 (z wewnętrznym pull-up)
                 │   DT    │───────── GPIO5 (z wewnętrznym pull-up)
                 │   CLK   │───────── GPIO4 (z wewnętrznym pull-up)
                 └─────────┘


    Uwagi:
    - Enkoder wymaga napięcia 3.3V
    - Wbudowane rezystory pull-up w ESP32-S3
    - Debouncing realizowany programowo (100µs)
```

### 3.1. Parametry enkodera pomiarowego

```
    Dla enkodera przemysłowego (pomiar dystansu):

    ┌────────────────────────────────────────────────────────┐
    │  PPR (Pulses Per Revolution): 600                      │
    │  Średnica koła pomiarowego: 200mm                      │
    │  Obwód koła: π × 200mm ≈ 628.32mm                      │
    │  Rozdzielczość: 628.32mm / 600 ≈ 1.05mm/impuls        │
    └────────────────────────────────────────────────────────┘
```

---

## 4. Schemat połączeń - Wyświetlacz ILI9341

```
                    ILI9341 TFT 2.8"
                 ┌─────────────────┐
                 │      VCC        │───────── 3.3V
                 │      GND        │───────── GND
                 │      CS         │───────── GPIO10
                 │      RESET      │───────── GPIO8
                 │      DC/RS      │───────── GPIO9
                 │      SDI/MOSI   │───────── GPIO11
                 │      SCK        │───────── GPIO12
                 │      LED        │───────── GPIO7 (przez MOSFET lub bezpośrednio)
                 │      SDO/MISO   │───────── GPIO13 (opcjonalny)
                 └─────────────────┘

    Magistrala SPI:
    ┌────────────────────────────────┐
    │  Częstotliwość: 40 MHz         │
    │  Tryb: SPI Mode 0              │
    │  Kolejność bitów: MSB First    │
    └────────────────────────────────┘
```

### 4.1. Podświetlenie (opcja z MOSFET)

```
    GPIO7 ────┤├──── GND
              │
    3.3V ─────┤ MOSFET
              │ (np. 2N7002)
              │
    LED+ ─────┘

    Sterowanie: PWM 5kHz, 8-bit (0-255)
```

---

## 5. Schemat połączeń - Przekaźniki

```
    ESP32-S3                    MODUŁ 6 PRZEKAŹNIKÓW

    GPIO15 ─────────────────── IN1 (Pistolet 1 - lewy skrajny)
    GPIO16 ─────────────────── IN2 (Pistolet 2)
    GPIO17 ─────────────────── IN3 (Pistolet 3)
    GPIO18 ─────────────────── IN4 (Pistolet 4)
    GPIO21 ─────────────────── IN5 (Pistolet 5)
    GPIO47 ─────────────────── IN6 (Pistolet 6 - prawy skrajny)

    GND    ─────────────────── GND
    5V     ─────────────────── VCC (zasilanie przekaźników)


    ┌──────────────────────────────────────────────────────────┐
    │  UWAGA: Przekaźniki są ACTIVE LOW                        │
    │  - Stan WYSOKI (HIGH) = przekaźnik WYŁĄCZONY             │
    │  - Stan NISKI (LOW)   = przekaźnik WŁĄCZONY              │
    └──────────────────────────────────────────────────────────┘
```

### 5.1. Wyjścia przekaźników

```
    Przekaźnik ──┬── NO (Normally Open)    ─── Do zaworu pistoletu
                 ├── COM (Common)          ─── Zasilanie +24V
                 └── NC (Normally Closed)  ─── Nie używane

    Parametry styków:
    ┌────────────────────────────────┐
    │  Max DC: 30V / 10A             │
    │  Max AC: 250V / 10A            │
    └────────────────────────────────┘
```

---

## 6. Schemat zasilania

```
    ┌─────────────────────────────────────────────────────────────┐
    │                    ZASILANIE SYSTEMU                        │
    ├─────────────────────────────────────────────────────────────┤
    │                                                             │
    │    Zasilanie główne: 12V DC lub 24V DC                      │
    │                         │                                   │
    │                         ▼                                   │
    │    ┌─────────────────────────────────────┐                 │
    │    │  Przetwornica DC-DC Step-Down       │                 │
    │    │  Wejście: 12-24V                    │                 │
    │    │  Wyjście: 5V / 3A                   │                 │
    │    └─────────────────────────────────────┘                 │
    │                         │                                   │
    │          ┌──────────────┼──────────────┐                   │
    │          ▼              ▼              ▼                   │
    │    ┌──────────┐  ┌──────────┐  ┌──────────────┐           │
    │    │ ESP32-S3 │  │ ILI9341  │  │ Przekaźniki  │           │
    │    │   5V     │  │  3.3V    │  │     5V       │           │
    │    │  (USB)   │  │ (z reg.) │  │              │           │
    │    └──────────┘  └──────────┘  └──────────────┘           │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘

    Uwaga: ESP32-S3 ma wbudowany regulator 3.3V
```

---

## 7. Rozmieszczenie fizyczne

```
    ┌─────────────────────────────────────────────────────────┐
    │                   OBUDOWA STEROWNIKA                    │
    ├─────────────────────────────────────────────────────────┤
    │                                                          │
    │   ┌─────────────────┐      ┌────────────────────────┐   │
    │   │                 │      │                        │   │
    │   │   WYŚWIETLACZ   │      │      ESP32-S3          │   │
    │   │    ILI9341      │      │       N16R8            │   │
    │   │   (2.8" TFT)    │      │                        │   │
    │   │                 │      │                        │   │
    │   └─────────────────┘      └────────────────────────┘   │
    │                                                          │
    │   ┌──────────────────────────────────────────────────┐  │
    │   │                                                  │  │
    │   │              MODUŁ 6 PRZEKAŹNIKÓW                │  │
    │   │                                                  │  │
    │   │   [R1]  [R2]  [R3]  [R4]  [R5]  [R6]            │  │
    │   │                                                  │  │
    │   └──────────────────────────────────────────────────┘  │
    │                                                          │
    │   ┌───────────┐  ┌───────────┐  ┌───────────────────┐   │
    │   │  ENKODER  │  │ PRZETW.   │  │ ZŁĄCZA WYJŚCIOWE  │   │
    │   │  (gniazdo)│  │  DC-DC    │  │   DO PISTOLETÓW   │   │
    │   └───────────┘  └───────────┘  └───────────────────┘   │
    │                                                          │
    └─────────────────────────────────────────────────────────┘
```

---

## 8. Lista materiałów (BOM)

| Lp. | Komponent                      | Ilość | Uwagi                        |
|-----|--------------------------------|-------|------------------------------|
| 1   | ESP32-S3 N16R8 DevKit          | 1     | 16MB Flash, 8MB PSRAM        |
| 2   | Wyświetlacz ILI9341 2.8" TFT   | 1     | SPI, 320x240, z dotykiem lub bez |
| 3   | Moduł 6 przekaźników           | 1     | 5V, optoisolacja, active low |
| 4   | Enkoder przemysłowy            | 1     | 600 PPR, wyjście AB          |
| 5   | Koło pomiarowe                 | 1     | Średnica 200mm               |
| 6   | Przetwornica DC-DC             | 1     | 12-24V → 5V, min 3A          |
| 7   | Obudowa przemysłowa            | 1     | IP65, odp. rozmiar           |
| 8   | Złącza przemysłowe             | -     | Wg potrzeb                   |
| 9   | Przewody połączeniowe          | -     | AWG 22-24                    |

---

## 9. Uwagi końcowe

1. **Wszystkie połączenia** powinny być zabezpieczone przed wilgocią
2. **Przewody do pistoletów** powinny być ekranowane
3. **Enkoder** powinien być zamontowany sztywno na osi koła pomiarowego
4. **Zabezpieczenie przepięciowe** zalecane na wejściach przekaźników
5. **Uziemienie** - wszystkie GND połączone w jednym punkcie

---

*Dokument opracowany dla projektu KTR251 v1.0.0*
