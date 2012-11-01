
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/01/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _PACKER_H
#define _PACKER_H

#include "tools.h"

typedef struct {
   CHAR   Key[16];
   CHAR   Display[32];
   CHAR   PackCmd[128];
   CHAR   UnpackCmd[128];
   CHAR   Id[32];
   LONG   Position;
} PACKER;

class DLL_EXPORT TPacker
{
public:
   TPacker (void);
   TPacker (PSZ pszDataPath);
   ~TPacker (void);

   CHAR   Key[16];
   CHAR   Display[32];
   CHAR   PackCmd[128];
   CHAR   UnpackCmd[128];
   CHAR   Error[32];
   CHAR   Id[32];
   LONG   Position;

   USHORT Add (VOID);
   USHORT CheckArc (PSZ pszArcName);
   VOID   Clear (VOID);
   USHORT Delete (VOID);
   SHORT  DoPack (PSZ pszArcName, PSZ pszFiles);
   SHORT  DoUnpack (PSZ pszArcName, PSZ pszPath, PSZ pszFiles = NULL);
   USHORT First (VOID);
   USHORT IsNext (VOID);
   USHORT IsPrevious (VOID);
   USHORT Next (VOID);
   USHORT Previous (VOID);
   USHORT Read (PSZ pszKey);
   USHORT Update (VOID);

private:
   int  fd;
   CHAR Command[256];
   CHAR DataPath[48];
};

#endif

