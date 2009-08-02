#!/bin/bash

date >> watchdog.log
echo "server start..." >> watchdog.log

./server -h 84.16.234.250 -S 80.237.141.62 > /dev/null

