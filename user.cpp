
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.14
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "user.h"

TUser::TUser (void)
{
   Valid = Validate = FALSE;
   strcpy (DatFile, "User.Dat");
   strcpy (IdxFile, "User.Idx");
   fdDat = fdIdx = -1;
   LastRead = new TLastRead;
   LastRead->Clear ();
   FileTag = new TFileTag;
   FileTag->Clear ();
}

TUser::TUser (PSZ pszUserFile)
{
   Valid = Validate = FALSE;
   strcpy (DatFile, pszUserFile);
   strcat (DatFile, ".Dat");
   strcpy (IdxFile, pszUserFile);
   strcat (IdxFile, ".Idx");
   fdDat = fdIdx = -1;
   LastRead = new TLastRead;
   LastRead->Clear ();
   FileTag = new TFileTag;
   FileTag->Clear ();
}

TUser::~TUser (void)
{
   if (fdDat != -1)
      close (fdDat);
   if (fdIdx != -1)
      close (fdIdx);
   if (LastRead != 0) {
      LastRead->Clear ();
      delete LastRead;
   }
   if (FileTag != 0) {
      FileTag->Clear ();
      delete FileTag;
   }
}

USHORT TUser::Add (USHORT usAddSize)
{
   USHORT retVal = FALSE, LastReads, Tags;
   USER Usr;
   UINDEX Idx;
   LASTREAD lastRead;
   FTAGS fTag;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdDat, 0L, SEEK_END);
      lseek (fdIdx, 0L, SEEK_END);

      LastReads = 0;
      if (LastRead->First () == TRUE)
         do {
            LastReads++;
         } while (LastRead->Next () == TRUE);

      Tags = 0;
      if (FileTag->First () == TRUE)
         do {
            Tags++;
         } while (FileTag->Next () == TRUE);

      memset (&Idx, 0, sizeof (Idx));
      Idx.NameCrc = StringCrc32 (Name);
      Idx.RealNameCrc = StringCrc32 (RealName);
      Idx.Position = tell (fdDat);
      Idx.Size = (USHORT)(sizeof (Usr) + LastReads * sizeof (LASTREAD) + Tags * sizeof (FTAGS));
      Idx.Size += usAddSize;
      Idx.Flags = 0;

      memset (&Usr, 0, sizeof (Usr));
      Usr.Id = USER_ID;
      Usr.Size = Idx.Size;
      strcpy (Usr.Name, Name);
      Usr.Password = Password;
      strcpy (Usr.RealName, RealName);
      strcpy (Usr.Company, Company);
      strcpy (Usr.Address1, Address1);
      strcpy (Usr.Address2, Address2);
      strcpy (Usr.Address3, Address3);
      strcpy (Usr.DayPhone, DayPhone);
      Usr.Ansi = Ansi;
      Usr.Avatar = Avatar;
      Usr.Color = Color;
      Usr.HotKey = HotKey;
      Usr.ReadHeader = ReadHeader;
      Usr.System = System;
      Usr.Sex = Sex;
      Usr.BirthDate = BirthDate;
      Usr.ScreenHeight = ScreenHeight;
      Usr.ScreenWidth = ScreenWidth;
      Usr.Level = Level;
      Usr.AccessFlags = AccessFlags;
      Usr.DenyFlags = DenyFlags;
      Usr.CreationDate = CreationDate;
      Usr.LastCall = LastCall;
      Usr.TotalCalls = TotalCalls;
      Usr.TodayTime = TodayTime;
      Usr.WeekTime = WeekTime;
      Usr.MonthTime = MonthTime;
      Usr.YearTime = YearTime;
      strcpy (Usr.MailBox, MailBox);
      strcpy (Usr.LimitClass, LimitClass);
      strcpy (Usr.Language, Language);
      strcpy (Usr.FtpHost, FtpHost);
      strcpy (Usr.FtpName, FtpName);
      strcpy (Usr.FtpPwd, FtpPwd);
      Usr.LastReads = LastReads;
      Usr.FileTags = Tags;

      write (fdDat, &Usr, sizeof (Usr));
      if (LastRead->First () == TRUE)
         do {
            memset (&lastRead, 0, sizeof (lastRead));
            strcpy (lastRead.Key, LastRead->Key);
            lastRead.Number = LastRead->Number;
            lastRead.QuickScan = LastRead->QuickScan;
            lastRead.PersonalOnly = LastRead->PersonalOnly;
            lastRead.ExcludeOwn = LastRead->ExcludeOwn;
            write (fdDat, &lastRead, sizeof (lastRead));
         } while (LastRead->Next () == TRUE);

      if (FileTag->First () == TRUE)
         do {
            memset (&fTag, 0, sizeof (fTag));
            fTag.Index = FileTag->Index;
            strcpy (fTag.Name, FileTag->Name);
            strcpy (fTag.Library, FileTag->Library);
            fTag.Size = FileTag->Size;
            strcpy (fTag.Complete, FileTag->Complete);
            fTag.DeleteAfter = FileTag->DeleteAfter;
            write (fdDat, &fTag, sizeof (fTag));
         } while (FileTag->Next () == TRUE);

      // Writes additional space to the user's record
      memset (&fTag, 0, sizeof (fTag));
      while (usAddSize > 0) {
         if (usAddSize < sizeof (fTag))
            usAddSize -= (USHORT)write (fdDat, &fTag, usAddSize);
         else
            usAddSize -= (USHORT)write (fdDat, &fTag, sizeof (fTag));
      }

      write (fdIdx, &Idx, sizeof (Idx));
      LastPosition = tell (fdIdx);

      Valid = TRUE;
      Validate = FALSE;

      retVal = TRUE;
   }

   if (fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
   if (fdIdx != -1) {
      close (fdIdx);
      fdIdx = -1;
   }

   return (retVal);
}

