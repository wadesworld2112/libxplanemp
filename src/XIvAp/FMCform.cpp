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
#include "FMCform.h"
#include "systemcallbacks.h"
#include "helpers.h"
#include "xivap.h"

#include "XPWidgetsEx.h"

#define DEFAULT_FPL_FILE string(getXivapRessourcesDir() + FLIGHTPLANS_DIR + DIR_CHAR + DEFAULT_FPL)
#define READFORM(field, str) { XPGetWidgetDescriptor(field, buffer, sizeof(buffer)); str = buffer; }

#define NAVREF_LATLON -2

FMCForm::~FMCForm()
{
	if(routefinder != NULL)
		delete routefinder;
}

FMCForm::FMCForm()
{
	routefinder = NULL;
	altitude_ = 0;

	create();
	reset();

	callUpdateNavList = false;

	// hook handlers
	XPAddWidgetCallback(routeTextField, fmcFormHandler);
	XPAddWidgetCallback(sidTextField, fmcFormHandler);
	XPAddWidgetCallback(starTextField, fmcFormHandler);
	XPAddWidgetCallback(inputTextField, fmcFormHandler);

	/*
	XPAddWidgetCallback(routeTextField, flightplanFormHandler);
	XPAddWidgetCallback(destTextField, flightplanFormHandler);
	XPAddWidgetCallback(alternateTextField, flightplanFormHandler);
	XPAddWidgetCallback(alternate2TextField, flightplanFormHandler);
	XPAddWidgetCallback(commentsTextField, flightplanFormHandler);
	XPAddWidgetCallback(airlineTextField, flightplanFormHandler);

	XPAddWidgetCallback(deptimeTextField, flightplanFormHandler);
	XPAddWidgetCallback(altTextField, flightplanFormHandler);
	XPAddWidgetCallback(speedTextField, flightplanFormHandler);
	XPAddWidgetCallback(eetTextField, flightplanFormHandler);
	XPAddWidgetCallback(enduranceTextField, flightplanFormHandler);
	XPAddWidgetCallback(pobTextField, flightplanFormHandler);
	XPAddWidgetCallback(picTextField, flightplanFormHandler);
	*/
}

void FMCForm::show()
{
	if(!XPIsWidgetVisible(window))
		XPShowWidget(window);
	XPBringRootWidgetToFront(window);
}

void FMCForm::hide()
{
	if(XPIsWidgetVisible(window))
		XPHideWidget(window);
}

