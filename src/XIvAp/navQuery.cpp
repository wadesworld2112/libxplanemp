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
#include "navQuery.h"
#include "helpers.h"

NavQuery::NavQuery()
{
	gPlaneLat		= XPLMFindDataRef("sim/flightmodel/position/latitude");
	gPlaneLon		= XPLMFindDataRef("sim/flightmodel/position/longitude");
}

string NavQuery::navref2string(const XPLMNavRef ref, const float lat, const float lon, const float maxdist) const
{
	XPLMNavType outType;
	float outLat, outLon, outHeight, outHeading;
	int outFrequency;
	char outID[256];
	char outName[256];
	char outReg[256];

	XPLMGetNavAidInfo(ref, &outType, &outLat, &outLon, &outHeight,
		&outFrequency, &outHeading, outID, outName, outReg);

	float dist = static_cast<float>(deg2dist(lat, lon, outLat, outLon));
	if(dist > maxdist)
		return "";

	string distance = itostring(static_cast<int>(dist));
	string brg = itostring(static_cast<int>(deg2crs(lat, lon, outLat, outLon)));

	string location = "(";
	if(outLat > 0) location += "N";
	else location += "S";
	location += float2coords(outLat);
	if(outLon > 0) location += " E";
	else location += " W";
	location += float2coords(outLon) + ")";

	string freq = "";
	if(outType == xplm_Nav_NDB)
		freq = itostring(outFrequency);
	else {
		freq = ftoa(outFrequency / 100.0);
		int p = pos('.', freq);
		freq = copy(freq, 0, p+4);
	}
		
	string result = "";
	switch(outType) {
		case xplm_Nav_VOR:
			result = string("VOR ")
				+ outID + " " + freq + " " + brg + "*/" + distance + "nm " + location;
			break;

		case xplm_Nav_NDB:
			result = string("NDB ")
				+ outID + " " + freq + " " + brg + "*/" + distance + "nm " + location;
			break;

		case xplm_Nav_Fix:
			result = string("FIX ")
				+ outID + " " + brg + "*/" + distance + "nm " + location;
			break;

		case xplm_Nav_DME:
			result = string("DME ")
				+ outID + " " + freq + " " + brg + "*/" + distance + "nm " + location;
			break;

		case xplm_Nav_ILS:
			result = string("ILS ")
				+ outID + " " + freq + " crs " + itostring(static_cast<int>(outHeading))
				+ " " + brg + "*/" + distance + "nm " + location;
			break;

		case xplm_Nav_Airport:
			result = string("ARP ")
				+ outID + " " + outName + " " + brg + "*/" + distance + "nm " + location;
			break;

		default:
		case xplm_Nav_Unknown:
			return "";
			break;
	}

	return result;
}

std::vector<string> NavQuery::query(const string& searchstr) const
{
	float lat = static_cast<float>(XPLMGetDatad(gPlaneLat));
	float lon = static_cast<float>(XPLMGetDatad(gPlaneLon));

	std::vector<string>	result;
	XPLMNavRef ref;

#define FINDTYPE(type) {														\
	ref = XPLMFindNavAid(NULL, pconst(strupcase(searchstr)), &lat, &lon, NULL, type);		\
	if(ref != XPLM_NAV_NOT_FOUND) {												\
		string str = navref2string(ref, lat, lon, NAVQUERY_MAX_DIST);			\
		if(str != "") result.push_back(str);									\
	}																			\
}

	FINDTYPE(xplm_Nav_VOR);
	FINDTYPE(xplm_Nav_NDB);
	FINDTYPE(xplm_Nav_Fix);
	FINDTYPE(xplm_Nav_DME);
	FINDTYPE(xplm_Nav_ILS);
	FINDTYPE(xplm_Nav_Airport);
	
	return result;
}
