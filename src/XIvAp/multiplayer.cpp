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
#include "multiplayer.h"
#include "xivap.h"
#include "helpers.h"
#include "TLVpacket.h"

#include <math.h>

#ifdef __ppc__
#include <Endian.h>
#endif

using namespace Multiplayer;

static MultiplayerEngine *_mp_engine = NULL;

XPMPPlaneCallbackResult PlaneDataCallback(XPMPPlaneID inPlane,
					   XPMPPlaneDataType inDataType,
					   void * ioData,
					   void * inRefcon)
{
	if(_mp_engine == NULL) return xpmpData_Unavailable;
	return _mp_engine->planeCallback(inPlane, inDataType, ioData, inRefcon);
}


int IntPrefs(const char *section, const char *key, int def)
{

/*
 * The two prefs funcs each take an ini section and key and return a value and also take
 * a default.  The renderer uses them for configuration.  Currently the following keys are
 * needed:
 *
 * section	key					type	default	description
 * planes	full_distance		float	3.0		
 * planes	max_full_count		int		50		
 * planes	resolution			int		3		value from 5 (best) to 0 (worst), read from the config file
 */

	//if(section == "planes" && key == "resolution") return def;
	if(section == "planes" && key == "resolution") {
		return xivap.GetPlaneRes();
	}
	if(section == "planes" && key == "max_full_count") return 100;
	if(section == "debug" && key == "model_matching") return 1;
	
//	xivap.addText(colWhite, string("IntPrefs called with section '") + section + "' key '" + key + "' default value "
//		+ itostring(def), false, true);
	return def;
}

float FloatPrefs(const char *section, const char *key, float def)
{
	if(section == "planes" && key == "full_distance") return def;

//	xivap.addText(colWhite, string("FloatPrefs called with section '") + section + "' key '" + key + "' default value "
//		+ string(ftoa(def)), false, true);
	return def;
}

/******************************************************************************/
/** OO code below here :) *****************************************************/
/******************************************************************************/

MultiplayerEngine::MultiplayerEngine():
_initialized(false), _enabled(false)
{
	_mp_engine = this;
	_defaultIcao = "A320"; // fixme - make this configurable
	_p2pmode = 0; // off
	p2penabled = false;
	p2psendbps = 0;
	p2precvbps = 0;
	p2plistenport = 0;
}

MultiplayerEngine::~MultiplayerEngine()
{
	_mp_engine = NULL;
	clear();
}

void MultiplayerEngine::clear()
{
	PlaneMap::iterator i = _planes.begin();
	while(i != _planes.end()) {
		if(i->second->XPregistered)
			XPMPDestroyPlane(i->second->id);
		delete i->second;
		++i;
	}
	_planes.clear();
}

void MultiplayerEngine::disable()
{
	clear();
}


bool MultiplayerEngine::init(bool enablep2p, int maxsendBps, int maxrecvBps, int listenPort, string defaultICAO)
{
	if(_initialized) return true;
	_defaultIcao = defaultICAO;

	string result = XPMPMultiplayerInit(
			getMachRessourcesDir() + "CSL",
			getXivapRessourcesDir() + "related.txt",
			getXivapRessourcesDir() + "lights.png",
			getXivapRessourcesDir() + "Doc8643.txt",
			_defaultIcao, IntPrefs, FloatPrefs);

	if(length(result) > 0) {
		_errorMessage = "'" + result + "'";
		return false;
	}

	p2penabled = enablep2p;
	p2psendbps = maxsendBps;
	p2precvbps = maxrecvBps;
	p2plistenport = listenPort;


	_initialized = true;

	return true;
}

void MultiplayerEngine::testp2p(string stunserver)
{
	if(p2penabled) {
		string type = "";
		_stunclient.SetServer(pconst(stunserver));

		_stunclient.SetPort(p2plistenport);

		switch(_stunclient.RunTest()) {
			case Stun::StunOpen:
				type = "open";
			case Stun::StunIndependentFilter:
				if(type == "") type = "independent filter";
				xivap.addText(colWhite, "STUN determined active p2p (" + type + ")", true, true);
				_p2pmode = 3;
				break;

			case Stun::StunDependentFilter:
				type = "dependent filter";
			case Stun::StunPortDependedFilter:
				if(type == "") type = "port dependent filter";
			case Stun::StunDependentMapping:
				if(type == "") type = "dependent mapping";
				xivap.addText(colWhite, "STUN determined passive p2p (" + type + ")", true, true);
				_p2pmode = 2;
				break;

			case Stun::StunBlocked:
				if(type == "") type = "blocked";
			case Stun::StunFirewall:
				if(type == "") type = "firewall";
				xivap.addText(colWhite, "STUN determined NO p2p (" + type + ")", true, true);
				_p2pmode = 0;
				break;

			case Stun::StunError:
			default:
				xivap.addText(colWhite, "STUN ERROR", true, true);
				_p2pmode = 0;
				break;
		}
	}
}

bool MultiplayerEngine::enable()
{
	if(_enabled) return true;
	if(!_initialized) init();

	string result = XPMPMultiplayerEnable(); // returns non-empty string on error
	if(length(result) > 0) {
		_errorMessage = "'" + result + "'";
		return false;
	}
	_enabled = true;

	XPMPLoadPlanesIfNecessary();
	return true;
}

