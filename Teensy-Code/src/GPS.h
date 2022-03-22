//
// Created by shawn on 3/17/22.
//

#ifndef TEENSY_CODE_GPS_H
#define TEENSY_CODE_GPS_H

#include "Sensor.h"
#include "gps_struct.h"
#include <TinyGPS++.h>
#include "Arduino.h"

//This implementation only really works with one GPS at a time, but I don't care
#define GPSSerial Serial8

struct GPS : public Sensor {
    explicit GPS(uint16_t baud);
    TinyGPSPlus parser;
    gps_struct data_struct;
    void updateData() override;
};


#endif //TEENSY_CODE_GPS_H
