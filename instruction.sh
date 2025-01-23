#!/bin/bash

# Detail of process
ps aux >> datav

# Use of memory
free -m >> datav

# Use of CPU 
top -b -n 1 | head -n 10 >> datav

# Detail of CPU
lscpu >> datav

echo "Wii" >> datav