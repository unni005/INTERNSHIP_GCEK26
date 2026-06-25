#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "iQOO Z11x 5G";
const char* password = "unni@2005";

WebServer server(80);

const int ledPin = 2;

// PWM Configuration
const int freq = 5000;
const int channel = 0;
const int resolution = 8;

int brightness = 0;

void handleRoot() {

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP32 LED Fader</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<style>

*{
  margin:0;
  padding:0;
  box-sizing:border-box;
  font-family:'Segoe UI',sans-serif;
}

body{
  display:flex;
  justify-content:center;
  align-items:center;
  min-height:100vh;
  background:linear-gradient(135deg,#0f172a,#1e293b);
}

.container{
  width:400px;
  max-width:90%;
  padding:30px;
  background:rgba(255,255,255,0.08);
  backdrop-filter:blur(10px);
  border-radius:20px;
  box-shadow:0 8px 32px rgba(0,0,0,0.3);
  text-align:center;
  color:white;
}

h1{
  margin-bottom:15px;
  font-size:28px;
}

.status{
  margin-bottom:20px;
  color:#22c55e;
  font-size:14px;
  font-weight:bold;
}

button{
  padding:12px 30px;
  margin:10px;
  border:none;
  border-radius:12px;
  font-size:16px;
  font-weight:bold;
  cursor:pointer;
  transition:0.3s;
}

button:hover{
  transform:translateY(-3px);
  box-shadow:0 5px 15px rgba(255,255,255,0.2);
}

.on-btn{
  background:#22c55e;
  color:white;
}

.off-btn{
  background:#ef4444;
  color:white;
}

.slider{
  width:100%;
  height:8px;
  border-radius:10px;
  appearance:none;
  background:#475569;
  outline:none;
  margin-top:15px;
}

.slider::-webkit-slider-thumb{
  appearance:none;
  width:22px;
  height:22px;
  border-radius:50%;
  background:#38bdf8;
  cursor:pointer;
  box-shadow:0 0 10px #38bdf8;
}

.slider::-moz-range-thumb{
  width:22px;
  height:22px;
  border-radius:50%;
  background:#38bdf8;
  cursor:pointer;
  border:none;
}

h3{
  margin-top:25px;
}

#value{
  color:#38bdf8;
  font-weight:bold;
  font-size:24px;
}

.footer{
  margin-top:25px;
  font-size:12px;
  color:#cbd5e1;
}

</style>

<script>
function updateBrightness(val){
  document.getElementById("value").innerHTML = val;

  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/set?value=" + val, true);
  xhttp.send();
}
</script>

</head>

<body>

<div class="container">

<h1>ESP32 LED Controller</h1>

<div class="status">
🟢 ESP32 Connected
</div>

<a href="/on">
<button class="on-btn">LED ON</button>
</a>

<a href="/off">
<button class="off-btn">LED OFF</button>
</a>

<h3>Brightness: <span id="value">%BRIGHTNESS%</span></h3>

<input type="range"
min="0"
max="255"
value="%BRIGHTNESS%"
class="slider"
oninput="updateBrightness(this.value)">

<div class="footer">
ESP32 LED Fader Control Panel
</div>

</div>

</body>
</html>
)rawliteral";

  html.replace("%BRIGHTNESS%", String(brightness));

  server.send(200, "text/html", html);
}


void handleOn() {
  brightness = 255;
  ledcWrite(channel, brightness);

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  brightness = 0;
  ledcWrite(channel, brightness);

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSet() {

  if(server.hasArg("value")) {
    brightness = server.arg("value").toInt();

    brightness = constrain(brightness, 0, 255);

    ledcWrite(channel, brightness);
  }

  server.send(200, "text/plain", "OK");
}

void setup() {

  Serial.begin(115200);

  ledcSetup(channel, freq, resolution);
  ledcAttachPin(ledPin, channel);

  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/set", handleSet);

  server.begin();

  Serial.println("Server Started");
}

void loop() {
  server.handleClient();
}