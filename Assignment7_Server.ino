#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "89b335ee-2a2f-44c7-9ef6-d51be29fa99e"

void setup() {
  Serial.begin(115200);

  BLEDevice::init("Team3_Server");

  BLEServer* pServer = BLEDevice::createServer();

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::setPower(ESP_PWR_LVL_P9);
  pAdvertising->start();

  Serial.println("BLE Server Activated, Advertising...");
}

void loop() {
  delay(1000);
}
