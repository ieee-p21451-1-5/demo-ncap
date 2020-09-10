#! /usr/bin/gawk -f

# This file should be put under /root/snmpd/scripts/.

{
	if (NR == 1) {
		# Skip the first line
		print $0
	} else {
		if ($2 == "TERM") {
			# Highlight the summary right before quitting `snmpd' program
			color = 33
		} else {
			# Highlight inward SNMP packets 
			color = 34
		print strftime("%Y/%m/%d %H:%M:%S"), "\033[01;"color"m"$0"\033[00m"; fflush()
		}
	}
}
