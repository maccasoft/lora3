
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "forum.h"

ForumData::ForumData (void)
{
   fdDat = -1;
   fdIdx = -1;
   IsValid = FALSE;
   Deleted = FALSE;
}

ForumData::~ForumData (void)
{
   if (fdDat != -1)
      close (fdDat);
   if (fdIdx != -1)
      close (fdIdx);
}

USHORT ForumData::Add (VOID)
{
   USHORT retVal = FALSE, Size, Msgs, Files;
   FORUM *Forum;
   INDEX Idx;

   if (fdIdx == -1)
      fdIdx = open ("FORUM.IDX", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = open ("FORUM.DAT", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdDat, 0L, SEEK_END);
      lseek (fdIdx, 0L, SEEK_END);

      Msgs = 0;
      if (Msg.First () != NULL)
         do {
            Msgs++;
         } while (Msg.Next () != NULL);
      Files = 0;
      if (File.First () != NULL)
         do {
            Files++;
         } while (File.Next () != NULL);
      Size = sizeof (FORUM);
      if ((Msgs + Files) > 1)
         Size += sizeof (Forum->Key) * (Msgs + Files - 1);

      if ((Forum = (FORUM *)malloc (Size)) != NULL) {
         memset (Forum, 0, Size);
         Forum->Size = Size;
         Forum->Msgs = Msgs;
         Forum->Files = Files;
         Class2Struct (Forum);

         memset (&Idx, 0, sizeof (Idx));
         strcpy (Idx.Key, Key);
         Idx.Level = Level;
         Idx.AccessFlags = AccessFlags;
         Idx.DenyFlags = DenyFlags;
         Idx.Position = tell (fdDat);
         Idx.Size = Size;

         write (fdDat, Forum, Size);
         write (fdIdx, &Idx, sizeof (Idx));
         retVal = TRUE;

         free (Forum);
      }
   }

   return (retVal);
}

VOID ForumData::Class2Struct (FORUM *Forum)
{
   USHORT i;

   strcpy (Forum->Display, Display);
   strcpy (Forum->Key, Key);
   Forum->Level = Level;
   Forum->AccessFlags = AccessFlags;
   Forum->DenyFlags = DenyFlags;
   Forum->Age = Age;
   strcpy (Forum->Location, Location);
   Forum->Flags = Flags;
   strcpy (Forum->MenuName, MenuName);
   strcpy (Forum->Moderator, Moderator);
   Forum->Cost = Cost;
   Forum->ActiveMsgs = ActiveMsgs;
   Forum->ActiveFiles = ActiveFiles;

   i = 0;
   if (Msg.First () != NULL)
      do {
         strcpy (Forum->AreaKey[i], (char *)Msg.Value ());
         i++;
      } while (Msg.Next () != NULL);
   if (File.First () != NULL)
      do {
         strcpy (Forum->AreaKey[i], (char *)File.Value ());
         i++;
      } while (File.Next () != NULL);
}

VOID ForumData::Delete (VOID)
{
   Deleted = TRUE;
}

USHORT ForumData::First (VOID)
{
   USHORT retVal = FALSE;

   if (fdIdx == -1)
      fdIdx = open ("FORUM.IDX", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = open ("FORUM.DAT", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);
      retVal = Next ();
   }

   return (retVal);
}

USHORT ForumData::IsNext (VOID)
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

USHORT ForumData::IsPrevious (VOID)
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

USHORT ForumData::Next (VOID)
{
   USHORT retVal = FALSE;
   FORUM *Forum;
   INDEX Idx;

   Reset ();
   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((Forum = (FORUM *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, Forum, Idx.Size) == Idx.Size) {
                  retVal = TRUE;
                  Struct2Class (Forum);
                  IsValid = TRUE;
               }

               free (Forum);
            }
         }
      }
   }

   return (retVal);
}

