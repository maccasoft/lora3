
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.03
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _INTERNET_H
#define _INTERNET_H

#include "bbs.h"
#include "combase.h"
#include "tools.h"

#define FTP_PORT           21
#define TELNET_PORT        23
#define FINGER_PORT        79
#define FTPDATA_PORT       2048
#define VMODEM_PORT        3141

class DLL_EXPORT TInternet
{
public:
   TInternet (class TBbs *bbs);
   ~TInternet (void);

   VOID   Finger (PSZ pszServer = NULL, USHORT usPort = FINGER_PORT);
   VOID   FTP (PSZ pszServer = NULL, USHORT usPort = FTP_PORT);
   VOID   Telnet (PSZ pszServer = NULL, USHORT usPort = TELNET_PORT);
   VOID   VModem (PSZ pszServer = NULL, USHORT usPort = VMODEM_PORT);

protected:
   USHORT Hash;
   USHORT Binary;
   USHORT DataPort;
   class  TTcpip    *Tcp;
   class  TTcpip    *Data;

   VOID   FTP_GET (PSZ pszFile, PSZ pszName, USHORT fHash, USHORT fBinary);
   VOID   FTP_MGET (PSZ pszFile);
   VOID   FTPsend (class TTcpip *Data, PSZ pszFile, USHORT fHash, USHORT fBinary);
   USHORT GetResponse (PSZ pszResponse, USHORT usMaxLen);

private:
   class  TBbs      *Bbs;
   class  TCom      *Com;
   class  TCom      *Snoop;
   class  TConfig   *Cfg;
   class  TLanguage *Lang;
   class  TLog      *Log;
   class  TUser     *User;
};

#endif

