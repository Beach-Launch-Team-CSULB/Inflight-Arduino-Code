//
// Created by shawn on 4/23/22.
//

#ifndef ALARA_CODE_KX_STRUCT_H
#define ALARA_CODE_KX_STRUCT_H

#include <Sensor_struct.h>
#include "SparkFun_Qwiic_KX13X.h"

struct kx_struct : public Sensor_struct{
    /**
     * Overriden toString(), returns time + acceleration values
     */
    String toString() override;
    /**
     * Returns CSV file header with time headers + accel headers
     * @return
     */
    String getHeader() const override;
    /**
     * Creates new KX struct from a KX class output data structure
     * @param data an outputData object to generate the data from
     */
    explicit kx_struct(outputData data);
    /**
     * Default constructor for a KX struct
     */
    kx_struct() = default;
    /**
     * Array of X/Y/Z acceleration values for a KX struct
     */
    float accel[3]{};
};


#endif //ALARA_CODE_KX_STRUCT_H
