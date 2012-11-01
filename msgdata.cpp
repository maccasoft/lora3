
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

TMsgData::TMsgData ()
{
   fdDat = -1;
   fdIdx = -1;
   strcpy (DataFile, "msg.dat");
   strcpy (IdxFile, "msg.idx");
   LastKey[0] = '\0';
   LastReaded = 0L;
}

TMsgData::TMsgData (PSZ pszDataPath)
{
   strcpy (DataFile, pszDataPath);
   strcpy (IdxFile, DataFile);

   fdDat = fdIdx = -1;
   strcat (DataFile, "msg.dat");
   strcat (IdxFile, "msg.idx");
   LastKey[0] = '\0';
   AdjustPath (strlwr (DataFile));
   AdjustPath (strlwr (IdxFile));
   LastReaded = 0L;
}

TMsgData::~TMsgData ()
{
   if (fdDat != -1)
      close (fdDat);
   if (fdIdx != -1)
      close (fdIdx);
}

USHORT TMsgData::Add ()
{
   USHORT retVal = FALSE, DoClose = FALSE;
   MESSAGE *Msg;
   INDEX Idx;

   if (fdIdx == -1) {
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }
   if (fdDat == -1) {
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdDat, 0L, SEEK_END);
      lseek (fdIdx, 0L, SEEK_END);

      if ((Msg = (MESSAGE *)malloc (sizeof (MESSAGE))) != NULL) {
         memset (Msg, 0, sizeof (MESSAGE));
         Msg->Size = sizeof (MESSAGE);
         Class2Struct (Msg);

         memset (&Idx, 0, sizeof (Idx));
         strcpy (Idx.Key, Key);
         Idx.Level = Level;
         Idx.AccessFlags = AccessFlags;
         Idx.DenyFlags = DenyFlags;
         Idx.Position = tell (fdDat);

         write (fdDat, Msg, sizeof (MESSAGE));
         write (fdIdx, &Idx, sizeof (Idx));
         retVal = TRUE;

         free (Msg);
      }
   }

   if (DoClose == TRUE) {
      if (fdDat != -1) {
         close (fdDat);
         fdDat = -1;
      }
      if (fdIdx != -1) {
         close (fdIdx);
         fdIdx = -1;
      }
   }

   return (retVal);
}

VOID TMsgData::Class2Struct (MESSAGE *Msg)
{
   strcpy (Msg->Display, Display);
   strcpy (Msg->Key, Key);
   Msg->Level = Level;
   Msg->AccessFlags = AccessFlags;
   Msg->DenyFlags = DenyFlags;
   Msg->WriteLevel = WriteLevel;
   Msg->WriteFlags = WriteFlags;
   Msg->DenyWriteFlags = DenyWriteFlags;
   Msg->Age = Age;
   Msg->Storage = Storage;
   strcpy (Msg->Path, Path);
   Msg->Board = Board;
   Msg->Flags = Flags;
   Msg->Group = Group;
   Msg->EchoMail = EchoMail;
   Msg->ShowGlobal = ShowGlobal;
   Msg->UpdateNews = UpdateNews;
   Msg->Offline = Offline;
   strcpy (Msg->MenuName, MenuName);
   strcpy (Msg->Moderator, Moderator);
   Msg->Cost = Cost;
   Msg->DaysOld = DaysOld;
   Msg->RecvDaysOld = RecvDaysOld;
   Msg->MaxMessages = MaxMessages;
   Msg->ActiveMsgs = ActiveMsgs;
   strcpy (Msg->NewsGroup, NewsGroup);
   Msg->Highest = Highest;
   Msg->HighWaterMark = HighWaterMark;
   strcpy (Msg->EchoTag, EchoTag);
   strcpy (Msg->Origin, Origin);
   strcpy (Msg->Address, Address);
   Msg->FirstMessage = FirstMessage;
   Msg->LastMessage = LastMessage;
   Msg->OriginIndex = OriginIndex;
   Msg->NewsHWM = NewsHWM;
}

