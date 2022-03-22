#include "BMP.h"
#include "BNO.h"
#include "GPS.h"
#include "ICM.h"
#include "InternalTemp.h"
#include "Utilities.h"
#include <LittleFS.h>
#include <SD.h>
#include <Streaming.h>
#include <TeensyDebug.h>

ICM * icm_device;
BNO * bno_device;
GPS * gps_device;
BMP * bmp_device;
InternalTemp * internal_temp_device;

const uint8_t num_sensors = 5;
Sensor * devices[num_sensors];
File device_files[num_sensors];
File sd_files[num_sensors];
File sd_files_bin[num_sensors];
bool flash_enable, sd_enable;
LittleFS_QSPIFlash flash;
SDClass sd;
String sdOutputDir;

void generate_flash_dir() {
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

void setup() {
    debug.begin();
    icm_device = new ICM(ICM_CS, ICM_INT_1, ICM_INT_2);
    bno_device = new BNO(BNO_CS, BNO_RESET, BNO_INT);
    gps_device = new GPS(GPS_BAUD);
    bmp_device = new BMP(BMP_CS, BMP_INT);
    internal_temp_device = new InternalTemp();
    devices[0] = icm_device;
    devices[1] = bno_device;
    devices[2] = bmp_device;
    devices[3] = gps_device;
    devices[4] = internal_temp_device;
    flash_enable = flash.begin();
    sd_enable = sd.begin(BUILTIN_SDCARD);
    generate_flash_dir();
    for(uint_fast8_t i = 0; i < num_sensors; i++) {
        //If you run into an issue getting past the beginning and you changed anything with the structs, uncomment this line and run once
        //flash.remove(devices[i]->getName().c_str());
        File copy_file = flash.open(devices[i]->getName().c_str());
        String sd_filename = sdOutputDir + devices[i]->getName() + ".csv";
        String sd_filename_bin = sdOutputDir + devices[i]->getName() + ".bin";
        sd_files[i] = sd.open(sd_filename.c_str(), FILE_WRITE);
        sd_files_bin[i] = sd.open(sd_filename_bin.c_str(), FILE_WRITE);
        //Dump files to SD card as .bin and .csv
        devices[i]->dumpToFile(copy_file, sd_files[i]);
        copy_file.seek(0);
        file_copy(&copy_file, &sd_files_bin[i], false);
        copy_file.close();
        flash.remove(devices[i]->getName().c_str());
        device_files[i] = flash.open(devices[i]->getName().c_str(), FILE_WRITE);
        devices[i]->setFile(&device_files[i]);
    }
}

void loop() {
    for(auto & device : devices) {
            device->updateData();
            Serial.print(device->data_struct_ptr->toString());
    }
    Serial.println();
}