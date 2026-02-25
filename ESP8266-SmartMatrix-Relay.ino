#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

// Pinout NodeMCU
#define CLK_PIN   D5
#define DATA_PIN  D7
#define CS_PIN    D8

// Pin relay
#define RELAY_PIN_D1 D1
#define RELAY_PIN_D2 D2
#define RELAY_PIN_D3 D3
#define RELAY_PIN_D4 D4

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
ESP8266WebServer server(80);

const char *ssid = "your_SSID";
const char *password = "your_PASSWORD";
String currentText = "Selamat Datang";
bool animationState = true;
int animationSpeed = 100;
textEffect_t selectedEffect = PA_SCROLL_LEFT;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Begin server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/edit", HTTP_GET, handleEdit);
  server.on("/toggleAnimation", HTTP_GET, handleToggleAnimation);
  server.on("/setAnimationSpeed", HTTP_GET, handleSetAnimationSpeed);
  server.on("/setAnimationEffect", HTTP_GET, handleSetAnimationEffect);
  server.on("/toggleRelayD1", HTTP_GET, handleToggleRelayD1);
  server.on("/toggleRelayD2", HTTP_GET, handleToggleRelayD2);
  server.on("/toggleRelayD3", HTTP_GET, handleToggleRelayD3);
  server.on("/toggleRelayD4", HTTP_GET, handleToggleRelayD4);
  server.begin();

  myDisplay.begin();
  myDisplay.displayText(currentText.c_str(), PA_CENTER, animationSpeed, 0, selectedEffect, selectedEffect);

  // Inisialisasi pin untuk relay sebagai OUTPUT
  pinMode(RELAY_PIN_D1, OUTPUT);
  pinMode(RELAY_PIN_D2, OUTPUT);
  pinMode(RELAY_PIN_D3, OUTPUT);
  pinMode(RELAY_PIN_D4, OUTPUT);

  digitalWrite(RELAY_PIN_D1, HIGH); // Relay D1 dimatikan saat startup
  digitalWrite(RELAY_PIN_D2, HIGH); // Relay D2 dimatikan saat startup
  digitalWrite(RELAY_PIN_D3, HIGH); // Relay D3 dimatikan saat startup
  digitalWrite(RELAY_PIN_D4, HIGH); // Relay D4 dimatikan saat startup
}

void loop() {
  // Handle WiFi requests
  server.handleClient();

  // Animasi running text
  if (animationState && myDisplay.displayAnimate()) {
    // Animasi selesai, atur teks baru
    myDisplay.displayReset();
    myDisplay.displayText(currentText.c_str(), PA_CENTER, animationSpeed, 0, selectedEffect, selectedEffect);
  }
}

