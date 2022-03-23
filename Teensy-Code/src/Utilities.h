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
        ICM_INT_1 = 24, ICM_INT_2 = 25, BMP_INT = 20, NRF_CE = 9, NRF_CSN = 10;
const uint_fast16_t packet_size = 8124;
// Approximate air pressure in Randsburg, CA
// Precision values to use when converting from float to decimal
const u_char float_depth = 18, double_depth = 18;

const uint16_t GPS_BAUD = 9600;

void file_copy(File *copy, File *paste, bool close);
// FLTS function- returns float as string with full precision
String flts(float s);
String flts(double s);
