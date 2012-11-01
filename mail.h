
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/22/95 - Initial coding.
// ----------------------------------------------------------------------

#include "collect.h"
#include "lora_api.h"
#include "msgbase.h"

#ifndef _MAILER_H
#define _MAILER_H

typedef struct {
   USHORT Zone;
   USHORT Net;
   USHORT Node;
   USHORT Point;
} KLUDGES;

class TKludges
{
public:
   TKludges (void);
   ~TKludges (void);

   USHORT Sort, KeepPoint;
   USHORT Zone, Net, Node, Point;
   CHAR   Address[32], ShortAddress[32];

   USHORT Add (VOID);
   USHORT AddString (PSZ pszString);
   USHORT Check (PSZ pszName);
   VOID   Clear (VOID);
   VOID   Delete (VOID);
   USHORT First (VOID);
   VOID   New (VOID);
   USHORT Next (VOID);

private:
   class  TCollection Data;
};

// ----------------------------------------------------------------------

class TMail
{
public:
   TMail (void);
   ~TMail (void);

   USHORT Packets;
   CHAR   Inbound[128];
   ULONG  MsgTossed, Duplicate, Bad, NetMail;
   ULONG  MsgSent;
   class  TConfig *Cfg;
   class  TLog *Log;

   VOID   Export (VOID);
   VOID   ExportNetMail (VOID);
   VOID   Import (VOID);
   USHORT IsArcmail (VOID);
   USHORT IsMail (VOID);
   USHORT UnpackArcmail (VOID);

private:
   USHORT IsAreasBBS;
   CHAR   LastTag[64], PktName[32];
   CHAR   Temp[128], Outbound[128];
   struct stat statbuf;
   class  TMsgBase *Msg;
   class  PACKET *Packet;
   class  TMsgData *Data;
   class  TEchoLink *Forward;
   class  TKludges *SeenBy, *Path;
   class  TNodes *Nodes;
   class  TDupes *Dupes;

   USHORT ExportEchoMail (ULONG Number, PSZ pszEchoTag);
   ULONG  ImportEchoMail (VOID);
   USHORT OpenNextPacket (VOID);
   USHORT OpenArea (PSZ pszEchoTag);
};

// --------------------------------------------------------------------------

class TRoute
{
public:
   TRoute (void);
   ~TRoute (void);

   class TConfig *Cfg;
   class TLog *Log;

   VOID  Run (PSZ pszFile = "route.cfg");

private:
   CHAR  Line[512], Temp[128], Name[128];
   CHAR  ArcMailName[128], Outbound[64];
   class TPacker *Packer;

   VOID  MakeArcMailName (PSZ pszAddress, CHAR Flag);
   VOID  RouteTo (VOID);
   VOID  SendTo (VOID);
};

// --------------------------------------------------------------------------

class TAreaMgr
{
public:
   TAreaMgr (void);
   ~TAreaMgr (void);

   class  TConfig *Cfg;
   class  TLog *Log;
   class  TMsgBase *Msg;

   VOID   MsgFooter (VOID);
   VOID   MsgHeader (VOID);
   VOID   Process (VOID);
   VOID   Rescan (PSZ pszEchoTag, PSZ pszAddress);
   USHORT SetPacker (PSZ Cmd);
   VOID   UpdateAreasBBS (VOID);

private:
   class  TNodes *Nodes;
   class  TEchoLink *EchoLink;
   class  TMsgData *Data;
   class  TCollection Text;
};

// --------------------------------------------------------------------------

class TTic
{
public:
   TTic (void);
   ~TTic (void);

   CHAR   Inbound[128];
   CHAR   Area[64], Name[32], Complete[128];
   CHAR   Password[32];
   ULONG  Size, Crc;
   class  TConfig *Cfg;
   class  TCollection *Description;
   class  TCollection *SeenBy, *Path;
   class  TAddress From;
   class  TAddress Origin;

   USHORT Check (VOID);
   VOID   Delete (VOID);
   VOID   Hatch (class TAddress *Dest);
   VOID   Hatch (class TAddress &Dest);
   USHORT Import (VOID);
   USHORT Open (PSZ pszFile);
   USHORT OpenNext (VOID);

private:
   CHAR   PktName[32];
   CHAR   CurrentFile[128];
};

#endif



