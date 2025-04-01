#include <WiFi.h>
#include <SocketIoClient.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// 🛜 WiFi
const char* ssid = "Oxxo";
const char* password = "ir3n4TOTALPLAY";
const char* serverAddress = "192.168.100.9"; // o tu IP local
const uint16_t serverPort = 3000;

SocketIoClient socket;

// 🔥 Sensor
int Fire_analog = 34;
int Fire_digital = 2;

// 🔵 BLE
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-5678-1234-abcdefabcdef"

BLECharacteristic* fireCharacteristic;

void setup() {
  Serial.begin(115200);
  pinMode(Fire_digital, INPUT);
  pinMode(Fire_analog, INPUT);

  // 🔵 Iniciar BLE
  BLEDevice::init("ESP32-FireSensor");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  fireCharacteristic = pService->createCharacteristic(
                         CHARACTERISTIC_UUID,
                         BLECharacteristic::PROPERTY_READ |
                         BLECharacteristic::PROPERTY_NOTIFY
                       );
  fireCharacteristic->addDescriptor(new BLE2902());
  fireCharacteristic->setValue("nohayfuego");
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  // 🛜 Conectar a Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("✅ Conectado");

  // 🔌 Conectar al servidor socket
  socket.begin(serverAddress, serverPort);
  socket.on("connect", [](const char* payload, size_t length) {
    Serial.println("🔗 Conectado a Socket.IO");
    socket.emit("esp32-ready");
  });
}

void loop() {
  socket.loop(); // mantener viva la conexión

  int firesensorAnalog = analogRead(Fire_analog);
  int firesensorDigital = digitalRead(Fire_digital);

  Serial.print("Analog: ");
  Serial.print(firesensorAnalog);
  Serial.print(" | Digital: ");
  Serial.print(firesensorDigital);

  String newValue;
   
  if (firesensorAnalog < 1000) {
    newValue = "fuego";
    Serial.println(" -> 🔥 ¡FUEGO DETECTADO!");
  } else {
    newValue = "nohayfuego";
    Serial.println(" -> ✅ Sin fuego");
  }

  static String lastValue = "";
  if (newValue === "fuego") {
    fireCharacteristic->setValue(newValue.c_str());
    fireCharacteristic->notify();

    // Emitir evento por socket
    socket.emit("fire-status", "\"fuego\"");

    Serial.println("📬 Enviado por socket: " + newValue);
    lastValue = newValue;
  }

  delay(500);
}