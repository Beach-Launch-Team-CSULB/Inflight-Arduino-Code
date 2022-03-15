#include <Adafruit_BMP3XX.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_BNO08x.h>
#include <InternalTemperature.h>
#include <SD.h>
#include <TimeLib.h>
#include <TinyGPS++.h>
#include <LittleFS.h>
#include <Streaming.h> //allows for printing with << operator
#include "Utilities.h"

// Buffer size for copying files
// Device enable values- checks if device is enabled, skips trying to talk to it if it is not true
bool icm_enabled, xtsd_enabled, teensy_sd_enabled, gps_enabled, flash_enabled;
device_enables enabled;
// GPS Time Synchronization value- Checks if GPS time is set, sets it once if it is not
// Avoids internal clock going out of sync by updating from GPS too often
bool time_set = false;
// Recorded values from GPS time
int_fast32_t gps_time, gps_speed, gps_date, gps_year, gps_month, gps_day, gps_hour,
    gps_minute, gps_second;
// Recorded not time GPS values
double gps_altitude, gps_latitude, gps_longitude, gps_hdop, gps_course_deg, gps_reading_age;
// Altitude to figure out if we should write to the SD card or not
double altitude = 0;
// Recorded data from BNO08X
float bno_accel[3], bno_mag[3], bno_gyro[3];
// Recorded data from BMP390L
double bmp_temp, bmp_pressure, bmp_altitude;
// Sets if the rocket has launched
bool in_air = false, alt_copy = false;
uint_fast16_t counter = 0;

sensors_event_t icm_accel, icm_gyro, icm_temp;

// Device objects
// QSPI flash memory on bottom of Teensy
LittleFS_QSPIFlash flash;
// High G accel/high dps gyro IMU
Adafruit_ICM20649 icm;
// Altimeter
Adafruit_BMP3XX bmp;
// Regular IMU, this one has a magnetometer
// Is fussy to set up so it needs a direct wired reset pin
Adafruit_BNO08x bno(BNO_RESET);
// GPS class- feed UART GPS output into this, and it will give you intelligible data
// The GPS data itself is serial data from GPSSerial
TinyGPSPlus gps;
// SD card device initializations
SDClass teensy_sd, xtsd;
// Sets initial value of device strings
String sd_string = "output0.csv", xtsd_string = sd_string, flash_string = sd_string;
uint_fast8_t alt_counter = 0;
uint_fast32_t end_timer = 0;
float alt_lockout = 750;

IntervalTimer bmp_timer;
// Initializes records variable- records data that cannot be written to SD card in memory
// struct altitude_nums {
//     float alt;
//     uint_fast32_t time;
//     altitude_nums(float f, uint_fast32_t t) {
//         alt = f;
//         time = t;
//     }
//     altitude_nums() {
//         alt = 0;
//         time = 0;
//     }
// };
// altitude_nums * altitude_list;

void bmp_interrupt() {
    Serial << "1: " << micros() << endl;
    // Stores data into variables for reference
    bmp_altitude = bmp.readAltitude(sea_level_pressure_hpa);
    bmp_temp = bmp.temperature;
    bmp_pressure = bmp.pressure;
    Serial << "2: " << micros() << endl;
}

void bno_interrupt() {
    delay(1);
    Serial.print("BNO interrupt called!");
    delay(1);
    Serial.print(digitalReadFast(BNO_INT));

    // Stores sensor data
    sh2_SensorValue sensor{};
    // Records finding state of variables
    bool found_accel = false, found_gyro = false, found_mag = false;
    // Serial.print("Sensor value retrieved:");
    // Serial.print(sensor.sensorId);
    // Reads sensor data until one of each sensor data point is collected
    while (!found_accel || !found_mag || !found_gyro)
    {
        // Gets sensor event from driver, stores in sensor
        bno.getSensorEvent(&sensor);
        // Stores data point in variable, updates flag
        switch (sensor.sensorId)
        {
            case SENSOR_TYPE_ACCELEROMETER:
                found_accel = true;
                // Serial.print("Reading from accel:");
                bno_accel[0] = sensor.un.accelerometer.x;
                bno_accel[1] = sensor.un.accelerometer.y;
                bno_accel[2] = sensor.un.accelerometer.z;
                break;
            case SENSOR_TYPE_MAGNETIC_FIELD:
                found_mag = true;
                // Serial.print("Reading from mag");
                bno_mag[0] = sensor.un.magneticField.x;
                bno_mag[1] = sensor.un.magneticField.y;
                bno_mag[2] = sensor.un.magneticField.z;
                break;
            case SENSOR_TYPE_ORIENTATION:
                found_gyro = true;
                // Serial.print("Reading from gyro");
                bno_gyro[0] = sensor.un.gyroscope.x;
                bno_gyro[1] = sensor.un.gyroscope.y;
                bno_gyro[2] = sensor.un.gyroscope.z;
                break;
                // Catch all option
            default:
                Serial.print("Sensor type " + String(sensor.sensorId) + " Not being used");
                break;
        }
    }
    // Serial.println("Number of repetitions necessary:" + String(num_repetitions));
}

