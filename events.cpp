
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.11
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    04/27/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

TEvents::TEvents (void)
{
   strcpy (DataFile, "events.dat");
   Data.Clear ();
}

TEvents::TEvents (PSZ pszDataFile)
{
   strcpy (DataFile, pszDataFile);
   strlwr (DataFile);
   if (strchr (DataFile, '.') == NULL)
      strcat (DataFile, ".dat");
   Data.Clear ();
}

TEvents::~TEvents (void)
{
   Data.Clear ();
}

VOID TEvents::Class2Struct (EVENT *Event)
{
   memset (Event, 0, sizeof (EVENT));

   strcpy (Event->Label, Label);
   Event->Hour = Hour;
   Event->Minute = Minute;
   if (Sunday == TRUE)
      Event->WeekDays |= DAY_SUNDAY;
   if (Monday == TRUE)
      Event->WeekDays |= DAY_MONDAY;
   if (Tuesday == TRUE)
      Event->WeekDays |= DAY_TUESDAY;
   if (Wednesday == TRUE)
      Event->WeekDays |= DAY_WEDNESDAY;
   if (Thursday == TRUE)
      Event->WeekDays |= DAY_THURSDAY;
   if (Friday == TRUE)
      Event->WeekDays |= DAY_FRIDAY;
   if (Saturday == TRUE)
      Event->WeekDays |= DAY_SATURDAY;
   Event->Length = Length;
   Event->LastDay = LastDay;
   Event->Dynamic = Dynamic;
   Event->Force = Force;
   Event->MailOnly = MailOnly;
   Event->ForceCall = ForceCall;
   strcpy (Event->Address, Address);
   Event->SendNormal = SendNormal;
   Event->SendCrash = SendCrash;
   Event->SendDirect = SendDirect;
   Event->SendImmediate = SendImmediate;
   Event->CallDelay = CallDelay;
   Event->StartImport = StartImport;
   Event->StartExport = StartExport;
   Event->ExportMail = ExportMail;
   Event->ImportNormal = ImportNormal;
   Event->ImportKnown = ImportKnown;
   Event->ImportProtected = ImportProtected;
   strcpy (Event->RouteCmd, RouteCmd);
   strcpy (Event->Command, Command);
   Event->MaxCalls = MaxCalls;
   Event->MaxConnects = MaxConnects;
   Event->AllowRequests = AllowRequests;
   Event->MakeRequests = MakeRequests;
   Event->ProcessTIC = ProcessTIC;
   Event->ClockAdjustment = ClockAdjustment;
}

VOID TEvents::Struct2Class (EVENT *Event)
{
   strcpy (Label, Event->Label);
   Hour = Event->Hour;
   Minute = Event->Minute;
   Sunday = Monday = Tuesday = Wednesday = Thursday = Friday = Saturday = FALSE;
   if (Event->WeekDays & DAY_SUNDAY)
      Sunday = TRUE;
   if (Event->WeekDays & DAY_MONDAY)
      Monday = TRUE;
   if (Event->WeekDays & DAY_TUESDAY)
      Tuesday = TRUE;
   if (Event->WeekDays & DAY_WEDNESDAY)
      Wednesday = TRUE;
   if (Event->WeekDays & DAY_THURSDAY)
      Thursday = TRUE;
   if (Event->WeekDays & DAY_FRIDAY)
      Friday = TRUE;
   if (Event->WeekDays & DAY_SATURDAY)
      Saturday = TRUE;
   Length = Event->Length;
   LastDay = Event->LastDay;
   Dynamic = Event->Dynamic;
   Force = Event->Force;
   MailOnly = Event->MailOnly;
   ForceCall = Event->ForceCall;
   strcpy (Address, Event->Address);
   SendNormal = Event->SendNormal;
   SendCrash = Event->SendCrash;
   SendDirect = Event->SendDirect;
   SendImmediate = Event->SendImmediate;
   CallDelay = Event->CallDelay;
   StartImport = Event->StartImport;
   StartExport = Event->StartExport;
   ExportMail = Event->ExportMail;
   ImportNormal = Event->ImportNormal;
   ImportKnown = Event->ImportKnown;
   ImportProtected = Event->ImportProtected;
   strcpy (RouteCmd, Event->RouteCmd);
   strcpy (Command, Event->Command);
   MaxCalls = Event->MaxCalls;
   MaxConnects = Event->MaxConnects;
   AllowRequests = Event->AllowRequests;
   MakeRequests = Event->MakeRequests;
   ProcessTIC = Event->ProcessTIC;
   ClockAdjustment = Event->ClockAdjustment;
}

