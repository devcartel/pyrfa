#!/usr/bin/python
#
# This file illustrates the example to request OMMlevel2 data using marketByOrderRequest
# The example data output is mentioned below:
#
# REFRESH/IMAGE:
#   {'MTYPE': 'REFRESH', 'RIC': 'ANZ.AX', 'SERVICE': 'NIP'}
#   {'ORDER_SIDE': 'BID', 'ORDER_TONE': '', 'SERVICE': 'NIP', 'SEQNUM_QT': 2744.0, 'BID': 123.0, 
#   'ORDER_PRC': 20.26, 'MTYPE': 'IMAGE',  'KEY': '538993C200035057B', 'ACTION': 'ADD', 'CHG_REAS': 6.0, 
#   'RIC': 'ANZ.AX', 'ORDER_SIZE': 50.0, 'EX_ORD_TYP': 0.0}
#
# UPDATE:
#    {'ORDER_SIDE':'BID','ORDER_TONE':'','SERVICE':'NIP',
#    'SEQNUM_QT':2745.0,'ORDER_PRC':20.536,'MTYPE':'UPDATE','KEY':'538993C200083483B',
#    'ACTION':'UPDATE','CHG_REAS':6.0,'RIC':'ANZ.AX','ORDER_SIZE':2800.0,'EX_ORD_TYP':0.0}
#
# DELETE:
#    {'ACTION': 'DELETE', 'MTYPE': 'UPDATE', 'SERVICE': 'NIP', 'RIC': 'ANZ.AX', 'KEY': '538993C200083484B'}
#
# STATUS:
#    {'STREAM_STATE': 'Open', 'SERVICE': 'NIP', 'TEXT': 'A23: Service has gone down. Will recall when service becomes available.', 
#    'MTYPE': 'STATUS', 'DATA_STATE': 'Suspect', 'RIC': 'ANZ.AX'}   
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

p.marketByOrderRequest("ANZ.AX");

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
