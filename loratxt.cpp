
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 2.99.20
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"
#include "lorawin.h"

#define MAIL_IMPORTKNOWN         0x0001L
#define MAIL_IMPORTPROTECTED     0x0002L
#define MAIL_IMPORTNORMAL        0x0004L
#define MAIL_EXPORT              0x0008L
#define MAIL_PACK                0x0010L
#define MAIL_NEWSGROUP           0x0020L
#define MAIL_TIC                 0x0040L
#define MAIL_EMAIL               0x0080L
#define MAIL_NOEXTERNAL          0x2000L
#define MAIL_STARTTIMER          0x4000L
#define MAIL_POSTQUIT            0x8000L

#if defined(__OS2__)
#define IDOK            1
#define IDCANCEL        2
#endif

#define INITIALIZE         1
#define WAITFOROK          2
#define WAITFORCALL        3
#define ANSWERING          4
#define HANGUP             5
#define WAITFORCONNECT     6

#define MODEM_DELAY        200
#define EVENTS_DELAY       10000

short  last_sel = 100;
USHORT Status, Current, Daemon = FALSE;
CHAR   PollNode[64], ExternalProgram[128];
LONG   TimeOut, CallDelay = 0L, ModemT, EventsT;
class  TConfig *Cfg;
class  TModem *Modem;
class  TPMLog *Log;
class  TEvents *Events;
class  TOutbound *Outbound;

VOID DisplayScreen (VOID)
{
   CHAR Temp[128];

   videoinit ();

   if (Daemon == FALSE) {
      hidecur ();
      if (wopen (0, 0, 24, 79, 5, LGREY|_BLACK, LGREY|_BLACK) != 0) {
         box_ (1, 0, 24, 79, 0, LGREY|_BLACK);
         whline (12, 0, 80, 0, LGREY|_BLACK);
         whline (21, 0, 80, 0, LGREY|_BLACK);
         wvline (1, 48, 12, 0, LGREY|_BLACK);
         if (Cfg->MailAddress.First () == TRUE)
            prints (0, 1, LGREEN|_BLACK, Cfg->MailAddress.String);
         sprintf (Temp, "%s v%s", NAME, VERSION);
         prints (0, (USHORT)(78 - strlen (Temp)), LGREEN|_BLACK, Temp);
         prints (1, 2, LCYAN|_BLACK, "LOG");
         prints (1, 50, LCYAN|_BLACK, "MODEM");
         prints (12, 2, LCYAN|_BLACK, "OUTBOUND / REMOTE");
         prints (21, 2, LCYAN|_BLACK, "STATUS");
      }

      videoupdate ();
   }
}

VOID RefreshOutbound (VOID)
{
   USHORT i;
   CHAR Flags[16], Temp[128], Status[32];

   if (Daemon == FALSE) {
      i = 13;
      fill_ (13, 1, 20, 78, ' ', LGREY|_BLACK);

      if (Outbound->FirstNode () == TRUE) {
         prints (i++, 2, YELLOW|_BLACK, "Node               Try/Con  Type       Size     Status");
         do {
            Flags[0] = (Outbound->Normal == TRUE) ? 'N' : ' ';
            Flags[1] = (Outbound->Crash == TRUE) ? 'C' : ' ';
            Flags[2] = (Outbound->Direct == TRUE) ? 'D' : ' ';
            Flags[3] = (Outbound->Hold == TRUE) ? 'H' : ' ';
            Flags[4] = (Outbound->Immediate == TRUE) ? 'I' : ' ';
            Flags[5] = '\0';
            if (Outbound->Normal == FALSE && Outbound->Crash == FALSE && Outbound->Direct == FALSE && Outbound->Immediate == FALSE)
               strcpy (Status, "Hold");
            else {
               strcpy (Status, "Temporary Hold");
               if (Outbound->Normal == TRUE && Events->SendNormal == TRUE)
                  strcpy (Status, Outbound->LastCall);
               else if (Outbound->Crash == TRUE && Events->SendCrash == TRUE)
                  strcpy (Status, Outbound->LastCall);
               else if (Outbound->Direct == TRUE && Events->SendDirect == TRUE)
                  strcpy (Status, Outbound->LastCall);
               else if (Outbound->Immediate == TRUE && Events->SendImmediate == TRUE)
                  strcpy (Status, Outbound->LastCall);
            }
            if (Events != NULL) {
               if (Events->MaxCalls != 0 && Outbound->Attempts >= Events->MaxCalls)
                  strcpy (Status, "Undialable (Attempts)");
               else if (Events->MaxConnects != 0 && Outbound->Failed >= Events->MaxConnects)
                  strcpy (Status, "Undialable (Failures)");
            }
            sprintf (Temp, "%-16.16s  %3d %3d   %s  %8lub    %s", Outbound->Address, Outbound->Attempts, Outbound->Failed, Flags, Outbound->Size, Status);
            prints (i++, 2, LCYAN|_BLACK, Temp);
         } while (i < 21 && Outbound->NextNode () == TRUE);
      }
      else
         prints (17, 2, YELLOW|_BLACK, "                      Nothing in outbound area");

      videoupdate ();
   }
}

VOID ClearScreen (VOID)
{
   wcloseall ();
   showcur ();
   videoupdate ();
   closevideo ();
}

USHORT CPollDlg (VOID)
{
   short i;
   CHAR Temp[64], String[128], Flag[8];
   class TAddress Addr;
   class TNodes *Nodes;

   Flag[0] = Temp[0] = '\0';
   if (wopen (10, 15, 12, 65, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
      wshadow (DGREY|_BLACK);
      wtitle (" Forced Poll ", TCENTER, WHITE|_LGREY);

      wprints (0, 1, WHITE|_GREEN, " Address ");
      winpbeg (WHITE|_BLUE, WHITE|_BLUE);
      winpdef (0, 11, Temp, "?????????????????????????????????????", 0, 2, NULL, 0);
      if (winpread () == W_ESCPRESS)
         Temp[0] = '\0';
      hidecur ();
      wclose ();
   }

   if (Temp[0] != '\0') {
      Cfg->MailAddress.First ();
      Addr.Parse (Temp);
      if (Addr.Zone == 0)
         Addr.Zone = Cfg->MailAddress.Zone;
      if (Addr.Net == 0)
         Addr.Net = Cfg->MailAddress.Net;
      Addr.Add ();
      Addr.First ();

      if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
         if (Nodes->Read (Addr) == TRUE) {
            wopen (9, 10, 14, 68, 0, WHITE|_LGREY, WHITE|_LGREY);
            wshadow (DGREY|_BLACK);
            wtitle (" Forced Poll ", TCENTER, WHITE|_LGREY);
            whline (2, 0, 58, 0, WHITE|_LGREY);
            wprints (0, 1, BLUE|_LGREY, Nodes->SystemName);
            wprints (0, (short)(56 - strlen (Nodes->Address)), BLUE|_LGREY, Nodes->Address);
            wprints (1, 1, BLUE|_LGREY, Nodes->Location);
            wprints (1, (short)(56 - strlen (Nodes->SysopName)), BLUE|_LGREY, Nodes->SysopName);

            wprints (3, 1, WHITE|_LGREY, "Priority:   (Normal/Crash/Direct/Immediate)");
            strcpy (Flag, "i");
            do {
               winpbeg (WHITE|_BLUE, WHITE|_BLUE);
               winpdef (3, 11, Flag, "?", 0, 2, NULL, 0);
               i = winpread ();
               Flag[0] = (CHAR)tolower (Flag[0]);
            } while (Flag[0] != 'i' && Flag[0] != 'n' && Flag[0] != 'c' && Flag[0] != 'd');

            hidecur ();
            wclose ();

            if (i == W_ESCPRESS)
               Temp[0] = '\0';
         }
         else {
            sprintf (String, "Node %s not found !", Addr.String);
            MessageBox (" Forced Poll ", String);
            Temp[0] = '\0';
         }
         delete Nodes;
      }
   }

   if (Temp[0] != '\0') {
      if (Outbound != NULL) {
         Outbound->PollNode (Addr.String, Flag[0]);

         if (Log != NULL)
            Log->Write ("+Building the outbound queue");
         Outbound->BuildQueue (Cfg->Outbound);
         unlink ("rescan.now");
         if (Log != NULL)
            Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);

         RefreshOutbound ();
      }
   }

   return (FALSE);
}

