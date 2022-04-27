//
// Created by shawn on 4/23/22.
//

#include "KX.h"

void KX::updateData() {
        data_store = sensor.getAccelData();
        data_struct = kx_struct(data_store);
        new_data_update(&data_deque, data_struct);
}

KX::KX(uint8_t CS, uint8_t int_pin) : Sensor("KX", &data_struct, sizeof(data_struct), 1) {
    SPI.begin();
    INT = int_pin;
    enable = sensor.beginSPI(CS) && sensor.initialize(DEFAULT_SETTINGS);
    sensor.setRange(KX134_RANGE32G);
}
