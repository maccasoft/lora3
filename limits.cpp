
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

TLimits::TLimits ()
{
   fdDat = -1;
   New ();
   strcpy (DatFile, "limits.dat");
}

TLimits::TLimits (PSZ pszDataPath)
{
   fdDat = -1;
   New ();
   strcpy (DatFile, pszDataPath);
   if (DatFile[0] != '\0') {
#if defined(__LINUX__)
      if (DatFile[strlen (DatFile) - 1] != '/')
         strcat (DatFile, "/");
#else
      if (DatFile[strlen (DatFile) - 1] != '\\')
         strcat (DatFile, "\\");
#endif
   }
   strcat (DatFile, "limits.dat");
}

TLimits::~TLimits ()
{
   if (fdDat != -1)
      close (fdDat);
}

USHORT TLimits::Add ()
{
   USHORT retVal = FALSE, closeFile = FALSE;
   LIMITS Limits;

   if (fdDat == -1) {
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      closeFile = TRUE;
   }

   if (fdDat != -1) {
      lseek (fdDat, 0L, SEEK_END);

      memset (&Limits, 0, sizeof (Limits));
      Limits.Size = sizeof (Limits);
      strcpy (Limits.Key, Key);
      strcpy (Limits.Description, Description);
      Limits.Level = Level;
      Limits.Flags = Flags;
      Limits.DenyFlags = DenyFlags;
      Limits.CallTimeLimit = CallTimeLimit;
      Limits.DayTimeLimit = DayTimeLimit;
      Limits.DownloadLimit = DownloadLimit;
      Limits.DownloadAt2400 = DownloadAt2400;
      Limits.DownloadAt9600 = DownloadAt9600;
      Limits.DownloadAt14400 = DownloadAt14400;
      Limits.DownloadAt28800 = DownloadAt28800;
      Limits.DownloadAt33600 = DownloadAt33600;
      Limits.DownloadRatio = DownloadRatio;
      Limits.RatioStart = RatioStart;
      Limits.DownloadSpeed = DownloadSpeed;

      write (fdDat, &Limits, sizeof (Limits));
      strcpy (LastKey, Key);
      retVal = TRUE;
   }

   if (closeFile == TRUE && fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }

   return (retVal);
}

VOID TLimits::New ()
{
   Key[0] = '\0';
   Description[0] = '\0';
   Level = 0;
   Flags = 0L;
   DenyFlags = 0L;
   CallTimeLimit = 0;
   DayTimeLimit = 0;
   DownloadLimit = 0;
   DownloadAt2400 = 0;
   DownloadAt9600 = 0;
   DownloadAt14400 = 0;
   DownloadAt28800 = 0;
   DownloadAt33600 = 0;
   DownloadRatio = 0;
   RatioStart = 0;
   DownloadSpeed = 0;
}

VOID TLimits::Delete ()
{
   int fdNew;
   USHORT CloseFile = FALSE;
   LIMITS Limits;

   if (fdDat == -1) {
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      CloseFile = TRUE;
   }

   fdNew = sopen ("temp3.dat", O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdNew != -1) {
      lseek (fdDat, 0L, SEEK_SET);

      while (read (fdDat, &Limits, sizeof (Limits)) == sizeof (Limits)) {
         if (strcmp (Key, Limits.Key))
            write (fdNew, &Limits, sizeof (Limits));
      }

      lseek (fdDat, 0L, SEEK_SET);
      lseek (fdNew, 0L, SEEK_SET);

      while (read (fdNew, &Limits, sizeof (Limits)) == sizeof (Limits))
         write (fdDat, &Limits, sizeof (Limits));
      chsize (fdDat, tell (fdDat));
   }

   if (fdNew != -1) {
      close (fdNew);
      unlink ("temp3.dat");
   }

   if (CloseFile == TRUE && fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
}

USHORT TLimits::First ()
{
   USHORT retVal = FALSE;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1) {
      lseek (fdDat, 0L, SEEK_SET);
      retVal = Next ();
   }

   return (retVal);
}

USHORT TLimits::Next ()
{
   USHORT retVal = FALSE;
   LIMITS Limits;

   New ();

   if (fdDat != -1) {
      if (read (fdDat, &Limits, sizeof (Limits)) == sizeof (Limits)) {
         strcpy (Key, Limits.Key);
         strcpy (Description, Limits.Description);
         Level = Limits.Level;
         Flags = Limits.Flags;
         DenyFlags = Limits.DenyFlags;
         CallTimeLimit = Limits.CallTimeLimit;
         DayTimeLimit = Limits.DayTimeLimit;
         DownloadLimit = Limits.DownloadLimit;
         DownloadAt2400 = Limits.DownloadAt2400;
         DownloadAt9600 = Limits.DownloadAt9600;
         DownloadAt14400 = Limits.DownloadAt14400;
         DownloadAt28800 = Limits.DownloadAt28800;
         DownloadAt33600 = Limits.DownloadAt33600;
         DownloadRatio = Limits.DownloadRatio;
         RatioStart = Limits.RatioStart;
         DownloadSpeed = Limits.DownloadSpeed;
         strcpy (LastKey, Key);
         retVal = TRUE;
      }
   }

   return (retVal);
}

