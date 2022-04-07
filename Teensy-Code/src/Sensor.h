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
    //Grabs new data values and saves them to flash file
    virtual void updateData() = 0;
    //Sets the file pointer to the output file
    void setFile(File * F);
    //Returns the name of the sensor
    String getName() const;
    //File Write counter variable
    uint32_t counter;
    //Dumps a struct file into a CSV formatted file
    void dumpToFile(File in, File out) const;
    //Initializes a sensor with the data struct, size of the data struct, and name of the sensor
    Sensor(String n, Sensor_struct * data_type, size_t data_type_size, uint16_t updateRate);
    //Pointer to a data struct to write data into
    Sensor_struct * data_struct_ptr;
    //Stores if the sensor is enabled
    bool enable;
    size_t data_struct_size;
    void write(Sensor_struct * data);
    uint16_t update_rate;
protected:
    //Name of the sensor and file on flash
    String name;
    //File pointer used for writing data
    File * out_file;
    //Size of data struct for writing from sizeof()
    //Byte rate counter
    uint16_t brc;
    void new_data_update();
};


#endif //TEENSY_CODE_SENSOR_H
