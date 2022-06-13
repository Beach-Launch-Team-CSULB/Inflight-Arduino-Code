//
// Created by shawn on 4/23/22.
//

#ifndef ALARA_CODE_KX_H
#define ALARA_CODE_KX_H

#include "Sensor.h"
#include "kx_struct.h"
#include "SparkFun_Qwiic_KX13X.h"

class KX : public Sensor {
    /**
     * KX SPI interrupt pin, not currently used but in interface for potential future changes
     */
    uint8_t INT;
    /**
     * data_store object used with QciicKX134 object to get data back
     */
    outputData data_store{};
public:
    /**
     * Overridden sensor update function
     */
    void updateData() override;
    /**
     * Sensor object used to generate data
     */
    QwiicKX134 sensor;
    /**
     * KX initialization constructor
     * @param CS SPI Chip Select pin for KX134 sensor
     * @param int_pin SPI digital interrupt pin for KX134 sensor
     */
    explicit KX(uint8_t CS, uint8_t int_pin);
    /**
     * data struct used for data storage
     */
    kx_struct data_struct;
    /**
     * Data deque used for data storage
     */
    std::deque<kx_struct> data_deque;
};


#endif //ALARA_CODE_KX_H
