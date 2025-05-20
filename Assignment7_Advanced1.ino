#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEScan.h>

#define SERVICE_UUID "89b335ee-2a2f-44c7-9ef6-d51be29fa99e"
const int SCAN_DURATION = 3;
const int TX_POWER = -68;
const float PATH_LOSS_EXP = 4;

WiFiServer httpSrv(80);
BLEScan* pBLEScan;
float lastDistance = -1.0;

float calculateDistance(int rssi) {
  return pow(10.0, (TX_POWER - rssi) / (10.0 * PATH_LOSS_EXP));
}

void setup() {
  Serial.begin(115200);

  WiFi.begin("SSID", "Password");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nHTTP Server IP: " + WiFi.localIP().toString());
  httpSrv.begin();

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
}

void loop() {
  BLEScanResults* results = pBLEScan->start(SCAN_DURATION, false);
  for (int i = 0; i < results->getCount(); i++) {
    BLEAdvertisedDevice dev = results->getDevice(i);
    if (dev.haveServiceUUID() && dev.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
      lastDistance = calculateDistance(dev.getRSSI());
      Serial.printf("RSSI: %d, Distance: %.2f m\n", dev.getRSSI(), lastDistance);
      break;
    }
  }
  pBLEScan->clearResults();

  WiFiClient client = httpSrv.available();
  if (client) {
    String req = client.readStringUntil('\r');
    client.readStringUntil('\n');
    if (req.startsWith("GET / ")) {
      String html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
        "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='1'></head><body>"
        "<h2>BLE Distance</h2>"
        "<p>Distance: " + String(lastDistance, 2) + " m</p>"
        "</body></html>";
      client.print(html);
    }
    while (client.connected() && client.available()) client.read();
    client.stop();
  }
  delay(100);
}
