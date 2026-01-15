#ifndef CONFIG_H
#define CONFIG_H

// ---------- DEVICE ----------
#define DEVICE_ID        "0001"
#define SENSOR_ID        "sensor_peso_01"

// ---------- WIFI ----------
#define WIFI_TIMEOUT_MS  15000

// ---------- MQTT ----------
#define MQTT_SERVER      "broker.hivemq.com"
#define MQTT_PORT        1883
#define MQTT_BASE_TOPIC  "gastracer"

// Topic final:
// gastracer/{device_id}/{sensor_id}/peso

// ---------- HX711 ----------
#define HX_DT            14   // GPIO14 (D5)
#define HX_SCK           12   // GPIO12 (D6)
#define CALIBRATION_FACTOR  -21.983999
#define MEDICION_MUESTRAS   5
#define MEDICION_INTERVALO_MS 2000

// ---------- OLED ----------
#define SCREEN_WIDTH     128
#define SCREEN_HEIGHT    32
#define OLED_RESET       -1
#define OLED_I2C_ADDR    0x3C
#define OLED_SDA         4
#define OLED_SCL         5

#endif
