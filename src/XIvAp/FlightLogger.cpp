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
#include "FlightLogger.h"

#include <math.h>

using namespace FL;

#define STATUS_MAX_TIMEDIFF		2*60
#define STATUS_MIN_DIST			1.0
#define STATUS_MIN_ALTDIFF		500.0
#define STATUS_MIN_PITCHDIFF	5.0
#define STATUS_MIN_BANKDIFF		5.0
#define STATUS_MIN_HDGDIFF		5.0
#define STATUS_MIN_IASDIFF		10.0
#define STATUS_MIN_GNDDIFF		10.0
#define STATUS_MIN_VSPDDIFF		200.0
#define STATUS_MIN_GLOADDIFF	0.1
#define STATUS_MIN_FUELDIFF		500.0
#define STATUS_MIN_MASSDIFF		500.0
#define STATUS_MIN_POWERDIFF	5.0


double status_deg2dist(double lat1, double lon1, double lat2, double lon2)
{
#define PI 3.14159265358

	// convert to radians
	lat1 = lat1 / 180.0 * PI;
	lat2 = lat2 / 180.0 * PI;
	lon1 = -lon1 / 180.0 * PI;
	lon2 = -lon2 / 180.0 * PI;

	// convert to nm
	double sin1 = (sin((lat1-lat2)/2.0));
	double sin2 = (sin((lon1-lon2)/2.0));

	return 2 * asin(sqrt(sin1*sin1 + cos(lat1)*cos(lat2)*(sin2*sin2))) * 180 * 60 / PI;
}


Status::Status()
{
	reset();
}

void Status::reset()
{
	lat = lon = altft = 0;
	pitch = bank = hdg = 0;
	iaskt = gndkt = 0;
	vspeedfpm = gload = 0;
	totfuel_kg = totmass_kg = 0;
	enginepwr = 0;
	onGround = true;
	timestamp = 0;
}

// computes the delta. if there is any, timestamp is set
// to now(), else its set to 0
Status Status::operator-(const Status& rhs) const
{
	Status result;
	result.reset();
	bool delta = false;

#define MAXDIFF(value, limit) {				\
	if(fabs(rhs.value - value) > limit) {	\
		delta = true;						\
		result.value = value;				\
	}										\
}

	// add position either if distance is too far,
	// or timestamps are too far apart
	if(status_deg2dist(lat, lon, rhs.lat, rhs.lon) >= STATUS_MIN_DIST
		|| msecs(now()) - msecs(rhs.timestamp) > STATUS_MAX_TIMEDIFF*1000)
	{
		delta = true;
		result.lat = lat;
		result.lon = lon;
	}

	MAXDIFF(altft, STATUS_MIN_ALTDIFF);
	MAXDIFF(pitch, STATUS_MIN_PITCHDIFF);
	MAXDIFF(bank, STATUS_MIN_BANKDIFF);
	MAXDIFF(hdg, STATUS_MIN_HDGDIFF);
	MAXDIFF(iaskt, STATUS_MIN_IASDIFF);
	MAXDIFF(gndkt, STATUS_MIN_GNDDIFF);
	MAXDIFF(vspeedfpm, STATUS_MIN_VSPDDIFF);
	MAXDIFF(gload, STATUS_MIN_GLOADDIFF);
	MAXDIFF(totfuel_kg, STATUS_MIN_FUELDIFF);
	MAXDIFF(totmass_kg, STATUS_MIN_MASSDIFF);
	MAXDIFF(enginepwr, STATUS_MIN_POWERDIFF);

	if(rhs.onGround != onGround) {
		delta = true;
		result.onGround = onGround;
	}
	
	if(delta)
		result.timestamp = now();
	else
		result.timestamp = 0;

	return result;
}

// -----------------------------------------------------------------------------

string logger_ftoa(double d)
{
	char buf[512];
	sprintf(buf, "%.5f", d);
	string result = buf;
	return result;
}


FlightLogger::FlightLogger(const string& logfile)
{
	//LoadAirports(airportsdat);
	reset(logfile);
	file = NULL;
}

FlightLogger::~FlightLogger()
{
	if(file) delete file;
}

void FlightLogger::reset(const string& filename)
{
	_lastLogged.reset();
	if(file) delete file;
	file = new logfile(filename, false);
}

void FlightLogger::log(const Status& s)
{
	Status l;
	l.reset();

	if(_lastLogged.timestamp == 0) {
		l = s;
		l.timestamp = now();
	} else {
		l = s - _lastLogged;
	}

	if(l.timestamp == 0)
		return;

	WriteLog(l);
	_lastLogged = s;
}

// write log entry to file for all fields that are not zero
void FlightLogger::WriteLog(const Status& s) 
{
	string line = "";

#define ADDLINE(desc, val) {									\
	if(line != "") line += string(", ");						\
	if(s.val != 0) line += string(desc) + string("=") + string(logger_ftoa(s.val));	\
}

	ADDLINE("LAT", lat);
	ADDLINE("LON", lon);
	ADDLINE("ALT", altft);
	ADDLINE("PITCH", pitch);
	ADDLINE("BANK", bank);
	ADDLINE("HDG", hdg);
	ADDLINE("IAS", iaskt);
	ADDLINE("GNDSPD", gndkt);
	ADDLINE("VSPD", vspeedfpm);
	ADDLINE("GLOAD", gload);
	ADDLINE("FUELKG", totfuel_kg);
	ADDLINE("MASSKG", totmass_kg);
	ADDLINE("POWER", enginepwr);

	if(line != "") line += ", ";
	line = string("GND=") + (s.onGround ? "1" : "0");

	file->putf("[%t] %s\r\n", s.timestamp, line);
}

/*
bool FlightLogger::LoadAirports(const string& filename)
{
	_airports.clear();
	FILE *in = fopen(pconst(filename), "ro");
	if(in == NULL) return false;

	char l[1024];
	int linesize = 1023;
	string line;

	// read all positions into our map
	ArpPos arp;
	do {
		fgets(l, linesize, in);
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;

			if(line[0] == '#' || line[0] == ';')
				continue; // ignore comments

			// LOWW:Schwechat:Vienna:AT:LOVV:48.1102:16.5697
			int p = pos(':', line);
			if(p != 4) continue;
			arp.icao = pconst(copy(line, 0, p));
			del(line, 0, p+1);
			
			p = pos(':', line);
			arp.name = pconst(copy(line, 0, p));
			del(line, 0, p+1);

			p = pos(':', line);
			arp.city = pconst(copy(line, 0, p));
			del(line, 0, p+1);
			
			p = pos(':', line);
			if(p > 3) continue;
			arp.country = pconst(copy(line, 0, p));
			del(line, 0, p+1);

			p = pos(':', line);
			if(p > 5) continue;
			arp.fir = pconst(copy(line, 0, p));
			del(line, 0, p+1);

			p = pos(':', line);
			arp.lat = static_cast<float>(atof(pconst(copy(line, 0, p))));
			del(line, 0, p+1);

			arp.lon = static_cast<float>(atof(pconst(line)));

			_airports[arp.icao] = arp;
		}
	} while(length(line) > 0);
	fclose(in);
	return true;
}
*/
