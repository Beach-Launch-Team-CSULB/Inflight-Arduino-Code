#include "BNO.h"
#include "InternalTemp.h"
#include "EventLog.h"
#include "Detection.h"
#include "KX.h"
#include <LittleFS.h>
#include <SD.h>
#include <Streaming.h>
#include <ALARA.h>

BNO * bno_device;
InternalTemp * internal_temp_device;
MS5607Sensor * ms5607;
KX * kx;
Detection state_detection;
const uint8_t num_sensors = 5;
Sensor * devices[num_sensors];
File device_files[num_sensors];
File sd_files[num_sensors];
File sd_files_bin[num_sensors];
bool flash_enable, sd_enable;
bool sd_dumped;
LittleFS_SPIFlash flash;
SDClass sd;
String sdOutputDir;

void sd_dump() {
    if (flash_enable) {
        flash.quickFormat();
        String prepend_dir = "/flash/";
        String dir_name = "flash_dump";
        if (!sd.exists(prepend_dir.c_str())) {
            sd.mkdir(prepend_dir.c_str());
        }
        uint_fast16_t count = 0;
        String file_name = prepend_dir + dir_name + "/";
        while (sd.exists(file_name.c_str())) {
            count++;
            file_name = prepend_dir + dir_name + String(count) + "/";
        }
        sd.mkdir(file_name.c_str());
        sdOutputDir = file_name;
    }
    for (uint_fast8_t i = 0; i < num_sensors; i++) {
        String sd_filename = sdOutputDir + devices[i]->getName() + ".csv";
        String sd_filename_bin = sdOutputDir + devices[i]->getName() + ".bin";
        if (sd_enable) {
            sd_files[i] = sd.open(sd_filename.c_str(), FILE_WRITE);
            sd_files_bin[i] = sd.open(sd_filename_bin.c_str(), FILE_WRITE);
        }
        if (flash_enable) {
            File copy_file = flash.open(devices[i]->getName().c_str());
            //Dump files to SD card as .bin and .csv
            if (sd_enable) {
                Serial.println("Preparing to dump files!");
                devices[i]->dumpToFile(&copy_file, &sd_files[i]);
                Serial.println("Dumped to bin file!");
                copy_file.seek(0);
                file_copy(&copy_file, &sd_files_bin[i], false);
                Serial.println("Dumped to csv file!");
                copy_file.close();
                flash.remove(devices[i]->getName().c_str());
            }
            else {
                Serial.println("SD card not inserted during dump");
            }
            device_files[i] = flash.open(devices[i]->getName().c_str(), FILE_WRITE);
            devices[i]->setFile(&device_files[i]);

        }
        Serial << "Finished dumping " << devices[i]->getName();
    }
    Serial.println("SD dump finished!");
}

void setup() {
    while(!Serial);
    Serial.println("Begin!");
    sd_enable = sd.begin(BUILTIN_SDCARD);
    bool sd_enable_2 = SD.begin(BUILTIN_SDCARD);
    flash_enable = flash.begin(NOR1_CS);
    sd_dumped = false;
    bno_device = new BNO();
    internal_temp_device = new InternalTemp();
    kx = new KX(KX_CSN, KX_TRIG);
    ms5607 = new MS5607Sensor();
    Serial.println("Reached end of setup!");
    Serial.flush();
    devices[0] = bno_device;
    devices[1] = internal_temp_device;
    devices[2] = ms5607;
    devices[3] = kx;
    devices[4] = &eventLog;
    sd.mediaPresent();
    Serial.println("Storage device enables");
    Serial.println(flash_enable);
    Serial.println(sd_enable);
    Serial.println(sd_enable_2);
    Serial.println(sd.mediaPresent());
    Serial.flush();
    sd_dump();
}

void loop() {
    for(auto & device : devices) {
        Serial.println("Sensor");
        Serial.flush();
        Serial.println(device->getName());
        device->updateData();
        Serial.println(device->data_struct_ptr->getHeader());
        Serial.println(device->data_struct_ptr->toString());
        Serial.flush();
    }
    Serial.println(getBlockSize());
    Serial.flush();
    Serial.println(state_detection.getState());
    if(getBlockSize() && (state_detection.getState() != 2 || state_detection.getState() != 1)) {
        eventLog.write("Dump limit reached! Dumping to SD");
        sd_dump();
        resetBlockCounter();
    }
    if(Serial.available()) {
        int state = Serial.readString().toInt();
        Serial << "Setting state " << state << endl;
        state_detection.setState(state);
    }
    if (state_detection.updateDetection(&bno_device->data_deque, &ms5607->data_deque)) {
        switch (state_detection.getState()) {
            case 1:
                eventLog.write("Launch detected! Updating byte rate");
                byte_rate = 1;
                max_bytes = 15000000;
                break;
            case 2:
                eventLog.write("Apogee Detected!");
                break;
            case 3:
                //Do a data dump
                if(!sd_dumped) {
                    Serial.println(sd_dumped);
                    eventLog.write("Case 3 reached! Rocket landed");
                    for (File f: device_files) {
                        f.close();
                    }
                    sd_dump();
                    sd_dumped = true;
                }
                break;
        }
    }
}