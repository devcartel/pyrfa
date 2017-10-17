#!/usr/bin/python
#
# Like a consumer but no updates after refresh messages
#
import threading
import pyrfa

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.acquireSession("Session1")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()

rics = "JPY=,EUR="
p.setInteractionType("snapshot")
p.marketPriceRequest(rics)

# run until all images received or timeout
def stop():
    global timeout
    timeout = True

timeout = False
t = threading.Timer(10, stop)
t.start()

refresh_received = 0
while not timeout and refresh_received < len(rics.split(',')):
    for u in p.dispatchEventQueue():
        if u:
            if u['MTYPE'] == "REFRESH":
                refresh_received += 1
            print(u['SERVICE'] + " - " + u['RIC'])
            for k,v in u.items():
                if type(v) is float:
                    print("%15s %g" % (k,v))
                else:
                    print("%15s %s" % (k,v))
            print("")
t.cancel()
