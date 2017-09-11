#!/usr/bin/python
#
# Decoding a legacy chain ric
#
import pyrfa

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.acquireSession("Session1")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()
p.setInteractionType("snapshot")

def snapshotRequest(chainRIC):
    p.marketPriceRequest(chainRIC)
    snapshots = p.dispatchEventQueue(1000)
    if snapshots:
        for snapshot in snapshots:
            if snapshot['MTYPE'] == 'IMAGE':
                return snapshot
    return ()

fids = ['LINK_1', 'LINK_2', 'LINK_3', 'LINK_4', 'LINK_5', 'LINK_6', 'LINK_7', 'LINK_8',
        'LINK_9', 'LINK_10', 'LINK_11', 'LINK_12', 'LINK_13', 'LINK_14', 
        'LONGLINK1', 'LONGLINK2', 'LONGLINK3', 'LONGLINK4', 'LONGLINK5', 'LONGLINK6', 'LONGLINK7',
        'LONGLINK8', 'LONGLINK9', 'LONGLINK10', 'LONGLINK11', 'LONGLINK12', 'LONGLINK13', 'LONGLINK14',
        'BR_LINK1', 'BR_LINK2', 'BR_LINK3', 'BR_LINK4', 'BR_LINK5', 'BR_LINK6', 'BR_LINK7', 'BR_LINK8',
        'BR_LINK9', 'BR_LINK10', 'BR_LINK11', 'BR_LINK12', 'BR_LINK13', 'BR_LINK14']
def expandChainRIC(chainRIC):
    expanded = []
    done = False
    snapshot = snapshotRequest(chainRIC)
    while not done:
        if not snapshot:
            break
        for fid in fids:
            if snapshot.has_key(fid) and snapshot[fid]:
                expanded.append(snapshot[fid])
        if snapshot.has_key('NEXT_LR') and snapshot['NEXT_LR']:
            snapshot = snapshotRequest(snapshot['NEXT_LR'])
        elif snapshot.has_key('LONGNEXTLR') and snapshot['LONGNEXTLR']:
            snapshot = snapshotRequest(snapshot['LONGNEXTLR'])
        elif snapshot.has_key('BR_NEXTLR') and snapshot['BR_NEXTLR']:
            snapshot = snapshotRequest(snapshot['BR_NEXTLR'])
        else:
            done = True
    return expanded

rics = expandChainRIC("0#.FTSE")
print(rics)
