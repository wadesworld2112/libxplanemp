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
#include "refInspector.h"
#include "systemcallbacks.h"
#include "xivap.h"
#include "helpers.h"

#include "XPWidgetsEx.h"

RefInspector::RefInspector()
{
	create();
	// hook up the handler
	XPAddWidgetCallback(window, inspectorFormHandler);
}

void RefInspector::show()
{
	if(!XPIsWidgetVisible(window))
		XPShowWidget(window);
	XPBringRootWidgetToFront(window);
}

void RefInspector::hide()
{
	if(XPIsWidgetVisible(window))
		XPHideWidget(window);
}

void RefInspector::create()
{
	int x = 50;
	int y = 750;
	int x2 = x + 350;
	int y2 = y - 120;

	window = XPCreateWidget(x, y, x2, y2,
					1,			// Visible
					"X-Plane References Inspector", // desc
					1,			// root
					NULL,		// no container
					xpWidgetClass_MainWindow);
	XPSetWidgetProperty(window, xpProperty_MainWindowHasCloseBoxes, 1);

	// reference
	XPCreateWidget(x+10, y-25, x+70, y-47,
					1, "Dataref:", 0, window, xpWidgetClass_Caption);
	refTextField = XPCreateWidget(x+70, y-25, x+340, y-47,
					1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(refTextField, xpProperty_TextFieldType, xpTextEntryField);

	y -= 50;

	// value
	XPCreateWidget(x+10, y-3, x+70, y-15,
					1, "Value:", 0, window, xpWidgetClass_Caption);
	enterTextField = XPCreateWidget(x+70, y, x+200, y-22,
					1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(enterTextField, xpProperty_TextFieldType, xpTextEntryField);

	// Read
	readBtn = XPCreateWidget(x+210, y, x+270, y-23,
					1, "Read", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(readBtn, xpProperty_ButtonType, xpPushButton);

	// Write
	writeBtn = XPCreateWidget(x+280, y, x+340, y-23,
					1, "Write", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(writeBtn, xpProperty_ButtonType, xpPushButton);

	y -= 25;

	resultCaption = XPCreateWidget(x+10, y, x+340, y-22,
					1, "The result", 0, window, xpWidgetClass_Caption);

}

int	RefInspector::handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	char buffer[512];

	if(inMessage == xpMessage_CloseButtonPushed) {
		hide();
		return 1;
	}
	if(inMessage == xpMsg_PushButtonPressed) {
		xivap.grabKeyboardFocus();

		if(inParam1 == (long)readBtn) {

			// read values from form
			XPGetWidgetDescriptor(refTextField, buffer, sizeof(buffer));
			string reference = buffer;

			XPLMDataRef ref = XPLMFindDataRef(pconst(reference));
			if(ref == NULL) {
				XPSetWidgetDescriptor(resultCaption, "Invalid reference");
				return 1;
			}

			string caption = "";
			XPLMDataTypeID type = XPLMGetDataRefTypes(ref);

			if(type == 0) {
				XPSetWidgetDescriptor(resultCaption, "valid reference with unknown type");
				return 1;
			}

			if(type & xplmType_Int) {
				caption += " int ";
				int x = XPLMGetDatai(ref);
				caption += itostring(x);
			}
			if(type & xplmType_Float || type & xplmType_Double) {
				caption += " float/double ";
				double x = XPLMGetDataf(ref);
				caption += ftoa(x);
			}
			if(type & xplmType_FloatArray) caption += " float array";
			if(type & xplmType_IntArray) caption += " int array";
			if(type & xplmType_Data) caption += " data";
			XPSetWidgetDescriptor(resultCaption, pconst(caption));

			return 1;
		}


		if(inParam1 == (long)writeBtn) {

			// read values from form
			XPGetWidgetDescriptor(refTextField, buffer, sizeof(buffer));
			string reference = buffer;

			XPGetWidgetDescriptor(enterTextField, buffer, sizeof(buffer));
			string entered = buffer;

			XPLMDataRef ref = XPLMFindDataRef(pconst(reference));
			if(ref == NULL) {
				XPSetWidgetDescriptor(resultCaption, "Invalid reference");
				return 1;
			}

			XPLMDataTypeID type = XPLMGetDataRefTypes(ref);

			if(type == 0) {
				XPSetWidgetDescriptor(resultCaption, "valid reference with unknown type");
				return 1;
			}

			if(type & xplmType_Int) {
				int x = static_cast<int>(stringtoi(entered));
				XPLMSetDatai(ref, x);
				return 1;
			}
			if(type & xplmType_Float || type & xplmType_Double) {
				float x = static_cast<float>(atof(pconst(entered)));
				XPLMSetDataf(ref, x);
				return 1;
			}

			return 1;
		}

	}

	return 0;
}
