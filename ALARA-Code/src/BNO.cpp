//
// Created by shawn on 3/18/22.
//

#include "BNO.h"

void BNO::updateData() {
    if(enable) {
        imu::Vector<3> accel = sensor.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        imu::Vector<3> gyro = sensor.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        imu::Vector<3> mag = sensor.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        int8_t temp = sensor.getTemp();
        data_struct = bno_struct(accel, gyro, mag, temp);
        new_data_update(&data_deque, data_struct);
    }
}

BNO::BNO() :
Sensor("BNO", &data_struct, sizeof(data_struct), 1) {
    name = "BNO";
    sensor = Adafruit_BNO055();
    enable = sensor.begin();
}