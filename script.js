const ESP32_URL = 'http://tu-dominio.com'; // Cambiar por la URL real del ESP32

// Elementos DOM
const startButton = document.getElementById('startButton');
const videoContainer = document.getElementById('videoContainer');
const video = document.getElementById('inputVideo');
const canvas = document.getElementById('outputCanvas');
const ctx = canvas.getContext('2d');

// Configuración de MediaPipe Hands
const hands = new Hands({
  locateFile: (file) => `https://cdn.jsdelivr.net/npm/@mediapipe/hands/${file}`
});

hands.setOptions({
  maxNumHands: 1,
  modelComplexity: 1,
  minDetectionConfidence: 0.7,
  minTrackingConfidence: 0.5
});

// Manejo de resultados de detección
hands.onResults((results) => {
  ctx.save();
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  ctx.drawImage(results.image, 0, 0, canvas.width, canvas.height);
  
  if (results.multiHandLandmarks?.[0]) {
    const landmarks = results.multiHandLandmarks[0];
    const dedos = detectarDedos(landmarks);
    enviarDatos(dedos);
    dibujarDeteccion(landmarks);
  }
  ctx.restore();
});

// Funciones de detección y dibujo
function detectarDedos(landmarks) {
  return [
    landmarks[4].x < landmarks[3].x ? 1 : 0,  // Pulgar
    landmarks[8].y < landmarks[6].y ? 1 : 0,   // Índice
    landmarks[12].y < landmarks[10].y ? 1 : 0, // Medio
    landmarks[16].y < landmarks[14].y ? 1 : 0, // Anular
    landmarks[20].y < landmarks[18].y ? 1 : 0  // Meñique
  ];
}

function dibujarDeteccion(landmarks) {
  ctx.beginPath();
  ctx.strokeStyle = '#4CAF50';
  ctx.lineWidth = 2;
  
  landmarks.forEach((lm, i) => {
    const x = lm.x * canvas.width;
    const y = lm.y * canvas.height;
    
    ctx.arc(x, y, 5, 0, 2 * Math.PI);
    ctx.fillStyle = '#4CAF50';
    ctx.fill();
    
    if (i > 0) {
      ctx.moveTo(landmarks[i-1].x * canvas.width, landmarks[i-1].y * canvas.height);
      ctx.lineTo(x, y);
      ctx.stroke();
    }
  });
}

// Comunicación con el ESP32
function enviarDatos(dedos) {
  fetch(`${ESP32_URL}/update`, {
    method: 'POST',
    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
    body: `fingers=${dedos.join(',')}`
  }).catch(console.error);
}

function moverDedo(dedo, accion) {
  fetch(`${ESP32_URL}/update`, {
    method: 'POST',
    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
    body: `dedo=${dedo}&accion=${accion}`
  }).catch(console.error);
}

function gesto(nombre) {
  fetch(`${ESP32_URL}/update`, {
    method: 'POST',
    headers: {'Content-Type': 'application/x-www-form-urlencoded'},
    body: `gesto=${nombre}`
  }).catch(console.error);
}

// Inicialización de cámara
startButton.addEventListener('click', () => {
  videoContainer.style.display = 'block';
  startButton.style.display = 'none';
  
  navigator.mediaDevices.getUserMedia({
    video: { 
      facingMode: 'user', 
      width: { ideal: 640 }, 
      height: { ideal: 480 } 
    }
  }).then(stream => {
    video.srcObject = stream;
    video.onloadedmetadata = () => {
      canvas.width = video.videoWidth;
      canvas.height = video.videoHeight;
      setInterval(() => hands.send({image: video}), 100);
    };
  }).catch(error => {
    console.error("Error al acceder a la cámara:", error);
    alert("No se pudo acceder a la cámara. Verifica los permisos.");
    videoContainer.style.display = 'none';
    startButton.style.display = 'block';
  });
});