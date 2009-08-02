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
#include "fsdapi.h"
#include "helpers.h"
#include "xivap.h"
#include "configFile.h"

#define FSDAPI_DUMP_COMMUNICATION
//#define IVAO

Flightplan::Flightplan():
	flightrules(""), aircrafttype(""), cruisespeed(""), departure(""), deptimeest(""),
	deptimeact(""), cruisealt(""), destination(""), hoursenroute(""), minenroute(""),
	hoursfuel(""), minfuel(""), alternate(""), remarks(""), route(""), _errorMessage(""),
	typeofflight(""), pob(""), alternate2(""), number(""), wtc(""), equip(""), transpondertype(""),
	fmcroute(""), alttype(""), speedtype(""), airline(""), livery(""), pic(""), callsign("")
{
}

void Flightplan::save(string filename)
{
	ConfigFile fpl;
	fpl.setConfig("FLIGHTPLAN", "RULES", flightrules);
	fpl.setConfig("FLIGHTPLAN", "FLIGHTTYPE", typeofflight);
	fpl.setConfig("FLIGHTPLAN", "NUMBER", number);
	fpl.setConfig("FLIGHTPLAN", "ACTYPE", aircrafttype);
	fpl.setConfig("FLIGHTPLAN", "WAKECAT", wtc);
	fpl.setConfig("FLIGHTPLAN", "EQUIPMENT", equip);
	fpl.setConfig("FLIGHTPLAN", "TRANSPONDER", transpondertype);
	fpl.setConfig("FLIGHTPLAN", "DEPICAO", departure);
	fpl.setConfig("FLIGHTPLAN", "DEPTIME", deptimeest);
	fpl.setConfig("FLIGHTPLAN", "SPEED", cruisespeed);
	fpl.setConfig("FLIGHTPLAN", "LEVELTYPE", alttype);
	fpl.setConfig("FLIGHTPLAN", "LEVEL", cruisealt);
	fpl.setConfig("FLIGHTPLAN", "ROUTE", route);
	fpl.setConfig("FLIGHTPLAN", "DESTICAO", destination);
	fpl.setConfig("FLIGHTPLAN", "EET", hoursenroute + minenroute);
	fpl.setConfig("FLIGHTPLAN", "ALTICAO", alternate);
	fpl.setConfig("FLIGHTPLAN", "ALT2ICAO", alternate2);
	fpl.setConfig("FLIGHTPLAN", "OTHER", remarks);
	fpl.setConfig("FLIGHTPLAN", "ENDURANCE", hoursfuel + minfuel);
	fpl.setConfig("FLIGHTPLAN", "POB", pob);
	fpl.setConfig("FLIGHTPLAN", "SPEEDTYPE",  speedtype);
	fpl.setConfig("FLIGHTPLAN", "AIRLINE",  airline);
	fpl.setConfig("FLIGHTPLAN", "LIVERY",  livery);
	fpl.setConfig("FLIGHTPLAN", "FMCROUTE",  fmcroute);
	fpl.setConfig("FLIGHTPLAN", "PIC",  pic);
	fpl.setConfig("FLIGHTPLAN", "CALLSIGN",  callsign);
	fpl.save(filename);
}

