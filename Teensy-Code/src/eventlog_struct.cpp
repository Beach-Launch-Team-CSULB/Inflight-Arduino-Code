//
// Created by shawn on 3/28/22.
//

#include "eventlog_struct.h"

String eventlog_struct::toString() {
    return get_time() + ", " + msg;
}

String eventlog_struct::getHeader() const {
    return get_time_hdr() + ", Event";
}

eventlog_struct::eventlog_struct(const char *msg_in) {
    strcpy(msg, msg_in);
    setTime();
}

eventlog_struct::eventlog_struct() = default;