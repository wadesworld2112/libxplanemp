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
#include "messageWindow.h"
#include "systemcallbacks.h"
#include "configFile.h"
#include "helpers.h"
#include "xivap.h"

MessageWindow::MessageWindow()
{
	window = NULL;
	elapsed = 0.0f;
}

MessageWindow::~MessageWindow()
{
	pluginStop();
}

void MessageWindow::create()
{
	// get font dimension
	XPLMGetFontDimensions(xplmFont_Basic, &fontWidth, &fontHeight, NULL);

	int x1, y1, x2, y2;

	// init window
	x2 = xivap.renderWindowWidth() - fontWidth * 2;
	x1 = x2 - columns * fontWidth - 10;
	y1 = xivap.renderWindowHeight() - 2 * fontHeight;
	y2 = y1 - fontHeight - 10;

	window = XPLMCreateWindow(x1, y1, x2, y2, 0,		// start invisible
							  msgTextWinDrawCallback, 
							  msgTextWinKeyCallback,
							  msgTextWinMouseCallback,
							  NULL);

	ConfigFile config;
	string str = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(str);
	str = config.readConfig("PREFERENCES", "MESSAGESDARK");
	if(str == "1") // default to light dark
		_extradark = true;
	else 
		_extradark = false;

	str = config.readConfig("PREFERENCES", "MESSAGESVISIBLE");
	if(str == "0")  // default to visible
		_visible = false;
	else 
		_visible = true;

	str = config.readConfig("PREFERENCES", "MESSAGESSHOWDURATION");
	int duration = atoi(pconst(str));
	if(duration > 0)
		_msgShowDuration = duration;
	else
		_msgShowDuration = seconds;
}

// callbacks
void MessageWindow::messageDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	float now = XPLMGetElapsedTime();
	if(elapsed + 1 < now) {
		elapsed = now;
		updateStatus();
	}

	// First we get the location of the window passed in to us
	int left, top, right, bottom;
	XPLMGetWindowGeometry(window, &left, &top, &right, &bottom);

	// if necessary, change the window size
	int bottom2 = top - 10 - static_cast<int>(messages.size()) * fontHeight;
	if(bottom2 != bottom) {
		bottom = bottom2;
		XPLMSetWindowGeometry(window, left, top, right, bottom);
	}
	
	// Draw a translucent dark rectangle that is our window's shape
	XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	// we want it a bit darker - draw that box a second time :)
	if(_extradark) XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	// draw text messages
	int j = 0;
	for(std::deque<Line>::const_iterator i = messages.begin(); i != messages.end(); ++i) {
		XPLMDrawString(i->color, left+5, bottom + 5 + j*fontHeight,
			const_cast<char*>(pconst(i->text)), NULL, xplmFont_Basic);
		++j;
	}
}

void MessageWindow::messageKeyCallback(XPLMWindowID inWindowID, char inKey,
									   XPLMKeyFlags inFlags, char inVirtualKey,
									   void *inRefcon, int losingFocus)
{
}

int MessageWindow::messageMouseCallback(XPLMWindowID inWindowID, int x, int y,
										XPLMMouseStatus inMouse, void *inRefcon)
{
	return 0;
}

// add one message string to the window. might be split into multiple lines if it is too long
void MessageWindow::addMessage(float *color, string message)
{
	float timestamp = XPLMGetElapsedTime();

	// wrap line if necessary
	string line = message;

	while(length(line) > columns) {
		int p = columns;
		while(line[p] != ' ' && p > 15) --p;
		if(p <= 15) p = columns; // no space found in first part of line -> wrap at last column

		_appendText(color, copy(line, 0, p), timestamp);
		del(line, 0, p+1);
		if(length(line) > 0)
			line = "   " + line;
	}

	if(length(line) > 0)
		_appendText(color, line, timestamp);

	if( _visible && !visible()) show();
}

void MessageWindow::_appendText(float *color, string text, float &timestamp)
{
	if(messages.size() >= maxlines)
		messages.pop_back();
	MessageWindow::Line l;
	l.color = color;
	l.text = text;
	l.timestamp = timestamp;
	messages.push_front(l);
}

// remove old messages from the window
void MessageWindow::updateStatus()
{
	if(messages.size() > 0) {
		std::deque<Line>::iterator it = messages.begin();
		while(it != messages.end()) {
			if(it->timestamp + _msgShowDuration < elapsed) {
				messages.erase(it);
				it = messages.begin();
			} else {
				++it;
			}
		}
	}
	if(messages.size() == 0 && visible()) hide();
}

void MessageWindow::setExtradark(bool value)
{
	_extradark = value;
	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
	config.setConfig("PREFERENCES", "MESSAGESDARK", (_extradark ? "1" : "0"));
	config.save(filename);
}
