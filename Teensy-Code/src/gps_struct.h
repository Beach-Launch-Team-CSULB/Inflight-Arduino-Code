//
// Created by shawn on 3/17/22.
//

#ifndef TEENSY_CODE_GPS_STRUCT_H
#define TEENSY_CODE_GPS_STRUCT_H

#include "Sensor_struct.h"
#include <TinyGPS++.h>

struct gps_struct: Sensor_struct
{
    explicit gps_struct(TinyGPSPlus gps);
    // TESTING - KNOWN BUG - GPS MODULE HAS NOT GOTTEN A LOCATION SYNC
    gps_struct() = default;
    // Updates GPS data points
    double gps_altitude{};
    uint16_t gps_year{};
    uint8_t gps_month{};
    uint8_t gps_day{};
    uint8_t gps_hour{};
    uint8_t gps_minute{};
    uint8_t gps_second{};
    double gps_latitude{};
    double gps_longitude{};
    int32_t gps_speed{};
    double gps_hdop{};
    double gps_course_deg{};
    uint32_t gps_reading_age{};
    String toString() override;
    String getHeader() const override;
};


#endif //TEENSY_CODE_GPS_STRUCT_H
