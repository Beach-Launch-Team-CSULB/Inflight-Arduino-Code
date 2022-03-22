//
// Created by shawn on 3/20/22.
//

#ifndef TEENSY_CODE_INTERNALTEMP_H
#define TEENSY_CODE_INTERNALTEMP_H

#include "Sensor.h"
#include "internal_temp_struct.h"
#include <InternalTemperature.h>

struct InternalTemp: public Sensor {
    void updateData() override;
    internal_temp_struct data_struct;
    InternalTemp();
    float getTemp() const;
};


#endif //TEENSY_CODE_INTERNALTEMP_H
