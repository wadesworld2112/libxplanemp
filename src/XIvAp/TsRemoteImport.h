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

/*
This is ported version of TsRemoteImport.pas (for Delphi)
creating a C "SDK" for TeamSpeak. Currently it only supports
windows, but this could easy be changed. It has not been
FULLY tested, but it should all work, if you have a bug report
please email me.  Do not email me if you are not 100% of what
you are doing, in other words I do not want to deal with someone
who simply cannot code, only genuine bug reports please.

Ported by:
Neil Popplewell
aka -TAT-Neo & Neo4E656F
email: neo@clantat.com
msn: x4e656f@hotmail.com
*/
#pragma once

char TS_DLL_NAME[] = "TSRemote.dll";
HMODULE tsrDLL;
const int cCodecCelp51 = 0,
		  cCodecCelp63 = 1,
		  cCodecGSM148 = 2,
		  cCodecGSM164 = 3,
		  cCodecWindowsCELP52 =4,
		  //Codec masks
		  cmCelp51 = 1 << cCodecCelp51,
		  cmCelp63 = 1 << cCodecCelp63,
		  cmGSM148 = 1 << cCodecGSM148,
	  	cmGSM164 = 1 << cCodecGSM164,
		  cmWindowsCELP52 = 1 << cCodecWindowsCELP52,
	   	  //PlayerChannel Privileges
		  pcpAdmin        = 1 << 0,
		  pcpOperator     = 1 << 1,
		  pcpAutoOperator = 1 << 2,
		  pcpVoiced       = 1 << 3,
		  pcpAutoVoice    = 1 << 4,
		  //PlayerPrivileges
		  ppSuperServerAdmin = 1 << 0,
		  ppServerAdmin      = 1 << 1,
		  ppCanRegister      = 1 << 2,
		  ppRegistered       = 1 << 3,
		  ppUnregistered     = 1 << 4,
		  //player flags
		  pfChannelCommander = 1 << 0,
		  pfWantVoice        = 1 << 1,
		  pfNoWhisper        = 1 << 2,
		  pfAway             = 1 << 3,
		  pfInputMuted       = 1 << 4,
		  pfOutputMuted      = 1 << 5,
		  pfRecording        = 1 << 6,
		  //channel flags
		  cfRegistered   = 1 << 0,
		  cfUnregistered = 1 << 1,
		  cfModerated    = 1 << 2,
		  cfPassword     = 1 << 3,
		  cfHierarchical = 1 << 4,
		  cfDefault      = 1 << 5,
		  //ServerType Flags
		  stClan       = 1 << 0,
		  stPublic     = 1 << 1,
		  stFreeware   = 1 << 2,
		  stCommercial = 1 << 3,

		  grRevoke = 0,
		  grGrant  = 1;

#pragma pack(push,1) //Byte alignment
struct TtsrVersion
{
	int Major;
	int Minor;
	int Release;
	int Build;
};
struct TtsrPlayerInfo
{
	int PlayerID;
	int ChannelID;
	char NickName[30];
	int PlayerChannelPrivileges;
	int PlayerPrivileges;
	int PlayerFlags;
};
struct TtsrServerInfo
{
	char ServerName[30];
	char WelcomeMessage[256];
	int ServerVMajor;
	int ServerVMinor;
	int ServerVRelease;
	int ServerVBuild;
	char ServerPlatform[30];
	char ServerIp[30];
	char ServerHost[100];
	int ServerType;
	int ServerMaxUsers;
	int SupportedCodecs;
	int ChannelCount;
	int PlayerCount;
};

struct TtsrChannelInfo
{
	int ChannelID;
	int ChannelParentID;
	int PlayerCountInChannel;
	int ChannelFlags;
	int Codec;
	char Name[30];
};
struct TtsrUserInfo
{
	TtsrPlayerInfo Player;
	TtsrChannelInfo Channel;
	TtsrChannelInfo ParentChannel;
};
#pragma pack(pop)
//##############################################################################
//#
//#  Function int InitTsRemoteLibrary(BOOL TryLocal);
//#
//#  Description:
//#    Loads and binds the TSRemote library
//#
//#  Input:
//#    TryLocal: if true, it will try to load the library from the same dir
//#              as the program location. If that fails it will fall back to
//#              the default locations.
//#
//#  Output:
//#    Result: (0=ok, -1= library already initialized,
//#             -2=error loading library
//#             -3=error during binding functions
//#
//##############################################################################
int InitTsRemoteLibrary(BOOL TryLocal);

