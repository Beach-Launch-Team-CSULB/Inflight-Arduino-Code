//
// Created by shawn on 3/28/22.
//

#include "EventLog.h"
#include <Streaming.h>

void EventLog::updateData() {
    Serial.println("EventLog print");
    Serial.flush();
    Serial.println(data_deque.size());
    while(!data_deque.empty()) {
        data_struct = data_deque.front();
        out_file->write(&data_struct, data_struct_size);
        data_deque.pop_front();
    }
    Serial.println("After file!");
    Serial.flush();
    delay(1000);
    Serial.println((long)out_file);
    Serial.flush();
    Serial.println(out_file->name());
    Serial.flush();
    delay(1000);
    out_file->flush();
}

EventLog::EventLog() : Sensor("EventLog", &data_struct, sizeof(data_struct), 1) {
enable = true;
}

void EventLog::write(String s) {
    if(s.length() > EVENTLOG_LENGTH) {
        s = s.substring(0, EVENTLOG_LENGTH);
    }
    data_deque.emplace_back(s.c_str());
    Serial.println(s);
}