
// LoraBBS Version 2.99 Free Edition
// Copyright (C) 1987-98 Marco Maccaferri
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _LORA_H
#define _LORA_H

#include "bluewave.h"
#include "combase.h"
#include "collect.h"
#include "ftrans.h"
#include "lora_api.h"
#include "menu.h"
#include "msgbase.h"

#define INP_FIELD       0x0001
#define INP_FANCY       0x0002
#define INP_NOCRLF      0x0004
#define INP_PWD         0x0008
#define INP_NOCOLOR     0x0010
#define INP_HOTKEY      0x0020
#define INP_NUMERIC     0x0040
#define INP_NONUMHOT    0x0080

#define ASK_DEFYES      0x0001
#define ASK_DEFNO       0x0002
#define ASK_HELP        0x0004

#define ANSWER_YES      1
#define ANSWER_NO       2
#define ANSWER_HELP     3

class TEmbedded
{
public:
   TEmbedded ();
   ~TEmbedded ();

   USHORT Task;
   USHORT EndRun, Hangup, TimeLimit;
   ULONG  StartCall;
   USHORT Ansi, Avatar, Color, Rip;
   USHORT HotKey, More, ScreenHeight;
   CHAR   Path[64], AltPath[64];
   ULONG  CarrierSpeed;
   class  TCom *Com, *Snoop;
   class  TUser *User;
   class  TLanguage *Language;
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TMsgData *MsgArea;
   class  TFileData *FileArea;

   USHORT AbortSession ();
   VOID   BufferedPrintf (PSZ pszFormat, ...);
   VOID   BufferedPrintfAt (USHORT usRow, USHORT usColumn, PSZ pszFormat, ...);
   VOID   ClrEol ();
   USHORT DisplayFile (PSZ pszFile);
   USHORT DisplayPrompt (PSZ pszString, USHORT usColor, USHORT usHilight, USHORT usUnbuffer = FALSE);
   USHORT DisplayString (PSZ pszString);
   USHORT GetAnswer (USHORT flQuestion);
   USHORT Getch ();
   PSZ    GetString (PSZ pszBuffer, USHORT usMaxlen, USHORT flAttrib = 0);
   USHORT KBHit ();
   VOID   Idle ();
   PSZ    Input (PSZ pszBuffer, USHORT usMaxlen, USHORT flAttrib = 0);
   SHORT  MoreQuestion (SHORT nLine);
   VOID   OutString (PSZ pszFormat, ...);
   VOID   PressEnter ();
   VOID   Printf (PSZ pszFormat, ...);
   VOID   PrintfAt (USHORT usRow, USHORT usColumn, PSZ pszFormat, ...);
   VOID   Putch (UCHAR ucByte);
   VOID   RunExternal (PSZ Command);
   VOID   SetColor (USHORT usColor);
   ULONG  TimeRemain (USHORT seconds = FALSE);
   VOID   UnbufferBytes ();

private:
   FILE   *fp, *AnswerFile;
   USHORT LastColor, IsMec, TrasLen, IsDown, Required;
   SHORT  LastChar, Stop, StopNested, Nested, Line;
   CHAR   Temp[512], Temp2[128], Response;
   CHAR   OnExit[128];
   PSZ    Position;
   ULONG  LastActivity;
   CHAR   Traslate[64], *TrasPtr;
   time_t last_time;

   SHORT  GetNextChar ();
   FILE  *OpenFile (PSZ pszName, PSZ pszAccess = "rb");
   SHORT  PeekNextChar ();
   VOID   ProcessControl (UCHAR ucControl);
   VOID   ProcessControlF ();
   VOID   ProcessControlO ();
   VOID   ProcessControlP ();
   VOID   ProcessControlK ();
   VOID   ProcessControlW ();
   VOID   TranslateKeyword ();
};


// ---------------------------------------------------------------------------

class TMessage
{
public:
   TMessage (PSZ pszDataFile);
   ~TMessage ();

