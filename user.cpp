
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

/*
static ULONG cr3tab[] = {
   0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
   0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
   0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
   0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
   0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
   0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
   0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
   0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
   0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
   0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
   0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
   0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
   0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
   0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
   0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
   0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
   0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
   0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
   0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
   0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
   0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
   0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
   0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
   0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
   0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
   0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
   0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
   0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
   0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
   0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
   0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
   0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

static ULONG StringCrc32 (CHAR *pszString, ULONG ulCrc)
{
   while (*pszString) {
      ulCrc = (cr3tab[((ULONG)ulCrc ^ (UCHAR)*pszString) & 0xFF] ^ ((ulCrc >> 8) & 0x00FFFFFFL));
      pszString++;
   }

   return (ulCrc);
}
*/

TUser::TUser (void)
{
   strcpy (DatFile, "users.dat");
   strcpy (IdxFile, "users.idx");
   fdDat = fdIdx = -1;

   MsgTag = new TMsgTag;
   FileTag = new TFileTag;
}

TUser::TUser (PSZ pszUserFile)
{
   strcpy (DatFile, pszUserFile);
   strcat (DatFile, ".dat");
   strcpy (IdxFile, pszUserFile);
   strcat (IdxFile, ".idx");
   fdDat = fdIdx = -1;

   MsgTag = new TMsgTag (pszUserFile);
   FileTag = new TFileTag (pszUserFile);
}

TUser::~TUser (void)
{
   if (fdDat != -1)
      close (fdDat);
   if (fdIdx != -1)
      close (fdIdx);

   if (FileTag != NULL)
      delete FileTag;
   if (MsgTag != NULL)
      delete MsgTag;
}

VOID TUser::Struct2Class (VOID)
{
   strcpy (Name, Usr.Name);
   Password = Usr.Password;
   strcpy (RealName, Usr.RealName);
   strcpy (Company, Usr.Company);
   strcpy (Address, Usr.Address);
   strcpy (City, Usr.City);
   strcpy (DayPhone, Usr.DayPhone);
   Ansi = Usr.Ansi;
   Avatar = Usr.Avatar;
   Color = Usr.Color;
   HotKey = Usr.HotKey;
   Sex = Usr.Sex;
   FullEd = Usr.FullEd;
   FullReader = Usr.FullReader;
   NoDisturb = Usr.NoDisturb;
   AccessFailed = Usr.AccessFailed;
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
   strcpy (LastMsgArea, Usr.LastMsgArea);
   strcpy (LastFileArea, Usr.LastFileArea);
   UploadFiles = Usr.UploadFiles;
   UploadBytes = Usr.UploadBytes;
   DownloadFiles = Usr.DownloadFiles;
   DownloadBytes = Usr.DownloadBytes;
   FilesToday = Usr.FilesToday;
   BytesToday = Usr.BytesToday;
   ImportPOP3Mail = Usr.ImportPOP3Mail;
   UseInetAddress = Usr.UseInetAddress;
   strcpy (InetAddress, Usr.InetAddress);
   strcpy (Pop3Pwd, Usr.Pop3Pwd);
   strcpy (Archiver, Usr.Archiver);
   strcpy (Protocol, Usr.Protocol);
   strcpy (Signature, Usr.Signature);
   FullScreen = Usr.FullScreen;
   IBMChars = Usr.IBMChars;
   MorePrompt = Usr.MorePrompt;
   ScreenClear = Usr.ScreenClear;
   InUserList = Usr.InUserList;
   MailCheck = Usr.MailCheck;
   NewFileCheck = Usr.NewFileCheck;
   BirthDay = Usr.BirthDay;
   BirthMonth = Usr.BirthMonth;
   BirthYear = Usr.BirthYear;
   LastPwdChange = Usr.LastPwdChange;
}

