//
// Created by shawn on 3/20/22.
//

#ifndef TEENSY_CODE_INTERNALTEMP_H
#define TEENSY_CODE_INTERNALTEMP_H

#include "Sensor.h"
#include "internal_temp_struct.h"

struct InternalTemp: public Sensor {
    /**
     * Overridden from Sensor base class, updates sensor data
     */
    void updateData() override;
    //Internal Temp data struct
    /**
     * Derived struct for data storage
     */
    internal_temp_struct data_struct;
    //Initializes internal temp device
    /**
     * Constructor for internal temperature sensor
     */
    InternalTemp();
    //Returns temperature
    /**
     * Derived data_deque for storing sensor values
     */
    std::deque<internal_temp_struct> data_deque;
};


#endif //TEENSY_CODE_INTERNALTEMP_H