   USHORT ShowKludges;
   USHORT Width, Height, More, DoCls;
   class  TMsgData  *Current;
   class  TConfig   *Cfg;
   class  TLog      *Log;
   class  TEmbedded *Embedded;
   class  TLanguage *Language;
   class  TUser     *User;
   class  TMsgBase  *Msg;

   VOID   BriefList ();
   VOID   BuildDate (PSZ format, PSZ dest, MDATE *date);
   VOID   Delete ();
   VOID   DisplayCurrent ();
   VOID   DisplayText ();
   VOID   GetOrigin (class TMsgData *Data, PSZ Origin);
   VOID   OpenArea (PSZ area);
   VOID   Read (ULONG Number);
   VOID   ReadMessages ();
   VOID   ReadNext ();
   VOID   ReadNonStop ();
   VOID   ReadOriginal ();
   VOID   ReadPrevious ();
   VOID   ReadReply ();
   VOID   Reply ();
   USHORT SelectArea (PSZ pszArea);
   USHORT SelectNewArea (PSZ pszArea);
   VOID   StartMessageQuestion (ULONG ulFirst, ULONG ulLast, USHORT fNewMessages, ULONG &ulMsg, USHORT &fForward);
   VOID   TitleList ();
   VOID   Unreceive ();
   VOID   Write ();

private:
   CHAR   DataPath[128];
};

#define RANGE_UNDEFINED       0
#define RANGE_ALL             1
#define RANGE_TAGGED          2
#define RANGE_CURRENT         3

#define ACTION_UNDEFINED      0
#define ACTION_READ           1
#define ACTION_LIST           2

#define TYPE_ALL              0
#define TYPE_PERSONAL         1
#define TYPE_KEYWORD          2
#define TYPE_NEW              3
#define TYPE_PERSONALNEW      4

class TInquire
{
public:
   TInquire ();
   ~TInquire ();

   USHORT Type, Action, Range, Stop;
   USHORT ShowKludges;
   CHAR   Keyword[64];
   class  TConfig *Cfg;
   class  TEmbedded *Embedded;
   class  TUser *User;
   class  TLanguage *Language;
   class  TMsgData *Current;
   class  TLog *Log;

   VOID   DeleteCurrent ();
   VOID   DisplayCurrent ();
   USHORT First ();
   USHORT Next ();
   USHORT Previous ();
   VOID   Query ();

private:
   ULONG  Number;
   class  TMsgBase *Msg;
   class  TMsgData *Data;

   VOID   BuildDate (PSZ format, PSZ dest, MDATE *date);
   USHORT FirstMessage ();
   USHORT NextMessage ();
   USHORT PreviousMessage ();
   USHORT SearchAction ();
   USHORT SearchRange ();
};

// ---------------------------------------------------------------------------

class TLibrary
{
public:
   TLibrary (PSZ pszDataFile);
   ~TLibrary ();

   USHORT Task;
   ULONG  CarrierSpeed;
   class  TConfig *Cfg;
   class  TEmbedded *Embedded;
   class  TLog *Log;
   class  TUser *User;
   class  TFileData *Current;
   class  TProgress *Progress;
   class  TLanguage *Language;

   VOID   Download (class TFileTag *Files = NULL, USHORT AnyLibrary = FALSE);
   USHORT DownloadFile (PSZ pszFile, PSZ pszName, ULONG ulSize);
   VOID   DownloadList ();
   VOID   ExternalProtocols (USHORT Batch);
   VOID   FileDetails (class TFileBase *File);
   VOID   ListDownloadedFiles ();
   VOID   ListFiles (class TFileBase *Data = NULL);
   VOID   ListRecentFiles ();
   VOID   AddTagged ();
   VOID   ListTagged ();
   VOID   DeleteTagged ();
   VOID   DeleteAllTagged ();
   VOID   RemoveFiles ();
   VOID   SearchFileName ();
   VOID   SearchKeyword ();
   VOID   SearchNewFiles ();
   USHORT SearchRange ();
   VOID   SearchText ();
   USHORT SelectArea (PSZ pszArea);
   VOID   TypeFile ();
   VOID   Upload ();
   VOID   UploadUser (PSZ user);

private:
   CHAR   DataPath[128];
   class  TStatistics *Stats;

