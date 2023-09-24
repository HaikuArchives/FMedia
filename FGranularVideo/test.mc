#!/boot/home/config/bin/mc -s

ins FVideo vid
ins FDifference diff
ins FGranularVideo granule
ins FVideoDisplay disp

con vid diff
con diff granule
con granule disp

slave vout time

set diff Rough 1

control diff
control granule

start disp
start granule
start diff
start vid
