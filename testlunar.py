#!/usr/bin/python

import pylunar

def printdate(lunardate):
    for k in lunardate:
        print "%s:%s" %(k, lunardate[k])   

#------------------------------------
data = pylunar.get_lunardate(2013, 4, 9)
print data

print"----------------------------------"

for i in data:
    print "date[%s]=%s" %(i, data[i])

print"----------------------------------"

for (k,v) in data.items():
    print "date[%s]=%s" %(k, v)

print"----------------------------------"

today = pylunar.get_today()
printdate(today)

print"----------------------------------"

print pylunar.get_todaystring()

print"----------------------------------"
print dir(pylunar)

funcs = vars(pylunar)
for i in funcs:
    print "%s : %s" % (i, funcs[i])

