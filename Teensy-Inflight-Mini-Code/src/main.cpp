#include <Arduino.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_ICM20948.h>
#include <InternalTemperature.h>
#include <SD.h>
#include <TimeLib.h>
#include <LittleFS.h>
#include <deque>
#define cm +"," +

// Macro to make appending strings easier- cm stands for comma
// Pin numbers used by devices
const uint_fast8_t BMP_CS = 4, ICM_CS = 10;
// Approximate air pressure in Randsburg, CA
const float sea_level_pressure_hpa = 1015.578;
// Precision values to use when converting from float to decimal
const u_char float_depth = 18;
// Buffer sie for copying files
// const uint_fast16_t buffer_size = 128;
// Device enable values- checks if device is enabled, skips trying to talk to it if it is not true
bool bmp_enabled, icm_enabled, teensy_sd_enabled, flash_enabled;
// SD card lockout variables
// For ground testing, set alt_copy to true
float alt_lockout = 750;
bool in_air = false;
bool alt_copy = false;
uint_fast32_t end_time = 0;
String *flight_computer_msg;

// Device objects
// QSPI flash memory on bottom of Teensy
LittleFS_QSPIFlash flash;
// High G accel/high dps gyro IMU
Adafruit_ICM20948 icm;
// Altimeter
Adafruit_BMP3XX bmp;
// SD card device initializations
SDClass teensy_sd;
// Sets initial value of device strings
String sd_string = "output0.csv", flash_string = sd_string;
uint_fast16_t counter = 0;
/////////////////////////////////////
// DETECTION FLAGS
//<-----
bool launch = false;
int launchIndex;
float launchTime;

bool apogee = false;
int apogeeIndex;
float apogeeTime;

bool land = false;
int landIndex;
float landTime;
//------->
// Used track the acceleration change over a range of readings
float vertAccelAvg1;
float vertAccelAvg2;

float altitudeAvg1;
float altitudeAvg2;

float sum1 = 0;
float sum2 = 0;

//****CONFIG: these variables will be adjusted based on the hardware situation****
// set how many values you will be calculating average from
// TODO SHAWN adust these for specific hardware setup
int bufferSize = 30;

std::deque<float> accelYBuff;
// int accelYIndex = 2;

std::deque<float> altBuff;
// int altitudeIndex = 0;
// set this to whatever the altitude should be under for landing
float ALTITUDE_LANDING_THRESHOLD = 5;

// these represent the values coming in from the sensors
// vector<float> values;
/////////////////////////////////////

struct
{
    sensors_event_t accel, gyro, temp;
} icm_data;
struct icm_sensor_data_t
{
    float reading[3]{};
    float velocity[3]{};
    float pitch{};
    float heading{};
    float roll{};
    icm_sensor_data_t(float reading_x, float reading_y, float reading_z, const float *vel, float pitch_val, float heading_val, float roll_val)
    {
        reading[0] = reading_x;
        reading[1] = reading_y;
        reading[2] = reading_z;
        velocity[0] = vel[0];
        velocity[1] = vel[1];
        velocity[2] = vel[2];
        pitch = pitch_val;
        heading = heading_val;
        roll = roll_val;
    }
    icm_sensor_data_t() = default;
};
struct bmp_data_t
{
    double temp = 0;
    double pres = 0;
    double alt = 0;
};
struct icm_data_t
{
    icm_sensor_data_t accel, gyro;
    float temp{};
};
struct sensor_data_t
{
    icm_data_t icm_data;
    uint8_t check_val = 239;
    bmp_data_t bmp_data{};
    time_t reading_time{};
    uint32_t reading_millis{};
    float teensy_int_temp{};
} sensor_data;

// FLTS function- returns float as string with full precision
String flts(double s)
{
    return {s, float_depth};
}
String flts(float s)
{
    return {s, float_depth};
}