USHORT CRequestDlg (VOID)
{
   FILE *fp;
   short i;
   CHAR Temp[64], String[128], File[128], Flag[8], *p;
   class TAddress Addr;
   class TNodes *Nodes;

   Flag[0] = Temp[0] = '\0';
   String[0] = '\0';

   if (wopen (10, 15, 12, 65, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
      wshadow (DGREY|_BLACK);
      wtitle (" Request File(s) ", TCENTER, WHITE|_LGREY);

      wprints (0, 1, WHITE|_GREEN, " Address ");
      winpbeg (WHITE|_BLUE, WHITE|_BLUE);
      winpdef (0, 11, Temp, "?????????????????????????????????????", 0, 2, NULL, 0);
      if (winpread () == W_ESCPRESS)
         Temp[0] = '\0';
      hidecur ();
      wclose ();
   }

   if (Temp[0] != '\0') {
      Cfg->MailAddress.First ();
      Addr.Parse (Temp);
      if (Addr.Zone == 0)
         Addr.Zone = Cfg->MailAddress.Zone;
      if (Addr.Net == 0)
         Addr.Net = Cfg->MailAddress.Net;
      Addr.Add ();
      Addr.First ();

      if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
         if (Nodes->Read (Addr) == TRUE) {
            wopen (9, 10, 15, 68, 0, WHITE|_LGREY, WHITE|_LGREY);
            wshadow (DGREY|_BLACK);
            wtitle (" Request File(s) ", TCENTER, WHITE|_LGREY);
            whline (2, 0, 58, 0, WHITE|_LGREY);
            wprints (0, 1, BLUE|_LGREY, Nodes->SystemName);
            wprints (0, (short)(56 - strlen (Nodes->Address)), BLUE|_LGREY, Nodes->Address);
            wprints (1, 1, BLUE|_LGREY, Nodes->Location);
            wprints (1, (short)(56 - strlen (Nodes->SysopName)), BLUE|_LGREY, Nodes->SysopName);

            wprints (3, 1, WHITE|_LGREY, "File(s):");
            wprints (4, 1, WHITE|_LGREY, "Priority:   (Normal/Crash/Direct/Immediate)");

            String[0] = '\0';
            winpbeg (WHITE|_BLUE, WHITE|_BLUE);
            winpdef (3, 11, String, "?????????????????????????????????????", 0, 2, NULL, 0);
            i = winpread ();
            strtrim (String);

            if (String[0] != '\0' && i != W_ESCPRESS) {
               strcpy (Flag, "i");
               do {
                  winpbeg (WHITE|_BLUE, WHITE|_BLUE);
                  winpdef (4, 11, Flag, "?", 0, 2, NULL, 0);
                  i = winpread ();
                  Flag[0] = (CHAR)tolower (Flag[0]);
               } while (Flag[0] != 'i' && Flag[0] != 'n' && Flag[0] != 'c' && Flag[0] != 'd');
            }

            hidecur ();
            wclose ();

            if (i == W_ESCPRESS)
               String[0] = '\0';
         }
         else {
            sprintf (Temp, "Node %s not found !", Addr.String);
            MessageBox (" Request File(s) ", Temp);
            String[0] = '\0';
         }
         delete Nodes;
      }
   }

   if (String[0] != '\0') {
      strcpy (Temp, Cfg->Outbound);
      Temp[strlen (Temp) - 1] = '\0';

      if (Cfg->MailAddress.Zone != Addr.Zone) {
         sprintf (File, "%s.%03x", Temp, Addr.Zone);
#if defined(__LINUX__)
         mkdir (File, 0666);
#else
         mkdir (File);
#endif
         if (Addr.Point != 0) {
#if defined(__LINUX__)
            sprintf (File, "%s.%03x/%04x%04x.pnt", Temp, Addr.Zone, Addr.Net, Addr.Node);
            mkdir (File, 0666);
            sprintf (File, "%s.%03x/%04x%04x.pnt/%08x.req", Temp, Addr.Zone, Addr.Net, Addr.Node, Addr.Point);
#else
            sprintf (File, "%s.%03x\\%04x%04x.pnt", Temp, Addr.Zone, Addr.Net, Addr.Node);
            mkdir (File);
            sprintf (File, "%s.%03x\\%04x%04x.pnt\\%08x.req", Temp, Addr.Zone, Addr.Net, Addr.Node, Addr.Point);
#endif
         }
         else
#if defined(__LINUX__)
            sprintf (File, "%s.%03x/%04x%04x.req", Temp, Addr.Zone, Addr.Net, Addr.Node);
#else
            sprintf (File, "%s.%03x\\%04x%04x.req", Temp, Addr.Zone, Addr.Net, Addr.Node);
#endif
      }
      else {
         if (Addr.Point != 0) {
#if defined(__LINUX__)
            sprintf (File, "%s/%04x%04x.pnt", Temp, Addr.Net, Addr.Node);
            mkdir (File, 0666);
            sprintf (File, "%s/%04x%04x.pnt/%08x.req", Temp, Addr.Net, Addr.Node, Addr.Point);
#else
            sprintf (File, "%s\\%04x%04x.pnt", Temp, Addr.Net, Addr.Node);
            mkdir (File);
            sprintf (File, "%s\\%04x%04x.pnt\\%08x.req", Temp, Addr.Net, Addr.Node, Addr.Point);
#endif
         }
         else
#if defined(__LINUX__)
            sprintf (File, "%s/%04x%04x.req", Temp, Addr.Net, Addr.Node);
#else
            sprintf (File, "%s\\%04x%04x.req", Temp, Addr.Net, Addr.Node);
#endif
      }

      if ((fp = fopen (File, "at")) != NULL) {
         if ((p = strtok (String, " ")) != NULL)
            do {
               fprintf (fp, "%s\n", p);
            } while ((p = strtok (NULL, " ")) != NULL);
         fclose (fp);
      }

      if (Outbound != NULL) {
         Outbound->PollNode (Addr.String, Flag[0]);

         if (Log != NULL)
            Log->Write ("+Building the outbound queue");
         Outbound->BuildQueue (Cfg->Outbound);
         unlink ("rescan.now");
         if (Log != NULL)
            Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);

         RefreshOutbound ();
      }
   }

   return (FALSE);
}

USHORT CAttachDlg (VOID)
{
   FILE *fp;
   short i;
   CHAR Temp[64], String[128], File[128], Flag[8], *p;
   class TAddress Addr;
   class TNodes *Nodes;

   Flag[0] = Temp[0] = '\0';
   String[0] = '\0';

   if (wopen (10, 15, 12, 65, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
      wshadow (DGREY|_BLACK);
      wtitle (" File Attach ", TCENTER, WHITE|_LGREY);

      wprints (0, 1, WHITE|_GREEN, " Address ");
      winpbeg (WHITE|_BLUE, WHITE|_BLUE);
      winpdef (0, 11, Temp, "?????????????????????????????????????", 0, 2, NULL, 0);
      if (winpread () == W_ESCPRESS)
         Temp[0] = '\0';
      hidecur ();
      wclose ();
   }

   if (Temp[0] != '\0') {
      Cfg->MailAddress.First ();
      Addr.Parse (Temp);
      if (Addr.Zone == 0)
         Addr.Zone = Cfg->MailAddress.Zone;
      if (Addr.Net == 0)
         Addr.Net = Cfg->MailAddress.Net;
      Addr.Add ();
      Addr.First ();

      if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
         if (Nodes->Read (Addr) == TRUE) {
            wopen (9, 10, 15, 68, 0, WHITE|_LGREY, WHITE|_LGREY);
            wshadow (DGREY|_BLACK);
            wtitle (" File Attach ", TCENTER, WHITE|_LGREY);
            whline (2, 0, 58, 0, WHITE|_LGREY);
            wprints (0, 1, BLUE|_LGREY, Nodes->SystemName);
            wprints (0, (short)(56 - strlen (Nodes->Address)), BLUE|_LGREY, Nodes->Address);
            wprints (1, 1, BLUE|_LGREY, Nodes->Location);
            wprints (1, (short)(56 - strlen (Nodes->SysopName)), BLUE|_LGREY, Nodes->SysopName);

            wprints (3, 1, WHITE|_LGREY, "File(s):");
            wprints (4, 1, WHITE|_LGREY, "Priority:   (Normal/Crash/Direct/Hold)");

            String[0] = '\0';
            winpbeg (WHITE|_BLUE, WHITE|_BLUE);
            winpdef (3, 11, String, "?????????????????????????????????????", 0, 2, NULL, 0);
            i = winpread ();
            strtrim (String);

            if (String[0] != '\0' && i != W_ESCPRESS) {
               strcpy (Flag, "h");
               do {
                  winpbeg (WHITE|_BLUE, WHITE|_BLUE);
                  winpdef (4, 11, Flag, "?", 0, 2, NULL, 0);
                  i = winpread ();
                  Flag[0] = (CHAR)tolower (Flag[0]);
               } while (Flag[0] != 'h' && Flag[0] != 'n' && Flag[0] != 'c' && Flag[0] != 'd');
               if (Flag[0] == 'n')
                  Flag[0] = 'f';
            }

            hidecur ();
            wclose ();

            if (i == W_ESCPRESS)
               String[0] = '\0';
         }
         else {
            sprintf (Temp, "Node %s not found !", Addr.String);
            MessageBox (" File Attach ", Temp);
            String[0] = '\0';
         }
         delete Nodes;
      }
   }

   if (String[0] != '\0') {
      strcpy (Temp, Cfg->Outbound);
      Temp[strlen (Temp) - 1] = '\0';

      if (Cfg->MailAddress.Zone != Addr.Zone) {
         sprintf (File, "%s.%03x", Temp, Addr.Zone);
#if defined(__LINUX__)
         mkdir (File, 0666);
         if (Addr.Point != 0) {
            sprintf (File, "%s.%03x/%04x%04x.pnt", Temp, Addr.Zone, Addr.Net, Addr.Node);
            mkdir (File, 0666);
            sprintf (File, "%s.%03x/%04x%04x.pnt/%08x.%clo", Temp, Addr.Zone, Addr.Net, Addr.Node, Addr.Point, Flag[0]);
         }
         else
            sprintf (File, "%s.%03x/%04x%04x.%clo", Temp, Addr.Zone, Addr.Net, Addr.Node, Flag[0]);
      }
      else {
         if (Addr.Point != 0) {
            sprintf (File, "%s/%04x%04x.pnt", Temp, Addr.Net, Addr.Node);
            mkdir (File, 0666);
            sprintf (File, "%s/%04x%04x.pnt/%08x.%clo", Temp, Addr.Net, Addr.Node, Addr.Point, Flag[0]);
         }
         else
            sprintf (File, "%s/%04x%04x.%clo", Temp, Addr.Net, Addr.Node, Flag[0]);
#else
         mkdir (File);
         if (Addr.Point != 0) {
            sprintf (File, "%s.%03x\\%04x%04x.pnt", Temp, Addr.Zone, Addr.Net, Addr.Node);
            mkdir (File);
            sprintf (File, "%s.%03x\\%04x%04x.pnt\\%08x.%clo", Temp, Addr.Zone, Addr.Net, Addr.Node, Addr.Point, Flag[0]);
         }
         else
            sprintf (File, "%s.%03x\\%04x%04x.%clo", Temp, Addr.Zone, Addr.Net, Addr.Node, Flag[0]);
      }
      else {
         if (Addr.Point != 0) {
            sprintf (File, "%s\\%04x%04x.pnt", Temp, Addr.Net, Addr.Node);
            mkdir (File);
            sprintf (File, "%s\\%04x%04x.pnt\\%08x.%clo", Temp, Addr.Net, Addr.Node, Addr.Point, Flag[0]);
         }
         else
            sprintf (File, "%s\\%04x%04x.%clo", Temp, Addr.Net, Addr.Node, Flag[0]);
#endif
      }

      if ((fp = fopen (File, "at")) != NULL) {
         if ((p = strtok (String, " ")) != NULL)
            do {
               fprintf (fp, "%s\n", p);
            } while ((p = strtok (NULL, " ")) != NULL);
         fclose (fp);
      }

      if (Outbound != NULL) {
         Outbound->PollNode (Addr.String, Flag[0]);

         if (Log != NULL)
            Log->Write ("+Building the outbound queue");
         Outbound->BuildQueue (Cfg->Outbound);
         unlink ("rescan.now");
         if (Log != NULL)
            Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);

         RefreshOutbound ();
      }
   }

   return (FALSE);
}

