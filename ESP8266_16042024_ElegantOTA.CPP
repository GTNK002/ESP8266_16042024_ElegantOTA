#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>


const char* ssid = "---------";
const char* password = "---------";

AsyncWebServer server(80);

// IR Remote Code for Lights
#define IR1            0x1FE48B7 //pw
#define IR2            0x1FE58A7 //mode
#define IR3            0x1FE7887 //mute
#define IR4            0x1FE807F //play
#define IR5            0x1FE40BF //backward
#define IR6            0x1FEC03F //forward
#define IR7            0x1FE20DF // eq
#define IR8            0x1FEA05F // v-
#define IR9            0x1FE609F // v+
#define IR10           0x1FE10EF // rpt
#define IR11           0x1FE906F // u/sd
#define IRF0           0x1FEE01F // 0
#define IRF1           0x1FE50AF // 1
#define IRF2           0x1FED827 // 2
#define IRF3           0x1FEF807 // 3
#define IRF4           0x1FE30CF // 4
#define IRF5           0x1FEB04F // 5
#define IRF6           0x1FE708F // 6
#define IRF7           0x1FE00FF // 7
#define IRF8           0x1FEF00F // 8
#define IRF9           0x1FE9867 // 9

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Smart Home</title>
<style>
body {
    font-family: Arial, sans-serif;
    background-color: Black;
    margin: 0;
    padding: 0;
}

.container {
    max-width: 360px;
    height: 500px;
    margin: 0 top;
    padding: 20px;
    background-color: grey;
    border-radius: 10px;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
}

.smart-home {
    text-align: center;
    margin-bottom: 20px;
}

.smart-home h1 {
    color: #333;
}

.smart-home h2 {
    color: #666;
    font-size: 18px;
}

.button-container {
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
}

.button-container button {
    width: 100px;
    margin: 1px;
    padding: 10px;
    border: none;
    border-radius: 5px;
    background-color: #646362;
    color: #CBCBCB;
    font-size: 14px;
    cursor: pointer;
    transition: background-color 0.3s;
}

a {
    text-decoration: none;
    color: #333;
}

a button {
    background-color: #008CBA;
}

a button:hover {
    background-color: #0073a5;
}
#serialMonitorContent {
    display: none;
    background-color: #f0f0f0;
    border-radius: 5px;
    padding: 10px;
    height: 200px;
    overflow: auto;
}
</style>
</head>
<body>
<div class="container">
  <div class="smart-home">
    <h1>Smart Home</h1>
    <h2>Bedroom 2 | Fan: <span id="fanStatus">Speed4</span></h2>
  </div>

  <div class="button-container">
    <button onclick="sendws('switch1')">Socket<br><span id="socketStatus">On</span></button>
    <button onclick="sendws('switch2')">Night<br><span id="nightStatus">Off</span></button>
    <button onclick="sendws('switch3')">Light<br><span id="lightStatus">On</span></button>
    <button onclick="sendws('fanOff')">Fan<br>OFF</button>
    <button onclick="sendws('fanSpeed1')">Fan<br>Speed 1</button>
    <button onclick="sendws('fanSpeed2')">Fan<br>Speed 2</button>
    <button onclick="sendws('fanSpeed3')">Fan<br>Speed 3</button>
    <button onclick="sendws('fanSpeed4')">Fan<br>Speed 4</button>
    <button onclick="openSerial()">Nerd<br>Page</button>
    <button onclick="sendws('tvButton')">TV<br>On/Off</button>
    <button onclick="sendws('acButton')">AC<br>On/Off</button>
    <button onclick="sendws('htButton')">Speaker<br>On/Off</button>
    <a href="/tv"><button>TV<br>Page</button></a>
    <a href="/ac"><button>AC<br>Page</button></a>
    <a href="/ht"><button>Speaker<br>Page</button></a>
  </div>
  <!-- Serial Monitor Content -->
  <div id="serialMonitorContent">
    <h2>Serial Monitor <button onclick="clearSerialMonitor()">Clear</button></h2>
    <div id="serialMonitorContentInner">
    </div>
    <button onclick="clearSerialMonitor()">Clear</button>
  </div>
</div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);

  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onLoad(event) {
    initWebSocket();
  }
  function sendws(x) {
    var serialMonitorContentInner = document.getElementById('serialMonitorContentInner');
    serialMonitorContentInner.innerHTML += '<p>' + x + '</p>';
    
    websocket.send(x);
  }
  function onMessage(event) {
    var data = event.data;

    var serialMonitorContentInner = document.getElementById('serialMonitorContentInner');
    serialMonitorContentInner.innerHTML += '<p>' + data + '</p>';
    
    if (data === 'switch1On') {
      document.getElementById('socketStatus').innerText = 'On';
    } 
    else if (data === 'switch1Off') {
      document.getElementById('socketStatus').innerText = 'Off';
    }
    else if (data === 'switch2On') {
      document.getElementById('nightStatus').innerText = 'On';
    } 
    else if (data === 'switch2Off') {
      document.getElementById('nightStatus').innerText = 'Off';
    }
    else if (data === 'switch3On') {
      document.getElementById('lightStatus').innerText = 'On';
    } 
    else if (data === 'switch3Off') {
      document.getElementById('lightStatus').innerText = 'Off';
    }
    else if (data === 'fanOff') {
      document.getElementById('fanStatus').innerText = 'Off';
    }
    else if (data === 'fanSpeed1') {
      document.getElementById('fanStatus').innerText = 'Speed1';
    }
    else if (data === 'fanSpeed2') {
      document.getElementById('fanStatus').innerText = 'Speed2';
    }
    else if (data === 'fanSpeed3') {
      document.getElementById('fanStatus').innerText = 'Speed3';
    }
    else if (data === 'fanSpeed4') {
      document.getElementById('fanStatus').innerText = 'Speed4';
    }
    else if (data === 'fanSpeed5') {
      document.getElementById('fanStatus').innerText = 'Speed5';
    }
  }
  function openSerial() {
    var serialMonitorContent = document.getElementById('serialMonitorContent');
    if (serialMonitorContent.style.display === 'none') {
        serialMonitorContent.style.display = 'block';
    } else {
        serialMonitorContent.style.display = 'none';
    }
  }
  function clearSerialMonitor() {
    var serialMonitorContentInner = document.getElementById('serialMonitorContentInner');
    serialMonitorContentInner.innerHTML = '';
  }
</script>
</body>
</html>
)rawliteral";

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! This is a sample response.");
  });

  AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
}


