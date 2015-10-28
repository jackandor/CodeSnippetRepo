import datetime

d1 = datetime.datetime(2015, 10, 14)
d2 = datetime.datetime(2015, 8, 13)

print((d1 - d2).days)

now = datetime.datetime.now()
start_time = time.mktime(now.timetuple())