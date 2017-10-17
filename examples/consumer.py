#!/usr/bin/python
#
# Subscribe to multiple RICs, using local dictionary, debug mode is manually off.
# marketPriceCloseAllRequest is called but not neccessary.
#
# REFRESH/IMAGE:
#     {'MTYPE':'REFRESH','RIC':'EUR=','SERVICE':'IDN_RDF_SDS'}
#     {'MTYPE':'IMAGE','RIC':'EUR=','SERVICE':'IDN_RDF_SDS','ASK':1.3712,'BID':1.3709,...}
#
# UPDATE:
#     {'SERVICE':'NIP','RIC':'EUR=','MTYPE':'UPDATE','ASK':'0.98','ASK_TIME':'13:51:32:120','BID':'0.988','BID_NET_CH':'0.0041'}
#
# STATUS:
#     {'STREAM_STATE':'Open','MTYPE':'STATUS','SERVICE':'IDN_RDF_SDS','RIC':'EUR=','DATA_STATE':'Suspect','TEXT':'Source unavailable... will recover when source is up'}
#
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
                fid = p.getFieldID(k)
                if type(v) is float:
                    print("%20s %g" % (k+' ('+str(fid)+')',v))
                else:
                    print("%20s %s" % (k+' ('+str(fid)+')',v))
            print("")

p.marketPriceCloseAllRequest()