void Flightplan::load(string filename)
{
	ConfigFile fpl;
	fpl.load(filename);
	flightrules = fpl.readConfig("FLIGHTPLAN", "RULES");
	typeofflight = fpl.readConfig("FLIGHTPLAN", "FLIGHTTYPE");
	number = fpl.readConfig("FLIGHTPLAN", "NUMBER");
	aircrafttype = fpl.readConfig("FLIGHTPLAN", "ACTYPE");
	wtc = fpl.readConfig("FLIGHTPLAN", "WAKECAT");
	equip = fpl.readConfig("FLIGHTPLAN", "EQUIPMENT");
	transpondertype = fpl.readConfig("FLIGHTPLAN", "TRANSPONDER");
	departure = fpl.readConfig("FLIGHTPLAN", "DEPICAO");
	deptimeest = fpl.readConfig("FLIGHTPLAN", "DEPTIME");
	cruisespeed = fpl.readConfig("FLIGHTPLAN", "SPEED");
	alttype = fpl.readConfig("FLIGHTPLAN", "LEVELTYPE");
	cruisealt = fpl.readConfig("FLIGHTPLAN", "LEVEL");
	route = fpl.readConfig("FLIGHTPLAN", "ROUTE");
	destination = fpl.readConfig("FLIGHTPLAN", "DESTICAO");

	hoursenroute = fpl.readConfig("FLIGHTPLAN", "EET");
	while(length(hoursenroute) < 4) hoursenroute = "0" + hoursenroute;
	minenroute = copy(hoursenroute, 2, 2);
	del(hoursenroute, 2, 2);

	alternate = fpl.readConfig("FLIGHTPLAN", "ALTICAO");
	alternate2 = fpl.readConfig("FLIGHTPLAN", "ALT2ICAO");
	remarks = fpl.readConfig("FLIGHTPLAN", "OTHER");

	hoursfuel = fpl.readConfig("FLIGHTPLAN", "ENDURANCE");
	while(length(hoursfuel) < 4) hoursfuel = "0" + hoursfuel;
	minfuel = copy(hoursfuel, 2, 2);
	del(hoursfuel, 2, 2);

	pob = fpl.readConfig("FLIGHTPLAN", "POB");
	speedtype = fpl.readConfig("FLIGHTPLAN", "SPEEDTYPE");
	airline = fpl.readConfig("FLIGHTPLAN", "AIRLINE");
	livery = fpl.readConfig("FLIGHTPLAN", "LIVERY");
	fmcroute = fpl.readConfig("FLIGHTPLAN", "FMCROUTE");
	pic = fpl.readConfig("FLIGHTPLAN", "PIC");
	callsign = fpl.readConfig("FLIGHTPLAN", "CALLSIGN");
}

void Flightplan::normalize()
{
	while(length(deptimeest) < 4) deptimeest = "0" + deptimeest;
	while(length(deptimeact) < 4) deptimeact = "0" + deptimeact;
	while(length(hoursenroute) < 2) hoursenroute = "0" + hoursenroute;
	while(length(minenroute) < 2) minenroute = "0" + minenroute;
	while(length(hoursfuel) < 2) hoursfuel = "0" + hoursfuel;
	while(length(minfuel) < 2) minfuel = "0" + minfuel;
	departure = strupcase(trim(departure));
	destination = strupcase(trim(destination));
	alternate = strupcase(trim(alternate));
	alternate2 = strupcase(trim(alternate2));
	route = strupcase(trim(route));
	if(stringtoi(pob) < 0) pob = "1";
	if(stringtoi(number) < 0) number = "1";

	aircrafttype = strupcase(aircrafttype);
	typeofflight = strupcase(typeofflight);
	flightrules = strupcase(flightrules);
	alttype = strupcase(alttype);
	wtc = strupcase(wtc);
}

string Flightplan::aircraft()
{
	return number + "/" + aircrafttype + "/" + wtc + "-" + equip + "/" + transpondertype;
}

#define FPCHECK(b, m) { if(b) { _errorMessage = string(m); return false; } }
#define FPCHECKST(s, c, m) { if(pos(s, c) < 0) { _errorMessage = string(m); return false; } }

bool Flightplan::isValid()
{
	normalize();

	FPCHECK(length(aircrafttype) < 2, "Aircraft type is invalid");
	FPCHECK(length(aircrafttype) > 4, "Aircraft type is invalid");

	if(length(airline) > 0) {
		FPCHECK(length(airline) != 3, "Airline code is invalid");
	}

	FPCHECK(length(typeofflight) != 1, "invalid flight type '" + typeofflight + "'");
	FPCHECKST(typeofflight, "SNGMX", "flight type must be one of S,N,G,M,X");

	FPCHECK(length(flightrules) != 1, "invalid flight rule '" + flightrules + "'");
	FPCHECKST(flightrules, "IVYZ", "flight rule must be one of I,V,Y,Z");

	FPCHECK(length(speedtype) != 1, "invalid speed type '" + speedtype + "'");
	FPCHECKST(speedtype, "NM", "flight rule must be one of N,M");

	FPCHECK(length(cruisespeed) < 2, "invalid cruise speed");
	FPCHECK(stringtoi(cruisespeed) <= 0, cruisespeed + " is not a valid number");

	FPCHECK(length(wtc) != 1, "invalid WTC '" + flightrules + "'");
	FPCHECKST(wtc, "LMH", "WTC must be one of L,M,H");

	FPCHECK(length(departure) < 3, "invalid departure airport");
	FPCHECK(length(deptimeest) != 4, "invalid estimated departure time");
	FPCHECK(length(deptimeact) > 4, "invalid actual departure time");

	if(alttype == "VFR") cruisealt = "0";
	else {
		FPCHECK(length(alttype) != 1, "invalid altitude type");
		FPCHECKST(alttype, "FA", "flight rule must be one of F,A");
		FPCHECK(stringtoi(cruisealt) <= 0, cruisealt + " is not a valid number");
	}

	FPCHECK(length(destination) < 3, "invalid destination airport");
	FPCHECK(length(hoursenroute) != 2, "invalid hours en route");
	FPCHECK(length(minenroute) != 2, "invalid minutes en route");
	FPCHECK(length(hoursfuel) != 2, "invalid fuel hours");
	FPCHECK(length(minfuel) != 2, "invalid fuel minutes");
	FPCHECK(length(alternate) > 4, "invalid alternate airport");
	FPCHECK(length(alternate2) > 4, "invalid 2nd alternate airport");
	FPCHECK(length(remarks) > 100, "remarks field is too long");
	FPCHECK(length(route) > 255, "route field is too long");
	_errorMessage = "";
	return true;
}