XPMPPlaneCallbackResult MultiplayerEngine::planeCallback(
					XPMPPlaneID inPlane,
					XPMPPlaneDataType inDataType,
					void * ioData,
					void * inRefcon)
{
	string callsign = findByID(inPlane);
	if(callsign == "")
		return xpmpData_Unavailable;

	MultiplayerPilot* plane = _planes[STDSTRING(callsign)];

	// return unavailable if rendering fails for some reason
	if(!plane->calculateFrame())
		return xpmpData_Unavailable;

	switch(inDataType) {
		case xpmpDataType_Position: {
				XPMPPlanePosition_t *pos = static_cast<XPMPPlanePosition_t*>(ioData);
				
				if(pos->lat == plane->position.pos.lat
					&& pos->lon == plane->position.pos.lon
					&& pos->pitch == plane->position.pos.pitch
					&& pos->roll == plane->position.pos.roll
					&& pos->heading == plane->position.pos.heading
					&& pos->elevation == plane->position.pos.elevation)
					return xpmpData_Unchanged;

				memcpy(ioData, static_cast<void*>(&plane->position), sizeof(XPMPPlanePosition_t));
				return xpmpData_NewData;
			}
			break;

		case xpmpDataType_Surfaces: {
				XPMPPlaneSurfaces_t *srfc = static_cast<XPMPPlaneSurfaces_t*>(ioData);
								
				if(srfc->flapRatio == plane->surfaces.flapRatio
					&& srfc->gearPosition == plane->surfaces.gearPosition
					&& srfc->thrust == plane->surfaces.thrust
					&& srfc->speedBrakeRatio == plane->surfaces.speedBrakeRatio
					&& srfc->lights.lightFlags == plane->surfaces.lights.lightFlags)
					return xpmpData_Unchanged;

				memcpy(ioData, static_cast<void*>(&plane->surfaces), sizeof(XPMPPlaneSurfaces_t));
				return xpmpData_NewData;									
			}
			break;

		case xpmpDataType_Radar: {
				XPMPPlaneRadar_t *radar = static_cast<XPMPPlaneRadar_t*>(ioData);
				
				if(radar->code == plane->radarStatus.code
					&& radar->mode == plane->radarStatus.mode)
					return xpmpData_Unchanged;

				memcpy(ioData, static_cast<void*>(&plane->radarStatus), sizeof(XPMPPlaneRadar_t));
				return xpmpData_NewData;									
			}
			break;

		default:
			break;
	}

	return xpmpData_Unavailable;
}

XPMPPlaneID MultiplayerEngine::findByCallsign(const string& callsign)
{
	PlaneMap::iterator i = _planes.find(STDSTRING(callsign));
	if(i == _planes.end()) return NULL;
	return i->second->id;
}

string MultiplayerEngine::findByID(const XPMPPlaneID &id)
{
	for(PlaneMap::iterator i = _planes.begin(); i != _planes.end(); ++i)
		if(i->second->id == id) return string(i->first.c_str());

	return "";
}

MultiplayerEngine::PlaneMap::iterator MultiplayerEngine::findByPeerInfo(const ipaddress& ip, int port)
{
	for(PlaneMap::iterator i = _planes.begin(); i != _planes.end(); ++i)
		if(i->second->p2pmode >= 1 && 
			(i->second->p2pip == ip && i->second->p2pport == port)
			|| (i->second->p2pip_int == ip && i->second->p2pport_int == port))
			return i;

	return _planes.end();
}

void MultiplayerEngine::decodeFSDPosition(const FSD::Message& packet, 
										  XPMPPlanePosition_t* position,
										  XPMPPlaneRadar_t* radarstatus,
										  bool& onground)
{
	unsigned int pbh;

	switch(packet.type) {
		case _FSD_PILOTPOS_:
			// ident callsign transponder rating latitude longitude truealt speed pitchbankheading
			// source dest    0           1      2        3         4       5     6
			
			if(radarstatus != NULL) {
				if(packet.source == "S") radarstatus->mode = xpmpTransponderMode_Standby;
				else if(packet.source == "N") radarstatus->mode = xpmpTransponderMode_ModeC;
				else if(packet.source == "Y") radarstatus->mode = xpmpTransponderMode_ModeC_Ident;
				// other codes: xpmpTransponderMode_Mode3A, xpmpTransponderMode_ModeC_Low
				radarstatus->code = static_cast<long>(stringtoi(packet.tokens[0]));
			}

			pbh = static_cast<unsigned int>(stringtoi(packet.tokens[6]));
			FsdAPI::decodePBH(pbh, position->pitch, position->roll, position->heading, onground);

			if(position != NULL) {
				position->lat = atof(packet.tokens[2]);
				position->lon = atof(packet.tokens[3]);
				position->elevation = atof(packet.tokens[4]);
				position->pitch	*= -1;
				position->roll	*= -1;
			}
			break;

		case _FSD_FASTPILOT_:
			break;

		default:
			break;
	} // switch
}

bool MultiplayerEngine::splitMTL(string mtl, string& aircraft, string& airline, string& livery)
{
	if(length(mtl) < 2) return false;

	// bug with old clients
	mtl = strupcase(mtl);
	if(length(mtl) == 5) del(mtl, 0, 1);
	if(length(mtl) == 6) del(mtl, 0, 2);

	airline = mtl;
	aircraft = copy(airline, 0, 4);	// first 4 letters are aircraft type
	del(airline, 0, 4);
	livery = "";
	if(length(airline) > 3) {		// if more than 3 letters left, split into airline and livery
		livery = airline;
		airline = copy(livery, 0, 3);
		del(livery, 0, 3);
	}
	return true;
}

void MultiplayerEngine::setAllModels(string aircraft, string airline, string livery)
{
	PlaneMap::iterator pilotIter = _planes.begin();
	MultiplayerPilot *pilot;

	while(pilotIter != _planes.end()) {
		pilot = pilotIter->second;
		if(pilot->XPregistered) {		// we already know this guy

			pilot->mtl = aircraft + airline + livery;
			pilot->mtlRequested = true;		// just that we dont ask for the MTL code
			pilot->setLightPattern(aircraft);

			XPMPChangePlaneModel(pilot->id, aircraft, airline, livery);
			xivap.addText(colCyan, "MP: MODEL SET for " + pilot->callsign
				+ " (" + aircraft + ":" + airline + ":" + livery + ")", true, true);
		}
		++ pilotIter;
	}
}

void MultiplayerEngine::removeCorpses()
{
	// remove all pilots that did not send a FSD pos. update
	// within the last FSD_PILOT_TIMEOUT seconds

	std::vector<PlaneMap::iterator> removeList;
	float now_ = XPLMGetElapsedTime();

	for(PlaneMap::iterator it = _planes.begin(); it != _planes.end(); ++it)
		if(it->second->lastFsdPos_t + FSD_PILOT_TIMEOUT < now_)
			removeList.push_back(it);

	for(std::vector<PlaneMap::iterator>::iterator it = removeList.begin(); it != removeList.end(); ++it)
	{
		// dont forget to un-register the pilot in XP...
		if((*it)->second->XPregistered) {
			xivap.addText(colCyan, "MP: timeout for pilot " + (*it)->second->callsign, true, true);
			XPMPDestroyPlane((*it)->second->id);
		}
		_planes.erase(*it);
	}
}

