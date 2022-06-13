//
// Created by shawn on 3/20/22.
//

#include "InternalTemp.h"
#include "InternalTemperature.h"

void InternalTemp::updateData() {
    // Generates new data struct, uses InternalTemp class to do so
    // If anyone makes a less bad way of reading the internal temperature, do so here
    data_struct = internal_temp_struct(InternalTemperatureClass::readTemperatureC());
    // Generates new data struct into deque
    new_data_update(&data_deque, data_struct);
}

InternalTemp::InternalTemp() :
Sensor("InternalTemp", &data_struct, sizeof(data_struct), 1) {
    // enables sensor since if you have a functioning ALARA you can read the internal temp
    enable = true;
}
