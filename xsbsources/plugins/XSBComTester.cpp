/* 
 * Copyright (c) 2004, Ben Supnik and Chris Serio.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *
 */

#include "XPLMPlugin.h"
#include "XSBComDefs.h"
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include <stdio.h>

PLUGIN_API int 	XPluginStart(char * outName, char * outSig, char * outDesc);
PLUGIN_API void	XPluginStop(void);
PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginDisable(void);
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, long inMsg, void * inParam);

XPLMPluginID	gPlugin;
XPLMMenuID		gMenu;

typedef	void (* SimpleFunc)();

void	MenuHandler(void *, void *);

void DoSubscribe()
{
	if (gPlugin != XPLM_NO_PLUGIN_ID)
		XPLMSendMessageToPlugin(gPlugin, XSB_CMD_SUBSCRIBE,
			(void *) (XSB_NETWORK | XSB_AUDIO | XSB_WEATHER | XSB_TEXT));
}

void Dounsubscribe()
{
	if (gPlugin != XPLM_NO_PLUGIN_ID)
		XPLMSendMessageToPlugin(gPlugin, XSB_CMD_UNSUBSCRIBE,
			(void *) (XSB_NETWORK | XSB_AUDIO | XSB_WEATHER | XSB_TEXT));
}

void DoSetCallsign()
{
	XPLMSetDatab(XPLMFindDataRef(XSB_CON_CALLSIGN), (void *) "DAL211", 0, 7);
}

void DoPoseLogin()
{
	if (gPlugin != XPLM_NO_PLUGIN_ID)
		XPLMSendMessageToPlugin(gPlugin, XSB_CMD_USER_LOGIN, NULL);
}

void DoConnect()
{
	if (gPlugin != XPLM_NO_PLUGIN_ID)
		XPLMSendMessageToPlugin(gPlugin, XSB_CMD_CONNECT, NULL);
}

void DoDisconnect()
{
	if (gPlugin != XPLM_NO_PLUGIN_ID)
		XPLMSendMessageToPlugin(gPlugin, XSB_CMD_DISCONNECT, NULL);
}

void DoCheckConnect()
{
	int result = XPLMGetDatai(XPLMFindDataRef(XSB_CON_STATUS));
	switch(result) {
	case 0:
		XPLMDebugString("Not connected.\n");
		break;
	case 1:
		XPLMDebugString("Connecting.\n");
		break;
	case 2:
		XPLMDebugString("Connected.\n");
		break;
	}
}	

void	DoSetAltitude()
{
	XPLMSetDatab(XPLMFindDataRef(XSB_FP_CRUISE_ALTITUDE), (void *) "FL350", 0, 6);
	XPLMSetDatai(XPLMFindDataRef(XSB_FP_FUEL_HRS), 5);
}

void	DoPoseFP()
{	
	if (gPlugin != XPLM_NO_PLUGIN_ID)
		XPLMSendMessageToPlugin(gPlugin, XSB_CMD_SHOW_FP, NULL);
}		

void	DoSendFP()
{	
	if (gPlugin != XPLM_NO_PLUGIN_ID)
		XPLMSendMessageToPlugin(gPlugin, XSB_CMD_SEND_FP, NULL);
}		

void	DoShowMsg()
{
	XPLMSetDatab(XPLMFindDataRef(XSB_INPUT_USER_MSG), (void *) "Hello World!", 0, 13);
	if (gPlugin != XPLM_NO_PLUGIN_ID)
		XPLMSendMessageToPlugin(gPlugin, XSB_CMD_USER_MSG, NULL);
}	

#pragma mark -

PLUGIN_API int 	XPluginStart(char * outName, char * outSig, char * outDesc)
{
	strcpy(outName, "XSB_com_tester");
	strcpy(outSig, "vatsim.xsquawkbox.xsb_com_tester");
	strcpy(outDesc, "A plugin for testing remote control of XSquawkBox.");

	gMenu = XPLMCreateMenu("XSBTester", 
					XPLMFindPluginsMenu(), 
					XPLMAppendMenuItem(XPLMFindPluginsMenu(), "XSBTester", NULL, 1),
					MenuHandler,
					NULL);

	XPLMAppendMenuItem(gMenu, "Subscribe", DoSubscribe, 1);
	XPLMAppendMenuItem(gMenu, "Unsubscribe", Dounsubscribe, 1);
	XPLMAppendMenuSeparator(gMenu);
	XPLMAppendMenuItem(gMenu, "Set Callsign", DoSetCallsign, 1);
	XPLMAppendMenuItem(gMenu, "Pose Login", DoPoseLogin, 1);
	XPLMAppendMenuItem(gMenu, "Connect", DoConnect, 1);
	XPLMAppendMenuItem(gMenu, "Disconnect", DoDisconnect, 1);
	XPLMAppendMenuItem(gMenu, "Check Con", DoCheckConnect, 1);
	XPLMAppendMenuSeparator(gMenu);
	XPLMAppendMenuItem(gMenu, "Set alt", DoSetAltitude, 1);
	XPLMAppendMenuItem(gMenu, "Pose FP", DoPoseFP, 1);
	XPLMAppendMenuItem(gMenu, "Send FP", DoSendFP, 1);
	XPLMAppendMenuSeparator(gMenu);
	XPLMAppendMenuItem(gMenu, "Print Msg", DoShowMsg, 1);
	return 1;
}

