# CHANGELOG
## PyRFA 8
8.2.0
* 22 May 2017
* Adds staleSubmit() and staleAllSubmit()
* Fixes a bug where serviceDownSubmit() inadvertently closes items on other services
* Changes closeSubmit() that can take only RIC name

8.1.0
* 17 March 2017
* Supports Python3.6
* Adds getClientSessions() for Interactive Provider
* Adds getClientWactchList() for Interactive Provider
* Automatically loads dictionaries from the installed package folder
* Able to privately submitData to a specific session ID
* Able to closeSubmit items for a specific session ID
* Available from PyPI

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