//##############################################################################
//#
//#  Function int CloseTsRemoteLibrary();
//#
//#  Description:
//#    Frees the hawkvoice library loaded by InitTsRemoteLibrary
//#
//#  Input:
//#    None
//#
//#  Output:
//#    Result: (0=ok, -1= library not initialized, -2=error during FreeLibrary
//#
//##############################################################################
int CloseTsRemoteLibrary();

//##############################################################################
//#
//#  Procedure int tsrGetLastError(char *pchBuffer, int BufferLength);
//#
//#  Description:
//#    Get the full error message that was send with the last error
//#
//#  Input:
//#    pchBuffer: A pointer to a nulterminated string where the error message
//#               will be copied to.
//#    BufferLength: The size of pchBuffer
//#
//#  Output:
//#    None
//#
//##############################################################################
typedef void (__stdcall  *LPtsrGetLastError)(char *pchBuffer, int BufferLength);
LPtsrGetLastError tsrGetLastError = NULL;

//##############################################################################
//#
//#  Function int tsrConnect(char* URL);
//#
//#  Description:
//#    Connect the ts to a new server as described in the URL. Will disconnect
//#    if the client is currently connected. The Url is the same format as the
//#    normal starup link ("teamspeak://voice.teamspeak.org:9500" etc)
//#
//#  Input:
//#    URL: A pointer to a null terminated string containing the url for the
//#         server.
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall *LPtsrConnect)(char* URL);
LPtsrConnect tsrConnect = NULL;

//##############################################################################
//#
//#  Function int tsrDisconnect();
//#
//#  Description:
//#    Disconnects the client from the current server.
//#
//#  Input:
//#    None
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrDisconnect)(void);
LPtsrDisconnect tsrDisconnect = NULL;

//##############################################################################
//#
//#  Function int tsrQuit();
//#
//#  Description:
//#    Disconnect, Close and terminate the client.
//#
//#  Input:
//#    None
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrQuit)(void);
LPtsrQuit tsrQuit = NULL;

//##############################################################################
//#
//#  Function int tsrSwitchChannelName(char *ChannelName, char *ChannelPassword);
//#
//#  Description:
//#    Switch to the channel with the name "Channelname"
//#    Not that tsrSwitchChannelID is preferred.
//#
//#  Input:
//#    ChannelName: Name of the channel to switch to
//#    ChannelPassword: Password for the channel. May be nil
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrSwitchChannelName)(char *ChannelName, char *ChannelPassword);
LPtsrSwitchChannelName tsrSwitchChannelName = NULL;

//##############################################################################
//#
//#  Function int tsrSwitchChannelID(int ChannelID, char *ChannelPassword);
//#
//#  Description:
//#    Switch to the channel with the id "channelID"
//#
//#  Input:
//#    ChannelID : ID of the channel to switch to
//#    ChannelPassword: Password for the channel. May be nil
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrSwitchChannelID)(int ChannelID, char *ChannelPassword);
LPtsrSwitchChannelID tsrSwitchChannelID = NULL;

//##############################################################################
//#
//#  Function int tsrGetVersion(TtsrVersion *tsrVersion);
//#
//#  Description:
//#    Get the version of the ts client
//#
//#  Input:
//#    Pointer to a TtsrVersion record
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#    if result = 0 then tsrVersion is filled with the client version
//#
//##############################################################################
typedef int (__stdcall  *LPtsrGetVersion)(TtsrVersion *tsrVersion);
LPtsrGetVersion tsrGetVersion = NULL;

//##############################################################################
//#
//#  Function int TtsrGetServerInfo(TtsrServerInfo *tsrServerInfo);
//#
//#  Description:
//#    Get the Info on the server (name, channelcount, playercount etc etc)
//#
//#  Input:
//#    Pointer to a TtsrServerInfo record
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#    if result = 0 then tsrServerInfo is filled with the server info
//#
//##############################################################################
typedef int (__stdcall  *LPtsrGetServerInfo)(TtsrServerInfo *tsrServerInfo);
LPtsrGetServerInfo TtsrGetServerInfo = NULL;

//##############################################################################
//#
//#  Function int tsrGetUserInfo(TtsrUserInfo *tsrUserInfo);
//#
//#  Description:
//#    Get the Info on the user (name, channel, flags etc etc)
//#
//#  Input:
//#    Pointer to a TtsrUserInfo record
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#    if result = 0 then tsrUserInfo is filled with the user info
//#
//##############################################################################
typedef int (__stdcall  *LPtsrGetUserInfo)(TtsrUserInfo *tsrUserInfo);
LPtsrGetUserInfo tsrGetUserInfo = NULL;

