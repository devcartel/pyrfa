#ifndef DIRECTORYHANDLER_H
#define DIRECTORYHANDLER_H

#include "StdAfx.h"
#include <boost/python.hpp>

using namespace boost::python;

class DirectoryHandler
{
public:
    DirectoryHandler(rfa::sessionLayer::OMMConsumer* pOMMConsumer, 
        rfa::common::EventQueue& eventQueue,
        rfa::common::Client& client,
        std::string& serviceName,
        rfa::logger::ComponentLogger& componentLogger);

    ~DirectoryHandler(void);

    void sendRequest();
    void closeRequest();

    void processResponse( const rfa::message::RespMsg& respMsg, boost::python::tuple& out);

    bool isServiceUp() const;
    void setDebugMode(const bool &debug);

private:

    void decodeDirectory( const rfa::common::Data& directory, boost::python::tuple& out);
    void decodeFilterList(const rfa::data::FilterList& filterList, const std::string& serviceName, dict &d);
    void decodeElementList(const rfa::data::ElementList& elementList, const std::string& serviceName, dict &d);
    rfa::common::RFA_String decodeArray(const rfa::data::Array& array);

    rfa::sessionLayer::OMMConsumer      *_pOMMConsumer;
    rfa::common::EventQueue             &_eventQueue;
    rfa::common::Client                 &_client;
    rfa::common::Handle                 *_pHandle;
    std::string                         &_serviceName;
    bool                                _isServiceUp;
    bool                                _debug;
    rfa::logger::ComponentLogger        &_componentLogger;
    rfa::common::RFA_String             _log;
};

#endif
