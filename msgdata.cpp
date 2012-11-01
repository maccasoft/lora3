
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.04
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgdata.h"

TMsgData::TMsgData (void)
{
   fdDat = -1;
   fdIdx = -1;
   IsValid = FALSE;
   Deleted = FALSE;
}

TMsgData::TMsgData (PSZ pszDataPath)
{
   fdDat = -1;
   fdIdx = -1;
   IsValid = FALSE;
   Deleted = FALSE;
   strcpy (DataPath, pszDataPath);
   if (DataPath[0] && DataPath[strlen (DataPath) - 1] != '\\')
      strcat (DataPath, "\\");
}

TMsgData::~TMsgData (void)
{
   if (fdDat != -1)
      close (fdDat);
   if (fdIdx != -1)
      close (fdIdx);
}

USHORT TMsgData::Add (VOID)
{
   USHORT retVal = FALSE, Size, Fwd;
   CHAR File[128];
   MESSAGE *Msg;
   INDEX Idx;

   if (fdIdx == -1) {
      sprintf (File, "%sMsg.Idx", DataPath);
      fdIdx = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }
   if (fdDat == -1) {
      sprintf (File, "%sMsg.Dat", DataPath);
      fdDat = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdDat, 0L, SEEK_END);
      lseek (fdIdx, 0L, SEEK_END);

      Fwd = 0;
      if (Forward.First () != NULL)
         do {
            Fwd++;
         } while (Forward.Next () != NULL);
      Size = sizeof (MESSAGE);
      if (Fwd > 1)
         Size += sizeof (ADDR) * (Fwd - 1);

      if ((Msg = (MESSAGE *)malloc (Size)) != NULL) {
         memset (Msg, 0, Size);
         Msg->Size = Size;
         Msg->Forwards = Fwd;
         Class2Struct (Msg);

         memset (&Idx, 0, sizeof (Idx));
         strcpy (Idx.Key, Key);
         Idx.Level = Level;
         Idx.AccessFlags = AccessFlags;
         Idx.DenyFlags = DenyFlags;
         Idx.Position = tell (fdDat);
         Idx.Size = Size;

         write (fdDat, Msg, Size);
         write (fdIdx, &Idx, sizeof (Idx));
         retVal = TRUE;

         free (Msg);
      }
   }

   return (retVal);
}

VOID TMsgData::Class2Struct (MESSAGE *Msg)
{
   USHORT Fwd;

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
   strcpy (Msg->EchoTag, EchoTag);
   strcpy (Msg->Origin, Origin);

   Feeder.First ();
   Msg->Feeder.Zone = Feeder.Zone;
   Msg->Feeder.Net = Feeder.Net;
   Msg->Feeder.Node = Feeder.Node;
   Msg->Feeder.Point = Feeder.Point;
   strcpy (Msg->Feeder.Domain, Feeder.Domain);

   Fwd = 0;
   if (Forward.First () != NULL)
      do {
         Msg->Forward[Fwd].Zone = Forward.Zone;
         Msg->Forward[Fwd].Net = Forward.Net;
         Msg->Forward[Fwd].Node = Forward.Node;
         Msg->Forward[Fwd].Point = Forward.Point;
         strcpy (Msg->Forward[Fwd].Domain, Forward.Domain);
         Fwd++;
      } while (Forward.Next () != NULL);
}

VOID TMsgData::Delete (VOID)
{
   INDEX Idx;

   if (fdDat != -1 && fdIdx != -1) {
      if (tell (fdIdx) >= sizeof (INDEX)) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         Idx.Flags |= IDX_DELETED;
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         write (fdIdx, &Idx, sizeof (Idx));
      }
   }
}

USHORT TMsgData::First (VOID)
{
   USHORT retVal = FALSE;
   CHAR File[128];

   if (fdIdx == -1) {
      sprintf (File, "%sMsg.Idx", DataPath);
      fdIdx = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }
   if (fdDat == -1) {
      sprintf (File, "%sMsg.Dat", DataPath);
      fdDat = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);
      retVal = Next ();
   }

   return (retVal);
}

