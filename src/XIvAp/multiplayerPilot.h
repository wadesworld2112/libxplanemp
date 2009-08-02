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

#ifndef MULTIPLAYER_PILOT
#define MULTIPLAYER_PILOT

#include "common.h"
#include "planeposition.h"
#include "interpolator.h"
//#include "XPLMMultiplayer.h"

namespace Multiplayer {

class MultiplayerPilot {
public:
	MultiplayerPilot();
	~MultiplayerPilot();

	/** reset to factory default ;) */
	void init();

	// set lighting pattern (strobes) pattern for this aircraft type
	void setLightPattern(string icao);

	// initializers for XPlane data structures
	//static void initPosition(XPMPPlanePosition_t& pos);
	static void initSurfaces(XPMPPlaneSurfaces_t& surfaces);

	bool ignoreLegacyUpdates();	// returns true if FSD pos. packets should be ignored

	int RegisterEchoRequest();			// start timer for ping-pong
	void HandleEchoReply(int serial);	// stop timer for ping-pong

	/** The aircrafts current position */
	PlanePosition position;
	bool onground;	// true if the pilot is on ground

	/** The current aircraft configuration */
	XPMPPlaneSurfaces_t surfaces;

	/** Radar status - xpdr code and mode */
	XPMPPlaneRadar_t radarStatus;
	
	string callsign, mtl;	// callsign and MTL code
	bool mtlRequested; // true if we already requested the MTL for this guy
	bool XPregistered; // true if this aircraft is registered with XPlane
	XPMPPlaneID id;	// the XPlane ID

	UInt32 timestamp;	// when did this guy send his last lifesign

	int posFrameCalculated; // frame# of last calculation

	float lastFsdPos_t; // when did this guy send his last FSD position update
						// (used to determine if he ran so far away that we don't get his updates any more)

	// p2p information
	int p2pmode;				// peers p2p mode: 0=off 1=firewalled 2=passive 3=active
	ipaddress p2pip, p2pip_int;	// public and private IP to reach peer
	int p2pport, p2pport_int;	// public and private port to reach peer
	bool p2psamelan;			// true if peer is on same lan

	int p2pmaxrate_send; // I TO HIM: that is the interval he told me is the maximum he wants
	int p2pmaxrate_recv; // HE TO ME: how much does HE send TO ME
	int p2pmaxrate_send_real; // that is the interval at which i'll actually send updates

	UInt32 p2plastpossent_t; // timestamp of last pos. packet sent to peer
	UInt32 p2plastposseen_t; // when did we see peers last p2p pos update
	UInt32 p2plastpostime_t; // timestamp contained in last p2p pos update from peer;

	UInt32 p2psentparams; // last params sent to this peer
	UInt32 p2psentparams_t; // ...and when we did that

	double p2prate;		// value between 0 and 1 that determines the _recv rate
						// (for data rate calculations)

	bool p2prequested;	// did we request this guys p2p information? (via FSD)
	bool didrequestp2p;	// did that guy request our p2p information? (via FSD)

	bool didseep2ptraffic; // will be set to true as soon as the peer sent a packet
	int p2pdeadcount; // number of attempts to initiate a connection

	UInt32 lastp2pping_t;		// when did we send the last PING to peer
	UInt32 p2platency;			// current estimate of p2p round trip to peer
	UInt32 lastp2phandshake_t;	// when did we send the last p2p handshake to peer


	// other stuff...
	double distance;			// distance to peer in nm
	double CalcDistance(double lat, double lon); // guess what...

	// the crystal ball...
	Interpolator interpolator;

	// same as above - but this one is used to predict errors
	Interpolator errorPredictor;

	bool calculateFrame();

private:
	int pingSerial;

};

} // namespace Multiplayer

#endif
