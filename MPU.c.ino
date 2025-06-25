#include <Wire.h>
#include <MahonyAHRS.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

const int MPU1 = 0x68;
const int MPU2 = 0x69;

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;

Mahony filter1;
Mahony filter2;

float GyroX1, GyroY1, GyroZ1, AccX1, AccY1, AccZ1;
float GyroX2, GyroY2, GyroZ2, AccX2, AccY2, AccZ2;
float roll1, pitch1, yaw1, roll2, pitch2, yaw2;
float elapsedTime, currentTime, previousTime;

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

class CharCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    if (value.length() > 0 && value.charAt(0) == 'r') {
      filter1.begin(200);
      filter2.begin(200);
      Serial.println("🔄 Filters reset via BLE.");
    }
  }
};

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Wake up IMUs
  Wire.beginTransmission(MPU1); Wire.write(0x6B); Wire.write(0x00); Wire.endTransmission(true);
  Wire.beginTransmission(MPU2); Wire.write(0x6B); Wire.write(0x00); Wire.endTransmission(true);

  delay(100);
  filter1.begin(200);  // Use higher filter frequency
  filter2.begin(200);
  previousTime = millis();

  Serial.println("timestamp_ms,imu1_roll,imu1_pitch,imu1_yaw,imu2_roll,imu2_pitch,imu2_yaw");

  // BLE setup
  BLEDevice::init("ESP32-BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->setCallbacks(new CharCallbacks());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("📡 BLE is now advertising...");
}

void loop() {
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;

  readIMU(MPU1, AccX1, AccY1, AccZ1, GyroX1, GyroY1, GyroZ1);
  readIMU(MPU2, AccX2, AccY2, AccZ2, GyroX2, GyroY2, GyroZ2);

  filter1.updateIMU(GyroX1 * DEG_TO_RAD, GyroY1 * DEG_TO_RAD, GyroZ1 * DEG_TO_RAD, AccX1, AccY1, AccZ1);
  filter2.updateIMU(GyroX2 * DEG_TO_RAD, GyroY2 * DEG_TO_RAD, GyroZ2 * DEG_TO_RAD, AccX2, AccY2, AccZ2);

  roll1 = filter1.getRoll();
  pitch1 = filter1.getPitch();
  yaw1 = filter1.getYaw();
  roll2 = filter2.getRoll();
  pitch2 = filter2.getPitch();
  yaw2 = filter2.getYaw();

  // Serial output
  Serial.print((unsigned long)currentTime); Serial.print(",");
  Serial.print(roll1); Serial.print(",");
  Serial.print(pitch1); Serial.print(",");
  Serial.print(yaw1); Serial.print(",");
  Serial.print(roll2); Serial.print(",");
  Serial.print(pitch2); Serial.print(",");
  Serial.println(yaw2);

  // BLE output
  if (deviceConnected) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
             roll1, pitch1, yaw1, roll2, pitch2, yaw2);
    pCharacteristic->setValue((uint8_t*)buffer, strlen(buffer));
    pCharacteristic->notify();
  }
}

void readIMU(byte address, float &ax, float &ay, float &az, float &gx, float &gy, float &gz) {
  Wire.beginTransmission(address); Wire.write(0x3B); Wire.endTransmission(false);
  Wire.requestFrom(address, 6, true);
  ax = (Wire.read() << 8 | Wire.read()) / 16384.0;
  ay = (Wire.read() << 8 | Wire.read()) / 16384.0;
  az = (Wire.read() << 8 | Wire.read()) / 16384.0;

  Wire.beginTransmission(address); Wire.write(0x43); Wire.endTransmission(false);
  Wire.requestFrom(address, 6, true);
  gx = (Wire.read() << 8 | Wire.read()) / 131.0;
  gy = (Wire.read() << 8 | Wire.read()) / 131.0;
  gz = (Wire.read() << 8 | Wire.read()) / 131.0;
}