USHORT CRescanDlg (VOID)
{
   short i;
   CHAR Temp[64], String[128], *t, *p;
   class TAddress Addr;
   class TNodes *Nodes;
   class TAreaManager *AreaMgr;

   Temp[0] = '\0';
   String[0] = '\0';

   if (wopen (10, 15, 12, 65, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
      wshadow (DGREY|_BLACK);
      wtitle (" Rescan Area(s) ", TCENTER, WHITE|_LGREY);

      wprints (0, 1, WHITE|_GREEN, " Address ");
      winpbeg (WHITE|_BLUE, WHITE|_BLUE);
      winpdef (0, 11, Temp, "?????????????????????????????????????", 0, 2, NULL, 0);
      if (winpread () == W_ESCPRESS)
         Temp[0] = '\0';
      hidecur ();
      wclose ();
   }

   if (Temp[0] != '\0') {
      Cfg->MailAddress.First ();
      Addr.Parse (Temp);
      if (Addr.Zone == 0)
         Addr.Zone = Cfg->MailAddress.Zone;
      if (Addr.Net == 0)
         Addr.Net = Cfg->MailAddress.Net;
      Addr.Add ();
      Addr.First ();

      if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
         if (Nodes->Read (Addr) == TRUE) {
            wopen (9, 10, 14, 68, 0, WHITE|_LGREY, WHITE|_LGREY);
            wshadow (DGREY|_BLACK);
            wtitle (" Rescan Area(s) ", TCENTER, WHITE|_LGREY);
            whline (2, 0, 58, 0, WHITE|_LGREY);
            wprints (0, 1, BLUE|_LGREY, Nodes->SystemName);
            wprints (0, (short)(56 - strlen (Nodes->Address)), BLUE|_LGREY, Nodes->Address);
            wprints (1, 1, BLUE|_LGREY, Nodes->Location);
            wprints (1, (short)(56 - strlen (Nodes->SysopName)), BLUE|_LGREY, Nodes->SysopName);

            wprints (3, 1, WHITE|_LGREY, "Area(s):");

            String[0] = '\0';
            winpbeg (WHITE|_BLUE, WHITE|_BLUE);
            winpdef (3, 11, String, "?????????????????????????????????????", 0, 2, NULL, 0);
            i = winpread ();
            strtrim (String);

            hidecur ();
            wclose ();

            if (i == W_ESCPRESS)
               String[0] = '\0';
         }
         else {
            sprintf (Temp, "Node %s not found !", Addr.String);
            MessageBox (" Rescan Area(s) ", Temp);
            String[0] = '\0';
         }
         delete Nodes;
      }
   }

   if (String[0] != '\0') {
      if ((AreaMgr = new TAreaManager) != NULL) {
         AreaMgr->Cfg = Cfg;
         AreaMgr->Log = Log;

         t = String;
         while ((p = strtok (t, " ")) != NULL) {
            t = strtok (NULL, "");
            AreaMgr->Rescan (p, Addr.String);
         }

         delete AreaMgr;
      }
   }

   return (FALSE);
}

USHORT CNewEcholinkDlg (VOID)
{
   short i;
   CHAR Temp[64], String[128], *t, *p;
   class TAddress Addr;
   class TNodes *Nodes;
   class TAreaManager *AreaMgr;

   Temp[0] = '\0';
   String[0] = '\0';

   if (wopen (10, 15, 12, 65, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
      wshadow (DGREY|_BLACK);
      wtitle (" New Echomail Link ", TCENTER, WHITE|_LGREY);

      wprints (0, 1, WHITE|_GREEN, " Address ");
      winpbeg (WHITE|_BLUE, WHITE|_BLUE);
      winpdef (0, 11, Temp, "?????????????????????????????????????", 0, 2, NULL, 0);
      if (winpread () == W_ESCPRESS)
         Temp[0] = '\0';
      hidecur ();
      wclose ();
   }

   if (Temp[0] != '\0') {
      Cfg->MailAddress.First ();
      Addr.Parse (Temp);
      if (Addr.Zone == 0)
         Addr.Zone = Cfg->MailAddress.Zone;
      if (Addr.Net == 0)
         Addr.Net = Cfg->MailAddress.Net;
      Addr.Add ();
      Addr.First ();

      if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
         if (Nodes->Read (Addr) == TRUE) {
            wopen (9, 10, 14, 68, 0, WHITE|_LGREY, WHITE|_LGREY);
            wshadow (DGREY|_BLACK);
            wtitle (" New Echomail Link ", TCENTER, WHITE|_LGREY);
            whline (2, 0, 58, 0, WHITE|_LGREY);
            wprints (0, 1, BLUE|_LGREY, Nodes->SystemName);
            wprints (0, (short)(56 - strlen (Nodes->Address)), BLUE|_LGREY, Nodes->Address);
            wprints (1, 1, BLUE|_LGREY, Nodes->Location);
            wprints (1, (short)(56 - strlen (Nodes->SysopName)), BLUE|_LGREY, Nodes->SysopName);

            wprints (3, 1, WHITE|_LGREY, "Area(s):");

            String[0] = '\0';
            winpbeg (WHITE|_BLUE, WHITE|_BLUE);
            winpdef (3, 11, String, "?????????????????????????????????????", 0, 2, NULL, 0);
            i = winpread ();
            strtrim (String);

            hidecur ();
            wclose ();

            if (i == W_ESCPRESS)
               String[0] = '\0';
         }
         else {
            sprintf (Temp, "Node %s not found !", Addr.String);
            MessageBox (" New Echomail Link ", Temp);
            String[0] = '\0';
         }
         delete Nodes;
      }
   }

   if (String[0] != '\0') {
      if ((AreaMgr = new TAreaManager) != NULL) {
         AreaMgr->Cfg = Cfg;
         AreaMgr->Log = Log;

         t = String;
         while ((p = strtok (t, " ")) != NULL) {
            t = strtok (NULL, "");
            if (!stricmp (p, "%-ALL"))
               AreaMgr->RemoveAll (Addr.String);
            else if (*p == '-')
               AreaMgr->RemoveArea (Addr.String, ++p);
            else {
               if (*p == '+')
                  p++;
               AreaMgr->AddArea (Addr.String, p);
            }
         }

         delete AreaMgr;
      }
   }

   return (FALSE);
}

// ----------------------------------------------------------------------
// Product informations dialog
// ----------------------------------------------------------------------

