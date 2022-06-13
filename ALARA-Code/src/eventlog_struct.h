//
// Created by shawn on 3/28/22.
//

#ifndef TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_STRUCT_H
#define TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_STRUCT_H


#include "Sensor_struct.h"
/**
 * Character length of the message in the event log
 */
const uint8_t EVENTLOG_LENGTH =  128;

struct eventlog_struct: public Sensor_struct {
    String toString() override;
    /**
     * Returns CSV file header for the EventLog
     * @return CSV file header
     */
    String getHeader() const override;
    /**
     * Default empty constructor for an event
     */
    eventlog_struct();
    /**
     * Character array message in each event
     */
    char msg[EVENTLOG_LENGTH]{};
    /**
     * Crates a new event log message
     * @param msg_in the formatted string to turn into the message
     */
    explicit eventlog_struct(const char *msg_in);
};


#endif //TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_STRUCT_H
