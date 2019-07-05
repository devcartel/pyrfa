
#include "LogMsgMapImpl.h"

LogMsgMapImpl::LogMsgMapImpl() {
    // init internal log message strings
    _LM_GENERIC_ONE    = "%1\n";
    _LM_GENERIC_TWO    = "P[1]:%1, P[2]:%2\n";
    _LM_UNEXPECTED_ONE = "Unexpected error: %1\n";
    _LM_UNDEFINED      = "Undefined message code\n";
}

LogMsgMapImpl::~LogMsgMapImpl() {
}

/*
* implementation of getMsg from LogMsgMap.h
* to be able to use internal log strings without *.mc files
*/
const rfa::common::RFA_String* LogMsgMapImpl::getMsg(const unsigned long id) const {
    switch ( id ) {
        case LM_GENERIC_ONE:
            return &_LM_GENERIC_ONE;
        case LM_GENERIC_TWO:
            return &_LM_GENERIC_TWO;
        case LM_UNEXPECTED_ONE:
            return &_LM_UNEXPECTED_ONE;
        default:
            return &_LM_UNDEFINED;
    }
}
