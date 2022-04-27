//
// Created by shawn on 3/20/22.
//

#include "internal_temp_struct.h"

String internal_temp_struct::toString() {
    return get_time() cm flts(temp);
}

String internal_temp_struct::getHeader() const {
    return get_time_hdr() cm "Temperature";
}

internal_temp_struct::internal_temp_struct(float temperature) {
    temp = temperature;
    setTime();
}
