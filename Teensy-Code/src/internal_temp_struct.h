//
// Created by shawn on 3/20/22.
//

#ifndef TEENSY_CODE_INTERNAL_TEMP_STRUCT_H
#define TEENSY_CODE_INTERNAL_TEMP_STRUCT_H

#include "Sensor_struct.h"
struct internal_temp_struct: public Sensor_struct {
    String toString() override;
    String getHeader() const override;
    float temp{};
    explicit internal_temp_struct(float temperature);
    internal_temp_struct() = default;
};


#endif //TEENSY_CODE_INTERNAL_TEMP_STRUCT_H
