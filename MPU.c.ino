#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>  // REQUIRED for notifications to work on Windows

// BLE UUIDs (NUS or custom — up to you, works fine here)
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define FSR_PIN  35
#define FLEX_PIN 34

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("✅ BLE device connected.");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("❌ BLE device disconnected.");
    BLEDevice::startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // SDA = 21, SCL = 22

  // Initialize BNO055
  if (!bno.begin()) {
    Serial.println("❌ BNO055 not detected.");
    while (1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);
  Serial.println("📈 BNO055 initialized.");

  // Initialize analog pins
  pinMode(FSR_PIN, INPUT);
  pinMode(FLEX_PIN, INPUT);

  // BLE Setup
  BLEDevice::init("ESP32-SensorHub");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // ✅ Use NOTIFY only (REMOVE WRITE), add BLE2902
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());  // <- Required for Windows BLE

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Android compatibility
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("📡 BLE advertising...");
}

void loop() {
  // Read sensor data
  sensors_event_t orientationData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

  int flexVal = analogRead(FLEX_PIN);  // 0–4095
  int fsrVal  = analogRead(FSR_PIN);   // 0–4095

  // Print to Serial
  Serial.print("Yaw: "); Serial.print(orientationData.orientation.x);
  Serial.print(" | Pitch: "); Serial.print(orientationData.orientation.y);
  Serial.print(" | Roll: "); Serial.print(orientationData.orientation.z);
  Serial.print(" | Flex: "); Serial.print(flexVal);
  Serial.print(" | FSR: "); Serial.println(fsrVal);

  // Format BLE message
  if (deviceConnected) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer),
             "%.2f,%.2f,%.2f,%d,%d",
             orientationData.orientation.x,
             orientationData.orientation.y,
             orientationData.orientation.z,
             flexVal,
             fsrVal);
    pCharacteristic->setValue(buffer);
    pCharacteristic->notify();
  }

  delay(100);  // 10Hz
}
