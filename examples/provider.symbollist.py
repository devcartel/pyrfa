#!/usr/bin/python
#
# Non-interactive (full cached) publisher for market price domain
#
import pyrfa
import time

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.setDebugMode(True)
p.acquireSession("Session4")
p.createOMMProvider()
p.login()
p.dictionaryRequest()

# add RICs to symbollist
SYMBOLLIST = {'ACTION':'ADD', 'RIC':'0#BMD', 'KEY':'FCPO', 'PROD_PERM':10, 'PROV_SYMB':'MY439483'},
SYMBOLLIST += {'ACTION':'ADD', 'RIC':'0#BMD', 'KEY':'FKLI'},
SYMBOLLIST += {'ACTION':'ADD', 'RIC':'0#BMD', 'KEY':'FKLQ'},
SYMBOLLIST += {'ACTION':'ADD', 'RIC':'0#BMD', 'KEY':'FKLT'},
SYMBOLLIST += {'ACTION':'ADD', 'RIC':'0#BMD', 'KEY':'FKLP'},
SYMBOLLIST += {'ACTION':'ADD', 'RIC':'0#BMD', 'KEY':'FKLL'},
SYMBOLLIST += {'ACTION':'ADD', 'RIC':'0#BMD', 'KEY':'FKLM'},

p.symbolListSubmit(SYMBOLLIST)

#update symbollist
SYMBOLLIST_UPDATE = {'ACTION':'UPDATE', 'RIC':'0#BMD', 'KEY':'FKLI', 'PROD_PERM':10, 'PROV_SYMB':'MY1244221'}
p.symbolListSubmit(SYMBOLLIST_UPDATE)

time.sleep(40)

#delete symbollist
SYMBOLLIST_DELETE = {'ACTION':'DELETE', 'RIC':'0#BMD', 'KEY':'FCPO'}
p.symbolListSubmit(SYMBOLLIST_DELETE)

while True:
    time.sleep(0.1)
 