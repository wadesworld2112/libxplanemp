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
#include "disconnectForm.h"
#include "systemcallbacks.h"
#include "xivap.h"

DisconnectForm::DisconnectForm()
{
	create();
}

void DisconnectForm::show()
{
	if(!XPIsWidgetVisible(window))
		XPShowWidget(window);
	XPBringRootWidgetToFront(window);
}

void DisconnectForm::hide()
{
	if(XPIsWidgetVisible(window))
		XPHideWidget(window);
}

void DisconnectForm::create()
{
	int x = 150;
	int y = 500;
	int x2 = x + 240;
	int y2 = y - 80;

	window = XPCreateWidget(x, y, x2, y2,
					1,			// Visible
					"Disconnect", // desc
					1,			// root
					NULL,		// no container
					xpWidgetClass_MainWindow);
	XPSetWidgetProperty(window, xpProperty_MainWindowHasCloseBoxes, 0);

	// bla bla
	XPCreateWidget(x+10, y-28, x+200, y-40,
					1, "Are you sure that you want to disconnect?",
					0, window, xpWidgetClass_Caption);

	// Yes
	yesButton = XPCreateWidget(x+30, y-48, x+30+80, y-71,
					1, "Yes", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(yesButton, xpProperty_ButtonType, xpPushButton);

	// No
	noButton = XPCreateWidget(x+130, y-48, x+130+80, y-71,
					1, "No", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(noButton, xpProperty_ButtonType, xpPushButton);

	// hook up the handler
	XPAddWidgetCallback(window, disconnectFormHandler);
}

int	DisconnectForm::handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMessage_CloseButtonPushed) {
		hide();
		return 1;
	}
	if(inMessage == xpMsg_PushButtonPressed) {
		xivap.grabKeyboardFocus();

		// no
		if(inParam1 == (long)noButton) {
			hide();
			return 1;
		}

		// yes
		if(inParam1 == (long)yesButton) {
			hide();
			xivap.disconnect();
			if(xivap.uiWindow.getPage() == SCREEN_SYSTEM)
				xivap.uiWindow.setPage(SCREEN_MAIN);

			return 1;
		}
	}
	return 0;
}
