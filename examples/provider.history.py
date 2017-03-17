#!/usr/bin/python
#
# Non-interactive (full cached) publisher for history domain
# HISTORY provides full indexed time series
#
import pyrfa
import time
import random
exec(open('./utils/every.py').read())

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.setDebugMode(True)
p.acquireSession("Session4")
p.createOMMProvider()
p.login()
p.dictionaryRequest()

IMAGES = {'RIC':'tANZ.AX', 'TRDPRC_1':40.124, 'SALTIM':'now', 'TRADE_ID':'123456789', 'BID_ORD_ID':'5307FBL20AL7B', 'ASK_ORD_ID':'5307FBL20BN8A'},
IMAGES += {'RIC':'tBHP.AX', 'TRDPRC_1':25.234, 'SALTIM':'now', 'TRADE_ID':'987654321', 'BID_ORD_ID':'5307FBL2XXXXB', 'ASK_ORD_ID':'5307FBL2YYYYA'},
p.historySubmit(IMAGES)

count = 0
def update():
    global count
    if count < 100:
        UPDATE = {'RIC':'tANZ.AX', 'TRDPRC_1':40.130, 'SALTIM':'now', 'TRADE_ID':'123456789', 'BID_ORD_ID':'5307FBL20AL7B', 'ASK_ORD_ID':'5307FBL20BN8A'}
        p.historySubmit(UPDATE)
        count += 1

loop = every(1, update)

try:
    while True:
        time.sleep(0.1)
except KeyboardInterrupt:
    loop.stop()