void MultiplayerEngine::eatThis(const FSD::Message &packet)
{
	if(!(_initialized && _enabled)) return;		// don't do shit when not enabled
	if(packet.type == _FSD_ADDPILOT_) return;	// I couldn't care less...

	string callsign;

	if(packet.type == _FSD_PILOTPOS_)
		callsign = packet.dest;
	else
		callsign = packet.source;

	PlaneMap::iterator pilotIter = _planes.find(STDSTRING(callsign));

	// if it is a #DP remove pilot
	if(packet.type == _FSD_DELPILOT_) {
		if(pilotIter != _planes.end()) {	// if he is in the list...
			if(pilotIter->second->XPregistered) { // and registered -> unregister
				xivap.addText(colCyan, "MP: Removing pilot " + callsign, true, true);
				XPMPDestroyPlane(pilotIter->second->id);
				_planes.erase(pilotIter);
			}
		}
		return;
	}

	MultiplayerPilot *pilot;

	if(pilotIter == _planes.end()) {				// this is a new guy...
		// add pilot
		pilot = new MultiplayerPilot();
		pilot->init();
		pilot->callsign = callsign;
		pilot->timestamp = xivap.watch.getTime();
		_planes[STDSTRING(callsign)] = pilot;
		xivap.addText(colCyan, "MP: Adding pilot " + callsign, true, true);
	} else {
		// plane already exists
		pilot = _planes[STDSTRING(callsign)];
	}

	pilot->timestamp = xivap.watch.getTime();

	/***************************************************
	 * Only if a valid MTL string is received from an other client,
	 * it will be added to the multiplayer engine.
	 *
	 * SAY AGAIN: IF THE OTHER CLIENT DOES NOT SEND A MTL CODE,
	 * IT WONT BE SEEN
	 ***************************************************
	 * Needed to do a nasty workaround. Reason? IVAO blocks
	 * unknown MTL codes. That is, it probably blocks 99% of the
	 * codes sent by xivap at the moment.
	 ***/

	if(packet.type == _FSD_PLANEINFO_) {	// this is the MTL code we've been waiting for
		// decode the MTL string
		string icao, airline, livery;
		if(!splitMTL(packet.tokens[0], icao, airline, livery))
			return;							// returns false if splitting fails

		// truncate the ICAO to make it work with three-letter codes like "H47"
		icao = trim(icao);

		pilot->mtl = packet.tokens[0];
		pilot->mtlRequested = true;		// just that we dont ask again for the MTL code
		xivap.addText(colCyan, "MP: Received MTL from " + callsign + ": " + pilot->mtl, true, true);

		if(pilot->XPregistered) {		// we already know this guy -> send the update to X-Plane
			XPMPChangePlaneModel(pilot->id, icao, airline, livery);
			xivap.addText(colCyan, "MP: Update pilot information on X-Plane: " + callsign 
				+ " (" + icao + ":" + airline + ":" + livery + ")", true, true);
		}

		pilot->setLightPattern(icao);

		return;
	}

	// if we have a MTL string and 2 positions, but the pilot is not yet registered
	// with X-Plane, register him
	if(!pilot->XPregistered && pilot->interpolator.usable()) {
		if(length(pilot->mtl) == 0)
			pilot->mtl = "A320";
		
		string icao, airline, livery;
		if(!splitMTL(pilot->mtl, icao, airline, livery))
			return;							// returns false if splitting fails

		// now that we have a valid MTL code, we can add the pilot to X-Plane
		pilot->XPregistered = true;
		pilot->id = XPMPCreatePlane(icao, airline, livery, PlaneDataCallback, NULL);
		xivap.addText(colCyan, "MP: Registering Pilot on X-Plane: " + callsign 
			+ " (" + icao + ":" + airline + ":" + livery + ")", true, true);
	}

	// if we dont know the MTL string for this pilot,
	// request it from FSD
	if(!pilot->mtlRequested) { 							
		xivap.fsd.sendPlaneInfoRequest(callsign);
		// and yeah.. we probably dont know the params either
		xivap.fsd.sendPlaneParamsRequest(callsign);
		pilot->mtlRequested = true;
	}

	if(packet.type == _FSD_PILOTPOS_) {
		// ignore standard position updates for clients
		// that send p2p data
		pilot->lastFsdPos_t = XPLMGetElapsedTime();
		if(pilot->ignoreLegacyUpdates())
			return;

		PlanePosition* pos = new PlanePosition();
		decodeFSDPosition(packet, &(pos->pos), &(pilot->radarStatus), pilot->onground);
		pos->timestamp = xivap.watch.getTime();
		pos->onground = pilot->onground;
		pos->p_timestamp = 0;
		pos->latency = 0;
		pilot->interpolator.takeNewPosition(pos, pilot->distance);

		pilot->CalcDistance(xivap.GetLat(), xivap.GetLon());

		double altdiff = pos->pos.elevation - xivap.elevationft();
		if(altdiff < 0) altdiff *= -1.0;
		if(altdiff > P2P_ALTDIFF)
			return;

		// if close enough, ask for p2pinfo (if we dont have it already)
		if(pilot->p2prequested || pilot->didrequestp2p || _p2pmode < 1)
			return;

		// see if close enough...
		if(pilot->distance > 0 && pilot->distance < P2P_RANGE) {
			pilot->p2prequested = true;

			// form address string: "127.0.0.1:4711"
			string ipport = iptostring(_stunclient.PublicIp())
				+ ":" + itostring(_stunclient.PublicPort());
			ipport += ":" + iptostring(xivap.fsd.get_myip())
				+ ":" + itostring(_stunclient.Port());

			xivap.RequestP2P(pilot->callsign, P2P_POS_PROTOCOL,
				_p2pmode, ipport);
		}

		return;
	}

	if(packet.type == _FSD_PLANEPARAMS_) {
		// ignore standard param updates for clients for
		// which we receive fast updates or p2p data
		if(pilot->ignoreLegacyUpdates())
			return;

		FSD::PlaneParams params;
		params.params = static_cast<unsigned int>(stringtoi(packet.tokens[0]));
		HandlePlaneParams(pilot, params);
	}

	if(packet.type == _FSD_INFOREQ_) {
		handleP2Prequest(pilot, packet);
		return;
	}

	if(packet.type == _FSD_INFOREPLY_) {
		handleP2Preply(pilot, packet);
		return;
	}

}