USHORT CProductDlg (VOID)
{
   int menu_sel = 996;
   CHAR Temp[128], RegName[64], RegNumber[32];
   USHORT RetVal = FALSE;

   wopen (6, 13, 18, 67, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);

   DisplayButton (9, 22, "   Ok   ");

   sprintf (Temp, "%s v%s", NAME, VERSION);
   wcenters (0, BLUE|_LGREY, Temp);
#if defined(__DOS__)
   wcenters (1, BLUE|_LGREY, "Professional Edition for DOS");
#elif defined(__LINUX__)
   wcenters (1, BLUE|_LGREY, "Professional Edition for Linux");
#endif
   if (ValidateKey ("bbs", RegName, RegNumber) != KEY_UNREGISTERED) {
      sprintf (Temp, "Registered to %s", RegName);
      wcenters (3, BLACK|_LGREY, Temp);
      sprintf (Temp, "Serial Number %s", RegNumber);
      wcenters (4, BLACK|_LGREY, Temp);
   }
   else
      wcenters (3, BLACK|_LGREY, "- Unregistered Evaluation Copy -");
   wcenters (6, WHITE|_LGREY, "Copyright (c) 1996-97 by Marco Maccaferri");
   wcenters (7, WHITE|_LGREY, "All rights reserved");

   do {
      wmenubegc ();
      wmenuitem (9, 22, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      switch (menu_sel = wmenuget ()) {
         case 996:
            RetVal = TRUE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

// ---------------------------------------------------------------------------
// Windowed log file display
// ---------------------------------------------------------------------------

TPMLog::TPMLog (void)
{
   if (Daemon == FALSE) {
      if ((window = wopen (2, 1, 11, 47, 5, LGREY|_BLACK, WHITE|_BLACK)) != 0)
         wprintf ("\n\n\n\n\n\n\n\n\n\n\n\n");
      videoupdate ();
   }
   Display = TRUE;
}

TPMLog::~TPMLog (void)
{
   if (window != 0) {
      wactiv (window);
      wclose ();
   }
}

VOID TPMLog::Write (PSZ pszFormat, ...)
{
   va_list arglist;
   time_t t;
   struct tm *timep;

   va_start (arglist, pszFormat);
   vsprintf (Buffer, pszFormat, arglist);
   va_end (arglist);

   t = time (NULL);
   timep = localtime (&t);
   sprintf (Temp, "%c %02d %3s %02d:%02d:%02d %s %s", Buffer[0], timep->tm_mday, Months[timep->tm_mon], timep->tm_hour, timep->tm_min, timep->tm_sec, "LORA", &Buffer[1]);

   if (fp != NULL) {
      fprintf (fp, "%s\n", Temp);
#if !defined(__NT__)
      fflush (fp);
#endif
   }

   if (window != 0 && Display == TRUE && Daemon == FALSE) {
      wactiv (window);
      sprintf (Temp, "%c %02d:%02d %s", Buffer[0], timep->tm_hour, timep->tm_min, &Buffer[1]);
      wprintf ("\n%.46s", Temp);
      videoupdate ();
   }
}

// ----------------------------------------------------------------------------
// Status window manager
// ----------------------------------------------------------------------------

TPMStatus::TPMStatus (void)
{
}

TPMStatus::~TPMStatus (void)
{
}

VOID TPMStatus::Clear (VOID)
{
   fill_ (22, 1, 23, 78, ' ', LGREY|_BLACK);
   videoupdate ();
}

VOID TPMStatus::SetLine (USHORT line, PSZ text, ...)
{
   va_list arglist;
   CHAR Temp[128];

   va_start (arglist, text);
   vsprintf (Temp, text, arglist);
   va_end (arglist);

   fill_ ((short)(22 + line), 1, (short)(22 + line), 78, ' ', LGREY|_BLACK);
   prints ((short)(22 + line), 2, WHITE|_BLACK, Temp);
   videoupdate ();
}

// ----------------------------------------------------------------------------
// File transfer progress indicator
// ----------------------------------------------------------------------------

class TPMProgress : public TProgress
{
public:
   TPMProgress (void);
   ~TPMProgress (void);

   VOID   Begin (VOID);
   VOID   End (VOID);
   VOID   Update (VOID);

private:
};

TPMProgress::TPMProgress (void)
{
}

TPMProgress::~TPMProgress (void)
{
}

VOID TPMProgress::Begin (VOID)
{
   CHAR Temp[128];

   if (Type == FILE_RECEIVING)
      sprintf (Temp, "Receiving %s; %lu bytes", RxFileName, RxSize);
   else if (Type == FILE_SENDING)
      sprintf (Temp, "Sending %s; %lu bytes", TxFileName, TxSize);
   fill_ (22, 1, 22, 78, ' ', LGREY|_BLACK);
   prints (22, 2, WHITE|_BLACK, Temp);
   videoupdate ();
}

VOID TPMProgress::Update (VOID)
{
   CHAR Temp[128];

   if (Type == FILE_RECEIVING)
      sprintf (Temp, "%8lu %5u", RxPosition, RxBlockSize);
   else if (Type == FILE_SENDING)
      sprintf (Temp, "%8lu %5u", TxPosition, TxBlockSize);
   fill_ (23, 1, 23, 78, ' ', LGREY|_BLACK);
   prints (23, 2, WHITE|_BLACK, Temp);
   videoupdate ();
}

VOID TPMProgress::End (VOID)
{
   fill_ (22, 1, 23, 78, ' ', LGREY|_BLACK);
   videoupdate ();
}

// ----------------------------------------------------------------------------
// Mailer status
// ----------------------------------------------------------------------------

#define CENTER_LINE     70

class TPMMailStatus : public TMailerStatus
{
public:
   TPMMailStatus (void);
   ~TPMMailStatus (void);

   VOID   Update (VOID);

private:
   CHAR   Temp[128];
};

TPMMailStatus::TPMMailStatus (void)
{
}

TPMMailStatus::~TPMMailStatus (void)
{
}

VOID TPMMailStatus::Update (VOID)
{
   CHAR Line[128], Num[16];

   fill_ (13, 1, 20, 78, ' ', LGREY|_BLACK);

   sprintf (Temp, "%s, %s, %s, %s", Address, SysopName, SystemName, Location);
   if (strlen (Temp) > CENTER_LINE)
      Temp[CENTER_LINE] = '\0';
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
   prints (13, 4, LGREY|_BLACK, Line);

   sprintf (Temp, "Connected at %lu baud with %s", Speed, Program);
   if (strlen (Temp) > CENTER_LINE)
      Temp[CENTER_LINE] = '\0';
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
   prints (14, 4, LGREY|_BLACK, Line);

   if (Akas[0] == '\0')
      strcpy (Temp, "AKAs: No aka presented");
   else
      sprintf (Temp, "AKAs: %s", Akas);
   if (strlen (Temp) > CENTER_LINE)
      Temp[CENTER_LINE] = '\0';
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
   prints (15, 4, LGREY|_BLACK, Line);

   strcpy (Temp, " ÚÄÄÄÄMailPKTÄÄÄÄÄÄÄDataÄÄÄÄÄ¿     ÚÄÄÄÄMailPKTÄÄÄÄÄÄÄDataÄÄÄÄÄ¿ ");
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
   prints (17, 4, LCYAN|_BLACK, Line);

   strcpy (Temp, "úúúúúúúúúúúúúúúúúúúúúúúúúúúúúúú   úúúúúúúúúúúúúúúúúúúúúúúúúúúúúúú");
   if (InPktFiles == 0xFFFFU)
      strcpy (Num, "N/A");
   else
      sprintf (Num, "%u", InPktFiles);
   memcpy (&Temp[11 - strlen (Num) + 1], Num, strlen (Num));
   if (InDataFiles == 0xFFFFU)
      strcpy (Num, "N/A");
   else
      sprintf (Num, "%u", InDataFiles);
   memcpy (&Temp[22 - strlen (Num) + 1], Num, strlen (Num));
   if (OutPktFiles == 0xFFFFU)
      strcpy (Num, "N/A");
   else
      sprintf (Num, "%u", OutPktFiles);
   memcpy (&Temp[45 - strlen (Num) + 1], Num, strlen (Num));
   if (OutDataFiles == 0xFFFFU)
      strcpy (Num, "N/A");
   else
      sprintf (Num, "%u", OutDataFiles);
   memcpy (&Temp[56 - strlen (Num) + 1], Num, strlen (Num));
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
   prints (18, 4, LCYAN|_BLACK, Line);

   strcpy (Temp, "úúúúúúúúúúúúúúúúúúúúúúúúúúúúúúú   úúúúúúúúúúúúúúúúúúúúúúúúúúúúúúú");
   if (InPktFiles == 0xFFFFU)
      strcpy (Num, "N/A");
   else
      sprintf (Num, "%lu", InPktBytes);
   memcpy (&Temp[11 - strlen (Num) + 1], Num, strlen (Num));
   if (InDataFiles == 0xFFFFU)
      strcpy (Num, "N/A");
   else
      sprintf (Num, "%lu", InDataBytes);
   memcpy (&Temp[22 - strlen (Num) + 1], Num, strlen (Num));
   if (OutPktFiles == 0xFFFFU)
      strcpy (Num, "N/A");
   else
      sprintf (Num, "%lu", OutPktBytes);
   memcpy (&Temp[45 - strlen (Num) + 1], Num, strlen (Num));
   if (OutDataFiles == 0xFFFFU)
      strcpy (Num, "N/A");
   else
      sprintf (Num, "%lu", OutDataBytes);
   memcpy (&Temp[56 - strlen (Num) + 1], Num, strlen (Num));
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
   prints (19, 4, LCYAN|_BLACK, Line);

   strcpy (Temp, " ÀÄÄÄÄÄÄINBOUND TRAFFICÄÄÄÄÄÄÙ     ÀÄÄÄÄÄOUTBOUND TRAFFICÄÄÄÄÄÄÙ ");
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
   prints (20, 4, LCYAN|_BLACK, Line);

   videoupdate ();
}

// ----------------------------------------------------------------------------
// Threads
// ----------------------------------------------------------------------------

void NodelistThread (PVOID *args)
{
   CompileNodelist ((USHORT)args);
}

VOID MailProcessorThread (PVOID Args)
{
   ULONG Actions = (ULONG)Args;
   CHAR Temp[128];
   class TMailProcessor *Processor;
   class TTicProcessor *Tic;

   Args = Args;

   if (Actions & MAIL_TIC) {
      if ((Tic = new TTicProcessor) != NULL) {
         Tic->Cfg = Cfg;
         Tic->Log = Log;
         if (Daemon == FALSE) {
            Tic->Output = new TPMList;
            if (Tic->Output != NULL)
               Tic->Output->Clear ();
         }
         if (Log != NULL)
            Log->Write ("+Processing inbound TICs");
         strcpy (Tic->Inbound, Cfg->NormalInbound);
         Tic->Import ();
         if (stricmp (Cfg->KnownInbound, Cfg->NormalInbound)) {
            strcpy (Tic->Inbound, Cfg->KnownInbound);
            Tic->Import ();
         }
         if (stricmp (Cfg->ProtectedInbound, Cfg->NormalInbound) && stricmp (Cfg->ProtectedInbound, Cfg->KnownInbound)) {
            strcpy (Tic->Inbound, Cfg->ProtectedInbound);
            Tic->Import ();
         }

         if (Tic->Output != NULL)
            delete Tic->Output;
         delete Tic;
      }
   }

   if ((Processor = new TMailProcessor) != NULL) {
      Processor->Cfg = Cfg;
      Processor->Log = Log;
      if (Daemon == FALSE) {
         Processor->Output = new TPMList;
         Processor->Status = new TPMStatus;
      }

      if (Actions & (MAIL_IMPORTNORMAL|MAIL_IMPORTKNOWN|MAIL_IMPORTPROTECTED)) {
         if (Cfg->ImportCmd[0] != '\0' && !(Actions & MAIL_NOEXTERNAL))
            RunExternal (Cfg->ImportCmd);
         else {
            if (Actions & MAIL_IMPORTNORMAL) {
               strcpy (Processor->Inbound, Cfg->NormalInbound);
               while (Processor->IsArcmail () == TRUE)
                  Processor->UnpackArcmail ();
               Processor->Import ();
            }
            if ((Actions & MAIL_IMPORTKNOWN) && stricmp (Cfg->KnownInbound, Cfg->NormalInbound)) {
               strcpy (Processor->Inbound, Cfg->KnownInbound);
               while (Processor->IsArcmail () == TRUE)
                  Processor->UnpackArcmail ();
               Processor->Import ();
            }
            if ((Actions & MAIL_IMPORTPROTECTED) && stricmp (Cfg->ProtectedInbound, Cfg->NormalInbound) && stricmp (Cfg->ProtectedInbound, Cfg->KnownInbound)) {
               strcpy (Processor->Inbound, Cfg->ProtectedInbound);
               while (Processor->IsArcmail () == TRUE)
                  Processor->UnpackArcmail ();
               Processor->Import ();
            }

            if (Processor->Packets == 0)
               Log->Write ("+No ECHOmail processed at this time");
            else
               Log->Write ("+%d packet(s): %lu NETmail, %lu ECHOmail, %lu Dupes, %lu Bad", Processor->Packets, Processor->NetMail, Processor->MsgTossed - Processor->NetMail, Processor->Duplicate, Processor->Bad);
         }
      }

      if (Actions & MAIL_NEWSGROUP) {
         Processor->News ();
         Log->Write ("+%lu message(s): %lu Sent, %lu Received", Processor->MsgSent, Processor->MsgTossed);
      }

      if (Actions & MAIL_EMAIL) {
         Processor->Mail ();
         Log->Write ("+%lu message(s): %lu Sent, %lu Received", Processor->MsgSent + Processor->MsgTossed, Processor->MsgSent, Processor->MsgTossed);
      }

      if ((Actions & MAIL_EXPORT) && (Actions & MAIL_PACK) && Cfg->SeparateNetMail == TRUE) {
         if (Cfg->PackCmd[0] != '\0' && !(Actions & MAIL_NOEXTERNAL))
            RunExternal (Cfg->PackCmd);
         else {
            Processor->ExportNetMail ();
            sprintf (Temp, "%sroute.cfg", Cfg->SystemPath);
            Processor->Pack (Temp);
         }
      }

      if (Actions & MAIL_EXPORT) {
         if (Cfg->ExportCmd[0] != '\0' && !(Actions & MAIL_NOEXTERNAL))
            RunExternal (Cfg->ExportCmd);
         else
            Processor->Export ();
      }

      if (Actions & MAIL_PACK) {
         if (Cfg->PackCmd[0] != '\0' && !(Actions & MAIL_NOEXTERNAL))
            RunExternal (Cfg->PackCmd);
         else {
            Processor->ExportNetMail ();
            sprintf (Temp, "%sroute.cfg", Cfg->SystemPath);
            Processor->Pack (Temp);
         }
      }

      if (Processor->Status != NULL)
         delete Processor->Status;
      if (Processor->Output != NULL)
         delete Processor->Output;
      delete Processor;
   }

   if ((Actions & MAIL_PACK) && Outbound != NULL) {
      if (Log != NULL)
         Log->Write ("+Building the outbound queue");
      Outbound->BuildQueue (Cfg->Outbound);
      unlink ("rescan.now");
      if (Log != NULL)
         Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);
   }

   if (!(Actions & MAIL_POSTQUIT)) {
      if (Log != NULL)
         Log->WriteBlank ();

      RefreshOutbound ();
   }
}

VOID BbsThread (PVOID Args)
{
   USHORT Remote;
   ULONG Flags;
   class TBbs *Bbs;

   Args = Args;
   Remote = REMOTE_NONE;

   if ((Bbs = new TBbs) != NULL) {
      Bbs->Log = Log;
      Bbs->Cfg = Cfg;
      Bbs->Events = Events;
      if (Daemon == FALSE) {
         Bbs->Progress = new TPMProgress;
         Bbs->MailerStatus = new TPMMailStatus;
         Bbs->Status = new TPMStatus;
      }
      if (Modem != NULL) {
         Bbs->Com = Modem->Serial;
         Bbs->Speed = Modem->Speed;
      }
      Bbs->Task = Cfg->TaskNumber;
      Bbs->Run ();
      Remote = Bbs->Remote;

      if (Bbs->Status != NULL) {
         Bbs->Status->Clear ();
         delete Bbs->Status;
      }
      if (Bbs->MailerStatus != NULL)
         delete Bbs->MailerStatus;
      if (Bbs->Progress != NULL)
         delete Bbs->Progress;

      delete Bbs;
   }

   Modem->SendCommand (Cfg->Hangup);
   if (Modem->Serial != NULL && Log != NULL) {
      if (Modem->Serial->Carrier () == TRUE)
         Log->Write ("!Unable to drop carrier");
   }

   if (Remote == REMOTE_MAILRECEIVED && Cfg->AfterMailCmd[0] != '\0') {
      if (Log != NULL)
         Log->Write (":Running %s", Cfg->AfterMailCmd);
      SpawnExternal (Cfg->AfterMailCmd);
   }
   else if (Remote == REMOTE_USER && Cfg->AfterCallerCmd[0] != '\0') {
      if (Log != NULL)
         Log->Write (":Running %s", Cfg->AfterCallerCmd);
      SpawnExternal (Cfg->AfterCallerCmd);
   }

   if ((Remote == REMOTE_MAILER || Remote == REMOTE_MAILRECEIVED) && Outbound != NULL) {
      if (Log != NULL)
         Log->Write ("+Building the outbound queue");
      Outbound->BuildQueue (Cfg->Outbound);
      Outbound->FirstNode ();
      if (Log != NULL)
         Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);

      RefreshOutbound ();
   }

   if (Log != NULL)
      Log->WriteBlank ();

   Flags = 0L;
   if (Remote == REMOTE_MAILRECEIVED && Events != NULL) {
      Flags |= MAIL_STARTTIMER;
      if (Events->ImportNormal == TRUE)
         Flags |= MAIL_IMPORTNORMAL;
      if (Events->ImportKnown == TRUE)
         Flags |= MAIL_IMPORTKNOWN;
      if (Events->ImportProtected == TRUE)
         Flags |= MAIL_IMPORTPROTECTED;
      if (Events->ExportMail == TRUE)
         Flags |= (MAIL_EXPORT|MAIL_PACK);
      MailProcessorThread ((PVOID)Flags);
   }
}

VOID LocalThread (PVOID Args)
{
   class TBbs *Bbs;
   class TScreen *Com;

   Args = Args;
   if (Log != NULL) {
      Log->Write ("+Connect Local");
      Log->Display = FALSE;
   }

   if ((Com = new TScreen) != NULL) {
      Com->Initialize ();
      if ((Bbs = new TBbs) != NULL) {
         Bbs->Com = Com;
         Bbs->Log = Log;
         Bbs->Cfg = Cfg;
         Bbs->Events = Events;
         Bbs->Task = Cfg->TaskNumber;
         Bbs->Local = TRUE;
         Bbs->Run ();
         delete Bbs;
      }
      delete Com;
   }

   if (Log != NULL) {
      Log->Display = TRUE;
      Log->WriteBlank ();
   }
}

VOID MailerThread (PVOID Args)
{
   USHORT RetVal = FALSE;
   ULONG Flags;
   class TDetect *Detect;

   Args = Args;

   if ((Detect = new TDetect) != NULL) {
      Detect->Log = Log;
      Detect->Cfg = Cfg;
      Detect->Events = Events;
      if (Daemon == FALSE) {
         Detect->Progress = new TPMProgress;
         Detect->MailerStatus = new TPMMailStatus;
         Detect->Status = new TPMStatus;
      }
      if (Modem != NULL) {
         Detect->Com = Modem->Serial;
         Detect->Speed = Modem->Speed;
      }
      if (strchr (PollNode, '/') != NULL || strchr (PollNode, ':') != NULL)
         Detect->Address.Add (PollNode);
      RetVal = Detect->RemoteMailer ();

      if (Detect->Status != NULL) {
         Detect->Status->Clear ();
         delete Detect->Status;
      }
      if (Detect->MailerStatus != NULL)
         delete Detect->MailerStatus;
      if (Detect->Progress != NULL)
         delete Detect->Progress;

      delete Detect;
   }

   Modem->SendCommand (Cfg->Hangup);
   if (Modem->Serial != NULL && Log != NULL) {
      if (Modem->Serial->Carrier () == TRUE)
         Log->Write ("!Unable to drop carrier");
   }

   if (RetVal == REMOTE_MAILRECEIVED && Cfg->AfterMailCmd[0] != '\0') {
      if (Log != NULL)
         Log->Write (":Running %s", Cfg->AfterMailCmd);
      SpawnExternal (Cfg->AfterMailCmd);
   }

   if ((RetVal == REMOTE_MAILER || RetVal == REMOTE_MAILRECEIVED) && Outbound != NULL) {
      if (Log != NULL)
         Log->Write ("+Building the outbound queue");
      Outbound->BuildQueue (Cfg->Outbound);
      Outbound->FirstNode ();
      if (Log != NULL)
         Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);
      RefreshOutbound ();
   }

   if (Log != NULL)
      Log->WriteBlank ();

   Flags = 0L;
   if (RetVal == REMOTE_MAILRECEIVED && Events != NULL) {
      Flags |= MAIL_STARTTIMER;
      if (Events->ImportNormal == TRUE)
         Flags |= MAIL_IMPORTNORMAL;
      if (Events->ImportKnown == TRUE)
         Flags |= MAIL_IMPORTKNOWN;
      if (Events->ImportProtected == TRUE)
         Flags |= MAIL_IMPORTPROTECTED;
      if (Events->ExportMail == TRUE)
         Flags |= (MAIL_EXPORT|MAIL_PACK);
      MailProcessorThread ((PVOID)Flags);
   }
}

VOID FaxReceiveThread (PVOID Args)
{
   CHAR Temp[128];
   class TFax *Fax;

   Args = Args;

   if (Cfg->FaxCommand[0] != '\0') {
#if defined(__LINUX__)
      sprintf (Temp, Cfg->FaxCommand, atoi (&Cfg->Device[3]), Cfg->Speed, Modem->Serial->hFile);
#elif defined(__DOS__)
      sprintf (Temp, Cfg->FaxCommand, atoi (&Cfg->Device[3]), Cfg->Speed, atoi (Cfg->Device) - 1);
#endif

      Log->Write ("+Spawning to %s", Temp);
      RunExternal (Temp);
      Log->Write (":Returned from %s", Temp);
   }
   else {
      if ((Fax = new TFax) != NULL) {
         Fax->Com = Modem->Serial;
         Fax->Log = Log;
         Fax->faxreceive ();
         delete Fax;
      }
   }

   Modem->SendCommand (Cfg->Hangup);
   if (Modem->Serial != NULL && Log != NULL) {
      if (Modem->Serial->Carrier () == TRUE)
         Log->Write ("!Unable to drop carrier");
   }
}

VOID SendInitThread (PVOID Args)
{
   Args = Args;
   Modem->SendCommand (Cfg->Initialize[Current]);
}

VOID ModemTimer (VOID)
{
   USHORT i;

   switch (Status) {
      case 0:
         if (Log != NULL && ValidateKey ("bbs", NULL, NULL) == KEY_UNREGISTERED) {
            Log->Write ("!WARNING: No license key found");
            Log->Write ("!Your system is limited to 2 lines");
/*
            if ((i = CheckExpiration ()) == 0) {
               Log->Write ("!This evaluation copy has expired");
               Status = 200;
            }
            else
               Log->Write ("!You have %d days left for evaluation", i);
*/
         }
         switch (ValidateKey ("bbs", NULL, NULL)) {
            case KEY_UNREGISTERED:
            case KEY_BASIC:
               if (Cfg->TaskNumber > 2) {
                  Log->Write ("!Invalid line number (%d)", Cfg->TaskNumber);
                  Status = 200;
               }
               break;
            case KEY_ADVANCED:
               if (Cfg->TaskNumber > 5) {
                  Log->Write ("!Invalid line number (%d)", Cfg->TaskNumber);
                  Status = 200;
               }
               break;
         }
         if (Status != 200) {
            if (Modem == NULL && Cfg != NULL) {
               if ((Modem = new TModem) != NULL) {
                  Modem->Log = Log;
                  strcpy (Modem->Device, Cfg->Device);
                  Modem->Speed = Cfg->Speed;
                  Modem->LockSpeed = Cfg->LockSpeed;
                  if (Cfg->Ring[0] != '\0')
                     strcpy (Modem->Ring, Cfg->Ring);
                  Modem->Initialize ();
                  Status = INITIALIZE;
                  Current = 0;
               }
            }
            if (Outbound != NULL) {
               if (Log != NULL)
                  Log->Write ("+Building the outbound queue");
               Outbound->BuildQueue (Cfg->Outbound);
               if (Outbound->FirstNode () == FALSE)
                  Outbound->New ();
               if (Log != NULL)
                  Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);
               RefreshOutbound ();
            }
         }
         break;

      case INITIALIZE:
         if (Modem != NULL && Cfg != NULL) {
            while (Current < 3 && Cfg->Initialize[Current][0] == '\0')
               Current++;
            if (Current >= 3) {
               Status = WAITFORCALL;
               TimeOut = TimerSet (10L * 6000L);
               Modem->Terminal = TRUE;
            }
            else {
               Modem->Terminal = FALSE;
               SendInitThread (NULL);
               Status = WAITFOROK;
               TimeOut = TimerSet (500);
            }
         }
         break;

      case WAITFOROK:
         if (Modem->GetResponse () == OK) {
            while (++Current < 3 && Cfg->Initialize[Current][0] == '\0')
               ;
            if (Current >= 3) {
               Status = WAITFORCALL;
               TimeOut = TimerSet (10L * 6000L);
            }
            else
               Status = INITIALIZE;
         }
         else if (TimeUp (TimeOut) == TRUE) {
            if (Log != NULL)
               Log->Write ("!Modem doesn't report 'Ok'");
            Status = INITIALIZE;
         }
         break;

      case WAITFORCALL:
         if ((i = Modem->GetResponse ()) == RING && Cfg != NULL) {
            Modem->SendCommand (Cfg->Answer);
            Status = ANSWERING;
            TimeOut = TimerSet (4500L);
         }
         else if (i == CONNECT) {
            BbsThread (NULL);
            Status = HANGUP;
         }
         else if (i == FAX) {
            FaxReceiveThread (NULL);
            Status = HANGUP;
         }
         else if (TimeUp (TimeOut) == TRUE) {
            Status = INITIALIZE;
            Current = 0;
         }
         break;

      case ANSWERING:
         if ((i = Modem->GetResponse ()) == CONNECT) {
            BbsThread (NULL);
            Status = HANGUP;
         }
         else if (i == FAX) {
            FaxReceiveThread (NULL);
            Status = HANGUP;
         }
         else if (i != NO_RESPONSE && i != RING)
            Status = INITIALIZE;
         else if (TimeUp (TimeOut) == TRUE) {
            if (Log != NULL)
               Log->Write ("!Answer timer expired");
            Status = HANGUP;
         }
         break;

      case HANGUP:
         Modem->Speed = Cfg->Speed;
         Modem->Initialize ();
         Status = INITIALIZE;
         Current = 0;
         if (Events != NULL)
            CallDelay = TimerSet ((ULONG)Events->CallDelay * 100L);
         break;

      case WAITFORCONNECT:
         if ((i = Modem->GetResponse ()) == CONNECT) {
            MailerThread (NULL);
            Status = HANGUP;
         }
         else if (i != NO_RESPONSE) {
            Status = INITIALIZE;
            Current = 0;
            if (Events != NULL)
               CallDelay = TimerSet ((ULONG)Events->CallDelay * 100L);
         }
         else if (TimeUp (TimeOut) == TRUE) {
            if (Log != NULL)
               Log->Write ("!Dialing timer expired");
            Status = HANGUP;
         }
         break;
   }

   ModemT = TimerSet (MODEM_DELAY / 10L);
}

