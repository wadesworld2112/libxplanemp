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

#ifndef WRIST_WATCH_H
#define WRIST_WATCH_H

#include "common.h"

#ifndef WIN32
#include <sys/time.h>
#include <time.h>
#endif

/** Class to retrieve very precise timestamps (microsecond granularity) */
class WristWatch {
public:
	WristWatch();

	/** Returns the time in microseconds since the object construction time */
	UInt32 getTime();

private:
#ifdef WIN32
	// Windows implementation following http://www.decompile.com/cpp/faq/windows_timer_api.htm
	LARGE_INTEGER _ticksPerSecond;
	LARGE_INTEGER _startTick;
#else
	struct timeval _startTime;
#endif
};

#endif
