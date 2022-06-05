//
// Created by shawn on 3/18/22.
//

#ifndef TEENSY_CODE_BMP_H
#define TEENSY_CODE_BMP_H

#include "Sensor.h"
#include "bmp_struct.h"
#include <Adafruit_BMP3XX.h>


struct BMP: public Sensor {
    /**
     * BMP3XX Interrupt pin
     * Not used in current version of code, but could be in a future iteration so the interface supports the pin
     */
    uint8_t BMP_INT;
    /**
     * Adafruit sensor object for the BMP3XX
     */
    Adafruit_BMP3XX sensor;
    /**
     * derived data struct used for storing collected sensor data
     */
    bmp_struct data_struct;
    /**
     * Used to store the last time the system updated the altitude for what it thought was the ground
     * Needs to update the ground altitude periodically so the ground altitude use for calculations is always accurate
     */
    time_t last_ground_altitude_update;
    /**
     * Derived function to update sensor data
     */
    void updateData() override;
    /**
     * Constructor for the BMP device, creates a BMP device
     * @param CS Arduino pin number for the SPI Chip select pin
     * @param INT Arduino pin number for the SPI interrupt pin
     */
    explicit BMP(uint8_t CS, uint8_t INT);
    /**
     * Update rate of the sensor, 1 because it has a low update rate relative to other sensors
     */
    const uint16_t update_rate = 1;
    /**
     * stores whether the sensor should activate the sea level pressure update code
     * Once the rocket launches we don't want this to happen
     */
    bool slp_reset;
    /**
     * Derived data_deque sensor buffer
     */
    std::deque<bmp_struct> data_deque;
    /**
     * Stores the sea level pressure in HPA
     */
    float sea_level_pressure_hpa;
    /**
     * Tells the sensor to or not to reset the sea level pressure
     */
    void reset_slp(bool set);
};

#endif //TEENSY_CODE_BMP_H
