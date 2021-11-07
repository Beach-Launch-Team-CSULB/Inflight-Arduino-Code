#include <Arduino.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_ICM20948.h>
#include <InternalTemperature.h>
#include <SD.h>
#include <TimeLib.h>
#include <LittleFS.h>
#define cm + "," +
//Macro to make appending strings easier- cm stands for comma
//Pin numbers used by devices
const uint_fast8_t BMP_CS = 4, ICM_CS = 10;
//Approximate air pressure in Randsburg, CA
const float sea_level_pressure_hpa = 1015.578;
//Precision values to use when converting from float to decimal
const u_char float_depth = 18;
//Buffer sie for copying files
const uint_fast16_t buffer_size = 128;
//Device enable values- checks if device is enabled, skips trying to talk to it if it is not true
bool bmp_enabled, icm_enabled, teensy_sd_enabled, flash_enabled;
//Recorded data from BMP390L
float bmp_temp, bmp_pressure, bmp_altitude = 0;
//SD card lockout variables
//For ground testing, set alt_copy to true
float alt_lockout = 750;
bool in_air = false, alt_copy = false;
uint_fast32_t end_time = 0;
String * flight_computer_msg;

//Device objects
//QSPI flash memory on bottom of Teensy
LittleFS_QSPIFlash flash;
//High G accel/high dps gyro IMU
Adafruit_ICM20948 icm;
//Altimeter
Adafruit_BMP3XX bmp;
//SD card device initializations
SDClass teensy_sd;
//Sets initial value of device strings
String sd_string = "output0.csv", flash_string = sd_string;

//FLTS function- returns float as string with full precision
String flts(float s) {
    return {s, float_depth};
}

