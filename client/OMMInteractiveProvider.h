/**
 *  OMMCProvServer is an interface which is doing interactive client publishing
 */

#ifndef _OMMINTERACTIVEPROVIDER_H
#define _OMMINTERACTIVEPROVIDER_H

#include "StdAfx.h"
#include "common/Encoder.h"
#include "common/Mutex.h"
#include "RDM/RDMFieldDictionary.h"
#include <boost/python.hpp>

class RDMFieldDict;
class Encoder;

using namespace boost::python;

// This class maintains an individual token watchlist for each client
class ClientWatchList
{
public:

	struct TokenInfo
	{
		rfa::sessionLayer::RequestToken*	pToken;
		rfa::message::AttribInfo			attribInfo;
		bool								bAttribInfoInUpdates;
		bool								bSubmitted;
		bool								bItemRequest;
		rfa::common::UInt8					msgModelType;
	};

	ClientWatchList( const rfa::common::Handle* clientSessionHandle );
	virtual ~ClientWatchList();

	void cleanup();
	TokenInfo* first();
	TokenInfo* next(rfa::sessionLayer::RequestToken*);
	TokenInfo* getTokenInfo(rfa::sessionLayer::RequestToken* pT);
	TokenInfo* addToken(rfa::sessionLayer::RequestToken* pT, const rfa::message::ReqMsg& reqMsg, bool isItemReq = true);
	bool removeToken(rfa::sessionLayer::RequestToken* pT);

	typedef std::map<rfa::sessionLayer::RequestToken*, TokenInfo*> TokenInfoMap;

private:

	const rfa::common::Handle*		_clientSessionHandle;
	TokenInfoMap					_watchlist;

private:
	//Do not implemement
	ClientWatchList();
	ClientWatchList(const ClientWatchList&);
	ClientWatchList& operator=(const ClientWatchList&);
};

// This class maintains a list of all clients' watchlists;
// note that due to the possibility of the concurent access
// to this watchlist from different threads, e.g. app and rfa thread,
// access to this watchlist need to be protected by a lock
// in this case application maintains a lock protecting this resource
class ProviderWatchList
{
public:

	ProviderWatchList();
	virtual ~ProviderWatchList();

	bool cleanup();
	bool addClientWatchList( const rfa::common::Handle* pCSH );
	bool removeClientWatchList( const rfa::common::Handle* pCSH );
	ClientWatchList* getClientWatchList( const rfa::common::Handle* pCSH );

	typedef rfa::common::RFA_Vector< const rfa::common::Handle* >	CLIENT_SESSIONS;
    typedef rfa::common::RFA_Vector<rfa::sessionLayer::RequestToken*>	TOKENS;
    void findTokens(const rfa::common::RFA_String& itemName, rfa::common::UInt8 msgModelType, TOKENS& tokens, CLIENT_SESSIONS& cshs);

	void getClientSessions( CLIENT_SESSIONS& sessions );

private:

	typedef std::map< const rfa::common::Handle*, ClientWatchList*>	CSH_MAP;
	CSH_MAP		_watchList;

private:
	//Do not implement
	ProviderWatchList(const ProviderWatchList&);
	ProviderWatchList& operator=(const ProviderWatchList&);
};

class OMMInteractiveProvider
{
public:
    OMMInteractiveProvider(rfa::sessionLayer::OMMProvider* pOMMProvider,
                           const std::string& vendorName,
                           const std::string& serviceName,
                           rfa::common::RFA_String fieldDictionaryFilename,
                           rfa::common::RFA_String enumTypeFilename,
                           const RDMFieldDict* dict,
                           rfa::logger::ComponentLogger& componentLogger);
    ~OMMInteractiveProvider();
    void    processReqMsg(const rfa::sessionLayer::OMMSolicitedItemEvent& event, boost::python::tuple& out);
    void    processPostMsg(const rfa::sessionLayer::OMMSolicitedItemEvent& event);
    void    submitPostedData(rfa::message::RespMsg&,
                             const rfa::common::PrincipalIdentity&,
                             const rfa::common::RFA_String&,
                             ProviderWatchList::TOKENS&,
                             ProviderWatchList::CLIENT_SESSIONS&);
	void    submitAckMsg(rfa::sessionLayer::RequestToken* pRT,
                         rfa::common::Handle* pCSH,
                         bool postIdSet,
                         rfa::common::UInt32 ackID,
                         bool sequenceNumSet,
                         rfa::common::UInt32 sequenceNum,
                         bool nackCodeSet,
                         rfa::message::AckMsg::NackCode nackcode,
                         const rfa::common::RFA_String& text,
                         rfa::common::UInt8 msgModelType);
    void    processLoginReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event, boost::python::tuple& out);
    void    processDirectoryReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event);
    void    processDictionaryReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event);
    void    processMarketPriceReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event, boost::python::tuple& out);
    void    processCloseReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event, boost::python::tuple& out);
    
    void    submitData(const rfa::common::RFA_String& item, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, int mType, const std::string& msgType, rfa::common::RFA_String& serviceName, const std::string& session="", const std::string& mapAction="", const std::string& mapKey="");
    void    directorySubmit(const UInt8 &mType, rfa::common::RFA_String& serviceName);
    void    closeSubmit(const rfa::common::RFA_String& item, const std::string& session="");
    void    closeAllSubmit();
    void    staleSubmit(const rfa::common::RFA_String& item, const std::string& session="");
    void    staleAllSubmit();
    void    serviceStateSubmit(int serviceState, rfa::common::RFA_String& serviceName);
    void    logoutSubmit(const std::string& session);
    void    logoutAllSubmit();
    void    rejectSessionSubmit(const std::string& session);
    void    rejectSessionAllSubmit();
    void    clearPublishedItemList();
    void    setDebugMode(const bool &debug);
    void    cleanup();
    
    bool    addClientWatchList(const rfa::common::Handle* pCSH);
    bool    removeClientWatchList(const rfa::common::Handle* pCSH, boost::python::tuple& out);
    
    void    printClientWatchlists();
    std::string    getClientSessions();
    std::string    getClientWatchList(const std::string& session);

private:
	rfa::sessionLayer::OMMProvider*		_pOMMProvider;
    rfa::common::RFA_String             _vendorName;
    rfa::common::RFA_String             _serviceName;
    int                                 _serviceState;
    bool                                _debug;
    rfa::common::RFA_String             _log;
    rfa::sessionLayer::RequestToken*	_pLoginToken;
    rfa::sessionLayer::RequestToken*	_pDirectoryToken;
    
    rfa::rdm::RDMFieldDictionary&		_rRDMFieldDictionary;
    bool								_bDictionaryDataRead;
    
    // provider watch list need to be locked due to possible
	// concurent access to it from different threads
	ProviderWatchList					_providerWatchList;
	CMutex                              _providerWatchListLock;
    
    //Encoder*                            _pEncoder;
    Encoder                             _encoder;
    
    rfa::message::RespMsg               _respMsg;
    rfa::data::FieldList                _fieldList;
    rfa::common::RespStatus             _respStatus;
    rfa::message::AttribInfo            _attribInfo;
	rfa::common::QualityOfService       _qos;
    Map                                 _map;

    const RDMFieldDict                  *_pDict;
    
private:
    // Declared, but not implemented to prevent default behavior generated by compiler
    OMMInteractiveProvider(const OMMInteractiveProvider&);
    OMMInteractiveProvider & operator=(const OMMInteractiveProvider&);
    rfa::logger::ComponentLogger                    &_componentLogger;
};

#endif // _OMMINTERACTIVEPROVIDER_H
