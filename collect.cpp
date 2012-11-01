
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

#include "_ldefs.h"
#include "collect.h"

TCollection::TCollection (void)
{
   List = NULL;
   Elements = 0;
}

TCollection::~TCollection (void)
{
   while (List != NULL)
      Remove ();
}

USHORT TCollection::Add (PVOID lpData)
{
   USHORT RetVal = 0;
   LDATA *New;

   if ((New = (LDATA *)malloc (sizeof (LDATA))) != NULL) {
      memset (New, 0, sizeof (LDATA));
      New->Value = (PVOID)lpData;

      if (List != NULL) {
         while (List->Next != NULL)
            List = List->Next;

         New->Previous = List;
         New->Next = List->Next;
         if (New->Next != NULL)
            New->Next->Previous = New;
         List->Next = New;
      }

      Elements++;
      List = New;
      RetVal = 1;
   }

   return (RetVal);
}

USHORT TCollection::Add (PSZ lpData)
{
   return (Add ((PVOID)lpData, (USHORT)(strlen (lpData) + 1)));
}

USHORT TCollection::Add (PVOID lpData, USHORT usSize)
{
   USHORT RetVal = 0;
   LDATA *New;

   if ((New = (LDATA *)malloc (sizeof (LDATA) + usSize)) != NULL) {
      memset (New, 0, sizeof (LDATA) + usSize);
      memcpy (New->Data, lpData, usSize);
      New->Value = (PVOID)New->Data;

      if (List != NULL) {
         while (List->Next != NULL)
            List = List->Next;

         New->Previous = List;
         New->Next = List->Next;
         if (New->Next != NULL)
            New->Next->Previous = New;
         List->Next = New;
      }

      Elements++;
      List = New;
      RetVal = 1;
   }

   return (RetVal);
}

VOID TCollection::Clear (VOID)
{
   while (List != NULL)
      Remove ();
   Elements = 0;
}

PVOID TCollection::First (VOID)
{
   PVOID RetVal = NULL;

   if (List != NULL) {
      while (List->Previous != NULL)
         List = List->Previous;
      RetVal = List->Value;
   }

   return (RetVal);
}

USHORT TCollection::Insert (PVOID lpData)
{
   USHORT RetVal = 0;
   LDATA *New;

   if ((New = (LDATA *)malloc (sizeof (LDATA))) != NULL) {
      memset (New, 0, sizeof (LDATA));
      New->Value = (PVOID)lpData;

      if (List != NULL) {
         New->Previous = List;
         New->Next = List->Next;
         if (New->Next != NULL)
            New->Next->Previous = New;
         List->Next = New;
      }

      Elements++;
      List = New;
      RetVal = 1;
   }

   return (RetVal);
}

USHORT TCollection::Insert (PSZ lpData)
{
   return (Insert (lpData, (USHORT)(strlen (lpData) + 1)));
}

USHORT TCollection::Insert (PVOID lpData, USHORT usSize)
{
   USHORT RetVal = 0;
   LDATA *New;

   if ((New = (LDATA *)malloc (sizeof (LDATA) + usSize)) != NULL) {
      memset (New, 0, sizeof (LDATA) + usSize);
      memcpy (New->Data, lpData, usSize);
      New->Value = (PVOID)New->Data;

      if (List != NULL) {
         New->Previous = List;
         New->Next = List->Next;
         if (New->Next != NULL)
            New->Next->Previous = New;
         List->Next = New;
      }

      Elements++;
      List = New;
      RetVal = 1;
   }

   return (RetVal);
}

PVOID TCollection::Last (VOID)
{
   PVOID RetVal = NULL;

   if (List != NULL) {
      while (List->Next != NULL)
         List = List->Next;
      RetVal = List->Value;
   }

   return (RetVal);
}

PVOID TCollection::Next (VOID)
{
   PVOID RetVal = NULL;

   if (List != NULL) {
      if (List->Next != NULL) {
         List = List->Next;
         RetVal = List->Value;
      }
   }

   return (RetVal);
}

PVOID TCollection::Previous (VOID)
{
   PVOID RetVal = NULL;

   if (List != NULL) {
      if (List->Previous != NULL) {
         List = List->Previous;
         RetVal = List->Value;
      }
   }

   return (RetVal);
}

VOID TCollection::Remove (VOID)
{
   LDATA *Temp;

   if (List != NULL) {
      if (List->Previous != NULL)
         List->Previous->Next = List->Next;
      if (List->Next != NULL)
         List->Next->Previous = List->Previous;
      Temp = List;
      if (List->Next != NULL)
         List = List->Next;
      else if (List->Previous != NULL)
         List = List->Previous;
      else
         List = NULL;
      free (Temp);
      Elements--;
   }
}

USHORT TCollection::Replace (PVOID lpData)
{
   USHORT RetVal = 0;
   LDATA *New;

   if (List != NULL) {
      if ((New = (LDATA *)malloc (sizeof (LDATA))) != NULL) {
         memset (New, 0, sizeof (LDATA));
         New->Value = (PVOID)lpData;
         New->Next = List->Next;
         New->Previous = List->Previous;

         if (New->Next != NULL)
            New->Next->Previous = New;
         if (New->Previous != NULL)
            New->Previous->Next = New;

         free (List);
         List = New;
         RetVal = 1;
      }
   }

   return (RetVal);
}

USHORT TCollection::Replace (PSZ lpData)
{
   return (Replace (lpData, (USHORT)(strlen (lpData) + 1)));
}

USHORT TCollection::Replace (PVOID lpData, USHORT usSize)
{
   USHORT RetVal = 0;
   LDATA *New;

   if (List != NULL) {
      if ((New = (LDATA *)malloc (sizeof (LDATA) + usSize)) != NULL) {
         memset (New, 0, sizeof (LDATA) + usSize);
         memcpy (New->Data, lpData, usSize);
         New->Value = (PVOID)New->Data;
         New->Next = List->Next;
         New->Previous = List->Previous;

         if (New->Next != NULL)
            New->Next->Previous = New;
         if (New->Previous != NULL)
            New->Previous->Next = New;

         free (List);
         List = New;
         RetVal = 1;
      }
   }

   return (RetVal);
}

PVOID TCollection::Value (VOID)
{
   return ((List == NULL) ? NULL : List->Value);
}

