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
#include "helpers.h"

#include <stdio.h>
#include <math.h>

string ftoa(double d)
{
	char buf[512];
	sprintf(buf, "%.5f", d);
	string result = buf;
	return result;
}

string stripchar(const string& str, char c)
{
	string result = str;
	int p = 0;
	while((p = pos(c, result)) > 0)
		del(result, p, 1);
	return result;
}

string stripcolons(const string& str)
{
	string result = str;
	int p = 0;
	while((p = pos(':', result)) > 0) result[p] = ' ';
	return result;
}

double deg2dist(double lat1, double lon1, double lat2, double lon2)
{
	// convert to radians
	lat1 = lat1 / 180.0 * PI;
	lat2 = lat2 / 180.0 * PI;
	lon1 = -lon1 / 180.0 * PI;
	lon2 = -lon2 / 180.0 * PI;

	// precompute...
	double sin1 = (sin((lat1-lat2)/2.0));
	double sin2 = (sin((lon1-lon2)/2.0));

	// return result in NM
	return 2 * asin(sqrt(sin1*sin1 + cos(lat1)*cos(lat2)*(sin2*sin2))) * 180 * 60 / PI;
}

double deg2crs(double lat1, double lon1, double lat2, double lon2)
{
	// convert to radians
	lat1 = lat1 / 180.0 * PI;
	lat2 = lat2 / 180.0 * PI;
	lon1 = -lon1 / 180.0 * PI;
	lon2 = -lon2 / 180.0 * PI;

	// precompute...
	double sin1 = (sin((lat1-lat2)/2.0));
	double sin2 = (sin((lon1-lon2)/2.0));

	double d = 2 * asin(sqrt(sin1*sin1 + cos(lat1)*cos(lat2)*(sin2*sin2)));

	double res;

	if(sin(lon2 - lon1) < 0)       
		res = acos((sin(lat2) - sin(lat1) * cos(d)) / (sin(d) * cos(lat1)));
	else
		res = 2 * PI - acos((sin(lat2) - sin(lat1) * cos(d)) / (sin(d) * cos(lat1)));

	// convert to degrees
	res = res * 180.0 / PI;
	while(res < 0) res += 360.0;
	while(res > 360) res -= 360.0;

	return res;
}


#define ISBLANK(c) (c == ' ' || c == '\r' || c == '\n' || c == '\t')

string trim(string line)
{
	while(length(line) > 0 && ISBLANK(line[0])) // strip left blanks
		del(line, 0, 1);
	
	while(length(line) > 0 && ISBLANK(line[length(line)-1])) // strip right blanks
		del(line, length(line)-1, 1);

	return line;
}

string strupcase(string line)
{
	for(int i = 0; i < length(line); ++i)
		line[i] = pt::upcase(line[i]);

	return line;
}

string getXplaneHomeDir()
{
	string result = "";
	const char* app = GetApplicationPath();
	static bool firsttime = true;

	if(app != NULL) {
		string path = string(app);
		int p;

#ifdef APPLE
		// Convert Mach-O path to POSIX path
		char posix[1024];
		HFS2PosixPath(pconst(path), posix, 1024);
		posix[1023] = 0; // just to make sure
		path = posix;
#endif

		p = rpos(DIR_CHAR, path);
		result = copy(path, 0, p) + DIR_CHAR;
		return result;
	}
	return string("");
}

string getXivapRessourcesDir()
{
	string path = getXplaneHomeDir();
	//const char* app = GetApplicationPath();
	static bool firsttime = true;

	path += ((pos(string(".app/Contents"), path) > 0) ? string("../../../") : string("")) 
	        + string("Resources") + DIR_CHAR + "plugins" + DIR_CHAR
			+ string(RESOURCES_DIR) + DIR_CHAR;

	if(firsttime) {
		XPLMDebugString(string("My resources directory is: ") + path + "\n");
		firsttime = false;
	}

	return path;
}

#ifdef APPLE
string getMachRessourcesDir()
{
	string result = "";
	const char* app = GetApplicationPath();
	string path(app);
	static bool firsttime = true;
	
	// faulty string on XP840:
	// Macintosh HD:Applications:X-Plane 8.40:X-Plane 840.app:Contents:MacOS:
	int p = pos(string(".app:Contents:"), path);
	if(p > 0) {
		// remove X-Plane 840.app:Contents:MacOS: from the application path
		path = copy(path, 0, p); // chop off everything after ".app:"
		//p = rpos(':', path);
		//path = copy(path, 0, p); // chop off the remaining "X-Plane 840"
	}
	
	p = rpos(':', path);
	result = copy(path, 0, p) + ":Resources:plugins:"
		+ string(RESOURCES_DIR) + ":";

	if(firsttime) {
		XPLMDebugString(string("Mach ressources directory is ") + result + "\n");
		firsttime = false;
	}

	return result;
}
#endif

string float2coords(const float& ord)
{
    int deg = static_cast<int>(ord);
    float secs = (ord - deg) * 60;
    if(secs < 0) secs *= -1;
    string result = itostring(deg) + ".";
	
	float ord2 = secs;
	deg = static_cast<int>(ord2);
    secs = (ord2 - deg) * 60;
    if(secs < 0) secs *= -1;
    result += itostring(deg) + "." + itostring(static_cast<int>(secs));
    return result;
}
