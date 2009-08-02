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

#ifndef UI_WINDOW_
#define UI_WINDOW_

#include "common.h"
#include "atcPosition.h"

#include <deque>

// left select button
#define LSK "[--] "
// right select button
#define RSK " [--]"
// length of select button strings (both must be equal in length)
#define SK_LEN 5

#define MARGIN_TOP 5
#define MARGIN_LEFT 5

#define SK_COLOR colWhite

#define LSK1	1
#define LSK2	2
#define LSK3	3
#define LSK4	4
#define LSK5	5
#define LSK6	6
#define RSK1	7
#define RSK2	8
#define RSK3	9
#define RSK4	10
#define RSK5	11
#define RSK6	12

#define SCREEN_INIT		1
#define SCREEN_MAIN		2
#define SCREEN_ACARS	3
#define SCREEN_ATCLIST	4
#define SCREEN_ATCSORT	5
#define SCREEN_SYSTEM	6
#define SCREEN_COM1		7
#define SCREEN_COM2		8
#define SCREEN_PREFS	9


// if adding pages, modify this number too
#define SCREEN_MAXPAGE	9

// lines in the window history
#define SCROLLBACK		200

// cursor blink frequency
#define CURSOR_INTERVAL	0.5f


// text alignments
enum UI_Alignment {
	AL_left	= 1,
	AL_ctr,
	AL_right
};

// write job list (for pluggable ui windows)
class UIWriteJob {
public:
	string text;
	float *color;
	int x, y;
	UI_Alignment align;
	bool big;
	bool inGWFMS;
};

class UiWindow {
public:
	UiWindow();

	void show();
	void hide();
	bool visible();

	void pluginStart();
	void pluginStop();

	// callbacks
	void textDrawCallback(XPLMWindowID inWindowID, void *inRefcon);
	void textToggleCallback(void *inRefcon);
	void textKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
		void *inRefcon, int losingFocus);
	int textMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

	void refresh() { prepScreen(); };

	void addMessage(float *color, const string& text, bool logToConsole = true, bool logToLogfile = false);

	void scrollUp();
	void scrollDown();

	void scrollUpFMS();
	void scrollDownFMS();


	void setPage(int page);
	int getPage() { return screen; };

	void keyEnter();
	void keyBackspace();
	void keyUpArrow();
	void keyDownArrow();
	void keyLeftArrow();
	void keyRightArrow();
	void handleKey(int key);

	// guess what they do.
	void dumpKeyboardFocus();
	void grabKeyboardFocus();

	void GWFMSPrint(bool clearScreen = true);
	void buttonClicked(int button);
	void setEditLine(const string& text) { editLine = text; };
	string getEditLine() const { return editLine; };

private:
	bool onEditPage() { return screen == SCREEN_MAIN || screen == SCREEN_ACARS; };

	void clearText() { joblist.clear(); };
	void addText(UI_Alignment align, int line, float *color, const string& text, bool big, bool inGWFMS = true);

	int mDownX, mDownY;
	bool dragging, clicking;
	int clickButton;

	bool _keyboardfocus;	// tells if window has keyboard focus or not
	float _lastKeyPress;	// last key was pressed at this time

	int beginDrag(int x, int y);
	int continueDrag(int x, int y);
	int endDrag(int x, int y);

	int checkClick(int x, int y);
	int continueClick(int x, int y);
	int endClick(int x, int y);

	int checkButton(int x, int y);

	// how big is one character in pixel
	int fontWidth, fontHeight;

	// which screen are we in
	int screen;

	// window handle
	XPLMWindowID window;

	// dimensions of the UI window
	int width, height;
	int left, top, right, bottom;

	/** returns left of the character at column */
	int getX(int column) { return left + MARGIN_LEFT + (SK_LEN + column) * fontWidth; };

	/** returns bottom of the character at column */
	int getY(int line)   { return top - MARGIN_TOP - (line+1) * fontHeight; };

	int getMiddleX(const string& str) { return (ui_columns - length(str))/2; };
	int getRightX(const string& str) { return ui_columns - length(str); };

	std::vector<UIWriteJob> joblist;	// paint job list - for draw callback

	typedef std::pair<float*, string> line;
	std::deque<line> uiText;	// the text in the UI
	std::deque<line> uiTextFMS;	// same for the GW-FMS

	// append some text to the UI
	void _appendText(float* color, const string& line);
	void _appendTextFMS(float* color, const string& line);

	size_t textLine;	// the start offset at which text display begins (for scrolling up/down)
	size_t textLineFMS;

	string editLine;	// the text line as it is being edited by the user
	bool _cursor;		// blinking cursor
	float _cursorTime;	// last time cursor changed status

	void create();
	void prepScreen();

	// prepare a new ATC list (in case we just changed the
	// sort order or switched to page COM1/COM2)
	void prepAtcList();
	size_t _atcListOffset;		// display of list starts at this entry

	std::vector<AtcPosition> _atcPositions;	// the sorted list of stations to display
	string _atcFreqs[6];	// the frequencies assigned to the select keys

	// display ATC list
	void atcList();

	// selects station #sk on the ATC list
	// returns true if operation succeeded
	bool selectStation(int sk);
	
	// Number of characters available between the select buttons
	//Jens: make wider
	const static int ui_columns = 100;
	const static int ui_columnsFMS = 25; // 25 characters in the FMS

	// don't change this
	const static int ui_rows = 12;

	bool _extradark;

	SortOrder _sortOrder;
};

#endif
