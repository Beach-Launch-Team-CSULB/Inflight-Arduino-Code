//
// Created by shawn on 3/17/22.
//

#ifndef TEENSY_CODE_SENSOR_H
#define TEENSY_CODE_SENSOR_H

#include "Sensor_struct.h"
#include <FS.h>
#include <utility>
#include <deque>

class Sensor {
public:
    //Grabs new data values and saves them to flash file
    virtual void updateData() = 0;
    //Sets the file pointer to the output file
    void setFile(File * F);
    //Returns the name of the sensor
    String getName() const;
    //Dumps a struct file into a CSV formatted file
    void dumpToFile(File in, File out) const;
    //Initializes a sensor with the data struct, size of the data struct, and name of the sensor
    Sensor(String n, Sensor_struct * data_type, size_t data_type_size, uint16_t updateRate);
    //Pointer to a data struct to write data into
    Sensor_struct * data_struct_ptr;
    //Stores if the sensor is enabled
    bool enable;
    //Saves full contents of buffer to memory
    size_t data_struct_size;
    void write(Sensor_struct * data);
    uint16_t update_rate;
    uint16_t buf_count;
    uint8_t divider_counter;
protected:
    //Name of the sensor and file on flash
    String name;
    //File pointer used for writing data
    File * out_file;
    //Size of data struct for writing from sizeof()
    template <class D> void new_data_update(std::deque<D> * derivedList, D current_element) {
        derivedList->push_back(current_element);
//        Serial.println("Outer print");
//        Serial.println(derivedList->size());
//        Serial.println(data_struct_size);
//        Serial.println(packet_size);
        while(derivedList->size() >= packet_size * update_rate) {
//                Serial.println("While print");
//                Serial.println(packet_size / 2);
//                Serial.println((unsigned long)(derivedList->size()));
//                Serial.println(byte_rate);
//                Serial.println(update_rate);
//                Serial.println(byte_rate * update_rate);
                divider_counter++;
                if(divider_counter == byte_rate) {
                    Serial.println("File write!");
//                    Serial.println(buf_count);
                    out_file->write(&derivedList->front(), sizeof(derivedList->front()));
                    updateBlockSize(data_struct_size);
                    buf_count++;
                    Serial.println(buf_count);
                    Serial.println(max_buf_size);
                    if(buf_count > max_buf_size) {
                        Serial.println("File flush!");
                        out_file->flush();
                        buf_count = 0;
                    }
                    divider_counter = 0;
                }
                derivedList->pop_front();
        }
    }
};

#endif //TEENSY_CODE_SENSOR_H
