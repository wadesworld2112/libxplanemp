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
#include "airlinesDB.h"
#include "helpers.h"

#include <algorithm>

AirlinesDB::AirlinesDB()
{
	// load the database file
	char l[512];
	int linesize = 511;
	FILE *in;

	string filename = getXivapRessourcesDir() + "airlines.dat";
	XPLMDebugString(string("My AirlinesDB is: ") + filename + "\n");
    in = fopen(pconst(filename), "ro");

	_database.clear();
    if(in == NULL) return;

	string line = "";
	do {
		fgets(l, linesize, in);
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;
			Airline entry;
			if(length(line) > 0) {
				// ABB:Air Belgium:Air Belgium:V
				int p = pos(':', line); if(p != 3) continue;
				entry.icao = strupcase(copy(line, 0, p));
				//while(length(entry.icao) < 3) entry.icao = " " + entry.icao;
				del(line, 0, p+1);

				// Air Belgium:Air Belgium:V
				p = pos(':', line); if(p < 0) continue;
				entry.name = copy(line, 0, p);
				del(line, 0, p+1);

				// Air Belgium:V
				p = pos(':', line); if(p < 0) continue;
				del(line, 0, p+1);

				// V
				entry.va = (line[0] == 'V');
				
				_database.push_back(entry);
			}
		}
	} while(length(line) > 0);
	fclose(in);



	// read liveries.txt
	filename = getXivapRessourcesDir() + "liveries.txt";
	XPLMDebugString(string("My liveries is: ") + filename + "\n");
    in = fopen(pconst(filename), "ro");

	_liveries.clear();
    if(in == NULL) return;

	line = "";
	do {
		fgets(l, linesize, in);
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;
			Livery entry;
			if(length(line) > 0) {
				if(line[0] == ';') continue;

				// SWA NEWMX New Mexico One
				int p = pos(' ', line); if(p != 3) continue;
				entry.icao = strupcase(copy(line, 0, p));
				del(line, 0, p+1);

				// NEWMX New Mexico One
				p = pos(' ', line); if(p != 5) continue;
				entry.code = strupcase(copy(line, 0, p));
				del(line, 0, p+1);

				// New Mexico One
				for(p = 0; p < length(line); ++p)
					if(line[p] == '"') line[p] = '\'';
				entry.description = trim(line);
				
				_liveries.push_back(entry);
			}
		}
	} while(!feof(in));
	fclose(in);

}

AirlinesDB::AirlinesList AirlinesDB::retrieve(string searchstr)
{
	// no search string - return the whole database
	if(length(searchstr) < 1)
		return _database;

	AirlinesList result;

	searchstr = strupcase(searchstr);

	for(AirlinesList::const_iterator it = _database.begin(); it != _database.end(); ++it) {
		// match search string against icao and name
		int p = pos(searchstr, it->icao);
		if(p >= 0) { result.push_back(*it); continue; }

		p = pos(searchstr, strupcase(it->name));
		if(p >= 0) result.push_back(*it);
	}
	
	// sort result
	std::sort(result.begin(), result.end(), ICAOComparator(searchstr));
	return result;
}

AirlinesDB::LiveryList AirlinesDB::getLiveries(string airline)
{
	LiveryList result;
	if(length(airline) != 3)
		return result;

	airline = strupcase(airline);

	for(LiveryList::const_iterator it = _liveries.begin(); it != _liveries.end(); ++it) {
		if(airline == it->icao)
			result.push_back(*it);
	}
	
	return result;
}