//##############################################################################
//#
//#  Function int tsrGetChannelInfoByID(int ChannelID,
//#										TtsrChannelInfo *tsrChannelInfo,
//#										TtsrPlayerInfo *tsrPlayerInfo,
//#										int *PlayerRecords);
//#
//#  Description:
//#    Get the Info on the channel specified by ChannelID and optionally also
//#    get the users from that channel
//#
//#  Input:
//#    ChannelID: The ID of the channel you want the info from
//#    tsrChannelInfo: pointer to a TtsrChannelInfo record;
//#    tsrPlayerInfo: This is the pointer to an array of TtsrPlayerInfo records
//#                   If it is NIL, no player records will be retrieved
//#    PlayerRecords: Pointer to an integer. It must contain how many records
//#                   tsrPlayerInfo has room for. (records, not bytes)
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#    if result = 0 then tsrChannelInfo is filled with the channel info.
//#                If tsrPlayerInfo was not NIL then the player records are
//#                filled. PlayerRecords indicates how many records were filled
//#
//##############################################################################
typedef int (__stdcall  *LPtsrGetChannelInfoByID)(int ChannelID,
												  TtsrChannelInfo *tsrChannelInfo,
												  TtsrPlayerInfo *tsrPlayerInfo,
												  int *PlayerRecords);
LPtsrGetChannelInfoByID tsrGetChannelInfoByID = NULL;

//##############################################################################
//#
//#  Function int tsrGetChannelInfoByName(char *ChannelName,
//#										  TtsrChannelInfo *tsrChannelInfo,
//#										  TtsrPlayerInfo *tsrPlayerInfo,
//#										  int *PlayerRecords);
//#
//#  Description:
//#    Get the Info on the channel specified by ChannelName and optionally also
//#    get the users from that channel
//#
//#  Input:
//#    ChannelName: The Name of the channel you want the info from
//#    tsrChannelInfo: pointer to a TtsrChannelInfo record;
//#    tsrPlayerInfo: This is the pointer to an array of TtsrPlayerInfo records
//#                   If it is NIL, no player records will be retrieved
//#    PlayerRecords: Pointer to an integer. It must contain how many records
//#                   tsrPlayerInfo has room for. (records, not bytes)
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#    if result = 0 then tsrChannelInfo is filled with the channel info.
//#                If tsrPlayerInfo was not NIL then the player records are
//#                filled. PlayerRecords indicates how many records were filled
//#
//##############################################################################
typedef int (__stdcall  *LPtsrGetChannelInfoByName)(char *ChannelName,
													TtsrChannelInfo *tsrChannelInfo,
													TtsrPlayerInfo *tsrPlayerInfo,
													int *PlayerRecords);
LPtsrGetChannelInfoByName tsrGetChannelInfoByName = NULL;

//##############################################################################
//#
//#  Function int tsrGetPlayerInfoByID(int PlayerID,
//#									   TtsrPlayerInfo *tsrServerInfo);
//#
//#  Description:
//#    Get the Info on the player specified by PlayerID.
//#
//#  Input:
//#    PlayerID: The ID of the player you want the info from
//#    tsrPlayerInfo: This is the pointer to a TtsrPlayerInfo record
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#    if result = 0 then tsrPlayerInfo is filled with the player info.
//#
//##############################################################################
typedef int (__stdcall  *LPtsrGetPlayerInfoByID)(int PlayerID,
												 TtsrPlayerInfo *tsrServerInfo);
LPtsrGetPlayerInfoByID tsrGetPlayerInfoByID = NULL;
//##############################################################################
//#
//#  Function int tsrGetPlayerInfoByName(char *PlayerName,
//#									   TtsrPlayerInfo *tsrServerInfo);
//#
//#  Description:
//#    Get the Info on the player specified by PlayerName.
//#
//#  Input:
//#    PlayerName: The Name of the player you want the info from
//#    tsrPlayerInfo: This is the pointer to a TtsrPlayerInfo record
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#    if result = 0 then tsrPlayerInfo is filled with the player info.
//#
//##############################################################################
typedef int (__stdcall  *LPtsrGetPlayerInfoByName)(char *PlayerName,
												 TtsrPlayerInfo *tsrServerInfo);
LPtsrGetPlayerInfoByName tsrGetPlayerInfoByName = NULL;

