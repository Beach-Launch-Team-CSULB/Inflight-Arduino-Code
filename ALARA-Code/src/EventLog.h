//
// Created by shawn on 3/28/22.
//

#ifndef TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_H
#define TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_H


#include "Sensor.h"
#include "eventlog_struct.h"
#include <deque>

/**
 * The Event Log, used for logging miscellaneous text to the flash such as events
 * Can be used as a timeline of the events, etc. after the flight
 */
class EventLog: public Sensor {
public:
    /**
     * Updates the data to flash from the storage deque
     */
    void updateData() override;
    /**
     * Used to write text data events into the file
     * Must be under a certain character limit or it will get cut off
     * @param s The string to write
     */
    void write(String s);
    /**
     * Creates an EventLog
     */
    EventLog();
private:
    /**
     * Derived data deque
     */
    std::deque<eventlog_struct> data_deque;
    /**
     * Derived data struct
     */
    eventlog_struct data_struct;
};

/**
 * Static object across the program to allow access to the Event log in various spots & to make sure only one exists
 */
static EventLog eventLog;

#endif //TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_H
