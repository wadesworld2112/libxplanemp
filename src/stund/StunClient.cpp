#include "StunClient.h"

using namespace Stun;

//#define CLIENT_TEST_MAIN

StunClient::StunClient(char* server, int port)
{
	_error = "";

	if(server == NULL) _server = "";
	else _server = server;

	_presPort = false;
	_hairpin = false;
	_type = StunError;
	_sAddr.addr = 0;
	_srcPort = port;
	_ipmsgsrvbind = 0;
	_haveSocket = false;
	_fixedPort = false;

	_sock2 = INVALID_SOCKET;

	initNetwork();
}

void StunClient::SetServer(const char *server)
{
	_server = server;
}

void StunClient::SetPort(int port)
{
	_srcPort = port;
	if(port != 0)
		_fixedPort = true;
	_type = StunError;
}

StunType StunClient::RunTest()
{
	if(_server == "") {
		_error = "StunClient must be initialized with a server name";
		_type = StunError;
		return _type;
	}
	_stunServerAddr.addr = 0;
	bool ret = stunParseServerName(const_cast<char*>(_server.c_str()), _stunServerAddr);
	if(!ret) {
		_error = _server + " is not a valid server";
		_type = StunError;
		return _type;
	}

	if(_srcPort == 0)
		_srcPort = stunRandomPort();

	_sAddr.port = _srcPort;

	int fd = -1;
#ifndef NO_PTYPES
	if(!_haveSocket)	
		_ipmsgsrvbind = _udpsocket.bindall(_srcPort);
	fd = _udpsocket.get_binding(_ipmsgsrvbind);

	if(fd > 0)
		_haveSocket = true;
	else {
		_type = StunError;
		_error = "Could not bind local socket";
		return _type;
	}
#endif

	NatType stun = stunNatType(_stunServerAddr,
		false, // verbose
		fd, // socket
		&_sock2,
		&_presPort, &_hairpin, _srcPort, &_sAddr);

	// fixed port: assuming a sane port forwarding environment
	// (internal port = external port)
	if(_fixedPort)
		stun = StunTypeIndependentFilter;

	switch(stun) {
	case StunTypeFailure:
		_type = StunError;
		_error = "Some stun error detetecting NAT type";
		break;
	case StunTypeUnknown:
		_type = StunError;
		_error = "Some unknown type error detetecting NAT type";
		break;
	case StunTypeOpen:
		_type = StunOpen;
		_error = "";
		break;
	case StunTypeIndependentFilter:
		_type = StunIndependentFilter;
		_error = "";
		break;
	case StunTypeDependentFilter:
		_type = StunDependentFilter;
		_error = "";
		break;
	case StunTypePortDependedFilter:
		_type = StunPortDependedFilter;
		_error = "";
		break;
	case StunTypeDependentMapping:
		_type = StunDependentMapping;
		_error = "";
		break;
	case StunTypeFirewall:
		_type = StunFirewall;
		_error = "";
		break;
	case StunTypeBlocked:
		_type = StunBlocked;
		_error = "";
		break;
	default:
		_type = StunError;
		_error = "Unkown NAT type";
		break;
	}

	return _type;
}

void StunClient::SendKeepalive()
{
#ifndef NO_PTYPES
	if(_server == "" || _srcPort == 0) {
		_error = "StunClient must be initialized with a server name and a port before sending pings";
		return;
	}

	if(_stunServerAddr.addr == 0) {
		_error = "Cannot ping before testrun";
		return;
	}

	int fd = _udpsocket.get_binding(_ipmsgsrvbind);
	stunSendPing( fd, _stunServerAddr, _srcPort, NULL, false );
#endif
}

#ifdef CLIENT_TEST_MAIN
int main(int argc, char** argv)
{
	StunClient client("gaia.linuxpunk.org");

	switch(client.RunTest()) {
		case StunError:
			std::cout << "error: " << client.error() << " ";
			break;
		case StunOpen:
			std::cout << "open ";
			break;
		case StunIndependentFilter:
			std::cout <<"independent filter ";
			break;
		case StunDependentFilter:
			std::cout << "dependent filter ";
			break;
		case StunPortDependedFilter:
			std::cout << "port dependent filter ";
			break;
		case StunDependentMapping:
			std::cout << "dependent mapping ";
			break;
		case StunFirewall:
			std::cout << "BAD: firewalled ";
			break;
		case StunBlocked:
			std::cout << "BAD, VERY BAD: blocked ";
			break;
		default:
			std::cout << "unknown result ";
			break;
	}

	std::cout << "- ";
	if(client.Hairpin()) std::cout << "hairpin ";
	else std::cout << "no hairpin ";
	std::cout << "- ";
	if(client.PresPort()) std::cout << "preserves port ";
	else std::cout << "does not preserve port ";
	std::cout << std::endl;

#ifndef NO_PTYPES
	std::cout << "My public IP address: " << client.PublicAddr() << std::endl;
#endif
	std::cout << "I should listen on local UDP port " << client.Port() << std::endl;

	switch(client.Type()) {
		case StunOpen:
		case StunIndependentFilter:
			std::cout << "I can use active P2P (others can open connections to me)" << std::endl;
			break;

		case StunDependentFilter:
		case StunPortDependedFilter:
		case StunDependentMapping:
			std::cout << "I can use passive P2P (others can NOT open connections to me)" << std::endl;
			break;

		case StunError:
		case StunBlocked:
		case StunFirewall:
		default:
			std::cout << "I cannot use P2P." << std::endl;
			break;

	}

	std::string dummy;
	std::cout << "Enter some text to exit" << std::endl;
	std::cin >> dummy;
}
#endif
