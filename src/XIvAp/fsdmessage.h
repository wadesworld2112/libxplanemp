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

#ifndef FSDMESSAGE_H_
#define FSDMESSAGE_H_

#include "common.h"
#include "FSDprotocol.h"
#include <vector>

namespace FSD {

enum SimulatorType {
	SIM_UNKNOWN = -1,
	// Microsoft Flightsims
	SIM_FS95   = 1,
	SIM_FS98   = 2,
	SIM_FS7    = 2,
	SIM_CFS    = 3,
	SIM_FS2000 = 4,
	SIM_CFS2   = 5,
	SIM_FS2002 = 6,
	SIM_FS8    = 6,
	SIM_CFS3   = 7,
	SIM_FS2004 = 8,
	SIM_FS9    = 8,
    // Other Flightsims
	SIM_XPLANE = 11,
	SIM_PS1    = 15,
	SIM_FLY    = 20,
	SIM_FLY2   = 21,
	SIM_FGEAR  = 25
};

enum SquawkMode {
	SQ_UNKNOWN = ' ',
	SQ_IDENT   = 'Y',
	SQ_CHARLIE = 'N',
	SQ_STANDBY = 'S'
};

enum AtcRating {
	ATC_UNKNOWN    = -1,
	ATC_NOTALLOWED = 0, /// No use in FSD yet
	ATC_OBS        = 1, /// Observer
	ATC_S1         = 2, /// Student 1
	ATC_S2         = 3, /// Student 2
	ATC_S3         = 4, /// Student 3
	ATC_C1         = 5, /// Controller 1
	ATC_C2         = 6, /// Controller 2
	ATC_C3         = 7, /// Controller 3
	ATC_I1         = 8, /// Instructor 1
	ATC_I2         = 9, /// Instructor 2
	ATC_I3         = 10 /// Instructor 3
};

enum PilotRating {
	PILOT_UNKNOWN    = -1,
	PILOT_NOTALLOWED = 0, /// No use in FSD yet
	PILOT_OBS        = 1, /// No use in FSD yet
	PILOT_SFO        = 2, /// Second Flight Officer
	PILOT_FFO        = 3, /// First Flight Officer
	PILOT_CAP        = 4, /// Captain
	PILOT_FC         = 5, /// Flight Captain
	PILOT_SC         = 6, /// Senior Captain
	PILOT_SFC        = 7, /// Senior Flight Captain
	PILOT_CC         = 8, /// Commercial Captain
	PILOT_CFC        = 9, /// Commercial Flight Captain
	PILOT_CSC        = 10, /// Commercial Senior Captain
	PILOT_SHOWADM    = 11
};

enum AdminRating {
	ADM_SUSPENDED  = 0,
	ADM_USER       = 2,
	ADM_SUP        = 11,
	ADM_ADMIN      = 12
};

static const char* BCAST_ALL   = "*";
static const char* BCAST_PILOT = "*P";
static const char* BCAST_ATC   = "*A";
static const char* BCAST_SUP   = "*S";

enum FlightType {
	FTYPE_UNKNOWN = ' ',
	FTYPE_IFR     = 'I',
	FTYPE_VFR     = 'V',
	FTYPE_IFRVFR  = 'Y',
	FTYPE_VFRIFR  = 'Z'
};

enum Error {
	ERR_NO          = 0,
	ERR_CSINUSE     = 1,  /// Callsign in use
	ERR_CSINVALID   = 2,  /// Invalid callsign
	ERR_REGDONE     = 3,  /// Already registered
	ERR_SYNTAX      = 4,  /// Syntax error
	ERR_INVSOURCE   = 5,  /// Invalid source in packet
	ERR_INVVIDPASS  = 6,  /// Invalid VID/Pass combination
	ERR_NOCS        = 7,  /// No such callsign
	ERR_NOFP        = 8,  /// No flightplan
	ERR_NOWP        = 9,  /// No such weatherprofile
	ERR_INVPROTOCOL = 10, /// Invalid Protocol
                          /// -> 11 was requested level too high -> not possible anymore
	ERR_SERVFULL    = 12, /// Server is full
	ERR_SUSPENDED   = 13, /// Client is suspended
	ERR_INVPOS      = 14  /// Invalid position update
};

enum Facility {
	FAC_OBS = 0,
	FAC_FSS = 1,
	FAC_DEL = 2,
	FAC_GND = 3,
	FAC_TWR = 4,
	FAC_APP = 5,
	FAC_CTR = 6
};

enum WeatherType {
	WX_METAR = 0,
	WX_TAF = 1,
	WX_SHORTTAF = 2
};

#ifdef __ppc__
union FS_PBH {
	unsigned int pbh;
	struct {
		int pitch             : 10;
		int bank              : 10;
		unsigned int hdg      : 10;
		unsigned int onground : 1;
		unsigned int unused   : 1;
	};
};
#else
union FS_PBH {
	unsigned int pbh;
	struct {
		unsigned int unused   : 1;
		unsigned int onground : 1;
		unsigned int hdg      : 10;
		int bank              : 10;
		int pitch             : 10;
	};
};
#endif

#ifdef __ppc__
union PlaneParams {
	unsigned int params;
	struct {
		unsigned int unused				: 12;
		unsigned int thrustReversers	: 1;
		unsigned int thrustRatio		: 3;
		unsigned int speedbrakeRatio	: 3;
		unsigned int flapsRatio			: 3;
		unsigned int engine4Running		: 1;
		unsigned int engine3Running		: 1;
		unsigned int engine2Running		: 1;
		unsigned int engine1Running		: 1;
		unsigned int taxiLight			: 1;
		unsigned int beaconLight		: 1;
		unsigned int strobeLight		: 1;
		unsigned int navLight			: 1;
		unsigned int landLight			: 1;
		unsigned int gear				: 1;
	};
};
#else
union PlaneParams {
	unsigned int params;
	struct {
		unsigned int gear				: 1;
		unsigned int landLight			: 1;
		unsigned int navLight			: 1;
		unsigned int strobeLight		: 1;
		unsigned int beaconLight		: 1;
		unsigned int taxiLight			: 1;
		unsigned int engine1Running		: 1;
		unsigned int engine2Running		: 1;
		unsigned int engine3Running		: 1;
		unsigned int engine4Running		: 1;
		unsigned int flapsRatio			: 3;
		unsigned int speedbrakeRatio	: 3;
		unsigned int thrustRatio		: 3;
		unsigned int thrustReversers	: 1;