void file_copy(File * copy, File * paste, bool close) {
    tempmonGetTemp();
    //Reads data from Flash to buffer, writes data from buffer to SD card
    //Stores size of buffer
    size_t n;
    //Stores data being copied
    auto *buf = new uint8_t[buffer_size];
    //Continuously copies data from file to buffer, writes data from buffer to file
    while ((n = copy->read(buf, sizeof(buf))) > 0) {
        paste->write(buf, n);
    }
    //Frees buffer from memory
    delete[] buf;
    Serial.print("Dumping " + String(copy->name()) + " Finished");
    //Closes both files
    if(close) {
        copy->close();
        paste->close();
    }
}
void setup() {
    //Starts UART device
    Serial1.begin(9600);
    flight_computer_msg = new String();
    Serial.print("Start logging\n");
    //Starts BMP390L communication over SPI
    bmp_enabled = bmp.begin_SPI(BMP_CS);
    if (!bmp_enabled) {
        Serial.print("BMP Init failed!\n");
    }
    else {
        Serial.print("BMP Initialized!\n");
        //Sets output data rate to 200 hz
        bmp.setOutputDataRate(BMP3_ODR_200_HZ);
        //Sets oversampling rates to 32x- will return results more quickly/more results
        bmp.setPressureOversampling(BMP3_OVERSAMPLING_32X);
        bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_32X);
        //Sets filter coefficient
        //idk what this is, should update later
        bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    }
    //Initializes ICM20649 device
    icm_enabled = icm.begin_SPI(ICM_CS);
    if (!icm_enabled) {
        Serial.print("ICM Init failed!\n");
    }
    else {
        Serial.print("ICM Initialized!\n");
        //Enables High G and High DPS mode
        icm.setAccelRange(ICM20948_ACCEL_RANGE_16_G);
        icm.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);
        //Sets output data rate to max supported by sensor
        icm.setAccelRateDivisor(0);
        icm.setGyroRateDivisor(0);
    }
    //Initializes SD card- uses BUILTIN_SDCARD to use SDIO with the Teensy's internal SD card slot
    teensy_sd_enabled = teensy_sd.begin(BUILTIN_SDCARD);
    if (!teensy_sd_enabled) {
        Serial.print("Teensy SD Init failed!\n");
    } else {
        Serial.print("Teensy SD Initialized!\n");
    }
    //Checks if QSPI flash is available
    flash_enabled = flash.begin();

    if (!flash_enabled) {
        Serial.println("Flash chip not initialized!");
    } else {
        Serial.println("Flash chip initialized!");
        if (teensy_sd_enabled && bmp_altitude < alt_lockout) {
            //Teensy SD copy program
            if (!teensy_sd.exists("/flash/")) {
                teensy_sd.mkdir("/flash/");
            }
            Serial.println("Starting Data Dump from Flash chip");
            uint_fast16_t count = 0;
            String file_name = "/flash/flash_dump0/";
            Serial.print("File name:\n");
            Serial.print(file_name);
            while (teensy_sd.exists(file_name.c_str())) {
                count++;
                file_name = "/flash/flash_dump" + String(count) + "/";
            }
            teensy_sd.mkdir(file_name.c_str());
            String flash_file = "output0.csv";
            uint_fast16_t flash_count = 0;
            uint_fast16_t dump_num = 0;
            for (uint_fast16_t i = 0; i < 1024; i++) {
                if (flash.exists(flash_file.c_str())) {
                    dump_num++;
                    Serial.print("Dumping " + String(flash_file.c_str()));
                    String flash_path = file_name + flash_file;
                    File teensy_copy = teensy_sd.open(flash_path.c_str(), FILE_WRITE);
                    File flash_copy = flash.open(flash_file.c_str(), FILE_READ);
                    Serial.print("Copying " + flash_file + " To " + flash_path);
                    file_copy(&flash_copy, &teensy_copy, true);
                    Serial.print("Deleting " + flash_file);
                    flash.remove(flash_file.c_str());
                }
                flash_count++;
                flash_file = "output" + String(flash_count) + ".csv";
            }
            Serial.println("Copy from flash finished");
            Serial.println(String(dump_num) + " Files copied");
        }
    }
    //Counts filenames upwards until it finds a free one on flash storage devices
    uint_fast32_t count = 0;
    if (teensy_sd_enabled) {
        while (teensy_sd.exists(sd_string.c_str())) {
            count++;
            sd_string = "output" + String(count) + ".csv";
            //Serial.print("writing data to " + sd_string);
        }
        //Serial.print("SD writing to: ");
        //Serial.println(sd_string.c_str());
    }
    if (flash_enabled) {
        while (flash.exists(flash_string.c_str())) {
            count++;
            flash_string = "output" + String(count) + ".csv";
        }
    }
    //Creates file header
    char * file_hdr = new char[1024];
    //Appends data into file header
    sprintf(file_hdr, "Current Time, Time Since Startup, Teeny Internal Temperature");
    //Checks if a given data point is enabled, writes data point if it is
    if (icm_enabled) {
        strcat(file_hdr, ", ICM Temperature, ICM Gyro X, ICM Gyro Y, "
                         "ICM Gyro Z, ICM Gyro Velocity X, ICM Gyro Velocity Y, ICM Gyro Velocity Z,"
                         "ICM Gyro Heading, ICM Gyro Pitch, ICM Gyro Roll, ICM Accel X, ICM Accel Y, ICM Accel Z, "
                         "ICM Accel Velocity X, ICM Accel Velocity Y, ICM Accel Velocity Z, ICM Accel Heading, "
                         "ICM Accel Pitch, ICM Accel Roll");
    }
    if (bmp_enabled) {
        strcat(file_hdr, ", BMP Temperature, BMP Pressure, BMP Altitude");
    }
    strcat(file_hdr, ", Flight Computer Message\n");
    Serial.print(file_hdr);
    //Writes data to file depending on if the storage medium is enabled
    if (teensy_sd_enabled) {
        File teensy_file = teensy_sd.open(sd_string.c_str(), FILE_WRITE);
        teensy_file.print(file_hdr);
        teensy_file.close();
    }
    if(flash_enabled) {
        File flash_file = flash.open(flash_string.c_str(), FILE_WRITE);
        flash_file.print(file_hdr);
        flash_file.close();
    }
}

