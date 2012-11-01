
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _NODES_H
#define _NODES_H

#include "tools.h"

typedef struct {
   CHAR   Address[64];        // Node's address
   CHAR   SystemName[64];     // System name
   CHAR   SysopName[48];      // Sysop name
   CHAR   Location[48];       // Location of this node (city/state)
   ULONG  Speed;              // Maximum connection speed
   ULONG  MinSpeed;           // Minimum connection speed allowed
   CHAR   Phone[48];          // Phone number
   CHAR   Flags[48];          // Nodelist-type flags
   CHAR   DialCmd[32];        // Special dialing command for this node
   UCHAR  RemapMail;          // TRUE=Remap mail directed to SysopName
   CHAR   SessionPwd[32];     // Mail session password
   CHAR   AreaMgrPwd[32];     // AreaManager (Echomail and Tic) password
   CHAR   OutPktPwd[9];       // Outbound packet password
   CHAR   InPktPwd[9];        // Inbound packet password
   UCHAR  UsePkt22;           // Use mail packet type 2.2
} NODES;

class DLL_EXPORT TNodes
{
public:
   TNodes (void);
   TNodes (PSZ pszDataPath);
   ~TNodes (void);

   CHAR   Address[64];
   CHAR   SystemName[64], SysopName[48], Location[48];
   ULONG  Speed, MinSpeed;
   CHAR   Phone[48], Flags[48], DialCmd[32];
   UCHAR  RemapMail;
   CHAR   SessionPwd[32], AreaMgrPwd[32];
   CHAR   OutPktPwd[9], InPktPwd[9];
   UCHAR  UsePkt22;

   VOID   Add (VOID);
   VOID   Delete (VOID);
   USHORT First (VOID);
   USHORT Next (VOID);
   VOID   New (VOID);
   USHORT Previous (VOID);
   USHORT Read (USHORT usZone, USHORT usNet, USHORT usNode, USHORT usPoint, PSZ pszDomain = NULL);
   USHORT Read (class TAddress *Address);
   USHORT Read (class TAddress &Address);
   VOID   Update (VOID);

private:
   int    fd;
   CHAR   DataPath[64];
   class  TAddress Addr1, Addr2;
};

#endif

