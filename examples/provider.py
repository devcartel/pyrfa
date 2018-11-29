#!/usr/bin/python
#
# Non-interactive (full cached) publisher for market price domain
#
import pyrfa
import random
import time
exec(open('./every.py').read())

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.setDebugMode(True)
p.acquireSession("Session4")
p.createOMMProvider()
p.login()
p.dictionaryRequest()

IMAGES = {'RIC':'EUR=', 'RDNDISPLAY':200, 'RDN_EXCHID':155, 'BID':0.988, 'ASK':0.999, 'DIVPAYDATE':'20110623'},
IMAGES += {'RIC':'C.N', 'RDNDISPLAY':200, 'RDN_EXCHID':'NAS', 'OFFCL_CODE':'isin1234XYZ', 'BID':4.23, 'DIVPAYDATE':'20110623', 'OPEN_TIME':'09:00:01.000'},
p.marketPriceSubmit(IMAGES)

# updates every x secs.
vol = 1000
def update():
    global vol
    vol += 1
    price = round(round(4 + random.random(),3),3)
    UPDATES = {'RIC':'C.N', 'ACVOL_1':vol, 'TRDPRC_1':price},
    UPDATES += {'RIC':'EUR=', 'BID_NET_CH':0.0041, 'BID':0.988, 'ASK':0.999,'ASK_TIME':'now'},
    p.marketPriceSubmit(UPDATES)

loop = every(1, update)

try:
    while True:
        time.sleep(0.1)
except KeyboardInterrupt:
    loop.stop()
