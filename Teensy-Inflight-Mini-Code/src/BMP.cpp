//
// Created by shawn on 3/18/22.
//

#include "BMP.h"


void BMP::updateData() {
    // Only do stuff if the sensor works
    if(enable) {
        // Reads altitude from sensor using sea level pressure
        float alt = sensor.readAltitude(sea_level_pressure_hpa);
        //Turns data into struct
        data_struct = bmp_struct(sensor.temperature, sensor.pressure, alt);
        // Updates data into data_deque from struct
        new_data_update(&data_deque, data_struct);
        // Check if the SLP Reset is enabled, and if the time difference is over 50 seconds
        if(slp_reset && ((now() - last_ground_altitude_update) > 50)) {
            // Read pressure into sea level pressure
            sea_level_pressure_hpa = (float)(sensor.pressure * 0.01);
            // Update last update time
            last_ground_altitude_update = now();
        }
    }
}

BMP::BMP(uint8_t CS, uint8_t INT) : Sensor("BMP", &data_struct, sizeof(data_struct), 1) {
    // Sets SLP reset to true by default because sensor will be on ground
    slp_reset = true;
    // starts sensor, sets enable
    enable = sensor.begin_SPI(CS);
    // Sets interrupt pin
    BMP_INT = INT;
    // Sets sensor settings - highest data rate possible
    sensor.setOutputDataRate(BMP3_ODR_200_HZ);
    // Sets oversampling rates to 32x- will return results more quickly/more results
    sensor.setPressureOversampling(BMP3_OVERSAMPLING_32X);
    sensor.setTemperatureOversampling(BMP3_OVERSAMPLING_32X);
    // Sets filter coefficient - may want to research this more
    sensor.setIIRFilterCoeff(BMP3_IIR_FILTER_DISABLE);
    // Sets default sea level pressure
    sea_level_pressure_hpa = (float)(sensor.readPressure() * 0.01);
    last_ground_altitude_update = now();
    //Reset ground pressure occasionally
}

void BMP::reset_slp(bool set) {
    slp_reset = set;
}
