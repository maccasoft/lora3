
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    02/03/96 - Initial coding.
// ----------------------------------------------------------------------

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

#define ASK_DEFYES      0x0001
#define ASK_DEFNO       0x0002
#define ASK_HELP        0x0004

#define ANSWER_YES      1
#define ANSWER_NO       2
#define ANSWER_HELP     3

class TEmbedded
{
public:
   TEmbedded (void);
   ~TEmbedded (void);

   USHORT Task;
   USHORT EndRun, Hangup, TimeLimit;
   ULONG  StartCall;
   USHORT Ansi, Avatar, Color, Rip;
   USHORT HotKey, More, ScreenHeight;
   CHAR   Path[64], AltPath[64];
   class  TCom *Com, *Snoop;
   class  TUser *User;
   class  TLanguage *Language;
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TMsgData *MsgArea;
   class  TFileData *FileArea;

   USHORT AbortSession (VOID);
   VOID   BufferedPrintf (PSZ pszFormat, ...);
   VOID   BufferedPrintfAt (USHORT usRow, USHORT usColumn, PSZ pszFormat, ...);
   VOID   ClrEol (VOID);
   USHORT DisplayFile (PSZ pszFile);
   USHORT DisplayPrompt (PSZ pszString, USHORT usColor, USHORT usHilight, USHORT usUnbuffer = FALSE);
   USHORT DisplayString (PSZ pszString);
   USHORT GetAnswer (USHORT flQuestion);
   USHORT Getch ();
   PSZ    GetString (PSZ pszBuffer, USHORT usMaxlen, USHORT flAttrib = 0);
   USHORT KBHit ();
   VOID   Idle (VOID);
   PSZ    Input (PSZ pszBuffer, USHORT usMaxlen, USHORT flAttrib = 0);
   SHORT  MoreQuestion (SHORT nLine);
   VOID   OutString (PSZ pszFormat, ...);
   VOID   PressEnter (VOID);
   VOID   Printf (PSZ pszFormat, ...);
   VOID   PrintfAt (USHORT usRow, USHORT usColumn, PSZ pszFormat, ...);
   VOID   Putch (UCHAR ucByte);
   VOID   RunExternal (PSZ Command);
   VOID   SetColor (USHORT usColor);
   USHORT TimeRemain (VOID);
   VOID   UnbufferBytes ();

private:
   FILE   *fp;
   USHORT LastColor;
   SHORT  LastChar, Stop, StopNested, Nested, Line;
   CHAR   Temp[512], Temp2[128], Response;
   PSZ    Position;
   ULONG  LastActivity;

   SHORT  GetNextChar (VOID);
   FILE  *OpenFile (PSZ pszName, PSZ pszAccess = "rb");
   SHORT  PeekNextChar (VOID);
   VOID   ProcessControl (UCHAR ucControl);
   VOID   ProcessControlF (UCHAR ucControl);
   VOID   ProcessControlW (UCHAR ucControl);
};


// ---------------------------------------------------------------------------

class TMessage
{
public:
   TMessage (PSZ pszDataFile);
   ~TMessage (void);

   USHORT ShowKludges;
   USHORT Width, Height, More, DoCls;
   class  TMsgData  *Current;
   class  TConfig   *Cfg;
   class  TLog      *Log;
   class  TEmbedded *Embedded;
   class  TLanguage *Language;
   class  TUser     *User;
   class  TMsgBase  *Msg;

   VOID   BriefList (VOID);
   VOID   Delete (VOID);
   VOID   DisplayCurrent (VOID);
   VOID   Read (ULONG Number);
   VOID   ReadMessages (VOID);
   VOID   ReadNext (VOID);
   VOID   ReadPrevious (VOID);
   VOID   Reply (VOID);
   USHORT SelectArea (PSZ pszArea);
   USHORT SelectNewArea (PSZ pszArea);
   VOID   StartMessageQuestion (ULONG ulFirst, ULONG ulLast, USHORT fNewMessages, ULONG &ulMsg, USHORT &fForward);
   VOID   TextList (VOID);
   VOID   TitleList (VOID);

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
   TInquire (void);
   ~TInquire (void);

   USHORT Type, Action, Range, Stop;
   USHORT ShowKludges;
   CHAR   Keyword[64];
   class  TConfig *Cfg;
   class  TEmbedded *Embedded;
   class  TUser *User;
   class  TLanguage *Language;
   class  TMsgData *Current;
   class  TLog *Log;

