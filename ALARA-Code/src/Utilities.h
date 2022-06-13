#include <FS.h>

#define buffer_size 128
#define cm +"," +
//Data storage constants
const uint_fast16_t packet_size = 512;
const uint_fast16_t max_buf_size = packet_size * 4;
// Precision values to use when converting from float to decimal
const u_char float_depth = 18, double_depth = 18;
/**
 * Block size update function
 * Sensor writes will add to the counter in this function until full,
 * after which the function will return false
 * @param num_written The bytes written by the current sensor
 * @return Whether or not the max number of bytes on the flash has been hit
 */
bool updateBlockSize(size_t num_written);
/**
 * Access the number of bytes written to the flash directly
 * @return The number of bytes written to the flash
 */
size_t getBlockSizeVal();
/**
 * Checks if the max number of bytes on the flash has been hit, returns the result
 * @return The number of free bytes on the flash
 */
bool getBlockSize();
/**
 * Resets the counter of the number of bytes written to flash to 0
 */
void resetBlockCounter();
/**
 * Does a byte-by-byte direct file copy between two files
 * Necessary for dumping data between separate file systems
 * @param copy The file to be copied
 * @param paste The file data will be copied to
 * @param close Whether the files should be closed at the end of the operation or not
 */
void file_copy(File *copy, File *paste, bool close);
/**
 * FLTS function- returns float as string with full precision
 * @param s The number to stringify
 * @return the stringified number
 */
String flts(float s);
/**
 * FLTS function- returns float as string with full precision
 * @param s The number to stringify
 * @return the stringified number
 */
String flts(double s);
// External variables in Utilities.cpp, your compiler will yell at you if you put these in the header
extern uint8_t byte_rate;
extern size_t max_bytes;
extern size_t bytes_written;