		unsigned int unused				: 12;

	};
};
#endif

enum WindTurb {
	TURB_NONE		= 0,
	TURB_OCCASIONAL	= 1,
	TURB_LIGHT		= 2,
	TURB_MODERATE	= 3,
	TURB_SEVERE		= 4
};

enum WindShear {
	SHEAR_GRADUAL	= 0,
	SHEAR_MODERATE	= 1,
	SHEAR_STEEP		= 2,
	SHEAR_INSTANT	= 3
};

enum CloudType {
	CLOUD_NONE			= 0,
	CLOUD_CIRRUS		= 1,
	CLOUD_CIRROSTRATUS	= 2,
	CLOUD_CIRROCUMULUS	= 3,
	CLOUD_ALTOSTRATUS	= 4,
	CLOUD_ALTOCUMULUS	= 5,
	CLOUD_STRATOCUMULUS	= 6,
	CLOUD_NIMBOSTRATUS	= 7,
	CLOUD_STRATUS		= 8,
	CLOUD_CUMULUS		= 9,
	CLOUD_THUNDERSTORM	= 10
};

enum CloudTop {
	CLTOP_FLAT			= 0,
	CLTOP_ROUND			= 1,
	CLTOP_ANVIL			= 2,
	CLTOP_MAX			= 3
};

enum CloudPrecip {
	PRECIP_NONE			= 0,
	PRECIP_RAIN			= 1,
	PRECIP_SNOW			= 2
};

enum TcasResolution	{
	TCAS_UNKNOWN  = ' ',
	TCAS_CLIMB    = 'C',
	TCAS_DESCEND  = 'D',
	TCAS_MAINTAIN = 'M'
};

static const char* INFO_INF    = _FSD_INFOREQ_INF_;
static const char* INFO_ATIS   = _FSD_INFOREQ_ATIS_;
static const char* TEXTTO_SUP  = "*S";

string nextToken(string& str);

class Message {
	public:
		Message(const string& str = "");

		string compose(bool colon_check = true);
		bool decompose(string str);

		string encoded, source, dest;
		int type;
		std::vector<string> tokens;
    };

} // namespace FSD

#endif
