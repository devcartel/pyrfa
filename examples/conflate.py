#!/usr/bin/python
#
# Subscribe to market price and and pause the streaming. Once paused,
# updates are conflated at ADS. To get 10-second conflated update, just
# resume and pause again every 10 seconds.
#
import pyrfa
exec(open('./utils/every.py').read())

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.acquireSession("Session1")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()
p.marketPriceRequest("JPY=")
p.dispatchEventQueue(1000)
p.pauseAll()

def getConflatedUpdate():
    p.resumeAll()
    updates = p.dispatchEventQueue(10)
    p.pauseAll()
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


loop = every(10, getConflatedUpdate)
end = False
try:
    while not end:
        pass
except KeyboardInterrupt:
    loop.stop()
    end = True