FsdAPI::FsdAPI()
{
	_connected = false;
	_verified = false;
	_realname = _id = _callsign = _host = _mypublicip = _password = "";
	_rating = -1;
	_admin = FSD::ADM_USER;
	_params.params = 0;
	_sendSubscribeWX = false;
	_sendPlaneInfo = false;
	_sendVoice = false;
	_mtl = "";
	_usingWX = false;
	_usingVoice = false;

	_loadMTLAliases();
}

void FsdAPI::_loadMTLAliases()
{
	// load the MTL alias list
	string filename = getXivapRessourcesDir() + "mtlalias.txt";

	char l[512];
	int linesize = 511;
    FILE *in = fopen(pconst(filename), "ro");

	_cslAliases.clear();
    if(in == NULL) return;

	string line = "";
	std::pair<std::string, std::string> entry;
	do {
		fgets(l, linesize, in);
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;
			line = strupcase(trim(line));

			if(length(line) > 0) {
				if(line[0] != '"') continue;

				del(line, 0, 1);
				// DHLRSTRP" "DHLOLD"
				int p = pos('"', line); if(p < 0) continue;
				entry.first = pconst(copy(line, 0, p));
				del(line, 0, p+1);
				line = trim(line);

				// "DHLOLD"
				if(line[0] != '"') continue;
				del(line, 0, 1);

				// DHLOLD"
				p = pos('"', line); if(p < 0) continue;
				entry.second = pconst(copy(line, 0, p));

				// MTL -> CSL
				_cslAliases[entry.second] = entry.first;
			}
		}
	} while(!feof(in));
	fclose(in);
}

void FsdAPI::connectPilot(string host, string port, string callsign, string id, string password,
		bool hideadm, FSD::SimulatorType sim, string realname)
{
	// refuse to connect a pilot without knowing plane information
	if(_mtl == "" || !_sendPlaneInfo)
		return;

	_socket.open(host, port);
	_connected = true;
	callsign = trim(strupcase(callsign));

	// source dest vid pass rating/hideadm revision simulator realname
	// #APN1697J:SERVER:VID:PASSWORD:1:B:1:Joe Blow
	FSD::Message packet;
	packet.type = _FSD_ADDPILOT_;
	packet.source = callsign;
	packet.dest = _FSD_SERVERDEST_;
	packet.tokens.push_back(id);
	packet.tokens.push_back(password);
	packet.tokens.push_back(hideadm ? "1" : "11");
	packet.tokens.push_back(_FSD_REVISION_);
	packet.tokens.push_back(itostring(sim));
	packet.tokens.push_back(realname);
	send(packet);

	_callsign = callsign;
	_password = password;
	_id = id;
	_host = host + ":" + port;
	_connectHost = host;
	_connectPort = atoi(port);
	_realname = realname;
#ifndef IVAO
	_verified = true;
#endif
}


