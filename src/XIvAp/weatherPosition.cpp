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

#include "weatherPosition.h"
#include "helpers.h"
#include "xivap.h"

#include <algorithm>


WindLayer::WindLayer():
turbulence(FSD::TURB_NONE), windshear(FSD::SHEAR_GRADUAL),
alt(0), speed(0), gusts(0), direction(0), variance(0)
{
}

VisLayer::VisLayer():
visibility(0), base(0), tops(0)
{
}

CloudLayer::CloudLayer():
type(FSD::CLOUD_NONE),
base(0), tops(0), deviation(0), coverage(0),
top(FSD::CLTOP_FLAT),
turbulence(FSD::TURB_NONE),
precip(FSD::PRECIP_NONE),
precipbase(0), preciprate(0), icingrate(0)
{
}

TempLayer::TempLayer():
alt(0), day(0), daynightvar(0), dewpoint(0)
{
}

WxStation::WxStation(const string& name_):
name(name_),
lat(0), lon(0), height(0), timestamp(0),
distance(0), qnh(0),
numWindlayers(0), numVislayers(0), numCloudlayers(0), numTemplayers(0),
navRef(XPLM_NAV_NOT_FOUND)
{
}

bool WxStation::isValid(float elapsed) const
{
	return windLayers.size() > 0 && visLayers.size() > 0
		&& tempLayers.size() > 0
		&& ((lat != 0 && lon != 0) || name == "GLOB");
}

bool WxStation::operator==(const WxStation& rhs) const
{
	if(this == &rhs) return true;
	return timestamp == rhs.timestamp && name == rhs.name;
}

void WxStation::dump() const
{
	// WX profile for EDNY (dist 33nm, valid): QNH 1022, #Wind: 4, #Vis: 1, #Cld: 3, #Tmp: 2
	string str = "WX profile for " + name + " (dist " + ftoa(distance) + "nm, ";
	str += (isValid() ? "valid" : "invalid");
	str += ", QNH " + ftoa((double)qnh);
	str += " #Wind: " + itostring(numWindlayers);
	str += " #Vis: " + itostring(numVislayers);
	str += " #Cld: " + itostring(numCloudlayers);
	str += " #Tmp: " + itostring(numTemplayers);
	xivap.addText(colLightGray, str, true, true);

	for(int i = 0; i < numWindlayers; ++i)
		xivap.addText(colLightGray, "Wind." + itostring(i) + " " + windLayers[i].dump(), true, true);
	for(int i = 0; i < numVislayers; ++i)
		xivap.addText(colLightGray, "Vis." + itostring(i) + " " + visLayers[i].dump(), true, true);
	for(int i = 0; i < numCloudlayers; ++i)
		xivap.addText(colLightGray, "Cld." + itostring(i) + " " + cloudLayers[i].dump(), true, true);
	for(int i = 0; i < numTemplayers; ++i)
		xivap.addText(colLightGray, "Temp." + itostring(i) + " " + tempLayers[i].dump(), true, true);

}

string WindLayer::dump() const
{
	string result = "Turb: ";
	switch(turbulence) {
		case FSD::TURB_NONE: result += "none"; break;
		case FSD::TURB_OCCASIONAL: result += "occas"; break;
		case FSD::TURB_LIGHT: result += "light"; break;
		case FSD::TURB_MODERATE: result += "moderate"; break;
		case FSD::TURB_SEVERE: result += "severe"; break;
	}

	result += " WShear: ";
	switch(windshear) {
		case FSD::SHEAR_GRADUAL: result += "gradual"; break;
		case FSD::SHEAR_MODERATE: result += "moderate"; break;
		case FSD::SHEAR_STEEP: result += "steep"; break;
		case FSD::SHEAR_INSTANT: result += "instant"; break;
	}
	result += " alt: " + itostring((int)alt) + " " 
		+ itostring((int)direction) + "@" + itostring((int)speed)
		+ " G" + itostring((int)gusts)
		+ " V" + itostring((int)variance);
	return result;
}

