#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#ifndef _WIN32
    #include <stdio.h>
#endif

#include "../common/RDMUtils.h"
#include "../common/RDMDict.h"
#include "OMMInteractiveProvider.h"

#include <boost/lexical_cast.hpp>

#define DICT_ID    1
#define FIELD_LIST_ID 3

OMMInteractiveProvider::OMMInteractiveProvider(rfa::sessionLayer::OMMProvider* pOMMProvider,
                                               const std::string& vendorName,
                                               const std::string& serviceName,
                                               rfa::common::RFA_String fieldDictionaryFilename,
                                               rfa::common::RFA_String enumTypeFilename,
                                               const RDMFieldDict* dict,
                                               rfa::logger::ComponentLogger& componentLogger):
    _pOMMProvider(pOMMProvider),
    _vendorName(vendorName.c_str()),
    _serviceName(serviceName.c_str()),
    _serviceState(1),
    _debug(false),
    _pLoginToken(0),
    _pDirectoryToken(0),
    _rRDMFieldDictionary(rfa::rdm::RDMFieldDictionary::create()),
    _bDictionaryDataRead(false),
    _pDict(dict),
    _componentLogger(componentLogger)
{
    if(_bDictionaryDataRead == false)
	{
		_rRDMFieldDictionary.readRDMFieldDictionary(fieldDictionaryFilename);
		_rRDMFieldDictionary.readRDMEnumTypeDef(enumTypeFilename);
		_rRDMFieldDictionary.setDictId(1);
		_bDictionaryDataRead = true;
	}
}

OMMInteractiveProvider::~OMMInteractiveProvider()
{
    cleanup();
}

void OMMInteractiveProvider::processPostMsg(const rfa::sessionLayer::OMMSolicitedItemEvent& event) {
    const rfa::message::PostMsg& postMsg = static_cast<const rfa::message::PostMsg&>(event.getMsg());
	rfa::sessionLayer::RequestToken& rToken = event.getRequestToken();
	rfa::common::Handle* pCSH = event.getHandle();
	UInt8 hintMask = postMsg.getHintMask();
	UInt8 indicationMask = postMsg.getIndicationMask();

	//RFA_String text( "<- Received PostMsg ", 0, false);
    _log = "[OMMInteractiveProvider::processPostMsg] Received PostMsg";
    
	// Get Visible Publisher Identity that sent the post message:
	const PrincipalIdentity& ppi = postMsg.getPrincipalIdentity();
	if ( ppi.getIdentityType() == PublisherPrincipalIdentityEnum )
	{
		const PublisherPrincipalIdentity& ppId = static_cast<const PublisherPrincipalIdentity&>(ppi);
		UInt32 userAddress = ppId.getUserAddress();
		UInt32 userID = ppId.getUserID();
		_log += " with publisher [user ID: ";
		_log.append(userID);
		_log.append("] at [user address: " );
		_log.append( userAddress );
		_log.append( "] " );
	}
	//text += "from Client Session Handle:";
	//AppUtil::log(__LINE__, AppUtil::TRACE, "%s %p", text.c_str(), pCSH);

	ClientWatchList* cwl = _providerWatchList.getClientWatchList(pCSH);
	ClientWatchList::TokenInfo* pTS = cwl->getTokenInfo(&rToken);

	//Populate attribInfo of the response message according to the following rule:
	//1) If this is off-stream post message (matches login request token),
	//   then get attribInfo from the post message; it MUST be there!
	//2) If this is on-stream post message (matches item request token),
	//   then get it from the TokenInfo which contains attribInfo received
	//   with this items request; attribInfo may or may not be on the post message
	//   since it is not needed (it may be optimized out by consumer app)
	rfa::message::AttribInfo ai;

	if(pTS->bItemRequest)
	{
		ai = pTS->attribInfo;
	}
	else
	{
		assert(hintMask & PostMsg::AttribInfoFlag);
		ai = postMsg.getAttribInfo();
	}

	// If the hint mask on the PostMsg has PostMsg::PermissionDataFlag set,
	// then the permission data included in the PostMsg is obtained by PostMsg::getPermissionData() method
	// which is used to permission the user that is posting the information.

	UInt32 postID = 0;
	bool bPostIdSet = false;
	if(hintMask & PostMsg::PostIdFlag)
	{
		bPostIdSet = true;
		postID = postMsg.getPostID();
	}

	UInt32 sequenceNum = 0;
	bool bSequenceNumSet = false;
	if(hintMask & PostMsg::SeqFlag)
	{
		bSequenceNumSet = true;
		sequenceNum = postMsg.getSeqNum();
	}

	bool bWantAck = (indicationMask & PostMsg::WantAckFlag) ? true : false;
	if(bWantAck)
	{
		rfa::common::RFA_String text;
		//if(_cfgVars.bIgnoreNackCode)
        text.append("Positive AckMsg");
		//else
		//	text.append("AccessDenied");

		submitAckMsg(&rToken, pCSH, bPostIdSet, postID, bSequenceNumSet, sequenceNum,
		             false, AckMsg::AccessDenied, text, postMsg.getMsgModelType());
	}

	//Re-send this post to all clients that subscribed to this item
	//But only if the postMsg's payload is a response message
	if(hintMask & PostMsg::PayloadFlag)
	{
		const Data& payload = postMsg.getPayload();
		if(MsgEnum == payload.getDataType())
		{
			UInt8 msgMMT = postMsg.getMsgModelType();
			const Msg& msg = static_cast<const Msg&>(payload);
			UInt8 payloadMMT = msg.getMsgModelType();
			if(RespMsgEnum == msg.getMsgType() && msgMMT == payloadMMT)
			{
				const RespMsg& respMsg = static_cast<const RespMsg&>(msg);
				RespMsg& ncRespMsg = const_cast<RespMsg&>(respMsg);

				ProviderWatchList::TOKENS requestTokens;
				ProviderWatchList::CLIENT_SESSIONS clientSessions;

				_providerWatchList.findTokens(ai.getName(), msgMMT, requestTokens, clientSessions);
				submitPostedData(ncRespMsg, ppi, ai.getServiceName(), requestTokens, clientSessions);
			}
		}
		//_pDecoder->decodeData(payload);
	}
}

