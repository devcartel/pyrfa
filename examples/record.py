#!/usr/bin/python
#
# Subscribe to multiple RICs, using local dictionary, debug mode is manually off.
# and write records to a file
#
# Sample records
#
#   1508294058645:{'MTYPE': 'UPDATE', 'ASIA_NETCH': '0.68', 'SERVICE': 'IDN_RDF_SDS', 'RIC': 'JPY='}
#   1508294062294:{'EURO_NETCH': '0.19', 'MTYPE': 'UPDATE', 'RIC': 'JPY=', 'SERVICE': 'IDN_RDF_SDS'}
#   1508294062363:{'MTYPE': 'UPDATE', 'ASIA_NETCH': '-0.0004', 'SERVICE': 'IDN_RDF_SDS', 'RIC': 'EUR='}
#   1508294065485:{'EURO_NETCH': '0.0021', 'MTYPE': 'UPDATE', 'RIC': 'EUR=', 'SERVICE': 'IDN_RDF_SDS'}
#   1508294068574:{'MTYPE': 'UPDATE', 'BID_NET_CH': '0.23', 'RIC': 'JPY=', 'SERVICE': 'IDN_RDF_SDS'}
#
import pyrfa
import time
import sys
    
class ds(dict):                                              
    def __str__(self):
        return str({k:str(v) if isinstance(v,float) else v for k,v in self.items()})

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.setDebugMode(False)
p.acquireSession("Session1")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()
p.marketPriceRequest("JPY=,EUR=")

if len(sys.argv) == 2:
    f = open(sys.argv[1], 'w', 0)
else:
    f = None

end = False
while not end:
    try:
        updates = p.dispatchEventQueue(100)
    except KeyboardInterrupt:
        end = True
    for update in updates:
        u = ds(update)
        ts = int(time.time() * 1000)
        rec = '{ts}:{u}\n'.format(ts=str(ts), u=u)
        if f:
            f.write(rec)
        else:
            print(rec)
if f: f.close()
