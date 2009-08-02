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

#ifndef AIRCRAFTDB_H
#define AIRCRAFTDB_H

#include "common.h"

class AircraftDB
{
public:
	AircraftDB();

	class Aircraft {
	public:
		Aircraft(): icao(""), manufacturer(""), modelname(""), type(""), category('X') {};
		string icao, manufacturer, modelname, type;
		char category;
	};
	typedef std::vector<Aircraft> AircraftList;

	AircraftList retrieve(string searchstr);
	Aircraft get(string icao);

private:
	void load();

	AircraftList _database;

};

#endif
