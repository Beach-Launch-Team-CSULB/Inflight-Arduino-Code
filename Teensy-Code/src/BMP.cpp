//
// Created by shawn on 3/18/22.
//

#include "BMP.h"
#include "Utilities.h"

void BMP::updateData() {
    float alt = sensor.readAltitude(sea_level_pressure_hpa);
    data_struct = bmp_struct(sensor.temperature, sensor.pressure, alt);
    out_file->write(&data_struct, sizeof(data_struct));
    Serial.println(out_file->size());
    counter += sizeof(data_struct);
    if(counter > packet_size) {
        counter = 0;
        out_file->flush();
    }
}

BMP::BMP(uint8_t CS, uint8_t INT) : Sensor("BMP", &data_struct, sizeof(data_struct)) {
    name = "BMP";
    enable = sensor.begin_SPI(CS);
    BMP_INT = INT;
    sensor.setOutputDataRate(BMP3_ODR_200_HZ);
    // Sets oversampling rates to 32x- will return results more quickly/more results
    sensor.setPressureOversampling(BMP3_OVERSAMPLING_32X);
    sensor.setTemperatureOversampling(BMP3_OVERSAMPLING_32X);
    // Sets filter coefficient
    sensor.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_7);
}