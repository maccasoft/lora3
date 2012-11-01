
// ----------------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.8
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------------

#ifndef _COMBASE_H
#define _COMBASE_H

#if defined(__DOS__) || (defined(__BORLANDC__) && !defined(__OS2__))
#if defined(__386__)
#define DOS4G
#endif
#include "commlib.h"
#include "ibmkeys.h"
#endif

#if defined(__DOS__) || defined(__OS2__) || defined(__LINUX__)
#include "cxl.h"
#endif

#if defined(__LINUX__)
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/kd.h>      /* RAW mode stuff, etc. */
#include <linux/keyboard.h>   /* mainly for NR_KEYS */
#include <linux/vt.h>       /* for VT stuff - nah, really? :) */
#endif

#define RSIZE        2048
#define TSIZE        128

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

private:
#if defined(__DOS__)
   PORT   *hPort;
#elif defined(__LINUX__)
   struct termios new_termio;
   struct termios old_termio;
#endif
};

#if defined(__OS2__) || defined(__DOS__) || defined(__LINUX__)
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

private:
   CXLWIN wh;
   USHORT RxPosition;
   USHORT Attr, Count, Params[10];
   CHAR   Prec, Ansi;
};
#endif

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
   USHORT Initialize (USHORT usPort, USHORT usSocket = 0);
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);
   USHORT WaitClient (VOID);

private:
   int    Sock, Accepted;
   int    LSock;
   USHORT fCarrierDown;
   USHORT RxPosition;

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

#if defined(__NT__)
   HANDLE hClientR, hClientW;
#endif

   USHORT BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   USHORT Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
#if defined(__OS2__)
   USHORT Initialize (PSZ pszPipeName, USHORT usInstances);
   USHORT ConnectServer (PSZ pszPipeName);
#elif defined(__NT__)
   USHORT Initialize (VOID);
   USHORT ConnectServer (HANDLE hRead, HANDLE hWrite);
#endif
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);
   USHORT WaitClient (VOID);

private:
#if defined(__OS2__)
   HFILE  hFile;
#elif defined(__NT__)
   HANDLE hServerR, hServerW;
#endif
};
#endif

#endif


