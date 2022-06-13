//
// Created by shawn on 3/20/22.
//

#ifndef TEENSY_CODE_INTERNALTEMP_H
#define TEENSY_CODE_INTERNALTEMP_H

#include "Sensor.h"
#include "internal_temp_struct.h"

struct InternalTemp: public Sensor {
    /**
     * Overridden sensor data update function
     */
    void updateData() override;
    /**
     * Internal Temp data struct
     */
    internal_temp_struct data_struct;
    /**
     * Initializes internal temp device
     */
    InternalTemp();
    /**
     * Data deque used to store data_structs prior to writing to flash
     */
    std::deque<internal_temp_struct> data_deque;
};


#endif //TEENSY_CODE_INTERNALTEMP_H
