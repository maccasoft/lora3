
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/13/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "offline.h"

TQWK::TQWK (class TBbs *bbs) : TOffline (bbs)
{
}

TQWK::~TQWK (void)
{
}

USHORT TQWK::Create (VOID)
{
   int i;
   FILE *fp;
   USHORT RetVal = FALSE;
   CHAR Temp[128], PktName[32];
   struct dosdate_t date;
   struct dostime_t time;
   class TLastRead *LastRead;

   Log->Write ("+Preparing QWK packet");
   LastRead = User->LastRead;
   sprintf (PktName, "%s.QWK", Id);

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
         Blocks = 2L;

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
                  Area++;
               }
            } while (LastRead->Next () == TRUE);

// ----------------------------------------------------------------------
// The message packet control files are created only if the user has at
// least one area tagged for the offline reader.
// ----------------------------------------------------------------------
         sprintf (Temp, "%sCONTROL.DAT", Work);
         if ((fp = fopen (Temp, "wt")) != NULL) {
            fprintf (fp, "%s\n", Cfg->SystemName);
            fprintf (fp, " \n");
            fprintf (fp, " \n");
            fprintf (fp, "%s\n", Cfg->SysopName);
            fprintf (fp, "00000,%s\n", Id);
            _dos_getdate (&date);
            _dos_gettime (&time);
            fprintf (fp, "%02d-%02d-%04d,%02d:%02d:%02d\n", date.month, date.day, date.year, time.hour, time.minute, time.second);
            strcpy (Temp, User->Name);
            fprintf (fp, "%s\n", strupr (Temp));
            fprintf (fp, " \n");
            fprintf (fp, "0\n");
            fprintf (fp, "%lu\n", Total);

            i = 0;
            if (LastRead->First () == TRUE)
               do {
                  if (LastRead->QuickScan == TRUE)
                     i++;
               } while (LastRead->Next () == TRUE);
            fprintf (fp, "%d\n", i - 1);

            i = 1;
            if (LastRead->First () == TRUE)
               do {
                  if (LastRead->QuickScan == TRUE)
                     fprintf (fp, "%d\n%s\n", i++, LastRead->Key);
               } while (LastRead->Next () == TRUE);

            fprintf (fp, "WELCOME\n");
            fprintf (fp, "NEWS\n");
            fprintf (fp, "GOODBYE\n");

            fclose (fp);
         }

         sprintf (Temp, "%sDOOR.ID", Work);
         if ((fp = fopen (Temp, "wt")) != NULL) {
            fprintf (fp, "DOOR = LoraBBS\n");
            fprintf (fp, "VERSION = %s\n", "0.01");
            fprintf (fp, "SYSTEM = LoraBBS\n");
            fprintf (fp, "CONTROLNAME = Lora\n");
            fprintf (fp, "CONTROLTYPE = ADD\n");
            fprintf (fp, "CONTROLTYPE = DROP\n");
            fclose (fp);
         }

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

USHORT TQWK::FetchReply (VOID)
{
   return (FALSE);
}

float TQWK::IEEToMSBIN (float f)
{
   short sign, exp;
   QWKCONV t;

   t.f[0] = f;
   sign = (short)(t.uc[3] / 0x80);
   exp = (short)(((t.ui[1] >> 7) - 0x7F + 0x81) & 0xFF);
   t.ui[1] = (USHORT)((t.ui[1] & 0x7F) | (sign << 7) | (exp << 8));

   return (t.f[0]);
}

