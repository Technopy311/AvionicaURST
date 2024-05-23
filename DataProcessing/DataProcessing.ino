#include <Arduino.h>
#include "FastIMU.h"
#include <Wire.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define IMU_ADDRESS 0x68    //Change to the address of the IMU
#define PERFORM_CALIBRATION //Comment to disable startup calibration
MPU6500 IMU;               //Change to the name of any supported IMU! 

// Currently supported IMUS: MPU9255 MPU9250 MPU6886 MPU6500 MPU6050 ICM20689 ICM20690 BMI055 BMX055 BMI160 LSM6DS3 LSM6DSL QMI8658

calData calib = { 0 };  //Calibration data
AccelData accelData;    //Sensor data
GyroData gyroData;
MagData magData;

#define rest_time_ms 1 // Rest time to use between data writes to SD card

// SD card functions - from SD_Test esp32 SD library example.


void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.print("F");
        return;
    }
    if(file.print(message)){
        Serial.print(".");
    } else {
        Serial.print("X");
    }
    file.close();
}

char accelX[8];
char accelY[8];
char accelZ[8];

char gyroX[8];
char gyroY[8];
char gyroZ[8];

char timechar[8];

unsigned long startTime = millis();

void setup() {
  Serial.begin(115200);

  //## IMU Setup
  Wire.begin();
  Wire.setClock(400000); //400khz clock
  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
    while (true) {
      ;
    }
  }
  
  #ifdef PERFORM_CALIBRATION
    Serial.println("FastIMU calibration & data example");
    if (IMU.hasMagnetometer()) {
      delay(1000);
      Serial.println("Move IMU in figure 8 pattern until done.");
      delay(3000);
      IMU.calibrateMag(&calib);
      Serial.println("Magnetic calibration done!");
    }
    else {
      delay(5000);
    }

    delay(5000);
    Serial.println("Keep IMU level.");
    delay(5000);
    IMU.calibrateAccelGyro(&calib);
    Serial.println("Calibration done!");
    Serial.println("Accel biases X/Y/Z: ");
    Serial.print(calib.accelBias[0]);
    Serial.print(", ");
    Serial.print(calib.accelBias[1]);
    Serial.print(", ");
    Serial.println(calib.accelBias[2]);
    Serial.println("Gyro biases X/Y/Z: ");
    Serial.print(calib.gyroBias[0]);
    Serial.print(", ");
    Serial.print(calib.gyroBias[1]);
    Serial.print(", ");
    Serial.println(calib.gyroBias[2]);
    delay(5000);
    IMU.init(calib, IMU_ADDRESS);
  #endif
  // Configure gyro and accel errors to +-250 degrees/s and +-2 g
  err = IMU.setGyroRange(250); 
  err = IMU.setAccelRange(2);  

  if (err != 0) {
    Serial.print("Error Setting range: ");
    Serial.println(err);
    while (true) {
      ;
    }
  }


  //## SD Card setup
  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }

  

  writeFile(SD, "/testing.csv", "Time(ms)\tAccelX(g)\tAccelY(g)\tAccelZ(g)\tGyroX(d/s)\tGyroY(d/s)\tGyroZ(d/s)");
  appendFile(SD, "/testing.csv", "\n");
  Serial.print("\n");
  delay(2000);
}

void loop() {
  unsigned long currentTime = millis();
  IMU.update();
  IMU.getAccel(&accelData);
  IMU.getGyro(&gyroData);
  unsigned long elapsedTime = currentTime-startTime;

  dtostrf(accelData.accelX, 4, 2, accelX);
  dtostrf(accelData.accelY, 4, 2, accelY);
  dtostrf(accelData.accelZ, 4, 2, accelZ);

  dtostrf(gyroData.gyroX, 4, 2, gyroX);
  dtostrf(gyroData.gyroY, 4, 2, gyroY);
  dtostrf(gyroData.gyroZ, 4, 2, gyroZ);


  dtostrf(elapsedTime, 6, 0, timechar);

  appendFile(SD, "/testing.csv", timechar);
  appendFile(SD, "/testing.csv", "\t");

  appendFile(SD, "/testing.csv", accelX);
  appendFile(SD, "/testing.csv", "\t");
  appendFile(SD, "/testing.csv", accelY);
  appendFile(SD, "/testing.csv", "\t");
  appendFile(SD, "/testing.csv", accelZ);
  appendFile(SD, "/testing.csv", "\t");
  delay(rest_time_ms);
  appendFile(SD, "/testing.csv", gyroX);
  appendFile(SD, "/testing.csv", "\t");
  appendFile(SD, "/testing.csv", gyroY);
  appendFile(SD, "/testing.csv", "\t");
  appendFile(SD, "/testing.csv", gyroZ);
  appendFile(SD, "/testing.csv", "\n");
  delay(rest_time_ms);
  
  Serial.print("\n");
  
}
