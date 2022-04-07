//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_ICM_H
#define TEENSY_CODE_ICM_H

#include "Sensor.h"
#include "icm_struct.h"
#include <Adafruit_ICM20948.h>
#include <deque>
const uint8_t icm_update_rate = 7;
class ICM: public Sensor {
    Adafruit_ICM20948 sensor;
public:
    explicit ICM(uint8_t CS);
    void updateData() override;
    icm_struct data_struct;
    std::deque<icm_struct> data_deque;
};


#endif //TEENSY_CODE_ICM_H
