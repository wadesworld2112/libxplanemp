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

#ifndef XIVAP_H_
#define XIVAP_H_

#include "common.h"
#include "fsdapi.h"
#include "connectForm.h"
#include "disconnectForm.h"
#include "flightplanForm.h"
#include "FMCform.h"
#include "uiWindow.h"
#include "atcPosition.h"
#include "weatherPosition.h"
#include "weatherGod.h"
#include "multiplayer.h"
#include "HTTPClient.h"
#include "messageWindow.h"
#include "chatWindow.h"
#include "aircraftDB.h"
#include "airlinesDB.h"
#include "XPmessageBox.h"
#include "refInspector.h"
#include "navQuery.h"
#include "WristWatch.h"
#include "planeposition.h"

#include "TeamSpeak.h"

#include <deque>

#define CONSOLE_LINES   28
#define CONSOLE_ROWS   150

// min. time span between sending 2 param packets
// DO NOT CHANGE - p2p pos updates depend on that value being 5
// (otherwise the framenumber calculation is b0rk3d)
#define PARAMS_MINDELAY	5.0f

// check for new ATIS'es this often
#define RADIO_ATIS_CHECK_INTERVAL	1.2f



class Xivap {
public:
	/** Constructor */
	Xivap();

	/** Destructor */
	~Xivap();

	double nextFSDPosUpdate, nextFSDPoll, nextStatusUpdate;

	int	menuConnect, menuDisconnect, menuFlightplan;
	XPLMMenuID menuEntry;

	typedef std::pair<float*, string> line;
	std::deque<line> consoleText;

	FsdAPI fsd;
	Flightplan fpl;

	void XPluginStart();
	void XPluginStop();
	void XPluginEnable();
	void XPluginDisable();
	void aircraftChange();
	void airportChange();

	void setAcType(const string& aircraft, const string& airline, const string& livery);

	void connect(const string& callsign, const string& vid, const string& password,
		const string& realname, bool hideadm, const string& server, const string& port,
		bool voice);
	void disconnect();

	void flightLoopCallback();
	void FSDPoll();
	void sendPosUpdate();
	void updateStatus();
	void updatePosition(PlanePosition* position = NULL);

	double elevationft() const { return el * 3.2808399; };
	double flightlevelft() const;
	bool onGround() const { return _onground; };

	void sendFlightplan();
	void addText(float* color, const string& text, bool logToScreen = true, bool logToFile = false);

	string comStr(int radio);
	string com1freq() { return freq2str(_com1freq); };
	string com2freq() { return freq2str(_com2freq); };

	string acType() { return _acType; };
	string deptAirport() { return _deptAirport; };
	string destAirport() { return _destAirport; };
	string flightRoute() { return _flightRoute; };

	// call this periodically
	// checks if one of the radios is tuned to a station for which we do not have an ATIS
	void checkRadioAtis();

	XPLMHotKeyID	uiWindowHotKey;
	XPLMWindowID	console, focusDumper;

	ConnectForm& connectForm();
	DisconnectForm& disconnectForm();
	FlightplanForm& flightplanForm();
	FMCForm& fmcForm();
	XPMessageBox& messageBox();
	RefInspector& refInspector();

	UiWindow uiWindow;
	MessageWindow msgWindow;
	std::vector<ChatWindow*> chatWindows;
	void closeChatWindow(ChatWindow* win);

	IdentMode xpdrModus() { return xpdrMode; };
	bool xpdrIdent() { return  xpdrMode == IdentID || _identOverride; };
	bool radioCoupled() { return _radioCoupled; };
	int activeRadio() { return _activeRadio; };

	void setRadioCoupled(bool value);
	void setComActive(int radio);

	void tuneCom(int radio, int freq);

	void setXpdrIdent();
	void xpdrModeToggle();

	bool online() { return fsd.connected() && fsd.verified(); };

	bool usingWeather() { return _useWeather; };
	void setWeather(bool value);
	bool usingVoice() { return _useVoice; };
	void setVoice(bool value);
	bool usingMultiplayer() { return _useMultiplayer; };
	void setMultiplayer(bool value);

	bool consoleVisible();
	void setConsoleVisible(bool value);

	bool autopilotOn() const;
	string autopilotState() const;

	int fontWidth, fontHeight;

	std::vector<AtcPosition> getAtcPositions(SortOrder sort);

	// process a user input line
	void processLine(string line);

	void dumpKeyboardFocus() { uiWindow.dumpKeyboardFocus(); };
	void grabKeyboardFocus() { uiWindow.grabKeyboardFocus(); };

	void handleCommand(string line);

	//double frameTiming, oldFrameTiming, highResolutionTime;
	int lastNullFrameTime;
	//static const int frameTimingFrames = 15;

