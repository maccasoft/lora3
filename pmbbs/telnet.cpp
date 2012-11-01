
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.16
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_defs.h"
#include "combase.h"
#include "mailer.h"
#include "menu.h"
#include "version.h"

#define MAX_CHANNELS       16
#define SYSTEM_TERMQ       "\\QUEUES\\TELTERM.QUE"

class TScreen : public TCom
{
public:
   TScreen (void);
   ~TScreen (void);

   SHORT  BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   SHORT  Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   SHORT  Initialize (VOID);
   UCHAR  ReadByte (VOID);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);
};

TScreen::TScreen (void)
{
   EndRun = FALSE;
   RxBytes = 0;
}

TScreen::~TScreen (void)
{
}

SHORT TScreen::BytesReady (VOID)
{
   SHORT RetVal = FALSE;

   if (kbhit ())
      RetVal = TRUE;

   return (RetVal);
}

VOID TScreen::BufferByte (UCHAR byte)
{
   TxBuffer[TxBytes++] = byte;
   if (TxBytes >= TSIZE)
      UnbufferBytes ();
}

VOID TScreen::BufferBytes (UCHAR *bytes, USHORT len)
{
   while (len > 0 && EndRun == FALSE) {
      TxBuffer[TxBytes++] = *bytes++;
      if (TxBytes >= TSIZE)
         UnbufferBytes ();
      len--;
   }
}

SHORT TScreen::Carrier (VOID)
{
   return (TRUE);
}

VOID TScreen::ClearOutbound (VOID)
{
   TxBytes = 0;
}

VOID TScreen::ClearInbound (VOID)
{
   RxBytes = 0;
}

SHORT TScreen::Initialize (VOID)
{
   return (TRUE);
}

UCHAR TScreen::ReadByte (VOID)
{
   return ((UCHAR)getch ());
}

VOID TScreen::SendByte (UCHAR byte)
{
   fwrite (&byte, 1, 1, stdout);
   fflush (stdout);
}

VOID TScreen::SendBytes (UCHAR *bytes, USHORT len)
{
   fwrite (bytes, len, 1, stdout);
   fflush (stdout);
}

VOID TScreen::UnbufferBytes (VOID)
{
   fwrite (TxBuffer, TxBytes, 1, stdout);
   fflush (stdout);
   TxBytes = 0;
}

// ----------------------------------------------------------------------

class TTelnet : public TBbs
{
public:
   TTelnet (USHORT usCfg, USHORT usChannel, USHORT usSocket);
   ~TTelnet (void);

   LONG   CheckResponse (VOID);
   VOID   ReleaseTimeSlice (VOID);
   VOID   Run (VOID);
   VOID   SendCommand (PSZ cmd);
   VOID   SetStatus (PSZ pszAction);
   VOID   SetBrowsingStatus (PSZ pszMenu);

private:
   USHORT Socket;
   HEV    hEvent;
   class  TTcpip *Tcpip;
   class  TScreen *Screen;
};

TTelnet::TTelnet (USHORT usCfg, USHORT usChannel, USHORT usSocket) : TBbs ()
{
   User = 0;
   if ((Cfg = new TConfig (".\\")) != 0) {
      Cfg->Read (usCfg);
      Cfg->Channel = usChannel;
   }
   Socket = usSocket;
}

TTelnet::~TTelnet (void)
{
   if (Cfg != 0)
      delete Cfg;
}

VOID TTelnet::Run (VOID)
{
   CHAR temp[64];
   ULONG CallLen;
   class TDetect *Detect;
   class TEMail *Mail;

   if ((Log = new TLog) != 0) {
      sprintf (temp, "%sCH%d.LOG", Cfg->LogPath, Cfg->Channel);
      if (Log->Open (temp) == TRUE) {
         Log->Level = Cfg->LogLevel;
         Log->Write (Log->Begin, VERSION, Cfg->Channel);

         if ((Tcpip = new TTcpip) != 0) {
            if (Tcpip->Initialize (0, Socket) == TRUE) {
               Com = Tcpip;

               if ((Screen = new TScreen) != 0) {
                  if (Screen->Initialize () == TRUE)
                     Snoop = Screen;
               }

               DosCreateEventSem (NULL, &hEvent, 0L, FALSE);

               Hangup = FALSE;
               Ansi = TRUE;
               Lang = new TLanguage;
               User = new TUser (Cfg->UserPath);
               Limits = new TLimits (Cfg->UserPath);

               SetStatus ("Channel has activity");
               LastActivity = StartCall = time (NULL);
               CarrierSpeed = 57600L;

               Tcpip->SendBytes ((UCHAR *)"\xFF\xFB\x01\xFF\xFB\x00", 6);
               DisplayBanner ();

               if ((Detect = new TDetect (this)) != 0) {
                  Detect->SelectLanguage ();
                  delete Detect;
               }

               if (LoginUser () == TRUE) {
                  SetStatus ("Browsing");
                  if (VerifyAccount () == TRUE) {
                     if ((Mail = new TEMail (this)) != 0) {
                        Mail->Check ();
                        delete Mail;
                     }
                     class TMenu *Menu = new TMenu (this);
                     Menu->Run ("TOP");
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
               Limits = 0;
               delete User;
               User = 0;
               delete Lang;
               Lang = 0;

               Pause (200);
               Com->SendBytes ((UCHAR *)"\r\n\x1B[0;37m", 9);
               SetStatus ("Local Session");

               DosCloseEventSem (hEvent);
            }

            delete Tcpip;
            if (Snoop != 0)
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
   DosWaitEventSem (hEvent, 10L);
}

VOID TTelnet::SetStatus (PSZ pszAction)
{
   pszAction = pszAction;
}

VOID TTelnet::SetBrowsingStatus (PSZ pszMenu)
{
   pszMenu = pszMenu;
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
HQUEUE hSysTermQ = NULLHANDLE;

void main (int argc, char *argv[])
{
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
         if ((Tcp = new TTcpip) != 0) {
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
         if ((Telnet = new TTelnet ((USHORT)atoi (argv[1]), (USHORT)atoi (argv[4]), (USHORT)atoi (argv[2]))) != 0) {
            Telnet->Run ();
            delete Telnet;
         }
      }
   }
}