USHORT TUser::CheckPassword (PSZ pszPassword)
{
   if (Password == StringCrc32 (strupr (pszPassword)))
      return (TRUE);
   else
      return (FALSE);
}

VOID TUser::Clear (VOID)
{
   memset (Name, 0, sizeof (Name));
   Password = 0L;
   memset (RealName, 0, sizeof (RealName));
   memset (Company, 0, sizeof (Company));
   memset (Address1, 0, sizeof (Address1));
   memset (Address2, 0, sizeof (Address2)),
   memset (Address3, 0, sizeof (Address3));
   memset (DayPhone, 0, sizeof (DayPhone));
   Ansi = Avatar = Color = HotKey = ReadHeader = System = Sex = FALSE;
   BirthDate = 0L;
   ScreenHeight = ScreenWidth = 0;
   Level = 0;
   AccessFlags = DenyFlags = 0L;
   CreationDate = 0L;
   LastCall = TotalCalls = 0L;
   TodayTime = WeekTime = MonthTime = YearTime = 0L;
   memset (MailBox, 0, sizeof (MailBox));
   memset (LimitClass, 0, sizeof (LimitClass));
   memset (Language, 0, sizeof (Language));
   memset (FtpHost, 0, sizeof (FtpHost));
   memset (FtpName, 0, sizeof (FtpName));
   memset (FtpPwd, 0, sizeof (FtpPwd));
   LastRead->Clear ();
   FileTag->Clear ();
   Valid = Validate = FALSE;
}

