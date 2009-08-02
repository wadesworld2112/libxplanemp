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

#ifndef SYSTEMCALLBACKS_H_
#define SYSTEMCALLBACKS_H_

#define MENU_CONNECT 0
#define MENU_DISCONNECT 1
#define MENU_FLIGHTPLAN 2
#define MENU_PREF 3
#define MENU_CONSOLE 4
#define MENU_MAIN 5
#define MENU_ABOUT 6

// empty callbacks
void emptyDrawCallback(XPLMWindowID inWindowID,  void *inRefcon);
void emptyKeyCallback(XPLMWindowID inWindowID,	char inKey,
					 XPLMKeyFlags inFlags,		char inVirtualKey,
						void *inRefcon,			int losingFocus);

int focusdumpMouseCallback(XPLMWindowID inWindowID,
						 int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

// console callbacks
void consoleDrawCallback(XPLMWindowID inWindowID,  void *inRefcon);

float FlightLoopCallback(float inElapsedSinceLastCall,
						 float inElapsedTimeSinceLastFlightLoop,
						 int inCounter, void *inRefcon);

void MenuHandlerCallback(void *inMenuRef, void *inItemRef);

int connectFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int disconnectFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int flightplanFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int fmcFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int msgBoxHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int inspectorFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);

// UI callbacks
void uiToggleCallback(void *inRefcon);
void uiTextWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon);
void uiTextWinToggleCallback(void *inRefcon);
void uiTextWinKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus);
int uiTextWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

// message window callbacks
void msgTextWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon);
void msgTextWinKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus);
int msgTextWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

// chat window callbacks
void chatTextWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon);
void chatTextWinKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus);
int chatTextWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

#endif