FSD::Message FsdAPI::receive()
{
	FSD::Message m;

	if(!_connected) return m;

	if(!_socket.connected()) {
		_connected = false;
		if(length(_socket.errorMessage()) > 0)
			xivap.uiWindow.addMessage(colRed, "Connection error: " + _socket.errorMessage());
		return m;
	}

	string str = _socket.readln();

	if(length(str) == 0)
		return m;

	// internal callbacks that dont have to leave the API
	// (authentification, inf request etc)
	do {
#ifdef FSDAPI_DUMP_COMMUNICATION
		xivap.addText(colWhite, str, true, true);
#endif
		if(length(str) == 0)
			str = _socket.readln();

		if(length(str) == 0) {
			str = "invalid";
			continue;
		}

		m.decompose(str);

		switch(m.type) {
			case _FSD_SERVERVERIFY_:
				clientVerify(m);
				str = "";
				break;

			case _FSD_REGINFO_:
				regInfo(m);
				str = "";
				m = FSD::Message();
				break;
		
			case _FSD_INFOREQ_: // INF request packets
				// Client Information
				if(m.tokens[0] == _FSD_INFOREQ_INF_) {
					sendInfoReply(m.source);
					str = "";
				} else if(m.tokens[0] == "RN") {
					sendInfoRequestReply(m.source, "RN", _realname);
					str = "";
				} else if(m.tokens[0] == "RV") {
					sendInfoRequestReply(m.source, "RV", _id);
					str = "";
				}
				break;

			case _FSD_KILL_:
				_connected = false;
				_socket.close();
				break;

			case _FSD_PING_:
				sendPong(m);
				str = "";
				break;

			case _FSD_ERROR_:
				_connected = _socket.connected();
				break;

			case _FSD_PLANEINFO_:
				if(length(m.tokens[0]) > 4) {
					// convert MTL string to CSL alias, if it is set
					string ac = copy(m.tokens[0], 0, 4); // aircraft code
					string s = m.tokens[0];
					del(s, 0, 4); // rest without aircraft code

					// look up this MTL code...
					AliasMap::iterator it = _cslAliases.find(pconst(s));
					// and replace it with the CSL alias if found
					if(it != _cslAliases.end())
						m.tokens[0] = ac + string(it->second.c_str());
				}
				break;

			default:
				break;
		}
	} while(length(str) == 0);

	return m;
}

void FsdAPI::sendInfoRequestReply(string dest, string request, string reply)
{
	FSD::Message m;
	m.type = _FSD_INFOREPLY_;
	m.source = _callsign;
	m.dest = dest;
	m.tokens.push_back(request);
	m.tokens.push_back(reply);
	send(m, false);
}

void FsdAPI::sendPong(const FSD::Message& m)
{
	// source dest time
	// $POEBBR_APP:N1697J:TIME
	FSD::Message reply;
	reply.type = _FSD_PONG_;
	reply.source = _callsign;
	reply.dest = m.source;
	reply.tokens.push_back(m.tokens[0]);

	send(reply);
}

void FsdAPI::regInfo(const FSD::Message& m)
{
	//revision signature atc/pilotrating adminrating ip
	// 0        1         2               3           4
	//!RSERVER:EBBR_APP:0:1234567890:7:11:80.120.12.15
	_rating = static_cast<int>(stringtoi(m.tokens[2]));
	int _admin = static_cast<FSD::AdminRating>(stringtoi(m.tokens[3]));
	_mypublicip = m.tokens[4];

	// FIXME - ask kenny...
	unsigned int hash1 = 0;
	for(int i = 0; i < length(_id); i++) {
		// hash number 1
		if ((i & 1) == 0)
			hash1 ^=((hash1 << ((clientSig + clientSeed) % 17))^_id[i]^(hash1 >> ((clientSig + clientSeed) % 16)));
		else
			hash1 ^= (~((hash1 << ((clientSig + clientSeed) % 21))^_id[i]^(hash1 >> ((clientSig + clientSeed) % 19))));
	}
	hash1 &= 0x7FFFFFFF;
	pt::large serverauth = stringtoi(m.tokens[1]);
	// client is supposed to check the server auth here. which I won't - i consider it deeply unethical and absolutely stupid.
	/*
	if(hash1 != serverauth) {
		_socket.close();
		_connected = false;
		return;
	}
	*/

	_verified = true;
	_verificationCallback();
}

