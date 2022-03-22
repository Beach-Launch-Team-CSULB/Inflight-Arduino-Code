//
// Created by shawn on 3/20/22.
//

#include "InternalTemp.h"
#include "Utilities.h"

void InternalTemp::updateData() {
    data_struct =
            internal_temp_struct(InternalTemperatureClass::InternalTemperatureClass::readTemperatureC());
    out_file->write(&data_struct, sizeof(data_struct));
    counter += sizeof(data_struct);
    if(counter > packet_size) {
        counter = 0;
        out_file->flush();
    }
}

float InternalTemp::getTemp() const {
    return data_struct.temp;
}

InternalTemp::InternalTemp() :
Sensor("InternalTemp", &data_struct, sizeof(data_struct)) {
    enable = true;
}
