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

#ifndef REF_INSPECTOR_
#define REF_INSPECTOR_

#include "common.h"

class RefInspector {
public:
	RefInspector();

	void show();
	void hide();

	int handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);

private:
	XPWidgetID window;
	XPWidgetID refTextField, enterTextField, resultCaption, readBtn, writeBtn;

	void create();
};

#endif
