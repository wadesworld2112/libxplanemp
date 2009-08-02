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

#ifndef FMC_FORM_
#define FMC_FORM_

#include "common.h"
#include "aircraftDB.h"
#include "airlinesDB.h"
#include "awyFinder.h"
#include "configFile.h"

class FMCForm {
public:
	FMCForm();
	~FMCForm();

	void show();
	void hide();

	int handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);

	// updates callsign and realname from xivap
	void setUserCredentials();

	void FMCExport();

	void setFields(string departure, string destination, string route, int altitude);

private:
	XPWidgetID window;
	XPWidgetID routeTextField;
	XPWidgetID depCaption, destCaption;

	XPWidgetID sidTextField, sidSaveBtn, sidLoadBtn, sidClearBtn, sidClearAllBtn;
	XPWidgetID sidWptListBox, sidPopup;

	XPWidgetID starTextField, starSaveBtn, starLoadBtn, starClearBtn, starClearAllBtn;
	XPWidgetID starWptListBox, starPopup;

	XPWidgetID inputTextField, inputListBox;
	XPWidgetID VORselect, NDBselect, DMEselect, FIXselect, LLselect;

	XPWidgetID addSidButton, addStarButton, exportButton, closeButton;

	void create();
	void reset() {};

	AwyFinder *routefinder;

	void updateNavList();	// updates search list with all matching entries in nav database
	void updateLatLon(string input);	// updates search list with a lat/lon entry
	bool callUpdateNavList;

	class FMCEntry {
	public:
		FMCEntry(XPLMNavRef r): ref(r), lat(0), lon(0) {};
		XPLMNavRef ref;
		float lat, lon;
	};
	typedef std::vector<FMCEntry> NavList;

	NavList navEntries;
	NavList sidPoints, starPoints;

	XPLMNavRef depAirport, dstAirport;

	string navDescription(XPLMNavRef navRef);
	string latlonStr(float lat, float lon);

	void saveProcedure(ConfigFile& db, string name, NavList &list);
	void saveSid();
	void saveStar();

	void loadProcedure(ConfigFile& db, string name, NavList &list, XPWidgetID listBox);
	void loadSid();
	void loadStar();

	void updateSIDList();
	void updateSTARList();

	std::vector<string> SIDnames, STARnames;
	int altitude_;
};

#endif
