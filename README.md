# Gastracer

Sistema IoT para monitoreo del consumo de gas mediante sensores de peso, orientado a la detección temprana de agotamiento de bombonas y generación de alertas.

El proyecto integra hardware, MQTT, procesamiento backend y base de datos relacional, con un diseño pensado para escalar a múltiples dispositivos y sensores.

---

## 🧠 Idea central

Medir el peso de una bombona de gas en tiempo real usando un sensor **HX711** conectado a un **ESP8266**.  
El peso se envía vía **MQTT**, se persiste en **MariaDB** y sirve como base para:

- Seguimiento histórico  
- Detección de umbrales  
- Generación de alertas  

---

## 🏗 Arquitectura general

[ HX711 + ESP8266 ]
|
| MQTT (peso en kg)
v
[ Broker MQTT (HiveMQ) ]
|
v
[ Servicio Python ]
|
v
[ MariaDB ]




---

## 📡 Flujo de datos

### ESP8266
- Lee el peso desde el HX711  
- Publica el valor en MQTT  

### Backend Python
- Se suscribe a `gastracer/#`
- Valida el tópico
- Mapea `device_id` + `sensor_id` contra la base de datos
- Inserta la medición en la tabla `medicion`

### Base de datos
- Almacena usuarios, dispositivos, sensores y mediciones
- Permite construir alertas y dashboards

---

## 📑 Formato de tópicos MQTT
gastracer/{device_id}/{sensor_id}/peso

### Ejemplo

gastracer/0001/sensor_peso_01/peso

**Payload**
12.45


*(peso en kilogramos)*

---

## 🗄 Modelo de datos (resumen)

### Entidades principales

**usuario**
- Datos del propietario o responsable
- Asociado a un estado y un rol

**rol**
- Define permisos lógicos (admin, usuario, técnico)

**estado_usuario**
- Activo, suspendido, etc.

**dispositivo**
- Representa una balanza física instalada
- Pertenece a un usuario y a una bombona

**bombona**
- Define capacidad y tipo de gas

**sensor**
- Sensores físicos asociados a un dispositivo

**medicion**
- Registros históricos de peso
- Base para alertas y análisis

---

## ⚙️ Stack tecnológico

### Hardware
- ESP8266
- HX711
- Celda de carga
- OLED SSD1306 *(opcional)*

### Software
- Arduino / PlatformIO
- MQTT (HiveMQ)
- Python 3
- MariaDB
- Git + GitHub

---

## 📦 Estructura del repositorio

gastracer/
│
├── gastracer-sensor/ # Código Arduino (ESP8266 + HX711)
│
├── scrips/
│ └── mqtt_to_db_gastracer.py # Servicio MQTT → MariaDB
│
├── sql/
│ └── schema.sql # Esquema de base de datos
│
├── requirements.txt # Dependencias Python
└── README.md


