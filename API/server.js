const express = require('express');
const http = require('http');
// const { Server } = require('socket.io');
const axios = require('axios');
const cors = require('cors');
require('dotenv').config();
const { connectDB } = require('./utils/db');
const authRoutes = require('./routes/auth');
const app = express();
const server = http.createServer(app);
const io = require('socket.io')(server, {
  cors: {
    origin: '*',
    methods: ['GET', 'POST'],
  }
});
connectDB();
app.use(cors());
app.use(express.json());

let esp32Socket = null;

io.on('connection', (socket) => {
  console.log('📡 Clientee conectaado:', socket.id);

  // Cuando se conecta el ESP32
  socket.on('esp32-ready', () => {
    esp32Socket = socket;
    console.log('✅ ESP32 reegistrado');
  });

  // ESP32 envía estado del sensor
  socket.on('fire-status', (data) => {
    console.log("🔥 Estados del sensor:", data);
    // Aquí podrías guardar en BD, mandar alertas, etc.
    socket.broadcast.emit('fire-status', data);
  });

  // APP móvil pide apagar el buzzer
  socket.on('apagar-buzzer', async() => {
    try {
      console.log('🧪 Enviando solicitud al ESP32 vía HTTP...');
  
      const response = await axios.post('http://192.168.100.2/apagar-buzzer');
      console.log('✅ Respuesta del ESP32:', response.data);
    } catch (err) {
      console.error('❌ Error aal contactar al ESP32:', err.message);
    }
    // setTimeout(() => {
    //   if (esp32Socket) {
    //     console.log('🧪 Emitiendo pruebas apagar-buzzer');
    //     esp32Socket.emit('apagar-buzzer');
    //     socket.emit('apagar-buzzer');
    //   }
    // }, 5000);
    //   console.log('🛑 Orden enviadaaa al ESP32: apagaar buzzer');
    //   socket.emit('apagar-buzzer');
  });

  socket.on('disconnect', () => {
    if (socket === esp32Socket) {
      console.log('❌ ESP32 desconectado');
      esp32Socket = null;
    } else {
      console.log('❌ Cliente desconectado:', socket.id);
    }
  });
});

app.use('/api', authRoutes);
app.get('/', (req, res) => {
  res.send("🔥 API FireSensor con Socket.IO está corriendo.");
});


server.listen(3000, '0.0.0.0', () => {
  console.log('🚀 Servidor escuchaando en http://localhost:3000');
});