void icm_interrupt() {
    icm.getEvent(&icm_accel, &icm_gyro, &icm_temp);
}


void setup()
{
    Serial.begin(115200);
    // Uncomment for testing
    // alt_copy = true;
    // Starts GPS UART device
    GPSSerial.begin(GPS_BAUD);
    Serial.print("Start logging\n");
    // Starts BMP390L communication over SPI
    enabled.bmp = bmp.begin_SPI(BMP_CS);
    if (!enabled.bmp)
    {
        Serial.print("BMP Init failed!\n");
    }
    else
    {
        Serial.print("BMP Initialized!\n");
        // Sets output data rate to 200 hz
        bmp.setOutputDataRate(BMP3_ODR_200_HZ);
        // Sets oversampling rates to 32x- will return results more quickly/more results
        bmp.setPressureOversampling(BMP3_OVERSAMPLING_32X);
        bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_32X);
        // Sets filter coefficient
        // idk what this is, should update later
        bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_7);
    }
    // Initializes ICM20649 device
    icm_enabled = icm.begin_SPI(ICM_CS);
    if (!icm_enabled)
    {
        Serial.print("ICM Init failed!\n");
    }
    else
    {
        Serial.print("ICM Initialized!\n");
        // Enables High G and High DPS mode
        icm.setAccelRange(ICM20649_ACCEL_RANGE_30_G);
        icm.setGyroRange(ICM20649_GYRO_RANGE_4000_DPS);
        // Sets output data rate to max supported by sensor
        icm.setAccelRateDivisor(0);
        icm.setGyroRateDivisor(0);
    }
    // Initialized BNO085
    enabled.bno = bno.begin_SPI(BNO_CS, BNO_INT);
    if (!enabled.bno)
    {
        Serial.print("BNO Init failed!\n");
    }
    else
    {
        Serial.print("BNO Initialized!\n");
        // Enable reports for accelerometer, gyroscope, and magnetometer
        // There are other optional reports like raw data and uncalibrated data from each sensor that may potentially
        //  be able to be added
        bno.enableReport(SH2_ACCELEROMETER);
        bno.enableReport(SH2_GYROSCOPE_CALIBRATED);
        bno.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED);
    }
    // Initializes SD card- uses BUILTIN_SDCARD to use SDIO with the Teensy's internal SD card slot
    teensy_sd_enabled = teensy_sd.begin(BUILTIN_SDCARD);
    if (!teensy_sd_enabled)
    {
        Serial.print("Teensy SD Init failed!\n");
    }
    else
    {
        Serial.print("Teensy SD Initialized!\n");
    }
    // Initializes XTSD device over SPI
    xtsd_enabled = xtsd.begin(XTSD_CS);
    if (enabled.bmp)
    {
        altitude = bmp_altitude;
    }
    else if (gps_enabled)
    {
        altitude = gps_altitude;
    }
    if (!xtsd_enabled)
    {
        Serial.print("XTSD Init failed!\n");
    }
    else
    {
        Serial.print("XTSD Initialized!\n");
        // Data copy program
        // If the SD card is installed upon system startup, the system will run this program to dump existing data from
        // the XTSD module to the SD card, and delete the according data.
        if (teensy_sd_enabled && altitude < alt_lockout)
        {
            // Data dumped from the XTSD module is stored in the /XTSD/ folder on the SD card's root,
            // While SD card data is stored on the SD card root directly.
            // If this directory does not exist, create it
            if (!teensy_sd.exists("/xtsd/"))
            {
                teensy_sd.mkdir("/xtsd/");
            }
            Serial.println("Starting Data Dump from XTSD Module");
            // TODO: Read binary data back as .bin dump and .csv excel sheet to SD card
            // When dumping data, will create separate directories for each dump taken
            // Counts upward through taken directories to find first free one, sets as directory to dump to
            uint_fast32_t count = 0;
            String output_directory_name = "/xtsd/xtsd_dump0/";
            while (teensy_sd.exists(output_directory_name.c_str()))
            {
                count++;
                output_directory_name = "/xtsd/xtsd_dump" + String(count) + "/";
            }
            // Creates directory at first not taken folder
            teensy_sd.mkdir(output_directory_name.c_str());
            // Counter for how many files have been copied
            uint_fast16_t dump_num = 0;
            // Iterates between first 1024 possible output data file names
            for (uint_fast16_t i = 0; i < 1024; i++)
            {
                // Current file name being dumped
                String xtsd_file = "output " + String(i) + ".csv";
                // Checks if file exists, dumps if it does
                if (xtsd.exists(xtsd_file.c_str()))
                {
                    Serial.print("Dumping ");
                    Serial.println(xtsd_file);
                    // Increments total for number of files copied
                    dump_num++;
                    // Sets output file path
                    String xtsd_path = output_directory_name + xtsd_file;
                    // Opens file being read from and written to
                    File teensy_copy = teensy_sd.open(xtsd_path.c_str(), FILE_WRITE);
                    File xtsd_copy = xtsd.open(xtsd_file.c_str(), FILE_READ);
                    file_copy(&xtsd_copy, &teensy_copy, true);
                    xtsd.remove(xtsd_file.c_str());
                }
            }
            Serial.println("Copy from XTSD finished");
            Serial.println(String(dump_num) + " Files copied");
        }
    }
    // Checks if QSPI flash is available
    flash_enabled = flash.begin();
    if (!flash_enabled)
    {
        Serial.println("Flash chip not initialized!");
    }
    else
    {
        Serial.println("Flash chip initialized!");
        if (teensy_sd_enabled && altitude < alt_lockout)
        {
            // Teensy SD copy program
            if (!teensy_sd.exists("/flash/"))
            {
                teensy_sd.mkdir("/flash/");
            }
            Serial.println("Starting Data Dump from Flash chip");
            // TODO: Read binary data back as .bin and .csv to SD card
            uint_fast16_t count = 0;
            String file_name = "/flash/flash_dump0/";
            Serial.print("File name:\n");
            Serial.print(file_name);
            while (teensy_sd.exists(file_name.c_str()))
            {
                count++;
                file_name = "/flash/flash_dump" + String(count) + "/";
            }
            teensy_sd.mkdir(file_name.c_str());
            String flash_file = "output0.csv";
            String bin_file = "output0.bin";
            uint_fast16_t flash_count = 0;
            uint_fast16_t dump_num = 0;
            for (uint_fast16_t i = 0; i < 1024; i++)
            {
                if (flash.exists(flash_file.c_str()))
                {
                    dump_num++;
                    Serial.print("Dumping " + String(flash_file.c_str()));
                    String flash_path = file_name + flash_file;
                    String bin_path = file_name + bin_file;
                    File teensy_copy = teensy_sd.open(flash_path.c_str(), FILE_WRITE);
                    File teensy_copy_bin = teensy_sd.open(bin_path.c_str(), FILE_WRITE);
                    File flash_copy = flash.open(flash_file.c_str(), FILE_READ);
                    file_copy(&flash_copy, &teensy_copy_bin, false);
                    csv_parse_dump(&flash_copy, &teensy_copy, true);
                    flash.remove(flash_file.c_str());
                }
                flash_count++;
                flash_file = "output" + String(flash_count) + ".csv";
                bin_file = "output" + String(flash_count) + ".bin";
            }
            Serial.println("Copy from flash finished");
            Serial.println(String(dump_num) + " Files copied");
        }
    }
    // Enables GPS if the module is awake
    gps_enabled = GPSSerial.available() < 0;
    // Counts filenames upwards until it finds a free one on flash storage devices
    uint_fast32_t count = 0;
    if (teensy_sd_enabled)
    {
        while (teensy_sd.exists(sd_string.c_str()))
        {
            count++;
            sd_string = "output" + String(count) + ".csv";
            // Serial.print("writing data to " + sd_string);
        }
        // Serial.print("SD writing to: ");
        // Serial.println(sd_string.c_str());
    }
    if (xtsd_enabled)
    {
        while (xtsd.exists(xtsd_string.c_str()))
        {
            count++;
            xtsd_string = "output" + String(count) + ".csv";
        }
    }
    if (flash_enabled)
    {
        while (flash.exists(flash_string.c_str()))
        {
            count++;
            flash_string = "output" + String(count) + ".csv";
        }
    }
    // Creates file header
    char *file_hdr = new char[1024];
    // Appends data into file header
    sprintf(file_hdr, "Current Time, Time Since Startup, Teeny Internal Temperature");
    // Checks if a given data point is enabled, writes data point if it is
    if (icm_enabled)
    {
        strcat(file_hdr, ", ICM Temperature, ICM Gyro X, ICM Gyro Y, "
                         "ICM Gyro Z, ICM Gyro Velocity X, ICM Gyro Velocity Y, ICM Gyro Velocity Z,"
                         "ICM Gyro Heading, ICM Gyro Pitch, ICM Gyro Roll, ICM Accel X, ICM Accel Y, ICM Accel Z, "
                         "ICM Accel Velocity X, ICM Accel Velocity Y, ICM Accel Velocity Z, ICM Accel Heading, "
                         "ICM Accel Pitch, ICM Accel Roll");
    }
    if (enabled.bmp)
    {
        strcat(file_hdr, ", BMP Temperature, BMP Pressure, BMP Altitude");
    }
    if (enabled.bno)
    {
        strcat(file_hdr, ", BNO Accel X, BNO Accel Y, BNO Accel Z, BNO Gyro X, BNO Gyro Y, BNO Gyro Z, BNO Mag X, "
                         "BNO Mag Y, BNO Mag Z");
    }
    if (gps_enabled)
    {
        strcat(file_hdr, ", GPS Altitude, GPS Year, GPS Month, GPS Day, GPS Hour, GPS Minute, GPS Second,"
                         "GPS Latitude, GPS Longitude, GPS Speed, GPS HDOP, GPS Course Degrees,"
                         "GPS Reading Age");
    }
    strcat(file_hdr, "\n");
    Serial.print(file_hdr);
    // Writes data to file depending on if the storage medium is enabled
    if (teensy_sd_enabled)
    {
        // File teensy_file = teensy_sd.open(sd_string.c_str(), FILE_WRITE);
        // teensy_file.print(file_hdr);//TESTING, ERROR HERE
        // teensy_file.close();
    }
    if (xtsd_enabled)
    {
        // File xtsd_file = xtsd.open(xtsd_string.c_str(), FILE_WRITE);
        // xtsd_file.print(file_hdr);
        // xtsd_file.close();
    }
    if (flash_enabled)
    {
        // File flash_file = flash.open(flash_string.c_str(), FILE_WRITE);
        // flash_file.print(file_hdr);
        // flash_file.close();
    }
    //bmp_timer.begin(bmp_interrupt, 100000);
}




