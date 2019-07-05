#ifndef RDMUTILS_H
#define RDMUTILS_H

#include "StdAfx.h"

class RDMEnumDef;

class RDMUtils
{
public:

    // convert RespStatus components to string
    static string streamStateToString(const rfa::common::RespStatus::StreamState& streamState);
    static string statusCodeToString(const rfa::common::UInt8 statusCode);
    static string dataStateToString(const rfa::common::RespStatus::DataState& dataState);

    // converts databuffer contents to a string
    static string dataBufferToString(const rfa::data::DataBuffer& dataBuffer, const RDMEnumDef* enumDef=0);

    // converts databuffer contents to a string with no RMTES conversion (e.g. for TS1 data)
    static string dataBufferToStringNoCoversion(const rfa::data::DataBuffer& dataBuffer);

    // converts databuffer contents to a double
    static double dataBufferToDouble(const rfa::data::DataBuffer& dataBuffer);

    // converts databuffer contents to a int
    static int dataBufferToInt(const rfa::data::DataBuffer& dataBuffer);

    // converts databuffer contents to a long
    static long dataBufferToLong(const rfa::data::DataBuffer& dataBuffer);

    // convert message model type to string
    static string msgModelTypeToString(rfa::common::UInt16 msgType);

    // converts response type to string
    static string msgRespTypeToString(rfa::common::UInt32 respType);

private:
    RDMUtils(void);
    RDMUtils(RDMUtils&);   
};

#endif
