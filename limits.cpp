
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/03/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

TLimits::TLimits (void)
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

TLimits::~TLimits (void)
{
   if (fdDat != -1)
      close (fdDat);
}

USHORT TLimits::Add (VOID)
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
      strcpy (Limits.Key, Key);
      strcpy (Limits.Description, Description);
      Limits.Level = Level;
      Limits.Flags = Flags;
      Limits.DenyFlags = DenyFlags;
      Limits.ExpireDays = ExpireDays;
      strcpy (Limits.ExpireClass, ExpireClass);
      Limits.ExpireLevel = ExpireLevel;
      Limits.ExpireFlags = ExpireFlags;
      Limits.ExpireDenyFlags = ExpireDenyFlags;
      Limits.CallTimeLimit = CallTimeLimit;
      Limits.DayTimeLimit = DayTimeLimit;
      Limits.WeekTimeLimit = WeekTimeLimit;
      Limits.MonthTimeLimit = MonthTimeLimit;
      Limits.YearTimeLimit = YearTimeLimit;
      Limits.CallDownloadLimit = CallDownloadLimit;
      Limits.DayDownloadLimit = DayDownloadLimit;
      Limits.WeekDownloadLimit = WeekDownloadLimit;
      Limits.MonthDownloadLimit = MonthDownloadLimit;
      Limits.YearDownloadLimit = YearDownloadLimit;
      Limits.InactivityTime = InactivityTime;
      Limits.ShowLevel = ShowLevel;
      Limits.ShowFlags = ShowFlags;
      Limits.ShowDenyFlags = ShowDenyFlags;

      write (fdDat, &Limits, sizeof (Limits));
      retVal = TRUE;
   }

   if (closeFile == TRUE && fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }

   return (retVal);
}

VOID TLimits::New (VOID)
{
   memset (Description, 0, sizeof (Description));
   memset (Key, 0, sizeof (Key));
   ExpireDays = 0;
   memset (ExpireClass, 0, sizeof (ExpireClass));
   ExpireLevel = 0;
   ExpireFlags = 0L;
   ExpireDenyFlags = 0L;
   CallTimeLimit = DayTimeLimit = WeekTimeLimit = MonthTimeLimit = 0;
   YearTimeLimit = 0L;
   CallDownloadLimit = DayDownloadLimit = WeekDownloadLimit = 0L;
   MonthDownloadLimit = YearDownloadLimit = 0L;
   InactivityTime = 0;
   DenyFlags = Flags = 0L;
   Level = 0;
   ShowLevel = 0;
   ShowFlags = 0L;
   ShowDenyFlags = 0L;
}

VOID TLimits::Delete (VOID)
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

USHORT TLimits::First (VOID)
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

USHORT TLimits::Next (VOID)
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
         ExpireDays = Limits.ExpireDays;
         strcpy (ExpireClass, Limits.ExpireClass);
         ExpireLevel = Limits.ExpireLevel;
         ExpireFlags = Limits.ExpireFlags;
         ExpireDenyFlags = Limits.ExpireDenyFlags;
         CallTimeLimit = Limits.CallTimeLimit;
         DayTimeLimit = Limits.DayTimeLimit;
         WeekTimeLimit = Limits.WeekTimeLimit;
         MonthTimeLimit = Limits.MonthTimeLimit;
         YearTimeLimit = Limits.YearTimeLimit;
         CallDownloadLimit = Limits.CallDownloadLimit;
         DayDownloadLimit = Limits.DayDownloadLimit;
         WeekDownloadLimit = Limits.WeekDownloadLimit;
         MonthDownloadLimit = Limits.MonthDownloadLimit;
         YearDownloadLimit = Limits.YearDownloadLimit;
         InactivityTime = Limits.InactivityTime;
         Flags = Limits.Flags;
         ShowLevel = Limits.ShowLevel;
         ShowFlags = Limits.ShowFlags;
         ShowDenyFlags = Limits.ShowDenyFlags;
         retVal = TRUE;
      }
   }

   return (retVal);
}

