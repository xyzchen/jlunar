#!/usr/bin/env python
from __future__ import print_function

import pylunar

def printdate(lunardate):
    for k in lunardate:
        print("{}:{}".format(k, lunardate[k]))

#------------------------------------
data = pylunar.get_lunardate(2013, 4, 9)
print(data)

print("\n----------------------------------\n")

for i in data:
    print("date[%s]=%s" %(i, data[i]))

print("\n----------------------------------\n")

for (k,v) in data.items():
    print("date[%s]=%s" %(k, v))

print("\n----------------------------------\n")

today = pylunar.get_today()
printdate(today)

print("\n----------------------------------\n")

print(pylunar.get_todaystring())

print("\n----------------------------------\n")
print(dir(pylunar))

funcs = vars(pylunar)
for i in funcs:
    print("%s : %s" % (i, funcs[i]))