   VOID   DeleteCurrent (VOID);
   VOID   DisplayCurrent (VOID);
   USHORT First (VOID);
   USHORT Next (VOID);
   USHORT Previous (VOID);
   VOID   Query (VOID);

private:
   ULONG  Number;
   class  TMsgBase *Msg;
   class  TMsgData *Data;

   USHORT FirstMessage (VOID);
   USHORT NextMessage (VOID);
   USHORT PreviousMessage (VOID);
   USHORT SearchAction (VOID);
   USHORT SearchRange (VOID);
};

// ---------------------------------------------------------------------------

class TLibrary
{
public:
   TLibrary (PSZ pszDataFile);
   ~TLibrary (void);

   USHORT Task;
   ULONG  CarrierSpeed;
   class  TConfig *Cfg;
   class  TEmbedded *Embedded;
   class  TLog *Log;
   class  TUser *User;
   class  TFileData *Current;
   class  TProgress *Progress;

   VOID   Download (class TFileTag *Files = NULL, USHORT AnyLibrary = FALSE);
   USHORT DownloadFile (PSZ pszFile, PSZ pszName, ULONG ulSize);
   VOID   DownloadList (VOID);
   VOID   ExternalProtocols (USHORT Batch);
   VOID   FileDetails (class TFileBase *File);
   VOID   ListDownloadedFiles (VOID);
   VOID   ListFiles (class TFileBase *Data = NULL);
   VOID   ListRecentFiles (VOID);
   VOID   ManageTagged (VOID);
   VOID   RemoveFiles (VOID);
   VOID   SearchFileName (VOID);
   VOID   SearchKeyword (VOID);
   VOID   SearchNewFiles (VOID);
   USHORT SearchRange (VOID);
   VOID   SearchText (VOID);
   USHORT SelectArea (PSZ pszArea);
   VOID   TypeFile (VOID);
   VOID   Upload (VOID);
   VOID   UploadUser (PSZ user);

private:
   CHAR   DataPath[128];
   class  TStatistics *Stats;
};

// ---------------------------------------------------------------------------

class TEMail
{
public:
   TEMail (void);
   ~TEMail (void);

   USHORT ShowKludges, Storage;
   USHORT Width, Height, More, DoCls;
   CHAR   BasePath[128];
   class  TConfig   *Cfg;
   class  TLog      *Log;
   class  TEmbedded *Embedded;
   class  TLanguage *Language;
   class  TUser     *User;
   class  TMsgBase  *Msg;

   VOID   BriefList (VOID);
   VOID   CheckUnread (VOID);
   VOID   Delete (VOID);
   VOID   DisplayCurrent (VOID);
   VOID   ReadMessages (VOID);
   VOID   StartMessageQuestion (ULONG ulFirst, ULONG ulLast, ULONG &ulMsg, USHORT &fForward);
};

// ---------------------------------------------------------------------------

class DLL_EXPORT TMailerStatus
{
public:
   TMailerStatus (void);
   virtual ~TMailerStatus (void);

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

   virtual VOID   Update (VOID);
};

// ---------------------------------------------------------------------------

class DLL_EXPORT TStatus
{
public:
   TStatus (void);
   virtual ~TStatus (void);

   virtual VOID   Clear (VOID);
   virtual VOID   SetLine (USHORT line, PSZ text, ...);
};

// ---------------------------------------------------------------------------

class DLL_EXPORT TBbs
{
public:
   TBbs (void);
   ~TBbs (void);

   USHORT Task, AutoDetect, TimeLimit, Remote;
   USHORT FancyNames, Logoff;
   ULONG  Speed, StartCall;
   class  TCom *Com, *Snoop;
   class  TLog *Log;
   class  TConfig *Cfg;
   class  TEmbedded *Embedded;
   class  TProgress *Progress;
   class  TMailerStatus *MailerStatus;
   class  TStatus *Status;

   VOID   ExecuteCommand (class TMenu *Menu);
   USHORT FileExist (PSZ FileName);
   VOID   IEMSILogin (VOID);
   USHORT Login (VOID);
   VOID   Run (VOID);

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

   VOID   ResetUseronRecord (VOID);
   VOID   SetUseronRecord (USHORT id, PSZ status = NULL, PSZ name = NULL);
   VOID   ToggleNoDisturb (VOID);
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
   TDetect (void);
   ~TDetect (void);

