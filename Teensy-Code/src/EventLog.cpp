//
// Created by shawn on 3/28/22.
//

#include "EventLog.h"
#include <Streaming.h>

void EventLog::updateData() {
    while(!events.empty()) {
        data_struct = events.top();
        new_data_update();
        events.pop();
    }
    out_file->flush();
}

EventLog::EventLog() : Sensor("EventLog", &data_struct, sizeof(data_struct), 1) {
enable = true;
}

void EventLog::write(String s) {
    if(s.length() > EVENTLOG_LENGTH) {
        s = s.substring(0, EVENTLOG_LENGTH);
    }
    events.push(eventlog_struct(s.c_str()));
}