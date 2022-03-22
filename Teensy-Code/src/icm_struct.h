//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_ICM_STRUCT_H
#define TEENSY_CODE_ICM_STRUCT_H

#include "Sensor_struct.h"
#include <Adafruit_Sensor.h>
struct icm_struct: Sensor_struct {
    String toString() override;
    String getHeader() const override;
    float temperature{};
    float gyro[3]{};
    float acceleration[3]{};
    explicit icm_struct(sensors_event_t * icm_data);
    icm_struct() = default;
};


#endif //TEENSY_CODE_ICM_STRUCT_H
