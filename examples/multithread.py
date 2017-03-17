#!/usr/bin/python
#
# Multithread publisher and subscriber implementation example
#
import time
import threading
import random
import pyrfa

def publish():
    p = pyrfa.Pyrfa()
    p.createConfigDb("./pyrfa.cfg")
    p.acquireSession("Session4")
    p.createOMMProvider()
    p.login()
    p.dictionaryRequest()
    IMAGES = {'RIC':'EUR=', 'RDNDISPLAY':200, 'RDN_EXCHID':155, 'BID':0.988, 'ASK':0.999, 'DIVPAYDATE':'20110623'},
    IMAGES = {'RIC':'JPY=', 'RDNDISPLAY':200, 'RDN_EXCHID':'NAS', 'OFFCL_CODE':'isin1234XYZ', 'BID':4.23, 'DIVPAYDATE':'20110623', 'OPEN_TIME':'09:00:01.000'},
    p.marketPriceSubmit(IMAGES)
    vol = 1000
    while not end.is_set():
        time.sleep(1)
        vol += 1
        price = round(round(4 + random.random(),3),3)
        UPDATES = {'RIC':'EUR=', 'ACVOL_1':vol, 'TRDPRC_1':price},
        UPDATES += {'RIC':'JPY=', 'BID_NET_CH':0.0041, 'BID':0.988, 'ASK':0.999,'ASK_TIME':'now'},
        p.marketPriceSubmit(UPDATES)
    
def subscribe():
    p = pyrfa.Pyrfa()
    p.createConfigDb("./pyrfa.cfg")
    p.acquireSession("Session3")
    p.createOMMConsumer()
    p.login()
    p.directoryRequest()
    p.dictionaryRequest()
    p.marketPriceRequest("EUR=,JPY=")
    while not end.is_set():
        time.sleep(0.01)
        for u in p.dispatchEventQueue():
            print("\n" + u['SERVICE'] + " - " + u['RIC'])
            for k,v in u.items():
                print("%15s %g" % (k,v) if type(v) is float else "%15s %s" % (k,v))
    p.marketPriceCloseAllRequest()


if __name__ == "__main__":
    try:
        try:
            end = threading.Event()

            t_publish = threading.Thread(target=publish)
            t_publish.start()

            #wait 10s until the service is up
            time.sleep(10)

            t_subscribe = threading.Thread(target=subscribe)
            t_subscribe.start()

            while True:
                time.sleep(0.01)
        finally:
            end.set()
    except:
        pass