VOID TUser::Class2Struct (VOID)
{
   memset (&Usr, 0, sizeof (Usr));
   Usr.Size = sizeof (Usr);
   strcpy (Usr.Name, Name);
   Usr.Password = Password;
   strcpy (Usr.RealName, RealName);
   strcpy (Usr.Company, Company);
   strcpy (Usr.Address, Address);
   strcpy (Usr.City, City);
   strcpy (Usr.DayPhone, DayPhone);
   Usr.Ansi = Ansi;
   Usr.Avatar = Avatar;
   Usr.Color = Color;
   Usr.HotKey = HotKey;
   Usr.Sex = Sex;
   Usr.FullEd = FullEd;
   Usr.FullReader = FullReader;
   Usr.NoDisturb = NoDisturb;
   Usr.AccessFailed = AccessFailed;
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
   strcpy (Usr.LastMsgArea, LastMsgArea);
   strcpy (Usr.LastFileArea, LastFileArea);
   Usr.UploadFiles = UploadFiles;
   Usr.UploadBytes = UploadBytes;
   Usr.DownloadFiles = DownloadFiles;
   Usr.DownloadBytes = DownloadBytes;
   Usr.FilesToday = FilesToday;
   Usr.BytesToday = BytesToday;
   Usr.ImportPOP3Mail = ImportPOP3Mail;
   Usr.UseInetAddress = UseInetAddress;
   strcpy (Usr.InetAddress, InetAddress);
   strcpy (Usr.Pop3Pwd, Pop3Pwd);
   strcpy (Usr.Archiver, Archiver);
   strcpy (Usr.Protocol, Protocol);
   strcpy (Usr.Signature, Signature);
   Usr.FullScreen = FullScreen;
   Usr.IBMChars = IBMChars;
   Usr.MorePrompt = MorePrompt;
   Usr.ScreenClear = ScreenClear;
   Usr.InUserList = InUserList;
   Usr.MailCheck = MailCheck;
   Usr.NewFileCheck = NewFileCheck;
   Usr.BirthDay = BirthDay;
   Usr.BirthMonth = BirthMonth;
   Usr.BirthYear = BirthYear;
   Usr.LastPwdChange = LastPwdChange;
}

USHORT TUser::Add (VOID)
{
   USHORT RetVal = FALSE;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdDat, 0L, SEEK_END);
      lseek (fdIdx, 0L, SEEK_END);

      memset (&Idx, 0, sizeof (Idx));
      Idx.Deleted = FALSE;
      Idx.NameCrc = StringCrc32 (Name, 0xFFFFFFFFL);
      Idx.RealNameCrc = StringCrc32 (RealName, 0xFFFFFFFFL);
      Idx.Position = tell (fdDat);

      Class2Struct ();

      write (fdDat, &Usr, sizeof (Usr));
      write (fdIdx, &Idx, sizeof (Idx));

      MsgTag->UserId = Idx.NameCrc;
      MsgTag->Load ();
      FileTag->UserId = Idx.NameCrc;
      FileTag->Load ();

      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TUser::Age (VOID)
{
   USHORT RetVal = 0;
   struct dosdate_t d_date;

   _dos_getdate (&d_date);

   if (BirthDay != 0 && BirthMonth != 0 && BirthYear > 1880 && BirthYear < d_date.year) {
      RetVal = (USHORT)(d_date.year - BirthYear);
      if (d_date.month < BirthMonth)
         RetVal--;
      else if (d_date.month == BirthMonth && d_date.day < BirthDay)
         RetVal--;
   }

   return (RetVal);
}

USHORT TUser::CheckPassword (PSZ pszPassword)
{
   USHORT RetVal = FALSE;

   if (Password == StringCrc32 (strupr (pszPassword), 0xFFFFFFFFL))
      RetVal = TRUE;

   return (RetVal);
}

VOID TUser::Clear (VOID)
{
   memset (&Usr, 0, sizeof (USER));
   Struct2Class ();

   MsgTag->Clear ();
   FileTag->Clear ();
}

