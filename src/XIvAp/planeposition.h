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

#ifndef PLANE_POSITION_H
#define PLANE_POSITION_H

#include "common.h"
//#include "XPLMMultiplayer.h"
#include "XPMPMultiplayer.h"

class PlanePosition {
public:
	PlanePosition();
	~PlanePosition();
	PlanePosition(const PlanePosition& rhs) { operator=(rhs); };
	PlanePosition& operator=(const PlanePosition& rhs);
	PlanePosition& operator+=(const PlanePosition& rhs);
	const PlanePosition operator+(const PlanePosition &other) const;
	PlanePosition& operator-=(const PlanePosition& rhs);
	const PlanePosition operator-(const PlanePosition &other) const;
	PlanePosition& operator/=(const double rhs);
	const PlanePosition operator/(const double other) const;
	PlanePosition& operator*=(const double rhs);
	PlanePosition* operator*();
	const PlanePosition operator*(const double other) const;

	XPMPPlanePosition_t pos;
	bool onground;				///< guess what that is...
	UInt32 timestamp;			///< when did we get that packet (our clock)
	UInt32 latency;				///< what was the latency to peer at that time
	UInt32 p_timestamp;			///< peers timestamp on packet (peers clock)
};

#endif
