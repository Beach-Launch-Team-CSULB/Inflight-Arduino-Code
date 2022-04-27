//
// Created by shawn on 3/17/22.
//

#include "GPS.h"

GPS::GPS(uint16_t baud) : Sensor("GPS", &data_struct, sizeof(data_struct), 1) {
    Serial.print("");
    name = "GPS";
    enable = false;
    GPSSerial.begin(baud);
    if(GPSSerial.available()) {
        enable = true;
    }
    data_struct.setTime();
    data_struct = gps_struct();
}
void GPS::updateData() {
    if(GPSSerial.available()) {
        while (GPSSerial.available()) {
            parser.encode(GPSSerial.read());
            enable = true;
        }
        data_struct = gps_struct(parser);
        new_data_update(&data_deque, data_struct);
    }
}