void OMMInteractiveProvider::submitAckMsg(rfa::sessionLayer::RequestToken* pRT,
                                          Handle* pCSH,
                                          bool postIdSet,
                                          UInt32 ackID,
                                          bool sequenceNumSet,
                                          UInt32 sequenceNum,
                                          bool nackCodeSet,
                                          rfa::message::AckMsg::NackCode nackCode,
                                          const RFA_String& text, UInt8 msgModelType)
{
	rfa::message::AckMsg ackMsg;
	ackMsg.setAckID(ackID);

	//Client app is required to set msg Model type of the posted  message.
	ackMsg.setMsgModelType(msgModelType);
	if(sequenceNumSet) ackMsg.setSeqNum(sequenceNum);
	if(nackCodeSet)    ackMsg.setNackCode(nackCode);
	if(!text.empty())  ackMsg.setText(text);

	ackMsg.setAssociatedMetaInfo(*pCSH);

	rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
	itemCmd.setMsg(ackMsg);
	itemCmd.setRequestToken(*pRT);
	_pOMMProvider->submit(&itemCmd);
}

void OMMInteractiveProvider::submitPostedData(RespMsg& respMsg,
                                              const PrincipalIdentity& ppi,
                                              const RFA_String& serviceName,
                                              ProviderWatchList::TOKENS& requestTokens,
                                              ProviderWatchList::CLIENT_SESSIONS& clientSessions)
{
	//This code "trusts" the consumer that encoded this respMsg in its post message
	//the only things that need to be done are:
	//1) addition of the "missing serviceName on respMsg's attribInfo
	//2) addition of the publisherPrincipalIdentity received on PostMsg
	//   overwrites any publisherPrincipalIdentity that was there on the respMsg.
	//3) versioning information
	if(respMsg.getHintMask() & RespMsg::AttribInfoFlag)
	{
		AttribInfo& ai = const_cast<AttribInfo&>(respMsg.getAttribInfo());
		ai.setServiceName(serviceName);
	}
	respMsg.setPrincipalIdentity(ppi);

	//Now loop thru all the request tokens opened for this item and submit this message
	rfa::sessionLayer::OMMSolicitedItemCmd cmd;
	cmd.setMsg(respMsg);
	for(unsigned int pos = 0; pos < requestTokens.size(); pos++)
	{
		//Versioning for each connection
		respMsg.setAssociatedMetaInfo(*clientSessions[pos]);
		cmd.setRequestToken(*requestTokens[pos]);
		_pOMMProvider->submit(&cmd);
	}
}

void OMMInteractiveProvider::processReqMsg(const rfa::sessionLayer::OMMSolicitedItemEvent& event, boost::python::tuple& out) {
    const rfa::message::ReqMsg& msg = static_cast<const rfa::message::ReqMsg&>(event.getMsg());
	if(!(msg.getInteractionType() & rfa::message::ReqMsg::InitialImageFlag) && !(msg.getInteractionType() & rfa::message::ReqMsg::InterestAfterRefreshFlag)) {
		_log = "[OMMInteractiveProvider::processReqMsg] Received close item request (";
        processCloseReq(event, out);
        _log += ")";
        if (_debug) {
            _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
            printClientWatchlists();
        }
		return;
	}
    _log = "[OMMInteractiveProvider::processReqMsg] Received ";
	switch(msg.getMsgModelType())
	{
        case rfa::rdm::MMT_LOGIN:
            _log += "MMT_LOGIN request (";
            _log += msg.getAttribInfo().getName();
            _log += ")";
			processLoginReq(event, out);
			break;
        case rfa::rdm::MMT_DIRECTORY:
            _log += "MMT_DIRECTORY request";
			processDirectoryReq(event);
			break;
        case rfa::rdm::MMT_DICTIONARY:
            _log += "MMT_DICTIONARY request (";
            _log += msg.getAttribInfo().getName();
            _log += ")";
			processDictionaryReq(event);
			break;
        case rfa::rdm::MMT_MARKET_PRICE:
            _log += "MMT_MARKET_PRICE request (";
            _log += msg.getAttribInfo().getName();
            _log += ")";
            processMarketPriceReq(event, out);
			break;
		default:
            _log += "request with unknown msgModelType ";
            _log.append(msg.getMsgModelType());
			break;
	}
    if (_debug) {
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        printClientWatchlists();
    }
}

