#include "BMP.h"
#include "ICM.h"
#include "InternalTemp.h"
#include "Detection.h"
#include "EventLog.h"
#include <LittleFS.h>
#include <SD.h>
#include <Streaming.h>

/**
 * Pointer to an ICM sensor
 */
ICM * icm_device;
/**
 * Pointer to a BMP sensor
 */
BMP * bmp_device;
/**
 * Pointer to the internal temperature sensor class
 */
InternalTemp * internal_temp_device;
/**
 * State detection object
 */
Detection state_detection;
/**
 * Number of sensors used
 */
const uint8_t num_sensors = 4;
/**
 * Pointer to array of sensor objects
 */
Sensor * devices[num_sensors];
/**
 * Files on flash used as primary storage
 */
File device_files[num_sensors];
/**
 * Files on SD card used as secondary storage
 */
File sd_files[num_sensors];
/**
 * Files on SD card used to dump device data
 */
File sd_files_bin[num_sensors];
/**
 * Boolean enable for if the flash chip works
 */
bool flash_enable;
/**
 * Boolean enable for if the SD card chip works
 */
bool sd_enable;
/**
 * Boolean to set if the SD card has been dumped yet
 */
bool sd_dumped;
/**
 * Device object for the flash chip
 */
LittleFS_QSPIFlash flash;
/**
 * Device object for the SD card
 */
SDClass sd;
/**
 * Directory to store
 */
String sdOutputDir;

/**
 * sd_dump: dumps the data on the flash to the SD card
 */
void sd_dump() {
        // Only try to dump data if thre is a flash chip installed and working
        // Will crash otherwise
        if (flash_enable) {
            // When any of the data structs are changed, the dump program will crash
            // To fix this, uncomment the line below, flash the program, then uncomment it and flash again
            // flash.quickFormat();
            /**
             * Directory all the dump files end up in
             */
            String prepend_dir = "/flash/";
            /**
             * Prefix of directory files will go in
             */
            String dir_name = "flash_dump";
            // Creates the prepend_dir directory if it doesn't already exist
            // TODO: idiotproof this section a bit more
            // I wnt to be able to have minimal functionality without an SD card instead of 0 functionality
            if (!sd.exists(prepend_dir.c_str())) {
                sd.mkdir(prepend_dir.c_str());
            }
            /**
             * Count for how many directories exist=
             */
            uint_fast16_t count = 0;
            /**
             * full path to use for file directory - eg "/flash/flash_dump1/"
             */
            String file_name = prepend_dir + dir_name + "/";
            // Iterates through directories named "flash_dump" + n until it finds an empty one
            while (sd.exists(file_name.c_str())) {
                count++;
                file_name = prepend_dir + dir_name + String(count) + "/";
            }
            // Creates output and sets as named output dir
            sd.mkdir(file_name.c_str());
            // Sets output directory to the one just generated
            sdOutputDir = file_name;
        }
        // Iterates through sensor array, not using any smarter loop since we need the iteration number
        for (uint_fast8_t i = 0; i < num_sensors; i++) {
            /**
             * Output filename for the human-readable CSV version of the data
             */
            String sd_filename = sdOutputDir + devices[i]->getName() + ".csv";
            /**
             * Output filename for the raw byte-by-byte copy of the structs
             * Only really useful if the MCU fucks up at dumping the CSV version of the data, which hasn't happened yet
             */
            String sd_filename_bin = sdOutputDir + devices[i]->getName() + ".bin";
            // Checks if the SD card exists before telling it to open files
            if (sd_enable) {
                /**
                 * Generate CSV and BIN files for each sensor
                 */
                sd_files[i] = sd.open(sd_filename.c_str(), FILE_WRITE);
                sd_files_bin[i] = sd.open(sd_filename_bin.c_str(), FILE_WRITE);
            }
            if (flash_enable) {
                /**
                 * Flash string
                 */
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
                // Creates and opens device files
                    device_files[i] = flash.open(devices[i]->getName().c_str(), FILE_WRITE);
                    devices[i]->setFile(&device_files[i]);

            }
            Serial << "Finished dumping " << devices[i]->getName();
        }
        Serial.println("SD dump finished!");
}

void setup()
{
    // Sets initial setup variables - SD card has not been dumped yet
    sd_dumped = false;
    // Sets device objects
    icm_device = new ICM(ICM_CS);
    bmp_device = new BMP(BMP_CS, BMP_INT);
    internal_temp_device = new InternalTemp();
    //Maps devices array to actual devices
    devices[0] = icm_device;
    devices[1] = bmp_device;
    devices[2] = internal_temp_device;
    devices[3] = &eventLog;
    // Starts the flash chip and SD card
    flash_enable = flash.begin();
    sd_enable = sd.begin(BUILTIN_SDCARD);
    // Initial SD dump to make sure there is no residual data on SD card
    sd_dump();
    // Sets byte rate - 4 initially prior to launch
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
    // Iterates through array of devices, runs sensor update function from derived class
    for(auto & device : devices) {
        device->updateData();
        // Prints gathered data to serial console for debug purposes
        Serial.println(device->data_struct_ptr->getHeader());
        Serial.println(device->data_struct_ptr->toString());
    }
    // Debug prints
    Serial.println(bmp_device->sea_level_pressure_hpa);
    Serial.println(getBlockSize());
    Serial.println(state_detection.getState());

    // Checks if the state is either pre-launch or post-launch and the flash limit is full
    if(getBlockSize() && (state_detection.getState() != 2 || state_detection.getState() != 1)) {
        // Writes that the limit has been reached to the event log
        eventLog.write("Dump limit reached! Dumping to SD");
        // Dumps to SD
        sd_dump();
        // Resets counter now that the dump limit has been reached and dumped
        resetBlockCounter();
    }
    // Reading input for manually changing the state of the software
    if(Serial.available()) {
        /**
         * Stores the serial input as an int to recognize the state as a debug prompt
         */
        int state = Serial.readString().toInt();
        Serial << "Setting state " << state << endl;
        state_detection.setState(state);
    }
    // Debug prints
    Serial.println(icm_device->data_deque.size());
    Serial.println(bmp_device->data_deque.size());
    // State machine for pre-launch/launch/apogee/land below
    if (state_detection.updateDetection(&icm_device->data_deque, &bmp_device->data_deque)) {
        switch (state_detection.getState()) {
            // TODO: Replace with definitions instead of numbers
            case 1: // Pre-launch
                // Writes relevant eventlog message
                eventLog.write("Launch detected! Updating byte rate");
                // Byte rate = 1 = full speed data logging
                byte_rate = 1;
                // Turns off sea level pressure resets by the altimeter to prevent it from setting the altimeter
                // while the rocket is in the air
                bmp_device->reset_slp(false);
                // Sets the system to not try to dump until the flash is completely full
                max_bytes = 15000000;
                break;
            case 2: // Apogee
                // Writes relevant message, is not able to do much else yet
                eventLog.write("Apogee Detected!");
                break;
            case 3: // Land
                //Do a data dump if one has not already been done
                if(!sd_dumped) {
                    Serial.println(sd_dumped);
                    // Writes relevant event message
                    eventLog.write("Case 3 reached! Rocket landed");
                    // Closes all the device files before doing an SD dump
                    for (File f: device_files) {
                        f.close();
                    }
                    // Dumps to SD card
                    sd_dump();
                    sd_dumped = true;
                }
                break;
        }
    }
}