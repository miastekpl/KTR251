/**
 * @file webserver_module.h
 * @brief Moduł serwera WWW z WebSocket
 * @version 1.0.0
 * @date 2026-02-04
 *
 * Moduł odpowiada za:
 * - Uruchomienie punktu dostępowego WiFi
 * - Serwowanie interfejsu WWW
 * - Komunikację WebSocket dla aktualizacji w czasie rzeczywistym
 * - API REST do sterowania
 */

#ifndef WEBSERVER_MODULE_H
#define WEBSERVER_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "config.h"

// =============================================================================
// TYPY CALLBACK
// =============================================================================

// Callback dla komend otrzymanych przez WebSocket
typedef void (*CommandCallback)(const char* command, JsonDocument& params);

// =============================================================================
// STRUKTURA DANYCH DO WYSYŁANIA
// =============================================================================

struct WebServerData {
    float distanceM;
    float speedKMH;
    uint32_t cycleCount;
    uint32_t totalPaintedM;
    bool isRunning;
    bool isPaused;
    uint8_t activePistols;
    uint8_t mode;
    int64_t pulseCount;
    uint32_t lineLength;
    uint32_t gapLength;
};

// =============================================================================
// KLASA MODUŁU SERWERA WWW
// =============================================================================

class WebServerModule {
public:
    /**
     * @brief Konstruktor
     */
    WebServerModule();

    /**
     * @brief Destruktor
     */
    ~WebServerModule();

    /**
     * @brief Inicjalizacja serwera
     * @return true jeśli sukces
     */
    bool begin();

    /**
     * @brief Aktualizacja (wywoływać w loop)
     */
    void update();

    /**
     * @brief Wysłanie danych do wszystkich klientów WebSocket
     * @param data Dane do wysłania
     */
    void broadcastData(const WebServerData& data);

    /**
     * @brief Wysłanie wiadomości tekstowej
     * @param message Wiadomość
     */
    void broadcastMessage(const char* message);

    /**
     * @brief Pobranie adresu IP
     * @return Adres IP jako String
     */
    String getIPAddress() const;

    /**
     * @brief Sprawdzenie czy WiFi jest połączone
     * @return true jeśli połączono
     */
    bool isConnected() const;

    /**
     * @brief Liczba połączonych klientów
     * @return Liczba klientów WebSocket
     */
    uint8_t getClientCount() const;

    /**
     * @brief Ustawienie callbacka dla komend
     * @param callback Funkcja callback
     */
    void setCommandCallback(CommandCallback callback);

    /**
     * @brief Ustawienie danych o wzorze (do wyświetlania)
     */
    void setPatternData(uint32_t lineLength, uint32_t gapLength);

private:
    AsyncWebServer* _server;
    AsyncWebSocket* _ws;

    CommandCallback _commandCallback;
    uint32_t _lastBroadcastTime;
    bool _wifiConnected;

    uint32_t _patternLineLength;
    uint32_t _patternGapLength;

    /**
     * @brief Inicjalizacja WiFi AP
     */
    bool initWiFi();

    /**
     * @brief Konfiguracja routingu HTTP
     */
    void setupRoutes();

    /**
     * @brief Obsługa zdarzeń WebSocket
     */
    void onWebSocketEvent(AsyncWebSocket* server,
                          AsyncWebSocketClient* client,
                          AwsEventType type,
                          void* arg,
                          uint8_t* data,
                          size_t len);

    /**
     * @brief Przetwarzanie komendy JSON
     */
    void processCommand(const char* json, AsyncWebSocketClient* client);

    /**
     * @brief Generowanie strony HTML
     */
    String generateHTML();

    /**
     * @brief Generowanie CSS
     */
    String generateCSS();

    /**
     * @brief Generowanie JavaScript
     */
    String generateJS();
};

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

extern WebServerModule WebServer;

#endif // WEBSERVER_MODULE_H
