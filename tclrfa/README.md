TCLRFA
======

TclRFA provides Tcl command extension for accessing RFA (Reuters/Robust Foundationa API)
C++ functions, making connection to RMDS and subscribe/publish market data to/from RMDS
using OMM data message model.

Features:

* subscription for MARKET_PRICE (level 1)
* subscription for MARKET_BY_ORDER (order book)
* subscription for MARKET_BY_PRICE (market depth)
* includes orderbook, depth ranker in utils folder
* snapshot request (no incremental updates)
* dictionary download or use local files
* directory request
* symbol list request
* timeseries request and decoder for IDN TS1
* custom domain MMT_HISTORY which can be used for intraday publishing
* non-interactive provider for MARKET_PRICE, MARKET_BY_ORDER, MARKET_BY_PRICE, SYMBOLLIST, HISTORY
* debug mode
* logging
* low-latency mode
* subscription outbound NIC binding
* example scripts

Tclrfa is written with C++ and ported as a stub extension for Tcl 8.4+

Table of contents
=================

1. [What's new](#whats-new)
2. [Performance](#performance)
3. [Support Systems](#supported-systems)
4. [Installation](#installation)
5. [Running Examples](#running-examples)
6. [Getting Started](#getting-started)
7. [Data Type](#data-type)
8. [Functions](#functions)
  1. [Initialization](#initialization)
  2. [Configuration](#configuration)
  3. [Session](#session)
  4. [Client](#client)
  5. [Directory](#directory)
  6. [Dictionary](#dictionary)
  7. [Logging](#logging)
  8. [Symbol List](#symbol-list)
  9. [Market Price](#market-price)
  10. [Market by Order](#market-by-order)
  11. [Market by Price](#market-by-price)
  12. [TS1](#ts1)
  13. [History](#history)
  14. [Getting Data](#getting-data)

What's new
----------
7.6.0.1

* 18 July 2014
* Minor bug fixes
* Updates common files

7.6.0.0

* 9 July 2014
* RFA 7.6.0.L1
* Less log output to stdout
* Remove using `update` in Tcl example scripts

7.5.1.2

* 11 February 2014
* Fixed provider can convert string into ASCII_STRING field
* Support Position parameter in the config file

7.5.1.1

* 5 November 2013
* isLoggedIn returning P2PS/ADS login status
* Fixed bug#35 TclRFA handles login status incorrectly when ServerList is unreachable

7.5.1.0

* 15 October 2013
* RFA 7.5.1.L1
* ApplicationId from config file submitted for login
* New config `responseQueueBatchInterval` for low latency inbound message setup
* New config `flushTimerInterval` for low latency outbound message setup
  
7.4.1.0

* 28 April 2013
* RFA 7.4.1.L1 for Linux and Windows
* Ability to bind OMM Consumer to a NIC in config file

7.2.1.0

* 2 Feb 2013
* Source repo relocated from Google Code to GitHub
* Using "tag" as part of the release number
* RFA 7.2.1.L1 for Linux
* RFA 7.2.1.E3 for Windows
    
7.2.0.283

* 26 March 2012
* RFA 7.2.0.L2
* Update dictionary to v.4.10
* New RDM domain support: MMT_HISTORY (unofficial) for indexed historical data
* Encoder supports all FIDs from dict now
* Support millisecond resolution in Provider
* Improve performance on large subcription with internal watchlist enhancement
* Auto submit Directory for Provider upon connection recovery
* Remove marketfeed dict from distribution
* Bugs fixed

7.0.1.245

* January 9, 2012
* Timeseries client for IDN TS1 service
* marketPrice client introduces a REFRESH tuple
* Snapshot only subscription by setInteractionType "snapshot"
* Fixed bugs

7.0.1.219

* November 18, 2011
* 64-bit Windows support
* SymbolList publication support
* Subscription request/close accepts multiple RICs now e.g. "A.B C.D"
* Bugs fixed

7.0.1.205

* October 20, 2011
* Fixed bug#46 that gives wrong update list when subscribing to marketbyprice/order and marketprice with the same tclrfa instance

7.0.1.200

* October 17, 2011
* Log status message in a log file

7.0.1.194

* October 14, 2011
* Using RFA 7.0.1.E2
* Fixed bug#32, Upgrade dictionary files to 4.00.14
* Fixed bug#43, using tclstub8.4 on Windows now so tclrfa would run in Tcl 8.4 and 8.5 on both 32-bit and 64-bit Windows
* Other bug fixes

7.0.0.186

* September 1, 2011
* Level 2 data support
* Consumer: marketByOrder, marketByPrice
* Provider: marketByOrder, marketByPrice
* Order book Tcl script (order book builder)
* Market depth Tcl script (market depth builder)
* Fixed bugs
    
7.0.0.163

* June 22, 2011
* 64bit Linux support

7.0.0.159

* May 5, 2011
* Raname package to be tclrfa*rfaversion*.*build* for better tracking
* Fixed bug#30 (cannot run with tcl8.4)
* Using tcl8.4 stub (previously 8.5 lib) now so it can run with tcl8.4+
* trap.tcl is using Tclx instead of Expect for SIGINT trapping
    
7.0.1

* Released on April 7, 2011
* Fixed crash when subcribe with undefined FIDs in dictionary
* Now "log" works properly with internal strings
* Thoughput test stats
    
7.0.0

* Compiled with RFA C++ 7.0.0.E2
* The wait is over. Introducing useInteralLogStrings, so long *.mc files
* createEventQueue and acquireLogger not exposed in Tcl anymore, they are called automatically upon invoking acquireSession
* Able to send "closed" status for published items in OMMCProv class
* Fixed a few bugs

6.5.0

* Compiled with RFA C++ 6.5.0
* OMM consumer
* OMM client publisher
* Statically linked with RFA
* Both 32-bit Linux and Windows Tcl extension

PERFORMANCE
-----------
Subscription

* 20,000 updates/sec on Core 2 Duo 2.6 GHz, RAM 4 GB on 32bit Windows 7
* 30,000 updates/sec on Core 2 Duo 2.66 GHz, RAM 2 GB on 32bit Linux 2.6.35

Publication

* N/A

SUPPORT SYSTEM
--------------
* Linux x86 32 and 64bit
* Windows x86 32 and 64bit
* Tcl8.4+

INSTALLATION
------------
* Require installing Tcl8.4+, recommend ActiveTcl from ActiveState 8.5 (http://www.activestate.com/activetcl/downloads)
* Optionally, copy lib/tclrfa7.x.x folder into your system's Tcl lib folder e.g. c:\Tcl\lib or /usr/lib/tcl
* However the examples also include "auto_path" so you don't actually need to do the above to run example scripts

RUNNING EXAMPLES
----------------
* On Windows CMD ,run with
    
        > tclsh consumer02.tcl
        
* On Linux or Windows Cygwin:
    
        $ tclsh consumer02.tcl
        $ ./consumer02.tcl

GETTING STARTED
----------------
To be familiar with TclRFA, please follow the steps below:

1. Download the latest TclRFA from www.devcartel.com/tclrfa.
2. Extract the package on your workstation.
3. Open the `/example` folder.
4. Open and edit `tclrfa.cfg` for `provider01.tcl` and `consumer01.tcl` as follows:

    ```
    #Consumer 
    \Connections\Connection_RSSL1\rsslPort = "14002"
    \Connections\Connection_RSSL1\ServerList = "P2PS/ADS IP address" 
    ... 
    #Provider
    \Connections\Connection_RSSL4\rsslPort = "14003"
    \Connections\Connection_RSSL4\ServerList = "MDH/ADH IP address"
    ```

5. Start `/example/provider.tcl` with the command below, wait for the data to be published to MDH/ADH cache:

    ```
    $ tclsh provider.tcl
    ```

6. Start `/example/consumer01.tcl` to consume data while running `provider.tcl` with

    ```
    $ tclsh consumer01.tcl
    ```

7. `consumer01.tcl` retrieves a market data full image followed by incremental updates of `EUR=`, below is the execution result:
    
    ```
    [Mon Jul 15 14:37:40 2013]: (ComponentName) Tclrfa: (Severity) Information:
    [Tclrfa::login] Login successful. (username: tclrfa)
    [DictionaryHandler::DictionaryHandler] Successfully load dictionaries from ../etc/RDM/RDMFieldDictionary, ../etc/RDM/enumtype.def 
    {NIP EUR= REFRESH} {NIP EUR= { RDNDISPLAY {100} RDN_EXCHID {SES} BID {0.988} ASK {0.999} DIVPAYDATE {23 JUN 2011} }} 
    {NIP EUR= { BID_NET_CH {0.0041} BID {0.988} ASK {0.999} ASK_TIME {14:37:40:921} }}
    ```
    
FUNCTIONS
---------

### Initialization

__[tclrfa]__  
Instantiate a TclRFA object

    % set t [tclrfa]
    
__setDebugMode__ _True|False_  
Enable or disable debug messages.

    % $t setDebugMode True
    
### Configuration

__createConfigDb__ _filename_  
_filename = configuration file location, full or relative path_

Read the configuration file. Initialize RFA context.

    % $t createConfigDb "./tclrfa.cfg"
    [Tclrfa::initializeRFA] Initializing RFA context...
    [Tclrfa::createConfigDb] Loading RFA config from file: ./tclrfa.cfg

__printConfigDb__ _config_node_  
_config_node = part of the configuration node to be printed out_

Print current configuration values. If the arg is omitted, this function returns all of the configuration values under the ‘Default’ namespace.

    % $t printConfigDb "\\Default\\Sessions"
    \Default\Sessions\Session1\connectionList = Connection_RSSL1
    \Default\Sessions\Session2\connectionList = Connection_RSSL2
    \Default\Sessions\Session3\connectionList = Connection_RSSL3
    \Default\Sessions\Session4\connectionList = Connection_RSSL4

### Session

__acquireSession__ _session_  
_session = session name_

Acquire a session as defined in the configuration file and look up for an appropriate connection type then establish a client/server network session.

    % $t acquireSession "Session1"

### Client

__createOMMConsumer__  
Create an OMM data consumer client.

    % $t createOMMConsumer

__createOMMProvider__  
Create an OMM data provider client.

    % $t createOMMProvider

__login__ _?username instanceid appid position?_  
Where:

* _username = DACS user ID_
* _instanceid = unique application instance ID_
* _appid = application ID (1-256)_
* _position = application position e.g. IP address_

Send an authentication message with user information. This step is mandatory in order to consume the market data from P2PS/ADS. If arguments are omitted, TclRFA will read values from configuration file.

    % $t login 
    ... 
    [Thu Jul 04 17:56:48 2013]: (ComponentName) Tclrfa: (Severity) Information: 
    [Tclrfa::login] Login successful. (username: TclRFA)

__isLoggedIn__  
Check whether the client successfully receives a login status from the P2PS/ADS.

    % $t isLoggedIn
    1

__setInteractionType__ _type_  
_type = “snapshot” or “streaming”_

Set subscription type `snapshot` or `streaming`. If `snapshot` is specified, the client will receive only the full image of an instrument then the subscribed stream will be closed.

    % $t setInteractionType “snapshot”

### Directory

__directoryRequest__  
Send a directory request through the acquired session. This step is the mandatory in order to consume the market data from P2PS/ADS.

    % $t directoryRequest

__submitDirectory__ _domain_  
_domain = data domain number_

Submit directory with domain type (capability) in a provider application, it currently supports:

* 6 - market price
* 7 - market by order
* 8 - market by price
* 10 - symbol list
* 12 - history

This function is normally called automatically upon data submission.

    % $t submitDirectory 6

### Dictionary

__dictionaryRequest__  
If `downloadDataDict` configuration is set to True then TclRFA will send a request for data dictionaries to P2PS/ADS. Otherwise, it uses local data dictionaries specified by `fieldDictionaryFilename` and `enumTypeFilename` from configuration file.

    % $t dictionaryRequest

__isNetworkDictionaryAvailable__  
Check whether the data dictionary is successfully downloaded from the server.

    % $t isNetworkDictionaryAvailable 
    1

### Logging

__log__ _message_  
Write a normal message to a log file.

    % $t log “Print log message out” 
    [Thu Jul 04 17:45:29 2013]: (ComponentName) Tclrfa: (Severity) Information: Print log message out

__logWarning__ _message_  
Write a warning message to a log file.

    % $t logWarning “Print warning message out” 
    [Thu Jul 04 17:47:03 2013]: (ComponentName) Tclrfa: (Severity) Warning: Print warning message out

__logError__ _message_  
Write an error message to a log file.

    % $t logError “Print error message out” 
    [Thu Jul 04 17:48:00 2013]: (ComponentName) Tclrfa: (Severity) Error: Unexpected error: Print error message out

### Symbol List

__symbolListRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

For consumer application to subscribe symbol lists. User can define multiple symbol list names. Data dispatched using dispatchEventQueue function.

Image

    { SERVICE_NAME SYMBOLLIST_NAME REFRESH }
    { SERVICE_NAME SYMBOLLIST_NAME COMMAND { ITEM_NAME { FID_NAME#1 {VALUE#1} ... FID_NAME#X {VALUE#X}} }

Update

    { SERVICE_NAME SYMBOLLIST_NAME COMMAND { ITEM_NAME { FID_NAME#1 {VALUE#1} ... FID_NAME#X {VALUE#X}} }

Example

    % $t symbolListRequest "0#BMD" 
    ... 
    % $t dispatchEventQueue 
    [SymbolListHandler::processResponse] SymbolList Refresh: 0#BMD.NIP {NIP 0#BMD REFRESH} {NIP 0#BMD ADD {FCPO { PROD_PERM {10} PROV_SYMB {MY439483}}}}

__symbolListCloseRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

Unsubscribe the specified symbol lists. User can define multiple symbol list names

    % $t symbolListCloseRequest “0#BMD” “0#ARCA”

__symbolListCloseAllRequest__  
Unsubscribe all symbol lists in the watch list.

    % $t symbolListCloseAllRequest

__isSymbolListRefreshComplete__  
Check whether the client receives a complete list of the symbol list.

    % $t isSymbolListRefreshComplete 
    1

__getSymbolList__ _RIC_  
_RIC = Reuters Instrument Code_

Return item names available under a symbol list in string format.

    % $t getSymbolList “0#BMD” 
    FPCO FPKC FPRD FPGO

__getSymbolListWatchList__  
Return names of the subscribed symbol lists.

    % $t getSymbolListWatchList 
    0#BMCA 0#ARCA

__symbolListSubmit__ _command data_  
Where:

* _command = instruction to manipulate a list_
* _data = Tcl dict_

For a provider client to publish a list of symbols to MDH/ADH under data domain 10, available commands are:

* ADD - add a new symbol to the list
* UPDATE - update a symbol’s attributes
* DELETE - delete a symbol from the list

The second one is a Tcl dict in following format:

    SYMBOLLIST_NAME ITEM#1_NAME {FID_NAME#1 {VALUE#1} FID_NAME#2 {VALUE#2>} ... FID_NAME#X {VALUE#X}}

Example

    dict set SYMBOLLIST 0#BMD FCPO { PROD_PERM {10} PROV_SYMB {MY439483} }
    % $t symbolListSubmit add $SYMBOLLIST 
    [Tclrfa::symbolListSubmit] mapAction: add 
    [Tclrfa::symbolListSubmit] symbolName: 0#BMD 
    [Tclrfa::symbolListSubmit] mapKey: FCPO 
    [Tclrfa::symbolListSubmit] fieldList: [Tclrfa::symbolListSubmit] fieldList: PROV_SYMB=MY439483,PROD_PERM=10

### Market Price

__marketPriceRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

For consumer client to subscribe market data from P2PS/ADS, user can define multiple item names. The data dispatched through dispatchEventQueue function in Tcl dict format:

Image

    {SERVICE_NAME ITEM_NAME REFRESH} {SERVICE_NAME ITEM_NAME {FID_NAME#1 {VALUE#1} FID_NAME#2 {VALUE#2>} ... FID_NAME#3 {VALUE#X}}}

Update

    {SERVICE_NAME ITEM_NAME {FID_NAME#1 {VALUE#1} FID_NAME#2 {VALUE#2>} ... FID_NAME#3 {VALUE#X}}}

Example 

    % $t marketPriceRequest “EUR=” 
    % $t dispatchEventQueue 10 
    {NIP EUR= REFRESH} {NIP EUR= { RDNDISPLAY {100} RDN_EXCHID {SES} BID {0.988} ASK {0.999} DIVPAYDATE {23 JUN 2011} }}

__marketPriceCloseRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

Unsubscribe items from streaming service. User can define multiple item names.

    % $t marketPriceCloseRequest “C.N” “JPY=”

__marketPriceCloseAllRequest__ 
Unsubscribe all items from streaming service.

    % $t marketPriceCloseAllRequest

__getMarketPriceWatchList__  
Returns names of the subscribed items.

    % $t getMarketPriceWatchList 
    C.N JPY=

__marketPriceSubmit__ _data_  
_data = Tcl dict_

For provider client to publish market data to MDH/ADH, the market data image/update must be in the following Tcl dict format:

    ITEM_NAME {FID_NAME#1 {VALUE#1} ... FID_NAME#X {VALUE#X}}

Example

    % dict set IMAGES EUR= { RDNDISPLAY {100} RDN_EXCHID {155} BID {0.988} ASK {0.999} DIVPAYDATE {20110623} }    
    % dict set IMAGES C.N { RDNDISPLAY {100} RDN_EXCHID {NAS} OFFCL_CODE {isin1234XYZ} BID {4.23} DIVPAYDATE {20110623} OPEN_TIME {09:00:01.000} }
    % $t marketPriceSubmit $IMAGES
    [Tclrfa::marketPriceSubmit] EUR= { RDNDISPLAY {100} RDN_EXCHID {155} BID {0.988} ASK {0.999} DIVPAYDATE {20110623} } C.N { RDNDISPLAY {100} RDN_EXCHID {NAS} OFFCL_CODE {isin1234XYZ} BID {4.23} DIVPAYDATE {20110623} OPEN_TIME {09:00:01.000} }
    [Tclrfa::marketPriceSubmit] symbolName: EUR=
    [Tclrfa::marketPriceSubmit] fieldList: RDNDISPLAY=100,RDN_EXCHID=155,BID=0.988,ASK=0.999,DIVPAYDATE=20110623,
    [Tclrfa::marketPriceSubmit] symbolName: C.N
    [Tclrfa::marketPriceSubmit] fieldList: RDNDISPLAY=100,RDN_EXCHID=NAS,OFFCL_CODE=isin1234XYZ,BID=4.23,DIVPAYDATE=20110623,OPEN_TIME=09:00:01.000,
    [Tclrfa::marketPriceSubmit] EUR= {BID_NET_CH 0.0041 BID 0.988 ASK 0.999 ASK_TIME now} C.N {ACVOL_1 1001 TRDPRC_1 4.561 TIMACT now}

### Market by Order

__marketByOrderRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

For a consumer application to subscribe order book data, user can define multiple item names. The data dispatched through dispatchEventQueue in Tcl dict format:

Image

    {SERVICE_NAME ITEM_NAME REFRESH} {SERVICE_NAME ITEM_NAME COMMAND { ORDER_ID { FID_NAME#1 {VALUE#1} ... FID_NAME#X {VALUE#X}}}

Update

    {SERVICE_NAME ITEM_NAME COMMAND { ORDER_ID { FID_NAME#1 {VALUE#1} ... FID_NAME#X {VALUE#X}}}

Example

    % $t marketByOrderRequest "ANZ.AX" 
    % $t dispatchEventQueue 10000     
    {NIP ANZ.AX REFRESH} {NIP ANZ.AX ADD {538993C200035057B { ORDER_PRC {20.25} ORDER_SIZE {369} ORDER_SIDE {BID} SEQNUM_QT {804} EX_ORD_TYP {0} CHG_REAS {10} ORDER_TONE {} }}} {NIP ANZ.AX ADD {538993C200083483B { ORDER_PRC {20.280} ORDER_SIZE {100} ORDER_SIDE {BID} SEQNUM_QT {2744} EX_ORD_TYP {0} CHG_REAS {6} ORDER_TONE {} }}}

__marketByOrderCloseRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

Unsubscribe items from order book streaming service. User can define multiple item names.

    % $t marketByOrderCloseRequest "ANZ.AX"

__marketByOrderCloseAllRequest__  
Unsubscribe all items from order book data streaming service.

    % $t marketByOrderCloseAllRequest

__getMarketByOrderWatchList__  
Return all subscribed item names on order book streaming service.

    % $t getMarketByOrderWatchList
    ANZ.AX

__marketByOrderSubmit__ _command data_  
Where:

* _command = instruction to manipulate an order book_
* _data = Tcl dict_

For a provider client to publish specified order book data to MDH/ADH, `marketByOrderSubmit` requires two parameters, the first one is the order book placement command:

* ADD - add new order to the item
* UPDATE - update an order’s attributes
* DELETE - remove an order from the book

The second one is a Tcl dict and must be in the following format:

    ITEM_NAME ORDER_ID {FID_NAME#1 {VALUE#1} FID_NAME#2 {VALUE#2>} ... FID_NAME#X {VALUE#X}}

Example

    % dict set ORDERS ANZ.AX 538993C200035057B { ORDER_PRC {20.260} ORDER_SIZE {50}
    ORDER_SIDE {BID} SEQNUM_QT {2744} EX_ORD_TYP {0} CHG_REAS {6} ORDER_TONE {} }
    
    % $t marketByOrderSubmit('ADD', ORDERS) 
    
    [Tclrfa::dispatchEventQueue] Event loop - approximate pending Events: 0
    [Tclrfa::marketByOrderSubmit] mapAction: add 
    [Tclrfa::marketByOrderSubmit] symbol name: ANZ.AX 
    [Tclrfa::marketByOrderSubmit] mapKey: 538993C200035057B 
    [Tclrfa::marketByOrderSubmit] fieldList: ORDER_PRC=20.260,ORDER_SIZE=50,ORDER_SIDE=BID,SEQNUM_QT=2744,EX_ORD_TYP=0,CHG_REAS=6,ORDER_TONE=, 
    [OMMCProvServer::submitData] sending refresh item: ANZ.AX 
    [OMMCProvServer::submitData] sending refresh service: NIP

### Market by Price

__marketByPriceRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

For consumer application to subscribe market depth data, user can define multiple item names. Data dispatched through dispatchEventQueue module in a tuple below:

Image

    { SERVICE_NAME ITEM_NAME REFRESH } { SERVICE_NAME ITEM_NAME COMMAND { DEPTH { FID_NAME#1 {VALUE#1} ... FID_NAME#X {VALUE#X}}}

Update

    { SERVICE_NAME ITEM_NAME COMMAND { DEPTH { FID_NAME#1 {VALUE#1} ... FID_NAME#X {VALUE#X}}}

Example

    % $t marketByOrderRequest "ANZ.CHA"     
    % $t dispatchEventQueue 1000 
    {NIP ANZ.CHA REFRESH} {NIP ANZ.CHA ADD {210000B { ORDER_PRC {21.0000} ORDER_SIDE {ASK} ORDER_SIZE {500} NO_ORD {13} QUOTIM_MS {16987567} ORDER_TONE {} }}} {NIP ANZ.CHA ADD {201000B { ORDER_PRC {20.1000} ORDER_SIDE {BID} ORDER_SIZE {97} NO_ORD {2} QUOTIM_MS {16987567} ORDER_TONE {} }}}

__marketByPriceCloseRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

Unsubscribe items from market depth data stream. User can define multiple item names.

    % $t marketByOrderCloseRequest "ANZ.AX"

__marketByPriceCloseAllRequest__  
Unsubscribe all items from market depth stream.

    % $t marketByPriceCloseAllRequest

__getMarketByPriceWatchList__  
Return all subscribed item names on market depth streaming service.

    % t getMarketByPriceWatchList 
    ANZ.CHA

__marketByPriceSubmit__ _command data_  
Where:

* _command = instruction to manipulate an order book_
* _data = Tcl dict_

For a provider client to publish the specified market depth data to MDH/ADH, `marketByPriceSubmit` requires two parameters, the first one is the depth placement command:

* ADD – add a new price to the depth
* UPDATE - update a price level’s attributes
* DELETE - remove a price from the depth

The second one is a Tcl dice and must be in the following format:

    ITEM_NAME DEPTH {FID_NAME#1 {VALUE#1} FID_NAME#2 {VALUE#2>} ... FID_NAME#X {VALUE#X}}

Example

    % dict set DEPTHS ANZ.CHA 201000B { ORDER_PRC {20.1000} ORDER_SIDE {BID} ORDER_SIZE {1300} NO_ORD {13} QUOTIM_MS {16987567} ORDER_TONE {} } 
    % $t marketByPriceSubmit add $DEPTHS 
    [Tclrfa::dispatchEventQueue] Event loop - approximate pending Events: 0
    [Tclrfa::marketByPriceSubmit] mapAction: add 
    [Tclrfa::marketByPriceSubmit] symbol name: ANZ.CHA 
    [Tclrfa::marketByPriceSubmit] mapKey: 201000B 
    [Tclrfa::marketByPriceSubmit] fieldList: ORDER_PRC=20.1000,ORDER_SIDE=BID,ORDER_SIZE=1300,NO_ORD=13,QUOTIM_MS=16987567,ORDER_TONE=, 
    [OMMCProvServer::submitData] sending refresh item: ANZ.CHA
    [OMMCProvServer::submitData] sending refresh service: NIP

### TS1

__setTimeSeriesPeriod__ _daily|weekly|monthly_  
Define a time period for a time series subscription. Default is daily.

__setTimeSeriesMaxRecords__ _max_records_  
_max_records = number of maximum time series output records_

Define the maximum record before calling `getTimeSeries.` Default is 10.

__getTimeSeries__ _RIC_  
_RIC = Reuters Instrument Code_

A helper function that subscribes, wait for data dissemination to be complete, unsubscribe from the service and return series as a list of records. getTimeSeries supports one time series retrieval at a time.

    % set ric "CHK.N" 
    % set period "daily" 
    % set maxrecords "15" 
    % $t setTimeSeriesPeriod $period 
    % $t setTimeSeriesMaxRecords $maxrecords 
    % set timeseries [$t getTimeSeries $ric] 
    % puts "\n\n############## $ric $period ([llength $timeseries] records)##############" 
    % puts [join $timeseries "\n"]\n\n 
    
    ############## CHK.N daily (21 records) ##############
    DATE,CLOSE,OPEN,HIGH,LOW,VOLUME,VWAP
    2012/02/26,25.110,25.360,25.440,25.010,2457997.000,25.218
    2012/02/23,25.450,24.830,25.830,24.820,3420367.000,25.451
    2012/02/22,24.980,24.130,24.990,23.960,3786628.000,24.582
    2012/02/21,24.030,24.220,24.340,23.640,4144242.000,23.998
    2012/02/20,24.620,25.000,25.060,24.540,3713133.000,24.767 
    2012/02/19,Market holiday 
    2012/02/16,24.710,24.220,24.980,24.130,5116842.000,24.508
    2012/02/15,23.770,23.070,23.790,22.730,3540557.000,23.489
    2012/02/14,23.020,22.740,23.270,22.520,3885523.000,22.923
    2012/02/13,22.710,22.800,22.950,22.450,3114849.000,22.684
    2012/02/12,22.660,22.750,23.320,22.260,5041853.000,22.746
    2012/02/09,22.130,21.900,22.190,21.560,3179328.000,21.970
    2012/02/08,22.340,22.320,22.640,22.030,3298569.000,22.355
    2012/02/07,22.110,22.310,22.510,22.030,2662700.000,22.187
    2012/02/06,22.180,22.680,22.680,22.020,2829459.000,22.175

### History

__historyRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

Request for historical data (data domain 12), this domain is not officially supported by Thomson Reuters. User can define multiple items. The data dispatched through `dispatchEventQueue` in Tcl dict format as below:

Image

    {SERVICE_NAME ITEM_NAME REFRESH } {SERVICE_NAME ITEM_NAME {FID_NAME#1 {VALUE#1} FID_NAME#2 {VALUE#2} ... FID_NAME#3 {VALUE#X}}}

Update

    {SERVICE_NAME ITEM_NAME {FID_NAME#1 {VALUE#1} FID_NAME#2 {VALUE#2} ... FID_NAME#3 {VALUE#X}}}

Example

    % $t historyRequest "tANZ.AX" 
    % $t dispatchEventQueue 
    [HistoryHandler::processResponse] History Refresh: tANZ.AX.NIP 
    {NIP tANZ.AX REFRESH} {NIP tANZ.AX { TRDPRC_1 {40.124} SALTIM {18:34:30:216} TRADE_ID {123456789} BID_ORD_ID {5307FBL20AL7B} ASK_ORD_ID {5307FBL20BN8A} }} {NIP tANZ.AX { TRDPRC_1 {40.124} SALTIM {18:34:30:216} TRADE_ID {123456789} BID_ORD_ID {5307FBL20AL7B} ASK_ORD_ID {5307FBL20BN8A} }} {NIP tANZ.AX { TRDPRC_1 {40.124} SALTIM {18:34:31:217} TRADE_ID {123456789} BID_ORD_ID {5307FBL20AL7B} ASK_ORD_ID {5307FBL20BN8A} }}} 

__historyCloseRequest__ _RIC ?RIC RIC ...?_  
_RIC = Reuters Instrument Code_

Unsubscribe items from historical data stream. The user can define multiple item names.

    % $t historyCloseRequest “tANZ.AX”

__historyCloseAllRequest__  
Unsubscribe all items from historical data stream.

    % $t historyCloseAllRequest

__historySubmit__ _data_  
_data = Tcl dict_

For a provider client to publish the specified history data to MDH/ADH, each history data image/update must be in the following format:

    ITEM_NAME {FID_NAME#1 {VALUE#1} ... {FID_NAME#X} VALUE#X}}

Example

    % dict set UPDATES tANZ.AX { TRDPRC_1 {40.124} SALTIM {now} TRADE_ID {123456789}
    BID_ORD_ID {5307FBL20AL7B} ASK_ORD_ID {5307FBL20BN8A} } 
    % $t historySubmit $UPDATES 
    [Tclrfa::historySubmit] symbolName: tANZ.AX 
    [Tclrfa::historySubmit] fieldList: TRDPRC_1=40.124,BID_ORD_ID=5307FBL20AL7B,ASK_ORD_ID=5307FBL20BN8A,TRADE_ID=123456789,SATIM=now 
    [OMMCProvServer::submitData] sending update item: tANZ.AX
    [OMMCProvServer::submitData] sending update service: NIP

### Getting Data

__dispatchEventQueue__ _?timeout?_  
_timeout = time in milliseconds to wait for data_

Dispatch the events (data) from EventQueue within a period of time (If timeout is omitted, it will return immediately). If there are many events in the queue at any given time, a single call gets all the data until the queue is empty. Data is in a list of Tcl dicts.

    % $t dispatchEventQueue 
    {NIP EUR= REFRESH} {NIP EUR= { RDNDISPLAY {100} RDN_EXCHID {SES} BID {0.988} ASK {0.999} DIVPAYDATE {23 JUN 2011} }}

