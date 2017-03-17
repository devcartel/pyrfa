#!/usr/bin/python
import pyrfa

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.setDebugMode(True)
p.acquireSession("Session1")
p.createOMMConsumer()

p.login()

p.directoryRequest()
