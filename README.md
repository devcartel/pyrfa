# PyRFA
[![version](https://img.shields.io/pypi/v/pyrfa.svg)](https://pypi.python.org/pypi/pyrfa)
[![pyversion](https://img.shields.io/pypi/pyversions/pyrfa.svg)](#platform-availability)
[![version](https://img.shields.io/pypi/format/pyrfa.svg)](https://pypi.python.org/pypi/pyrfa)
[![platform](https://img.shields.io/badge/platform-linux--64%20|%20win--64%20|%20win--32-lightgray.svg)](#platform-availability)
[![license](https://img.shields.io/pypi/l/pyrfa.svg)](LICENSE.txt)

PyRFA is a Python API for accessing Thomson Reuters market data feeds known as Elektron,
Thomson Reuter Enterprise Platform for Real-time (TREP-RT) or legacy RMDS. It supports subscription
and publication of market data using OMM data message model.

![application](http://media.virbcdn.com/cdn_images/resize_1024x1365/6d/64a90d875c40da95-ScreenShot2017-10-18at151123.png)

## Features

* Subscription for `MARKET_PRICE` (level 1)
* Subscription for `MARKET_BY_ORDER` (order book)
* Subscription for `MARKET_BY_PRICE` (market depth)
* Snapshot/streaming request
* Multiple service subscription
* Pause and resume subscription
* OMM Posting
* View
* Dictionary download or use local files
* Directory request
* Symbol list request
* Time-series request and decoder for IDN TS1
* Custom domain `MMT_HISTORY` which can be used for intraday timeseries publishing
* Non-interactive provider (full-cached) for `MARKET_PRICE`, `MARKET_BY_ORDER`, `MARKET_BY_PRICE`, `SYMBOLLIST`, `HISTORY` domains
* Interactive provider for `MARKET_PRICE` domain
* Debug mode
* Logging
* Low-latency mode
* Subscription outbound NIC binding

## Installation
PyRFA supports both Windows and Linux platforms. Simply install from [PyPI](https://pypi.python.org/pypi/pyrfa) using `pip`:

```
> pip install pyrfa
```

## Platform Availability

### Windows
Version | Release Date | 64bit-Windows, Python 3.6 | 64bit-Windows, Python 3.5 | 64bit-Windows, Python 3.4 | 64bit-Windows, Python 2.7 | 64bit-Windows, Python 2.6 | 32bit-Windows, Python 2.7 | 32bit-Windows, Python 2.6
:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
8.3.0 | 7 Feb 18 | ⚬ | ⚬ | ⚬ | ⚬ | ⚬ | |
7.7.0 | 17 Mar 17 | | | | | | ⚬ | ⚬ 

### Linux
Version | Release Date | 64bit-Linux, Python 3.6 | 64bit-Linux, Python 3.5 | 64bit-Linux, Python 3.4 | 64bit-Linux/RHEL7, Python 2.7 | 64bit-Linux/RHEL6, Python 2.6
:-:|:-:|:-:|:-:|:-:|:-:|:-:
8.3.0 | 7 Feb 18 | ⚬ | ⚬ | ⚬ | ⚬ | ⚬

## Example

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
        for data in p.dispatchEventQueue(100):
            print(data)
    except KeyboardInterrupt:
        end = True
```
Output:
```python
...
{'MTYPE': 'UPDATE', 'RIC': 'JPY=', 'SERVICE': 'IDN_RDF_SDS', 'IRGPRC': 0.24}
{'MTYPE': 'UPDATE', 'ASIA_NETCH': 0.7, 'SERVICE': 'IDN_RDF_SDS', 'RIC': 'JPY='}
{'MTYPE': 'UPDATE', 'BID_NET_CH': 0.26, 'RIC': 'JPY=', 'SERVICE': 'IDN_RDF_SDS'}
...
```
## Support
* [PyRFA Enterprise Support](http://devcartel.com/pyrfa-enterprise)
* [Issue tracker](https://github.com/devcartel/pyrfa/issues)
* [Documentation](https://github.com/devcartel/pyrfa-enterprise/tree/master/docs)

