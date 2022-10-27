#!/usr/bin/gawk -f

# This file should be put under /root/demo-ncap/snmpd/scripts/.

{
	# Skip the first two lines
	if (NR <= 2) {
		print $0
	} else {
		if ($6 ~ /Response/) {
			# Highlight SNMP response messages
			color = 32
			# Discard the native timestamp and instead use a universal way in our application
			$1 = ""
			print strftime("%Y/%m/%d %H:%M:%S")"\033[01;"color"m"$0"\033[00m"; fflush()
		} else if ($6 ~ /Request/) {
			# Highlight SNMP request messages
			color = 34
			$1 = ""
			print strftime("%Y/%m/%d %H:%M:%S")"\033[01;"color"m"$0"\033[00m"; fflush()
		} else if ($2 == "packets") {
			# Highlight the summary right before quitting `tcpdump' program
			color = 33
			print "\033[01;"color"m"$0"\033[00m"; fflush()
		} else {
			print $0; fflush()
		}
	}
}
