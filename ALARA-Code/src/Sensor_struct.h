//
// Created by shawn on 3/15/22.
//

#ifndef TEENSY_CODE_SENSOR_STRUCT_H
#define TEENSY_CODE_SENSOR_STRUCT_H
#include <WString.h>
#include <core_pins.h>
#include <TimeLib.h>
#include "Utilities.h"

struct Sensor_struct {
    //These are functions, so they are not part of the struct and go in program code
    /**
     *Returns string data in CSV format
     *@return A string with the data in CSV format
     */
    virtual String toString() = 0;
    /**
     * CSV file header return operation
     * @return the CSV file header
     */
    virtual String getHeader() const = 0;
    /**
     * UNIX timestamp from Teensy, can get updated by GPS, computer time, or RTC
     * The time is stored using 16 bytes
     */
    time_t unix_time{}; //8 bytes
    /**
     * Milliseconds reading from millis()
     */
    uint32_t milliseconds{}; //4 bytes
    /**
     * Microseconds reading from micros();
     */
    uint32_t microseconds{}; //4 bytes
    /**
     * Sets the timestamps using the current time
     */
    void setTime() {
        unix_time = now();
        milliseconds = millis();
        microseconds = micros();
    }
    /**
     * Sets time manually for if an override is needed
     * @param unix_t the UNIX timestamp
     * @param millis the milliseconds value
     * @param micros the microseconds value
     */
    void setTime(time_t unix_t, uint32_t millis, uint32_t micros) {
        unix_time = unix_t;
        milliseconds = millis;
        microseconds = micros;
    }
    /**
     * Returns the current time as a string
     * @return the current time as a string
     */
    String get_time() const;
    /**
     * Returns a time header for creation of a CSV
     * @return A time header for creation of a CSV
     */
    static String get_time_hdr();
};



#endif //TEENSY_CODE_SENSOR_STRUCT_H