   SHORT  MoreQuestion (SHORT nLine);
   VOID   TagListed ();
};

// ---------------------------------------------------------------------------

class TEMail
{
public:
   TEMail ();
   ~TEMail ();

   USHORT ShowKludges, Storage;
   USHORT Width, Height, More, DoCls;
   CHAR   BasePath[128];
   class  TConfig   *Cfg;
   class  TLog      *Log;
   class  TEmbedded *Embedded;
   class  TLanguage *Language;
   class  TUser     *User;
   class  TMsgBase  *Msg;

   VOID   BriefList ();
   VOID   BuildDate (PSZ format, PSZ dest, MDATE *date);
   VOID   CheckUnread ();
   VOID   Delete ();
   VOID   DisplayCurrent ();
   VOID   DisplayText ();
   VOID   Read (ULONG Number);
   VOID   ReadMessages (USHORT fUnreaded = FALSE);
   VOID   ReadNext ();
   VOID   ReadNonStop ();
   VOID   ReadPrevious ();
   VOID   Reply (USHORT ToCurrent = FALSE);
   VOID   StartMessageQuestion (ULONG ulFirst, ULONG ulLast, ULONG &ulMsg, USHORT &fForward);
   VOID   Write (USHORT Type, PSZ Argument = NULL);
};

// ---------------------------------------------------------------------------

class DLL_EXPORT TMailerStatus
{
public:
   TMailerStatus ();
   virtual ~TMailerStatus ();

   CHAR   SystemName[64];
   CHAR   SysopName[64];
   CHAR   Location[64];
   CHAR   Address[48], Akas[128];
   CHAR   Program[48];
   USHORT InPktFiles, InDataFiles;
   USHORT OutPktFiles, OutDataFiles;
   ULONG  InPktBytes, InDataBytes;
   ULONG  OutPktBytes, OutDataBytes;
   ULONG  Speed;

   virtual VOID   Update ();
};

// ---------------------------------------------------------------------------

class DLL_EXPORT TStatus
{
public:
   TStatus ();
   virtual ~TStatus ();

   virtual VOID   Clear ();
   virtual VOID   SetLine (USHORT line, PSZ text, ...);
};

// ---------------------------------------------------------------------------

class DLL_EXPORT TBbs
{
public:
   TBbs ();
   ~TBbs ();

   USHORT Task, AutoDetect, TimeLimit, Remote, Local;
   USHORT FancyNames, Logoff;
   ULONG  Speed, StartCall;
   class  TCom *Com, *Snoop;
   class  TLog *Log;
   class  TEvents *Events;
   class  TConfig *Cfg;
   class  TEmbedded *Embedded;
   class  TProgress *Progress;
   class  TMailerStatus *MailerStatus;
   class  TStatus *Status;

   VOID   ExecuteCommand (class TMenu *Menu);
   USHORT FileExist (PSZ FileName);
   VOID   IEMSILogin ();
   USHORT Login ();
   VOID   Run ();

private:
   USHORT whStatus, Reload;
   CHAR   Name[36], Password[16], Cmd[128];
   CHAR   MenuName[32];
   class  TCollection Stack;
   class  TScreen *Screen;
   class  TUser *User;
   class  TLanguage *Language;
   class  TMenu *Menu;
   class  TMessage *Message;
   class  TLibrary *Library;
   class  TEMail *EMail;

   VOID   CheckBirthday ();
   VOID   DisableUseronRecord ();
   VOID   SetBirthDate ();
   VOID   SetUseronRecord (PSZ pszStatus);
   VOID   ToggleNoDisturb ();
};

// ---------------------------------------------------------------------------

#define REMOTE_NONE           0
#define REMOTE_USER           1
#define REMOTE_MAILER         2
#define REMOTE_MAILRECEIVED   3
#define REMOTE_PPP            4

class DLL_EXPORT TDetect
{
public:
   TDetect ();
   ~TDetect ();

