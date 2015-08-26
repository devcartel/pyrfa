PyRFA 8.0
=========

PyRFA is a Python API for accessing Thomson Reuters market data feeds know as RMDS or
Thomson Reuter Enterprise Platform for Real-time (TREP-RT). It supports subscription
and publication of market data using OMM data message model.

__Features__

* Subscription for `MARKET_PRICE` (level 1)
* Subscription for `MARKET_BY_ORDER` (order book)
* Subscription for `MARKET_BY_PRICE` (market depth)
* Snapshot request
* Multiple service subscription
* Pause and resume subscription
* OMM Posting
* Dictionary download or use local files
* Directory request
* Symbol list request
* Timeseries request and decoder for IDN TS1
* Custom domain `MMT_HISTORY` which can be used for intraday timeseries publishing
* Non-interactive provider for `MARKET_PRICE`, `MARKET_BY_ORDER`, `MARKET_BY_PRICE`, `SYMBOLLIST`, `HISTORY`
* Debug mode
* Logging
* Low-latency mode
* Subscription outbound NIC binding
* Example scripts

Table of contents
=================

1. [Changelog](#changelog)
2. [Performance](#performance)
3. [Supported Systems](#supported-systems)
4. [Installation](#installation)
5. [Example](#example)
6. [Data Type](#data-type)
7. [Functions](#functions)
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
  12. [OMM Posting](#omm-posting)
  13. [Pause and Resume](#pause-and-resume)
  14. [TS1](#ts1)
  15. [History](#history)
  16. [Getting Data](#getting-data)
8. [License](#license)

Changelog
=========
8.0.0.3

* 25 August 2015
* Prevent memory leak caused by login handler
* Fixed another potential memory leak

8.0.0.2

* 17 August 2015
* Fixed timeseries floating point data limitation
* Fixed memory leak in data dictionary handler

8.0.0.1

* 6 August 2015
* Supports Pause and Resume
* Supports OMM Posting for market price
* Provider can submit data as unsolicited REFRESH using MTYPE = IMAGE
* Provider can submit data to different service using SERVICE key in dict 
* Supports sending service up/down status

8.0.0.0

* 22 June 2015
* Compiled with RFA 8.0.0.L1
* New output message in pure dictionary format
* Supports STATUS output message type
* New message types include REFRESH, IMAGE, UPDATE, STATUS
* RIC and SERVICE now treated as part of data output
* Translate Reuters price tick symbol to unicode
* Fixed for service group failover by RFA 8.0
* Support for Python 3.4
* Available in 64-bit only

7.6.1.3

* 24 August 2015
* New output message in pure dictionary format
* Supports STATUS output message type
* New message types include REFRESH, IMAGE, UPDATE, STATUS
* RIC and SERVICE now treated as part of data output
* Translate Reuters price tick symbol to unicode
* Supports Pause and Resume
* Supports OMM Posting for market price
* Provider can submit data as unsolicited REFRESH using MTYPE = IMAGE
* Provider can submit data to different service using SERVICE key in dict 
* Supports sending service up/down status
* Fixed timeseries floating point data limitation
* Fixed memory leak in data dictionary handler
* Prevent memory leak caused by login handler
* Fixed another potential memory leak

7.6.1.2

* 22 April 2015
* Released for Windows and Linux
* 27 January 2015
* Support sending service down status programmatically

7.6.1.1

* 17 December 2014
* Fixed OMM provider not submit directory data after reconnecting to ADH/MDH


7.6.1.0

* 8 December 2014
* Compiled with RFA 7.6.1.E1
* Fixed publisher crash when extracting floating point value
* Fixed crashes when publishing time field with no millisecond
* Fixed MarketByOrder not return data
* Backward support for RHEL 5 with Python 2.4
* Other minor fixes


7.6.0.3

* 3 Sep 2014
* Added logger eventqueue to process log events
* Fixed log() functions purge data in buffer resulted missing updates
* Will be free software under MIT license
* Support for RHEL 6.4 with Python 2.6

7.6.0.2

* 1 Aug 2014
* Added setup.py: PyRFA package installer
* Updated service group in config example
* setServiceName() to select a service programmatically
* Fixed core dump upon exit if call getWatchList in script

7.6.0.1

* 18 July 2014
* Minor bug fixes
* Updates common files

7.6.0.0

* 9 July 2014
* RFA 7.6.0.L1
* Less log output to stdout
* Use try/except in Python example scripts

7.5.1.2

* 11 February 2014
* Fixed provider can convert string into ASCII_STRING field
* Fixed detect and put empty decoded string for field value
* Support Position parameter in the config file

7.5.1.1

* 5 November 2013
* Supported timeseries request and decoder for IDN TS1
* isLoggedIn returning P2PS/ADS login status
* Fixed bug#28 PyRFA handles login status incorrectly when ServerList is unreachable

7.5.1.0

* 15 October 2013
* RFA 7.5.1.L1
* ApplicationId from config file submitted for login
* New config `responseQueueBatchInterval` for low latency inbound message setup
* New config `flushTimerInterval` for low latency outbound message setup

7.4.1.2

* 16 August 2013
* Fixed library dependency problem on Linux version

7.4.1.1

* 15 August 2013
* Linux version support both python2.6 and python2.7
* Release for Windows x64

7.4.1.0

* 28 April 2013
* RFA 7.4.1.L1 for Linux and Windows
* Ability to bind OMM Consumer to a NIC in config file

7.2.1.beta

* 7 April 2013
* Using "tag" as part of the release number
* Examples scripts
* Utils scripts e.g. `every()`
* Support 32bit Windows
* Support 64bit Linux
* RFA 7.2.1.L1 for Linux
* RFA 7.2.1.E3 for Windows

Performance
===========

Subscription

* N/A

Publication

* N/A

Supported systems
=================

* Linux x86 64bit
* Windows x86 64bit
* Python 2.6, 2.7, 3.4

Installation
============
Run below commands in terminal or command prompt:

```
> unzip pyrfa<version>-<platform>.zip
> cd pyrfa<version>-<platform>/
> python setup.py install 
```
    
Example
=======

```python
import pyrfa
import sys
p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.acquireSession("Session1")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()
p.marketPriceRequest("JPY=,EUR=")
end = False
while not end:
    try:
        updates = p.dispatchEventQueue(100)
    except KeyboardInterrupt:
        end = True
    if updates:
        print("")
        for u in updates:
            print(u['SERVICE'] + " - " + u['RIC'])
            for k,v in u.items():
                sys.stdout.write(k.rjust(15) + " ")
                print(round(v,10) if type(v) is float else v)
            print("")
p.marketPriceCloseAllRequest()
```

Data Type
=========

OMM DATA TYPE | PYTHON
--------------|--------------
ENUM          | STRING
FLOAT         | DOUBLE
DOUBLE        | DOUBLE
REAL32        | DOUBLE
REAL64        | DOUBLE
INT32         | INTEGER
UINT32        | INTEGER
INT64         | LONG
UINT64        | LONG

Configuration File
==================
### Example

    \pyrfa\debug = false

    \Logger\AppLogger\useInternalLogStrings  = true
    \Logger\AppLogger\windowsLoggerEnabled   = false
    \Logger\AppLogger\fileLoggerEnabled      = true
    \Logger\AppLogger\fileLoggerFilename     = "./pyrfa.log"

    \Connections\Connection_RSSL1\rsslPort = "14002"
    \Connections\Connection_RSSL1\ServerList = "127.0.0.1"
    \Connections\Connection_RSSL1\connectionType = "RSSL"
    \Connections\Connection_RSSL1\logEnabled = true
    \Connections\Connection_RSSL1\UserName = "USERNAME"
    \Connections\Connection_RSSL1\InstanceId = "1"
    \Connections\Connection_RSSL1\ApplicationId = "180"
    \Connections\Connection_RSSL1\Position = "127.0.0.1"
    \Connections\Connection_RSSL1\ServiceName = "SERVICE"
    \Connections\Connection_RSSL1\fieldDictionaryFilename = "../etc/RDM/RDMFieldDictionary"
    \Connections\Connection_RSSL1\enumTypeFilename  = "../etc/RDM/enumtype.def"
    \Connections\Connection_RSSL1\downloadDataDict = false

    \Sessions\Session1\connectionList = "Connection_RSSL1"

    \ServiceGroups\SG1\serviceList = "SERVICE1, SERVICE2"
    \Sessions\Session1\serviceGroupList = "SG1"

### Debug
Namespace: `\pyrfa\`

| Parameter        | Example value    | Description                                            |
|------------------|------------------|--------------------------------------------------------|
| `debug`          | `true`/`false`   | Enable/Disable debug mode                              |

### Logging  
Namespace: `\Logger\AppLogger\`
   
| Parameter            | Example value    | Description                                        |
|----------------------|------------------|----------------------------------------------------|
| `fileLoggerEnabled`  | `true`/`false`   | Enable/Disable logging capability                  |
| `fileLoggerFilename` | `"./pyrfa.log"`  | Log file name                                      |


### Connection
Namespace: `\Connections\<connection_name>\`

| Parameter        | Example value    | Description                                            |
|------------------|------------------|--------------------------------------------------------|
| `rsslPort`       | `"14002"`        | P2PS/ADS RSSL port number                              |
| `ServerList`     | `"127.0.0.1"`    | P2PS/ADS IP address or hostnam                         |
| `connectionType` | `"RSSL"`         | The specific type of the connection. Must be `RSSL`    |
| `logEnabled`     | `true`/`false`   | Enable/Disable logging capability                      |
| `UserName`       | `"pyrfa"`        | DACS username                                          |
| `InstanceId`     | `"123"`          | Application instance ID                                |
| `ApplicationId`  | `"180"`          | Application ID                                         |
| `Position`       | `"127.0.0.1/net"`| DACS position                                          |
| `ServiceName`    | `"NIP"`          | Service name to be subscribe                           |
| `VendorName`     | `"OMMCProv_DevCartel"` | Name of the vendor that provides the data for this service |
| `symbolList`     | `"0#BMD"`        | Symbollist name to be subscribe(d)                     |
| `maxSymbols`     | `30`             | Configure maximum number of symbol inside symbollist   |
| `fieldDictionaryFilename` | `"../etc/RDM/RDMFieldDictionary"`| Dictionary file path          |
| `enumTypeFilename` | `"../etc/RDM/enumtype.def"` | enumtype file path                        |
| `downloadDataDict` | `true`/`false` | Enable/Disable data dictionary download from P2PS/ADS  |
| `dumpDataDict`   | `true`/`false`   | Enable/Disable to dump data dictionary from P2PS/ADS   |

### Session
Namespace: `\Sessions\<session_name>\`

| Parameter          | Example value         | Description                                            |
|--------------------|-----------------------|--------------------------------------------------------|
| `connectionList`   | `"Connection_RSSL1"`  | Match `connection_name`                                |
| `serviceGroupList` | `"SG1"`               | Service group name                                     |

`session_name` must be passed to `acquireSession()` function.

### Service Group
Namespace: `\ServiceGroups\<service_group_name>\`

| Parameter          | Example value         | Description                                            |
|--------------------|-----------------------|--------------------------------------------------------|
| `serviceList`      | `"SERVICE1, SERVICE2"`| Available service names for the group                  |

Functions
=========

### Initialization

__Pyrfa.pyrfa()__    
Instantiate a PyRFA object.

    >>> p = Pyrfa.pyrfa()

__Pyrfa.setDebugMode([_Boolean_])__    
Enable or disable debug messages. If argument is empty, it will read a value from configuration file.

    >>> p.setDebugMode(True)

### Configuration

__Pyrfa.createConfigDb(_String_)__    
Read the configuration file where **_String_** can be full path or relative path. Initialize RFA context.

    >>> p.createConfigDb("./pyrfa.cfg")
    [Pyrfa::initializeRFA] Initializing RFA context and Python libraries...
    [Pyrfa::createConfigDb] Loading RFA config from file: ./pyrfa.cfg

__Pyrfa.printConfigDb([_String_])__    
Print a configuration node. If the input parameter **_String_** is omitted, this function returns all of the configuration values under the 'Default' namespace

    >>> p.printConfigDb("\\Default\\Sessions")
    \Default\Sessions\Session1\connectionList = Connection_RSSL1
    \Default\Sessions\Session2\connectionList = Connection_RSSL2
    \Default\Sessions\Session3\connectionList = Connection_RSSL3
    \Default\Sessions\Session4\connectionList = Connection_RSSL4

### Session

__Pyrfa.acquireSession(_String_)__    
Acquire a session as defined in the configuration file where **_String_** is a session name. Look up for an appropriate connection and create a client-server network session. For example, Session1 is defined as follows:

    \Sessions\Session1\connectionList = "Connection_RSSL1"

This module will create a session using `Connection_RSSL1`.

    >>> p.acquireSession("Session4")

### Client

__Pyrfa.createOMMConsumer()__    
Create an OMM consumer client.

    >>> p.createOMMConsumer()

__Pyrfa.createOMMProvider()__    
Create an OMM provider client.

    >>> p.createOMMProvider()

__Pyrfa.login([_username=String_], [_instanceId=String_] , [_applicationId=String_], [_position=String_])__    
Send a login message through the acquired session. This step is mandatory in order to consume the market data from P2PS/ADS. If any argument is omitted, PyRFA will look it up from configuration file.

    >>> p.login()
    [Thu Jul 04 17:56:47 2013]: (ComponentName) Static: (Severity) Information:
    The RSSL_Cons_Adapter initialization succeeded
    ...
    [Thu Jul 04 17:56:48 2013]: (ComponentName) Pyrfa: (Severity) Information:
    [Pyrfa::login] Login successful. (username: pyrfa)

__Pyrfa.isLoggedIn()__    
Check whether the client successfully receives a login status from the P2PS/ADS.

    >>> p.isLoggedIn()
    True

__Pyrfa.setInteractionType(_String_)__  
Set subscription type to either `snapshot` or `streaming`. If `snapshot` is specified, the client will receive only a full image of an instrument then the subscribed stream will be closed.

    >>> p.setInteractionType("snapshot")

__Pyrfa.setServiceName(_String_)__  
Programmatically set service name before making a subcription. Call this before making any request. This allows subcription to multiple services.

    >>> p.setServiceName('IDN')
    >>> p.marketPriceRequest('EUR=')

### Directory

__Pyrfa.directoryRequest()__  
Send a directory request through the acquired session. This step is the mandatory in order to consume the market data from P2PS/ADS.

    >>> p.directoryRequest()

__Pyrfa.directorySubmit([_domain=String_],[_service=String_])__  
Submit directory with domain type (capability) in a provider application and service name, domain type currently supports:

* '6' - market price
* '7' - market by order
* '8' - market by price
* '10' - symbol list
* '12' - history

This function is called automatically upon data submission. If **_service_** is omitted, it will use values from configuration file.

    >>> p.directorySubmit('6','IDN')
    
__Pyrfa.serviceDownSubmit(_String_)__  
Submit the specified down service status to ADH.

    >>> p.serviceDownSubmit('IDN')

### Dictionary

__Pyrfa.dictionaryRequest()__  
If downloadDataDict configuration is set to True then PyRFA will send a request for data dictionaries to P2PS/ADS. Otherwise, it uses local data dictionaries specified by `fieldDictionaryFilename` and `enumTypeFilename` from configuration file.

    >>> p.dictionaryRequest()

__Pyrfa.isNetworkDictionaryAvailable()__  
Check whether the data dictionary is successfully downloaded from the server. Returns boolean.

    >>> p.isNetworkDictionaryAvailable()
    True

### Logging

__Pyrfa.log(_String_)__  
Write a message to a log file.

    >>> p.log('Print log message out')
    [Thu Jul 04 17:45:29 2013]: (ComponentName) Pyrfa: (Severity) Information: Print log message out

__Pyrfa.logWarning(_String_)__  
Write a warning message to a log file.

    >>> p.logWarning('Print warning message out')
    [Thu Jul 04 17:47:03 2013]: (ComponentName) Pyrfa: (Severity) Warning: Print warning message out

__Pyrfa.logError(_String_)__  
Write an error message to a log file.

    >>> p.logError('Print error message out')
    [Thu Jul 04 17:48:00 2013]: (ComponentName) Pyrfa: (Severity) Error: Unexpected error: Print error message out

### Symbol List

__Pyrfa.symbolListRequest(_String_)__  
For consumer application to subscribe symbol lists. User can define multiple symbol list names using “,” to separate each name in **_String_** e.g. `ric1,ric2,ric3`. Data dispatched through `dispatchEventQueue()` function in dictionarys:

Image

    {{'MTYPE':'REFRESH','RIC':'<SYMBOLLIST_NAME>','SERVICE':'<SERVICE_NAME>'},   
    {'ACTION':'<ADD/UPDATE/DELETE>','MTYPE':'IMAGE','SERVICE':'<SERVICE_NAME>','RIC':'<SYMBOLLIST_NAME>','<FID_NAME#1>':'<VALUE#1>'},   
    {'ACTION':'<ADD/UPDATE/DELETE>','MTYPE':'IMAGE','SERVICE':'<SERVICE_NAME>','RIC':'<SYMBOLLIST_NAME>','<FID_NAME#2>':'<VALUE#2>'},   
    ...   
    {'ACTION':'<ADD/UPDATE/DELETE>','MTYPE':'IMAGE','SERVICE':'<SERVICE_NAME>','RIC':'<SYMBOLLIST_NAME>','<FID_NAME#X>':'<VALUE#X>'}}

__Pyrfa.symbolListCloseRequest(_String_)__  
Unsubscribe the specified symbol lists. User can define multiple symbol list names using “,” to separate each name in **_String_**.

    >>> p.symbolListCloseRequest("0#BMD")

__Pyrfa.symbolListCloseAllRequest()__  
Unsubscribe all symbol lists.

    >>> p.symbolListCloseAllRequest()

__Pyrfa.isSymbolListRefreshComplete()__  
Check whether the client receives a complete list of the symbol list.

    >>> p.isSymbolListRefreshComplete()
    True

__Pyrfa.getSymbolList(_String_)__  
Return item names available under the symbol list in string format.

    >>> p.getSymbolList("0#BMD")
    FPCO FPKC FPRD FPGO

__Pyrfa.getSymbolListWatchList()__  
Return names of the subscribed symbol Lists with service names.

    >>> p.getSymbolListWatchList()
    0#BMCA.RDFD 0#ARCA.RDFD

__Pyrfa.symbolListSubmit(_Tuple_)__  
For a provider client to publish a list of symbols to MDH/ADH under data domain 10, **_Tuple_** must contain python dictionaries in the below format. `MTYPE IMAGE` can be added to `Tuple` in order to publish the IMAGE of the item (default `MTYPE` is `UPDATE`).  
```
{'ACTION':'<ADD/UPDATE/DELETE>','RIC':'<SYMBOLLIST_NAME>','<FID_NAME#X>':'<VALUE#X>'}
```

    >>> SYMBOLLIST = {'ACTION':'ADD', 'RIC':'0#BMD', 'KEY':'FCPO', 'PROD_PERM':10, 'PROV_SYMB':'MY439483'},
    [Pyrfa::symbolListSubmit] mapAction: add
    [Pyrfa::symbolListSubmit] symbolName: 0#BMD
    [Pyrfa::symbolListSubmit] mapKey: FCPO
    [Pyrfa::symbolListSubmit] fieldList: [Pyrfa::symbolListSubmit] fieldList: PROV_SYMB=MY439483,PROD_PERM=10
    [OMMCProvServer::submitData] sending refresh item: 0#BMD
    [OMMCProvServer::submitData] sending refresh service: NIP

### Market Price

__Pyrfa.marketPriceRequest(_String_)__  
For consumer client to subscribe market data from P2PS/ADS, user can define multiple item names using “,” to separate each name in **_String_** e.g “ric1,ric2”. The data dispatched through `dispatchEventQueue()` function in a dictionary format as below:

Image

    {'MTYPE':'REFRESH','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>'}
    {'MTYPE':'IMAGE','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>','<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#2>,...,'<FID_NAME#X>':<VALUE#X>}   

Update

    {'MTYPE':'<UPDATE/IMAGE>','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>','<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#X>,...,'<FID_NAME#X>':<VALUE#X>}

Status
```
{'STREAM_STATE':'<Open/Closed>','SERVICE':'<SERVICE_NAME>','TEXT':'<TEXT_MESSAGE>','MTYPE':'STATUS','DATA_STATE':'Suspect','RIC':'<ITEM_NAME>'}
```  
    >>> p.marketPriceRequest('EUR=')
    >>> p.dispatchEventQueue()
    {'MTYPE':'REFRESH','RIC':'EUR=','SERVICE':'IDN_RDF_SDS'}
    {'MTYPE':'IMAGE','RIC':'EUR=','SERVICE':'IDN_RDF_SDS','ASK':1.3712,'BID':1.3709,...}

    
__Pyrfa.marketPriceCloseRequest(_String_)__  
Unsubscribe items from streaming data. User can define multiple item names using “,” to separate each name.

    >>> p.marketPriceCloseRequest ('C.N, EUR=')

__Pyrfa.marketPriceCloseAllRequest()__  
Unsubscribe all items from streaming data.

    >>> p.marketPriceCloseAllRequest()

__Pyrfa.getMarketPriceWatchList()__  
Returns names of the subscribed items with service names.

```
>>> p.getMarketPriceWatchList()   
C.N.IDN_SELECTFEED JPY=.IDN_SELECTFEED
```

__Pyrfa.marketPriceSubmit(_Tuple_)__  
For provider client to publish market data to MDH/ADH, the market data image/update **_Tuple_** must contain python dictionaries in the below format. `MTYPE IMAGE` can be added to `Tuple` in order to publish the IMAGE of the item (default `MTYPE` is `UPDATE`).  
```
{'RIC':'<ITEM_NAME>',<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#X>,...,'<FID_NAME#X>':<VALUE#X>}
```  
    >>> IMAGES = {'RIC':'EUR=', 'RDNDISPLAY':200, 'RDN_EXCHID':155, 'BID':0.988, 'ASK':0.999, 'DIVPAYDATE':'20110623'},
    >>> p.marketPriceSubmit(IMAGES)
    [Pyrfa::marketPriceSubmit] symbolName: EUR=
    [Pyrfa::marketPriceSubmit] fieldList: BID_NET_CH=0.0041,BID=0.988,ASK_TIME=now,ASK=0.999
    [OMMCProvServer::submitData] sending update item: EUR=
    [OMMCProvServer::submitData] sending update service: NIP
    
__Pyrfa.marketPricePause(_String_)__  
Pause subscription to items. User can define multiple item names using “,” to separate each name in **_String_**

    >> p.marketPricePause('EUR=')

__Pyrfa.marketPriceResume(_String_)__  
Resume subscription to the item. User can define multiple item names using “,” to separate each name in **_String_**

    >> p.marketPriceResume('EUR=')

### Market by Order

__Pyrfa.marketByOrderRequest(_String_)__  
For a consumer application to subscribe order book data, user can define multiple item names using “,” to separate each name in **_String_**. The data dispatched through `dispatchEventQueue()` module in a dictionary below:

Image

    {'MTYPE':'REFRESH','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>'}
    {'MTYPE':'IMAGE','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>','ACTION':'ADD','<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#2>,...,'<FID_NAME#X>':<VALUE#X>}

Update

    {'MTYPE':'<UPDATE>','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>','ACTION':'UPDATE','<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#X>,...,'<FID_NAME#X>':<VALUE#X>}

Delete

    {'MTYPE':'UPDATE','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>','ACTION':'DELETE','KEY':'<ORDER_ID>'}

Status
```
{'STREAM_STATE':'<Open/Closed>','SERVICE':'<SERVICE_NAME>','TEXT':'<TEXT_MESSAGE>','MTYPE':'STATUS','DATA_STATE':'Suspect','RIC':'<ITEM_NAME>'}
```
    >>> p.marketByOrderRequest("ANZ.AX");
    >>> p.dispatchEventQueue()
    {'MTYPE': 'REFRESH', 'RIC': 'ANZ.AX', 'SERVICE': 'NIP'}
    {'ORDER_SIDE': 'BID', 'ORDER_TONE': '', 'SERVICE': 'NIP', 'SEQNUM_QT': 2744.0, 'BID': 123.0, 'ORDER_PRC': 20.26, 'MTYPE': 'IMAGE', 'KEY': '538993C200035057B', 'ACTION': 'ADD', 'CHG_REAS': 6.0, 'RIC': 'ANZ.AX', 'ORDER_SIZE': 50.0, 'EX_ORD_TYP': 0.0}

__Pyrfa.marketByOrderCloseRequest(_String_)__  
Unsubscribe an item from order book data stream. User can define multiple item names using “,” to separate each name under **_String_**.

    >>> p.marketByOrderCloseRequest("ANZ.AX")

__Pyrfa.marketByOrderCloseAllRequest()__  
Unsubscribe all items from order book data streaming service.

    >>> p.marketByOrderCloseAllRequest()

__Pyrfa.getMarketByOrderWatchList()__  
Return all subscribed item names on order book streaming data with service names.

    >>> p.getMarketByOrderWatchList()
    ANZ.AX.IDN_SELECTFEED

__Pyrfa.marketByOrderSubmit(_Tuple_)__  
For a provider client to publish specified order book data to MDH/ADH, marketByOrderSubmit(). **_Tuple_** must contain python dictionaries in the below format. `MTYPE IMAGE` can be added to `Tuple` in order to publish the IMAGE of the item (default `MTYPE` is `UPDATE`).  
```
{'ACTION':'<ADD/UPDATE/DELETE>','RIC':'<ITEM_NAME>','KEY':'<ORDER_ID>','<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#2>,...,'<FID_NAME#X>':<VALUE#X>}
```
    >>> ORDERS = {'ACTION':'ADD', 'RIC':'ANZ.AX', 'KEY':'538993C200035057B', 'ORDER_PRC': '20.260', 'ORDER_SIZE':50, 'ORDER_SIDE':'BID', 'SEQNUM_QT':2744, 'EX_ORD_TYP':0, 'CHG_REAS':6,'ORDER_TONE':''},
    >>> p.marketByOrderSubmit(ORDERS)
    [Pyrfa::marketByOrderSubmit] mapAction: update
    [Pyrfa::marketByOrderSubmit] symbolName: ANZ.AX
    [Pyrfa::marketByOrderSubmit] mapKey: 538993C200083483B
    [Pyrfa::marketByOrderSubmit] fieldList: [Pyrfa::marketByOrderSubmit] fieldList: EX_ORD_TYP=0,ORDER_SIZE=50,CHG_REAS=6,ORDER_PRC=20.260,SEQNUM_QT=2744,ORDER_TONE=,ORDER_SIDE=BID
    [OMMCProvServer::submitData] sending update item: ANZ.AX
    [OMMCProvServer::submitData] sending update service: NIP

### Market by Price

__Pyrfa.marketByPriceRequest(_String_)__  
For consumer application to subscribe market depth data, user can define multiple item names using “,” to separate each name. Data dispatched through `dispatchEventQueue()` module in a dictionary below:

Image

    {'MTYPE':'REFRESH','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>'}
    {'MTYPE':'IMAGE','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>','ACTION':'ADD','<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#2>,...,'<FID_NAME#X>':<VALUE#X>}

Update

    {'MTYPE':'UPDATE','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>','ACTION':'UPDATE','<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#X>,...,'<FID_NAME#X>':<VALUE#X>}

Delete

    {'MTYPE':'UPDATE','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>','ACTION':'DELETE','KEY':'<ORDER_ID>'}

Status
```
{'STREAM_STATE':'<Open/Closed>','SERVICE':'<SERVICE_NAME>','TEXT':'<TEXT_MESSAGE>','MTYPE':'STATUS','DATA_STATE':'Suspect','RIC':'<ITEM_NAME>'}
```
    >>> p.marketByPriceRequest("ANZ.CHA")
    >>> p.dispatchEventQueue()
    {'MTYPE': 'REFRESH', 'RIC': 'ANZ.CHA', 'SERVICE': 'NIP'}
    {'ORDER_SIDE': 'BID', 'ORDER_TONE': '', 'SERVICE': 'NIP', 'ORDER_PRC': 20.959, 'NO_ORD': 15, 'MTYPE': 'IMAGE', 'QUOTIM_MS': 16987567, 'KEY': '210001B', 'ACTION': 'ADD', 'RIC': 'ANZ.CHA', 'ORDER_SIZE': 200.0}

__Pyrfa.marketByPriceCloseRequest(_String_)__  
Unsubscribe an item from market depth data stream. User can define multiple item names using “,” to separate each name.

    >>> p.marketByPriceCloseRequest("ANZ.CHA")

__Pyrfa.marketByPriceCloseAllRequest()__  
Unsubscribe all items from market depth streaming service.

    >>> p.marketByPriceCloseAllRequest()

__Pyrfa.getMarketByPriceWatchList()__  
Return all subscribed item names on market depth streaming data with service names.

    >>> p.getMarketByPriceWatchList()
     ANZ.CHA.IDN_SELECTFEED

__Pyrfa.marketByPriceSubmit(_Tuple_)__  
For a provider client to publish the specified market depth data to MDH/ADH, marketByPriceSubmit(). **_Tuple_** must contain python dictionaries in the below format. `MTYPE IMAGE` can be added to `Tuple` in order to publish the IMAGE of the item (default `MTYPE` is `UPDATE`).   
```
{'ACTION':'<ADD/UPDATE/DELETE>','RIC':'<ITEM_NAME>','KEY':'<DEPTH>',<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#2>, ... ,'<FID_NAME#X>':<VALUE#X>}    
```

    >>> DEPTHS = {'ACTION':'ADD', 'RIC':'ANZ.CHA','KEY':'201000B','ORDER_PRC': '20.1000', 'ORDER_SIDE':'BID', 'ORDER_SIZE':'1300', 'NO_ORD':13, 'QUOTIM_MS':16987567,'ORDER_TONE':''},
    >>> p.marketByPriceSubmit(DEPTHS)
    [Pyrfa::marketByPriceSubmit] symbolName: ANZ.CHA
    [Pyrfa::marketByPriceSubmit] mapKey: 210000B
    [Pyrfa::marketByPriceSubmit] fieldList: [Pyrfa::marketByPriceSubmit] fieldList:
    ORDER_SIZE=500,QUOTIM_MS=16987567,NO_ORD=13,ORDER_PRC=21.0000,ORDER_TONE=,ORDER_SIDE=ASK
    [OMMCProvServer::submitData] sending update item: ANZ.CHA
    [OMMCProvServer::submitData] sending update service: NIP

### OMM Posting

__marketPricePost(_Tuple_)__   
OMM Posting (off-stream) leverages on consumer login channel to contribute aka. "post" data up to ADH/ADS cache. Note that the posted service must be up. `MTYPE IMAGE` can be added to `data` in order to post the IMAGE (default `MTYPE` is `UPDATE`)   

Post Image

    {'MTYPE':'IMAGE','RIC':'<ITEM_NAME>', 'TRDPRC_1':price, 'TIMACT':'now'}
    
Post Update _default_
    
    {'RIC':'<ITEM_NAME>', 'TRDPRC_1':price,'TIMACT':'now'}

Post Update to different service
```
{'RIC':'<ITEM_NAME>', 'TRDPRC_1':price, 'TIMACT':'now', 'SERVICE':'<SERVICE_NAME>'}
```  
    
    >>> p.marketPricePost(({'MTYPE':'IMAGE','RIC':'TRI.N', 'TRDPRC_1':price,'TIMACT':'now'},))
    [Pyrfa::marketPricePost] fieldList: TRDPRC_1=4.445,RIC=TRI.N,TIMACT=now,MTYPE=IMAGE
    [OMMPost::submitData] sending refresh item: TRI.N
    [OMMPost::submitData] sending refresh service: NIP
    [Encoder::encodeMarketPriceDataBody]
    TRDPRC_1(6)=4445e-3
    TIMACT(5)=19:27:24.281

### Pause and Resume

__Pyrfa.pauseAll()__  
Pause all subscriptions on all domains. Updates are conflated during the pause.

    >>> p.pauseAll()

__Pyrfa.resumeAll()__  
Resume all subscriptions.

    >>> p.resumeAll()

### TS1

__setTimeSeriesPeriod(_String_)__  
Define a time period for a time series subscription. String can be one of `daily` (default), `weekly`, `monthly`.

__setTimeSeriesMaxRecords(_Int_)__  
Define the maximum output before calling getTimeSeries. Default is 10.

__getTimeSeries(_String_)__  
A helper function that subscribes, wait for data dissemination to be complete, unsubscribe from the service and return series as a list of records. getTimeSeries supports one time series retrieval at a time.

    >>> ric = "CHK.N"
    >>> period = "daily"
    >>> maxrecords = 10
    >>> p.setTimeSeriesPeriod(period)
    >>> p.setTimeSeriesMaxRecords(maxrecords)
    >>> timeseries = p.getTimeSeries(ric)
    >>> print "\n\n############## " + ric + " " + period + " (" + str(len(timeseries)) + " records) " + "##############"
    >>> for record in timeseries:
            print record
    ############## CHK.N daily (11 records) ##############

    DATE,CLOSE,OPEN,HIGH,LOW,VOLUME,VWAP
    2013/11/03,28.840,27.980,29.050,27.950,1998632.000,28.666
    2013/10/31,28.000,27.900,28.100,27.550,1027979.000,27.932
    2013/10/30,27.960,28.190,28.270,27.680,1345424.000,28.005
    2013/10/29,28.150,28.360,28.650,27.770,1370013.000,28.121
    2013/10/28,28.320,28.260,28.500,28.210,1246324.000,28.325
    2013/10/27,28.160,28.260,28.470,28.110,1328412.000,28.228
    2013/10/24,28.470,28.410,28.680,28.153,2462643.000,28.447
    2013/10/23,28.370,27.660,28.680,27.470,1773109.000,28.163

### History
__Pyrfa.historyRequest(_String_)__  
Request for historical data (RDM type 12), this domain is not officially supported by Thomson Reuters. User can define multiple item using “,” to separate each one under **_String_**. The data dispatched through `dispatchEventQueue()` module in a dictionary below:

Image
```
{'MTYPE':'REFRESH','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>'}
{'MTYPE':'IMAGE','RIC':'<ITEM_NAME>','SERVICE':'<SERVICE_NAME>','<FID_NAME#1>':<VALUE#1>,'<FID_NAME#2>':<VALUE#2>,...,'<FID_NAME#3>':<VALUE#X>}
```
    >>> p.historyRequest('tANZ.AX')
    >>> p.dispatchEventQueue()
    {'MTYPE':'REFRESH','RIC':'tANZ.AX','SERVICE':'NIP'}
    {'SERVICE':'NIP','SALTIM':'08:05:22:612:000:000','MTYPE':'IMAGE','TRADE_ID':'123456789','BID_ORD_ID':'5307FBL20AL7B','TRDPRC_1':40.124,'RIC':'tANZ.AX','ASK_ORD_ID':'5307FBL20BN8A'}

__Pyrfa.historyCloseRequest(_String_)__  
Unsubscribe items from historical data streaming service. The user can define multiple item names using “,” to separate each name under **_String_**.

    >>> p.historyCloseRequest('tANZ.AX')

__Pyrfa.historyCloseAllRequest()__  
Unsubscribe all items from historical data streaming service.

    >>> p.historyCloseAllRequest()

__Pyrfa.historySubmit(_Tuple_)__  
For a provider client to publish the specified history data to MDH/ADH, each history image/update. **_Tuple_** must contain python dictionaries in the following format:
```
{'RIC':'<ITEM_NAME>','<FID_NAME#1>':<VALUE#1>,...,'<FID_NAME#X>':<VALUE#X>}
```

    >>> UPDATES = {'RIC':'tANZ.AX', 'TRDPRC_1':40.124, 'SALTIM':'now', 'TRADE_ID':'123456789', 'BID_ORD_ID':'5307FBL20AL7B', 'ASK_ORD_ID':'5307FBL20BN8A'},
    >>> p.historySubmit(UPDATES)
    [Pyrfa::historySubmit] symbolName: tANZ.AX
    [Pyrfa::historySubmit] fieldList: TRDPRC_1=40.124,BID_ORD_ID=5307FBL20AL7B,ASK_ORD_ID=5307FBL20BN8A,TRADE_ID=123456789,SATIM=now
    [OMMCProvServer::submitData] sending update item: tANZ.AX
    [OMMCProvServer::submitData] sending update service: NIP

### Getting Data
__Pyrfa.dispatchEventQueue([_Timeout_])__  
Dispatch the events (data) from EventQueue within a period of time (If **_Timeout_** is omitted, it will return immediately). If there are many events in the queue at any given time, a single call gets all the data until the queue is empty. Data is in dictionary format.

    >>> p.dispatchEventQueue()
    {'MTYPE':'REFRESH','RIC':'EUR=','SERVICE':'IDN_RDF_SDS'}
    {'MTYPE':'IMAGE','RIC':'EUR=','SERVICE':'IDN_RDF_SDS','ASK':1.3712,'BID':1.3709}

License
=======
Copyright (C) 2015-2018 DevCartel Company Limited

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
