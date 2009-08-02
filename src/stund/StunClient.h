#ifndef _STUN_CLIENT_H
#define _STUN_CLIENT_H

#include <string>

#ifndef NO_PTYPES
#include "pinet.h"

USING_PTYPES
#endif

// stun client includes
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>   

#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#endif

#include "udp.h"
#include "stun.h"

namespace Stun {

enum StunType {
	StunError		= -1,
	StunOpen		= 1,
	StunIndependentFilter	= 2,
	StunDependentFilter	= 3,
	StunPortDependedFilter	= 4,
	StunDependentMapping	= 5,
	StunFirewall		= 6,
	StunBlocked		= 7
};


class StunClient {
public:
	StunClient(char *server = NULL, int port = 0);

	StunType RunTest();
	StunType Type() const { return _type; };
	bool Hairpin() const { return _hairpin; };
	bool PresPort() const { return _presPort; };
	void SetServer(const char *server);

#ifdef NO_PTYPES
	long PublicIp() { return ntohl(_sAddr.addr); };
#else
	const ipaddress PublicIp() { return ipaddress(ntohl(_sAddr.addr)); };
	ipmsgserver& udpSocket() { return _udpsocket; };
#endif
	int PublicPort() { return (_fixedPort ? _srcPort : _sAddr.port); };

	int Port() const { return _srcPort; };
	void SetPort(int port);

	void SendKeepalive();

	const std::string& error() const { return _error; };

private:
	StunAddress4 _stunServerAddr;
	int _srcPort;
	StunAddress4 _sAddr;

	std::string _error, _server;

	bool _presPort, _hairpin;
	StunType _type;

#ifndef NO_PTYPES
	ipmsgserver _udpsocket;
	ipaddress _myInternalIp;
#endif
	int _myInternalPort;

	int _ipmsgsrvbind;
	int _sock2;
	bool _haveSocket;
	bool _fixedPort;
};

} // namespace

#endif