bool FsdAPI::send(FSD::Message &msg, bool colon_check)
{
	// don't send shit to a non-verified connection
	if(!_verified && msg.type != _FSD_CLIENTVERIFY_ && msg.type != _FSD_ADDPILOT_ && msg.type != _FSD_ADDATC_)
		return false;

	if(!_connected)
		return false;

	if(!_socket.connected()) {
		_connected = false;
		if(length(_socket.errorMessage()) > 0)
			xivap.uiWindow.addMessage(colRed, "Connection error: " + _socket.errorMessage());
		return false;
	}

	string line = msg.compose(colon_check);
	if(length(line) > 0 && length(line) < MAX_FSD_PACKETLEN) {
		_socket.writeln(line);
#ifdef FSDAPI_DUMP_COMMUNICATION
		xivap.addText(colLightGray, line);
#endif
		return true;
	}
	return false;
}

void FsdAPI::clientVerify(const FSD::Message& m)
{
	// from server: source dest revision seed
	// !SSERVER:DAT120M:0:1234567890

	// **************************
	// removed on request by IVAO
	// **************************

	_connected = true;
}

void FsdAPI::sendInfoReply(const string& dest)
{
	// send INF reply to the sender
	string reply = string(FSD_SOFTWARE_NAME) + " " + SOFTWARE_VERSION 
#ifdef IVAO
		+ " IVAO"
#endif
		+ " (" + xivap.revision() + ") " + PLATFORM 
		+ " - " + _realname + " (" + _id + ") MTL " + _mtl
		+ " - PrvIP " + _socket.ip() + " PubIP "
		+ _mypublicip + " - Srv " + _host;

	if(_usingWX) reply += " - WX on";
	else reply += " - WX off";

	if(xivap.autopilotOn())
		reply += " - AP on: [" + xivap.autopilotState() + "]";
	else
		reply += " - AP off";
	
	reply += " - P2P " + itostring(xivap.p2pmode());

	sendMessage(dest, reply);
}

void FsdAPI::sendMessage(string destination, string message)
{
	FSD::Message m;
	m.type = _FSD_TEXTMESSAGE_;
	m.source = _callsign;
	m.dest = stripcolons(destination);
	m.tokens.push_back(stripcolons(message));
	send(m);
}

void FsdAPI::sendPilotPos(IdentMode ident, int transponder, float lat, float lon, int alt,
						  int speed, float pitch, float bank, float hdg, bool onground, int pressurealt)
{
	if(!_connected)
		return;

	// ident callsign transponder rating latitude longitude truealt speed pitchbankheading
	// @N:N169J:1200:3:43.12345:-78.543:12000:120:3487239347:60
	FSD::Message pos;
	pos.type = _FSD_PILOTPOS_;
	switch(ident) {
		case IdentSby:	pos.source = "S"; break;
		case IdentC:	pos.source = "N"; break;
		case IdentID:	pos.source = "Y"; break;
	}

	pos.dest = _callsign;

	string xpdrstr = itostring(transponder);
	while(length(xpdrstr) < 4) xpdrstr = "0" + xpdrstr;
	while(length(xpdrstr) > 4) del(xpdrstr, 0, 1);
	for(int i = 0; i < 4; ++i)
		if(xpdrstr[i] < '0' || xpdrstr[i] > '7') xpdrstr[i] = '0';
	pos.tokens.push_back(xpdrstr);

	pos.tokens.push_back(itostring(_rating));
	pos.tokens.push_back(ftoa(lat));
	pos.tokens.push_back(ftoa(lon));
	pos.tokens.push_back(itostring(alt));
	pos.tokens.push_back(itostring(speed));

	FSD::FS_PBH pbh;
	pbh.pbh = 0;
	pbh.pitch = static_cast<int>(pitch * PITCH_MULTIPLIER);
	pbh.bank = static_cast<int>(bank * BANK_MULTIPLIER);
	pbh.hdg = static_cast<int>(hdg * HDG_MULTIPLIER);
	if(onground) pbh.onground = 1;

	pos.tokens.push_back(itostring(pbh.pbh));

	int altdiff = pressurealt - alt;
	pos.tokens.push_back(itostring(altdiff));

	send(pos);
}

void FsdAPI::decodePBH(const unsigned int pbh, float& pitch, float& bank, float& heading, bool& onground)
{
	FSD::FS_PBH pbhstrct;

	pbhstrct.pbh = pbh;
	pitch = pbhstrct.pitch / PITCH_MULTIPLIER;
	bank = pbhstrct.bank / BANK_MULTIPLIER;
	heading = pbhstrct.hdg / HDG_MULTIPLIER;
	onground = pbhstrct.onground == 1;
}

