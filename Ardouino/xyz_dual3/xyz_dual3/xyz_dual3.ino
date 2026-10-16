/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <Wire.h>

#include <ADXL345.h>

ADXL345 accel1(ADXL345_STD);
ADXL345 accel2(ADXL345_ALT);


uint32_t counter = 0;

#define VIBPIN 0

#define I2C_SDA 5
#define I2C_SCL 6

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
String txValue = "no value";

float dfX1, dfY1, dfZ1, dfAX1, dfAY1, dfAZ1, dfTemp1;
float dfX2, dfY2, dfZ2, dfAX2, dfAY2, dfAZ2, dfTemp2;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA91" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA92"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA93"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
        int iValue = rxValue.toInt();
        analogWrite(VIBPIN, iValue);
      }
    }
};


void setup() {
  Serial.begin(115200);
	Wire.begin(I2C_SDA, I2C_SCL, 10000);
  byte deviceID1 = accel1.readDeviceID();
  if (deviceID1 != 0) {
    Serial.print("0x");
    Serial.print(deviceID1, HEX);
    Serial.println("");
  } 
  else {
    Serial.println("read device id: failed");
    while(1) {
      delay(100);
    }
  }
  byte deviceID2 = accel2.readDeviceID();
  if (deviceID2 != 0) {
    Serial.print("0x");
    Serial.print(deviceID2, HEX);
    Serial.println("");
  } 
  else {
    Serial.println("read device id: failed");
    while(1) {
      delay(100);
    }
  }
    if (!accel1.writeRate(ADXL345_RATE_200HZ)) {
    Serial.println("write rate1: failed");
    while(1) {
      delay(100);
    }
  }
    if (!accel2.writeRate(ADXL345_RATE_200HZ)) {
    Serial.println("write rate2: failed");
    while(1) {
      delay(100);
    }
  }
  if (!accel1.writeRange(ADXL345_RANGE_2G)) {
    Serial.println("write range1: failed");
    while(1) {
      delay(100);
    }
  }
  if (!accel2.writeRange(ADXL345_RANGE_2G)) {
    Serial.println("write range2: failed");
    while(1) {
      delay(100);
    }
  }
  if (!accel1.start()) {
    Serial.println("start 1: failed");
    while(1) {
      delay(100);
    }
  }
  if (!accel2.start()) {
    Serial.println("start 2: failed");
    while(1) {
      delay(100);
    }
  }

  Serial.println("start...");


  // Create the BLE Device
  BLEDevice::init("Dual Sensor Top");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  pinMode(VIBPIN, OUTPUT);
}

void loop() {

  if (accel1.update()) {
  dfX1 = accel1.getRawX();
  dfY1 = accel1.getRawY();
  dfZ1 = accel1.getRawZ();
  dfTemp1 = 0;
  dfAX1 = accel1.getX();
  dfAY1 = accel1.getY();
  dfAZ1 = accel1.getZ();
    }
   
  else {
    Serial.println("update 1 failed");
    while(1) {
      delay(100);
    }
  }
  if (accel2.update()) {
  dfX2 = accel2.getRawX();
  dfY2 = accel2.getRawY();
  dfZ1 = accel2.getRawZ();
  dfTemp2 = 0;
  dfAX2 = accel2.getX();
  dfAY2 = accel2.getY();
  dfAZ2 = accel2.getZ();
    }
  
  else {
    Serial.println("update 2 failed");
    while(1) {
      delay(100);
    }
  }
  counter++;
	
        txValue = "T1:";
        txValue += dfTemp1;
        txValue += ";X1:";
        txValue += dfX1;
        txValue += ";Y1:";
        txValue += dfY1;
        txValue += ";Z1:";
        txValue += dfZ1;
        txValue += ";AX1:";
        txValue += dfAX1;
        txValue += ";AY1:";
        txValue += dfAY1;
        txValue += ";AZ1:";
        txValue += dfAZ1;
        txValue += ";T2:";
        txValue += dfTemp2;
        txValue += ";X2:";
        txValue += dfX2;
        txValue += ";Y2:";
        txValue += dfY2;
        txValue += ";Z2:";
        txValue += dfZ2;
        txValue += ";AX2:";
        txValue += dfAX2;
        txValue += ";AY2:";
        txValue += dfAY2;
        txValue += ";AZ2:";
        txValue += dfAZ2;
  Serial.println(txValue);
    if (deviceConnected) {
        pTxCharacteristic->setValue(txValue);
        pTxCharacteristic->notify();
		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}
  if (counter % 10 == 0)
    {
 //       Serial.println(txValue);
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}