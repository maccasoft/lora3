
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/22/95 - Initial coding.
// ----------------------------------------------------------------------

#include "tools.h"

#ifndef _OUTBOUND_H
#define _OUTBOUND_H

typedef struct {
   USHORT Zone;
   USHORT Net;
   USHORT Node;
   USHORT Point;
   CHAR   Domain[32];
   CHAR   Name[32];           // File name
   CHAR   Complete[128];
   ULONG  Size;               // File size in bytes
   UCHAR  ArcMail;            // TRUE=File is a compressed mail packet
   UCHAR  MailPKT;            // TRUE=File is an uncompressed mail packet
   UCHAR  Request;            // TRUE=File is a files request
   UCHAR  Poll;               // TRUE=Only the poll flag is present
   UCHAR  DeleteAfter;        // TRUE=Should be deleted after sent
   UCHAR  TruncateAfter;      // TRUE=Should be truncated to 0 bytes after sent
   CHAR   Status;             // 'H'=Hold, 'C'=Crash, 'D'=Direct, 'F'=Normal
} OUTFILE;

typedef struct {
   USHORT Zone;
   USHORT Net;
   USHORT Node;
   USHORT Point;
   CHAR   Domain[32];
   ULONG  Size;               // Total files size in bytes
   USHORT Files;              // Total number of files
   UCHAR  ArcMail;            // TRUE=Node has compressed mail files
   UCHAR  MailPKT;            // TRUE=Node has uncompressed mail files
   UCHAR  Request;            // TRUE=Node has file requests pending
   UCHAR  Crash;              // TRUE=Node has priority flags CRASH
   UCHAR  Direct;             // TRUE=Node has priority flags DIRECT
   UCHAR  Hold;               // TRUE=Node has priority flags HOLD
   UCHAR  Immediate;          // TRUE=Node has priority flags IMMEDIATE
   UCHAR  Normal;             // TRUE=Node has priority flags NORMAL
   USHORT Attempts;           // Number of dialing attempts
   USHORT Failed;             // Number of calls failed (connection but no handshake)
} QUEUE;

class DLL_EXPORT TOutbound
{
public:
   TOutbound (PSZ pszPath);
   TOutbound (PSZ pszPath, USHORT usZone, USHORT usNet, USHORT usNode, USHORT usPoint = 0, PSZ pszDomain = "");
   ~TOutbound (void);

   USHORT Zone, Net, Node, Point;
   CHAR   Domain[32];
   CHAR   Name[32], Complete[128];
   ULONG  Size;
   UCHAR  ArcMail, MailPKT, Request, Poll;
   UCHAR  DeleteAfter, TruncateAfter;
   CHAR   Status;

   USHORT Number;
   CHAR   Address[64];
   UCHAR  Crash, Direct, Hold, Immediate, Normal;
   USHORT Attempts, Failed;

   USHORT DefaultZone;
   USHORT TotalFiles;
   ULONG  TotalSize;

   USHORT Add (VOID);
   USHORT Add (USHORT usZone, USHORT usNet, USHORT usNode, USHORT usPoint = 0, PSZ pszDomain = "");
   USHORT AddQueue (OUTFILE &Out);
   VOID   BuildQueue (PSZ pszPath);
   USHORT First (VOID);
   USHORT FirstNode (VOID);
   VOID   New (VOID);
   USHORT Next (VOID);
   USHORT NextNode (VOID);
   VOID   Remove (VOID);
   VOID   Update (VOID);

private:
   CHAR   Path[48], Outbound[48];
   class  TCollection Files;
   class  TCollection Nodes;
};

#endif



