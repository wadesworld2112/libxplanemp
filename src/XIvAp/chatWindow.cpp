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
#include "chatWindow.h"
#include "systemcallbacks.h"
#include "xivap.h"
#include "helpers.h"

ChatWindow::ChatWindow(const string& callsign, int x, int y)
{
	columns = 60;
	rows = 8;
	XPLMGetFontDimensions(xplmFont_Basic, &fontWidth, &fontHeight, NULL);
	_extradark = false;
	_extended = true;

	mDownX = mDownY = 0;
	scroll = 0;
	clicking = dragging = false;
	titleLine = editLine = displayLine = "";

	_lastKeyPress = _cursorTime = 0.0f;
	_cursor = true;
	_newText = false;

	if(x > 900) x = 900;
	if(x < 0) x = 0;
	if(y > 750) y = 750;
	if(y < 15) y = 15;

	callsigns.push_back(strupcase(trim(callsign)));
	titleLine = callsign;
	if(length(titleLine) > columns - 9)
		titleLine = copy(titleLine, 0, columns - 12) + "...";

	int x2, y2;
	top = y;
	left = x;

	// init window
	x2 = x + fontWidth * columns + 10;
	y2 = y - fontHeight * rows - 10;

	window = XPLMCreateWindow(x, y, x2, y2, 1,
							  chatTextWinDrawCallback, 
							  chatTextWinKeyCallback,
							  chatTextWinMouseCallback,
							  this);
	grabKeyboardFocus();

}

void ChatWindow::grabKeyboardFocus()
{
	XPLMTakeKeyboardFocus(window);
	XPLMBringWindowToFront(window);
	_keyboardfocus = true;
	_cursor = true;
	_newText = false;
}

ChatWindow::~ChatWindow()
{
	if(_keyboardfocus) XPLMTakeKeyboardFocus(0);
	if(window) XPLMDestroyWindow(window);
}

// callbacks
void ChatWindow::chatDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	// First we get the location of the window passed in to us
	int right, bottom;
	XPLMGetWindowGeometry(window, &left, &top, &right, &bottom);

	// Draw a translucent dark rectangle that is our window's shape
	XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	// we want it a bit darker - draw that box a second time :)
	if(_extradark) XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	// title line
	float *col = colDarkGreen;
	if(_newText) col = colYellow;

	XPLMDrawString(col, getX(0), getY(0), const_cast<char*>(pconst(titleLine)),
		NULL, xplmFont_Basic);

	// minimize and close button
	XPLMDrawString(colWhite, getX(columns - 9), getY(0), "[D][_][X]", NULL, xplmFont_Basic);

	if(_extended) {
		// dashed line
		string dashes = "";
		for(int i = 0; i < columns; ++i) dashes += "-";
		XPLMDrawString(colWhite, getX(0), getY(rows-2), const_cast<char*>(pconst(dashes)),
			NULL, xplmFont_Basic);

		// blinking cursor
		float now = XPLMGetElapsedTime();
		if(!_keyboardfocus)
			_cursor = false;
		else if(_cursorTime + 1 < now) {
			_cursorTime = now;
			_cursor = !_cursor;
		}

		// edit line
		string edl = displayLine;
		if(_cursor) edl += "_";
		XPLMDrawString(colGreen, getX(0), getY(rows-1), const_cast<char*>(pconst(edl)),
			NULL, xplmFont_Basic);

		// draw text messages
		int j = 0;
		for(std::deque<line>::const_iterator i = messages.begin(); i != messages.end() && j - scroll < rows - 3; ++i) {
			if(j >= scroll) {
				XPLMDrawString(i->first, left+5, getY(rows - 3 - j + scroll),
					const_cast<char*>(pconst(i->second)), NULL, xplmFont_Basic);
			}
			++j;
		}
	}
}

