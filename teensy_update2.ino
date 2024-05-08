#include <Wire.h> // Wire Library
#include "Adafruit_HTU21DF.h" // Humidity Sensor Library
#include <Adafruit_LSM6DSOX.h> // IMU Sensor Library
#include <Adafruit_LIS3MDL.h> // Magnetometer Library
#include <Adafruit_Sensor.h> 
// #include <Adafruit_BNO055.h>
#include <Adafruit_DPS310.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = BUILTIN_SDCARD;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Adafruit_LSM6DSOX sox;
Adafruit_LIS3MDL lis3mdl;
Adafruit_DPS310 dps;
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();

// Adafruit_BNO055 bno;



// const int numReadings = 10;
const int numReadings = 11;

// const int numReadings2=11;
float readings[numReadings][13]; // Array to store sensor readings


void setup() {
  Serial.begin(9600);
  
  if (!SD.begin(chipSelect)) {
    Serial.println(F("SD card initialization failed!"));
    return;
  }
  Serial.println(F("SD card initialized."));
  
  if (!htu.begin()) {
    Serial.println(F("HTU21D-F sensor initialization failed!"));
    return;
  }
  Serial.println(F("HTU21D-F sensor initialized."));

  if (!sox.begin_I2C()) {
    Serial.println(F("LSM6DSOX sensor initialization failed!"));
    return;
  }
  Serial.println(F("LSM6DSOX sensor initialized."));

  if (!lis3mdl.begin_I2C()) { 
    Serial.println(F("LIS3MDL sensor initialization failed!"));
    return;

  }
  Serial.println(F("LIS3MDL sensor initialized."));

  if(!dps.begin_I2C()){
    Serial.println(F("DPS sensor initilization failed"));
    return;
  }
  Serial.println(F("DPS sensor initialized!"));


  SD.remove("sensor_data3.csv");

  //air pressure precision
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps_pressure->printSensorDetails();




  // Create a new file
  File dataFile = SD.open("sensor_data3.csv", FILE_WRITE);
  if (dataFile) {
    // Write header
    dataFile.println("Timestamp,Humidity (%),Temperature (C),Accel X (m/s^2),Accel Y (m/s^2),Accel Z (m/s^2),Gyro X (rad/s),Gyro Y (rad/s),Gyro Z (rad/s),Mag X,Mag Y,Mag Z, Air Pressure (hPA)");

    // Close the file
    dataFile.close();
    Serial.println("CSV file created");
  } else {
    Serial.println("Error creating CSV file");
  }
}

void loop() {
  // Read sensor data
  float humidity = htu.readHumidity();
  float temperature = htu.readTemperature();
  // dps_pressure->getEvent(&pressure_event);

  // float pressure= pressure_event.pressure
  
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp2;
  sensors_event_t pressure_event;

  sox.getEvent(&accel, &gyro, &temp2);
  // dps.getEvents(&pressure_event);
  dps_pressure->getEvent(&pressure_event);


  
  lis3mdl.read();
  
  // Store sensor readings in the array
  static int index = 0;
  readings[index][0] = millis();
  readings[index][1] = humidity;
  readings[index][2] = temperature;
  readings[index][3] = accel.acceleration.x;
  readings[index][4] = accel.acceleration.y;
  readings[index][5] = accel.acceleration.z;
  readings[index][6] = gyro.gyro.x;
  readings[index][7] = gyro.gyro.y;
  readings[index][8] = gyro.gyro.z;
  readings[index][9] = lis3mdl.x;
  readings[index][10] = lis3mdl.y;
  readings[index][11] = lis3mdl.z;
  readings[index][12] = pressure_event.pressure;
  // readings[index][12]= pressure;

  
  // Increment index
  index++;
  if (index >= numReadings) {
    // Write sensor data to CSV file
    writeDataToSDCard();
    index = 0; 
  }
  
  delay(100); // Delay between readings
}

void writeDataToSDCard() {
  File dataFile = SD.open("sensor_data3.csv", FILE_WRITE);
  if (dataFile) {
    // Write sensor data to file
    for (int i = 0; i < numReadings; i++) {
      for (int j = 0; j < 13; j++) {
        dataFile.print(readings[i][j]);
        if (j < 12) {
          dataFile.print(",");
        } else {
          dataFile.println();
        }
      }
    }
    dataFile.close(); // Close the file
    Serial.println("Data written to CSV file");
  } else {
    Serial.println("Error writing to CSV file");
  }
}
