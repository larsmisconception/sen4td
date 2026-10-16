/*
 * HX711 Load Cell Scale Factor Calibration Note
 *
 * The scale factor is used to convert raw ADC values from the HX711
 * into grams.
 *
 * Calibration Steps:
 * 1. Tare the scale (set it to zero with no weight).
 * 2. Place a known weight on the load cell (e.g., 1000g).
 * 3. Observe the raw reading from the HX711.
 * 4. Calculate the scale factor using:
 *      scale_factor = raw_reading / known_weight
 * 5. Replace this value eith scale_factor:
 *      scale.set_scale(scale_factor);
 *
 * Tips:
 * - Use accurate, known weights for calibration.
 * - Make sure the load cell is mounted securely during calibration.
 * - Recalibrate if the physical setup changes.
 */
#include <Arduino.h>
#include "7semi_HX711.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>

#include "GY521.h"

GY521 sensor(0x68);
BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
String txValue = "no value";
float dfWeightbuffer1 =0;
float dfWeightbuffer2 =0;
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA87" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA88"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA89"

#define VIBPIN 0

int counter =0;
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

/**
 * Pin configuration
 * - HX_DOUT_PIN: HX711 DOUT pin
 * - HX_SCK_PIN : HX711 SCK pin
 */
const int HX_DOUT_PIN_1 = 7;
const int HX_SCK_PIN_1  = 8;

const int HX_DOUT_PIN_2 = 9;
const int HX_SCK_PIN_2  = 10;
#define I2C_SDA 5
#define I2C_SCL 6

/**
 * Calibration factor
 * - Value is raw units per gram
 * - Adjust after calibration for your load cell
 */
float CALIBRATION_FACTOR = 1;

/**
 * HX711 instance
 * - Uses data and clock pins
 */
HX711_7semi scale1(HX_DOUT_PIN_1, HX_SCK_PIN_1);
HX711_7semi scale2(HX_DOUT_PIN_2, HX_SCK_PIN_2);

float dfX, dfY, dfZ, dfAX, dfAY, dfAZ, dfTemp;

/**
 * Setup
 * - Start serial
 * - Initialize HX711
 * - Apply calibration and tare
 */
void setup() {
  Serial.begin(115200);
  digitalWrite(HX_DOUT_PIN_1, HIGH);
  digitalWrite(HX_DOUT_PIN_2, HIGH);
  scale1.begin();
  scale1.setGain(GAIN_128);
  scale1.setTimeout(1000);
  scale1.setScale(1);
  Serial.println("HX711 ch1 enabled");
  Serial.println("Taring... remove any load");
  delay(2000);
  scale1.tare(10);   /* uses default sample count */
  Serial.println("Tare done.");

  scale2.begin();
  scale2.setGain(GAIN_128);
  scale2.setTimeout(1000);
  scale2.setScale(1);
  Serial.println("HX711 ch2 enabled");
  Serial.println("Taring... remove any load");
  delay(2000);
  scale2.tare(10);   /* uses default sample count */
  Serial.println("Tare done.");

	Wire.begin(I2C_SDA, I2C_SCL, 10000);
  
  sensor.wakeup();
  sensor.setAccelSensitivity(0);  //  8g
  sensor.setGyroSensitivity(0);   //  500 degrees/s
  sensor.setDLPFMode(5);
  sensor.setThrottle();
  Serial.println("start Gyro...");

  // Create the BLE Device
  BLEDevice::init("Multisensor-Bra");

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
  pinMode(VIBPIN, OUTPUT);}

/**
 * Main loop
 * - Read weight in grams
 * - Print to serial monitor
 */
void loop() {
  float weight1 = scale1.readAverage(5);  /* uses default averaging */
  txValue = "B1:";
  txValue += weight1;
  float weight2 = scale2.readAverage(5);  /* uses default averaging */
  txValue += ";B2:";
  txValue += weight2;

  
  sensor.read();
  dfX = sensor.getAngleX();
  dfY = sensor.getAngleY();
  dfZ = sensor.getAngleZ();
  dfTemp = sensor.getTemperature();
  dfAX = sensor.getAccelX();
  dfAY = sensor.getAccelY();
  dfAZ = sensor.getAccelZ();

        txValue += ";T:";
        txValue += dfTemp;
        txValue += ";X:";
        txValue += dfX;
        txValue += ";Y:";
        txValue += dfY;
        txValue += ";Z:";
        txValue += dfZ;
        txValue += ";AX:";
        txValue += dfAX;
        txValue += ";AY:";
        txValue += dfAY;
        txValue += ";AZ:";
        txValue += dfAZ;
if (deviceConnected) {
        pTxCharacteristic->setValue(txValue);
        pTxCharacteristic->notify();
		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}
    Serial.println(txValue);

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