USHORT TUser::Delete (VOID)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   ULONG NameCrc;

   if (fdDat == -1) {
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }
   if (fdIdx == -1) {
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1 && fdIdx != -1) {
      NameCrc = StringCrc32 (Name, 0xFFFFFFFFL);
      lseek (fdIdx, 0L, SEEK_SET);
      while (RetVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (Idx.Deleted == FALSE && Idx.NameCrc == NameCrc)
            RetVal = TRUE;
      }

      if (RetVal == TRUE) {
         Idx.Deleted = TRUE;
         lseek (fdIdx, tell (fdIdx) - sizeof (Idx), SEEK_SET);
         write (fdIdx, &Idx, sizeof (Idx));
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

   return (RetVal);
}

USHORT TUser::First (VOID)
{
   USHORT RetVal = FALSE;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);
      RetVal = Next ();
   }

   return (RetVal);
}

USHORT TUser::GetData (PSZ pszName, USHORT fCheckRealName)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   ULONG TestCrc;

   if (fdDat == -1) {
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }
   if (fdIdx == -1) {
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1 && fdIdx != -1) {
      Clear ();

      TestCrc = StringCrc32 (pszName, 0xFFFFFFFFL);

      lseek (fdIdx, 0L, SEEK_SET);
      while (RetVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (Idx.Deleted == FALSE && Idx.NameCrc == TestCrc)
            RetVal = TRUE;
      }
      if (RetVal == FALSE && fCheckRealName == TRUE) {
         lseek (fdIdx, 0L, SEEK_SET);
         while (RetVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
            if (Idx.Deleted == FALSE && Idx.RealNameCrc == TestCrc)
               RetVal = TRUE;
         }
      }
      if (RetVal == TRUE) {
         lseek (fdDat, Idx.Position, SEEK_SET);
         read (fdDat, &Usr, sizeof (Usr));

         Struct2Class ();

         MsgTag->UserId = Idx.NameCrc;
         MsgTag->Load ();
         FileTag->UserId = Idx.NameCrc;
         FileTag->Load ();
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

   return (RetVal);
}

USHORT TUser::Next (VOID)
{
   USHORT RetVal = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      while (RetVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (Idx.Deleted == FALSE)
            RetVal = TRUE;
      }

      if (RetVal == TRUE) {
         Clear ();

         if (tell (fdDat) != Idx.Position)
            lseek (fdDat, Idx.Position, SEEK_SET);
         read (fdDat, &Usr, sizeof (Usr));

         Struct2Class ();
      }
   }

   return (RetVal);
}

VOID TUser::SetPassword (PSZ pszPassword)
{
   Password = StringCrc32 (strupr (pszPassword), 0xFFFFFFFFL);
}

VOID TUser::Pack (VOID)
{
   int fdNew;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   fdNew = sopen ("users.new", O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdNew != -1 && fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);

      while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (Idx.Deleted == FALSE) {
            if (tell (fdDat) != Idx.Position)
               lseek (fdDat, Idx.Position, SEEK_SET);
            read (fdDat, &Usr, sizeof (Usr));
            write (fdNew, &Usr, sizeof (Usr));
         }
      }

      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);
      lseek (fdNew, 0L, SEEK_SET);

      while (read (fdNew, &Usr, sizeof (Usr)) == sizeof (Usr)) {
         memset (&Idx, 0, sizeof (Idx));
         Idx.Deleted = FALSE;
         Idx.NameCrc = StringCrc32 (Usr.Name, 0xFFFFFFFFL);
         Idx.RealNameCrc = StringCrc32 (Usr.RealName, 0xFFFFFFFFL);
         Idx.Position = tell (fdDat);

         write (fdDat, &Usr, sizeof (Usr));
         write (fdIdx, &Idx, sizeof (Idx));
      }

      chsize (fdDat, tell (fdDat));
      chsize (fdIdx, tell (fdIdx));
   }

   if (fdNew != -1) {
      close (fdNew);
      unlink ("users.new");
   }

   if (fdIdx != -1) {
      close (fdIdx);
      fdIdx = -1;
   }
   if (fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
}

