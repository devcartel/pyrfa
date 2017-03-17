#!/usr/bin/python
#
# Non-interactive (full cached) publisher for market price domain
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

# add order book
ORDERS = {'ACTION':'ADD', 'RIC':'ANZ.AX', 'KEY':'538993C200035057B', 'ORDER_PRC': '20.260', 'ORDER_SIZE':50, 'ORDER_SIDE':'BID', 'SEQNUM_QT':2744, 'EX_ORD_TYP':0, 'CHG_REAS':6,'ORDER_TONE':''},
ORDERS += {'ACTION':'ADD', 'RIC':'ANZ.AX', 'KEY':'538993C200083483B', 'ORDER_PRC': '20.280', 'ORDER_SIZE':100, 'ORDER_SIDE':'ASK', 'SEQNUM_QT':2744, 'EX_ORD_TYP':0, 'CHG_REAS':6,'ORDER_TONE':''},
ORDERS += {'ACTION':'ADD', 'RIC':'ANZ.AX', 'KEY':'538993C200083466B', 'ORDER_PRC': '20.280', 'ORDER_SIZE':70, 'ORDER_SIDE':'ASK', 'SEQNUM_QT':2744, 'EX_ORD_TYP':0, 'CHG_REAS':6,'ORDER_TONE':''},
p.marketByOrderSubmit(ORDERS)

# delete an order
p.marketByOrderSubmit({'ACTION':'DELETE', 'RIC':'ANZ.AX', 'KEY':'538993C200083466B'})

# add market depths
DEPTHS = {'ACTION':'ADD', 'RIC':'ANZ.CHA','KEY':'201000B','ORDER_PRC': '20.1000', 'ORDER_SIDE':'BID', 'ORDER_SIZE':'1300', 'NO_ORD':13, 'QUOTIM_MS':16987567,'ORDER_TONE':''},
DEPTHS += {'ACTION':'ADD', 'RIC':'ANZ.CHA','KEY':'211000B','ORDER_PRC': '20.2000', 'ORDER_SIDE':'BID', 'ORDER_SIZE':'100', 'NO_ORD':13, 'QUOTIM_MS':16987567,'ORDER_TONE':''},
DEPTHS += {'ACTION':'ADD', 'RIC':'ANZ.CHA','KEY':'210001B','ORDER_PRC': '21.0000', 'ORDER_SIDE':'ASK', 'ORDER_SIZE':'500', 'NO_ORD':13, 'QUOTIM_MS':16987567,'ORDER_TONE':''},
p.marketByPriceSubmit(DEPTHS)

# delete a market depth
p.marketByPriceSubmit({'ACTION':'DELETE', 'RIC':'ANZ.CHA', 'KEY':'210001B'})

# updates every x secs.
size = 100
no_ord = 14
def update():
    global size
    global no_ord
    size += 100
    no_ord += 1
    price = round(20 + round(random.random(),3),3)
    #update order
    ORDERS_UPDATE = {'ACTION':'UPDATE', 'RIC':'ANZ.AX', 'KEY':'538993C200083483B', 'ORDER_PRC': price, 'ORDER_SIZE':size, 'ORDER_SIDE':'BID', 'SEQNUM_QT':2745, 'EX_ORD_TYP':0, 'CHG_REAS':6,'ORDER_TONE':''}
    p.marketByOrderSubmit(ORDERS_UPDATE)
    
    #update depth
    DEPTHS_UPDATE = {'ACTION':'UPDATE', 'RIC':'ANZ.CHA','KEY':'201000B','ORDER_PRC': price , 'ORDER_SIDE':'BID', 'ORDER_SIZE':size, 'NO_ORD':no_ord, 'QUOTIM_MS':16987567,'ORDER_TONE':''}
    p.marketByPriceSubmit(DEPTHS_UPDATE)
    
loop = every(1, update)

try:
    while True:
        time.sleep(0.1)
except KeyboardInterrupt:
    loop.stop()
 