
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.09
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/06/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "schedule.h"

TScheduler::TScheduler (void)
{
   strcpy (DataFile, "Events.Dat");
   fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
}

TScheduler::TScheduler (PSZ pszDataPath)
{
   strcpy (DataFile, pszDataPath);
   if (DataFile[0] != '\0' && DataFile[strlen (DataFile) - 1] != '\\')
      strcat (DataFile, "\\");
   strcat (DataFile, "Events.Dat");
   fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
}

TScheduler::~TScheduler (void)
{
   if (fd != -1)
      close (fd);
}

VOID TScheduler::Add (VOID)
{
   int fdn;
   USHORT Added = FALSE;
   ULONG Position;

   fdn = sopen ("Events.New", O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYRW, S_IREAD|S_IWRITE);

   if (fd != -1 && fdn != -1) {
      lseek (fd, 0L, SEEK_SET);
      while (read (fd, &Temp, sizeof (Temp)) == sizeof (Temp)) {
         if (Added == FALSE) {
            if (Temp.Hour != Hour) {
               if (Temp.Hour > Hour)
                  Added = TRUE;
            }
            else {
               if (Temp.Minute > Minute)
                  Added = TRUE;
            }
            if (Added == TRUE) {
               Position = tell (fdn);
               Class2Struct ();
               write (fdn, &Temp, sizeof (Temp));

               lseek (fd, tell (fd) - sizeof (Temp), SEEK_SET);
               read (fd, &Temp, sizeof (Temp));
            }
         }

         write (fdn, &Temp, sizeof (Temp));
      }

      if (Added == FALSE) {
         Position = tell (fdn);
         Class2Struct ();
         write (fdn, &Temp, sizeof (Temp));
      }

      lseek (fd, 0L, SEEK_SET);
      lseek (fdn, 0L, SEEK_SET);

      while (read (fdn, &Temp, sizeof (Temp)) == sizeof (Temp))
         write (fd, &Temp, sizeof (Temp));

      lseek (fd, Position, SEEK_SET);
      read (fd, &Temp, sizeof (Temp));
      Struct2Class ();
   }

   if (fdn != -1) {
      close (fdn);
      unlink ("Events.New");
   }
}

VOID TScheduler::Class2Struct (VOID)
{
   memset (&Temp, 0, sizeof (Temp));

   strcpy (Temp.Label, Label);
   Temp.Day = Day;
   Temp.Month = Month;
   Temp.Year = Year;
   Temp.Length = Length;
   Temp.Hour = Hour;
   Temp.Minute = Minute;
   Temp.Force = Force;
   Temp.RepDay = RepDay;
   Temp.RepMonth = RepMonth;
   Temp.RepYear = RepYear;
   Temp.RepHour = RepHour;
   Temp.RepMinute = RepMinute;
   Temp.Channel = Channel;

   Temp.Started = Started;

   Temp.CallNode = CallNode;
   Temp.ForceCall = ForceCall;
   strcpy (Temp.Address, Address);
   Temp.Delay = Delay;
   Temp.Retries = Retries;
   Temp.Failures = Failures;

   Temp.ImportMail = ImportMail;
   Temp.ExportMail = ExportMail;
   Temp.StartImport = StartImport;
   Temp.StartExport = StartExport;
   Temp.EndImport = EndImport;
   Temp.EndExport = EndExport;

   Temp.UserMaint = UserMaint;
   Temp.MessageMaint = MessageMaint;
   Temp.UpdateNewsgroups = UpdateNewsgroups;
   strcpy (Temp.External, External);
}

