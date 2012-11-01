
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