VOID TMsgData::Delete ()
{
   int fdNew;
   ULONG Position;
   MESSAGE *Msg;
   INDEX Idx;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   fdNew = sopen ("Temp1.Dat", O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1 && fdNew != -1) {
      lseek (fdDat, 0L, SEEK_SET);

      if ((Msg = (MESSAGE *)malloc (sizeof (MESSAGE))) != NULL) {
         while (read (fdDat, Msg, sizeof (MESSAGE)) == sizeof (MESSAGE)) {
            if (strcmp (LastKey, Msg->Key))
               write (fdNew, Msg, sizeof (MESSAGE));
         }

         if ((Position = tell (fdIdx)) > 0L)
            Position -= sizeof (Idx);

         close (fdDat);
         close (fdIdx);
         fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);
         fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);

         if (fdDat != -1 && fdIdx != -1) {
            lseek (fdNew, 0L, SEEK_SET);

            while (read (fdNew, Msg, sizeof (MESSAGE)) == sizeof (MESSAGE)) {
               memset (&Idx, 0, sizeof (Idx));
               strcpy (Idx.Key, Msg->Key);
               Idx.Level = Msg->Level;
               Idx.AccessFlags = Msg->AccessFlags;
               Idx.DenyFlags = Msg->DenyFlags;
               Idx.Position = tell (fdDat);

               write (fdIdx, &Idx, sizeof (Idx));
               write (fdDat, Msg, sizeof (MESSAGE));
            }

            lseek (fdIdx, Position, SEEK_SET);
            if (Next () == FALSE) {
               if (Previous () == FALSE)
                  New ();
            }
         }

         free (Msg);
      }
   }

   if (fdNew != -1) {
      close (fdNew);
      unlink ("Temp1.Dat");
   }
}

USHORT TMsgData::First ()
{
   USHORT retVal = FALSE;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);
      retVal = Next ();
   }

   return (retVal);
}

USHORT TMsgData::Insert (class TMsgData *Data)
{
   PSZ p;

   strcpy (Display, Data->Display);
   strcpy (Key, Data->Key);
   Level = Data->Level;
   AccessFlags = Data->AccessFlags;
   DenyFlags = Data->DenyFlags;
   WriteLevel = Data->WriteLevel;
   WriteFlags = Data->WriteFlags;
   DenyWriteFlags = Data->DenyWriteFlags;
   Age = Data->Age;
   Storage = Data->Storage;

   strcpy (Path, Data->Path);
   if (Data->Storage == ST_FIDO || Data->Storage == ST_HUDSON) {
      if (Path[strlen (Path) - 1] != '\\' && Path[strlen (Path) - 1] != '/')
         strcat (Path, "\\");
   }
   else {
      if (Path[strlen (Path) - 1] == '\\' || Path[strlen (Path) - 1] == '/')
         Path[strlen (Path) - 1] = '\0';
   }
#if defined(__LINUX__)
   while ((p = strchr (Path, '\\')) != NULL)
      *p = '/';
#else
   while ((p = strchr (Path, '/')) != NULL)
      *p = '\\';
#endif

   Board = Data->Board;
   Flags = Data->Flags;
   Group = Data->Group;
   EchoMail = Data->EchoMail;
   ShowGlobal = Data->ShowGlobal;
   UpdateNews = Data->UpdateNews;
   Offline = Data->Offline;
   strcpy (MenuName, Data->MenuName);
   strcpy (Moderator, Data->Moderator);
   Cost = Data->Cost;
   DaysOld = Data->DaysOld;
   RecvDaysOld = Data->RecvDaysOld;
   MaxMessages = Data->MaxMessages;
   ActiveMsgs = Data->ActiveMsgs;
   strcpy (NewsGroup, Data->NewsGroup);
   Highest = Data->Highest;
   HighWaterMark = Data->HighWaterMark;
   strcpy (EchoTag, Data->EchoTag);
   strcpy (Origin, Data->Origin);
   strcpy (Address, Data->Address);
   FirstMessage = Data->FirstMessage;
   LastMessage = Data->LastMessage;
   OriginIndex = Data->OriginIndex;
   NewsHWM = Data->NewsHWM;

   return (Insert ());
}

USHORT TMsgData::Insert ()
{
   int fdNew;
   USHORT retVal = FALSE;
   ULONG Position;
   MESSAGE *Msg;
   INDEX Idx;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   fdNew = sopen ("Temp1.Dat", O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1 && fdNew != -1) {
      lseek (fdDat, 0L, SEEK_SET);

      if ((Msg = (MESSAGE *)malloc (sizeof (MESSAGE))) != NULL) {
         while (read (fdDat, Msg, sizeof (MESSAGE)) == sizeof (MESSAGE)) {
            write (fdNew, Msg, sizeof (MESSAGE));
            if (!strcmp (LastKey, Msg->Key)) {
               memset (Msg, 0, sizeof (MESSAGE));
               Msg->Size = sizeof (MESSAGE);
               Class2Struct (Msg);
               write (fdNew, Msg, sizeof (MESSAGE));
            }
         }

         lseek (fdDat, 0L, SEEK_SET);
         lseek (fdNew, 0L, SEEK_SET);

         Position = tell (fdIdx);
         lseek (fdIdx, 0L, SEEK_SET);

         while (read (fdNew, Msg, sizeof (MESSAGE)) == sizeof (MESSAGE)) {
            memset (&Idx, 0, sizeof (Idx));
            strcpy (Idx.Key, Msg->Key);
            Idx.Level = Msg->Level;
            Idx.AccessFlags = Msg->AccessFlags;
            Idx.DenyFlags = Msg->DenyFlags;
            Idx.Position = tell (fdDat);

            write (fdIdx, &Idx, sizeof (Idx));
            write (fdDat, Msg, sizeof (MESSAGE));
         }

         lseek (fdIdx, Position, SEEK_SET);
         Next ();

         retVal = TRUE;
         free (Msg);
      }
   }

   if (fdNew != -1) {
      close (fdNew);
      unlink ("Temp1.Dat");
   }

   return (retVal);
}

