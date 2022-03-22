//
// Created by shawn on 3/17/22.
//

#include "GPS.h"
#include "Utilities.h"

GPS::GPS(uint16_t baud) : Sensor("GPS", &data_struct, sizeof(data_struct)) {
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
        data_struct.setTime();
    out_file->write(&data_struct, sizeof(data_struct));
    }
    counter += sizeof(data_struct);
    if(counter > packet_size) {
        counter = 0;
        out_file->flush();
    }
}