   USHORT Task, Remote;
   UCHAR  Ansi, Avatar, Rip, FullEd, MorePrompt;
   UCHAR  IBMChars, HotKeys, ScreenClear, MailCheck, FileCheck;
   USHORT EMSI, YooHoo, IEMSI;
   USHORT Capabilities;
   CHAR   Name[48], RealName[48], City[48];
   CHAR   Password[48];
   CHAR   RemoteSystem[48], RemoteSysop[48], RemoteLocation[48];
   CHAR   RemoteProgram[48];
   CHAR   Inbound[48];
   ULONG  Speed;
   class  TAddress Address;
   class  TCom *Com;
   class  TEvents *Events;
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TProgress *Progress;
   class  TMailerStatus *MailerStatus;
   class  TStatus *Status;

   USHORT AbortSession ();
   USHORT EMSIReceiver ();
   USHORT EMSISender ();
   VOID   IEMSIReceiver ();
   USHORT RemoteMailer ();
   VOID   Terminal ();
   USHORT WaZOOReceiver ();
   USHORT WaZOOSender ();

private:
   CHAR   ReceiveIEMSI[2048], *ReceiveEMSI;
   CHAR   SendIEMSI[2048], *SendEMSI;
   ULONG  StartCall, LastPktName;

   USHORT CheckEMSIPacket ();
   VOID   ParseEMSIPacket ();
   VOID   ParseIEMSIPacket ();
   USHORT ReceiveHello ();
   USHORT ReceiveEMSIPacket ();
   USHORT ReceiveIEMSIPacket ();
   VOID   Receiver ();
   VOID   Sender ();
   USHORT SendHello ();
   VOID   SendEMSIPacket ();
   VOID   SendIEMSIPacket ();
   SHORT  TimedRead ();
};

// ---------------------------------------------------------------------------

#define FTP_PORT           21
#define TELNET_PORT        23
#define FINGER_PORT        79
#define FTPDATA_PORT       2048
#define VMODEM_PORT        3141
#define IRC_PORT           6667

class TInternet
{
public:
   TInternet ();
   ~TInternet ();

   class  TConfig *Cfg;
   class  TCom *Com, *Snoop;
   class  TEmbedded *Embedded;
   class  TLog *Log;
   class  TUser *User;

   VOID   Finger (PSZ pszServer = NULL, USHORT usPort = FINGER_PORT);
   VOID   FTP (PSZ pszServer = NULL, USHORT usPort = FTP_PORT);
   VOID   IRC (PSZ pszServer = NULL, PSZ pszNick = NULL, USHORT usPort = IRC_PORT);
   VOID   Telnet (PSZ pszServer = NULL, USHORT usPort = TELNET_PORT);

private:
   USHORT Hash, Binary, DataPort;
   CHAR   Temp[128], Cmd[48], Host[32];
   UCHAR  Buffer[2048];
   CHAR   *Tokens[256];
   class  TTcpip *Tcp;
   class  TTcpip *Data;

   USHORT GetResponse (PSZ pszResponse, USHORT usMaxLen);
   VOID   FTP_GET (PSZ pszFile, PSZ pszName, USHORT fHash);
   VOID   FTP_MGET (PSZ pszFile);
   VOID   FTP_PUT (PSZ pszFile, USHORT fHash, USHORT fBinary);
};

// ---------------------------------------------------------------------------

class TEditor
{
public:
   TEditor ();
   virtual ~TEditor ();

   USHORT UseFullScreen;
   USHORT StartCol, StartRow;
   USHORT Width, Height;
   class  TEmbedded *Embedded;
   class  TLanguage *Language;

   virtual USHORT AppendText ();
   virtual VOID   ChangeText ();
   virtual VOID   Clear ();
   virtual VOID   DeleteLine ();
   virtual VOID   DisplayScreen ();
   virtual USHORT ExternalEditor (PSZ EditorCmd);
   virtual USHORT FullScreen ();
   virtual USHORT InputText ();
   virtual USHORT InsertLines ();
   virtual VOID   ListText ();
   virtual VOID   RetypeLine ();

protected:
   USHORT cx, cy;
   CHAR   Wrap[128], *Buffer, *Cursor;
   CHAR   ActualLine[128];
   ULONG  LineCrc[51];
   class  TCollection Text;

