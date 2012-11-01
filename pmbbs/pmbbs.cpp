
// --------------------------------------------------------------------
// LoraBBS Professional Edition - Version 0.15
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History list:
//    23/04/95 - Initial coding
// --------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"
#include "mailer.h"
#include "nodes.h"
#include "menu.h"
#include "schedule.h"
#include "version.h"

HAB hab;
HWND hwndFrame, hwndClient, hwndContainer;

#define IDW_MAIN           0x0100

// ----------------------------------------------------------------------

class TModem : public TBbs
{
public:
   TModem (USHORT usChannel);
   ~TModem (void);

   LONG   CheckResponse (VOID);
   VOID   ReleaseTimeSlice (VOID);
   VOID   Run (VOID);
   VOID   SendCommand (PSZ pszCmd);

private:
   USHORT Channel, Position;
   CHAR   Response[128];
   ULONG  Time;
   WINDOW whStatus;
#if defined(__OS2__)
   HEV    hEvent;
#endif
   struct tm *tp;
   class  TSerial *Serial;
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
   Snoop = NULL;
   Log = NULL;
   Serial = NULL;
   whStatus = 0;
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

#define STAT_INITIALIZE    1
#define STAT_INITWAITOK    2
#define STAT_WAITFORCALL   3
#define STAT_ANSWERING     4

VOID TModem::Run (VOID)
{
#if defined(__DOS__)
   USHORT Port;
#endif
   USHORT InitStr, State, PrecState;
   CHAR Temp[64];
   ULONG CallLen;
   LONG Response, TimeOut;

   Time = 0L;
   PrecState = 0;

   if (Cfg == NULL) {
      if ((Cfg = new TConfig (".\\")) != NULL)
         Cfg->Read (Channel);
   }

   if (Log == NULL) {
      if ((Log = new TLog) != NULL) {
         sprintf (Temp, "%sCH%d.LOG", Cfg->LogPath, Cfg->Channel);
         if (Log->Open (Temp) == TRUE) {
            Log->Display = FALSE;
            Log->Level = Cfg->LogLevel;
            Log->Write (Log->Begin, VERSION, Cfg->Channel);
         }
      }
   }

   if ((Status = new TStatus (".\\")) != NULL) {
      Status->Activate (Cfg->Channel);
      strcpy (Status->Status, "Initializing...");
      Status->Speed = 0L;
      Status->Update ();
   }

   if ((Serial = new TSerial) != NULL) {
#if defined(__OS2__) || defined(__NT__)
      if (Serial->Initialize (Cfg->Device, Cfg->Speed, 8, 0, 0) == TRUE) {
#else
      if (!strnicmp (Cfg->Device, "COM", 3))
         Port = (USHORT)atoi (&Cfg->Device[3]);
      else
         Port = (USHORT)atoi (Cfg->Device);

      sprintf (Temp, "%-6lu Com%d", Cfg->Speed, Port);
      prints (7, 65, YELLOW|_BLACK, Temp);

      if (Serial->Initialize (Port, Cfg->Speed, 8, 'N', 1) == TRUE) {
#endif
         Serial->SetDTR (TRUE);
         Com = Serial;

#if defined(__OS2__)
         DosCreateEventSem (NULL, &hEvent, 0L, FALSE);
#endif

         EndRun = FALSE;
         State = STAT_INITIALIZE;

         while (EndRun == FALSE) {
            if (PrecState != State) {
               if (Status != NULL) {
                  if (State == STAT_INITIALIZE) {
                     strcpy (Status->Status, "Initializing...");
                     Status->Update ();
                  }
                  else if (State == STAT_WAITFORCALL) {
                     strcpy (Status->Status, "Waiting for call");
                     Status->Update ();
                  }
                  else if (State == STAT_ANSWERING) {
                     strcpy (Status->Status, "Channel has activity");
                     Status->Update ();
                  }
               }
               PrecState = State;
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
                  else
                     State = STAT_WAITFORCALL;
                  break;

               case STAT_INITWAITOK:
                  if ((Response = CheckResponse ()) == AT_OK) {
                     InitStr++;
                     while (InitStr < 3 && Cfg->Initialize[InitStr][0] == '\0')
                        InitStr++;
                     if (InitStr < 3)
                        SendCommand (Cfg->Initialize[InitStr]);
                     else
                        State = STAT_WAITFORCALL;
                  }
                  else if (TimeUp (TimeOut)) {
                     if (Log != NULL)
                        Log->Write ("!Modem doesn't report OK");
                     SendCommand (Cfg->Initialize[InitStr]);
                  }
                  break;

               case STAT_WAITFORCALL:
                  if ((Response = CheckResponse ()) == AT_RING) {
                     SendCommand (Cfg->Answer);
                     Com->ClearInbound ();
                     TimeOut = TimerSet (4500);
                     State = STAT_ANSWERING;
                  }
                  break;

               case STAT_ANSWERING:
                  if (TimeUp (TimeOut))
                     State = STAT_INITIALIZE;

                  if ((Response = CheckResponse ()) > 0L) {
                     Hangup = FALSE;
                     Ansi = TRUE;
                     Lang = new TLanguage;
                     User = new TUser (Cfg->UserFile);
                     Limits = new TLimits (Cfg->SystemPath);

                     LastActivity = StartCall = time (NULL);
                     CarrierSpeed = Response;

                     DisplayBanner ();
                     ReadFile ((Cfg->Logo[0] == '\0') ? "LOGO" : Cfg->Logo);

                     if (Status != NULL) {
                        strcpy (Status->Status, "Login");
                        Status->Speed = Response;
                        Status->Update ();
                     }

                     do {
                        Printf (Lang->EnterName);
                        GetString (Temp, 35, INP_FIELD|INP_FANCY);
                     } while (AbortSession () == FALSE && LoginUser (Temp) == FALSE);

                     if (AbortSession () == FALSE) {
                        if (VerifyAccount () == TRUE) {
                           class TMenu *Menu = new TMenu (this);

                           if (Status != NULL) {
                              strcpy (Status->User, User->Name);
                              strcpy (Status->Status, "Browsing");
                              Status->Update ();
                           }

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

                     delete Limits;
                     Limits = NULL;
                     delete User;
                     User = NULL;
                     delete Lang;
                     Lang = NULL;

                     Pause (200);
                     State = STAT_INITIALIZE;

                     if (Status != NULL) {
                        memset (Status->User, 0, sizeof (Status->User));
                        memset (Status->Status, 0, sizeof (Status->Status));
                        Status->Update ();
                     }
                  }
                  break;
            }

            ReleaseTimeSlice ();
         }

#if defined(__OS2__)
         DosCloseEventSem (hEvent);
#endif
         if (Log != NULL)
            Log->Write (Log->End);
      }

      delete Serial;
   }

   if (Status != NULL) {
      Status->Deactivate (Cfg->Channel);
      delete Status;
   }

   if (Log != NULL) {
      Log->Write (Log->End);
      delete Log;
   }
}

VOID TModem::ReleaseTimeSlice (VOID)
{
   if (whStatus != 0) {
      if (time (NULL) != Time) {
         Time = time (NULL);
         tp = localtime ((time_t *)&Time);
         printsf (24, 71, WHITE|_BLUE, "%02d:%02d:%02d", tp->tm_hour % 24, tp->tm_min % 60, tp->tm_sec % 60);
      }
   }

#if defined(__OS2__)
   DosWaitEventSem (hEvent, 10L);
#endif
}

VOID TModem::SendCommand (PSZ pszCmd)
{
   while (*pszCmd) {
      switch (*pszCmd) {
         case '|':
            Serial->SendByte ((char)13);
            if (Snoop != NULL)
               Snoop->SendByte ((char)13);
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

#if defined(__DOS__) || defined(__OS2__)
class TTelnet : public TBbs
{
public:
   TTelnet (USHORT usChannel);
   ~TTelnet (void);

   VOID   ReleaseTimeSlice (VOID);
   VOID   Run (VOID);

private:
   USHORT Channel;
   ULONG  Time;
#if defined(__OS2__)
   HEV    hEvent;
#endif
};

TTelnet::TTelnet (USHORT usChannel) : TBbs ()
{
   Com = Snoop = NULL;
   Log = NULL;
   Channel = usChannel;
}

TTelnet::~TTelnet (void)
{
   if (Cfg != NULL)
      delete Cfg;
}

VOID TTelnet::ReleaseTimeSlice (VOID)
{
#if defined(__OS2__)
   DosWaitEventSem (hEvent, 10L);
#endif
}

VOID TTelnet::Run (VOID)
{
   CHAR Temp[64];
   ULONG CallLen;

   Time = 0L;

   if (Cfg == NULL) {
      if ((Cfg = new TConfig (".\\")) != NULL)
         Cfg->Read (Channel);
   }

   if (Log == NULL) {
      if ((Log = new TLog) != NULL) {
         sprintf (Temp, "%sCH%d.LOG", Cfg->LogPath, Cfg->Channel);
         if (Log->Open (Temp) == TRUE) {
            Log->Display = FALSE;
            Log->Level = Cfg->LogLevel;
            Log->Write (Log->Begin, VERSION, Cfg->Channel);
         }
      }
   }

   if ((Status = new TStatus (".\\")) != NULL) {
      Status->Activate (Cfg->Channel);
      strcpy (Status->Status, "Initializing...");
      Status->Speed = 0L;
      Status->Update ();
   }

#if defined(__OS2__)
   DosCreateEventSem (NULL, &hEvent, 0L, FALSE);
#endif

   if (Com != NULL) {
      Hangup = FALSE;
      Ansi = TRUE;
      Lang = new TLanguage;
      User = new TUser (Cfg->UserFile);
      Limits = new TLimits (Cfg->SystemPath);

      LastActivity = StartCall = time (NULL);
      CarrierSpeed = 57600L;

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
         if (User->Update () == FALSE)
            Log->Write ("!Unable to update user");

         Log->Write (Log->UserOffline, User->Name, User->TotalCalls, CallLen);
      }

      delete Limits;
      Limits = NULL;
      delete User;
      User = NULL;
      delete Lang;
      Lang = NULL;

      Pause (200);
      delete Com;
   }

#if defined(__OS2__)
   DosCloseEventSem (hEvent);
#endif

   if (Status != NULL) {
      Status->Deactivate ();
      delete Status;
   }

   if (Log != NULL) {
      Log->Write (Log->End);
      delete Log;
   }
}
#endif

// ----------------------------------------------------------------------

typedef struct {
   RECORDCORE RecordCore;
   PSZ    pszNumber;
   PSZ    pszUser;
   CHAR   Number[16];
   CHAR   User[64];
} TASKRECORD, * PTASKRECORD;

MRESULT EXPENTRY MainWinProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) {
      case WM_INITDLG: {
         ULONG nFields;
         CNRINFO cnrInfo;
         PFIELDINFO pFieldInfo, pTopField;
         PTASKRECORD pRecord;
         FIELDINFOINSERT fieldinsert;

         if ((hwndContainer = WinWindowFromID (hwnd, 101)) != NULLHANDLE) {
            memset (&cnrInfo, 0, sizeof (CNRINFO));
            cnrInfo.cb = sizeof (CNRINFO);
            cnrInfo.flWindowAttr = CV_DETAIL|CA_DRAWICON;
//            cnrInfo.flWindowAttr = CV_DETAIL|CA_DETAILSVIEWTITLES|CA_DRAWICON;
            cnrInfo.slBitmapOrIcon.cx = 16;
            cnrInfo.slBitmapOrIcon.cy = 16;
            WinSendMsg (hwndContainer, CM_SETCNRINFO, MPFROMP (&cnrInfo), MPFROMLONG (CMA_FLWINDOWATTR|CMA_SLBITMAPORICON));

            nFields = 2;
            pFieldInfo = (PFIELDINFO)WinSendMsg (hwndContainer, CM_ALLOCDETAILFIELDINFO, (MPARAM)nFields, (MPARAM)0);
            pTopField = pFieldInfo;

            // Inizializza i parametri del primo campo (icona).
            pFieldInfo->cb = sizeof (FIELDINFO);
            pFieldInfo->pTitleData = "";
            pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR|CFA_HORZSEPARATOR;
            pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
            pFieldInfo->offStruct = (ULONG)&pRecord->pszNumber - (ULONG)pRecord;
            pFieldInfo->pUserData = NULL;
            pFieldInfo->cxWidth = 0L;

            pFieldInfo = pFieldInfo->pNextFieldInfo;
            pFieldInfo->cb = sizeof (FIELDINFO);
            pFieldInfo->pTitleData = "";
            pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_LEFT|CFA_SEPARATOR|CFA_HORZSEPARATOR;
            pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
            pFieldInfo->offStruct = (ULONG)&pRecord->pszUser - (ULONG)pRecord;
            pFieldInfo->pUserData = NULL;
            pFieldInfo->cxWidth = 0L;

            // Inserisce i descrittori delle colonne nel container.
            fieldinsert.cb = sizeof (FIELDINFOINSERT);
            fieldinsert.pFieldInfoOrder = (PFIELDINFO)CMA_FIRST;
            fieldinsert.fInvalidateFieldInfo = TRUE;
            fieldinsert.cFieldInfoInsert = nFields;
            WinSendMsg (hwndContainer, CM_INSERTDETAILFIELDINFO, MPFROMP (pTopField), MPFROMP (&fieldinsert));
         }
         break;
      }

      case WM_COMMAND:
         break;

      case WM_TIMER: {
         PTASKRECORD pRecord;
         RECORDINSERT recordInsert;
         class TStatus *Status;

         if ((Status = new TStatus (".\\")) != NULL) {
            WinSendMsg (hwndContainer, CM_REMOVERECORD, 0L, MPFROM2SHORT (0, CMA_FREE));
            if (Status->First () == TRUE) {
               do {
                  if ((pRecord = (PTASKRECORD)WinSendMsg (hwndContainer, CM_ALLOCRECORD, (MPARAM)(sizeof (TASKRECORD) - sizeof (RECORDCORE)), (MPARAM)1)) != NULL) {
                     pRecord->RecordCore.hptrIcon = NULLHANDLE;
                     pRecord->RecordCore.pszIcon = pRecord->Number;
                     pRecord->pszNumber = pRecord->Number;
                     pRecord->pszUser = pRecord->User;

                     sprintf (pRecord->Number, "%u", Status->Task);
                     if (Status->User[0] != '\0')
                        sprintf (pRecord->User, "%s (%s)", Status->User, Status->Location);
                     else
                        strcpy (pRecord->User, Status->Status);

                     recordInsert.cb = sizeof (RECORDINSERT);
                     recordInsert.pRecordOrder = (PRECORDCORE)CMA_END;
                     recordInsert.zOrder = (ULONG)CMA_TOP;
                     recordInsert.cRecordsInsert = 1;
                     recordInsert.fInvalidateRecord = FALSE;
                     recordInsert.pRecordParent = NULL;
                     WinSendMsg (hwndContainer, CM_INSERTRECORD, MPFROMP (pRecord), MPFROMP (&recordInsert));
                  }
               } while (Status->Next () == TRUE);
               WinSendMsg (hwndContainer, CM_INVALIDATEDETAILFIELDINFO, 0L, 0L);
            }
            delete Status;
         }
         break;
      }

      default:
         return (WinDefDlgProc (hwnd, msg, mp1, mp2));
   }

   return ((MRESULT)FALSE);
}

void ModemThread (void *arg)
{
   class TBbs *Modem;

   if ((Modem = (class TBbs *)arg) != NULL) {
      Modem->Run ();
      delete Modem;
   }

   _endthread ();
}

void TelnetServer (void *arg)
{
   USHORT Channel, Port, Socket;
#if defined(__OS2__)
   HEV hEvent;
#endif
   class TConfig *Cfg;
   class TTcpip *Server;
   class TTcpip *Client;
   class TTelnet *Modem;

   Channel = (*(USHORT *)arg);
   if ((Cfg = new TConfig (".\\")) != NULL) {
      Cfg->Read (Channel);
      Port = Cfg->Port;
      delete Cfg;
   }

   if ((Server = new TTcpip) != NULL) {
#if defined(__OS2__)
      DosCreateEventSem (NULL, &hEvent, 0L, FALSE);
#endif

      if (Server->Initialize (Port) == TRUE) {
         for (;;) {
            if ((Socket = Server->WaitClient ()) != 0) {
               if ((Client = new TTcpip) != NULL) {
                  Client->Initialize (0, Socket);
                  if ((Modem = new TTelnet (Channel)) != NULL) {
                     Modem->Com = Client;
                     _beginthread (ModemThread, NULL, 8192, (void *)Modem);
                  }
                  else
                     delete Client;
               }
            }
#if defined(__OS2__)
            DosWaitEventSem (hEvent, 10L);
#endif
         }
      }
      delete Server;

#if defined(__OS2__)
      DosCloseEventSem (hEvent);
#endif
   }
}

void main (void)
{
   HMQ hmq;
   class TConfig *Cfg;
   class TModem *Modem;

   _grow_handles (200);
   unlink ("status.dat");

   if ((hab = WinInitialize (0)) != 0L) {
      if ((hmq = WinCreateMsgQueue (hab, 0)) != 0L) {
         if ((hwndFrame = WinLoadDlg (HWND_DESKTOP, HWND_DESKTOP, (PFNWP)MainWinProc, (HMODULE)0, 256, NULL)) != NULLHANDLE) {
            WinStartTimer (hab, hwndFrame, 1, 2000L);

            if ((Cfg = new TConfig (".\\")) != NULL) {
               if (Cfg->ReadFirstChannel () == TRUE)
                  do {
                     switch (Cfg->Type) {
                        case CH_MODEM:
                           if ((Modem = new TModem (Cfg->Channel)) != NULL)
                              _beginthread (ModemThread, NULL, 8192, (void *)Modem);
                           break;
                     }
                  } while (Cfg->ReadNextChannel () == TRUE);
               delete Cfg;
            }

            WinProcessDlg (hwndFrame);

            WinStopTimer (hab, hwndFrame, 1);
            WinDismissDlg (hwndFrame, 0L);
         }

         WinDestroyMsgQueue (hmq);
      }

      WinTerminate (hab);
   }
}



