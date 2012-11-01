
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    13/06/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"
#include "lora_api.h"
#include "msgbase.h"

class TFTP
{
public:
   TFTP (void);
   ~TFTP (void);

   USHORT EndRun;
   CHAR   ClientIP[16];
   class  TConfig *Cfg;
   class  TTcpip *Tcp;
   class  TLog *Log;

   USHORT FileExist (PSZ pszFile);
   VOID   GetCommand (VOID);
   USHORT Login (VOID);
   VOID   ReadFile (PSZ pszCode, PSZ pszFile);
   VOID   Run (VOID);

private:
   USHORT Valid, Anonymous, Binary, DataPort;
   USHORT Known, Protected;
   PSZ    Month[12];
   CHAR   Response[128], Temp[4096];
   struct dosdate_t date;
   struct dostime_t d_time;
   class  TAddress  Addr;
   class  TTcpip    *Data;
   class  TUser     *User;
   class  TNodes    *Nodes;
   class  TFileData *Area;
   class  TFileBase *Files;
   class  TOutbound *Outbound;
};

// ----------------------------------------------------------------------

class TWeb
{
public:
   TWeb (void);
   ~TWeb (void);

   class  TConfig *Cfg;
   class  TLog *Log;
   class  TTcpip *Tcp;

   VOID   GetRequest (VOID);
   VOID   Run (VOID);
   VOID   SendFile (PSZ File, PSZ MimeType = NULL);
   VOID   URLNotFound (PSZ URL);

private:
   CHAR   Temp[2048];
   CHAR   Request[128];
   CHAR   Location[128];
};

// ----------------------------------------------------------------------

typedef struct {
   USHORT Index;
   ULONG  Number;
   USHORT Deleted;
   ULONG  Size;
} MAILINDEX;

class TPOP3
{
public:
   TPOP3 (void);
   ~TPOP3 (void);

   class  TConfig *Cfg;
   class  TTcpip *Tcp;
   class  TLog *Log;

   VOID   GetCommand (VOID);
   VOID   Run (VOID);

private:
   CHAR   Response[256], Temp[2048];
   class  TUser *User;
   class  TMsgBase *Msg;
   class  TCollection MailDrop;
   MAILINDEX Idx;
};

// ----------------------------------------------------------------------

class TNNTP
{
public:
   TNNTP (void);
   ~TNNTP (void);

   class  TConfig *Cfg;
   class  TTcpip *Tcp;
   class  TLog *Log;

   VOID   DisplayXOver (ULONG XFrom, ULONG XTo);
   VOID   GetCommand (VOID);
   VOID   Run (VOID);
   VOID   SendBody (VOID);
   VOID   SendHeader (USHORT Termination);

private:
   CHAR   Response[256], Temp[2048];
   class  TMsgBase *Msg;
};

// ----------------------------------------------------------------------

#if defined(__OS2__)

MRESULT EXPENTRY InternetDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

#elif defined(__NT__)

BOOL CALLBACK InternetDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif


