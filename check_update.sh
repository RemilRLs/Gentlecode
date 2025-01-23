#!/bin/bash

IP_FILE_INFECTED="infected_ip.txt"

if [ ! -f "$IP_FILE_INFECTED" ]; then
    echo "[X] - File $IP_FILE_INFECTED not found"
    exit 1
fi




UPDATE_DONE_COUNT=0
TOTAL_MACHINES=$(wc -l < "$IP_FILE_INFECTED") 

# https://sentry.io/answers/determine-whether-a-file-exists-or-not-in-bash/

echo "[*] - Start update for $TOTAL_MACHINES machines"

# I'm checking if everyone have done the update 
while read -r ip; do 
    echo "[+] - Checking if machine $ip has already done the update"
    if ssh root@$ip "test -f /tmp/.update_done > /dev/null 2>&1"; then 
        echo "[+] - Machine $ip have already done the update" 
        UPDATE_DONE_COUNT=$((UPDATE_DONE_COUNT+1))
    else 
        echo "[X] - The machine $ip didn't do the update yet"
    fi 
done < "$IP_FILE_INFECTED"


if [ "$UPDATE_DONE_COUNT" -eq "$TOTAL_MACHINES" ]; then 
    echo "[*] - All machines have done the update"
else 
    echo "[X] - Not all machines have done the update"
fi
