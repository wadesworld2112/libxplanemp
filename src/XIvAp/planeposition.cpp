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
#include "planeposition.h"
#include <math.h>

PlanePosition::PlanePosition()
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat = 0;
	pos.lon = 0;
	pos.elevation = 0;
	pos.heading = 0;
	pos.pitch = 0;
	pos.roll = 0;

	onground = false;

	timestamp = 0;
	latency = 0;
	p_timestamp = 0;
}

PlanePosition::~PlanePosition()
{
}

PlanePosition& PlanePosition::operator=(const PlanePosition& rhs)
{
	if(this == &rhs)
		return *this;

	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat = rhs.pos.lat;
	pos.lon = rhs.pos.lon;
	pos.elevation = rhs.pos.elevation;
	pos.heading = rhs.pos.heading;
	pos.pitch = rhs.pos.pitch;
	pos.roll = rhs.pos.roll;

	onground = rhs.onground;

	timestamp = rhs.timestamp;
	latency = rhs.latency;
	p_timestamp = rhs.p_timestamp;

	return *this;
}

PlanePosition& PlanePosition::operator+=(const PlanePosition& rhs)
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat += rhs.pos.lat;
	pos.lon += rhs.pos.lon;
	pos.elevation += rhs.pos.elevation;
	pos.heading += rhs.pos.heading;
	pos.pitch += rhs.pos.pitch;
	pos.roll += rhs.pos.roll;

	timestamp += rhs.timestamp;
	latency += rhs.latency;
	p_timestamp += rhs.p_timestamp;

	return *this;
}

const PlanePosition PlanePosition::operator+(const PlanePosition &other) const {
    return PlanePosition(*this) += other;
}

PlanePosition& PlanePosition::operator-=(const PlanePosition& rhs)
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat -= rhs.pos.lat;
	pos.lon -= rhs.pos.lon;
	pos.elevation -= rhs.pos.elevation;
	pos.heading -= rhs.pos.heading;
	// if we turned past north, this is wrong... fix elsewhere!
	pos.pitch -= rhs.pos.pitch;
	pos.roll -= rhs.pos.roll;

	timestamp -= rhs.timestamp;
	latency -= rhs.latency;
	p_timestamp -= rhs.p_timestamp;

	return *this;
}

const PlanePosition PlanePosition::operator-(const PlanePosition &other) const {
    return PlanePosition(*this) -= other;
}

PlanePosition& PlanePosition::operator/=(const double rhs)
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat /= rhs;
	pos.lon /= rhs;
	pos.elevation /= rhs;
	pos.heading /= rhs;
	pos.pitch /= rhs;
	pos.roll /= rhs;

	timestamp /= rhs;
	latency /= rhs;
	p_timestamp /= rhs;

	return *this;
}

const PlanePosition PlanePosition::operator/(const double other) const {
    return PlanePosition(*this) /= other;
}

PlanePosition& PlanePosition::operator*=(const double rhs)
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat *= rhs;
	pos.lon *= rhs;
	pos.elevation *= rhs;
	pos.heading *= rhs;
	pos.pitch *= rhs;
	pos.roll *= rhs;

	timestamp *= rhs;
	latency *= rhs;
	p_timestamp *= rhs;

	return *this;
}

PlanePosition* PlanePosition::operator*()
{
	return this;
}

const PlanePosition PlanePosition::operator*(const double other) const {
    return PlanePosition(*this) *= other;
}


