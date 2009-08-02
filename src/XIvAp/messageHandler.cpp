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
#include "messageHandler.h"
#include "xivap.h"

#include "GW-FMS_SDK.h"

void MessageHandler(XPLMPluginID inFrom, long inMsg, void *inParam)
{
	if(inFrom == XPLM_PLUGIN_XPLANE) {
		switch(inMsg) {
			case XPLM_MSG_PLANE_LOADED:
				xivap.aircraftChange();
				break;
			case XPLM_MSG_AIRPORT_LOADED:
				xivap.airportChange();
				break;
		}
	}

	stEventData *theEventData = (stEventData*)inParam;

	static bool visible = false;

	switch(inMsg) {
		// ---------------------------------------------------------------------------
		// -- XSQUAWKBOX SDK ---------------------------------------------------------
		// ---------------------------------------------------------------------------

		case XSB_CMD_USER_LOGIN:
			// This command poses the user's login dialog box
			xivap.connectForm().show();
			break;

		case XSB_CMD_CONNECT:
			// This command connects to the network without posing the login dialog box
			//xivap.connect();
			break;

		case XSB_CMD_DISCONNECT:
			// This command disconnect's from the network
			xivap.disconnect();
			break;

		case XSB_CMD_SHOW_FP:
			// This command poses the flight plan dialog box
			xivap.flightplanForm().show();
			break;

		case XSB_CMD_SEND_FP:
			// This command sends the FP to ATC
			xivap.sendFlightplan();
			break;


		// ---------------------------------------------------------------------------
		// -- GOODWAY FMS ------------------------------------------------------------
		// ---------------------------------------------------------------------------

		case kGWFMS_EVENT_WhoIsModule:
			//theVersion = GW_FMS_GetGWFMS_Version();
			//theError = GW_FMS_InitModule( "LimitedTest", "X-IVAP" );
			GW_FMS_InitModule( "41137780223", "X-IVAP" );
			xivap.uiWindow.GWFMSPrint();
			break;
		
		case kGWFMS_EVENT_Activate:
			//xivap.addText(colWhite, "kGWFMS_EVENT_Activate", true, true);
			break;
		
		case kGWFMS_EVENT_Error:
			if(theEventData->sInt == kGWFMS_Error_PlugInLicenceNotValide)
				xivap.addText(colWhite, "GW-FMS license is invalid, FMS supposrt disabled", true, true);
			else if(theEventData->sInt == kGWFMS_Error_PlugInLicenceTest)
				xivap.addText(colWhite, "Using a GW-FMS test license", true, true);
			break;
		
		case kGWFMS_EVENT_CallBack:
			if(visible) {
				xivap.addText(colWhite, "kGWFMS_EVENT_CallBack", true, true);
				xivap.uiWindow.GWFMSPrint();
			}
			break;

		case kGWFMS_EVENT_Show:
			//xivap.addText(colWhite, "kGWFMS_EVENT_Show", true, true);
			visible = true;
			break;

		case kGWFMS_EVENT_Hide:
			//xivap.addText(colWhite, "kGWFMS_EVENT_Hide", true, true);
			visible = false;
			break;

		case kGWFMS_EVENT_LSK_Pressed:
			switch(theEventData->sInt)
			{
				case kGWFMS_FuncKey_L1:	xivap.uiWindow.buttonClicked(LSK1);	break;
				case kGWFMS_FuncKey_R1:	xivap.uiWindow.buttonClicked(RSK1);	break;
				case kGWFMS_FuncKey_L2:	xivap.uiWindow.buttonClicked(LSK2);	break;
				case kGWFMS_FuncKey_R2:	xivap.uiWindow.buttonClicked(RSK2);	break;
				case kGWFMS_FuncKey_L3:	xivap.uiWindow.buttonClicked(LSK3);	break;
				case kGWFMS_FuncKey_R3:	xivap.uiWindow.buttonClicked(RSK3);	break;
				case kGWFMS_FuncKey_L4:	xivap.uiWindow.buttonClicked(LSK4);	break;
				case kGWFMS_FuncKey_R4:	xivap.uiWindow.buttonClicked(RSK4);	break;
				case kGWFMS_FuncKey_L5:	xivap.uiWindow.buttonClicked(LSK5);	break;
				case kGWFMS_FuncKey_R5:	xivap.uiWindow.buttonClicked(RSK5);	break;
				case kGWFMS_FuncKey_L6:	xivap.uiWindow.buttonClicked(LSK6);	break;
				case kGWFMS_FuncKey_R6:	xivap.uiWindow.buttonClicked(RSK6);	break;
			}

			//line = xivap.uiWindow.getEditLine();
			//GW_FMS_EraseScratchPAD();
			//GW_FMS_SetScratchPAD(const_cast<char*>(pconst(line)), kGWFMS_TEXTColor_Green);
			xivap.uiWindow.GWFMSPrint();
			break;

		case kGWFMS_EVENT_FUNC_Pressed:
			switch(theEventData->sInt)
			{
				case kGWFMS_FuncKey_ATCCOMM:
					if(xivap.activeRadio() == 1)
						xivap.uiWindow.setPage(SCREEN_COM1);
					else
						xivap.uiWindow.setPage(SCREEN_COM2);
					break;
				case kGWFMS_FuncKey_NEXTPAGE:
					xivap.uiWindow.setPage(SCREEN_MAIN);
					break;
				case kGWFMS_FuncKey_ARROW_UP:
					xivap.uiWindow.keyUpArrow();
					break;
				case kGWFMS_FuncKey_ARROW_DOWN:
					xivap.uiWindow.keyDownArrow();
					break;
			}
			xivap.uiWindow.GWFMSPrint();
			break;
	}
}

