
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/13/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "offline.h"

TAscii::TAscii (class TBbs *bbs) : TOffline (bbs)
{
}

TAscii::~TAscii (void)
{
}

USHORT TAscii::Create (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], PktName[32];
   class TLastRead *LastRead;

   Log->Write ("+Preparing Ascii packet");
   LastRead = User->LastRead;
   sprintf (PktName, "%s.MSG", Id);

   Total = 0L;
   TotalPersonal = 0L;

   if (BuildPath (Work) == TRUE) {
      if (LastRead->First () == TRUE)
         do {
            if (LastRead->QuickScan == TRUE)
               RetVal = TRUE;
         } while (RetVal == FALSE && LastRead->Next () == TRUE);

      if (RetVal == TRUE) {
         Bbs->Printf ("\n\x16\x01\012Preparing %s packet...\n\n", PktName);

         Bbs->Printf ("\x16\x01\0170%%    10%%  20%%   30%%   40%%   50%%   60%%   70%%   80%%   90%%   100%%\n|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|\r");
         Area = 1;
         NewLR.Clear ();

         if (LastRead->Read ("e-mail") == TRUE) {
            strcpy (NewLR.Key, LastRead->Key);
            NewLR.Number = LastRead->Number;
            NewLR.QuickScan = LastRead->QuickScan;
            NewLR.PersonalOnly = LastRead->PersonalOnly;
            NewLR.ExcludeOwn = LastRead->ExcludeOwn;
         }
         else {
            strcpy (NewLR.Key, "e-mail");
            NewLR.Number = 0L;
            NewLR.QuickScan = TRUE;
            NewLR.PersonalOnly = FALSE;
            NewLR.ExcludeOwn = FALSE;
         }
         NewLR.Add ();
         PackEMail (NewLR.Number);

         if (LastRead->First () == TRUE)
            do {
               if (LastRead->QuickScan == TRUE) {
                  strcpy (NewLR.Key, LastRead->Key);
                  NewLR.Number = LastRead->Number;
                  NewLR.QuickScan = LastRead->QuickScan;
                  NewLR.PersonalOnly = LastRead->PersonalOnly;
                  NewLR.ExcludeOwn = LastRead->ExcludeOwn;
                  NewLR.Add ();
                  PackArea (NewLR.Key, NewLR.Number);
                  Log->Write (":  Area %s, %ld msgs. (%ld personal)", NewLR.Key, Current, Personal);
                  NewLR.Update ();
               }
               Area++;
            } while (LastRead->Next () == TRUE);

         Log->Write ("*Packed %ld messages (%ld personal)", Total, TotalPersonal);

         if (Total > 0L) {
            if (BuildPath (Path) == TRUE) {
               sprintf (Temp, "%s%s", Path, PktName);
               if (Compress (Temp) == TRUE)
                  Download (Temp, PktName);
            }
         }
      }
   }
   else
      Log->Write ("!Path Error: %s", Work);

   return (RetVal);
}

VOID TAscii::PackArea (PSZ pszKey, ULONG &ulLast)
{
   FILE *fp;
   CHAR Temp[128], *Text;
   ULONG Number;

   Number = ulLast;
   Current = Personal = 0L;

   if (Total < Limit && (MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (MsgArea->Read (pszKey) == TRUE) {
         if (MsgArea->Storage == ST_JAM)
            Msg = new JAM (MsgArea->Path);
         else if (MsgArea->Storage == ST_SQUISH)
            Msg = new SQUISH (MsgArea->Path);
         else if (MsgArea->Storage == ST_USENET)
            Msg = new USENET (Cfg->NewsServer, MsgArea->NewsGroup);

         sprintf (Temp, "%s%03d.TXT", Work, Area);
         fp = fopen (Temp, "wt");

         if (Msg != NULL && fp != NULL) {
            if (Msg->Next (Number) == TRUE) {
               do {
                  Msg->Read (Number);

                  fprintf (fp, "\n==============================================\n    Msg. #%ld of %ld (%s)\n", Number, Msg->Highest (), MsgArea->Display);
                  fprintf (fp, "   Date: %d %s %d %2d:%02d\n", Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
                  fprintf (fp, "   From: %s\n", Msg->From);
                  if (Msg->To[0])
                     fprintf (fp, "     To: %s\n", Msg->To);
                  fprintf (fp, "Subject: %s\n----------------------------------------------\n", Msg->Subject);

                  Current++;
                  Total++;
                  if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
// ----------------------------------------------------------------------
// If the message is directed to the user, then writes the message
// number and area number in the personal index file too.
// ----------------------------------------------------------------------
                     Personal++;
                     TotalPersonal++;
                  }

                  if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                     do {
// ----------------------------------------------------------------------
// Get rid of the FidoNet<tm> control lines that are only wasting space,
// and write only the useful text lines.
// ----------------------------------------------------------------------
                        if (Text[0] != 0x01 && strnicmp (Text, "SEEN-BY: ", 9))
                           fprintf (fp, "%s\n", Text);
                     } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);

                  if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                     BarWidth = (USHORT)((Total * 61L) / TotalPack);
                     Bbs->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
                  }
               } while (Total < Limit && Msg->Next (Number) == TRUE);
            }
         }

         if (Msg != NULL)
            delete Msg;
         if (fp != NULL)
            fclose (fp);
      }
      delete MsgArea;
   }

   ulLast = Number;
}

VOID TAscii::PackEMail (ULONG &ulLast)
{
   FILE *fp;
   CHAR Temp[128], *Text;
   ULONG Number;

   Number = ulLast;
   Current = Personal = 0L;

   sprintf (Temp, "%s%s", Cfg->MailSpool, User->MailBox);
   if ((Msg = new SQUISH (Temp)) != NULL) {
      sprintf (Temp, "%s%03d.TXT", Work, 0);
      fp = fopen (Temp, "wt");

      if (fp != NULL) {
         if (Msg->Next (Number) == TRUE) {
            do {
               Msg->Read (Number);

               fprintf (fp, "\n==============================================\n    Msg. #%ld of %ld (%s)\n", Number, Msg->Highest (), "E-MAIL");
               fprintf (fp, "   Date: %d %s %d %2d:%02d\n", Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
               fprintf (fp, "   From: %s\n", Msg->From);
               if (Msg->To[0])
                  fprintf (fp, "     To: %s\n", Msg->To);
               fprintf (fp, "Subject: %s\n----------------------------------------------\n", Msg->Subject);

               Current++;
               Total++;
               Personal++;
               TotalPersonal++;

               if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                  do {
// ----------------------------------------------------------------------
// Get rid of the FidoNet<tm> control lines that are only wasting space,
// and write only the useful text lines.
// ----------------------------------------------------------------------
                     if (Text[0] != 0x01 && strnicmp (Text, "SEEN-BY: ", 9))
                        fprintf (fp, "%s\n", Text);
                  } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);

               if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                  BarWidth = (USHORT)((Total * 61L) / TotalPack);
                  Bbs->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
               }
            } while (Total < Limit && Msg->Next (Number) == TRUE);
         }
      }

      if (Msg != NULL)
         delete Msg;
      if (fp != NULL)
         fclose (fp);
   }

   ulLast = Number;
}