USHORT TUser::Delete (VOID)
{
   USHORT retVal = FALSE;
   UINDEX Idx;
   ULONG NameCrc;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      LastPosition = 0L;
      NameCrc = StringCrc32 (Name);
      lseek (fdIdx, 0L, SEEK_SET);
      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && Idx.NameCrc == NameCrc) {
            retVal = TRUE;
            LastPosition = tell (fdIdx);
         }
      }

      if (LastPosition >= sizeof (Idx) && retVal == TRUE) {
         lseek (fdIdx, LastPosition - sizeof (Idx), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         Idx.Flags |= IDX_DELETED;
         lseek (fdIdx, LastPosition - sizeof (Idx), SEEK_SET);
         write (fdIdx, &Idx, sizeof (Idx));
      }
   }

   if (fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
   if (fdIdx != -1) {
      close (fdIdx);
      fdIdx = -1;
   }

   return (retVal);
}

USHORT TUser::Exist (PSZ pszName, USHORT fCheckRealName)
{
   USHORT retVal = FALSE;
   ULONG testCrc;
   UINDEX Idx;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      testCrc = StringCrc32 (pszName);

      lseek (fdIdx, 0L, SEEK_SET);
      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && Idx.NameCrc == testCrc)
            retVal = TRUE;
      }
      if (retVal == FALSE && fCheckRealName == TRUE) {
         lseek (fdIdx, 0L, SEEK_SET);
         while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
            if (!(Idx.Flags & IDX_DELETED) && Idx.RealNameCrc == testCrc)
               retVal = TRUE;
         }
      }
   }

   if (fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
   if (fdIdx != -1) {
      close (fdIdx);
      fdIdx = -1;
   }

   return (retVal);
}

USHORT TUser::First (USHORT fComplete)
{
   USHORT retVal = FALSE;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      LastPosition = 0L;
      lseek (fdIdx, 0L, SEEK_SET);
      retVal = Next (fComplete);
   }

   return (retVal);
}

USHORT TUser::GetData (PSZ pszName, USHORT fCheckRealName)
{
   USHORT i, retVal = FALSE;
   ULONG testCrc;
   USER Usr;
   LASTREAD lastRead;
   FTAGS fTag;
   UINDEX Idx;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      Clear ();

      testCrc = StringCrc32 (pszName);

      lseek (fdIdx, 0L, SEEK_SET);
      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && Idx.NameCrc == testCrc)
            retVal = TRUE;
      }
      if (retVal == FALSE && fCheckRealName == TRUE) {
         lseek (fdIdx, 0L, SEEK_SET);
         while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
            if (!(Idx.Flags & IDX_DELETED) && Idx.RealNameCrc == testCrc)
               retVal = TRUE;
         }
      }
      if (retVal == TRUE) {
         LastPosition = tell (fdIdx);

         lseek (fdDat, Idx.Position, SEEK_SET);
         read (fdDat, &Usr, sizeof (Usr));
         for (i = 0; i < Usr.LastReads; i++) {
            read (fdDat, &lastRead, sizeof (lastRead));
            strcpy (LastRead->Key, lastRead.Key);
            LastRead->QuickScan = lastRead.QuickScan;
            LastRead->Number = lastRead.Number;
            LastRead->PersonalOnly = lastRead.PersonalOnly;
            LastRead->ExcludeOwn = lastRead.ExcludeOwn;
            LastRead->Add ();
         }
         for (i = 0; i < Usr.FileTags; i++) {
            read (fdDat, &fTag, sizeof (fTag));
            FileTag->New ();
            strcpy (FileTag->Name, fTag.Name);
            strcpy (FileTag->Library, fTag.Library);
            FileTag->Size = fTag.Size;
            strcpy (FileTag->Complete, fTag.Complete);
            FileTag->DeleteAfter = fTag.DeleteAfter;
            FileTag->Add ();
         }

         strcpy (Name, Usr.Name);
         Password = Usr.Password;
         strcpy (RealName, Usr.RealName);
         strcpy (Company, Usr.Company);
         strcpy (Address1, Usr.Address1);
         strcpy (Address2, Usr.Address2);
         strcpy (Address3, Usr.Address3);
         strcpy (DayPhone, Usr.DayPhone);
         Ansi = Usr.Ansi;
         Avatar = Usr.Avatar;
         Color = Usr.Color;
         HotKey = Usr.HotKey;
         ReadHeader = Usr.ReadHeader;
         System = Usr.System;
         Sex = Usr.Sex;
         BirthDate = Usr.BirthDate;
         ScreenHeight = Usr.ScreenHeight;
         ScreenWidth = Usr.ScreenWidth;
         Level = Usr.Level;
         AccessFlags = Usr.AccessFlags;
         DenyFlags = Usr.DenyFlags;
         CreationDate = Usr.CreationDate;
         LastCall = Usr.LastCall;
         TotalCalls = Usr.TotalCalls;
         TodayTime = Usr.TodayTime;
         WeekTime = Usr.WeekTime;
         MonthTime = Usr.MonthTime;
         YearTime = Usr.YearTime;
         strcpy (MailBox, Usr.MailBox);
         strcpy (LimitClass, Usr.LimitClass);
         strcpy (Language, Usr.Language);
         strcpy (FtpHost, Usr.FtpHost);
         strcpy (FtpName, Usr.FtpName);
         strcpy (FtpPwd, Usr.FtpPwd);
         Valid = Validate = TRUE;
      }
   }

   if (fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
   if (fdIdx != -1) {
      close (fdIdx);
      fdIdx = -1;
   }

   return (retVal);
}

