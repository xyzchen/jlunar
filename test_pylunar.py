#!/usr/bin/env python3
#-*- coding:utf-8 -*-

from __future__ import print_function

import pylunar

#打印日历
def print_dict(lunardate):
    for k in lunardate:
        print("\033[32m{}\033[0m:{}".format(k, lunardate[k]))

#----------------------------
# 测试
#----------------------------
print(pylunar.__doc__)

print("\n----------- pylunar.__dict__ --------\n")
print_dict(pylunar.__dict__)
#print(json.dumps(pylunar.__dict__, indent=4, sort_keys=True, ensure_ascii=False))


print("\n--------dir(pylunar)------------------\n")
print("\n".join(dir(pylunar)))


print("\n--------vars(pylunar)------------------\n")
print_dict(vars(pylunar))


print("\n-------- pylunar.get_todaystring --------\n")
print(pylunar.get_todaystring())


print("\n-------- pylunar.get_today --------\n")
today = pylunar.get_today()
print_dict(today)


print("\n-------- pylunar.get_lunardate(2013, 4, 9) --------\n")
data = pylunar.get_lunardate(2013, 4, 9)
print_dict(data)

print("\n ------ 测试今天前后的10天 ----------------\n")
offset = pylunar.get_offset_solardays(today['year'], today['month'], today['day'])
for i in range(offset-9, offset+10):
	lday = pylunar.get_date_from_offsetdays(i)
	print_dict(lday)
	print("")
