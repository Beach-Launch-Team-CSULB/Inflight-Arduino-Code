//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BNO_STRUCT_H
#define TEENSY_CODE_BNO_STRUCT_H

#include "Sensor_struct.h"
#include "vector.h"

struct bno_struct: Sensor_struct {
    /**
     * Acceleration readings from sensor in m/s/s
     */
    double acceleration[3]{};
    /**
     * Gyroscope readings from sensor in dps
     */
    double gyroscope[3]{};
    /**
     * Absolute orientation of sensor relative to Earth
     */
    double magnetometer[3]{};
    /**
     * Temperature of the sensor
     */
    int8_t temp{};
    /**
     * Creates new BNO struct from sensor data
     * @param accel Acceleration in proper format for sensor data type
     * @param gyro Gyroscope data in BNO055 library data format
     * @param mag The magnetometer data in BNO055 library data format
     * @param temp The temperature of the BNO085
     */
    bno_struct(imu::Vector<3> accel, imu::Vector<3> gyro, imu::Vector<3> mag, int8_t temp);
    /**
     * Default constructor for the BNO055 struct
     */
    bno_struct() = default;
    /**
     * Returns the header for the accel/gyro/mag/temp string
     * @return Formatted string with the sensor info header
     */
    String getHeader() const override;
    /**
     * Returns the accel/gyro/mag/temp string
     * @return Formatted string with the sensor info
     */
    String toString() override;
};


#endif //TEENSY_CODE_BNO_STRUCT_H
