#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#include "MDEncoderFormatter.h"
#include "MFEncoder.h"

CMDEncoderFormatter::CMDEncoderFormatter():
_dataFormat(MarketDataEnums::Unknown),
_mfEncoder(0)
{
}

CMDEncoderFormatter::~CMDEncoderFormatter()
{
	if ( _mfEncoder )
		delete _mfEncoder;
}

void CMDEncoderFormatter::setDataFormat(MarketDataEnums::DataFormat dataFormat)
{
	_dataFormat = dataFormat;
}

const MarketDataEnums::DataFormat CMDEncoderFormatter::getDataFormat() const
{
	return _dataFormat;
}

void CMDEncoderFormatter::clearBuffer()
{
	if ( _mfEncoder )
		_mfEncoder->clearBuffer();

	_tibMsg.ReUse();

	_page.reset( _updateList, MAX_PAGEBUFFER );
}

void CMDEncoderFormatter::initializeMarketfeedData()
{
	if ( !_mfEncoder )
		_mfEncoder = new CMFEncoder();
}

const char * CMDEncoderFormatter::getMarketfeedDataBuffer() const
{
	if ( !_mfEncoder )
		return 0;

	return _mfEncoder->getBuffer();
}

Buffer CMDEncoderFormatter::FormatMarketfeedImageBuffer(const RFA_String & ricName)
{
	if ( !_mfEncoder )
	{
		cout << "Marketfeed buffer must be inintialized before formatting by calling initializeMarketfeedData()" << endl;
		return 0;
	}

	if ( _dataFormat == MarketDataEnums::Marketfeed || 
		_dataFormat == MarketDataEnums::ANSI_Page )
	{
		_mfEncoder->clearHeader();

		_mfEncoder->setMessageType(CMessageType::SnapResponse);
		_mfEncoder->setTag(RFA_String("XXX", 0, false));
		_mfEncoder->setRICName(ricName);
		_mfEncoder->setFieldListNumber(2);
		_mfEncoder->setRTL(RFA_String("0", 0, false));

		_mfEncoder->makeBuffer();
		return Buffer((unsigned char *)_mfEncoder->getBuffer(), strlen(_mfEncoder->getBuffer()));
	}
	else 
	{
		cout << "DataFormat does not match. FormatMarketfeedImageBuffer() supports Marketfeed and ANSI_Page only." << endl;
		return 0;
	}
}

Buffer CMDEncoderFormatter::FormatMarketfeedUpdateBuffer(const RFA_String & ricName) 
{
	if ( !_mfEncoder )
	{
		cout << "Marketfeed buffer must be inintialized before formatting by calling initializeMarketfeedData()" << endl;
		return 0;
	}

	if ( _dataFormat == MarketDataEnums::Marketfeed || 
		_dataFormat == MarketDataEnums::ANSI_Page )
	{
		_mfEncoder->clearHeader();

		_mfEncoder->setMessageType(CMessageType::Update);
		_mfEncoder->setTag(RFA_String("XXX", 0, false));
		_mfEncoder->setRICName(ricName);
		_mfEncoder->setRTL(RFA_String("0", 0, false));

		_mfEncoder->makeBuffer();
		return Buffer((unsigned char *)_mfEncoder->getBuffer(), strlen(_mfEncoder->getBuffer()));
	}
	else 
	{
		cout << "DataFormat does not match. FormatMarketfeedUpdateBuffer() supports Marketfeed and ANSI_Page only." << endl;
		return 0;
	}
}

void CMDEncoderFormatter::makeMarketfeedFields(FieldList fidList, unsigned int uiRand)
{		
	if ( !_mfEncoder )
	{
		cout << "Failed to make marketfeed fields because initializeMarketfeedData() never been called." << endl;
		return;
	}

	if ( _dataFormat == MarketDataEnums::Marketfeed || 
		_dataFormat == MarketDataEnums::ANSI_Page )
	{
		char buffer[10];
		buffer[0] = '\0';
		for ( FieldList::const_iterator cit = fidList.begin(); cit != fidList.end(); )
		{
			SNPRINTF( buffer, sizeof(buffer), "%d", rand_r(&uiRand) );
			_mfEncoder->appendField( *cit++, RFA_String(buffer, 0, false) );
		}
	}
	else 
	{
		cout << "DataFormat does not match. makeMarketfeedFields() supports Marketfeed and ANSI_Page only." << endl;
	}
}

void CMDEncoderFormatter::makeMarketfeedField(const RFA_String &fid,const RFA_String &value)
{
	if ( !_mfEncoder )
	{
		cout << "Failed to make a marketfeed field because initializeMarketfeedData() never been called." << endl;
		return;
	}

	if ( _dataFormat == MarketDataEnums::Marketfeed || 
		_dataFormat == MarketDataEnums::ANSI_Page )
	{
		_mfEncoder->appendField(fid, value);
	}
	else 
	{
		cout << "DataFormat does not match. makeMarketfeedField() supports Marketfeed and ANSI_Page only." << endl;
	}
}

const TibMsg & CMDEncoderFormatter::getTibMsg()
{
	return _tibMsg;
}

void CMDEncoderFormatter::initializeTibMsg(char * buffer, int size)
{
	if ( _dataFormat == MarketDataEnums::QForm || 
		_dataFormat == MarketDataEnums::TibMsgSelfDescribed ||
		 _dataFormat == MarketDataEnums::ANSI_Page )
	{
		_tibMsg = TibMsg ( buffer, size );
	}
	else 
	{
		cout << "DataFormat does not match. initializeTibMsg() supports QForm, TibMsgSelfDescribed and ANSI_Page only." << endl;
	}
}

