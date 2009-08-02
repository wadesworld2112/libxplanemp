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

#ifndef AIRLINESDB_H
#define AIRLINESDB_H

#include "common.h"

class AirlinesDB
{
public:
	AirlinesDB();

	class Airline {
	public:
		Airline(): icao(""), name(""), va(false) {};
		string icao, name;
		bool va;
	};
	typedef std::vector<Airline> AirlinesList;

	class Livery {
	public:
		Livery(): icao(""), code(""), description("") {};
		string icao, code, description;
	};
	typedef std::vector<Livery> LiveryList;

	AirlinesList retrieve(string searchstr);
	LiveryList getLiveries(string airline);

private:
	class ICAOComparator {
	public:
		ICAOComparator(string what): _what(what) { _what = what; };
		bool operator()(const Airline& a, const Airline& b) const {
			// the commented code below will crash on Mac and Linux
			// if(a.icao == _what) return true;
			// std::string astr = pconst(a.icao);
			// std::string bstr = pconst(b.icao);
			for(int i = 0; i < length(a.icao) && i < length(b.icao); ++i)
				if(a.icao[i] != b.icao[i]) return a.icao[i] < b.icao[i];
			return false;
		}
	private:
		string _what;
	};

	AirlinesList _database;
	LiveryList _liveries;

};

#endif
