//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BNO_H
#define TEENSY_CODE_BNO_H

#include <Adafruit_BNO08x.h>
#include "Sensor.h"
#include "bno_struct.h"

struct BNO: public Sensor {
    BNO(uint8_t CS, int8_t RST, uint8_t INT);
    void updateData() override;
    bno_struct data_struct;
    Adafruit_BNO08x sensor;
};


#endif //TEENSY_CODE_BNO_H