VOID TScheduler::Delete (VOID)
{
   int fdn;
   ULONG Position;

   fdn = sopen ("Events.New", O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYRW, S_IREAD|S_IWRITE);

   if (fdn != -1 && fd != -1) {
      Position = tell (fd);
      lseek (fd, 0L, SEEK_SET);
      while (read (fd, &Temp, sizeof (Temp)) == sizeof (Temp)) {
         if (Position != tell (fd))
            write (fdn, &Temp, sizeof (Temp));
      }

      lseek (fd, 0L, SEEK_SET);
      lseek (fdn, 0L, SEEK_SET);

      while (read (fdn, &Temp, sizeof (Temp)) == sizeof (Temp))
         write (fd, &Temp, sizeof (Temp));

      chsize (fd, tell (fd));
      if (Position > tell (fd))
         Position = tell (fd);

      if (Position >= sizeof (Temp)) {
         lseek (fd, Position - sizeof (Temp), SEEK_SET);
         read (fd, &Temp, sizeof (Temp));
         Struct2Class ();
      }
      else
         New ();
   }

   if (fdn != -1) {
      close (fdn);
      unlink ("Events.New");
   }
}

VOID TScheduler::Executed (USHORT flUpdate)
{
   USHORT RetVal;
   struct dosdate_t date;
   struct dostime_t time;
   struct tm mktm;

   _dos_getdate (&date);
   _dos_gettime (&time);

   if (fd != -1 || flUpdate == FALSE) {
      RetVal = FALSE;
      memset (&mktm, 0, sizeof (mktm));
      if (RepDay == 0)
         RepDay = 1;
      mktm.tm_mday = date.day + RepDay;
      mktm.tm_mon = date.month + RepMonth - 1;
      mktm.tm_year = date.year + RepYear - 1900;
      mktime (&mktm);

      Day = (UCHAR)mktm.tm_mday;
      Month = (UCHAR)(mktm.tm_mon + 1);
      Year = (USHORT)(mktm.tm_year + 1900);

      if (flUpdate == TRUE) {
         Class2Struct ();
         lseek (fd, tell (fd) - sizeof (Temp), SEEK_SET);
         write (fd, &Temp, sizeof (Temp));
      }
   }
}

USHORT TScheduler::First (VOID)
{
   USHORT RetVal = FALSE;

   if (fd != -1) {
      lseek (fd, 0L, SEEK_SET);
      RetVal = Next ();
   }

   return (RetVal);
}

VOID TScheduler::New (VOID)
{
   memset (Label, 0, sizeof (Label));
   Day = Month = 0;
   Year = Length = 0;
   Hour = Minute = 0;
   Force = RepDay = RepMonth = 0;
   RepYear = 0;
   RepHour = RepMinute = 0;
   Channel = 0;

   Started = FALSE;

   CallNode = ForceCall = FALSE;
   memset (Address, 0, sizeof (Address));
   Delay = Retries = Failures = 0;

   ImportMail = ExportMail = StartImport = FALSE;
   StartExport = EndImport = EndExport = FALSE;

   UserMaint = MessageMaint = UpdateNewsgroups = FALSE;
   memset (External, 0, sizeof (External));
}

