/**
 * \file Teamspeak.h
 * 
 * This file contains the interface of the TeamSpeak class.
 *
 * $Author: kenny.moens $
 * $LastChangedDate: 2004-04-30 16:28:24 +0200 (Fri, 30 Apr 2004) $
 * $LastChangedRevision: 250 $
 * 
 * (c)2005 IVAO - International Virtual Aviation Organisation
 */
#ifndef TEAMSPEAK_H
#define TEAMSPEAK_H

#include "common.h"

#ifdef WIN32
#define HAVE_TEAMSPEAK
#endif

#ifdef APPLE
#define HAVE_TEAMSPEAK
#endif

class TeamSpeak 
{
public:
	// Constructor / Destructor
	TeamSpeak();
	~TeamSpeak();

	// -- Messages --
	void Start();

	// switches the teamspeak to another channel
	void SwitchChannel(string vid, 
                     string pass, 
                     string server, 
                     string pilotcall,
                     string atccall);

	// disconnects from the current teamspeak server (returns false if failed)
	bool Disconnect();

	// quits teamspeak (returns false if failed)
	bool Quit();

private:
#ifdef WIN32
	// start without checking if running already
	void ForceStart();

	// Tries to connected to the same server (with regular DLL)
	bool TryWithLibrary(string& vid, 
                      string& pass, 
                      string& server, 
                      string& pilotcall,
                      string& atccall);

	// Connect (with regular DLL)
	bool SdkConnect(string& vid, 
                  string& pass, 
                  string& server, 
                  string& pilotcall,
                  string& atccall);

	// Tires to connect with the url
	bool TryWithURL(const string& vid, 
                  const string& pass, 
                  const string& server, 
                  const string& pilotcall,
                  const string& atccall);

#endif

	// Generate the url
    string GenerateURL(const string& vid, 
                      const string& pass, 
                      const string& server, 
                      const string& pilotcall,
                      const string& atccall);

#ifdef WIN32
	// Variables
	bool dll_loaded_;
#endif
};

#endif // TEAMSPEAK_H
