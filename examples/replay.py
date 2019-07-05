#!/usr/bin/python
#
# Interactive provider for Replay Data Service
#
import pyrfa
import time
import random
import getopt
import sys
import ast
exec(open('./every.py').read())

class Application():
    def __init__(self, config, session):
        if session is None:
            session = "Session1"
        self.p = pyrfa.Pyrfa()
        self.p.createConfigDb(config)
        self.p.acquireSession(session)
        self.p.createOMMProvider()
        self.p.dictionaryRequest()
        self.connection = self.p.getConfigDb("\\Default\\Sessions\\" + session + "\\connectionList")
        self.publishInterval = self.p.getConfigDb("\\Default\\Connections\\" + self.connection + "\\publishInterval")
        self.userName = self.p.getConfigDb("\\Default\\Connections\\" + self.connection + "\\userName")
        self.dataFile = self.p.getConfigDb("\\Default\\Connections\\" + self.connection + "\\dataFile")
        self.records = None
        self.recordNumSent = 0
        self.offset = 0
        self.loop = None
        self.snapshots = {}
        
        # Read data from file
        with open(self.dataFile, 'r') as f:
            self.records = f.readlines()
            self.offset = int(time.time() * 1000 ) - int(self.records[0].split(':', 1)[0])
        f.close()

    def processLogin(self, event):
        if event['USERNAME'] != self.userName:
            self.p.logoutSubmit(event['SESSIONID'])
        self.p.logWarning('processLogin - username ' + event['USERNAME'] + ' not authorized.')

    def processRequest(self, event):
        try:
            if event['RIC'] in self.snapshots:
                self.p.logInfo('processRequest - sending snapshot ' + str(self.snapshots[event['RIC']]))
                self.p.marketPriceSubmit(self.snapshots[event['RIC']])
            else:
                self.p.closeSubmit(event['RIC'])
                self.p.logWarning('processRequest - item not in cache (' + event['RIC'] + ')')
        except Exception as e:
            self.p.logError('processRequest - ' + str(e))

    def sendUpdate(self, useTimestamp=False):
        if self.records is None:
            return
        try:
            idx = self.recordNumSent % len(self.records)
            record = self.records[idx]
            timestamp = int(record.split(':', 1)[0])
        
            if useTimestamp:
                currentTime = time.time() * 1000
                if currentTime < (timestamp + self.offset):
                    return
            
            data = ast.literal_eval(record.split(':', 1)[1].replace("\\xe2\\x87\\xa7","\\xde").replace("\\xe2\\x87\\xa9","\\xfe"))
            self.recordNumSent += 1
            if idx == 0:
                self.offset = int(time.time() * 1000 ) - int(self.records[0].split(':', 1)[0])
            
            unifiedWatchList = []
            sessions = self.p.getClientSessions().split(' ')
            for session in sessions:
                clientWatchList = self.p.getClientWatchList(session).split(' ')
                unifiedWatchList = list(set().union(unifiedWatchList,clientWatchList))
            for ric in unifiedWatchList:
                if data['RIC'] == ric:
                    self.p.marketPriceSubmit(data)
            
            # Save snapshot
            if data['RIC'] not in self.snapshots:
                self.snapshots[data['RIC']] = {}
            del data['MTYPE']
            del data['SERVICE']
            self.snapshots[data['RIC']].update(data)
        except Exception as e:
            self.p.logError('sendUpdate - ' + str(e))
    
    def onExit(self):
        if self.loop is not None:
            self.loop.stop()
#        self.p.logoutAllSubmit()
        
##########################
# Main
##########################
def usage():
    print("python replay.py -c config [-s session]")

if __name__ == '__main__':
    if len(sys.argv) <= 2:
        usage()
        sys.exit(0)
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hc:s:", "help")
    except getopt.GetoptError as err:
        print(str(err))
        usage()
        sys.exit(2)
    session = None
    for o, a in opts:
        if o in ("-c"):
            config = a
        elif o in ("-s"):
            session = a
        elif o in ("-h", "--help"):
            usage()
            sys.exit(0)
        else:
            assert False, "unhandled option"

    app = Application(config, session)
    # If use publishInterval (seconds), or use timestamp (ms) in data file
    if app.publishInterval:
        app.loop = every(float(app.publishInterval), app.sendUpdate)
    try:
        while True:
            time.sleep(0.01)
            events = app.p.dispatchEventQueue()
            if events:
                for event in events:
                    app.p.logInfo('event - ' + str(event))
                    if event['MTYPE'] == 'LOGIN':
                        app.processLogin(event)
                    elif event['MTYPE'] == 'REQUEST':
                        app.processRequest(event)
            if app.loop is None:
                app.sendUpdate(True)
    except KeyboardInterrupt:
        app.onExit()