// ---------------------------------------------------------------------------
// PLUGIN DATA CALLBACKS -----------------------------------------------------
// ---------------------------------------------------------------------------

int Handler_getInteger(void *refcon) // common integer getter
{
	xivap.addText(colWhite, string("int read from ") + (char*)refcon, true, true);

	if(refcon == (void*)XSB_VERS_NUMBER)	return SOFTWARE_VERSION_INT;
	if(refcon == (void*)XSB_CON_PORT)		return xivap.fsd.connectPort();
	if(refcon == (void*)XSB_CON_STATUS) {
		if(xivap.fsd.connected()) return 2;
		else return 0;
	}
	if(refcon == (void*)XSB_FP_FLIGHT_TYPE) {
		// int, either I, V, D, or S
		if(xivap.fpl.typeofflight == "V") return 'V';
		if(xivap.fpl.typeofflight == "I") return 'I';
		if(xivap.fpl.typeofflight == "S") return 'S';
		// wtf is 'D' supposed to be?
		return -1;
	}
	if(refcon == (void*)XSB_FP_TCAS_TYPE) {
		// int, T, H, B, L, or 0
		// erm... no idea...
		return -1;
	}
	if(refcon == (void*)XSB_FP_NAV_TYPE) {
		// int, F, G, R, A, etc.
		// incompatible. equipment is a multi-letter string in xivap.
		return 'F';
	}
	if(refcon == (void*)XSB_FP_SPEED) {
		// int, knots
		return atoi(xivap.fpl.cruisespeed);
	}
	if(refcon == (void*)XSB_FP_DEPARTURE_TIME) {
		// int, zulu time, e.g. 1340 = 13:40
		return atoi(xivap.fpl.deptimeest);
	}
	if(refcon == (void*)XSB_FP_DEPARTURE_TIME_ACTUAL) {
		// int, zulu time, e.g. 1340 = 13:40
		return atoi(xivap.fpl.deptimeact);
	}
	if(refcon == (void*)XSB_FP_ENROUTE_HRS) {
		// int, hrs
		return atoi(xivap.fpl.hoursenroute);
	}
	if(refcon == (void*)XSB_FP_ENROUTE_MINS) {
		// int, mins
		return atoi(xivap.fpl.minenroute);
	}
	if(refcon == (void*)XSB_FP_FUEL_HRS) {
		// int, mins
		return atoi(xivap.fpl.hoursfuel);
	}
	if(refcon == (void*)XSB_FP_FUEL_MINS) {
		// int, mins
		return atoi(xivap.fpl.minfuel);
	}

	return -1;
}


// reduce the amount of typing in the function below ...
#define GS_RETURNMACRO(ref, strg) {								\
	if(refcon == (void*)ref) {									\
		long len = strlen(strg);								\
		if(outbuf == NULL) return len;							\
		result = len;											\
		/* make sure not to write more than buflength */		\
		if(inMaxLength > len + 1) len = inMaxLength - 1;		\
		strncpy((char *)outbuf, strg, len + 1);					\
		return result;											\
	}															\
}

long Handler_getString(void *refcon, void *outbuf, int inOffset, long inMaxLength) // common string getter
{
	xivap.addText(colWhite, string("str read from ") + (char*)refcon, true, true);

	long result;
	GS_RETURNMACRO(XSB_VERS_STRING, SOFTWARE_VERSION);

	GS_RETURNMACRO(XSB_CON_CALLSIGN, xivap.fsd.callsign());
	GS_RETURNMACRO(XSB_CON_SERVER, xivap.fsd.connectHost());
	GS_RETURNMACRO(XSB_CON_PILOT_ID, xivap.fsd.vid());
	GS_RETURNMACRO(XSB_CON_PASSWORD, "(null)");
	GS_RETURNMACRO(XSB_CON_REALNAME, xivap.realname);
	GS_RETURNMACRO(XSB_CON_MODEL, xivap.fsd.mtl());
	
	if(refcon == (void*)XSB_FP_CRUISE_ALTITUDE) {
		string dummy = xivap.fpl.alttype + xivap.fpl.cruisealt;
		GS_RETURNMACRO(XSB_FP_CRUISE_ALTITUDE, dummy);
	}
	GS_RETURNMACRO(XSB_FP_CRUISE_ALTITUDE, xivap.fpl.destination);
	GS_RETURNMACRO(XSB_FP_ALTERNATE_AIRPORT, xivap.fpl.alternate);
	GS_RETURNMACRO(XSB_FP_DEPARTURE_AIRPORT, xivap.fpl.departure);
	GS_RETURNMACRO(XSB_FP_REMARKS, xivap.fpl.remarks);
	GS_RETURNMACRO(XSB_FP_ROUTE, xivap.fpl.route);

	return -1;
}

