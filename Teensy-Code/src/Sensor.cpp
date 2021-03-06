//
// Created by shawn on 3/21/22.
//
#include "Sensor.h"
#include "Utilities.h"
#include <Streaming.h>
#include <Int64String.h>

Sensor::Sensor(String n, Sensor_struct * data_type, size_t data_type_size, uint16_t updateRate) {
    enable = false;
    name = std::move(n);
    data_struct_ptr = data_type;
    data_struct_size = data_type_size;
    brc = 0;
    counter = 0;
    out_file = nullptr;
    update_rate = updateRate;
}

void Sensor::dumpToFile(File in, File out) const {
        size_t bytesRead;
        uint32_t ctr = 0;
        Serial.println("Input File Size: " + int64String(in.size()));
        Serial.println("Data struct size: " + String(data_struct_size));
        bytesRead = in.read(data_struct_ptr, data_struct_size);
        out.println(data_struct_ptr->getHeader());
        while (bytesRead == data_struct_size)
        {
            out.println(data_struct_ptr->toString());
            bytesRead = in.read(data_struct_ptr, data_struct_size);
            ctr++;
        }
        out.flush();
        Serial.println(ctr);
}

void Sensor::setFile(File *F) {
    out_file = F;
}

String Sensor::getName() const {
        return name;
}

void Sensor::new_data_update() {
    if(brc > (byte_rate * update_rate)) {
//        Serial << "Data struct name is " << getName() << ", Data struct size is" << data_struct_size;
        out_file->write(data_struct_ptr, data_struct_size);
        brc = 0;
        counter += data_struct_size;
        if(counter > packet_size || (byte_rate > 0 && counter > 1000)) {
            eventLog.write("Counter is " + String(counter) + ", Performing flush for " + name);
            counter = 0;
            out_file->flush();
        }
        updateBlockSize(data_struct_size);
    }
    brc++;
}

void Sensor::write(Sensor_struct *data) {
    out_file->write(data, data_struct_size);
}
