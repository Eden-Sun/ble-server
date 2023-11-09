/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "0823143b-306c-4735-83db-64053a24765c"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("onConnect");
    deviceConnected = true;
    BLEDevice::startAdvertising();
  };

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("onDisconnect");
    deviceConnected = false;
  }
};

String generateRandomString(int length)
{
  String characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  String randomString = "";

  for (int i = 0; i < length; ++i)
  {
    int randomIndex = random(characters.length());
    randomString += characters.charAt(randomIndex);
  }

  return randomString;
}

BLECharacteristic *pCharacteristic = NULL;
BLEServer *pServer = NULL;
String question = "";
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("awesomeboy");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  question = generateRandomString(8);
  Serial.println(question.c_str());
  pCharacteristic->setValue(question.c_str());
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (deviceConnected)
  {
    const char *newV = pCharacteristic->getValue().c_str();
    if (!question.compareTo(String(newV)))
    {
      Serial.println(*newV);
    };
    delay(200);
  }

  // disconnecting
  // if (!deviceConnected && oldDeviceConnected) {
  //   delay(500);                   // give the bluetooth stack the chance to get things ready
  //   pServer->startAdvertising();  // restart advertising
  //   Serial.println("start advertising");
  //   oldDeviceConnected = deviceConnected;
  // }

  // connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }

  // Serial.println(pCharacteristic->getValue().c_str());
}
