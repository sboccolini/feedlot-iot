# Sistema IoT Feed Lot Bovino

Sistema de monitoreo y control IoT para feed lot bovino desarrollado para la materia **Aplicaciones TCP/IP 2026 — UNRC — LaSEm**.

## Stack tecnológico

| Componente | Tecnología |
|---|---|
| Protocolo de campo | MQTT (Eclipse Mosquitto) |
| Lógica de control | Node-RED |
| Interfaz web | Home Assistant |
| Base de datos | InfluxDB 2.0 |
| Infraestructura | Docker Compose |
| Nodos de campo | ESP32 / ESP8266 |

## Arquitectura

```
┌─────────────────────────────────────────┐
│  CAPA PRESENTACIÓN                      │
│  Home Assistant :8123                   │
└──────────────────┬──────────────────────┘
                   │ MQTT / HTTP
┌──────────────────▼──────────────────────┐
│  CAPA LÓGICA Y ALMACENAMIENTO           │
│  Mosquitto :1883 · Node-RED :1880       │
│  InfluxDB :8086                         │
└──────────────────┬──────────────────────┘
                   │ MQTT (TCP/IP)
┌──────────────────▼──────────────────────┐
│  CAPA CAMPO                             │
│  ESP32/ESP8266 — Electrificador         │
│  ESP32/ESP8266 — Tanque de agua         │
│  ESP32/ESP8266 — Ambiental/Bebederos    │
│  ESP32/ESP8266 — Iluminación manga      │
└─────────────────────────────────────────┘
```

## Subsistemas

- **Electrificador:** detección de corte de red 220V, conmutación automática a batería 12V, recarga automática al volver la red
- **Tanque de agua:** control de nivel con histéresis (bomba ON ≤20%, OFF ≥95%)
- **Ambiental/Bebederos:** temperatura, humedad y alertas de bebederos vacíos por corral
- **Iluminación manga:** control manual y programación horaria desde Home Assistant

## Árbol de tópicos MQTT

```
feedlot/
├── energia/
│   ├── red_220v          ← "OK" | "CORTE"
│   ├── bateria_v         ← "12.45"
│   ├── rele_bateria/set  ← "ON" | "OFF"
│   └── rele_cargador/set ← "ON" | "OFF"
├── tanque/
│   ├── nivel             ← "73" (%)
│   └── bomba/set         ← "ON" | "OFF"
├── ambiental/
│   ├── temperatura       ← "28.4" (°C)
│   └── humedad           ← "65.2" (%)
├── bebederos/
│   ├── corral_1          ← "OK" | "VACIO"
│   ├── corral_2          ← "OK" | "VACIO"
│   └── corral_3          ← "OK" | "VACIO"
└── iluminacion/manga/set ← "ON" | "OFF"
```

## Instalación y despliegue

### Requisitos
- Ubuntu 22.04
- Docker + Docker Compose

### Pasos

```bash
# 1. Clonar el repositorio
git clone https://github.com/sboccolini/feedlot-iot.git
cd feedlot-iot

# 2. Crear directorios de datos
mkdir -p mosquitto/data nodered_data ha_config influxdb_data

# 3. Levantar el stack
docker compose up -d

# 4. Verificar
docker compose ps
```

### Accesos

| Servicio | URL | Credenciales |
|---|---|---|
| Home Assistant | `http://localhost:8123` | Configurar en primer acceso |
| Node-RED | `http://localhost:1880` | Sin autenticación |
| InfluxDB | `http://localhost:8086` | admin / feedlot2026 |
| Mosquitto | `tcp://localhost:1883` | Anónimo |

## Importar flows de Node-RED

1. Abrir `http://localhost:1880`
2. Menú ☰ → Import → seleccionar `flows_feedlot.json`
3. Deploy

## Configuración de los ESP

En cada firmware reemplazar:

```cpp
const char* ssid        = "TU_WIFI";
const char* password    = "TU_PASSWORD";
const char* mqtt_server = "IP_DE_TU_PC";
```

## Estructura del repositorio

```
feedlot-iot/
├── docker-compose.yml
├── flows_feedlot.json
├── mosquitto/
│   └── config/
│       └── mosquitto.conf
├── ha_config/
│   └── configuration.yaml
└── firmware/
    ├── nodo_energia.ino
    ├── nodo_tanque.ino
    ├── nodo_ambiental.ino
    └── nodo_iluminacion.ino
```

## Materia

Aplicaciones TCP/IP 2026 · UNRC · LaSEm · Tema 3
