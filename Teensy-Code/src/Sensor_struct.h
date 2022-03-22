//
// Created by shawn on 3/15/22.
//

#ifndef TEENSY_CODE_SENSOR_STRUCT_H
#define TEENSY_CODE_SENSOR_STRUCT_H
#include <WString.h>
#include <core_pins.h>
#include <TimeLib.h>

struct Sensor_struct {
    virtual String toString() = 0;
    virtual String getHeader() const = 0;
    time_t unix_time{};
    uint32_t milliseconds{};
    uint32_t microseconds{};
    void setTime() {
        unix_time = now();
        milliseconds = millis();
        microseconds = micros();
    }
    void setTime(time_t unix_t, uint32_t millis, uint32_t micros) {
        unix_time = unix_t;
        milliseconds = millis;
        microseconds = micros;
    }
    String get_time() const;
    static String get_time_hdr();
};



#endif //TEENSY_CODE_SENSOR_STRUCT_H