//This application may choose to deny a login request by sending a Status RespMsg with the RespState.StreamState of Closed.
//This application may force a logoff at any time by sending a Status RespMsg with a Closed StreamState.
void OMMInteractiveProvider::processLoginReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event, boost::python::tuple& out) {
    dict d;
    const rfa::message::ReqMsg& reqMsg = static_cast<const rfa::message::ReqMsg&>(event.getMsg());
	_pLoginToken = &event.getRequestToken();
	rfa::common::Handle* pCSH = event.getHandle();
	//AppUtil::log(__LINE__, AppUtil::INFO, "<- Received MMT_LOGIN Request token: %p handle: %p", _pLoginToken, pCSH);

	_providerWatchListLock.lock();
	ClientWatchList* cwl = _providerWatchList.getClientWatchList(pCSH);
	cwl->addToken(_pLoginToken, reqMsg, false);
	_providerWatchListLock.unlock();
	//if ( _cfgVars.bDisplayTables ) displayTables();
    
	//Login request message must contain attribInfo
	assert( reqMsg.getHintMask() & ReqMsg::AttribInfoFlag );

	//Decode and print AttribInfo Attribs for the login request
	//This section can be elaborated to extract credentials and authenticate as desired before continuing.
	//For now, just print what is in the attrib and accept login.
	//if(reqMsg.getAttribInfo().getHintMask() & AttribInfo::AttribFlag)
	//{
	//	const Data& data = reqMsg.getAttribInfo().getAttrib();
	//	_pDecoder->decodeData(data);
	//}

    rfa::message::RespMsg respMsg;
	rfa::common::RespStatus rStatus;
	rStatus.setStreamState(RespStatus::OpenEnum);
	rStatus.setDataState(RespStatus::OkEnum);
	rStatus.setStatusCode(RespStatus::NoneEnum);
	rfa::common::RFA_String text("Refresh Completed", 0, false);
	rStatus.setStatusText(text);

	const rfa::message::AttribInfo& reqAI = reqMsg.getAttribInfo();
	rfa::message::AttribInfo respAI;
	rfa::data::ElementList reencodedAttrInfo;
	_encoder.reencodeAttribs( reqAI.getAttrib(), reencodedAttrInfo, Encoder::SUPPORT_POST_FLAG );
	respAI.setAttrib( reencodedAttrInfo );

	if ( reqAI.getHintMask() & AttribInfo::NameFlag )
		respAI.setName( reqAI.getName() );

	if ( reqAI.getHintMask() & AttribInfo::NameTypeFlag )
		respAI.setNameType( reqAI.getNameType() );

    _encoder.encodeLoginMsg( &respMsg, respAI, rStatus );

	rfa::sessionLayer::OMMSolicitedItemCmd loginCmd;
	loginCmd.setMsg(respMsg);

	//This request token generally should be saved for future possible logouts
	loginCmd.setRequestToken(*_pLoginToken);
	//submitCmd(&loginCmd, 0, "MMT_LOGIN Refresh");
    _pOMMProvider->submit(&loginCmd);
    
    d["MTYPE"] = "LOGIN";
    d["USERNAME"] = reqAI.getName().c_str();
    d["SERVICE"] = _serviceName.c_str();
    d["SESSIONID"] = boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH));
    out += boost::python::make_tuple(d);
}

void OMMInteractiveProvider::processDirectoryReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event) {
    const rfa::message::ReqMsg& reqMsg = static_cast<const rfa::message::ReqMsg&>(event.getMsg());
	//rfa::sessionLayer::RequestToken& rToken = event.getRequestToken();
	//const rfa::common::Handle* pCSH = rToken.getHandle();
    _pDirectoryToken = &event.getRequestToken();
	rfa::common::Handle* pCSH = event.getHandle();

	//The DataMask in the AttribInfo could be checked here to find out if there was a specific part of the directory that is requested.
	//(SERVICE_INFO_FILTER, SERVICE_STATE_FILTER, etc...)
	//For this example this is disregarded and the whole directory will be sent.
	//rfa::common::UInt8 requestedDataMask =  reqMsg.getAttribInfo().getDataMask();

	//The serviceName from the AttribInfo could be checked here to see if the consumer is requesting a particular servicname for the directory.
	//For this example this is disregarded and the whole directory will be sent.
	//RFA_String requestedServiceName = reqMsg.getAttribInfo().getServiceName();

	rfa::common::RespStatus rStatus;
	rStatus.setStreamState(RespStatus::OpenEnum);
	rStatus.setDataState(RespStatus::OkEnum);
	rStatus.setStatusCode(RespStatus::NoneEnum);
	rfa::common::RFA_String text("Refresh Completed");
	rStatus.setStatusText(text);
    
    //Add capability
    _encoder.setDomainModelType(6); //MMT_MARKET_PRICE
    
	rfa::message::RespMsg respMsg;
	_encoder.encodeDirectoryMsg(&respMsg, reqMsg.getAttribInfo(), rStatus);

	Map dataBody;
	QualityOfService QoS;
	QoS.setRate(QualityOfService::tickByTick);
	QoS.setTimeliness(QualityOfService::realTime);
	if(pCSH)
		dataBody.setAssociatedMetaInfo(*pCSH);
	_encoder.encodeDirectoryDataBody(&dataBody, _serviceName, _vendorName, _serviceState, &QoS);
	respMsg.setPayload(dataBody);

	rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
	itemCmd.setMsg(static_cast<rfa::common::Msg&>(respMsg));

	//This request token generally should be saved for future possible logouts
	//itemCmd.setRequestToken(rToken);
    itemCmd.setRequestToken(*_pDirectoryToken);
	//submitCmd(&itemCmd, 0, "MMT_DIRECTORY Refresh");
    _pOMMProvider->submit(&itemCmd);
}