void FMCForm::create()
{
	int x = 225;
	int y = 675;
	int x2 = x + 400;
	int y2 = y - 550;

	window = XPCreateWidget(x, y, x2, y2,
					1,			// Visible
					"FMC Utility", // desc
					1,			// root
					NULL,		// no container
					xpWidgetClass_MainWindow);
	XPSetWidgetProperty(window, xpProperty_MainWindowHasCloseBoxes, 1);

	y = y - 25; x = x + 10;

	// Introduction Text
	XPCreateWidget(x, y, x+390, y-12,
		1, "The FMC will be programmed with this information from your flight plan:", 0, window, xpWidgetClass_Caption);

	y -= 18;
	XPCreateWidget(x, y, x+380, y-82, 1, "", 0, window, xpWidgetClass_SubWindow);
	
	x += 10;
	y -= 2;

	// Departure
	XPCreateWidget(x, y, x+72, y-12, 1, "Departure:", 0, window, xpWidgetClass_Caption);
	depCaption = XPCreateWidget(x+75, y, x+380, y-12, 1, "LOIH (Hohenems)", 0, window, xpWidgetClass_Caption);

	y -= 14;
	// Destination
	XPCreateWidget(x, y, x+72, y-12, 1, "Destination:", 0, window, xpWidgetClass_Caption);
	destCaption = XPCreateWidget(x+75, y, x+380, y-12, 1, "LOWI (Innsbruck Kranebitten)", 0, window, xpWidgetClass_Caption);

	// Route
	y -= 18;
	XPCreateWidget(x, y, x+72, y-12, 1, "Route:", 0, window, xpWidgetClass_Caption);
	y -= 12;
	routeTextField = XPCreateWidget(x, y, x+360, y-22, 1, "", 0, window, xpWidgetClass_TextField);
	y -= 20;
	XPCreateWidget(x, y, x+72, y-12, 1, "(Airways on the route will be replaced by the appropriate waypoints)", 0, window, xpWidgetClass_Caption);

	// SID GROUP
	x -= 10; y -= 25;
	XPCreateWidget(x, y, x+390, y-12,
		1, "Standard Instrument Departure (SID):", 0, window, xpWidgetClass_Caption);

	y -= 18;
	XPCreateWidget(x, y, x+380, y-102, 1, "", 0, window, xpWidgetClass_SubWindow);

	x += 10;
	y -= 2; int y_copy = y;
	// SID
	XPCreateWidget(x, y-4, x+72, y-16, 1, "SID:", 0, window, xpWidgetClass_Caption);
	sidTextField = XPCreateWidget(x+35, y, x+100, y-22, 1, "", 0, window, xpWidgetClass_TextField);

	y -= 22;
	// SID popup
	sidPopup = XPCreatePopup(x+5, y, x+95, y-22, 1, "(no SID;", window);
	
	// Load SID
	y -= 32;
	sidLoadBtn = XPCreateWidget(x, y, x+100, y-22, 1, "Load SID", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(sidLoadBtn, xpProperty_ButtonType, xpPushButton);

	// Save SID
	y -= 22;
	sidSaveBtn = XPCreateWidget(x, y, x+100, y-22, 1, "Save SID", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(sidSaveBtn, xpProperty_ButtonType, xpPushButton);

	y = y_copy;
	x += 120;
	// waypoints
	XPCreateWidget(x, y, x+100, y-12, 1, "Waypoints:", 0, window, xpWidgetClass_Caption);

	y -= 14;
	sidWptListBox = XPCreateListBox(x, y, x + 250, y - 62, 1, "", window);

	y -= 62;
	// clear buttons
	sidClearBtn = XPCreateWidget(x+10, y, x+110, y-22, 1, "Clear WPT", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(sidClearBtn, xpProperty_ButtonType, xpPushButton);
	sidClearAllBtn = XPCreateWidget(x+120, y, x+220, y-22, 1, "Clear ALL", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(sidClearAllBtn, xpProperty_ButtonType, xpPushButton);
	x -= 120;



	// STAR GROUP
	x -= 10; y -= 32;
	XPCreateWidget(x, y, x+390, y-12,
		1, "Standard Terminal Arrival Route (STAR):", 0, window, xpWidgetClass_Caption);

	y -= 18;
	XPCreateWidget(x, y, x+380, y-102, 1, "", 0, window, xpWidgetClass_SubWindow);

	x += 10;
	y -= 2; y_copy = y;
	// STAR
	XPCreateWidget(x, y-4, x+72, y-16, 1, "STAR:", 0, window, xpWidgetClass_Caption);
	starTextField = XPCreateWidget(x+35, y, x+100, y-22, 1, "", 0, window, xpWidgetClass_TextField);

	y -= 22;
	starPopup = XPCreatePopup(x+5, y, x+95, y-22, 1, "(no STAR;", window);

	// Load STAR
	y -= 32;
	starLoadBtn = XPCreateWidget(x, y, x+100, y-22, 1, "Load STAR", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(starLoadBtn, xpProperty_ButtonType, xpPushButton);

	// Save STAR
	y -= 22;
	starSaveBtn = XPCreateWidget(x, y, x+100, y-22, 1, "Save STAR", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(starSaveBtn, xpProperty_ButtonType, xpPushButton);

	y = y_copy;
	x += 120;
	// waypoints
	XPCreateWidget(x, y, x+100, y-12, 1, "Waypoints:", 0, window, xpWidgetClass_Caption);

	y -= 14;
	starWptListBox = XPCreateListBox(x, y, x + 250, y - 62, 1, "", window);

	y -= 62;
	// clear buttons
	starClearBtn = XPCreateWidget(x+10, y, x+110, y-22, 1, "Clear WPT", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(starClearBtn, xpProperty_ButtonType, xpPushButton);
	starClearAllBtn = XPCreateWidget(x+120, y, x+220, y-22, 1, "Clear ALL", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(starClearAllBtn, xpProperty_ButtonType, xpPushButton);
	x -= 120;


	// PROGRAM GROUP
	x -= 10; y -= 32;
	XPCreateWidget(x, y, x+390, y-12,
		1, "Add waypoints to SID/STAR:", 0, window, xpWidgetClass_Caption);

	y -= 18;
	XPCreateWidget(x, y, x+380, y-110, 1, "", 0, window, xpWidgetClass_SubWindow);


	x += 10;
	y -= 2;
	int x_copy = x;
	y_copy = y;

	XPCreateWidget(x, y-4, x+50, y-16, 1, "Nav type:", 0, window, xpWidgetClass_Caption);
	y -= 22;

	// VOR
	VORselect = XPCreateWidget(x, y, x+12, y-22, 1, "", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(VORselect, xpProperty_ButtonType, xpRadioButton);
	XPSetWidgetProperty(VORselect, xpProperty_ButtonBehavior, xpButtonBehaviorRadioButton);
	XPCreateWidget(x+15, y-3, x+130, y-15, 1, "VOR", 0, window, xpWidgetClass_Caption);
	XPSetWidgetProperty(VORselect, xpProperty_ButtonState, 1);

	// NDB
	x += 45;
	NDBselect = XPCreateWidget(x, y, x+12, y-22, 1, "", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(NDBselect, xpProperty_ButtonType, xpRadioButton);
	XPSetWidgetProperty(NDBselect, xpProperty_ButtonBehavior, xpButtonBehaviorRadioButton);
	XPCreateWidget(x+15, y-3, x+130, y-15, 1, "NDB", 0, window, xpWidgetClass_Caption);

	// DME
	y -= 22;
	x = x_copy;
	DMEselect = XPCreateWidget(x, y, x+12, y-22, 1, "", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(DMEselect, xpProperty_ButtonType, xpRadioButton);
	XPSetWidgetProperty(DMEselect, xpProperty_ButtonBehavior, xpButtonBehaviorRadioButton);
	XPCreateWidget(x+15, y-3, x+130, y-15, 1, "DME", 0, window, xpWidgetClass_Caption);

	// Fix
	x += 45;
	FIXselect = XPCreateWidget(x, y, x+12, y-22, 1, "", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(FIXselect, xpProperty_ButtonType, xpRadioButton);
	XPSetWidgetProperty(FIXselect, xpProperty_ButtonBehavior, xpButtonBehaviorRadioButton);
	XPCreateWidget(x+15, y-3, x+130, y-15, 1, "FIX", 0, window, xpWidgetClass_Caption);

	// Fix
	x = x_copy;
	y -= 22;
	LLselect = XPCreateWidget(x, y, x+12, y-22, 1, "", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(LLselect, xpProperty_ButtonType, xpRadioButton);
	XPSetWidgetProperty(LLselect, xpProperty_ButtonBehavior, xpButtonBehaviorRadioButton);
	XPCreateWidget(x+15, y-3, x+130, y-15, 1, "Lat/Lon (Example:", 0, window, xpWidgetClass_Caption);
	y -= 15;
	XPCreateWidget(x+15, y, x+130, y-12, 1, "N47 23.07 E9 42.02)", 0, window, xpWidgetClass_Caption);


	y = y_copy;
	x = x_copy + 120;
	XPCreateWidget(x, y-4, x+50, y-16, 1, "Search:", 0, window, xpWidgetClass_Caption);
	inputTextField = XPCreateWidget(x+50, y, x+180, y-22, 1, "", 0, window, xpWidgetClass_TextField);

	// listbox
	y -= 22;
	inputListBox = XPCreateListBox(x, y, x + 250, y - 62, 1, "", window);

	// add buttons
	y -= 62;
	addSidButton = XPCreateWidget(x+10, y, x+110, y-22, 1, "Add to SID", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(addSidButton, xpProperty_ButtonType, xpPushButton);
	addStarButton = XPCreateWidget(x+120, y, x+220, y-22, 1, "Add to STAR", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(addStarButton, xpProperty_ButtonType, xpPushButton);

	x = x_copy - 20;
	y -= 30;
	// export
	exportButton = XPCreateWidget(x+10, y, x+160, y-22, 1, "Export to FMC", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(exportButton, xpProperty_ButtonType, xpPushButton);

	// close
	x += 310;
	closeButton = XPCreateWidget(x, y, x+80, y-22, 1, "Close", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(closeButton, xpProperty_ButtonType, xpPushButton);

	// hook up the handler
	XPAddWidgetCallback(window, fmcFormHandler);
}

int	FMCForm::handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMessage_CloseButtonPushed) {
		hide();
		return 1;
	}
	
	if(inMessage == xpMsg_PushButtonPressed) {
		// cancel
		if(inParam1 == (long)closeButton) {
			hide();
			return 1;
		}

		// export
		if(inParam1 == (long)exportButton) {
			FMCExport();
			return 1;
		}

		if(inParam1 == (long)sidClearAllBtn) {
			// clear the list box
			XPSetWidgetProperty(sidWptListBox, xpProperty_ListBoxClear, 1);
			sidPoints.clear();
			return 1;
		}

		if(inParam1 == (long)starClearAllBtn) {
			// clear the list box
			XPSetWidgetProperty(starWptListBox, xpProperty_ListBoxClear, 1);
			starPoints.clear();
			return 1;
		}

		if(inParam1 == (long)sidClearBtn) {
			int	item = XPGetWidgetProperty(sidWptListBox, xpProperty_ListBoxCurrentItem, NULL);
			if(item >= (int)sidPoints.size() || item < 0) return 1;
			XPSetWidgetProperty(sidWptListBox, xpProperty_ListBoxDeleteItem, 1);
			NavList::iterator it = sidPoints.begin();
			while(item > 0) { ++it; --item; };
			sidPoints.erase(it);
			return 1;
		}

		if(inParam1 == (long)starClearBtn) {
			int	item = XPGetWidgetProperty(starWptListBox, xpProperty_ListBoxCurrentItem, NULL);
			if(item >= (int)starPoints.size() || item < 0) return 1;
			XPSetWidgetProperty(starWptListBox, xpProperty_ListBoxDeleteItem, 1);
			NavList::iterator it = starPoints.begin();
			while(item > 0) { ++it; --item; };
			starPoints.erase(it);
			return 1;
		}

		if(inParam1 == (long)addSidButton) {
			if(navEntries.size() <= 0) return 1;
			// add point to SID
			int	item = XPGetWidgetProperty(inputListBox, xpProperty_ListBoxCurrentItem, NULL);
			if(item > static_cast<int>(navEntries.size())) return 1;

			XPSetWidgetProperty(sidWptListBox, xpProperty_ListBoxCurrentItem, sidPoints.size());

			sidPoints.push_back(navEntries[item]);
			char buffer[1024];
			XPGetWidgetDescriptor(inputListBox, buffer, sizeof(buffer));
			XPSetWidgetDescriptor(sidWptListBox, buffer);
			XPSetWidgetProperty(sidWptListBox, xpProperty_ListBoxAddItem, 1);
			return 1;
		}

		if(inParam1 == (long)addStarButton) {
			if(navEntries.size() <= 0) return 1;
			// add point to STAR
			int	item = XPGetWidgetProperty(inputListBox, xpProperty_ListBoxCurrentItem, NULL);
			if(item > static_cast<int>(navEntries.size())) return 1;

			XPSetWidgetProperty(starWptListBox, xpProperty_ListBoxCurrentItem, starPoints.size());

			starPoints.push_back(navEntries[item]);
			char buffer[1024];
			XPGetWidgetDescriptor(inputListBox, buffer, sizeof(buffer));
			XPSetWidgetDescriptor(starWptListBox, buffer);
			XPSetWidgetProperty(starWptListBox, xpProperty_ListBoxAddItem, 1);
			return 1;
		}

		if(inParam1 == (long)sidSaveBtn) {
			saveSid();
			return 1;
		}

		if(inParam1 == (long)starSaveBtn) {
			saveStar();
			return 1;
		}

		if(inParam1 == (long)sidLoadBtn) {
			loadSid();
			return 1;
		}

		if(inParam1 == (long)starLoadBtn) {
			loadStar();
			return 1;
		}

	}

	if(inMessage == xpMessage_PopupNewItemPicked) {
		if(inParam1 == (long)sidPopup) {
			unsigned int curItem = XPGetWidgetProperty(sidPopup, xpProperty_PopupCurrentItem, NULL);
			if(curItem > SIDnames.size() || SIDnames.size() == 0) return 1;
			XPSetWidgetDescriptor(sidTextField, pconst(SIDnames[curItem]));
			return 1;
		}

		if(inParam1 == (long)starPopup) {
			unsigned int curItem = XPGetWidgetProperty(starPopup, xpProperty_PopupCurrentItem, NULL);
			if(curItem > STARnames.size() || STARnames.size() == 0) return 1;
			XPSetWidgetDescriptor(starTextField, pconst(STARnames[curItem]));
			return 1;
		}
	}

	if(inMessage == xpMsg_ButtonStateChanged) {
		if(inParam1 == (long)VORselect || inParam1 == (long)NDBselect ||
		   inParam1 == (long)DMEselect || inParam1 == (long)FIXselect ||
		   inParam1 == (long)LLselect) { 

			XPSetWidgetProperty(VORselect, xpProperty_ButtonState, 0);
			XPSetWidgetProperty(NDBselect, xpProperty_ButtonState, 0);
			XPSetWidgetProperty(DMEselect, xpProperty_ButtonState, 0);
			XPSetWidgetProperty(FIXselect, xpProperty_ButtonState, 0);
			XPSetWidgetProperty(LLselect, xpProperty_ButtonState, 0);

			XPSetWidgetProperty((XPWidgetID)inParam1, xpProperty_ButtonState, 1);

			updateNavList();
		}
	}
		
	if(inMessage == xpMsg_KeyPress) {
		XPWidgetID widget = XPGetWidgetWithFocus();

		XPKeyState_t *key = (XPKeyState_t*)inParam1;

		// SDK bug workaround - we don't receive the text field changed message...
		if(widget == inputTextField)
			callUpdateNavList = true;

		// don't eat some special chars: backspace, del, leftarrow, rightarrow
		if(key->key == 8 || key->key == 46 || key->key == 28 || key->key == 29) return 0;

		// these should only get UPPERCASE characters
		if(widget == sidTextField || widget == starTextField || widget == routeTextField) {
			key->key = pt::upcase(key->key);

			if(widget == sidTextField || widget == starTextField) {
				if(!((key->key >= '0' && key->key <= '9') || (key->key >= 'A' && key->key <= 'Z')))
					return 1; // accept only 0-9 and A-Z
			}
			return 0; // don't consume the keystroke
		}
	}

	if(inMessage == xpMsg_Draw && callUpdateNavList) {
		callUpdateNavList = false;
		updateNavList();
		return 0;
	}

	return 0;
}

void FMCForm::FMCExport()
{
	// update xivap's fpl
	char buffer[1024];
	string route;
	READFORM(routeTextField, route);
	xivap.fpl.route = route;

	// reset XP's FMC
	int i = XPLMCountFMSEntries();
	for(int j = i-1; j >= 0; --j)
		XPLMClearFMSEntry(j);

	int fmsIndex = 0;
	
	XPLMNavRef lastPoint = depAirport;

	// first entry: Dep. airport
	if(depAirport != XPLM_NAV_NOT_FOUND)
		XPLMSetFMSEntryInfo(fmsIndex++, depAirport, altitude_);

	// SID waypoints
	for(size_t sp = 0; sp < sidPoints.size(); ++sp) {
		if(sidPoints[sp].ref == NAVREF_LATLON) {
			XPLMSetFMSEntryLatLon(fmsIndex++, sidPoints[sp].lat, sidPoints[sp].lon, altitude_);
		} else {
			// don't program the same point twice
			if(sidPoints[sp].ref != lastPoint)
				XPLMSetFMSEntryInfo(fmsIndex++, sidPoints[sp].ref, altitude_);
			lastPoint = sidPoints[sp].ref;
		}
	}

	if(length(xivap.fpl.route) > 2) {
		string datfile = getXplaneHomeDir() + string("Resources") + DIR_CHAR + "Earth Nav Data"
			+ DIR_CHAR + "awy.dat";
		if(routefinder == NULL)
			routefinder = new AwyFinder(datfile);

		std::vector<XPLMNavRef> waypoints = routefinder->getWaypoints(xivap.fpl.route, lastPoint);
		for(unsigned int i = 0; i < waypoints.size(); ++i) {
			if(waypoints[i] != lastPoint)
				XPLMSetFMSEntryInfo(fmsIndex++, waypoints[i], altitude_);
			lastPoint = waypoints[i];
		}

	}

	// STAR waypoints
	for(size_t sp = 0; sp < starPoints.size(); ++sp) {
		if(starPoints[sp].ref == NAVREF_LATLON) {
			XPLMSetFMSEntryLatLon(fmsIndex++, starPoints[sp].lat, sidPoints[sp].lon, altitude_);
		} else {
			if(starPoints[sp].ref != lastPoint)
				XPLMSetFMSEntryInfo(fmsIndex++, starPoints[sp].ref, altitude_);
			lastPoint = starPoints[sp].ref;
		}
	}

	// last entry: Dest. airport
	if(dstAirport != XPLM_NAV_NOT_FOUND)
		XPLMSetFMSEntryInfo(fmsIndex++, dstAirport, altitude_);

	xivap.flightplanForm().fillForm();
}

void FMCForm::setFields(string departure, string destination, string route, int altitude)
{
	XPSetWidgetDescriptor(routeTextField, pconst(route));

	char name_buf[512];
	string caption;

	depAirport = XPLMFindNavAid(NULL, pconst(departure), NULL, NULL, NULL, xplm_Nav_Airport);
	if(depAirport == XPLM_NAV_NOT_FOUND) {
		caption = departure + " (unknown)";	
	} else {
		XPLMGetNavAidInfo(depAirport, NULL, NULL, NULL, NULL, NULL, NULL, NULL, name_buf, NULL);
		caption = departure + " (" + string(name_buf) + ")";
	}
	XPSetWidgetDescriptor(depCaption, pconst(caption));

	dstAirport = XPLMFindNavAid(NULL, pconst(destination), NULL, NULL, NULL, xplm_Nav_Airport);
	if(dstAirport == XPLM_NAV_NOT_FOUND) {
		caption = destination + " (unknown)";	
	} else {
		XPLMGetNavAidInfo(dstAirport, NULL, NULL, NULL, NULL, NULL, NULL, NULL, name_buf, NULL);
		caption = destination + " (" + string(name_buf) + ")";
	}
	XPSetWidgetDescriptor(destCaption, pconst(caption));

	altitude_ = altitude;

	updateSIDList();
	updateSTARList();
}

void FMCForm::updateLatLon(string input)
{
	if(length(input) < 4) return;

	// N47 33.9 E019 21.2
	if(input[0] != 'N' && input[0] != 'S') return;
	bool north = input[0] == 'N';

	del(input, 0, 1);
	input = trim(input);

	// 47 33.9 E019 21.2

	// DEGREES
	int p = 0;
	input += " ";
	while(input[p] >= '0' && input[p] <= '9' && p+1 < length(input)) ++p;
	if(p > 2 || p == 0) return;

	string num = copy(input, 0, p);
	del(input, 0, p+1);
	input = trim(input);

	double lat = atof(pconst(num));

	if(length(input) < 2) return;

	// 33.9 E019 21.2
	p = 0;
	input += " ";
	while(input[p] >= '0' && input[p] <= '9' && p+1 < length(input)) ++p;
	if(p > 2) return;

	double min;
	if(p == 0) goto SkipToLon;

	num = copy(input, 0, p);
	del(input, 0, p+1);
	input = trim(input);
	if(length(input) < 2) return;

	min = atof(pconst(num));
	min = min / 60.0f;
	lat += min;

	// 9 E019 21.2
	p = 0;
	input += " ";
	while(input[p] >= '0' && input[p] <= '9' && p+1 < length(input)) ++p;
	if(p > 2) return;

	if(p == 0) goto SkipToLon;

	num = copy(input, 0, p);
	del(input, 0, p+1);
	input = trim(input);
	if(length(input) < 2) return;

	min = atof(pconst(num));
	min = min / 60.0f / 60.0f;
	lat += min;

SkipToLon:
	if(!north) lat = -lat;

	// ------------------------------------------
	// E019 21.2
	if(input[0] != 'E' && input[0] != 'W') return;
	bool east = input[0] == 'E';

	del(input, 0, 1);
	input = trim(input);

	// 019 21.2

	// DEGREES
	p = 0;
	input += " ";
	while(input[p] >= '0' && input[p] <= '9' && p+1 < length(input)) ++p;
	if(p > 3 || p == 0) return;

	num = copy(input, 0, p);
	del(input, 0, p+1);
	input = trim(input);

	double lon = atof(pconst(num));

	// 21.2
	p = 0;
	input += " ";
	while(input[p] >= '0' && input[p] <= '9' && p+1 < length(input)) ++p;
	if(p > 2) return;

	if(p == 0) goto SkipToEnd;

	num = copy(input, 0, p);
	del(input, 0, p+1);
	input = trim(input);

	min = atof(pconst(num));
	min = min / 60.0f;
	lon += min;

	// 2
	p = 0;
	input += " ";
	while(input[p] >= '0' && input[p] <= '9' && p+1 < length(input)) ++p;
	if(p > 2) return;

	if(p == 0) goto SkipToEnd;

	num = copy(input, 0, p);
	del(input, 0, p+1);

	min = atof(pconst(num));
	min = min / 60.0f / 60.0f;
	lon += min;


SkipToEnd:
	if(!east) lon = -lon;

	// add this entry to the list box
	string entryText = latlonStr(static_cast<float>(lat), static_cast<float>(lon));
	XPSetWidgetDescriptor(inputListBox, pconst(entryText));
	XPSetWidgetProperty(inputListBox, xpProperty_ListBoxAddItem, 1);
	FMCEntry e(NAVREF_LATLON);
	e.lat = static_cast<float>(lat);
	e.lon = static_cast<float>(lon);
	navEntries.push_back(e);
}

string FMCForm::latlonStr(float lat, float lon)
{
	string latstr = "N";
	float f = static_cast<float>(lat);
	if(f < 0) {
		latstr = "S";
		f = -f;
	}
	string d = float2coords(f);
	int p = pos('.', d);
	d[p] = ' ';
	for(int i = p; i < 2; ++i) d = "0" + d;
	latstr += d;

	string lonstr = "E";
	f = static_cast<float>(lon);
	if(f < 0) {
		lonstr = "W";
		f = -f;
	}
	d = float2coords(f);
	p = pos('.', d);
	d[p] = ' ';
	for(int i = p; i < 3; ++i) d = "0" + d;
	lonstr += d;

	string entryText = latstr + "/" + lonstr;

	return entryText;
}

string FMCForm::navDescription(XPLMNavRef navRef)
{
	XPLMNavType returnType;
	float lat, lon;
	char id_buffer[512];
	char name_buffer[512];
	XPLMGetNavAidInfo(navRef, &returnType, &lat, &lon, NULL, NULL, NULL, id_buffer, name_buffer, NULL);

	string id = id_buffer;
	string name = name_buffer;

	string entryText = id + " (" + name + ", ";

	// lat to string
	string dummy = "N";
	if(lat < 0) {
		dummy = "S";
		lat = -lat;
	}
	string dummy2 = float2coords(lat);
	int p1 = rpos('.', dummy2);
	dummy2 = copy(dummy2, 0, p1);
	p1 = pos('.', dummy2);
	// make sure that part before . is 2 chars long
	for(int i = p1; i < 2; ++i) dummy2 = "0" + dummy2;
	while(length(dummy2) < 6) dummy2 += "0";
	entryText += dummy + dummy2 + " / ";

	// convert lon to string
	dummy = "E";
	if(lon < 0) {
		dummy = "W";
		lon = -lon;
	}
	dummy2 = float2coords(lon);
	p1 = rpos('.', dummy2);
	dummy2 = copy(dummy2, 0, p1);
	p1 = pos('.', dummy2);
	// make sure that part before . is 3 chars long
	for(int i = p1; i < 3; ++i) dummy2 = "0" + dummy2;
	while(length(dummy2) < 5) dummy2 += "0";
	entryText += dummy + dummy2 + ")";

	return entryText;
}

void FMCForm::updateNavList()
{
	char buffer[1024];
	string input;
	READFORM(inputTextField, input);
	
	input = strupcase(trim(input));
	navEntries.clear();

	// clear the list box
	XPSetWidgetProperty(inputListBox, xpProperty_ListBoxClear, 1);

	XPLMNavType navType = xplm_Nav_VOR;
	if(XPGetWidgetProperty(NDBselect, xpProperty_ButtonState, NULL)) navType = xplm_Nav_NDB;
	else if(XPGetWidgetProperty(DMEselect, xpProperty_ButtonState, NULL)) navType = xplm_Nav_DME;
	else if(XPGetWidgetProperty(FIXselect, xpProperty_ButtonState, NULL)) navType = xplm_Nav_Fix;

	// dont do anything until at least 2 characters have been entered
	if(length(input) < 2) return;

	if(navType == xplm_Nav_Fix && length(input) < 3) return;

	if(XPGetWidgetProperty(LLselect, xpProperty_ButtonState, NULL)) {
		updateLatLon(input);
		return;
	}

	// go through all navaids of the requested type
	XPLMNavRef navRef = XPLMFindFirstNavAidOfType(navType);
	while(navRef != XPLM_NAV_NOT_FOUND) {
		// see if this navaid matches
		XPLMNavType returnType;
		char id_buffer[512];
		char name_buffer[512];

		XPLMGetNavAidInfo(navRef, &returnType, NULL, NULL, NULL, NULL, NULL, id_buffer, name_buffer, NULL);
		if(returnType != navType) {
			navRef = XPLM_NAV_NOT_FOUND;
			continue;
		}

		string id = id_buffer;
		int p1 = pos(input, strupcase(id));
		string name = name_buffer;
		int p2 = pos(input, strupcase(name));

		if(p1 < 0 && p2 < 0) {
			navRef = XPLMGetNextNavAid(navRef);
			continue;		// input is not a part of ID or name
		}

		// Houston, we found a match
		navEntries.push_back(navRef);

		// add this entry to the list box
		string entryText = navDescription(navRef);
		XPSetWidgetDescriptor(inputListBox, pconst(entryText));
		XPSetWidgetProperty(inputListBox, xpProperty_ListBoxAddItem, 1);

		navRef = XPLMGetNextNavAid(navRef);
	}
}

void FMCForm::saveProcedure(ConfigFile& db, string name, NavList &list)
{
	db.clearSection(name);
	db.setConfig(name, "POINTS", itostring(static_cast<int>(list.size())));
	for(size_t i = 0; i < list.size(); ++i) {
		if(list[i].ref == NAVREF_LATLON) {
			db.setConfig(name, "TYPE" + itostring(static_cast<int>(i)), "LatLon");
			db.setConfig(name, "LAT" + itostring(static_cast<int>(i)), ftoa(list[i].lat));
			db.setConfig(name, "LON" + itostring(static_cast<int>(i)), ftoa(list[i].lon));
		} else {
			XPLMNavType type;
			float lat, lon;
			char id[64];
			XPLMGetNavAidInfo(list[i].ref, &type, &lat, &lon, NULL, NULL, NULL, id, NULL, NULL);

			string typestr;
			switch(type) {
				case xplm_Nav_VOR: typestr = "VOR"; break;
				case xplm_Nav_NDB: typestr = "NDB"; break;
				case xplm_Nav_DME: typestr = "DME"; break;
				case xplm_Nav_Fix: typestr = "FIX"; break;
				default: typestr = "ERROR"; break;
			}
			db.setConfig(name, "TYPE" + itostring(static_cast<int>(i)), typestr);
			db.setConfig(name, "ICAO" + itostring(static_cast<int>(i)), id);
			db.setConfig(name, "LAT" + itostring(static_cast<int>(i)), ftoa(lat));
			db.setConfig(name, "LON" + itostring(static_cast<int>(i)), ftoa(lon));
		}
	}
}

void FMCForm::saveSid()
{
	char buffer[512];
	if(depAirport == XPLM_NAV_NOT_FOUND) return;
	XPLMGetNavAidInfo(depAirport, NULL, NULL, NULL, NULL, NULL, NULL, buffer, NULL, NULL);
	string airport = buffer;
	string sidName;
	READFORM(sidTextField, sidName);
	if(length(sidName) < 2) return;

	string filename = getXivapRessourcesDir() + "SidStar" + DIR_CHAR + airport + "_SIDS.txt";
	ConfigFile db;
	db.load(filename);

	saveProcedure(db, sidName, sidPoints);

	db.save(filename);

	updateSIDList();
}

void FMCForm::saveStar()
{
	char buffer[512];
	if(dstAirport == XPLM_NAV_NOT_FOUND) return;
	XPLMGetNavAidInfo(dstAirport, NULL, NULL, NULL, NULL, NULL, NULL, buffer, NULL, NULL);
	string airport = buffer;
	string starName;
	READFORM(starTextField, starName);
	if(length(starName) < 2) return;

	string filename = getXivapRessourcesDir() + "SidStar" + DIR_CHAR + airport + "_STARS.txt";
	ConfigFile db;
	db.load(filename);

	saveProcedure(db, starName, starPoints);

	db.save(filename);

	updateSTARList();
}

void FMCForm::loadProcedure(ConfigFile& db, string name, NavList &list, XPWidgetID listBox)
{
	list.clear();
	XPSetWidgetProperty(listBox, xpProperty_ListBoxClear, 1);

	string str = db.readConfig(name, "POINTS");
	if(str == "") return;
	int wpts = static_cast<int>(stringtoi(str));

	for(int i = 0; i < wpts; ++i) {
		str = db.readConfig(name, "TYPE" + itostring(i));
		if(str == "LatLon") {

			FMCEntry entry(NAVREF_LATLON);
			str = db.readConfig(name, "LAT" + itostring(i));
			if(str == "") continue;
			entry.lat = static_cast<float>(atof(pconst(str)));
			str = db.readConfig(name, "LON" + itostring(i));
			if(str == "") continue;
			entry.lon = static_cast<float>(atof(pconst(str)));
			list.push_back(entry);

			string entryText = latlonStr(entry.lat, entry.lon);
			XPSetWidgetDescriptor(listBox, pconst(entryText));
			XPSetWidgetProperty(listBox, xpProperty_ListBoxAddItem, 1);

		} else {

			XPLMNavType type;
			if(str == "FIX") type = xplm_Nav_Fix;
			else if(str == "VOR") type = xplm_Nav_VOR;
			else if(str == "NDB") type = xplm_Nav_NDB;
			else if(str == "DME") type = xplm_Nav_DME;
			else continue;

			string icao = db.readConfig(name, "ICAO" + itostring(i));
			if(icao == "") continue;
			str = db.readConfig(name, "LAT" + itostring(i));
			if(str == "") continue;
			float lat = static_cast<float>(atof(pconst(str)));
			str = db.readConfig(name, "LON" + itostring(i));
			if(str == "") continue;
			float lon = static_cast<float>(atof(pconst(str)));

			FMCEntry entry(XPLM_NAV_NOT_FOUND);
			entry.ref = XPLMFindNavAid(NULL, pconst(icao), &lat, &lon, NULL, type);
			if(entry.ref == XPLM_NAV_NOT_FOUND) continue;

			string entryText = navDescription(entry.ref);
			XPSetWidgetDescriptor(listBox, pconst(entryText));
			XPSetWidgetProperty(listBox, xpProperty_ListBoxAddItem, 1);
			list.push_back(entry);
		}
	}
}

void FMCForm::loadSid()
{
	char buffer[512];
	if(depAirport == XPLM_NAV_NOT_FOUND) return;
	XPLMGetNavAidInfo(depAirport, NULL, NULL, NULL, NULL, NULL, NULL, buffer, NULL, NULL);
	string airport = buffer;
	string sidName;
	READFORM(sidTextField, sidName);
	if(length(sidName) < 2) return;

	string filename = getXivapRessourcesDir() + "SidStar" + DIR_CHAR + airport + "_SIDS.txt";
	ConfigFile db;
	db.load(filename);

	loadProcedure(db, sidName, sidPoints, sidWptListBox);
}

void FMCForm::loadStar()
{
	char buffer[512];
	if(dstAirport == XPLM_NAV_NOT_FOUND) return;
	XPLMGetNavAidInfo(dstAirport, NULL, NULL, NULL, NULL, NULL, NULL, buffer, NULL, NULL);
	string airport = buffer;
	string starName;
	READFORM(starTextField, starName);
	if(length(starName) < 2) return;

	string filename = getXivapRessourcesDir() + "SidStar" + DIR_CHAR + airport + "_STARS.txt";
	ConfigFile db;
	db.load(filename);

	loadProcedure(db, starName, starPoints, starWptListBox);
}

void FMCForm::updateSIDList()
{
	string caption = "";
	SIDnames.clear();

	if(depAirport != XPLM_NAV_NOT_FOUND) {
		char buffer[512];
		XPLMGetNavAidInfo(depAirport, NULL, NULL, NULL, NULL, NULL, NULL, buffer, NULL, NULL);
		string airport = buffer;

		string filename = getXivapRessourcesDir() + "SidStar" + DIR_CHAR + airport + "_SIDS.txt";
		ConfigFile db;
		db.load(filename);

		std::vector<string> names = db.listSections();
		for(std::vector<string>::iterator it = names.begin(); it != names.end(); ++it) {
			SIDnames.push_back(*it);
			if(length(caption) > 0) caption += ";";
			caption += *it;
		}
		if(length(caption) > 0) caption += ";";
	}
	if(length(caption) == 0) caption = "(no SIDs;";

	XPSetWidgetDescriptor(sidPopup, pconst(caption));
	XPSetWidgetProperty(sidPopup, xpProperty_PopupCurrentItem, 0);
}

void FMCForm::updateSTARList()
{
	string caption = "";
	STARnames.clear();

	if(dstAirport != XPLM_NAV_NOT_FOUND) {
		char buffer[512];
		XPLMGetNavAidInfo(dstAirport, NULL, NULL, NULL, NULL, NULL, NULL, buffer, NULL, NULL);
		string airport = buffer;

		string filename = getXivapRessourcesDir() + "SidStar" + DIR_CHAR + airport + "_STARS.txt";
		ConfigFile db;
		db.load(filename);

		std::vector<string> names = db.listSections();
		for(std::vector<string>::iterator it = names.begin(); it != names.end(); ++it) {
			STARnames.push_back(*it);
			if(length(caption) > 0) caption += ";";
			caption += *it;
		}
		if(length(caption) > 0) caption += ";";
	}
	if(length(caption) == 0) caption = "(no STARs;";

	XPSetWidgetDescriptor(starPopup, pconst(caption));
	XPSetWidgetProperty(starPopup, xpProperty_PopupCurrentItem, 0);
}
