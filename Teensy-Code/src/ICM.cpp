//
// Created by shawn on 3/18/22.
//

#include "ICM.h"
#include "Utilities.h"

void ICM::updateData() {
    if(enable) {
        for (int_fast8_t i = 0; i < update_rate; i++) {
            sensors_event_t icm_reading[3];
            sensor.getEvent(&icm_reading[0], &icm_reading[1], &icm_reading[2]);
            data_struct = icm_struct(icm_reading);
            new_data_update();
            state_detection.addIMUData(data_struct);
        }
    }
}

ICM::ICM(uint_fast8_t CS, uint_fast8_t int1,
         uint_fast8_t int2) : Sensor("ICM", &data_struct, sizeof(data_struct), 7) {
    name = "ICM";
    INT_1 = int1;
    INT_2 = int2;
    enable = sensor.begin_SPI(CS);
    sensor.setAccelRange(ICM20649_ACCEL_RANGE_30_G);
    sensor.setGyroRange(ICM20649_GYRO_RANGE_4000_DPS);
    // Sets output data rate to max supported by sensor
    sensor.setAccelRateDivisor(0);
    sensor.setGyroRateDivisor(0);
}
