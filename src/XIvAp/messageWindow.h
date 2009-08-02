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

#ifndef MESSAGE_WINDOW_
#define MESSAGE_WINDOW_

#include "common.h"

#include <deque>

class MessageWindow
{
public:
	MessageWindow();
	~MessageWindow();

	void addMessage(float* color, string message);

	// callbacks
	void messageDrawCallback(XPLMWindowID inWindowID, void *inRefcon);
	void messageKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
		void *inRefcon, int losingFocus);
	int messageMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

	void setExtradark(bool value);

	bool visible() { return XPLMGetWindowIsVisible(window) != 0; };
	void show() { XPLMSetWindowIsVisible(window, 1); };
	void hide() { XPLMSetWindowIsVisible(window, 0); };

	void pluginStart() { create(); };
	void pluginStop() { if(window) XPLMDestroyWindow(window); window = NULL; };

private:
	void create();

	class Line {
	public:
		Line(): color(NULL), text(""), timestamp(0.0f) {};

		float* color;
		string text;
		float timestamp;
	};
	std::deque<Line> messages; // the messages
	void _appendText(float* color, string text, float& timestamp);

	// discard old messages
	void updateStatus();

	int fontWidth, fontHeight;

	// window handle
	XPLMWindowID window;
	bool _extradark;
	bool _visible;
	float elapsed;

	const static int columns = 80;
	const static int maxlines = 8;
	const static int seconds = 20;
	int _msgShowDuration;

};

#endif