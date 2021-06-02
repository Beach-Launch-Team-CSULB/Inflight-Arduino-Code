// Basic demo for readings from Adafruit BNO08x
#include <Adafruit_BNO08x.h>

// For SPI mode, we also need a RESET
// but not for I2C or UART
const int8_t BNO08X_RESET = -1;

Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

// Here is where you define the sensor outputs you want to receive
void setReports() {
    Serial.println("Setting desired reports");
    if (! bno08x.enableReport(SH2_ACCELEROMETER, 1)) {
        Serial.println("Could not enable accelerometer");
    }
    if (! bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, 1)) {
        Serial.println("Could not enable gyroscope");
    }
    if (! bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, 1)) {
        Serial.println("Could not enable ");
    }
}


void setup(void) {
    Serial.begin(115200);
    while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens
    Serial.println("Adafruit BNO08x test!");

    // Try to initialize!
    bool f = false;
    while(!bno08x.begin_I2C()) {
        if(!f) {
            Serial.println("Failed to find BNO08x chip");
            f = true;
        }
    }
    Serial.println("BNO08x Found!");

    for (int n = 0; n < bno08x.prodIds.numEntries; n++) {
        Serial.print("Part ");
        Serial.print(bno08x.prodIds.entry[n].swPartNumber);
        Serial.print(": Version :");
        Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
        Serial.print(" Build ");
        Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
    }

    setReports();

    Serial.println("Reading events");
    delay(100);
}



void loop() {
    delay(10);

    if (bno08x.wasReset()) {
        Serial.print("sensor was reset ");
        setReports();
    }

    while(!bno08x.getSensorEvent(&sensorValue));

    switch (sensorValue.sensorId) {

        case SH2_ACCELEROMETER:
            Serial.print("Accelerometer:\nX: ");
            Serial.print(sensorValue.un.accelerometer.x);
            Serial.print(" Y: ");
            Serial.print(sensorValue.un.accelerometer.y);
            Serial.print(" Z: ");
            Serial.println(sensorValue.un.accelerometer.z);
            break;
        case SH2_GYROSCOPE_CALIBRATED:
            Serial.print("Gyroscope:\nX: ");
            Serial.print(sensorValue.un.gyroscope.x);
            Serial.print(" Y: ");
            Serial.print(sensorValue.un.gyroscope.y);
            Serial.print(" Z: ");
            Serial.println(sensorValue.un.gyroscope.z);
        case SH2_MAGNETIC_FIELD_CALIBRATED:
            Serial.print("Magnetometer: \nX");
            Serial.print(sensorValue.un.magneticField.x);
            Serial.print(" Y: ");
            Serial.print(sensorValue.un.magneticField.y);
            Serial.print(" Z: ");
            Serial.println(sensorValue.un.magneticField.z);
    }

}