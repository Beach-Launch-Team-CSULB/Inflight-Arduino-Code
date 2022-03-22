//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_ICM_H
#define TEENSY_CODE_ICM_H

#include "Sensor.h"
#include "icm_struct.h"
#include <Adafruit_ICM20649.h>
class ICM: public Sensor {
    uint_fast8_t INT_1{}, INT_2{};
    Adafruit_ICM20649 sensor;
    icm_struct data_struct;
public:
    ICM(uint_fast8_t CS, uint_fast8_t int1, uint_fast8_t int2);
    void updateData() override;
};


#endif //TEENSY_CODE_ICM_H