VOID TEvents::Add (VOID)
{
   USHORT RetVal, TimeNew, TimeEvent;
   EVENT *lpEvent;
   VOID *Value = NULL;

   Class2Struct (&Event);

   RetVal = FALSE;
   TimeNew = (USHORT)(Event.Hour * 60 + Event.Minute);

   if ((lpEvent = (EVENT *)Data.First ()) != NULL) {
      TimeEvent = (USHORT)(lpEvent->Hour * 60 + lpEvent->Minute);
      if ((TimeNew + Event.Length) <= (TimeEvent + lpEvent->Length)) {
         Data.Insert (&Event, sizeof (EVENT));
         Value = Data.Value ();
         Data.Insert (lpEvent, sizeof (EVENT));
         Data.First ();
         Data.Remove ();
         RetVal = TRUE;
      }
      if (RetVal == FALSE)
         do {
            TimeEvent = (USHORT)(lpEvent->Hour * 60 + lpEvent->Minute);
            if ((TimeNew + Event.Length) <= (TimeEvent + lpEvent->Length)) {
               Data.Previous ();
               Data.Insert (&Event, sizeof (EVENT));
               Value = Data.Value ();
               RetVal = TRUE;
            }
         } while (RetVal == FALSE && (lpEvent = (EVENT *)Data.Next ()) != NULL);
   }

   if (RetVal == FALSE) {
      Data.Add (&Event, sizeof (EVENT));
      Value = Data.Value ();
   }

   if (Data.First () != NULL)
      do {
         if (Data.Value () == Value)
            break;
      } while (Data.Next () != NULL);
}

VOID TEvents::Delete (VOID)
{
   if (Data.Value () != NULL) {
      Data.Remove ();
      if (Data.Value () != NULL)
         Struct2Class ((EVENT *)Data.Value ());
      else
         New ();
   }
}

