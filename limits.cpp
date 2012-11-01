
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "user.h"

TLimits::TLimits (void)
{
   fdDat = -1;
   Clear ();
   DataPath[0] = '\0';
}

TLimits::TLimits (PSZ pszDataPath)
{
   fdDat = -1;
   Clear ();
   strcpy (DataPath, pszDataPath);
   if (DataPath[strlen (DataPath) - 1] != '\\')
      strcat (DataPath, "\\");
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
      sprintf (Temp, "%sLimits.Dat", DataPath);
      fdDat = open (Temp, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
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

VOID TLimits::Clear (VOID)
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
}

USHORT TLimits::First (VOID)
{
   USHORT retVal = FALSE;

   if (fdDat == -1) {
      sprintf (Temp, "%sLimits.Dat", DataPath);
      fdDat = open (Temp, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   }

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

   Clear ();

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

   Clear ();

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

   Clear ();

   if (fdDat == -1) {
      sprintf (Temp, "%sLimits.Dat", DataPath);
      fdDat = open (Temp, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
   }

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
      sprintf (Temp, "%sLimits.Dat", DataPath);
      fdDat = open (Temp, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
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