USHORT TScheduler::Next (VOID)
{
   USHORT RetVal = FALSE;

   if (fd != -1) {
      if (read (fd, &Temp, sizeof (Temp)) == sizeof (Temp)) {
         Struct2Class ();
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT TScheduler::Previous (VOID)
{
   USHORT RetVal = FALSE;

   if (fd != -1 && tell (fd) >= sizeof (Temp) * 2L) {
      lseek (fd, tell (fd) - sizeof (Temp) * 2L, SEEK_SET);
      if (read (fd, &Temp, sizeof (Temp)) == sizeof (Temp)) {
         Struct2Class ();
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT TScheduler::ReadCurrent (USHORT usChannel)
{
   USHORT RetVal = FALSE;

   if (First () == TRUE)
      do {
         if (usChannel == 0 || usChannel == Channel) {
            if (ShouldExecute () == TRUE)
               RetVal = TRUE;
         }
      } while (RetVal == FALSE && Next () == TRUE);

   return (RetVal);
}

USHORT TScheduler::ShouldExecute (VOID)
{
   USHORT RetVal = FALSE;
   struct dosdate_t date;
   struct dostime_t time;
   struct tm mktm1, mktm2;

   _dos_getdate (&date);
   _dos_gettime (&time);

   memset (&mktm1, 0, sizeof (mktm1));
   mktm1.tm_min = time.minute;
   mktm1.tm_hour = time.hour;
   mktm1.tm_mday = date.day;
   mktm1.tm_mon = date.month - 1;
   mktm1.tm_year = date.year - 1900;

   Day = date.day;
   Month = date.month;
   Year = date.year;

   memset (&mktm2, 0, sizeof (mktm2));
   mktm2.tm_min = Minute;
   mktm2.tm_hour = Hour;
   mktm2.tm_mday = Day;
   mktm2.tm_mon = Month - 1;
   mktm2.tm_year = Year - 1900;

   if (Force == TRUE) {
      if (mktime (&mktm1) >= mktime (&mktm2))
         RetVal = TRUE;
   }
   else {
      if (mktime (&mktm1) >= mktime (&mktm2)) {
         mktm2.tm_min += Length;
         if (mktime (&mktm1) < mktime (&mktm2))
            RetVal = TRUE;
      }
   }

   return (RetVal);
}

VOID TScheduler::Struct2Class (VOID)
{
   strcpy (Label, Temp.Label);
   Day = Temp.Day;
   Month = Temp.Month;
   Year = Temp.Year;
   Length = Temp.Length;
   Hour = Temp.Hour;
   Minute = Temp.Minute;
   Force = Temp.Force;
   RepDay = Temp.RepDay;
   RepMonth = Temp.RepMonth;
   RepYear = Temp.RepYear;
   RepHour = Temp.RepHour;
   RepMinute = Temp.RepMinute;
   Channel = Temp.Channel;

   Started = Temp.Started;

   CallNode = Temp.CallNode;
   ForceCall = Temp.ForceCall;
   strcpy (Address, Temp.Address);
   Delay = Temp.Delay;
   Retries = Temp.Retries;
   Failures = Temp.Failures;

   ImportMail = Temp.ImportMail;
   ExportMail = Temp.ExportMail;
   StartImport = Temp.StartImport;
   StartExport = Temp.StartExport;
   EndImport = Temp.EndImport;
   EndExport = Temp.EndExport;

   UserMaint = Temp.UserMaint;
   MessageMaint = Temp.MessageMaint;
   UpdateNewsgroups = Temp.UpdateNewsgroups;
   strcpy (External, Temp.External);
}

LONG TScheduler::TimeToNext (VOID)
{
   LONG RetVal = -1L, Nxt;
   struct dosdate_t date;
   struct dostime_t time;
   struct tm mktm1, mktm2;
   _dos_getdate (&date);
   _dos_gettime (&time);

   memset (&mktm1, 0, sizeof (mktm1));
   mktm1.tm_min = time.minute;
   mktm1.tm_hour = time.hour;
   mktm1.tm_mday = date.day;
   mktm1.tm_mon = date.month - 1;
   mktm1.tm_year = date.year - 1900;

   if (fd != -1) {
      lseek (fd, 0L, SEEK_SET);
      while (read (fd, &Temp, sizeof (Temp)) == sizeof (Temp)) {
         memset (&mktm2, 0, sizeof (mktm2));
         mktm2.tm_min = Temp.Minute;
         mktm2.tm_hour = Temp.Hour;
         mktm2.tm_mday = Temp.Day;
         mktm2.tm_mon = Temp.Month - 1;
         mktm2.tm_year = Temp.Year - 1900;
         Nxt = mktime (&mktm2) - mktime (&mktm1);
         if (Nxt >= 0L && (RetVal > Nxt || RetVal == -1L))
            RetVal = Nxt;
      }
   }

   return (RetVal);
}

VOID TScheduler::Update (VOID)
{
   if (fd != -1 && tell (fd) >= sizeof (Temp)) {
      Class2Struct ();
      lseek (fd, tell (fd) - sizeof (Temp), SEEK_SET);
      write (fd, &Temp, sizeof (Temp));
   }
}



