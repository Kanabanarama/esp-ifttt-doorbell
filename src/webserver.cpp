#include <webserver.h>

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML>
<html>
<head>
  <title>ESP-Doorbell</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    *, *:after, *:before {
      box-sizing: border-box;
    }
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
      font-family: 'Open Sans', sans-serif;
      font-size: 14px;
      text-align: center;
      background-color: lightgreen;
    }
    .container {
      padding: 60px 80px;
      background-color: white;
      box-shadow: 0 0 4px 1px #BBB;
      margin: auto;
      text-align: center;
    }
    .wrap {
      position: relative;
      width: 80px;
      height: 80px;
      margin: 20px auto 30px auto;
    }
    .wrap:last-child {
      margin-bottom: 0;
    }
    .clicker {
      background-color: white;
      outline: none;
      font-size: 50px;
      font-weight: 600;
      position: absolute;
      cursor: pointer;
      padding: 0;
      border: none;
      height: 64px;
      width: 64px;
      left: 8px;
      top: 8px;
      border-radius: 100px;
      z-index: 2;
    }
    .clicker:active {
      transform: translate(0, 1px);
      height: 63px;
      box-shadow: 0px 1px 0 0 #bebebe inset;
    }
    .circle {
      position: relative;
      border-radius: 40px;
      width: 80px;
      height: 80px;
      z-index: 1;
    }
    .circle.third {
      border-radius: 0;
    }
    .clicker.faster:hover + .circle, .clicker.faster:active + .circle {
      animation: rotator linear .4s infinite;
    }
    .clicker.fast:hover + .circle, .clicker.fast:active + .circle {
      animation: rotator linear .5s infinite;
    }
    .clicker:hover + .circle, .clicker:active + .circle {
      animation: rotator linear .8s infinite;
    }
    @keyframes rotator {
      from {
        transform: rotate(0deg);
      }
      to {
        transform: rotate(360deg);
      }
    }
    .angled {
      background-image: linear-gradient(45deg, white 0%, white 30%, #14beeb 30%, #14beeb 70%, white 70%, white 100%);
    }
    .angled.second {
      background-image: linear-gradient(white 0%, white 30%, #faa078 30%, #faa078 70%, white 70%, white 100%);
    }
    .angled.third {
      background-image: linear-gradient(45deg, white 0%, white 30%, #82e687 30%, #82e687 70%, white 70%, white 100%);
    }
    .status {
      float: right;
      color: dark-green;
    }
  </style>
</head>
<body>
  <h2>ESP-Klingel</h2>
  %PLACEHOLDER%
  <div class="container">
    <div class="wrap">
    <button class="clicker" onclick="sendOpenRequest()">&#128274;</button>
      <div class="circle angled"></div>
    </div>
  </div>
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
      Serial.println(request->url().c_str());
      request->send_P(200, "text/html", index_html, processor);
    }
};

AsyncWebServer server(80);

void Webserver::setup() {
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/open", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("Opening door.");
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void Webserver::loop() {}