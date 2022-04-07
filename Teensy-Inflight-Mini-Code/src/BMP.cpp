//
// Created by shawn on 3/18/22.
//

#include "BMP.h"


void BMP::updateData() {
    if(enable) {
        float alt = sensor.readAltitude(sea_level_pressure_hpa);
        data_struct = bmp_struct(sensor.temperature, sensor.pressure, alt);
        new_data_update(&data_deque, data_struct);
        if(slp_reset && ((now() - last_ground_altitude_update) > 50)) {
            sea_level_pressure_hpa = (float)(sensor.pressure * 0.01);
            last_ground_altitude_update = now();
        }
    }
}

BMP::BMP(uint8_t CS, uint8_t INT) : Sensor("BMP", &data_struct, sizeof(data_struct), 1) {
    name = "BMP";
    slp_reset = true;
    enable = sensor.begin_SPI(CS);
    BMP_INT = INT;
    sensor.setOutputDataRate(BMP3_ODR_200_HZ);
    // Sets oversampling rates to 32x- will return results more quickly/more results
    sensor.setPressureOversampling(BMP3_OVERSAMPLING_32X);
    sensor.setTemperatureOversampling(BMP3_OVERSAMPLING_32X);
    // Sets filter coefficient
    sensor.setIIRFilterCoeff(BMP3_IIR_FILTER_DISABLE);
    sea_level_pressure_hpa = (float)(sensor.readPressure() * 0.01);
    last_ground_altitude_update = now();
    //Reset ground pressure occasionally
}

void BMP::reset_slp(bool set) {
    slp_reset = set;
}