string VisLayer::dump() const
{
	string result = "Visibility " + itostring((int)visibility)
		+ " base " + itostring((int)base)
		+ " tops " + itostring((int)tops);
	return result;
}

string CloudLayer::dump() const
{
	string result;

	switch(type) {
		case FSD::CLOUD_NONE: result = "No Clds"; break;
		case FSD::CLOUD_CIRRUS: result = "Cirrus"; break;
		case FSD::CLOUD_CIRROSTRATUS: result = "CirrStrat"; break;
		case FSD::CLOUD_CIRROCUMULUS: result = "CirrCum"; break;
		case FSD::CLOUD_ALTOSTRATUS: result = "AltoStrat"; break;
		case FSD::CLOUD_ALTOCUMULUS: result = "AltoCum"; break;
		case FSD::CLOUD_STRATOCUMULUS: result = "StratCum"; break;
		case FSD::CLOUD_NIMBOSTRATUS: result = "NimbStrat"; break;
		case FSD::CLOUD_STRATUS: result = "Stratus"; break;
		case FSD::CLOUD_CUMULUS: result = "Cumulus"; break;
		case FSD::CLOUD_THUNDERSTORM: result = "Thdstrm"; break;
	}

	result += " base: " + itostring((int)base)
		+ " tops: " + itostring((int)tops)
		+ " dev: " + itostring((int)deviation)
		+ " cover: " + itostring(coverage) + "/8";
	
	result += " top: ";
	switch(top) {
		case FSD::CLTOP_FLAT: result += "flat"; break;
		case FSD::CLTOP_ROUND: result += "flat"; break;
		case FSD::CLTOP_ANVIL: result += "flat"; break;
		case FSD::CLTOP_MAX: result += "flat"; break;
	}

	result += " turb: ";
	switch(turbulence) {
		case FSD::TURB_NONE: result += "none"; break;
		case FSD::TURB_OCCASIONAL: result += "occas"; break;
		case FSD::TURB_LIGHT: result += "light"; break;
		case FSD::TURB_MODERATE: result += "moderate"; break;
		case FSD::TURB_SEVERE: result += "severe"; break;
	}

	result += " precip: ";
	switch(precip) {
		case FSD::PRECIP_NONE: result += "none"; break;
		case FSD::PRECIP_RAIN: result += "rain"; break;
		case FSD::PRECIP_SNOW: result += "snow"; break;
	}

	result += " base: " + itostring((int)precipbase)
		+ " rate: " + itostring((int)preciprate)
		+ " icing: " + itostring((int)icingrate);

	return result;
}

string TempLayer::dump() const
{
	string result = "alt: " + itostring((int)alt)
		+ " day: " + itostring((int)alt)
		+ " daynightvar: " + itostring((int)daynightvar)
		+ " dewpoint: " + itostring((int)dewpoint);
	return result;
}

