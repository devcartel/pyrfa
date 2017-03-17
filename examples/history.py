#!/usr/bin/python
#
# Request for historical data (RDM type 12) published by provider.history.tcl
# This domain is not officially supported by Thomson Reuters
# Sample:
#   {'MTYPE':'REFRESH','RIC':'tANZ.AX','SERVICE':'NIP'}
#   {'SERVICE':'NIP','SALTIM':'08:05:22:612:000:000','MTYPE':'IMAGE','TRADE_ID':'123456789',
#    'BID_ORD_ID':'5307FBL20AL7B','TRDPRC_1':40.124,'RIC':'tANZ.AX','ASK_ORD_ID':'5307FBL20BN8A'}
#
import sys
import pyrfa

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.setDebugMode(False)
p.acquireSession("Session3")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()

p.historyRequest("tANZ.AX")

count = 0
while not p.isHistoryRefreshComplete():
    for u in p.dispatchEventQueue():
        if count == 1:
            print(u['SERVICE'] + " - " + u['RIC'])
            print("-----------------------")
            for k,v in u.items():
                sys.stdout.write(k+',')
            print("")
            for k,v in u.items():
                sys.stdout.write(str(v)+',')
        elif count > 1:
            for k,v in u.items():
                sys.stdout.write(str(v)+',')
        count += 1
        print("")

print("\n\n########## total history records: %s ###################\n\n" % (count - 1))