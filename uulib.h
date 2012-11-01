
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
   USHORT Decode64 (PSZ pszBuffer);
   USHORT Decode (class TCollection *Text);
   USHORT Decode (class TCollection &Text);
   USHORT DecodeFile (PSZ pszSource, PSZ pszDestination = NULL);
   VOID   Encode (BYTE *lpBuffer, USHORT usSize);
   USHORT EncodeFile (PSZ pszSource, PSZ pszDestination, PSZ pszRemote = NULL);

private:
};

#endif

