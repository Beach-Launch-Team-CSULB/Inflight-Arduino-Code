//
// Created by shawn on 3/17/22.
//

#ifndef TEENSY_CODE_SENSOR_H
#define TEENSY_CODE_SENSOR_H

#include "Sensor_struct.h"
#include <FS.h>

#include <utility>

class Sensor {
public:
    virtual void updateData() = 0;
    void setFile(File * F);
    bool getStatus() const;
    String getName() const;
    uint16_t counter{};
    void dumpToFile(File in, File out) const;
    Sensor(String n, Sensor_struct * data_type, size_t data_type_size);
    Sensor_struct * data_struct_ptr;
    bool enable;
protected:
    String name;
    File * out_file{};
    size_t data_struct_size;
};


#endif //TEENSY_CODE_SENSOR_H
