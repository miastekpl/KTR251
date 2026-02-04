/**
 * @file webserver_module.cpp
 * @brief Implementacja modułu serwera WWW
 * @version 1.0.0
 * @date 2026-02-04
 */

#include "webserver_module.h"

// =============================================================================
// GLOBALNA INSTANCJA
// =============================================================================

WebServerModule WebServer;

// =============================================================================
// KONSTRUKTOR / DESTRUKTOR
// =============================================================================

WebServerModule::WebServerModule() {
    _server = nullptr;
    _ws = nullptr;
    _commandCallback = nullptr;
    _lastBroadcastTime = 0;
    _wifiConnected = false;
    _patternLineLength = DEFAULT_LINE_LENGTH_MM;
    _patternGapLength = DEFAULT_GAP_LENGTH_MM;
}

WebServerModule::~WebServerModule() {
    if (_ws) delete _ws;
    if (_server) delete _server;
}

// =============================================================================
// INICJALIZACJA
// =============================================================================

bool WebServerModule::begin() {
    DEBUG_PRINTLN(F("[WEBSERVER] Inicjalizacja serwera WWW..."));

    // Inicjalizacja WiFi
    if (!initWiFi()) {
        DEBUG_PRINTLN(F("[WEBSERVER] Blad inicjalizacji WiFi!"));
        return false;
    }

    // Tworzenie serwera i WebSocket
    _server = new AsyncWebServer(WEB_SERVER_PORT);
    _ws = new AsyncWebSocket(WEBSOCKET_PATH);

    // Konfiguracja WebSocket
    _ws->onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client,
                        AwsEventType type, void* arg, uint8_t* data, size_t len) {
        this->onWebSocketEvent(server, client, type, arg, data, len);
    });

    _server->addHandler(_ws);

    // Konfiguracja routingu
    setupRoutes();

    // Start serwera
    _server->begin();

    DEBUG_PRINTF("[WEBSERVER] Serwer uruchomiony na http://%s:%d\n",
                 getIPAddress().c_str(), WEB_SERVER_PORT);

    return true;
}

bool WebServerModule::initWiFi() {
    DEBUG_PRINTLN(F("[WEBSERVER] Konfiguracja WiFi AP..."));

    // Tryb AP
    WiFi.mode(WIFI_AP);

    // Uruchomienie AP
    bool result = WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD,
                               WIFI_AP_CHANNEL, 0, WIFI_AP_MAX_CONN);

    if (result) {
        _wifiConnected = true;
        DEBUG_PRINTF("[WEBSERVER] AP uruchomiony: %s\n", WIFI_AP_SSID);
        DEBUG_PRINTF("[WEBSERVER] Haslo: %s\n", WIFI_AP_PASSWORD);
        DEBUG_PRINTF("[WEBSERVER] IP: %s\n", WiFi.softAPIP().toString().c_str());
    }

    return result;
}

// =============================================================================
// ROUTING HTTP
// =============================================================================

void WebServerModule::setupRoutes() {
    // Strona główna
    _server->on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "text/html", generateHTML());
    });

    // CSS
    _server->on("/style.css", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "text/css", generateCSS());
    });

    // JavaScript
    _server->on("/app.js", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "application/javascript", generateJS());
    });

    // API - Status
    _server->on("/api/status", HTTP_GET, [](AsyncWebServerRequest* request) {
        JsonDocument doc;
        doc["firmware"] = FIRMWARE_VERSION;
        doc["uptime"] = millis() / 1000;

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

    // 404
    _server->onNotFound([](AsyncWebServerRequest* request) {
        request->send(404, "text/plain", "Not found");
    });
}

// =============================================================================
// WEBSOCKET
// =============================================================================

void WebServerModule::onWebSocketEvent(AsyncWebSocket* server,
                                        AsyncWebSocketClient* client,
                                        AwsEventType type,
                                        void* arg,
                                        uint8_t* data,
                                        size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            DEBUG_PRINTF("[WEBSERVER] Klient polaczony: %u\n", client->id());
            break;

        case WS_EVT_DISCONNECT:
            DEBUG_PRINTF("[WEBSERVER] Klient rozlaczony: %u\n", client->id());
            break;

        case WS_EVT_DATA: {
            AwsFrameInfo* info = (AwsFrameInfo*)arg;
            if (info->final && info->index == 0 && info->len == len) {
                if (info->opcode == WS_TEXT) {
                    data[len] = 0;
                    processCommand((char*)data, client);
                }
            }
            break;
        }

        case WS_EVT_ERROR:
            DEBUG_PRINTF("[WEBSERVER] Blad WebSocket: %u\n", client->id());
            break;

        default:
            break;
    }
}