void WxDB::add(const FSD::Message &m, float elapsed)
{
	string stationName = m.tokens[0];

	WxMap::iterator it = _map.find(pconst(stationName));
	if(it == _map.end()) {
		WxStation s(stationName);

		if(stationName != "GLOB") { // X-Plane doesnt know GLOB of course

			// set lat+lon if airport is known
			// first, ask X-Plane if it knows the airport
			s.navRef = XPLMFindNavAid(NULL, stationName, NULL, NULL, NULL, xplm_Nav_Airport);
			if(s.navRef != XPLM_NAV_NOT_FOUND) {
				XPLMGetNavAidInfo(s.navRef,	NULL, &s.lat, &s.lon, &s.height,
							NULL, NULL, NULL, NULL, NULL);
			} else {
				// X-Plane doesnt know it, try our own DB...
				PosMap::const_iterator it = _positions.find(pconst(stationName));
				if(it != _positions.end()) {
					s.lat = it->second.lat;
					s.lon = it->second.lon;
					s.height = it->second.alt;
				}
			}

		}

		_map[pconst(stationName)] = s;
		it = _map.find(pconst(stationName));
	}
	if(it == _map.end()) return; // paranoia

	switch(m.type) {
		case _FSD_WEAGENERAL_:
			// new weather data for this station
			it->second.windLayers.clear();
			it->second.visLayers.clear();
			it->second.tempLayers.clear();
			it->second.cloudLayers.clear();
			it->second.qnh = static_cast<float>(atof(m.tokens[1]));
			it->second.numWindlayers = static_cast<int>(stringtoi(m.tokens[2]));
			it->second.numVislayers = static_cast<int>(stringtoi(m.tokens[3]));
			it->second.numCloudlayers = static_cast<int>(stringtoi(m.tokens[4]));
			it->second.numTemplayers = static_cast<int>(stringtoi(m.tokens[5]));

#ifdef WX_DEBUG
			if(it->second.qnh < 800)
				xivap.addText(colYellow, "QNH bug in wx packet: " + m.encoded);
#endif

			break;

		case _FSD_WEAWINDLAY_: {
				if(m.tokens.size() < 2) return;

				unsigned int count = static_cast<unsigned int>(stringtoi(m.tokens[1]));
				if(m.tokens.size() < count*7 + 2) return;

				int j = 2;
				for(unsigned int i = 0; i < count; ++i) {
					WindLayer l;
					//	[ alt speed gusts direction variance turbulence windshear ]
					l.alt =				static_cast<float>(atof(m.tokens[j++]));
					l.speed =			static_cast<float>(atof(m.tokens[j++]));
					l.gusts =			static_cast<float>(atof(m.tokens[j++]));
					l.direction =		static_cast<float>(atof(m.tokens[j++]));
					l.variance =		static_cast<float>(atof(m.tokens[j++]));
					l.turbulence =		static_cast<FSD::WindTurb>(stringtoi(m.tokens[j++]));
					l.windshear =		static_cast<FSD::WindShear>(stringtoi(m.tokens[j++]));

					it->second.windLayers.push_back(l);
				}
			}
			break;

		case _FSD_WEAVISLAY_: {
				if(m.tokens.size() < 2) return;

				unsigned int count = static_cast<unsigned int>(stringtoi(m.tokens[1]));
				if(m.tokens.size() < count*3 + 2) return;

				int j = 2;
				for(unsigned int i = 0; i < count; ++i) {
					VisLayer l;
					// [ visibility base tops ]
					l.visibility =		static_cast<float>(atof(m.tokens[j++]));
					l.base =			static_cast<float>(atof(m.tokens[j++]));
					l.tops =			static_cast<float>(atof(m.tokens[j++]));

					it->second.visLayers.push_back(l);
				}
			}
			break;

		case _FSD_WEACLOUDLAY_: {
				if(m.tokens.size() < 2) return;

				unsigned int count = static_cast<unsigned int>(stringtoi(m.tokens[1]));
				if(m.tokens.size() < count*11 + 2) return;

				int j = 2;
				for(unsigned int i = 0; i < count; ++i) {
					CloudLayer l;
					// [ cloudtype cloudbase cloudtops clouddeviation cloudcoverage cloudtop
					//   cloudturbulence preciptype precipbase preciprate icingrate ]

					l.type =			static_cast<FSD::CloudType>(stringtoi(m.tokens[j++]));
					l.base =			static_cast<float>(atof(m.tokens[j++]));
					l.tops =			static_cast<float>(atof(m.tokens[j++]));
					l.deviation =		static_cast<float>(atof(m.tokens[j++]));
					l.coverage =		static_cast<int>(stringtoi(m.tokens[j++]));
					l.top =				static_cast<FSD::CloudTop>(stringtoi(m.tokens[j++]));
					l.turbulence =		static_cast<FSD::WindTurb>(stringtoi(m.tokens[j++]));
					l.precip =			static_cast<FSD::CloudPrecip>(stringtoi(m.tokens[j++]));
					l.precipbase =		static_cast<float>(atof(m.tokens[j++]));
					l.preciprate =		static_cast<float>(atof(m.tokens[j++]));
					l.icingrate =		static_cast<float>(atof(m.tokens[j++]));

					// paranoia
					if(l.tops < l.base) l.tops = l.base + 250;

					it->second.cloudLayers.push_back(l);
				}
			}
			break;

		case _FSD_WEATEMPLAY_: {
				if(m.tokens.size() < 2) return;

				unsigned int count = static_cast<unsigned int>(stringtoi(m.tokens[1]));
				if(m.tokens.size() < count*4 + 2) return;

				int j = 2;
				for(unsigned int i = 0; i < count; ++i) {
					TempLayer l;
					// [ alt day daynightvar dewpoint ]
					l.alt =				static_cast<float>(atof(m.tokens[j++]));
					l.day =				static_cast<float>(atof(m.tokens[j++]));
					l.daynightvar =		static_cast<float>(atof(m.tokens[j++]));
					l.dewpoint =		static_cast<float>(atof(m.tokens[j++]));

					it->second.tempLayers.push_back(l);
				}
			}
			break;
	}
	it->second.timestamp = elapsed;
}

