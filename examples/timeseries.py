#!/usr/bin/python
#
# - get timeseries from TS1 service over IDN using RFA TS1 decoder
# - timeseries period options litterally are: daily, weekly or monthly
# - number of fileds are varied by different markets
# - setTimeSeriesPeriod is optional (daily)
# - setTimeSeriesMaxRecords is optional (10)
#
# series format:
#     ('yyyy/mm/dd,close,open,high,low,volume,vwap,pe', '...' , '...' )
# or
#     ('yyyy/mm/dd,Market holiday')
#
import threading
import pyrfa

p = pyrfa.Pyrfa()
p.createConfigDb("./pyrfa.cfg")
p.setDebugMode(False)
p.acquireSession("Session1")
p.createOMMConsumer()
p.login()
p.directoryRequest()
p.dictionaryRequest()

ric = "CHK.N"
period = "daily"
maxrecords = 20

p.setTimeSeriesPeriod(period)
p.setTimeSeriesMaxRecords(maxrecords)
timeseries = p.getTimeSeries(ric)
print(timeseries)
print("\n\n############## " + ric + " " + period + " (" + str(len(timeseries)) +  " records) " + "##############")
for record in timeseries:
    print(record)       