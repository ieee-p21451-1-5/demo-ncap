#! /bin/bash

# This file should be put under /root/snmpd/scripts/.

cd /root/snmpd/
tcpdump -l -i any 'udp port 161' 2>&1 | \
	gawk -f ./scripts/tcpdump_output_process.awk > \
	./logs/snmpd-traffic-log-$(date +'%Y%m%d%H%M%S')
