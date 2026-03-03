# 📡 Servidor HTTP Modular para ESP32

Este repositorio contiene la implementación base del firmware para el **Dispositivo Modular STRIDE**, un **PROYECTO DE TRABAJO DE FIN DE GRADO (TFG)**.

El objetivo principal es establecer una **arquitectura de software flexible** utilizando **ESP-IDF** para el microcontrolador ESP32, resolviendo la **falta de versatilidad** de los dispositivos IoT comerciales. Este núcleo central actúa como el "cerebro" del sistema, integrando conectividad **WiFi, una API y una pantalla**, permitiendo la **fácil reconfiguración** mediante módulos intercambiables los cuales pueden ser programados con facilidad gracias a su propio DSL desarrollado.

## ⚙️ Características Técnicas y Funcionalidades

El proyecto implementa las siguientes funcionalidades iniciales:

- **Arquitectura:** Estructura de código pensada para ser **escalable y fácilmente mantenible**, con capas lógicas separadas (WiFi, Servidor, Controladores de Hardware).
- **Conectividad Híbrida:** Inicialización de la interfaz WiFi en **modo AP (Access Point) + STA (Station)**.
- **Servidor HTTP Ligero:** Servidor implementado para la interacción remota y la obtención de datos.
- **Control de Hardware:** Controladores separados para el hardware.
