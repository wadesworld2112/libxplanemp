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

#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include "common.h"
#include "fsdmessage.h"
#include "multiplayerPilot.h"
#include "interpolator.h"

#include "StunClient.h"

// pilots that did not send a FSD pos. update for that long will be removed
#define FSD_PILOT_TIMEOUT 30.0f

//#define STUN_SERVER "gaia.linuxpunk.org"
#define STUN_SERVER "stun.ivao.aero"
//#define STUN_SERVER "gaia.linuxpunk.org"

// use p2p packets within that radius/altitude band only
#define P2P_RANGE		10.0
#define P2P_ALTDIFF		3000.0

// number of attempts to try to connect a peer
#define P2P_MAX_ATTEMPTS 5

// do not accept more than this many updates per seconds
#define P2P_MAX_RATE 10

// default send/receive rates for p2p packets
// (in bytes per seconds)
#define P2P_DEFAULT_MAX_SENDBPS	1024
#define P2P_DEFAULT_MAX_RECVBPS	5000

#include <map>

namespace Multiplayer {

class MultiplayerEngine {
public:
	MultiplayerEngine();
	~MultiplayerEngine();

	/** This returns true if the multiplayer engine was initialized,
	 *  and false if not. If it is false, errorMessage() contains
	 *  a error description in human readable form.
	 */
	bool init(bool enablep2p = true,
		int maxsendBps = P2P_DEFAULT_MAX_SENDBPS,
		int maxrecvBps = P2P_DEFAULT_MAX_RECVBPS,
		int listenPort = 0,
		string defaultICAO = "A320");

	bool initialized() const { return _initialized; };

	/** This returns true if the multiplayer engine was enabled,
	 *  and false if not. If it is false, errorMessage() contains
	 *  a error description in human readable form.
	 */
	bool enable();
	void disable();
	bool enabled() const { return _enabled; };

	/** Returns the last error received from X-Plane */
	const string& errorMessage() const { return _errorMessage; };

	/**
	 * The callback function to update plane positions.
	 *
	 * Possible return values:
	 * xpmpData_Unavailable = 0,	The information has never been specified.
	 * xpmpData_Unchanged = 1,		The information from the last time the plug-in was aksed.
	 * xpmpData_NewData = 2		    The information has changed this sim cycle.
	 */
	XPMPPlaneCallbackResult planeCallback(XPMPPlaneID inPlane,
					   XPMPPlaneDataType inDataType,
					   void * ioData,
					   void * inRefcon);

	/** Remove all planes */
	void clear();

	/** Process pilot related packets */
	void eatThis(const FSD::Message &packet);

	// call this once per frame.
	void frameTick();

	// for CSL debugging
	void setAllModels(string aircraft, string airline, string livery);

	// perform p2p lookup
	void testp2p(string stunserver);
	
	int p2pmode() const { return _p2pmode; };

private:
	/** remove pilots who didnt send any FSD updates within the last FSD_PILOT_TIMEOUT seconds */
	void removeCorpses();

	string _errorMessage;
	bool _initialized, _enabled;

	string _defaultIcao;	// default aircraft code

	typedef std::map<std::string, MultiplayerPilot*> PlaneMap;
	PlaneMap _planes;

	/** Find a pilot by callsign, returns NULL if not found */
	XPMPPlaneID findByCallsign(const string& callsign);

	/** Find a pilot by callsign, returns NULL if not found */
	string findByID(const XPMPPlaneID& id);

	/** Find a pilot by p2p information */
	PlaneMap::iterator findByPeerInfo(const ipaddress& ip, int port);

	/** decodes a FSD position packet */
	static void decodeFSDPosition(const FSD::Message& packet,
							XPMPPlanePosition_t* position,
							XPMPPlaneRadar_t* radarstatus,
							bool& onground);

	/** splits a MTL string into its components.
	 * returns FALSE if the MTL string does not seem to be valid */
	static bool splitMTL(string mtl, string& aircraft, string& airline, string& livery);

	Interpolator _interpolator;
	Stun::StunClient _stunclient;
	int _p2pmode; // 0 = off, 1 = passive 2 = active
	bool p2penabled;
	int p2psendbps, p2precvbps, p2plistenport;

	void HandlePlaneParams(MultiplayerPilot *pilot, FSD::PlaneParams params);

	void ProcessPPOS1Interval(MultiplayerPilot *pilot, int interval);
	void ProcessPPOS1Pos(MultiplayerPilot *pilot, const char *buffer, int buflen);
	void ProcessPPOS1Params(MultiplayerPilot *pilot, const char *buffer, int buflen);

	void pingPeers();
	void pollPeers();
	void calcPeerFrequencies();

	// returns false if nothing was sent due to wrong timing
	bool sendPeerPositions();

	void handleP2Prequest(MultiplayerPilot *pilot, const FSD::Message &packet);
	void handleP2Preply(MultiplayerPilot *pilot, const FSD::Message &packet);

	void p2phandshake(MultiplayerPilot *pilot);
};

} // namespace Multiplayer

#endif