   USHORT Task;
   USHORT Ansi, Avatar, Rip;
   USHORT Remote;
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
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TProgress *Progress;
   class  TMailerStatus *MailerStatus;
   class  TStatus *Status;

   USHORT AbortSession (VOID);
   USHORT EMSIReceiver (VOID);
   USHORT EMSISender (VOID);
   VOID   IEMSIReceiver (VOID);
   USHORT RemoteMailer (VOID);
   VOID   Terminal (VOID);
   USHORT WaZOOReceiver (VOID);
   USHORT WaZOOSender (VOID);

private:
   CHAR   ReceiveIEMSI[2048], *ReceiveEMSI;
   CHAR   SendIEMSI[2048], *SendEMSI;
   ULONG  StartCall, LastPktName;

   USHORT CheckEMSIPacket (VOID);
   VOID   ParseEMSIPacket (VOID);
   VOID   ParseIEMSIPacket (VOID);
   USHORT ReceiveHello (VOID);
   USHORT ReceiveEMSIPacket (VOID);
   USHORT ReceiveIEMSIPacket (VOID);
   VOID   Receiver (VOID);
   VOID   Sender (VOID);
   USHORT SendHello (VOID);
   VOID   SendEMSIPacket (VOID);
   VOID   SendIEMSIPacket (VOID);
   SHORT  TimedRead (VOID);
};

// ---------------------------------------------------------------------------

#define FTP_PORT           21
#define TELNET_PORT        23
#define FINGER_PORT        79
#define FTPDATA_PORT       2048
#define VMODEM_PORT        3141

class TInternet
{
public:
   TInternet (void);
   ~TInternet (void);

   class  TConfig *Cfg;
   class  TCom *Com, *Snoop;
   class  TEmbedded *Embedded;
   class  TLog *Log;
   class  TUser *User;

   VOID   Finger (PSZ pszServer = NULL, USHORT usPort = FINGER_PORT);
   VOID   FTP (PSZ pszServer = NULL, USHORT usPort = FTP_PORT);
   VOID   Telnet (PSZ pszServer = NULL, USHORT usPort = TELNET_PORT);

private:
   USHORT Hash, Binary, DataPort;
   CHAR   Temp[128], Cmd[48], Host[32];
   UCHAR  Buffer[2048];
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
   TEditor (void);
   virtual ~TEditor (void);

   USHORT ScreenWidth;
   class  TEmbedded *Embedded;
   class  TLanguage *Lang;

   USHORT AppendText (VOID);
   VOID   ChangeText (VOID);
   VOID   Clear (VOID);
   VOID   DeleteLine (VOID);
   USHORT InputText (VOID);
   USHORT InsertLines (VOID);
   VOID   ListText (VOID);
   VOID   RetypeLine (VOID);

protected:
   CHAR   Wrap[128];
   class  TCollection Text;

   PSZ    GetString (CHAR *pszBuffer, USHORT usMaxlen);
   PSZ    StringReplace (PSZ pszStr, PSZ pszSearch, PSZ pszReplace);
};

class TFullEditor
{
public:
   TFullEditor (void);
   ~TFullEditor (void);

   USHORT EchoMail;
   USHORT StartCol, StartRow;
   USHORT Width, Height;
   CHAR   AreaTitle[128];
   CHAR   From[64], To[64];
   CHAR   Subject[72];
   CHAR   FromAddress[48], ToAddress[48];
   CHAR   Origin[96];
   ULONG  Msgn, Number;
   class  TEmbedded *Embedded;
   class  TLanguage *Lang;
   class  TMsgBase *Msg;
   class  TLog *Log;
   class  TCollection Text;

   VOID   InputSubject (VOID);
   VOID   InputTo (VOID);
   USHORT Reply (VOID);
   USHORT Run (VOID);
   USHORT Write (VOID);
   VOID   Save (VOID);

private:
   USHORT cx, cy;
   CHAR   *Buffer, *Cursor;
   ULONG  LineCrc[51];
   struct dosdate_t d_date;
   struct dostime_t d_time;

   VOID   Display (USHORT line);
   VOID   DisplayScreen (VOID);
   PSZ    GetFirstChar (USHORT start, USHORT line);
   VOID   GotoXY (USHORT x, USHORT y);
   VOID   MoveCursor (USHORT start);
   VOID   Pause (VOID);
   VOID   SetCursor (USHORT start);
};