void FsdAPI::disconnectPilot()
{
	string packet = FSD_HEADS[_FSD_DELPILOT_] + _callsign;
	_socket.writeln(packet);
	_socket.close();
	_connected = false;
	_params.params = 0;
}

bool FsdAPI::sendFlightplan(Flightplan& fpl)
{
	if(!_connected) return false;

	// compose flightplan packet
	FSD::Message m;
	m.type = _FSD_FLIGHTPLAN_;
	m.source = _callsign;
	m.dest = _FSD_SERVERDEST_;
	m.tokens.push_back(fpl.flightrules);
	m.tokens.push_back(fpl.aircraft());
	m.tokens.push_back(fpl.speedtype + fpl.cruisespeed);
	m.tokens.push_back(fpl.departure);
	m.tokens.push_back(fpl.deptimeest);
	m.tokens.push_back(fpl.deptimeact);
	m.tokens.push_back(fpl.alttype + fpl.cruisealt);
	m.tokens.push_back(fpl.destination);
	m.tokens.push_back(fpl.hoursenroute);
	m.tokens.push_back(fpl.minenroute);
	m.tokens.push_back(fpl.hoursfuel);
	m.tokens.push_back(fpl.minfuel);
	m.tokens.push_back(fpl.alternate);
	m.tokens.push_back(fpl.remarks);
	m.tokens.push_back(fpl.route);
	m.tokens.push_back(fpl.typeofflight);
	m.tokens.push_back(fpl.pob);
	m.tokens.push_back(fpl.alternate2);

	//xivap.addText(colWhite, "flightplan string: " + m.compose());

	// ...and send it
	return send(m);
}

Flightplan FsdAPI::getFlightplan(const FSD::Message& m)
{
	Flightplan result;

	result.cruisespeed = m.tokens[0];

	if(result.cruisespeed[0] < '0' || result.cruisespeed[0] > '9') {
		result.speedtype = result.cruisespeed[0];
		del(result.cruisespeed, 0, 1);
	}

	result.departure = m.tokens[1];
	result.deptimeest = m.tokens[2];
	result.deptimeact = m.tokens[3];
	result.cruisealt = m.tokens[4];
	result.destination = m.tokens[5];
	result.hoursenroute = m.tokens[6];
	result.minenroute = m.tokens[7];
	result.alternate = m.tokens[8];
	result.remarks = m.tokens[9];
	result.route = m.tokens[10];

	result.normalize();

	return result;
}

void FsdAPI::sendWxRequest(FSD::WeatherType what, string station)
{
	station = trim(strupcase(station));
	if(length(station) <= 2) return;
	if(length(station) > 4) return;

	FSD::Message m;
	m.type = _FSD_WEADATA_;
	m.source = _callsign;
	m.dest = _FSD_SERVERDEST_;
	m.tokens.push_back(itostring(what));
	m.tokens.push_back(station);
	send(m);
}

void FsdAPI::sendInfoRequest(string dest, string request)
{
	dest = trim(dest);
	if(length(dest) < 3) return;
	request = trim(request);
	if(length(request) < 1) return;

	FSD::Message m;
	m.type = _FSD_INFOREQ_;
	m.source = _callsign;
	m.dest = dest;
	m.tokens.push_back(request);
	send(m, false);
}

bool FsdAPI::setParams(FSD::PlaneParams p)
{
	if(!(_verified && _connected))
		return false;				// don't send the info yet

	if(p.params == _params.params)
		return false;				// params did not change

#ifdef PARAMS_DEBUG
	string debug = "SENDING PARAMS: Gear:" + itostring(_params.gear)
		+ " Bcn:" + itostring(p.beaconLight)
		+ " Lnd:" + itostring(p.landLight)
		+ " Nav:" + itostring(p.navLight)
		+ " Strb:" + itostring(p.strobeLight)
		+ " Txi:" + itostring(p.taxiLight)
		+ " ThRev:" + itostring(p.thrustReversers)
		+ " E1:" + itostring(p.engine1Running)
		+ " E2:" + itostring(p.engine2Running)
		+ " E3:" + itostring(p.engine3Running)
		+ " E4:" + itostring(p.engine4Running)
		+ " Flps:" + itostring(p.flapsRatio)
		+ " SpdBrk:" + itostring(p.speedbrakeRatio)
		+ " Thrtle:" + itostring(p.thrustRatio);
	xivap.addText(colGray, debug, true, true);
#endif

	FSD::Message m;
	m.type = _FSD_PLANEPARAMS_;
	m.source = _callsign;
	m.dest = _FSD_SERVERDEST_;

	m.tokens.push_back(itostring(p.params));

	if(send(m)) {
		_params.params = p.params;
		return true;
	}
	return false;
}

