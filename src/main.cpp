/**
 * @file main.cpp
 * @brief MINIMALNY TEST - diagnoza boot loop ESP32-S3 N16R8
 */

#include <Arduino.h>

// WYŁĄCZ WSZYSTKIE MODUŁY - CZYSTY TEST
#define MINIMAL_TEST 1

#ifndef MINIMAL_TEST
#include "config.h"
#include "modules/encoder/encoder_module.h"
#include "modules/relays/relay_module.h"
#include "modules/display/display_module.h"
#include "modules/webserver/webserver_module.h"
#include "modules/patterns/patterns_module.h"
#endif

void setup() {
    // Minimalne opóźnienie
    delay(2000);

    // Inicjalizacja Serial przez USB CDC
    Serial.begin(115200);

    // Czekaj na połączenie USB
    unsigned long start = millis();
    while (!Serial && millis() - start < 5000) {
        delay(100);
    }

    Serial.println();
    Serial.println("================================");
    Serial.println("  ESP32-S3 N16R8 - TEST BOOT");
    Serial.println("================================");
    Serial.printf("Chip: %s\n", ESP.getChipModel());
    Serial.printf("Revision: %d\n", ESP.getChipRevision());
    Serial.printf("Cores: %d\n", ESP.getChipCores());
    Serial.printf("CPU Freq: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Flash: %d MB\n", ESP.getFlashChipSize() / 1024 / 1024);
    Serial.printf("Flash Speed: %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
    Serial.printf("PSRAM: %d KB\n", ESP.getPsramSize() / 1024);
    Serial.printf("Free Heap: %d KB\n", ESP.getFreeHeap() / 1024);
    Serial.printf("Free PSRAM: %d KB\n", ESP.getFreePsram() / 1024);
    Serial.println("================================");
    Serial.println("BOOT TEST PASSED!");
    Serial.println("================================");
}

void loop() {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= 2000) {
        Serial.printf("[%lu] Heap: %d KB, Running OK\n",
            millis() / 1000, ESP.getFreeHeap() / 1024);
        lastPrint = millis();
    }
    delay(10);
}