PSZ TUser::GetMailBox (PSZ pszName, USHORT fCheckRealName)
{
   USHORT retVal = FALSE;
   PSZ pszValue = NULL;
   ULONG testCrc;
   USER Usr;
   UINDEX Idx;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      testCrc = StringCrc32 (pszName);

      lseek (fdIdx, 0L, SEEK_SET);
      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && Idx.NameCrc == testCrc)
            retVal = TRUE;
      }
      if (retVal == FALSE && fCheckRealName == TRUE) {
         lseek (fdIdx, 0L, SEEK_SET);
         while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
            if (!(Idx.Flags & IDX_DELETED) && Idx.RealNameCrc == testCrc)
               retVal = TRUE;
         }
      }
      if (retVal == TRUE) {
         lseek (fdDat, Idx.Position, SEEK_SET);
         read (fdDat, &Usr, sizeof (Usr));
         strcpy (TempMailBox, Usr.MailBox);
         pszValue = TempMailBox;
      }
   }

   if (fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
   if (fdIdx != -1) {
      close (fdIdx);
      fdIdx = -1;
   }

   return (pszValue);
}

USHORT TUser::IsNext (VOID)
{
   USHORT RetVal = FALSE;
   UINDEX Idx;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdIdx != -1) {
      lseek (fdIdx, LastPosition, SEEK_SET);
      while (RetVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED))
            RetVal = TRUE;
      }
   }

   if (fdIdx != -1) {
      close (fdIdx);
      fdIdx = -1;
   }

   return (RetVal);
}

USHORT TUser::IsPrevious (VOID)
{
   USHORT RetVal = FALSE;
   UINDEX Idx;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdIdx != -1) {
      lseek (fdIdx, LastPosition, SEEK_SET);

      if (tell (fdIdx) >= sizeof (Idx) * 2)
         do {
            lseek (fdIdx, tell (fdIdx) - sizeof (Idx) * 2, SEEK_SET);
            if (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
               if (!(Idx.Flags & IDX_DELETED))
                  RetVal = TRUE;
            }
         } while (RetVal == FALSE && tell (fdIdx) >= sizeof (Idx) * 2);
   }

   if (fdIdx != -1) {
      close (fdIdx);
      fdIdx = -1;
   }

   return (RetVal);
}

