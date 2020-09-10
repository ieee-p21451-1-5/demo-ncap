#! /bin/bash

# This file should be put under /root/demo-ncap/snmpd/scripts/.

cd /root/demo-ncap/snmpd/
tcpdump -l -i any 'udp port 161' 2>&1 | \
	gawk -f ./scripts/tcpdump-output-process.awk > \
	./logs/snmpd-traffic-log-$(date +'%Y%m%d%H%M%S')
