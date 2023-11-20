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
#define PASSLEGTH 8

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

String answer = "";

String generateRandomString(int length)
{

  String characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  String randomString = "";
  answer = "";
  for (int i = 0; i < length; ++i)
  {
    int randomIndex = random(characters.length());
    randomString += characters.charAt(randomIndex);
  }
  for (int i = 0; i < length; ++i)
  {
    answer += randomString.charAt(length - i - 1);
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
  question = generateRandomString(PASSLEGTH);
  Serial.println("Q:" + question);
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
    std::string readValue = pCharacteristic->getValue();
    const char *responseCPtr = readValue.c_str();
    // is equal to question
    if (strncmp(answer.c_str(), responseCPtr, 8) == 0)
    {
      // correct
      // differ
      Serial.println("Key Authorized.");
      char *token = strtok((char *)responseCPtr, ";");
      // split first token
      char *cmd = strtok(NULL, ";");
      Serial.println(cmd);
      // random new question
      question = generateRandomString(PASSLEGTH);
      pCharacteristic->setValue(question.c_str());
    };
    delay(50);
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
