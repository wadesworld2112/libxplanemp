// GW-FMS_SDK.h

/***********************************************************************
This is the SDK header to allows you to connect your module to GW-FMS
To do that, you need to have a PERSONNAL CODE delivered by 
www.xpgoodway.com
***********************************************************************/

#ifndef	GW_FMS_SDK
#define GW_FMS_SDK

#include "XPLMPlugin.h"
#include "XPLMDataAccess.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define	kGWFMS_SDKVersion			1001


#define	kGWFMS_FONT_ARROW_LEFT		1
#define	kGWFMS_FONT_ARROW_RIGHT		2
#define	kGWFMS_FONT_ARROW_UP		3
#define	kGWFMS_FONT_ARROW_DOWN		4
#define	kGWFMS_FONT_FLY_OVER		5
#define	kGWFMS_FONT_BOX				6


enum{
	kGWFMS_TEXTAlign_Left = 0
	,kGWFMS_TEXTAlign_Center
	,kGWFMS_TEXTAlign_Right
};

enum{
	kGWFMS_TEXTColor_White = 0
	,kGWFMS_TEXTColor_Green
	,kGWFMS_TEXTColor_Blue
	,kGWFMS_TEXTColor_Amber
	,kGWFMS_TEXTColor_Magenta
	,kGWFMS_TEXTColor_Yellow
};

enum{
	kGWFMS_TEXTSize_Small = 0
	,kGWFMS_TEXTSize_Big
};

enum{
	kGWFMS_TEXTLine_Title = 0
	,kGWFMS_TEXTLine_11
	,kGWFMS_TEXTLine_12
	,kGWFMS_TEXTLine_21
	,kGWFMS_TEXTLine_22
	,kGWFMS_TEXTLine_31
	,kGWFMS_TEXTLine_32
	,kGWFMS_TEXTLine_41
	,kGWFMS_TEXTLine_42
	,kGWFMS_TEXTLine_51
	,kGWFMS_TEXTLine_52
	,kGWFMS_TEXTLine_61
	,kGWFMS_TEXTLine_62
};

enum{
	kGWFMS_TEXTLineBuffer_1 = 0
	,kGWFMS_TEXTLineBuffer_2
	,kGWFMS_TEXTLineBuffer_3
};

enum{
	kGWFMS_No_Error = 0
	,kGWFMS_Error_NotPresent = -1000// GW-FMS not loaded
	,kGWFMS_Error_NotOpen// GW-FMS Not open (hidden and not initialised)
	,kGWFMS_Error_NotInit// Your module it is not initialised
	,kGWFMS_Error_TextIndexNotAvailable// Text inLine or inLineBuffer are not good
	,kGWFMS_Error_TextTooLong// Text you are trying to send is too long for the line
	,kGWFMS_Error_PlugInLicenceNotValide// If your plugin id is not good or LicenceTest timeout occured (contact GoodWay Team).
	,kGWFMS_Error_PlugInLicenceTest// If your plugin id "LimitedTest". You need one plugin ID (it is free) (contact GoodWay Team).
	,kGWFMS_Error_UnknownFunction// If the plugin function is unknown.
};

/* PRIVATE DATA FOR SDK */
enum{
	kGWFMSPRIVATE_Action_InitModule = 1
	,kGWFMSPRIVATE_Action_DEBUG
	,kGWFMSPRIVATE_Action_GetVersion
	,kGWFMSPRIVATE_Action_ClearPage
	,kGWFMSPRIVATE_Action_SetText
	,kGWFMSPRIVATE_Action_EraseScratchPAD
	,kGWFMSPRIVATE_Action_SetScratchPAD
	,kGWFMSPRIVATE_Action_ShowScrollArrows
	,kGWFMSPRIVATE_Action_ShowNextPageArrows
	,kGWFMSPRIVATE_Action_RequestAttention
	,kGWFMSPRIVATE_Action_SetCallBack
	,kGWFMSPRIVATE_Action_SetModuleEnable
};

