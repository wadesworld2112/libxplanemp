Documentation of the X-IvAp config file (X-IvAp.conf) options
=============================================================


Section [PREFERENCES]
=====================
Holds global preferences (the stuff you can toggle: weather, voice, ...)


MULTIPLAYER=1    possible values: 0, 1 (0 = no, 1 = yes)
                   enable/disable multiplayer (set via user interface)

WEATHER=1        possible values: 0, 1 (0 = no, 1 = yes)
                   enable/disable weather (set via user interface)

COUPLING=0       possible values: 0, 1 (0 = no, 1 = yes)
                   enable/disable radio coupling (set via user interface)

VOICE=1          possible values: 0, 1 (0 = no, 1 = yes)                   enable/disable teamspeak (set via user interface)

DARK=1           possible values: 0, 1 (0 = no, 1 = yes)
                   enable/disable dark console (set via user interface)



Section [ACCOUNT]
=====================
Holds user credentials (Name, VID, Password, ...)

HIDEADM=1        possible values: 0, 1 (0 = no, 1 = yes)
                   enable/disable supervisor hiding on IVAN (set on connect window)
PORT=6809        FSD server port - you should leave it at this (set on connect window)
SERVER=62.141.43.246
                 IP of last server used to connect to. X-IVAP will use this IP to
                 automatically preselect that server when you connect for the next time
BASE=LOIH        Home base of Pilot (4 letter ICAO code, set on connect window)
REALNAME=Martin Domig
                 Now that shouldn't be too hard to gues... (set on connect window)
PASSWORD=A6Ad2bsX
                 IVAN password used for connection. Note: this is case-sensitive! (set on connect window)
VID=156618
                 IVAO ID used for connection (set on connect window)
CALLSIGN=OEDMG
                 Last callsign used to connect (set on connect window)


Section [P2P]
=====================
Holds peer2peer configuration options.
NOTE: These options are NOT tuneable via the user interface (as of May 21 2006),
and this is intentional. You should NOT change these settings unless you know what
you are doing.
Please refer to the peer2peer documentation for more information.

ENABLE=1         possible values: 0, 1 (0 = no, 1 = yes)
                   if 1, p2p is enabled (default)

FORCED_PORT=0    possible values: 0 - 65535
                   if set to anything but zero, this port will be used for p2p connections.
                   intended use: configure port forwarding on your router/modem device.
                   if you are behind a NAT/firewall that does not allow you to use active p2p,
                   you can force xivap to use a predefined port with this setting. If you then
                   configure port forwarding for the SAME port to your computer (the one you run
                   xivap on), you should be able to use active p2p. Let me know if this works or not.

MAXSENDBPS=1024  maximum data rate (bytes per second) used to send p2p position updates to other
                 players. NOTE: this is (more or less) a guaranteed MAXIMUM, meaning that xivap will
                 never use more. usually it will use a lot less than your maximum (~1/3rd).

MAXRECVBPS=5000  maximum data rate (bytes per second) allowed for incoming p2p position updates from
                 other peers. same as above applies.

STUNSERVER=stun.ivao.aero
                 This server is used to determine your NAT device type. Upon connecting to IVAN, xivap
                 will send a couple of probes to that server to determine your NAT type. 

