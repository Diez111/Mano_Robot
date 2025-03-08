#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

// Prototipos de funciones (para evitar errores de declaración)
void moverDedoIndividual(String dedo, String accion);
void realizarGesto(String gesto);
void controlarDedos(String data);
void abrirMano();

// Configuración de WiFi (ajusta según tu red)
const char* ssid = "Telecentro-4915";         // Cambia esto por el nombre de tu red WiFi
const char* password = "TNYKZM5DM3JM";   // Cambia esto por la contraseña de tu red WiFi

// Servidor web en el puerto 80
WebServer server(80);

// Configuración de los servos
Servo thumb;  // Servo para el pulgar
Servo ind;    // Servo para el índice
Servo middle; // Servo para el medio
Servo ring;   // Servo para el anular
Servo pinky;  // Servo para el meñique

// Pines de los servos (ajusta según tu conexión física al ESP32)
const int thumb_pin = 27;  // Pin GPIO para el servo del pulgar
const int index_pin = 26;  // Pin GPIO para el servo del índice
const int middle_pin = 25; // Pin GPIO para el servo del medio
const int ring_pin = 33;   // Pin GPIO para el servo del anular
const int pinky_pin = 32;  // Pin GPIO para el servo del meñique

// Posiciones calibradas para los servos (ajusta según el rango de tus servos)
const int thumb_open = 0;    // Posición para abrir el pulgar
const int thumb_close = 110; // Posición para cerrar el pulgar
const int index_open = 0;    // Posición para abrir el índice
const int index_close = 180; // Posición para cerrar el índice
const int middle_open = 0;   // Posición para abrir el medio
const int middle_close = 150;// Posición para cerrar el medio
const int ring_open = 180;   // Posición para abrir el anular
const int ring_close = 20;   // Posición para cerrar el anular
const int pinky_open = 180;  // Posición para abrir el meñique
const int pinky_close = 20;  // Posición para cerrar el meñique

void setup() {
  // Iniciar comunicación serial para depuración
  Serial.begin(115200);
  Serial.println("Iniciando sistema de control de mano robótica...");

  // Conectar los servos a sus respectivos pines
  thumb.attach(thumb_pin);
  ind.attach(index_pin);
  middle.attach(middle_pin);
  ring.attach(ring_pin);
  pinky.attach(pinky_pin);

  // Conectar a la red WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a la red WiFi ");
  Serial.print(ssid);
  Serial.print("...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado exitosamente!");
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());

  // Configurar las rutas del servidor web
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", paginaWeb());
  });

  server.on("/update", HTTP_POST, []() {
    if (server.hasArg("dedo") && server.hasArg("accion")) {
      // Control manual de un dedo individual
      String dedo = server.arg("dedo");
      String accion = server.arg("accion");
      moverDedoIndividual(dedo, accion);
      server.send(200, "text/plain", "OK");
    } 
    else if (server.hasArg("gesto")) {
      // Ejecución de un gesto predefinido
      String gestoNombre = server.arg("gesto");
      realizarGesto(gestoNombre);
      server.send(200, "text/plain", "OK");
    } 
    else if (server.hasArg("fingers")) {
      // Control automático basado en detección de manos
      String data = server.arg("fingers");
      if (data.length() > 0) {
        controlarDedos(data);
        server.send(200, "text/plain", "OK");
      } else {
        server.send(400, "text/plain", "Error: Datos de dedos vacíos");
      }
    } 
    else {
      server.send(400, "text/plain", "Error: Parámetros incorrectos o faltantes");
    }
  });

  // Iniciar el servidor web
  server.begin();
  Serial.println("Servidor web iniciado correctamente.");

  // Abrir la mano al inicio como estado inicial
  abrirMano();
}

void loop() {
  // Manejar solicitudes entrantes al servidor
  server.handleClient();
}

// Definición de la interfaz web (HTML + CSS + JavaScript)
String paginaWeb() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Control de Mano Robótica</title>
  <script src="https://cdn.jsdelivr.net/npm/@mediapipe/hands/hands.js"></script>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      background-color: #1a1a1a;
      color: #e0e0e0;
      margin: 0;
      padding: 20px;
    }
    h1, h2, h3 {
      color: #ffffff;
    }
    button {
      padding: 12px 24px;
      margin: 5px;
      font-size: 16px;
      background-color: #333333;
      color: #ffffff;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      transition: background-color 0.3s;
    }
    button:hover {
      background-color: #555555;
    }
    .dedo-control, .gestos, .camera-section {
      margin: 20px 0;
    }
    .dedo-control div {
      margin: 15px 0;
    }
    #videoContainer {
      position: relative;
      width: 100%;
      max-width: 640px;
      margin: 0 auto;
      display: none;
    }
    video, canvas {
      position: absolute;
      top: 0;
      left: 0;
      width: 100%;
      height: auto;
      transform: scaleX(-1); /* Espejo para reflejar la mano del usuario */
    }
    #startButton {
      background-color: #4CAF50;
    }
    #startButton:hover {
      background-color: #45a049;
    }
    @media (max-width: 768px) {
      button {
        width: 100%;
        margin: 10px 0;
        font-size: 14px;
      }
      .dedo-control div, .gestos {
        display: flex;
        flex-direction: column;
        align-items: center;
      }
      #videoContainer {
        max-width: 100%;
      }
    }
  </style>
