#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

#define SERVICE_UUID "89b335ee-2a2f-44c7-9ef6-d51be29fa99e"
const int SCAN_DURATION = 3;
const int TX_POWER = -68;
const float PATH_LOSS_EXP = 4;

BLEScan* pBLEScan;

float calculateDistance(int rssi) {
  return pow(10.0, (TX_POWER - rssi) / (10.0 * PATH_LOSS_EXP));
}

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  Serial.println("BLE Distance Monitor Started");
}

void loop() {
  BLEScanResults* foundDevices = pBLEScan->start(SCAN_DURATION, false);

  for (int i = 0; i < foundDevices->getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices->getDevice(i);
    
    if (device.haveServiceUUID() && 
        device.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
        
      int rssi = device.getRSSI();
      float distance = calculateDistance(rssi);

      Serial.print("RSSI: ");
      Serial.print(rssi);
      Serial.print(" dBm | Estimated Distance: ");
      Serial.print(distance, 2);
      Serial.println(" m");
    }
  }

  pBLEScan->clearResults();
  delay(1000);
}
