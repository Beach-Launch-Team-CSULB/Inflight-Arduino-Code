//
// Created by shawn on 4/22/22.
//

#ifndef ALARA_CODE_MS5607_STRUCT_H
#define ALARA_CODE_MS5607_STRUCT_H

#include "Sensor_struct.h"

struct ms5607_struct : Sensor_struct {
    String toString() override;
    //Returns CSV file header
    String getHeader() const override;
    ms5607_struct(int32_t temperature, uint32_t pressure, uint32_t altitude);
    ms5607_struct() = default;
    int32_t temp{};
    uint32_t pres{},  alt{};
};


#endif //ALARA_CODE_MS5607_STRUCT_H
