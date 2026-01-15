CREATE DATABASE IF NOT EXISTS gastracer
CHARACTER SET utf8mb4
COLLATE utf8mb4_unicode_ci;

USE gastracer;

CREATE TABLE estado_usuario (
    id_estado_usuario INT AUTO_INCREMENT PRIMARY KEY,
    nombre VARCHAR(50) NOT NULL,
    descripcion VARCHAR(255)
) ENGINE=InnoDB;

CREATE TABLE rol (
    id_rol INT AUTO_INCREMENT PRIMARY KEY,
    nombre VARCHAR(50) NOT NULL,
    descripcion VARCHAR(255)
) ENGINE=InnoDB;

CREATE TABLE tipo_alerta (
    id_tipo_alerta INT AUTO_INCREMENT PRIMARY KEY,
    nombre VARCHAR(50) NOT NULL,
    descripcion VARCHAR(255)
) ENGINE=InnoDB;

CREATE TABLE usuario (
    id_usuario INT AUTO_INCREMENT PRIMARY KEY,
    nombre VARCHAR(100) NOT NULL,
    email VARCHAR(100) NOT NULL UNIQUE,
    telefono VARCHAR(30),
    rut VARCHAR(20),
    direccion VARCHAR(255),
    id_estado_usuario INT NOT NULL,
    CONSTRAINT fk_usuario_estado
        FOREIGN KEY (id_estado_usuario)
        REFERENCES estado_usuario(id_estado_usuario)
) ENGINE=InnoDB;

CREATE TABLE bombona (
    id_bombona INT AUTO_INCREMENT PRIMARY KEY,
    capacidad_kg DECIMAL(5,2) NOT NULL,
    tipo_gas VARCHAR(50) NOT NULL
) ENGINE=InnoDB;

CREATE TABLE dispositivo (
    id_dispositivo INT AUTO_INCREMENT PRIMARY KEY,
    id_usuario INT NOT NULL,
    id_bombona INT NOT NULL,
    ubicacion VARCHAR(100),
    estado_dispositivo VARCHAR(30),
    fecha_instalacion DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_dispositivo_usuario
        FOREIGN KEY (id_usuario)
        REFERENCES usuario(id_usuario),
    CONSTRAINT fk_dispositivo_bombona
        FOREIGN KEY (id_bombona)
        REFERENCES bombona(id_bombona)
) ENGINE=InnoDB;

CREATE TABLE sensor (
    id_sensor INT AUTO_INCREMENT PRIMARY KEY,
    id_dispositivo INT NOT NULL,
    tipo_sensor VARCHAR(50) NOT NULL,
    unidad VARCHAR(20) NOT NULL,
    estado_sensor VARCHAR(30),
    CONSTRAINT fk_sensor_dispositivo
        FOREIGN KEY (id_dispositivo)
        REFERENCES dispositivo(id_dispositivo)
) ENGINE=InnoDB;

CREATE TABLE medicion (
    id_medicion INT AUTO_INCREMENT PRIMARY KEY,
    id_sensor INT NOT NULL,
    valor DECIMAL(10,3) NOT NULL,
    fecha_hora DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_medicion_sensor
        FOREIGN KEY (id_sensor)
        REFERENCES sensor(id_sensor)
) ENGINE=InnoDB;

CREATE TABLE alerta (
    id_alerta INT AUTO_INCREMENT PRIMARY KEY,
    id_dispositivo INT NOT NULL,
    id_tipo_alerta INT NOT NULL,
    id_medicion INT NULL,
    fecha_hora DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    estado_alerta VARCHAR(30),
    CONSTRAINT fk_alerta_dispositivo
        FOREIGN KEY (id_dispositivo)
        REFERENCES dispositivo(id_dispositivo),
    CONSTRAINT fk_alerta_tipo
        FOREIGN KEY (id_tipo_alerta)
        REFERENCES tipo_alerta(id_tipo_alerta),
    CONSTRAINT fk_alerta_medicion
        FOREIGN KEY (id_medicion)
        REFERENCES medicion(id_medicion)
) ENGINE=InnoDB;

CREATE TABLE config_alerta (
    id_config_alerta INT AUTO_INCREMENT PRIMARY KEY,
    id_usuario INT NOT NULL,
    id_dispositivo INT NOT NULL,
    umbral_kg DECIMAL(5,2) NOT NULL,
    activo BOOLEAN NOT NULL DEFAULT TRUE,
    CONSTRAINT fk_config_usuario
        FOREIGN KEY (id_usuario)
        REFERENCES usuario(id_usuario),
    CONSTRAINT fk_config_dispositivo
        FOREIGN KEY (id_dispositivo)
        REFERENCES dispositivo(id_dispositivo)
) ENGINE=InnoDB;