void MultiplayerEngine::handleP2Preply(MultiplayerPilot *pilot, const FSD::Message &packet)
{
	// safeguards...
	if(packet.tokens.size() < 3) return;
	if(packet.tokens[0] != "P2P") return;
	if(packet.tokens[2] != P2P_POS_PROTOCOL) return;

	xivap.addText(colRed, "got p2p reply from " + pilot->callsign + ": " + packet.encoded, true, true);

	// ignore p2p reply if p2p is off
	if(_p2pmode == 0)
		return;

	// got a p2p info reply: $CRBBBB:AAAA:P2P:<mode>:<application>[:<ip>:<port>:<intip>:<intport>]
	//                                    0   1      2              3    4       5       6

	pilot->p2pmode = atoi(pconst(packet.tokens[1]));

	if(pilot->p2pmode >= 1 && packet.tokens.size() < 7)
		return; // did send mode3 without connection info

	if(pilot->p2pmode >= 1 && !pilot->didseep2ptraffic) {
		pilot->p2pip = phostbyname(packet.tokens[3]);
		//xivap.addText(colWhite, "SET IP TO " + iptostring(pilot->p2pip), true, true);
		pilot->p2pport = atoi(pconst(packet.tokens[4]));
		pilot->p2pip_int = phostbyname(packet.tokens[5]);
		pilot->p2pport_int = atoi(pconst(packet.tokens[6]));
		if(pilot->p2pip == _stunclient.PublicIp())
			pilot->p2psamelan = true;
	}

	// initiate p2p connection unless peer is passive,
	// but always if we are on the same lan (= same public IP)
	if(pilot->p2pmode >= 3 || pilot->p2psamelan) {
		p2phandshake(pilot);
	}
}

void MultiplayerEngine::handleP2Prequest(MultiplayerPilot *pilot, const FSD::Message &packet)
{
	// safeguards...
	if(packet.tokens.size() < 3) return;
	if(packet.tokens[0] != "P2P") return;
	if(packet.tokens[2] != P2P_POS_PROTOCOL) return;

	pilot->didrequestp2p = true;

	xivap.addText(colRed, "got p2p request from " + pilot->callsign + ": " + packet.encoded, true, true);

	// got a p2p info request: $CQAAAA:BBBB:P2P:<mode>:<application>[:<ip>:<port>:<intip>:<intport>]
	//                                      0   1      2              3    4      5       6

	pilot->p2pmode = atoi(pconst(packet.tokens[1]));
	if(packet.tokens.size() < 7)
		return; // did not send connection info

	if(pilot->p2pmode >= 1 && !pilot->didseep2ptraffic) {
		pilot->p2pip = phostbyname(packet.tokens[3]);
		//xivap.addText(colWhite, "SET IP TO " + iptostring(pilot->p2pip), true, true);
		pilot->p2pport = atoi(pconst(packet.tokens[4]));
		pilot->p2pip_int = phostbyname(packet.tokens[5]);
		pilot->p2pport_int = atoi(pconst(packet.tokens[6]));
		if(pilot->p2pip == _stunclient.PublicIp())
			pilot->p2psamelan = true;
	}

	// we can accept that connection
	string ipport = iptostring(_stunclient.PublicIp())
			+ ":" + itostring(_stunclient.PublicPort());
	ipport += ":" + iptostring(xivap.fsd.get_myip())
			+ ":" + itostring(_stunclient.Port());

	// p2p should work on the same LAN whatever STUN thought, so try anyway
	if(!(pilot->p2psamelan || (_p2pmode < 1))) { // p2p disabled?
		xivap.SendP2PReply(pilot->callsign, P2P_POS_PROTOCOL, _p2pmode, ipport);
		p2phandshake(pilot);
		return;
	}

	// if we are (at least) passive and he is active
	// or we both are on the same LAN (= behind same NAT)
	// initiate the p2p connection
	if((_p2pmode >= 2 && pilot->p2pmode >= 3))	{
		xivap.SendP2PReply(pilot->callsign, P2P_POS_PROTOCOL, _p2pmode, ipport);
		p2phandshake(pilot);
	}
	if(pilot->p2psamelan)	{
		// Active by definition if on the same network
		xivap.SendP2PReply(pilot->callsign, P2P_POS_PROTOCOL, 3, ipport);
		p2phandshake(pilot);
	}
}

void MultiplayerEngine::p2phandshake(MultiplayerPilot *pilot)
{
	static TLVPacket response;
	static char buf[64];

	response.reset();
	// lets say hello ;)
	response.setTLV(IVAO_PTPOS_Callsign, length(xivap.callsign)+1, pconst(xivap.callsign));
	response.setTV(IVAO_EchoRequest, pilot->RegisterEchoRequest()); // ping him, since we're at it...
	response.setTV(IVAO_PTPOS_CallsignReq, 0);
	pilot->lastp2phandshake_t = xivap.watch.getTime();

	if(pilot->p2psamelan)
		xivap.addText(colRed, "this peer is on same LAN, using internal IP", true, true);

	// use private ip+port if external IP is the same, public ip+port otherwise
	const ipaddress& peers_ip = (pilot->p2psamelan)
		? pilot->p2pip_int		// on same lan
		: pilot->p2pip;			// remote IP
	const int peers_port = (pilot->p2psamelan)
		? pilot->p2pport_int	// on same lan
		: pilot->p2pport;		// remote IP

	try {
		UInt16 len;
		if(response.assemble(buf, sizeof(buf), &len))
			_stunclient.udpSocket().sendto(buf, len, peers_ip, peers_port);
	} catch (estream* e) {
		xivap.addText(colRed, "Warning: " + e->get_message(), true, true);
		delete e;
	}

	pilot->p2pdeadcount++;
	xivap.addText(colWhite, "sent p2p handshake to " + pilot->callsign, true, true);
}

