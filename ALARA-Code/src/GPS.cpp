//
// Created by shawn on 3/17/22.
//

#include "GPS.h"

GPS::GPS(uint16_t baud) : Sensor("GPS", &data_struct, sizeof(data_struct), 1) {
    // Creates new GPS object
    // Sensor is disabled unless the serial device the GPS is connected to works
    enable = false;
    // Starts UART connection to GPS
    // Curretly uses Serial1 but subject to change based on how GPS is connected
    GPSSerial.begin(baud);
    if(GPSSerial.available()) {
        enable = true;
    }
    // Creates empty GPS data structure, sets empty info other than current time until reading
    data_struct.setTime();
    data_struct = gps_struct();
}
void GPS::updateData() {
    // Don't bother updating unless the GPS has new data, skip to next sensor
    if(GPSSerial.available()) {
        // Feed GPS data into the parser until there isn't any more
        while (GPSSerial.available()) {
            parser.encode(GPSSerial.read());
            enable = true;
        }
        // Create new data struct from GPS data parsed
        data_struct = gps_struct(parser);
        // Add new data to update deque
        new_data_update(&data_deque, data_struct);
    }
}
