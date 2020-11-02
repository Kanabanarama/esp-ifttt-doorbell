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
        text-align: center;
        font-family: "Open Sans", sans-serif;
        font-size: 14px;
        background-color: lightgreen;
        margin: 0;
      }
      .container {
        display: flex;
        align-items: center;
        justify-content: center;
      }
      .top {
        width:100%;
        height:50px;
        position: fixed;
        top: 0;
        text-align: left;
        overflow: hidden;
        z-index: 9001;
      }
      .main {
        height: 100vh;
        justify-content: center;
        align-items: center;
        display: flex;
      }
      .title {
        width: 150px;
        height: 40px;
        position: relative;
        top: 20px;
        left: 10px;
        font-size: 16px;
      }
      .the-i {
        font-weight: bolder;
        color: darkgreen;
      }
      .status {
        position: relative;
        top: 6px;
        left: -30px;
        font-size: 12px;
        color: darkgreen;
      }
      .about {
        height: 40px;
        width: 40px;
        position: absolute;
        top: 10px;
        right: 10px;
      }
      .button {
        width: 300px;
        height: 300px;
        margin: 10px;
        font-size: 160px;
        text-align: center;
        cursor: pointer;
        color: #999;
        background-color: #fff;
        border: none;
        border-radius: 12px;
        box-shadow: 0 9px #999;
      }
      .button:active {
        color: #999;
        box-shadow: 0 3px #333;
        transform: translateY(4px);
      }
      .button:focus{
        border: none;
        outline: none;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <div class="top">
        <div class="logo">
          <span class="title">ESP-Kl<span class="the-i">I</span>ngel</span>
          <span class="status">&#11044; ONLINE</span>
        </div>
        <div class="about">
          <a href="https://github.com/kanabanarama/esp-ifttt-doorbell" target="_blank">
            <svg role="img" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
              <title>GitHub icon</title>
              <path d="M12 .297c-6.63 0-12 5.373-12 12 0 5.303 3.438 9.8 8.205 11.385.6.113.82-.258.82-.577 0-.285-.01-1.04-.015-2.04-3.338.724-4.042-1.61-4.042-1.61C4.422 18.07 3.633 17.7 3.633 17.7c-1.087-.744.084-.729.084-.729 1.205.084 1.838 1.236 1.838 1.236 1.07 1.835 2.809 1.305 3.495.998.108-.776.417-1.305.76-1.605-2.665-.3-5.466-1.332-5.466-5.93 0-1.31.465-2.38 1.235-3.22-.135-.303-.54-1.523.105-3.176 0 0 1.005-.322 3.3 1.23.96-.267 1.98-.399 3-.405 1.02.006 2.04.138 3 .405 2.28-1.552 3.285-1.23 3.285-1.23.645 1.653.24 2.873.12 3.176.765.84 1.23 1.91 1.23 3.22 0 4.61-2.805 5.625-5.475 5.92.42.36.81 1.096.81 2.22 0 1.606-.015 2.896-.015 3.286 0 .315.21.69.825.57C20.565 22.092 24 17.592 24 12.297c0-6.627-5.373-12-12-12"/>
            </svg>
          </a>
        </div>
      </div>
      <div class="main">
        <button class="button" onclick="sendOpenRequest()">&#128274;</button>
      </div>
    </div>
    <script>
      function sendOpenRequest(element) {
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
      if(path == "/connect") {
        if (!request->authenticate(USERNAME, PASSWORD)) {
          return request->requestAuthentication();
        }
        signals.push_back("connect");
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

  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    signals.push_back("update");
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void Webserver::loop() {}