WxStation WxDB::findName(string name, float elapsed)
{
	WxMap::iterator i = _map.find(std::string((const char*)name));
	if(i != _map.end())
		if(i->second.isValid(elapsed))
			return i->second;

	return WxStation();
}

std::vector<WxStation> WxDB::get(double lat, double lon, float elapsed)
{
	std::vector<WxStation> result;

	for(WxMap::iterator i = _map.begin(); i != _map.end(); ++i) {
		if(i->second.isValid(elapsed)) {
			i->second.distance = deg2dist(lat, lon, i->second.lat, i->second.lon);
			result.push_back(i->second);
		}
	}

	std::sort(result.begin(), result.end(), DistanceComparator());

	return result;
}

void WxDB::discardObsolete(float elapsed)
{
	for(WxMap::iterator i = find_if(_map.begin(), _map.end(), InvalidPredicate);
		i != _map.end(); i = find_if(_map.begin(), _map.end(), InvalidPredicate))
			_map.erase(i);
}

void WxDB::loadPositions(const string& filename)
{
	_positions.clear();
	FILE *in = fopen(pconst(filename), "ro");
	if(in == NULL) return;

	char l[512];
	int linesize = 511;
	string line;
	// read all positions into our map, altitudes are in feet
	// LOWS  47.794000 013.003000 1401
	WxPos pos;
	do {
		fgets(l, linesize, in);
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;
			pos.icao = pconst(copy(line, 0, 4));
			del(line, 0, 4);
			line = trim(line);
			int p = 0;
			// search for first blank or - char
			while(line[p] != ' ' && line[p] != '-' && p < length(line)) ++p;
			if(p >= length(line)) continue;
			pos.lat = static_cast<float>(atof(copy(line, 0, p)));
			del(line, 0, p);
			line = trim(line);
			while(line[p] != ' ' && line[p] != '-' && p < length(line)) ++p;
			if(p >= length(line)) continue;
			pos.lon = static_cast<float>(atof(copy(line, 0, p)));
			del(line, 0, p);
			line = trim(line);
			pos.alt = static_cast<float>(atof(line)) / 3.2808399f; // convert feet to meters
			_positions[pos.icao] = pos;
		}
	} while(length(line) > 0);
	fclose(in);
}