USHORT TMsgData::Last ()
{
   USHORT retVal = FALSE, IsValid;
   MESSAGE *Msg;
   INDEX Idx;

   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_END);
      if (tell (fdIdx) >= sizeof (INDEX)) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((Msg = (MESSAGE *)malloc (sizeof (MESSAGE))) != NULL) {
               if (read (fdDat, Msg, sizeof (MESSAGE)) == sizeof (MESSAGE)) {
                  retVal = TRUE;
                  New ();
                  Struct2Class (Msg);
                  IsValid = TRUE;
               }

               free (Msg);
            }
         }
      }

      while (IsValid == FALSE && tell (fdIdx) >= sizeof (INDEX) * 2) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX) * 2, SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((Msg = (MESSAGE *)malloc (sizeof (MESSAGE))) != NULL) {
               if (read (fdDat, Msg, sizeof (MESSAGE)) == sizeof (MESSAGE)) {
                  retVal = TRUE;
                  New ();
                  Struct2Class (Msg);
                  IsValid = TRUE;
               }

               free (Msg);
            }
         }
      }
   }

   return (retVal);
}

VOID TMsgData::New ()
{
   memset (Display, 0, sizeof (Display));
   memset (Key, 0, sizeof (Key));
   Level = 0;
   AccessFlags = 0L;
   DenyFlags = 0L;
   WriteLevel = 0;
   WriteFlags = 0L;
   DenyWriteFlags = 0L;
   Age = 0;
   Storage = 0;
   memset (Path, 0, sizeof (Path));
   Board = 0;
   Flags = 0;
   Group = 0;
   EchoMail = FALSE;
   ShowGlobal = TRUE;
   UpdateNews = FALSE;
   Offline = TRUE;
   memset (MenuName, 0, sizeof (MenuName));
   memset (Moderator, 0, sizeof (Moderator));
   Cost = 0;
   DaysOld = 0;
   RecvDaysOld = 0;
   MaxMessages = 0;
   FirstMessage = LastMessage = ActiveMsgs = 0L;
   memset (NewsGroup, 0, sizeof (NewsGroup));
   Highest = 0L;
   memset (EchoTag, 0, sizeof (EchoTag));
   memset (Origin, 0, sizeof (Origin));
   HighWaterMark = 0L;
   memset (Address, 0, sizeof (Address));
   NewsHWM = 0L;
}

USHORT TMsgData::Next ()
{
   USHORT retVal = FALSE, IsValid;
   MESSAGE *Msg;
   INDEX Idx;

   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED)) {
            if (tell (fdDat) != Idx.Position)
               lseek (fdDat, Idx.Position, SEEK_SET);
            if ((Msg = (MESSAGE *)malloc (sizeof (MESSAGE))) != NULL) {
               if (read (fdDat, Msg, sizeof (MESSAGE)) == sizeof (MESSAGE)) {
                  retVal = TRUE;
                  New ();
                  Struct2Class (Msg);
                  IsValid = TRUE;
               }

               free (Msg);
            }
         }
      }
   }

   return (retVal);
}

VOID TMsgData::Pack ()
{
   int fdNewIdx, fdNewDat;
   INDEX Idx;
   MESSAGE *Msg;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdIdx != -1 && fdDat != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      if ((Msg = (MESSAGE *)malloc (sizeof (MESSAGE))) != NULL) {
         fdNewIdx = open ("Msg-New.Idx", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
         fdNewDat = open ("Msg-New.Dat", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

         if (fdNewIdx != -1 && fdNewDat != -1) {
            while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
               if (!(Idx.Flags & IDX_DELETED)) {
                  lseek (fdDat, Idx.Position, SEEK_SET);
                  read (fdDat, Msg, sizeof (MESSAGE));
                  Idx.Position = tell (fdNewDat);
                  write (fdNewDat, Msg, sizeof (MESSAGE));
                  write (fdNewIdx, &Idx, sizeof (Idx));
               }
            }

            close (fdNewIdx);
            close (fdNewDat);

            unlink (DataFile);
            rename ("Msg-New.Dat", DataFile);
            unlink (IdxFile);
            rename ("Msg-New.Idx", IdxFile);
         }

         unlink ("Msg-New.Dat");
         unlink ("Msg-New.Idx");
         free (Msg);
      }

      close (fdIdx);
      close (fdDat);
      fdIdx = fdDat = -1;
   }
}

