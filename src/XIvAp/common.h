#ifndef COMMON_H_
#define COMMON_H_

// $Id: common.h 319 2006-05-22 20:22:10Z martin.domig $
// $Author: martin.domig $
// $Date: 2006-05-22 22:22:10 +0200 (Mon, 22 May 2006) $
// $Rev: 319 $
// $URL: https://svn.ivao.aero/svn/pilot/xsb/trunk/src/XIvAp/common.h $

#include <map>
#include <vector>
#include <string>

#ifndef WIN32
#include <inttypes.h>
#endif

// define some basic types
typedef unsigned char  UInt8;
typedef unsigned short UInt16;
#ifdef APPLE
typedef long unsigned int UInt32;
#else
typedef unsigned int   UInt32;
#endif

#include "ptypes.h"
#include "pasync.h"
#include "pinet.h"
#include "pport.h"
#include "pstreams.h"
#include "ptime.h"

USING_PTYPES

#include <stdio.h>
#include <string.h>
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMMenus.h"
#include "XPLMNavigation.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPUIGraphics.h"
#include "XPLMPlugin.h"

#include "ptpos.h"

#if 0
#undef ntohl
#undef ntohs
#undef htonl
#undef htons
#endif

inline float htonf( const float f )
{
    int i = htonl( *(int *)&f );
    return *(float *)&i;
}

inline float ntohf( const float f )
{
    int i = ntohl( *(int *)&f );
    return *(float *)&i;
}


// macro to convert a pt::string to std::string
#define STDSTRING(s) std::string(pconst(s))

// defines

#define _FSD_POLL_INTERVAL_			0.2f

#define SOFTWARE_NAME		"X-IvAp"
#define SOFTWARE_VERSION	"0.1.8"
#define SOFTWARE_VERSION_INT    18
#define SOFTWARE_COPYRIGHT	"(c) 2007 by Martin Domig, Kristof Provost, Andrew McGregor"
#define SOFTWARE_REVISION	"$Rev: 55 $"

#define UNICOM_NAME		"AIR-AIR"
#define UNICOM_FREQ		"123.450"
#define UNICOM_FREQ_NUM		12345
#define UNICOM_DIST		120

#define RESOURCES_DIR	"X-IvAp Resources"
#define CONFIG_FILE		"X-IvAp.conf"
#define STATIONS_FILE	"icao.dat"

#define FLIGHTPLANS_DIR	"Flightplans"
#define DEFAULT_FPL		"Default.fpl"

#define SERVERSTATUS_URL "http://www.ivao.aero/network/status.asp"
#define SERVERS_FILENAME "serverlist.txt"
#define OWN_SERVERS_FILENAME "myservers.txt"

// p2p settings

#define P2P_POS_PROTOCOL "PPOS1"

// will ping peers this often
#define P2P_PING_INTERVAL 3000000

// send keepalive to stun server this often
#define P2P_KEEPALIVE_INTERVAL 30000000

// interval (seconds) at which we re-calculate peer pos. frequencies
#define P2P_FREQUENCY_INTERVAL 5.0f

// atc positions become invalid after being dead for this time
#define ATCPOS_MAX_AGE 60.0f

// weather will be discarded after being dead for this time
#define WEATHER_MAX_AGE 60.f * 60.f  // 1h

// check for new weather stations this often
#define WEATHER_UPDATE_INTERVAL 30.0f

// use GLOB weather if the nearest wx station is more than this many nm away
#define WEATHER_GLOB_TRESHOLD 60

// if we are flying between 2 weather stations, switch to the closest station
// only if the difference in distance to the current station is greater than this
#define WEATHER_SWITCH_HYSTERESE 3

class Xivap;
extern Xivap xivap;

extern float colWhite[];
extern float colLightGray[];
extern float colBlue[];
extern float colLightBlue[];
extern float colRed[];
extern float colGreen[];
extern float colDarkGreen[];
extern float colYellow[];
extern float colCyan[];
extern float colGray[];


#ifdef WIN32
#define PLATFORM "Windows"
#define FSD_SOFTWARE_NAME "X-IvAp/win"

#pragma warning ( disable : 4311 )

#endif

#ifdef LINUX
#define PLATFORM "Linux"
#define FSD_SOFTWARE_NAME "X-IvAp/lin"
#endif

#ifdef APPLE
#define PLATFORM "Mac"
#define FSD_SOFTWARE_NAME "X-IvAp/mac"
#endif

// uncomment this line to compile a private beta
// #define INTERNAL_BETA

// comment this to compile a public release
// #define BETA

#define EXPIRE_YEAR		2005
#define EXPIRE_MONTH	11
#define EXPIRE_DAY		1

#ifdef INTERNAL_BETA
// warn on release builds
#ifndef BETA
#define BETA
#endif
#pragma message( "WARNING - INTERNAL BETA BUILD --------------------" )
#else
#ifdef BETA
#pragma message( "WARNING - BETA BUILD -------" )
#endif


#endif

#endif // header