USHORT TUser::Previous (VOID)
{
   USHORT RetVal = FALSE;

   if (fdDat == -1)
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1 && (tell (fdIdx) - sizeof (Idx)) >= sizeof (Idx)) {
      do {
         lseek (fdIdx, tell (fdIdx) - sizeof (Idx) * 2, SEEK_SET);
         if (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
            if (Idx.Deleted == FALSE)
               RetVal = TRUE;
         }
      } while (RetVal == FALSE && tell (fdIdx) >= sizeof (Idx) * 2);

      if (RetVal == TRUE) {
         Clear ();

         lseek (fdDat, Idx.Position, SEEK_SET);
         read (fdDat, &Usr, sizeof (Usr));

         Struct2Class ();
      }
   }

   return (RetVal);
}

VOID TUser::Reindex (VOID)
{
   ULONG Position;

   if (fdDat != -1)
      close (fdDat);
   if (fdIdx != -1)
      close (fdIdx);

   fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdDat, 0L, SEEK_SET);

      Position = tell (fdDat);
      while (read (fdDat, &Usr, sizeof (Usr)) == sizeof (Usr)) {
         if (Usr.Size == sizeof (Usr)) {
            memset (&Idx, 0, sizeof (Idx));
            Idx.Deleted = FALSE;
            Idx.NameCrc = StringCrc32 (Usr.Name, 0xFFFFFFFFL);
            Idx.RealNameCrc = StringCrc32 (Usr.RealName, 0xFFFFFFFFL);
            Idx.Position = Position;
            write (fdIdx, &Idx, sizeof (Idx));
         }
         Position = tell (fdDat);
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
}

