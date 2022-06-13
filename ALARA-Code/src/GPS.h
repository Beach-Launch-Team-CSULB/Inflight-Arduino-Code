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
#define GPSSerial Serial1

struct GPS : public Sensor {
    /**
     * Initializes the GPS
     * The one ALARA I had used when I wrote this code had a broken GPS so I didn't bother wriitng code for that GPS
     * so this code assumes a "generic" UART GPS
     * Hopefully ALARAv2 and/or ALARAv3 fix this issue and we rewrite this GPS interface to use not UART since UART is garbage
     * @param baud The baud rate to use to connect the GPS
     */
    explicit GPS(uint16_t baud);
    /**
     * The TinyGPSPlus object used to parse the UART data into not gibberish
     */
    TinyGPSPlus parser;
    /**
     * the gps data structure used to store data
     */
    gps_struct data_struct;
    /**
     * Function used to update sensor data
     */
    void updateData() override;
    /**
     * data deque used to store GPS data points prior to flash write
     */
    std::deque<gps_struct> data_deque;
};


#endif //TEENSY_CODE_GPS_H
