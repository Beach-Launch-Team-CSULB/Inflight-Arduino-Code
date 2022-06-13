//
// Created by shawn on 3/25/22.
//

#ifndef TEENSY_INFLIGHT_MINI_CODE_DETECTION_H
#define TEENSY_INFLIGHT_MINI_CODE_DETECTION_H


#include <deque>
#include <cstdint>
#include "MS5607Sensor.h"
#include "BNO.h"
class Detection {
    /**
     * Tracks the current state of the rocket between several possible launch states
     * 0 is pre-launch, 1 is launch pre-apogee, 2 is launch post-apogee, and 3 is land
     * TODO idea: make this a #DEFINE statement block thing
     */
    uint8_t launch_state;
//------->
// Used track the acceleration change over a range of readings
//****CONFIG: these variables will be adjusted based on the hardware situation****
// set how many values you will be calculating average from
// TODO SHAWN adust these for specific hardware setup
// set this to whatever the altitude should be under for landing
/**
 * minimum altitude threshold to consider the rocket has landed at
 */
    const float ALTITUDE_LANDING_THRESHOLD = 30;
    /**
     * Min meters per second for the rocket to be considered having launched
     */
    const float LAUNCH_MIN_M_PER_S = 30;
    /**
     * Switch for test mode
     */
    bool test;
public:
    /**
     * Updates the state of the detection system from IMU and BPS data
     * Pass the data structs directly into this function for calculations
     * TODO: Pls rename these variables
     * @param icm_data IMU data from the BNO085
     * @param bmp_data BPS data from the MS5607
     * @return Whether the state has changed. If it has, the state variable will have been updated and should be checked.
     */
    bool updateDetection(std::deque<bno_struct> * icm_data, std::deque<ms5607_struct> * bmp_data);
    /**
     * Function that returns the current launch state of the system
     */
    uint8_t getState() const {
        return launch_state;
    }
    /**
     * Constructor for a State Detection object
     * Main work is done by updateDetection() and getState() so you don't need to pass anything into it to make one
     */
    Detection();
    /**
     * Manually changes the system state
     * Used for testing
     */
    void setState(uint8_t state);
};


#endif //TEENSY_INFLIGHT_MINI_CODE_DETECTION_H
