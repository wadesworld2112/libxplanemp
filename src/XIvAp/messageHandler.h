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

#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "common.h"
#include "XSBComDefs.h"

void MessageHandler(XPLMPluginID inFrom, long inMsg, void *inParam);

// data ref getters
int Handler_getInteger(void *refcon);
long Handler_getString(void *refcon, void *outbuf, int inOffset, long inMaxLength);

// data ref setters
void Handler_setInteger(void *refcon, int value);
void Handler_setString(void *refcon, void *value, int offset, long length);

#endif
