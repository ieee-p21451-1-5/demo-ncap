#!/bin/bash

# This file should be put under /root/demo-ncap/demo-ncap/sync/.

ntpstat

while
    printf "Not synced yet...\n"
    sleep 1s
    ntpstat
    (( $? ))
do
    :
done

printf "Synced.\n"
