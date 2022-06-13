//
// Created by shawn on 3/28/22.
//

#include "eventlog_struct.h"

String eventlog_struct::toString() {
    // Returns the time + the event log message as a formatted string
    return get_time() + ", " + msg;
}

String eventlog_struct::getHeader() const {
    // Time header + CSV element for the event
    return get_time_hdr() + ", Event";
}

eventlog_struct::eventlog_struct(const char *msg_in) {
    // Copies message from msg_in assuming fixed message length, which is necessary for the struct to work anywayss
    strcpy(msg, msg_in);
    // Sets time
    setTime();
}

eventlog_struct::eventlog_struct() = default;