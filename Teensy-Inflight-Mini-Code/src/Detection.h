//
// Created by shawn on 3/25/22.
//

#ifndef TEENSY_INFLIGHT_MINI_CODE_DETECTION_H
#define TEENSY_INFLIGHT_MINI_CODE_DETECTION_H


#include <deque>
#include <cstdint>
#include "icm_struct.h"
#include "bmp_struct.h"
#include "ICM.h"
#include "BMP.h"

class Detection {
    uint8_t launch_state;
//------->
// Used track the acceleration change over a range of readings
//****CONFIG: these variables will be adjusted based on the hardware situation****
// set how many values you will be calculating average from
// TODO SHAWN adust these for specific hardware setup
// set this to whatever the altitude should be under for landing
    const float ALTITUDE_LANDING_THRESHOLD = 30;
    const float LAUNCH_MIN_M_PER_S = 30;
    bool test;
public:
    bool updateDetection(std::deque<icm_struct> * icm_data, std::deque<bmp_struct> * bmp_data);
    uint8_t getState() const {
        return launch_state;
    }
    Detection();
    void setState(uint8_t state);
};


#endif //TEENSY_INFLIGHT_MINI_CODE_DETECTION_H
