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
/**
 * AwyFinder
 *
 * Airway route finder class for X-Plane
 * Copyright (c) 2005 by Martin Domig <martin@domig.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *
 * Usage:
 * 
 * Instantiate one object of this class, pass the path to X-Planes awy.dat
 * (in Resources/Earth Nav Data/awy.dat) to the constructor. The constructor will
 * then load the contents of the dat file into memory, this usually takes some time
 * and consumes a considerable amount of RAM, so you might want to instantiate this
 * class only once.
 *
 * getWaypoints(string route, XPLMNavRef start) returns a STL vector with
 * XPLMNavRef's which you can use to program the FMC.
 *
 * route
 * should be a string of the form "FIX AIRWAY FIX AIRWAY FIX"
 * If you want to go direct from one fix to another one, insert DCT instead of
 * the airway. However, that will fail if the fixes are too far apart.
 *
 * Note:
 * This code is using the string class from the C++ Portable Types Library (ptypes)
 * by Hovik Melikyan (http://www.melikyan.com/ptypes/). Check it out if you are
 * planning on writing plugins for more than one platforms.
 * You may need to adapt a few things here and there to get it running without
 * ptypes, but that shouldn't be too hard. Just replace the calls to pos() copy()
 * del() with the apropriate std::string equivalents.
 *
 */

#include "awyFinder.h"
#include "helpers.h"

#include <math.h>
#include <set>

AwyFinder::AwyFinder(string datfile)
{
	FILE *in = fopen(pconst(datfile), "ro");
	if(in == NULL) return;

	char l[4096];
	int linesize = 4095;
	string line = "";
	int i = 0;
	// read all lines into our list
	do {
		fgets(l, linesize, in);
		i++;
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;
			line = trim(line);
			// ignore first 2 lines
			if(length(line) > 0 && i > 2) {
				Leg entry;
				// AKUDA  35.966667 -008.950000 PESAS  37.036692 -007.383333 2 195 600 UN858
				// fix1
				int p = pos(' ', line); if(p < 0) continue;
				entry.fix1 = copy(line, 0, p); del(line, 0, p); line = trim(line);
				p = pos(' ', line); if(p < 0) continue;
				string dummy = copy(line, 0, p); del(line, 0, p); line = trim(line);
				entry.lat1 = atof(pconst(dummy));
				p = pos(' ', line); if(p < 0) continue;
				dummy = copy(line, 0, p); del(line, 0, p); line = trim(line);
				entry.lon1 = atof(pconst(dummy));

				// PESAS  37.036692 -007.383333 2 195 600 UN858
				// fix2
				p = pos(' ', line); if(p < 0) continue;
				entry.fix2 = copy(line, 0, p); del(line, 0, p); line = trim(line);
				p = pos(' ', line); if(p < 0) continue;
				dummy = copy(line, 0, p); del(line, 0, p); line = trim(line);
				entry.lat2 = atof(pconst(dummy));
				p = pos(' ', line); if(p < 0) continue;
				dummy = copy(line, 0, p); del(line, 0, p); line = trim(line);
				entry.lon2 = atof(pconst(dummy));

				bool twoway = true; // find something that indicates one-way airways some day

				// airway
				p = rpos(' ', line); if(p < 0) continue;
				del(line, 0, p);
				string awy = trim(line);

				do {
					p = pos('-', awy);
					if(p > 0) {
						entry.airway = copy(awy, 0, p);
						del(awy, 0, p+1);
					} else {
						entry.airway = awy;
						awy = "";
					}

					Navmap::iterator it = awymap.find(pconst(entry.fix1));
					if(it == awymap.end()) {
						Leglist *list = new Leglist(5);
						list->push_back(entry);
						awymap[pconst(entry.fix1)] = list;
					} else {
						Leglist *list = it->second;
						list->push_back(entry);
					}

					// remember this airway
					airways[pconst(entry.airway)] = 1;

					if(twoway) {
						it = awymap.find(pconst(entry.fix2));
						Leg entry2;
						entry2.fix1 = entry.fix2;
						entry2.lat1 = entry.lat2;
						entry2.lon1 = entry.lon2;
						entry2.fix2 = entry.fix1;
						entry2.lat2 = entry.lat1;
						entry2.lon2 = entry.lon1;
						entry2.airway = entry.airway;

						if(it == awymap.end()) {
							Leglist *list = new Leglist(5);
							list->push_back(entry2);
							awymap[pconst(entry.fix2)] = list;
						} else {
							Leglist *list = it->second;
							list->push_back(entry2);
						}
					}

				} while(length(awy) > 0);
			}
		}
	} while(length(line) > 0);
	fclose(in);
}