void Handler_setInteger(void *refcon, int value)
{
	xivap.addText(colWhite, string("int write to ") + (char*)refcon, true, true);

	if(refcon == (void*)XSB_FP_FLIGHT_TYPE) {
		// int, either I, V, D, or S
		switch(value) {
			case 'I': xivap.fpl.flightrules = "I"; break;
			case 'V': xivap.fpl.flightrules = "V"; break;
			case 'S': xivap.fpl.flightrules = "S"; break;
		}
		return;
	}
	if(refcon == (void*)XSB_FP_SPEED) {
		// int, knots
		xivap.fpl.speedtype = "N";
		xivap.fpl.cruisespeed = itostring(value);
		return;
	}
	if(refcon == (void*)XSB_FP_DEPARTURE_TIME) {
		// int, zulu time, e.g. 1340 = 13:40
		xivap.fpl.deptimeest = itostring(value);
		while(length(xivap.fpl.deptimeest) < 4) xivap.fpl.deptimeest = "0" + xivap.fpl.deptimeest;
		return;
	}
	if(refcon == (void*)XSB_FP_DEPARTURE_TIME_ACTUAL) {
		// int, zulu time, e.g. 1340 = 13:40
		xivap.fpl.deptimeest = itostring(value);
		while(length(xivap.fpl.deptimeact) < 4) xivap.fpl.deptimeact = "0" + xivap.fpl.deptimeact;
		return;
	}
	if(refcon == (void*)XSB_FP_ENROUTE_HRS) {
		// int, hrs
		xivap.fpl.hoursenroute = itostring(value);
		return;
	}
	if(refcon == (void*)XSB_FP_ENROUTE_MINS) {
		// int, mins
		xivap.fpl.minenroute = itostring(value);
		return;
	}
	if(refcon == (void*)XSB_FP_FUEL_HRS) {
		// int, hrs
		xivap.fpl.hoursfuel = itostring(value);
		return;
	}
	if(refcon == (void*)XSB_FP_FUEL_MINS) {
		// int, mins
		xivap.fpl.minfuel = itostring(value);
		return;
	}

	xivap.addText(colWhite, string("not implemented: int write to ") + (char*)refcon, true, true);
}

void Handler_setString(void *refcon, void *value, int offset, long length)
{
	xivap.addText(colWhite, string("str write to ") + (char*)refcon, true, true);

	if(refcon == (void*)XSB_FP_DEPARTURE_AIRPORT) {
		xivap.fpl.departure = (char*)value;
		return;
	}
	if(refcon == (void*)XSB_FP_ARRIVAL_AIRPORT) {
		xivap.fpl.destination = (char*)value;
		return;
	}
	if(refcon == (void*)XSB_FP_ALTERNATE_AIRPORT) {
		xivap.fpl.alternate = (char*)value;
		return;
	}
	if(refcon == (void*)XSB_FP_CRUISE_ALTITUDE) {
		string dummy = (char*)value;
		if(dummy[0] == 'F') {
			xivap.fpl.alttype = "FL";
			del(dummy, 0, 2);
		}
		xivap.fpl.cruisealt = dummy; 
		return;
	}
	if(refcon == (void*)XSB_FP_REMARKS) {
		xivap.fpl.remarks = (char*)value;
		return;
	}
	if(refcon == (void*)XSB_FP_ROUTE) {
		xivap.fpl.route = (char*)value;
		return;
	}

	if(refcon == (void*)XSB_CON_CALLSIGN) {
		if(xivap.fsd.connected()) return; // dont set callsign while connected
		xivap.fpl.callsign = (char*)value;
		return;
	}
	/*
	if(refcon == (void*)XSB_CON_SERVER) {
		if(xivap.fsd.connected()) return; // dont set server while connected
		// ...
		return;
	}
	if(refcon == (void*)XSB_CON_PILOT_ID) {
		if(xivap.fsd.connected()) return; // dont set while connected
		// ...
		return;
	}
	if(refcon == (void*)XSB_CON_PASSWORD) {
		if(xivap.fsd.connected()) return; // dont set while connected
		// ...
		return;
	}
	if(refcon == (void*)XSB_CON_REALNAME) {
		if(xivap.fsd.connected()) return; // dont set while connected
		
		// ...
		return;
	}
	*/
	if(refcon == (void*)XSB_CON_MODEL) {
		xivap.fpl.aircrafttype = (char*)value;
		return;
	}

	xivap.addText(colWhite, string("not implemented: str write to ") + (char*)refcon, true, true);
}
