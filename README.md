# PyRFA
PyRFA is a Python API for accessing Thomson Reuters market data feeds know as Elektron,
Thomson Reuter Enterprise Platform for Real-time (TREP-RT) or legacy RMDS. It supports subscription
and publication of market data using OMM data message model.

Features:

* Subscription for `MARKET_PRICE` (level 1)
* Subscription for `MARKET_BY_ORDER` (order book)
* Subscription for `MARKET_BY_PRICE` (market depth)
* Snapshot request
* Multiple service subscription
* Pause and resume subscription
* OMM Posting
* View
* Dictionary download or use local files
* Directory request
* Symbol list request
* Timeseries request and decoder for IDN TS1
* Custom domain `MMT_HISTORY` which can be used for intraday timeseries publishing
* Non-interactive provider for `MARKET_PRICE`, `MARKET_BY_ORDER`, `MARKET_BY_PRICE`, `SYMBOLLIST`, `HISTORY` domain
* Interactive provider for `MARKET_PRICE` domain
* Debug mode
* Logging
* Low-latency mode
* Subscription outbound NIC binding

# INSTALLATION
PyRFA supports both Windows and Linux platform. Select your platform for download below.
### Windows

Version | Release Date | Windows (64bit, Python3.5) | Windows (64bit, Python3.4) | Windows (64bit, Python2.7) | Windows (64bit, Python2.6) | Windows (x86, Python2.7) | Windows (x86, Python2.6)
:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
8.0.3 | 13 Dec 16 | [download](https://github.com/devcartel/pyrfa/releases/download/8.0.3/pyrfa8.0.3-win32-x86_64-py35.zip) | [download](https://github.com/devcartel/pyrfa/releases/download/8.0.3/pyrfa8.0.3-win32-x86_64-py34.zip) |  [download](https://github.com/devcartel/pyrfa/releases/download/8.0.3/pyrfa8.0.3-win32-x86_64-py27.zip) | [download](https://github.com/devcartel/pyrfa/releases/download/8.0.3/pyrfa8.0.3-win32-x86_64-py26.zip) | |
7.6.4 | 13 Dec 16 | || | | [download](https://github.com/devcartel/pyrfa/releases/download/7.6.4/pyrfa7.6.4-win32-x86-py27.zip)| [download](https://github.com/devcartel/pyrfa/releases/download/7.6.4/pyrfa7.6.4-win32-x86-py26.zip)

### Linux

Version | Release Date | Linux (64bit, Python3.5) | Linux (64bit, Python3.4) | Linux/RHEL7 (64bit, Python2.7) | RHEL6 (64bit, Python2.6) | RHEL5 (64bit, Python2.4)
:-:|:-:|:-:|:-:|:-:|:-:|:-:
8.0.3 | 13 Dec 16 | [download](https://github.com/devcartel/pyrfa/releases/download/8.0.3/pyrfa8.0.3-linux-x86_64-py35.zip) | [download](https://github.com/devcartel/pyrfa/releases/download/8.0.3/pyrfa8.0.3-linux-x86_64-py34.zip) | [download](https://github.com/devcartel/pyrfa/releases/download/8.0.3/pyrfa8.0.3-linux-x86_64-py27.zip) | [download](https://github.com/devcartel/pyrfa/releases/download/8.0.3/pyrfa8.0.3-rhel64-gcc447-x86_64-py26.zip) |
7.6.4 | 13 Dec 16 | | | | | [download](https://github.com/devcartel/pyrfa/releases/download/7.6.4/pyrfa7.6.4-rhel5-gcc412-x86_64-py24.zip)

Then run:

```
> unzip pyrfa<version>-<platform>.zip
> cd pyrfa<version>-<platform>/
> python setup.py install --force
```

Or simply install from download links using `pip`:
```
> pip install --upgrade https://github.com/devcartel/pyrfa/releases/download/8.0.3/pyrfa8.0.3-linux-x86_64-py27.zip
```

# TABLE OF CONTENTS
1. [Installation](#installation)
2. [Changelog](#changelog)
3. [Performance](#performance)
4. [Supported Systems](#supported-systems)
5. [Example](#example)
6. [Data Type](#data-type)
7. [Configuration File](#configuration-file)
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
  12. [OMM Posting](#omm-posting)
  13. [Pause and Resume](#pause-and-resume)
  14. [Timeseries](#timeseries)
  15. [History](#history)
  16. [Getting Data](#getting-data)
  17. [Publication](#publication)
  18. [Interactive Provider](#interactive-provider)
9. [License](#license)

# CHANGELOG
## PyRFA 8
8.0.3
* 13 December 2016
* Interactive provider is able to logout clients with logoutSubmit
* Fixed a bug where NIP not closeAllSubmit when call serviceDownSubmit

8.0.2
* 5 October 2016
* Supports Python 3.5
* Supports unicode string function calls
* Adds data output for interactive provider
* Makes PyRFA totally silent
* Fixed a Timeseries bug
* New versioning number
* Other bugs fixed
* Compiled with RFA C++ 8.0.1.E1
 
8.0.1.1
* 23 June 2016
* Supports RMTES strings to UTF-8
* New getFieldID() function

8.0.1.0
* 19 May 2016
* Supports FID filtering subscription with View
* Updates RDMDictionary and enumtype.def
* Compiled with RFA 8.0.1.L1

8.0.0.6
* 3 March 2016
* Supports Interactive Provider

8.0.0.5
* 8 October 2015
* Replaced serviceStateSubmit() with serviceUpSubmit() and serviceDownSubmit()
* Fixed a bug where it fails to close a complete published item list

8.0.0.4
* 30 September 2015
* Added serviceStateSubmit() for bringing service up or down
* Fixed serviceDownSubmit() that caused session disconnection

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

## PyRFA 7
7.6.4
* 13 December 2016
* Interactive provider is able to logout clients with logoutSubmit
* Fixed a bug where NIP not closeAllSubmit when call serviceDownSubmit

7.6.3
* 5 October 2016
* Supports unicode string function calls
* Adds data output for interactive provider
* Makes PyRFA totally silent
* Fixed a Timeseries bug
* New versioning number
* Other bugs fixed
* Compiled with RFA C++ 7.6.2.E2
 
7.6.2.2
* 23 June 2016
* Supports RMTES strings to UTF-8
* New getFieldID() function

7.6.2.1
* 19 May 2016
* Supports Interactive Provider
* Supports FID filtering subscription with View
* Updates RDMDictionary and enumtype.def

7.6.2.0
* 17 December 2015
* Compiled with RFA 7.6.2.L1

7.6.1.4
* 23 November 2015
* Compiled with RFA 7.6.1.E5
* Added serviceStateSubmit() for bringing service up or down
* Fixed serviceDownSubmit() that caused session disconnection
* Removed serviceStateSubmit()
* Added serviceUpSubmit()
* Fixed a bug where it fails to close a complete published item list

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

# PERFORMANCE
Subscription
* N/A

Publication
* N/A

# SUPPORTED SYSTEMS
* Linux x86 64bit
* Windows x86 32 and 64bit
* Python 2.6, 2.7, 3.4, 3.5
    
# EXAMPLE
```python
import pyrfa
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
                if type(v) is float:
                    print("%15s %g" % (k,v))
                else:
                    print("%15s %s" % (k,v))
            print("")
p.marketPriceCloseAllRequest()
```

# DATA TYPE

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

# CONFIGURATION FILE
## Example of pyrfa.cfg

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

## Parameter
### Debug
Namespace: `\pyrfa\`

| Parameter        | Example value    | Description                                            |
|------------------|------------------|--------------------------------------------------------|
| `debug`          | `true`/`false`   | Enable/Disable debug mode                              |

### Logger  
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
| `connectionType` | `"RSSL"`         | `RSSL`, `RSSL_NIPROV` or `RSSL_PROV`                   |
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

# FUNCTIONS
## Initialization

__Pyrfa.pyrfa()__    
_➥return: object_  
Instantiate a PyRFA object.

```python
p = Pyrfa.pyrfa()
```

__Pyrfa.setDebugMode([_mode_])__  
_mode: boolean_  
Enable or disable debug messages. If argument is empty, it will read a value from `\pyrfa\debug` in the configuration file. This function can only be called after `createConfigDb`. Example:

```python
p.setDebugMode(True)
```

## Configuration

__Pyrfa.createConfigDb(_filename_)__    
_filename: str_  
Locate and load a configuration file where _filename_ can be an absolute path or a relative path. Example:

```python
p.createConfigDb("./pyrfa.cfg")
```

__Pyrfa.printConfigDb([_node_])__  
_node: str_  
Print a configuration node. If the input parameter _node_ is omitted, this function returns all of the configuration values under the `Default` namespace. Example:

```python
p.printConfigDb("\\Default\\Sessions")
```
Output:
```
\Default\Sessions\Session1\connectionList = Connection_RSSL1
\Default\Sessions\Session2\connectionList = Connection_RSSL2
\Default\Sessions\Session3\connectionList = Connection_RSSL3
\Default\Sessions\Session4\connectionList = Connection_RSSL4
```

## Session

__Pyrfa.acquireSession(_sessionName_)__  
_sessionName: str_  
Acquire a session as defined in the configuration file where _sessionName_ under `Sessions` node.Then look up for an appropriate connection and create a client-server network session. Example:

```python
p.acquireSession('Session1')
```

## Client

__Pyrfa.createOMMConsumer()__    
Create an OMM consumer client.

__Pyrfa.createOMMProvider()__    
Create an OMM provider client. Type of provider is defined by `connectionType`. Use `RSSL_PROV` for interactive provider and `RSSL_NIPROV` for non-interactive, full-cached provider.

__Pyrfa.login([_username_],[_instanceId_],[_applicationId_],[_position_])__    
_username: str_  
_instanceId: str_  
_applicationId: str_  
_position: str_  
Send a login message through the acquired session. This step is mandatory in order to consume the market data from P2PS/ADS. If any argument is omitted, PyRFA will look it up from configuration file.

__Pyrfa.isLoggedIn()__  
_➥return: boolean_  
Check whether the client successfully receives a login status from the P2PS/ADS.

__Pyrfa.setInteractionType(_type_)__  
_type: str_  
Set subscription _type_ to either `snapshot` or `streaming` before making a subscription request. If `snapshot` is set, the client will receive only a full image of an instrument then the subscribed stream will be closed. Example:

```python
p.setInteractionType('snapshot')
```

__Pyrfa.setServiceName(_serviceName_)__  
_serviceName: str_  
Programmatically set service name before making a subcription request. Call this function before making any request. PyRFA also allows subcription to multiple services. Example:

```python
p.setServiceName('IDN')
p.marketPriceRequest('EUR=')
```

## Directory

__Pyrfa.directoryRequest()__  
Send a directory request through the acquired session. This step is the mandatory in order to consume the market data from P2PS/ADS.

## Dictionary

__Pyrfa.dictionaryRequest()__  
If `downloadDataDict` configuration is set to `True` then PyRFA will send a request for data dictionaries to P2PS/ADS. Otherwise, it uses local data dictionaries specified by `fieldDictionaryFilename` and `enumTypeFilename` from configuration file.

__Pyrfa.isNetworkDictionaryAvailable()__  
_➥return: boolean_  
Check whether the data dictionary is successfully downloaded from the server.

__Pyrfa.getFieldID(_fieldName_)__  
_fieldName: str_  
_➥return: int_  
Translates field name to field ID. Example:

```python
fid = p.getFieldID('DSPL_NMLL')
print(fid)
```
Output:
```python
1352
```

## Logging

__Pyrfa.logInfo(_message_)__  
_message: str_  
Write an informational message to a log file.

__Pyrfa.logWarning(_message_)__  
_message: str_  
Write a warning message to a log file.

__Pyrfa.logError(_message_)__  
_message: str_  
Write an error message to a log file.

Example:
```python
p.log('Print log message out')
p.logWarning('Print warning message out')
p.logError('Print error message out')
```

Output in log file with date, time and severity level:
```
[Thu Jul 04 17:45:29 2013]: (ComponentName) Pyrfa: (Severity) Information: Print log message out
[Thu Jul 04 17:47:03 2013]: (ComponentName) Pyrfa: (Severity) Warning: Print warning message out
[Thu Jul 04 17:48:00 2013]: (ComponentName) Pyrfa: (Severity) Error: Unexpected error: Print error message out
```

## Symbol List

__Pyrfa.symbolListRequest(_symbolList_)__  
_symbolList: str_  
For consumer application to subscribe symbol lists. User can define multiple symbol list names using “,” to separate each name in _symbolList_ e.g. `'ric1,ric2,ric3'`. Data dispatched through `dispatchEventQueue` function in dictionary.

__Pyrfa.symbolListCloseRequest(_symbolList_)__  
_symbolList: str_  
Unsubscribe the specified symbol lists. User can define multiple symbol list names using “,” to separate each name in _symbolList_.

__Pyrfa.symbolListCloseAllRequest()__  
Unsubscribe all symbol lists.

__Pyrfa.isSymbolListRefreshComplete()__  
_➥return: boolean_  
Check whether the client receives a complete list of the symbol list.

__Pyrfa.getSymbolListWatchList()__  
_➥return: str_  
Return names of the subscribed symbol Lists with service names in string format. Example:

__Pyrfa.getSymbolList(_symbolList_)__  
_symbolList: str_  
_➥return: str_  
A helper function that subscribes for a symbol list and returns item names available under a symbol list in string format without dealing with symbol list subscription and data dispatching. Note that not all services provide symbol list in its capability. Example:

```python
symbollist = p.getSymbolList('0#BMD')
print(symbollist)
```
Output:
```
FPCO FPKC FPRD FPGO
```

## Market Price

__Pyrfa.marketPriceRequest(_symbols_)__  
_symbols: str_  
For consumer client to subscribe market data from P2PS/ADS, user can define multiple item names using “,” to separate each name in _symbols_ e.g `'ric1,ric2'`. Example:

```python
p.marketPriceRequest('EUR=')
while True:
    updates = dispatchEventQueue(100)
    if updates:
        for u in updates:
            print(u)
```

IMAGE:
```python
{'MTYPE':'REFRESH','RIC':'EUR=','SERVICE':'NIP'},
{'MTYPE':'IMAGE','SERVICE':'NIP','ASK_TIME':'20:43:54:829:000:000','BID':0.988,'DIVPAYDATE':'23 JUN 2011','RDN_EXCHID':'SES','BID_NET_CH':0.004,'RDNDISPLAY':200,'ASK':0.999,'RIC':'EUR='} 
```

UPDATE:
```python
{'MTYPE':'UPDATE','SERVICE':'NIP','BID':0.988,'ASK_TIME':'20:43:57:830:000:000','BID_NET_CH':0.0041,'ASK':0.999,'RIC':'EUR='}
```

STATUS:
```python
{'STREAM_STATE':'Open','SERVICE':'NIP','TEXT':'Source unavailable... will recover when source is up','MTYPE':'STATUS','DATA_STATE':'Suspect','RIC':'EUR='}
{'STREAM_STATE':'Closed','SERVICE':'NIP','TEXT':'F10: Not In Cache','MTYPE':'STATUS','DATA_STATE':'Suspect','RIC':'JPY='}
```

__Pyrfa.setView([_fids_])__  
_fids: str_  
To specifies a view (a subset of fields to be filtered at source) for the next subscribed items. User can define multitple fields using “,” to separate each field in _fids_ which can be a valid field name or number. Example:

```python
p.setView('RDNDISPLAY,TRDPRC_1,22,25')
p.marketPriceRequest('EUR=')
```
Or reset view to get all the fields with:
```python
p.setView()
```

__Pyrfa.marketPriceCloseRequest(_symbols_)__  
_symbols: str_  
Unsubscribe items from streaming data. User can define multiple item names using “,” to separate each name.

__Pyrfa.marketPriceCloseAllRequest()__  
Unsubscribe all items from streaming data.

__Pyrfa.getMarketPriceWatchList()__  
_➥return: str_  
Returns names of the subscribed items suffixed with its service names. Example:
```python
watchlist = p.getMarketPriceWatchList()
print(watchlist)
```
Output:
```
EUR=.IDN_SELECTFEED JPY=.IDN_SELECTFEED
```

## Market by Order

__Pyrfa.marketByOrderRequest(_symbols_)__  
_symbols: str_  
For a consumer application to subscribe order book data, user can define multiple item names using “,” to separate each name in _symbols_. Example:

```python
p.marketByOrderRequest('ANZ.AX')
while True:
    updates = dispatchEventQueue(100)
    if updates:
        for u in updates:
            print(u)
```

IMAGE:
```python
{'MTYPE':'REFRESH','RIC':'ANZ.AX','SERVICE':'NIP'}
{'ORDER_SIDE':'BID','ORDER_TONE':'','SERVICE':'NIP','SEQNUM_QT':2744,'ORDER_PRC':20.26,'MTYPE':'IMAGE','KEY':'538993C200035057B','ACTION':'ADD','CHG_REAS':6,'RIC':'ANZ.AX','ORDER_SIZE':50,'EX_ORD_TYP':0}
{'ORDER_SIDE':'BID','ORDER_TONE':'','SERVICE':'NIP','SEQNUM_QT':2745,'ORDER_PRC':20.84,'MTYPE':'IMAGE','KEY':'538993C200083483B','ACTION':'ADD','CHG_REAS':6,'RIC':'ANZ.AX','ORDER_SIZE':1100,'EX_ORD_TYP':0}
```

UPDATE:
```python
{'MTYPE':'UPDATE','ORDER_SIDE':'BID','ORDER_TONE':'','SERVICE':'NIP','SEQNUM_QT':2745,'ORDER_PRC':20.152999999999999,'KEY':'538993C200083483B','ACTION':'UPDATE','CHG_REAS':6,'RIC':'ANZ.AX','ORDER_SIZE':1200,'EX_ORD_TYP':0}
```

DELETE:
```python
{'ACTION': 'DELETE', 'MTYPE': 'UPDATE', 'SERVICE': 'NIP', 'RIC': 'ANZ.AX', 'KEY': '538993C200083483B'}
```

STATUS:
```python
{'STREAM_STATE': 'Open', 'SERVICE': 'NIP', 'TEXT': 'A23: Service has gone down. Will recall when service becomes available.', 'MTYPE': 'STATUS', 'DATA_STATE': 'Suspect', 'RIC': 'ANZ.AX'}
{'STREAM_STATE':'Closed','SERVICE':'NIP','TEXT':'F10: Not In Cache','MTYPE':'STATUS','DATA_STATE':'Suspect','RIC':'ANZ.AX'}
```

__Pyrfa.marketByOrderCloseRequest(_symbols_)__  
_symbols: str_  
Unsubscribe an item from order book data stream. User can define multiple item names using “,” to separate each name under _symbols_.

__Pyrfa.marketByOrderCloseAllRequest()__  
Unsubscribe all items from order book data streaming service.

__Pyrfa.getMarketByOrderWatchList()__  
_➥return: str_  
Return all subscribed item names on order book streaming data with service names.

## Market by Price

__Pyrfa.marketByPriceRequest(_symbols_)__  
_symbols: str_  
For consumer application to subscribe market depth data, user can define multiple item names using “,” to separate each name. Example:

```python
p.marketByPriceRequest('ANZ.CHA')
while True:
    updates = dispatchEventQueue(100)
    if updates:
        for u in updates:
            print(u)
```
IMAGE:
```ptyhon
{'MTYPE': 'REFRESH', 'RIC': 'ANZ.CHA', 'SERVICE': 'NIP'}
{'MTYPE': 'IMAGE', 'ORDER_SIDE': 'BID', 'ORDER_TONE': '', 'SERVICE': 'NIP','ORDER_PRC': 20.959, 'NO_ORD': 15, 'QUOTIM_MS': 16987567, 'KEY': '210001B', 'ACTION': 'ADD', 'RIC': 'ANZ.CHA', 'ORDER_SIZE': 200.0}
```

UPDATE:
```python
{'ORDER_SIDE': 'BID', 'ORDER_TONE': '', 'SERVICE': 'NIP', 'ORDER_PRC': 20.257, 'NO_ORD': 24, 'MTYPE': 'UPDATE', 'QUOTIM_MS': 16987567, 'KEY': '210000B', 'ACTION': 'UPDATE', 'RIC': 'ANZ.CHA', 'ORDER_SIZE': 1100.0}
```

DELETE:
```python
{'ACTION': 'DELETE', 'MTYPE': 'UPDATE', 'SERVICE': 'NIP', 'RIC': 'ANZ.CHA', 'KEY': '210001B'}
```

STATUS:
```python
{'STREAM_STATE':'Open','SERVICE':'NIP', 'TEXT':'Source unavailable... will recover when source is up','MTYPE':'STATUS','DATA_STATE':'Suspect','RIC':'ANZ.CHA'}
{'STREAM_STATE':'Closed','SERVICE':'NIP','TEXT':'F10: Not In Cache','MTYPE':'STATUS','DATA_STATE':'Suspect','RIC':'ANZ.CHA'}
```

__Pyrfa.marketByPriceCloseRequest(_symbols_)__  
_symbols: str_  
Unsubscribe an item from market depth data stream. User can define multiple item names using “,” to separate each name.

__Pyrfa.marketByPriceCloseAllRequest()__  
Unsubscribe all items from market depth streaming service.

__Pyrfa.getMarketByPriceWatchList()__  
_➥return: str_  
Return all subscribed item names on market depth streaming data with service names.

## OMM Posting
OMM Posting leverages on consumer login channel (off-stream) to contribute data up to ADH/ADS cache or provider application. The posted service must be up before receiving any post message. For posting to an Interactive Provider, the posted RIC must already be made available by the provider.

__marketPricePost(_data_)__  
_data: dict_  
Post market price data. _data_ can be populated as below. `MTYPE` = `IMAGE` can also be added to _data_ dict in order to force post as `IMAGE` (default `MTYPE` is `UPDATE`) as well as `SERVICE`. Example:

```python
p.marketPricePost({'MTYPE':'IMAGE','RIC':'TRI.N','TRDPRC_1':price,'TIMACT':'now'})
```

POSTED IMAGE:
```python
{'MTYPE':'IMAGE','RIC':'TRI.N','TRDPRC_1':4.911,'TIMACT':'now'}
```

POSTED UPDATE:
```python
{'RIC':'TRI.N','TRDPRC_1':4.536,'TIMACT':'now'}
```
    
POSTED UPDATE TO A SELECTIVE SERVICE:
```python
{'RIC':'TRI.N','TRDPRC_1':4.247,'TIMACT':'now','SERVICE':'NIP'} 
```

## Pause and Resume

__Pyrfa.pauseAll()__  
Pause all subscriptions on all domains. Updates are conflated during the pause.

__Pyrfa.resumeAll()__  
Resume all subscriptions.

__Pyrfa.marketPricePause(_symbols_)__  
_symbols: str_  
Pause subscription to items. User can define multiple item names using “,” to separate each name in _symbols_.

__Pyrfa.marketPriceResume(_symbols_)__  
_symbols: str_  
Resume subscription to the item. User can define multiple item names using “,” to separate each name in _symbols_.

## Timeseries
Time Series One (TS1) provides access to historical data distributed via the Reuter Integrated Data Network (IDN). It provides a range of facts (such as Open, High, Low, Close) for the equity, money, fixed income, commodities and energy markets. TS1 data is available in three frequencies -- _daily_, _weekly_, and _monthly_. For daily data there is up to _two years_ worth of history, for weekly data there is _five years_, and for monthly data up to _ten years_.

__setTimeSeriesPeriod(_period_)__  
_period: str_  
Define a time period for a time series subscription. String can be one of `daily`, `weekly`, `monthly`.

__setTimeSeriesMaxRecords(_maxRecords_)__  
_maxRecords: int_  
Define the maximum output before calling `getTimeSeries`.

__getTimeSeries(_symbol_)__  
_symbol: str_  
_➥return: tuple_  
A helper function that subscribes, wait for data dissemination to be complete, unsubscribe from the service and return series as a list of records. `getTimeSeries` supports only one time series retrieval at a time. Example:

```python
ric = 'CHK.N'
period = 'daily'
maxrecords = 10
p.setTimeSeriesPeriod('daily')
p.setTimeSeriesMaxRecords(10)
timeseries = p.getTimeSeries('CHK.N')
print "\n\n############## " + ric + " " + period + " (" + str(len(timeseries)) + " records) " + "##############"
for record in timeseries:
    print record
```

Output:
```
############## CHK.N daily (11 records) ##############

DATE,CLOSE,OPEN,HIGH,LOW,VOLUME,VWAP
2013/11/03,28.840,27.980,29.050,27.950,1998632,28.666
2013/10/31,28.000,27.900,28.100,27.550,1027979,27.932
2013/10/30,27.960,28.190,28.270,27.680,1345424,28.005
2013/10/29,28.150,28.360,28.650,27.770,1370013,28.121
2013/10/28,28.320,28.260,28.500,28.210,1246324,28.325
2013/10/27,28.160,28.260,28.470,28.110,1328412,28.228
2013/10/24,28.470,28.410,28.680,28.153,2462643,28.447
2013/10/23,28.370,27.660,28.680,27.470,1773109,28.163
```

## History

__Pyrfa.historyRequest(_symbols_)__  
_symbols: str_
Request for historical data (RDM type 12), this domain is not officially supported by Thomson Reuters. User can define multiple item using “,” to separate each one under _symbols_. Example:

```python
p.historyRequest('tANZ.AX')
while True:
    updates = dispatchEventQueue(100)
    if updates:
        for u in updates:
            print(u)
```

IMAGE:
```python
{'MTYPE':'REFRESH','RIC':'tANZ.AX','SERVICE':'NIP'}
{'SERVICE':'NIP','SALTIM':'08:05:22:612:000:000','MTYPE':'IMAGE','TRADE_ID':'123456789', 'BID_ORD_ID':'5307FBL20AL7B','TRDPRC_1':40.124,'RIC':'tANZ.AX','ASK_ORD_ID':'5307FBL20BN8A'}
```

__Pyrfa.historyCloseRequest(_symbols_)__  
_symbols: str_  
Unsubscribe items from historical data streaming service. The user can define multiple item names using “,” to separate each name under _symbols_.

__Pyrfa.historyCloseAllRequest()__  
Unsubscribe all items from historical data streaming service.

## Getting Data
__Pyrfa.dispatchEventQueue([_timeout_])__  
_timeout: int_  
_➥return: tuple_  
Dispatch the events and data from event queue within a period of time in milliseconds (If _timeout_ is omitted, it will return immediately). If there are many events in the queue at any given time, a single call gets all the data until the queue is empty. Data is in dictionary format.

## Publication
__Pyrfa.directorySubmit([_domains_],[_service_])__  
_domains: str_  
_service: str_  
Submit directory with domain types (capability) in a provider application and service name, domain type currently supports:

* 6 - market price
* 7 - market by order
* 8 - market by price
* 10 - symbol list
* 12 - history

This function is called automatically upon data submission. If _service_ is omitted, it will use the value from configuration file. Example:
```python
p.directorySubmit('6,7,8','IDN')
```
    
__Pyrfa.serviceDownSubmit([_service_])__  
_service: str_  
Submit the specified down service status to ADH. If _service_ is omitted, it will use the value from configuration file. For Interactive Provider, _service_ will be ignored and use the default value from configuration file instead. This function must be called after `directorySubmit`.

__Pyrfa.serviceUpSubmit([_service_])__  
_service: str_  
Submit the specified up service status to ADH. If _service_ is omitted, it will use the value from configuration file. For Interactive Provider, _service_ will be ignored and use the default value from configuration file instead. This function must be called after `directorySubmit`. However, service will be automatically up if an `IMAGE` is sent.

__Pyrfa.symbolListSubmit(_data_)__  
_data: dict_  
For a provider client to publish a list of symbols to MDH/ADH under data domain 10, _data_ must contain python dictionaries. _data_ dict can be populated as below and `MTYPE` = `IMAGE` can be added to _data_ in order to publish the `IMAGE` of the item (default `MTYPE` is `UPDATE`). `ACTION` can be `ADD`, `UPDATE` and `DELETE`. Example:

```python
p.symbolListSubmit({'ACTION':'UPDATE', 'RIC':'0#BMD', 'KEY':'FKLI', 'PROD_PERM':10, 'PROV_SYMB':'MY1244221'})
```  

__Pyrfa.marketPriceSubmit(_data_)__  
_data: dict_  
For provider client to publish market data to MDH/ADH, the market data image/update _data_ must contain python dictionaries. _data_ dict can be populated as below and `MTYPE` = `IMAGE` can be added to _data_ in order to publish the `IMAGE` of the item (default `MTYPE` is `UPDATE`). Example:

```python
IMAGES = {'RIC':'EUR=', 'RDNDISPLAY':200, 'RDN_EXCHID':155, 'BID':0.988, 'ASK':0.999, 'DIVPAYDATE':'20110623'},
IMAGES += {'RIC':'C.N', 'RDNDISPLAY':200, 'RDN_EXCHID':'NAS', 'OFFCL_CODE':'isin1234XYZ', 'BID':4.23, 'DIVPAYDATE':'20110623', 'OPEN_TIME':'09:00:01.000'},
p.marketPriceSubmit(IMAGES)
```

__Pyrfa.marketByOrderSubmit(_data_)__  
_data: dict_  
For a provider client to publish specified order book data to MDH/ADH, marketByOrderSubmit(). _data_ must contain python dictionaries. _data_ tuple can be populated as below and `MTYPE` = `IMAGE` can be added to _data_ in order to publish the `IMAGE` of the item (default `MTYPE` is `UPDATE`). `ACTION` can be `ADD`, `UPDATE` and `DELETE`. Example:

```python
ORDER = {'ACTION':'ADD', 'RIC':'ANZ.AX', 'KEY':'538993C200035057B', 'ORDER_PRC': '20.260', 'ORDER_SIZE':50, 'ORDER_SIDE':'BID', 'SEQNUM_QT':2744, 'EX_ORD_TYP':0, 'CHG_REAS':6,'ORDER_TONE':''}
p.marketByOrderSubmit(ORDER)
```

__Pyrfa.marketByPriceSubmit(_data_)__  
_data: dict_  
For a provider client to publish the specified market depth data to MDH/ADH, marketByPriceSubmit(). _data_ must contain python dictionaries. _data_ tuple can be populated as below and `MTYPE` = `IMAGE` can be added to _data_ in order to publish the `IMAGE` of the item (default `MTYPE` is `UPDATE`). `ACTION` can be `ADD`, `UPDATE` and `DELETE`. Example:

```python
DEPTH = {'ACTION':'ADD', 'RIC':'ANZ.CHA','KEY':'201000B','ORDER_PRC': '20.1000', 'ORDER_SIDE':'BID', 'ORDER_SIZE':'1300', 'NO_ORD':13, 'QUOTIM_MS':16987567,'ORDER_TONE':''}
p.marketByPriceSubmit(DEPTH)
```

__Pyrfa.historySubmit(_data_)__  
_data: dict_ 
For a provider client to publish the specified history data to MDH/ADH, each history image/update. _data_ must contain python dictionaries. _data_ tuple can be populated as below and `MTYPE` = `IMAGE` can be added to _data_ in order to publish the `IMAGE` of the item (default `MTYPE` is `UPDATE`). Example:

```python
UPDATE = {'RIC':'tANZ.AX', 'TRDPRC_1':40.124, 'SALTIM':'now', 'TRADE_ID':'123456789', 'BID_ORD_ID':'5307FBL20AL7B', 'ASK_ORD_ID':'5307FBL20BN8A'}
p.historySubmit(UPDATE)
```

__Pyrfa.closeSubmit([_symbol.service_])__  
_symbol.service: str_  
For a provider to close published items with specific services. User can define multiple symbol/service pairs using “,” to separate each name.  Example:

```python
p.closeSubmit('EUR=.DEV')
```

__Pyrfa.closeAllSubmit()__  
For a provider to close all published item.

## Interactive Provider
A publisher server for market price domain. Interactive provider's `dispatchEventQueue` output yields `MTYPE` of `LOGIN`, `REQUEST`, `CLOSE` and `LOGOUT`. Example:

```python
...
p.createOMMProvider()
p.dictionaryRequest()
updates = p.dispatchEventQueue():
    if updates:
        for u in updates:
            print(u)
```
Output:
```python
{'MTYPE': 'LOGIN', 'USERNAME': 'pyrfa', 'SESSIONID': '140194443590688', 'SERVICE': 'DIRECT_FEED'}
{'MTYPE': 'REQUEST', 'USERNAME': 'pyrfa', 'SESSIONID': '140194443590688', 'RIC': 'JPY=', 'SERVICE': 'DIRECT_FEED'}
{'MTYPE': 'CLOSE', 'USERNAME': 'JPY=', 'SESSIONID': '140194443590688', 'RIC': 'JPY=', 'SERVICE': 'DIRECT_FEED'}
{'MTYPE': 'LOGOUT', 'USERNAME': 'pyrfa', 'SESSIONID': '140194443590688', 'SERVICE': 'DIRECT_FEED'}
```

# License
Copyright (C) 2016-2018 DevCartel Company Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
