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

IPAddress local_IP(192, 168, 0, 251);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

WebServer server(80);

const int threshold = 20;

const int enablePin2 = 27; // Pin GPIO27
const int R_I_EN = 26;
const int enablePin = 14;

const int pwmChannel = 0;
const int pwmChannel2 = 1;
const int pwmFreq = 5000;
const int pwmResolution = 8;

int velocidad = 0;

// Pin para el encoder infrarrojo
const int encoderPin = 13;
volatile unsigned long revolutions = 0;
volatile unsigned long lastEncoderTime = 0;
const unsigned long debounceTime = 10; // 10 ms de debounce
bool state_led = true;

// Pines para los sensores ultrasónicos
const int trigPin1 = 32, echoPin1 = 33; // Mantiene los pines originales
const int trigPin2 = 15, echoPin2 = 22; // Nuevo sensor
const int trigPin3 = 4, echoPin3 = 5;   // Nuevo sensor
const int trigPin4 = 18, echoPin4 = 19; // Nuevo sensor

void IRAM_ATTR encoderISR()
{
    unsigned long currentTime = millis();
    if (currentTime - lastEncoderTime > debounceTime)
    {
        revolutions++;
        lastEncoderTime = currentTime;
        if (velocidad >= 25)
        {
            state_led = !state_led;
        }
        else
        {
            state_led = 0;
        }
        digitalWrite(2, state_led);
    }
}

// ... (keep the existing setup_wifi, handleRoot, handleSetSpeed, and handleGetRevolutions functions)

void setup_wifi()
{
    delay(10);
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
    {
        Serial.println("STA Failed to configure");
    }

    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Conectado a red WiFi!");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void handleRoot()
{
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

void handleSetSpeed()
{
    if (server.hasArg("speed"))
    {
        velocidad = server.arg("speed").toInt();
        velocidad = constrain(velocidad, 0, 255);

        if (velocidad < 1)
        {
            ledcWrite(pwmChannel, 0);
            delayMicroseconds(100);
            ledcWrite(pwmChannel2, 0);
            digitalWrite(R_I_EN, LOW);
        }
        else
        {
            ledcWrite(pwmChannel, velocidad);
            delayMicroseconds(100);
            ledcWrite(pwmChannel2, 0);
            digitalWrite(R_I_EN, HIGH);
        }

        server.send(200, "text/plain", "OK");
    }
    else
    {
        server.send(400, "text/plain", "Falta el parámetro de velocidad");
    }
}

void handleGetRevolutions()
{
    server.send(200, "text/plain", String(revolutions));
}

void setup()
{
    Serial.begin(115200);

    pinMode(R_I_EN, OUTPUT);
    pinMode(enablePin2, OUTPUT);
    pinMode(enablePin, OUTPUT);
    pinMode(2, OUTPUT); // LED pin
    pinMode(encoderPin, INPUT);

    // Configuración de los pines de los sensores ultrasónicos
    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    pinMode(trigPin3, OUTPUT);
    pinMode(echoPin3, INPUT);
    pinMode(trigPin4, OUTPUT);
    pinMode(echoPin4, INPUT);

    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(enablePin, pwmChannel);

    ledcSetup(pwmChannel2, pwmFreq, pwmResolution);
    ledcAttachPin(enablePin2, pwmChannel2);

    attachInterrupt(digitalPinToInterrupt(encoderPin), encoderISR, RISING);

    setup_wifi();

    server.on("/", HTTP_GET, handleRoot);
    server.on("/setSpeed", HTTP_POST, handleSetSpeed);
    server.on("/getRevolutions", HTTP_GET, handleGetRevolutions);

    server.begin();
    digitalWrite(2, 0);
}

float getDistance(int trigPin, int echoPin)
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Tiempo máximo de espera (en microsegundos) para recibir el eco.
    long duration = pulseIn(echoPin, HIGH, 10000); // 30,000 µs = 30 ms

    // Si no se recibe eco (duration = 0), retornamos un valor alto para indicar un fallo.
    if (duration == 0)
    {
        return 9999.9; // Valor grande que indica que no se recibió eco
    }

    // Calculamos la distancia en cm
    float distance = duration * 0.034 / 2;
    return distance;
}

void loop()
{
    server.handleClient();

    float distance1 = getDistance(trigPin1, echoPin1);
    float distance2 = getDistance(trigPin2, echoPin2);
    float distance3 = getDistance(trigPin3, echoPin3);
    float distance4 = getDistance(trigPin4, echoPin4);

    // Check if any sensor detects an object within 20 cm
    if (distance1 <= threshold || distance2 <= threshold || distance3 <= threshold || distance4 <= threshold)
    {
        // Apagar el motor si se detecta un objeto a 20 cm o menos en cualquier sensor
        digitalWrite(R_I_EN, LOW);
        ledcWrite(pwmChannel, 0);
        delayMicroseconds(10);
        ledcWrite(pwmChannel2, 0);
        digitalWrite(2, 0);
    }
    else
    {
        // Mantener la velocidad actual si no hay objetos cercanos
        digitalWrite(R_I_EN, HIGH);
        ledcWrite(pwmChannel, velocidad);
        delayMicroseconds(10);
        ledcWrite(pwmChannel2, 0);
    }
    if (velocidad < 25)
        digitalWrite(2, 0);
    delay(10); // Pequeña pausa para no saturar los sensores
}