void OMMInteractiveProvider::processDictionaryReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event) {
    const rfa::message::ReqMsg& reqMsg = static_cast<const rfa::message::ReqMsg&>(event.getMsg());
	rfa::sessionLayer::RequestToken& rToken = event.getRequestToken();
	//const rfa::common::Handle* pCSH = rToken.getHandle();

	UInt8 verbosity = rfa::rdm::DICTIONARY_NORMAL;
	if((reqMsg.getAttribInfo().getHintMask() & AttribInfo::DataMaskFlag))
		verbosity = reqMsg.getAttribInfo().getDataMask();

	const rfa::common::Handle* handle = event.getRequestToken().getHandle();

	rfa::common::UInt8 indicationMask = 0;
	bool complete = false;

	if(reqMsg.getAttribInfo().getName()== "RWFFld")
	{
		do {

			rfa::message::RespMsg respMsg;
			Series dictionaryBody;
			rfa::common::RespStatus rStatus;
			rStatus.setStreamState(RespStatus::OpenEnum);
			rStatus.setDataState(RespStatus::OkEnum);
			rStatus.setStatusCode(RespStatus::NoneEnum);

			if(handle)
				dictionaryBody.setAssociatedMetaInfo(*handle);
			complete = _rRDMFieldDictionary.encodeRDMFieldDictionary(dictionaryBody, verbosity, false);
			if(complete)
			{
				indicationMask = respMsg.getIndicationMask() | RespMsg::RefreshCompleteFlag;
				rStatus.setStatusText(RFA_String("Refresh Completed", 0, false));
			}
			else
			{
				indicationMask = respMsg.getIndicationMask();
				rStatus.setStatusText(RFA_String("", 0, false));
			}
			_encoder.encodeDictionaryMsg(&respMsg, reqMsg.getAttribInfo(), rStatus, indicationMask);
			respMsg.setPayload(dictionaryBody);

			rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
			itemCmd.setMsg(static_cast<rfa::common::Msg&>(respMsg));
			itemCmd.setRequestToken(rToken);
			//submitCmd(&itemCmd, 0, "MMT_DICTIONARY Refresh");
            _pOMMProvider->submit(&itemCmd);
		} while(!complete);
	}
	else if(reqMsg.getAttribInfo().getName()== "RWFEnum")
	{
		//int numFrag = 0;
		do {

			rfa::message::RespMsg respMsg;
			Series dictionaryBody;
			rfa::common::RespStatus rStatus;
			rStatus.setStreamState(RespStatus::OpenEnum);
			rStatus.setDataState(RespStatus::OkEnum);
			rStatus.setStatusCode(RespStatus::NoneEnum);

			if(handle)
				dictionaryBody.setAssociatedMetaInfo(*handle);
			complete = _rRDMFieldDictionary.encodeRDMEnumDictionary(dictionaryBody, verbosity, false);
			if(complete)
			{
				indicationMask = respMsg.getIndicationMask() | RespMsg::RefreshCompleteFlag;
				rStatus.setStatusText(RFA_String("Refresh Completed", 0, false));
			}
			else
			{
				indicationMask = respMsg.getIndicationMask();
				rStatus.setStatusText(RFA_String("", 0, false));
			}
			_encoder.encodeDictionaryMsg(&respMsg, reqMsg.getAttribInfo(), rStatus, indicationMask);
			respMsg.setPayload(dictionaryBody);

			rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
			itemCmd.setMsg(static_cast<rfa::common::Msg&>(respMsg));
			itemCmd.setRequestToken(rToken);

			//submitCmd(&itemCmd, 0, "MMT_DICTIONARY Refresh");
            _pOMMProvider->submit(&itemCmd);
		} while(!complete);
	}
}

void OMMInteractiveProvider::processMarketPriceReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event, boost::python::tuple& out) {
    dict d;
    const rfa::message::ReqMsg& reqMsg = static_cast<const rfa::message::ReqMsg&>(event.getMsg());
	rfa::sessionLayer::RequestToken& rToken = event.getRequestToken();
	const rfa::common::Handle* pCSH = rToken.getHandle();

	rfa::sessionLayer::OMMSolicitedItemCmd solItemCmd;
    _respMsg.clear();
	if ( reqMsg.getInteractionType() == (rfa::message::ReqMsg::InitialImageFlag | rfa::message::ReqMsg::InterestAfterRefreshFlag) )
	{
		_providerWatchListLock.lock();

		ClientWatchList* cwl = _providerWatchList.getClientWatchList(pCSH);

		if ( !cwl )
		{
			_providerWatchListLock.unlock();
			return;
		}

		ClientWatchList::TokenInfo* pTS = cwl->getTokenInfo(&rToken);
		if ( !pTS )
		{
			//A new streaming request, delegate submit to the timer by adding it to the watch list
			pTS = cwl->addToken(&rToken, reqMsg, true);
		}
		else
		{
			//A re-request of an already watched streaming request, submit on demand
            rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
            
            _respStatus.clear();
            _respStatus.setStreamState(RespStatus::OpenEnum);
            _respStatus.setDataState(RespStatus::OkEnum);    
            _respStatus.setStatusCode(RespStatus::NoneEnum);
            _respStatus.setStatusText(RFA_String("Refresh Completed", 0, false));

            _qos.setRate(QualityOfService::tickByTick);
            _qos.setTimeliness(QualityOfService::realTime);

            _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::RefreshEnum, pTS->attribInfo,  _respStatus, &_qos, true, true);

            itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
            itemCmd.setRequestToken(*pTS->pToken);

            _pOMMProvider->submit(&itemCmd);
		}
		_providerWatchListLock.unlock();
        
        d["MTYPE"] = "REQUEST";
        d["RIC"] = reqMsg.getAttribInfo().getName().c_str();
        d["USERNAME"] = cwl->first()->attribInfo.getName().c_str();
        d["SERVICE"] = _serviceName.c_str();
        d["SESSIONID"] = boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH));
        out += boost::python::make_tuple(d);
	}
	else if ( reqMsg.getInteractionType() & ReqMsg::InitialImageFlag )
	{
		//A snapshot request, we don't watch snapshot requests, submit on demand
        rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
            
        _respStatus.clear();
        _respStatus.setStreamState(RespStatus::NonStreamingEnum);
        _respStatus.setDataState(RespStatus::OkEnum);    
        _respStatus.setStatusCode(RespStatus::NoneEnum);
        _respStatus.setStatusText(RFA_String("Refresh Completed", 0, false));

        _qos.setRate(QualityOfService::tickByTick);
        _qos.setTimeliness(QualityOfService::realTime);

        _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::RefreshEnum, reqMsg.getAttribInfo(),  _respStatus, &_qos, true, true);

        itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
        itemCmd.setRequestToken(rToken);

        _pOMMProvider->submit(&itemCmd);
	}
}

void OMMInteractiveProvider::processCloseReq(const rfa::sessionLayer::OMMSolicitedItemEvent& event, boost::python::tuple& out) {
    dict d;
	rfa::sessionLayer::RequestToken& rToken = event.getRequestToken();
	rfa::common::Handle* pCSH = event.getHandle();
	ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

	// make sure that the client watch list still is there
	if ( !cwl ) return;

	// make sure that token info was found
	ClientWatchList::TokenInfo* tokenInfoPtr = cwl->getTokenInfo( &rToken );

	if ( !tokenInfoPtr ) return;

	if ( tokenInfoPtr->bItemRequest )
	{
		// this is item token, just remove this token and its info from the watchlist
        _log += tokenInfoPtr->attribInfo.getName();
        d["MTYPE"] = "CLOSE";
        d["RIC"] = tokenInfoPtr->attribInfo.getName().c_str();
        d["USERNAME"] = cwl->first()->attribInfo.getName().c_str();
        d["SERVICE"] = _serviceName.c_str();
        d["SESSIONID"] = boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH));
        out += boost::python::make_tuple(d);
		cwl->removeToken( &rToken );
	}
	else
	{
		// this is login token, need to remove all item tokens associated with
		// this client session
        _log += tokenInfoPtr->attribInfo.getName();
        // remove all token in removeClientWatchList when session disconnection
		//cwl->cleanup();
	}
}

