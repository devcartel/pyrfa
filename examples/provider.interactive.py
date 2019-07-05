#!/usr/bin/python
#
# Interactive (server) publisher for market price domain
#
import pyrfa
import time
import random
exec(open('./every.py').read())

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.acquireSession("Session5")
p.createOMMProvider()
p.dictionaryRequest()

def update():
    unifiedWatchList = []
    sessions = p.getClientSessions().split(' ')
    for session in sessions:
        clientWatchList = p.getClientWatchList(session).split(' ')
        unifiedWatchList = list(set().union(unifiedWatchList,clientWatchList))
        print('[sessions] '+ session + ' ' + str(clientWatchList))
    print('[unifiedWatchList] ' + str(unifiedWatchList))
    for ric in unifiedWatchList:
        price = round(round(4 + random.random(),3),3)
        p.marketPriceSubmit({'RIC':ric,'TRDPRC_1':price,'TIMACT':'now'})

loop = every(5, update)

try:
    while True:
        time.sleep(0.01)
        updates = p.dispatchEventQueue()
        if updates:
            for u in updates:
                print('[data] ' + str(u))
                if u['MTYPE'] in ['LOGIN']:
                    # simple authentication
                    if not u['USERNAME'] in ['pyrfa']:
                        p.logoutSubmit(u['SESSIONID'])
except KeyboardInterrupt:
    loop.stop()
    p.logoutAllSubmit()
