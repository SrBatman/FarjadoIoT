#include <WiFi.h>
#include <WebServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLERemoteCharacteristic.h>
#include <BLEClient.h>

// Wi-Fi
const char* ssid = "Oxxo";
const char* password = "ir3n4TOTALPLAY";

// Web Server
WebServer server(80);

// BLE
#define SCAN_TIME 5       // Segundos de escaneo BLE
#define BUZZER_PIN 27     // Pin conectado al buzzer

// UUIDs del servicio y característica BLE
static BLEUUID serviceUUID("12345678-1234-1234-1234-1234567890ab");
static BLEUUID characteristicUUID("abcdefab-1234-5678-1234-abcdefabcdef");

static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

// 🔔 Callback de notificación BLE
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {

  String value = "";
  for (int i = 0; i < length; i++) {
    value += (char)pData[i];
  }

  Serial.print("\xf0\x9f\x93\xa5 Notificación BLE: ");
  Serial.println(value);

  if (value == "fuego") {
    Serial.println("‼️ ¡FUEGO DETECTADO! Activando buzzer");
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    Serial.println("✅ Sin fuego");
    // No se apaga automáticamente
  }
}

// 📡 Callback del escaneo BLE
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("\xf0\x9f\x94\x8d Dispositivo encontrado: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.getServiceUUID().equals(serviceUUID)) {
      Serial.println("✅ Servicio FireSensor encontrado");
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

// 🔗 Conexión al servidor BLE
bool connectToServer() {
  Serial.print("Conectando a ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  pClient->connect(myDevice);
  Serial.println("✅ Conectado al servidor BLE");

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.println("❌ Servicio no encontrado");
    return false;
  }

  pRemoteCharacteristic = pRemoteService->getCharacteristic(characteristicUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.println("❌ Característica no encontrada");
    return false;
  }

  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
    Serial.println("📬 Suscrito a notificaciones BLE");
  }

  connected = true;
  return true;
}

// 🔌 Endpoint HTTP para apagar buzzer
void handleApagarBuzzer() {
  Serial.println("🚩 POST recibido: /apagar-buzzer");
  digitalWrite(BUZZER_PIN, LOW);
  server.send(200, "application/json", "{\"status\":\"buzzer apagado\"}");
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // 🔌 Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi conectado");
  Serial.print("🟢 IP Local: ");
  Serial.println(WiFi.localIP());

  // 🔸 API REST
  server.on("/apagar-buzzer", HTTP_POST, handleApagarBuzzer);
  server.begin();
  Serial.println("🌐 API REST iniciada en ESP32");

  // 🔎 BLE Scan
  BLEDevice::init("ESP32-FireClient");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(SCAN_TIME, false);
}

void loop() {
  server.handleClient(); // maneja las solicitudes HTTP

  if (doConnect && !connected) {
    if (connectToServer()) {
      Serial.println("✅ Conexión BLE exitosa");
    } else {
      Serial.println("❌ Reintentando escaneo...");
      BLEDevice::getScan()->start(SCAN_TIME, false);
    }
    doConnect = false;
  }

  delay(100);
}