VOID EventsTimer (PVOID Args)
{
   USHORT i, DoCall = FALSE;
   CHAR *p;
   struct stat statbuf;
   class TAddress Address;

   Args = Args;

   Events->TimeToNext ();
   if (Events->NextNumber != 0) {
      if ((p = (CHAR *)malloc (128)) != NULL) {
         sprintf (p, "Event %d starts in %d minute(s)", Events->NextNumber, Events->TimeRemain);
         if (Daemon == FALSE)
            prints (22, 2, WHITE|_BLACK, p);
         free (p);
      }

      Events->First ();
      for (i = 1; i < Events->NextNumber; i++)
         Events->Next ();

      if ((p = (CHAR *)malloc (128)) != NULL) {
         strcpy (p, "Flags for next event:");
         if (Events->MailOnly == FALSE)
            strcat (p, " B");
         if (Events->SendNormal == FALSE && Events->SendCrash == FALSE && Events->SendDirect == FALSE && Events->SendImmediate == FALSE)
            strcat (p, " R");
         if (Events->Force == TRUE)
            strcat (p, " F");
         if (Events->SendCrash == TRUE)
            strcat (p, " C");
         if (Events->SendDirect == TRUE)
            strcat (p, " D");
         if (Events->SendImmediate == TRUE)
            strcat (p, " I");
         if (Events->AllowRequests == FALSE)
            strcat (p, " N");
         if (Events->Dynamic == TRUE)
            strcat (p, " Y");
         if (Events->ForceCall == TRUE) {
            strcat (p, " P=");
            strcat (p, Events->Address);
         }
         if (Daemon == FALSE)
            prints (23, 2, WHITE|_BLACK, p);
         free (p);
      }
   }

   if (Events->SetCurrent () == TRUE) {
      if (Events->Started == TRUE) {
         if (Log != NULL) {
            if (Events->Label[0] != '\0')
               Log->Write (":Starting Event %d - %s", Events->Number, Events->Label);
            else
               Log->Write (":Starting Event %d", Events->Number);
         }

         if (Events->Command[0] != '\0') {
            Log->Write ("#Executing %s", Events->Command);
            RunExternal (Events->Command);
         }

         if (Events->StartImport == TRUE && Events->StartExport == TRUE && Cfg->UseSinglePass == TRUE) {
            RunExternal (Cfg->SinglePassCmd);
            RunExternal (Cfg->PackCmd);
         }
         else {
            if (Events->StartImport == TRUE) {
               if (Events->ImportNormal == TRUE || Events->ImportProtected == TRUE || Events->ImportKnown == TRUE)
                  RunExternal (Cfg->ImportCmd);
            }
            if (Events->StartExport == TRUE) {
               if (Cfg->SeparateNetMail == TRUE)
                  RunExternal (Cfg->PackCmd);
               RunExternal (Cfg->ExportCmd);
               RunExternal (Cfg->PackCmd);
            }
         }

         if (Outbound != NULL) {
            if (Log != NULL)
               Log->Write ("+Building the outbound queue");
            Outbound->BuildQueue (Cfg->Outbound);
            unlink ("rescan.now");

            if (Events->ForceCall == TRUE && Events->Address[0] != '\0') {
               Outbound->New ();
               Address.Parse (Events->Address);
               if (Cfg->MailAddress.First () == TRUE) {
                  if (Address.Zone == 0)
                     Address.Zone = Cfg->MailAddress.Zone;
                  if (Address.Net == 0)
                     Address.Net = Cfg->MailAddress.Net;
                  Address.Add ();
                  Address.First ();
               }
               Outbound->Zone = Address.Zone;
               Outbound->Net = Address.Net;
               Outbound->Node = Address.Node;
               Outbound->Point = Address.Point;
               Outbound->Poll = TRUE;
               Outbound->Crash = Events->SendCrash;
               Outbound->Direct = Events->SendDirect;
               Outbound->Normal = Events->SendNormal;
               Outbound->Immediate = Events->SendImmediate;
               Outbound->Add ();
            }
            else
               Outbound->FirstNode ();

            if (Log != NULL)
               Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);
            RefreshOutbound ();
         }

         CallDelay = TimerSet ((ULONG)Events->CallDelay * 100L);
         Events->Save ();
      }

      if (CallDelay == 0L)
         CallDelay = TimerSet ((ULONG)Events->CallDelay * 100L);

      if (TimeUp (CallDelay) && Outbound != NULL && Outbound->TotalNodes > 0 && Status != WAITFORCONNECT) {
         if (Events->Address[0] != '\0') {
            Address.Parse (Events->Address);
            if (Cfg->MailAddress.First () == TRUE) {
               if (Address.Zone == 0)
                  Address.Zone = Cfg->MailAddress.Zone;
               if (Address.Net == 0)
                  Address.Net = Cfg->MailAddress.Net;
               Address.Add ();
               Address.First ();
            }
            DoCall = FALSE;
            if (Outbound->FirstNode () == TRUE)
               do {
                  if (Outbound->Zone == Address.Zone && Outbound->Net == Address.Net && Outbound->Node == Address.Node && Outbound->Point == Address.Point) {
                     DoCall = TRUE;
                     break;
                  }
               } while (Outbound->NextNode () == TRUE);
         }

         if (Events->Address[0] == '\0' || DoCall == TRUE) {
            DoCall = FALSE;
            if (Events->SendCrash == TRUE && Outbound->Crash == TRUE)
               DoCall = TRUE;
            if (Events->SendDirect == TRUE && Outbound->Direct == TRUE)
               DoCall = TRUE;
            if (Events->SendNormal == TRUE && Outbound->Normal == TRUE)
               DoCall = TRUE;
            if (Events->SendImmediate == TRUE && Outbound->Immediate == TRUE)
               DoCall = TRUE;
         }

         if (DoCall == TRUE) {
            strcpy (PollNode, Outbound->Address);
            Modem->Terminal = FALSE;
            strcpy (Modem->NodelistPath, Cfg->NodelistPath);
            strcpy (Modem->DialCmd, Cfg->Dial);
            Modem->Poll (PollNode);
            Status = WAITFORCONNECT;
            TimeOut = TimerSet ((ULONG)Cfg->DialTimeout * 100L);
         }

         DoCall = FALSE;
         if (Events->Address[0] == '\0') {
            while (Outbound->NextNode () == TRUE) {
               if (Events->SendCrash == TRUE && Outbound->Crash == TRUE) {
                  DoCall = TRUE;
                  break;
               }
               if (Events->SendDirect == TRUE && Outbound->Direct == TRUE) {
                  DoCall = TRUE;
                  break;
               }
               if (Events->SendNormal == TRUE && Outbound->Normal == TRUE) {
                  DoCall = TRUE;
                  break;
               }
               if (Events->SendImmediate == TRUE && Outbound->Immediate == TRUE) {
                  DoCall = TRUE;
                  break;
               }
            }
            if (DoCall == FALSE) {
               if (Outbound->FirstNode () == TRUE)
                  do {
                     if (Events->SendCrash == TRUE && Outbound->Crash == TRUE) {
                        DoCall = TRUE;
                        break;
                     }
                     if (Events->SendDirect == TRUE && Outbound->Direct == TRUE) {
                        DoCall = TRUE;
                        break;
                     }
                     if (Events->SendNormal == TRUE && Outbound->Normal == TRUE) {
                        DoCall = TRUE;
                        break;
                     }
                     if (Events->SendImmediate == TRUE && Outbound->Immediate == TRUE) {
                        DoCall = TRUE;
                        break;
                     }
                  } while (Outbound->NextNode () == TRUE);
            }
         }
      }
   }

   if (stat ("rescan.now", &statbuf) == 0) {
      if (Log != NULL)
         Log->Write ("+Building the outbound queue");
      Outbound->BuildQueue (Cfg->Outbound);
      unlink ("rescan.now");
      if (Log != NULL)
         Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);
   }

   EventsT = TimerSet (EVENTS_DELAY / 10L);
   if (Daemon == FALSE)
      videoupdate ();
}

