//
// Created by shawn on 3/17/22.
//

#ifndef TEENSY_CODE_GPS_STRUCT_H
#define TEENSY_CODE_GPS_STRUCT_H

#include "Sensor_struct.h"
#include <TinyGPS++.h>

struct gps_struct: Sensor_struct
{
    /**
     * Creates a new GPS struct, pulls necessary data from a TinyGPSPlus parser
     */
    explicit gps_struct(TinyGPSPlus gps);
    // TESTING - KNOWN BUG - GPS MODULE HAS NOT GOTTEN A LOCATION SYNC
    /**
     * Default constructor for a GPS struct
     */
    gps_struct() = default;
    // Updates GPS data points
    /**
     * GPS altitude stored as double precision float
     */
    double gps_altitude{};
    /**
     * Current year, stored as uint16 to support over 60000 years
     */
    uint16_t gps_year{};
    /**
     * Current month, stored from 0-12
     */
    uint8_t gps_month{};
    /**
     * Current day from 1-31 stored as uint8
     */
    uint8_t gps_day{};
    /**
     * Current hour from 1-24 stored as uint8
     */
    uint8_t gps_hour{};
    /**
     * Current minute from 0-59
     */
    uint8_t gps_minute{};
    /**
     * Current second from 0-59
     */
    uint8_t gps_second{};
    /**
     * Current latitude, stored as dual precision float
     */
    double gps_latitude{};
    /**
     * Current longitude, stored as dual precision float
     */
    double gps_longitude{};
    /**
     * Current speed in m/s
     */
    int32_t gps_speed{};
    /**
     * High Degree of Precision value
     */
    double gps_hdop{};
    /**
     * Course degree of the GPS
     */
    double gps_course_deg{};
    /**
     * Age of the GPS reading
     */
    uint32_t gps_reading_age{};
    /**
     * Overridden toString with GPS reading info
     */
    String toString() override;
    /**
     * Overridden header with GPS header info
     */
    String getHeader() const override;
};


#endif //TEENSY_CODE_GPS_STRUCT_H