void handleRoot() {
  String page = "<!DOCTYPE html>";
  page += "<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  page += "<title>ESP8266 Web Config</title>";
  page += "<link href='https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css' rel='stylesheet'>";
  page += "<style>body {background-color: #343a40; color: #ffffff; font-family: 'Arial, sans-serif';} ";
  page += ".card {margin-top: 20px; background-color: #2C3E50; color: #ECF0F1; padding: 15px; border-radius: 10px;}";
  page += ".form-group {margin-bottom: 15px;} .btn-primary, .btn-success, .btn-warning {margin-top: 10px;}</style>";
  page += "</head><body><div class='container'>";
  page += "<h1 class='mt-5 text-center'>ESP8266 Web Config</h1>";
  page += "<div class='card mt-4'><div class='card-body'>";
  page += "<h4 class='card-title'>Current Text:</h4>";
  page += "<p class='card-text h5'>" + currentText + "</p>";
  page += "<button class='btn btn-primary btn-block' onclick='toggleAnimation()'>Toggle Animation</button>";
  page += "<form action='/edit' method='get'>";
  page += "<div class='form-group'>";
  page += "<label for='newText'>New Text:</label>";
  page += "<input type='text' class='form-control' id='newText' name='text'>";
  page += "</div>";
  page += "<button type='submit' class='btn btn-success btn-block'>Submit</button>";
  page += "</form>";

  // Opsi animasi
  page += "<form class='mt-4'>";
  page += "<div class='form-group'>";
  page += "<label for='animationSpeed'>Animation Speed:</label>";
  page += "<input type='number' class='form-control' id='animationSpeed' name='speed' value='" + String(animationSpeed) + "'>";
  page += "</div>";
  page += "<div class='form-group'>";
  page += "<label for='animationEffect'>Animation Effect:</label>";
  page += "<select class='form-control' id='animationEffect' name='effect'>";
  page += "<option value='0' " + String(selectedEffect == PA_NO_EFFECT ? "selected" : "") + ">No Effect</option>";
  page += "<option value='1' " + String(selectedEffect == PA_SCROLL_LEFT ? "selected" : "") + ">Blink</option>";
  page += "<option value='2' " + String(selectedEffect == PA_SCROLL_RIGHT ? "selected" : "") + ">Geser ke Atas</option>";
  page += "<option value='3' " + String(selectedEffect == PA_SCROLL_UP ? "selected" : "") + ">Geser Ke Bawah</option>";
  page += "<option value='4' " + String(selectedEffect == PA_SCROLL_DOWN ? "selected" : "") + ">Geser Ke Kiri</option>";
  page += "<option value='5' " + String(selectedEffect == PA_FADE ? "selected" : "") + ">Geser Ke Kanan</option>";
  page += "</select>";
  page += "</div>";
  page += "<button type='button' class='btn btn-warning btn-block' onclick='updateSettings()'>Update Settings</button>";
  page += "</form></div></div>";

  // Kontrol Relay
  page += "<div class='card mt-4'><div class='card-body'>";
  page += "<h4 class='card-title'>Relay Control:</h4>";
  page += "<button class='btn btn-danger btn-block' onclick='toggleRelayD1()'>Toggle Relay 1</button>";
  page += "<button class='btn btn-danger btn-block' onclick='toggleRelayD2()'>Toggle Relay 2</button>";
  page += "<button class='btn btn-danger btn-block' onclick='toggleRelayD3()'>Toggle Relay 3</button>";
  page += "<button class='btn btn-danger btn-block' onclick='toggleRelayD4()'>Toggle Relay 4</button>";
  page += "</div></div>";

  page += "<div class='card mt-4'><div class='card-body'>";
  page += "<h4 class='card-title'>Status:</h4>";
  page += "<p class='card-text' id='wifiStatus'></p>";
  page += "</div></div></div>";

  // Script untuk mengirim permintaan toggleAnimation ke server
  page += "<script>function toggleAnimation() {";
  page += "var xhr = new XMLHttpRequest();";
  page += "xhr.open('GET', '/toggleAnimation', true);";
  page += "xhr.send();";
  page += "showNotification('Animation Toggled');";
  page += "}</script>";

  // Script untuk memperbarui pengaturan animasi
  page += "<script>function updateSettings() {";
  page += "var speed = document.getElementById('animationSpeed').value;";
  page += "var effect = document.getElementById('animationEffect').value;";
  page += "var xhr = new XMLHttpRequest();";
  page += "xhr.open('GET', '/setAnimationSpeed?speed=' + speed, true);";
  page += "xhr.send();";
  page += "xhr.open('GET', '/setAnimationEffect?effect=' + effect, true);";
  page += "xhr.send();";
  page += "showNotification('Settings Updated');";
  page += "}</script>";

  // Script untuk mengirim permintaan toggleRelay ke server
  page += "<script>function toggleRelayD1() {";
  page += "var xhr = new XMLHttpRequest();";
  page += "xhr.open('GET', '/toggleRelayD1', true);";
  page += "xhr.send();";
  page += "showNotification('Relay 1 Toggled');";
  page += "}</script>";

  page += "<script>function toggleRelayD2() {";
  page += "var xhr = new XMLHttpRequest();";
  page += "xhr.open('GET', '/toggleRelayD2', true);";
  page += "xhr.send();";
  page += "showNotification('Relay 2 Toggled');";
  page += "}</script>";

  page += "<script>function toggleRelayD3() {";
  page += "var xhr = new XMLHttpRequest();";
  page += "xhr.open('GET', '/toggleRelayD3', true);";
  page += "xhr.send();";
  page += "showNotification('Relay 3 Toggled');";
  page += "}</script>";

  page += "<script>function toggleRelayD4() {";
  page += "var xhr = new XMLHttpRequest();";
  page += "xhr.open('GET', '/toggleRelayD4', true);";
  page += "xhr.send();";
  page += "showNotification('Relay 4 Toggled');";
  page += "}</script>";

  // Script untuk menampilkan status WiFi secara dinamis
  page += "<script>function updateWiFiStatus() {";
  page += "var statusElement = document.getElementById('wifiStatus');";
  page += "var status = 'WiFi Status: ' + (navigator.onLine ? 'Connected' : 'Disconnected');";
  page += "statusElement.innerText = status;}</script>";

  // Script untuk menampilkan notifikasi
  page += "<script>function showNotification(message) {";
  page += "alert(message);";
  page += "}</script>";

  page += "<script>updateWiFiStatus();</script>"; // Panggil fungsi pertama kali
  page += "</body></html>";

  server.send(200, "text/html", page);
}

void handleEdit() {
  String newText = server.arg("text");
  currentText = newText;

  // Tidak perlu mereset efek atau kecepatan animasi saat mengganti teks
  // Cukup atur teks baru
  myDisplay.displayText(currentText.c_str(), PA_CENTER, animationSpeed, 0, selectedEffect, selectedEffect);

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleToggleAnimation() {
  animationState = !animationState;
  server.send(200, "text/plain", "Animation toggled");
}

void handleSetAnimationSpeed() {
  String speed = server.arg("speed");
  animationSpeed = speed.toInt();
  server.send(200, "text/plain", "Animation speed set to: " + speed);
}

void handleSetAnimationEffect() {
  String effect = server.arg("effect");
  selectedEffect = static_cast<textEffect_t>(effect.toInt());
  server.send(200, "text/plain", "Animation effect set to: " + effect);
}

void handleToggleRelayD1() {
  // Toggle status relay D1
  digitalWrite(RELAY_PIN_D1, !digitalRead(RELAY_PIN_D1));
  server.send(200, "text/plain", "Relay D1 toggled");
}

void handleToggleRelayD2() {
  // Toggle status relay D2
  digitalWrite(RELAY_PIN_D2, !digitalRead(RELAY_PIN_D2));
  server.send(200, "text/plain", "Relay D2 toggled");
}

void handleToggleRelayD3() {
  // Toggle status relay D3
  digitalWrite(RELAY_PIN_D3, !digitalRead(RELAY_PIN_D3));
  server.send(200, "text/plain", "Relay D3 toggled");
}

void handleToggleRelayD4() {
  // Toggle status relay D4
  digitalWrite(RELAY_PIN_D4, !digitalRead(RELAY_PIN_D4));
  server.send(200, "text/plain", "Relay D4 toggled");
}