void loop() {
    //Read data from internal temperature sensor
    float teensy_temp = InternalTemperatureClass::readTemperatureC();
    if(Serial1.available()) {
        delete flight_computer_msg;
        flight_computer_msg = new String();
        while (Serial1.available()) {
            flight_computer_msg->append(Serial1.read());
        }
    }
    flight_computer_msg->replace('\n', ' ');
    //BMP Temperature, Pressure, and Altitude values
    if (bmp_enabled) {
        //Does reading using BMP driver
        bmp.performReading();
        //Stores data into variables for reference
        bmp_temp = bmp.readTemperature();
        bmp_pressure = bmp.readPressure();
        //Calculates altitude using sea level pressure above
        bmp_altitude = bmp.readAltitude(sea_level_pressure_hpa);
    }
    //Stores data from ICM into variables
    sensors_event_t icm_accel, icm_gyro, icm_temp;
    if (icm_enabled) {
        icm.getEvent(&icm_accel, &icm_gyro, &icm_temp);
    }
    String write_str = String(now()) cm millis() cm flts(teensy_temp);
    write_str.reserve(4096);
    //Iterates through sensors, adds values to output string if enabled
    if (icm_enabled) {
        write_str += "," + flts(icm_temp.temperature) cm flts(icm_gyro.gyro.x) cm
                     flts(icm_gyro.gyro.y) cm
                     flts(icm_gyro.gyro.z) cm flts(icm_gyro.gyro.v[0]) cm
                     flts(icm_gyro.gyro.v[1]) cm flts(icm_gyro.gyro.v[2]) cm flts(icm_gyro.gyro.heading) cm
                     flts(icm_gyro.gyro.pitch) cm flts(icm_gyro.gyro.roll) cm flts(icm_accel.acceleration.x) cm
                     flts(icm_accel.acceleration.y) cm flts(icm_accel.acceleration.z) cm
                     flts(icm_accel.acceleration.v[0]) cm flts(icm_accel.acceleration.v[1]) cm
                     flts(icm_accel.acceleration.v[2]) cm flts(icm_accel.acceleration.heading) cm
                     flts(icm_accel.acceleration.pitch) cm flts(icm_accel.acceleration.roll);
    }
    if (bmp_enabled) {
        write_str += "," + flts(bmp_temp) cm flts(bmp_pressure) cm flts(bmp_altitude);
    }
    write_str.append(flight_computer_msg->c_str());
    write_str.append("\n");
    //Writes data to storage medium
    if (flash_enabled) {
        File flash_file = flash.open(flash_string.c_str(), FILE_WRITE);
        flash_file.print(write_str);
        flash_file.close();
    }
    //SD card altitude lockout- Updates altitude from either GPS or altimeter
    if (teensy_sd_enabled) {
        if(bmp_altitude > alt_lockout) {
            in_air = true;
            end_time = millis() + 120000;
        }
        else if (bmp_altitude < alt_lockout && flash_enabled && !alt_copy && in_air && end_time < millis()) {
            File teensy_file = teensy_sd.open(sd_string.c_str(), FILE_WRITE);
            File flash_file = flash.open(flash_string.c_str(), FILE_READ);
            file_copy(&flash_file, &teensy_file, false);
            teensy_file.close();
            flash_file.close();
            flash.remove(flash_string.c_str());
            alt_copy = true;
        }
        if(alt_copy || !flash_enabled) {
            File teensy_file = flash.open(sd_string.c_str(), FILE_WRITE);
            teensy_file.print(write_str);
            teensy_file.close();
        }
    }

    if (Serial) {
        Serial.print(write_str);
    }
    // delay(1000);
}