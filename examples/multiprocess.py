#!/usr/bin/python
#
# Interactive (server) publisher for market price domain
#
import time
import multiprocessing
import random
import pyrfa

def publish():
    try:
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
        while True:
            time.sleep(1)
            vol += 1
            price = round(round(4 + random.random(),3),3)
            UPDATES = {'RIC':'EUR=', 'ACVOL_1':vol, 'TRDPRC_1':price},
            UPDATES += {'RIC':'JPY=', 'BID_NET_CH':0.0041, 'BID':0.988, 'ASK':0.999,'ASK_TIME':'now'},
            p.marketPriceSubmit(UPDATES)
    except:
        pass
    
def subscribe():
    try:
        try:
            p = pyrfa.Pyrfa()
            p.createConfigDb("./pyrfa.cfg")
            p.acquireSession("Session3")
            p.createOMMConsumer()
            p.login()
            p.directoryRequest()
            p.dictionaryRequest()
            p.marketPriceRequest("EUR=,JPY=")
            while True:
                time.sleep(0.01)
                for u in p.dispatchEventQueue():
                    print("\n" + u['SERVICE'] + " - " + u['RIC'])
                    for k,v in u.items():
                        print("%15s %g" % (k,v) if type(v) is float else "%15s %s" % (k,v))
        finally:
            p.marketPriceCloseAllRequest()
    except:
        pass

if __name__ == "__main__":
    try:
        try:
            t_publish = multiprocessing.Process(target=publish)
            t_publish.start()

            #wait 10s until the service is up
            time.sleep(10)

            t_subscribe = multiprocessing.Process(target=subscribe)
            t_subscribe.start()

            while True:
                time.sleep(0.01)
        finally:
            time.sleep(1)
            for process in multiprocessing.active_children():
                process.terminate()
    except:
        pass 
