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

#include "common.h"
#include "PlatformUtils.h"

/** Convert double to string */
string ftoa(double d);

#define PI 3.14159265359

string stripchar(const string& str, char c);
string stripcolons(const string& str);
double deg2dist(double lat1, double lon1, double lat2, double lon2);
double deg2crs(double lat1, double lon1, double lat2, double lon2);

// remove leading and trailing spaces
string trim(string line);

// convert string to UPPERCASE
string strupcase(string line);

string getXivapRessourcesDir();
string getXplaneHomeDir();
string getPosixPath();

#ifdef APPLE
// nasty mach-o SDK workaround
string getMachRessourcesDir();
#else
#define getMachRessourcesDir getXivapRessourcesDir
#endif


string float2coords(const float& ord);
