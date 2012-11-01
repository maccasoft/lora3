
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

#include "_ldefs.h"
#include "combase.h"
#include "lora_api.h"
#include "lora.h"

class TFTP
{
public:
   TFTP ();
   ~TFTP ();

   CHAR   ClientIP[16];
   class  TConfig *Cfg;
   class  TTcpip *Tcp;
   class  TLog *Log;

   USHORT FileExist (PSZ pszFile);
   VOID   GetCommand ();
   USHORT Login ();
   VOID   ReadFile (PSZ pszCode, PSZ pszFile);
   VOID   Run ();

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
   TWeb ();
   ~TWeb ();

   class  TConfig *Cfg;
   class  TLog *Log;
   class  TTcpip *Tcp;

   VOID   GetRequest ();
   VOID   Run ();
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
   TPOP3 ();
   ~TPOP3 ();

   class  TConfig *Cfg;
   class  TTcpip *Tcp;
   class  TLog *Log;

   VOID   GetCommand ();
   VOID   Run ();

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
   TNNTP ();
   ~TNNTP ();

   class  TConfig *Cfg;
   class  TTcpip *Tcp;
   class  TLog *Log;

   VOID   GetCommand ();
   VOID   Run ();
   VOID   SendBody ();
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