void ChatWindow::chatKeyCallback(XPLMWindowID inWindowID, char inKey,
									   XPLMKeyFlags inFlags, char inVirtualKey,
									   void *inRefcon, int losingFocus)
{
	if(losingFocus) {
		_keyboardfocus = false;
		_cursor = false;
		return;
	}

	// drop keyboard focus if window is not extended
	// attention - put this BELOW the above if(losingFocus), otherwise
	// X-Plane goes into a stack overflow
	if(!_extended) {
		_keyboardfocus = false;
		_cursor = false;
		XPLMTakeKeyboardFocus(0);
		return;
	}

	if((inFlags & (xplm_OptionAltFlag | xplm_ControlFlag)) != 0) {
		// Ctrl or Alt key (Alt doesnt work?) is down
		// This is a shortcut of some kind -> get rid of the keyboard focus
		_keyboardfocus = false;
		_cursor = false;
		XPLMTakeKeyboardFocus(0);
		return;
	}

	// accept key event if key is pressed down or held down
	if(((inFlags & xplm_DownFlag) != 0) // key down
		|| ((inFlags & xplm_DownFlag) == 0 && (inFlags & xplm_UpFlag) == 0)) { // key held down

		if(inKey < 0) return; // special characters on the keyboard

		switch(inKey) {
			case 13: keyEnter(); break;
			case 8: keyBackspace(); break;
			case 30: keyUpArrow(); break;
			case 31: keyDownArrow(); break;
			case 28: keyLeftArrow(); break;
			case 29: keyRightArrow(); break;
			default: handleKey(inKey); break;
		}

		_lastKeyPress = XPLMGetElapsedTime();
		_cursor = true;	// show cursor after keystroke
		_cursorTime = _lastKeyPress;
		_newText = false;
	}
}

int ChatWindow::chatMouseCallback(XPLMWindowID inWindowID, int x, int y,
										XPLMMouseStatus inMouse, void *inRefcon)
{
	if(_extended) _newText = false;
	switch(inMouse) {
		case xplm_MouseDown:
			XPLMBringWindowToFront(window);
			if(_extended)
				grabKeyboardFocus();

			if(checkClick(x, y))
				return 1;
			else
				return beginDrag(x, y);

			break;

		case xplm_MouseDrag:
			if(dragging) return continueDrag(x, y);
			else if(clicking) return continueClick(x, y);
			break;

		case xplm_MouseUp:
			if(dragging) return endDrag(x, y);
			else if(clicking) return endClick(x, y);
			break;
	}
	return 0;
}

void ChatWindow::toggleExtended()
{
	if(_extended) {
		// collapse window
		_extended = false;
		int x2 = left + fontWidth * columns + 10;
		int y2 = top - fontHeight - 10;
		XPLMSetWindowGeometry(window, left, top, x2, y2);

		// get rid of keyboard focus if necessary
		if(_keyboardfocus) {
			_keyboardfocus = false;
			_cursor = false;
			XPLMTakeKeyboardFocus(0);
		}

	} else {
		// extend window
		_extended = true;
		int x2 = left + fontWidth * columns + 10;
		int y2 = top - fontHeight * rows - 10;
		XPLMSetWindowGeometry(window, left, top, x2, y2);
		grabKeyboardFocus();
	}
}


int ChatWindow::checkButton(int x, int y)
{
	if(y <= getY(-1) && y >= getY(0)) {
		// click into top bar
		if(x >= getX(columns - 9) && x < getX(columns - 6))
			return CHAT_DARKEN_BUTTON;
		if(x >= getX(columns - 6) && x < getX(columns - 3))
			return CHAT_TOGGLE_BUTTON;
		if(x >= getX(columns - 3) && x <= getX(columns))
			return CHAT_CLOSE_BUTTON;
	}

	return 0;
}

int ChatWindow::checkClick(int x, int y)
{
	clickButton = checkButton(x, y);
	if(clickButton == 0) {
		clicking = false;
		return 0;
	}
	clicking = true;
	return 1;
}

int ChatWindow::continueClick(int x, int y)
{
	if(clicking) return 1;
	return 0;
}

int ChatWindow::endClick(int x, int y)
{
	int button = checkButton(x, y);
	if(button == clickButton)
		buttonClicked(button);
	clickButton = 0;
	clicking = false;
	return 0;
}

int ChatWindow::beginDrag(int x, int y)
{
	mDownX = x;
	mDownY = y;
	dragging = true;
	return 1;
}

int ChatWindow::continueDrag(int x, int y)
{
	int dX = mDownX - x;
	int dY = mDownY - y;

	left -= dX;
	top -= dY;

	mDownX = x;
	mDownY = y;

	int x2 = left + fontWidth * columns + 10;
	int y2 = top - fontHeight - 10;
	if(_extended) y2 -= (rows-1) * fontHeight;
	XPLMSetWindowGeometry(window, left, top, x2, y2);

	return 1;
}

int ChatWindow::endDrag(int x, int y)
{
	dragging = false;
	return 1;
}

void ChatWindow::buttonClicked(int button)
{
	switch(button) {
		case CHAT_CLOSE_BUTTON:
			xivap.closeChatWindow(this);
			break;

		case CHAT_TOGGLE_BUTTON:
			toggleExtended();
			break;

		case CHAT_DARKEN_BUTTON:
			_extradark = !_extradark;
			break;

		default:
			break;
	}
}

