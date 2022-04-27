//
// Created by shawn on 3/21/22.
//

#include "Utilities.h"
#include <Streaming.h>

uint8_t byte_rate = 8;
size_t bytes_written = 0;
size_t max_bytes = 8000000;

void file_copy(File *copy, File *paste, bool close) {
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
    Serial.println("Dumping " + String(copy->name()) + " Finished");
    paste->flush();
    // Closes both files, deletes original file
    if (close)
    {
        copy->close();
        paste->close();
    }
}

String flts(float s) {
    return {s, float_depth};
}

String flts(double s) {
    return {s, double_depth};
}

bool updateBlockSize(size_t num_written){
bytes_written += num_written;
return bytes_written >= max_bytes;
}

bool getBlockSize() {
    return bytes_written >= max_bytes;
}

size_t getBlockSizeVal() {
    return bytes_written;
}

void resetBlockCounter() {
    bytes_written = 0;
    Serial << "Bytes Written: " << bytes_written << endl;
}