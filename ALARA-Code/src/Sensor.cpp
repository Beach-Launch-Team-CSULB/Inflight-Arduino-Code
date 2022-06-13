//
// Created by shawn on 3/21/22.
//
#include "Sensor.h"

#include <Streaming.h>
#include <Int64String.h>

Sensor::Sensor(String n, Sensor_struct * data_type, size_t data_type_size, uint16_t updateRate) {
    //Enable defaults to false until set otherwise when sensor is initialized
    enable = false;
    // Buffer counter and divder counter are 0 until set otherwise
    buf_count = 0;
    divider_counter = 0;
    name = std::move(n);
    data_struct_ptr = data_type;
    data_struct_size = data_type_size;
    out_file = nullptr;
    update_rate = updateRate;
}

void Sensor::dumpToFile(File * in, File * out) const {
    // Stores the number of bytes by the read operation out of the file
    size_t bytesRead;
    // Counter for how many loops the system has read
    uint32_t ctr = 0;
    Serial.println("Input File Size: " + int64String(in->size()));
    Serial.println("Data struct size: " + String(data_struct_size));
    // Read operation, reads data_struct_size bytes into the struct pointed to by data_struct_ptr
    // Data_struct_ptr and data_struct_size are derived from the implementation of each sensor
    bytesRead = in->read(data_struct_ptr, data_struct_size);
    // Loop reads until the end of the file
    while (bytesRead == data_struct_size)
    {
        // Stringifies the data structure and writes it to a file
        out->println(data_struct_ptr->toString());
        //Looped read operation
        bytesRead = in->read(data_struct_ptr, data_struct_size);
        ctr++;
    }
    out->flush();
    Serial.println(ctr);
}

void Sensor::setFile(File *F) {
    // Out file is used as internal file object
    out_file = F;
}

String Sensor::getName() const {
    return name;
}

void Sensor::write(Sensor_struct *data) {
    // File write operation
    out_file->write(data, data_struct_size);
}

