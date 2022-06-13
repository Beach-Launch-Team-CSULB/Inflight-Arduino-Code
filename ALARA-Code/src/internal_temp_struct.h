//
// Created by shawn on 3/20/22.
//

#ifndef TEENSY_CODE_INTERNAL_TEMP_STRUCT_H
#define TEENSY_CODE_INTERNAL_TEMP_STRUCT_H

#include "Sensor_struct.h"
struct internal_temp_struct: public Sensor_struct {
    /**
     * Generates string of the sensor data with the time and current temperature
     * @return Formatted string with sensor info
     */
    String toString() override;
    /**
     * Generates CSV string of sensor info
     */
    String getHeader() const override;
    /**
     * Temperature of the sensor
     */
    float temp{};
    /**
     * Generates an internal temp struct from the temperature
     */
    explicit internal_temp_struct(float temperature);
    /**
     * Default constructor of the struct
     */
    internal_temp_struct() = default;
};


#endif //TEENSY_CODE_INTERNAL_TEMP_STRUCT_H