USHORT TEvents::First (VOID)
{
   USHORT RetVal = FALSE;
   EVENT *Event;

   if ((Event = (EVENT *)Data.First ()) != NULL) {
      Struct2Class (Event);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TEvents::Load (VOID)
{
   int fd;
   USHORT RetVal = FALSE;

   Data.Clear ();

   if ((fd = sopen (DataFile, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &Event, sizeof (EVENT)) == sizeof (EVENT))
         Data.Add (&Event, sizeof (EVENT));
      close (fd);

      RetVal = First ();
   }

   return (RetVal);
}

VOID TEvents::New (VOID)
{
   memset (Label, 0, sizeof (Label));
   Hour = Minute = 0;
   Sunday = Monday = Tuesday = Wednesday = Thursday = Friday = Saturday = FALSE;
   Length = 1;
   LastDay = 0;
   Dynamic = Force = MailOnly = ForceCall = FALSE;
   memset (Address, 0, sizeof (Address));
   SendNormal = SendCrash = SendDirect = SendImmediate = FALSE;
   CallDelay = 30;
   StartImport = StartExport = ExportMail = FALSE;
   ImportNormal = ImportProtected = ImportKnown = FALSE;
   memset (RouteCmd, 0, sizeof (RouteCmd));
   memset (Command, 0, sizeof (Command));
   MaxCalls = 10;
   MaxConnects = 5;
   AllowRequests = MakeRequests = ProcessTIC = ClockAdjustment = FALSE;
}

USHORT TEvents::Next (VOID)
{
   USHORT RetVal = FALSE;
   EVENT *Event;

   if ((Event = (EVENT *)Data.Next ()) != NULL) {
      Struct2Class (Event);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TEvents::Previous (VOID)
{
   USHORT RetVal = FALSE;
   EVENT *Event;

   if ((Event = (EVENT *)Data.Previous ()) != NULL) {
      Struct2Class (Event);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TEvents::Save (VOID)
{
   int fd;

   if ((fd = sopen (DataFile, O_WRONLY|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if (Data.First () != NULL)
         do {
            write (fd, Data.Value (), sizeof (EVENT));
         } while (Data.Next () != NULL);

      chsize (fd, tell (fd));
      close (fd);
   }
}

USHORT TEvents::SetCurrent (VOID)
{
   USHORT RetVal = FALSE, TimeNow, TimeEvent;
   USHORT CurDay;
   EVENT *Event;
   time_t t;
   struct tm ltm;

   t = time (NULL);
   memcpy (&ltm, localtime (&t), sizeof (struct tm));
   TimeNow = (USHORT)(ltm.tm_hour * 60 + ltm.tm_min);
   CurDay = (USHORT)(1 << ltm.tm_wday);
   Number = 0;

   if ((Event = (EVENT *)Data.First ()) != NULL)
      do {
         Number++;
         TimeEvent = (USHORT)(Event->Hour * 60 + Event->Minute);
         if (TimeNow >= TimeEvent && TimeNow < (TimeEvent + Event->Length) && (CurDay & Event->WeekDays)) {
            if (Event->LastDay != ltm.tm_yday) {
               Event->LastDay = (USHORT)ltm.tm_yday;
               Started = TRUE;
            }
            else
               Started = FALSE;

            Struct2Class (Event);
            RetVal = TRUE;
         }
      } while (RetVal == FALSE && (Event = (EVENT *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TEvents::TimeToNext (VOID)
{
   USHORT TimeNow, TimeEvent, ToMid;
   USHORT CurDay, Num;
   EVENT *Event;
   time_t t;
   struct tm ltm;

   t = time (NULL);
   memcpy (&ltm, localtime (&t), sizeof (struct tm));
   TimeNow = (USHORT)(ltm.tm_hour * 60 + ltm.tm_min);
   CurDay = (USHORT)(1 << ltm.tm_wday);

   NextNumber = 0;
   NextLabel[0] = '\0';
   TimeRemain = 3000;
   Num = 0;

   if ((Event = (EVENT *)Data.First ()) != NULL)
      do {
         Num++;
         TimeEvent = (USHORT)(Event->Hour * 60 + Event->Minute);
         if (Event->LastDay != ltm.tm_yday && TimeEvent > TimeNow && (CurDay & Event->WeekDays)) {
            if (TimeRemain > (TimeEvent - TimeNow)) {
               TimeRemain = (USHORT)(TimeEvent - TimeNow);
               strcpy (NextLabel, Event->Label);
               NextNumber = Num;
            }
         }
      } while ((Event = (EVENT *)Data.Next ()) != NULL);

   if (TimeRemain > 1440) {
      ToMid = (USHORT)(1440 - TimeNow);
      TimeNow = 0;
      CurDay <<= 1;
      if (CurDay > DAY_SATURDAY)
         CurDay = DAY_SUNDAY;
      Num = 0;

      if ((Event = (EVENT *)Data.First ()) != NULL)
         do {
            Num++;
            TimeEvent = (USHORT)(Event->Hour * 60 + Event->Minute);
//            if (Event->LastDay != ltm.tm_yday && TimeEvent > TimeNow && (CurDay & Event->WeekDays)) {
            if (TimeEvent >= TimeNow && (CurDay & Event->WeekDays)) {
               if (TimeRemain > (TimeEvent - TimeNow + ToMid)) {
                  TimeRemain = (USHORT)(TimeEvent - TimeNow + ToMid);
                  strcpy (NextLabel, Event->Label);
                  NextNumber = Num;
               }
            }
         } while ((Event = (EVENT *)Data.Next ()) != NULL);
   }

   if (TimeRemain > 1440)
      TimeRemain = 1440;
}

VOID TEvents::Update (VOID)
{
   if (Data.Value () != NULL) {
      Data.Remove ();
      Add ();
      Struct2Class ((EVENT *)Data.Value ());
   }
}

