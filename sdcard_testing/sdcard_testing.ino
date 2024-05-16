#include <Arduino.h>
#include <SD.h>

/*

Library: 
  https://github.com/espressif/arduino-esp32/blob/master/libraries/SD/README.md

Module pins - Digital ESP32 pin
  CS - D15
  MOSI - D23
  SCK - D18
  MISO - D19
*/

#define SDCARD_MOSI_PIN 23
#define SDCARD_MISO_PIN 19
#define
#define CSPIN 15

File testFile;  // Create File object

void setup() {
  Serial.begin(115200);
  Serial.print("Starting SD...\n");
  if (!SD.begin(15)) {
    Serial.print("\tCould not begin SD\n");
    return;
  }
  Serial.print("\tStart succesful!\n");

  testFile = SD.open("test.txt");  //Open file
  if (testFile) {
    Serial.print("\tCreated file \"test.txt\"\n");
    while (testFile.available()) {
      Serial.write(testFile.read());
    }
    testFile.close();  //Close file
  } else {
    Serial.print("\t Error opening file :C\n");
  }
}

void loop() {
  // Lol
}
