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

#ifndef FLIGHTPLAN_FORM_
#define FLIGHTPLAN_FORM_

#include "common.h"
#include "aircraftDB.h"
#include "airlinesDB.h"
#include "awyFinder.h"

class FlightplanForm {
public:
	FlightplanForm();
	~FlightplanForm();

	void show();
	void hide();
	void fillForm();

	int handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);

	// updates callsign and realname from xivap
	void setUserCredentials();
	void setFPTextFields();
	void setAcfIcao();

private:
	XPWidgetID window;
	XPWidgetID callsignTextField, actypeTextField,
		equipTextField, departureTextField, deptimeTextField,
		speedTextField, altTextField, routeTextField, destTextField, eetTextField,
		alternateTextField, alternate2TextField, commentsTextField, enduranceTextField,
		pobTextField, picTextField, airlineTextField;
	// XPWidgetID fmcTextField;
	XPWidgetID exportButton, resetButton, sendButton, cancelButton;

	XPWidgetID flightRulesPopup, typePopup, wtcPopup, speedtypePopup, xpdrPopup,
		numberTextField, alttypePopup, actypePopup, airlinePopup, liveryPopup;

	AircraftDB::AircraftList acList;
	AirlinesDB::AirlinesList alList;
	AirlinesDB::LiveryList livList;
	void updateACList();
	void updateALList();
	bool callUpdateAC, callUpdateAL;

	void create();
	void reset();
	void send(bool closeWindow = true);

	void FMCExport();

	AwyFinder *routefinder;
	
};

#endif
