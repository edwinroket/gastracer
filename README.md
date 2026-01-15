# Gastracer

Sistema IoT para monitoreo de consumo de gas mediante sensores de peso (HX711 + ESP8266).

## Arquitectura
- ESP8266 publica peso vía MQTT
- Servicio Python consume tópicos `gastracer/#`
- Inserción de mediciones en MariaDB
- Sistema preparado para alertas de nivel de gas

## Stack
- ESP8266 / Arduino
- MQTT (HiveMQ)
- Python 3
- MariaDB

## Instalación
```bash
pip install -r requirements.txt
