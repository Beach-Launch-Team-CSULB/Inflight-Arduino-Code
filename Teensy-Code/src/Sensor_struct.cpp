#include "Sensor_struct.h"
#include "Utilities.h"

String Sensor_struct::get_time() const {
    return String(unix_time) cm milliseconds cm microseconds;
}

String Sensor_struct::get_time_hdr() {
    return "Unix time, Milliseconds, Microseconds";
}