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
    const uint16_t update_rate = 1;};

#endif //TEENSY_CODE_BMP_H
