//
// Created by shawn on 3/20/22.
//

#include "InternalTemp.h"
#include "InternalTemperature.h"

void InternalTemp::updateData() {
    data_struct = internal_temp_struct(InternalTemperatureClass::readTemperatureC());
    new_data_update(&data_deque, data_struct);
}

InternalTemp::InternalTemp() :
Sensor("InternalTemp", &data_struct, sizeof(data_struct), 1) {
    enable = true;
}