   VOID   BuildDate (PSZ format, PSZ dest, MDATE *date);
   VOID   Display (USHORT line);
   PSZ    GetFirstChar (USHORT start, USHORT line);
   PSZ    GetString (CHAR *pszBuffer, USHORT usMaxlen);
   VOID   GotoXY (USHORT x, USHORT y);
   VOID   MoveCursor (USHORT start);
   VOID   SetCursor (USHORT start);
   PSZ    StringReplace (PSZ pszStr, PSZ pszSearch, PSZ pszReplace);
   VOID   UpdateLine (USHORT y, PSZ pszLine);
};

class TMsgEditor : public TEditor
{
public:
   TMsgEditor ();
   ~TMsgEditor ();

   USHORT EchoMail;
   CHAR   AreaKey[16];
   CHAR   AreaTitle[128];
   CHAR   UserName[48];
   CHAR   Address[64];
   CHAR   Origin[64];
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TMsgBase *Msg;

   VOID   DisplayScreen ();
   VOID   Forward ();
   VOID   InputSubject ();
   USHORT InputTo ();
   VOID   Menu ();
   USHORT Modify ();
   VOID   QuoteText ();
   USHORT Reply ();
   VOID   Save ();
   USHORT Write ();

private:
   CHAR   To[36];
   CHAR   Subject[72];
   ULONG  Number, Msgn;
   struct dosdate_t d_date;
   struct dostime_t d_time;
};

class TCommentEditor : public TEditor
{
public:
   TCommentEditor ();
   ~TCommentEditor ();

   class  TFileBase *File;

   VOID   Menu ();
   VOID   Save ();
   USHORT Write ();
};

#define MAIL_LOCAL         0
#define MAIL_FIDONET       1
#define MAIL_INTERNET      2

class TMailEditor : public TEditor
{
public:
   TMailEditor ();
   ~TMailEditor ();

   USHORT Type, Storage;
   UCHAR  Private;
   CHAR   BasePath[128];
   CHAR   UserName[48];
   CHAR   Origin[64];
   CHAR   Address[64];
   CHAR   AreaTitle[128];
   CHAR   To[36];
   CHAR   ToAddress[64];
   CHAR   Subject[72];
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TMsgBase *Msg;

   VOID   DisplayScreen ();
   VOID   Forward ();
   USHORT InputAddress ();
   USHORT InputSubject ();
   USHORT InputTo ();
   VOID   Menu ();
   USHORT Modify ();
   VOID   QuoteText ();
   USHORT Reply ();
   VOID   Save ();
   USHORT Write ();

private:
   ULONG  Number;
   struct dosdate_t d_date;
   struct dostime_t d_time;
};

// ---------------------------------------------------------------------------

class TOffline
{
public:
   TOffline ();
   virtual ~TOffline ();

   USHORT Area;
   CHAR   Id[16], Path[128];
   ULONG  Limit, CarrierSpeed;
   ULONG  Current, Total;
   ULONG  Personal, TotalPersonal;
   ULONG  Reply;
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TEmbedded *Embedded;
   class  TUser *User;
   class  TLanguage *Language;
   class  TProgress *Progress;

   virtual VOID   AddConference ();
   virtual VOID   AddKludges (class TCollection &Text, class TMsgData *Data);
   virtual USHORT Create ();
   virtual USHORT Compress (PSZ pszPacket);
   virtual VOID   Display ();
   virtual VOID   Download (PSZ pszFile, PSZ pszName);
   virtual VOID   ManageTagged ();
   virtual USHORT FetchReply ();
   virtual VOID   PackArea (ULONG &ulLast);
   virtual VOID   PackEMail (ULONG &ulLast);
   virtual USHORT Prescan ();
   virtual VOID   RemoveArea ();
   virtual VOID   RestrictDate ();
   virtual VOID   Scan (PSZ pszKey, ULONG ulLast);
   virtual USHORT TooOld (ULONG Restrict, class TMsgBase *Msg);
   virtual VOID   Upload ();

protected:
   CHAR   Work[128];
   USHORT BarWidth;
   ULONG  TotalPack;
   class  TMsgData *MsgArea;
   class  TMsgBase *Msg;
   class  TMsgTag NewMsgTag;
   class  TMsgTag *MsgTag;
};

