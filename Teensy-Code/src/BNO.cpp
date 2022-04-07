//
// Created by shawn on 3/18/22.
//

#include "BNO.h"
#include "Utilities.h"

void BNO::updateData() {
    if(enable) {
        sh2_SensorValue sensor_data{};
        // Records finding state of variables
        bool found_accel = false, found_gyro = false, found_mag = false;
        float accel[3];
        float gyro[3];
        float mag[3];
        // Reads sensor_data data until one of each sensor_data data point is collected
        while (!found_accel || !found_mag || !found_gyro) {
            // Gets sensor_data event from driver, stores in sensor_data
            sensor.getSensorEvent(&sensor_data);
            // Stores data point in variable, updates flag
            switch (sensor_data.sensorId) {
                case SENSOR_TYPE_ACCELEROMETER:
                    found_accel = true;
                    // Serial.print("Reading from accel:");
                    accel[0] = sensor_data.un.accelerometer.x;
                    accel[1] = sensor_data.un.accelerometer.y;
                    accel[2] = sensor_data.un.accelerometer.z;
                    break;
                case SENSOR_TYPE_MAGNETIC_FIELD:
                    found_mag = true;
                    // Serial.print("Reading from mag");
                    mag[0] = sensor_data.un.magneticField.x;
                    mag[1] = sensor_data.un.magneticField.y;
                    mag[2] = sensor_data.un.magneticField.z;
                    break;
                case SENSOR_TYPE_ORIENTATION:
                    found_gyro = true;
                    // Serial.print("Reading from gyro");
                    gyro[0] = sensor_data.un.gyroscope.x;
                    gyro[1] = sensor_data.un.gyroscope.y;
                    gyro[2] = sensor_data.un.gyroscope.z;
                    break;
                    // Catch all option
                default:
                    Serial.print("Sensor type " + String(sensor_data.sensorId) + " Not being used");
                    break;
            }
        }
        data_struct = bno_struct(accel, gyro, mag);
        new_data_update();
    }
}

BNO::BNO(uint8_t CS, int8_t RST, uint8_t INT) :
Sensor("BNO", &data_struct, sizeof(data_struct), 1) {
    name = "BNO";
    sensor = Adafruit_BNO08x(RST);
    enable = sensor.begin_SPI(CS, INT);
    sensor.enableReport(SH2_ACCELEROMETER);
    sensor.enableReport(SH2_GYROSCOPE_CALIBRATED);
    sensor.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED);
}