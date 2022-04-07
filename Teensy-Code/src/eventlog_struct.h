//
// Created by shawn on 3/28/22.
//

#ifndef TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_STRUCT_H
#define TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_STRUCT_H


#include "Sensor_struct.h"
const uint8_t EVENTLOG_LENGTH = 128;

struct eventlog_struct: public Sensor_struct {
    String toString() override;
    //Returns CSV file header
    String getHeader() const override;
    eventlog_struct();
    char msg[EVENTLOG_LENGTH]{};
    explicit eventlog_struct(const char *msg_in);
};


#endif //TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_STRUCT_H
