//
// Created by shawn on 4/23/22.
//

#include "kx_struct.h"

String kx_struct::toString() {
    return get_time() + flts(accel[0]) cm flts(accel[1]) cm flts(accel[2]);
}

String kx_struct::getHeader() const {
    return get_time_hdr() + "Accel X, Accel Y, Accel Z";
}

kx_struct::kx_struct(outputData data) {
    accel[0] = data.xData;
    accel[1] = data.yData;
    accel[2] = data.zData;
}