VOID TQWK::PackArea (PSZ pszKey, ULONG &ulLast)
{
   int fdm, fdi, fdp;
   float out, in;
   CHAR Temp[128], *Text;
   ULONG Number, Pos, Size;

   Number = ulLast;
   Current = Personal = 0L;

   if (Total < Limit && (MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (MsgArea->Read (pszKey) == TRUE) {
         Msg = NULL;
         if (MsgArea->Storage == ST_JAM)
            Msg = new JAM (MsgArea->Path);
         else if (MsgArea->Storage == ST_SQUISH)
            Msg = new SQUISH (MsgArea->Path);
         else if (MsgArea->Storage == ST_USENET)
            Msg = new USENET (Cfg->NewsServer, MsgArea->NewsGroup);

         sprintf (Temp, "%s%03d.NDX", Work, Area);
         fdi = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
         lseek (fdi, 0L, SEEK_END);

         sprintf (Temp, "%sPERSONAL.NDX", Work);
         fdp = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
         lseek (fdp, 0L, SEEK_END);

         sprintf (Temp, "%sMESSAGES.DAT", Work);
         if ((fdm = open (Temp, O_WRONLY|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
            lseek (fdm, 0L, SEEK_END);
            if (tell (fdm) == 0L) {
// ----------------------------------------------------------------------
// The first record of the MESSAGE.DAT file must be the Sparkware id
// block, otherwise some applications may complain.
// ----------------------------------------------------------------------
               write (fdm, "Produced by Qmail...", 20);
               write (fdm, "Copywright (c) 1987 by Sparkware.  ", 35);
               write (fdm, "All Rights Reserved", 19);
               memset (Temp, ' ', 54);
               write (fdm, Temp, 54);
            }
         }

         if (Msg != NULL && fdi != -1 && fdp != -1 && fdm != -1) {
            if (Msg->Next (Number) == TRUE) {
               do {
                  Msg->Read (Number);

                  Current++;
                  Total++;
                  sprintf (Temp, "%lu", Blocks);
                  in = atof (Temp);
                  out = IEEToMSBIN (in);
                  write (fdi, &out, sizeof (float));
                  write (fdi, "", 1);
                  if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
// ----------------------------------------------------------------------
// If the message is directed to the user, then writes the message
// number and area number in the personal index file too.
// ----------------------------------------------------------------------
                     Personal++;
                     TotalPersonal++;
                     write (fdp, &out, sizeof (float));
                     write (fdp, "", 1);
                  }

// ----------------------------------------------------------------------
// Fills the QWK message header.
// ----------------------------------------------------------------------
                  memset (&Qwk, ' ', sizeof (Qwk));
                  sprintf (Temp, "%-*lu", sizeof (Qwk.Msgnum), Number);
                  memcpy (Qwk.Msgnum, Temp, sizeof (Qwk.Msgnum));
                  sprintf (Temp, "%02d-%02d-%02d", Msg->Written.Month, Msg->Written.Day, Msg->Written.Year % 100);
                  memcpy (Qwk.Msgdate, Temp, sizeof (Qwk.Msgdate));
                  sprintf (Temp, "%02d:%02d", Msg->Written.Hour, Msg->Written.Minute);
                  memcpy (Qwk.Msgtime, Temp, sizeof (Qwk.Msgtime));
                  Msg->From[sizeof (Qwk.MsgFrom) - 1] = '\0';
                  memcpy (Qwk.MsgFrom, Msg->From, strlen (Msg->From));
                  Msg->To[sizeof (Qwk.MsgTo) - 1] = '\0';
                  memcpy (Qwk.MsgTo, Msg->To, strlen (Msg->To));
                  Msg->Subject[sizeof (Qwk.MsgSubj) - 1] = '\0';
                  memcpy (Qwk.MsgSubj, Msg->Subject, strlen (Msg->Subject));
                  Qwk.Msglive = 0xE1;
                  Qwk.Msgarealo = (UCHAR)(Area & 0xFF);
                  Qwk.Msgareahi = (UCHAR)((Area & 0xFF00) >> 8);

                  Pos = tell (fdm);
                  write (fdm, &Qwk, sizeof (Qwk));

                  Size = 128L;
                  if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                     do {
// ----------------------------------------------------------------------
// Get rid of the FidoNet<tm> control lines that are only wasting space,
// and write only the useful text lines.
// ----------------------------------------------------------------------
                        if (Text[0] != 0x01 && strnicmp (Text, "SEEN-BY: ", 9)) {
                           Size += (ULONG)write (fdm, Text, strlen (Text));
                           Size += (ULONG)write (fdm, "\xE3", 1);
                        }
                     } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);

// ----------------------------------------------------------------------
// Adjust the message size so it is a multiple of 128 bytes.
// ----------------------------------------------------------------------
                  if ((Size % 128L) != 0) {
                     memset (Temp, ' ', 128);
                     Size += (ULONG)write (fdm, Temp, (int)(128L - (Size % 128L)));
                  }

                  Blocks += (Size / 128L);
                  sprintf (Temp, "%-*lu", sizeof (Qwk.Msgrecs), Size / 128L);
                  memcpy (Qwk.Msgrecs, Temp, sizeof (Qwk.Msgrecs));
                  lseek (fdm, Pos, SEEK_SET);
                  write (fdm, &Qwk, sizeof (Qwk));
                  lseek (fdm, 0L, SEEK_END);

                  if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                     BarWidth = (USHORT)((Total * 61L) / TotalPack);
                     Bbs->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
                  }
               } while (Total < Limit && Msg->Next (Number) == TRUE);
            }
         }

         if (Msg != NULL)
            delete Msg;
         if (fdm != -1)
            close (fdm);
         if (fdi != -1)
            close (fdi);
         if (fdp != -1)
            close (fdp);
      }
      delete MsgArea;
   }

   ulLast = Number;
}