</head>
<body>
  <h1>Control de Mano Robótica</h1>
  <p>Controla los dedos manualmente o usa la cámara para detectar gestos.</p>

  <div class="dedo-control">
    <h2>Control Individual</h2>
    <div>
      <h3>Pulgar</h3>
      <button onclick="moverDedo('pulgar', 'abrir')">Abrir</button>
      <button onclick="moverDedo('pulgar', 'cerrar')">Cerrar</button>
    </div>
    <div>
      <h3>Índice</h3>
      <button onclick="moverDedo('indice', 'abrir')">Abrir</button>
      <button onclick="moverDedo('indice', 'cerrar')">Cerrar</button>
    </div>
    <div>
      <h3>Medio</h3>
      <button onclick="moverDedo('medio', 'abrir')">Abrir</button>
      <button onclick="moverDedo('medio', 'cerrar')">Cerrar</button>
    </div>
    <div>
      <h3>Anular</h3>
      <button onclick="moverDedo('anular', 'abrir')">Abrir</button>
      <button onclick="moverDedo('anular', 'cerrar')">Cerrar</button>
    </div>
    <div>
      <h3>Meñique</h3>
      <button onclick="moverDedo('menique', 'abrir')">Abrir</button>
      <button onclick="moverDedo('menique', 'cerrar')">Cerrar</button>
    </div>
  </div>

  <div class="gestos">
    <h2>Gestos Predefinidos</h2>
    <button onclick="gesto('medio')">Dedo Medio</button>
    <button onclick="gesto('like')">Like</button>
    <button onclick="gesto('rock')">Rock and Roll</button>
  </div>

  <div class="camera-section">
    <h2>Detección de Manos</h2>
    <button id="startButton">Iniciar Cámara</button>
    <div id="videoContainer">
      <video id="inputVideo" autoplay playsinline></video>
      <canvas id="outputCanvas"></canvas>
    </div>
  </div>

  <script>
    const startButton = document.getElementById('startButton');
    const videoContainer = document.getElementById('videoContainer');
    const video = document.getElementById('inputVideo');
    const canvas = document.getElementById('outputCanvas');
    const ctx = canvas.getContext('2d');

    const hands = new Hands({
      locateFile: (file) => {
        return "https://cdn.jsdelivr.net/npm/@mediapipe/hands/" + file;
      }
    });
    hands.setOptions({
      maxNumHands: 1,          // Detectar solo una mano
      modelComplexity: 1,      // Modelo más preciso
      minDetectionConfidence: 0.7, // Confianza mínima para detección
      minTrackingConfidence: 0.5   // Confianza mínima para seguimiento
    });
    hands.onResults((results) => {
      ctx.save();
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      ctx.drawImage(results.image, 0, 0, canvas.width, canvas.height);
      if (results.multiHandLandmarks && results.multiHandLandmarks[0]) {
        const landmarks = results.multiHandLandmarks[0];
        const dedos = detectarDedos(landmarks);
        console.log("Dedos detectados:", dedos);
        enviarDatos(dedos);
        dibujarDeteccion(landmarks);
      }
      ctx.restore();
    });

    function detectarDedos(landmarks) {
      return [
        (landmarks[4].x < landmarks[3].x) ? 1 : 0,  // Pulgar abierto (1) o cerrado (0)
        (landmarks[8].y < landmarks[6].y) ? 1 : 0,  // Índice
        (landmarks[12].y < landmarks[10].y) ? 1 : 0,// Medio
        (landmarks[16].y < landmarks[14].y) ? 1 : 0,// Anular
        (landmarks[20].y < landmarks[18].y) ? 1 : 0 // Meñique
      ];
    }

    function dibujarDeteccion(landmarks) {
      ctx.beginPath();
      ctx.strokeStyle = '#4CAF50';
      ctx.lineWidth = 2;
      for (let i = 0; i < landmarks.length; i++) {
        const x = landmarks[i].x * canvas.width;
        const y = landmarks[i].y * canvas.height;
        ctx.arc(x, y, 5, 0, 2 * Math.PI);
        ctx.fillStyle = '#4CAF50';
        ctx.fill();
        if (i > 0) {
          ctx.moveTo(landmarks[i-1].x * canvas.width, landmarks[i-1].y * canvas.height);
          ctx.lineTo(x, y);
          ctx.stroke();
        }
      }
    }

    function enviarDatos(dedos) {
      fetch('/update', {
        method: 'POST',
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: 'fingers=' + dedos.join(',')
      }).catch(error => console.error("Error enviando datos al ESP32:", error));
    }

    startButton.addEventListener('click', () => {
      videoContainer.style.display = 'block';
      startButton.style.display = 'none';
      navigator.mediaDevices.getUserMedia({
        video: { facingMode: 'user', width: { ideal: 640 }, height: { ideal: 480 } }
      }).then(stream => {
        video.srcObject = stream;
        video.onloadedmetadata = () => {
          canvas.width = video.videoWidth;
          canvas.height = video.videoHeight;
          setInterval(() => {
            hands.send({image: video});
          }, 100); // Enviar datos cada 100ms
        };
      }).catch(error => {
        console.error("Error al acceder a la cámara:", error);
        alert("No se pudo acceder a la cámara. Verifica los permisos.");
        videoContainer.style.display = 'none';
        startButton.style.display = 'block';
      });
    });

    function moverDedo(dedo, accion) {
      fetch('/update', {
        method: 'POST',
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: `dedo=${dedo}&accion=${accion}`
      }).catch(error => console.error("Error enviando comando:", error));
    }

    function gesto(nombre) {
      fetch('/update', {
        method: 'POST',
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: `gesto=${nombre}`
      }).catch(error => console.error("Error enviando gesto:", error));
    }
  </script>