PLUGIN_API void	XPluginStop(void)
{
}
PLUGIN_API int XPluginEnable(void)
{
	gPlugin = XPLMFindPluginBySignature("vatsim.protodev.clients.xsquawkbox");
	return 1;
}

PLUGIN_API void XPluginDisable(void)
{
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, long inMsg, void * inParam)
{
	int		frqs[10];
	int		count;
	char	buf[256];
	char	buf2[512];
	int		vers,len;

	switch(inMsg) {
	case XSB_MSG_CONNECTED:
		XPLMDebugString("Tester: XSB Connected.\n");
		break;
	case XSB_MSG_DISCONNECTED:
		XPLMDebugString("Tester: XSB disconnected.\n");
		break;
	case XSB_MSG_INCOMING_AUDIO:
		if (inParam)
			XPLMDebugString("Tester: Incoming audio start.\n");
		else
			XPLMDebugString("Tester: Incoming audio stop.\n");
		break;
	case XSB_MSG_MIC_STATUS:
		if (inParam)
			XPLMDebugString("Tester: Mic start.\n");
		else
			XPLMDebugString("Tester: Mic stop.\n");
		break;
	case XSB_MSG_METAR:
		buf[XPLMGetDatab(XPLMFindDataRef(XSB_WEATHER_METAR), buf, 0, sizeof(buf)-1)] = 0;
		XPLMDebugString("Got METAR: ");
		XPLMDebugString(buf);
		XPLMDebugString("\n");
		break;
	case XSB_MSG_TEXT:
		buf[XPLMGetDatab(XPLMFindDataRef(XSB_TEXT_FROM), buf, 0, sizeof(buf)-1)] = 0;
		XPLMDebugString("Tester FROM: ");
		XPLMDebugString(buf);
		buf[XPLMGetDatab(XPLMFindDataRef(XSB_TEXT_MESSAGE), buf, 0, sizeof(buf)-1)] = 0;
		XPLMDebugString(": ");
		XPLMDebugString(buf);
		count = XPLMGetDatavi(XPLMFindDataRef(XSB_TEXT_FREQS), frqs, 0, sizeof(frqs) / sizeof(int));
		for (int n = 0; n < count; ++n)
		{
			sprintf(buf, " from freq %d ", frqs[n]);
			XPLMDebugString(buf);
		}
		XPLMDebugString("\n");
		break;
	case XSB_MSG_USER_COMMAND:
		buf[XPLMGetDatab(XPLMFindDataRef(XSB_INPUT_STRING), buf, 0, sizeof(buf)-1)] = 0;
		XPLMDebugString("Got Input: ");
		XPLMDebugString(buf);
		XPLMDebugString("\n");
		if (!strncmp(buf, ".speak ", 7))
		{
			XPLMSpeakString(buf+6);
			XPLMSetDatab(XPLMFindDataRef(XSB_INPUT_STRING), (void *) "", 0, 1);
		}
		if (!strncmp(buf, ".lookup ", 8))
		{
			XPLMSetDatab(XPLMFindDataRef(XSB_ATC_CALLSIGN), (void *) (buf+8), 0, strlen(buf+8));
			XPLMSetDatab(XPLMFindDataRef(XSB_INPUT_STRING), (void *) "", 0, 1);
			if (gPlugin != XPLM_NO_PLUGIN_ID)
				XPLMSendMessageToPlugin(gPlugin, XSB_CMD_LOOKUP_ATC, NULL);
			len = sprintf(buf,"Freq=%d",XPLMGetDatai(XPLMFindDataRef(XSB_ATC_FREQ)));			
			XPLMSetDatab(XPLMFindDataRef(XSB_INPUT_USER_MSG), (void *) (buf), 0, len);
			if (gPlugin != XPLM_NO_PLUGIN_ID)
				XPLMSendMessageToPlugin(gPlugin, XSB_CMD_USER_MSG, NULL);			
		}
		break;
	case XSB_MSG_ABOUT_BOX:
		buf[XPLMGetDatab(XPLMFindDataRef(XSB_VERS_STRING), buf, 0, sizeof(buf)-1)] = 0;
		vers = XPLMGetDatai(XPLMFindDataRef(XSB_VERS_NUMBER));
		len = sprintf(buf2, "Com tester: v=%d, '%s'", vers, buf);

		XPLMSetDatab(XPLMFindDataRef(XSB_INPUT_USER_MSG), (void *) buf2, 0, len);
		if (gPlugin != XPLM_NO_PLUGIN_ID)
			XPLMSendMessageToPlugin(gPlugin, XSB_CMD_USER_MSG, NULL);
		break;
	}
}

void	MenuHandler(void *, void * func)
{
	SimpleFunc sfunc = (SimpleFunc)(func);
	sfunc();
}	
