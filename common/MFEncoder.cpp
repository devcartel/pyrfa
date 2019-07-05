#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#include "MFEncoder.h"

CMFEncoder::CMFEncoder():
_msgType(CMessageType::SnapResponse),
_header(0),
_fidBuffer(0),
_buffer(0),
_ricName(""),
_tag(""),
_rStatus(""),
_fieldListNo(0),
_rtl(""),
_deltaRTL(""),
_verSub("")
{
}

CMFEncoder::~CMFEncoder()
{
	clearBuffer();
}

void CMFEncoder::setMessageType(CMessageType::MessageTypeEnums msgType)
{
	_msgType = msgType;
}

const CMessageType::MessageTypeEnums CMFEncoder::getMessageType() const
{
	return _msgType;
}

void CMFEncoder::setRICName(const RFA_String & ricName)
{
	_ricName = ricName;
}

const RFA_String & CMFEncoder::getRICName() const
{
	return _ricName;
}

void CMFEncoder::setTag(const RFA_String & tag)
{
	_tag = tag;
}

const RFA_String & CMFEncoder::getTag() const
{
	return _tag;
}

void CMFEncoder::setRStatus(const RFA_String & rStatus)
{
	_rStatus = rStatus;
}

const RFA_String & CMFEncoder::getRStatus() const
{
	return _rStatus;
}

void CMFEncoder::setFieldListNumber(const int fieldListNo)
{
	_fieldListNo = fieldListNo;
}

const int CMFEncoder::getFieldListNumber() const
{
	return _fieldListNo;
}

void CMFEncoder::setRTL(const RFA_String & rtl)
{
	_rtl = rtl;
}

const RFA_String & CMFEncoder::getRTL() const
{
	return _rtl;
}

void CMFEncoder::setDeltaRTL(const RFA_String & deltaRTL)
{
	_deltaRTL = deltaRTL;
}

const RFA_String & CMFEncoder::getDeltaRTL() const
{
	return _deltaRTL;
}

void CMFEncoder::setBuffer(const RFA_String & strBuffer)
{
	if ( !_buffer )
		_buffer = new char [MAX_BUFFER];

	strncpy ( _buffer, strBuffer.c_str(), strBuffer.length() + 1 );
}

const char * CMFEncoder::getBuffer() const
{
	return _buffer;
}

void CMFEncoder::setVerSub(const RFA_String & verSub)
{
	_verSub = verSub;
}

const RFA_String & CMFEncoder::getVerSub() const
{
	return _verSub;
}

unsigned long CMFEncoder::headerLength()
{
	return _header ? strlen ( _header ) : 0;
}

unsigned long CMFEncoder::fidBufferLength()
{
	return _fidBuffer ? strlen ( _fidBuffer ) : 0;
}

unsigned long CMFEncoder::bufferLength()
{
	return _buffer ? strlen ( _buffer ) : 0;
}

void CMFEncoder::makeHeader()
{
	stringstream strHeader;
	strHeader << RTR_FS << (int) _msgType;
	
	if ( _msgType != CMessageType::AggregateUpdate )
		strHeader << RTR_US << _tag.c_str();
		
	strHeader << RTR_GS << _ricName.c_str();

	switch (_msgType)
	{
	case CMessageType::SnapResponse :
		if ( !_rStatus.empty() )
			strHeader << RTR_RS << _rStatus.c_str();
		
		strHeader << RTR_US << _fieldListNo;
		
		break;
		
	case CMessageType::Update :
	case CMessageType::Correction :
	case CMessageType::ClosingRun :
		break;

	case CMessageType::Verify :
		if ( !_verSub.empty() )
			strHeader << RTR_RS << _verSub.c_str();

		strHeader << RTR_US << _fieldListNo;

		break;

	case CMessageType::AggregateUpdate :
		if ( !_deltaRTL.empty() )
			strHeader << RTR_US << _deltaRTL.c_str();

		break;
	}
	
	strHeader << RTR_US << _rtl.c_str();
	strHeader << '\0';

	if ( !_header )
		_header = new char [MAX_BUFFER];
	
	strncpy ( _header, strHeader.str().c_str(), strHeader.str().length() + 1 );
}

