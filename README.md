# PyRFA
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

## INSTALLATION
PyRFA supports both Windows and Linux platforms. Simply install from [PyPI](https://pypi.python.org/pypi/pyrfa) using `pip`:

```
> pip install pyrfa
```

Or [download](https://pypi.python.org/pypi/pyrfa) a distribution package (.whl) and install it off-line with `pip` e.g.:

```
> pip install /path/to/pyrfa-8.2.0-cp36-none-manylinux1_x86_64.whl
```

### Platform Availability

Version | Release Date | Windows (64bit, Python3.6) | Windows (64bit, Python3.5) | Windows (64bit, Python3.4) | Windows (64bit, Python2.7) | Windows (64bit, Python2.6) | Windows (32bit, Python2.7) | Windows (32bit, Python2.6)
:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
8.2.1 | 11 Sep 17 | ✔ | ✔ | ✔ | ✔ | ✔ | |
7.7.0 | 17 Mar 17 | | | | | | ✔ | ✔ 

Version | Release Date | Linux (64bit, Python3.6) | Linux (64bit, Python3.5) | Linux (64bit, Python3.4) | Linux/RHEL7 (64bit, Python2.7) | Linux/RHEL6 (64bit, Python2.6) | RHEL5 (64bit, Python2.4)
:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
8.2.1 | 11 Sep 17 | ✔| ✔ | ✔ | ✔ | ✔ | [Enterprise program](http://devcartel.com/enterprise)

✔ = available from PyPi. See [CHANGELOG](CHANGELOG8.md) for more information.

## EXAMPLE

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

## Support
* [API documentation](API.md)
* Report an issue on our [GitHub](https://github.com/devcartel/pyrfa/issues)
* Enterprise support is [available](http://devcartel.com/enterprise)