void FsdAPI::subscribeWeather()
{
	if(!_connected || !_verified) {
		_sendSubscribeWX = true;
		return;
	}

	FSD::Message m;
	m.type = _FSD_WEAREG_;
	m.source = _callsign;
	m.dest = _FSD_SERVERDEST_;
	m.tokens.push_back("1");
	if(send(m)) {
		_sendSubscribeWX = false;
		_usingWX = true;
	}
}

void FsdAPI::unsubscribeWeather()
{
	_sendSubscribeWX = false;
	_usingWX = false;
	FSD::Message m;
	m.type = _FSD_WEAREG_;
	m.source = _callsign;
	m.dest = _FSD_SERVERDEST_;
	m.tokens.push_back("0");
	send(m);
}

void FsdAPI::sendVoice(bool status)
{
	if(!_connected || !_verified) {
		_sendVoice = true;
		_usingVoice = status;
		return;
	}

	FSD::Message m;
	m.type = _FSD_NOVOICE_;
	m.source = _callsign;
	m.dest = _FSD_SERVERDEST_;
	m.tokens.push_back(_callsign);
	if(status) m.tokens.push_back("1");
	else m.tokens.push_back("0");
	if(send(m))
		_sendVoice = false;
}

void FsdAPI::_verificationCallback()
{
	if(_sendSubscribeWX) subscribeWeather();
	if(_sendPlaneInfo) sendPlaneInfo(_mtl);
	if(_sendVoice) sendVoice(_usingVoice);
}

void FsdAPI::sendPlaneInfo(string mtl)
{
	if(!_connected || !_verified) {
		_sendPlaneInfo = true;
		_mtl = mtl;
		return;
	}
	_sendPlaneInfo = false;
	_mtl = mtl;

	// look up the CSL alias table: if the _mtl string is in there,
	// send the corresponding alias
	// alias table contains MTL -> CSL mapping (file is the other way around)
	if(length(_mtl) > 4) {
		string ac = copy(_mtl, 0, 4);
		string s = _mtl;
		del(s, 0, 4);
		AliasMap::iterator it = _cslAliases.begin();
		bool found = false;
		while(it != _cslAliases.end() && !found) {
			if(it->second.c_str() == s) {
				found = true;
				_mtl = ac + string(it->first.c_str());
			}
			++it;
		}
	}

	// don't send 5 letter CSL livery codes. If the livery code exceeds
	// 3 characters, remove it
	if(length(_mtl) > 10) {
		_mtl = copy(mtl, 0, 7); //B727aaaxxxxx -> B727aaa
	}


	FSD::Message m;
	m.type = _FSD_PLANEINFO_;
	m.source = _callsign;
	m.dest = _FSD_SERVERDEST_;
	m.tokens.push_back(_mtl);
	send(m);

	// debugging
	string mtldebug = "SENDING PLANE INFO: " + _mtl;
	xivap.addText(colGray, _mtl, false, true);
	xivap.addText(colGray, m.compose(), false, true);
}

void FsdAPI::sendPlaneInfoRequest(const string& callsign)
{
	FSD::Message m;
	m.type = _FSD_REQPLANEINFO_;
	m.source = _callsign;
	m.dest = callsign;
	send(m);
}

void FsdAPI::sendPlaneParamsRequest(const string& callsign)
{
	FSD::Message m;
	m.type = _FSD_REQPLANEPARAMS_;
	m.source = _callsign;
	m.dest = callsign;
	send(m);
}

void FsdAPI::sendKill(string callsign, string reason)
{
	reason = "Killed by " + _callsign + ": " + reason;

	FSD::Message m;
	m.type = _FSD_KILL_;
	m.source = _callsign;
	m.dest = callsign;
	m.tokens.push_back(reason);
	send(m);
}
