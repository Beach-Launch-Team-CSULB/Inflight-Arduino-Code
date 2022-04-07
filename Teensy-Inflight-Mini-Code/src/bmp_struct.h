//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BMP_STRUCT_H
#define TEENSY_CODE_BMP_STRUCT_H

#include "Sensor_struct.h"
#include <WString.h>

struct bmp_struct: Sensor_struct {
    String toString() override;
    double temperature{}, pressure{};
    float altitude{};
    bmp_struct(double temp, double press, float alt);
    bmp_struct() = default;
    String getHeader() const override;
};


#endif //TEENSY_CODE_BMP_STRUCT_H
