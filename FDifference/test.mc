#!/boot/home/config/bin/mc -s

ins FVideo vid
ins FDifference node
con vid node
con node vout

slave vout time

control node

start vout
start node
start vid