class TBlueWave : public TOffline
{
public:
   TBlueWave ();
   ~TBlueWave ();

   USHORT Create ();
   USHORT FetchReply ();
   VOID   PackArea (ULONG &ulLast);

private:
   INF_HEADER    Inf;
   INF_AREA_INFO AreaInf;
   MIX_REC       Mix;
   FTI_REC       Fti;
   UPI_HEADER    Upih;
   UPI_REC       Upir;
   UPL_HEADER    Uplh;
   UPL_REC       Uplr;
};

typedef struct {
   UCHAR  Msgstat;
   UCHAR  Msgnum[7];
   UCHAR  Msgdate[8];
   UCHAR  Msgtime[5];
   CHAR   MsgTo[25];
   CHAR   MsgFrom[25];
   CHAR   MsgSubj[25];
   UCHAR  Msgpass[12];
   UCHAR  Msgrply[8];
   UCHAR  Msgrecs[6];
   UCHAR  Msglive;
   UCHAR  Msgarealo;
   UCHAR  Msgareahi;
   UCHAR  Msgfiller[3];
} QWKHDR;

typedef union {
   UCHAR  uc[10];
   USHORT ui[5];
   ULONG  ul[2];
   float  f[2];
   double d[1];
} QWKCONV;

class TQWK : public TOffline
{
public:
   TQWK ();
   ~TQWK ();

   USHORT Create ();
   USHORT FetchReply ();
   VOID   PackArea (ULONG &ulLast);

private:
   CHAR   TempStr[32];
   ULONG  Blocks;
   QWKHDR Qwk;

   float  IEEToMSBIN (float f);
   PSZ    StripSpaces (PSZ pszString, USHORT usSize);
};

class TAscii : public TOffline
{
public:
   TAscii ();
   ~TAscii ();

   USHORT Create ();
   VOID   PackArea (ULONG &ulLast);
};

class TPoint : public TOffline
{
public:
   TPoint ();
   ~TPoint ();

   USHORT Create ();
   USHORT FetchReply ();
   VOID   PackArea (ULONG &ulLast);
};

// ---------------------------------------------------------------------------

typedef struct {
   CHAR   Key[16];
   ULONG  ActiveFiles;
   ULONG  ActiveMsgs;
   CHAR   Display[128];
} LISTDATA;

class TListings
{
public:
   TListings ();
   virtual ~TListings ();

   class  TEmbedded *Embedded;
   class  TLog *Log;
   class  TUser *User;
   class  TLanguage *Language;

   virtual VOID   Begin ();
   virtual VOID   Down ();
   virtual VOID   Exit ();
   virtual VOID   DownloadTag ();
   virtual USHORT DrawScreen ();
   virtual VOID   PageDown ();
   virtual VOID   PageUp ();
   virtual VOID   PrintCursor (USHORT y);
   virtual VOID   PrintLine ();
   virtual VOID   PrintTitles ();
   virtual VOID   RemoveCursor (USHORT y);
   virtual USHORT Run ();
   virtual VOID   Select ();
   virtual VOID   Tag ();
   virtual VOID   Up ();

protected:
   USHORT y;
   USHORT RetVal, End, Found;
   USHORT Redraw, Titles;
   class  TCollection List;
   class  TCollection Data;
   LISTDATA *pld;
};

// ----------------------------------------------------------------------

#if defined(__OS2__)
VOID CallRexx (class TBbs *lpBbs, PSZ Name);
VOID CallRexx (class TEmbedded *lpEmbed, PSZ Name);
#endif

VOID ParseAddress (PSZ text, PSZ name, PSZ address);

#endif


