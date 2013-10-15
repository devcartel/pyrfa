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

WHAT'S NEW
----------
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
* Fixed bug#46 that gives wrong update list when subscribing to marketbyprice/order
      and marketprice with the same tclrfa instance

7.0.1.200
* October 17, 2011
* Log status message in a log file

7.0.1.194
* October 14, 2011
* Using RFA 7.0.1.E2
* Fixed bug#32, Upgrade dictionary files to 4.00.14
* Fixed bug#43, using tclstub8.4 on Windows now so tclrfa would run
      in Tcl 8.4 and 8.5 on both 32-bit and 64-bit Windows
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
* Raname package to be tclrfa<rfaversion>.<build> for better tracking
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
* createEventQueue and acquireLogger not exposed in Tcl anymore,
      they are called automatically upon invoking acquireSession
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

