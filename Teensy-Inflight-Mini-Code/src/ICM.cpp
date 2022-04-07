//
// Created by shawn on 3/18/22.
//

#include "ICM.h"


void ICM::updateData() {
    if(enable) {
        for (int_fast8_t i = 0; i < update_rate; i++) {
            sensors_event_t icm_reading[3];
            sensor.getEvent(&icm_reading[0], &icm_reading[1], &icm_reading[2]);
            data_struct = icm_struct(icm_reading);
            new_data_update(&data_deque, data_struct);
        }
    }
}

ICM::ICM(uint8_t CS) : Sensor("ICM", &data_struct, sizeof(data_struct), icm_update_rate) {
    name = "ICM";
    enable = sensor.begin_SPI(CS);
    sensor.setAccelRange(ICM20948_ACCEL_RANGE_16_G);
    sensor.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);
    // Sets output data rate to max supported by sensor
    sensor.setAccelRateDivisor(0);
    // Accel rate divisor is 1125 / (1 + divisor)
    sensor.setGyroRateDivisor(0);
    // Accel rate divisor is 1100 / (1 + divisor)
}
