//
// Created by shawn on 3/18/22.
//

#include "icm_struct.h"
#include "Utilities.h"

String icm_struct::toString() {
    return get_time() cm flts(acceleration[0]) cm flts(acceleration[1]) cm flts(acceleration[2]) cm
    flts(gyro[0]) cm flts(gyro[1]) cm flts(gyro[2]) cm flts(temperature);
}

icm_struct::icm_struct(sensors_event_t * icm_data) {
    setTime();
    for(int_fast8_t i = 0; i < 3; i++) {
        acceleration[i] = icm_data[0].acceleration.v[i];
        gyro[i] = icm_data[1].gyro.v[i];
    }
    temperature = icm_data[2].temperature;
}

String icm_struct::getHeader() const {
    return get_time_hdr() cm "Acceleration X, Acceleration Y, Acceleration Z, "
                             "Gyroscope X, Gyroscope Y, Gyroscope Z, Temperature";
}