class TMsgEditor : public TEditor
{
public:
   TMsgEditor (void);
   ~TMsgEditor (void);

   USHORT EchoMail;
   CHAR   UserName[48];
   CHAR   Origin[64], Address[64];
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TMsgBase *Msg;

   VOID   Forward (VOID);
   VOID   InputSubject (VOID);
   VOID   InputTo (VOID);
   VOID   Menu (VOID);
   USHORT Modify (VOID);
   VOID   QuoteText (VOID);
   USHORT Reply (VOID);
   VOID   Save (VOID);
   USHORT Write (VOID);

private:
   CHAR   To[36];
   CHAR   Subject[72];
   ULONG  Number;
};

class TCommentEditor : public TEditor
{
public:
   TCommentEditor (void);
   ~TCommentEditor (void);

   class  TFileBase *File;

   VOID   Menu (VOID);
   VOID   Save (VOID);
   USHORT Write (VOID);
};

class TMailEditor : public TEditor
{
public:
   TMailEditor (void);
   ~TMailEditor (void);

   USHORT Storage;
   CHAR   BasePath[128];
   CHAR   UserName[48];
   CHAR   Origin[64], Address[64];
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TMsgBase *Msg;

   VOID   Forward (VOID);
   USHORT InputSubject (VOID);
   USHORT InputTo (VOID);
   VOID   Menu (VOID);
   USHORT Modify (VOID);
   VOID   QuoteText (VOID);
   USHORT Reply (VOID);
   VOID   Save (VOID);
   USHORT Write (VOID);

private:
   CHAR   To[36];
   CHAR   Subject[72];
   ULONG  Number;
};

// ---------------------------------------------------------------------------

class TOffline
{
public:
   TOffline (void);
   virtual ~TOffline (void);

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
   class  TLanguage *Lang;
   class  TProgress *Progress;

   virtual VOID   AddConference (VOID);
   virtual USHORT Create (VOID);
   virtual USHORT Compress (PSZ pszPacket);
   virtual VOID   Display (VOID);
   virtual VOID   Download (PSZ pszFile, PSZ pszName);
   virtual USHORT FetchReply (VOID);
   virtual VOID   PackArea (ULONG &ulLast);
   virtual VOID   PackEMail (ULONG &ulLast);
   virtual USHORT Prescan (VOID);
   virtual VOID   RemoveArea (VOID);
   virtual VOID   Scan (PSZ pszKey, ULONG ulLast);
   virtual VOID   Upload (VOID);

protected:
   CHAR   Work[128];
   USHORT BarWidth;
   ULONG  TotalPack;
   class  TMsgData *MsgArea;
   class  TMsgBase *Msg;
   class  TMsgTag NewMsgTag;
};

class TBlueWave : public TOffline
{
public:
   TBlueWave (void);
   ~TBlueWave (void);

   USHORT Create (VOID);
   USHORT FetchReply (VOID);
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
   TQWK (void);
   ~TQWK (void);

   USHORT Create (VOID);
   USHORT FetchReply (VOID);
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
   TAscii (void);
   ~TAscii (void);

   USHORT Create (VOID);
   VOID   PackArea (ULONG &ulLast);
};

class TPoint : public TOffline
{
public:
   TPoint (void);
   ~TPoint (void);

   USHORT Create (VOID);
   USHORT FetchReply (VOID);
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
   TListings (void);
   virtual ~TListings (void);

   class  TEmbedded *Embedded;
   class  TLog *Log;
   class  TUser *User;
   class  TLanguage *Language;

   virtual VOID   Begin (VOID);
   virtual VOID   Down (VOID);
   virtual VOID   Exit (VOID);
   virtual VOID   DownloadTag (VOID);
   virtual USHORT DrawScreen (VOID);
   virtual VOID   PageDown (VOID);
   virtual VOID   PageUp (VOID);
   virtual VOID   PrintCursor (USHORT y);
   virtual VOID   PrintLine (VOID);
   virtual VOID   PrintTitles (VOID);
   virtual VOID   RemoveCursor (USHORT y);
   virtual USHORT Run (VOID);
   virtual VOID   Select (VOID);
   virtual VOID   Tag (VOID);
   virtual VOID   Up (VOID);

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


