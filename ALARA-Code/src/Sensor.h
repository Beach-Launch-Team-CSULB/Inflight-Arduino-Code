//
// Created by shawn on 3/17/22.
//

#ifndef TEENSY_CODE_SENSOR_H
#define TEENSY_CODE_SENSOR_H

#include "Sensor_struct.h"
#include <FS.h>
#include <utility>
#include <deque>

/**
 * Sensor base class, each sensor is derived from this class
 */
class Sensor {
public:
    /**
     *  Grabs new data values and saves them to flash file
     */
    virtual void updateData() = 0;
    /**
     * Sets the file pointer to the output file
     * @param F The file pointer
     */
    void setFile(File * F);
    /**
     * Returns the name of the sensor
     * @return the sensor name
     */
    String getName() const;
    /**
     * Dumps a struct file into a CSV formatted file
     * @param in The input file
     * @param out The output file
     */
    void dumpToFile(File * in, File * out) const;
    /**
     * Initializes a sensor with the data struct, size of the data struct, and name of the sensor
     * @param n The name of the sensor
     * @param data_type A struct of the corresponding sensor type.
     * @param data_type_size The size of the struct of the corresponding sensor type.
     * @param updateRate The update rate of the sensor relative to each loop
     */
    Sensor(String n, Sensor_struct * data_type, size_t data_type_size, uint16_t updateRate);
    /**
     * Pointer to a data struct to write data into
     */
    Sensor_struct * data_struct_ptr;
    /**
     * Stores if the sensor is enabled
     */
    bool enable;
    /**
     * Saves full contents of buffer to memory
     */
    size_t data_struct_size;
    /**
     * Writes an input sensor struct directly to the output file
     * @param data A pointer to the input sensor struct to be written
     */
    void write(Sensor_struct * data);
    //
    /**
     * The rate at which the sensor updates, roughly relative to the rate other system sensors update
     * For example, one sensor may update 4 times per cycle while another updates 3 times
     */
    uint16_t update_rate;
    /**
     * Amount of sensor structs to hold in buffer before writing
     */
    uint16_t buf_count;
    /**
     * Used to limit update rate before launch
     */
    uint8_t divider_counter;
protected:
    /**
     * Name of the sensor and file on flash
     */
    String name;
    /**
     * File pointer used for writing data
     */
    File * out_file;
    /**
     * Takes in a list of elements and the current element, pushes element into list and writes data when full
     * @tparam D The class the element is the same type as
     * @param derivedList The derived list of elements used as the write buffer
     * @param current_element The most recent element to push into the list and/or write
     */
    template <class D> void new_data_update(std::deque<D> * derivedList, D current_element) {
        derivedList->push_back(current_element);
        while(derivedList->size() >= packet_size * update_rate) {
            divider_counter++;
            //Quick and easy way to reduce the write rate
            //Will count up to divider_counter before it actually writes
            if(divider_counter == byte_rate) {
//                   Serial.println(buf_count);
                //Writes data to file
                out_file->write(&derivedList->front(), sizeof(derivedList->front()));
                // Updates the block size so that the system knows how much more data can be written before
                // a data dump is needed
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
