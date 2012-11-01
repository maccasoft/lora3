
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.13
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/13/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "offline.h"

PSZ Extensions[] = {
   ".SU0", ".MO0", ".TU0", ".WE0", ".TH0", ".FR0", ".SA0"
};

TBlueWave::TBlueWave (class TBbs *bbs) : TOffline (bbs)
{
}

TBlueWave::~TBlueWave (void)
{
}

USHORT TBlueWave::Create (VOID)
{
   int fd;
   USHORT RetVal = FALSE;
   CHAR Temp[128], PktName[32];
   struct dosdate_t date;
   class TLastRead *LastRead;

   Log->Write ("+Preparing BlueWave packet");
   LastRead = User->LastRead;
   _dos_getdate (&date);
   sprintf (PktName, "%s%s", Id, Extensions[date.dayofweek]);

   Total = 0L;
   TotalPersonal = 0L;

   if (BuildPath (Work) == TRUE) {
      sprintf (Temp, "%s%s.INF", Work, Id);
      if ((fd = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
         memset (&Inf, 0, sizeof (Inf));
         Inf.ver = PACKET_LEVEL;
         strcpy ((PSZ)Inf.loginname, User->Name);
         strcpy ((PSZ)Inf.aliasname, User->RealName);
         strcpy ((PSZ)Inf.sysop, Cfg->SysopName);
         strcpy ((PSZ)Inf.systemname, Cfg->SystemName);
         Inf.inf_header_len = sizeof (INF_HEADER);
         Inf.inf_areainfo_len = sizeof (INF_AREA_INFO);
         Inf.mix_structlen = sizeof (MIX_REC);
         Inf.fti_structlen = sizeof (FTI_REC);
         Inf.uses_upl_file = 1;
         strcpy ((PSZ)Inf.packet_id, Id);
         write (fd, &Inf, sizeof (Inf));

         memset (&AreaInf, 0, sizeof (AreaInf));
         sprintf ((PSZ)AreaInf.areanum, "%u", 0);
         strcpy ((PSZ)AreaInf.title, "E-Mail");
         write (fd, &AreaInf, sizeof (AreaInf));

         if ((MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
            Area = 1;
            if (MsgArea->First () == TRUE)
               do {
                  memset (&AreaInf, 0, sizeof (AreaInf));
                  sprintf ((PSZ)AreaInf.areanum, "%u", Area++);
                  MsgArea->EchoTag[sizeof (AreaInf.echotag) - 1] = '\0';
                  strcpy ((PSZ)AreaInf.echotag, MsgArea->EchoTag);
                  MsgArea->Display[sizeof (AreaInf.title) - 1] = '\0';
                  strcpy ((PSZ)AreaInf.title, MsgArea->Display);
                  write (fd, &AreaInf, sizeof (AreaInf));
               } while (MsgArea->Next () == TRUE);
            delete MsgArea;
         }

         close (fd);
      }

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

         if ((MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
            Area = 1;
            if (MsgArea->First () == TRUE)
               do {
                  if (LastRead->First () == TRUE)
                     do {
                        if (!stricmp (MsgArea->Key, NewLR.Key) && LastRead->QuickScan == TRUE) {
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
                     } while (LastRead->Next () == TRUE);
                  Area++;
               } while (MsgArea->Next () == TRUE);
            delete MsgArea;
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

USHORT TBlueWave::FetchReply (VOID)
{
   return (FALSE);
}

VOID TBlueWave::PackArea (PSZ pszKey, ULONG &ulLast)
{
   int fdm, fdfti, fdmix;
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

         sprintf (Temp, "%s%s.FTI", Work, Id);
         fdfti = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
         lseek (fdfti, 0L, SEEK_END);

         sprintf (Temp, "%s%s.MIX", Work, Id);
         fdmix = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
         lseek (fdmix, 0L, SEEK_END);

         sprintf (Temp, "%s%s.DAT", Work, Id);
         fdm = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
         lseek (fdm, 0L, SEEK_END);

         memset (&Mix, 0, sizeof (Mix));
         sprintf ((PSZ)Mix.areanum, "%u", Area);
         Mix.msghptr = tell (fdfti);

         if (Msg != NULL && fdfti != -1 && fdmix != -1 && fdm != -1) {
            if (Msg->Next (Number) == TRUE) {
               do {
                  Msg->Read (Number);

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

                  memset (&Fti, 0, sizeof (Fti));
                  Msg->From[sizeof (Fti.from) - 1] = '\0';
                  strcpy ((PSZ)Fti.from, Msg->From);
                  Msg->To[sizeof (Fti.to) - 1] = '\0';
                  strcpy ((PSZ)Fti.to, Msg->To);
                  Msg->Subject[sizeof (Fti.subject) - 1] = '\0';
                  strcpy ((PSZ)Fti.subject, Msg->Subject);
                  sprintf ((PSZ)Fti.date, "%2d %.3s %2d %2d:%02d:%02d", Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Second);
                  Fti.msgnum = (tWORD)Number;
                  Fti.msgptr = tell (fdm);
                  if (Cfg->Address.First () == TRUE) {
                     Fti.orig_zone = Cfg->Address.Zone;
                     Fti.orig_net = Cfg->Address.Net;
                     Fti.orig_node = Cfg->Address.Node;
                  }

                  if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                     do {
// ----------------------------------------------------------------------
// Get rid of the FidoNet<tm> control lines that are only wasting space,
// and write only the useful text lines.
// ----------------------------------------------------------------------
                        if (Text[0] != 0x01 && strnicmp (Text, "SEEN-BY: ", 9)) {
                           Fti.msglength += write (fdm, Text, strlen (Text));
                           Fti.msglength += write (fdm, "\r\n", 2);
                        }
                     } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);

                  write (fdfti, &Fti, sizeof (Fti));

                  if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                     BarWidth = (USHORT)((Total * 61L) / TotalPack);
                     Bbs->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
                  }
               } while (Total < Limit && Msg->Next (Number) == TRUE);
            }
         }

         Mix.totmsgs = (tWORD)Current;
         Mix.numpers = (tWORD)Personal;
         write (fdmix, &Mix, sizeof (Mix));

         if (Msg != NULL)
            delete Msg;
         if (fdfti != -1)
            close (fdfti);
         if (fdmix != -1)
            close (fdmix);
         if (fdm != -1)
            close (fdm);
      }
      delete MsgArea;
   }

   ulLast = Number;
}

VOID TBlueWave::PackEMail (ULONG &ulLast)
{
   int fdm, fdfti, fdmix;
   CHAR Temp[128], *Text;
   ULONG Number;

   Number = ulLast;
   Current = Personal = 0L;

   sprintf (Temp, "%s%s", Cfg->MailSpool, User->MailBox);
   if ((Msg = new SQUISH (Temp)) != NULL) {
      sprintf (Temp, "%s%s.FTI", Work, Id);
      fdfti = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
      lseek (fdfti, 0L, SEEK_END);

      sprintf (Temp, "%s%s.MIX", Work, Id);
      fdmix = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
      lseek (fdmix, 0L, SEEK_END);

      sprintf (Temp, "%s%s.DAT", Work, Id);
      fdm = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE);
      lseek (fdm, 0L, SEEK_END);

      memset (&Mix, 0, sizeof (Mix));
      sprintf ((PSZ)Mix.areanum, "%u", 0);
      Mix.msghptr = tell (fdfti);

      if (fdfti != -1 && fdmix != -1 && fdm != -1) {
         if (Msg->Next (Number) == TRUE) {
            do {
               Msg->Read (Number);

               Current++;
               Total++;
               Personal++;
               TotalPersonal++;

               memset (&Fti, 0, sizeof (Fti));
               Msg->From[sizeof (Fti.from) - 1] = '\0';
               strcpy ((PSZ)Fti.from, Msg->From);
               Msg->To[sizeof (Fti.to) - 1] = '\0';
               strcpy ((PSZ)Fti.to, Msg->To);
               Msg->Subject[sizeof (Fti.subject) - 1] = '\0';
               strcpy ((PSZ)Fti.subject, Msg->Subject);
               sprintf ((PSZ)Fti.date, "%2d %.3s %2d %2d:%02d:%02d", Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Second);
               Fti.msgnum = (tWORD)Number;
               Fti.msgptr = tell (fdm);
               if (Cfg->Address.First () == TRUE) {
                  Fti.orig_zone = Cfg->Address.Zone;
                  Fti.orig_net = Cfg->Address.Net;
                  Fti.orig_node = Cfg->Address.Node;
               }

               if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                  do {
// ----------------------------------------------------------------------
// Get rid of the FidoNet<tm> control lines that are only wasting space,
// and write only the useful text lines.
// ----------------------------------------------------------------------
                     if (Text[0] != 0x01 && strnicmp (Text, "SEEN-BY: ", 9)) {
                        Fti.msglength += write (fdm, Text, strlen (Text));
                        Fti.msglength += write (fdm, "\r\n", 2);
                     }
                  } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);

               write (fdfti, &Fti, sizeof (Fti));

               if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                  BarWidth = (USHORT)((Total * 61L) / TotalPack);
                  Bbs->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
               }
            } while (Total < Limit && Msg->Next (Number) == TRUE);
         }
      }

      Mix.totmsgs = (tWORD)Current;
      Mix.numpers = (tWORD)Personal;
      write (fdmix, &Mix, sizeof (Mix));

      if (Msg != NULL)
         delete Msg;
      if (fdfti != -1)
         close (fdfti);
      if (fdmix != -1)
         close (fdmix);
      if (fdm != -1)
         close (fdm);
   }

   ulLast = Number;
}



