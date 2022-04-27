//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BNO_STRUCT_H
#define TEENSY_CODE_BNO_STRUCT_H

#include "Sensor_struct.h"
#include "vector.h"

struct bno_struct: Sensor_struct {
    double acceleration[3]{}, gyroscope[3]{}, magnetometer[3]{};
    int8_t temp{};
    bno_struct(imu::Vector<3> accel, imu::Vector<3> gyro, imu::Vector<3> mag, int8_t temp);
    bno_struct() = default;
    String getHeader() const override;
    String toString() override;
};


#endif //TEENSY_CODE_BNO_STRUCT_H
