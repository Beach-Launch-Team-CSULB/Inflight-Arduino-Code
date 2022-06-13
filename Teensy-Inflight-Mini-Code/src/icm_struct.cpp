//
// Created by shawn on 3/18/22.
//

#include "icm_struct.h"


String icm_struct::toString() {
    // Returns time, accel X/Y/Z, gyro X/Y/Z, and temp as formatted string
    return get_time() cm flts(acceleration[0]) cm flts(acceleration[1]) cm flts(acceleration[2]) cm
    flts(gyro[0]) cm flts(gyro[1]) cm flts(gyro[2]) cm flts(temperature);
}

icm_struct::icm_struct(sensors_event_t * icm_data) {
    // Uses current time for info
    setTime();
    // Acceleration info
    // Compensates for the orientation of the IMU relative to the rocket
    acceleration[0] = icm_data[0].acceleration.v[1];
    acceleration[1] = icm_data[0].acceleration.v[0] * -1;
    acceleration[2] = icm_data[0].acceleration.v[2];
    //Gyro info
    // Compensates for the orientation of the IMU relative to the rocket
    gyro[0] = icm_data[0].gyro.v[1];
    gyro[1] = icm_data[0].gyro.v[0] * -1;
    gyro[2] = icm_data[0].gyro.v[2];
    // Updates temp
    temperature = icm_data[2].temperature;
}

String icm_struct::getHeader() const {
    // ICM CSV header string
    return get_time_hdr() cm "Acceleration X, Acceleration Y, Acceleration Z, "
                             "Gyroscope X, Gyroscope Y, Gyroscope Z, Temperature";
}
