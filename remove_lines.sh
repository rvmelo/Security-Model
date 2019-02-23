#!/bin/sh

grep -n "172.31.29.192" alert_data.txt | grep -Eo '^[^:]+'| while read -r line ; do
    echo "Processing $line"
    # your code goes here
    sed -i "${line}d" test.txt
    sed -i "${line}d" alert_data.txt
done
