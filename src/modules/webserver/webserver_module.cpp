/**
 * @file webserver_module.cpp
 * @brief Implementacja modułu serwera WWW z obsługą wzorców
 * @version 1.1.0
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
    _activePatternIndex = 0;
    memset(_activePatternName, 0, sizeof(_activePatternName));
    strcpy(_activePatternName, "P-1a");
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

    if (!initWiFi()) {
        DEBUG_PRINTLN(F("[WEBSERVER] Blad inicjalizacji WiFi!"));
        return false;
    }

    _server = new AsyncWebServer(WEB_SERVER_PORT);
    _ws = new AsyncWebSocket(WEBSOCKET_PATH);

    _ws->onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client,
                        AwsEventType type, void* arg, uint8_t* data, size_t len) {
        this->onWebSocketEvent(server, client, type, arg, data, len);
    });

    _server->addHandler(_ws);
    setupRoutes();
    _server->begin();

    DEBUG_PRINTF("[WEBSERVER] Serwer uruchomiony na http://%s:%d\n",
                 getIPAddress().c_str(), WEB_SERVER_PORT);

    return true;
}

bool WebServerModule::initWiFi() {
    DEBUG_PRINTLN(F("[WEBSERVER] Konfiguracja WiFi AP..."));

    WiFi.mode(WIFI_AP);
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
    _server->on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "text/html", generateHTML());
    });

    _server->on("/style.css", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "text/css", generateCSS());
    });

    _server->on("/app.js", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "application/javascript", generateJS());
    });

    _server->on("/api/status", HTTP_GET, [](AsyncWebServerRequest* request) {
        JsonDocument doc;
        doc["firmware"] = FIRMWARE_VERSION;
        doc["uptime"] = millis() / 1000;
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });

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
                                        void* arg, uint8_t* data, size_t len) {
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
        default: break;
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

    if (_commandCallback) {
        _commandCallback(cmd, doc);
    }

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
    if (_ws) _ws->cleanupClients();
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
    doc["patternIdx"] = _activePatternIndex;
    doc["patternName"] = _activePatternName;

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

void WebServerModule::setPatternData(uint32_t lineLength, uint32_t gapLength,
                                      uint8_t patternIndex, const char* patternName) {
    _patternLineLength = lineLength;
    _patternGapLength = gapLength;
    _activePatternIndex = patternIndex;
    if (patternName) {
        strncpy(_activePatternName, patternName, sizeof(_activePatternName) - 1);
    }
}

// =============================================================================
// GENEROWANIE HTML Z PRZYCISKAMI WZORCÓW
// =============================================================================

String WebServerModule::generateHTML() {
    return R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>KTR251 - Road Marking Controller</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <header>
        <h1>KTR251</h1>
        <div class="header-info">
            <span class="pattern-name" id="patternName">P-1a</span>
            <span class="status" id="connStatus">...</span>
        </div>
    </header>

    <main>
        <section class="data-panel">
            <div class="data-card large">
                <label>Dystans</label>
                <div class="value"><span id="distance">0.0</span><span class="unit">m</span></div>
            </div>
            <div class="data-card">
                <label>Predkosc</label>
                <div class="value"><span id="speed">0.0</span><span class="unit">km/h</span></div>
            </div>
            <div class="data-card">
                <label>Cykle</label>
                <div class="value"><span id="cycles">0</span></div>
            </div>
        </section>

        <section class="pistols-panel">
            <h2>Pistolety</h2>
            <div class="pistol-grid">
                <button class="pistol-btn" data-id="0">1</button>
                <button class="pistol-btn" data-id="1">2</button>
                <button class="pistol-btn" data-id="2">3</button>
                <button class="pistol-btn" data-id="3">4</button>
                <button class="pistol-btn" data-id="4">5</button>
                <button class="pistol-btn" data-id="5">6</button>
            </div>
        </section>

        <section class="patterns-panel">
            <h2>Wzorce malowania</h2>
            <div class="pattern-grid">
                <button class="pat-btn" data-idx="0">P-1a</button>
                <button class="pat-btn" data-idx="1">P-1b</button>
                <button class="pat-btn" data-idx="2">P-1c</button>
                <button class="pat-btn" data-idx="3">P-1d</button>
                <button class="pat-btn" data-idx="4">P-1e</button>
                <button class="pat-btn" data-idx="5">P-2a</button>
                <button class="pat-btn" data-idx="6">P-2b</button>
                <button class="pat-btn" data-idx="7">P-3a</button>
                <button class="pat-btn" data-idx="8">P-3b</button>
                <button class="pat-btn" data-idx="9">P-4</button>
                <button class="pat-btn" data-idx="10">P-6</button>
                <button class="pat-btn" data-idx="11">P-7a</button>
                <button class="pat-btn" data-idx="12">P-7b</button>
                <button class="pat-btn" data-idx="13">P-7c</button>
                <button class="pat-btn" data-idx="14">P-7d</button>
            </div>
            <div class="pattern-info" id="patternInfo">
                <span id="patternDesc">Przerywana dluga</span>
                <span id="patternParams">4.0m / 8.0m / 12cm</span>
            </div>
        </section>

        <section class="control-panel">
            <button class="ctrl-btn start" onclick="sendCmd('start')">START</button>
            <button class="ctrl-btn stop" onclick="sendCmd('stop')">STOP</button>
            <button class="ctrl-btn reset" onclick="sendCmd('reset')">RESET</button>
        </section>
    </main>

    <footer>
        <span>v)rawliteral" + String(FIRMWARE_VERSION) + R"rawliteral(</span>
        <span id="painted">0m</span>
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
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:#1a1a2e;color:#eee;min-height:100vh;padding-bottom:40px}
header{background:#16213e;padding:0.8rem 1rem;display:flex;justify-content:space-between;align-items:center;border-bottom:2px solid #0f3460;position:sticky;top:0;z-index:100}
header h1{color:#00ff88;font-size:1.3rem}
.header-info{display:flex;gap:0.8rem;align-items:center}
.pattern-name{background:#00ff88;color:#000;padding:0.2rem 0.6rem;border-radius:4px;font-weight:bold;font-size:0.9rem}
.status{padding:0.2rem 0.6rem;border-radius:12px;font-size:0.75rem;background:#333}
.status.connected{background:#00aa55}
.status.disconnected{background:#aa3333}
main{padding:0.8rem;max-width:480px;margin:0 auto}
section{background:#16213e;border-radius:8px;padding:0.8rem;margin-bottom:0.8rem}
h2{font-size:0.8rem;color:#666;margin-bottom:0.6rem;text-transform:uppercase;letter-spacing:1px}
.data-panel{display:grid;grid-template-columns:1fr 1fr;gap:0.6rem}
.data-card{background:#0f3460;border-radius:6px;padding:0.8rem;text-align:center}
.data-card.large{grid-column:span 2}
.data-card label{display:block;font-size:0.65rem;color:#666;text-transform:uppercase;margin-bottom:0.2rem}
.data-card .value{font-size:1.8rem;font-weight:bold;color:#00ff88}
.data-card.large .value{font-size:2.5rem}
.data-card .unit{font-size:0.9rem;color:#555;margin-left:2px}
.pistol-grid{display:grid;grid-template-columns:repeat(6,1fr);gap:0.4rem}
.pistol-btn{aspect-ratio:1;border:none;border-radius:6px;font-size:1rem;font-weight:bold;cursor:pointer;background:#333;color:#555;transition:all 0.15s}
.pistol-btn.active{background:#00ff88;color:#000}
.pattern-grid{display:grid;grid-template-columns:repeat(5,1fr);gap:0.4rem;margin-bottom:0.6rem}
.pat-btn{padding:0.5rem 0.2rem;border:2px solid #333;border-radius:4px;background:transparent;color:#888;font-size:0.7rem;font-weight:bold;cursor:pointer;transition:all 0.15s}
.pat-btn.active{border-color:#00ff88;color:#00ff88;background:rgba(0,255,136,0.1)}
.pat-btn:active{transform:scale(0.95)}
.pattern-info{background:#0f3460;border-radius:4px;padding:0.5rem;text-align:center;display:flex;justify-content:space-between;font-size:0.8rem}
#patternDesc{color:#aaa}
#patternParams{color:#00ff88;font-weight:bold}
.control-panel{display:grid;grid-template-columns:1fr 1fr 1fr;gap:0.6rem}
.ctrl-btn{padding:1rem;border:none;border-radius:6px;font-size:0.9rem;font-weight:bold;cursor:pointer;transition:transform 0.1s}
.ctrl-btn:active{transform:scale(0.95)}
.ctrl-btn.start{background:#00aa55;color:#fff}
.ctrl-btn.stop{background:#aa3333;color:#fff}
.ctrl-btn.reset{background:#0f3460;color:#fff}
footer{position:fixed;bottom:0;left:0;right:0;background:#16213e;padding:0.4rem 1rem;display:flex;justify-content:space-between;font-size:0.75rem;color:#555;border-top:1px solid #0f3460}
)rawliteral";
}

// =============================================================================
// GENEROWANIE JAVASCRIPT
// =============================================================================

String WebServerModule::generateJS() {
    return R"rawliteral(
let ws,ri,cp=-1;
const pats=[
{n:'P-1a',d:'Przerywana dluga',l:4000,g:8000,w:120},
{n:'P-1b',d:'Przerywana krotka',l:2000,g:4000,w:120},
{n:'P-1c',d:'Wydzielajaca',l:2000,g:2000,w:120},
{n:'P-1d',d:'Prowadzaca waska',l:1000,g:1000,w:120},
{n:'P-1e',d:'Prowadzaca szeroka',l:1000,g:1000,w:240},
{n:'P-2a',d:'Ciagla waska',l:0,g:0,w:120},
{n:'P-2b',d:'Ciagla szeroka',l:0,g:0,w:240},
{n:'P-3a',d:'Przekraczalna dluga',l:4000,g:2000,w:120},
{n:'P-3b',d:'Przekraczalna krotka',l:1000,g:1000,w:120},
{n:'P-4',d:'Podwojna ciagla',l:0,g:0,w:240},
{n:'P-6',d:'Ostrzegawcza',l:4000,g:2000,w:120},
{n:'P-7a',d:'Krawedz. przeryw. szer.',l:1000,g:1000,w:240},
{n:'P-7b',d:'Krawedz. ciagla szer.',l:0,g:0,w:240},
{n:'P-7c',d:'Krawedz. przeryw. waska',l:1000,g:1000,w:120},
{n:'P-7d',d:'Krawedz. ciagla waska',l:0,g:0,w:120}
];

function connect(){
ws=new WebSocket(`ws://${location.host}/ws`);
ws.onopen=()=>{
document.getElementById('connStatus').textContent='OK';
document.getElementById('connStatus').className='status connected';
clearInterval(ri);
};
ws.onclose=()=>{
document.getElementById('connStatus').textContent='---';
document.getElementById('connStatus').className='status disconnected';
ri=setInterval(()=>{if(ws.readyState===3)connect();},3000);
};
ws.onmessage=(e)=>{
const d=JSON.parse(e.data);
if(d.type==='data')updateUI(d);
};
}

function updateUI(d){
document.getElementById('distance').textContent=d.distance.toFixed(1);
document.getElementById('speed').textContent=d.speed.toFixed(1);
document.getElementById('cycles').textContent=d.cycles;
document.getElementById('painted').textContent=(d.painted/1000).toFixed(1)+'m';

document.querySelectorAll('.pistol-btn').forEach((b,i)=>{
b.classList.toggle('active',(d.pistols&(1<<i))!==0);
});

if(d.patternIdx!==cp){
cp=d.patternIdx;
document.querySelectorAll('.pat-btn').forEach((b,i)=>{
b.classList.toggle('active',i===cp);
});
document.getElementById('patternName').textContent=d.patternName||pats[cp].n;
updatePatInfo(cp);
}
}

function updatePatInfo(i){
const p=pats[i];
document.getElementById('patternDesc').textContent=p.d;
if(p.l===0){
document.getElementById('patternParams').textContent='Ciagla / '+(p.w/10)+'cm';
}else{
document.getElementById('patternParams').textContent=(p.l/1000)+'m / '+(p.g/1000)+'m / '+(p.w/10)+'cm';
}
}

function sendCmd(c,p={}){
if(ws&&ws.readyState===1)ws.send(JSON.stringify({cmd:c,...p}));
}

document.querySelectorAll('.pistol-btn').forEach(b=>{
b.addEventListener('click',()=>sendCmd('togglePistol',{id:parseInt(b.dataset.id)}));
});

document.querySelectorAll('.pat-btn').forEach(b=>{
b.addEventListener('click',()=>{
const i=parseInt(b.dataset.idx);
sendCmd('setPatternByIndex',{index:i});
updatePatInfo(i);
});
});

connect();
)rawliteral";
}
