//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BNO_H
#define TEENSY_CODE_BNO_H

#include <Adafruit_BNO055.h>
#include "Sensor.h"
#include "bno_struct.h"

struct BNO: public Sensor {
    /**
     * Creates a BNO object
     * I2C sensor with (hopefully) fixed address so no parameters needed
     */
    BNO();
    /**
     * Updates sensor from new data
     */
    void updateData() override;
    /**
     * Data struct used for storing BNO data
     */
    bno_struct data_struct;
    /**
     * Base sensor used to store sensor data in
     */
    Adafruit_BNO055 sensor;
    /**
     * Data deque used for state detection and sensor writes
     */
    std::deque<bno_struct> data_deque;
};


#endif //TEENSY_CODE_BNO_H
