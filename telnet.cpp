
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.16
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"
#include "mailer.h"
#include "menu.h"
#include "version.h"

#define MAX_CHANNELS       16
#define SYSTEM_TERMQ       "\\QUEUES\\TELTERM.QUE"

class TTelnet : public TBbs
{
public:
   TTelnet (USHORT usCfg, USHORT usChannel, USHORT usSocket);
   ~TTelnet (void);

   LONG   CheckResponse (VOID);
   VOID   ReleaseTimeSlice (VOID);
   VOID   Run (VOID);
   VOID   SendCommand (PSZ cmd);

private:
   USHORT Socket;
#if defined(__OS2__)
   HEV    hEvent;
#endif
   class  TTcpip *Tcpip;
   class  TScreen *Screen;
};

TTelnet::TTelnet (USHORT usCfg, USHORT usChannel, USHORT usSocket) : TBbs ()
{
   User = NULL;
   if ((Cfg = new TConfig (".\\")) != NULL) {
      Cfg->Read (usCfg);
      Cfg->Channel = usChannel;
   }
   Socket = usSocket;
}

TTelnet::~TTelnet (void)
{
   if (Cfg != NULL)
      delete Cfg;
}

VOID TTelnet::Run (VOID)
{
   CHAR temp[64];
   ULONG CallLen;
   class TDetect *Detect;
   class TEMail *Mail;

   if ((Log = new TLog) != NULL) {
      sprintf (temp, "%sCH%d.LOG", Cfg->LogPath, Cfg->Channel);
      if (Log->Open (temp) == TRUE) {
         Log->Level = Cfg->LogLevel;
         Log->Write (Log->Begin, VERSION, Cfg->Channel);

         if ((Tcpip = new TTcpip) != NULL) {
            if (Tcpip->Initialize (0, Socket) == TRUE) {
               Com = Tcpip;

               if ((Screen = new TScreen) != NULL) {
                  if (Screen->Initialize () == TRUE)
                     Snoop = Screen;
               }

#if defined(__OS2__)
               DosCreateEventSem (NULL, &hEvent, 0L, FALSE);
#endif

               Hangup = FALSE;
               Ansi = TRUE;
               Lang = new TLanguage;
               User = new TUser (Cfg->UserFile);
               Limits = new TLimits (Cfg->SystemPath);

               LastActivity = StartCall = time (NULL);
               CarrierSpeed = 57600L;

               Tcpip->SendBytes ((UCHAR *)"\xFF\xFD\x01\xFF\xFD\x00\xFF\xFB\x01\xFF\xFB\x00", 12);
               DisplayBanner ();

               if ((Detect = new TDetect (this)) != NULL) {
                  Detect->SelectLanguage ();
                  delete Detect;
               }

               if (LoginUser () == TRUE) {
                  if (VerifyAccount () == TRUE) {
                     if ((Mail = new TEMail (this)) != NULL) {
                        Mail->Check ();
                        delete Mail;
                     }
                     class TMenu *Menu = new TMenu (this);
                     Menu->Run ((Cfg->FirstMenu[0] == '\0') ? "TOP" : Cfg->FirstMenu);
                     delete Menu;
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
                  Log->WriteBlank ();
               }

               delete Limits;
               Limits = NULL;
               delete User;
               User = NULL;
               delete Lang;
               Lang = NULL;

               Pause (200);
               Com->SendBytes ((UCHAR *)"\r\n\x1B[0;37m", 9);

#if defined(__OS2__)
               DosCloseEventSem (hEvent);
#endif
            }

            delete Tcpip;
            if (Snoop != NULL)
               delete Screen;
         }

         Log->Write (Log->End);
         Log->Close ();
      }

      delete Log;
   }
}

VOID TTelnet::ReleaseTimeSlice (VOID)
{
#if defined(__OS2__)
   DosWaitEventSem (hEvent, 10L);
#endif
}

// ----------------------------------------------------------------------
// Entry point per il server telnet.
//
//    argv[1] = Numero del canale per la configurazione.
//    argv[2] = Socket di comunicazione (0 == Modo server).
//    argv[3] = Porta TCP/IP da utilizzare (23 == Telnet).
//    argv[4] = Numero del canale per log e visualizzazione.
// ----------------------------------------------------------------------

typedef struct {
   ULONG SessionID;
   ULONG ResultCode;
} TERMNOTIFY, *PTERMNOTIFY;

ULONG idChannels[MAX_CHANNELS];
#if defined(__OS2__)
HQUEUE hSysTermQ = NULLHANDLE;
#endif

void main (int argc, char *argv[])
{
#if defined(__OS2__)
   USHORT i, Socket;
   CHAR ObjBuf[128];
   BYTE Priority;
   ULONG Size;
   PTERMNOTIFY pNotify;
   REQUESTDATA Request;
   HEV hEvent, hTermQEvent;
   class TTcpip *Tcp;
   class TTelnet *Telnet;

   printf ("\nLoraBBS Professional Edition for OS/2 PM. Version %s\n", VERSION);
   printf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\n\n");

   if (argc > 3) {
      if (atoi (argv[2]) == 0) {
         if ((Tcp = new TTcpip) != NULL) {
            if (Tcp->Initialize ((USHORT)atoi (argv[3])) == TRUE) {
               DosCreateEventSem (NULL, &hEvent, 0L, FALSE);
               if (DosCreateQueue (&hSysTermQ, QUE_FIFO|QUE_CONVERT_ADDRESS, SYSTEM_TERMQ) == 0)
                  DosCreateEventSem (NULL, &hTermQEvent, 0L, FALSE);

               for (i = 0; i < MAX_CHANNELS; i++)
                  idChannels[i] = 0L;

               printf ("Server ready on port %d\n\n", atoi (argv[3]));

               for (;;) {
                  if (DosReadQueue (hSysTermQ, &Request, &Size, (PPVOID)&pNotify, 0L, DCWW_NOWAIT, &Priority, hTermQEvent) == 0) {
                     printf ("Received termination from id %d", (pNotify->SessionID & 0xFFFFL));
                     for (i = 0; i < MAX_CHANNELS; i++) {
                        if (idChannels[i] == (pNotify->SessionID & 0xFFFFL)) {
                           idChannels[i] = 0L;
                           printf (", channel %d\n", i + atoi (argv[1]));
                        }
                     }
                  }
                  if ((Socket = Tcp->WaitClient ()) != 0) {
                     STARTDATA StartData;
                     PID Pid;

                     if (!stricmp (Tcp->ClientName, Tcp->ClientIP))
                        printf ("Incoming call from %s\n", Tcp->ClientIP);
                     else
                        printf ("Incoming call from %s (%s)\n", Tcp->ClientName, Tcp->ClientIP);

                     for (i = 0; i < MAX_CHANNELS; i++) {
                        if (idChannels[i] == 0L)
                           break;
                     }

                     if (i < MAX_CHANNELS) {
                        StartData.Length = sizeof (STARTDATA);
                        StartData.Related = SSF_RELATED_CHILD;
                        StartData.FgBg = SSF_FGBG_BACK;
                        StartData.TraceOpt = SSF_TRACEOPT_NONE;
                        if ((StartData.PgmTitle = (PSZ)malloc (32)) != NULL) {
                           sprintf (StartData.PgmTitle, "Channel #%d", i + atoi (argv[1]));
                           StartData.PgmName = "telnet.exe";
                           if ((StartData.PgmInputs = (PSZ)malloc (32)) != NULL)
                              sprintf (StartData.PgmInputs, "%s %d %s %d", argv[1], Socket, argv[3], i + atoi (argv[1]));
                           StartData.TermQ = SYSTEM_TERMQ;
                           StartData.Environment = 0;
                           StartData.InheritOpt = SSF_INHERTOPT_PARENT;
                           StartData.SessionType = SSF_TYPE_FULLSCREEN;
                           StartData.IconFile = 0;
                           StartData.PgmHandle = 0;
                           StartData.PgmControl = SSF_CONTROL_VISIBLE;
                           StartData.InitXPos = 30;
                           StartData.InitYPos = 40;
                           StartData.InitXSize = 200;
                           StartData.InitYSize = 140;
                           StartData.Reserved = 0;
                           StartData.ObjectBuffer = ObjBuf;
                           StartData.ObjectBuffLen = sizeof (ObjBuf);

                           printf ("Spawning channel #%d", i + atoi (argv[1]));
                           DosStartSession (&StartData, &idChannels[i], &Pid);
                           printf (", id %d\n", idChannels[i]);
                        }
                     }
                     else
                        printf ("ERROR: Too much open connections\n");
                  }
                  DosWaitEventSem (hEvent, 200L);
               }

               if (hSysTermQ != NULLHANDLE) {
                  DosCloseEventSem (hTermQEvent);
                  DosCloseQueue (hSysTermQ);
               }
            }
            delete Tcp;
         }
      }
      else {
         if ((Telnet = new TTelnet ((USHORT)atoi (argv[1]), (USHORT)atoi (argv[4]), (USHORT)atoi (argv[2]))) != NULL) {
            Telnet->Run ();
            delete Telnet;
         }
      }
   }
#endif
}




