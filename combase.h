
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.10
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _COMBASE_H
#define _COMBASE_H

#if defined(__DOS__) || defined(__BORLANDC__)
#if defined(__386__)
#define DOS4G
#endif
#include "commlib.h"
#include "ibmkeys.h"
#endif

#if defined(__DOS__) || defined(__BORLANDC__) || defined(__OS2__)
#include "cxl.h"
#endif

#define RSIZE              2048
#define TSIZE              2048

class DLL_EXPORT TCom
{
public:
   TCom (void) {};
   virtual ~TCom (void) {};

   USHORT  EndRun;
   USHORT  RxBytes, TxBytes;
   CHAR    Device[32], Speed[16];
   USHORT  Dtr, Rts, Cts, Dsr, Dcd, Rxd, Txd, Ri;

   virtual SHORT  BytesReady (VOID) = 0;
   virtual VOID   BufferByte (UCHAR byte) = 0;
   virtual VOID   BufferBytes (UCHAR *bytes, USHORT len) = 0;
   virtual SHORT  Carrier (VOID) = 0;
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

#if defined(__OS2__) || defined(__NT__)
class DLL_EXPORT TPipe : public TCom
{
public:
   TPipe (void);
   ~TPipe (void);

   SHORT  BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   SHORT  Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   SHORT  Initialize (PSZ pszPipeName, USHORT usInstances);
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);

private:
#if defined(__OS2__)
   HFILE  hFile;
#elif defined(__NT__)
   HANDLE hFile;
#endif
};
#endif

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

#if defined(__OS2__)
   HFILE  hFile;
#elif defined(__NT__)
   HANDLE hFile;
#else
   PORT   *hPort;
#endif

   SHORT  BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   SHORT  Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
#if defined(__OS2__) || defined(__NT__)
   SHORT  Initialize (PSZ pszDevice, ULONG ulSpeed, UCHAR nData, UCHAR nParity, UCHAR nStop);
#else
   SHORT  Initialize (USHORT nPort, ULONG ulSpeed, UCHAR nData, UCHAR nParity, UCHAR nStop);
#endif
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   SetDTR (USHORT fStatus);
   VOID   SetRTS (USHORT fStatus);
   VOID   SetParameters (ULONG ulSpeed, UCHAR nData, UCHAR nParity, UCHAR nStop);
   VOID   UnbufferBytes (VOID);
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

   SHORT  BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   SHORT  Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   VOID   ClosePort (VOID);
   SHORT  ConnectServer (PSZ pszServerName, USHORT usPort);
   USHORT Initialize (USHORT usPort, USHORT usSocket = 0);
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);
   USHORT WaitClient (VOID);

private:
   int    Sock;
   int    LSock;
   USHORT fCarrierDown;
   USHORT RxPosition;
};

class DLL_EXPORT TScreen : public TCom
{
public:
   TScreen (void);
   ~TScreen (void);

   SHORT  BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   SHORT  Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   SHORT  Initialize (VOID);
   UCHAR  ReadByte (VOID);
   USHORT ReadBytes (UCHAR *bytes, USHORT len);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);

#if defined(__DOS__) || defined(__OS2__)
private:
   WINDOW wh;
   USHORT Attr, Count, Params[10];
   CHAR   Prec, Ansi;
#endif
};

#endif