void file_copy(File *copy, File *paste, bool close)
{
    //    Reads data from Flash to buffer, writes data from buffer to SD card
    //    Stores size of buffer
    size_t n;
    // Stores data being copied
    uint_fast32_t buffer_size = 128;
    auto *buf = new uint8_t[buffer_size];
    // Continuously copies data from file to buffer, writes data from buffer to file
    while ((n = copy->read(buf, sizeof(buf))) > 0)
    {
        paste->write(buf, n);
    }
    // Frees buffer from memory
    delete[] buf;
    Serial.print("Dumping " + String(copy->name()) + " Finished");
    // Closes both files
    if (close)
    {
        copy->close();
        paste->close();
    }
    //    size_t bytes_copied = 1;
    //    bool hdr = false;
    //    byte copy_data [sizeof(sensor_data_t)];
    //    while(bytes_copied > 0) {
    //        bytes_copied = copy->read(copy_data, sizeof(sensor_data_t));
    //        auto * sensor_out = (sensor_data_t*)(&copy_data);
    //        if(!hdr) {
    //            String write_hdr = "Reading time, Reading millis, Teensy internal temp, ICM temperature, "
    //                               "ICM gyro x, ICM gyro y, ICM gyro z, ICM gyro vel x, ICM gyro vel y, ICM gyro vel z. "
    //                               "ICM gyro heading, ICM gyro pitch, ICM gyro roll, ICM accel x, ICM accel y, ICM accel z, "
    //                               "ICM accel vel x, ICM accel vel y, ICM accel vel z, ICM accel heading, ICM accel pitch, ICM accel roll";
    //            paste->print(write_hdr);
    //            hdr = true;
    //        }
    //        if(sensor_out->check_val == 239) {
    //            Serial.print("Check val passed!");
    //            paste->write("Check val passed!");
    //            String write_str = String(sensor_out->reading_time) cm sensor_out->reading_millis cm flts(
    //                    sensor_out->teensy_int_temp) +
    //                               "," + flts(sensor_out->icm_data.temp) cm flts(sensor_out->icm_data.gyro.reading[0]) cm
    //                               flts(sensor_out->icm_data.gyro.reading[1]) cm
    //                               flts(sensor_out->icm_data.gyro.reading[2]) cm flts(
    //                    sensor_out->icm_data.gyro.velocity[0]) cm
    //                               flts(sensor_out->icm_data.gyro.velocity[1]) cm flts(
    //                    sensor_out->icm_data.gyro.velocity[2]) cm flts(sensor_out->icm_data.gyro.heading) cm
    //                               flts(sensor_out->icm_data.gyro.pitch) cm flts(sensor_out->icm_data.gyro.roll) cm flts(
    //                    sensor_out->icm_data.accel.reading[0]) cm
    //                               flts(sensor_out->icm_data.accel.reading[1]) cm flts(
    //                    sensor_out->icm_data.accel.reading[2]) cm
    //                               flts(sensor_out->icm_data.accel.velocity[0]) cm flts(
    //                    sensor_out->icm_data.accel.velocity[1]) cm
    //                               flts(sensor_out->icm_data.accel.velocity[2]) cm flts(
    //                    sensor_out->icm_data.accel.heading) cm
    //                               flts(sensor_out->icm_data.accel.pitch) cm flts(sensor_out->icm_data.accel.roll);
    //            paste->print(write_str.c_str());
    //        }
    //        else {
    //            paste->println("Data check failed! Value is " + String((char*)(&copy_data)) + " Byte value is " +
    //            String(sizeof(bytes_copied)) + "Real size is " + String(sizeof(sensor_data_t)));
    //        }
}
//    Serial.print("Dumping " + String(copy->name()) + " Finished");
//    if(close) {
//        copy->close();
//        paste->close();
//    }
void setup()
{
    // Starts UART device
    Serial1.begin(9600);
    flight_computer_msg = new String();
    Serial.print("Start logging\n");
    // Starts BMP390L communication over SPI
    bmp_enabled = bmp.begin_SPI(BMP_CS);
    if (!bmp_enabled)
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
        bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    }
    // Initializes ICM20649 device
    icm_enabled = icm.begin_SPI(ICM_CS);
    if (!icm_enabled)
    {
        Serial.print("ICM Init failed!\n");
    }
    // hello
    else
    {
        Serial.print("ICM Initialized!\n");
        // Enables High G and High DPS mode
        icm.setAccelRange(ICM20948_ACCEL_RANGE_16_G);
        icm.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);
        // Sets output data rate to max supported by sensor
        icm.setAccelRateDivisor(0);
        icm.setGyroRateDivisor(0);
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
    // Checks if QSPI flash is available
    flash_enabled = flash.begin();

    if (!flash_enabled)
    {
        Serial.println("Flash chip not initialized!");
    }
    else
    {
        Serial.println("Flash chip initialized!");
        if (teensy_sd_enabled && sensor_data.bmp_data.alt < alt_lockout)
        {
            // Teensy SD copy program
            if (!teensy_sd.exists("/flash/"))
            {
                teensy_sd.mkdir("/flash/");
            }
            Serial.println("Starting Data Dump from Flash chip");
            uint_fast16_t count = 0;
            String file_name = "/flash/flash_dump0/";
            Serial.println("File name:");
            Serial.println(file_name);
            while (teensy_sd.exists(file_name.c_str()))
            {
                count++;
                file_name = "/flash/flash_dump" + String(count) + "/";
            }
            teensy_sd.mkdir(file_name.c_str());
            String flash_file = "output0.csv";
            uint_fast16_t flash_count = 0;
            uint_fast16_t dump_num = 0;
            for (uint_fast16_t i = 0; i < 1024; i++)
            {
                if (flash.exists(flash_file.c_str()))
                {
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
    if (bmp_enabled)
    {
        strcat(file_hdr, ", BMP Temperature, BMP Pressure, BMP Altitude");
    }
    strcat(file_hdr, ", Flight Computer Message\n");
    Serial.print(file_hdr);
    // Writes data to file depending on if the storage medium is enabled
    if (teensy_sd_enabled)
    {
        File teensy_file = teensy_sd.open(sd_string.c_str(), FILE_WRITE);
        teensy_file.print(file_hdr);
        teensy_file.close();
    }
    if (flash_enabled)
    {
        File flash_file = flash.open(flash_string.c_str(), FILE_WRITE);
        flash_file.print(file_hdr);
        flash_file.close();
    }
}

void loop()
{
    // Read data from internal temperature sensor
    sensor_data.teensy_int_temp = InternalTemperatureClass::readTemperatureC();
    if (Serial1.available())
    {
        delete flight_computer_msg;
        flight_computer_msg = new String();
        while (Serial1.available())
        {
            flight_computer_msg->append(Serial1.read());
        }
    }
    flight_computer_msg->replace('\n', ' ');
    // BMP Temperature, Pressure, and Altitude values
    if (bmp_enabled)
    {
        // Calculates altitude using sea level pressure
        sensor_data.bmp_data.alt = bmp.readAltitude(sea_level_pressure_hpa);
        // Stores data into variables for reference
        sensor_data.bmp_data.temp = bmp.temperature;
        sensor_data.bmp_data.pres = bmp.pressure;
    }
    // Stores data from ICM into variables
    if (icm_enabled)
    {
        icm.getEvent(&icm_data.accel, &icm_data.gyro, &icm_data.temp);
        sensor_data.icm_data.accel = icm_sensor_data_t(icm_data.accel.acceleration.x,
                                                       icm_data.accel.acceleration.y,
                                                       icm_data.accel.acceleration.z,
                                                       icm_data.accel.acceleration.v,
                                                       icm_data.accel.acceleration.pitch,
                                                       icm_data.accel.acceleration.heading,
                                                       icm_data.accel.acceleration.roll);
        sensor_data.icm_data.gyro = icm_sensor_data_t(icm_data.accel.gyro.x,
                                                      icm_data.accel.gyro.y,
                                                      icm_data.accel.gyro.z,
                                                      icm_data.accel.gyro.v,
                                                      icm_data.accel.gyro.pitch,
                                                      icm_data.accel.gyro.heading,
                                                      icm_data.accel.gyro.roll);
        sensor_data.icm_data.temp = icm_data.temp.temperature;
    }
    sensor_data.reading_time = now();
    sensor_data.reading_millis = millis();
    String write_str = String(now()) cm millis() cm flts(sensor_data.teensy_int_temp);
    write_str.reserve(4096);
    ////////////////////////////////////////////
    //***INFLIGHT DETECTION***
    if (accelYBuff.size() >= bufferSize)
    {
        accelYBuff.pop_back();
    }
    if (altBuff.size() >= bufferSize)
    {
        altBuff.pop_back();
    }
    // Which sensor to use
    accelYBuff.push_front(icm_data.accel.acceleration.y);

    //**NOTE** altitude data is in a different csv file
    altBuff.push_front(sensor_data.bmp_data.alt);

    //<--- Get the averages for vertical acceleration
    sum1 = 0;
    sum2 = 0;
    for (int i = 0; i < accelYBuff.size(); i++)
    {
        if (i < accelYBuff.size() / 2)
        {
            sum1 += accelYBuff.at(i);
        }
        else
        {
            sum2 += accelYBuff.at(i);
        }
    }
    vertAccelAvg1 = sum1 / (accelYBuff.size() / 2);
    vertAccelAvg2 = sum2 / (accelYBuff.size() / 2);
    //---->

    //<--- Get the averages for altitude
    sum1 = 0;
    sum2 = 0;
    for (int i = 0; i < altBuff.size(); i++)
    {
        if (i < altBuff.size() / 2)
        {
            sum1 += altBuff.at(i);
        }
        else
        {
            sum2 += altBuff.at(i);
        }
    }
    altitudeAvg1 = sum1 / (altBuff.size() / 2);
    altitudeAvg2 = sum2 / (altBuff.size() / 2);
    //---->

    // cout << "Avg1: " << (avg1);
    // cout << "Avg2: " << (avg2);

    float verticalAcceleration = vertAccelAvg1 - vertAccelAvg2;
    // cout<< verticalAcceleration;
    // LAUNCH CONDITIONS
    if (!launch && (verticalAcceleration > 2 || verticalAcceleration < -2) && accelYBuff.size() == bufferSize)
    {

        Serial.println("**Launch detected***");
        Serial.println(verticalAcceleration);
        launch = true;
        // launchIndex = rows;
        launchTime = sensor_data.reading_time;
    }

    // landing altitude threshold occurs at index 1190581636 | the actual value is 4.67 IN ALT_002.csv from 08/14/2021
    // the real code should check the altitude is under 5
    // //todo Remove the line and add a condition to check the (altitude < 5)
    // LAND CONDITIONS
    if (!land && altitudeAvg1 < ALTITUDE_LANDING_THRESHOLD && launch && (verticalAcceleration > 2 || verticalAcceleration < -2))
    {
        // note: if apogee detection ends up working well could
        //  add that as a condition for land
        land = true;
        // landIndex = rows;
        landTime = sensor_data.reading_time;
    }
    // todo add a condition for being high enough in the air.
    // APOGEE CONDITION
    if (launch && !land && verticalAcceleration < 2 && verticalAcceleration < -2)
    {
        // todo increase precision/accuracy for these conditions
        apogee = true;
        // apogeeIndex = rows;
        apogeeTime = sensor_data.reading_time;
    }

    // cout << values[0] << " | " << verticalAcceleration << " | " << launch << " | " << land << " | " << apogee << "\n";
    ////////////////////////////////////////////

    // Iterates through sensors, adds values to output string if enabled
    if (icm_enabled)
    {
        write_str += "," + flts(sensor_data.icm_data.temp) cm flts(sensor_data.icm_data.gyro.reading[0]) cm
                               flts(sensor_data.icm_data.gyro.reading[1]) cm
                                   flts(sensor_data.icm_data.gyro.reading[2]) cm flts(sensor_data.icm_data.gyro.velocity[0]) cm
                                       flts(sensor_data.icm_data.gyro.velocity[1]) cm flts(sensor_data.icm_data.gyro.velocity[2]) cm flts(sensor_data.icm_data.gyro.heading) cm
                                           flts(sensor_data.icm_data.gyro.pitch) cm flts(sensor_data.icm_data.gyro.roll) cm flts(sensor_data.icm_data.accel.reading[0]) cm
                                               flts(sensor_data.icm_data.accel.reading[1]) cm flts(sensor_data.icm_data.accel.reading[2]) cm
                                                   flts(sensor_data.icm_data.accel.velocity[0]) cm flts(sensor_data.icm_data.accel.velocity[1]) cm
                                                       flts(sensor_data.icm_data.accel.velocity[2]) cm flts(sensor_data.icm_data.accel.heading) cm
                                                           flts(sensor_data.icm_data.accel.pitch) cm flts(sensor_data.icm_data.accel.roll);
    }
    if (bmp_enabled)
    {
        write_str += "," + flts(sensor_data.bmp_data.temp) cm flts(sensor_data.bmp_data.pres) cm flts(sensor_data.bmp_data.alt);
    }
    write_str.append(flight_computer_msg->c_str());
    write_str.append("\n");
    // Writes data to storage medium
    if (flash_enabled)
    {
        File flash_file = flash.open(flash_string.c_str(), FILE_WRITE);
        // flash_file.write(&sensor_data, sizeof(sensor_data_t));
        flash_file.print(write_str);
    }
    // SD card altitude lockout- Updates altitude from either GPS or altimeter
    if (teensy_sd_enabled)
    {
        if (sensor_data.bmp_data.alt > alt_lockout && millis() > 2000)
        {
            double alc = sensor_data.bmp_data.alt;
            delay(100);
            Serial.println("In Air! " + String(alc));
            delay(100);
            in_air = true;
            end_time = millis() + 120000;
        }
        else if (sensor_data.bmp_data.alt < alt_lockout && flash_enabled && !alt_copy && in_air && end_time < millis())
        {
            Serial.println("Sensor dump triggered!");
            File teensy_file = teensy_sd.open(sd_string.c_str(), FILE_WRITE);
            File flash_file = flash.open(flash_string.c_str(), FILE_READ);
            file_copy(&flash_file, &teensy_file, false);
            teensy_file.close();
            flash_file.close();
            flash.remove(flash_string.c_str());
            Serial.println("Sensor dump completed");
            alt_copy = true;
        }
        if (alt_copy || !flash_enabled)
        {
            Serial.println("Direct SD Writing enabled!");
            File teensy_file = flash.open(sd_string.c_str(), FILE_WRITE);
            //            const char * fwr_dat = (char*)(&sensor_data);
            //            teensy_file.write(fwr_dat);
            teensy_file.print(write_str);
            teensy_file.close();
        }
    }

    if (Serial)
    {
        Serial.print(write_str);
    }
    counter++;
    if (counter > 4096 && teensy_sd_enabled)
    {
        Serial.println("Flash Data dump");
        counter = 0;
        File flash_file = flash.open(flash_string.c_str());
        File teensy_file = teensy_sd.open(sd_string.c_str());
        file_copy(&flash_file, &teensy_file, true);
        flash.remove(flash_string.c_str());
    }
    // delay(1000);
}