USHORT TUser::Next (USHORT fComplete)
{
   USHORT i, retVal = FALSE;
   USER Usr;
   LASTREAD lastRead;
   FTAGS fTag;
   UINDEX Idx;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1) {
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      lseek (fdIdx, LastPosition, SEEK_SET);
   }

   if (fdDat != -1 && fdIdx != -1) {
      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED))
            retVal = TRUE;
      }

      if (retVal == TRUE) {
         Clear ();
         LastPosition = tell (fdIdx);

         lseek (fdDat, Idx.Position, SEEK_SET);
         read (fdDat, &Usr, sizeof (Usr));

         if (fComplete == TRUE) {
            for (i = 0; i < Usr.LastReads; i++) {
               read (fdDat, &lastRead, sizeof (lastRead));
               strcpy (LastRead->Key, lastRead.Key);
               LastRead->Number = lastRead.Number;
               LastRead->QuickScan = lastRead.QuickScan;
               LastRead->PersonalOnly = lastRead.PersonalOnly;
               LastRead->ExcludeOwn = lastRead.ExcludeOwn;
               LastRead->Add ();
            }
            for (i = 0; i < Usr.FileTags; i++) {
               read (fdDat, &fTag, sizeof (fTag));
               FileTag->New ();
               strcpy (FileTag->Name, fTag.Name);
               strcpy (FileTag->Library, fTag.Library);
               FileTag->Size = fTag.Size;
               strcpy (FileTag->Complete, fTag.Complete);
               FileTag->DeleteAfter = fTag.DeleteAfter;
               FileTag->Add ();
            }
         }

         strcpy (Name, Usr.Name);
         Password = Usr.Password;
         strcpy (RealName, Usr.RealName);
         strcpy (Company, Usr.Company);
         strcpy (Address1, Usr.Address1);
         strcpy (Address2, Usr.Address2);
         strcpy (Address3, Usr.Address3);
         strcpy (DayPhone, Usr.DayPhone);
         Ansi = Usr.Ansi;
         Avatar = Usr.Avatar;
         Color = Usr.Color;
         HotKey = Usr.HotKey;
         ReadHeader = Usr.ReadHeader;
         System = Usr.System;
         Sex = Usr.Sex;
         BirthDate = Usr.BirthDate;
         ScreenHeight = Usr.ScreenHeight;
         ScreenWidth = Usr.ScreenWidth;
         Level = Usr.Level;
         AccessFlags = Usr.AccessFlags;
         DenyFlags = Usr.DenyFlags;
         CreationDate = Usr.CreationDate;
         LastCall = Usr.LastCall;
         TotalCalls = Usr.TotalCalls;
         TodayTime = Usr.TodayTime;
         WeekTime = Usr.WeekTime;
         MonthTime = Usr.MonthTime;
         YearTime = Usr.YearTime;
         strcpy (MailBox, Usr.MailBox);
         strcpy (LimitClass, Usr.LimitClass);
         strcpy (Language, Usr.Language);
         strcpy (FtpHost, Usr.FtpHost);
         strcpy (FtpName, Usr.FtpName);
         strcpy (FtpPwd, Usr.FtpPwd);
         Valid = Validate = TRUE;
      }
   }

   return (retVal);
}

VOID TUser::Pack (VOID)
{
}

VOID TUser::SetPassword (PSZ pszPassword)
{
   Password = StringCrc32 (strupr (pszPassword));
   Validate = FALSE;
}

