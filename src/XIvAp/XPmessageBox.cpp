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
#include "XPmessageBox.h"
#include "systemcallbacks.h"
#include "xivap.h"

XPMessageBox::XPMessageBox()
{
	create();
}

void XPMessageBox::show(string message)
{
	string capt1, capt2, capt3;
	capt1 = message;
	capt2 = "";
	capt3 = "";

	if(length(capt1) > 37) {
		int p = 37;
		while(capt1[p] != ' ' && p > 15) --p;
		if(p <= 15) p = 37; // no space found in first part of line -> wrap at last column

		capt2 = capt1;
		capt1 = copy(capt1, 0, p);
		del(capt2, 0, p+1);
	}

	if(length(capt2) > 37) {
		int p = 37;
		while(capt2[p] != ' ' && p > 15) --p;
		if(p <= 15) p = 37; // no space found in first part of line -> wrap at last column

		capt3 = capt2;
		capt2 = copy(capt2, 0, p);
		del(capt3, 0, p+1);
	}


	XPSetWidgetDescriptor(text1, pconst(capt1));
	XPSetWidgetDescriptor(text2, pconst(capt2));
	XPSetWidgetDescriptor(text3, pconst(capt3));

	if(!XPIsWidgetVisible(window))
		XPShowWidget(window);
	XPBringRootWidgetToFront(window);
}

void XPMessageBox::hide()
{
	if(XPIsWidgetVisible(window))
		XPHideWidget(window);
}

void XPMessageBox::create()
{
	int x = 300;
	int y = 500;
	int x2 = x + 280;
	int y2 = y - 120;

	window = XPCreateWidget(x, y, x2, y2,
					1,			// Visible
					"Message",	// desc
					1,			// root
					NULL,		// no container
					xpWidgetClass_MainWindow);
	XPSetWidgetProperty(window, xpProperty_MainWindowHasCloseBoxes, 0);

	XPCreateWidget(x+10, y-30, x+270, y-85, 1, "", 0, window, xpWidgetClass_SubWindow);

	y -= 35;
	// bla bla
	text1 = XPCreateWidget(x+20, y, x+260, y-12, 1, "", 0, window, xpWidgetClass_Caption);
	y -= 14;
	text2 = XPCreateWidget(x+20, y, x+260, y-12, 1, "", 0, window, xpWidgetClass_Caption);
	y -= 14;
	text3 = XPCreateWidget(x+20, y, x+260, y-12, 1, "", 0, window, xpWidgetClass_Caption);

	y -= 30;
	// OK
	okButton = XPCreateWidget(x+30, y, x+250, y-24,
					1, "Understood", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(okButton, xpProperty_ButtonType, xpPushButton);

	// hook up the handler
	XPAddWidgetCallback(window, msgBoxHandler);
}

int	XPMessageBox::handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMessage_CloseButtonPushed) {
		hide();
		return 1;
	}
	if(inMessage == xpMsg_PushButtonPressed) {
		xivap.grabKeyboardFocus();

		// no
		if(inParam1 == (long)okButton) {
			hide();
			return 1;
		}
	}
	return 0;
}