void WxStation::sortWindLayers(float altitude, size_t layers)
{
	if(windLayers.size() > layers) {

		// first, sort by vertical distance
		std::sort(windLayers.begin(), windLayers.end(), WindComparatorVdist(altitude));

		if(windLayers.size() > layers) {
			// if we have enough layers, make sure that the wind layers below and above are set
			bool have_above = false;
			bool have_below = false;
			for(size_t i = 0; i < layers; ++i) {
				if(windLayers[i].alt < altitude) have_below = true;
				else if(windLayers[i].alt > altitude) have_above = true;
				else { have_below = true; have_above = true; }; // exact altitude match
			}

			if(!have_below) {
				// look for the first wind layer below
				for(size_t i = layers; i < windLayers.size(); ++i) {
					if(windLayers[i].alt < altitude) { // ... and swap it with the 3th layer in the vector
						WindLayer l = windLayers[2];   // (which is the one most far away from us)
						windLayers[2] = windLayers[i];
						windLayers[i] = l;
						return;
					}
				}
			} else if(!have_above) {
				// look for the first wind layer above
				for(size_t i = layers; i < windLayers.size(); ++i) {
					if(windLayers[i].alt > altitude) { // ... and swap it with the 3th layer in the vector
						WindLayer l = windLayers[2];   // (which is the one most far away from us)
						windLayers[2] = windLayers[i];
						windLayers[i] = l;
						return;
					}
				}
			}
		} // if layers > windLayers.size()

		// sort the first 3 entrys by their altitude
		std::vector<WindLayer>::iterator middle = windLayers.begin();
		for(size_t i = 0; i < layers && middle != windLayers.end(); ++i) ++middle;
		std::sort(windLayers.begin(), middle, WindComparatorAlt());

	}
}

void WxStation::sortVisLayers(float altitude)
{
}

void WxStation::debugDump() const
{
#ifndef APPLE
	xivap.addText(colWhite,	string("WX station dump for ") + name, true, true);
	xivap.addText(colWhite,	string("valid: ") + (isValid() ? "yes" : "no")
		+ " lat " + ftoa(lat) + " lon " + ftoa(lon) + " alt " + ftoa(height)
		+ " timestamp " + ftoa(timestamp) + " dist " + ftoa(distance) + " qnh " + ftoa(qnh),
		false, true);

	xivap.addText(colWhite,	string("# wind layers: ") + itostring(windLayers.size()), false, true);
	for(size_t i = 0; i < windLayers.size(); ++i) {
		xivap.addText(colWhite, string("#") + itostring(i)
			+ " alt " + ftoa(windLayers[i].alt) + " speed " + ftoa(windLayers[i].speed)
			+ " gusts " + ftoa(windLayers[i].gusts) + " dir " + ftoa(windLayers[i].direction)
			+ " variance " + ftoa(windLayers[i].variance)
			+ " turb " + itostring(windLayers[i].turbulence) + " shear " + itostring(windLayers[i].windshear),
			false, true);
	}

	xivap.addText(colWhite,	string("# visibility layers: ") + itostring(visLayers.size()), false, true);
	for(size_t i = 0; i < visLayers.size(); ++i) {
		xivap.addText(colWhite, string("#") + itostring(i)
			+ " vis " + ftoa(visLayers[i].visibility) + " base " + ftoa(visLayers[i].base)
			+ " top " + ftoa(visLayers[i].tops),
			false, true);
	}

	xivap.addText(colWhite,	string("# cloud layers: ") + itostring(cloudLayers.size()),	false, true);
	for(size_t i = 0; i < cloudLayers.size(); ++i) {
		xivap.addText(colWhite, string("#") + itostring(i)
			+ " type " + itostring(cloudLayers[i].type)
			+ " coverage " + itostring(cloudLayers[i].coverage)
			+ " base " + ftoa(cloudLayers[i].base)
			+ " tops " + ftoa(cloudLayers[i].tops) + " deviat " + ftoa(cloudLayers[i].deviation)
			+ " precbase " + ftoa(cloudLayers[i].precipbase) + " precrate " + ftoa(cloudLayers[i].preciprate)
			+ " icingrate " + ftoa(cloudLayers[i].icingrate)
			+ " toptype " + itostring(cloudLayers[i].top) + " turb " + itostring(cloudLayers[i].turbulence)
			+ " precip " + itostring(cloudLayers[i].precip),
			false, true);
	}
#endif // apple
}
