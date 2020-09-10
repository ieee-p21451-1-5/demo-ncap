#! /bin/bash

# This file should be put under /root/demo-ncap/snmpd/scripts/.

cd /root/demo-ncap/snmpd
/usr/local/sbin/snmpd -Lo -af | \
	gawk -f ./scripts/snmpd-output-process.awk > \
	./logs/snmpd-log-$(date +'%Y%m%d%H%M%S')