void loop()
{
    // Read data from internal temperature sensor
    int x = 0;
    float teensy_temp = InternalTemperatureClass::readTemperatureC();
    // BMP Temperature, Pressure, and Altitude values
    if (enabled.bmp)
    {
        // Does reading using BMP driver
        bmp_altitude = bmp.readAltitude(sea_level_pressure_hpa);
        // Stores data into variables for reference
        bmp_temp = bmp.temperature;
        bmp_pressure = bmp.pressure;
        // Calculates altitude using sea level pressure above
    }
    // Stores data from ICM into variables
    if (icm_enabled)
    {
        icm.getEvent(&icm_accel, &icm_gyro, &icm_temp);
    }
    if (enabled.bno)
    {
        // Stores sensor data
        sh2_SensorValue sensor{};
        // Records finding state of variables
        bool found_accel = false, found_gyro = false, found_mag = false;
        // Serial.print("Sensor value retrieved:");
        // Serial.print(sensor.sensorId);
        // Reads sensor data until one of each sensor data point is collected
        while (!found_accel || !found_mag || !found_gyro)
        {
            // Gets sensor event from driver, stores in sensor
            bno.getSensorEvent(&sensor);
            // Stores data point in variable, updates flag
            switch (sensor.sensorId)
            {
            case SENSOR_TYPE_ACCELEROMETER:
                found_accel = true;
                // Serial.print("Reading from accel:");
                bno_accel[0] = sensor.un.accelerometer.x;
                bno_accel[1] = sensor.un.accelerometer.y;
                bno_accel[2] = sensor.un.accelerometer.z;
                break;
            case SENSOR_TYPE_MAGNETIC_FIELD:
                found_mag = true;
                // Serial.print("Reading from mag");
                bno_mag[0] = sensor.un.magneticField.x;
                bno_mag[1] = sensor.un.magneticField.y;
                bno_mag[2] = sensor.un.magneticField.z;
                break;
            case SENSOR_TYPE_ORIENTATION:
                found_gyro = true;
                // Serial.print("Reading from gyro");
                bno_gyro[0] = sensor.un.gyroscope.x;
                bno_gyro[1] = sensor.un.gyroscope.y;
                bno_gyro[2] = sensor.un.gyroscope.z;
                break;
            // Catch all option
            default:
                Serial.print("Sensor type " + String(sensor.sensorId) + " Not being used");
                break;
            }
        }
        // Serial.println("Number of repetitions necessary:" + String(num_repetitions));
    }
    if(GPSSerial.available()) {
        while (GPSSerial.available()) {
            // Continuously dumps UART data from GPS into driver
            gps_enabled = true;
            gps.encode(GPSSerial.read());
        }
    }
    if (gps_enabled)
    {
        elapsedMillis()
        // Updates GPS data points
        gps_altitude = gps.altitude.meters();
        gps_year = gps.date.year();
        gps_month = gps.date.month();
        gps_day = gps.date.day();
        gps_hour = gps.time.hour();
        gps_minute = gps.time.minute();
        gps_second = gps.time.second();
        gps_latitude = gps.location.lat();
        gps_longitude = gps.location.lng();
        gps_speed = gps.speed.value();
        gps_hdop = gps.hdop.hdop();
        gps_course_deg = gps.course.deg();
        gps_reading_age = gps.time.age();
        // Timeset function
        // Checks if timestamp is out of date, sync system clock with GPS time if it is
        if (gps.time.age() < 50 && !time_set)
        {
            setTime(gps_hour, gps_minute, gps_second, gps_day, gps_month, gps_year);
            time_set = true;
            Serial.println("Clock synchronized with GPS time");
        }
    }
    String write_str = String(now()) cm millis() cm flts(teensy_temp);
    write_str.reserve(4096);
    // Iterates through sensors, adds values to output string if enabled
    All_the_data toWrite;
    /*
    TODO, NEEDS TO WORK:
    temp needs to be preserved
    gyro measurements
    acceleration x y z

    NICE TO HAVE
    heading pitch roll for both accel and gyro - NOT WORKING


    */
    if (icm_enabled) // ERROR remove icm_gyro.gyro.v, icm_gyro.gyro.pitch;roll;heading,
    {
        // Serial << "BEGIN " << write_str << endl;
        write_str += "," +
                flts(icm_temp.temperature) cm flts(icm_gyro.gyro.x) cm flts(icm_gyro.gyro.y) cm
                flts(icm_gyro.gyro.z) cm flts(icm_gyro.gyro.v[0]) cm
                flts(icm_gyro.gyro.v[1]) cm flts(icm_gyro.gyro.v[2]) cm flts(icm_gyro.gyro.heading) cm
                flts(icm_gyro.gyro.pitch) cm flts(icm_gyro.gyro.roll) cm flts(icm_accel.acceleration.x) cm
                flts(icm_accel.acceleration.y) cm flts(icm_accel.acceleration.z) cm
                flts(icm_accel.acceleration.v[0]) cm flts(icm_accel.acceleration.v[1]) cm
                flts(icm_accel.acceleration.v[2]) cm flts(icm_accel.acceleration.heading) cm
                flts(icm_accel.acceleration.pitch) cm flts(icm_accel.acceleration.roll);
        // write to icm_data struct in All_the_data
        // icm_temp, icm_gyro, icm_accel
        toWrite.icm_data.init(icm_accel, icm_gyro, icm_temp);
        // Serial << "\nHERE" << endl;
        // Serial << endl << write_str << endl;
        // toWrite.icm_data.print();
        // Serial << endl;
    }

    if (enabled.bmp)
    {
        write_str += "," + flts(bmp_temp) cm flts(bmp_pressure)
        cm flts(bmp_altitude);
        toWrite.altimeter_data.init(bmp_temp, bmp_pressure, bmp_altitude);
    }
    if (enabled.bno)
    {
        write_str += "," + flts(bno_accel[0]) cm flts(bno_accel[1]) cm flts(bno_accel[2]) cm
                               flts(bno_gyro[0]) cm flts(bno_gyro[1]) cm flts(bno_gyro[2]) cm flts(bno_mag[0]) cm
                                   flts(bno_mag[1]) cm flts(bno_mag[2]);
        toWrite.bno_data.init(bno_accel, bno_gyro, bno_mag);
        // Serial << "\n\nbno_data: ";
        // toWrite.bno_data.print();
        // Serial << endl
        //        << endl;
    }
    if (gps_enabled)
    {
        write_str += "," + flts(gps_altitude) cm gps_year cm gps_month cm gps_day cm gps_hour cm gps_minute
                               cm gps_second cm
                               flts(gps_latitude)
        cm flts(gps_longitude)
        cm gps_speed cm flts(gps_hdop)
        cm flts(gps_course_deg)
        cm flts(gps_reading_age);

        toWrite.gps_data.init(gps);
        // Serial << "GPS DATA: " << endl;
        // toWrite.gps_data.print();
    }
    write_str.append("\n");
    Serial.print(write_str);
    // Writes data to storage medium
    // TODO: Write binary data here instead of a string
    // TODO: Write with specific set frequency until launch, implement roughly here
    // TODO: Add radio send code approximately here
    if (xtsd_enabled)
    {
        // ERROR FREQUENTLY FAILS TO INITIALIZE

        File xtsd_file = xtsd.open(xtsd_string.c_str(), FILE_WRITE);
        xtsd_file.write(&toWrite, sizeof(toWrite));
        // xtsd_file.print(write_str);
        xtsd_file.close();
    }
    if (flash_enabled)
    {
        File flash_file = flash.open(flash_string.c_str(), FILE_WRITE);
        flash_file.write(&toWrite, sizeof(toWrite));
        flash_file.flush();
        // testing
    }
    // SD card altitude lockout- Updates altitude from either GPS or altimeter
    // TODO: Apogee, launch, and land detection go here
    if (teensy_sd_enabled)
    {
        if (enabled.bmp)
        {
            altitude = bmp_altitude;
        }
        else if (gps_enabled)
        {
            altitude = gps_altitude;
        }
        if (altitude > alt_lockout)
        {
            in_air = true;
            end_timer = millis() + 12000;
        }
        else if (in_air && altitude < alt_lockout && flash_enabled && !alt_copy && end_timer < millis())
        {
            File teensy_file = teensy_sd.open(sd_string.c_str(), FILE_WRITE);
            File flash_file = flash.open(flash_string.c_str(), FILE_READ);
            file_copy(&flash_file, &teensy_file, false);
            flash.remove(flash_string.c_str());
            alt_copy = true;
        }
        if (alt_copy || !flash_enabled)
        {
            File teensy_file = flash.open(sd_string.c_str(), FILE_WRITE);
            // teensy_file.print(write_str);
            teensy_file.write(&toWrite, sizeof(toWrite));
            teensy_file.close();
        }
    }
    counter++;
//    if (counter > 4096 && teensy_sd_enabled)
//    {
//        Serial.print("Data dump");
//        counter = 0;
//        File flash_file = flash.open(flash_string.c_str());
//        File teensy_file = teensy_sd.open(sd_string.c_str());
//        file_copy(&flash_file, &teensy_file, true);
//        flash.remove(flash_string.c_str());
//    }
}