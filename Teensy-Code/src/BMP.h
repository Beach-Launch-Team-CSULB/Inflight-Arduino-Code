//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BMP_H
#define TEENSY_CODE_BMP_H

#include "Sensor.h"
#include "bmp_struct.h"
#include <Adafruit_BMP3XX.h>
const float sea_level_pressure_hpa = 1015.578;


struct BMP: public Sensor {
    uint8_t BMP_INT;
    Adafruit_BMP3XX sensor;
    bmp_struct data_struct;
    void updateData() override;
    explicit BMP(uint8_t CS, uint8_t INT);
};

#endif //TEENSY_CODE_BMP_H