VOID AddShadow (VOID)
{
   wshadow (DGREY|_BLACK);
}

VOID AddFileShadow (VOID)
{
   whline (2, 0, 27, 3, RED|_LGREY);
   whline (4, 0, 27, 3, RED|_LGREY);
   wshadow (DGREY|_BLACK);
}

VOID AddUtilityShadow (VOID)
{
   whline (1, 0, 31, 3, RED|_LGREY);
   whline (5, 0, 31, 3, RED|_LGREY);
   whline (7, 0, 31, 3, RED|_LGREY);
   wshadow (DGREY|_BLACK);
}

VOID AddMailShadow (VOID)
{
   whline (3, 0, 31, 3, RED|_LGREY);
   whline (5, 0, 31, 3, RED|_LGREY);
   whline (9, 0, 31, 3, RED|_LGREY);
   wshadow (DGREY|_BLACK);
}

VOID AddConfigureShadow (VOID)
{
   whline (4, 0, 31, 3, RED|_LGREY);
   wshadow (DGREY|_BLACK);
}

VOID AddBBSShadow (VOID)
{
   whline (1, 0, 31, 3, RED|_LGREY);
   wshadow (DGREY|_BLACK);
}

VOID AddHelpShadow (VOID)
{
   whline (4, 0, 31, 3, RED|_LGREY);
   wshadow (DGREY|_BLACK);
}

VOID ProcessSelection (VOID)
{
   struct _item_t *item;

   item = wmenuicurr ();
   last_sel = item->tagid;
}

