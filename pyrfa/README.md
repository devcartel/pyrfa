PYRFA
=====

PyRFA is a Python extension for accessing RFA (Reuters/Robust Foundationa API)
C++ functions, making connection to RMDS and subscribe/publish market data to/from RMDS
using OMM data message model.

Features:
* subscription for MARKET_PRICE (level 1)
* subscription for MARKET_BY_ORDER (order book)
* subscription for MARKET_BY_PRICE (market depth)
* snapshot request (no incremental updates)
* dictionary download or use local files
* directory request
* symbol list request
* timeseries request and decoder for IDN TS1
* custom domain MMT_HISTORY which can be used for intraday timeseries publishing
* non-interactive provider for MARKET_PRICE, MARKET_BY_ORDER, MARKET_BY_PRICE, SYMBOLLIST, HISTORY
* debug mode
* logging
* low-latency mode
* subscription outbound NIC binding
* example scripts

WHAT'S NEW
----------
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
* Utils scripts e.g. every()
* Support 32bit Windows
* Support 64bit Linux
* RFA 7.2.1.L1 for Linux
* RFA 7.2.1.E3 for Windows
    
PERFORMANCE
-----------
Subscription
* N/A

Publication
* N/A

SUPPORT SYSTEM
--------------
* Linux x86 64bit
* Windows x86 32bit
* Python 2.6, 2.7

INSTALLATION
------------
* Require installing Python 2.6 or 2.7
* Optionally, copy lib/pyrfa folder into your system's Python lib folder e.g. c:\Python2.7\Lib or /usr/lib/python2.7
* However the examples also include "sys.path.append()" so you don't actually need to do the above to run example scripts

RUNNING EXAMPLES
----------------
* On Windows CMD ,run with
    
        > python consumer02.py
        
* On Linux or Windows Cygwin:
    
        $ python consumer02.py
        $ ./consumer02.py