typedef struct {
	XPLMPluginID	pluginID;
	char			action_type;

	char			module_code[20];
	char			module_name[20];

	char			text[30];
	int				line;
	char			linebuffer;
	int				size;
	int				alignment;
	int				color;

	char			truefalse;
	char			*ptr;
	
	long			error;
} stData;

/* END PRIVATE DATA FOR SDK */



// Allows you to retreive the GW-FMS SDK version. This function can be called when you want.
static int GW_FMS_GetSDKVersion()
{
	return kGWFMS_SDKVersion;
}

// Allows you to declare your module to GW-FMS.  You need to do that BEFORE ALL. 
// Return 0 if Success, else, error code
static int GW_FMS_InitModule( char *inModuleCode, char *inModuleName)
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( strlen( inModuleName) > 18)
		return kGWFMS_Error_TextTooLong;
	
	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_InitModule;
	
	strcpy( theData.module_code, inModuleCode);
	strcpy( theData.module_name, inModuleName);
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}

// Allows you to retreive the GW-FMS version. This function can be called when you want.
// Return the version or -1 if error
static int GW_FMS_GetGWFMS_Version()
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return -1;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_GetVersion;
	
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}

// Allows you active a debug mode. GW-FMS will log all receive functions data into a text file PlugDebug.txt. 
// Return 0 if Success, else, error code
static int GW_FMS_Debug( int inActivate)
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_DEBUG;
	
	theData.truefalse = inActivate;
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}

// Allows you clean the entire page (before display a new one). It also remove scroll and next page arrows. 
// Return 0 if Success, else, error code
static int GW_FMS_ClearPage()
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_ClearPage;
	
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}

// Allows you to display text on FMS. Line size limit is 25. 
// Return 0 if Success, else, error code
static int GW_FMS_SetText( char *inText, int inLine, char inLineBuffer, int inSize, int inAlignment, int inColor)
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_SetText;
	
	if( strlen( inText) > 25)
		return kGWFMS_Error_TextTooLong;
	strcpy( theData.text, inText);
	theData.line = inLine;
	theData.linebuffer = inLineBuffer;
	theData.size = inSize;
	theData.alignment = inAlignment;
	theData.color = inColor;
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}

// Empty the ScratchPAD. 
// Return 0 if Success, else, error code
static int GW_FMS_EraseScratchPAD()
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_EraseScratchPAD;
	
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}

// Allows you to display text on ScratchPAD. Line size limit is XX. 
// Return 0 if Success, else, error code
static int GW_FMS_SetScratchPAD( char *inText, int inColor)
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_SetScratchPAD;
	
	if( strlen( inText) > 22)
		return kGWFMS_Error_TextTooLong;
	strcpy( theData.text, inText);
	theData.color = inColor;
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}

// Allows you to show or hide the up and down little arrow on bottom right (if you have a list bigger then the display). 
// Return 0 if Success, else, error code
static int GW_FMS_ShowScrollArrows( char inIsVisible)
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_ShowScrollArrows;
	
	theData.truefalse = inIsVisible;
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}

// Allows you to show or hide the Next page arrow on top right (if your page has multi pages). 
// Return 0 if Success, else, error code
static int GW_FMS_ShowNextPageArrow( char inIsVisible)
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_ShowNextPageArrows;
	
	theData.truefalse = inIsVisible;
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return kGWFMS_No_Error;
}

// Allows you to hilight the FMGC and MCDU MENU Light (to require user attention).
// Return 0 if Success, else, error code
static int GW_FMS_RequestAttention( bool inRequest)
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_RequestAttention;
	
	theData.truefalse = inRequest;
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}

// Allows you to receive event when your page is displayed (event activated by default). This event is received every half second (pretty).
// Return 0 if Success, else, error code
static int GW_FMS_SetReceiveEvent( char inIsOn)
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_SetCallBack;
	
	theData.truefalse = inIsOn;
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return kGWFMS_No_Error;
}

