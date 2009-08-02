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
#include "flightplanForm.h"
#include "systemcallbacks.h"
#include "helpers.h"
#include "xivap.h"

#include "XPWidgetsEx.h"

#define DEFAULT_FPL_FILE string(getXivapRessourcesDir() + FLIGHTPLANS_DIR + DIR_CHAR + DEFAULT_FPL)
#define READFORM(field, str) { XPGetWidgetDescriptor(field, buffer, sizeof(buffer)); str = buffer; }

FlightplanForm::~FlightplanForm()
{
	if(routefinder != NULL)
		delete routefinder;
}

FlightplanForm::FlightplanForm()
{
	routefinder = NULL;

	create();
	reset();

	// hook handlers
	XPAddWidgetCallback(callsignTextField, flightplanFormHandler);
	XPAddWidgetCallback(actypeTextField, flightplanFormHandler);
	XPAddWidgetCallback(departureTextField, flightplanFormHandler);
	XPAddWidgetCallback(equipTextField, flightplanFormHandler);
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

	callUpdateAC = false;
	callUpdateAL = false;
}

void FlightplanForm::show()
{
	if(!XPIsWidgetVisible(window)) {
		XPShowWidget(window);
		//xivap.airportChange(); // why is that in here?
	}
	XPBringRootWidgetToFront(window);
}

void FlightplanForm::hide()
{
	if(XPIsWidgetVisible(window))
		XPHideWidget(window);
}

