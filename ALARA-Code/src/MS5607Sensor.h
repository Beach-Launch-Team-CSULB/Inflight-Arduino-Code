//
// Created by shawn on 4/22/22.
//

#ifndef ALARA_CODE_MS5607SENSOR_H
#define ALARA_CODE_MS5607SENSOR_H

#include "Sensor.h"
#include "ms5607.h"
#include "ms5607_struct.h"

class MS5607Sensor: public Sensor {
public:
    /**
     * Updates temp data from MS507 sensor, updates data deque
     */
    void updateData() override;
    /**
     * Internal MS507 sensor object used to collect data
     */
    MS5607 sensor;
    /**
     * Constructor for new sensor
     * No parameters needed since the driver is ALARA-specific and this derived sensor class is only designed for use
     * with ALARAv1
     */
    MS5607Sensor();
    /**
     * Internal ms5607_struct used for object data storage
     */
    ms5607_struct data_struct;
    /**
     * deque of derived MS5607 objects
     */
    std::deque<ms5607_struct> data_deque;
};


#endif //ALARA_CODE_MS5607SENSOR_H
