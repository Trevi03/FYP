#include <ArduinoBLE.h>
#include "LEDControl.h"
#include <math.h>

#define DEBUG_MODE true
#if DEBUG_MODE
  #define DEBUG_PRINT(x)  Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// General variables
static bool datawrite = false; // start or stop data stream
unsigned long count = 0;
unsigned long startTime = 0;
float t = 0; // for simulating fake data
uint8_t blePacket[240];  // 10 samples × 24 bytes/sample
int sampleIndex = 0;
unsigned long lastSampleTime = 0;
const unsigned long sampleInterval = 1; // 1 ms = 1 kHz

// Define the official Generic Health Sensor service
BLEService sensorService("1840");

// Custom UUID for packed 8-channel data
BLECharacteristic dataChar("FFF1", BLERead | BLENotify, 240); // 8 channels × 3 bytes = 24 bytes for 10 batches

// Characteristic for commands (writable)
BLECharacteristic commandChar("FFF2", BLEWrite | BLEWriteWithoutResponse, 20);

// Advertising parameters should have a global scope. Do NOT define them in 'setup' or in 'loop'
const uint8_t completeRawAdvertisingData[] = {0x02, 0x01, 0x06,0x03, 0x03, 0x40, 0x18};

void blePeripheralConnectHandler(BLEDevice central) {
  DEBUG_PRINT("Connected event, central: ");
  DEBUG_PRINTLN(central.address());
  startPulse("GREEN");
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  DEBUG_PRINT("Disconnected event, central: ");
  DEBUG_PRINTLN(central.address());
  BLE.advertise();
  toggleBlinkLED("BLUE");
}

void setup() {
  setupLEDs();

  if (DEBUG_MODE) {
    Serial.begin(9600);
    while (!Serial); // wait for serial monitor to open
  }

  if (!BLE.begin()) {
    DEBUG_PRINTLN("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  sensorService.addCharacteristic(dataChar);
  sensorService.addCharacteristic(commandChar);
  BLE.addService(sensorService); // Register service

  // Build advertising data packet
  BLEAdvertisingData advData;
  // If a packet has a raw data parameter, then all the other parameters of the packet will be ignored
  advData.setRawData(completeRawAdvertisingData, sizeof(completeRawAdvertisingData));
  // Copy set parameters in the actual advertising packet
  BLE.setAdvertisingData(advData);

  // Build scan response data packet
  BLEAdvertisingData scanData;
  scanData.setLocalName("TBI_Monitor");
  // Copy set parameters in the actual scan response packet
  BLE.setScanResponseData(scanData);
  BLE.setDeviceName("TBI_Monitor");
  
  BLE.setConnectable(true);

  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  BLE.advertise();
  toggleBlinkLED("BLUE");
  DEBUG_PRINTLN("BLE device is now advertising...");
}

void loop() {
  BLE.poll();
  long buffer[9];

  if (commandChar.written()){
    // Check for command from the GUI end
    char buf[21] = {0};  // +1 for null terminator
    int len = commandChar.readValue((uint8_t*)buf, 20);
    buf[len] = '\0';  // null terminate
    DEBUG_PRINT("Received command: ");
    DEBUG_PRINTLN(buf);
    
    processCommand(buf);
  }

  checkLED();

  // This is where the ADS read comes in
  unsigned long now = millis();
  if (now - lastSampleTime >= sampleInterval) {
    lastSampleTime = now;

    long buffer[9];
    if (getFakeADSData(buffer)) {

      DEBUG_PRINT("Sample ");
      DEBUG_PRINT(sampleIndex);
      DEBUG_PRINT(": ");

      for (int ch = 1; ch <= 8; ch++) {
        long value = buffer[ch]; // long is 4 bytes

        // send the lower 24 bits (3 bytes) over BLE
        int offset = sampleIndex * 24 + (ch - 1) * 3;
        blePacket[offset + 0] = (value >> 16) & 0xFF;
        blePacket[offset + 1] = (value >> 8) & 0xFF;
        blePacket[offset + 2] = value & 0xFF;

        DEBUG_PRINT("CH");
        DEBUG_PRINT(ch);
        DEBUG_PRINT("=");
        DEBUG_PRINT(value);
        DEBUG_PRINT(" ");
      }
      DEBUG_PRINTLN();

      sampleIndex++;

      if (sampleIndex == 10) {
        dataChar.writeValue(blePacket, 240);
        sampleIndex = 0;
      }
    }
  }

  // // Loop speed
  // count++;
  // if (micros() - startTime >= 1000000) { // loops per second
  //   DEBUG_PRINTLN(count);
  //   count = 0;
  //   startTime = micros();
  // }
}

void processCommand(char* buf){
  if (strcmp(buf, "DISCONNECT") == 0) {
    BLE.disconnect();
    DEBUG_PRINTLN("Disconnect command received!");
    blePeripheralDisconnectHandler(BLE.central());
  }
  else if (strcmp(buf, "START") == 0) {
    datawrite = true;
    startPulse("RED");
  }
  else if (strcmp(buf, "STOP") == 0) {
    datawrite = false;
    startPulse("RED");
  }
}

// Simulate ADS1298 data with ±2.4 V full scale (differential input), gain = 1
boolean getFakeADSData(long *buffer) {
  if (datawrite) {
    buffer[0] = 0;  // Reserved for status or unused

    for (int i = 1; i <= 8; i++) {
      float voltage = sin(t + i * 0.3) * 2.0;  // ±2.4 V sine wave per channel
      // Scale to 24-bit signed integer range (-8388608 to 8388607)
      buffer[i] = (long)((voltage / 2.4) * 8388607.0); 
      //signed int32 integer holding the 24-bit ADC simulated data
    }

    t += 0.01;  // Advance time for sine wave

    return true;
  } else {
    return false;
  }
}
