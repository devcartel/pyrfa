# PyRFA
[![version](https://img.shields.io/pypi/v/pyrfa.svg)](https://pypi.python.org/pypi/pyrfa)
[![pyversion](https://img.shields.io/pypi/pyversions/pyrfa.svg)](#platform-availability)
[![version](https://img.shields.io/pypi/format/pyrfa.svg)](https://pypi.python.org/pypi/pyrfa)
[![platform](https://img.shields.io/badge/platform-linux--64%20|%20win--64%20|%20win--32-lightgray.svg)](#platform-availability)
[![license](https://img.shields.io/pypi/l/pyrfa.svg)](LICENSE.txt)
[![paypal](https://img.shields.io/badge/fund%20this%20project-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=ADMRKER4V4LZL)

PyRFA is a Python [API](API.md) for accessing Thomson Reuters market data feeds know as Elektron,
Thomson Reuter Enterprise Platform for Real-time (TREP-RT) or legacy RMDS. It supports subscription
and publication of market data using OMM data message model.

**Features**

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

## Installation
PyRFA supports both Windows and Linux platforms. Simply install from [PyPI](https://pypi.python.org/pypi/pyrfa) using `pip`:

```
> pip install pyrfa
```

Or [download](https://pypi.python.org/pypi/pyrfa) a distribution package (.whl) and install it off-line with `pip` e.g.:

```
> pip install /path/to/pyrfa-8.2.1-cp36-none-manylinux1_x86_64.whl
```

## Platform Availability

Version | Release Date | Windows (64bit, Python3.6) | Windows (64bit, Python3.5) | Windows (64bit, Python3.4) | Windows (64bit, Python2.7) | Windows (64bit, Python2.6) | Windows (32bit, Python2.7) | Windows (32bit, Python2.6)
:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
8.2.1 | 11 Sep 17 | ✔ | ✔ | ✔ | ✔ | ✔ | |
7.7.0 | 17 Mar 17 | | | | | | ✔ | ✔ 

Version | Release Date | Linux (64bit, Python3.6) | Linux (64bit, Python3.5) | Linux (64bit, Python3.4) | Linux/RHEL7 (64bit, Python2.7) | Linux/RHEL6 (64bit, Python2.6)
:-:|:-:|:-:|:-:|:-:|:-:|:-:
8.2.1 | 11 Sep 17 | ✔| ✔ | ✔ | ✔ | ✔

✔ = available from PyPi. See [CHANGELOG](CHANGELOG8.md) for more information.

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
## API Reference
See [API.md](API.md) for complete reference.

## Discussion
If you run into bugs or have questions, file them in our [issue tracker](https://github.com/devcartel/pyrfa/issues).

## Sponsorship
[![paypal](https://img.shields.io/badge/fund%20this%20project-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=ADMRKER4V4LZL)  
This project requires commercial licensing from Thomson Reuters. If you guys feel like supporting this project for [$19.99/month](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=ADMRKER4V4LZL) in order to keep PyRFA available, updated and improved. We appreciate your support.  :)
