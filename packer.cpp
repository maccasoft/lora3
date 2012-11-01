
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/01/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "packer.h"
#include <errno.h>

TPacker::TPacker (void)
{
   fd = -1;
}

TPacker::TPacker (PSZ pszDataPath)
{
   fd = -1;
   strcpy (DataPath, pszDataPath);
   if (DataPath[0] && DataPath[strlen (DataPath) - 1] != '\\')
      strcat (DataPath, "\\");
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
      sprintf (Temp, "%sPacker.Dat", DataPath);
      fd = open (Temp, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   }

   if (fd != -1) {
      memset (&Pack, 0, sizeof (Pack));
      strcpy (Pack.Key, Key);
      strcpy (Pack.Display, Display);
      strcpy (Pack.PackCmd, PackCmd);
      strcpy (Pack.UnpackCmd, UnpackCmd);
      strcpy (Pack.Id, Id);
      Pack.Position = Position;

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
   int fd;
   USHORT RetVal = FALSE;
   UCHAR c, Buffer[16], c1, *a;
   PSZ p;

   if ((fd = open (pszArcName, O_RDONLY|O_BINARY)) != -1) {
      if (Position < 0)
         lseek (fd, filelength (fd) - Position, SEEK_SET);
      else
         lseek (fd, Position, SEEK_SET);

      read (fd, Buffer, sizeof (Buffer));
      close (fd);

      p = Id;
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
   }

   return (RetVal);
}

VOID TPacker::Clear (VOID)
{
   memset (Key, 0, sizeof (Key));
   memset (Display, 0, sizeof (Display));
   memset (PackCmd, 0, sizeof (PackCmd));
   memset (UnpackCmd, 0, sizeof (UnpackCmd));
   memset (Id, 0, sizeof (Id));
   Position = 0L;
}

USHORT TPacker::Delete (VOID)
{
   int fdn;
   USHORT RetVal = FALSE;
   CHAR Temp[64], New[64];
   ULONG Position;
   PACKER Pack;

   sprintf (New, "%sPacker.New", DataPath);
   fdn = open (New, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   if (fd != -1 && fdn != -1) {
      Position = tell (fd);
      lseek (fd, 0L, SEEK_SET);

      while (read (fd, &Pack, sizeof (Pack)) == sizeof (Pack)) {
         if (tell (fd) != Position)
            write (fdn, &Pack, sizeof (Pack));
      }

      close (fd);
      fd = -1;
      close (fdn);
      fdn = -1;

      sprintf (Temp, "%sPacker.Dat", DataPath);
      unlink (Temp);
      rename (New, Temp);

      RetVal = TRUE;
   }

   if (fdn != -1) {
      close (fdn);
      unlink (New);
   }

   return (RetVal);
}

SHORT TPacker::DoPack (PSZ pszArcName, PSZ pszFiles)
{
   SHORT RetVal = FALSE, i;
   CHAR *p, *a;
#if defined(__WATCOMC__)
   const CHAR *args[32];
#else
   CHAR *args[32];
#endif

   a = Command;
   p = PackCmd;
   do {
      if (*p == '%') {
         p++;
         if (*p == '1' || toupper (*p) == 'A') {
            if (pszArcName != NULL) {
               strcpy (a, pszArcName);
               a += strlen (pszArcName);
            }
         }
         else if (*p == '2' || toupper (*p) == 'F') {
            if (pszFiles != NULL) {
               strcpy (a, pszFiles);
               a += strlen (pszFiles);
            }
         }
         else
            *a++ = *p;
      }
      else
         *a++ = *p;
   } while (*p++ != '\0');

   if ((p = strtok (Command, " ")) != NULL) {
      i = 0;
      do {
         args[i++] = p;
      } while ((p = strtok (NULL, " ")) != NULL);
      args[i] = NULL;

      if ((RetVal = (SHORT)spawnvp (P_WAIT, args[0], args)) == -1) {
         switch (errno) {
            case E2BIG:
               strcpy (Error, "Arg list too long");
               break;
            case EINVAL:
               strcpy (Error, "Invalid argument");
               break;
            case ENOENT:
               strcpy (Error, "Path or file name not found");
               break;
            case ENOEXEC:
               strcpy (Error, "Exec format error");
               break;
            case ENOMEM:
               strcpy (Error, "Not enough core");
               break;
            default:
               strcpy (Error, "Unknown error");
               break;
         }
         RetVal = FALSE;
      }
      else
         RetVal = TRUE;
   }

   return (RetVal);
}

SHORT TPacker::DoUnpack (PSZ pszArcName, PSZ pszPath, PSZ pszFiles)
{
   SHORT RetVal = FALSE, i;
   CHAR *p, *a;
#if defined(__WATCOMC__)
   const CHAR *args[32];
#else
   CHAR *args[32];
#endif

   a = Command;
   p = UnpackCmd;
   do {
      if (*p == '%') {
         p++;
         if (*p == '1' || toupper (*p) == 'A') {
            if (pszFiles != NULL) {
               strcpy (a, pszArcName);
               a += strlen (pszArcName);
            }
         }
         else if (*p == '2' || toupper (*p) == 'P') {
            if (pszFiles != NULL) {
               strcpy (a, pszPath);
               a += strlen (pszPath);
            }
         }
         else if (*p == '3' || toupper (*p) == 'F') {
            if (pszFiles != NULL) {
               strcpy (a, pszFiles);
               a += strlen (pszFiles);
            }
         }
         else
            *a++ = *p;
      }
      else
         *a++ = *p;
   } while (*p++ != '\0');

   if ((p = strtok (Command, " ")) != NULL) {
      i = 0;
      do {
         args[i++] = p;
      } while ((p = strtok (NULL, " ")) != NULL);
      args[i] = NULL;

      if ((RetVal = (SHORT)spawnvp (P_WAIT, args[0], args)) == -1) {
         switch (errno) {
            case E2BIG:
               strcpy (Error, "Arg list too long");
               break;
            case EINVAL:
               strcpy (Error, "Invalid argument");
               break;
            case ENOENT:
               strcpy (Error, "Path or file name not found");
               break;
            case ENOEXEC:
               strcpy (Error, "Exec format error");
               break;
            case ENOMEM:
               strcpy (Error, "Not enough core");
               break;
            default:
               strcpy (Error, "Unknown error");
               break;
         }
         RetVal = FALSE;
      }
      else
         RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TPacker::First (VOID)
{
   CHAR Temp[64];

   if (fd == -1) {
      sprintf (Temp, "%sPacker.Dat", DataPath);
      fd = open (Temp, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   }

   if (fd != -1)
      lseek (fd, 0L, SEEK_SET);

   return (Next ());
}

USHORT TPacker::IsNext (VOID)
{
   USHORT RetVal = FALSE;

   if (fd != -1) {
      if (filelength (fd) - tell (fd) >= sizeof (PACKER))
         RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TPacker::IsPrevious (VOID)
{
   USHORT RetVal = FALSE;

   if (fd != -1) {
      if (tell (fd) >= sizeof (PACKER) * 2)
         RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TPacker::Next (VOID)
{
   USHORT RetVal = FALSE;
   PACKER Pack;

   if (fd != -1 && read (fd, &Pack, sizeof (Pack)) == sizeof (Pack)) {
      strcpy (Key, Pack.Key);
      strcpy (Display, Pack.Display);
      strcpy (PackCmd, Pack.PackCmd);
      strcpy (UnpackCmd, Pack.UnpackCmd);
      strcpy (Id, Pack.Id);
      Position = Pack.Position;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TPacker::Previous (VOID)
{
   USHORT RetVal = FALSE;
   PACKER Pack;

   if (fd != -1) {
      if (tell (fd) >= sizeof (Pack) * 2) {
         lseek (fd, tell (fd) - sizeof (Pack) * 2, SEEK_SET);
         if (read (fd, &Pack, sizeof (Pack)) == sizeof (Pack)) {
            strcpy (Key, Pack.Key);
            strcpy (Display, Pack.Display);
            strcpy (PackCmd, Pack.PackCmd);
            strcpy (UnpackCmd, Pack.UnpackCmd);
            strcpy (Id, Pack.Id);
            Position = Pack.Position;
            RetVal = TRUE;
         }
      }
   }

   return (RetVal);
}

USHORT TPacker::Read (PSZ pszKey)
{
   USHORT RetVal = FALSE;
   CHAR Temp[64];
   PACKER Pack;

   if (fd == -1) {
      sprintf (Temp, "%sPacker.Dat", DataPath);
      fd = open (Temp, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   }

   if (fd != -1) {
      lseek (fd, 0L, SEEK_SET);
      while (RetVal == FALSE && read (fd, &Pack, sizeof (Pack)) == sizeof (Pack)) {
         if (!stricmp (Pack.Key, pszKey)) {
            strcpy (Key, Pack.Key);
            strcpy (Display, Pack.Display);
            strcpy (PackCmd, Pack.PackCmd);
            strcpy (UnpackCmd, Pack.UnpackCmd);
            strcpy (Id, Pack.Id);
            Position = Pack.Position;
            RetVal = TRUE;
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

      if (lseek (fd, tell (fd) - sizeof (Pack), SEEK_SET) != -1L) {
         if (write (fd, &Pack, sizeof (Pack)) == sizeof (Pack))
            RetVal = TRUE;
      }
   }

   return (RetVal);
}



