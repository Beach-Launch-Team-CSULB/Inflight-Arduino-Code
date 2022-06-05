//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BMP_STRUCT_H
#define TEENSY_CODE_BMP_STRUCT_H

#include "Sensor_struct.h"
#include <WString.h>

struct bmp_struct: Sensor_struct {
    /**
     * Overridden from Sensor_struct
     * Returns the temperature, pressure, altitude, and reading time as a string
     */
    String toString() override;
    /**
     * The temperature of the sensor
     */
    double temperature{};
    /**
     * The air pressure near the sensor
     */
    double pressure{};
    /**
     * The calculated altitude of the sensor using a recorded sea level pressure and a barometric calculation
     */
    float altitude{};
    /**
     * Constructor for the bmp_struct, used by the BMP Sensor class to pack data
     * @param temp Temperature of the sensor
     * @param press Air Pressure near the sensor
     * @param alt Altitude of the sensor
     */
    bmp_struct(double temp, double press, float alt);
    /**
     * Default constructor, used to make the struct a data structure usable within certain other data structures
     */
    bmp_struct() = default;
    /**
     * Generates the CSV header for this sensor
     * @return the CSV formatted header for this sensor
     */
    String getHeader() const override;
};


#endif //TEENSY_CODE_BMP_STRUCT_H
