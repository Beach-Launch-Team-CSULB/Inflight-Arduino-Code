//
// Created by shawn on 3/28/22.
//

#ifndef TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_H
#define TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_H


#include "Sensor.h"
#include "eventlog_struct.h"
#include <stack>

class EventLog: public Sensor {
public:
    void updateData() override;
    void write(String s);
    EventLog();
private:
    std::stack<eventlog_struct> events;
    eventlog_struct data_struct;
};


#endif //TEENSY_INFLIGHT_MINI_CODE_EVENTLOG_H