USHORT TMsgData::IsNext (VOID)
{
   USHORT retVal = FALSE;
   ULONG ulPosIdx;
   INDEX Idx;

   ulPosIdx = tell (fdIdx);

   while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
      if (!(Idx.Flags & IDX_DELETED))
         retVal = TRUE;
   }

   lseek (fdIdx, ulPosIdx, SEEK_SET);

   return (retVal);
}

USHORT TMsgData::IsPrevious (VOID)
{
   USHORT retVal = FALSE;
   ULONG ulPosIdx;
   INDEX Idx;

   ulPosIdx = tell (fdIdx);

   while (retVal == FALSE && tell (fdIdx) >= sizeof (Idx) * 2) {
      lseek (fdIdx, tell (fdIdx) - sizeof (Idx) * 2, SEEK_SET);
      read (fdIdx, &Idx, sizeof (Idx));
      if (!(Idx.Flags & IDX_DELETED))
         retVal = TRUE;
   }

   lseek (fdIdx, ulPosIdx, SEEK_SET);

   return (retVal);
}

USHORT TMsgData::Last (VOID)
{
   USHORT retVal = FALSE;
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
            if ((Msg = (MESSAGE *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, Msg, Idx.Size) == Idx.Size) {
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
            if ((Msg = (MESSAGE *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, Msg, Idx.Size) == Idx.Size) {
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

VOID TMsgData::New (VOID)
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
   ShowGlobal = FALSE;
   UpdateNews = FALSE;
   memset (MenuName, 0, sizeof (MenuName));
   memset (Moderator, 0, sizeof (Moderator));
   Cost = 0;
   DaysOld = 0;
   RecvDaysOld = 0;
   MaxMessages = 0;
   ActiveMsgs = 0;
   memset (NewsGroup, 0, sizeof (NewsGroup));
   Highest = 0L;
   memset (EchoTag, 0, sizeof (EchoTag));
   memset (Origin, 0, sizeof (Origin));
   Feeder.Clear ();
   Forward.Clear ();

   IsValid = FALSE;
   Deleted = FALSE;
}

USHORT TMsgData::Next (VOID)
{
   USHORT retVal = FALSE;
   MESSAGE *Msg;
   INDEX Idx;

   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((Msg = (MESSAGE *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, Msg, Idx.Size) == Idx.Size) {
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

VOID TMsgData::Pack (VOID)
{
   int fdNewIdx, fdNewDat;
   CHAR File[128], New[128];
   USHORT MaxSize;
   INDEX Idx;
   MESSAGE *Msg;

   if (fdIdx == -1) {
      sprintf (File, "%sMsg.Idx", DataPath);
      fdIdx = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }
   if (fdDat == -1) {
      sprintf (File, "%sMsg.Dat", DataPath);
      fdDat = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fdIdx != -1 && fdDat != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      MaxSize = 0;
      while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && MaxSize < Idx.Size)
            MaxSize = Idx.Size;
      }
      lseek (fdIdx, 0L, SEEK_SET);

      if (MaxSize != 0 && (Msg = (MESSAGE *)malloc (MaxSize)) != NULL) {
         sprintf (File, "%sMsg-New.Idx", DataPath);
         fdNewIdx = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
         sprintf (File, "%sMsg-New.Dat", DataPath);
         fdNewDat = open (File, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

         if (fdNewIdx != -1 && fdNewDat != -1) {
            while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
               if (!(Idx.Flags & IDX_DELETED)) {
                  lseek (fdDat, Idx.Position, SEEK_SET);
                  read (fdDat, Msg, Idx.Size);
                  Idx.Position = tell (fdNewDat);
                  write (fdNewDat, Msg, Idx.Size);
                  write (fdNewIdx, &Idx, sizeof (Idx));
               }
            }

            close (fdNewIdx);
            close (fdNewDat);

            sprintf (File, "%sMsg.Dat", DataPath);
            unlink (File);
            sprintf (New, "%sMsg-New.Dat", DataPath);
            rename (New, File);
            sprintf (File, "%sMsg.Idx", DataPath);
            unlink (File);
            sprintf (New, "%sMsg-New.Idx", DataPath);
            rename (New, File);
         }

         sprintf (File, "%sMsg-New.Dat", DataPath);
         unlink (File);
         sprintf (File, "%sMsg-New.Idx", DataPath);
         unlink (File);
         free (Msg);
      }

      close (fdIdx);
      close (fdDat);
      fdIdx = fdDat = -1;
   }
}

USHORT TMsgData::Previous (VOID)
{
   USHORT retVal = FALSE;
   MESSAGE *Msg;
   INDEX Idx;

   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && tell (fdIdx) >= sizeof (INDEX) * 2) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX) * 2, SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((Msg = (MESSAGE *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, Msg, Idx.Size) == Idx.Size) {
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
   CHAR File[128];
   MESSAGE *Msg;
   INDEX Idx;

   New ();
   IsValid = TRUE;

   if (fdIdx == -1) {
      sprintf (File, "%sMsg.Idx", DataPath);
      fdIdx = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }
   if (fdDat == -1) {
      sprintf (File, "%sMsg.Dat", DataPath);
      fdDat = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);

      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && !stricmp (pszName, Idx.Key)) {
            if ((Msg = (MESSAGE *)malloc (Idx.Size)) != NULL) {
               lseek (fdDat, Idx.Position, SEEK_SET);
               read (fdDat, Msg, Idx.Size);

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

USHORT TMsgData::ReRead (VOID)
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
   USHORT i;

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
   strcpy (EchoTag, Msg->EchoTag);
   strcpy (Origin, Msg->Origin);

   Feeder.Clear ();
   Feeder.Add (Msg->Feeder.Zone, Msg->Feeder.Net, Msg->Feeder.Node, Msg->Feeder.Point, Msg->Feeder.Domain);
   Feeder.First ();

   Forward.Clear ();
   for (i = 0; i < Msg->Forwards; i++)
      Forward.Add (Msg->Forward[i].Zone, Msg->Forward[i].Net, Msg->Forward[i].Node, Msg->Forward[i].Point, Msg->Forward[i].Domain);
   Forward.First ();
}

USHORT TMsgData::Update (VOID)
{
   USHORT retVal = FALSE, Size, Fwd, DoClose = FALSE;
   CHAR File[128];
   MESSAGE *Msg;
   INDEX Idx;

   if (fdIdx == -1) {
      sprintf (File, "%sMsg.Idx", DataPath);
      fdIdx = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }
   if (fdDat == -1) {
      sprintf (File, "%sMsg.Dat", DataPath);
      fdDat = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1 && fdIdx != -1) {
      if (DoClose == TRUE) {
         lseek (fdIdx, 0L, SEEK_SET);
         while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
            if (!(Idx.Flags & IDX_DELETED) && !stricmp (Key, Idx.Key))
               retVal = TRUE;
         }
      }
      else
          retVal = TRUE;

      if (retVal == TRUE && tell (fdIdx) >= sizeof (INDEX)) {
         retVal = FALSE;

         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));

         Fwd = 0;
         if (Forward.First () != NULL)
            do {
               Fwd++;
            } while (Forward.Next () != NULL);
         Size = sizeof (MESSAGE);
         if (Fwd > 1)
            Size += sizeof (ADDR) * (Fwd - 1);

         if ((Msg = (MESSAGE *)malloc (Size)) != NULL) {
            memset (Msg, 0, Size);
            Msg->Size = Size;
            Msg->Forwards = Fwd;
            Class2Struct (Msg);

            if (Idx.Size < Size)
               Idx.Position = filelength (fdDat);

            Idx.Size = Size;
            strcpy (Idx.Key, Key);
            if (Deleted == TRUE)
               Idx.Flags |= IDX_DELETED;
            else
               Idx.Flags &= ~IDX_DELETED;
            lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
            write (fdIdx, &Idx, sizeof (Idx));

            lseek (fdDat, Idx.Position, SEEK_SET);
            write (fdDat, Msg, Idx.Size);
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