void CMDEncoderFormatter::makeTibFields(FieldList fidList, unsigned int uiRand)
{
	if ( _dataFormat == MarketDataEnums::QForm || 
		_dataFormat == MarketDataEnums::TibMsgSelfDescribed ||
		 _dataFormat == MarketDataEnums::ANSI_Page )
	{
		for ( FieldList::const_iterator cit = fidList.begin(); cit != fidList.end(); )
			_tibMsg.Append( (TibMsg_name)(*cit++).c_str(), (Tib_i32)rand_r(&uiRand) );

		assert( !_tibMsg.NeedPack() );
	}
	else 
	{
		cout << "DataFormat does not match. makeTibFields() supports QForm, TibMsgSelfDescribed and ANSI_Page only." << endl;
	}
}

void CMDEncoderFormatter::makeTibField(const RFA_String &fid, const void * value, TibMsg_type type, int size)
{
	if ( _dataFormat == MarketDataEnums::QForm ||
		 _dataFormat == MarketDataEnums::TibMsgSelfDescribed ||
		 _dataFormat == MarketDataEnums::ANSI_Page )
	{
		TibErr err;
		if ( size )
			err = _tibMsg.Append( (TibMsg_name) fid.c_str(), type, (TibMsg_size) size, (TibMsg_data) value);
		else 
			err = _tibMsg.Append( (TibMsg_name) fid.c_str(), (const char*) value );	
		
		//assert( !_tibMsg.NeedPack() );
	}
	else 
	{
		cout << "DataFormat does not match. makeTibField() supports QForm, TibMsgSelfDescribed and ANSI_Page only." << endl;
	}
}

Buffer CMDEncoderFormatter::FormatTibMsgDataBuffer()
{
	return Buffer(( unsigned char *)_tibMsg.Packed(), _tibMsg.PackSize(), _tibMsg.PackSize() );
}

void CMDEncoderFormatter::initializePageCell(char cChar,
				PageCell::GraphicSet graphicSet,
				PageCell::CellStyle style,
				PageCell::CellStyle fadeStyle,
				PageCell::CellColor foregroundColor,
				PageCell::CellColor backgroundColor,
				PageCell::CellColor foregroundFadeColor,
				PageCell::CellColor backgroundFadeColor)
{
	if ( _dataFormat == MarketDataEnums::ANSI_Page )
	{
		_modelCell = PageCell( cChar,
							graphicSet,
							style,
							fadeStyle,
							foregroundColor,
							backgroundColor,
							foregroundFadeColor,
							backgroundFadeColor);
		
		for (int i = 0; i < MAX_PAGEBUFFER; i++)
			_strUpdatePageBuff[i] = ' ';
		
		//Create a page on the stack
		//By default it will contain 25 rows and 80 columns
		_page = Page();
	}
	else 
	{
		cout << "DataFormat does not match. initializePageCell() supports ANSI_Page only." << endl;
	}
}

void CMDEncoderFormatter::makePageCells(const char* value, short beginningRow, short beginningColumn)
{
	if ( _dataFormat == MarketDataEnums::ANSI_Page )
	{
		/* The following section will create a raw ansi page in a char buffer */
		/* The page says "TESTING ANSIPAGE PUBLISHING #" where # is a random  */
		/* ansi character.  This is to verify that we are recieving updates   */

		long numUpdates = 0;
	
		//Initialize page using the ANSI "ESC c" sequence
		char strInit[] = "\033c";
		_page.decode( strInit,
				 sizeof( strInit ),
				 _updateList,
				 MAX_UPDATES,
				 &numUpdates );
	
		//Update page with updates
		//and fill in the update list.	

		int indexUpdateList = 0;
		int indexColumn = 0;
		for (int i = 0; i< (int) strlen(value) ; i++ )
		{
			if ( value[i] == 10 ) // the end of row should be finished by an end of line (endl, \n)
			{
				_updateList[indexUpdateList].setRow( beginningRow + indexUpdateList );
				_updateList[indexUpdateList].setBeginningColumn( beginningColumn );
				_updateList[indexUpdateList].setEndingColumn( beginningColumn + indexColumn );
				indexUpdateList++;
				indexColumn = 0;
			}
			else 
			{
				_modelCell.setChar( value[i] );
				_page.setPageCell( beginningRow + indexUpdateList,  beginningColumn + indexColumn, _modelCell );
				indexColumn++;
			}
		}

		long nTextLength = 0;

		//Encodes page and stores in _strUpdatePageBuff
		_page.encode( true,
				 _updateList,
				 indexUpdateList, // Number of actual updates in the list at this point
				 _strUpdatePageBuff,
				 sizeof( _strUpdatePageBuff ),
				 &nTextLength );
	}
	else
	{
		cout << "DataFormat does not match. makePageCells() supports ANSI_Page only." << endl;
	}
}

Buffer CMDEncoderFormatter::FormatAnsiPageDataBuffer()
{
	if ( _dataFormat == MarketDataEnums::ANSI_Page )
	{
		char tibMsgBuffer[2048];
		initializeTibMsg(tibMsgBuffer, sizeof(tibMsgBuffer));
		_tibMsg.Append( (TibMsg_name)"", TIBMSG_OPAQUE, (TibMsg_size) sizeof( _strUpdatePageBuff ), (TibMsg_data) &_strUpdatePageBuff);

		assert( !_tibMsg.NeedPack() );

		return FormatTibMsgDataBuffer();
	}
	else
	{
		cout << "DataFormat does not match. FormatAnsiPageDataBuffer() supports ANSI_Page only." << endl;
		return 0;
	}
}

const Page & CMDEncoderFormatter::getAnsiPage() 
{
	return _page;
}
