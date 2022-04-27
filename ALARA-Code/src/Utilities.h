#include <FS.h>

#define buffer_size 128
#define cm +"," +

const uint_fast16_t packet_size = 512;
const uint_fast16_t max_buf_size = packet_size * 4;
// Precision values to use when converting from float to decimal
const u_char float_depth = 18, double_depth = 18;
bool updateBlockSize(size_t num_written);
size_t getBlockSizeVal();
bool getBlockSize();
void resetBlockCounter();
void file_copy(File *copy, File *paste, bool close);
// FLTS function- returns float as string with full precision
String flts(float s);
String flts(double s);
extern uint8_t byte_rate;
extern size_t max_bytes;
extern size_t bytes_written;
