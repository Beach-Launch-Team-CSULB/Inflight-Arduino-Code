//
// Created by shawn on 3/21/22.
//
#include "Sensor.h"
#include <Streaming.h>

Sensor::Sensor(String n, Sensor_struct * data_type, size_t data_type_size) {
    enable = false;
    name = std::move(n);
    data_struct_ptr = data_type;
    data_struct_size = data_type_size;
}

void Sensor::dumpToFile(File in, File out) const {
        size_t bytesRead;
        int counter = 0;
        Serial << "Input File Size: " << in.size() << endl;
        Serial << "Data struct size: " << data_struct_size;
        bytesRead = in.read(data_struct_ptr, data_struct_size);
        out.println(data_struct_ptr->getHeader());

        while (bytesRead == data_struct_size)
        {
            out.println(data_struct_ptr->toString());
            bytesRead = in.read(data_struct_ptr, data_struct_size);
            counter++;
        }
        out.flush();
        Serial.print(counter);
}

void Sensor::setFile(File *F) {
    out_file = F;
}

String Sensor::getName() const {
        return name;
}

bool Sensor::getStatus() const {
        return enable;
}



