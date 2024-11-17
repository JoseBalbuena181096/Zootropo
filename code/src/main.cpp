#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <ESP32Servo.h>
#include <WebServer.h>

const char *ssid = "zootropo";
const char *password = "12345678";

IPAddress local_IP(192, 168, 0, 252);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

WebServer server(80);

const int enablePin2 = 27;
const int R_I_EN = 26;
const int enablePin = 14;

const int pwmChannel = 0;
const int pwmChannel2 = 1;
const int pwmFreq = 5000;
const int pwmResolution = 8;

int velocidad = 70;
TaskHandle_t LedTaskHandle = NULL;
TaskHandle_t WifiTaskHandle = NULL;

// Tarea para controlar el LED en el núcleo 1
void LedControlTask(void * parameter) {
    pinMode(2, OUTPUT);
    while (1) {
        digitalWrite(2, HIGH);
        vTaskDelay(pdMS_TO_TICKS(2));  // Encendido por 15ms
        digitalWrite(2, LOW);
        vTaskDelay(pdMS_TO_TICKS(109));  // Apagado por 25ms
    }
}

void setup_wifi() {
    delay(10);
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("STA Failed to configure");
    }

    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Conectado a red WiFi!");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void handleRoot() {
    String html = "<html><head>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; text-align: center; }";
    html += "h1 { color: #333; }";
    html += "input[type='range'] { width: 80%; height: 25px; }";
    html += "</style>";
    html += "</head><body>";
    html += "<h1>Control de Motor DC</h1>";
    html += "<input type='range' min='0' max='255' value='" + String(velocidad) + "' id='speedSlider'>";
    html += "<p>Velocidad: <span id='speedValue'>" + String(velocidad) + "</span></p>";
    html += "<script>";
    html += "var slider = document.getElementById('speedSlider');";
    html += "var output = document.getElementById('speedValue');";
    html += "slider.oninput = function() {";
    html += "  output.innerHTML = this.value;";
    html += "  fetch('/setSpeed?speed=' + this.value, {method: 'POST'});";
    html += "}";
    html += "</script>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleSetSpeed() {
    if (server.hasArg("speed")) {
        velocidad = server.arg("speed").toInt();
        velocidad = constrain(velocidad, 0, 255);

        if (velocidad < 1) {
            ledcWrite(pwmChannel, 0);
            delayMicroseconds(100);
            ledcWrite(pwmChannel2, 0);
            digitalWrite(R_I_EN, LOW);
        } else {
            ledcWrite(pwmChannel, velocidad);
            delayMicroseconds(100);
            ledcWrite(pwmChannel2, 0);
            digitalWrite(R_I_EN, HIGH);
        }

        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Falta el parámetro de velocidad");
    }
}

void WifiServerTask(void * parameter) {
    setup_wifi();

    server.on("/", HTTP_GET, handleRoot);
    server.on("/setSpeed", HTTP_POST, handleSetSpeed);

    server.begin();

    while (1) {
        server.handleClient();
        vTaskDelay(10);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(R_I_EN, OUTPUT);
    pinMode(enablePin2, OUTPUT);
    pinMode(enablePin, OUTPUT);

    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(enablePin, pwmChannel);

    ledcSetup(pwmChannel2, pwmFreq, pwmResolution);
    ledcAttachPin(enablePin2, pwmChannel2);

    digitalWrite(R_I_EN, HIGH);
    ledcWrite(pwmChannel, velocidad);
    delayMicroseconds(10);
    ledcWrite(pwmChannel2, 0);

    // Crear tarea para el LED en el núcleo 1
    xTaskCreatePinnedToCore(
        LedControlTask,
        "LEDControl",
        2048,
        NULL,
        1,
        &LedTaskHandle,
        1
    );

    // Crear tarea para WiFi y servidor web en el núcleo 0
    xTaskCreatePinnedToCore(
        WifiServerTask,
        "WifiServer",
        8192,
        NULL,
        1,
        &WifiTaskHandle,
        0
    );
}

void loop() {
    // Mantener la velocidad actual del motor
    digitalWrite(R_I_EN, HIGH);
    ledcWrite(pwmChannel, velocidad);
    delayMicroseconds(10);
    ledcWrite(pwmChannel2, 0);
}