bool AwyFinder::isAirway(string& awy)
{
	return airways.find(pconst(awy)) != airways.end();
}

AwyFinder::~AwyFinder()
{
	Navmap::iterator it;
	for(it = awymap.begin(); it != awymap.end(); ++it)
		delete it->second;
}

XPLMNavRef AwyFinder::getClosest(string id, float curlat, float curlon, float maxdist)
{
	XPLMNavRef start;
	XPLMNavType navType = xplm_Nav_VOR; // start the search with VORs
	do {
		start = XPLMFindNavAid(NULL, pconst(id), (float*)&curlat, (float*)&curlon,
					NULL, navType);

		// make sure that the next fix is within a reasonable distance
		if(start != XPLM_NAV_NOT_FOUND) {
			float navLat, navLon;
			XPLMGetNavAidInfo(start, NULL, &navLat, &navLon, NULL, NULL, NULL, NULL, NULL, NULL);
			double a = curlat - navLat;
			double b = curlon - navLon;
			// nah - to far away. Invalidate it
			if(sqrt(a*a + b*b) > maxdist)
				start = XPLM_NAV_NOT_FOUND;
		}

		// if no fix found, try the next type.
		// In this sequence: VOR, NDB, DME FIX
		if(start == XPLM_NAV_NOT_FOUND) {
			if(navType == xplm_Nav_VOR) navType = xplm_Nav_NDB;
			else if(navType == xplm_Nav_NDB) navType = xplm_Nav_DME;
			else if(navType == xplm_Nav_DME) navType = xplm_Nav_Fix;
			else if(navType == xplm_Nav_Fix) navType = xplm_Nav_Unknown;
		}
	} while(start == XPLM_NAV_NOT_FOUND && navType != xplm_Nav_Unknown);

	return start;
}

std::vector<XPLMNavRef> AwyFinder::getWaypoints(string route, XPLMNavRef start)
{
	std::vector<XPLMNavRef> result;
	int state = 1; // 1 = need fix1, 2 = need airway, 3 = need fix2
	route = trim(strupcase(route));
	float curlat, curlon;
	float *latp = NULL;
	float *lonp = NULL;
	bool firstfix = true;
	XPLMNavRef fix1 = XPLM_NAV_NOT_FOUND, fix2 = XPLM_NAV_NOT_FOUND;
	string airway = "";

	if(start != XPLM_NAV_NOT_FOUND) {
		XPLMGetNavAidInfo(start, NULL, &curlat, &curlon, NULL, NULL, NULL, NULL, NULL, NULL);
		latp = &curlat;
		lonp = &curlon;
	}

	while(length(route) > 0) {
		// get next token from the route into wpt
		string wpt = route;
		int p = pos(' ', route);
		if(p > 0) {
			wpt = copy(route, 0, p);
			del(route, 0, p+1);
			route = trim(route);
		} else {
			route = "";
		}

		if(state == 1 || state == 3) {
			// wpt HAS to be a fix/navaid. Everything else is discarded

			// search for this fix
			start = getClosest(wpt, curlat, curlon, 15);

			if(start != XPLM_NAV_NOT_FOUND) {
				// found something :)
				XPLMGetNavAidInfo(start, NULL, &curlat, &curlon, NULL, NULL, NULL, NULL, NULL, NULL);
				latp = &curlat;
				lonp = &curlon;

				if(state == 1) {
					fix1 = start;
					if(firstfix || length(route) == 0) {
						firstfix = false;
						result.push_back(fix1);
					}
					state = 2; // next should be an airway
					continue;
				} else {
					// state == 3
					fix2 = start;

					std::vector<XPLMNavRef> segment = getSegment(fix1, fix2, airway);
					for(unsigned int i = 0; i < segment.size(); ++i)
						result.push_back(segment[i]);
					result.push_back(fix2);

					fix1 = fix2;
					fix2 = XPLM_NAV_NOT_FOUND;
					airway = "";
					state = 2; // need airway
					continue;
				}

			} else {
				// found nothing
				state = 1; // we need a fix next
				fix1 = fix2 = XPLM_NAV_NOT_FOUND;
				airway = "";
				continue;
			}
		} else if(state == 2) {

			if(isAirway(wpt)) {
				// lets hope that it is a valid airway
				airway = wpt;
				state = 3;
				continue;
			}

			// this entry is supposed to be an airway, but many people enter only
			// waypoints. Lets see if this is a waypoint maybe.
			XPLMNavRef point = XPLMFindNavAid(NULL, pconst(wpt), (float*)&curlat, (float*)&curlon,
					NULL, xplm_Nav_VOR | xplm_Nav_NDB | xplm_Nav_DME | xplm_Nav_Fix);

			if(point != XPLM_NAV_NOT_FOUND) { // FIXME - check if the IDs really match 100%
				// it could be a waypoint... djeez
				point = getClosest(wpt, curlat, curlon, 15);
			}

			if(point != XPLM_NAV_NOT_FOUND) {
				// have a valid waypoint. as he wishes...
				result.push_back(point);
				fix1 = point;
				airway = "";
				state = 2;
			}

			continue;
		}
	}

	return result;
}