USHORT TMsgData::Previous ()
{
   USHORT retVal = FALSE, IsValid;
   MESSAGE *Msg;
   INDEX Idx;

   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && tell (fdIdx) >= sizeof (INDEX) * 2) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX) * 2, SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((Msg = (MESSAGE *)malloc (sizeof (MESSAGE))) != NULL) {
               if (read (fdDat, Msg, sizeof (MESSAGE)) == sizeof (MESSAGE)) {
                  retVal = TRUE;
                  New ();
                  Struct2Class (Msg);
                  IsValid = TRUE;
               }

               free (Msg);
            }
         }
      }
   }

   return (retVal);
}

USHORT TMsgData::Read (PSZ pszName, USHORT fCloseFile)
{
   USHORT retVal = FALSE;
   MESSAGE *Msg;
   INDEX Idx;

   New ();

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);

      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && !stricmp (pszName, Idx.Key)) {
            if ((Msg = (MESSAGE *)malloc (sizeof (MESSAGE))) != NULL) {
               lseek (fdDat, Idx.Position, SEEK_SET);
               read (fdDat, Msg, sizeof (MESSAGE));

               retVal = TRUE;
               Struct2Class (Msg);

               free (Msg);
            }
         }
      }
   }

   if (fCloseFile == TRUE) {
      if (fdDat != -1) {
         close (fdDat);
         fdDat = -1;
      }
      if (fdIdx != -1) {
         close (fdIdx);
         fdIdx = -1;
      }
   }

   return (retVal);
}

USHORT TMsgData::ReadEcho (PSZ pszEchoTag)
{
   FILE *fp;
   USHORT retVal = FALSE;
   MESSAGE *Msg;

   New ();

   fp = fopen (DataFile, "r+b");
   Msg = (MESSAGE *)malloc (sizeof (MESSAGE));

   if (fp != NULL && Msg != NULL) {
      setvbuf (fp, NULL, _IOFBF, 2048);
      while (fread (Msg, sizeof (MESSAGE), 1, fp) == 1) {
         if (!stricmp (pszEchoTag, Msg->EchoTag)) {
            retVal = TRUE;
            Struct2Class (Msg);
            break;
         }
      }
   }

   if (Msg != NULL)
      free (Msg);
   if (fp != NULL)
      fclose (fp);

   return (retVal);
}

USHORT TMsgData::ReRead ()
{
   USHORT retVal = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      if (tell (fdIdx) >= sizeof (INDEX)) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         retVal = Next ();
      }
   }

   return (retVal);
}

VOID TMsgData::Struct2Class (MESSAGE *Msg)
{
   PSZ p;

   strcpy (Display, Msg->Display);
   strcpy (Key, Msg->Key);
   Level = Msg->Level;
   AccessFlags = Msg->AccessFlags;
   DenyFlags = Msg->DenyFlags;
   WriteLevel = Msg->WriteLevel;
   WriteFlags = Msg->WriteFlags;
   DenyWriteFlags = Msg->DenyWriteFlags;
   Age = Msg->Age;
   Storage = Msg->Storage;

   strcpy (Path, Msg->Path);
   if (Msg->Storage == ST_FIDO || Msg->Storage == ST_HUDSON) {
      if (Path[strlen (Path) - 1] != '\\' && Path[strlen (Path) - 1] != '/')
         strcat (Path, "\\");
   }
   else {
      if (Path[strlen (Path) - 1] == '\\' || Path[strlen (Path) - 1] == '/')
         Path[strlen (Path) - 1] = '\0';
   }
#if defined(__LINUX__)
   while ((p = strchr (Path, '\\')) != NULL)
      *p = '/';
#else
   while ((p = strchr (Path, '/')) != NULL)
      *p = '\\';
#endif

   Board = Msg->Board;
   Flags = Msg->Flags;
   Group = Msg->Group;
   EchoMail = Msg->EchoMail;
   ShowGlobal = Msg->ShowGlobal;
   UpdateNews = Msg->UpdateNews;
   Offline = Msg->Offline;
   strcpy (MenuName, Msg->MenuName);
   strcpy (Moderator, Msg->Moderator);
   Cost = Msg->Cost;
   DaysOld = Msg->DaysOld;
   RecvDaysOld = Msg->RecvDaysOld;
   MaxMessages = Msg->MaxMessages;
   ActiveMsgs = Msg->ActiveMsgs;
   strcpy (NewsGroup, Msg->NewsGroup);
   Highest = Msg->Highest;
   HighWaterMark = Msg->HighWaterMark;
   strcpy (EchoTag, Msg->EchoTag);
   strcpy (Origin, Msg->Origin);
   strcpy (Address, Msg->Address);
   FirstMessage = Msg->FirstMessage;
   LastMessage = Msg->LastMessage;
   OriginIndex = Msg->OriginIndex;
   NewsHWM = Msg->NewsHWM;

   strcpy (LastKey, Msg->Key);
}

