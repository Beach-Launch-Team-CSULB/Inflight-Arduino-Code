#include "Sensor_struct.h"

String Sensor_struct::get_time() const {
    //Stringifies the UNIX time, milliseconds, and microseconds stored and writes
    return String((unsigned long)(unix_time)) cm String(milliseconds) cm String(microseconds);
}

String Sensor_struct::get_time_hdr() {
    //Writes header of CSV
    return "Unix time, Milliseconds, Microseconds";
}