void MultiplayerEngine::HandlePlaneParams(MultiplayerPilot *pilot, FSD::PlaneParams params)
{
#ifdef PARAMS_DEBUG
	string debug = "PARAMS FOR " + pilot->callsign
		+ ": Gear:" + itostring(params.gear)
		+ " Bcn:" + itostring(params.beaconLight)
		+ " Lnd:" + itostring(params.landLight)
		+ " Nav:" + itostring(params.navLight)
		+ " Strb:" + itostring(params.strobeLight)
		+ " Txi:" + itostring(params.taxiLight)
		+ " ThRev:" + itostring(params.thrustReversers)
		+ " E1:" + itostring(params.engine1Running)
		+ " E2:" + itostring(params.engine2Running)
		+ " E3:" + itostring(params.engine3Running)
		+ " E4:" + itostring(params.engine4Running)
		+ " Flps:" + itostring(params.flapsRatio)
		+ " SpdBrk:" + itostring(params.speedbrakeRatio)
		+ " Thrtle:" + itostring(params.thrustRatio);
	xivap.addText(colGray, debug, true, true);
#endif

	pilot->p2psentparams_t = xivap.watch.getTime();

	pilot->surfaces.flapRatio			= params.flapsRatio / 8.0f;
	pilot->surfaces.gearPosition		= static_cast<float>(params.gear);

	// leave only the lights on that need to be on
	pilot->surfaces.lights.landLights = 0;
	pilot->surfaces.lights.navLights = 0;
	pilot->surfaces.lights.strbLights = 0;
	pilot->surfaces.lights.bcnLights = 0;
	if(params.beaconLight) pilot->surfaces.lights.bcnLights = 1;
	if(params.strobeLight) pilot->surfaces.lights.strbLights = 1;
	if(params.navLight) pilot->surfaces.lights.navLights = 1;
	if(params.landLight) pilot->surfaces.lights.landLights = 1;

										// extend slats as soon as flaps go down
	pilot->surfaces.slatRatio			= params.flapsRatio >= 1 ? 1.0f : 0.0f;

	pilot->surfaces.speedBrakeRatio	= params.speedbrakeRatio / 8.0f;
	pilot->surfaces.spoilerRatio		= params.speedbrakeRatio / 8.0f;
	pilot->surfaces.thrust				= params.thrustRatio / 8.0f;
	pilot->surfaces.wingSweep			= pilot->onground ? 0.8f : 0.0f;

	// wtf is that anyway?
	pilot->surfaces.yolkHeading = 0;
	pilot->surfaces.yolkPitch = 0;

	/*
	string str = "gear " + itostring(params.gear) +
		" landL " + itostring(params.landLight) + 
		" navL " + itostring(params.navLight) + 
		" strbL " + itostring(params.strobeLight) + 
		" bcnL " + itostring(params.beaconLight) + 
		" taxiL " + itostring(params.taxiLight) + 
		" E1 " + itostring(params.engine1Running) + 
		" E2 " + itostring(params.engine2Running) + 
		" E3 " + itostring(params.engine3Running) + 
		" E4 " + itostring(params.engine4Running) + 
		" flaps " + itostring(params.flapsRatio) + 
		" spdbr " + itostring(params.speedbrakeRatio) + 
		" thrst " + itostring(params.thrustRatio) + 
		" rev " + itostring(params.thrustReversers);
	xivap.addText(colGray, "MP: Params for " + callsign + ": " + str, true, true);
	*/
}

void MultiplayerEngine::ProcessPPOS1Interval(MultiplayerPilot *pilot, int interval)
{
	pilot->p2pmaxrate_send = interval;
	if(pilot->p2pmaxrate_send_real > interval)
		pilot->p2pmaxrate_send_real = interval;
}

void MultiplayerEngine::ProcessPPOS1Pos(MultiplayerPilot *pilot, const char *buffer, int buflen)
{
	if(buflen < 5*sizeof(UInt32))
		return;

	/*
	Content: 3D position: Lat|Lon|Alt|PBH|timestamp
		Latitude, South is negative (float, 32bit)
		Longitude, West is negative (float, 32bit)
		Altitude in feet MSL (float, 32bit)
		PBH according FSD specification (Uint32)
		Timestamp (Uint32)
	*/
	const char *p = buffer;

	// dirty trick to bring the float to host byte order...
	UInt32 val = ntohl(*(UInt32*)p);
	float packet_lat = *((float*)&val);
	p += sizeof(UInt32);

	val = ntohl(*(UInt32*)p);
	float packet_lon = *((float*)&val);
	p += sizeof(UInt32);

	val = ntohl(*(UInt32*)p);
	float packet_alt = *((float*)&val);
	p += sizeof(UInt32);

	UInt32 packet_pbh = ntohl(*(UInt32*)p);
	p += sizeof(UInt32);

	UInt32 timestamp = ntohl(*(UInt32*)p);
	p += sizeof(UInt32);

	// do some sanity checks:

	// packet is older than the last one received (messed up packet sequence)
	if(timestamp <= pilot->p2plastpostime_t)
		return;
	
    UInt32 now_ = xivap.watch.getTime();

	// we should have all data now... ----------------------

	PlanePosition *pos = new PlanePosition();
	pos->pos.lat = packet_lat;
	pos->pos.lon = packet_lon;
	pos->pos.elevation = packet_alt;
	FsdAPI::decodePBH(packet_pbh, pos->pos.pitch, pos->pos.roll, pos->pos.heading, pilot->onground);
	pos->onground = pilot->onground;
	pos->p_timestamp = timestamp; // time value of received packet (peers timestamp)
	pos->timestamp = now_;
	pos->latency = pilot->p2platency;
	// process it
	pilot->interpolator.takeNewPosition(pos, pilot->distance);

	pilot->p2plastpostime_t = timestamp;
	pilot->p2plastposseen_t = now_;

	static char packetbuf[512];
	{
		
		// if peer is far away, tell him to stop sending updates
		double altdiff = pilot->interpolator.lastKnownPosition().pos.elevation - xivap.elevationft();
		if(altdiff < 0) altdiff *= -1.0;

		// see if close enough...
		pilot->CalcDistance(xivap.GetLat(), xivap.GetLon());
		if(pilot->distance > P2P_RANGE * 2 || altdiff > P2P_ALTDIFF * 2) {
			// send STFU here
			TLVPacket packet;
			packet.setTV(IVAO_PTPOS_Interval, 0);
			UInt16 len;
			try {
				if(packet.assemble(packetbuf, 511, &len))
					_stunclient.udpSocket().sendto(packetbuf, len, pilot->p2pip, pilot->p2pport);
				xivap.addText(colWhite, "Sent STFU to " + pilot->callsign, true, true);
			} catch (estream* e) {
				xivap.addText(colRed, "ERROR: " + e->get_message(), true, true);
				delete e;
			}
		}
	}
}