void CMFEncoder::makeIntBuffer()
{
	if ( !_buffer )
		_buffer = new char [MAX_BUFFER];

	strncpy ( _buffer, _header, headerLength() + 1 );
	unsigned long length = headerLength();

	if ( _fidBuffer )
	{
		strncat ( _buffer, _fidBuffer, fidBufferLength() + 1 );
		length = bufferLength();
	}
	
	if ( _buffer[length] != RTR_FS )
	{
		stringstream rtrFS;
		rtrFS << RTR_FS << '\0';
		strncat ( _buffer, rtrFS.str().c_str(), rtrFS.str().length() + 1 );
	}
}

const char * CMFEncoder::makeBuffer()
{
	makeHeader();
	makeIntBuffer();

	return _buffer;
}

void CMFEncoder::clearBuffer()
{
	if ( _fidBuffer )
	{
		delete [] _fidBuffer;
		_fidBuffer = 0;
	}

	if ( _buffer )
	{
		delete [] _buffer;
		_buffer = 0;
	}

	clearHeader();
}

void CMFEncoder::clearHeader()
{
	if ( _header )
	{
		delete [] _header;
		_header = 0;
	}
}

void CMFEncoder::appendField(const RFA_String & fid, const RFA_String & value)
{	
	//Allow append field after making buffer
	stringstream strStr;
	
	strStr << RTR_RS << fid.c_str();
	strStr << RTR_US << value.c_str();
	strStr << '\0';
	
	if ( !_fidBuffer )
	{
		_fidBuffer = new char [ MAX_BUFFER ];
		strncpy ( _fidBuffer, strStr.str().c_str(), strStr.str().length() + 1 );
	}
	else
		strncat ( _fidBuffer, strStr.str().c_str(), strStr.str().length() + 1 );

	//Client is needed to call makeBuffer() when finishs to append fields in the _buffer
	//makeIntBuffer();
}

void CMFEncoder::changeFieldValue(const RFA_String & fid, const RFA_String & newValue)
{
	int startFieldPos, startValPos, endValPos;
	RFA_String strBuffer(_fidBuffer, fidBufferLength(), false);
	
	stringstream rtrFID;

	rtrFID << RTR_RS << fid.c_str() << RTR_US << '\0';
	startFieldPos = strBuffer.find( rtrFID.str().c_str() ) + 1;
	
	if ( startFieldPos > 0 )
	{
		stringstream rtrUS, rtrRS;
		rtrUS << RTR_US << '\0';
		startValPos = strBuffer.find( rtrUS.str().c_str(), startFieldPos) + 1;
		
		rtrRS << RTR_RS << '\0';
		endValPos = strBuffer.find( rtrRS.str().c_str(), startValPos);

		if ( endValPos == -1 )
			endValPos = fidBufferLength();

		if ( (int)newValue.length() != ( endValPos - startValPos ) )
		{
			stringstream tmpStr;
			tmpStr << _fidBuffer + endValPos << '\0';

			memcpy ( _fidBuffer + startValPos, newValue.c_str(), newValue.length() );
			memcpy ( _fidBuffer + startValPos + newValue.length(), (const char *) tmpStr.str().c_str(), tmpStr.str().length() );
		}
		else
			memcpy ( _fidBuffer + startValPos, newValue.c_str(), newValue.length() );
		
		makeIntBuffer();
	}
	//else : Not found the fid in _buffer so do nothing
}

void CMFEncoder::removeField(const RFA_String & fid)
{
	int startFieldPos, endValPos;
	RFA_String strBuffer(_fidBuffer, fidBufferLength(), false);
	
	stringstream rtrFID;

	rtrFID << RTR_RS << fid.c_str() << RTR_US << '\0';
	startFieldPos = strBuffer.find( rtrFID.str().c_str() ) + 1;

	if ( startFieldPos > 0 )
	{
		stringstream rtrRS;
		rtrRS << RTR_RS << '\0';
		endValPos = strBuffer.find( rtrRS.str().c_str(), startFieldPos);

		if ( endValPos == -1 )
			endValPos = fidBufferLength() - 1;

		stringstream tmpStr;
		tmpStr << _fidBuffer + endValPos + 1 << '\0';

		memset ( _fidBuffer + startFieldPos, 0, fidBufferLength() - startFieldPos );
		strncat ( _fidBuffer, tmpStr.str().c_str(), tmpStr.str().length() + 1 );

		makeIntBuffer();
	}
}

