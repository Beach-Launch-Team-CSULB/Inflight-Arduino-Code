//
// Created by shawn on 3/18/22.
//

#include "BNO.h"

void BNO::updateData() {
    if(enable) {
        // imu::Vector<> is Adafruit's version of a C++ vector but worse
        // Grab the value from the library using the library data format, store in the most accessible format to send to
        // data struct
        /**
         * acceleration in BNO055 library device format
         */
        imu::Vector<3> accel = sensor.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        /**
         * gyroscope reading in BNO055 library device format
         */
        imu::Vector<3> gyro = sensor.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        /**
         * magnetometer position in BNO055 library device format
         */
        imu::Vector<3> mag = sensor.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        /**
         * Temperature from 0-255
         */
        int8_t temp = sensor.getTemp();
        // Store into data struct
        data_struct = bno_struct(accel, gyro, mag, temp);
        // Update deque from new data
        new_data_update(&data_deque, data_struct);
    }
}

BNO::BNO() :
Sensor("BNO", &data_struct, sizeof(data_struct), 1) {
    // Initialize sensor from library
    // May be able to remove this line
    sensor = Adafruit_BNO055();
    // Begin sensor
    enable = sensor.begin();
}