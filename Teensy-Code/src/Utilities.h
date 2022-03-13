#include <SD.h>
#define buffer_size 128
#define cm +"," +
// GPS baud rate
#define GPS_BAUD 9600
#define sea_level_pressure_hpa 1015.578

// Macro to make appending strings easier- cm stands for comma
// Using UART serial port 8
#define GPSSerial Serial8

const uint_fast8_t BMP_CS = 15, ICM_CS = 16, XTSD_CS = 36, BNO_INT = 21, BNO_CS = 37, BNO_RESET = 19,
        ICM_INT_1 = 24, ICM_INT_2 = 25, BMP_INT = 20;
// Approximate air pressure in Randsburg, CA
// Precision values to use when converting from float to decimal
const u_char float_depth = 18, double_depth = 18;

void file_copy(File *copy, File *paste, bool close)
{
    // Reads data from XTSD to buffer, writes data from buffer to SD card
    // Stores size of buffer
    size_t n;
    // Stores data being copied
    auto *buf = new uint8_t[buffer_size];
    // Continuously copies data from file to buffer, writes data from buffer to file
    while ((n = copy->read(buf, sizeof(buf))) > 0)
    {
        paste->write(buf, n);
    }
    // Frees buffer from memory
    delete[] buf;
    Serial.print("Dumping " + String(copy->name()) + " Finished");
    // Closes both files, deletes original file
    if (close)
    {
        copy->close();
        paste->close();
    }
}

// FLTS function- returns float as string with full precision
String flts(float s)
{
    return {s, float_depth};
}
String flts(double s)
{
    return {s, double_depth};
}

struct BNO_IMU
{
    float acceleration[3]{};
    float gyro[3]{};
    float magnetometer[3]{};
    void init(const float *c_acceleration, const float *c_gyro, const float *c_magnetometer)
    {
        for (int i = 0; i < 3; i++)
        {
            acceleration[i] = c_acceleration[i];
            gyro[i] = c_gyro[i];
            magnetometer[i] = c_magnetometer[i];
        }
    }
    BNO_IMU() = default;
    void print()
    {
        Serial << "acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << endl;
        Serial << "gyro:" << gyro[0] << ", " << gyro[1] << ", " << gyro[2] << endl;
        Serial << "magnetometer :" << magnetometer[0] << ", " << magnetometer[1] << ", " << magnetometer[2] << endl;
    }
    String toString() {
        return flts(acceleration[0]) cm acceleration[1] cm acceleration[2] cm gyro[0] cm gyro[1] cm gyro[2]
        cm magnetometer[0] cm magnetometer[1] cm magnetometer[2];
    }
};

struct gps_struct
{
    void init(TinyGPSPlus gps)
    {
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
    }
    // TESTING - KNOWN BUG - GPS MODULE HAS NOT GOTTEN A LOCATION SYNC
    void print() const
    {
        // Updates GPS data points
        Serial << "gps_altitude " << gps_altitude << endl;
        Serial << "gps_year " << gps_year << endl;
        Serial << "gps_month " << gps_month << endl;
        Serial << "gps_day " << gps_day << endl;
        Serial << "gps_hour " << gps_hour << endl;
        Serial << "gps_minute " << gps_minute << endl;
        Serial << "gps_second " << gps_second << endl;
        Serial << "gps_latitude " << gps_latitude << endl;
        Serial << "gps_longitude " << gps_longitude << endl;
        Serial << "gps_speed " << gps_speed << endl;
        Serial << "gps_hdop " << gps_hdop << endl;
        Serial << "gps_course_deg " << gps_course_deg << endl;
        Serial << "gps_reading_age " << gps_reading_age << endl;
    }

    // void init(double gpsAltitude, int year, int month, int day, int hour, int second, double latitude,
    //           double longitude, int speed, double hdop, double courseDeg, double readingAge)
    // {
    //     gps_altitude = gpsAltitude;
    //     gps_struct::year = year;
    //     gps_struct::month = month;
    //     gps_struct::day = day;
    //     gps_struct::hour = hour;
    //     gps_struct::second = second;
    //     gps_struct::latitude = latitude;
    //     gps_struct::longitude = longitude;
    //     gps_struct::speed = speed;
    //     gps_struct::hdop = hdop;
    //     course_deg = courseDeg;
    //     reading_age = readingAge;
    // }

    gps_struct() = default;

