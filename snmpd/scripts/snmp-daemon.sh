#! /bin/bash

# This file should be put under /root/snmpd/scripts/.

cd /root/snmpd
/usr/local/sbin/snmpd -Lo -af | \
	gawk -f ./scripts/snmpd-output-process.awk > \
	./logs/snmpd-log-$(date +'%Y%m%d%H%M%S')
