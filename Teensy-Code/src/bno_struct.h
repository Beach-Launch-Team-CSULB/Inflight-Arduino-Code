//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BNO_STRUCT_H
#define TEENSY_CODE_BNO_STRUCT_H

#include "Sensor_struct.h"

struct bno_struct: Sensor_struct {
    String toString() override;
    uint8_t type{}; //
    float acceleration[3]{}, gyroscope[3]{}, magnetometer[3]{};
    bno_struct(const float accel[3], const float gyro[3], const float mag[3]);
    bno_struct() = default;
    String getHeader() const override;
};


#endif //TEENSY_CODE_BNO_STRUCT_H
