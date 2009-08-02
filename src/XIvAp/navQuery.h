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

#ifndef NAV_QUERY_H
#define NAV_QUERY_H

#include "common.h"

// only show infomration of nav. database entries less than this many nm away
#define NAVQUERY_MAX_DIST 100

class NavQuery {
public:
	NavQuery();

	std::vector<string> query(const string& searchstr) const;

private:
	string navref2string(const XPLMNavRef ref, const float lat, const float lon, const float maxdist) const;

	XPLMDataRef gPlaneLat, gPlaneLon;
};

#endif