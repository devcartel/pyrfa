#!/usr/bin/python
#
# OMM Posting (off-stream) leverages on consumer login channel to contribute aka. "post" data
# up to ADH/ADS cache. Posted service must be up.
#
import pyrfa
import time
import random
exec(open('./every.py').read())

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.setDebugMode(True)
p.acquireSession("Session3")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()

# Post an update every 5 secs.
def post():
    global p
    price = round(round(4 + random.random(),3),3)
    p.marketPricePost({'RIC':'TRI.N', 'TRDPRC_1':price,'TIMACT':'now'})

loop = every(5, post)

try:
    while True:
        time.sleep(0.1)
except KeyboardInterrupt:
    loop.stop()
