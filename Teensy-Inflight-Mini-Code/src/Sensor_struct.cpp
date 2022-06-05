#include "Sensor_struct.h"

//Stringifies the UNIX time, milliseconds, and microseconds stored and writes
String Sensor_struct::get_time() const {
    return String((unsigned long)(unix_time)) cm String(milliseconds) cm String(microseconds);
}

//Writes header of CSV
String Sensor_struct::get_time_hdr() {
    return "Unix time, Milliseconds, Microseconds";
}