//
// Created by shawn on 3/20/22.
//

#include "internal_temp_struct.h"


String internal_temp_struct::toString() {
    // Concatenates the time and the temperature
    return get_time() cm flts(temp);
}

String internal_temp_struct::getHeader() const {
    // Adds a temperature to the CSV header
    return get_time_hdr() cm "Temperature";
}

internal_temp_struct::internal_temp_struct(float temperature) {
    temp = temperature;
    // Uses current time for temp reading
    setTime();
}
