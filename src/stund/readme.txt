STUN server setup readme

STUN is a small service that listens on UDP ports 3478 and 3479 for incoming requests,
which it answers without any further action. No data files or configuration database
is needed, traffic will be low.
The server does NOT require root privileges.


----------------------------------------------------------
STUN needs *2 public IPs* in order to work correctly.
----------------------------------------------------------


There are 2 possibilities:

1. Run it on one machine with 2 IP addresses (can be interface aliases)
2. Run it on two machines, but tell stun where the other server is located


1. Running STUN on one machine with 2 IPs
----------------------------------------------------------

Start the STUN server like this:
./server -h 10.10.10.1 -a 10.10.10.2

(10.10.10.1 and 10.10.10.2 are the public IPs of the server)


2. Running STUN on two linked machines
----------------------------------------------------------

Assuming you have 2 servers, server A on 10.10.10.1 and server B on 20.20.20.1

On Server A, use:
./server -h 10.10.10.1 -S 20.20.20.1

On Server B, use:
./server -h 20.20.20.1 -S 10.10.10.1



Using the watchdog to put the server into daemon mode
----------------------------------------------------------

watchdog is a small program that puts itself into daemon mode and then executes
whatever you pass as argument in an endless loop. This is recommended, since it
will prevent the server from terminating due to SIGPIPE (and other) signals, and
will restart it should it crash.


Example:

./watchdog startserver.sh

Will execute "startserver.sh" in daemon mode. To start the stun server from there
(and add some logging to see how often it was being restarted), use this shellscript:


#!/bin/bash

date >> watchdog.log
echo "server start..." >> watchdog.log
./server -h 10.10.10.1 -S 20.20.20.1 > /dev/null




Stun command line options
----------------------------------------------------------

STUN server version 0.97
Usage: 
 ./server [-v] [-h] [-h IP_Address] [-a IP_Address] [-p port] [-o port] [-m mediaport]
 
 If the IP addresses of your NIC are 10.0.1.150 and 10.0.1.151, run this program with
    ./server -v  -h 10.0.1.150 -a 10.0.1.151
 STUN servers need two IP addresses and two ports, these can be specified with:
  -h sets the primary IP
  -a sets the secondary IP
  -p sets the primary port and defaults to 3478
  -o sets the secondary port and defaults to 3479
  -b makes the program run in the backgroud
  -m sets up a STERN server starting at port m
  -v runs in verbose mode
 In case you dont have a secondary IP, you can specify a partner server with:
  -S partner server IP
  -P partner server port (defaults to 3478)


--
Martin Domig <martin@domig.net>
