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

#ifndef ATCPOSITION_H
#define ATCPOSITION_H

#include "common.h"
#include "fsdmessage.h"


enum SortOrder {
	SortDistance	=	0,
	SortFrequency	=	1,
	SortCallsign	=	2
};

class AtcPosition {
public:
	AtcPosition();
	AtcPosition(const FSD::Message& m);

	bool isValid(float time = 0);

	pt::string callsign;
	pt::string vid;
	pt::string realname;
	FSD::AtcRating rating;
	pt::string frequency;
	FSD::Facility facility;
	int range;
	double lat, lon, distance;
	bool haveAtis;
	float timestamp;
};

class AtcList {
public:
	void add(const FSD::Message& m, float elapsed);
	void remove(const FSD::Message& m);
	std::vector<AtcPosition> get(SortOrder order, double lat, double lon, float elapsed);
	void clear() {_map.clear(); };

	// returns the nearest ATC position with this frequency
	AtcPosition findFreq(string frequency, double lat, double lon);
	AtcPosition findName(string name);
	void setAtis(string name, bool value);

private:
	// the "UNICOM" atc station, it is always displayed in the ATC list
	AtcPosition unicom();

	typedef std::map<std::string, AtcPosition> AtcMap;
	AtcMap _map;

	class CallsignComparator {
	public:
		int operator()(const AtcPosition a, const AtcPosition b) const {
			return std::string((const char*)a.callsign) < std::string((const char*)b.callsign);
		}
	};
	class DistanceComparator {
	public:
		int operator()(const AtcPosition a, const AtcPosition b) const {
			return a.distance < b.distance;
		}
	};
	class FrequencyComparator {
	public:
		int operator()(const AtcPosition a, const AtcPosition b) const {
			return std::string((const char*)a.frequency) < std::string((const char*)b.frequency);
		}
	};

};

#endif
