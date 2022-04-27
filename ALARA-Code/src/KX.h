//
// Created by shawn on 4/23/22.
//

#ifndef ALARA_CODE_KX_H
#define ALARA_CODE_KX_H

#include "Sensor.h"
#include "kx_struct.h"
#include "SparkFun_Qwiic_KX13X.h"

class KX : public Sensor {
    uint8_t INT;
    outputData data_store{};
public:
    void updateData() override;
    QwiicKX134 sensor;
    explicit KX(uint8_t CS, uint8_t int_pin);
    kx_struct data_struct;
    std::deque<kx_struct> data_deque;
};


#endif //ALARA_CODE_KX_H
