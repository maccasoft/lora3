
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
#include "msgbase.h"

TDupes::TDupes ()
{
   strcpy (DataFile, "dupes.dat");
   strcpy (IndexFile, "dupes.idx");
   dd.EchoTag[0] = '\0';
}

TDupes::TDupes (PSZ pszDataPath)
{
   strcpy (DataFile, pszDataPath);
   if (DataFile[0] != '\0') {
#if defined(__LINUX__)
      if (DataFile[strlen (DataFile) - 1] != '/')
         strcat (DataFile, "/");
#else
      if (DataFile[strlen (DataFile) - 1] != '\\')
         strcat (DataFile, "\\");
#endif
   }
   strcpy (IndexFile, DataFile);

   strcat (DataFile, "dupes.dat");
   strcat (IndexFile, "dupes.idx");
   dd.EchoTag[0] = '\0';
}

TDupes::~TDupes ()
{
}

VOID TDupes::Add (PSZ pszEchoTag, class TMsgBase *Msg)
{
   if (stricmp (dd.EchoTag, pszEchoTag))
      Load (pszEchoTag);

   dd.Dupes[dd.Position++] = GetEID (Msg);
   if (dd.Position >= MAX_DUPES)
      dd.Position = 0;
}

USHORT TDupes::Check (PSZ pszEchoTag, class TMsgBase *Msg)
{
   USHORT i, RetVal = FALSE;
   ULONG Crc;

   if (stricmp (dd.EchoTag, pszEchoTag))
      Load (pszEchoTag);

   Crc = GetEID (Msg);

   for (i = 0; i < MAX_DUPES; i++) {
      if (dd.Dupes[i] == Crc) {
         RetVal = TRUE;
         break;
      }
   }

   return (RetVal);
}

VOID TDupes::Delete ()
{
   int fd;
   CHAR EchoTag[64];
   ULONG WritePos, ReadPos;

   strcpy (EchoTag, dd.EchoTag);
   WritePos = ReadPos = 0L;

   if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &dd, sizeof (DUPEDATA)) == sizeof (DUPEDATA)) {
         if (stricmp (dd.EchoTag, EchoTag)) {
            lseek (fd, WritePos, SEEK_SET);
            write (fd, &dd, sizeof (DUPEDATA));
            WritePos += sizeof (DUPEDATA);
         }
         ReadPos += sizeof (DUPEDATA);
         lseek (fd, ReadPos, SEEK_SET);
      }
      close (fd);
   }

   dd.EchoTag[0] = '\0';
}

ULONG TDupes::GetEID (class TMsgBase *Msg)
{
   USHORT Found = FALSE;
   CHAR *Text;
   ULONG Crc = 0L;

   if ((Text = (CHAR *)Msg->Text.First ()) != NULL) {
      do {
         if (*Text != '\001' && strncmp (Text, "AREA:", 5))
            break;
         if (!strncmp (Text, "\001MSGID: ", 8) || !strncmp (Text, "\001Message-ID: ", 13)) {
            Crc = StringCrc32 (Text, 0xFFFFFFFFL);
            Found = TRUE;
            break;
         }
      } while ((Text = (CHAR *)Msg->Text.Next ()) != NULL);
   }

   if (Found == FALSE) {
      Crc = StringCrc32 (Msg->From, 0xFFFFFFFFL);
      Crc = StringCrc32 (Msg->To, Crc);
      Crc = StringCrc32 (Msg->Subject, Crc);
   }

   return (Crc);
}

#define MAX_INDEX    32

USHORT TDupes::Load (PSZ pszEchoTag)
{
   int fd, i, readed;
   USHORT Found = FALSE;
   DUPEIDX *idx;

   if ((idx = (DUPEIDX *)malloc (sizeof (DUPEIDX) * MAX_INDEX)) != NULL) {
      if ((fd = sopen (IndexFile, O_RDONLY|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         while ((readed = read (fd, idx, sizeof (DUPEIDX) * MAX_INDEX)) != 0) {
            readed /= sizeof (DUPEIDX);
            for (i = 0; i < readed; i++) {
               if (!stricmp (pszEchoTag, idx[i].EchoTag)) {
                  Found = TRUE;
                  break;
               }
            }
         }
         close (fd);
      }
      free (idx);
   }

   if (Found == TRUE) {
      if ((fd = sopen (DataFile, O_RDONLY|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         lseek (fd, idx[i].Position, SEEK_SET);
         read (fd, &dd, sizeof (DUPEDATA));
         close (fd);
      }
   }
   else {
      memset (&dd, 0, sizeof (DUPEDATA));
      strcpy (dd.EchoTag, pszEchoTag);
   }

   return (Found);
}

VOID TDupes::Save ()
{
   int fd, i, readed;
   USHORT Found = FALSE;
   DUPEIDX *idx, nidx;

   if (dd.EchoTag[0] != '\0') {
      if ((idx = (DUPEIDX *)malloc (sizeof (DUPEIDX) * MAX_INDEX)) != NULL) {
         if ((fd = sopen (IndexFile, O_RDONLY|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
            while ((readed = read (fd, idx, sizeof (DUPEIDX) * MAX_INDEX)) != 0) {
               readed /= sizeof (DUPEIDX);
               for (i = 0; i < readed; i++) {
                  if (!stricmp (dd.EchoTag, idx[i].EchoTag)) {
                     Found = TRUE;
                     break;
                  }
               }
            }
            close (fd);
         }
         free (idx);
      }

      if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         if (Found == TRUE)
            lseek (fd, idx[i].Position, SEEK_SET);
         else {
            lseek (fd, 0L, SEEK_END);
            memset (&nidx, 0, sizeof (DUPEIDX));
            strcpy (nidx.EchoTag, dd.EchoTag);
            nidx.Position = tell (fd);
         }
         write (fd, &dd, sizeof (DUPEDATA));
         close (fd);

         if (Found == FALSE) {
            if ((fd = sopen (IndexFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
               lseek (fd, 0L, SEEK_END);
               write (fd, &nidx, sizeof (DUPEIDX));
               close (fd);
            }
         }
      }
   }
}

