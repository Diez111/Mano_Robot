# Mano_Robot

Proyecto de mano robótica controlada por ESP32 que integra servomotores y una interfaz web para el control manual y mediante detección de gestos.

## Inspiración y Modificaciones

El credito del modelo original para los modelos stl que modifique es de: [Hollies Hand v5](https://cults3d.com/es/modelo-3d/artilugios/hollies-hand-v5) disponible en Cults3D.

## Lista de Materiales

- **Arsumo Nodemcu Esp32 Wifi + Bluetooth 5v Wroom**  
  Microcontrolador (cerebro del proyecto)

- **Shield ESP32 de 30 pines**  
  Para montar la ESP32 y conectar componentes externos

- **5 Servomotores SG90**  
  Para el movimiento de las articulaciones de la mano

- **Hilo de pescar de 0.5 mm**  
  Simula los tendones de la mano robótica

- **7 Tornillos 3.5 x 12.7 mm**  
  Para fijar y ajustar las partes del modelo

- **Adaptador AC/DC 5V de 1A**  
  Alimentación para los servomotores

- **Conector hembra para adaptador**  
  Para la conexión del adaptador

- **2 Jumper macho-hembra de 10 cm**  
  Para la conexión de alimentación y señales

- **Lija fina**  
  Para limpiar y dar acabado a la impresión 3D

- **Herramientas necesarias:**  
  Destornillador estrella, lima manual y cuchilla de punta fina

## Diagrama de Conexión / Hardware

El código asigna cada servomotor a un pin específico del ESP32:

- **Pulgar:** Pin 27  
- **Índice:** Pin 26  
- **Medio:** Pin 25  
- **Anular:** Pin 33  
- **Meñique:** Pin 32  

Asegúrate de conectar correctamente los servos al ESP32 según esta configuración para lograr un movimiento coordinado.

## Instalación y Configuración

Sigue estos pasos para poner en marcha el proyecto:

1. **Clonar el repositorio:**

   ```bash
   [git clone https://github.com/Diez111/Mano_Robot.git](https://github.com/Diez111/Mano_Robot.git)
   ```

2. **Abrir el proyecto en el IDE de Arduino:**

   Importa el código en tu entorno de desarrollo y asegúrate de tener instaladas las librerías necesarias:
   
   - [ESP32Servo](https://github.com/jkb-git/ESP32Servo)
   - [WiFi](https://www.arduino.cc/en/Reference/WiFi)
   - [WebServer](https://www.arduino.cc/en/Reference/WebServer)

3. **Configurar la red WiFi:**

   En el código, actualiza los valores de `ssid` y `password` con los datos de tu red.

4. **Cargar el código al ESP32:**

   Conecta tu ESP32 y sube el programa utilizando el IDE de Arduino.

## Funcionamiento y Uso

El sistema se inicia conectándose a la red WiFi y arrancando un servidor web en el puerto 80. La página web permite:

- **Control manual individual:**  
  Botones para abrir y cerrar cada dedo de la mano (pulgar, índice, medio, anular y meñique).

- **Gestos predefinidos:**  
  Ejecución de gestos como "Dedo Medio", "Like" y "Rock and Roll".

- **Detección de gestos:**  
  Uso de la cámara y la librería Mediapipe Hands para detectar la posición de los dedos y controlar la mano automáticamente.

El código organiza la funcionalidad en secciones:
- **Inicialización y conexión WiFi:** Configura el ESP32 y establece la conexión a Internet.
- **Control de servomotores:** Funciones para mover cada dedo a posiciones predefinidas.
- **Servidor web e interfaz:** Implementa una interfaz visual para el control a través del navegador.
- **Integración con detección de gestos:** Recibe datos de la cámara para interpretar y enviar comandos de movimiento.

## Contribuciones

Se agradecen las contribuciones para mejorar el proyecto. Si deseas colaborar o tienes sugerencias, por favor abre un *issue* o realiza un *pull request* en el repositorio.

## Enlaces Útiles

- **Repositorio del proyecto:** [https://github.com/Diez111/Mano_Robot.git](https://github.com/Diez111/Mano_Robot.git)
- **Modelo 3D original:** [Hollies Hand v5 en Cults3D](https://cults3d.com/es/modelo-3d/artilugios/hollies-hand-v5)