USHORT TLimits::Previous (VOID)
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
         ExpireDays = Limits.ExpireDays;
         strcpy (ExpireClass, Limits.ExpireClass);
         ExpireLevel = Limits.ExpireLevel;
         ExpireFlags = Limits.ExpireFlags;
         ExpireDenyFlags = Limits.ExpireDenyFlags;
         CallTimeLimit = Limits.CallTimeLimit;
         DayTimeLimit = Limits.DayTimeLimit;
         WeekTimeLimit = Limits.WeekTimeLimit;
         MonthTimeLimit = Limits.MonthTimeLimit;
         YearTimeLimit = Limits.YearTimeLimit;
         CallDownloadLimit = Limits.CallDownloadLimit;
         DayDownloadLimit = Limits.DayDownloadLimit;
         WeekDownloadLimit = Limits.WeekDownloadLimit;
         MonthDownloadLimit = Limits.MonthDownloadLimit;
         YearDownloadLimit = Limits.YearDownloadLimit;
         InactivityTime = Limits.InactivityTime;
         Flags = Limits.Flags;
         ShowLevel = Limits.ShowLevel;
         ShowFlags = Limits.ShowFlags;
         ShowDenyFlags = Limits.ShowDenyFlags;
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
            ExpireDays = Limits.ExpireDays;
            strcpy (ExpireClass, Limits.ExpireClass);
            ExpireLevel = Limits.ExpireLevel;
            ExpireFlags = Limits.ExpireFlags;
            ExpireDenyFlags = Limits.ExpireDenyFlags;
            CallTimeLimit = Limits.CallTimeLimit;
            DayTimeLimit = Limits.DayTimeLimit;
            WeekTimeLimit = Limits.WeekTimeLimit;
            MonthTimeLimit = Limits.MonthTimeLimit;
            YearTimeLimit = Limits.YearTimeLimit;
            CallDownloadLimit = Limits.CallDownloadLimit;
            DayDownloadLimit = Limits.DayDownloadLimit;
            WeekDownloadLimit = Limits.WeekDownloadLimit;
            MonthDownloadLimit = Limits.MonthDownloadLimit;
            YearDownloadLimit = Limits.YearDownloadLimit;
            InactivityTime = Limits.InactivityTime;
            Flags = Limits.Flags;
            ShowLevel = Limits.ShowLevel;
            ShowFlags = Limits.ShowFlags;
            ShowDenyFlags = Limits.ShowDenyFlags;
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

USHORT TLimits::Update (VOID)
{
   USHORT retVal = FALSE, closeFile = FALSE;
   LIMITS Limits;

   if (fdDat == -1) {
      fdDat = sopen (DatFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      closeFile = TRUE;
   }

   if (fdDat != -1) {
      while (retVal == FALSE && read (fdDat, &Limits, sizeof (Limits)) == sizeof (Limits)) {
         if (!stricmp (Key, Limits.Key)) {
            lseek (fdDat, tell (fdDat) - sizeof (Limits), SEEK_SET);
            memset (&Limits, 0, sizeof (Limits));
            strcpy (Limits.Key, Key);
            strcpy (Limits.Description, Description);
            Limits.Level = Level;
            Limits.Flags = Flags;
            Limits.DenyFlags = DenyFlags;
            Limits.ExpireDays = ExpireDays;
            strcpy (Limits.ExpireClass, ExpireClass);
            Limits.ExpireLevel = ExpireLevel;
            Limits.ExpireFlags = ExpireFlags;
            Limits.ExpireDenyFlags = ExpireDenyFlags;
            Limits.CallTimeLimit = CallTimeLimit;
            Limits.DayTimeLimit = DayTimeLimit;
            Limits.WeekTimeLimit = WeekTimeLimit;
            Limits.MonthTimeLimit = MonthTimeLimit;
            Limits.YearTimeLimit = YearTimeLimit;
            Limits.CallDownloadLimit = CallDownloadLimit;
            Limits.DayDownloadLimit = DayDownloadLimit;
            Limits.WeekDownloadLimit = WeekDownloadLimit;
            Limits.MonthDownloadLimit = MonthDownloadLimit;
            Limits.YearDownloadLimit = YearDownloadLimit;
            Limits.InactivityTime = InactivityTime;
            Limits.Flags = Flags;
            Limits.ShowLevel = ShowLevel;
            Limits.ShowFlags = ShowFlags;
            Limits.ShowDenyFlags = ShowDenyFlags;
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

