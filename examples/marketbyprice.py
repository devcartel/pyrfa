#!/usr/bin/python
#
# This file illustrates the example to request OMMlevel2 data using marketByPriceRequest
# The example data output is mentioned below:
#
# REFRESH/IMAGE:
#   {'MTYPE': 'REFRESH', 'RIC': 'ANZ.CHA', 'SERVICE': 'NIP'}
#   {'ORDER_SIDE': 'BID', 'ORDER_TONE': '', 'SERVICE': 'NIP', 
#    'ORDER_PRC': 20.959, 'NO_ORD': 15, 'MTYPE': 'IMAGE', 'QUOTIM_MS': 16987567, 
#    'KEY': '210001B', 'ACTION': 'ADD', 'RIC': 'ANZ.CHA', 'ORDER_SIZE': 200.0}
#
# UPDATE:
#    {'ORDER_SIDE': 'BID', 'ORDER_TONE': '', 'SERVICE': 'NIP', 
#    'ORDER_PRC': 20.257, 'NO_ORD': 24, 'MTYPE': 'UPDATE', 'QUOTIM_MS': 16987567, 
#    'KEY': '210000B', 'ACTION': 'UPDATE', 'RIC': 'ANZ.CHA', 'ORDER_SIZE': 1100.0}
#
# DELETE:
#    {'ACTION': 'DELETE', 'MTYPE': 'UPDATE', 'SERVICE': 'NIP', 'RIC': 'ANZ.CHA', 'KEY': '210001B'}
#
# STATUS:
#    {'STREAM_STATE':'Open','SERVICE':'NIP',
#    'TEXT':'Source unavailable... will recover when source is up','MTYPE':'STATUS','DATA_STATE':'Suspect','RIC':'ANZ.CHA'}   
#
import pyrfa

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.setDebugMode(True)
p.acquireSession("Session3")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()

p.marketByPriceRequest("ANZ.CHA");

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
del p
