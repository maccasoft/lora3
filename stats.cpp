
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

TStatistics::TStatistics (void)
{
   strcpy (DataFile, "stats.dat");
}

TStatistics::TStatistics (PSZ pszDataPath)
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
   strcat (DataFile, "stats.dat");
}

TStatistics::~TStatistics (void)
{
}

USHORT TStatistics::First (VOID)
{
   LastTask = 0;
   return (Next ());
}

USHORT TStatistics::Next (VOID)
{
   int fd;
   USHORT RetVal = FALSE;

   memset (&Sys, 0, sizeof (Sys));

   if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      read (fd, &Sys, sizeof (Sys));
      while (read (fd, &Line, sizeof (Line)) == sizeof (Line)) {
         if (Line.Number > LastTask) {
            RetVal = TRUE;
            break;
         }
      }
      close (fd);
   }

   if (RetVal == TRUE) {
      LineNumber = Line.Number;
      Status = Line.Status;
      strcpy (User, Line.User);
      strcpy (From, Line.From);
      strcpy (Action, Line.Action);
      strcpy (LineLastCaller, Line.LastCaller);
      if (LineLastCaller[0] == '\0')
         strcpy (LineLastCaller, "None");
      Calls = Line.Calls;
      MailCalls = Line.MailCalls;
      TodayCalls = Line.TodayCalls;

      LastTask = LineNumber;
   }

   return (RetVal);
}

VOID TStatistics::Read (USHORT usLine)
{
   int fd;
   USHORT Found = FALSE;

   memset (&Sys, 0, sizeof (Sys));

   if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      read (fd, &Sys, sizeof (Sys));
      while (read (fd, &Line, sizeof (Line)) == sizeof (Line)) {
         if (Line.Number == usLine) {
            Found = TRUE;
            break;
         }
      }
      close (fd);
   }

   strcpy (LastCaller, Sys.LastCaller);
   if (LastCaller[0] == '\0')
      strcpy (LastCaller, "None");
   TotalCalls = Sys.Calls;
   TotalMailCalls = Sys.MailCalls;
   TotalTodayCalls = Sys.TodayCalls;

   if (Found == TRUE) {
      LineNumber = Line.Number;
      Status = Line.Status;
      strcpy (User, Line.User);
      strcpy (From, Line.From);
      strcpy (Action, Line.Action);
      strcpy (LineLastCaller, Line.LastCaller);
      if (LineLastCaller[0] == '\0')
         strcpy (LineLastCaller, "None");
      Calls = Line.Calls;
      MailCalls = Line.MailCalls;
      TodayCalls = Line.TodayCalls;
   }
   else {
      LineNumber = usLine;
      Status = STAT_OFFLINE;
      strcpy (User, "None");
      Action[0] = From[0] = '\0';
      strcpy (LineLastCaller, "None");
      Calls = 0L;
      MailCalls = 0L;
      TodayCalls = 0L;
   }
}

VOID TStatistics::Update (VOID)
{
   int fd;
   USHORT Found = FALSE;

   memset (&Sys, 0, sizeof (Sys));

   strcpy (Sys.LastCaller, LastCaller);
   Sys.Calls = TotalCalls;
   Sys.MailCalls = TotalMailCalls;
   Sys.TodayCalls = TotalTodayCalls;

   if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      write (fd, &Sys, sizeof (Sys));
      while (read (fd, &Line, sizeof (Line)) == sizeof (Line)) {
         if (Line.Number == LineNumber) {
            Found = TRUE;
            break;
         }
      }

      memset (&Line, 0, sizeof (Line));

      Line.Number = LineNumber;
      strcpy (Line.LastCaller, LineLastCaller);
      Line.Calls = Calls;
      Line.MailCalls = MailCalls;
      Line.TodayCalls = TodayCalls;
      Line.Status = Status;
      strcpy (Line.User, User);
      strcpy (Line.From, From);
      strcpy (Line.Action, Action);

      if (Found == TRUE)
         lseek (fd, tell (fd) - sizeof (Line), SEEK_SET);
      write (fd, &Line, sizeof (Line));

      close (fd);
   }
}
