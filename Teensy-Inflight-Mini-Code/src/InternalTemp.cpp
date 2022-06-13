//
// Created by shawn on 3/20/22.
//

#include "InternalTemp.h"


void InternalTemp::updateData() {
    // Creates new internal temp struct, uses NXP SDK function for temperature
    data_struct = internal_temp_struct(tempmonGetTemp());
    // Updates data deque with new temp struct element
    new_data_update(&data_deque, data_struct);
}

InternalTemp::InternalTemp() :
// Instantiates Sensor base class, uses struct, update rate of 1
Sensor("InternalTemp", &data_struct, sizeof(data_struct), 1) {
    enable = true;
}
