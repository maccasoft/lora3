
// --------------------------------------------------------------------
// LoraBBS Professional Edition - Version 0.21
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History list:
//    03/06/95 - Initial coding
// --------------------------------------------------------------------

#ifndef _EDITOR_H
#define _EDITOR_H

#include "bbs.h"
#include "tools.h"

class DLL_EXPORT TEditor
{
public:
   TEditor (class TBbs *bbs);
   virtual ~TEditor (void);

   class  TCollection Text;

   USHORT AppendText (VOID);
   VOID   ChangeText (VOID);
   VOID   Clear (VOID);
   VOID   DeleteLine (VOID);
   USHORT InputText (VOID);
   USHORT InsertLines (VOID);
   VOID   ListText (VOID);
   VOID   RetypeLine (VOID);

protected:
   CHAR   Wrap[128];
   class  TBbs      *Bbs;
   class  TLanguage *Lang;
   class  TUser     *User;

   PSZ    GetString (CHAR *pszBuffer, USHORT usMaxlen);
   PSZ    StringReplace (PSZ pszStr, PSZ pszSearch, PSZ pszReplace);
};

#endif