void MultiplayerEngine::ProcessPPOS1Params(MultiplayerPilot *pilot, const char *buffer, int buflen)
{
	if(buflen < sizeof(UInt32))
		return;

	UInt32 p = ntohl(*(UInt32*)buffer);
	FSD::PlaneParams params;
	params.params = p;
	HandlePlaneParams(pilot, params);
	//pilot->lastp2ptimestamp = xivap.watch.getTime();
}

void MultiplayerEngine::frameTick()
{
	// this thing is called once per frame. keep it short.

	if(!(_initialized && _enabled)) return;

	static UInt32 nextp2pkeepalive = 0;
	static UInt32 nextp2pping = 0;
	static float nextFrequencyCalc = 0.0f;
	UInt32 now_ = xivap.watch.getTime();

	if(_p2pmode >= 1) { // do this stuff only if p2p is active

		// send pings every n seconds
		if(now_ > nextp2pkeepalive) {
			nextp2pkeepalive = now_ + P2P_KEEPALIVE_INTERVAL;
			_stunclient.SendKeepalive();
		}

		if(now_ > nextp2pping) {
			// send ping to peers here
			nextp2pping = now_ + P2P_PING_INTERVAL;
			pingPeers();
		}

		// once in a while (=every 5 seconds), re-calculate max. frequency
		// requests and send them to the peers
		if(XPLMGetElapsedTime() > nextFrequencyCalc + P2P_FREQUENCY_INTERVAL) {
			nextFrequencyCalc = XPLMGetElapsedTime();

			// update peer frequency information
			calcPeerFrequencies();
		}

		static UInt32 nextp2psendpackets = now_;
		if(now_ >= nextp2psendpackets + 1000000 / (P2P_MAX_RATE*3)) // *3 because of aliasing
		{
			// send pos. packets to peers
			if(sendPeerPositions())
				nextp2psendpackets = now_;
		}	

		// poll peers
		pollPeers();
	}

	// remove idle pilots
	static float corpse_timeout = XPLMGetElapsedTime();
	if(corpse_timeout + FSD_PILOT_TIMEOUT < XPLMGetElapsedTime()) {
		removeCorpses();
		corpse_timeout = XPLMGetElapsedTime();
	}
}

// returns false if nothing was sent due to wrong timing
bool MultiplayerEngine::sendPeerPositions()
{
	bool updated = false;
	bool send = false;
	bool sendparams = false;
	bool assembled = false;
	static TLVPacket packet;
	static char buf[64];
	static char asmbuf[64];
	UInt16 len;

	const FSD::PlaneParams& myParams = xivap.updateParams();
	UInt32 par = htonl(myParams.params);
	UInt32 now_ = xivap.watch.getTime();

	static PlanePosition currentPos;

	for(PlaneMap::iterator i = _planes.begin(); i != _planes.end(); ++i) {
		MultiplayerPilot& p = *(i->second);

		// dont send pos update if...
		if(p.p2pmode == 0							// peer does not use p2p
			|| (p.p2pmode == 1 && !p.p2psamelan)	// peer is blocked and not on LAN
			|| p.p2pport == 0						// dont have p2p info
			|| p.p2pmaxrate_send_real < 1)			// peer doesnt want to get traffic
			continue;

		send = sendparams = false;

		if(p.p2psentparams_t + 5000000 < now_ || p.p2plastposseen_t == 0) {
			// send params and pos at least once in 5 seconds
			// this will also send a packet when none has been sent before
			sendparams = true;
			send = true;
		}

		// p.p2pmaxrate_send_real is the max. frequency at which
		// we should be sending pos. packets to this guy
		if(p.p2plastpossent_t + (1000000 / p.p2pmaxrate_send_real) < now_)
			send = true;

		if(send) {
			// if we would be sending a packet, check if it is really necessary
			// to send it based on error prediction - that way we won't be sending
			// unnecessary updates (saves a LOT of traffic for parking aircraft
			// at crowded airports etc.)

			if(!updated) {
				xivap.updatePosition(&currentPos); // update only if really needed...
				updated = true; // dont do this each frame
			}

			send = p.errorPredictor.errorTooLarge(now_ + p.p2platency, currentPos);
			//if(!send)
			//	xivap.addText(colGreen, "not sending - small error", true, true);
		}

		// if for some reason we did not send an update within 2 secs...
		if(now_ > p.p2plastpossent_t + 2000000)
			send = true;

		if(!send)
			continue;

		// if we didnt do it already for this frame, create the TLVpacket
		if(!updated) {
			xivap.updatePosition(&currentPos); // update only if really needed...
			updated = true; // dont do this each frame
		}

		if(!sendparams) {
			if(myParams.params != p.p2psentparams) {
				// this guy has an old params status, resend it
				p.p2psentparams = myParams.params;
				if(!sendparams)
					assembled = false;
				sendparams = true;
			} else {
				if(sendparams) {
					sendparams = false;
					assembled = false;
				}
			}
		}

		if(!assembled) {
			// packet content has changed, need to reassemble it

			/*
			Content: 3D position: Lat|Lon|Alt|PBH|timestamp
				Latitude, South is negative (float, 32bit)
				Longitude, West is negative (float, 32bit)
				Altitude in feet MSL (float, 32bit)
				PBH according FSD specification (Uint32)
				Timestamp (Uint32)
			*/

			// dirty trick to bring the float to network byte order...
			char *pt = buf;
			*(float*)pt = htonf((float)xivap.GetLat());
			pt += sizeof(UInt32);

			*(float*)pt = htonf((float)xivap.GetLon());
			pt += sizeof(UInt32);

			*(float*)pt = htonf((float)xivap.elevationft());
			pt += sizeof(UInt32);

			FSD::FS_PBH pbh;
			pbh.pbh = 0;
			pbh.pitch = static_cast<int>(xivap.GetPitch() * PITCH_MULTIPLIER);
			pbh.bank = static_cast<int>(xivap.GetBank() * BANK_MULTIPLIER);
			pbh.hdg = static_cast<int>(xivap.GetHeading() * HDG_MULTIPLIER);
			if(xivap.onGround()) pbh.onground = 1;

			*(UInt32*)pt = htonl(pbh.pbh);
			pt += sizeof(UInt32);

			*(UInt32*)pt = htonl(xivap.watch.getTime());
			pt += sizeof(UInt32);

			// ready to rock&roll
			packet.reset();
			packet.setTLV(IVAO_PTPOS_Position, (UInt16)(pt - buf), buf);

			if(sendparams) {
				packet.setTLV(IVAO_PTPOS_Params, sizeof(UInt32), (char*)&par);
				p.p2psentparams_t = now_;
			}

			assembled = true;
		}

		try {
			if(packet.assemble(asmbuf, 63, &len))
				_stunclient.udpSocket().sendto(asmbuf, len, p.p2pip, p.p2pport);
		} catch (estream* e) {
			xivap.addText(colRed, "Warning: " + e->get_message(), true, true);
			delete e;
		}

		p.p2plastpossent_t = now_;

		// pass a copy of the current position to the error prediction interpolator
		p.errorPredictor.takeNewPosition(new PlanePosition(currentPos), p.distance);
	}
	return true;
}