// Allows you to disable your module. FMGC return to the MCDU MENU page if you disable it and you are on your page.
// Return 0 if Success, else, error code
static int GW_FMS_SetModuleEnable( char inIsEnable)
{
	stData			theData;
	XPLMDataRef		theXPLMDataRef;

	if( (theXPLMDataRef = XPLMFindDataRef( "GoodWayTeam/GWFMSPlugin/Func")) == NULL)
		return kGWFMS_Error_NotPresent;
	
	memset( &theData, 0, sizeof( stData));
	theData.pluginID = XPLMGetMyID();
	
	theData.action_type = kGWFMSPRIVATE_Action_SetModuleEnable;
	
	theData.truefalse = inIsEnable;
	XPLMSetDatab( theXPLMDataRef, &theData, 0, sizeof( stData));
	
	return theData.error;
}


// Events
typedef struct {
	char		sChar;
	int			sInt;
	char		sString[30];
} stEventData;


#define	kGWFMS_FuncKey_DIR			'DI'
#define	kGWFMS_FuncKey_PROG			'PR'
#define	kGWFMS_FuncKey_PERF			'PE'
#define	kGWFMS_FuncKey_INIT			'IN'
#define	kGWFMS_FuncKey_DATA			'DA'
#define	kGWFMS_FuncKey_F_PLN		'FP'
#define	kGWFMS_FuncKey_RADNAV		'RN'
#define	kGWFMS_FuncKey_FUELPRED		'FU'
#define	kGWFMS_FuncKey_SEC_F_PLN	'SP'
#define	kGWFMS_FuncKey_ATCCOMM		'AC'
//#define	kGWFMS_FuncKey_MCDUMENU// Not available
#define	kGWFMS_FuncKey_AIRPORT		'AP'
#define	kGWFMS_FuncKey_NEXTPAGE		'NX'
#define	kGWFMS_FuncKey_ARROW_UP		'UP'
#define	kGWFMS_FuncKey_ARROW_DOWN	'DW'
#define	kGWFMS_FuncKey_L1			'L1'
#define	kGWFMS_FuncKey_L2			'L2'
#define	kGWFMS_FuncKey_L3			'L3'
#define	kGWFMS_FuncKey_L4			'L4'
#define	kGWFMS_FuncKey_L5			'L5'
#define	kGWFMS_FuncKey_L6			'L6'
#define	kGWFMS_FuncKey_R1			'R1'
#define	kGWFMS_FuncKey_R2			'R2'
#define	kGWFMS_FuncKey_R3			'R3'
#define	kGWFMS_FuncKey_R4			'R4'
#define	kGWFMS_FuncKey_R5			'R5'
#define	kGWFMS_FuncKey_R6			'R6'


enum{
	kGWFMS_EVENT_WhoIsModule = 'GFMS'// When your GW-FMS want to know who are the modules. reply by using InitModule
	,kGWFMS_EVENT_Activate// When your module is accepted or refused by GW FMS (sChar: 0 refused, 1 accepted - inFromWho: The GW_FMS Plugin ID)
	,kGWFMS_EVENT_Error// When GW-FMS want to send you one error, it uses this event (sInt: Error code)
	,kGWFMS_EVENT_CallBack// Called if you want it from SetReceiveEvent (no additional message)
	,kGWFMS_EVENT_Show// One time when your module is displayed (no additional message)
	,kGWFMS_EVENT_Hide// When your module is hide (because other module is called) (no additional message)
	,kGWFMS_EVENT_LSK_Pressed// When pilot press one of LSK key (Left/Right)  (sInt: See Function Key enum - sString: Text in the ScratchPAD)
	,kGWFMS_EVENT_FUNC_Pressed// When pilot press one of function key  (sInt: See Function Key enum)
};

#endif





