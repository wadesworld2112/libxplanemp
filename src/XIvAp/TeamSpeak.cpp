/**
 * \file Teamspeak.h
 * 
 * This file contains the implementation of the TeamSpeak class.
 *
 * Windows implementation: (c)2004 IVAO - International Virtual Aviation Organisation
 * Mac implementation: (c) 2006 Andrew Mcgregor
 */
#ifdef APPLE
#include <ApplicationServices/ApplicationServices.h>
#endif
#ifdef WIN32
#include "TeamSpeak.h"
#include "TsRemoteImport.h"
#endif

#include "helpers.h"

#include "xivap.h" // for debugging

/*
 * Initialises the teamspeak library
 */
TeamSpeak::TeamSpeak() 
{
#ifdef WIN32
	// DLL isn't loaded yet
	dll_loaded_ = false;

	// path of teamspeak dll
	string dll = getXivapRessourcesDir() + TS_DLL_NAME;

	// SDK log
	xivap.addText(colCyan, "Teamspeak: trying to load sdk from " + dll, true, true);

	// Load the DLL
	if(InitTsRemoteLibrary(dll) == 0)
		dll_loaded_ = true;

	// tell loaded
	if(dll_loaded_) xivap.addText(colCyan, "Teamspeak: sdk found and linked", true, true);
	else            xivap.addText(colCyan, "Teamspeak: sdk not found, trying with teamspeak:// urls", true, true);
#endif
}

/*
 * Release the teamspeak library
 */
TeamSpeak::~TeamSpeak() 
{
	// stop
	//Quit();

#ifdef WIN32
	// unlink
	if(dll_loaded_)
		CloseTsRemoteLibrary();
#endif
}

#ifdef WIN32
// -------------------------------------------------------------------------------------------------------
// structure to check if a program is running
struct ProgramRunning
{
	bool running;
};

// -------------------------------------------------------------------------------------------------------
BOOL CALLBACK EnumForTeamspeak(HWND hWnd, LPARAM lParam)
{
	// variable for data
	ProgramRunning* program = (ProgramRunning*)lParam;

	// check if we have a root window
	HWND parent = (HWND)GetWindowLong(hWnd, GWL_HWNDPARENT);
	if(parent) return TRUE;

	// check on window title (usrhost/router)
	char rawtitle[1023];
	GetWindowText(hWnd, rawtitle, sizeof(rawtitle));

	string title(rawtitle);
	if(title == "TeamSpeak 2")
	{
		program->running = true;
		return FALSE;
	}

	return TRUE;
}
#endif

/*
 * start of teamspeak
 */
void TeamSpeak::Start()
{
#ifdef WIN32
	// prepare a check
	ProgramRunning ts;
	ts.running = false;

	// enumerate all windows...
	EnumWindows(EnumForTeamspeak, (LPARAM)&ts);

	// if teamspeak isn't running, start it!
	if(!ts.running) ForceStart();
#endif
}

#ifdef WIN32
void TeamSpeak::ForceStart()
{
	// get the teamspeak data from registry
	HKEY hKey;
	RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT("teamspeak\\DefaultIcon"), 0, KEY_QUERY_VALUE, &hKey);
	
	unsigned char buffer[_MAX_PATH];
	unsigned long datatype;
	unsigned long bufferlength = sizeof(buffer);

	RegQueryValueEx(hKey, "", NULL, &datatype, buffer, &bufferlength);
	RegCloseKey(hKey);

	// start it up
	string app((const char*)buffer);
	ShellExecute(NULL, "open", app, NULL, NULL, SW_SHOWMINNOACTIVE);

	xivap.addText(colCyan, "Teamspeak: starting teamspeak installed in " + app, true, true);
}
#endif

/*
 * Switches teamspeak, but first checks if teamspeak isn't connected yet to that channel
 */
void TeamSpeak::SwitchChannel(string vid, 
                              string pass, 
                              string server, 
                              string pilotcall,
                              string atccall)
{
	xivap.addText(colCyan, "Teamspeak: switch to channel " + atccall + " on " + server, true, true);
#ifdef APPLE
    string url = GenerateURL(vid, pass, server, pilotcall, atccall);

    CFURLRef pathRef;

    pathRef = CFURLCreateWithString(NULL, CFStringCreateWithCString(NULL, pconst(url), kCFStringEncodingUTF8), NULL);
    
    if (pathRef)
    {
        LSOpenCFURLRef(pathRef, NULL);
        CFRelease(pathRef);
    }	
#endif
#ifdef WIN32
  // first try the library
  bool library(TryWithLibrary(vid, pass, server, pilotcall, atccall));

  // if it failed, try with the url
  if(!library)
	{
		TryWithURL(vid, pass, server, pilotcall, atccall);
		xivap.addText(colCyan, "Teamspeak: sdk switch failed, trying with url", true, true);
	}
#endif
}

