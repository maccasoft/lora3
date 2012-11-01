
// ----------------------------------------------------------------------
// UU-decode/encode Library
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    16/11/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _UULIB_H
#define _UULIB_H

#include "collect.h"

class DLL_EXPORT TUULib
{
public:
   TUULib (void);
   ~TUULib (void);

   USHORT MaxLines;
   USHORT Size;
   BYTE   Buffer[128];

   USHORT Decode (PSZ pszBuffer);
   USHORT Decode (class TCollection *Text);
   USHORT Decode (class TCollection &Text);
   USHORT DecodeFile (PSZ pszSource, PSZ pszDestination = NULL);
   VOID   Encode (BYTE *lpBuffer, USHORT usSize);
   USHORT EncodeFile (PSZ pszSource, PSZ pszDestination, PSZ pszRemote = NULL);

private:
};

#endif

