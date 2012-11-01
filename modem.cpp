
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.16
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include <bios.h>
#include "combase.h"
#include "mailer.h"
#include "nodes.h"
#include "menu.h"
#include "schedule.h"
#include "version.h"

class TModemStatus
{
public:
   TModemStatus (void);
   ~TModemStatus (void);

   USHORT Oh;
   class  TCom *Com;

   VOID   Idle (VOID);

private:
   short  whStatus;
};

TModemStatus::TModemStatus (void)
{
   short wh;
   CHAR Temp[128];

   Oh = FALSE;
   wh = whandle ();

   if ((whStatus = wopen (24, 0, 24, 79, 5, LGREY|_BLUE, LGREY|_BLUE)) != 0) {
      sprintf (Temp, " %-8s ³ %-6s ³ RI CD OH RD SD TR MR RS CS ³", "", "");
      wprints (0, 0, WHITE|_BLUE, Temp);
   }

   if (wh != 0)
      wactiv (wh);
}

TModemStatus::~TModemStatus (void)
{
   short wh;

   if (whStatus != 0) {
      wh = whandle ();
      wactiv (whStatus);
      wclose ();
      if (whStatus != wh)
         wactiv (wh);
   }
}

VOID TModemStatus::Idle (VOID)
{
   CHAR Temp[32];

   if (whStatus != 0) {
      sprintf (Temp, " %-8s ³ %-6s", Com->Device, Com->Speed);
      prints (24, 0, WHITE|_BLUE, Temp);

      prints (24, 21, (Com->Ri == TRUE) ? WHITE|_BLUE : BLACK|_BLUE, "RI");
      prints (24, 24, (Com->Dcd == TRUE) ? WHITE|_BLUE : BLACK|_BLUE, "CD");
      prints (24, 27, (Oh == TRUE) ? WHITE|_BLUE : BLACK|_BLUE, "OH");
      prints (24, 30, (Com->Rxd == TRUE) ? WHITE|_BLUE : BLACK|_BLUE, "RD");
      prints (24, 33, (Com->Txd == TRUE) ? WHITE|_BLUE : BLACK|_BLUE, "SD");
      prints (24, 36, (Com->Dtr == TRUE) ? WHITE|_BLUE : BLACK|_BLUE, "TR");
      prints (24, 39, (Com->Dsr == TRUE) ? WHITE|_BLUE : BLACK|_BLUE, "MR");
      prints (24, 42, (Com->Rts == TRUE) ? WHITE|_BLUE : BLACK|_BLUE, "RS");
      prints (24, 45, (Com->Cts == TRUE) ? WHITE|_BLUE : BLACK|_BLUE, "CS");
   }
}

// ----------------------------------------------------------------------

class TModemSystem
{
public:
   TModemSystem (void);
   ~TModemSystem (void);

   VOID   Idle (VOID);
   VOID   Mode (PSZ pszMode);

private:
   short   whStatus;
   ULONG   Change, LastTime;
};

TModemSystem::TModemSystem (void)
{
   if ((whStatus = wopen (1, 46, 12, 79, 1, LGREEN|_BLACK, LGREY|_BLACK)) != 0) {
      prints (1, 48, LGREEN|_BLACK, "SYSTEM");
      wrjusts (3, 15, LCYAN|_BLACK, "System mode:");
      wrjusts (4, 15, LCYAN|_BLACK, "Elapsed time:");
      wrjusts (5, 15, LCYAN|_BLACK, "Next action:");
      wrjusts (6, 15, LCYAN|_BLACK, "Event tag:");
      wrjusts (7, 15, LCYAN|_BLACK, "Scheduled for:");
      wrjusts (8, 15, LCYAN|_BLACK, "Time remaining:");

      Mode ("Initializing");
   }

   Change = time (NULL);
   LastTime = 0L;
}

TModemSystem::~TModemSystem (void)
{
   short wh;

   if (whStatus != 0) {
      wh = whandle ();
      wactiv (whStatus);
      wclose ();
      if (whStatus != wh)
         wactiv (wh);
   }
}

