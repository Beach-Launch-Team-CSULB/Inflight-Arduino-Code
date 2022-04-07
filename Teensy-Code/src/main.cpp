#include "BMP.h"
#include "BNO.h"
#include "GPS.h"
#include "ICM.h"
#include "InternalTemp.h"
#include "Utilities.h"
#include <LittleFS.h>
#include <SD.h>
#include <Streaming.h>
#include <RH_NRF24.h>
#include <RH_RF95.h>

ICM * icm_device;
BNO * bno_device;
GPS * gps_device;
BMP * bmp_device;
InternalTemp * internal_temp_device;

//Radios
RH_NRF24 * nrf24;
RH_RF95 * rf95;

const uint8_t num_sensors = 6;
Sensor * devices[num_sensors];
File device_files[num_sensors];
File sd_files[num_sensors];
File sd_files_bin[num_sensors];
bool flash_enable, sd_enable;
bool sd_dumped;
bool nrf_enable;
LittleFS_QSPIFlash flash;
SDClass sd;
String sdOutputDir;

void sd_dump() {
    if(flash_enable) {
        String prepend_dir = "/flash/";
        String dir_name = "flash_dump";
        if (!sd.exists(prepend_dir.c_str()))
        {
            sd.mkdir(prepend_dir.c_str());
        }
        uint_fast16_t count = 0;
        String file_name = prepend_dir + dir_name + "/";
        while (sd.exists(file_name.c_str()))
        {
            count++;
            file_name = prepend_dir + dir_name + String(count) + "/";
        }
        sd.mkdir(file_name.c_str());
        sdOutputDir = file_name;
    }
    for (uint_fast8_t i = 0; i < num_sensors; i++) {
        String sd_filename = sdOutputDir + devices[i]->getName() + ".csv";
        String sd_filename_bin = sdOutputDir + devices[i]->getName() + ".bin";
        if(sd_enable) {
            sd_files[i] = sd.open(sd_filename.c_str(), FILE_WRITE);
            sd_files_bin[i] = sd.open(sd_filename_bin.c_str(), FILE_WRITE);
        }
        if(flash_enable) {
            File copy_file = flash.open(devices[i]->getName().c_str());
            //Dump files to SD card as .bin and .csv
            if(sd_enable) {
                devices[i]->dumpToFile(copy_file, sd_files[i]);
                copy_file.seek(0);
                file_copy(&copy_file, &sd_files_bin[i], false);
                copy_file.close();
                flash.remove(devices[i]->getName().c_str());
                device_files[i] = flash.open(devices[i]->getName().c_str(), FILE_WRITE);
                devices[i]->setFile(&device_files[i]);
            }

        }
        Serial <<"Finished dumping " << devices[i]->getName();
    }
    Serial.println("SD dump finished!");
}

void setup() {
    sd_dumped = false;
    icm_device = new ICM(ICM_CS, ICM_INT_1, ICM_INT_2);
    bno_device = new BNO(BNO_CS, BNO_RESET, BNO_INT);
    gps_device = new GPS(GPS_BAUD);
    bmp_device = new BMP(BMP_CS, BMP_INT);
    Detection::setPointers(icm_device, bmp_device);
    internal_temp_device = new InternalTemp();
    nrf24 = new RH_NRF24(NRF_CE, NRF_CSN);
    nrf_enable = nrf24->init();
    nrf24->setChannel(1);
    nrf24->setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm);
    devices[0] = icm_device;
    devices[1] = bno_device;
    devices[2] = bmp_device;
    devices[3] = gps_device;
    devices[4] = internal_temp_device;
    devices[5] = &eventLog;
    flash_enable = flash.begin();
    sd_enable = sd.begin(BUILTIN_SDCARD);
    sd_dump();
}

void loop() {
    for(auto & device : devices) {
        device->updateData();
        Serial.println(device->data_struct_ptr->getHeader());
        Serial.println(device->data_struct_ptr->toString());
    }
    if(updateBlockSize() && (state_detection.getState() != 2)) {
        eventLog.write("Dump limit reached! Dumping to SD");
        sd_dump();
        resetBlockCounter();
    }
    if(Serial.available()) {
        int state = Serial.readString().toInt();
        Serial << "Setting state " << state << endl;
        state_detection.setState(state);
    }
    if(state_detection.updateDetection()) {
        switch (state_detection.getState()) {
            case 1:
                //Set data logging to max speed
                eventLog.write("Case 1 reached! Updating byte rate");
                byte_rate = 0;
                break;
            case 3:
                //Do a data dump
                if(!sd_dumped) {
                    Serial.println(sd_dumped);
                    eventLog.write("Case 3 reached! Dumping to SD");

                    for (File f: device_files) {
                        f.close();
                    }
                    sd_dump();
                    sd_dumped = true;
                }
                break;
        }
    }
    struct __attribute__((unused)) RadioPacket {
        RadioPacket(uint32_t id, float temp, float latitude, float longitude, uint32_t readingAge, float pressure) : id(
                id), temp(temp), latitude(latitude), longitude(longitude),  reading_age(
                readingAge), pressure(pressure) {}
        uint32_t id;
        float temp;
        float latitude;
        float longitude;
        uint32_t reading_age;
        float pressure;
    };
    auto * psend = (uint8_t*)(new RadioPacket(0, (float)bmp_device->data_struct.temperature,
                                              (float)gps_device->data_struct.gps_latitude,
                                              (float)gps_device->data_struct.gps_longitude,
                                              gps_device->data_struct.gps_reading_age,
                                              (float)bmp_device->data_struct.pressure));
    nrf24->send(psend, sizeof(psend));
}