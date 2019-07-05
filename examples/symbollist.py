#!/usr/bin/python
#
# Request for symbolList. Currently RFA only support refresh messages
# for symbolList. Hence, polling is required and symbolListRequest is called
# internally by getSymbolList.
#
# IMAGE/REFRESH:
#   ({'MTYPE':'REFRESH','RIC':'0#BMD','SERVICE':'NIP'},
#    {'ACTION':'ADD','MTYPE':'IMAGE','SERVICE':'NIP','RIC':'0#BMD','KEY':'FKLI'},
#    {'ACTION':'ADD','MTYPE':'IMAGE','SERVICE':'NIP','RIC':'0#BMD','KEY':'FKLL'},
#    {'ACTION':'ADD','MTYPE':'IMAGE','SERVICE':'NIP','RIC':'0#BMD','KEY':'FKLM'})
#
import pyrfa

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.acquireSession("Session3")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()
RIC = "0#BMD"
symbolList = p.getSymbolList(RIC)
print("\n=======\n" + RIC + "\n=======")
print(symbolList.replace(" ","\n"))
