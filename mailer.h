
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.04
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _MAILER_H
#define _MAILER_H

#include "bbs.h"
#include "combase.h"
#include "ftrans.h"
#include "msgbase.h"
#include "msgdata.h"
#include "outbound.h"
#include "tools.h"
#include "user.h"

#define YOOHOO             0xF1
#define TSYNC              0xAE

#define isOPUS             0x05
#define isLORA             0x4E
#define isMAX_PRODUCT      0xE2

#define EMSI_NONE          0
#define EMSI_ACK           1
#define EMSI_NAK           2
#define EMSI_ICI           3
#define EMSI_INQ           4
#define EMSI_REQ           5
#define EMSI_DAT           6

class DLL_EXPORT TDetect : public TCrc, public TTimer
{
public:
   TDetect (class TBbs *bbs);
   ~TDetect (void);

   class  TAddress Address;

   VOID   DetectRemote (VOID);
   VOID   DetectTerminal (VOID);
   VOID   IEMSIReceiver (VOID);
   VOID   SelectLanguage (VOID);

private:
   class  TBbs    *Bbs;
   class  TConfig *Cfg;
   class  TCom    *Com;
   class  TLog    *Log;
   class  TUser   *User;

   CHAR   szReceiveIEMSI[512];
   CHAR   szSendIEMSI[512];

   USHORT CheckEMSIPacket (VOID);
   VOID   ParseIEMSIPacket (VOID);
   USHORT ReceiveIEMSIPacket (VOID);
   VOID   SendIEMSIPacket (VOID);
   SHORT  TimedRead (VOID);
};

class DLL_EXPORT TMailer : public TCrc, public TTimer
{
public:
   TMailer (class TBbs *lpBbs);
   ~TMailer (void);

   CHAR   Name[60];
   CHAR   Sysop[20];
   USHORT Zone, Net, Node, Point;
   class  TAddress Address;

   USHORT EMSIReceiver (VOID);
   USHORT EMSISender (VOID);
   VOID   Receiver (VOID);
   VOID   Sender (VOID);
   USHORT WaZOOReceiver (VOID);
   USHORT WaZOOSender (VOID);

private:
   USHORT Capabilities;
   CHAR   RemoteSystem[64];
   CHAR   RemoteSysop[32];
   CHAR   RemoteProgram[48];
   CHAR   ReceiveEMSI[1024];
   CHAR   SendEMSI[1024];
   CHAR   Inbound[64];

   class  TBbs    *Bbs;
   class  TConfig *Cfg;
   class  TCom    *Com;
   class  TLog    *Log;

   VOID   ParseEMSIPacket (VOID);
   USHORT ReceiveEMSIPacket (VOID);
   USHORT ReceiveHello (VOID);
   VOID   SendEMSIPacket (VOID);
   USHORT SendHello (VOID);
   SHORT  TimedRead (VOID);
};

typedef struct {
   USHORT OrigNode;
   USHORT DestNode;
   USHORT Year;
   USHORT Month;
   USHORT Day;
   USHORT Hour;
   USHORT Minute;
   USHORT Second;
   USHORT Rate;
   USHORT Version;
   USHORT OrigNet;
   USHORT DestNet;
   UCHAR  ProductL;
   UCHAR  Serial;
   CHAR   Password[8];
   USHORT OrigZone;
   USHORT DestZone;
   USHORT Auxnet;
   USHORT CWValidation;
   UCHAR  ProductH;
   UCHAR  Revision;
   USHORT Capability;
   USHORT OrigZone2;
   USHORT DestZone2;
   USHORT OrigPoint;
   USHORT DestPoint;
   UCHAR  Filler[4];
} PKT2HDR;

typedef struct {
   USHORT OrigNode;
   USHORT DestNode;
   USHORT OrigPoint;
   USHORT DestPoint;
   CHAR   Reserved[8];
   USHORT SubVersion;
   USHORT Version;
   USHORT OrigNet;
   USHORT DestNet;
   UCHAR  Product;
   UCHAR  Serial;
   CHAR   Password[8];
   USHORT OrigZone;
   USHORT DestZone;
   CHAR   OrigDomain[8];
   CHAR   DestDomain[8];
   UCHAR  Filler[4];
} PKT22HDR;

typedef struct {
   USHORT Version;
   USHORT OrigNode;
   USHORT DestNode;
   USHORT OrigNet;
   USHORT DestNet;
   USHORT Attrib;
   USHORT Cost;
} MSGHDR;

class DLL_EXPORT TImport
{
public:
   TImport (void);
   TImport (PSZ pszSourcePath);
   ~TImport (void);

   CHAR   PktName[32];
   CHAR   SourcePath[128];
   ULONG  MsgTossed, Duplicate, Bad, NetMail;
   USHORT HostZone, HostNet, HostNode, HostPoint;

   VOID   CloseFile (VOID);
   VOID   DeleteFile (VOID);
   USHORT Forward (class TAddress &Addr);
   USHORT OpenArea (PSZ pszEchoTag);
   USHORT OpenNextFile (VOID);
   USHORT ProcessFile (VOID);

private:
   FILE   *InFile;
   USHORT EchoZone, EchoNet, EchoNode, EchoPoint;
   USHORT DestZone, DestNet, DestNode, DestPoint;
   USHORT FromZone, FromPoint, ToZone, ToPoint;
   CHAR   Line[1024], LastRead;
   CHAR   From[64], To[64], Subject[72];
   CHAR   LastTag[64], OpenFileName[128];
   ULONG  Current;
   class  MsgBase *Msg;
   class  TCollection Text;
   class  TAddress Links;

   USHORT GetLine (VOID);
   VOID   ProcessPacket (VOID);
   USHORT ProcessType2Packet (PKT2HDR &Hdr);
   USHORT ProcessType22Packet (PKT22HDR &Hdr);
   USHORT ProcessType2plusPacket (PKT2HDR &Hdr);
};

#endif

