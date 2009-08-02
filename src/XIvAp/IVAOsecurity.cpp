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
#include "IVAOsecurity.h"

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define MAGIC1 761
#define MAGIC2 26578

#define add(id) allowed_members_.push_back(id * MAGIC1 - MAGIC2)

/**
 * Constructor
 */
IVAOSecurity::IVAOSecurity() 
{
#ifdef INTERNAL_BETA

add(100002); // Erwin Lion (IVAO Network Admin)
add(100380); // Gustavo Hernandez (IVAO Vice President)
add(100796); // Mike Kara (Member of Honour + Real Life ATC)
add(103563); // Alain Debrouwer (IVAO Director)
add(104333); // Daniele Pistollato (IVAO Softdev Assistant Director)
add(105390); // Martin Kowalski (Assisting in MTL Development)
add(108442); // Kenny Moens (IVAO Softdev Member)
add(109084); // Marcus Schneider (IVAO Assistant Network Admin)
add(111173); // Jim Ramos (US Division Member)
add(116260); // Alfonso Ballesteros (IVAO Membership Director + Very Active)
add(117852); // Mytho Leal (IVAO Softdev Member)
add(124365); // Filip Jonckers (IVAO Softdev Director)
add(127930); // Jo Boerjan (BE-DIR + Very Active Pilot + ATC)
add(128367); // Gergely Kósa (IVAO Softdev Member)
add(132404); // Jonas Grönwald (Public Relations Assistant Director)
add(133382); // Juergen Kopelent (MTL Developer)
add(136707); // Luca Bertagnolio (Personal Friend + Good Knowledge of Linux + PS1 User)
add(139302); // Allen Sindel (IVAO Assistant Director)
add(142664); // Alexander Gruenauer (Status Indicator Developer + Assisting in MTL)
add(148661); // Mikkel Gylling Hangaard (IVAO-AOAD, X-Plane user)
add(155904); // Splendor Bouman (CA-ADIR, X-Plane specialist, Mac owner)
add(156618); // Martin Domig (IVAO Softdev Member)
add(159751); // Scott Wilson (X-Plane version 7.63 on Mac OSX)
add(161738); // Kristof Provost (IVAO Softdev Member)
add(170606); // Peter Meininger (Provides CSL models for X-IvAp)
add(186320); // heimo wiederhofer (Quite active X-Plane user)

#endif
}

/**
 * Access Checking
 */
bool IVAOSecurity::CheckAccess(int vid) 
{
#ifdef INTERNAL_BETA
	IntList::iterator iter(std::find(allowed_members_.begin(), allowed_members_.end(), vid * MAGIC1 - MAGIC2));
	return iter != allowed_members_.end();
#else
  return true;
#endif
}

/**
 * Username transformation
 */
string IVAOSecurity::TransformUsername(const string& username) 
{
	return username + "° (" + SOFTWARE_VERSION + ")";
}

/**
 * Data checking
 */
bool IVAOSecurity::CheckDate() 
{
#ifdef BETA
	struct tm deadline_tm = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	deadline_tm.tm_mday = EXPIRE_DAY;
	deadline_tm.tm_mon = EXPIRE_MONTH - 1;
	deadline_tm.tm_year = EXPIRE_YEAR - 1900;

	time_t now = time(NULL);
	time_t deadline = mktime(&deadline_tm);

	if(now > deadline) return false;
#endif
	return true;
}
