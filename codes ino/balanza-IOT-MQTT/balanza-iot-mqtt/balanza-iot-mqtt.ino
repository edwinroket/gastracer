// ====================== INCLUDES ======================
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>

// ====================== OLED ======================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET   -1
#define I2C_ADDR     0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ====================== HX711 ======================
#define HX_DT   14  // D5
#define HX_SCK  12  // D6
HX711 scale;

float CALIBRATION_FACTOR = 9.121500;

// ====================== WIFI (DHCP) ======================
const char* ssid = "localhost";
const char* password = "123456789";

// ====================== MQTT ======================
WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "test.mosquitto.org";
const int   mqtt_port   = 1883;

const char topicPeso[]  = "/gastraser/Peso/Edwin";
const char topicFecha[] = "/gastraser/Fecha/Edwin";

String clientId = "BalanzaEdwin-" + String(random(0xffff), HEX);

// ====================== NTP ======================
#define MY_NTP_SERVER "pool.ntp.org"
#define MY_TZ "<-04>4<-03>,M9.1.6/24,M4.1.6/24"

time_t now;
tm tmstruct;

// ====================== OLED ======================
void showMsg(const String& l1, const String& l2="") {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(l1);
  display.setCursor(0,16);
  display.println(l2);
  display.display();
}

// ====================== MQTT ======================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect(clientId.c_str())) {
      Serial.println("OK");
    } else {
      Serial.print("Fallo rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ====================== SETUP ======================
void setup() {
  Serial.begin(115200);

  // I2C manual: SDA=4, SCL=5
  Wire.begin(4, 5);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR)) {
    Serial.println("Error OLED");
  } else {
    showMsg("Balanza IoT", "Iniciando...");
  }

  // HX711
  scale.begin(HX_DT, HX_SCK);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare();

  // --- WIFI DHCP ---
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 30) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nError WiFi");
  }

  // --- NTP ---
  configTime(MY_TZ, MY_NTP_SERVER);

  // --- MQTT ---
  client.setServer(mqtt_server, mqtt_port);

  showMsg("Listo", "Esperando peso...");
}

// ====================== LOOP ======================
void loop() {

  // WiFi reconexión simple
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    return;
  }

  // MQTT reconexión
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Fecha/hora actual
  time(&now);
  localtime_r(&now, &tmstruct);

  char fecha[25];
  sprintf(fecha, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d",
          tmstruct.tm_mday, tmstruct.tm_mon + 1, tmstruct.tm_year + 1900,
          tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);

  // Leer balanza
  if (scale.is_ready()) {
    float w = scale.get_units(1);
    float wkg = w / 1000.0;

    // OLED
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Peso actual:");
    display.setTextSize(2);
    display.setCursor(10,14);
    display.print(wkg, 2);
    display.print(" kg");
    display.display();

    // Serial
    Serial.printf("Peso: %.2f kg | %s\n", wkg, fecha);

    // Publicar MQTT
    client.publish(topicPeso, String(wkg, 2).c_str());
    delay(150);
    client.publish(topicFecha, fecha);
  }

  delay(1000);
}
