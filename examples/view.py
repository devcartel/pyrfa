#!/usr/bin/python
#
# Specify a set of fields in a request to subscribe only defined field.
#
import pyrfa

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.acquireSession("Session1")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()
p.setView("RDNDISPLAY,TRDPRC_1,22,25")
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