void WebServerModule::processCommand(const char* json, AsyncWebSocketClient* client) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        DEBUG_PRINTF("[WEBSERVER] Blad parsowania JSON: %s\n", error.c_str());
        return;
    }

    const char* cmd = doc["cmd"];
    if (!cmd) return;

    DEBUG_PRINTF("[WEBSERVER] Komenda: %s\n", cmd);

    // Wywołaj callback
    if (_commandCallback) {
        _commandCallback(cmd, doc);
    }

    // Odpowiedź
    JsonDocument response;
    response["status"] = "ok";
    response["cmd"] = cmd;

    String responseStr;
    serializeJson(response, responseStr);
    client->text(responseStr);
}

// =============================================================================
// BROADCAST
// =============================================================================

void WebServerModule::update() {
    // Cleanup starych połączeń
    if (_ws) {
        _ws->cleanupClients();
    }
}

void WebServerModule::broadcastData(const WebServerData& data) {
    if (!_ws || _ws->count() == 0) return;

    JsonDocument doc;
    doc["type"] = "data";
    doc["distance"] = data.distanceM;
    doc["speed"] = data.speedKMH;
    doc["cycles"] = data.cycleCount;
    doc["painted"] = data.totalPaintedM;
    doc["running"] = data.isRunning;
    doc["paused"] = data.isPaused;
    doc["pistols"] = data.activePistols;
    doc["mode"] = data.mode;
    doc["pulses"] = data.pulseCount;
    doc["lineLen"] = _patternLineLength;
    doc["gapLen"] = _patternGapLength;

    String json;
    serializeJson(doc, json);
    _ws->textAll(json);
}

void WebServerModule::broadcastMessage(const char* message) {
    if (!_ws || _ws->count() == 0) return;

    JsonDocument doc;
    doc["type"] = "message";
    doc["text"] = message;

    String json;
    serializeJson(doc, json);
    _ws->textAll(json);
}

// =============================================================================
// GETTERY / SETTERY
// =============================================================================

String WebServerModule::getIPAddress() const {
    return WiFi.softAPIP().toString();
}

bool WebServerModule::isConnected() const {
    return _wifiConnected;
}

uint8_t WebServerModule::getClientCount() const {
    return _ws ? _ws->count() : 0;
}

void WebServerModule::setCommandCallback(CommandCallback callback) {
    _commandCallback = callback;
}

void WebServerModule::setPatternData(uint32_t lineLength, uint32_t gapLength) {
    _patternLineLength = lineLength;
    _patternGapLength = gapLength;
}

// =============================================================================
// GENEROWANIE HTML
// =============================================================================

