#include <webserver.h>

vector<string> signals;

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML>
<html>
  <head>
    <title>ESP-Doorbell</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <link rel="icon" href="data:," />
    <style type="text/css">
      body {
        display: -webkit-box;
        display: -moz-box;
        display: box;
        display: -webkit-flex;
        display: -moz-flex;
        display: -ms-flexbox;
        display: flex;
        -webkit-align-content: center;
        -moz-align-content: center;
        align-content: center;
        -ms-flex-line-pack: center;
        color: #353535;
        min-height: 100vh;
        font-family: "Open Sans", sans-serif;
        font-size: 14px;
        text-align: center;
        background-color: lightgreen;
      }
      .top {
        width: 100%;
        position: absolute;
        text-align: left;
        top: 0;
        left: 0;
      }
      .container {
        margin: auto;
        text-align: center;
      }
      .button {
        padding: 35px 45px;
        font-size: 66px;
        text-align: center;
        cursor: pointer;
        outline: none;
        color: #fff;
        background-color: #cf5808;
        border: none;
        border-radius: 12px;
        box-shadow: 0 9px #999;
      }
      .button:active {
        box-shadow: 0 3px rgb(56, 56, 56);
        transform: translateY(4px);
      }
      .title {
        position: relative;
        top: 20px;
        left: 10px;
        font-size: 16px;
      }
      .the-i {
        font-style: bolder;
        color: darkgreen;
      }
      .status {
        position: relative;
        top: 6px;
        left: -30px;
        font-size: 12px;
        color: darkgreen;
      }
    </style>
  </head>
  <body>

    <div class="container">
      <div class="top">
        <span class="title">ESP-Kl<span class="the-i">I</span>ngel</span>
        <span class="status">&#11044; ONLINE</span>
      </div>

      <button class="button" onclick="sendOpenRequest()">&#128274;</button>
    </div>
    <script>
      function sendOpenRequest() {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/open", true);
        xhr.send();
      }
    </script>
  </body>
</html>
)rawliteral";

String processor(const String& var) {
  if(var == "PLACEHOLDER"){
    String replace = "<small class=\"status\">ONLINE</small>";
    return replace;
  }
  return String();
}

class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request) {
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      String path = request->url().c_str();

      if(path == "/") {
        if (!request->authenticate(USERNAME, PASSWORD)) {
          return request->requestAuthentication();
        }
        Serial.println("New client in AP mode.");
        request->send_P(200, "text/html", index_html, processor);
      }

      if(path == "/open") {
        if (!request->authenticate(USERNAME, PASSWORD)) {
          return request->requestAuthentication();
        }
        signals.push_back("open");
        request->send(200, "text/plain", "OK");
      }
    }
};

boolean Webserver::received(string signalName) {
  vector<string>::iterator result = find(signals.begin(), signals.end(), signalName);
  boolean found = false;
  if (result != signals.end()) {
    signals.erase(result);
    found = true;
  }
  return found;
}

AsyncWebServer server(80);

void Webserver::setup() {
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("New client in STA mode.");
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/open", HTTP_GET, [] (AsyncWebServerRequest *request) {
    signals.push_back("open");
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void Webserver::loop() {}
