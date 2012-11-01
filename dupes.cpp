
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.11
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    12/08/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

TDupes::TDupes (void)
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

TDupes::~TDupes (void)
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

VOID TDupes::Delete (VOID)
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

   if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
      do {
         if (!strncmp (Text, "\001MSGID: ", 8) || !strncmp (Text, "\001Message-ID: ", 13)) {
            Crc = StringCrc32 (Text, 0xFFFFFFFFL);
            Found = TRUE;
            break;
         }
      } while ((Text = (CHAR *)Msg->Text.Next ()) != NULL);

   if (Found == FALSE) {
      Crc = StringCrc32 (Msg->From, 0xFFFFFFFFL);
      Crc = StringCrc32 (Msg->To, Crc);
      Crc = StringCrc32 (Msg->Subject, Crc);
   }

   return (Crc);
}

USHORT TDupes::Load (PSZ pszEchoTag)
{
   int fd;
   USHORT Found = FALSE;
   DUPEIDX idx;

   if ((fd = sopen (IndexFile, O_RDONLY|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &idx, sizeof (DUPEIDX)) == sizeof (DUPEIDX)) {
         if (!stricmp (pszEchoTag, idx.EchoTag)) {
            Found = TRUE;
            break;
         }
      }
      close (fd);
   }

   if (Found == TRUE) {
      if ((fd = sopen (DataFile, O_RDONLY|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         lseek (fd, idx.Position, SEEK_SET);
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

VOID TDupes::Save (VOID)
{
   int fd;
   USHORT Found = FALSE;
   DUPEIDX idx;

   if (dd.EchoTag[0] != '\0') {
      if ((fd = sopen (IndexFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         while (read (fd, &idx, sizeof (DUPEIDX)) == sizeof (DUPEIDX)) {
            if (!stricmp (idx.EchoTag, dd.EchoTag)) {
               Found = TRUE;
               break;
            }
         }
         close (fd);
      }

      if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         if (Found == TRUE)
            lseek (fd, idx.Position, SEEK_SET);
         else {
            lseek (fd, 0L, SEEK_END);
            memset (&idx, 0, sizeof (DUPEIDX));
            strcpy (idx.EchoTag, dd.EchoTag);
            idx.Position = tell (fd);
         }
         write (fd, &dd, sizeof (DUPEDATA));
         close (fd);

         if (Found == FALSE) {
            if ((fd = sopen (IndexFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
               lseek (fd, 0L, SEEK_END);
               write (fd, &idx, sizeof (DUPEIDX));
               close (fd);
            }
         }
      }
   }
}

