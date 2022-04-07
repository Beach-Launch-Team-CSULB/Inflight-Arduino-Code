#include "Sensor_struct.h"


String Sensor_struct::get_time() const {
    return String((unsigned long)(unix_time)) cm String(milliseconds) cm String(microseconds);
}

String Sensor_struct::get_time_hdr() {
    return "Unix time, Milliseconds, Microseconds";
}