//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_ICM_H
#define TEENSY_CODE_ICM_H

#include "Sensor.h"
#include "icm_struct.h"
#include <Adafruit_ICM20948.h>
#include <deque>
/**
 * Update rate of the ICM sensor
 * Relatively high compared to other sensors due to the high data rate
 */
const uint8_t icm_update_rate = 7;
/**
 * Sensor class for the ICM20948 sensor
 */
class ICM: public Sensor {
    /**
     * Adafruit sensor object for the ICM20948 sensor
     */
    Adafruit_ICM20948 sensor;
public:
    /**
     * ICM20948 constructor
     * @param CS the SPI chip select pin for the sensor
     */
    explicit ICM(uint8_t CS);
    /**
     * Updates the sensor data, overridden from Sensor base class
     */
    void updateData() override;
    /**
     * Derived data struct for storage
     */
    icm_struct data_struct;
    /**
     * Derived data_deque used for storage
     */
    std::deque<icm_struct> data_deque;
};


#endif //TEENSY_CODE_ICM_H
