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

static ICM * imu;
static BMP * bps;
static std::deque<icm_struct> accelYBuff;
static std::deque<bmp_struct> altBuff;

class Detection {
    uint8_t launch_state;
//------->
// Used track the acceleration change over a range of readings

//****CONFIG: these variables will be adjusted based on the hardware situation****
// set how many values you will be calculating average from
// TODO SHAWN adust these for specific hardware setup
    const uint16_t bpsBufferSize = 700;
    const uint16_t imuBufferSize = 100;
// set this to whatever the altitude should be under for landing
    const float ALTITUDE_LANDING_THRESHOLD = 150;
    bool test;
    bool alt_lock;
public:
    bool updateDetection();
    void addIMUData(const icm_struct& icm_data) const;
    void addBPSData(const bmp_struct& bmp_data) const;
    uint8_t getState() const {
        return launch_state;
    }

    static void setPointers(ICM *imu_ptr, BMP *bps_ptr);
    Detection();
    void setState(uint8_t state);
};


#endif //TEENSY_INFLIGHT_MINI_CODE_DETECTION_H
