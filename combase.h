
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

#ifndef _COMBASE_H
#define _COMBASE_H

#if defined(__OS2__)
#include <types.h>
#include <netinet\in.h>
#include <sys\socket.h>
#include <netdb.h>
#include <sys\ioctl.h>
#elif defined(__NT__)
#elif defined(__LINUX__)
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#else
extern "C" {
#include <sys\types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <pctcp/types.h>
#include <pctcp/rwconf.h>
#include <pctcp/pctcp.h>
#include <pctcp/syscalls.h>
};
#endif

#if defined(__DOS__) || (defined(__BORLANDC__) && !defined(__OS2__))
#if defined(__386__)
#define DOS4G
#endif
#include "commlib.h"
#include "ibmkeys.h"
#endif

#include "cxl.h"

#if defined(__LINUX__)
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/kd.h>      /* RAW mode stuff, etc. */
#include <linux/keyboard.h>   /* mainly for NR_KEYS */
#include <linux/vt.h>       /* for VT stuff - nah, really? :) */
#endif

#define RSIZE        2048
#define TSIZE        512

class DLL_EXPORT TCom
{
public:
   TCom (void) {};
   virtual ~TCom (void) {};

   USHORT  EndRun;
   USHORT  RxBytes, TxBytes;

   virtual USHORT BytesReady (VOID) = 0;
   virtual VOID   BufferByte (UCHAR byte) = 0;
   virtual VOID   BufferBytes (UCHAR *bytes, USHORT len) = 0;
   virtual USHORT Carrier (VOID) = 0;
   virtual VOID   ClearOutbound (VOID) = 0;
   virtual VOID   ClearInbound (VOID) = 0;
   virtual UCHAR  ReadByte (VOID) = 0;
   virtual USHORT ReadBytes (UCHAR *bytes, USHORT len) = 0;
   virtual VOID   SendByte (UCHAR byte) = 0;
   virtual VOID   SendBytes (UCHAR *bytes, USHORT len) = 0;
   virtual VOID   UnbufferBytes (VOID) = 0;

   virtual VOID   SetName (PSZ name) = 0;
   virtual VOID   SetCity (PSZ name) = 0;
   virtual VOID   SetLevel (PSZ level) = 0;
   virtual VOID   SetTimeLeft (ULONG seconds) = 0;
   virtual VOID   SetTime (ULONG seconds) = 0;

protected:
   UCHAR  RxBuffer[RSIZE], TxBuffer[TSIZE];
   UCHAR  *NextByte;
};

#define DTR                1
#define RTS                2
#define CTS                16
#define DSR                32
#define RI                 64
#define DCD                128
#define DATA_READY         0x0100
#define TX_SHIFT_EMPTY     0x4000

class DLL_EXPORT TSerial : public TCom
{
public:
   TSerial (void);
   ~TSerial (void);

#if defined(__OS2__) || defined(__NT__) || defined(__LINUX__)
   CHAR   Device[32];
#elif defined(__DOS__)
   USHORT Com;
#endif
   ULONG  Speed;
   USHORT DataBits, StopBits;
   CHAR   Parity;
#if defined(__OS2__)
   HFILE  hFile;
#elif defined(__NT__)
   HANDLE hFile;
#elif defined(__LINUX__)
   int    hFile;
   struct termios tty;
#endif

   USHORT BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   USHORT Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   USHORT Initialize (VOID);
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   SetDTR (USHORT fStatus);
   VOID   SetRTS (USHORT fStatus);
   VOID   SetParameters (ULONG ulSpeed, USHORT nData, UCHAR nParity, USHORT nStop);
   VOID   UnbufferBytes (VOID);

   VOID   SetName (PSZ name);
   VOID   SetCity (PSZ name);
   VOID   SetLevel (PSZ level);
   VOID   SetTimeLeft (ULONG seconds);
   VOID   SetTime (ULONG seconds);

private:
#if defined(__DOS__)
   PORT   *hPort;
#elif defined(__LINUX__)
   struct termios new_termio;
   struct termios old_termio;
#endif
};

class DLL_EXPORT TScreen : public TCom
{
public:
   TScreen (void);
   ~TScreen (void);

   USHORT BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   USHORT Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   USHORT Initialize (VOID);
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);

   VOID   SetName (PSZ name);
   VOID   SetCity (PSZ name);
   VOID   SetLevel (PSZ level);
   VOID   SetTimeLeft (ULONG seconds);
   VOID   SetTime (ULONG seconds);

private:
   CXLWIN wh;
   USHORT Running;
   USHORT RxPosition, Counter;
   USHORT Attr, Count, Params[10];
   CHAR   Prec, Ansi;
};

class DLL_EXPORT TTcpip : public TCom
{
public:
   TTcpip (void);
   ~TTcpip (void);

   CHAR   ClientIP[16];
   CHAR   ClientName[128];
   CHAR   HostIP[16];
   ULONG  HostID;

   USHORT BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   USHORT Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   VOID   ClosePort (VOID);
   USHORT ConnectServer (PSZ pszServerName, USHORT usPort);
   USHORT Initialize (USHORT usPort, USHORT usSocket = 0, USHORT usProtocol = IPPROTO_TCP);
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);
   USHORT WaitClient (VOID);

   USHORT GetPacket (PVOID lpBuffer, USHORT usSize);
   USHORT PeekPacket (PVOID lpBuffer, USHORT usSize);
   USHORT SendPacket (PVOID lpBuffer, USHORT usSize);

   VOID   SetName (PSZ name);
   VOID   SetCity (PSZ name);
   VOID   SetLevel (PSZ level);
   VOID   SetTimeLeft (ULONG seconds);
   VOID   SetTime (ULONG seconds);

private:
   int    Sock, Accepted;
   int    LSock;
   USHORT fCarrierDown;
   USHORT RxPosition;
   struct sockaddr_in udp_client;

#if defined(__OS2__)
   friend VOID WaitThread (PVOID Args);
#endif
};

class DLL_EXPORT TStdio : public TCom
{
public:
   TStdio (void);
   ~TStdio (void);

   USHORT BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   USHORT Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   USHORT Initialize (VOID);
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);

   VOID   SetName (PSZ name);
   VOID   SetCity (PSZ name);
   VOID   SetLevel (PSZ level);
   VOID   SetTimeLeft (ULONG seconds);
   VOID   SetTime (ULONG seconds);

private:
   USHORT RxPosition;
#if defined(__LINUX__)
   int tty_fd;
   struct termios new_termio, old_termio;
#endif
};

#if defined(__OS2__) || defined(__NT__)
class DLL_EXPORT TPipe : public TCom
{
public:
   TPipe (void);
   ~TPipe (void);

   CHAR   Name[64], City[64], Level[64];
   ULONG  TimeLeft, Time;

   USHORT BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   USHORT Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   USHORT Initialize (PSZ pszPipeName, PSZ pszCtlName, USHORT usInstances);
   USHORT ConnectServer (PSZ pszPipeName, PSZ pszCtlName);
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);
   USHORT WaitClient (VOID);

   VOID   SetName (PSZ name);
   VOID   SetCity (PSZ name);
   VOID   SetLevel (PSZ level);
   VOID   SetTimeLeft (ULONG seconds);
   VOID   SetTime (ULONG seconds);

private:
   USHORT CtlConnect, PipeConnect;
#if defined(__OS2__)
   HFILE  hFile, hFileCtl;
#elif defined(__NT__)
   HANDLE hFile, hFileCtl;
#endif
};
#endif

#endif


