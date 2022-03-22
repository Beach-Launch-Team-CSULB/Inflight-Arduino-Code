//
// Created by shawn on 3/21/22.
//

#include "Utilities.h"

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
    Serial.print("Dumping " + String(copy->name()) + " Finished");
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