VOID TQWK::PackEMail (ULONG &ulLast)
{
   int fdm, fdp;
   float out, in;
   CHAR Temp[128], *Text;
   ULONG Number, Pos, Size;

   Number = ulLast;
   Current = Personal = 0L;

   sprintf (Temp, "%s%s", Cfg->MailSpool, User->MailBox);
   if ((Msg = new SQUISH (Temp)) != NULL) {
      sprintf (Temp, "%sPERSONAL.NDX", Work);
      fdp = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
      lseek (fdp, 0L, SEEK_END);

      sprintf (Temp, "%sMESSAGES.DAT", Work);
      if ((fdm = open (Temp, O_WRONLY|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
         lseek (fdm, 0L, SEEK_END);
         if (tell (fdm) == 0L) {
// ----------------------------------------------------------------------
// The first record of the MESSAGE.DAT file must be the Sparkware id
// block, otherwise some applications may complain.
// ----------------------------------------------------------------------
            write (fdm, "Produced by Qmail...", 20);
            write (fdm, "Copywright (c) 1987 by Sparkware.  ", 35);
            write (fdm, "All Rights Reserved", 19);
            memset (Temp, ' ', 54);
            write (fdm, Temp, 54);
         }
      }

      if (Msg != NULL && fdp != -1 && fdm != -1) {
         if (Msg->Next (Number) == TRUE) {
            do {
               Msg->Read (Number);

               Current++;
               Total++;
               sprintf (Temp, "%lu", Blocks);
               in = atof (Temp);
               out = IEEToMSBIN (in);
               Personal++;
               TotalPersonal++;
               write (fdp, &out, sizeof (float));
               write (fdp, "", 1);

// ----------------------------------------------------------------------
// Fills the QWK message header.
// ----------------------------------------------------------------------
               memset (&Qwk, ' ', sizeof (Qwk));
               sprintf (Temp, "%-*lu", sizeof (Qwk.Msgnum), Number);
               memcpy (Qwk.Msgnum, Temp, sizeof (Qwk.Msgnum));
               sprintf (Temp, "%02d-%02d-%02d", Msg->Written.Month, Msg->Written.Day, Msg->Written.Year % 100);
               memcpy (Qwk.Msgdate, Temp, sizeof (Qwk.Msgdate));
               sprintf (Temp, "%02d:%02d", Msg->Written.Hour, Msg->Written.Minute);
               memcpy (Qwk.Msgtime, Temp, sizeof (Qwk.Msgtime));
               Msg->From[sizeof (Qwk.MsgFrom) - 1] = '\0';
               memcpy (Qwk.MsgFrom, Msg->From, strlen (Msg->From));
               Msg->To[sizeof (Qwk.MsgTo) - 1] = '\0';
               memcpy (Qwk.MsgTo, Msg->To, strlen (Msg->To));
               Msg->Subject[sizeof (Qwk.MsgSubj) - 1] = '\0';
               memcpy (Qwk.MsgSubj, Msg->Subject, strlen (Msg->Subject));
               Qwk.Msglive = 0xE1;
               Qwk.Msgarealo = (UCHAR)(0 & 0xFF);
               Qwk.Msgareahi = (UCHAR)((0 & 0xFF00) >> 8);

               Pos = tell (fdm);
               write (fdm, &Qwk, sizeof (Qwk));

               Size = 128L;
               if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                  do {
// ----------------------------------------------------------------------
// Get rid of the FidoNet<tm> control lines that are only wasting space,
// and write only the useful text lines.
// ----------------------------------------------------------------------
                     if (Text[0] != 0x01 && strnicmp (Text, "SEEN-BY: ", 9)) {
                        Size += (ULONG)write (fdm, Text, strlen (Text));
                        Size += (ULONG)write (fdm, "\xE3", 1);
                     }
                  } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);

// ----------------------------------------------------------------------
// Adjust the message size so it is a multiple of 128 bytes.
// ----------------------------------------------------------------------
               if ((Size % 128L) != 0) {
                  memset (Temp, ' ', 128);
                  Size += (ULONG)write (fdm, Temp, (int)(128L - (Size % 128L)));
               }

               Blocks += (Size / 128L);
               sprintf (Temp, "%-*lu", sizeof (Qwk.Msgrecs), Size / 128L);
               memcpy (Qwk.Msgrecs, Temp, sizeof (Qwk.Msgrecs));
               lseek (fdm, Pos, SEEK_SET);
               write (fdm, &Qwk, sizeof (Qwk));
               lseek (fdm, 0L, SEEK_END);

               if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                  BarWidth = (USHORT)((Total * 61L) / TotalPack);
                  Bbs->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
               }
            } while (Total < Limit && Msg->Next (Number) == TRUE);
         }
      }

      if (Msg != NULL)
         delete Msg;
      if (fdm != -1)
         close (fdm);
      if (fdp != -1)
         close (fdp);
   }

   ulLast = Number;
}


