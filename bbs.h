
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _BBS_H
#define _BBS_H

#include "combase.h"
#include "config.h"
#include "tools.h"
#include "user.h"

typedef struct {
   USHORT Active;
   USHORT Task;
   CHAR   User[48];
   ULONG  Speed;
   CHAR   Status[48];
   CHAR   Location[48];
   ULONG  UpTime;
   ULONG  ChangeTime;
} CHSTAT;

class DLL_EXPORT TStatus
{
public:
   TStatus (PSZ pszDataPath);
   ~TStatus (void);

   USHORT Active, Task;
   CHAR   User[48], Status[48], Location[48];
   ULONG  Speed, UpTime, ChangeTime;

   VOID   Activate (USHORT usTask);
   VOID   Deactivate (USHORT usTask = 0xFFFFU);
   USHORT First (VOID);
   USHORT Next (VOID);
   USHORT Read (USHORT usTask);
   VOID   Update (VOID);

private:
   USHORT CurrentTask;
   USHORT LastTask;
   CHAR   DataFile[128];
   CHSTAT Stat;
};

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

#define TE_NONE         0
#define TE_CALL         1
#define TE_DAY          2
#define TE_WEEK         3
#define TE_MONTH        4
#define TE_YEAR         5
#define TE_INACTIVITY   6

class DLL_EXPORT TBbs : public TCrc, public TTimer
{
public:
   TBbs (void);
   virtual ~TBbs (void);

   USHORT EndRun, Hangup;
   ULONG  StartCall, LastActivity;
   ULONG  CarrierSpeed;
   CHAR   Ansi, Avatar, Color, HotKey, Rip, More;
   CHAR   Remote, TimeWarning;
   USHORT ScreenHeight, ScreenWidth;

   class  TConfig   *Cfg;
   class  TCom      *Com;
   class  TLanguage *Lang;
   class  TLimits   *Limits;
   class  TLog      *Log;
   class  TCom      *Snoop;
   class  TUser     *User;
   class  TStatus   *Status;

   USHORT AbortSession (VOID);
   VOID   ClrEol (VOID);
   VOID   DisplayBanner (VOID);
   USHORT GetAnswer (USHORT flQuestion);
   SHORT  Getch (VOID);
   PSZ    GetString (PSZ pszBuffer, USHORT usMaxlen, USHORT flAttrib);
   SHORT  KBHit (VOID);
   USHORT LoginUser (PSZ pszName);
   SHORT  MoreQuestion (SHORT nLine);
   VOID   Printf (PSZ pszFormat, ...);
   VOID   PrintfAt (USHORT usRow, USHORT usColumn, PSZ pszFormat, ...);
   VOID   Putch (UCHAR ucByte);
   USHORT ReadFile (PSZ pszName, PSZ path = NULL);
   VOID   SetColor (USHORT usColor);
   LONG   TimeLeft (VOID);
   USHORT TimerExpired (VOID);
   USHORT VerifyAccount (VOID);

   virtual VOID ReleaseTimeSlice (VOID) = 0;
   virtual VOID Run (VOID) = 0;
};

// --------------------------------------------------------------------
// This class is used to read a file from disk and display it to the
// user. Every command embedded into the file is parsed and the
// informations/actions required are displayed/performed.
// The base class is used from the BBS class to perform the basic
// embedded commands actions, each section should derive a new class
// that can handle section-specific embedded commands.
// --------------------------------------------------------------------
class DLL_EXPORT TEmbedded
{
public:
   TEmbedded (class TBbs *pBbs);
   virtual ~TEmbedded (void);

   USHORT DisplayFile (PSZ pszFile);
   USHORT DisplayPrompt (PSZ pszString, UCHAR ucColor, UCHAR ucHilight);
   USHORT DisplayString (PSZ pszString);
   VOID   OutString (PSZ pszFormat, ...);

   virtual VOID   BasicControl (UCHAR ucControl);
   virtual SHORT  GetNextChar (VOID);
   virtual SHORT  PeekNextChar (VOID);
   virtual VOID   ProcessControl (UCHAR ucControl);
   virtual VOID   ProcessControlF (UCHAR ucControl);
   virtual VOID   ProcessControlW (UCHAR ucControl);

protected:
   class TBbs    *Bbs;
   class TCom    *Com;
   class TConfig *Cfg;
   class TCom    *Snoop;
   class TUser   *User;

private:
   FILE  *fp;
   SHORT LastChar, Stop, StopNested, Nested;
   CHAR  Temp[256], Response;
   PSZ   Position;
};

#endif

