#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// BLE UUIDs (Nordic UART or custom format)
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// Sensor pins
#define FSR_PIN   35
#define FLEX_PIN  34

// IMU object
Adafruit_BNO055 bno(55, 0x28, &Wire);

// BLE objects
BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

// BLE server callbacks
class SensorServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* server) override {
    deviceConnected = true;
    Serial.println("BLE device connected");
  }

  void onDisconnect(BLEServer* server) override {
    deviceConnected = false;
    Serial.println("BLE device disconnected");
    BLEDevice::startAdvertising();  // Resume advertising after disconnect
  }
};

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // ESP32 I2C pins: SDA = 21, SCL = 22

  // Initialize BNO055
  if (!bno.begin()) {
    Serial.println("BNO055 not detected. Check wiring or I2C address.");
    while (true);
  }
  delay(1000);
  bno.setExtCrystalUse(true);
  Serial.println("BNO055 initialized");

  // Set sensor pins
  pinMode(FSR_PIN, INPUT);
  pinMode(FLEX_PIN, INPUT);

  // BLE initialization
  BLEDevice::init("ESP32-SensorHub");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new SensorServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());  // Required for notifications

  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE advertising started");
}

void loop() {
  // Get orientation data
  sensors_event_t orientation;
  bno.getEvent(&orientation, Adafruit_BNO055::VECTOR_EULER);

  // Read flex and FSR analog values
  int flexValue = analogRead(FLEX_PIN);
  int fsrValue  = analogRead(FSR_PIN);

  // Output to serial
  Serial.print("Yaw: ");   Serial.print(orientation.orientation.x);
  Serial.print(" | Pitch: "); Serial.print(orientation.orientation.y);
  Serial.print(" | Roll: ");  Serial.print(orientation.orientation.z);
  Serial.print(" | Flex: ");  Serial.print(flexValue);
  Serial.print(" | FSR: ");   Serial.println(fsrValue);

  // Send data over BLE if connected
  if (deviceConnected) {
    char dataPacket[64];
    snprintf(dataPacket, sizeof(dataPacket),
             "%.2f,%.2f,%.2f,%d,%d",
             orientation.orientation.x,
             orientation.orientation.y,
             orientation.orientation.z,
             flexValue,
             fsrValue);

    pCharacteristic->setValue(dataPacket);
    pCharacteristic->notify();
  }

  delay(100);  // 10 Hz sampling
}