PSZ Months[] = {
   "January", "February", "March", "April", "May", "June",
   "July", "August", "September", "October", "November", "December"
};

PSZ WeekDays[] = {
   "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

VOID TModemSystem::Idle (VOID)
{
   short wh;
   CHAR Temp[48];
   ULONG t;
   struct tm *tp;

   if (whStatus != 0) {
      wh = whandle ();
      wactiv (whStatus);

      t = time (NULL);
      if (LastTime != t) {
         tp = localtime ((time_t *)&t);
         sprintf (Temp, "%s %d %s %d", WeekDays[tp->tm_wday], tp->tm_mday, Months[tp->tm_mon], tp->tm_year + 1900);
         wprints (0, (short)(16 - strlen (Temp) / 2), YELLOW|_BLACK, Temp);
         sprintf (Temp, "%02d:%02d:%02d", tp->tm_hour % 24, tp->tm_min % 60, tp->tm_sec % 60);
         wprints (1, (short)(16 - strlen (Temp) / 2), YELLOW|_BLACK, Temp);
         LastTime = t;

         t = LastTime - Change;
         if (t < 3600L)
            sprintf (Temp, "%02ld:%02ld", t / 60L, t % 60L);
         else
            sprintf (Temp, "%ld:%02ld:%02ld", t / 3600L, (t % 3600L) / 60L, ((t % 3600L) % 60L) % 60L);
         wprints (4, 17, YELLOW|_BLACK, Temp);
      }

      wactiv (wh);
   }
}

VOID TModemSystem::Mode (PSZ pszMode)
{
   short wh;
   CHAR Temp[24];

   if (whStatus != 0) {
      wh = whandle ();
      wactiv (whStatus);

      sprintf (Temp, "%-16.16s", pszMode);
      wprints (3, 17, YELLOW|_BLACK, Temp);
      Change = time (NULL);

      wactiv (wh);
   }
}

// ----------------------------------------------------------------------

class TModem : public TBbs
{
public:
   TModem (USHORT usChannel);
   ~TModem (void);

   LONG   CheckResponse (VOID);
   VOID   Pause (LONG lHund);
   VOID   ReleaseTimeSlice (VOID);
   VOID   Run (VOID);
   VOID   RunBBS (USHORT isLocal);
   VOID   SendCommand (PSZ pszCmd);

private:
   USHORT Channel, Position;
   CHAR   Response[128];
   ULONG  Time;
#if defined(__OS2__)
   HEV    hEvent;
#endif
   struct tm *tp;
   class  TModemSystem *System;
   class  TModemStatus *MdmStat;
   class  TSerial *Serial;
   class  TScreen *Screen;
};

#define AT_NO_RESPONSE   0L
#define AT_BUSY         -1L
#define AT_ERROR        -2L
#define AT_NO_ANSWER    -3L
#define AT_NO_CARRIER   -4L
#define AT_NO_DIALTONE  -5L
#define AT_OK           -6L
#define AT_RING         -7L
#define AT_RINGING      -8L
#define AT_VOICE        -9L

#define AT_FHNG         -10
#define AT_FCON         -11
#define AT_FPOLL        -12
#define AT_FVO          -13
#define AT_FDIS         -14
#define AT_FNSF         -15
#define AT_FCSI         -16
#define AT_FPTS         -17
#define AT_FDCS         -18
#define AT_FNSS         -19
#define AT_FTSI         -20
#define AT_FET          -21

TModem::TModem (USHORT usChannel) : TBbs ()
{
   Log = NULL;
   Serial = NULL;
   Screen = NULL;
   Position = 0;
   Channel = usChannel;
}

TModem::~TModem (void)
{
   if (Cfg != NULL)
      delete Cfg;
}

LONG TModem::CheckResponse (VOID)
{
   USHORT doCheck = FALSE;
   CHAR c, *p;
   LONG RetVal = AT_NO_RESPONSE;

//   if (MdmStat != NULL) {
//      Com->Carrier ();
//      MdmStat->Idle ();
//   }
//   if (System != NULL)
//      System->Idle ();

   while (doCheck == FALSE && Com->BytesReady () == TRUE) {
      c = (CHAR)Com->ReadByte ();
      if (Snoop != NULL)
         Snoop->SendByte (c);

      if (c == '\r') {
         Response[Position] = '\0';
         doCheck = TRUE;
         Position = 0;
      }
      else if (c != '\n' && Position < sizeof (Response)) {
         Response[Position++] = c;
         if (Position == sizeof (Response))
            Response[Position - 1] = '\0';
      }
   }

   if (doCheck == TRUE) {
      if (Response[0] == '+' && Response[1] == 'F') {
         if (!strncmp (Response, "+FHS:", 5))
            RetVal = AT_FHNG;
         if (!strncmp (Response, "+FCO:", 5))
            RetVal = AT_FCON;
         if (!strncmp (Response, "+FPO", 4))
            RetVal = AT_FPOLL;
         if (!strncmp (Response, "+FVO", 4))
            RetVal = AT_FVO;
         if (!strncmp (Response, "+FIS:", 5))
            RetVal = AT_FDIS;
         if (!strncmp (Response, "+FNF:", 5))
            RetVal = AT_FNSF;
         if (!strncmp (Response, "+FCI:", 5))
            RetVal = AT_FCSI;
         if (!strncmp (Response, "+FPS:", 5))
            RetVal = AT_FPTS;
         if (!strncmp (Response, "+FCS:", 5))
            RetVal = AT_FDCS;
         if (!strncmp (Response, "+FNS:", 5))
            RetVal = AT_FNSS;
         if (!strncmp (Response, "+FTI:", 5))
            RetVal = AT_FTSI;
         if (!strncmp (Response, "+FET:", 5))
            RetVal = AT_FET;
      }
      else {
         if (!stricmp (Response, "OK"))
            RetVal = AT_OK;
         if (!stricmp (Response, "BUSY"))
            RetVal = AT_BUSY;
         if (!stricmp (Response, "ERROR"))
            RetVal = AT_ERROR;
         if (!stricmp (Response, "NO ANSWER"))
            RetVal = AT_NO_ANSWER;
         if (!stricmp (Response, "NO CARRIER"))
            RetVal = AT_NO_CARRIER;
         if (!stricmp (Response, "NO DIALTONE"))
            RetVal = AT_NO_DIALTONE;
         if (!stricmp (Response, "RING"))
            RetVal = AT_RING;
         if (!stricmp (Response, "RINGING"))
            RetVal = AT_RINGING;
         if (!stricmp (Response, "VOICE"))
            RetVal = AT_VOICE;
         if (!stricmp (Response, "CONNECT FAX"))
            RetVal = AT_FCON;

         if (!strnicmp (Response, "CONNECT", 7) || !strnicmp (Response, "CARRIER", 7)) {
            if (Response[7] == ' ')
               RetVal = atol (&Response[8]);
            else
               RetVal = 300L;
         }
      }
   }

   if (Log != NULL && RetVal != AT_NO_RESPONSE && RetVal != AT_OK) {
      strlwr (Response);
      Response[0] = (CHAR)toupper (Response[0]);
      p = Response;
      while ((p = strchr (p, '/')) != NULL) {
         p++;
         *p = (CHAR)toupper (*p);
      }
      p = Response;
      while ((p = strchr (p, ' ')) != NULL) {
         p++;
         *p = (CHAR)toupper (*p);
      }
      Log->Write (Log->ModemResponse, Response);
   }

   return (RetVal);
}

VOID TModem::RunBBS (USHORT isLocal)
{
   CHAR Temp[64];
   ULONG CallLen;

   Hangup = FALSE;
   Ansi = TRUE;
   Lang = new TLanguage;
   User = new TUser (Cfg->UserFile);
   Limits = new TLimits (Cfg->SystemPath);

   if (System != NULL)
      System->Mode ("Connected");

   LastActivity = StartCall = time (NULL);
   if ((Screen = new TScreen) != NULL) {
      if (Screen->Initialize () == TRUE) {
         Log->Display = FALSE;
         if (isLocal == TRUE)
            Com = Screen;
         else
            Snoop = Screen;
      }
   }

   DisplayBanner ();
   ReadFile ((Cfg->Logo[0] == '\0') ? "LOGO" : Cfg->Logo);

   do {
      Printf (Lang->EnterName);
      GetString (Temp, 35, INP_FIELD|INP_FANCY);
   } while (AbortSession () == FALSE && LoginUser (Temp) == FALSE);

   if (AbortSession () == FALSE) {
      if (VerifyAccount () == TRUE) {
         class TMenu *Menu = new TMenu (this);

         if (Menu != NULL) {
            Menu->Run ((Cfg->FirstMenu[0] == '\0') ? "TOP" : Cfg->FirstMenu);
            delete Menu;
         }
      }

      CallLen = (time (NULL) - StartCall) / 60L;

      User->LastCall = time (NULL);
      User->TodayTime += CallLen;
      User->WeekTime += CallLen;
      User->MonthTime += CallLen;
      User->YearTime += CallLen;
      if (User->Update () == FALSE && Log != NULL)
         Log->Write ("!Unable to update user");

      if (Log != NULL)
         Log->Write (Log->UserOffline, User->Name, User->TotalCalls, CallLen);
   }

   if (Screen != NULL) {
      delete Screen;
      Screen = NULL;
      Snoop = NULL;
      Com = Serial;
      Log->Display = TRUE;
      Log->Update ();
   }

   delete Limits;
   Limits = NULL;
   delete User;
   User = NULL;
   delete Lang;
   Lang = NULL;

   Pause (200);
}

VOID TModem::Pause (LONG lHund)
{
   LONG Endtime;

   Endtime = TimerSet (lHund);
   while (!TimeUp (Endtime))
      ReleaseTimeSlice ();
}

#define STAT_INITIALIZE    1
#define STAT_INITWAITOK    2
#define STAT_WAITFORCALL   3
#define STAT_ANSWERING     4

VOID TModem::Run (VOID)
{
#if defined(__DOS__)
   USHORT Port;
#endif
   USHORT InitStr, State;
   CHAR Temp[64];
   LONG Response, TimeOut;

   Time = 0L;
   hidecur ();
   clrscrn ();

   if (Cfg == NULL) {
      if ((Cfg = new TConfig (".\\")) != NULL)
         Cfg->Read (Channel);
   }

   if ((System = new TModemSystem) != NULL)
      System->Idle ();

   if (wopen (13, 0, 23, 79, 1, LCYAN|_BLACK, LGREY|_BLACK) != 0) {
      prints (13, 2, LCYAN|_BLACK, "OUTBOUND");
      wprints (0, 1, YELLOW|_BLACK, "Destination        Try/Con/Bad/Online  Priorities/Pkt types  Status");
   }

   if ((Log = new TLog) != NULL) {
      sprintf (Temp, "%sCH%d.LOG", Cfg->LogPath, Cfg->Channel);
      if (Log->Open (Temp) == TRUE) {
//         Log->Display = FALSE;
      	Log->Level = Cfg->LogLevel;
      	Log->Write (Log->Begin, VERSION, Cfg->Channel);
      }
   }

#if defined(__DOS__)
   sprintf (Temp, "%s/No key", STD_COMPLETE);
#else
   sprintf (Temp, "%s/No key", PRO_COMPLETE);
#endif
   prints (0, (short)(79 - strlen (Temp)), WHITE|_BLACK, Temp);

   Log->Write ("+Message-base sharing is enabled");

   Log->Write ("!WARNING: No license key found");
   Log->Write ("!The software WILL NOT function");
   Log->Write ("!completely without a license key!");

   if ((Serial = new TSerial) != NULL) {
#if defined(__OS2__) || defined(__NT__)
      if (Serial->Initialize (Cfg->Device, Cfg->Speed, 8, 0, 0) == TRUE) {
#else
      if (!strnicmp (Cfg->Device, "COM", 3))
         Port = (USHORT)atoi (&Cfg->Device[3]);
      else
         Port = (USHORT)atoi (Cfg->Device);

      if (Serial->Initialize (Port, Cfg->Speed, 8, 'N', 1) == TRUE) {
#endif
         Serial->SetDTR (TRUE);
         Serial->SetRTS (TRUE);

         Com = Serial;
         strcpy (Com->Device, Cfg->Device);
         sprintf (Com->Speed, "%lu", Cfg->Speed);

         if ((MdmStat = new TModemStatus) != NULL) {
            MdmStat->Com = Com;
            Com->Carrier ();
            MdmStat->Idle ();
         }

#if defined(__OS2__)
         DosCreateEventSem (NULL, &hEvent, 0L, FALSE);
#endif

         EndRun = FALSE;
         State = STAT_INITIALIZE;

         while (EndRun == FALSE) {
            if (_bios_keybrd (_KEYBRD_READY)) {
               switch (_bios_keybrd (_KEYBRD_READ)) {
                  case 0x2500:      // Alt-K = Local connection
                     CarrierSpeed = Cfg->Speed;
                     Log->Write (Log->ModemResponse, "Connect Local");
                     RunBBS (TRUE);
                     State = STAT_INITIALIZE;
                     if (System != NULL)
                        System->Mode ("Initializing");
                     break;

                  case 0x2D00:      // Alt-X = Exit
                     EndRun = TRUE;
                     if (System != NULL)
                        System->Mode ("Shutdown");
                     break;
               }
            }

            switch (State) {
               case STAT_INITIALIZE:
                  InitStr = 0;
                  while (InitStr < 3 && Cfg->Initialize[InitStr][0] == '\0')
                     InitStr++;

                  if (InitStr < 3) {
                     SendCommand (Cfg->Initialize[InitStr]);
                     TimeOut = TimerSet (1500);
                     State = STAT_INITWAITOK;
                  }
                  else {
                     State = STAT_WAITFORCALL;
                     if (System != NULL)
                        System->Mode ("Idle");
                  }
                  break;

               case STAT_INITWAITOK:
                  if ((Response = CheckResponse ()) == AT_OK) {
                     InitStr++;
                     while (InitStr < 3 && Cfg->Initialize[InitStr][0] == '\0')
                        InitStr++;
                     if (InitStr < 3)
                        SendCommand (Cfg->Initialize[InitStr]);
                     else {
                        State = STAT_WAITFORCALL;
                        if (System != NULL)
                           System->Mode ("Idle");
                     }
                  }
                  else if (TimeUp (TimeOut)) {
                     if (Log != NULL)
                        Log->Write ("!ERROR: Unable to inizialize modem");
                     SendCommand (Cfg->Initialize[InitStr]);
                  }
                  break;

               case STAT_WAITFORCALL:
                  if ((Response = CheckResponse ()) == AT_RING) {
                     SendCommand (Cfg->Answer);
                     Com->ClearInbound ();
                     TimeOut = TimerSet (4500);
                     State = STAT_ANSWERING;
                     if (System != NULL)
                        System->Mode ("Answering");
                  }
                  break;

               case STAT_ANSWERING:
                  if (TimeUp (TimeOut))
                     State = STAT_INITIALIZE;

                  if ((Response = CheckResponse ()) > 0L) {
                     CarrierSpeed = Response;
                     RunBBS (FALSE);
                     State = STAT_INITIALIZE;
                     if (System != NULL)
                        System->Mode ("Initializing");
                  }
                  break;
            }

            ReleaseTimeSlice ();
         }

         if (Log != NULL)
            Log->Write (Log->End);

         Pause (200);
#if defined(__OS2__)
         DosCloseEventSem (hEvent);
#endif
      }

      delete Serial;
   }

   if (System != NULL)
      delete System;

   if (MdmStat != NULL)
      delete MdmStat;

   if (Log != NULL) {
      Log->Write (Log->End);
      delete Log;
   }

   showcur ();
}

VOID TModem::ReleaseTimeSlice (VOID)
{
#if defined(__OS2__)
   DosWaitEventSem (hEvent, 10L);
#endif
   if (MdmStat != NULL) {
      Com->Carrier ();
      MdmStat->Idle ();
   }
   if (System != NULL && Screen == NULL)
      System->Idle ();
}

VOID TModem::SendCommand (PSZ pszCmd)
{
   while (*pszCmd) {
      switch (*pszCmd) {
         case '|':
            Serial->SendByte ((char)13);
            if (Snoop != NULL)
               Snoop->SendByte ((char)13);
//            if (MdmStat != NULL) {
//               Com->Carrier ();
//               MdmStat->Idle ();
//            }
//            if (System != NULL)
//               System->Idle ();
            Pause (10);
            break;

         case 'v':
            Serial->SetDTR (FALSE);
            Pause (10);
            break;

         case '^':
            Serial->SetDTR (TRUE);
            Pause (10);
            break;

         case '~':
            Pause (50);
            break;

         case '`':
            Pause (10);
            break;

         default:
            Serial->SendByte (*pszCmd);
            if (Snoop != NULL)
               Snoop->SendByte (*pszCmd);
            break;
      }
      pszCmd++;
   }

   Serial->SendByte ((char)13);
   if (Snoop != NULL)
      Snoop->SendByte ((char)13);

   Pause (10);
}

// ----------------------------------------------------------------------

void main (int argc, char *argv[])
{
   USHORT i, Quit, DoLocal, Port, Task;
   PSZ LogFile, CfgPath;
   ULONG Speed;
   class TConfig *Cfg;
   class TModem *Modem;

   Quit = DoLocal = FALSE;
   LogFile = NULL;
   CfgPath = "";
   Port = 0;
   Speed = 0L;
   Task = 0xFFFFU;

   for (i = 1; i < argc; i++) {
      if (!stricmp (argv[i], "?") || !stricmp (argv[i], "-?") || !strnicmp (argv[i], "-h", 2)) {
#if defined(__OS2__)
         cprintf ("\r\nLoraBBS Standard Edition for OS/2 - Version %s\r\n", VERSION);
#elif defined(__NT__)
         cprintf ("\r\nLoraBBS Standard Edition for Windows/NT - Version %s\r\n", VERSION);
#else
         cprintf ("\r\nLoraBBS Standard Edition for DOS - Version %s\r\n", VERSION);
#endif
         cprintf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\r\n\r\n");
         cprintf ("Usage:\r\n        BBS [switches] [TERM|IMPORT|EXPORT|PACK]\r\n\r\n");
         cprintf ("Where [switches] are:\r\n\r\n");
         cprintf ("   -c<path>  = Sets the path where to find the configuration files\r\n");
         cprintf ("   -l        = Run in local connection mode\r\n");
         cprintf ("   -n<num>   = Sets the task number\r\n");
         cprintf ("   -p<port>  = Overrides the COM port number\r\n");
         cprintf ("   -r<file>  = Sets the name of the log file\r\n");
         cprintf ("   -s<speed> = Overrides the DTE<>DCE speed\r\n");
         Quit = TRUE;
      }
      else if (!strnicmp (argv[i], "-c", 2))
         CfgPath = &argv[i][2];
      else if (!stricmp (argv[i], "-l")) {
         DoLocal = TRUE;
         if (Task == 0xFFFFU)
            Task = 0;
      }
      else if (!strnicmp (argv[i], "-n", 2))
         Task = (USHORT)atoi (&argv[i][2]);
      else if (!strnicmp (argv[i], "-p", 2))
         Port = (USHORT)atoi (&argv[i][2]);
      else if (!strnicmp (argv[i], "-r", 2))
         LogFile = &argv[i][2];
      else if (!strnicmp (argv[i], "-s", 2))
         Speed = atol (&argv[i][2]);
   }

   if (Task == 0xFFFFU)
      Task = 1;

   if (Quit == FALSE) {
      if ((Cfg = new TConfig (CfgPath)) != NULL) {
         if (Cfg->Read (Task) == TRUE) {
            if (Port != 0)
               sprintf (Cfg->Device, "COM%d", Port);
            if (Speed != 0L)
               Cfg->Speed = Speed;

           if ((Modem = new TModem (Task)) != NULL) {
               Modem->Cfg = Cfg;
               Modem->Run ();
               delete Modem;
            }

            wcloseall ();
            cclrscrn (LGREY|_BLACK);
         }
      }
   }
}