void OMMInteractiveProvider::cleanup()
{
    ProviderWatchList::CLIENT_SESSIONS clientSessions;
	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); pos++ )
	{
		_pOMMProvider->unregisterClient(const_cast<rfa::common::Handle*>(clientSessions[pos]));
	}

	_providerWatchListLock.unlock();
	_providerWatchList.cleanup();
    _rRDMFieldDictionary.destroy();
}

void OMMInteractiveProvider::directorySubmit(const UInt8 &domainType, rfa::common::RFA_String& serviceName)
{
}

void OMMInteractiveProvider::clearPublishedItemList() {
}

void OMMInteractiveProvider::submitData(const rfa::common::RFA_String& item, const rfa::common::RFA_Vector<rfa::common::RFA_String>& fieldList, int domainType, const std::string& msgType, rfa::common::RFA_String& serviceName, const std::string& session, const std::string& mapAction, const std::string& mapKey)
{
    rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		const rfa::common::Handle* pCSH = clientSessions[pos];
        if((boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH)) != session) && session.length())
            continue;

		ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

		if(!cwl || !cwl->first()) continue;

		ClientWatchList::TokenInfo* pTS = cwl->first();
		while(pTS)
		{
			if(pTS->bItemRequest && (pTS->attribInfo.getName() == item))//Need to skip login request token
			{
				//rfa::common::RFA_String reason("Solicited");
				//reason += " MMT_MARKET_PRICE ";
				//rfa::common::RespStatus status;
                _respMsg.clear();
				if(!pTS->bSubmitted || msgType == "image")
				{
                    _respStatus.clear();
                    _respStatus.setStreamState(RespStatus::OpenEnum);
                    _respStatus.setDataState(RespStatus::OkEnum);    
                    _respStatus.setStatusCode(RespStatus::NoneEnum);
                    _respStatus.setStatusText(RFA_String("Refresh Completed", 0, false));
                    
                    _qos.setRate(QualityOfService::tickByTick);
                    _qos.setTimeliness(QualityOfService::realTime);
                    
                    _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::RefreshEnum, pTS->attribInfo,  _respStatus, &_qos, true, true); 
                    _encoder.encodeMarketPriceDataBody(&_fieldList, RespMsg::RefreshEnum, fieldList, _pDict);
                    _respMsg.setPayload(_fieldList);
                    
                    itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
					itemCmd.setRequestToken(*pTS->pToken);

                    _pOMMProvider->submit(&itemCmd);
					pTS->bSubmitted = true;
                    break;
				}
				else
				{
                    _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::UpdateEnum, pTS->attribInfo,  _respStatus, &_qos, pTS->bAttribInfoInUpdates, true); 
                    _encoder.encodeMarketPriceDataBody(&_fieldList, RespMsg::RefreshEnum, fieldList, _pDict);
                    _respMsg.setPayload(_fieldList);
					itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
					itemCmd.setRequestToken(*pTS->pToken);
                    _pOMMProvider->submit(&itemCmd);
                    break;
				}
			}
			pTS = cwl->next(pTS->pToken);
		}
	}

	_providerWatchListLock.unlock();
}

void OMMInteractiveProvider::closeSubmit(const rfa::common::RFA_String& item, const std::string& session) {
    rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		const rfa::common::Handle* pCSH = clientSessions[pos];
        if((boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH)) != session) && session.length())
            continue;

		ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

		if(!cwl || !cwl->first()) continue;

		ClientWatchList::TokenInfo* pTS = cwl->first();
		while(pTS)
		{
			if(pTS->bItemRequest && (pTS->attribInfo.getName() == item))//Need to skip login request token
			{
                _respMsg.clear();
                _respStatus.clear();
                _respStatus.setStreamState(RespStatus::ClosedEnum);
                _respStatus.setStatusText(RFA_String("Item Closed", 0, false));

                _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::StatusEnum, pTS->attribInfo,  _respStatus, NULL, true, true); 

                itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
                itemCmd.setRequestToken(*pTS->pToken);

                _pOMMProvider->submit(&itemCmd);
                
                cwl->removeToken(pTS->pToken);
                break;
			}
			pTS = cwl->next(pTS->pToken);     
		}
	}

	_providerWatchListLock.unlock();
    if (_debug) {
        _log = "[OMMInteractiveProvider::closeSubmit]";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        printClientWatchlists();
    }
}

void OMMInteractiveProvider::closeAllSubmit() {
    rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		const rfa::common::Handle* pCSH = clientSessions[pos];
		ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

		if(!cwl || !cwl->first()) continue;

		ClientWatchList::TokenInfo* pTS = cwl->first();
		while(pTS)
		{
			if(pTS->bItemRequest)//Need to skip login request token
			{
                _respMsg.clear();
                _respStatus.clear();
                _respStatus.setStreamState(RespStatus::ClosedEnum);
                _respStatus.setStatusText(RFA_String("Item Closed", 0, false));

                _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::StatusEnum, pTS->attribInfo,  _respStatus, NULL, true, true); 

                itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
                itemCmd.setRequestToken(*pTS->pToken);

                _pOMMProvider->submit(&itemCmd);
                cwl->removeToken(pTS->pToken);
                pTS = cwl->first();
			}
			pTS = cwl->next(pTS->pToken);     
		}
	}

	_providerWatchListLock.unlock();
    if (_debug) {
        _log = "[OMMInteractiveProvider::closeAllSubmit]";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        printClientWatchlists();
    }
}

