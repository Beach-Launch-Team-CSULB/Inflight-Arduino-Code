//
// Created by shawn on 3/20/22.
//

#include "InternalTemp.h"
#include "Utilities.h"

void InternalTemp::updateData() {
    data_struct = internal_temp_struct(tempmonGetTemp());
    new_data_update();
}

InternalTemp::InternalTemp() :
Sensor("InternalTemp", &data_struct, sizeof(data_struct), 1) {
    enable = true;
}
