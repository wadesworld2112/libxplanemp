/**
 * \file FSDProtocol.h
 * 
 * This file contains the names of all messages.
 *
 * $Author: kenny.moens $
 * $LastChangedDate: 2004-04-30 16:28:24 +0200 (Fri, 30 Apr 2004) $
 * $LastChangedRevision: 1583 $
 * 
 * (c)2004 IVAO - International Virtual Aviation Organisation
 */

#include "common.h"

#define FSD_SIGREG_2

/**
 * General FSD Details
 */
#define _FSD_REVISION_               "B"

#define _FSD_SEP_                      "\r\n"
#define _FSD_MAXLENGTH_                3 // of packet ids!
#define _FSD_SERVERDEST_               "SERVER"
#define _FSD_MAXMSGLENGTH_             990
#define _FSD_POSUPDATEINT_             5000 //250

const string FSD_HEADS[] = {
	"invalid",	// 0
	"@",		// 1
	"%",		// 2
	"*",		// 3
	"!S",		// 4
	"!C",		// 5
	"!R",		// 6
	"#AA",		// 7
	"#AP",		// 8
	"#DA",		// 9
	"#DP",		// 10
	"#TM",		// 11
	"#AT",		// 12
	"#WX",		// 13
	"#DL",		// 14
	"#SB",		// 15
	"$FP",		// 16
	"$CF",		// 17
	"$!!",		// 18
	"$PI",		// 19
	"$PO",		// 20
	"$CQ",		// 21
	"$CR",		// 22
	"$NV",		// 23
	"$ER",		// 24
	"-PR",		// 25
	"-PD",		// 26
	"-MR",		// 27
	"-MD",		// 28
	"-RF",		// 29
	"-FP",		// 30
	"&D",		// 31
	"&R",		// 32
	"&G",		// 33
	"&W",		// 34
	"&T",		// 35
	"&C",		// 36
	"&V"		// 37
};

#define NUM_FSD_HEADS 38

/**
 * The FSD communication messages
 */

#define _FSD_INVALID_         0

// Position Updates
#define _FSD_PILOTPOS_        1 //     "@"
#define _FSD_ATCPOS_          2 //     "%"
#define _FSD_FASTPILOT_       3 //     "*"

// Server/Client Verification
#define _FSD_SERVERVERIFY_    4 //      "!S"
#define _FSD_CLIENTVERIFY_    5 //      "!C"
#define _FSD_REGINFO_         6 //      "!R"

// Communication
#define _FSD_ADDATC_          7 //      "#AA"
#define _FSD_ADDPILOT_        8 //      "#AP"
#define _FSD_DELATC_          9 //      "#DA"
#define _FSD_DELPILOT_       10 //      "#DP"
#define _FSD_TEXTMESSAGE_    11 //      "#TM"
#define _FSD_ATIS_           12 //      "#AT"
#define _FSD_WEATHERREQ_     13 //      "#WX"
#define _FSD_WINDVAR_        14 //      "#DL"
#define _FSD_CUSTOMPILOT_    15 //      "#SB"

#define _FSD_CUSTOMPILOT_REQPLANEINFO_ "PIR"
#define _FSD_CUSTOMPILOT_PLANEINFO_    "PI"

// Administrative
#define _FSD_FLIGHTPLAN_    16 //      "$FP"
#define _FSD_CHANGEFP_      17 //      "$CF"
#define _FSD_KILL_          18 //      "$!!"
#define _FSD_PING_          19 //      "$PI"
#define _FSD_PONG_          20 //      "$PO"

#define _FSD_INFOREQ_       21 //      "$CQ"
#define _FSD_INFOREQ_INF_              "INF"
#define _FSD_INFOREQ_P2P_              "P2P"
#define _FSD_INFOREQ_COM_              "C?"
#define _FSD_INFOREQ_ATIS_             "ATIS"
#define _FSD_INFOREPLY_     22 //      "$CR"
#define _FSD_INFOREPLY_TCAS2_          "T2"
#define _FSD_NOVOICE_       23 //      "$NV"
#define _FSD_ERROR_         24 //      "$ER"

// pilot packets
#define _FSD_REQPLANEINFO_   25  //    "-PR"
#define _FSD_PLANEINFO_      26  //    "-PD"
#define _FSD_REQPLANEPARAMS_ 27  //    "-MR"
#define _FSD_PLANEPARAMS_    28  //    "-MD"
#define _FSD_REQFASTUPD_     29  //    "-RF"
#define _FSD_FASTUPDINT_     30  //    "-FP"

// weather packets
#define _FSD_WEADATA_        31 //     "&D"
#define _FSD_WEADATA_METAR_            0
#define _FSD_WEADATA_TAF_              1
#define _FSD_WEADATA_STAF_             2

#define _FSD_WEAREG_         32 //     "&R"
#define _FSD_WEAREG_NONE_              0
#define _FSD_WEAREG_REGISTER_          1
#define _FSD_WEAREG_STATION_           2

#define _FSD_WEAGENERAL_     33 //     "&G"
#define _FSD_WEAWINDLAY_     34 //     "&W"
#define _FSD_WEATEMPLAY_     35 //     "&T"
#define _FSD_WEACLOUDLAY_    36 //     "&C"
#define _FSD_WEAVISLAY_      37 //     "&V"

#define _FSD_FOLLOWME_                 0x40

/*
 * Fast Update Specific
 */
//#define _FSD_FASTUPDATE_ROW_ "015453545254535451545354525453545" // WARNING: never let 0 follow 0 ! "0434243414342434"

#define _FSD_FASTUPDATE_NOSWITCH_ -1
#define _FSD_FASTUPDATE_MIN_ 0
#define _FSD_FASTUPDATE_MAX_ 3
extern const int FSD_FASTUPDATE_RATE[];

#define _FSD_FAST_NONE_ 0

#define _FSD_FAST_MINTIME_ 78.125

#define _FSD_FAST_LOWERBOUND_ 59