void OMMInteractiveProvider::staleSubmit(const rfa::common::RFA_String& item, const std::string& session) {
    rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		const rfa::common::Handle* pCSH = clientSessions[pos];
        if((boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH)) != session) && session.length())
            continue;

		ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

		if(!cwl || !cwl->first()) continue;

		ClientWatchList::TokenInfo* pTS = cwl->first();
		while(pTS)
		{
			if(pTS->bItemRequest && (pTS->attribInfo.getName() == item))//Need to skip login request token
			{
                _respMsg.clear();
                _respStatus.clear();
                _respStatus.setStreamState(RespStatus::OpenEnum);
                _respStatus.setDataState(RespStatus::SuspectEnum); 
                _respStatus.setStatusText(RFA_String("Item Stale", 0, false));

                _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::StatusEnum, pTS->attribInfo,  _respStatus, NULL, true, true); 

                itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
                itemCmd.setRequestToken(*pTS->pToken);

                _pOMMProvider->submit(&itemCmd);
                
                //cwl->removeToken(pTS->pToken);
                break;
			}
			pTS = cwl->next(pTS->pToken);     
		}
	}

	_providerWatchListLock.unlock();
    if (_debug) {
        _log = "[OMMInteractiveProvider::staleSubmit]";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        printClientWatchlists();
    }
}

void OMMInteractiveProvider::staleAllSubmit() {
    rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		const rfa::common::Handle* pCSH = clientSessions[pos];
		ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

		if(!cwl || !cwl->first()) continue;

		ClientWatchList::TokenInfo* pTS = cwl->first();
		while(pTS)
		{
			if(pTS->bItemRequest)//Need to skip login request token
			{
                _respMsg.clear();
                _respStatus.clear();
                _respStatus.setStreamState(RespStatus::OpenEnum);
                _respStatus.setDataState(RespStatus::SuspectEnum); 
                _respStatus.setStatusText(RFA_String("Item Stale", 0, false));

                _encoder.encodeMarketPriceMsg(&_respMsg, RespMsg::StatusEnum, pTS->attribInfo,  _respStatus, NULL, true, true); 

                itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
                itemCmd.setRequestToken(*pTS->pToken);

                _pOMMProvider->submit(&itemCmd);
                //cwl->removeToken(pTS->pToken);
                //pTS = cwl->first();
			}
			pTS = cwl->next(pTS->pToken);     
		}
	}

	_providerWatchListLock.unlock();
    if (_debug) {
        _log = "[OMMInteractiveProvider::staleAllSubmit]";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        printClientWatchlists();
    }
}

void OMMInteractiveProvider::serviceStateSubmit(int serviceState, rfa::common::RFA_String& serviceName) {
    _respMsg.clear();
    _map.clear();
    if ( _pDirectoryToken )
        _map.setAssociatedMetaInfo(*_pDirectoryToken->getHandle());
    
    //const UInt8 domainType = rfa::rdm::MMT_DIRECTORY;
    
    // Encode msg header
    _attribInfo.clear();
    _attribInfo.setDataMask(SERVICE_INFO_FILTER | SERVICE_STATE_FILTER);

    /*if(_itemMap.empty()) {
        // directory refresh
        ItemInfo *itemInfo = new ItemInfo();
        itemInfo->name = serviceName;
        itemInfo->srvName = serviceName;
        itemInfo->token = &(_pOMMProvider->generateItemToken());
        _itemMap.push_back(itemInfo);
    }*/
    // Set RespTypeNum
    _respStatus.clear();
    _respStatus.setStreamState(RespStatus::OpenEnum);
    _respStatus.setDataState(RespStatus::SuspectEnum);    
    _respStatus.setStatusCode(RespStatus::NoneEnum);
    RFA_String  tmpStr("Service state updated");
    _respStatus.setStatusText(tmpStr);

    _encoder.encodeDirectoryMsg(&_respMsg, _attribInfo, _respStatus, false);

    // Reset respTypeNum to unsolicited refresh
    //_respMsg.setRespTypeNum(rfa::rdm::REFRESH_UNSOLICITED);

    // Encode Data Body 
    _qos.setRate(QualityOfService::realTime);
    _qos.setTimeliness(QualityOfService::tickByTick);

    _encoder.encodeDirectoryDataBody(&_map,serviceName,_vendorName, serviceState, &_qos);    
    _respMsg.setPayload(_map);

    _log = "[OMMInteractiveProvider::serviceStateSubmit] Updating directory with service state: ";
    _log.append(serviceState);
    _log.append(" for service ");
    _log.append(serviceName);
    _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
    
    rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
    itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
    //itemCmd.setItemToken(_itemMap[0]->token);
    
    itemCmd.setRequestToken(*_pDirectoryToken);
    _pOMMProvider->submit(&itemCmd);
}

void OMMInteractiveProvider::logoutSubmit(const std::string& session) {
    rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		rfa::common::Handle* pCSH = const_cast<rfa::common::Handle*>(clientSessions[pos]);
		ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

		if(!cwl || !cwl->first()) continue;

		ClientWatchList::TokenInfo* pTS = cwl->first();
		if(!pTS->bItemRequest && (boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH)) == session))
        {
            // Logging out a client session
            _respMsg.clear();
            _respStatus.clear();
            _respStatus.setStreamState(RespStatus::ClosedEnum);
            _respStatus.setDataState(RespStatus::SuspectEnum);
            _respStatus.setStatusText(RFA_String("Provider logout", 0, false));
            _respMsg.setRespStatus(_respStatus);
            _respMsg.setAttribInfo(pTS->attribInfo);
            _respMsg.setMsgModelType(rfa::rdm::MMT_LOGIN);
            _respMsg.setRespType(RespMsg::StatusEnum);
            itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
            itemCmd.setRequestToken(*pTS->pToken);
            _pOMMProvider->submit(&itemCmd);
            break;
        }
	}
    
	_providerWatchListLock.unlock();
    if (_debug) {
        _log = "[OMMInteractiveProvider::logoutSubmit]";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        printClientWatchlists();
    }
}

