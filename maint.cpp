
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.04
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/07/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "schedule.h"

TMaint::TMaint (class TConfig *lpCfg, class TLog *lpLog)
{
   Cfg = lpCfg;
   Log = lpLog;
}

TMaint::~TMaint (void)
{
}

VOID TMaint::DeleteMessages (VOID)
{
   ULONG Number, Today, Message, Deleted, Total;
   struct tm mktm;
   class TMsgData *MsgArea;
   class MsgBase *Msg;

   if (Log != NULL)
      Log->Write ("+Message Maintenance");
   Today = time (NULL);

   if ((MsgArea = new TMsgData) != NULL) {
      if (MsgArea->First () == TRUE)
         do {
            Msg = NULL;
            if (MsgArea->Storage == ST_JAM)
               Msg = new JAM (MsgArea->Path);
            else if (MsgArea->Storage == ST_SQUISH)
               Msg = new SQUISH (MsgArea->Path);

            if (Msg != NULL) {
               Deleted = 0L;
               Total = MsgArea->ActiveMsgs;

               if (Log != NULL)
                  Log->Write ("+Area: %s", MsgArea->Display);

               if (MsgArea->DaysOld > 0) {
                  Number = 0L;
                  while (Msg->Next (Number) == TRUE) {
                     Msg->ReadHeader (Number);
                     memset (&mktm, 0, sizeof (mktm));
                     mktm.tm_year = Msg->Arrived.Year - 1900;
                     mktm.tm_mon = Msg->Arrived.Month - 1;
                     mktm.tm_mday = Msg->Arrived.Day;
                     mktm.tm_hour = Msg->Arrived.Hour;
                     mktm.tm_min = Msg->Arrived.Minute;
                     mktm.tm_sec = Msg->Arrived.Second;
                     Message = mktime (&mktm);

                     if (Today > Message) {
                        Message = (Today - Message) / 86400L;
                        if (Message > MsgArea->DaysOld) {
                           Msg->Delete (Number);
                           Deleted++;
                           MsgArea->ActiveMsgs--;
                        }
                     }
                  }
               }

               if (MsgArea->MaxMessages > 0 && Msg->Number () > MsgArea->MaxMessages) {
                  Number = 0L;
                  while (Msg->Next (Number) == TRUE && Msg->Number () > MsgArea->MaxMessages) {
                     Msg->Delete (Number);
                     Deleted++;
                     MsgArea->ActiveMsgs--;
                  }
               }

               if (Deleted > 0) {
                  if (Log != NULL)
                     Log->Write ("+Packing Files");
                  Msg->Pack ();
                  MsgArea->Update ();
               }

               if (Log != NULL)
                  Log->Write (":Total Msgs: %lu, Deleted: %lu, Remaining: %lu", Total, Deleted, MsgArea->ActiveMsgs);

               delete Msg;
            }
         } while (MsgArea->Next () == TRUE);
      delete MsgArea;
   }
}

#if defined(__OS2__)
VOID TMaint::UpdateNewsgroups (VOID)
{
   ULONG Added;
   struct dosdate_t date;
   struct dostime_t time;
   class TMsgData *Data;
   class MsgBase *Local = NULL, *News;

   if (Log != NULL)
      Log->Write ("+Update USENET Newsgroups");

   if ((Data = new TMsgData) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (Data->UpdateNews == TRUE && Data->NewsGroup[0] != '\0') {
               // If the area is originated from a newsgroup, retrieves the new messages
               // from the news server.
               if ((News = new USENET (Cfg->NewsServer, Data->NewsGroup)) != NULL) {
                  if (Data->Storage == ST_JAM)
                     Local = new JAM (Data->Path);
                  else if (Data->Storage == ST_SQUISH)
                     Local = new SQUISH (Data->Path);
                  if (Local != NULL) {
                     if (News->Next (Data->Highest) == TRUE) {
                        Added = 0L;
                        if (Log != NULL)
                           Log->Write (":Retriving %s", Data->NewsGroup);
                        do {
                           if (News->Read (Data->Highest) == TRUE) {
                              Local->New ();
                              strcpy (Local->From, News->From);
                              strcpy (Local->To, News->To);
                              strcpy (Local->Subject, News->Subject);
                              Local->Written.Day = News->Written.Day;
                              Local->Written.Month = News->Written.Month;
                              Local->Written.Year = News->Written.Year;
                              Local->Written.Hour = News->Written.Hour;
                              Local->Written.Minute = News->Written.Minute;
                              _dos_getdate (&date);
                              _dos_gettime (&time);
                              Local->Arrived.Day = date.day;
                              Local->Arrived.Month = date.month;
                              Local->Arrived.Year = (USHORT)date.year;
                              Local->Arrived.Hour = time.hour;
                              Local->Arrived.Minute = time.minute;
                              Local->Arrived.Second = time.second;
                              Local->Write (Local->Highest () + 1L, News->Text);
                              Data->ActiveMsgs++;
                              Added++;
                           }
                        } while (News->Next (Data->Highest) == TRUE);
                        Data->Update ();
                        if (Log != NULL)
                           Log->Write (" %lu Messages Added, Total: %lu", Added, Data->ActiveMsgs);
                     }
                     delete Local;
                     Local = NULL;
                  }
                  delete News;
               }
            }
            else {
               News = NULL;
               if (Data->Storage == ST_JAM)
                  News = new JAM (Data->Path);
               else if (Data->Storage == ST_SQUISH)
                  News = new SQUISH (Data->Path);
               else if (Data->Storage == ST_USENET)
                  News = new USENET (Cfg->NewsServer, Data->NewsGroup);
               Data->ActiveMsgs = 0L;
               if (News != NULL) {
                  Data->ActiveMsgs = News->Number ();
                  delete News;
               }
               Data->Update ();
            }
         } while (Data->Next () == TRUE);
      delete Data;
   }
}
#endif

VOID TMaint::Users (VOID)
{
   ULONG Today, Elapsed, Deleted = 0L, Total = 0L;
   class TUser *User;

   if (Log != NULL)
      Log->Write ("+Users Maintenance");

   if ((User = new TUser (Cfg->UserFile)) != NULL) {
      if (Cfg->MaxInactiveUser > 0) {
         Today = time (NULL);

         if (User->First () == TRUE)
            do {
               Total++;
               if (Today > User->LastCall) {
                  Elapsed = (Today - User->LastCall) / 86400L;
                  if (Elapsed > Cfg->MaxInactiveUser) {
                     if (Log != NULL)
                        Log->Write (" Deleting: %s", User->Name);
                     User->Delete ();
                     Deleted++;
                  }
               }
            } while (User->Next () == TRUE);
      }
      else {
         if (User->First () == TRUE)
            do {
               Total++;
            } while (User->Next () == TRUE);
      }

      if (Log != NULL)
         Log->Write ("+Packing Users Database");
      User->Pack ();

      if (Log != NULL)
         Log->Write (":Total Users: %lu, Deleted: %lu, Remaining: %lu", Total, Deleted, Total - Deleted);

      delete User;
   }
}



