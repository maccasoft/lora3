
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
#include "lora_api.h"

TOkFile::TOkFile (void)
{
   fdDat = -1;
   strcpy (DataFile, "okfile.dat");
}

TOkFile::TOkFile (PSZ pszDataPath)
{
   fdDat = -1;
   strcpy (DataFile, pszDataPath);
   strcat (DataFile, "okfile.dat");
   AdjustPath (strlwr (DataFile));
}

TOkFile::~TOkFile (void)
{
   if (fdDat != -1)
      close (fdDat);
}

VOID TOkFile::Add (VOID)
{
   USHORT DoClose = FALSE;
   OKFILE ok;

   if (fdDat == -1) {
      fdDat = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1) {
      memset (&ok, 0, sizeof (ok));
      ok.Size = sizeof (ok);
      strcpy (ok.Name, Name);
      strcpy (ok.Path, Path);
      strcpy (ok.Pwd, Pwd);
      ok.Normal = Normal;
      ok.Known = Known;
      ok.Protected = Protected;

      lseek (fdDat, 0L, SEEK_END);
      write (fdDat, &ok, sizeof (ok));
   }

   if (DoClose == TRUE && fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
}

VOID TOkFile::DeleteAll (VOID)
{
   if (fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }

   unlink (DataFile);
}

USHORT TOkFile::First (VOID)
{
   if (fdDat == -1)
      fdDat = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

   if (fdDat != -1)
      lseek (fdDat, 0L, SEEK_SET);

   return (Next ());
}

USHORT TOkFile::Next (VOID)
{
   USHORT RetVal = FALSE;
   OKFILE ok;

   if (fdDat != -1) {
      if (read (fdDat, &ok, sizeof (ok)) == sizeof (ok)) {
         strcpy (Name, ok.Name);
         strcpy (Path, ok.Path);
         strcpy (Pwd, ok.Pwd);
         Normal = ok.Normal;
         Known = ok.Known;
         Protected = ok.Protected;

         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT TOkFile::Read (PSZ pszName)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   OKFILE ok;

   if (fdDat == -1) {
      fdDat = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1) {
      lseek (fdDat, 0L, SEEK_SET);
      while (read (fdDat, &ok, sizeof (ok)) == sizeof (ok)) {
         if (!stricmp (ok.Name, pszName)) {
            strcpy (Name, ok.Name);
            strcpy (Path, ok.Path);
            strcpy (Pwd, ok.Pwd);
            Normal = ok.Normal;
            Known = ok.Known;
            Protected = ok.Protected;

            RetVal = TRUE;
            break;
         }
      }
   }

   if (DoClose == TRUE && fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }

   return (RetVal);
}

VOID TOkFile::Update (VOID)
{
   OKFILE ok;

   if (fdDat != -1) {
      lseek (fdDat, tell (fdDat) - sizeof (ok), SEEK_SET);
      ok.Size = sizeof (ok);
      strcpy (ok.Name, Name);
      strcpy (ok.Path, Path);
      strcpy (ok.Pwd, Pwd);
      ok.Normal = Normal;
      ok.Known = Known;
      ok.Protected = Protected;
      write (fdDat, &ok, sizeof (ok));
   }
}


