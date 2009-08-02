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

#ifndef AWYFINDER_H
#define AWYFINDER_H

#include "common.h"

class AwyFinder {
public:
	AwyFinder(string datfile);
	~AwyFinder();

	std::vector<XPLMNavRef> getWaypoints(string route, XPLMNavRef start);

private:
	class Leg {
	public:
		Leg(): fix1(""), airway(""), fix2(""),
			lat1(0), lon1(0), lat2(0), lon2(0) {};

		string fix1, airway, fix2;
		double lat1, lon1, lat2, lon2;
	};
	typedef std::vector<Leg> Leglist;
	typedef std::map<std::string, Leglist*> Navmap;
	Navmap awymap;

	typedef std::map<std::string, int> AwyList;
	AwyList airways;

	std::vector<XPLMNavRef> getSegment(XPLMNavRef fix1, XPLMNavRef fix2, string airway);

	XPLMNavRef getClosest(string id, float lat, float lon, float maxdist);
	bool isAirway(string& awy);
};

#endif