void FlightplanForm::create()
{
	int x = 200;
	int y = 700;
	int x2 = x + 400;
	int y2 = y - 445;

	window = XPCreateWidget(x, y, x2, y2,
					0,			// Visible
					"International Flight Plan", // desc
					1,			// root
					NULL,		// no container
					xpWidgetClass_MainWindow);
	XPSetWidgetProperty(window, xpProperty_MainWindowHasCloseBoxes, 1);

	y = y - 25; x = x + 10;

	XPCreateWidget(x+47, y, x+135, y-12, 1, "7 aircraft ident.", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+142, y, x+212, y-12, 1, "8 flight rules", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+225, y, x+320, y-12, 1, "type of flight", 0, window, xpWidgetClass_Caption);

	// callsign
	y = y - 12;
	XPCreateWidget(x, y, x+42, y-22, 1, "<<=(FPL", 0, window, xpWidgetClass_Caption);
	callsignTextField = XPCreateWidget(x+47, y, x+118, y-22, 1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(callsignTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(callsignTextField, xpProperty_MaxCharacters, 9);

	// flight rules
	XPCreateWidget(x+128, y, x+138, y-22, 1, "-", 0, window, xpWidgetClass_Caption);
	flightRulesPopup = XPCreatePopup(x+147, y, x+187, y-22, 1, "V  (VFR);I  (IFR);Y  (IFR -> VFR);Z  (VFR -> IFR);", window);

	// flight type
	XPCreateWidget(x+212, y, x+220, y-22, 1, "-", 0, window, xpWidgetClass_Caption);
	typePopup = XPCreatePopup(x+230, y, x+270, y-22, 1, "S  (Scheduled Air Transport);N  (Non-Scheduled Air Transport);G  (General Aviation);M  (Military);X  (Other);", window);

	XPCreateWidget(x+294, y, x+316, y-22, 1, "<<=", 0, window, xpWidgetClass_Caption);

	y = y - 24;
	XPCreateWidget(x+20, y, x+67, y-12, 1, "9 number", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+78, y, x+162, y-12, 1, "type of aircraft", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+174, y, x+247, y-12, 1, "wake cat.", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+241, y, x+241+70, y-12, 1, "10 equipment", 0, window, xpWidgetClass_Caption);

	// number
	y = y - 12;
	XPCreateWidget(x+8, y, x+20, y-22, 1, "-", 0, window, xpWidgetClass_Caption);
	numberTextField = XPCreateWidget(x+20, y, x+20+24, y-22, 1, "1", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(numberTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(numberTextField, xpProperty_MaxCharacters, 1);

	// aircraft
	actypeTextField = XPCreateWidget(x+78, y, x+152, y-22, 1, "C172", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(actypeTextField, xpProperty_TextFieldType, xpTextEntryField);
	//XPSetWidgetProperty(actypeTextField, xpProperty_MaxCharacters, 4);

	// WTC
	XPCreateWidget(x+158, y, x+168, y-22, 1, "/", 0, window, xpWidgetClass_Caption);
	wtcPopup = XPCreatePopup(x+174, y, x+220, y-22, 1, "L  (Light);M  (Medium);H  (Heavy);", window);	

	// Equipment
	XPCreateWidget(x+221, y, x+233, y-22, 1, "-", 0, window, xpWidgetClass_Caption);
	equipTextField = XPCreateWidget(x+241, y, x+295, y-22, 1, "S", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(equipTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(equipTextField, xpProperty_MaxCharacters, 7);

	// transpondertype
	XPCreateWidget(x+298, y, x+318, y-22, 1, "/", 0, window, xpWidgetClass_Caption);
	xpdrPopup = XPCreatePopup(x+318, y, x+350, y-22, 1, "C  (Mode C);S  (Mode S, with alt and aircraft id);X  (Mode S, no alt and no aircraft id);P  (Mode S, alt but no aircraft id);I  (Mode S, no alt but aircraft id);A  (Mode A - no altitude reporting);N  (no transponder on board);", window);	
	XPCreateWidget(x+355, y, x+370, y-22, 1, "<<=", 0, window, xpWidgetClass_Caption);

	y = y - 24;
	// aircraft select popup
	actypePopup = XPCreatePopup(x+78, y, x+295, y-22, 1, ";", window);
	updateACList();

	y = y - 24;
	// airline
	XPCreateWidget(x+20, y-2, x+67, y-16, 1, "Airline", 0, window, xpWidgetClass_Caption);
	airlineTextField = XPCreateWidget(x+65, y, x+120, y-22, 1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(actypeTextField, xpProperty_TextFieldType, xpTextEntryField);
	airlinePopup = XPCreatePopup(x+135, y, x+350, y-22, 1, "none;", window);
	updateALList();

	// livery code
	y = y - 24;
	XPCreateWidget(x+20, y-2, x+75, y-16, 1, "Special livery", 0, window, xpWidgetClass_Caption);
	liveryPopup = XPCreatePopup(x+100, y, x+350, y-22, 1, "n/a;", window);

	y = y - 28;
	XPCreateWidget(x+20, y, x+100, y-12, 1, "13 departure", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+114, y, x+210, y-12, 1, "dep. time", 0, window, xpWidgetClass_Caption);

	// departure
	y = y - 12;
	XPCreateWidget(x+8, y, x+20, y-22, 1, "-", 0, window, xpWidgetClass_Caption);
	departureTextField = XPCreateWidget(x+20, y, x+20+46, y-22, 1, "LOIH", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(departureTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(departureTextField, xpProperty_MaxCharacters, 4);

	// dep. time
	deptimeTextField = XPCreateWidget(x+114, y, x+114+46, y-22, 1, "1200", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(deptimeTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(deptimeTextField, xpProperty_MaxCharacters, 4);
	XPCreateWidget(x+171, y, x+200, y-22, 1, "<<=", 0, window, xpWidgetClass_Caption);

	y = y - 24;
	XPCreateWidget(x+20, y, x+100, y-12, 1, "15 crusing speed", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+125, y, x+210, y-12, 1, "level", 0, window, xpWidgetClass_Caption);

	// speed type
	y = y - 12;
	XPCreateWidget(x+8, y, x+20, y-22, 1, "-", 0, window, xpWidgetClass_Caption);
	speedtypePopup = XPCreatePopup(x+20, y, x+55, y-22, 1, "N  (Knots);M  (Mach);", window);	

	// speed
	speedTextField = XPCreateWidget(x+61, y, x+100, y-22, 1, "0110", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(speedTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(speedTextField, xpProperty_MaxCharacters, 4);

	// alt type
	alttypePopup = XPCreatePopup(x+125, y, x+172, y-22, 1, "F  (Flight Level);A  (Altitude);VFR;", window);

	// alt
	altTextField = XPCreateWidget(x+178, y, x+220, y-22, 1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(altTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(altTextField, xpProperty_MaxCharacters, 4);

	y = y - 24;
	XPCreateWidget(x+20, y, x+100, y-12, 1, "route", 0, window, xpWidgetClass_Caption);

	// route
	y = y - 12;
	routeTextField = XPCreateWidget(x+20, y, x+322+24, y-22, 1, "DCT VFR", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(routeTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(routeTextField, xpProperty_MaxCharacters, 512);
	XPCreateWidget(x+355, y, x+370, y-22, 1, "<<=", 0, window, xpWidgetClass_Caption);

	y = y - 24;
	XPCreateWidget(x+20, y, x+100, y-12, 1, "16 destination", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+112, y, x+150, y-12, 1, "total EET", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+190, y, x+250, y-12, 1, "alternate", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+280, y, x+340, y-12, 1, "2nd alternate", 0, window, xpWidgetClass_Caption);

	// destination
	y = y - 12;
	XPCreateWidget(x+8, y, x+20, y-22, 1, "-", 0, window, xpWidgetClass_Caption);
	destTextField = XPCreateWidget(x+20, y, x+20+46, y-22, 1, "LSZE", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(destTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(destTextField, xpProperty_MaxCharacters, 4);

	// EET
	eetTextField = XPCreateWidget(x+112, y, x+112+46, y-22, 1, "0030", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(eetTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(eetTextField, xpProperty_MaxCharacters, 4);

	// alternate
	alternateTextField = XPCreateWidget(x+190, y, x+190+46, y-22, 1, "EDNY", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(alternateTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(alternateTextField, xpProperty_MaxCharacters, 4);

	// 2nd alternate
	alternate2TextField = XPCreateWidget(x+280, y, x+280+46, y-22, 1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(alternate2TextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(alternate2TextField, xpProperty_MaxCharacters, 4);

	XPCreateWidget(x+355, y, x+370, y-22, 1, "<<=", 0, window, xpWidgetClass_Caption);

	y = y - 24;
	XPCreateWidget(x+20, y, x+180, y-12, 1, "other information", 0, window, xpWidgetClass_Caption);

	// remarks
	y = y - 12;
	XPCreateWidget(x+8, y, x+20, y-22, 1, "-", 0, window, xpWidgetClass_Caption);
	commentsTextField = XPCreateWidget(x+20, y, x+322+24, y-22, 1, "X-IvAp", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(commentsTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(commentsTextField, xpProperty_MaxCharacters, 512);
	XPCreateWidget(x+355, y, x+370, y-22, 1, "<<=", 0, window, xpWidgetClass_Caption);

	y = y - 24;
	XPCreateWidget(x+20, y, x+180, y-12, 1, "supplementary information", 0, window, xpWidgetClass_Caption);

	y = y - 14;
	XPCreateWidget(x+42, y, x+150, y-12, 1, "19 endurance", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+154, y, x+180, y-12, 1, "POB", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+240, y, x+350, y-12, 1, "pilot in command", 0, window, xpWidgetClass_Caption);

	y = y - 12;
	XPCreateWidget(x+8, y, x+40, y-22, 1, "- E /", 0, window, xpWidgetClass_Caption);
	enduranceTextField = XPCreateWidget(x+42, y, x+42+46, y-22, 1, "0300", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(enduranceTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(enduranceTextField, xpProperty_MaxCharacters, 4);

	XPCreateWidget(x+120, y, x+160, y-22, 1, "- P /", 0, window, xpWidgetClass_Caption);
	pobTextField = XPCreateWidget(x+154, y, x+154+30, y-22, 1, "1", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(pobTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(pobTextField, xpProperty_MaxCharacters, 4);

	XPCreateWidget(x+206, y, x+255, y-22, 1, "- C /", 0, window, xpWidgetClass_Caption);
	picTextField = XPCreateWidget(x+240, y, x+322+24, y-22, 1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(picTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(picTextField, xpProperty_MaxCharacters, 30);
	XPCreateWidget(x+355, y, x+370, y-22, 1, "<<=", 0, window, xpWidgetClass_Caption);

	/*
	y = y - 24;
	XPCreateWidget(x+20, y, x+180, y-12, 1, "FMC waypoints", 0, window, xpWidgetClass_Caption);

	y = y - 12;
	XPCreateWidget(x+8, y, x+20, y-22, 1, "-", 0, window, xpWidgetClass_Caption);
	fmcTextField = XPCreateWidget(x+20, y, x+322+24, y-44, 1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(fmcTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(fmcTextField, xpProperty_MaxCharacters, 512);
	XPCreateWidget(x+355, y-32, x+370, y-44, 1, "<<=", 0, window, xpWidgetClass_Caption);
	*/

	y = y - 35;
	// Export
	exportButton = XPCreateWidget(x+20, y, x+55+55, y-22, 1, "FMC Utility", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(exportButton, xpProperty_ButtonType, xpPushButton);
	// Reset
	resetButton = XPCreateWidget(x+8+55+5+55+5, y, x+8+55+5+55+5+55, y-22, 1, "Reset", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(resetButton, xpProperty_ButtonType, xpPushButton);

	// Send fpl
	sendButton = XPCreateWidget(x+220, y, x+220+80, y-22, 1, "Send FPL", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(sendButton, xpProperty_ButtonType, xpPushButton);
	// Cancel
	cancelButton = XPCreateWidget(x+305, y, x+370, y-22, 1, "Cancel", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(cancelButton, xpProperty_ButtonType, xpPushButton);

	// hook up the handler
	XPAddWidgetCallback(window, flightplanFormHandler);
}

int	FlightplanForm::handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMessage_CloseButtonPushed) {
		hide();
		return 1;
	}
	if(inMessage == xpMsg_PushButtonPressed) {
		// cancel
		if(inParam1 == (long)cancelButton) {
			reset();
			hide();
			return 1;
		}

		// reset
		if(inParam1 == (long)resetButton) {
			reset();
			return 1;
		}

		// send
		if(inParam1 == (long)sendButton) {
			send();
			return 1;
		}

		// export
		if(inParam1 == (long)exportButton) {
			xivap.fmcForm().show();

			send(false); // update xivap's flightplan

			char buffer[1024];
			string dep;
			READFORM(departureTextField, dep);
			string dest;
			READFORM(destTextField, dest);
			string route;
			READFORM(routeTextField, route);

			// get the altitude from the flight plan
			int i = XPGetWidgetProperty(alttypePopup, xpProperty_PopupCurrentItem, NULL);
			int multi = 1;
			switch(i) {
				case 0: multi = 100; break;	// FL are * 100 feet
				case 1: multi = 100; break; // Alts are * 100 feet
				case 2: multi = 0; break;   // he filed VFR. Set alt to 0
			}
			string altstr;
			READFORM(altTextField, altstr);
			int fplAlt = static_cast<int>(stringtoi(altstr)) * multi;

			xivap.fmcForm().setFields(dep, dest, route, fplAlt);
			return 1;
		}
	}

	if(inMessage == xpMessage_PopupNewItemPicked) {
		if(inParam1 == (long)actypePopup) {
			unsigned int curItem = XPGetWidgetProperty(actypePopup, xpProperty_PopupCurrentItem, NULL);
			if(curItem >= 0 && curItem < acList.size()) {
				XPSetWidgetDescriptor(actypeTextField, pconst(acList[curItem].icao));
				XPSetWidgetProperty(actypeTextField, xpProperty_ScrollPosition, 0);
				// set WTC
				switch(acList[curItem].category) {
					case 'L':
						XPSetWidgetProperty(wtcPopup, xpProperty_PopupCurrentItem, 0);
						break;
					case 'M':
						XPSetWidgetProperty(wtcPopup, xpProperty_PopupCurrentItem, 1);
						break;
					case 'H':
					default:
						XPSetWidgetProperty(wtcPopup, xpProperty_PopupCurrentItem, 2);
						break;
				}
				updateACList();
				return 1;
			}
		}

		if(inParam1 == (long)alttypePopup) {
			unsigned int curItem = XPGetWidgetProperty(alttypePopup, xpProperty_PopupCurrentItem, NULL);
			if(curItem == 2) // Altitude VFR
				XPSetWidgetDescriptor(altTextField, "");
		}		

		if(inParam1 == (long)airlinePopup) {
			unsigned int curItem = XPGetWidgetProperty(airlinePopup, xpProperty_PopupCurrentItem, NULL);
			if(curItem == 0) {
				XPSetWidgetDescriptor(airlineTextField, "");
			} else if(curItem > 0 && curItem -1 < alList.size()) {
				XPSetWidgetDescriptor(airlineTextField, pconst(alList[curItem-1].icao));
				XPSetWidgetProperty(airlineTextField, xpProperty_ScrollPosition, 0);
			}
			updateALList();
			return 1;
		}
	}

	if(inMessage == xpMsg_KeyPress) {
		XPWidgetID widget = XPGetWidgetWithFocus();

		XPKeyState_t *key = (XPKeyState_t*)inParam1;

		// SDK bug workaround - we don't receive the text field changed message...
		if(widget == actypeTextField)
			callUpdateAC = true;

		if(widget == airlineTextField)
			callUpdateAL = true;

		// don't eat some special chars: backspace, del, leftarrow, rightarrow
		if(key->key == 8 || key->key == 46 || key->key == 28 || key->key == 29) return 0;

		// don't allow to edit the callsign while online
		if(widget == callsignTextField && xivap.online())
			return 1;
		// same for real name
		if(widget == picTextField && xivap.online())
			return 1;

		// these should only get UPPERCASE characters
		if(widget == callsignTextField || widget == departureTextField || widget == actypeTextField
			|| widget == equipTextField || widget == routeTextField || widget == destTextField
			|| widget == alternateTextField || widget == alternate2TextField || widget == airlineTextField
			|| widget == commentsTextField)
		{
			key->key = pt::upcase(key->key);

			if(widget == callsignTextField || widget == departureTextField || widget == destTextField
				|| widget == alternateTextField || widget == alternate2TextField) {
				if(!((key->key >= '0' && key->key <= '9') || (key->key >= 'A' && key->key <= 'Z')))
					return 1; // accept only 0-9 and A-Z
			}
			return 0; // don't consume the keystroke
		}

		// these accept only numbers
		if(widget == deptimeTextField || widget == altTextField || widget == speedTextField
			|| widget == eetTextField || widget == enduranceTextField || widget == pobTextField)
		{
			if(key->key < '0' || key->key > '9') return 1; // 'eat' invalid keystrokes
			return 0;
		}

	}

	if(inMessage == xpMsg_Draw && (callUpdateAC || callUpdateAL)) {
		if(callUpdateAC) {
			callUpdateAC = false;
			updateACList();
		}
		if(callUpdateAL) {
			callUpdateAL = false;
			updateALList();
		}
		return 0;
	}

	return 0;
}

void FlightplanForm::reset()
{	
	setUserCredentials();

	xivap.fpl.load(DEFAULT_FPL_FILE);
	fillForm();
}

void FlightplanForm::fillForm()
{
	if(!xivap.fpl.isValid()) {
		Flightplan f;
		// hardcoded defaults
		f.departure = "LOIH";
		f.destination = "LSZE";
		f.alternate = "EDNY";
		f.aircrafttype = "C172";
		f.flightrules = "V";
		f.typeofflight = "G";
		f.number = "1";
		f.pob = "2";
		f.alttype = "VFR";
		f.hoursenroute = "00";
		f.minenroute = "30";
		f.hoursfuel = "01";
		f.minfuel = "00";
		f.deptimeest = "1200";
		f.wtc = "L";
		f.equip = "S";
		f.transpondertype = "C";
		f.speedtype = "N";
		f.cruisespeed = "0120";
		f.route = "DCT VFR";
		f.remarks = "X-IvAp";
		xivap.fpl = f;
	}

	long i = 0;
	switch(xivap.fpl.flightrules[0]) {
		case 'V': i = 0; break;
		case 'Y': i = 2; break;
		case 'Z': i = 3; break;
		default: i = 1; break; // default to I
	}
	XPSetWidgetProperty(flightRulesPopup, xpProperty_PopupCurrentItem, i);

	switch(xivap.fpl.typeofflight[0]) {
		case 'S': i = 0; break;
		case 'N': i = 1; break;
		case 'G': i = 2; break;
		case 'M': i = 3; break;
		default: i = 4; break;	// default to X
	}
	XPSetWidgetProperty(typePopup, xpProperty_PopupCurrentItem, i);

	switch(xivap.fpl.wtc[0]) {
		case 'L': i = 0; break;
		case 'M': i = 1; break;
		default:
		case 'H': i = 2; break;
	}
	XPSetWidgetProperty(wtcPopup, xpProperty_PopupCurrentItem, i);

	XPSetWidgetDescriptor(numberTextField, pconst(xivap.fpl.number));
	XPSetWidgetDescriptor(actypeTextField, pconst(xivap.fpl.aircrafttype));
	XPSetWidgetDescriptor(airlineTextField, pconst(xivap.fpl.airline));
	XPSetWidgetDescriptor(equipTextField, pconst(xivap.fpl.equip));

	switch(xivap.fpl.transpondertype[0]) {
		case 'C': i = 0; break;
		case 'S': i = 1; break;
		case 'X': i = 2; break;
		case 'P': i = 3; break;
		case 'I': i = 4; break;
		case 'A': i = 5; break;
		default:
		case 'N': i = 6; break;
	}
	XPSetWidgetProperty(xpdrPopup, xpProperty_PopupCurrentItem, i);

	XPSetWidgetDescriptor(departureTextField, pconst(xivap.fpl.departure));
	XPSetWidgetDescriptor(deptimeTextField, pconst(xivap.fpl.deptimeest));

	switch(xivap.fpl.speedtype[0]) {
		case 'N': i = 0; break;
		default:
		case 'M': i = 1; break;
	}
	XPSetWidgetProperty(speedtypePopup, xpProperty_PopupCurrentItem, i);

	XPSetWidgetDescriptor(speedTextField, pconst(xivap.fpl.cruisespeed));

	switch(xivap.fpl.alttype[0]) {
		case 'F': i = 0; break;
		case 'A': i = 1; break;
		default:
		case 'V': i = 2; break;
	}
	XPSetWidgetProperty(alttypePopup, xpProperty_PopupCurrentItem, i);

	XPSetWidgetDescriptor(altTextField, pconst(xivap.fpl.cruisealt));
	XPSetWidgetDescriptor(routeTextField, pconst(xivap.fpl.route));
	XPSetWidgetDescriptor(destTextField, pconst(xivap.fpl.destination));
	XPSetWidgetDescriptor(eetTextField, pconst(xivap.fpl.hoursenroute + xivap.fpl.minenroute));
	XPSetWidgetDescriptor(alternateTextField, pconst(xivap.fpl.alternate));
	XPSetWidgetDescriptor(alternate2TextField, pconst(xivap.fpl.alternate2));
	XPSetWidgetDescriptor(commentsTextField, pconst(xivap.fpl.remarks));
	XPSetWidgetDescriptor(enduranceTextField, pconst(xivap.fpl.hoursfuel + xivap.fpl.minfuel));
	XPSetWidgetDescriptor(pobTextField, pconst(xivap.fpl.pob));
	if(!xivap.online()) {
		XPSetWidgetDescriptor(picTextField, pconst(xivap.fpl.pic));
		XPSetWidgetDescriptor(callsignTextField, pconst(xivap.fpl.callsign));
	}
	//XPSetWidgetDescriptor(fmcTextField, pconst(xivap.fpl.fmcroute));

	updateACList();
	updateALList();

	if(length(xivap.fpl.livery) > 0 && livList.size() > 0) {
		for(unsigned int i = 0; i < livList.size(); ++i)
			if(xivap.fpl.livery == livList[i].code)
				XPSetWidgetProperty(liveryPopup, xpProperty_PopupCurrentItem, i+1);
	}
}


void FlightplanForm::setFPTextFields()
{
return;
	if (length(xivap.deptAirport()) > 0) 
	{
		XPSetWidgetDescriptor(departureTextField, pconst(xivap.deptAirport()));
	}
	if (length(xivap.destAirport()) > 0) 
	{
		XPSetWidgetDescriptor(destTextField, pconst(xivap.destAirport()));
	}
	if (length(xivap.flightRoute()) > 0) 
	{
		XPSetWidgetDescriptor(routeTextField, pconst(xivap.flightRoute()));
	}	
}

void FlightplanForm::setAcfIcao()
{
	XPSetWidgetDescriptor(actypeTextField, pconst(xivap.acType()));
	updateACList();
}

void FlightplanForm::setUserCredentials()
{
	if(length(xivap.callsign) > 0)
		XPSetWidgetDescriptor(callsignTextField, pconst(xivap.callsign));
	if(length(xivap.realname) > 0)
		XPSetWidgetDescriptor(picTextField, pconst(trim(xivap.realname)));
}

void FlightplanForm::send(bool closeWindow)
{
	char buffer[1024];

	READFORM(callsignTextField, xivap.fpl.callsign);
	xivap.fpl.callsign = trim(strupcase(xivap.fpl.callsign));

	READFORM(picTextField, xivap.fpl.pic);
	xivap.fpl.pic = trim(xivap.fpl.pic);

	if(!xivap.online()) {
		xivap.realname = trim(xivap.fpl.pic);
		xivap.callsign = trim(strupcase(xivap.fpl.callsign));
	}

	int i = XPGetWidgetProperty(flightRulesPopup, xpProperty_PopupCurrentItem, NULL);
	switch(i) {
		case 0: xivap.fpl.flightrules = "V"; break;
		case 1: xivap.fpl.flightrules = "I"; break;
		case 2: xivap.fpl.flightrules = "Y"; break;
		case 3: xivap.fpl.flightrules = "Z"; break;
	}

	i = XPGetWidgetProperty(typePopup, xpProperty_PopupCurrentItem, NULL);
	switch(i) {
		case 0: xivap.fpl.typeofflight = "S"; break;
		case 1: xivap.fpl.typeofflight = "N"; break;
		case 2: xivap.fpl.typeofflight = "G"; break;
		case 3: xivap.fpl.typeofflight = "M"; break;
		case 4: xivap.fpl.typeofflight = "X"; break;
	}

	READFORM(numberTextField, xivap.fpl.number);
	READFORM(actypeTextField, xivap.fpl.aircrafttype);
	READFORM(airlineTextField, xivap.fpl.airline);

	i = XPGetWidgetProperty(wtcPopup, xpProperty_PopupCurrentItem, NULL);
	switch(i) {
		case 0: xivap.fpl.wtc = "L"; break;
		case 1: xivap.fpl.wtc = "M"; break;
		case 2: xivap.fpl.wtc = "H"; break;
	}
	
	READFORM(equipTextField, xivap.fpl.equip);

	i = XPGetWidgetProperty(xpdrPopup, xpProperty_PopupCurrentItem, NULL);
	switch(i) {
		case 0: xivap.fpl.transpondertype = "C"; break;
		case 1: xivap.fpl.transpondertype = "S"; break;
		case 2: xivap.fpl.transpondertype = "X"; break;
		case 3: xivap.fpl.transpondertype = "P"; break;
		case 4: xivap.fpl.transpondertype = "I"; break;
		case 5: xivap.fpl.transpondertype = "A"; break;
		case 6: xivap.fpl.transpondertype = "N"; break;
	}

	READFORM(departureTextField, xivap.fpl.departure);
	READFORM(deptimeTextField, xivap.fpl.deptimeest);

	i = XPGetWidgetProperty(speedtypePopup, xpProperty_PopupCurrentItem, NULL);
	switch(i) {
		case 0: xivap.fpl.speedtype = "N"; break;
		case 1: xivap.fpl.speedtype = "M"; break;
	}

	READFORM(speedTextField, xivap.fpl.cruisespeed);

	i = XPGetWidgetProperty(alttypePopup, xpProperty_PopupCurrentItem, NULL);
	switch(i) {
		case 0: xivap.fpl.alttype = "F"; break;
		case 1: xivap.fpl.alttype = "A"; break;
		case 2: xivap.fpl.alttype = "VFR"; break;
	}
	READFORM(altTextField, xivap.fpl.cruisealt);

	READFORM(routeTextField, xivap.fpl.route);
	READFORM(destTextField, xivap.fpl.destination);

	string dummy;
	READFORM(eetTextField, dummy);
	while(length(dummy) < 4) dummy = "0" + dummy;
	xivap.fpl.hoursenroute = copy(dummy, 0, 2);
	xivap.fpl.minenroute = copy(dummy, 2, 2);

	READFORM(alternateTextField, xivap.fpl.alternate);
	READFORM(alternate2TextField, xivap.fpl.alternate2);
	READFORM(commentsTextField, xivap.fpl.remarks);

	READFORM(enduranceTextField, dummy);
	while(length(dummy) < 4) dummy = "0" + dummy;
	xivap.fpl.hoursfuel = copy(dummy, 0, 2);
	xivap.fpl.minfuel = copy(dummy, 2, 2);

	READFORM(pobTextField, xivap.fpl.pob);

	i = XPGetWidgetProperty(liveryPopup, xpProperty_PopupCurrentItem, NULL);
	xivap.fpl.livery = "";
	if(livList.size() > 0 && i > 0)
		xivap.fpl.livery = livList[i-1].code;

	xivap.fpl.normalize();

	// stop here if all we want to do is setting the internal flightplan
	// to the correct values
	if(!closeWindow)
		return;

	if(xivap.fpl.isValid()) {
		xivap.setAcType(xivap.fpl.aircrafttype, xivap.fpl.airline, xivap.fpl.livery);
		xivap.sendFlightplan();
		xivap.fpl.save(DEFAULT_FPL_FILE);
		xivap.connectForm().setUserCredentials();
		xivap.connectForm().hide();
		hide();
	} else {
		xivap.uiWindow.addMessage(colYellow, "Flight plan error: " + xivap.fpl.errorMessage());
		xivap.messageBox().show("Flight plan error: " + xivap.fpl.errorMessage());
	}
}

void FlightplanForm::updateACList()
{
	char buffer[1024];

	string actype;
	READFORM(actypeTextField, actype);
	acList = xivap.aircraftDB.retrieve(actype);
	string caption = "";
	for(AircraftDB::AircraftList::const_iterator it = acList.begin(); it != acList.end(); ++it)
		caption += it->icao + " (" + it->manufacturer + " " + it->modelname + ");";
	if(length(caption) < 1) caption = ";";
	XPSetWidgetDescriptor(actypePopup, pconst(caption));

	unsigned int i = XPGetWidgetProperty(actypePopup, xpProperty_PopupCurrentItem, NULL);
	if(i >= acList.size())
		XPSetWidgetProperty(actypePopup, xpProperty_PopupCurrentItem, 0);
}

void FlightplanForm::updateALList()
{
	char buffer[1024];

	string altype;
	READFORM(airlineTextField, altype);
	alList = xivap.airlinesDB.retrieve(altype);
	string caption = "none;";
	for(AirlinesDB::AirlinesList::const_iterator it = alList.begin(); it != alList.end(); ++it) {
		caption += it->icao + " (" + it->name; 
		if(it->va) caption += " VA";
		caption += ");";
	}
	XPSetWidgetDescriptor(airlinePopup, pconst(caption));

	unsigned int i = XPGetWidgetProperty(airlinePopup, xpProperty_PopupCurrentItem, NULL);
	if(i >= alList.size() || alList.size() == 1)
		XPSetWidgetProperty(airlinePopup, xpProperty_PopupCurrentItem, 1);

	livList = xivap.airlinesDB.getLiveries(altype);
	caption = "none;";
	for(AirlinesDB::LiveryList::const_iterator it = livList.begin(); it != livList.end(); ++it)
		caption += it->description + ";";
	XPSetWidgetDescriptor(liveryPopup, pconst(caption));
	if(livList.size() == 0)
		XPSetWidgetProperty(liveryPopup, xpProperty_PopupCurrentItem, 0);

}