//##############################################################################
//#
//#  Function int tsrGetChannels(TtsrChannelInfo *tsrChannels,
//#								 int *ChannelRecords);
//#
//#  Description:
//#    Get a list of the channels on the server.
//#
//#  Input:
//#    tsrChannels: A pointer to an array of TtsrChannelInfo records
//#    ChannelRecords: pointer to a integer which specifies how many
//#                    TtsrChannelInfo records tsrChannels can hold.
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#    if result = 0 then tsrChannels is filled with the channel info.
//#              ChannelRecords will have the number or records that were filled
//#
//##############################################################################
typedef int (__stdcall  *LPtsrGetChannels)(TtsrChannelInfo *tsrChannels,
										   int *ChannelRecords);
LPtsrGetChannels tsrGetChannels = NULL;

//##############################################################################
//#
//#  Function int tsrGetPlayers(TtsrPlayerInfo *tsrPlayers,
//#										  int *PlayerRecords);
//#
//#  Description:
//#    Get a list of the Players on the server.
//#
//#  Input:
//#    tsrPlayers: A pointer to an array of TtsrPlayerInfo records
//#    PlayerRecords: pointer to a integer which specifies how many
//#                    TtsrPlayerInfo records tsrPlayers can hold.
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#    if result = 0 then tsrPlayers is filled with the Player info.
//#              PlayerRecords will have the number or records that were filled
//#
//##############################################################################
typedef int (__stdcall  *LPtsrGetPlayers)(TtsrPlayerInfo *tsrPlayers,
										  int *PlayerRecords);
LPtsrGetPlayers tsrGetPlayers = NULL;

//##############################################################################
//#
//#  Function int tsrSetPlayerFlags(int tsrPlayerFlags );
//#
//#  Description:
//#    Set player flags of the user
//#
//#  Input:
//#    tsrPlayerFlags: An integer wich has the bitmask for all the flags
//#                    All flags are set to this bitmask.
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrSetPlayerFlags)(int tsrPlayerFlags );
LPtsrSetPlayerFlags tsrSetPlayerFlags = NULL;

//##############################################################################
//#
//#  Function int tsrSetWantVoiceReason(char *tsrReason);
//#
//#  Description:
//#    set the reason you want voice on a channel
//#
//#  Input:
//#    tsrReason: The reseason for voice
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrSetWantVoiceReason)(char *tsrReason);
LPtsrSetWantVoiceReason tsrSetWantVoiceReason = NULL;

//##############################################################################
//#
//#  Function int tsrSetOperator(int PlayerID,int GrantRevoke);
//#
//#  Description:
//#    Grant or revoke Operator status
//#
//#  Input:
//#    PlayerID: ID of the player to set the operator status for
//#    GrantRevoke: Set to grGrant to grant or grRevoke to revoke the privilege
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrSetOperator)(int PlayerID,int GrantRevoke);
LPtsrSetOperator tsrSetOperator = NULL;

//##############################################################################
//#
//#  Function int tsrSetVoice(int PlayerID,int GrantRevoke);
//#
//#  Description:
//#    Grant or revoke Voice status
//#
//#  Input:
//#    PlayerID: ID of the player to set the Voice status for
//#    GrantRevoke: Set to grGrant to grant or grRevoke to revoke the privilege
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrSetVoice)(int PlayerID,int GrantRevoke);
LPtsrSetVoice tsrSetVoice = NULL;

//##############################################################################
//#
//#  Function int tsrKickPlayerFromServer(int PlayerID, char *Reason);
//#
//#  Description:
//#    Kick a player from the server;
//#
//#  Input:
//#    PlayerID: ID of the player to set the Voice status for
//#    Reason: The reason why he was kicked
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrKickPlayerFromServer)(int PlayerID, char *Reason);
LPtsrKickPlayerFromServer tsrKickPlayerFromServer = NULL;

//##############################################################################
//#
//#  Function int tsrKickPlayerFromChannel(int PlayerID, char *Reason);
//#
//#  Description:
//#    Kick a player from the Channel;
//#
//#  Input:
//#    PlayerID: ID of the player to set the Voice status for
//#    Reason: The reason why he was kicked
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrKickPlayerFromChannel)(int PlayerID, char *Reason);
LPtsrKickPlayerFromChannel tsrKickPlayerFromChannel = NULL;

//##############################################################################
//#
//#  Function int tsrSendTextMessageToChannel(int ChannelID,char *Message);
//#
//#  Description:
//#    Send a text message to a channel
//#
//#  Input:
//#    ChannelID: The ID of the channel you want to send the txt message to
//#    Message : The message you want to send.
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrSendTextMessageToChannel)(int ChannelID,
														char *Message);
LPtsrSendTextMessageToChannel tsrSendTextMessageToChannel = NULL;

