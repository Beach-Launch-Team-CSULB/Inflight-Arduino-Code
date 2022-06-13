//
// Created by shawn on 4/22/22.
//

#include "MS5607Sensor.h"

MS5607Sensor::MS5607Sensor() : Sensor("MS5607", &data_struct, sizeof(data_struct), 1) {
    // The sensor object doesn't have a good logical way to determine if the sensor is working, so just assume it does
    enable = true;
    Serial.println("Attempting to initialize MS507");
    // Uses sensor's builtin init function to initialize sensor
    sensor.init();
    Serial.println("Initialization finished");
}

void MS5607Sensor::updateData() {
    // Use sensor libraries to get temperature, pressure, and altitude, store so they can be used to build a struct
    uint32_t a = sensor.get_altitude();
    uint32_t p = sensor.get_pressure();
    int32_t t = sensor.get_temp();
    // Wrap data into sensor struct
    data_struct = ms5607_struct(t, p, a);
    // Update data with new struct
    new_data_update(&data_deque, data_struct);
}
