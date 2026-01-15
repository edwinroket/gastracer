# -*- coding: utf-8 -*-
import paho.mqtt.client as mqtt
import mysql.connector
import time
import logging
from datetime import datetime

# ===============================
# CONFIGURACION
# ===============================

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_BASE_TOPIC = "gastracer/#"

DB_CONFIG = {
    'user': 'gastracer',
    'password': 'e123456789',
    'host': '127.0.0.1',   # IMPORTANTE: localhost por tunel SSH
    'database': 'gastracer',
    'port': 3306
}

# ===============================
# LOGGING
# ===============================
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s"
)

conn = None

# ===============================
# DB
# ===============================
def obtener_conexion():
    global conn
    try:
        if conn is None or not conn.is_connected():
            conn = mysql.connector.connect(**DB_CONFIG)
            logging.info("Conectado a MariaDB")
        return conn
    except mysql.connector.Error as e:
        logging.error(f"Error DB: {e}")
        conn = None
        return None


def obtener_id_sensor(device_id, sensor_code):
    """
    Mapea IDs logicos (MQTT) a IDs reales de BD
    """
    conn = obtener_conexion()
    if not conn:
        return None

    cursor = conn.cursor()
    query = """
        SELECT s.id_sensor
        FROM sensor s
        JOIN dispositivo d ON d.id_dispositivo = s.id_dispositivo
        WHERE d.id_dispositivo = %s
          AND s.tipo_sensor = %s
    """
    cursor.execute(query, (device_id, sensor_code))
    row = cursor.fetchone()
    cursor.close()
    return row[0] if row else None


def insertar_medicion(id_sensor, valor):
    conn = obtener_conexion()
    if not conn:
        return

    cursor = conn.cursor()
    query = """
        INSERT INTO medicion (id_sensor, valor, fecha_hora)
        VALUES (%s, %s, NOW())
    """
    cursor.execute(query, (id_sensor, valor))
    conn.commit()
    cursor.close()

    logging.info(f"Insertado sensor={id_sensor} valor={valor}kg")


# ===============================
# MQTT
# ===============================
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        logging.info("Conectado a MQTT")
        client.subscribe(MQTT_BASE_TOPIC)
    else:
        logging.error(f"Error MQTT rc={rc}")


def on_message(client, userdata, msg):
    try:
        topic_parts = msg.topic.split("/")
        if len(topic_parts) != 4:
            return

        _, device_id, sensor_code, medida = topic_parts

        if medida != "peso":
            return

        valor = float(msg.payload.decode().strip())

        id_sensor = obtener_id_sensor(device_id, sensor_code)
        if not id_sensor:
            logging.warning(f"Sensor no registrado: {device_id}/{sensor_code}")
            return

        insertar_medicion(id_sensor, valor)

    except Exception as e:
        logging.error(f"Error procesando mensaje: {e}")


def iniciar():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    while True:
        try:
            client.connect(MQTT_BROKER, MQTT_PORT, 60)
            client.loop_forever()
        except Exception as e:
            logging.warning(f"MQTT error: {e}, reintentando...")
            time.sleep(5)


if __name__ == "__main__":
    obtener_conexion()
    iniciar()
