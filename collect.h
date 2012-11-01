
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

#ifndef _TOOLS_H
#define _TOOLS_H

typedef struct _lData {
   struct _lData  *Previous;
   struct _lData  *Next;
   PVOID           Value;
   CHAR            Data[1];
} LDATA;

class DLL_EXPORT TCollection
{
public:
   TCollection (void);
   ~TCollection (void);

   USHORT Elements;

   USHORT Add (PVOID lpData);
   USHORT Add (PSZ lpData);
   USHORT Add (PVOID lpData, USHORT usSize);
   VOID   Clear (VOID);
   PVOID  First (VOID);
   USHORT Insert (PVOID lpData);
   USHORT Insert (PSZ lpData);
   USHORT Insert (PVOID lpData, USHORT usSize);
   PVOID  Last (VOID);
   PVOID  Next (VOID);
   PVOID  Previous (VOID);
   VOID   Remove (VOID);
   USHORT Replace (PVOID lpData);
   USHORT Replace (PSZ lpData);
   USHORT Replace (PVOID lpData, USHORT usSize);
   PVOID  Value (VOID);

private:
   LDATA *List;
};

#endif

