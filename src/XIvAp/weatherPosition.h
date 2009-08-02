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

#ifndef WXPOSITION_H
#define WXPOSITION_H

#include "common.h"
#include "fsdmessage.h"

//#define WX_DEBUG

class WindLayer {
public:
	WindLayer();
	string dump() const;

	FSD::WindTurb turbulence;
	FSD::WindShear windshear;
	float alt, speed, gusts, direction, variance;
};

class VisLayer {
public:
	VisLayer();
	string dump() const;

	float visibility, base, tops;
};

class CloudLayer {
public:
	CloudLayer();
	string dump() const;

	FSD::CloudType type;
	float base, tops, deviation;
	int coverage;
	FSD::CloudTop top;
	FSD::WindTurb turbulence;
	FSD::CloudPrecip precip;
	float precipbase, preciprate, icingrate;
};

class TempLayer {
public:
	TempLayer();
	string dump() const;

	float alt, day, daynightvar, dewpoint;
};


/**
 * The weather station
 */
class WxStation {
public:
	WxStation(const string& name = "");

	bool isValid(float elapsed = 0) const;

	string name;
	float lat, lon, height, timestamp;
	double distance;
	float qnh;
	int numWindlayers, numVislayers, numCloudlayers, numTemplayers;

	XPLMNavRef navRef;

	std::vector<WindLayer> windLayers;
	std::vector<VisLayer> visLayers;
	std::vector<CloudLayer> cloudLayers;
	std::vector<TempLayer> tempLayers;

	void sortWindLayers(float altitude, size_t layers);
	void sortVisLayers(float altitude);

	void dump() const;
	void debugDump() const;

	bool operator==(const WxStation& rhs) const;

private:
	// This class is used to sort the windlayers so that the layer closest
	// to a given altitude are first in the vector
	class WindComparatorVdist {
	  public:
		WindComparatorVdist(float alt): _alt(alt) {};
		float _alt;
		bool operator()(const WindLayer& a, const WindLayer& b) {
			float da = a.alt - _alt; if(da < 0) da = -da;
			float db = b.alt - _alt; if(db < 0) db = -db;
			return da < db;
		};
	};
	class WindComparatorAlt {
	  public:
		bool operator()(const WindLayer& a, const WindLayer& b) { return a.alt < b.alt; };
	};
};

class WxDB {
public:
	// add this information to the DB
	void add(const FSD::Message& m, float elapsed);

	// returns a sorted list of weather stations, sorted by distance.
	// closest station first
	std::vector<WxStation> get(double lat, double lon, float elapsed);

	// clear the db
	void clear() {_map.clear(); };

	// remove obsolete entries
	void discardObsolete(float elapsed);

	// returns the WX station with the given name
	WxStation findName(string name, float elapsed = 0);

	void loadPositions(const string& filename);

private:
	typedef std::pair<const std::string, WxStation> WxPair;
	typedef std::map<std::string, WxStation> WxMap;
	WxMap _map;

	class WxPos {
	public:
		WxPos(): icao(""), lat(0), lon(0), alt(0) {};
		std::string icao;
		float lat, lon, alt;
	};
	typedef std::map<std::string, WxPos> PosMap;
	PosMap _positions;


	static bool InvalidPredicate(WxPair& p) { return !p.second.isValid(); }

	class DistanceComparator {
	public:
		int operator()(const WxStation a, const WxStation b) const {
			return a.distance < b.distance;
		}
	};
};

#endif