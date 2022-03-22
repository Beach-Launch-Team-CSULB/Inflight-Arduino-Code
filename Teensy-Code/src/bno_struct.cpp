//
// Created by shawn on 3/18/22.
//

#include "bno_struct.h"
#include "Utilities.h"

String bno_struct::toString() {
    return get_time() cm flts(acceleration[0]) cm flts(acceleration[1]) cm flts(acceleration[2])
    cm flts(gyroscope[0]) cm flts(gyroscope[1]) cm flts(gyroscope[2]) cm flts(magnetometer[0])
    cm flts(magnetometer[1]) cm flts(magnetometer[2]);
}

bno_struct::bno_struct(const float *accel, const float *gyro, const float *mag) {
    for(int_fast8_t i = 0; i < 3; i++) {
        acceleration[i] = accel[i];
        gyroscope[i] = gyro[i];
        magnetometer[i] = mag[i];
    }
    setTime();
}

String bno_struct::getHeader() const {
    return get_time_hdr() cm "Acceleration X, Acceleration Y, Acceleration Z, Gyroscope X, Gyroscope Y, Gyroscope Z, "
                             "Magnetometer X, Magnetometer Y, Magnetometer Z";
}