void MultiplayerEngine::calcPeerFrequencies()
{
	double weightSum = 0;
	int recvsum = 0;

	for(PlaneMap::iterator i = _planes.begin(); i != _planes.end(); ++i) {
		MultiplayerPilot& p = *(i->second);

		if(p.p2pmode < 1 || p.p2pport == 0)
			continue;

		// full rate for peers within 0.6nm
		double rate = (P2P_RANGE - p.distance + 0.6) / P2P_RANGE;
		if(rate < 0) rate = 0;
		if(rate > 1) rate = 1;

		double altdiff = p.interpolator.lastKnownPosition().pos.elevation - xivap.elevationft();
		if(altdiff < 0) altdiff *= -1.0;

		// full rate for peers within +- 1200ft
		double rate2 = (P2P_ALTDIFF - altdiff + 1200) / P2P_ALTDIFF;
		if(rate2 < 0) rate2 = 0;
		if(rate2 > 1) rate2 = 1;
		rate *= rate2;

		p.p2prate = rate;
		weightSum += rate;
		recvsum += p.p2pmaxrate_recv;
	}

	double recvFactor = 1;
	double sendFactor = 1;

	// calculate the amount of bytes per second we would be sending at that rate
	double bps = weightSum * P2P_MAX_RATE * (PTPOS_POSITION_SIZE + 5); // 5 extra bytes just to be sure (pings etc)

	if(bps > p2psendbps) // i send too much!
		sendFactor = p2psendbps / bps; // scale it down

	// calculate the amount of traffic coming to us
	bps = recvsum * PTPOS_POSITION_SIZE;
	if(bps > p2precvbps) // i receive too much!
		recvFactor = p2precvbps / bps; // scale it down

	// update peers with their new maxima
	for(PlaneMap::iterator i = _planes.begin(); i != _planes.end(); ++i) {
		MultiplayerPilot& p = *(i->second);

		if(p.p2pmode < 1 || p.p2pport == 0)
			continue;

		// that much I send to HIM
		p.p2pmaxrate_send_real = (int)(P2P_MAX_RATE * p.p2prate * sendFactor);
		if(p.p2pmaxrate_send_real > p.p2pmaxrate_send && p.p2pmaxrate_send >= 0)
			p.p2pmaxrate_send_real = p.p2pmaxrate_send;

		// that much HE is allowed to send to ME
		int sendmax = (int)(P2P_MAX_RATE * recvFactor * p.p2prate);

		if(sendmax != p.p2pmaxrate_recv) {
			static TLVPacket packet;
			static char buf[64];
			packet.reset();
			packet.setTV(IVAO_PTPOS_Interval, sendmax);
			UInt16 len;
			try {
				if(packet.assemble(buf, 63, &len))
					_stunclient.udpSocket().sendto(buf, len, p.p2pip, p.p2pport);
			} catch (estream* e) {
				xivap.addText(colRed, "Warning: " + e->get_message(), true, true);
				delete e;
			}
			p.p2pmaxrate_recv = sendmax;
			xivap.addText(colWhite, "P2P sent interval " + itostring(sendmax) + " to " + p.callsign,
				true, true);
		}
	}
}

