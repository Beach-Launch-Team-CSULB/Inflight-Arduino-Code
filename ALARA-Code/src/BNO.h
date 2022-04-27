//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BNO_H
#define TEENSY_CODE_BNO_H

#include <Adafruit_BNO055.h>
#include "Sensor.h"
#include "bno_struct.h"

struct BNO: public Sensor {
    BNO();
    void updateData() override;
    bno_struct data_struct;
    Adafruit_BNO055 sensor;
    std::deque<bno_struct> data_deque;
};


#endif //TEENSY_CODE_BNO_H
