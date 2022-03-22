//
// Created by shawn on 3/18/22.
//

#include "bmp_struct.h"
#include "Utilities.h"

String bmp_struct::toString() {
    return get_time() cm flts(temperature) cm flts(pressure) cm flts(altitude);
}

bmp_struct::bmp_struct(double temp, double press, float alt) {
    temperature = temp;
    pressure = press;
    altitude = alt;
    setTime();
}

String bmp_struct::getHeader() const {
    return get_time_hdr() cm "Temperature, Pressure, Altitude";
}
