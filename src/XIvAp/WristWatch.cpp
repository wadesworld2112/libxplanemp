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
#include "WristWatch.h"

WristWatch::WristWatch()
{
#ifdef WIN32
	QueryPerformanceFrequency(&_ticksPerSecond);
	QueryPerformanceCounter(&_startTick);
#else
	gettimeofday(&_startTime, 0);
#endif
}

UInt32 WristWatch::getTime()
{
#ifdef WIN32
	// what time is it?
	LARGE_INTEGER tick;
	QueryPerformanceCounter(&tick);

	// convert the tick into the number of seconds since system start
	LARGE_INTEGER time;
	time.QuadPart = tick.QuadPart - _startTick.QuadPart;
	UInt32 usec = static_cast<UInt32>((time.QuadPart * 1000000) / _ticksPerSecond.QuadPart );
	return usec;
#else
	struct timeval curTime;
	gettimeofday(&curTime, 0);
	UInt32 usec = static_cast<UInt32>((curTime.tv_sec - _startTime.tv_sec) * 1000000
		+ (curTime.tv_usec - _startTime.tv_usec));
	return usec;
#endif
}
