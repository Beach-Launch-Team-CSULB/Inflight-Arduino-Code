//
// Created by shawn on 3/17/22.
//

#include "gps_struct.h"

String gps_struct::toString() {
    // Returns all relevant GPS data points in correct order
    return get_time() cm flts(gps_altitude) cm gps_year cm gps_month cm gps_day cm gps_hour cm gps_minute
    cm gps_second cm flts(gps_latitude) cm flts(gps_longitude) cm gps_speed cm flts(gps_hdop)
    cm flts(gps_course_deg) cm gps_reading_age;
}

String gps_struct::getHeader() const {
    // Returns all header info
    return get_time_hdr() cm "GPS Altitude, GPS Year, GPS Month, GPS Day, GPS Hour, GPS Minute, GPS Second, "
                             "GPS Latitude, GPS Longitude, GPS HDOP, GPS Course Deg, GPS Reading Age";
}

gps_struct::gps_struct(TinyGPSPlus gps) {
        // Updates GPS data points
        gps_altitude = gps.altitude.meters();
        gps_year = gps.date.year();
        gps_month = gps.date.month();
        gps_day = gps.date.day();
        gps_hour = gps.time.hour();
        gps_minute = gps.time.minute();
        gps_second = gps.time.second();
        gps_latitude = gps.location.lat();
        gps_longitude = gps.location.lng();
        gps_speed = gps.speed.value();
        gps_hdop = gps.hdop.hdop();
        gps_course_deg = gps.course.deg();
        gps_reading_age = gps.time.age();
        setTime();
}
