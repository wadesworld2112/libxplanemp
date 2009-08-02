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
#include "atcPosition.h"
#include "helpers.h"

#include <algorithm>

AtcPosition::AtcPosition():
callsign(""),
vid(""),
realname(""),
rating(FSD::ATC_UNKNOWN),
frequency(""),
facility(FSD::FAC_OBS),
range(0),
lat(0),
lon(0),
distance(0),
haveAtis(false),
timestamp(0)
{
}

AtcPosition::AtcPosition(const FSD::Message &m):
callsign(""),
vid(""),
realname(""),
rating(FSD::ATC_UNKNOWN),
frequency(""),
facility(FSD::FAC_OBS),
range(0),
lat(0),
lon(0),
distance(0),
haveAtis(false),
timestamp(0)
{
	switch(m.type) {
		case _FSD_ADDATC_:
			callsign = m.source;
			realname = m.tokens[0];
			vid = m.tokens[1];
			rating = static_cast<FSD::AtcRating>(stringtoi(m.tokens[3]));
			break;

		case _FSD_ATCPOS_:
			callsign = m.source;
			string s = m.dest; // 29320
			frequency = "1" + copy(s, 0, 2) + "."; // 129.
			del(s, 0, 2);
			frequency += s; // 129.320
			facility = static_cast<FSD::Facility>(stringtoi(m.tokens[0]));
			range = static_cast<int>(stringtoi(m.tokens[1]));
			rating = static_cast<FSD::AtcRating>(stringtoi(m.tokens[2]));
			lat = static_cast<double>(atof(pconst(m.tokens[3])));
			lon = static_cast<double>(atof(pconst(m.tokens[4])));
			break;
	}
}

bool AtcPosition::isValid(float time)
{
	if(facility == FSD::FAC_OBS) return false;	// ignore observers
	if(length(frequency) != 7) return false;	// ignore empty stations (or fucked frequencies)
	if(pos("_T_", string(callsign)) != -1) return false;	// ignore AAAA_T_BBB
	if(pos("_X_", string(callsign)) != -1) return false;	// ignore AAAA_X_BBB
	if(time != 0 && timestamp + ATCPOS_MAX_AGE < time) {
		facility = FSD::FAC_OBS;	// mark as unusable for next run
		return false;
	}

	return true;
}

void AtcList::add(const FSD::Message& m, float elapsed)
{
	AtcPosition p(m);
	p.timestamp = elapsed;

	AtcMap::iterator i = _map.find(std::string((const char*)p.callsign));
	if(i == _map.end()) {
		// this is a new guy
		_map[std::string((const char*)p.callsign)] = p;	

	} else {

		// we already have him, update information
		if(p.frequency != "") {
			i->second.lat = p.lat;
			i->second.lon = p.lon;
			i->second.range = p.range;
			i->second.frequency = p.frequency;
			i->second.rating = p.rating;
			i->second.facility = p.facility;
			i->second.timestamp = elapsed;
		}
	}
}

void AtcList::remove(const FSD::Message& m)
{
	std::string callsign = pconst(m.source);
	AtcMap::iterator i = _map.find(callsign);
	if(i != _map.end())
		_map.erase(i);
}

std::vector<AtcPosition> AtcList::get(SortOrder order, double lat, double lon, float elapsed)
{
	std::vector<AtcPosition> result;
	//result.push_back(unicom());

	for(AtcMap::iterator i = _map.begin(); i != _map.end(); ++i) {
		if(i->second.isValid(elapsed)) {

			/* debugging stuff...
			string str = "Adding: " + i->second.callsign + " " + i->second.frequency;
			switch(i->second.facility) {
				case FSD::FAC_OBS: str = "OBS " + str; break;
				case FSD::FAC_DEL: str = "DEL " + str; break;
				case FSD::FAC_GND: str = "GND " + str; break;
				case FSD::FAC_TWR: str = "TWR " + str; break;
				case FSD::FAC_APP: str = "APP " + str; break;
				case FSD::FAC_CTR: str = "CTR " + str; break;
				case FSD::FAC_FSS: str = "FSS " + str; break;
			}
			xivap.addText(colWhite, str);
			*/

			i->second.distance = deg2dist(lat, lon, i->second.lat, i->second.lon);
			result.push_back(i->second);
		}
	}

	// now sort the vector according to the sort criterium

	switch(order) {
		case SortDistance:
			std::sort(result.begin(), result.end(), DistanceComparator());
			break;
		case SortCallsign:
			std::sort(result.begin(), result.end(), CallsignComparator());
			break;
		default:
		case SortFrequency:
			std::sort(result.begin(), result.end(), FrequencyComparator());
			break;
	}

	return result;
}

AtcPosition AtcList::unicom()
{
	AtcPosition result;
	result.callsign = UNICOM_NAME;
	result.frequency = UNICOM_FREQ;
	result.facility = FSD::FAC_FSS;
	result.distance = UNICOM_DIST;
	return result;
}

AtcPosition AtcList::findFreq(string frequency, double lat, double lon)
{
	AtcPosition result;
	result.distance = 9999;

	AtcMap::iterator i = _map.begin();
	while(i != _map.end()) {
		if(i->second.isValid() && i->second.frequency == frequency) {
			i->second.distance = deg2dist(lat, lon, i->second.lat, i->second.lon);
			if(i->second.distance < result.distance) result = i->second;
		}
		++i;
	}
	return result;
}

AtcPosition AtcList::findName(string name)
{
	AtcMap::iterator i = _map.find(std::string((const char*)name));
	if(i != _map.end()) {
		if(i->second.isValid())
			return i->second;
	}
	return AtcPosition();
}

void AtcList::setAtis(string name, bool value)
{
	AtcMap::iterator i = _map.find(std::string((const char*)name));
	if(i != _map.end())
		i->second.haveAtis = value;
}
