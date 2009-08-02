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

#ifndef IVAOSECURITY_H
#define IVAOSECURITY_H

#include "common.h"

#include <list>
#include <algorithm>

class IVAOSecurity 
{
private:
  // typedef for easy coding
  typedef std::list<int> IntList;

public:
	IVAOSecurity();

	// returns true if access granted
	bool CheckAccess(int vid);

	// transforms the username
	string TransformUsername(const string& username);

	// time limit checking
	bool CheckDate();

private:
	IntList allowed_members_;
};

#endif // IVAOSECURITY_H