String WebServerModule::generateHTML() {
    return R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>KTR251 - Road Marking Controller</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <header>
        <h1>KTR251</h1>
        <span class="status" id="connStatus">Laczenie...</span>
    </header>

    <main>
        <section class="data-panel">
            <div class="data-card large">
                <label>Dystans</label>
                <div class="value"><span id="distance">0.0</span> <span class="unit">m</span></div>
            </div>

            <div class="data-card">
                <label>Predkosc</label>
                <div class="value"><span id="speed">0.0</span> <span class="unit">km/h</span></div>
            </div>

            <div class="data-card">
                <label>Cykle</label>
                <div class="value"><span id="cycles">0</span></div>
            </div>

            <div class="data-card">
                <label>Namalowano</label>
                <div class="value"><span id="painted">0</span> <span class="unit">m</span></div>
            </div>
        </section>

        <section class="pistols-panel">
            <h2>Pistolety</h2>
            <div class="pistol-grid" id="pistolGrid">
                <button class="pistol-btn" data-id="0">1</button>
                <button class="pistol-btn" data-id="1">2</button>
                <button class="pistol-btn" data-id="2">3</button>
                <button class="pistol-btn" data-id="3">4</button>
                <button class="pistol-btn" data-id="4">5</button>
                <button class="pistol-btn" data-id="5">6</button>
            </div>
        </section>

        <section class="pattern-panel">
            <h2>Wzor malowania</h2>
            <div class="pattern-inputs">
                <div class="input-group">
                    <label>Dlugosc linii (mm)</label>
                    <input type="number" id="lineLength" value="6000" min="100" max="50000">
                </div>
                <div class="input-group">
                    <label>Dlugosc przerwy (mm)</label>
                    <input type="number" id="gapLength" value="12000" min="100" max="100000">
                </div>
                <button class="btn" onclick="setPattern()">Ustaw wzor</button>
            </div>
        </section>

        <section class="mode-panel">
            <h2>Tryb pracy</h2>
            <div class="mode-buttons">
                <button class="mode-btn" data-mode="0">Reczny</button>
                <button class="mode-btn" data-mode="1">Auto Linia</button>
                <button class="mode-btn" data-mode="2">Auto Ciagla</button>
            </div>
        </section>

        <section class="control-panel">
            <button class="ctrl-btn start" onclick="sendCmd('start')">START</button>
            <button class="ctrl-btn stop" onclick="sendCmd('stop')">STOP</button>
            <button class="ctrl-btn" onclick="sendCmd('reset')">RESET</button>
        </section>
    </main>

    <footer>
        <span>KTR251 v)rawliteral" + String(FIRMWARE_VERSION) + R"rawliteral(</span>
        <span id="wsClients">Klienci: 0</span>
    </footer>

    <script src="/app.js"></script>
</body>
</html>
)rawliteral";
}

// =============================================================================
// GENEROWANIE CSS
// =============================================================================

String WebServerModule::generateCSS() {
    return R"rawliteral(
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
    background: #1a1a2e;
    color: #eee;
    min-height: 100vh;
}

header {
    background: #16213e;
    padding: 1rem;
    display: flex;
    justify-content: space-between;
    align-items: center;
    border-bottom: 2px solid #0f3460;
}

header h1 {
    color: #00ff88;
    font-size: 1.5rem;
}

.status {
    padding: 0.3rem 0.8rem;
    border-radius: 20px;
    font-size: 0.8rem;
    background: #333;
}

.status.connected { background: #00aa55; }
.status.disconnected { background: #aa3333; }

main {
    padding: 1rem;
    max-width: 600px;
    margin: 0 auto;
}

section {
    background: #16213e;
    border-radius: 10px;
    padding: 1rem;
    margin-bottom: 1rem;
}

h2 {
    font-size: 1rem;
    color: #888;
    margin-bottom: 0.8rem;
    text-transform: uppercase;
    letter-spacing: 1px;
}

.data-panel {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 0.8rem;
}

.data-card {
    background: #0f3460;
    border-radius: 8px;
    padding: 1rem;
    text-align: center;
}

.data-card.large {
    grid-column: span 2;
}

.data-card label {
    display: block;
    font-size: 0.75rem;
    color: #888;
    text-transform: uppercase;
    margin-bottom: 0.3rem;
}

.data-card .value {
    font-size: 2rem;
    font-weight: bold;
    color: #00ff88;
}

.data-card.large .value {
    font-size: 3rem;
}

.data-card .unit {
    font-size: 1rem;
    color: #666;
}

.pistol-grid {
    display: grid;
    grid-template-columns: repeat(6, 1fr);
    gap: 0.5rem;
}

.pistol-btn {
    aspect-ratio: 1;
    border: none;
    border-radius: 8px;
    font-size: 1.2rem;
    font-weight: bold;
    cursor: pointer;
    background: #333;
    color: #666;
    transition: all 0.2s;
}

.pistol-btn.active {
    background: #00ff88;
    color: #000;
}

.pistol-btn.enabled {
    background: #0f3460;
    color: #fff;
}

.pattern-inputs {
    display: grid;
    gap: 0.8rem;
}

.input-group label {
    display: block;
    font-size: 0.8rem;
    color: #888;
    margin-bottom: 0.3rem;
}

.input-group input {
    width: 100%;
    padding: 0.6rem;
    border: 1px solid #333;
    border-radius: 5px;
    background: #0f3460;
    color: #fff;
    font-size: 1rem;
}

.btn {
    padding: 0.8rem 1.5rem;
    border: none;
    border-radius: 5px;
    background: #0f3460;
    color: #fff;
    font-size: 1rem;
    cursor: pointer;
    transition: background 0.2s;
}

.btn:hover {
    background: #1a4a7a;
}

.mode-buttons {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 0.5rem;
}

.mode-btn {
    padding: 0.8rem;
    border: 2px solid #333;
    border-radius: 5px;
    background: transparent;
    color: #888;
    font-size: 0.85rem;
    cursor: pointer;
    transition: all 0.2s;
}

.mode-btn.active {
    border-color: #00ff88;
    color: #00ff88;
}

.control-panel {
    display: grid;
    grid-template-columns: 1fr 1fr 1fr;
    gap: 0.8rem;
}

.ctrl-btn {
    padding: 1.2rem;
    border: none;
    border-radius: 8px;
    font-size: 1rem;
    font-weight: bold;
    cursor: pointer;
    transition: transform 0.1s;
}

.ctrl-btn:active {
    transform: scale(0.95);
}

.ctrl-btn.start {
    background: #00aa55;
    color: #fff;
}

.ctrl-btn.stop {
    background: #aa3333;
    color: #fff;
}

footer {
    position: fixed;
    bottom: 0;
    left: 0;
    right: 0;
    background: #16213e;
    padding: 0.5rem 1rem;
    display: flex;
    justify-content: space-between;
    font-size: 0.8rem;
    color: #666;
}
)rawliteral";
}

