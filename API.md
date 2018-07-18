# PyRFA - API

**TABLE OF CONTENTS**

1. [Data Type](#data-type)
2. [Configuration File](#configuration-file)
3. [API](#api)
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
   17. [Non-Interactive Provider](#non-interactive-provider)
   18. [Interactive Provider](#interactive-provider)

## DATA TYPE

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

## CONFIGURATION FILE
### Example of pyrfa.cfg

    \pyrfa\debug = false

    \Logger\AppLogger\useInternalLogStrings  = true
    \Logger\AppLogger\windowsLoggerEnabled   = false
    \Logger\AppLogger\fileLoggerEnabled      = true
    \Logger\AppLogger\fileLoggerFilename     = "./pyrfa.log"

    \Connections\Connection_RSSL1\rsslPort = "14002"
    \Connections\Connection_RSSL1\serverList = "127.0.0.1"
    \Connections\Connection_RSSL1\connectionType = "RSSL"
    \Connections\Connection_RSSL1\logEnabled = true
    \Connections\Connection_RSSL1\userName = "USERNAME"
    \Connections\Connection_RSSL1\instanceId = "1"
    \Connections\Connection_RSSL1\applicationId = "180"
    \Connections\Connection_RSSL1\position = "127.0.0.1"
    \Connections\Connection_RSSL1\serviceName = "SERVICE"
    \Connections\Connection_RSSL1\downloadDataDict = false

---

### Parameter
#### Debug
Namespace: `\pyrfa\`

| Parameter        | Example value    | Description                                            |
|------------------|------------------|--------------------------------------------------------|
| `debug`          | `true`/`false`   | Enable/Disable debug mode                              |

#### Logger  
Namespace: `\Logger\AppLogger\`

| Parameter            | Example value    | Description                                        |
|----------------------|------------------|----------------------------------------------------|
| `fileLoggerEnabled`  | `true`/`false`   | Enable/Disable logging capability                  |
| `fileLoggerFilename` | `"./pyrfa.{T}.log"` | Sets the name of the file (with absolute or relative file path) in which the log message is written. Used only when `fileLoggerEnabled` is set to `true`.<br />Options:<br />{A} = process name<br />{P} = PID<br />{T} = UTC timestamp<br />{H} = hostname


#### Connection
Namespace: `\Connections\<connection_name>\`

| Parameter        | Example value    | Description                                            |
|------------------|------------------|--------------------------------------------------------|
| `rsslPort`       | `"14002"`        | P2PS/ADS RSSL port number                              |
| `serverList`     | `"127.0.0.1"`    | P2PS/ADS IP address or hostnam                         |
| `connectionType` | `"RSSL"`         | `RSSL`, `RSSL_NIPROV` or `RSSL_PROV`                   |
| `logEnabled`     | `true`/`false`   | Enable/Disable logging capability                      |
| `userName`       | `"pyrfa"`        | DACS username                                          |
| `instanceId`     | `"123"`          | Application instance ID                                |
| `applicationId`  | `"180"`          | Application ID                                         |
| `position`       | `"127.0.0.1/net"`| DACS position                                          |
| `serviceName`    | `"NIP"`          | Service name to be subscribe                           |
| `vendorName`     | `"OMMCProv_DevCartel"` | Vendor name for provider application             |
| `symbolList`     | `"0#BMD"`        | Symbollist name to be subscribed                       |
| `downloadDataDict` | `true`/`false` | Enable/Disable data dictionary download from P2PS/ADS  |
| `dumpDataDict`   | `true`/`false`   | Enable/Disable to dump data dictionary from P2PS/ADS   |

#### Session
Namespace: `\Sessions\<session_name>\`

| Parameter          | Example value         | Description                                            |
|--------------------|-----------------------|--------------------------------------------------------|
| `connectionList`   | `"Connection_RSSL1"`  | Match `connection_name`                                |

`session_name` must be passed to `acquireSession()` function.

#### Service Groups

*!! This section is only available on PyRFA Enterprise Support subcription. Please visit http://devcartel.com/pyrfa-enterprise for more information. !!*

## API
### Initialization

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
---

### Configuration

__Pyrfa.createConfigDb(_filename_)__    
_filename: str_  
Locate and load a configuration file where _filename_ can be an absolute path or a relative path. Example:

```python
p.createConfigDb("./pyrfa.cfg")
```

__Pyrfa.getConfigDb(_node_)__  
_node: str_  
_➥return: str_  
Get a configuration value as a string. Example:

```python
p.getConfigDb("\\Default\\Connections\\Connection_RSSL1\\userName")
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
---

### Session

__Pyrfa.acquireSession(_sessionName_)__  
_sessionName: str_  
Acquire a session as defined in the configuration file where _sessionName_ under `Sessions` node.Then look up for an appropriate connection and create a client-server network session. Example:

```python
p.acquireSession('Session1')
```

---

### Client

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
Set subscription _type_ to either `snapshot` or `streaming` before making a subscription request. If `snapshot` is set, the client will receive only a full image of an instrument then the subscribed stream will be closed. Default is `streaming`. Example:

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

---

### Directory

__Pyrfa.directoryRequest()__  
_➥return: tuple_  
Send a directory request through the acquired session. This step is the mandatory in order to consume the market data from P2PS/ADS. Returns a tuple of service information in dict format. Example:

```python
({'VENDOR': 'DEV', 'NAME': 'IDN_RDF_SDS', 'SERVICE': 'DIRECT_FEED',
 'CAPABILITIES': '5 6', 'SUPPORTS_OUT_OF_BAND_SNAPSHOTS': '1', 'MTYPE': 'DIRECTORY',
 'IS_SOURCE': '1', 'DICTIONARIES_USED': 'RWFFld RWFEnum', 'SERVICE_ID': '1',
 'ACCEPTING_REQUESTS': '1', 'SERVICE_STATE': '1'},)
```
---

### Dictionary

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
p.getFieldID('DSPL_NMLL')
```
Output:
```python
1352
```
---

### Logging

__Pyrfa.logInfo(_message_)__  
_message: str_  
Write an informational message to a log file.

__Pyrfa.logWarning(_message_)__  
_message: str_  
Write a warning message to a log file.

__Pyrfa.logError(_message_)__  
_message: str_  
Write an error message to a log file. Example:

```python
p.logInfo('Print log message out')
p.logWarning('Print warning message out')
p.logError('Print error message out')
```

Output in log file with date, time and severity level:
```
[Thu Jul 04 17:45:29 2013]: (ComponentName) Pyrfa: (Severity) Information: Print log message out
[Thu Jul 04 17:47:03 2013]: (ComponentName) Pyrfa: (Severity) Warning: Print warning message out
[Thu Jul 04 17:48:00 2013]: (ComponentName) Pyrfa: (Severity) Error: Unexpected error: Print error message out
```
---

### Symbol List

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
Return names of the subscribed symbol Lists with service names in string format.

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
---

### Market Price

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

__Pyrfa.setView()__  

*!! This section is only available on PyRFA Enterprise Support subcription. Please visit http://devcartel.com/pyrfa-enterprise for more information. !!*

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

---

### Market by Order

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

---

### Market by Price

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

---

### OMM Posting
OMM Posting leverages on consumer login channel (off-stream) to contribute data up to ADH/ADS cache or provider application. The posted service must be up before receiving any post message. For posting to an Interactive Provider, the posted RIC must already be made available by the provider.

*!! This section is only available on PyRFA Enterprise Support subcription. Please visit http://devcartel.com/pyrfa-enterprise for more information. !!*

---

### Pause and Resume
Pause and resume openning subcriptions. Updates are conflated during the pause and only work with interaction type `streaming`.

*!! This section is only available on PyRFA Enterprise Support subcription. Please visit http://devcartel.com/pyrfa-enterprise for more information. !!*

---

### Timeseries
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

---

### History

__Pyrfa.historyRequest(_symbols_)__  
_symbols: str_  
Request for historical data (RDM type 12), this domain is not officially supported by Thomson Reuters. User can define multiple item names using “,” to separate each one under _symbols_. Example:

*!! This section is only available on PyRFA Enterprise Support subcription. Please visit http://devcartel.com/pyrfa-enterprise for more information. !!*

---

### Getting Data
__Pyrfa.dispatchEventQueue([_timeout_])__  
_timeout: int (Optional)_  
_➥return: tuple_  
Dispatch the events and data from event queue within a period of time in milliseconds (If _timeout_ is omitted, it will return immediately). If there are many events in the queue at any given time, a single call gets all the data until the queue is empty. Data is in dictionary format.

---

### Non-Interactive Provider
__Pyrfa.directorySubmit([_domains_],[_service_])__  
_domains: str (Optional)_  
_service: str (Optional)_  
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
_service: str (Optional)_  
Submit the specified down service status to ADH. If _service_ is omitted, it will use the value from configuration file. For Interactive Provider, _service_ will be ignored and use the default value from configuration file instead. This function must be called after `directorySubmit`.

__Pyrfa.serviceUpSubmit([_service_])__  
_service: str (Optional)_  
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

__Pyrfa.closeSubmit(_symbols,[service]_)__  
_symbol: str_  
_service: str (Optional)_  
For a provider to close published items on a default service or on a specified service. User can input multiple symbols using “,” to separate each symbol name. Example:

```python
p.closeSubmit('EUR=,JPY=')
p.closeSubmit('EUR=','DEV')
```

__Pyrfa.closeAllSubmit()__  
For a provider to close all published item.

__Pyrfa.staleSubmit(_symbols,[service]_)__  
_symbol: str_  
_service: str (Optional)_  
For a provider to mark published items as stale on a default service or on a specified service. User can input multiple symbols using “,” to separate each symbol name. Example:

```python
p.staleSubmit('EUR=,JPY=')
p.staleSubmit('EUR=','DEV')
```

__Pyrfa.staleAllSubmit()__  
For a provider to mark all published items as stale.

---

### Interactive Provider
A publisher server for market price domain. Interactive provider's `dispatchEventQueue` output yields `MTYPE` of `LOGIN`, `REQUEST`, `CLOSE` and `LOGOUT`. Example:

*!! This section is only available on PyRFA Enterprise Support subcription. Please visit http://devcartel.com/pyrfa-enterprise for more information. !!*