VOID ProcessMenu (VOID)
{
   USHORT RetVal = FALSE;

   wmenubeg (0, 0, 0, 79, 5, BLACK|_LGREY, BLACK|_LGREY, NULL);
   wmenuitem (0,  1, " File ", 0, 100, M_HASPD, NULL, 0, 0);
      wmenubeg (1, 1, 9, 28, 3, RED|_LGREY, BLUE|_LGREY, AddFileShadow);
      wmenuitem ( 0, 0, " Request file(s)    Alt-R ", 0, 107, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 1, 0, " Send file(s)       Alt-S ", 0, 108, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 3, 0, " Rebuild Outbound   Alt-Q ", 0, 106, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 5, 0, " Import AREAS.BBS         ", 0, 121, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 6, 0, " Write AREAS.BBS          ", 0, 117, M_CLALL, ProcessSelection, 0, 0);
      wmenuend (107, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
   wmenuitem (0,  7, " Utility ", 0, 200, M_HASPD, NULL, 0, 0);
      wmenubeg (1, 7, 11, 38, 3, RED|_LGREY, BLUE|_LGREY, AddUtilityShadow);
      wmenuitem ( 0, 0, " Forced poll            Alt-M ", 0, 109, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 2, 0, " Request ECHOmail link        ", 0, 111, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
      wmenuitem ( 3, 0, " New ECHOmail link            ", 0, 112, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 4, 0, " Rescan ECHOmail              ", 0, 113, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 6, 0, " New TIC file link            ", 0, 114, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
      wmenuitem ( 8, 0, " Build nodelist index         ", 0, 119, M_CLALL, ProcessSelection, 0, 0);
      wmenuend (109, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
   wmenuitem (0, 16, " Mail ", 0, 300, M_HASPD, NULL, 0, 0);
      wmenubeg (1, 16, 13, 45, 3, RED|_LGREY, BLUE|_LGREY, AddMailShadow);
      wmenuitem ( 0, 0, " Import Mail          Alt-I ", 0, 101, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 1, 0, " Export echomail            ", 0, 102, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 2, 0, " Pack NetMail               ", 0, 103, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 4, 0, " Process ECHOmail     Alt-P ", 0, 104, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 6, 0, " Process NEWSgroups         ", 0, 116, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 7, 0, " Process E-Mail             ", 0, 118, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 8, 0, " Process TIC files          ", 0, 105, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem (10, 0, " Import from bad msgs.      ", 0, 105, M_CLALL, ProcessSelection, 0, 0);
      wmenuend (101, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
   wmenuitem (0, 22, " Configure ", 0, 400, M_HASPD, NULL, 0, 0);
      wmenubeg (1, 22, 13, 43, 3, RED|_LGREY, BLUE|_LGREY, AddConfigureShadow);
      wmenuitem ( 0, 0, " Global          -> ", 0, 410, 0, NULL, 0, 0);
         wmenubeg (1, 44, 9, 64, 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
         wmenuitem (0, 0, " General Options   ", 0, 201, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (1, 0, " Site Informations ", 0, 202, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (2, 0, " Addresses         ", 0, 204, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (3, 0, " Directory / Paths ", 0, 208, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (4, 0, " Time Adjustment   ", 0, 205, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
         wmenuitem (5, 0, " Internet Options  ", 0, 206, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (6, 0, " Fax Options       ", 0, 207, M_CLALL, ProcessSelection, 0, 0);
         wmenuend (201, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
      wmenuitem ( 1, 0, " Mailer          -> ", 0, 420, 0, NULL, 0, 0);
         wmenubeg (2, 44, 9, 66, 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
         wmenuitem ( 0, 0, " Miscellaneous       ", 0, 310, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem ( 1, 0, " Mail Processing     ", 0, 311, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem ( 2, 0, " Areafix             ", 0, 312, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem ( 3, 0, " TIC Processor       ", 0, 301, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
         wmenuitem ( 4, 0, " Files Requests      ", 0, 302, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
         wmenuitem ( 5, 0, " External Processing ", 0, 304, M_CLALL, ProcessSelection, 0, 0);
         wmenuend (310, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
      wmenuitem ( 2, 0, " BBS             -> ", 0, 430, 0, NULL, 0, 0);
         wmenubeg (3, 44, 12, 65, 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
         wmenuitem (0, 0, " Message Areas      ", 0, 401, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (1, 0, " File Areas         ", 0, 402, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (2, 0, " General Options    ", 0, 403, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (3, 0, " Offline Reader     ", 0, 404, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (4, 0, " New Users          ", 0, 405, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (5, 0, " User Limits        ", 0, 406, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (6, 0, " Paging Hours       ", 0, 407, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
         wmenuitem (7, 0, " External Protocols ", 0, 408, M_CLALL, ProcessSelection, 0, 0);
         wmenuend (401, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
      wmenuitem ( 3, 0, " Modem           -> ", 0, 440, 0, NULL, 0, 0);
         wmenubeg (4, 44, 9, 67, 3, RED|_LGREY, BLUE|_LGREY, AddShadow);
         wmenuitem (0, 0, " Hardware              ", 0, 501, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (1, 0, " Command Strings       ", 0, 502, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (2, 0, " Answer Control        ", 0, 503, M_CLALL, ProcessSelection, 0, 0);
         wmenuitem (3, 0, " Nodelist Flags        ", 0, 504, M_CLALL, ProcessSelection, 0, 0);
         wmenuend (501, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
      wmenuitem ( 5, 0, " Event Scheduler    ", 0, 601, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 6, 0, " Nodelist           ", 0, 602, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 7, 0, " Compressors        ", 0, 603, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 8, 0, " Nodes              ", 0, 604, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 9, 0, " Menu Files         ", 0, 605, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem (10, 0, " User Editor        ", 0, 606, M_CLALL, ProcessSelection, 0, 0);
      wmenuend (410, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
   wmenuitem (0, 33, " BBS ", 0, 500, M_HASPD, NULL, 0, 0);
      wmenubeg (1, 33, 6, 55, 3, RED|_LGREY, BLUE|_LGREY, AddBBSShadow);
      wmenuitem ( 0, 0, " Local Login   Alt-K ", 0, 115, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 2, 0, " Answer Now    Alt-A ", 0, 506, M_CLALL, ProcessSelection, 0, 0);
      wmenuitem ( 3, 0, " Hangup        Alt-H ", 0, 505, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
      wmenuend (115, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);
   wmenuitem (0, 38, " Help ", 0, 600, M_HASPD, NULL, 0, 0);
      wmenubeg (1, 38, 8, 61, 3, RED|_LGREY, BLUE|_LGREY, AddHelpShadow);
      wmenuitem ( 0, 0, " Help index...        ", 0, 901, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
      wmenuitem ( 1, 0, " General help...      ", 0, 902, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
      wmenuitem ( 2, 0, " Using help...        ", 0, 903, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
      wmenuitem ( 3, 0, " Keys help...         ", 0, 904, M_CLALL|M_NOSEL, ProcessSelection, 0, 0);
      wmenuitem ( 5, 0, " Product informations ", 0, 905, M_CLALL, ProcessSelection, 0, 0);
      wmenuend (901, M_PD|M_SAVE, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);

   last_sel = (short)((last_sel / 100) * 100);
   wmenuend (last_sel, M_OMNI, 0, 0, BLUE|_LGREY, WHITE|_LGREY, DGREY|_LGREY, YELLOW|_BLACK);

   kbput (0x1C0D);
   if (wmenuget () != -1) {
      switch (last_sel) {
         case 101:
            MailProcessorThread ((PVOID)(MAIL_IMPORTKNOWN|MAIL_IMPORTPROTECTED|MAIL_IMPORTNORMAL));
            break;
         case 102:
            MailProcessorThread ((PVOID)(MAIL_EXPORT));
            break;
         case 103:      // System / Pack mail
            MailProcessorThread ((PVOID)(MAIL_PACK|MAIL_STARTTIMER));
            break;
         case 104:      // System / Process ECHOmail
            MailProcessorThread ((PVOID)(MAIL_IMPORTKNOWN|MAIL_IMPORTPROTECTED|MAIL_IMPORTNORMAL|MAIL_EXPORT|MAIL_PACK|MAIL_STARTTIMER));
            break;
         case 105:      // System / Process TIC
            MailProcessorThread ((PVOID)(MAIL_TIC|MAIL_STARTTIMER));
            break;
         case 106:
            if (Outbound != NULL) {
               if (Log != NULL)
                  Log->Write ("+Building the outbound queue");
               Outbound->BuildQueue (Cfg->Outbound);
               unlink ("rescan.now");
               if (Log != NULL)
                  Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);

               RefreshOutbound ();
            }
            break;
         case 107:
            CRequestDlg ();
            break;
         case 108:
            CAttachDlg ();
            break;
         case 109:
            CPollDlg ();
            break;
         case 112:
            CNewEcholinkDlg ();
            break;
         case 113:
            CRescanDlg ();
            break;
         case 115:
            kbput (0x2500);
            break;
         case 116:      // System / Process Newsgroups
            MailProcessorThread ((PVOID)(MAIL_NEWSGROUP|MAIL_STARTTIMER));
            break;
         case 117: {    // System / EchoMail / Write AREAS.BBS
            class TAreaManager *Mgr;

            if ((Mgr = new TAreaManager) != NULL) {
               Mgr->Cfg = Cfg;
               Mgr->Log = Log;
               Mgr->UpdateAreasBBS ();
               delete Mgr;
            }
            break;
         }
         case 118:      // System / Process E-Mail
            MailProcessorThread ((PVOID)(MAIL_EMAIL|MAIL_STARTTIMER));
            break;
         case 119:
            CompileNodelist (TRUE);
            break;
         case 121: {    // System / EchoMail / Import AREAS.BBS
            class TAreaManager *Mgr;

            if ((Mgr = new TAreaManager) != NULL) {
               Mgr->Cfg = Cfg;
               Mgr->Log = Log;
               Mgr->ImportAreasBBS ();
               delete Mgr;
            }
            break;
         }
         case 130:
            kbput (0x2d00);
            break;
         case 201:
            if ((RetVal = CGeneralOptDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 202:
            if ((RetVal = CSiteInfoDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 204:
            if ((RetVal = CAddressDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 206:
            if ((RetVal = CInternetDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 207:
            if ((RetVal = CFaxDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 208:
            if ((RetVal = CDirectoriesDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 304:
            if ((RetVal = CExternalProcDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 310:
            if ((RetVal = CMailerMiscDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 311:
            if ((RetVal = CMailProcessingDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 312:
            if ((RetVal = CAreafixDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 401:
            CMessageDlg ();
            break;
         case 402:
            CFileDlg ();
            break;
         case 403:
            if ((RetVal = CBBSGeneralDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 404:
            if ((RetVal = COfflineDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 405:
            if ((RetVal = CNewUsersDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 501:
            if ((RetVal = CHardwareDlg ()) == TRUE) {
               Cfg->Save ();
               if (Modem != NULL) {
                  delete Modem;
                  Modem = NULL;
               }
               if (Modem == NULL && Cfg != NULL) {
                  if ((Modem = new TModem) != NULL) {
                     Modem->Log = Log;
                     strcpy (Modem->Device, Cfg->Device);
                     Modem->Speed = Cfg->Speed;
                     Modem->LockSpeed = Cfg->LockSpeed;
                     if (Cfg->Ring[0] != '\0')
                        strcpy (Modem->Ring, Cfg->Ring);
                     Modem->Initialize ();
                     Status = INITIALIZE;
                     Current = 0;
                  }
               }
            }
            break;
         case 502:
            if ((RetVal = CCommandsDlg ()) == TRUE) {
               Cfg->Save ();
               if (Modem != NULL) {
                  delete Modem;
                  Modem = NULL;
               }
               if (Modem == NULL && Cfg != NULL) {
                  if ((Modem = new TModem) != NULL) {
                     Modem->Log = Log;
                     strcpy (Modem->Device, Cfg->Device);
                     Modem->Speed = Cfg->Speed;
                     Modem->LockSpeed = Cfg->LockSpeed;
                     if (Cfg->Ring[0] != '\0')
                        strcpy (Modem->Ring, Cfg->Ring);
                     Modem->Initialize ();
                     Status = INITIALIZE;
                     Current = 0;
                  }
               }
            }
            break;
         case 503:
            if ((RetVal = CAnswerDlg ()) == TRUE)
               Cfg->Save ();
            break;
         case 504:
            CNodeFlagsDlg ();
            break;
         case 506:
            Modem->Serial->SendBytes ((UCHAR *)"ATA\r", 4);
            Status = ANSWERING;
            TimeOut = TimerSet (4500L);
            break;
         case 601:
            CEventDlg ();
            Events->Load ();
            break;
         case 602:
            CNodelistDlg ();
            break;
         case 603:
            CCompressorDlg ();
            break;
         case 604:
            CNodesDlg ();
            break;
         case 605: {
            CHAR Temp[128], *p = NULL;

            Temp[0] = '\0';
            if (wopen (10, 15, 12, 65, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
               wshadow (DGREY|_BLACK);
               wtitle (" Menu Editor ", TCENTER, WHITE|_LGREY);

               wprints (0, 1, WHITE|_GREEN, " Filename ");
               winpbeg (WHITE|_BLUE, WHITE|_BLUE);
               winpdef (0, 12, Temp, "????????????????????????????????????", 0, 2, NULL, 0);
               if (winpread () == W_ESCPRESS)
                  Temp[0] = '\0';
               else {
                  while (strlen (Temp) > 0 && Temp[strlen (Temp) - 1] == ' ')
                     Temp[strlen (Temp) - 1] = '\0';
                  p = Temp;
               }
               hidecur ();
               wclose ();
            }

            if (p != NULL && *p == '\0') {
               sprintf (Temp, "%s*.mnu", Cfg->MenuPath);
               p = wpickfile (6, 8, 18, 71, 1, WHITE|_LGREY, WHITE|_LGREY, WHITE|_GREEN, 1, Temp, AddShadow);
            }

            if (p != NULL)
               CMenuEditorDlg (p);
            break;
         }
         case 606:
            CUserDlg ();
            break;
         case 905:
            CProductDlg ();
            break;
      }
   }

   if (RetVal == FALSE)
      Cfg->Reload ();
}

void main (int argc, char *argv[])
{
   int i;
   USHORT Task = 1, Local, Poll, CanExit, EndRun, Interactive, Setup;
   USHORT DoImport, DoExport, DoPack, DoNews, DoTic, DoNodelist, DoMail;
   CHAR *Config, *Channel, *Device, Temp[128];
   ULONG Flags, Speed;

   Log = NULL;
   Modem = NULL;
   Events = NULL;
   Outbound = NULL;
   DoImport = DoExport = DoPack = DoNews = DoTic = DoNodelist = DoMail = FALSE;
   Setup = Interactive = FALSE;
   Config = Channel = Device = NULL;
   Speed = 0L;
   Local = Poll = FALSE;
   Daemon = FALSE;

   for (i = 1; i < argc; i++) {
      if (!stricmp (argv[i], "/LINE")) {
         i++;
         Task = (USHORT)atoi (argv[i]);
      }
      else if (!strncmp (argv[i], "-n", 2))
         Task = (USHORT)atoi (&argv[i][2]);
      else if (!stricmp (argv[i], "IMPORT") || !stricmp (argv[i], "IN") || !stricmp (argv[i], "TOSS")) {
         DoImport = TRUE;
         Interactive = TRUE;
      }
      else if (!stricmp (argv[i], "EXPORT") || !stricmp (argv[i], "OUT") || !stricmp (argv[i], "SCAN")) {
         DoExport = TRUE;
         Interactive = TRUE;
      }
      else if (!stricmp (argv[i], "PACK")) {
         DoPack = TRUE;
         Interactive = TRUE;
      }
      else if (!stricmp (argv[i], "NEWS")) {
         DoNews = TRUE;
         Interactive = TRUE;
      }
      else if (!stricmp (argv[i], "MAIL")) {
         DoMail = TRUE;
         Interactive = TRUE;
      }
      else if (!stricmp (argv[i], "POLL")) {
         i++;
         strcpy (PollNode, argv[i]);
         Poll = TRUE;
      }
      else if (!stricmp (argv[i], "TIC"))
         DoTic = TRUE;
      else if (!stricmp (argv[i], "NODELIST")) {
         DoNodelist = TRUE;
         Interactive = TRUE;
      }
      else if (!stricmp (argv[i], "SETUP")) {
         Interactive = FALSE;
         Setup = TRUE;
         Daemon = FALSE;
      }
      else if (!strncmp (argv[i], "-p", 2))
         Device = &argv[i][2];
      else if (!strncmp (argv[i], "-b", 2))
         Speed = atol (&argv[i][2]);
      else if (!strcmp (argv[i], "-l"))
         Local = TRUE;
#if defined(__LINUX__)
      else if (!strcmp (argv[i], "-daemon"))
         Daemon = TRUE;
#endif
      else if (Config == NULL)
         Config = argv[i];
      else if (Channel == NULL)
         Channel = argv[i];
   }

   if (Config == NULL)
      Config = getenv ("LORA_CONFIG");
   if (Channel == NULL)
      Channel = getenv ("LORA_CHANNEL");

   if ((Cfg = new TConfig) != NULL) {
      Cfg->TaskNumber = Task;
      if (Cfg->Load (Config, Channel) == FALSE)
         Cfg->Default ();
      if ((Events = new TEvents (Cfg->SchedulerFile)) != NULL)
         Events->Load ();
      if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL) {
         if (Cfg->MailAddress.First () == TRUE)
            Outbound->DefaultZone = Cfg->MailAddress.Zone;
      }
   }

#if defined(__LINUX__)
   if (Local == TRUE)
      Daemon = TRUE;
#endif

   if (Device != NULL)
      strcpy (Cfg->Device, Device);
   if (Speed != 0L)
      Cfg->Speed = Speed;
#if defined(__LINUX__)
   if (Cfg->SystemPath[strlen (Cfg->SystemPath) - 1] == '/')
      Cfg->SystemPath[strlen (Cfg->SystemPath) - 1] = '\0';
   chdir (Cfg->SystemPath);
   strcat (Cfg->SystemPath, "/");
#endif

   DisplayScreen ();

   if ((Log = new TPMLog) != NULL) {
      if (Daemon == TRUE)
         Log->Display = FALSE;
      if (Cfg->LogFile[0] == '\0')
         sprintf (Temp, "lora%u.log", Cfg->TaskNumber);
      else
         sprintf (Temp, Cfg->LogFile, Cfg->TaskNumber);
      Log->Open (Temp);
#if defined(__LINUX__)
      Log->Write ("+Begin, v%s (Linux)", VERSION);
#elif defined(__DOS__)
      Log->Write ("+Begin, v%s (DOS)", VERSION);
#endif
      Log->Write ("+Message-base sharing is enabled");
   }

   if (Interactive == FALSE) {
      if (Poll == TRUE)
         CanExit = FALSE;
      if (Local == FALSE) {
         EventsT = TimerSet (50L);

         EndRun = FALSE;
         while (EndRun == FALSE) {
            if (Setup == FALSE) {
               ModemTimer ();

               if (Poll == TRUE && Status == WAITFORCALL) {
                  if (CanExit == TRUE)
                     EndRun = TRUE;
                  else {
                     Modem->Terminal = FALSE;
                     strcpy (Modem->NodelistPath, Cfg->NodelistPath);
                     strcpy (Modem->DialCmd, Cfg->Dial);
                     Modem->Poll (PollNode);
                     Status = WAITFORCONNECT;
                     TimeOut = TimerSet ((ULONG)Cfg->DialTimeout * 100L);
                     CanExit = TRUE;
                  }
               }

               if (Poll == FALSE && TimeUp (EventsT) && Status == WAITFORCALL)
                  EventsTimer (NULL);
            }

            if (Daemon == FALSE) {
               if (kbmhit ()) {
                  switch (getxch ()) {
                     case 0x1b:     // ESC - Pulldown menu
                     case 0x011b:
                     case CTRLA:
                        ProcessMenu ();
                        break;
                     case 'A':
                     case 0x1E00:   // Alt-A - Auto answer
                        Modem->Serial->SendBytes ((UCHAR *)"ATA\r", 4);
                        Status = ANSWERING;
                        TimeOut = TimerSet (4500L);
                        break;
                     case 'I':
                     case 0x1700:   // Alt-I - Import mail
                        MailProcessorThread ((PVOID)(MAIL_IMPORTKNOWN|MAIL_IMPORTPROTECTED|MAIL_IMPORTNORMAL));
                        break;
                     case 'K':
                     case 0x2500:   // Alt-K - Local login
                        LocalThread (NULL);
                        break;
                     case 'M':
                     case 0x3200:   // Alt-M - Manual Poll
                        CPollDlg ();
                        break;
                     case 'P':
                     case 0x1900:   // Alt-P - Process ECHOmail
                        MailProcessorThread ((PVOID)(MAIL_IMPORTKNOWN|MAIL_IMPORTPROTECTED|MAIL_IMPORTNORMAL|MAIL_EXPORT|MAIL_PACK|MAIL_STARTTIMER));
                        break;
                     case 'Q':
                     case 0x1000:   // Alt-Q - Rescan outbound
                        if (Outbound != NULL) {
                           if (Log != NULL)
                              Log->Write ("+Building the outbound queue");
                           Outbound->BuildQueue (Cfg->Outbound);
                           unlink ("rescan.now");
                           if (Log != NULL)
                              Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);

                           RefreshOutbound ();
                        }
                        break;
                     case 'R':
                     case 0x1300:   // Alt-R - Request files
                        CRequestDlg ();
                        break;
                     case 'S':
                     case 0x1F00:   // Alt-S - Send files
                        CAttachDlg ();
                        break;
                     case 'X':
                     case 0x2D00:   // Alt-X - Uscita
                        EndRun = TRUE;
                        break;
                  }
               }
            }
         }
      }
      else
         LocalThread (NULL);
   }
   else {
      if (DoNodelist == TRUE)
         CompileNodelist (TRUE);

      Flags = MAIL_POSTQUIT|MAIL_NOEXTERNAL;
      if (DoImport == TRUE)
         Flags |= MAIL_IMPORTNORMAL|MAIL_IMPORTPROTECTED|MAIL_IMPORTKNOWN;
      if (DoExport == TRUE)
         Flags |= MAIL_EXPORT;
      if (DoPack == TRUE)
         Flags |= MAIL_PACK;
      if (DoNews == TRUE)
         Flags |= MAIL_NEWSGROUP;
      if (DoMail == TRUE)
         Flags |= MAIL_EMAIL;
      if (DoTic == TRUE)
         Flags |= MAIL_TIC;
      if (Flags != (MAIL_POSTQUIT|MAIL_NOEXTERNAL))
         MailProcessorThread ((PVOID)Flags);
   }

   if (Modem != NULL)
      delete Modem;
   if (Log != NULL) {
      Log->Write (":End");
      Log->WriteBlank ();
      delete Log;
   }
   if (Cfg != NULL)
      delete Cfg;
   if (Events != NULL)
      delete Events;
   if (Outbound != NULL)
      delete Outbound;

   ClearScreen ();
}