#ifdef WIN32

/**
 * Switch using the library
 */
bool TeamSpeak::TryWithLibrary(string& vid, 
                               string& pass, 
                               string& server, 
                               string& pilotcall,
                               string& atccall)
{
	// error buffer
	char error[1000];

  // check if the dll is loaded
	if(!dll_loaded_) return false;

  // get server details
	TtsrServerInfo servinfo;
	int err = TtsrGetServerInfo(&servinfo);

	// not connected to a server...
	if(err == -1)
		return SdkConnect(vid, pass, server, pilotcall, atccall);
	// another error...
	else if(err != 0)
	{
		tsrGetLastError(error, 1000);
		xivap.addText(colCyan, "Teamspeak: failed to retreive current server info, " + string(error), true, true);
		return false;
	}

	// check if connected on the same server, if not try to connect (and switch channel directly) !
	string tsserver = servinfo.ServerHost;
	int p = pos(':', tsserver);
	tsserver = copy(tsserver, 0, p);
	if(tsserver != server)
		return SdkConnect(vid, pass, server, pilotcall, atccall);

  // check if the user is on the channel, if not try to switch
	TtsrUserInfo userinfo;
	if(tsrGetUserInfo(&userinfo) != 0) 
	{
		tsrGetLastError(error, 1000);
		xivap.addText(colCyan, "Teamspeak: failed to retreive user info, " + string(error), true, true);
		return false;
	}

	string channel = userinfo.Channel.Name;
	if(channel != atccall)
	{
		// switch channel
	    if(tsrSwitchChannelName((char*)pconst(atccall), (char*)"") == 0)
			return true;

		// failed
		tsrGetLastError(error, 1000);
		//xivap.addText(colCyan, "Teamspeak: failed to switch channel, " + string(error));
		return false;
	}

  // user is on the channel, so return true
  return true;
}

	
/**
 * Connect teamspeak using SDK
 */
bool TeamSpeak::SdkConnect(string& vid, 
                           string& pass, 
                           string& server, 
                           string& pilotcall,
                           string& atccall)
{
	// error
	char error[1000];

  // try to connect...
	if(tsrConnect((char*)pconst(GenerateURL(vid, pass, server, pilotcall, atccall))) == 0)
		return true;

	// failed
	tsrGetLastError(error, 1000);
	xivap.addText(colCyan, "Teamspeak: failed to connect to server, " + string(error), true, true);
	return false;
}

/**
 * Switch using the URL
 */
bool TeamSpeak::TryWithURL(const string& vid, 
                           const string& pass, 
                           const string& server, 
                           const string& pilotcall,
                           const string& atccall)
{
  string url = GenerateURL(vid, pass, server, pilotcall, atccall);

	// log it - contains the password!
	//string logurl(url);
	//xivap.addText(colCyan, "Teamspeak: url = " + logurl);

  ShellExecute(0/*m_hWnd*/, "open", url, "", "", SW_SHOWDEFAULT);

  return true;
}
#endif

/**
 * Generates the teamspeak URL
 */
string TeamSpeak::GenerateURL(const string& vid, 
                               const string& pass, 
                               const string& server, 
                               const string& pilotcall,
                               const string& atccall)
{
	string teamspeak = "teamspeak://" + server + "?nickname=" + pilotcall + "?loginname=" + vid
		+ "?password=" + pass + "?channel=" + atccall;
  //teamspeak.Format("teamspeak://%s?nickname=%s?loginname=%s?password=%s?channel=%s", 
  //                  server, pilotcall, vid, pass, atccall);
  return teamspeak;
}

/**
 * Disconnects from the current teamspeak server
 */
bool TeamSpeak::Disconnect()
{
#ifdef APPLE
	// sending a null URL to teamspeex makes it pop up at the right time,
	// but it doesn't disconnect.  No way to do that, sadly.
    string url = "teamspeak://";

    CFURLRef pathRef;

    pathRef = CFURLCreateWithString(NULL, CFStringCreateWithCString(NULL, pconst(url), kCFStringEncodingUTF8), NULL);
    
    if (pathRef)
    {
        LSOpenCFURLRef(pathRef, NULL);
        CFRelease(pathRef);
    }	
#endif
#ifdef WIN32
	// dll not loaded, so return false
	if(!dll_loaded_) return false;

	// disconnect now
	xivap.addText(colCyan, "Teamspeak: disconnect from the server", true, true);
	return tsrDisconnect() == 0;
#endif
}

/**
 * Quits the teamspeak 
 */
bool TeamSpeak::Quit()
{
#ifdef WIN32
	// dll not loaded, so return false
	if(!dll_loaded_) return false;
#endif

	// quit now
	//logger_->Write("Teamspeak: shutdown teamspeak");
	//return tsrQuit() == 0;
	return true;
}