USHORT TMsgData::Update (PSZ pszNewKey)
{
   USHORT retVal = FALSE, Size, DoClose = FALSE;
   MESSAGE *Msg;
   INDEX Idx;

   if (fdIdx == -1) {
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }
   if (fdDat == -1) {
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1 && fdIdx != -1) {
      if (DoClose == TRUE) {
         lseek (fdIdx, 0L, SEEK_SET);
         while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
            if (!(Idx.Flags & IDX_DELETED) && !stricmp (Key, Idx.Key)) {
               retVal = TRUE;
               break;
            }
         }
      }
      else {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));

         if (strcmp (Idx.Key, Key)) {
            lseek (fdIdx, 0L, SEEK_SET);
            while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
               if (!(Idx.Flags & IDX_DELETED) && !stricmp (Key, Idx.Key)) {
                  retVal = TRUE;
                  break;
               }
            }
         }
         else
            retVal = TRUE;
      }

      if (retVal == TRUE && tell (fdIdx) >= sizeof (INDEX)) {
         retVal = FALSE;
         Size = sizeof (MESSAGE);

         if ((Msg = (MESSAGE *)malloc (Size)) != NULL) {
            memset (Msg, 0, Size);
            Msg->Size = Size;
            if (pszNewKey != NULL)
               strcpy (Key, pszNewKey);

            Class2Struct (Msg);

            strcpy (Idx.Key, Key);
            lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
            write (fdIdx, &Idx, sizeof (Idx));

            lseek (fdDat, Idx.Position, SEEK_SET);
            write (fdDat, Msg, sizeof (MESSAGE));
            free (Msg);

            retVal = TRUE;
         }
      }
   }

   if (DoClose == TRUE) {
      if (fdDat != -1) {
         close (fdDat);
         fdDat = -1;
      }
      if (fdIdx != -1) {
         close (fdIdx);
         fdIdx = -1;
      }
   }

   return (retVal);
}

// --------------------------------------------------------------------------

#define ECHOLINK_INDEX     32

TEchoLink::TEchoLink ()
{
   Data.Clear ();
   strcpy (DataFile, "echolink.dat");
   Skip4D = FALSE;
}

TEchoLink::TEchoLink (PSZ pszDataPath)
{
   Data.Clear ();

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
   strcpy (DataFile, DataFile);
   strcat (DataFile, "echolink.dat");
   Skip4D = FALSE;
}

TEchoLink::~TEchoLink ()
{
   Data.Clear ();
}

USHORT TEchoLink::Add ()
{
   USHORT Insert = FALSE;
   ECHOLINK Buffer, *Current;

   memset (&Buffer, 0, sizeof (ECHOLINK));

   Buffer.Free = FALSE;
   Buffer.EchoTag = EchoTag;
   Buffer.Zone = Zone;
   Buffer.Net = Net;
   Buffer.Node = Node;
   Buffer.Point = Point;
   strcpy (Buffer.Domain, Domain);
   Buffer.SendOnly = SendOnly;
   Buffer.ReceiveOnly = ReceiveOnly;
   Buffer.PersonalOnly = PersonalOnly;
   Buffer.Passive = Passive;
   Buffer.Skip = Skip;

   if ((Current = (ECHOLINK *)Data.First ()) != NULL) {
      if (Current->Zone > Zone)
         Insert = TRUE;
      else if (Current->Zone == Zone && Current->Net > Net)
         Insert = TRUE;
      else if (Current->Zone == Zone && Current->Net == Net && Current->Node > Node)
         Insert = TRUE;
      else if (Current->Zone == Zone && Current->Net == Net && Current->Node == Node && Current->Point > Point)
         Insert = TRUE;

      if (Insert == TRUE) {
         Data.Insert (&Buffer, sizeof (ECHOLINK));
         Data.Insert (Current, sizeof (ECHOLINK));
         Data.First ();
         Data.Remove ();
         Data.First ();
      }
      else {
         while ((Current = (ECHOLINK *)Data.Next ()) != NULL) {
            if (Current->Zone > Zone)
               Insert = TRUE;
            else if (Current->Zone == Zone && Current->Net > Net)
               Insert = TRUE;
            else if (Current->Zone == Zone && Current->Net == Net && Current->Node > Node)
               Insert = TRUE;
            else if (Current->Zone == Zone && Current->Net == Net && Current->Node == Node && Current->Point > Point)
               Insert = TRUE;

            if (Insert == TRUE) {
               Data.Previous ();
               Data.Insert (&Buffer, sizeof (ECHOLINK));
               break;
            }
         }
         if (Insert == FALSE) {
            Data.Add (&Buffer, sizeof (ECHOLINK));
            Insert = TRUE;
         }
      }
   }
   else {
      if (Insert == FALSE) {
         Data.Add (&Buffer, sizeof (ECHOLINK));
         Insert = TRUE;
      }
   }

   return (Insert);
}

