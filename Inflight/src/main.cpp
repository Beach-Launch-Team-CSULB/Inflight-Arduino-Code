// Basic demo for accelerometer readings from Adafruit ICM20649


#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_BNO08x.h>
//#include <SD.h>


using namespace std;

Adafruit_ICM20649 icm;
Adafruit_BMP3XX bmp;
Adafruit_BNO08x bno;

//const float numMinutes = 0.01;
const float seaLevelPressure = 1019;
//const uint8_t cs_SD = 1;
//const uint16_t measurement_delay_us = 65535;
const uint8_t numSensorsEnabled = 3;

sensors_event_t events[3];
sh2_SensorValue_t bnoSensorValue;
bool collectedReadings[numSensorsEnabled];
uint16_t numReadings;

struct bnoReading {
    sh2_Gyroscope_t gyroscope;
    sh2_Accelerometer_t accelerometer;
    sh2_MagneticField_t magnetometer;
};
bnoReading bnoData;

void checkReading(uint8_t i) {
    if(!collectedReadings[i]) {
        collectedReadings[i] = true;
        numReadings++;
    }
}

void setup(void) {
    Serial.begin(115200);
    while (!Serial)
        delay(10);

    Serial.println("Begin data logging");

    // Try to initialize!
    if (!icm.begin_I2C()) {

        Serial.println("Failed to find ICM20649 over I2C");
        while (1) {
            delay(10);
        }
    }

    Serial.println("ICM20649 Found");
    if (!bmp.begin_I2C()) {

        Serial.println("Failed to find BMP390L over I2C");
        while (1) {
            delay(10);
        }
    }
    Serial.println("BMP390L Found");

    if (!bmp.begin_I2C()) {

        Serial.println("Failed to find BNO085 over I2C");
        while (1) {
            delay(10);
        }
    }
    Serial.println("BNO085 Found");
/*
    if(!SD.begin(cs_SD)) {
        Serial.println("Failed to find SD card over SPI");
        while (1) {
            delay(10);
        }
    }
    Serial.println("SD card found");
*/

    //ICM Accel settings
    icm.setAccelRange(ICM20649_ACCEL_RANGE_30_G);
    icm.setAccelRateDivisor(0);
    icm.enableAccelDLPF(true, ICM20X_ACCEL_FREQ_246_0_HZ);

    //ICM Gyro settings
    icm.setGyroRange(ICM20649_GYRO_RANGE_4000_DPS);
    icm.setGyroRateDivisor(0);
    icm.enableGyrolDLPF(true, ICM20X_GYRO_FREQ_196_6_HZ);

    Serial.println("Set ICM characteristics");

    //BMP390L settings
    bmp.setTemperatureOversampling(1);
    bmp.setPressureOversampling(1);
    bmp.setPressureOversampling(1);
    bmp.setIIRFilterCoeff(128);
    bmp.setOutputDataRate(BMP3_ODR_200_HZ);
    //Sea Level pressure set at top of program

    Serial.println("Set BMP390L characteristics");

    //BNO085 settings
    //sh2_SensorId_e setSensors [numSensorsEnabled] = {SH2_GYROSCOPE_CALIBRATED, SH2_ACCELEROMETER, SH2_MAGNETIC_FIELD_CALIBRATED};
    /*
    if(!bno.enableReport(SH2_ACCELEROMETER)) {
        Serial.print("Failed to enable accelerometer");
    }
    else {
        Serial.print("Enabled gyroscope");
    }
    if(!bno.enableReport(SH2_GYROSCOPE_CALIBRATED)) {
        Serial.print("Failed to enable gyroscope");
    }
    else {
        Serial.print("Enabled gyroscope");
    }

    if(!bno.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED)) {
        Serial.print("Failed to enable magnetometer");
    }
    else {
        Serial.print("Enabled magnetometer");
    }*/

}

void loop() {
    Serial.println("Begin loop");
    if(!icm.getEvent(&events[0], &events[1], &events[2])) {
        Serial.print("ICM20649 failed to perform reading");
    }
    else {
        Serial.print("ICM20649 performed reading");
    }
    if (! bmp.performReading()) {
        Serial.println("BMP390L Failed to perform reading");
    }
    else {
        Serial.println("BMP390L performed reading");
    }

    numReadings = 0;
    while(numReadings < numSensorsEnabled) {
        if(!bno.getSensorEvent(&bnoSensorValue)) {
            Serial.print("BNO085 failed to perform reading");
        }
        Serial.print(bnoSensorValue.sensorId);
        switch (bnoSensorValue.sensorId) {
            case SH2_RAW_GYROSCOPE:
                bnoData.gyroscope = bnoSensorValue.un.gyroscope;
                checkReading(0);
                break;

            case SH2_RAW_ACCELEROMETER:
                bnoData.accelerometer = bnoSensorValue.un.accelerometer;
                checkReading(1);
                break;

            case SH2_RAW_MAGNETOMETER:
                bnoData.magnetometer = bnoSensorValue.un.magneticField;
                checkReading(2);
                break;

        }

    Serial.print("\t\tTemperature: ");
    Serial.print(events[2].temperature);
    Serial.println(" deg C");
    Serial.print("\t\tAccel X: ");
    Serial.print(events[0].acceleration.x);
    Serial.print(" \tY: ");
    Serial.print(events[0].acceleration.y);
    Serial.print(" \tZ: ");
    Serial.print(events[0].acceleration.z);
    Serial.println(" m/s^2 ");
    Serial.print("\t\tGyro X: ");
    Serial.print(events[1].gyro.x);
    Serial.print(" \tY: ");
    Serial.print(events[1].gyro.y);
    Serial.print(" \tZ: ");
    Serial.print(events[1].gyro.z);
    Serial.println(" radians/s ");
    Serial.println();
    delay(100);
    Serial.print("Temperature = ");
    Serial.print(bmp.temperature);
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bmp.pressure / 100.0);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bmp.readAltitude(seaLevelPressure));
    Serial.println(" m");


    //TODO: Add sensor prints here for BNO085
    Serial.print("BNO085 Acceleration\nX:");
    Serial.print(bnoData.accelerometer.x);
    Serial.print(" Y: ");
    Serial.print(bnoData.accelerometer.y);
    Serial.print(" Z: ");
    Serial.println(bnoData.accelerometer.z);
    Serial.println("BNO085 Gyroscope\nX: ");
    Serial.print(bnoData.gyroscope.x);
    Serial.print(" Y: ");
    Serial.print(bnoData.gyroscope.y);
    Serial.print(" Z: ");
    Serial.println(bnoData.gyroscope.z);
    Serial.println("BNO085 Magnetometer\nX: ");
    Serial.print(bnoData.magnetometer.x);
    Serial.println(" Y: ");
    Serial.print(bnoData.magnetometer.y);
    Serial.println(" Z: ");
    Serial.print(bnoData.magnetometer.z);

    delay(100);
    }
}


