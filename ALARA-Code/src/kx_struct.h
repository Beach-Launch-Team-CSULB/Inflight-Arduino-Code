//
// Created by shawn on 4/23/22.
//

#ifndef ALARA_CODE_KX_STRUCT_H
#define ALARA_CODE_KX_STRUCT_H

#include <Sensor_struct.h>
#include "SparkFun_Qwiic_KX13X.h"

struct kx_struct : public Sensor_struct{
    String toString() override;
    //Returns CSV file header
    String getHeader() const override;
    explicit kx_struct(outputData data);
    kx_struct() = default;
    float accel[3]{};
};


#endif //ALARA_CODE_KX_STRUCT_H
