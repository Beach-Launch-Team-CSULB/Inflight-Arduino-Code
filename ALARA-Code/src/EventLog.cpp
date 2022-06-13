//
// Created by shawn on 3/28/22.
//

#include "EventLog.h"
#include <Streaming.h>

void EventLog::updateData() {
    // Iterates through data deque, writes everything to the output file
    while(!data_deque.empty()) {
        data_struct = data_deque.front();
        out_file->write(&data_struct, data_struct_size);
        data_deque.pop_front();
    }
    out_file->flush();
}

EventLog::EventLog() : Sensor("EventLog", &data_struct, sizeof(data_struct), 1) {
    // EventLog is always enabled, or at least should be
    enable = true;
}

void EventLog::write(String s) {
    // Cuts off string if it is too long
    if(s.length() > EVENTLOG_LENGTH) {
        s = s.substring(0, EVENTLOG_LENGTH);
    }
    // Adds message to the back of the data deque
    data_deque.emplace_back(s.c_str());
    Serial.println(s);
}