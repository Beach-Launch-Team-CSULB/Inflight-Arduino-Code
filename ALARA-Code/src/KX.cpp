//
// Created by shawn on 4/23/22.
//

#include "KX.h"

void KX::updateData() {
         //Puts data in data store
        data_store = sensor.getAccelData();
        // Updates data struct from data store
        data_struct = kx_struct(data_store);
        // Updates data deque from data struct
        new_data_update(&data_deque, data_struct);
}

KX::KX(uint8_t CS, uint8_t int_pin) : Sensor("KX", &data_struct, sizeof(data_struct), 1) {
    // Makes sure SPI is working, sensor library will crash if you don't do this
    SPI.begin();
    // Store the interrupt pin in case a future version of the library needs this
    INT = int_pin;
    // enables the sensor, initializes sensor with default settings
    // Uses this structure because it shouldn't try to initialize the sensor if the sensor doesn't work
    // and it's convenient
    enable = sensor.beginSPI(CS) && sensor.initialize(DEFAULT_SETTINGS);
    // Sets sensor range to 32G, the max
    sensor.setRange(KX134_RANGE32G);
}