</body>
</html>
)rawliteral";
}

// Función para mover un dedo individualmente
void moverDedoIndividual(String dedo, String accion) {
  int posicion = 0;
  bool valido = true;

  if (accion == "abrir") {
    if (dedo == "pulgar") posicion = thumb_open;
    else if (dedo == "indice") posicion = index_open;
    else if (dedo == "medio") posicion = middle_open;
    else if (dedo == "anular") posicion = ring_open;
    else if (dedo == "menique") posicion = pinky_open;
    else valido = false;
  } 
  else if (accion == "cerrar") {
    if (dedo == "pulgar") posicion = thumb_close;
    else if (dedo == "indice") posicion = index_close;
    else if (dedo == "medio") posicion = middle_close;
    else if (dedo == "anular") posicion = ring_close;
    else if (dedo == "menique") posicion = pinky_close;
    else valido = false;
  } 
  else {
    valido = false;
  }

  if (valido) {
    if (dedo == "pulgar") thumb.write(posicion);
    else if (dedo == "indice") ind.write(posicion);
    else if (dedo == "medio") middle.write(posicion);
    else if (dedo == "anular") ring.write(posicion);
    else if (dedo == "menique") pinky.write(posicion);

    Serial.println("Moviendo " + dedo + " a " + accion + " (posición: " + String(posicion) + ")");
  } else {
    Serial.println("Error: Dedo o acción no válida - Dedo: " + dedo + ", Acción: " + accion);
  }
}

// Función para realizar gestos predefinidos
void realizarGesto(String gesto) {
  if (gesto == "medio") {
    thumb.write(thumb_close);
    ind.write(index_close);
    middle.write(middle_open);
    ring.write(ring_close);
    pinky.write(pinky_close);
    Serial.println("Gesto ejecutado: Dedo Medio");
  } 
  else if (gesto == "like") {
    thumb.write(thumb_open);
    ind.write(index_close);
    middle.write(middle_close);
    ring.write(ring_close);
    pinky.write(pinky_close);
    Serial.println("Gesto ejecutado: Like");
  } 
  else if (gesto == "rock") {
    thumb.write(thumb_close);
    ind.write(index_open);
    middle.write(middle_close);
    ring.write(ring_close);
    pinky.write(pinky_open);
    Serial.println("Gesto ejecutado: Rock and Roll");
  } 
  else {
    Serial.println("Error: Gesto no reconocido - " + gesto);
  }
}

// Función para controlar todos los dedos según datos de la cámara
void controlarDedos(String data) {
  int dedos[5] = {0}; // Array para almacenar el estado de los 5 dedos (0 = cerrado, 1 = abierto)
  int index = 0;
  int start = 0;

  // Parsear la cadena de datos (ejemplo: "1,0,1,0,0")
  while (index < 5 && start < data.length()) {
    int commaIndex = data.indexOf(',', start);
    if (commaIndex == -1) commaIndex = data.length();
    String value = data.substring(start, commaIndex);
    if (value.length() > 0) {
      dedos[index] = value.toInt();
    }
    index++;
    start = commaIndex + 1;
  }

  // Mover los servos según los datos recibidos
  thumb.write(dedos[0] ? thumb_open : thumb_close);
  ind.write(dedos[1] ? index_open : index_close);
  middle.write(dedos[2] ? middle_open : middle_close);
  ring.write(dedos[3] ? ring_open : ring_close);
  pinky.write(dedos[4] ? pinky_open : pinky_close);

  Serial.println("Controlando dedos: " + data);
  delay(50); // Pequeño retardo para estabilidad de los servos
}

// Función para abrir completamente la mano
void abrirMano() {
  thumb.write(thumb_open);
  ind.write(index_open);
  middle.write(middle_open);
  ring.write(ring_open);
  pinky.write(pinky_open);
  Serial.println("Mano completamente abierta");
}