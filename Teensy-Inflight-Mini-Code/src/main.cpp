#include "BMP.h"
#include "ICM.h"
#include "InternalTemp.h"
#include "Detection.h"
#include "EventLog.h"
#include <LittleFS.h>
#include <SD.h>
#include <Streaming.h>

ICM * icm_device;
BMP * bmp_device;
InternalTemp * internal_temp_device;
Detection state_detection;
const uint8_t num_sensors = 4;
Sensor * devices[num_sensors];
File device_files[num_sensors];
File sd_files[num_sensors];
File sd_files_bin[num_sensors];
bool flash_enable, sd_enable;
bool sd_dumped;
LittleFS_QSPIFlash flash;
SDClass sd;
String sdOutputDir;

void sd_dump() {
        if (flash_enable) {
            //flash.quickFormat();
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
                    devices[i]->dumpToFile(copy_file, sd_files[i]);
                    copy_file.seek(0);
                    file_copy(&copy_file, &sd_files_bin[i], false);
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

void setup()
{
    sd_dumped = false;
    icm_device = new ICM(ICM_CS);
    bmp_device = new BMP(BMP_CS, BMP_INT);
    internal_temp_device = new InternalTemp();
    devices[0] = icm_device;
    devices[1] = bmp_device;
    devices[2] = internal_temp_device;
    devices[3] = &eventLog;
    flash_enable = flash.begin();
    sd_enable = sd.begin(BUILTIN_SDCARD);
    sd_dump();
    byte_rate = 4;
    //byte_rate = 1;
    // Starts UART device
    //Serial1.begin(9600);
    // Initializes SD card- uses BUILTIN_SDCARD to use SDIO with the Teensy's internal SD card slot
    // Checks if QSPI flash is available
    // Counts filenames upwards until it finds a free one on flash storage devices
    // Creates file header
}

void loop()
{
    for(auto & device : devices) {
        device->updateData();
        Serial.println(device->data_struct_ptr->getHeader());
        Serial.println(device->data_struct_ptr->toString());
    }
    Serial.println(bmp_device->sea_level_pressure_hpa);
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
    if (state_detection.updateDetection(&icm_device->data_deque, &bmp_device->data_deque)) {
        switch (state_detection.getState()) {
            case 1:
                //Set data logging to max speed
                Serial.println("Previous byte rate:");
                Serial.println(byte_rate);
                eventLog.write("Launch detected! Updating byte rate");
                byte_rate = 1;
                bmp_device->reset_slp(false);
                break;
            case 2:
                eventLog.write("Apogee Detected!");
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
}