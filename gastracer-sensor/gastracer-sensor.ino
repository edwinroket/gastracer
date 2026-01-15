#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config.h"
#include "secrets.h"

// ---------- OLED ----------
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- HX711 ----------
HX711 scale;

// ---------- MQTT ----------
WiFiClient espClient;
PubSubClient client(espClient);

// ---------- Helpers ----------
String mqttTopic() {
  return String(MQTT_BASE_TOPIC) + "/" +
         DEVICE_ID + "/" +
         SENSOR_ID + "/peso";
}

void showMsg(const String& l1, const String& l2 = "") {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(l1);
  display.setCursor(0, 16);
  display.println(l2);
  display.display();
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  showMsg("Conectando WiFi", WIFI_SSID);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    if (millis() - start > WIFI_TIMEOUT_MS) {
      showMsg("WiFi error", "Timeout");
      ESP.restart();
    }
  }

  showMsg("WiFi OK", WiFi.localIP().toString());
}

void reconnectMQTT() {
  while (!client.connected()) {
    String clientId = "ESP-" + String(DEVICE_ID);
    if (client.connect(clientId.c_str())) {
      showMsg("MQTT conectado");
    } else {
      delay(3000);
    }
  }
}

float readWeightKg() {
  float raw = scale.get_units(MEDICION_MUESTRAS);
  return raw / 1000.0;
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);

  Wire.begin(OLED_SDA, OLED_SCL);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR);
  showMsg("Gastracer", "Iniciando...");

  connectWiFi();

  client.setServer(MQTT_SERVER, MQTT_PORT);

  scale.begin(HX_DT, HX_SCK);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare();

  if (!scale.wait_ready_timeout(2000)) {
    showMsg("HX711 error", "Sin señal");
    ESP.restart();
  }

  showMsg("Sistema listo");
}

// ---------- LOOP ----------
void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  float weightKg = readWeightKg();

  Serial.print("Peso: ");
  Serial.print(weightKg, 2);
  Serial.println(" kg");

  char payload[16];
  dtostrf(weightKg, 1, 2, payload);
  client.publish(mqttTopic().c_str(), payload);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Peso actual");
  display.setTextSize(2);
  display.setCursor(6, 14);
  display.print(weightKg, 2);
  display.print(" kg");
  display.display();

  delay(MEDICION_INTERVALO_MS);
}