bool ChatWindow::hasCallsign(const string& callsign)
{
	if(!visible()) return false;
	for(std::vector<string>::iterator it = callsigns.begin(); it != callsigns.end(); ++it)
		if(*it == callsign) return true;
	return false;
}

void ChatWindow::addMessage(float *color, string message)
{
	// wrap line if necessary
	string line = message;

	while(length(line) > columns) {
		int p = columns;
		while(line[p] != ' ' && p > 15) --p;
		if(p <= 15) p = columns; // no space found in first part of line -> wrap at last column

		_appendText(color, copy(line, 0, p));
		del(line, 0, p+1);
		if(length(line) > 0)
			line = "   " + line;
	}

	if(length(line) > 0)
		_appendText(color, line);

	_newText = true;
}

void ChatWindow::_appendText(float *color, string str)
{
	// replace " characters with '
	int p = pos('"', str);
	while(p >= 0) {
		str[p] = '\'';
		p = pos('"', str);
	}

	if(messages.size() >= maxlines)
		messages.pop_back();

	messages.push_front(ChatWindow::line(color, str));
	if(scroll > 0) ++scroll;
}

void ChatWindow::keyEnter()
{
	processLine(editLine);
	editLine = displayLine = "";
}

void ChatWindow::keyBackspace()
{
	if(length(editLine) > 0)
		del(editLine, length(editLine) - 1, 1);
	displayLine = editLine;
	if(length(displayLine) > columns - 1)
		displayLine = copy(displayLine, length(displayLine) - columns + 1, length(displayLine));
}

void ChatWindow::keyUpArrow()
{
	if(static_cast<int>(messages.size()) - scroll > rows - 3)
		++scroll;
}

void ChatWindow::keyDownArrow()
{
	if(scroll > 0) --scroll;
}

void ChatWindow::keyLeftArrow()
{
}

void ChatWindow::keyRightArrow()
{
}

void ChatWindow::handleKey(int key)
{
	if(key == '"') key = '\'';
	if(length(editLine) < 300)
		editLine += key;

	displayLine = editLine;
	if(length(displayLine) > columns - 1)
		displayLine = copy(displayLine, length(displayLine) - columns + 1, length(displayLine));
}

void ChatWindow::processLine(string line)
{
	string l2 = strupcase(line);
	if(l2 == ".HELP") {
		addMessage(colYellow, "Everything you type here goes to all callsigns in the titlebar. You don't have to use .MSG here.");
		addMessage(colYellow, "To add a callsign to the list, type +<callsign>");
		addMessage(colYellow, "To remove it, type -<callsign>");
		addMessage(colYellow, "Use your arrow-up and -down keys to scroll up and down in this window");
	} else if(l2[0] == '.') {
		xivap.handleCommand(line);
	} else if(l2[0] == '+') {
		del(l2, 0, 1);
		l2 = trim(l2);
		int p = pos(' ', l2);
		if(p > 0) l2 = copy(l2, 0, p);
		if(!hasCallsign(l2))
			callsigns.push_back(trim(l2));

		titleLine = "";
		for(unsigned int i = 0; i < callsigns.size(); ++i) {
			if(length(titleLine) > 0) titleLine += ",";
			titleLine += callsigns[i];
		}
		if(length(titleLine) > columns - 9)
			titleLine = copy(titleLine, 0, columns - 12) + "...";

	} else if(l2[0] == '-' && callsigns.size() > 1) {
		del(l2, 0, 1);
		l2 = trim(l2);
		int p = pos(' ', l2);
		if(p > 0) l2 = copy(l2, 0, p);
		if(hasCallsign(l2)) {
			std::vector<string>::iterator it = callsigns.begin();
			while(it != callsigns.end()) {
				if(*it == l2) {
					callsigns.erase(it);
					it = callsigns.end();
				} else
					++it;
			}
		}

		titleLine = "";
		for(unsigned int i = 0; i < callsigns.size(); ++i) {
			if(length(titleLine) > 0) titleLine += ",";
			titleLine += callsigns[i];
		}
		if(length(titleLine) > columns - 9)
			titleLine = copy(titleLine, 0, columns - 12) + "...";

	} else {
		addMessage(colGreen, "> " + line);
		_newText = false;
		for(unsigned int i = 0; i < callsigns.size(); ++i)
			xivap.fsd.sendMessage(callsigns[i], line);
	}
}
