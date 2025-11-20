# 📡 Servidor HTTP Modular para ESP32

Este repositorio contiene la implementación base del firmware para el **Dispositivo Modular STRIDE**, un **PROYECTO DE TRABAJO DE FIN DE GRADO (TFG)**.

El objetivo principal es establecer una **arquitectura de software flexible** utilizando **ESP-IDF** para el microcontrolador ESP32, resolviendo la **falta de versatilidad** de los dispositivos IoT comerciales. Este núcleo central actúa como el "cerebro" del sistema, integrando conectividad **WiFi, una API y una pantalla**, permitiendo la **fácil reconfiguración** mediante módulos intercambiables.

## ⚙️ Características Técnicas y Funcionalidades

El proyecto implementa las siguientes funcionalidades iniciales:

- **Arquitectura Modular:** Estructura de código pensada para ser **escalable y fácilmente mantenible**, con capas lógicas separadas (WiFi, Servidor, Controladores de Hardware).
- **Conectividad Híbrida:** Inicialización de la interfaz WiFi en **modo AP (Access Point) + STA (Station)**.
- **Servidor HTTP Ligero:** Servidor implementado para la interacción remota y la obtención de datos.
- **Control de Hardware (Drivers):** Controladores separados para el hardware, como el **LED**.

## 📋 API Endpoints Actuales

El servidor HTTP expone las siguientes rutas:

| Método     | Ruta          | Descripción                                                                                                                                                             | Datos de Salida |
| :--------- | :------------ | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :-------------- |
| `GET`      | `/`           | Página HTML de bienvenida.                                                                                                                                              | HTML            |
| `GET`      | `/api/status` | Proporciona **información crítica del ESP32** en formato JSON, como la cantidad de **cores**, el **modelo**, la **frecuencia de la CPU** y el **estado de la memoria**. | JSON            |
| `GET/PUTS` | `/api/led`    | Permite **alternar el estado (Toggle)** del LED conectado en el **pin GPIO 26**.                                                                                        | Estado del LED  |

## 📂 Estructura del Proyecto (Árbol de Directorios)

El código se organiza en módulos para facilitar la escalabilidad futura del TFG:

/main
│── main.c
│── CMakeLists.txt

├── config/
│ ├── credentials.h
│ └── routes.h

├── wifi/
│ ├── wifi.c
│ └── wifi.h

├── server/
│ ├── http_server.c
│ ├── http_server.h
│ └── handlers/
│ ├── handler_root.c
│ ├── handler_status.c
│ └── handler_led.c

└── drivers/
├── led_driver.c
└── led_driver.h

## 🚀 Cómo Compilar y Flashear

Este proyecto requiere el entorno de desarrollo **ESP-IDF** (Espressif IoT Development Framework).

Ejecuta los siguientes comandos desde la raíz del proyecto para configurar, compilar y flashear el firmware en el ESP32:

```bash
# 1. Establece el objetivo de compilación (ej. esp32-s3 si se usa ese chip)
idf.py set-target esp32

# 2. Compila el proyecto
idf.py build

# 3. Flashea el firmware y abre el monitor serial
# Reemplaza 'PORT' con el puerto serial de tu ESP32 y '[N]' con el baud rate (opcional)
idf.py -p PORT -b [N] flash monitor
```

## 📈 Desarrollo Futuro (Contexto TFG)

La arquitectura modular implementada sienta las bases para las siguientes fases del Trabajo de Fin de Grado:

1. Integración de Módulos: Ampliación del módulo drivers/ para controlar dispositivos complejos, incluyendo la integración de sensores como el BME280 (para temperatura, presión y humedad) o el sensor de luz BH1750.
2. Visualización: Implementación de la pantalla (posiblemente usando un driver ILI9341) para la gestión de datos y la interfaz de usuario.
3. Almacenamiento Local: Integración de la ranura SD utilizando un breakout board de tarjeta Micro SD para el registro de datos (datalogging).
4. Casos de Uso de Validación: Programación de la lógica de aplicación para escenarios de cultivos hidropónicos (sensores de pH, conductividad, temperatura) y aplicaciones de senderismo (geolocalización, altímetro, brújula digital o sensores de condiciones ambientales).
