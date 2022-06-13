//
// Created by shawn on 3/18/22.
//

#include "bmp_struct.h"


String bmp_struct::toString() {
    // Returns time information, temperature, pressure, and altitude
    return get_time() cm flts(temperature) cm flts(pressure) cm flts(altitude);
}

bmp_struct::bmp_struct(double temp, double press, float alt) {
    // Sets all fields and uses current time as reading time
    temperature = temp;
    pressure = press;
    altitude = alt;
    setTime();
}

String bmp_struct::getHeader() const {
    //Returns time info + header info for temperature, pressure, and altitude
    return get_time_hdr() cm "Temperature, Pressure, Altitude";
}