void OMMInteractiveProvider::logoutAllSubmit() {
    rfa::sessionLayer::OMMSolicitedItemCmd itemCmd;
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		rfa::common::Handle* pCSH = const_cast<rfa::common::Handle*>(clientSessions[pos]);
		ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

		if(!cwl || !cwl->first()) continue;

		ClientWatchList::TokenInfo* pTS = cwl->first();
		if(!pTS->bItemRequest)
        {
            // Logging out a client session
            _respMsg.clear();
            _respStatus.clear();
            _respStatus.setStreamState(RespStatus::ClosedEnum);
            _respStatus.setDataState(RespStatus::SuspectEnum);
            _respStatus.setStatusText(RFA_String("Provider logout", 0, false));
            _respMsg.setRespStatus(_respStatus);
            _respMsg.setAttribInfo(pTS->attribInfo);
            _respMsg.setMsgModelType(rfa::rdm::MMT_LOGIN);
            _respMsg.setRespType(RespMsg::StatusEnum);
            itemCmd.setMsg(static_cast<rfa::common::Msg&>(_respMsg));
            itemCmd.setRequestToken(*pTS->pToken);
            _pOMMProvider->submit(&itemCmd);
        }
	}
    
	_providerWatchListLock.unlock();
    if (_debug) {
        _log = "[OMMInteractiveProvider::logoutAllSubmit]";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        printClientWatchlists();
    }
}

void OMMInteractiveProvider::rejectSessionSubmit(const std::string& session) {
    rfa::sessionLayer::OMMClientSessionCmd clientSessionCmd;
    rfa::sessionLayer::ClientSessionStatus clientSessionStatus;
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		rfa::common::Handle* pCSH = const_cast<rfa::common::Handle*>(clientSessions[pos]);

		if(boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH)) == session)
        {
            // Reject the client session
            clientSessionCmd.setClientSessionHandle(pCSH);
            clientSessionStatus.setState(rfa::sessionLayer::ClientSessionStatus::Inactive);
            clientSessionStatus.setStatusCode(rfa::sessionLayer::ClientSessionStatus::Reject);
            clientSessionCmd.setStatus(clientSessionStatus);
            _pOMMProvider->submit(&clientSessionCmd);
            
            // Unregister client
            //_pOMMProvider->unregisterClient(pCSH);
            
            // Must remove as inactive clien event not invoked
            _providerWatchList.removeClientWatchList(pCSH);
            break;
        }
	}
    
	_providerWatchListLock.unlock();
    if (_debug) {
        _log = "[OMMInteractiveProvider::rejectSessionSubmit]";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        printClientWatchlists();
    }
}

void OMMInteractiveProvider::rejectSessionAllSubmit() {
    rfa::sessionLayer::OMMClientSessionCmd clientSessionCmd;
    rfa::sessionLayer::ClientSessionStatus clientSessionStatus;
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );

	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		rfa::common::Handle* pCSH = const_cast<rfa::common::Handle*>(clientSessions[pos]);

        // Reject the client session
        clientSessionCmd.setClientSessionHandle(pCSH);
        clientSessionStatus.setState(rfa::sessionLayer::ClientSessionStatus::Inactive);
        clientSessionStatus.setStatusCode(rfa::sessionLayer::ClientSessionStatus::Reject);
        clientSessionCmd.setStatus(clientSessionStatus);
        _pOMMProvider->submit(&clientSessionCmd);

        // Unregister client
        //_pOMMProvider->unregisterClient(pCSH);
        
        // Must remove as inactive clien event not invoked
        _providerWatchList.removeClientWatchList(pCSH);
	}
    
	_providerWatchListLock.unlock();
    if (_debug) {
        _log = "[OMMInteractiveProvider::rejectSessionAllSubmit]";
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
        printClientWatchlists();
    }
}

void OMMInteractiveProvider::setDebugMode(const bool &debug) {
    _debug = debug;
    _encoder.setDebugMode(_debug);
}

bool OMMInteractiveProvider::addClientWatchList(const rfa::common::Handle* pCSH) {
    bool result = false;
    _providerWatchListLock.lock();
    result = _providerWatchList.addClientWatchList(pCSH);
    _providerWatchListLock.unlock();
    return result;
}

bool OMMInteractiveProvider::removeClientWatchList(const rfa::common::Handle* pCSH, boost::python::tuple& out) {
    dict d;
    bool result = false;
    _providerWatchListLock.lock();
    ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );
    
    if(!cwl || !cwl->first()) return result;

    d["MTYPE"] = "LOGOUT";
    d["USERNAME"] = cwl->first()->attribInfo.getName().c_str();
    d["SERVICE"] = _serviceName.c_str();
    d["SESSIONID"] = boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH));
    out += boost::python::make_tuple(d);
    cwl->cleanup();
    
    result = _providerWatchList.removeClientWatchList(pCSH);
    _providerWatchListLock.unlock();
    return result;
}

void OMMInteractiveProvider::printClientWatchlists() {
	ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );
    
	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
        _log = "[OMMInteractiveProvider::printClientWatchlists] ";
		const rfa::common::Handle* pCSH = clientSessions[pos];
		ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

		if(!cwl || !cwl->first()) continue;

		ClientWatchList::TokenInfo* pTS = cwl->first();
		while(pTS)
		{
            _log += pTS->attribInfo.getName();
            _log += " ";
			pTS = cwl->next(pTS->pToken);     
		}
        _componentLogger.log(LM_GENERIC_ONE,rfa::common::Information,_log.c_str());
	}

	_providerWatchListLock.unlock();
}

std::string OMMInteractiveProvider::getClientSessions() {
    std::string sessions;
    ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );
    
	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		const rfa::common::Handle* pCSH = clientSessions[pos];
        sessions.append(boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH)));
        sessions.append(" ");
	}
	_providerWatchListLock.unlock();
    return sessions;
}