VOID ForumData::Pack (VOID)
{
   int fdNewIdx, fdNewDat;
   USHORT MaxSize;
   INDEX Idx;
   FORUM *Forum;

   if (fdIdx == -1)
      fdIdx = open ("FORUM.IDX", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = open ("FORUM.DAT", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

   if (fdIdx != -1 && fdDat != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      MaxSize = 0;
      while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && MaxSize < Idx.Size)
            MaxSize = Idx.Size;
      }
      lseek (fdIdx, 0L, SEEK_SET);

      if (MaxSize != 0 && (Forum = (FORUM *)malloc (MaxSize)) != NULL) {
         fdNewIdx = open ("FORM-NEW.IDX", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
         fdNewDat = open ("FORM-NEW.DAT", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

         if (fdNewIdx != -1 && fdNewDat != -1) {
            while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
               if (!(Idx.Flags & IDX_DELETED)) {
                  lseek (fdDat, Idx.Position, SEEK_SET);
                  read (fdDat, Forum, Idx.Size);
                  Idx.Position = tell (fdNewDat);
                  write (fdNewDat, Forum, Idx.Size);
                  write (fdNewIdx, &Idx, sizeof (Idx));
               }
            }

            close (fdNewIdx);
            close (fdNewDat);

            unlink ("FORUM.DAT");
            unlink ("FORUM.IDX");
            rename ("FORM-NEW.DAT", "FORUM.DAT");
            rename ("FORM-NEW.IDX", "FORUM.IDX");
         }

         unlink ("FORM-NEW.DAT");
         unlink ("FORM-NEW.IDX");
         free (Forum);
      }

      close (fdIdx);
      close (fdDat);
      fdIdx = fdDat = -1;
   }
}

USHORT ForumData::Previous (VOID)
{
   USHORT retVal = FALSE;
   FORUM *Forum;
   INDEX Idx;

   Reset ();
   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && tell (fdIdx) >= sizeof (INDEX) * 2) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX) * 2, SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((Forum = (FORUM *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, Forum, Idx.Size) == Idx.Size) {
                  retVal = TRUE;
                  Struct2Class (Forum);
                  IsValid = TRUE;
               }

               free (Forum);
            }
         }
      }
   }

   return (retVal);
}

USHORT ForumData::Read (PSZ pszName, USHORT fCloseFile)
{
   USHORT retVal = FALSE;
   FORUM *Forum;
   INDEX Idx;

   Reset ();
   IsValid = TRUE;

   if (fdIdx == -1)
      fdIdx = open ("FORUM.IDX", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = open ("FORUM.DAT", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);

      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && !stricmp (pszName, Idx.Key)) {
            if ((Forum = (FORUM *)malloc (Idx.Size)) != NULL) {
               lseek (fdDat, Idx.Position, SEEK_SET);
               read (fdDat, Forum, Idx.Size);

               retVal = TRUE;
               Struct2Class (Forum);

               free (Forum);
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

USHORT ForumData::ReRead (VOID)
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

VOID ForumData::Reset (VOID)
{
   memset (Display, 0, sizeof (Display));
   memset (Key, 0, sizeof (Key));
   Level = 0;
   AccessFlags = DenyFlags = 0L;
   Age = 0;
   Flags = 0;
   memset (MenuName, 0, sizeof (MenuName));
   memset (Moderator, 0, sizeof (Moderator));
   Cost = 0L;
   ActiveFiles = ActiveMsgs = 0L;
   Msg.Clear ();
   File.Clear ();

   IsValid = FALSE;
   Deleted = FALSE;
}

VOID ForumData::Struct2Class (FORUM *Forum)
{
   USHORT m, i;

   strcpy (Display, Forum->Display);
   strcpy (Key, Forum->Key);
   Level = Forum->Level;
   AccessFlags = Forum->AccessFlags;
   DenyFlags = Forum->DenyFlags;
   Age = Forum->Age;
   strcpy (Location, Forum->Location);
   Flags = Forum->Flags;
   strcpy (MenuName, Forum->MenuName);
   strcpy (Moderator, Forum->Moderator);
   Cost = Forum->Cost;
   ActiveMsgs = Forum->ActiveMsgs;
   ActiveFiles = Forum->ActiveFiles;

   Msg.Clear ();
   for (m = 0, i = 0; i < Forum->Msgs; m++, i++)
      Msg.Add (Forum->AreaKey[m], 32);

   File.Clear ();
   for (i = 0; i < Forum->Files; m++, i++)
      File.Add (Forum->AreaKey[m], 32);
}

USHORT ForumData::Update (VOID)
{
   USHORT retVal = FALSE, Size, Msgs, Files;
   FORUM *Forum;
   INDEX Idx;

   if (IsValid == TRUE && fdDat != -1 && fdIdx != -1) {
      if (tell (fdIdx) >= sizeof (INDEX)) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));

         Msgs = 0;
         if (Msg.First () != NULL)
            do {
               Msgs++;
            } while (Msg.Next () != NULL);
         Files = 0;
         if (File.First () != NULL)
            do {
               Files++;
            } while (File.Next () != NULL);
         Size = sizeof (FORUM);
         if ((Msgs + Files) > 1)
            Size += sizeof (Forum->Key) * (Msgs + Files - 1);

         if ((Forum = (FORUM *)malloc (Size)) != NULL) {
            memset (Forum, 0, Size);
            Forum->Size = Size;
            Forum->Msgs = Msgs;
            Forum->Files = Files;
            Class2Struct (Forum);

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
            write (fdDat, Forum, Idx.Size);
            free (Forum);

            retVal = TRUE;
         }
      }
   }

   return (retVal);
}