USHORT TEchoLink::AddString (PSZ pszString)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], *p;
   class TAddress Address;

   strcpy (Temp, pszString);
   if ((p = strtok (Temp, " ")) != NULL)
      do {
         Skip = FALSE;
         ReceiveOnly = SendOnly = PersonalOnly = FALSE;
         if (Check (p) == FALSE) {
            while (isdigit (*p) == 0 && *p != '.') {
               if (*p == '>')
                  ReceiveOnly = TRUE;
               if (*p == '<')
                  SendOnly = TRUE;
               if (*p == '!')
                  PersonalOnly = TRUE;
               p++;
            }
            Address.Parse (p);
            if (Check (Address.String) == FALSE) {
               if (Address.Zone != 0)
                  Zone = Address.Zone;
               if (Address.Net != 0)
                  Net = Address.Net;
               Node = Address.Node;
               Point = Address.Point;
               strcpy (Domain, Address.Domain);
               RetVal = Add ();
            }
         }
      } while ((p = strtok (NULL, " ")) != NULL);

   return (RetVal);
}

VOID TEchoLink::Change (PSZ pszFrom, PSZ pszTo)
{
   int fd, i, Count, Changed;
   ULONG CrcFrom, CrcTo, Position;
   ECHOLINK *Buffer;

   CrcFrom = StringCrc32 (pszFrom, 0xFFFFFFFFL);
   CrcTo = StringCrc32 (pszTo, 0xFFFFFFFFL);

   if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if ((Buffer = (ECHOLINK *)malloc (sizeof (ECHOLINK) * ECHOLINK_INDEX)) != NULL) {
         do {
            Changed = FALSE;

            Position = tell (fd);
            Count = read (fd, Buffer, sizeof (ECHOLINK) * ECHOLINK_INDEX) / sizeof (ECHOLINK);
            for (i = 0; i < Count; i++) {
               if (Buffer[i].EchoTag == CrcFrom) {
                  Buffer[i].EchoTag = CrcTo;
                  Changed = TRUE;
               }
            }

            if (Changed == TRUE) {
               lseek (fd, Position, SEEK_SET);
               write (fd, Buffer, sizeof (ECHOLINK) * Count);
            }
         } while (Count == ECHOLINK_INDEX);
         free (Buffer);
      }

      close (fd);
   }
}

