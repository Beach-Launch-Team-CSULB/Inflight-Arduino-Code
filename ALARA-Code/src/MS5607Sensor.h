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
    void updateData() override;
    MS5607 sensor;
    //Initializes internal temp device
    MS5607Sensor();
    //Returns temperature
    ms5607_struct data_struct;
    std::deque<ms5607_struct> data_deque;
};


#endif //ALARA_CODE_MS5607SENSOR_H
