//
// Created by shawn on 4/22/22.
//

#include "MS5607Sensor.h"

MS5607Sensor::MS5607Sensor() : Sensor("MS5607", &data_struct, sizeof(data_struct), 1) {
enable = true;
Serial.println("Attempting to initialize MS507");
sensor.init();
Serial.println("Initialization finished");
}

void MS5607Sensor::updateData() {
uint32_t a = sensor.get_altitude();
uint32_t p = sensor.get_pressure();
int32_t t = sensor.get_temp();
data_struct = ms5607_struct(t, p, a);
new_data_update(&data_deque, data_struct);
}
