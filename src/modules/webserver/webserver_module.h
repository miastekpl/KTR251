/**
 * @file webserver_module.h
 * @brief Moduł serwera WWW z WebSocket i obsługą wzorców
 * @version 1.1.0
 * @date 2026-02-04
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
    WebServerModule();
    ~WebServerModule();

    bool begin();
    void update();

    void broadcastData(const WebServerData& data);
    void broadcastMessage(const char* message);

    String getIPAddress() const;
    bool isConnected() const;
    uint8_t getClientCount() const;

    void setCommandCallback(CommandCallback callback);
    void setPatternData(uint32_t lineLength, uint32_t gapLength,
                        uint8_t patternIndex, const char* patternName);

private:
    AsyncWebServer* _server;
    AsyncWebSocket* _ws;

    CommandCallback _commandCallback;
    uint32_t _lastBroadcastTime;
    bool _wifiConnected;

    uint32_t _patternLineLength;
    uint32_t _patternGapLength;
    uint8_t _activePatternIndex;
    char _activePatternName[16];

    bool initWiFi();
    void setupRoutes();
    void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                          AwsEventType type, void* arg, uint8_t* data, size_t len);
    void processCommand(const char* json, AsyncWebSocketClient* client);

    String generateHTML();
    String generateCSS();
    String generateJS();
};

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

extern WebServerModule WebServer;

#endif // WEBSERVER_MODULE_H