	const FSD::PlaneParams& updateParams();
	UInt32 getParams();

	int renderWindowWidth() const { return _renderWindowWidth; };
	int renderWindowHeight() const { return _renderWindowHeight; };

	string realname, callsign;

	AircraftDB aircraftDB;
	AirlinesDB airlinesDB;

	// returns SVN revision number
	const string& revision() const { return _revision; };

	double GetLat() const { return lat; };
	double GetLon() const { return lon; };
	float GetSpeed() const { return speed; };
	float GetPitch() const { return pitch; };
	float GetBank() const { return bank; };
	float GetHeading() const { return heading; };

	bool posSent;

	void RequestP2P(const string& callsign, const string& protocol, int mode, const string& ipport);
	void SendP2PReply(const string& callsign, const string& protocol, int mode, const string& ipport);

	WristWatch watch;
	
	int p2pmode() const { return _multiplayer.p2pmode(); };

	int GetPlaneRes() { return _plane_resolution; };

private:

	string _lastPrivSender; // the callsign of the last station that sent a private message

	string _acType, _acAirline, _acLivery; // MTL
	string _deptAirport;
	string _destAirport;
	string _flightRoute;

	// Forms and Windows
	ConnectForm *_connectForm;
	DisconnectForm *_disconnectForm;
	FlightplanForm *_flightplanForm;
	bool _fplPrefiled, _fplSend;
	FMCForm *_fmcForm;
	XPMessageBox *_messageBox;
	RefInspector *_refInspector;

	// position packets
	XPLMDataRef gPlaneLat, gPlaneLon, gPlaneEl;
	XPLMDataRef gPlanePitch, gPlaneBank, gPlaneHeading, gPlaneSpeed, gPlaneAGL;
	XPLMDataRef gXpdrCode, gXpdrMode, gXpdrId;

	// panel integration
	XPLMDataRef gCom1, gCom2, gAudioSwitches, gBarometer, gIndicatedAlt;
	
	// autopilot
	XPLMDataRef gAPmode, gAPstate; 

	// rendering window
	int _renderWindowWidth, _renderWindowHeight;

	// plane params
	int numEngines;
	XPLMDataRef gGearDeploy;
	XPLMDataRef gLightBeacon, gLightLanding, gLightNav, gLightStrobe, gLightTaxi;
	XPLMDataRef gEngineMode, gEngineThrust, gEngineRunning;
	XPLMDataRef gFlapsDeploy, gSpeedBrakes;
	XPLMDataRef gAcfTailnum, gAcfDescription;
	
	double _lastParams;
	FSD::PlaneParams _planeParams;	// current plane parameter flags
	void sendPlaneParams();

	void handleTextMessage(const FSD::Message& m);
	void handleInfoReply(const FSD::Message& m);
	void handleError(const FSD::Message& m);
	void handleKill(const FSD::Message& m);
	void handleWxReply(const FSD::Message& m);
	void handleAtisReply(const FSD::Message& m);

	// process FSDs weather feed
	void handleWxData(const FSD::Message& m);

	void addATC(const FSD::Message& m);
	void delATC(const FSD::Message& m);
	void updateATC(const FSD::Message& m);

	void handleFPchange(const FSD::Message& m);

	string getActiveRadioFreq();
	int listeningToRadioFreq(string freq);

	// converts a integer frequency into a string
	string freq2str(int freq);

	// converts a integer frequency to atc station callsign if applicable
	string freq2name(int freq);

	// status variables
	double lat, lon, el, groundalt;
	float pitch, bank, heading, speed, agl;
	int xpdrCode;
	IdentMode xpdrMode;
	bool _onground;

	int _activeRadio; // 1 or 2
	bool _radioCoupled; // true if listening to both
	int _com1freq, _com2freq;

	bool _identOverride;
	double _setXpdrIdOff;
	AtcList _atcList;
	bool isVoiceUrl(string str);
	double _nextRadioAtisCheck;

	bool _useWeather;
	WxDB _weatherDB;
	WeatherGod _erwin;
	WxStation currentWxStation;
	double _nextWxCheck, _nextWxTransition;
	void checkWeather(double elapsed);

	bool _useVoice;
#ifdef HAVE_TEAMSPEAK
	TeamSpeak tsRemote;
	bool _tsLoaded;
	bool _tsTuned;
#endif

	Multiplayer::MultiplayerEngine _multiplayer;
	bool _useMultiplayer;

	HTTP::HTTPClient _HTTPclient;
	bool _downloadingServerStatus;

	string _revision;

	NavQuery _navQuery;

	string _stun_server;
	bool _p2p_enabled;
	int _p2p_forced_port;
	int _p2p_max_sendbps, _p2p_max_recvbps;

	int _plane_resolution;
	string _default_icao;
};

#endif
