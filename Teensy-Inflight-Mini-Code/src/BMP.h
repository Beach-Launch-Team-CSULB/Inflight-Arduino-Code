//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BMP_H
#define TEENSY_CODE_BMP_H

#include "Sensor.h"
#include "bmp_struct.h"
#include <Adafruit_BMP3XX.h>


struct BMP: public Sensor {
    uint8_t BMP_INT;
    Adafruit_BMP3XX sensor;
    bmp_struct data_struct;
    time_t last_ground_altitude_update;
    void updateData() override;
    explicit BMP(uint8_t CS, uint8_t INT);
    const uint16_t update_rate = 1;
    bool slp_reset;
    std::deque<bmp_struct> data_deque;
    float sea_level_pressure_hpa;
    void reset_slp(bool set);
};

#endif //TEENSY_CODE_BMP_H