std::string OMMInteractiveProvider::getClientWatchList(const std::string& session) {
    std::string watchlist;
    ProviderWatchList::CLIENT_SESSIONS clientSessions;

	_providerWatchListLock.lock();
	_providerWatchList.getClientSessions( clientSessions );
    
	for ( unsigned int pos = 0; pos < clientSessions.size(); ++pos )
	{
		const rfa::common::Handle* pCSH = clientSessions[pos];
        if(boost::lexical_cast<std::string>(reinterpret_cast<unsigned long>(pCSH)) == session) {
            ClientWatchList* cwl = _providerWatchList.getClientWatchList( pCSH );

            if(!cwl || !cwl->first()) continue;

            ClientWatchList::TokenInfo* pTS = cwl->first();
            while(pTS)
            {
                if(pTS->bItemRequest) {//skip login token
                    watchlist.append(pTS->attribInfo.getName().c_str());
                    watchlist.append(" ");
                }
                pTS = cwl->next(pTS->pToken);
            }
            break;
        }
	}
	_providerWatchListLock.unlock();
    return watchlist;
}

///////////////////////////////////////////////////////////////////////////////
// ClientWatchList implementation
///////////////////////////////////////////////////////////////////////////////

ClientWatchList::ClientWatchList( const Handle* clientSessionHandle ) :
 _clientSessionHandle( clientSessionHandle )
{
}

ClientWatchList::~ClientWatchList()
{
	cleanup();
}

void ClientWatchList::cleanup()
{
	for(TokenInfoMap::iterator iter = _watchlist.begin(); iter != _watchlist.end(); iter++)
		delete iter->second;
	_watchlist.clear();
}

ClientWatchList::TokenInfo* ClientWatchList::addToken(rfa::sessionLayer::RequestToken* pT, const rfa::message::ReqMsg& reqMsg, bool isItemReq)
{
	TokenInfoMap::iterator iter = _watchlist.find(pT);
    
	if(iter != _watchlist.end())
		return 0;

	TokenInfo* sub = new TokenInfo();
	sub->pToken = pT;
	sub->attribInfo = reqMsg.getAttribInfo();
	sub->bAttribInfoInUpdates = (reqMsg.getIndicationMask() & rfa::message::ReqMsg::AttribInfoInUpdatesFlag) ? true : false;
	sub->bSubmitted = false;
	sub->bItemRequest = isItemReq;
	sub->msgModelType = reqMsg.getMsgModelType();
	_watchlist.insert(std::pair<rfa::sessionLayer::RequestToken*, TokenInfo*>(pT, sub));
	return sub;
}

ClientWatchList::TokenInfo* ClientWatchList::getTokenInfo(rfa::sessionLayer::RequestToken* pT)
{
	TokenInfoMap::iterator iter = _watchlist.find(pT);
	if(iter == _watchlist.end())
		return 0;
	return iter->second;
}

bool ClientWatchList::removeToken(rfa::sessionLayer::RequestToken* pT)
{
	TokenInfoMap::iterator iter = _watchlist.find(pT);
	if(iter == _watchlist.end())
		return false;
	delete iter->second;
	_watchlist.erase(iter);
	return true;
}

ClientWatchList::TokenInfo* ClientWatchList::first()
{
	ClientWatchList::TokenInfoMap::iterator iter = _watchlist.begin();
	if(iter == _watchlist.end())
		return 0;
	return iter->second;
}

ClientWatchList::TokenInfo* ClientWatchList::next(rfa::sessionLayer::RequestToken* pT)
{
	ClientWatchList::TokenInfoMap::iterator iter = _watchlist.find(pT);
	if(iter == _watchlist.end()) return 0;
	if(++iter == _watchlist.end()) return 0;
	return iter->second;
}

ProviderWatchList::ProviderWatchList()
{
}

ProviderWatchList::~ProviderWatchList()
{
	_watchList.clear();
}

bool ProviderWatchList::addClientWatchList( const Handle* pCSH )
{
	CSH_MAP::iterator iter = _watchList.find( pCSH );

	if ( iter != _watchList.end() ) return false;

	ClientWatchList* cwl = new ClientWatchList( pCSH );
	_watchList.insert( std::pair< const Handle*, ClientWatchList*>( pCSH, cwl ) );

	return true;
}

bool ProviderWatchList::removeClientWatchList( const Handle* pCSH )
{
	CSH_MAP::iterator iter = _watchList.find( pCSH );

	if ( iter == _watchList.end() ) return false;

	delete iter->second;
	_watchList.erase(iter);

	return true;
}

ClientWatchList* ProviderWatchList::getClientWatchList( const Handle* pCSH )
{
	CSH_MAP::iterator iter = _watchList.find( pCSH );

	if ( iter == _watchList.end() ) return 0;

	return iter->second;
}

bool ProviderWatchList::cleanup()
{
	for(CSH_MAP::iterator iter = _watchList.begin(); iter != _watchList.end(); ++iter)
	{
		iter->second->cleanup();
		delete iter->second;
	}
	return true;
}

void ProviderWatchList::getClientSessions(CLIENT_SESSIONS& sessions)
{
	sessions.clear();
	for(CSH_MAP::iterator iter = _watchList.begin(); iter != _watchList.end(); ++iter)
		sessions.push_back(iter->first);
}

void ProviderWatchList::findTokens(const RFA_String& itemName, UInt8 msgModelType, TOKENS& tokens, CLIENT_SESSIONS& cshs)
{
	tokens.clear();
	CLIENT_SESSIONS clientSessions;
	getClientSessions(clientSessions);
	for(unsigned int pos = 0; pos < clientSessions.size(); pos++)
	{
		ClientWatchList* cwl = getClientWatchList(clientSessions[pos]);
		if(!cwl || !cwl->first()) continue;
		ClientWatchList::TokenInfo* pTS = cwl->first();
		while(pTS)
		{
			if(pTS->attribInfo.getName() == itemName && pTS->msgModelType == msgModelType)
			{
				tokens.push_back(pTS->pToken);
				cshs.push_back(clientSessions[pos]);
			}
			pTS = cwl->next(pTS->pToken);
		}
	}
}