USHORT TUser::Previous (USHORT fComplete)
{
   USHORT i, retVal = FALSE;
   USER Usr;
   LASTREAD lastRead;
   FTAGS fTag;
   UINDEX Idx;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1) {
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      lseek (fdIdx, LastPosition, SEEK_SET);
   }

   if (fdDat != -1 && fdIdx != -1 && (LastPosition - sizeof (Idx)) >= sizeof (Idx)) {
      if (tell (fdIdx) >= sizeof (Idx) * 2)
         do {
            lseek (fdIdx, tell (fdIdx) - sizeof (Idx) * 2, SEEK_SET);
            if (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
               if (!(Idx.Flags & IDX_DELETED))
                  retVal = TRUE;
            }
         } while (retVal == FALSE && tell (fdIdx) >= sizeof (Idx) * 2);

      if (retVal == TRUE) {
         Clear ();
         LastPosition = tell (fdIdx);

         lseek (fdDat, Idx.Position, SEEK_SET);
         read (fdDat, &Usr, sizeof (Usr));

         if (fComplete == TRUE) {
            for (i = 0; i < Usr.LastReads; i++) {
               read (fdDat, &lastRead, sizeof (lastRead));
               strcpy (LastRead->Key, lastRead.Key);
               LastRead->Number = lastRead.Number;
               LastRead->QuickScan = lastRead.QuickScan;
               LastRead->PersonalOnly = lastRead.PersonalOnly;
               LastRead->ExcludeOwn = lastRead.ExcludeOwn;
               LastRead->Add ();
            }
            for (i = 0; i < Usr.FileTags; i++) {
               read (fdDat, &fTag, sizeof (fTag));
               FileTag->New ();
               strcpy (FileTag->Name, fTag.Name);
               strcpy (FileTag->Library, fTag.Library);
               FileTag->Size = fTag.Size;
               strcpy (FileTag->Complete, fTag.Complete);
               FileTag->DeleteAfter = fTag.DeleteAfter;
               FileTag->Add ();
            }
         }

         strcpy (Name, Usr.Name);
         Password = Usr.Password;
         strcpy (RealName, Usr.RealName);
         strcpy (Company, Usr.Company);
         strcpy (Address1, Usr.Address1);
         strcpy (Address2, Usr.Address2);
         strcpy (Address3, Usr.Address3);
         strcpy (DayPhone, Usr.DayPhone);
         Ansi = Usr.Ansi;
         Avatar = Usr.Avatar;
         Color = Usr.Color;
         HotKey = Usr.HotKey;
         ReadHeader = Usr.ReadHeader;
         System = Usr.System;
         Sex = Usr.Sex;
         BirthDate = Usr.BirthDate;
         ScreenHeight = Usr.ScreenHeight;
         ScreenWidth = Usr.ScreenWidth;
         Level = Usr.Level;
         AccessFlags = Usr.AccessFlags;
         DenyFlags = Usr.DenyFlags;
         CreationDate = Usr.CreationDate;
         LastCall = Usr.LastCall;
         TotalCalls = Usr.TotalCalls;
         TodayTime = Usr.TodayTime;
         WeekTime = Usr.WeekTime;
         MonthTime = Usr.MonthTime;
         YearTime = Usr.YearTime;
         strcpy (MailBox, Usr.MailBox);
         strcpy (LimitClass, Usr.LimitClass);
         strcpy (Language, Usr.Language);
         strcpy (FtpHost, Usr.FtpHost);
         strcpy (FtpName, Usr.FtpName);
         strcpy (FtpPwd, Usr.FtpPwd);
         Valid = Validate = TRUE;
      }
   }

   return (retVal);
}