// =============================================================================
// GENEROWANIE JAVASCRIPT
// =============================================================================

String WebServerModule::generateJS() {
    return R"rawliteral(
let ws;
let reconnectInterval;
let currentMode = 0;

function connect() {
    const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
    ws = new WebSocket(`${protocol}//${location.host}/ws`);

    ws.onopen = () => {
        document.getElementById('connStatus').textContent = 'Polaczony';
        document.getElementById('connStatus').className = 'status connected';
        clearInterval(reconnectInterval);
    };

    ws.onclose = () => {
        document.getElementById('connStatus').textContent = 'Rozlaczony';
        document.getElementById('connStatus').className = 'status disconnected';
        reconnectInterval = setInterval(() => {
            if (ws.readyState === WebSocket.CLOSED) connect();
        }, 3000);
    };

    ws.onmessage = (event) => {
        const data = JSON.parse(event.data);
        if (data.type === 'data') {
            updateUI(data);
        }
    };
}

function updateUI(data) {
    document.getElementById('distance').textContent = data.distance.toFixed(1);
    document.getElementById('speed').textContent = data.speed.toFixed(1);
    document.getElementById('cycles').textContent = data.cycles;
    document.getElementById('painted').textContent = (data.painted / 1000).toFixed(1);

    // Aktualizuj pistolety
    const pistolBtns = document.querySelectorAll('.pistol-btn');
    pistolBtns.forEach((btn, i) => {
        const isActive = (data.pistols & (1 << i)) !== 0;
        btn.classList.toggle('active', isActive);
    });

    // Aktualizuj tryb
    if (data.mode !== currentMode) {
        currentMode = data.mode;
        document.querySelectorAll('.mode-btn').forEach(btn => {
            btn.classList.toggle('active', parseInt(btn.dataset.mode) === currentMode);
        });
    }

    // Aktualizuj wzor
    if (data.lineLen) document.getElementById('lineLength').value = data.lineLen;
    if (data.gapLen) document.getElementById('gapLength').value = data.gapLen;
}

function sendCmd(cmd, params = {}) {
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ cmd, ...params }));
    }
}

function setPattern() {
    const lineLen = parseInt(document.getElementById('lineLength').value);
    const gapLen = parseInt(document.getElementById('gapLength').value);
    sendCmd('setPattern', { lineLen, gapLen });
}

// Event listeners
document.querySelectorAll('.pistol-btn').forEach(btn => {
    btn.addEventListener('click', () => {
        sendCmd('togglePistol', { id: parseInt(btn.dataset.id) });
    });
});

document.querySelectorAll('.mode-btn').forEach(btn => {
    btn.addEventListener('click', () => {
        sendCmd('setMode', { mode: parseInt(btn.dataset.mode) });
    });
});

// Start
connect();
)rawliteral";
}