//##############################################################################
//#
//#  Function int tsrSendTextMessage(char *Message);
//#
//#  Description:
//#    Send a text message to everyone
//#
//#  Input:
//#    Message : The message you want to send.
//#
//#  Output:
//#    Result: 0 = OK, else the error number
//#
//##############################################################################
typedef int (__stdcall  *LPtsrSendTextMessage)(char *Message);
LPtsrSendTextMessage tsrSendTextMessage = NULL;

int InitTsRemoteLibrary(const char* file)
{
	tsrDLL = LoadLibrary(file);
	if (!tsrDLL)
		return -2;

	tsrQuit = (LPtsrQuit)GetProcAddress(tsrDLL, "tsrQuit");
	tsrConnect = (LPtsrConnect)GetProcAddress(tsrDLL, "tsrConnect");
	tsrDisconnect = (LPtsrDisconnect)GetProcAddress(tsrDLL, "tsrDisconnect");
	tsrSendTextMessage = (LPtsrSendTextMessage)GetProcAddress(tsrDLL, "tsrSendTextMessage");
	tsrSendTextMessageToChannel = (LPtsrSendTextMessageToChannel)GetProcAddress(tsrDLL, "tsrSendTextMessageToChannel");
	tsrSwitchChannelID = (LPtsrSwitchChannelID)GetProcAddress(tsrDLL,"tsrSwitchChannelID");
	tsrSwitchChannelName = (LPtsrSwitchChannelName)GetProcAddress(tsrDLL,"tsrSwitchChannelName");
	tsrGetLastError = (LPtsrGetLastError)GetProcAddress(tsrDLL,"tsrGetLastError");
	tsrSetWantVoiceReason = (LPtsrSetWantVoiceReason)GetProcAddress(tsrDLL,"tsrSetWantVoiceReason");
	tsrSetVoice = (LPtsrSetVoice)GetProcAddress(tsrDLL,"tsrSetVoice");
	tsrSetOperator = (LPtsrSetOperator)GetProcAddress(tsrDLL,"tsrSetOperator");
	tsrSetPlayerFlags = (LPtsrSetPlayerFlags)GetProcAddress(tsrDLL,"tsrSetPlayerFlags");
	tsrKickPlayerFromServer = (LPtsrKickPlayerFromServer)GetProcAddress(tsrDLL,"tsrKickPlayerFromServer");
	tsrKickPlayerFromChannel = (LPtsrKickPlayerFromChannel)GetProcAddress(tsrDLL,"tsrKickPlayerFromChannel");
	tsrGetVersion = (LPtsrGetVersion)GetProcAddress(tsrDLL,"tsrGetVersion");
	tsrGetUserInfo = (LPtsrGetUserInfo)GetProcAddress(tsrDLL,"tsrGetUserInfo");
	tsrGetChannelInfoByID = (LPtsrGetChannelInfoByID)GetProcAddress(tsrDLL,"tsrGetChannelInfoByID");
	tsrGetChannelInfoByName = (LPtsrGetChannelInfoByName)GetProcAddress(tsrDLL,"tsrGetChannelInfoByName");
	tsrGetPlayerInfoByID = (LPtsrGetPlayerInfoByID)GetProcAddress(tsrDLL,"tsrGetPlayerInfoByID");
	tsrGetPlayerInfoByName = (LPtsrGetPlayerInfoByName)GetProcAddress(tsrDLL,"tsrGetPlayerInfoByName");
	TtsrGetServerInfo = (LPtsrGetServerInfo)GetProcAddress(tsrDLL,"tsrGetServerInfo");
	tsrGetPlayers = (LPtsrGetPlayers)GetProcAddress(tsrDLL,"tsrGetPlayers");
	tsrGetChannels = (LPtsrGetChannels)GetProcAddress(tsrDLL,"tsrGetChannels");
	if (!tsrQuit || !tsrDisconnect || !tsrConnect || !tsrSendTextMessage || !tsrSendTextMessageToChannel
		|| !tsrSwitchChannelID || !tsrSwitchChannelID || !tsrGetLastError || !tsrSetWantVoiceReason
		|| !tsrSetVoice || !tsrSetOperator || !tsrSetPlayerFlags || !tsrKickPlayerFromServer
		|| !tsrKickPlayerFromChannel || !tsrGetVersion || !tsrGetUserInfo || !tsrGetChannelInfoByID
		|| !tsrGetChannelInfoByName || !tsrGetPlayerInfoByID|| !tsrGetPlayerInfoByName
		|| !TtsrGetServerInfo || !tsrGetChannels) return -3;
	else return 0;
}
int CloseTsRemoteLibrary()
{
	if (!tsrDLL) return -1;
	return FreeLibrary(tsrDLL)?0:-2;
}