
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
#include <errno.h>

TPacker::TPacker (void)
{
   fd = -1;
#if defined(__LINUX__)
   strcpy (DataPath, "./");
#else
   strcpy (DataPath, ".\\");
#endif
}

TPacker::TPacker (PSZ pszDataPath)
{
   fd = -1;
   strcpy (DataPath, pszDataPath);
#if defined(__LINUX__)
   if (DataPath[0] && DataPath[strlen (DataPath) - 1] != '/')
      strcat (DataPath, "/");
#else
   if (DataPath[0] && DataPath[strlen (DataPath) - 1] != '\\')
      strcat (DataPath, "\\");
#endif
}

TPacker::~TPacker (void)
{
   if (fd != -1)
      close (fd);
}

USHORT TPacker::Add (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[64];
   PACKER Pack;

   if (fd == -1) {
      sprintf (Temp, "%spacker.dat", DataPath);
      fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fd != -1) {
      memset (&Pack, 0, sizeof (Pack));
      strcpy (Pack.Key, Key);
      strcpy (Pack.Display, Display);
      strcpy (Pack.PackCmd, PackCmd);
      strcpy (Pack.UnpackCmd, UnpackCmd);
      strcpy (Pack.Id, Id);
      Pack.Position = Position;
      if (Dos == TRUE)
         Pack.OS |= OS_DOS;
      if (OS2 == TRUE)
         Pack.OS |= OS_OS2;
      if (Windows == TRUE)
         Pack.OS |= OS_WINDOWS;
      if (Linux == TRUE)
         Pack.OS |= OS_LINUX;

      if (lseek (fd, 0L, SEEK_END) != -1L) {
         if (write (fd, &Pack, sizeof (Pack)) == sizeof (Pack))
            RetVal = TRUE;
      }

      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TPacker::CheckArc (PSZ pszArcName)
{
   int fdd;
   USHORT RetVal = FALSE;
   CHAR Temp[64];
   UCHAR c, Buffer[16], c1, *a;
   PSZ p;
   PACKER Pack;

   if (fd == -1) {
      sprintf (Temp, "%spacker.dat", DataPath);
      fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if ((fdd = sopen (pszArcName, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      lseek (fd, 0L, SEEK_SET);
      while (read (fd, &Pack, sizeof (Pack)) == sizeof (Pack)) {
#if defined(__OS2__)
         if (!(Pack.OS & OS_OS2))
            continue;
#elif defined(__NT__)
         if (!(Pack.OS & OS_WINDOWS))
            continue;
#elif defined(__LINUX__)
         if (!(Pack.OS & OS_LINUX))
            continue;
#else
         if (!(Pack.OS & OS_DOS))
            continue;
#endif

         if (Pack.Position < 0)
            lseek (fdd, filelength (fd) + Pack.Position, SEEK_SET);
         else
            lseek (fdd, Pack.Position, SEEK_SET);

         read (fdd, Buffer, sizeof (Buffer));

         p = Pack.Id;
         a = Buffer;
         RetVal = TRUE;

         while (*p != '\0' && RetVal == TRUE) {
            c = (UCHAR)(toupper (*p++) - '0');
            if (c > 9)
               c -= 7;
            if (*p != '\0') {
               c1 = (UCHAR)(toupper (*p++) - '0');
               if (c1 > 9)
                  c1 -= 7;
               c = (UCHAR)((c << 4) | c1);
            }
            if (*a++ != c)
               RetVal = FALSE;
         }

         if (RetVal == TRUE)
            break;
      }
      close (fdd);
   }

   if (RetVal == TRUE) {
      strcpy (Key, Pack.Key);
      strcpy (Display, Pack.Display);
      strcpy (PackCmd, Pack.PackCmd);
      strcpy (UnpackCmd, Pack.UnpackCmd);
      strcpy (Id, Pack.Id);
      Position = Pack.Position;
      Dos = (UCHAR)((Pack.OS & OS_DOS) ? TRUE : FALSE);
      OS2 = (UCHAR)((Pack.OS & OS_OS2) ? TRUE : FALSE);
      Windows = (UCHAR)((Pack.OS & OS_WINDOWS) ? TRUE : FALSE);
      Linux = (UCHAR)((Pack.OS & OS_LINUX) ? TRUE : FALSE);
   }

   return (RetVal);
}

USHORT TPacker::Delete (VOID)
{
   int fdn;
   USHORT RetVal = FALSE;
   CHAR Temp[64];
   ULONG Position;
   PACKER Pack;

   sprintf (Temp, "%spacker.new", DataPath);
   fdn = sopen (Temp, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fd != -1 && fdn != -1) {
      Position = tell (fd);
      lseek (fd, 0L, SEEK_SET);

      while (read (fd, &Pack, sizeof (Pack)) == sizeof (Pack)) {
         if (tell (fd) != Position)
            write (fdn, &Pack, sizeof (Pack));
      }

      lseek (fd, 0L, SEEK_SET);
      lseek (fdn, 0L, SEEK_SET);

      while (read (fdn, &Pack, sizeof (Pack)) == sizeof (Pack))
         write (fd, &Pack, sizeof (Pack));
      chsize (fd, tell (fd));

      if (tell (fd) > Position)
         lseek (fd, Position, SEEK_SET);
      if (Next () == FALSE) {
         if (Previous () == FALSE)
            New ();
      }

      RetVal = TRUE;
   }

   if (fdn != -1) {
      close (fdn);
      sprintf (Temp, "%spacker.new", DataPath);
      unlink (Temp);
   }

   return (RetVal);
}

USHORT TPacker::DoPack (PSZ pszArcName, PSZ pszFiles)
{
   USHORT RetVal = FALSE;

   strcpy (Command, PackCmd);
   if (pszArcName != NULL) {
      strsrep (Command, "%1", pszArcName);
      strsrep (Command, "%a", pszArcName);
   }
   if (pszFiles != NULL) {
      strsrep (Command, "%2", pszFiles);
      strsrep (Command, "%f", pszFiles);
   }

   RunExternal (Command);
   RetVal = TRUE;

   return (RetVal);
}

USHORT TPacker::DoUnpack (PSZ pszArcName, PSZ pszPath, PSZ pszFiles)
{
   USHORT RetVal = FALSE, AppendSlash = FALSE;
   CHAR CurrentDir[128];

   strcpy (Command, UnpackCmd);
   if (pszArcName != NULL) {
      strsrep (Command, "%1", pszArcName);
      strsrep (Command, "%a", pszArcName);
   }
   strsrep (Command, "%2", "");
   if (pszFiles != NULL) {
      strsrep (Command, "%3", pszFiles);
      strsrep (Command, "%f", pszFiles);
   }

   getcwd (CurrentDir, sizeof (CurrentDir) - 1);
   if (pszPath[strlen (pszPath) - 1] == '\\') {
      pszPath[strlen (pszPath) - 1] = '\0';
      AppendSlash = TRUE;
   }
   chdir (pszPath);

   RunExternal (Command);
   RetVal = TRUE;

   chdir (CurrentDir);
   if (AppendSlash == TRUE)
      strcat (pszPath, "\\");

   return (RetVal);
}

USHORT TPacker::First (USHORT checkOS)
{
   CHAR Temp[64];

   if (fd == -1) {
      sprintf (Temp, "%spacker.dat", DataPath);
      fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fd != -1)
      lseek (fd, 0L, SEEK_SET);

   return (Next (checkOS));
}

VOID TPacker::New (VOID)
{
   memset (Key, 0, sizeof (Key));
   memset (Display, 0, sizeof (Display));
   memset (PackCmd, 0, sizeof (PackCmd));
   memset (UnpackCmd, 0, sizeof (UnpackCmd));
   memset (Id, 0, sizeof (Id));
   Position = 0L;
   Dos = OS2 = Windows = Linux = FALSE;
}

USHORT TPacker::Next (USHORT checkOS)
{
   USHORT RetVal = FALSE;
   PACKER Pack;

   if (fd != -1) {
      while (read (fd, &Pack, sizeof (Pack)) == sizeof (Pack)) {
         if (checkOS == TRUE) {
#if defined(__OS2__)
            if (!(Pack.OS & OS_OS2))
               continue;
#elif defined(__NT__)
            if (!(Pack.OS & OS_WINDOWS))
               continue;
#elif defined(__LINUX__)
            if (!(Pack.OS & OS_LINUX))
               continue;
#else
            if (!(Pack.OS & OS_DOS))
               continue;
#endif
         }
         strcpy (Key, Pack.Key);
         strcpy (Display, Pack.Display);
         strcpy (PackCmd, Pack.PackCmd);
         strcpy (UnpackCmd, Pack.UnpackCmd);
         strcpy (Id, Pack.Id);
         Position = Pack.Position;
         Dos = (UCHAR)((Pack.OS & OS_DOS) ? TRUE : FALSE);
         OS2 = (UCHAR)((Pack.OS & OS_OS2) ? TRUE : FALSE);
         Windows = (UCHAR)((Pack.OS & OS_WINDOWS) ? TRUE : FALSE);
         Linux = (UCHAR)((Pack.OS & OS_LINUX) ? TRUE : FALSE);
         RetVal = TRUE;
         break;
      }
   }

   return (RetVal);
}

USHORT TPacker::Previous (USHORT checkOS)
{
   USHORT RetVal = FALSE;
   PACKER Pack;

   if (fd != -1) {
      while (tell (fd) >= sizeof (Pack) * 2) {
         lseek (fd, tell (fd) - sizeof (Pack) * 2, SEEK_SET);
         if (read (fd, &Pack, sizeof (Pack)) == sizeof (Pack)) {
            if (checkOS == TRUE) {
#if defined(__OS2__)
               if (!(Pack.OS & OS_OS2))
                  continue;
#elif defined(__NT__)
               if (!(Pack.OS & OS_WINDOWS))
                  continue;
#elif defined(__LINUX__)
               if (!(Pack.OS & OS_LINUX))
                  continue;
#else
               if (!(Pack.OS & OS_DOS))
                  continue;
#endif
            }
            strcpy (Key, Pack.Key);
            strcpy (Display, Pack.Display);
            strcpy (PackCmd, Pack.PackCmd);
            strcpy (UnpackCmd, Pack.UnpackCmd);
            strcpy (Id, Pack.Id);
            Position = Pack.Position;
            Dos = (UCHAR)((Pack.OS & OS_DOS) ? TRUE : FALSE);
            OS2 = (UCHAR)((Pack.OS & OS_OS2) ? TRUE : FALSE);
            Windows = (UCHAR)((Pack.OS & OS_WINDOWS) ? TRUE : FALSE);
            Linux = (UCHAR)((Pack.OS & OS_LINUX) ? TRUE : FALSE);
            RetVal = TRUE;
            break;
         }
      }
   }

   return (RetVal);
}

USHORT TPacker::Read (PSZ pszKey, USHORT checkOS)
{
   USHORT RetVal = FALSE;
   CHAR Temp[64];
   PACKER Pack;

   if (fd == -1) {
      sprintf (Temp, "%spacker.dat", DataPath);
      fd = sopen (Temp, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fd != -1) {
      lseek (fd, 0L, SEEK_SET);
      while (read (fd, &Pack, sizeof (Pack)) == sizeof (Pack)) {
         if (checkOS == TRUE) {
#if defined(__OS2__)
            if (!(Pack.OS & OS_OS2))
               continue;
#elif defined(__NT__)
            if (!(Pack.OS & OS_WINDOWS))
               continue;
#elif defined(__LINUX__)
            if (!(Pack.OS & OS_LINUX))
               continue;
#else
            if (!(Pack.OS & OS_DOS))
               continue;
#endif
         }
         if (!stricmp (Pack.Key, pszKey)) {
            strcpy (Key, Pack.Key);
            strcpy (Display, Pack.Display);
            strcpy (PackCmd, Pack.PackCmd);
            strcpy (UnpackCmd, Pack.UnpackCmd);
            strcpy (Id, Pack.Id);
            Position = Pack.Position;
            Dos = (UCHAR)((Pack.OS & OS_DOS) ? TRUE : FALSE);
            OS2 = (UCHAR)((Pack.OS & OS_OS2) ? TRUE : FALSE);
            Windows = (UCHAR)((Pack.OS & OS_WINDOWS) ? TRUE : FALSE);
            Linux = (UCHAR)((Pack.OS & OS_LINUX) ? TRUE : FALSE);
            RetVal = TRUE;
            break;
         }
      }
   }

   return (RetVal);
}

USHORT TPacker::Update (VOID)
{
   USHORT RetVal = FALSE;
   PACKER Pack;

   if (fd != -1 && tell (fd) >= sizeof (Pack)) {
      memset (&Pack, 0, sizeof (Pack));
      strcpy (Pack.Key, Key);
      strcpy (Pack.Display, Display);
      strcpy (Pack.PackCmd, PackCmd);
      strcpy (Pack.UnpackCmd, UnpackCmd);
      strcpy (Pack.Id, Id);
      Pack.Position = Position;
      if (Dos == TRUE)
         Pack.OS |= OS_DOS;
      if (OS2 == TRUE)
         Pack.OS |= OS_OS2;
      if (Windows == TRUE)
         Pack.OS |= OS_WINDOWS;
      if (Linux == TRUE)
         Pack.OS |= OS_LINUX;

      if (lseek (fd, tell (fd) - sizeof (Pack), SEEK_SET) != -1L) {
         if (write (fd, &Pack, sizeof (Pack)) == sizeof (Pack))
            RetVal = TRUE;
      }
   }

   return (RetVal);
}



