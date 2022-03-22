#include <FS.h>
#include <Adafruit_Sensor.h>
#include <Streaming.h>
#include "Sensor_struct.h"
#include "gps_struct.h"
#include "bno_struct.h"
#include "icm_struct.h"

#define buffer_size 128
#define cm +"," +

const uint_fast8_t BMP_CS = 15, ICM_CS = 16, XTSD_CS = 36, BNO_INT = 21, BNO_CS = 37, BNO_RESET = 19,
        ICM_INT_1 = 24, ICM_INT_2 = 25, BMP_INT = 20;
const uint_fast16_t packet_size = 1024;
// Approximate air pressure in Randsburg, CA
// Precision values to use when converting from float to decimal
const u_char float_depth = 18, double_depth = 18;

const uint16_t GPS_BAUD = 9600;

void file_copy(File *copy, File *paste, bool close);
// FLTS function- returns float as string with full precision
String flts(float s);
String flts(double s);

//struct All_the_data {
//    uint32_t milliseconds = millis();
//    uint32_t microseconds = micros();
//    bno_struct bno_data;
//    icm_struct icm_data;
//    bmp_struct altimeter_data;
//    gps_struct gps_data;
//    String toString()
//    {
//        return String(milliseconds) cm microseconds += bno_data.toString() cm icm_data.toString()
//                cm altimeter_data.toString() cm gps_data.toString();
//    }
//};
//
//// This method dumps data from files with the All_The_Data struct stored on them to Serial.
//// does not handle opening/closing the file for portability
//void dump_to_serial(File stored_data)
//{
//    // Serial << "File Size: " << stored_data.size() << endl;
//
//    size_t bytesRead;
//    All_the_data fromFile;
//    int counter = 0;
//    bytesRead = stored_data.read(&fromFile, sizeof(All_the_data));
//    Serial.println(sizeof(All_the_data));
//    while (bytesRead == sizeof(All_the_data))
//    {
//        Serial.print(fromFile.toString());
//        bytesRead = stored_data.read(&fromFile, sizeof(All_the_data));
//        counter++;
//    }
//
//    Serial << "Structs Read: " << counter << endl;
//}