USHORT TLimits::Previous ()
{
   USHORT retVal = FALSE;
   LIMITS Limits;

   New ();

   if (fdDat != -1) {
      if (tell (fdDat) >= sizeof (Limits) * 2) {
         lseek (fdDat, tell (fdDat) - sizeof (Limits) * 2, SEEK_SET);
         read (fdDat, &Limits, sizeof (Limits));
         strcpy (Key, Limits.Key);
         strcpy (Description, Limits.Description);
         Level = Limits.Level;
         Flags = Limits.Flags;
         DenyFlags = Limits.DenyFlags;
         CallTimeLimit = Limits.CallTimeLimit;
         DayTimeLimit = Limits.DayTimeLimit;
         DownloadLimit = Limits.DownloadLimit;
         DownloadAt2400 = Limits.DownloadAt2400;
         DownloadAt9600 = Limits.DownloadAt9600;
         DownloadAt14400 = Limits.DownloadAt14400;
         DownloadAt28800 = Limits.DownloadAt28800;
         DownloadAt33600 = Limits.DownloadAt33600;
         DownloadRatio = Limits.DownloadRatio;
         RatioStart = Limits.RatioStart;
         DownloadSpeed = Limits.DownloadSpeed;
         strcpy (LastKey, Key);
         retVal = TRUE;
      }
   }

   return (retVal);
}

USHORT TLimits::Read (PSZ pszName, USHORT fCloseFile)
{
   USHORT retVal = FALSE;
   LIMITS Limits;

   New ();

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1) {
      lseek (fdDat, 0L, SEEK_SET);

      while (retVal == FALSE && read (fdDat, &Limits, sizeof (Limits)) == sizeof (Limits)) {
         if (!stricmp (pszName, Limits.Key)) {
            strcpy (Key, Limits.Key);
            strcpy (Description, Limits.Description);
            Level = Limits.Level;
            Flags = Limits.Flags;
            DenyFlags = Limits.DenyFlags;
            CallTimeLimit = Limits.CallTimeLimit;
            DayTimeLimit = Limits.DayTimeLimit;
            DownloadLimit = Limits.DownloadLimit;
            DownloadAt2400 = Limits.DownloadAt2400;
            DownloadAt9600 = Limits.DownloadAt9600;
            DownloadAt14400 = Limits.DownloadAt14400;
            DownloadAt28800 = Limits.DownloadAt28800;
            DownloadAt33600 = Limits.DownloadAt33600;
            DownloadRatio = Limits.DownloadRatio;
            RatioStart = Limits.RatioStart;
            DownloadSpeed = Limits.DownloadSpeed;
            strcpy (LastKey, Key);
            retVal = TRUE;
         }
      }
   }

   if (fCloseFile == TRUE && fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }

   return (retVal);
}

USHORT TLimits::Update ()
{
   USHORT retVal = FALSE, closeFile = FALSE;
   LIMITS Limits;

   if (fdDat == -1) {
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      closeFile = TRUE;
   }

   if (fdDat != -1) {
      lseek (fdDat, 0L, SEEK_SET);
      while (retVal == FALSE && read (fdDat, &Limits, sizeof (Limits)) == sizeof (Limits)) {
         if (!stricmp (LastKey, Limits.Key)) {
            lseek (fdDat, tell (fdDat) - sizeof (Limits), SEEK_SET);
            memset (&Limits, 0, sizeof (Limits));
            Limits.Size = sizeof (Limits);
            strcpy (Limits.Key, Key);
            strcpy (Limits.Description, Description);
            Limits.Level = Level;
            Limits.Flags = Flags;
            Limits.DenyFlags = DenyFlags;
            Limits.CallTimeLimit = CallTimeLimit;
            Limits.DayTimeLimit = DayTimeLimit;
            Limits.DownloadLimit = DownloadLimit;
            Limits.DownloadAt2400 = DownloadAt2400;
            Limits.DownloadAt9600 = DownloadAt9600;
            Limits.DownloadAt14400 = DownloadAt14400;
            Limits.DownloadAt28800 = DownloadAt28800;
            Limits.DownloadAt33600 = DownloadAt33600;
            Limits.DownloadRatio = DownloadRatio;
            Limits.RatioStart = RatioStart;
            Limits.DownloadSpeed = DownloadSpeed;
            write (fdDat, &Limits, sizeof (Limits));
            retVal = TRUE;
         }
      }
   }

   if (closeFile == TRUE && fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }

   return (retVal);
}