std::vector<XPLMNavRef> AwyFinder::getSegment(XPLMNavRef fix1, XPLMNavRef fix2, string airway)
{
	std::vector<XPLMNavRef> result;
	std::set<XPLMNavRef> dirtylist;
	char buf[64];

	bool triedOtherWay = false;

	float lat1, lon1;
	XPLMGetNavAidInfo(fix1, NULL, &lat1, &lon1, NULL, NULL, NULL, buf, NULL, NULL);
	string id1 = buf;
	float lat2, lon2;
	XPLMGetNavAidInfo(fix2, NULL, &lat2, &lon2, NULL, NULL, NULL, buf, NULL, NULL);
	string id2 = buf;

	XPLMNavRef fix = XPLM_NAV_NOT_FOUND;

	Navmap::const_iterator it = awymap.find(pconst(id1));

	while(it != awymap.end()) {
		Leglist* list = it->second;
		bool done = false;
		for(Leglist::iterator lit = list->begin(); lit != list->end() && !done; ++lit) {
			double a = lit->lat1 - lat1;
			double b = lit->lon1 - lon1;
			if(sqrt(a*a + b*b) > 5)
				continue;

			if(lit->airway == airway) {
				done = true;
				if(lit->fix2 == id2)
					return result;	// finished

				// search for this fix
				fix = getClosest(lit->fix2, static_cast<float>(lit->lat2 + 0.5),
					static_cast<float>(lit->lon2 + 0.5), 5);

				if(fix == XPLM_NAV_NOT_FOUND) {
					// this really shouldnt happen
					result.clear();
					return result;
				}

				// see if this fix is already in our segment (to avoid loops)
				if(fix == fix1) done = false; // and don't allow to go back to the start
				else if(dirtylist.find(fix) != dirtylist.end()) done = false;

				if(done) {
					result.push_back(fix);
					dirtylist.insert(fix);
					id1 = pconst(lit->fix2);
					lat1 = (float)lit->lat2;
					lon1 = (float)lit->lon2;
					it = awymap.find(pconst(id1));
				}
			}
		}

		if(!done) {
			// we are at the end of the airway without having reached the second waypoint
			result.clear();
			if(triedOtherWay) {
				// time to give up :(
				return result;
			} else {
				triedOtherWay = true;
				// lets try again, this time the other way (if there is one)
				XPLMGetNavAidInfo(fix1, NULL, &lat1, &lon1, NULL, NULL, NULL, buf, NULL, NULL);
				string id1 = buf;
				it = awymap.find(pconst(id1));
			}
		}
	}

	// if we are here, we didn't find anything useful
	result.clear();
	return result;
}