USHORT TUser::Update (VOID)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   ULONG NameCrc;

   if (fdDat == -1) {
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }
   if (fdIdx == -1) {
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1 && fdIdx != -1) {
      NameCrc = StringCrc32 (Name, 0xFFFFFFFFL);
      lseek (fdIdx, 0L, SEEK_SET);
      while (RetVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (Idx.Deleted == FALSE && Idx.NameCrc == NameCrc)
            RetVal = TRUE;
      }

      if (RetVal == TRUE) {
         lseek (fdDat, Idx.Position, SEEK_SET);

         Class2Struct ();

         write (fdDat, &Usr, sizeof (Usr));

         MsgTag->UserId = Idx.NameCrc;
         MsgTag->Save ();
         FileTag->UserId = Idx.NameCrc;
         FileTag->Save ();
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

   return (RetVal);
}

// --------------------------------------------------------------------------

TMsgTag::TMsgTag (void)
{
   Data.Clear ();
   strcpy (DatFile, "msgtags.dat");
}

TMsgTag::TMsgTag (PSZ pszUserFile)
{
   CHAR *p;

   Data.Clear ();
   strcpy (DatFile, pszUserFile);

   p = &DatFile[strlen (DatFile)];
   while (*p != '\\' && *p != '/' && *p != ':' && p > DatFile)
      p--;
   if (*p == '\\' || *p == '/')
      p++;

   strcpy (p, "msgtags.dat");
}

TMsgTag::~TMsgTag (void)
{
   Data.Clear ();
}

VOID TMsgTag::Add (VOID)
{
   MSGTAGS Buffer;

   memset (&Buffer, 0, sizeof (MSGTAGS));

   Buffer.Free = FALSE;
   Buffer.Tagged = Tagged;
   Buffer.UserId = UserId;
   strcpy (Buffer.Area, Area);
   Buffer.LastRead = LastRead;
   Buffer.OlderMsg = OlderMsg;
   Data.Add (&Buffer, sizeof (MSGTAGS));
}

VOID TMsgTag::Change (PSZ pszOldName, PSZ pszNewName)
{
   int fd, i, Count, Changed;
   ULONG Position;
   MSGTAGS *Buffer;

   if ((fd = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if ((Buffer = (MSGTAGS *)malloc (sizeof (MSGTAGS) * MSGTAGS_INDEX)) != NULL) {
         do {
            Changed = FALSE;

            Position = tell (fd);
            Count = read (fd, Buffer, sizeof (MSGTAGS) * MSGTAGS_INDEX) / sizeof (MSGTAGS);
            for (i = 0; i < Count; i++) {
               if (Buffer[i].Free == FALSE && !stricmp (Buffer[i].Area, pszOldName)) {
                  strcpy (Buffer[i].Area, pszNewName);
                  Changed = TRUE;
               }
            }

            if (Changed == TRUE) {
               lseek (fd, Position, SEEK_SET);
               write (fd, Buffer, sizeof (MSGTAGS) * Count);
            }
         } while (Count == MSGTAGS_INDEX);
         free (Buffer);
      }

      close (fd);
   }
}

VOID TMsgTag::Clear (VOID)
{
   Data.Clear ();

   Tagged = FALSE;
   Area[0] = '\0';
   LastRead = 0L;
   OlderMsg = 0L;
}

USHORT TMsgTag::First (VOID)
{
   USHORT RetVal = FALSE;
   MSGTAGS *Buffer;

   if ((Buffer = (MSGTAGS *)Data.First ()) != NULL) {
      Tagged = Buffer->Tagged;
      strcpy (Area, Buffer->Area);
      LastRead = Buffer->LastRead;
      OlderMsg = Buffer->OlderMsg;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TMsgTag::Load (VOID)
{
   int fd, i, Count;
   MSGTAGS *Buffer;

   Data.Clear ();

   if ((fd = sopen (DatFile, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if ((Buffer = (MSGTAGS *)malloc (sizeof (MSGTAGS) * MSGTAGS_INDEX)) != NULL) {
         do {
            Count = read (fd, Buffer, sizeof (MSGTAGS) * MSGTAGS_INDEX) / sizeof (MSGTAGS);
            for (i = 0; i < Count; i++) {
               if (Buffer[i].Free == FALSE && Buffer[i].UserId == UserId)
                  Data.Add (&Buffer[i], sizeof (MSGTAGS));
            }
         } while (Count == MSGTAGS_INDEX);
         free (Buffer);
      }
      close (fd);
   }

   if ((Buffer = (MSGTAGS *)Data.First ()) != NULL) {
      Tagged = Buffer->Tagged;
      strcpy (Area, Buffer->Area);
      LastRead = Buffer->LastRead;
      OlderMsg = Buffer->OlderMsg;
   }
}

VOID TMsgTag::New (VOID)
{
   Tagged = FALSE;
   Area[0] = '\0';
   LastRead = 0L;
   OlderMsg = 0L;
}

USHORT TMsgTag::Next (VOID)
{
   USHORT RetVal = FALSE;
   MSGTAGS *Buffer;

   if ((Buffer = (MSGTAGS *)Data.Next ()) != NULL) {
      Tagged = Buffer->Tagged;
      strcpy (Area, Buffer->Area);
      LastRead = Buffer->LastRead;
      OlderMsg = Buffer->OlderMsg;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TMsgTag::Previous (VOID)
{
   USHORT RetVal = FALSE;
   MSGTAGS *Buffer;

   if ((Buffer = (MSGTAGS *)Data.Previous ()) != NULL) {
      Tagged = Buffer->Tagged;
      strcpy (Area, Buffer->Area);
      LastRead = Buffer->LastRead;
      OlderMsg = Buffer->OlderMsg;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TMsgTag::Read (PSZ pszArea)
{
   USHORT RetVal = FALSE;

   if (First () == TRUE)
      do {
         if (!stricmp (pszArea, Area)) {
            RetVal = TRUE;
            break;
         }
      } while (Next () == TRUE);

   return (RetVal);
}

VOID TMsgTag::Save (VOID)
{
   int fd, i, Count, Changed;
   ULONG Position;
   MSGTAGS *Buffer, *Record;

   if ((fd = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      Record = (MSGTAGS *)Data.First ();

      if ((Buffer = (MSGTAGS *)malloc (sizeof (MSGTAGS) * MSGTAGS_INDEX)) != NULL) {
         do {
            Changed = FALSE;

            Position = tell (fd);
            Count = read (fd, Buffer, sizeof (MSGTAGS) * MSGTAGS_INDEX) / sizeof (MSGTAGS);
            for (i = 0; i < Count && Record != NULL; i++) {
               if (Buffer[i].UserId == Record->UserId || Buffer[i].Free == TRUE) {
                  memcpy (&Buffer[i], Record, sizeof (MSGTAGS));
                  Buffer[i].Free = FALSE;
                  Record = (MSGTAGS *)Data.Next ();
                  Changed = TRUE;
               }
            }

            for (; i < Count; i++) {
               if (Buffer[i].UserId == UserId) {
                  memset (&Buffer[i], 0, sizeof (MSGTAGS));
                  Buffer[i].Free = TRUE;
                  Changed = TRUE;
               }
            }

            if (Changed == TRUE) {
               lseek (fd, Position, SEEK_SET);
               write (fd, Buffer, sizeof (MSGTAGS) * Count);
            }
         } while (Count == MSGTAGS_INDEX);
         free (Buffer);
      }

      if (Record != NULL) {
         do {
            write (fd, Record, sizeof (MSGTAGS));
         } while ((Record = (MSGTAGS *)Data.Next ()) != NULL);
      }

      close (fd);
   }
}

VOID TMsgTag::Update (VOID)
{
   MSGTAGS Buffer;

   if (Data.Value () != NULL) {
      memcpy (&Buffer, Data.Value (), sizeof (MSGTAGS));
      Buffer.Tagged = Tagged;
      strcpy (Buffer.Area, Area);
      Buffer.LastRead = LastRead;
      Buffer.OlderMsg = OlderMsg;
      memcpy (Data.Value (), &Buffer, sizeof (MSGTAGS));
   }
}

// --------------------------------------------------------------------------

TFileTag::TFileTag (void)
{
   TotalFiles = 0;
   TotalBytes = 0L;
   Data.Clear ();

   strcpy (DatFile, "filetags.dat");
   New ();
}

TFileTag::TFileTag (PSZ pszUserFile)
{
   CHAR *p;

   TotalFiles = 0;
   TotalBytes = 0L;
   Data.Clear ();

   strcpy (DatFile, pszUserFile);
   New ();

   p = &DatFile[strlen (DatFile)];
   while (*p != '\\' && *p != '/' && *p != ':' && p > DatFile)
      p--;
   if (*p == '\\' || *p == '/')
      p++;

   strcpy (p, "filetags.dat");
}

TFileTag::~TFileTag (void)
{
   Data.Clear ();
}

USHORT TFileTag::Add (VOID)
{
   USHORT RetVal = FALSE;
   FILETAGS Buffer;

   memset (&Buffer, 0, sizeof (FILETAGS));

   Buffer.Free = FALSE;
   Buffer.UserId = UserId;
   strcpy (Buffer.Area, Area);
   strcpy (Buffer.Name, Name);
   strcpy (Buffer.Complete, Complete);
   Buffer.Size = Size;
   Buffer.DeleteAfter = DeleteAfter;
   Buffer.CdRom = CdRom;
   Buffer.Index = Index = (USHORT)(Data.Elements + 1);

   if ((RetVal = Data.Add (&Buffer, sizeof (FILETAGS))) == TRUE) {
      TotalFiles++;
      TotalBytes += Size;
   }

   return (RetVal);
}

USHORT TFileTag::Check (PSZ pszName)
{
   USHORT RetVal = FALSE;
   FILETAGS *ft;

   if ((ft = (FILETAGS *)Data.First ()) != NULL)
      do {
         if (!stricmp (ft->Name, pszName)) {
            strcpy (Area, ft->Area);
            strcpy (Name, ft->Name);
            strcpy (Complete, ft->Complete);
            Size = ft->Size;
            DeleteAfter = ft->DeleteAfter;
            CdRom = ft->CdRom;
            Index = ft->Index;
            RetVal = TRUE;
         }
      } while (RetVal == FALSE && (ft = (FILETAGS *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TFileTag::Clear (VOID)
{
   Data.Clear ();
   New ();
   Index = 0;
   TotalFiles = 0;
}

USHORT TFileTag::First (VOID)
{
   USHORT RetVal = FALSE;
   FILETAGS *Buffer;

   if ((Buffer = (FILETAGS *)Data.First ()) != NULL) {
      strcpy (Area, Buffer->Area);
      strcpy (Name, Buffer->Name);
      strcpy (Complete, Buffer->Complete);
      Size = Buffer->Size;
      DeleteAfter = Buffer->DeleteAfter;
      CdRom = Buffer->CdRom;
      Index = Buffer->Index;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TFileTag::Load (VOID)
{
   int fd, i, Count;
   FILETAGS *Buffer;

   Data.Clear ();

   if ((fd = sopen (DatFile, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if ((Buffer = (FILETAGS *)malloc (sizeof (FILETAGS) * FILETAGS_INDEX)) != NULL) {
         do {
            Count = read (fd, Buffer, sizeof (FILETAGS) * FILETAGS_INDEX) / sizeof (FILETAGS);
            for (i = 0; i < Count; i++) {
               if (Buffer[i].Free == FALSE && Buffer[i].UserId == UserId) {
                  Buffer[i].Index = Index = (USHORT)(Data.Elements + 1);
                  if (Data.Add (&Buffer[i], sizeof (FILETAGS)) == TRUE) {
                     TotalFiles++;
                     TotalBytes += Buffer[i].Size;
                  }
               }
            }
         } while (Count == FILETAGS_INDEX);
         free (Buffer);
      }
      close (fd);
   }

   if ((Buffer = (FILETAGS *)Data.First ()) != NULL) {
      strcpy (Area, Buffer->Area);
      strcpy (Name, Buffer->Name);
      strcpy (Complete, Buffer->Complete);
      Size = Buffer->Size;
      DeleteAfter = Buffer->DeleteAfter;
      CdRom = Buffer->CdRom;
      Index = Buffer->Index;
   }
}

VOID TFileTag::New (VOID)
{
   Area[0] = '\0';
   Name[0] = '\0';
   Complete[0] = '\0';
   Size = 0L;
   CdRom = DeleteAfter = FALSE;
   Index = (USHORT)(Data.Elements + 1);
}

USHORT TFileTag::Next (VOID)
{
   USHORT RetVal = FALSE;
   FILETAGS *Buffer;

   if ((Buffer = (FILETAGS *)Data.Next ()) != NULL) {
      strcpy (Area, Buffer->Area);
      strcpy (Name, Buffer->Name);
      strcpy (Complete, Buffer->Complete);
      Size = Buffer->Size;
      DeleteAfter = Buffer->DeleteAfter;
      CdRom = Buffer->CdRom;
      Index = Buffer->Index;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TFileTag::Previous (VOID)
{
   USHORT RetVal = FALSE;
   FILETAGS *Buffer;

   if ((Buffer = (FILETAGS *)Data.Previous ()) != NULL) {
      strcpy (Area, Buffer->Area);
      strcpy (Name, Buffer->Name);
      strcpy (Complete, Buffer->Complete);
      Size = Buffer->Size;
      DeleteAfter = Buffer->DeleteAfter;
      CdRom = Buffer->CdRom;
      Index = Buffer->Index;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TFileTag::Reindex (VOID)
{
   USHORT LastIndex;
   FILETAGS *ft;

   LastIndex = 1;
   if ((ft = (FILETAGS *)Data.First ()) != NULL)
      do {
         ft->Index = LastIndex++;
      } while ((ft = (FILETAGS *)Data.Next ()) != NULL);
}

VOID TFileTag::Remove (PSZ pszName)
{
   USHORT RetVal = FALSE;
   FILETAGS *ft, *Buffer;

   if (pszName != NULL) {
      if ((ft = (FILETAGS *)Data.First ()) != NULL) {
         do {
            if (!stricmp (ft->Name, pszName))
               RetVal = TRUE;
         } while (RetVal == FALSE && (ft = (FILETAGS *)Data.Next ()) != NULL);
      }
      if (RetVal == TRUE) {
         if (ft->DeleteAfter == TRUE)
            unlink (ft->Complete);
         Data.Remove ();
         TotalFiles--;
         TotalBytes -= ft->Size;
      }
   }
   else {
      if (DeleteAfter == TRUE)
         unlink (Complete);
      Data.Remove ();
      TotalFiles--;
      TotalBytes -= Size;
   }

   if ((Buffer = (FILETAGS *)Data.Value ()) != NULL) {
      strcpy (Area, Buffer->Area);
      strcpy (Name, Buffer->Name);
      strcpy (Complete, Buffer->Complete);
      Size = Buffer->Size;
      DeleteAfter = Buffer->DeleteAfter;
      CdRom = Buffer->CdRom;
      Index = Buffer->Index;
   }
}

USHORT TFileTag::Select (USHORT usIndex)
{
   USHORT RetVal = FALSE;
   FILETAGS *ft;

   if ((ft = (FILETAGS *)Data.First ()) != NULL)
      do {
         if (ft->Index == usIndex) {
            strcpy (Area, ft->Area);
            strcpy (Name, ft->Name);
            strcpy (Complete, ft->Complete);
            Size = ft->Size;
            DeleteAfter = ft->DeleteAfter;
            CdRom = ft->CdRom;
            Index = ft->Index;
            RetVal = TRUE;
         }
      } while (RetVal == FALSE && (ft = (FILETAGS *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TFileTag::Save (VOID)
{
   int fd, i, Count, Changed;
   ULONG Position;
   FILETAGS *Buffer, *Record;

   if ((fd = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      Record = (FILETAGS *)Data.First ();

      if ((Buffer = (FILETAGS *)malloc (sizeof (FILETAGS) * FILETAGS_INDEX)) != NULL) {
         do {
            Changed = FALSE;

            Position = tell (fd);
            Count = read (fd, Buffer, sizeof (FILETAGS) * FILETAGS_INDEX) / sizeof (FILETAGS);
            for (i = 0; i < Count && Record != NULL; i++) {
               if (Buffer[i].UserId == Record->UserId || Buffer[i].Free == TRUE) {
                  memcpy (&Buffer[i], Record, sizeof (FILETAGS));
                  Record = (FILETAGS *)Data.Next ();
                  Changed = TRUE;
               }
            }

            for (; i < Count; i++) {
               if (Buffer[i].UserId == UserId) {
                  memset (&Buffer[i], 0, sizeof (FILETAGS));
                  Buffer[i].Free = TRUE;
                  Changed = TRUE;
               }
            }

            if (Changed == TRUE) {
               lseek (fd, Position, SEEK_SET);
               write (fd, Buffer, sizeof (FILETAGS) * Count);
            }
         } while (Count == FILETAGS_INDEX);
         free (Buffer);
      }

      if (Record != NULL) {
         do {
            write (fd, Record, sizeof (FILETAGS));
         } while ((Record = (FILETAGS *)Data.Next ()) != NULL);
      }

      close (fd);
   }
}

VOID TFileTag::Update (VOID)
{
   FILETAGS *Buffer;

   if ((Buffer = (FILETAGS *)Data.Value ()) != NULL) {
      TotalBytes -= Buffer->Size;
      strcpy (Buffer->Area, Area);
      strcpy (Buffer->Name, Name);
      strcpy (Buffer->Complete, Complete);
      Buffer->Size = Size;
      Buffer->DeleteAfter = DeleteAfter;
      Buffer->CdRom = CdRom;
      TotalBytes += Size;
   }
}