    // Updates GPS data points
    double gps_altitude;
    uint16_t gps_year;
    uint8_t gps_month;
    uint8_t gps_day;
    uint8_t gps_hour;
    uint8_t gps_minute;
    uint8_t gps_second;
    double gps_latitude;
    double gps_longitude;
    int32_t gps_speed;
    double gps_hdop;
    double gps_course_deg;
    uint32_t gps_reading_age;
    // double gps_altitude{};
    // int year{}, month{}, day{}, hour{}, second{};
    // double latitude{}, longitude{};
    // int speed{};
    // double hdop{};
    // double course_deg{};
    // double reading_age{};
    String toString() const {
        return flts(gps_altitude) cm gps_year cm gps_month cm gps_day cm gps_hour cm gps_minute cm gps_second
        cm gps_latitude cm gps_longitude cm gps_speed cm gps_hdop cm gps_course_deg cm gps_reading_age;
    }
};




struct ICM_IMU // icm20649IMU is one of our IMU's WORKING
{
    float temperature;
    float gyro[3]; // xV,yV,zV
    float acceleration[3]; // verified works
    ICM_IMU() = default;

    void init(sensors_event_t &icm_accel, sensors_event_t &icm_gyro, sensors_event_t &icm_temp)
    {

        gyro[0] = icm_gyro.gyro.x; // verified working
        gyro[1] = icm_gyro.gyro.y;
        gyro[2] = icm_gyro.gyro.z;

        acceleration[0] = icm_accel.acceleration.x; // verified working
        acceleration[1] = icm_accel.acceleration.y;
        acceleration[2] = icm_accel.acceleration.z;

        temperature = icm_temp.temperature; // not fully verified
    }
    String toString()
    {
       return flts(acceleration[0]) cm acceleration[1] cm acceleration[2] cm
               gyro[0] cm gyro[1] cm gyro[2] cm temperature;
    }
};

struct BMP_Altimeter // BMP
{
    double temperature, pressure, altitude;
    void init(double temp, double press, double alt)
    {
        temperature = temp;
        pressure = press;
        altitude = alt;
    }
    // Temp,pressure, altitude, icm_temperature ..., last sensor field \n
    // temp value, pressure value, icm temp value..., last sensor value\n
    void print() const
    {
        Serial << "Temperature: " << temperature << endl;
        Serial << "pressure: " << pressure << endl;
        Serial << "altitude: " << altitude << endl;
    }

    String toString() const
    {
        return flts(temperature) cm flts(pressure)
               cm flts(altitude);
    }
};

struct All_the_data
{
    uint32_t milliseconds = millis();
    uint32_t microseconds = micros();
    BNO_IMU bno_data{};
    ICM_IMU icm_data{};
    BMP_Altimeter altimeter_data{};
    gps_struct gps_data{};
    String toString()
    {
        return String(milliseconds) cm microseconds += bno_data.toString() cm icm_data.toString()
                cm altimeter_data.toString() cm gps_data.toString();
    }
};

// This method dumps data from files with the All_The_Data struct stored on them to Serial.
// does not handle opening/closing the file for portability
void dump_to_serial(File stored_data)
{
    // Serial << "File Size: " << stored_data.size() << endl;

    size_t bytesRead;
    All_the_data fromFile;
    int counter = 0;
    bytesRead = stored_data.read(&fromFile, sizeof(All_the_data));
    Serial.println(sizeof(All_the_data));
    while (bytesRead == sizeof(All_the_data))
    {
        Serial.print(fromFile.toString());
        bytesRead = stored_data.read(&fromFile, sizeof(All_the_data));
        counter++;
    }

    Serial << "Structs Read: " << counter << endl;
}

void csv_parse_dump(File * in, File * out, bool close)
{
    // Serial << "File Size: " << stored_data.size() << endl;

    size_t bytesRead;
    All_the_data fromFile;
    int counter = 0;
    bytesRead = in->read(&fromFile, sizeof(All_the_data));
    while (bytesRead == sizeof(All_the_data))
    {
        Serial.print(fromFile.toString());
        out->print(fromFile.toString());
        bytesRead = in->read(&fromFile, sizeof(All_the_data));
        counter++;
    }
    out->flush();
    delay(1);
    Serial.print("Bytes read: ");
    Serial.println(bytesRead);
    Serial.print("Counter: ");
    Serial.println(counter);
    delay(1);
    if(close) {
        out->close();
        in->close();
    }
}

struct device_enables {
    bool bmp, bno, icm, xtsd, teensy_sd, gps, flash;
};


