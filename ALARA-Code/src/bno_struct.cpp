//
// Created by shawn on 3/18/22.
//

#include "bno_struct.h"

String bno_struct::toString() {
    return get_time() cm flts(acceleration[0]) cm flts(acceleration[1]) cm flts(acceleration[2])
    cm flts(gyroscope[0]) cm flts(gyroscope[1]) cm flts(gyroscope[2]) cm flts(magnetometer[0])
    cm flts(magnetometer[1]) cm flts(magnetometer[2]);
}

String bno_struct::getHeader() const {
    return get_time_hdr() cm "Acceleration X, Acceleration Y, Acceleration Z, Gyroscope X, Gyroscope Y, Gyroscope Z, "
                             "Magnetometer X, Magnetometer Y, Magnetometer Z";
}

bno_struct::bno_struct(imu::Vector<3> accel, imu::Vector<3> gyro, imu::Vector<3> mag, int8_t temperature) {
    for(int_fast8_t i = 0; i < 3; i++) {
        acceleration[i] = accel[i];
        gyroscope[i] = gyro[i];
        magnetometer[i] = mag[i];
    }
    temp = temperature;
    setTime();
}
