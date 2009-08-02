/****************************************************************************

    X-IvAp  -  Martin Domig <martin@domig.net>
    Copyright (C) 2006 by Martin Domig

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

#ifndef FSDAPI_H_
#define FSDAPI_H_

#include "common.h"
#include "fsdmessage.h"
#include "tcpsocket.h"

//#define PARAMS_DEBUG

#define SOFTWARE_KEY		"H1M3nI1rM7MV5v9I4IY9"

#define MAX_FSD_PACKETLEN	400

// PBH shit
#define PITCH_MULTIPLIER (float)(256.0  / 90.0)
#define BANK_MULTIPLIER  (float)(512.0  / 180.0)
#define HDG_MULTIPLIER   (float)(1024.0 / 360.0)

enum IdentMode {
	IdentSby	= 'S',
	IdentC		= 'N',
	IdentID		= 'Y'
};

class Flightplan {
public:
	Flightplan();

	bool isValid();
	const string& errorMessage() { return _errorMessage; };

	string aircraft();

	string flightrules, aircrafttype, cruisespeed, departure, deptimeest, deptimeact,
		cruisealt, destination, hoursenroute, minenroute, hoursfuel, minfuel, alternate,
		remarks, route, typeofflight, pob, alternate2, alttype, speedtype, airline, livery;
	string number, wtc, equip, transpondertype, fmcroute, pic, callsign;

	// bring all fields to a "standard", in case some punk client
	// sends a weird flight plan update. For example, time "0" becomes time "0000"
	void normalize();

	// save this flightplan to a file
	void save(string filename);

	// load fpl from file
	void load(string filename);

private:
	string _errorMessage;
};

class FsdAPI {

public:
	FsdAPI();

	FSD::Message receive();
	bool send(FSD::Message& msg, bool colon_check = true);

	void connectPilot(string host, string port, string callsign, string id, string password,
		bool hideadm, FSD::SimulatorType sim, string realname);
	void disconnectPilot();

	bool connected() { return _connected; };
	bool verified() { return _verified; };

	void sendPilotPos(IdentMode ident, int transponder, float lat, float lon, int alt,
						int speed, float pitch, float bank, float hdg, bool onground, int pressurealt);

	void sendMessage(string destination, string message);

	const string& callsign() { return _callsign; };
	const string& vid() { return _id; };
	const string& password() { return _password; };
	const string& mtl() { return _mtl; };
	const string& connectHost() { return _connectHost; };
	const int connectPort() { return _connectPort; };

	void sendPlaneInfoRequest(const string& callsign);
	void sendPlaneParamsRequest(const string& callsign);
	void sendInfoRequest(string dest, string request);
	void sendInfoRequestReply(string dest, string request, string reply);

	// returns true if new parameters were sent to FSD
	bool setParams(FSD::PlaneParams p);

	bool sendFlightplan(Flightplan& fpl);

	void sendWxRequest(FSD::WeatherType what, string station);
	void subscribeWeather();
	void unsubscribeWeather();
	void sendPlaneInfo(string mtl);
	void sendVoice(bool voice);

	void sendKill(string callsign, string reason);

	Flightplan getFlightplan(const FSD::Message& m);

	static void decodePBH(const unsigned int pbh, float& pitch, float& bank, float& heading, bool& onground);

	const ipaddress get_myip() { return _socket.get_myip(); };

private:
	tcpsocket _socket;
	bool _connected;
	bool _verified; // true after the verification process is finished

	string _realname, _id, _callsign, _password, _host, _port, _mypublicip;
	string _connectHost;
	int _connectPort;

	int _rating;
	FSD::AdminRating _admin;
	string _mtl;

	void clientVerify(const FSD::Message& m);
	void sendInfoReply(const string& dest);
	void regInfo(const FSD::Message& m);
	void sendPong(const FSD::Message& m);

	// stuff that needs to be done after our FSD connection was verified
	void _verificationCallback();
	bool _sendSubscribeWX;
	bool _sendPlaneInfo;
	bool _sendVoice;
	bool _usingWX;
	bool _usingVoice;

	FSD::PlaneParams _params;

	// rev. 3 authentification
	pt::ularge clientSeed, clientSig;

	typedef std::map<std::string, std::string> AliasMap;
	AliasMap _cslAliases;
	void _loadMTLAliases();
};

#endif
