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

#ifndef CHAT_WINDOW_
#define CHAT_WINDOW_

#include "common.h"

#define CHAT_CLOSE_BUTTON 1
#define CHAT_TOGGLE_BUTTON 2
#define CHAT_DARKEN_BUTTON 3

#include <deque>

class ChatWindow
{
public:
	ChatWindow(const string& callsign, int x = 20, int y = 768 - 20);
	~ChatWindow();

	void addMessage(float* color, string message);

	// callbacks
	void chatDrawCallback(XPLMWindowID inWindowID, void *inRefcon);
	void chatKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
		void *inRefcon, int losingFocus);
	int chatMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

	int checkClick(int x, int y);
	int continueClick(int x, int y);
	int endClick(int x, int y);
	int checkButton(int x, int y);
	void buttonClicked(int button);
	int beginDrag(int x, int y);
	int continueDrag(int x, int y);
	int endDrag(int x, int y);

	bool hasCallsign(const string& callsign);
	void grabKeyboardFocus();
	void toggleExtended();

	int getTop() { return top; };
	int getLeft() { return left; };

	bool visible() { return XPLMGetWindowIsVisible(window) != 0; };
	void hide() { XPLMSetWindowIsVisible(window, 0); };

private:
	XPLMWindowID window;
	int fontWidth, fontHeight;
	int columns, rows;
	int scroll;
	int top, left;
	bool _extradark, _keyboardfocus, _extended;
	int getX(int column) { return left + 5 + column * fontWidth; };
	int getY(int line)   { return top - 5 - (line+1) * fontHeight; };

	bool clicking, dragging;
	int mDownX, mDownY, clickButton;

	typedef std::pair<float*, string> line;
	std::deque<line> messages;
	void _appendText(float* color, string line);

	std::vector<string> callsigns;
	string titleLine, editLine, displayLine;
	void processLine(string line);

	const static int maxlines = 300;

	void keyEnter();
	void keyBackspace();
	void keyUpArrow();
	void keyDownArrow();
	void keyLeftArrow();
	void keyRightArrow();
	void handleKey(int key);

	float _lastKeyPress, _cursorTime;
	bool _cursor, _newText;

};

#endif
