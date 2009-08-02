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
#include "common.h"
#include "xivap.h"
#include "systemcallbacks.h"
#include "messageHandler.h"

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
#ifdef INTERNAL_BETA
	if(!xivap.betaBlocker.CheckDate())
		return 0;
#endif

	strcpy(outName, "X-IvAp");
	strcpy(outSig, "ivao.xivap");
	string desc = string(SOFTWARE_NAME) + " " 
		+ SOFTWARE_VERSION + " (" + SOFTWARE_REVISION + ") - Fly online on IVAO " + SOFTWARE_COPYRIGHT;
	strcpy(outDesc, pconst(desc));

	int mySubMenuItem = XPLMAppendMenuItem(XPLMFindPluginsMenu(), // Put in plugins menu
						"X-IvAp",	// Item Title
						0,			// Item Ref
						1);			// Force English

	xivap.menuEntry = XPLMCreateMenu("X-IvAp", 
						XPLMFindPluginsMenu(),
						mySubMenuItem, 			// Menu Item to attach to.
						MenuHandlerCallback,	// The handler
						0);	

	XPLMAppendMenuItem(xivap.menuEntry, "Toggle X-IvAp main window", (void*)MENU_MAIN, 1);
	xivap.menuConnect = XPLMAppendMenuItem(xivap.menuEntry, "Connect...", (void*)MENU_CONNECT, 1);
	xivap.menuDisconnect = XPLMAppendMenuItem(xivap.menuEntry, "Disconnect", (void*)MENU_DISCONNECT, 1);
	xivap.menuFlightplan = XPLMAppendMenuItem(xivap.menuEntry, "Send Flightplan...", (void*)MENU_FLIGHTPLAN, 1);
	XPLMAppendMenuItem(xivap.menuEntry, "Toggle console (debug) window", (void*)MENU_CONSOLE, 1);
	XPLMAppendMenuItem(xivap.menuEntry, "About...", (void*)MENU_ABOUT, 1);

	// disable disconnect menu entry
	XPLMEnableMenuItem(xivap.menuEntry, xivap.menuDisconnect, 0);

	xivap.XPluginStart();

	// Register our callback for once every frame
	XPLMRegisterFlightLoopCallback(FlightLoopCallback,		// Callback
								   -1.0,					// Interval -1.0 = once per frame
								   NULL);					// refcon not used.

	// We must return 1 to indicate successful initialization, otherwise we
	// will not be called back again. */

	return 1;
}

PLUGIN_API void	XPluginStop(void)
{
#ifdef BETA
	if(!xivap.betaBlocker.CheckDate())
		return;
#endif

	// Unregister the callback
	XPLMUnregisterFlightLoopCallback(FlightLoopCallback, NULL);

	xivap.XPluginStop();
}

PLUGIN_API void XPluginDisable(void)
{
#ifdef BETA
	if(!xivap.betaBlocker.CheckDate())
		return;
#endif

	xivap.XPluginDisable();
}

// macro to register RW callbacks
#define CALLBACK_REGISTER_INT(ref) \
	XPLMRegisterDataAccessor(ref, xplmType_Int, 1, &Handler_getInteger, &Handler_setInteger, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, ref, ref)

#define CALLBACK_REGISTER_STR(ref) \
	XPLMRegisterDataAccessor(ref, xplmType_Data, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &Handler_getString, &Handler_setString, ref, ref)

PLUGIN_API int XPluginEnable(void)
{
#ifdef BETA
	if(!xivap.betaBlocker.CheckDate())
		return 0;
#endif

	xivap.XPluginEnable();

	// ----------------------------------
	// register callbacks for plugin SDK
	// ----------------------------------

	// int, readonly
	XPLMRegisterDataAccessor(XSB_VERS_NUMBER, xplmType_Int, 0, &Handler_getInteger, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, XSB_VERS_NUMBER, XSB_VERS_NUMBER);
	XPLMRegisterDataAccessor(XSB_CON_STATUS, xplmType_Int, 0, &Handler_getInteger, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, XSB_CON_STATUS, XSB_CON_STATUS);

	// string, readonly
	XPLMRegisterDataAccessor(XSB_VERS_STRING, xplmType_Data, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &Handler_getString, NULL, XSB_VERS_STRING, XSB_VERS_STRING);

	CALLBACK_REGISTER_INT(XSB_CON_PORT);
	CALLBACK_REGISTER_INT(XSB_FP_FLIGHT_TYPE);
	CALLBACK_REGISTER_INT(XSB_FP_TCAS_TYPE);
	CALLBACK_REGISTER_INT(XSB_FP_NAV_TYPE);
	CALLBACK_REGISTER_INT(XSB_FP_SPEED);
	CALLBACK_REGISTER_INT(XSB_FP_DEPARTURE_TIME);
	CALLBACK_REGISTER_INT(XSB_FP_DEPARTURE_TIME_ACTUAL);
	CALLBACK_REGISTER_INT(XSB_FP_ENROUTE_HRS);
	CALLBACK_REGISTER_INT(XSB_FP_ENROUTE_MINS);
	CALLBACK_REGISTER_INT(XSB_FP_FUEL_HRS);
	CALLBACK_REGISTER_INT(XSB_FP_FUEL_MINS);

	CALLBACK_REGISTER_STR(XSB_VERS_STRING);
	CALLBACK_REGISTER_STR(XSB_CON_CALLSIGN);
	CALLBACK_REGISTER_STR(XSB_CON_SERVER);
	CALLBACK_REGISTER_STR(XSB_CON_PILOT_ID);
	CALLBACK_REGISTER_STR(XSB_CON_PASSWORD);
	CALLBACK_REGISTER_STR(XSB_CON_REALNAME);
	CALLBACK_REGISTER_STR(XSB_CON_MODEL);
	CALLBACK_REGISTER_STR(XSB_FP_CRUISE_ALTITUDE);
	CALLBACK_REGISTER_STR(XSB_FP_CRUISE_ALTITUDE);
	CALLBACK_REGISTER_STR(XSB_FP_ALTERNATE_AIRPORT);
	CALLBACK_REGISTER_STR(XSB_FP_DEPARTURE_AIRPORT);
	CALLBACK_REGISTER_STR(XSB_FP_REMARKS);
	CALLBACK_REGISTER_STR(XSB_FP_ROUTE);

	// ----------------------------------

	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, long inMsg, void *inParam)
{
#ifdef BETA
	if(!xivap.betaBlocker.CheckDate())
		return;
#endif

	MessageHandler(inFrom, inMsg, inParam);
}

float FlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,    
                         int inCounter, void *inRefcon)
{
	// multiplayer paint hook
	xivap.flightLoopCallback();

	return -1.0;
}                                   

void consoleDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	int		left, top, right, bottom;
	
	// First we get the location of the window passed in to us.
	XPLMGetWindowGeometry(inWindowID, &left, &top, &right, &bottom);
	
	// We now use an XPLMGraphics routine to draw a translucent dark
	// rectangle that is our window's shape.
	XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	// Finally we draw the text into the window, also using XPLMGraphics
	// routines.  The NULL indicates no word wrapping.
	for(unsigned i = 0; i < xivap.consoleText.size(); ++i) {
		XPLMDrawString(xivap.consoleText[i].first,	// color
					   left + 5, bottom + 5 + i * 10, // bottom left
					   const_cast<char*>(pconst(xivap.consoleText[i].second)),
					   NULL, xplmFont_Basic);		
	}
}                                   

void emptyDrawCallback(XPLMWindowID inWindowID,  void *inRefcon)
{
}

void emptyKeyCallback(XPLMWindowID inWindowID,
						char inKey, XPLMKeyFlags inFlags,
						char inVirtualKey, void *inRefcon, int losingFocus)
{
}

int focusdumpMouseCallback(XPLMWindowID inWindowID,
						 int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	xivap.dumpKeyboardFocus();
	XPLMTakeKeyboardFocus(0);
	return 0;
}

void MenuHandlerCallback(void *inMenuRef, void *inItemRef)
{
	int menuEntry = (int)inItemRef;
	switch(menuEntry) {
		case MENU_CONNECT:
			xivap.connectForm().show();
			break;
		case MENU_DISCONNECT:
			xivap.disconnectForm().show();
			break;
		case MENU_FLIGHTPLAN:
			xivap.flightplanForm().show();
			break;
		case MENU_CONSOLE:
			if(xivap.consoleVisible()) xivap.setConsoleVisible(false);
			else xivap.setConsoleVisible(true);
			break;
		case MENU_MAIN:
			uiToggleCallback(NULL);
			break;
		case MENU_ABOUT:
			xivap.messageBox().show(string(FSD_SOFTWARE_NAME)
				+ " " + SOFTWARE_VERSION + "(Rev. " + xivap.revision() + ")  -  " 
				+ SOFTWARE_COPYRIGHT + "     www.ivao.aero/softdev/X-IvAp");
			break;

		default:
			break;
	}
}                                   


int	connectFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.connectForm().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	disconnectFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.disconnectForm().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	msgBoxHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.messageBox().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	flightplanFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.flightplanForm().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	fmcFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.fmcForm().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	inspectorFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.refInspector().handler(inMessage, inWidget, inParam1, inParam2);
}						

// UI callbacks
void uiTextWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	xivap.uiWindow.textDrawCallback(inWindowID, inRefcon);
}

void uiTextWinToggleCallback(void *inRefcon)
{
	xivap.uiWindow.textToggleCallback(inRefcon);
}

void uiTextWinKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus)
{
	xivap.uiWindow.textKeyCallback(inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus);
}

int uiTextWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	return xivap.uiWindow.textMouseCallback(inWindowID, x, y, inMouse, inRefcon);
}

void uiToggleCallback(void *inRefcon)
{
	if(xivap.uiWindow.visible()) xivap.uiWindow.hide();
	else xivap.uiWindow.show();
}

// message window callbacks
void msgTextWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	xivap.msgWindow.messageDrawCallback(inWindowID, inRefcon);
}

void msgTextWinKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus)
{
	xivap.msgWindow.messageKeyCallback(inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus);
}

int msgTextWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	return xivap.msgWindow.messageMouseCallback(inWindowID, x, y, inMouse, inRefcon);
}

// chat window callbacks
void chatTextWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	ChatWindow *obj = static_cast<ChatWindow*>(inRefcon);
	obj->chatDrawCallback(inWindowID, inRefcon);
}

void chatTextWinKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus)
{
	ChatWindow *obj = static_cast<ChatWindow*>(inRefcon);
	obj->chatKeyCallback(inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus);
}

int chatTextWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	ChatWindow *obj = static_cast<ChatWindow*>(inRefcon);
	return obj->chatMouseCallback(inWindowID, x, y, inMouse, inRefcon);
}
