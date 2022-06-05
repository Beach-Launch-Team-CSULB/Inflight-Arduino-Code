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

/**
 * The Detection class, handles all of the data, etc. used to tell the system what state the rocket is currently in
 */
class Detection {
    /**
     * Current state system is in
     * 0 is pre-launch, 1 is launch pre-apogee, 2 is launch post-apogee, and 3 is land
     * TODO idea: make this a #DEFINE statement block thing
     */
    uint8_t launch_state;
//Below are Amin's original notes
// Used track the acceleration change over a range of readings
//****CONFIG: these variables will be adjusted based on the hardware situation****
// set how many values you will be calculating average from
// TODO SHAWN adust these for specific hardware setup
// set this to whatever the altitude should be under for landing
    /**
     * Altitude landing threshold: Minimum altitude to trigger landing
     */
    const float ALTITUDE_LANDING_THRESHOLD = 30;
    /**
     * Launch Minimum meters per second speed: Minimum speed for the vehicle to be traveling to trigger a launch
     */
    const float LAUNCH_MIN_M_PER_S = 30;
    /**
     * Flag for if the system is in test mode or not
     */
    bool test;
public:
    /**
     * Updates the current state of the rocket using input IMU and BPS data
     * @param icm_data The buffer of icm_struct's to use for gyro, magnetometer, and accel data
     * @param bmp_data The buffer of bmp_struct's to use for barometric pressure/altitude
     * @return The current state of the rocket
     */
    bool updateDetection(std::deque<icm_struct> * icm_data, std::deque<bmp_struct> * bmp_data);
    /**
     * The current state of the rocket
     * @return The current state of the rocket
     */
    uint8_t getState() const {
        return launch_state;
    }
    /**
     * Creates a new detection object
     */
    Detection();
    /**
     * Manually sets a state, used for debugging
     * @param state The state number to set
     */
    void setState(uint8_t state);
};


#endif //TEENSY_INFLIGHT_MINI_CODE_DETECTION_H
