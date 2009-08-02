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
#include "configFile.h"
#include "helpers.h"

ConfigFile::ConfigFile()
{
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::load(string filename)
{
	char l[512];
	int linesize = 511;
	FILE *in;

    in = fopen(pconst(filename), "ro");
	contents.clear();
    if(in == NULL) return;

	string line = "";
	// read all lines into our list
	do {
		fgets(l, linesize, in);
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;
			if(length(line) > 0) contents.push_back(line);
		}
	} while(length(line) > 0);
	fclose(in);
}

void ConfigFile::save(string filename)
{
	FILE *in;
    in = fopen(pconst(filename), "w");	// will destroy existing contents
	if(in == NULL) return;

	std::list<string>::const_iterator it = contents.begin();
	while(it != contents.end()) {
		// save all lines to file
		fputs(pconst(*it), in);
		++it;
	}
	fclose(in);
}

std::list<string>::iterator ConfigFile::findSection(string section)
{
	std::list<string>::iterator it;

	// convert "   sEcTioN " to "SECTION"
	section = strupcase(trim(section));

	// search for section
	for(it = contents.begin(); it != contents.end(); ++it) {
		string l = trim(*it);
		if(length(l) < 3) continue; // ignore short lines
		if(l[0] == '#' || l[0] == ';') continue; // ignore comments

		if(l[0] == '[' && l[length(l) - 1] == ']' ) { // new section: [<something>]
			l = copy(l, 1, length(l) - 2);		// "[abc]" -> "abc"
			l = strupcase(l);					// "abc" -> "ABC"
			if(l == section) return it;	// bingo
		}
	}

	return it;
}

std::list<string>::iterator ConfigFile::findKey(string section, string key)
{
	key = strupcase(trim(key));

	// search for key in section
	std::list<string>::iterator it = findSection(section);
	++it;
	for(; it != contents.end(); ++it) {
		string l = trim(*it);
		if(length(l) < 3) continue; // ignore short lines
		if(l[0] == '#' || l[0] == ';') continue; // ignore comments

		if(l[0] == '[' && l[length(l) - 1] == ']' ) // new section: [<something>]
			return contents.end();

		// entries in a section are of form "something = something else"
		int p = pos('=', l);	// split the line at '='
		if(p < 1) continue;

		string linekey = copy(l, 0, p);	// key is first half
		linekey = trim(strupcase(linekey));
		if(key == linekey) return it;
	}
	return it;
}

string ConfigFile::readConfig(string section, string key)
{
	std::list<string>::iterator it = findKey(section, key);
	if(it == contents.end()) return "";

	string line = *it;
	int p = pos('=', line);	// split the line at '='
	del(line, 0, p+1);				// remove all from the beginning including the =
	line = trim(line);
	return line;
}

void ConfigFile::setConfig(string section, string key, string value)
{
	section = strupcase(trim(section));
	key = strupcase(trim(key));
	value = trim(value);

	string line = key + "=" + value + "\r\n";

	std::list<string>::iterator it = findKey(section, key);
	if(it != contents.end()) *it = line;
	else {
		it = findSection(section);
		if(it != contents.end()) {
			++it;
			contents.insert(it, line);
		} else {
			string secstr = "[" + section + "]\r\n";
			contents.insert(contents.end(), secstr);
			contents.insert(contents.end(), line);
		}
	}
}

void ConfigFile::clearSection(string section)
{
	section = strupcase(trim(section));
	std::list<string>::iterator it = findSection(section);
	std::list<string>::iterator it2 = it;
	++it2;
	bool found = false;
	while(it2 != contents.end() && !found) {
		string l = trim(*it2);
		if(l[0] == '[') found = true;
		else ++it2;
	}
	contents.erase(it, it2);
}

std::vector<string> ConfigFile::listSections()
{
	std::vector<string> result;
	std::list<string>::iterator it = contents.begin();
	while(it != contents.end()) {
		string l = trim(*it);
		if(l[0] == '[') {
			del(l, 0, 1);
			del(l, length(l)-1, 1);
			result.push_back(trim(l));
		}
		++it;
	}

	return result;
}