USHORT TUser::Update (VOID)
{
   USHORT retVal = FALSE, newSize, LastReads, Tags;
   ULONG NameCrc;
   USER Usr;
   UINDEX Idx;
   LASTREAD lastRead;
   FTAGS fTag;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      LastPosition = 0L;
      NameCrc = StringCrc32 (Name);
      lseek (fdIdx, 0L, SEEK_SET);
      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && Idx.NameCrc == NameCrc) {
            retVal = TRUE;
            LastPosition = tell (fdIdx);
         }
      }

      if (LastPosition >= sizeof (Idx) && retVal == TRUE) {
         LastReads = 0;
         if (LastRead->First () == TRUE)
            do {
               LastReads++;
            } while (LastRead->Next () == TRUE);

         Tags = 0;
         if (FileTag->First () == TRUE)
            do {
               Tags++;
            } while (FileTag->Next () == TRUE);
                                                                                          
         newSize = (USHORT)(sizeof (Usr) + LastReads * sizeof (LASTREAD) + Tags * sizeof (FTAGS));
         if (newSize > Idx.Size) {
            if (Idx.Position + Idx.Size == filelength (fdDat))
               lseek (fdDat, Idx.Position, SEEK_SET);
            else
               lseek (fdDat, 0L, SEEK_END);
            Idx.Size = newSize;
         }
         else
            lseek (fdDat, Idx.Position, SEEK_SET);

         Idx.NameCrc = StringCrc32 (Name);
         Idx.RealNameCrc = StringCrc32 (RealName);
         Idx.Position = tell (fdDat);
         Idx.Flags = 0;

         memset (&Usr, 0, sizeof (Usr));
         Usr.Id = USER_ID;
         Usr.Size = newSize;
         strcpy (Usr.Name, Name);
         Usr.Password = Password;
         strcpy (Usr.RealName, RealName);
         strcpy (Usr.Company, Company);
         strcpy (Usr.Address1, Address1);
         strcpy (Usr.Address2, Address2);
         strcpy (Usr.Address3, Address3);
         strcpy (Usr.DayPhone, DayPhone);
         Usr.Ansi = Ansi;
         Usr.Avatar = Avatar;
         Usr.Color = Color;
         Usr.HotKey = HotKey;
         Usr.ReadHeader = ReadHeader;
         Usr.System = System;
         Usr.Sex = Sex;
         Usr.BirthDate = BirthDate;
         Usr.ScreenHeight = ScreenHeight;
         Usr.ScreenWidth = ScreenWidth;
         Usr.Level = Level;
         Usr.AccessFlags = AccessFlags;
         Usr.DenyFlags = DenyFlags;
         Usr.CreationDate = CreationDate;
         Usr.LastCall = LastCall;
         Usr.TotalCalls = TotalCalls;
         Usr.TodayTime = TodayTime;
         Usr.WeekTime = WeekTime;
         Usr.MonthTime = MonthTime;
         Usr.YearTime = YearTime;
         strcpy (Usr.MailBox, MailBox);
         strcpy (Usr.LimitClass, LimitClass);
         strcpy (Usr.Language, Language);
         strcpy (Usr.FtpHost, FtpHost);
         strcpy (Usr.FtpName, FtpName);
         strcpy (Usr.FtpPwd, FtpPwd);
         Usr.LastReads = LastReads;
         Usr.FileTags = Tags;

         write (fdDat, &Usr, sizeof (Usr));
         if (LastRead->First () == TRUE)
            do {
               memset (&lastRead, 0, sizeof (lastRead));
               strcpy (lastRead.Key, LastRead->Key);
               lastRead.Number = LastRead->Number;
               lastRead.QuickScan = LastRead->QuickScan;
               lastRead.PersonalOnly = LastRead->PersonalOnly;
               lastRead.ExcludeOwn = LastRead->ExcludeOwn;
               write (fdDat, &lastRead, sizeof (lastRead));
            } while (LastRead->Next () == TRUE);

         if (FileTag->First () == TRUE)
            do {
               memset (&fTag, 0, sizeof (fTag));
               fTag.Index = FileTag->Index;
               strcpy (fTag.Name, FileTag->Name);
               strcpy (fTag.Library, FileTag->Library);
               fTag.Size = FileTag->Size;
               strcpy (fTag.Complete, FileTag->Complete);
               fTag.DeleteAfter = FileTag->DeleteAfter;
               write (fdDat, &fTag, sizeof (fTag));
            } while (FileTag->Next () == TRUE);

         lseek (fdIdx, LastPosition - sizeof (Idx), SEEK_SET);
         write (fdIdx, &Idx, sizeof (Idx));
      }
   }

   if (fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
   if (fdIdx != -1) {
      close (fdIdx);
      fdIdx = -1;
   }

   return (retVal);
}