USHORT TEchoLink::Check (PSZ pszAddress)
{
   USHORT RetVal = FALSE;
   ECHOLINK *El;
   class TAddress Addr;

   while (isdigit (*pszAddress) == 0 && *pszAddress != '.')
      pszAddress++;
   Addr.Parse (pszAddress);

   if ((El = (ECHOLINK *)Data.First ()) != NULL)
      do {
         if (El->Zone == Addr.Zone && El->Net == Addr.Net && El->Node == Addr.Node && El->Point == Addr.Point) {
            EchoTag = El->EchoTag;
            Zone = El->Zone;
            Net = El->Net;
            Node = El->Node;
            Point = El->Point;
            strcpy (Domain, El->Domain);
            SendOnly = El->SendOnly;
            ReceiveOnly = El->ReceiveOnly;
            PersonalOnly = El->PersonalOnly;
            Passive = El->Passive;
            Skip = El->Skip;
            if (Point == 0)
               sprintf (Address, "%u:%u/%u", Zone, Net, Node);
            else
               sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
            if (Domain[0] != '\0') {
               strcat (Address, "@");
               strcat (Address, Domain);
            }
            RetVal = TRUE;
            break;
         }
      } while ((El = (ECHOLINK *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TEchoLink::Clear ()
{
   Data.Clear ();
   New ();
}

VOID TEchoLink::Delete ()
{
   if (Data.Value () != NULL)
      Data.Remove ();
}

USHORT TEchoLink::First ()
{
   USHORT RetVal = FALSE;
   ECHOLINK *El;

   if ((El = (ECHOLINK *)Data.First ()) != NULL) {
      EchoTag = El->EchoTag;
      Zone = El->Zone;
      Net = El->Net;
      Node = El->Node;
      Point = El->Point;
      strcpy (Domain, El->Domain);
      SendOnly = El->SendOnly;
      ReceiveOnly = El->ReceiveOnly;
      PersonalOnly = El->PersonalOnly;
      Passive = El->Passive;
      Skip = El->Skip;
      if (Point == 0)
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      else
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      if (Domain[0] != '\0') {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
      if (Skip4D == TRUE && Point != 0)
         ShortAddress[0] = '\0';
      else
         strcpy (ShortAddress, Address);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TEchoLink::Load (PSZ pszEchoTag)
{
   int fd, i, Count;
   CHAR Temp[64];
   ULONG Crc;
   ECHOLINK *Buffer;

   Data.Clear ();
   strcpy (Temp, pszEchoTag);
   strupr (Temp);
   Crc = StringCrc32 (Temp, 0xFFFFFFFFL);
   EchoTag = Crc;

   if ((fd = sopen (DataFile, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if ((Buffer = (ECHOLINK *)malloc (sizeof (ECHOLINK) * ECHOLINK_INDEX)) != NULL) {
         do {
            Count = read (fd, Buffer, sizeof (ECHOLINK) * ECHOLINK_INDEX) / sizeof (ECHOLINK);
            for (i = 0; i < Count; i++) {
               if (Buffer[i].Free == FALSE && Buffer[i].EchoTag == Crc)
                  Data.Add (&Buffer[i], sizeof (ECHOLINK));
            }
         } while (Count == ECHOLINK_INDEX);
         free (Buffer);
      }
      close (fd);
   }

   First ();
}

VOID TEchoLink::New ()
{
   Zone = 0;
   Net = 0;
   Node = 0;
   Point = 0;
   Address[0] = Domain[0] = '\0';
   SendOnly = FALSE;
   ReceiveOnly = FALSE;
   PersonalOnly = FALSE;
   Passive = FALSE;
   Skip = FALSE;
}

USHORT TEchoLink::Next ()
{
   USHORT RetVal = FALSE;
   ECHOLINK *El;

   if ((El = (ECHOLINK *)Data.Next ()) != NULL) {
      if (Skip4D == FALSE || El->Point == 0) {
         if (Zone != El->Zone) {
            if (El->Point == 0)
               sprintf (ShortAddress, "%u:%u/%u", El->Zone, El->Net, El->Node);
            else
               sprintf (ShortAddress, "%u:%u/%u.%u", El->Zone, El->Net, El->Node, El->Point);
         }
         else if (Net != El->Net) {
            if (El->Point == 0)
               sprintf (ShortAddress, "%u/%u", El->Net, El->Node);
            else
               sprintf (ShortAddress, "%u/%u.%u", El->Net, El->Node, El->Point);
         }
         else if (Node != El->Node) {
            if (El->Point == 0)
               sprintf (ShortAddress, "%u", El->Node);
            else
               sprintf (ShortAddress, "%u.%u", El->Node, El->Point);
         }
         else
            sprintf (ShortAddress, ".%u", El->Point);
      }
      else
         ShortAddress[0] = '\0';

      EchoTag = El->EchoTag;
      Zone = El->Zone;
      Net = El->Net;
      Node = El->Node;
      Point = El->Point;
      strcpy (Domain, El->Domain);
      SendOnly = El->SendOnly;
      ReceiveOnly = El->ReceiveOnly;
      PersonalOnly = El->PersonalOnly;
      Passive = El->Passive;
      Skip = El->Skip;
      if (Point == 0)
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      else
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      if (Domain[0] != '\0') {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TEchoLink::Previous ()
{
   USHORT RetVal = FALSE;
   ECHOLINK *El;

   if ((El = (ECHOLINK *)Data.Previous ()) != NULL) {
      EchoTag = El->EchoTag;
      Zone = El->Zone;
      Net = El->Net;
      Node = El->Node;
      Point = El->Point;
      strcpy (Domain, El->Domain);
      SendOnly = El->SendOnly;
      ReceiveOnly = El->ReceiveOnly;
      PersonalOnly = El->PersonalOnly;
      Passive = El->Passive;
      Skip = El->Skip;
      if (Point == 0)
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      else
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      if (Domain[0] != '\0') {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TEchoLink::Save ()
{
   int fd, i, Count, Changed;
   ULONG Position;
   ECHOLINK *Buffer, *Record;

   if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      Record = (ECHOLINK *)Data.First ();

      if ((Buffer = (ECHOLINK *)malloc (sizeof (ECHOLINK) * ECHOLINK_INDEX)) != NULL) {
         do {
            Changed = FALSE;

            Position = tell (fd);
            Count = read (fd, Buffer, sizeof (ECHOLINK) * ECHOLINK_INDEX) / sizeof (ECHOLINK);
            for (i = 0; i < Count && Record != NULL; i++) {
               if (Buffer[i].EchoTag == Record->EchoTag || Buffer[i].Free == TRUE) {
                  memcpy (&Buffer[i], Record, sizeof (ECHOLINK));
                  Record = (ECHOLINK *)Data.Next ();
                  Changed = TRUE;
               }
            }

            for (; i < Count; i++) {
               if (Buffer[i].EchoTag == EchoTag) {
                  memset (&Buffer[i], 0, sizeof (ECHOLINK));
                  Buffer[i].Free = TRUE;
                  Changed = TRUE;
               }
            }

            if (Changed == TRUE) {
               lseek (fd, Position, SEEK_SET);
               write (fd, Buffer, sizeof (ECHOLINK) * Count);
            }
         } while (Count == ECHOLINK_INDEX);
         free (Buffer);
      }

      if (Record != NULL) {
         do {
            write (fd, Record, sizeof (ECHOLINK));
         } while ((Record = (ECHOLINK *)Data.Next ()) != NULL);
      }

      close (fd);
   }
}

VOID TEchoLink::Update ()
{
   ECHOLINK *Buffer;

   if ((Buffer = (ECHOLINK *)Data.Value ()) != NULL) {
      Buffer->EchoTag = EchoTag;
      Buffer->Zone = Zone;
      Buffer->Net = Net;
      Buffer->Node = Node;
      Buffer->Point = Point;
      strcpy (Buffer->Domain, Domain);
      Buffer->SendOnly = SendOnly;
      Buffer->ReceiveOnly = ReceiveOnly;
      Buffer->PersonalOnly = PersonalOnly;
      Buffer->Passive = Passive;
      Buffer->Skip = Skip;
      if (Point == 0)
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      else
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      if (Domain[0] != '\0') {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
   }
}

// --------------------------------------------------------------------------

TEchotoss::TEchotoss (PSZ path)
{
   strcpy (DataFile, path);
   strcat (DataFile, "echotoss.log");
   AdjustPath (strlwr (DataFile));

   Data.Clear ();
}

TEchotoss::~TEchotoss ()
{
   Data.Clear ();
}

VOID TEchotoss::Add (PSZ tag)
{
   USHORT Found = FALSE;
   PSZ p;

   if ((p = (PSZ)Data.First ()) != NULL)
      do {
         if (!stricmp (p, tag)) {
            Found = TRUE;
            break;
         }
      } while ((p = (PSZ)Data.Next ()) != NULL);

   if (Found == FALSE)
      Data.Add (tag);
}

VOID TEchotoss::Clear ()
{
   Data.Clear ();
}

VOID TEchotoss::Delete ()
{
   unlink (DataFile);
}

USHORT TEchotoss::First ()
{
   USHORT RetVal = FALSE;
   PSZ p;

   if ((p = (PSZ)Data.First ()) != NULL) {
      strcpy (Tag, p);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TEchotoss::Load ()
{
   FILE *fp;
   USHORT RetVal = FALSE;
   CHAR Temp[128], *p;

   if ((fp = fopen (DataFile, "rt")) != NULL) {
      RetVal = TRUE;
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         if ((p = strtok (Temp, " \r\n")) != NULL)
            Add (p);
      }
      fclose (fp);
   }

   return (RetVal);
}

USHORT TEchotoss::Next ()
{
   USHORT RetVal = FALSE;
   PSZ p;

   if ((p = (PSZ)Data.Next ()) != NULL) {
      strcpy (Tag, p);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TEchotoss::Save ()
{
   FILE *fp;
   PSZ p;

   if ((fp = fopen (DataFile, "wt")) != NULL) {
      if ((p = (PSZ)Data.First ()) != NULL)
         do {
            fprintf (fp, "%s\n", p);
         } while ((p = (PSZ)Data.Next ()) != NULL);
      fclose (fp);
   }
}


