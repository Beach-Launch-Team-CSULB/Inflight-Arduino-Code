//
// Created by shawn on 3/21/22.
//
#include "Sensor.h"

#include <Streaming.h>
#include <Int64String.h>

Sensor::Sensor(String n, Sensor_struct * data_type, size_t data_type_size, uint16_t updateRate) {
    enable = false;
    buf_count = 0;
    divider_counter = 0;
    name = std::move(n);
    data_struct_ptr = data_type;
    data_struct_size = data_type_size;
    out_file = nullptr;
    update_rate = updateRate;
}

void Sensor::dumpToFile(File * in, File * out) const {
        size_t bytesRead;
        uint32_t ctr = 0;
        Serial.println("Input File Size: " + int64String(in->size()));
        Serial.println("Data struct size: " + String(data_struct_size));
        bytesRead = in->read(data_struct_ptr, data_struct_size);
        while (bytesRead == data_struct_size)
        {
            out->println(data_struct_ptr->toString());
            bytesRead = in->read(data_struct_ptr, data_struct_size);
            ctr++;
        }
        out->flush();
        Serial.println(ctr);
}

void Sensor::setFile(File *F) {
    out_file = F;
}

String Sensor::getName() const {
        return name;
}

void Sensor::write(Sensor_struct *data) {
    out_file->write(data, data_struct_size);
}