void MultiplayerEngine::pingPeers()
{
	// ping all p2p peers
	// to determine latency and prevent the p2p
	// connection from timing out
	if(_p2pmode < 1)
		return;

	static TLVPacket packet;
	static char buf[512];
	int count = 0; // avoid sending 1000 echo requests each time

	UInt32 now_ = xivap.watch.getTime();

	for(PlaneMap::iterator i = _planes.begin(); i != _planes.end() && count < 10; ++i) {
		bool send = false;

		// re-initiate handshakes...
		if(i->second->p2pdeadcount > 0
			&& i->second->p2pdeadcount < P2P_MAX_ATTEMPTS
			&& i->second->lastp2phandshake_t + P2P_PING_INTERVAL < now_)
		{
			// lets say hello again...
			packet.setTLV(IVAO_PTPOS_Callsign, length(xivap.callsign)+1, pconst(xivap.callsign));
			packet.setTV(IVAO_PTPOS_CallsignReq, 0);
			i->second->lastp2phandshake_t = now_;
			i->second->p2pdeadcount++;
			send = true;
		}

		// send pings
		if(i->second->p2pmode > 1
			&& i->second->p2pport > 0
			&& i->second->lastp2pping_t + P2P_PING_INTERVAL < now_
			&& i->second->p2pdeadcount < P2P_MAX_ATTEMPTS
			&& i->second->distance < P2P_RANGE * 1.5) {

			packet.setTV(IVAO_EchoRequest, i->second->RegisterEchoRequest()); // not using the timestamp there

			++count;
			send = true;
		}

		if(send) {
			UInt16 len;
			try {
				if(packet.assemble(buf, 511, &len))
					_stunclient.udpSocket().sendto(buf, len, i->second->p2pip, i->second->p2pport);
			} catch (estream* e) {
				xivap.addText(colRed, "Warning: " + e->get_message(), true, true);
				delete e;
			}
			packet.reset();
		}
	}
}

void MultiplayerEngine::pollPeers()
{
	// PPOS1 handling loop
	// this loop handles all possible ppos1 packets that could
	// be sent from p2p peers.

	bool haveData = true;
	while(haveData) {
		try {
			haveData = _stunclient.udpSocket().poll();
		} catch (estream* e) {
			xivap.addText(colRed, "Warning: " + e->get_message(), true, true);
			haveData = false;
			delete e;
		}

		if(!haveData) continue;

		static char buf[512];
		static char reply_buf[512];
		static ipaddress ip;
		static int port;
		static TLVPacket packet, response;
		bool sendResponse = false;

		string sender = "(unknown)";

#ifdef P2P_DEBUG
		string desc = ""; // for debugging, to see whats going on
#endif
		int size = 0;
		try {
			size = _stunclient.udpSocket().receive(buf, 511, ip, port);
		} catch (estream* e) {
			xivap.addText(colRed, "Warning: " + e->get_message(), true, true);
			delete e;
		}
		if(size < 4) continue;

		packet.setBuffer(buf, size);
		
		if(packet.hasEntry(IVAO_EchoRequest)) {
			// immediately send echo reply
			sendResponse = true;
			response.setTV(IVAO_EchoReply, packet.getValue(IVAO_EchoRequest));
#ifdef P2P_DEBUG
			desc += "EchoRequest ";
#endif
		}

		if(packet.hasEntry(IVAO_PTPOS_CallsignReq)) {
			sendResponse = true;
			response.setTLV(IVAO_PTPOS_Callsign, length(xivap.callsign)+1, pconst(xivap.callsign));
#ifdef P2P_DEBUG
			desc += "CallsignRequest ";
#endif
		}

		if(packet.hasEntry(IVAO_PTPOS_Callsign)) {
#ifdef P2P_DEBUG
			desc += "Callsign ";
#endif

			string peer_callsign = "";
			// just a lil integrity check...
			int len = packet.getLength(IVAO_PTPOS_Callsign);
			if(len < 30 && len > 1) {
				// check for terminating \0 too
				if(packet.getContent(IVAO_PTPOS_Callsign)[len-1] == 0) {
					peer_callsign = packet.getContent(IVAO_PTPOS_Callsign);
					PlaneMap::iterator p = _planes.find(STDSTRING(peer_callsign));
					if(p != _planes.end()) {
						// yay! found him - remember his IP and port (for later identification)
						p->second->p2pip = p->second->p2pip_int = ip;
						//xivap.addText(colWhite, "SET IP TO " + iptostring(ip), true, true);
						p->second->p2pport = p->second->p2pport_int = port;
						p->second->didseep2ptraffic = true;
						p->second->p2pdeadcount = 0;
						sender = p->second->callsign;
					}
				}
			}
		}

		if(sendResponse) {
			UInt16 response_size;
			try {
				if(response.assemble(reply_buf, 511, &response_size))
					_stunclient.udpSocket().send(reply_buf, response_size);
			} catch (estream* e) {
				xivap.addText(colRed, "ERROR: " + e->get_message(), true, true);
				delete e;
			}
			response.reset();
		}
		
		PlaneMap::iterator pilotIter = findByPeerInfo(ip, port);
		if(pilotIter == _planes.end()) continue;

		pilotIter->second->didseep2ptraffic = true;
		pilotIter->second->p2pdeadcount = 0;
		sender = pilotIter->second->callsign;

		if(packet.hasEntry(IVAO_EchoReply)) {
			pilotIter->second->HandleEchoReply(packet.getValue(IVAO_EchoReply));
#ifdef P2P_DEBUG
			desc += "EchoReply: " + itostring((int)(pilotIter->second->p2platency)) + " ";
#else
			// always log ping times to logfile
			//Jens: disable logToScreen, only logfile
			xivap.addText(colWhite, "Latency to " + sender + ": " +
				itostring((int)(pilotIter->second->p2platency)), false, true);
#endif
		}

		// update interval
		if(packet.hasEntry(IVAO_PTPOS_Interval)) {
#ifdef P2P_DEBUG
			desc += "Interval " + itostring(packet.getValue(IVAO_PTPOS_Interval)) + " ";
#endif
			ProcessPPOS1Interval(pilotIter->second, packet.getValue(IVAO_PTPOS_Interval));
		}

		// position
		if(packet.hasEntry(IVAO_PTPOS_Position)) {
			ProcessPPOS1Pos(pilotIter->second, 
				packet.getContent(IVAO_PTPOS_Position),
				packet.getLength(IVAO_PTPOS_Position));
		}

		// params
		if(packet.hasEntry(IVAO_PTPOS_Params)) {
#ifdef P2P_DEBUG
			desc += "Params ";
#endif
			ProcessPPOS1Params(pilotIter->second, 
				packet.getContent(IVAO_PTPOS_Params),
				packet.getLength(IVAO_PTPOS_Params));
		}

#ifdef P2P_DEBUG
		if(desc != "")
			xivap.addText(colWhite, "P2P from " + sender + " " + desc, true, true);
#endif
	}
}
