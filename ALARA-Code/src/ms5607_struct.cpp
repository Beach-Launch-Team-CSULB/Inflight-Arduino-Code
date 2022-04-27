//
// Created by shawn on 4/22/22.
//

#include "ms5607_struct.h"

String ms5607_struct::toString() {
    return get_time() cm temp cm pres cm alt;
}

String ms5607_struct::getHeader() const {
    return get_time_hdr() + ", Temperature, Pressure, Altitude";
}

ms5607_struct::ms5607_struct(int32_t temperature, uint32_t pressure, uint32_t altitude) {
    temp = temperature;
    pres = pressure;
    alt = altitude;
    setTime();
}
