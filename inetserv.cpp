
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    13/06/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "inetserv.h"
#include "lora.h"

#if defined(__NT__)
#define DS_3DLOOK           0x0004L
#define DS_CENTER           0x0800L

#define WS_EX_WINDOWEDGE        0x00000100L
#define WS_EX_CLIENTEDGE        0x00000200L

#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
#endif

#if defined(__OS2__)
#define IDOK            1
#define IDCANCEL        2
#endif

#define WMU_ADDITEM        1
#define WMU_CLEAR          2

#if defined(__OS2__)
HAB  hab;
HWND hwndMainFrame, hwndMainClient;
#elif defined(__NT__)
HINSTANCE hinst;
HWND hwndMainClient, hwndMainList;
#endif

#if defined(__OS2__)
MRESULT EXPENTRY AboutDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
#elif defined(__NT__)
BOOL CALLBACK AboutDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
{
   switch (msg) {
#if defined(__OS2__)
      case WM_INITDLG: {
         RECTL Dlg;
         SWP swp;

         WinQueryWindowPos (hwndMainFrame, &swp);
         WinQueryWindowRect (hwnd, &Dlg);
         WinSetWindowPos (hwnd, HWND_TOP, swp.x + ((swp.cx - Dlg.xRight) / 2), swp.y + swp.cy - Dlg.yTop - 48, 0, 0, SWP_MOVE);
         break;
      }
#elif defined(__NT__)
      case WM_INITDIALOG:
         return ((BOOL)TRUE);

      case WM_CTLCOLORBTN:
      case WM_CTLCOLORSTATIC:
      case WM_CTLCOLORDLG: {
         LOGBRUSH logBrush;

         SetBkColor ((HDC)wParam, GetSysColor (COLOR_MENU));

         logBrush.lbStyle = BS_SOLID;
         logBrush.lbColor = GetSysColor (COLOR_MENU);
         logBrush.lbHatch = 0;

         return ((BOOL)CreateBrushIndirect (&logBrush));
      }
#endif

#if defined(__OS2__)
      case WM_ERASEBACKGROUND:
         return ((MRESULT)TRUE);
#endif

      case WM_CLOSE:
#if defined(__OS2__)
         WinDismissDlg (hwnd, TRUE);
#elif defined(__NT__)
         EndDialog (hwnd, FALSE);
#endif
         break;
   }

#if defined(__OS2__)
   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
#elif defined(__NT__)
   return ((BOOL)FALSE);
#endif
}

// ---------------------------------------------------------------------------

class TPMLog : public TLog
{
public:
   TPMLog (HWND hwnd);
   ~TPMLog (void);

   VOID   Write (PSZ pszFormat, ...);
   VOID   WriteSend (PSZ pszFormat, ...);

private:
   HWND   hwndList;
#if defined(__OS2__)
   HMTX   hmtx;
#endif
};

TPMLog::TPMLog (HWND hwnd)
{
   hwndList = hwnd;
#if defined(__OS2__)
   DosCreateMutexSem ("\\SEM32\\INETLOG", &hmtx, 0L, FALSE);
#endif
}

TPMLog::~TPMLog (void)
{
#if defined(__OS2__)
   DosCloseMutexSem (hmtx);
#endif
}

VOID TPMLog::Write (PSZ pszFormat, ...)
{
   va_list arglist;
   PSZ MsgTemp;
   time_t t;
   struct tm *timep;

#if defined(__OS2__)
   DosRequestMutexSem (hmtx, -1);
#endif

   va_start (arglist, pszFormat);
   vsprintf (Buffer, pszFormat, arglist);
   va_end (arglist);

   t = time (NULL);
   timep = localtime (&t);
   sprintf (Temp, "%c %02d %3s %02d:%02d:%02d %s %s", Buffer[0], timep->tm_mday, Months[timep->tm_mon], timep->tm_hour, timep->tm_min, timep->tm_sec, "LORA", &Buffer[1]);

   if (fp != NULL) {
      fprintf (fp, "%s\n", Temp);
      fflush (fp);
   }

   if (hwndList != NULL) {
      sprintf (Temp, "%c %02d:%02d %s", Buffer[0], timep->tm_hour, timep->tm_min, &Buffer[1]);
      if ((MsgTemp = (PSZ)malloc (strlen (Temp) + 1)) != NULL) {
         strcpy (MsgTemp, Temp);
#if defined(__OS2__)
         WinPostMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDITEM), MPFROMP (MsgTemp));
#elif defined(__NT__)
         PostMessage (hwndList, WM_USER, WMU_ADDITEM, (LPARAM)MsgTemp);
#endif
      }
   }

#if defined(__OS2__)
   DosReleaseMutexSem (hmtx);
#endif
}

VOID TPMLog::WriteSend (PSZ pszFormat, ...)
{
   va_list arglist;
   PSZ MsgTemp;
   time_t t;
   struct tm *timep;

#if defined(__OS2__)
   DosRequestMutexSem (hmtx, -1);
#endif

   va_start (arglist, pszFormat);
   vsprintf (Buffer, pszFormat, arglist);
   va_end (arglist);

   t = time (NULL);
   timep = localtime (&t);
   sprintf (Temp, "%c %02d %3s %02d:%02d:%02d %s %s", Buffer[0], timep->tm_mday, Months[timep->tm_mon], timep->tm_hour, timep->tm_min, timep->tm_sec, "LORA", &Buffer[1]);

   if (fp != NULL) {
      fprintf (fp, "%s\n", Temp);
      fflush (fp);
   }

   if (hwndList != NULL) {
      sprintf (Temp, "%c %02d:%02d %s", Buffer[0], timep->tm_hour, timep->tm_min, &Buffer[1]);
      if ((MsgTemp = (PSZ)malloc (strlen (Temp) + 1)) != NULL) {
         strcpy (MsgTemp, Temp);
#if defined(__OS2__)
         WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDITEM), MPFROMP (MsgTemp));
#elif defined(__NT__)
         SendMessage (hwndList, WM_USER, WMU_ADDITEM, (LPARAM)MsgTemp);
#endif
      }
   }

#if defined(__OS2__)
   DosReleaseMutexSem (hmtx);
#endif
}

// ----------------------------------------------------------------------------

#define MAX_TELNET_SLOTS      16

USHORT TelnetSlots[MAX_TELNET_SLOTS];
class  TConfig *Cfg;
class  TPMLog *Log;
class  TTcpip *Web, *Mail, *Pop, *Ftp, *Telnet, *News;

// ----------------------------------------------------------------------------

VOID WebThread (PVOID Args)
{
   class TTcpip *Data;
   class TWeb *Server;

#if defined(__OS2__)
   DosSetPriority (PRTYS_THREAD, PRTYC_IDLETIME, PRTYD_MAXIMUM, 0);
#endif

   if ((Data = (class TTcpip *)Args) != NULL) {
      if ((Server = new TWeb) != NULL) {
         Server->Tcp = Data;
         Server->Cfg = Cfg;
         Server->Log = Log;
         Server->Run ();
      }
      delete Data;
   }

   _endthread ();
}

VOID FtpThread (PVOID Args)
{
   class TTcpip *Data;
   class TFTP *Server;

#if defined(__OS2__)
   DosSetPriority (PRTYS_THREAD, PRTYC_IDLETIME, PRTYD_MAXIMUM, 0);
#endif

   if ((Data = (class TTcpip *)Args) != NULL) {
      if ((Server = new TFTP) != NULL) {
         Server->Tcp = Data;
         Server->Cfg = Cfg;
         Server->Log = Log;
         strcpy (Server->ClientIP, Data->ClientIP);
         if (Server->Login () == TRUE)
            Server->Run ();
      }
      delete Data;
   }

   _endthread ();
}

VOID PopThread (PVOID Args)
{
   class TTcpip *Data;
   class TPOP3 *Server;

#if defined(__OS2__)
   DosSetPriority (PRTYS_THREAD, PRTYC_IDLETIME, PRTYD_MAXIMUM, 0);
#endif

   if ((Data = (class TTcpip *)Args) != NULL) {
      if ((Server = new TPOP3) != NULL) {
         Server->Tcp = Data;
         Server->Cfg = Cfg;
         Server->Log = Log;
         Server->Run ();
      }
      delete Data;
   }

   _endthread ();
}

VOID NewsThread (PVOID Args)
{
   class TTcpip *Data;
   class TNNTP *Server;

#if defined(__OS2__)
   DosSetPriority (PRTYS_THREAD, PRTYC_IDLETIME, PRTYD_MAXIMUM, 0);
#endif

   if ((Data = (class TTcpip *)Args) != NULL) {
      if ((Server = new TNNTP) != NULL) {
         Server->Tcp = Data;
         Server->Cfg = Cfg;
         Server->Log = Log;
         Server->Run ();
      }
      delete Data;
   }

   _endthread ();
}

VOID BbsThread (PVOID Args)
{
   USHORT i, Mailer, Task = 1;
   CHAR Temp[128];
   class TTcpip *Data;
   class TBbs *Bbs;
   class TLog *Log;

#if defined(__OS2__)
   DosSetPriority (PRTYS_THREAD, PRTYC_IDLETIME, PRTYD_MAXIMUM, 0);
#endif

   if ((Data = (class TTcpip *)Args) != NULL) {
      Mailer = FALSE;
      for (i = 0; i < MAX_TELNET_SLOTS; i++) {
         if (TelnetSlots[i] == FALSE) {
            TelnetSlots[i] = TRUE;
            Task = (USHORT)(Cfg->TaskNumber + i);
            break;
         }
      }
      if (i < MAX_TELNET_SLOTS) {
         if ((Log = new TLog) != NULL) {
            sprintf (Temp, Cfg->LogFile, Task);
            Log->Open (Temp);
         }
         if ((Bbs = new TBbs) != NULL) {
            Bbs->Log = Log;
            Bbs->Cfg = Cfg;
            Bbs->Com = Data;
            Bbs->Task = Task;

            Data->SendBytes ((UCHAR *)"\xFF\xFD\x01\xFF\xFD\x00\xFF\xFB\x01\xFF\xFB\x00", 12);

            Bbs->Run ();
            Mailer = Bbs->Mailer;
            delete Bbs;
         }
         if (Log != NULL)
            delete Log;

         TelnetSlots[i] = FALSE;
      }
      delete Data;
   }

   _endthread ();
}

// ----------------------------------------------------------------------------

#if defined(__NT__)
USHORT Win95DialogBox (HINSTANCE hinst, PSZ Dialog, HWND hwnd, DLGPROC DialogProc)
{
   USHORT RetVal = FALSE;
   HRSRC hrsrc;
   HGLOBAL hGlobal;
   DLGTEMPLATE *dlgTemplate;

   if ((hrsrc = FindResource (NULL, Dialog, RT_DIALOG)) != NULL) {
      if ((hGlobal = LoadResource (NULL, hrsrc)) != NULL) {
         if ((dlgTemplate = (DLGTEMPLATE *)LockResource (hGlobal)) != NULL) {
            dlgTemplate->style |= DS_3DLOOK|DS_CENTER;
            RetVal = (USHORT)DialogBoxIndirect (hinst, dlgTemplate, hwnd, (DLGPROC)DialogProc);
         }
      }
      FreeResource (hrsrc);
   }

   return (RetVal);
}
#endif

#if defined(__OS2__)
MRESULT EXPENTRY MainWinProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
#elif defined(__NT__)
LRESULT CALLBACK MainWinProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
{
   switch (msg) {
      case WM_CREATE: {
         CHAR Temp[128];
#if defined(__OS2__)
         HWND hwndMainList;

         if ((hwndMainList = WinCreateWindow (hwnd, WC_LISTBOX, NULL, LS_NOADJUSTPOS|LS_HORZSCROLL|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 101, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndMainList, PP_FONTNAMESIZE, 14, "11.System VIO");
#elif defined(__NT__)
         LOGFONT logFont;
         HFONT hFont;

         if ((hwndMainList = CreateWindow ("LISTBOX", "", LBS_NOINTEGRALHEIGHT|LBS_MULTIPLESEL|WS_CHILD, 0, 0, 100, 50, hwnd, NULL, hinst, NULL)) != NULL) {
            logFont.lfHeight = 12;
            logFont.lfWidth = 8;
            logFont.lfEscapement = 0;
            logFont.lfOrientation = 0;
            logFont.lfWeight = FW_NORMAL;
            logFont.lfItalic = FALSE;
            logFont.lfUnderline = FALSE;
            logFont.lfStrikeOut = FALSE;
            logFont.lfCharSet = OEM_CHARSET;
            logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
            logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
            logFont.lfQuality = DEFAULT_QUALITY;
            logFont.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
            strcpy (logFont.lfFaceName, "Fixedsys");
            if ((hFont = CreateFontIndirect (&logFont)) != NULL)
               SendMessage (hwndMainList, WM_SETFONT, (WPARAM)hFont, MAKELPARAM (FALSE, 0));
            ShowWindow (hwndMainList, SW_SHOW);
         }
#endif
         if (hwndMainList != NULL) {
            if ((Log = new TPMLog (hwnd)) != NULL) {
               if (Cfg->LogFile[0] != '\0')
                  sprintf (Temp, "lora%u.log", Cfg->TaskNumber);
               else
                  sprintf (Temp, Cfg->LogFile, Cfg->TaskNumber);

               Log->Open (Temp);
#if defined(__OS2__)
               Log->Write ("+Begin, v%s (OS/2)", VERSION);
#elif defined(__NT__)
               Log->Write ("+Begin, v%s (Win32)", VERSION);
#endif
               Log->Write ("+Message-base sharing is enabled");
            }
         }
         break;
      }

#if defined(__NT__)
      case WM_CTLCOLORLISTBOX: {
         LOGBRUSH logBrush;

         SetBkColor ((HDC)wParam, GetSysColor (COLOR_WINDOW));

         logBrush.lbStyle = BS_SOLID;
         logBrush.lbColor = GetSysColor (COLOR_WINDOW);
         logBrush.lbHatch = 0;

         return ((BOOL)CreateBrushIndirect (&logBrush));
      }
#endif

      case WM_USER:
#if defined(__OS2__)
         switch (SHORT1FROMMP (mp1)) {
#elif defined(__NT__)
         switch (wParam) {
#endif
            case WMU_ADDITEM: {
               USHORT Item;

#if defined(__OS2__)
               if ((USHORT)WinSendMsg (WinWindowFromID (hwnd, 101), LM_QUERYITEMCOUNT, 0L, 0L) > 200)
                  WinSendMsg (WinWindowFromID (hwnd, 101), LM_DELETEITEM, MPFROMSHORT (0), 0L);

               Item = (USHORT)WinSendMsg (WinWindowFromID (hwnd, 101), LM_INSERTITEM, MPFROMSHORT (LIT_END), mp2);
               WinSendMsg (WinWindowFromID (hwnd, 101), LM_SELECTITEM, MPFROMSHORT (Item), MPFROMSHORT (TRUE));
               free ((PSZ)mp2);
#elif defined(__NT__)
               SendMessage (hwndMainList, LB_SETSEL, FALSE, (LPARAM)-1);
               Item = (USHORT)SendMessage (hwndMainList, LB_ADDSTRING, 0, lParam);
               SendMessage (hwndMainList, LB_SETSEL, TRUE, (LPARAM)Item);
               free ((PSZ)lParam);
#endif
               break;
            }

            case WMU_CLEAR:
#if defined(__OS2__)
               WinSendMsg (WinWindowFromID (hwnd, 101), LM_DELETEALL, 0L, 0L);
#elif defined(__NT__)
#endif
               break;
         }
         break;

      case WM_TIMER: {
         USHORT Socket;
         class TTcpip *Data;

         if (Web != NULL) {
            if ((Socket = Web->WaitClient ()) != 0) {
               Log->Write (":Web: Incoming from %s (%s)", Web->ClientName, Web->ClientIP);
               if ((Data = new TTcpip) != NULL) {
                  Data->Initialize (0, Socket);
#if defined(__BORLANDC__)
                  _beginthread (WebThread, 8192, (PVOID)Data);
#elif defined(_MSC_VER)
                  DWORD ThreadId;

                  CreateThread (NULL, 8192, (LPTHREAD_START_ROUTINE)WebThread, (PVOID)Data, 0L, &ThreadId);
#else
                  _beginthread (WebThread, NULL, 8192U, (PVOID)Data);
#endif
               }
            }
         }
         if (Ftp != NULL) {
            if ((Socket = Ftp->WaitClient ()) != 0) {
               Log->Write (":FTP: Incoming from %s (%s)", Ftp->ClientName, Ftp->ClientIP);
               if ((Data = new TTcpip) != NULL) {
                  Data->Initialize (0, Socket);
                  strcpy (Data->ClientIP, Ftp->ClientIP);
#if defined(__BORLANDC__)
                  _beginthread (FtpThread, 8192, (PVOID)Data);
#elif defined(_MSC_VER)
                  DWORD ThreadId;

                  CreateThread (NULL, 8192, (LPTHREAD_START_ROUTINE)FtpThread, (PVOID)Data, 0L, &ThreadId);
#else
                  _beginthread (FtpThread, NULL, 32768U, (PVOID)Data);
#endif
               }
            }
         }
         if (Telnet != NULL) {
            if ((Socket = Telnet->WaitClient ()) != 0) {
               Log->Write (":Telnet: Incoming from %s (%s)", Telnet->ClientName, Telnet->ClientIP);
               if ((Data = new TTcpip) != NULL) {
                  Data->Initialize (0, Socket);
#if defined(__BORLANDC__)
                  _beginthread (BbsThread, 8192, (PVOID)Data);
#elif defined(_MSC_VER)
                  DWORD ThreadId;

                  CreateThread (NULL, 8192, (LPTHREAD_START_ROUTINE)BbsThread, (PVOID)Data, 0L, &ThreadId);
#else
                  _beginthread (BbsThread, NULL, 32768U, (PVOID)Data);
#endif
               }
            }
         }
         if (Pop != NULL) {
            if ((Socket = Pop->WaitClient ()) != 0) {
               Log->Write (":POP3: Incoming from %s (%s)", Pop->ClientName, Pop->ClientIP);
               if ((Data = new TTcpip) != NULL) {
                  Data->Initialize (0, Socket);
#if defined(__BORLANDC__)
                  _beginthread (PopThread, 8192, (PVOID)Data);
#elif defined(_MSC_VER)
                  DWORD ThreadId;

                  CreateThread (NULL, 8192, (LPTHREAD_START_ROUTINE)PopThread, (PVOID)Data, 0L, &ThreadId);
#else
                  _beginthread (PopThread, NULL, 8192, (PVOID)Data);
#endif
               }
            }
         }
         if (News != NULL) {
            if ((Socket = News->WaitClient ()) != 0) {
               Log->Write (":NNTP: Incoming from %s (%s)", News->ClientName, News->ClientIP);
               if ((Data = new TTcpip) != NULL) {
                  Data->Initialize (0, Socket);
#if defined(__BORLANDC__)
                  _beginthread (NewsThread, 8192, (PVOID)Data);
#elif defined(_MSC_VER)
                  DWORD ThreadId;

                  CreateThread (NULL, 8192, (LPTHREAD_START_ROUTINE)NewsThread, (PVOID)Data, 0L, &ThreadId);
#else
                  _beginthread (NewsThread, NULL, 8192, (PVOID)Data);
#endif
               }
            }
         }

         if (Telnet == NULL) {
            if ((Telnet = new TTcpip) != NULL) {
               if (Telnet->Initialize (23) == TRUE)
                  Log->Write ("+Telnet Server started on port 23");
               else {
                  delete Telnet;
                  Telnet = NULL;
               }
            }
         }
         if (Ftp == NULL) {
            if ((Ftp = new TTcpip) != NULL) {
               if (Ftp->Initialize (21) == TRUE)
                  Log->Write ("+FTP Server started on port 21");
               else {
                  delete Ftp;
                  Ftp = NULL;
               }
            }
         }
         if (Web == NULL) {
            if ((Web = new TTcpip) != NULL) {
               if (Web->Initialize (80) == TRUE)
                  Log->Write ("+HTTP Server started on port 80");
               else {
                  delete Web;
                  Web = NULL;
               }
            }
         }
         if (Pop == NULL) {
            if ((Pop = new TTcpip) != NULL) {
               if (Pop->Initialize (110) == TRUE)
                  Log->Write ("+POP3 Server started on port 110");
               else {
                  delete Pop;
                  Pop = NULL;
               }
            }
         }
         if (News == NULL) {
            if ((News = new TTcpip) != NULL) {
               if (News->Initialize (119) == TRUE)
                  Log->Write ("+NNTP Server started on port 119");
               else {
                  delete News;
                  News = NULL;
               }
            }
         }
         break;
      }

      case WM_SIZE:
#if defined(__OS2__)
         WinSetWindowPos (WinWindowFromID (hwnd, 101), NULLHANDLE, 0, 0, SHORT1FROMMP (mp2), SHORT2FROMMP (mp2), SWP_SIZE|SWP_SHOW);
#elif defined(__NT__)
         MoveWindow (hwndMainList, 0, 0, LOWORD (lParam), HIWORD (lParam), TRUE);
#endif
         break;

#if defined(__OS2__)
      case WM_ERASEBACKGROUND:
         return ((MRESULT)TRUE);
#endif

      case WM_COMMAND:
#if defined(__OS2__)
         switch (SHORT1FROMMP (mp1)) {
#elif defined(__NT__)
         switch (wParam) {
#endif
            case 206:      // Global / Internet Options
#if defined(__OS2__)
               if ((USHORT)WinDlgBox (HWND_DESKTOP, hwnd, InternetDlgProc, NULLHANDLE, 23, NULL) == TRUE)
#elif defined(__NT__)
               if (Win95DialogBox (hinst, "INTERNET", hwnd, (DLGPROC)InternetDlgProc) == TRUE)
#endif
                  Cfg->Save ();
               break;
         }
         break;

#if defined(__OS2__)
      case WM_CONTROL:
         break;
#endif

#if defined(__OS2__)
      case WM_CLOSE:
#elif defined(__NT__)
      case WM_DESTROY:
#endif
         if (Log != NULL) {
            Log->Write (":End");
            Log->WriteBlank ();
            delete Log;
         }
#if defined(__NT__)
         PostQuitMessage (0);
#endif
         break;
   }

#if defined(__OS2__)
   return (WinDefWindowProc (hwnd, msg, mp1, mp2));
#elif defined(__NT__)
   return (DefWindowProc (hwnd, msg, wParam, lParam));
#endif
}

#if defined(__OS2__)
void main (int argc, char *argv[])
{
   int i;
   USHORT Task = 1;
   CHAR Title[128], *Config = NULL, *Channel = NULL;
   HMQ hmq;
   QMSG qmsg;
   ULONG flFrame;
   POINTL aptl[2];
//   HACCEL haccel;

   Log = NULL;
   Web = Mail = Pop = Ftp = Telnet = News = NULL;

   for (i = 1; i < argc; i++) {
      if (!stricmp (argv[i], "/LINE")) {
         i++;
         Task = (USHORT)atoi (argv[i]);
      }
      else if (Config == NULL)
         Config = argv[i];
      else if (Channel == NULL)
         Channel = argv[i];
   }

   if ((Cfg = new TConfig) != NULL) {
      Cfg->TaskNumber = Task;
      if (Cfg->Load (Config, Channel) == FALSE)
         Cfg->Default ();
   }

   if ((hab = WinInitialize (0)) != 0) {
      if ((hmq = WinCreateMsgQueue (hab, 0)) != 0) {
         WinRegisterClass (hab, "MAIN_WINDOW", MainWinProc, CS_CLIPCHILDREN|CS_SIZEREDRAW|CS_MOVENOTIFY, 0);

         flFrame = (FCF_TASKLIST|FCF_TITLEBAR|FCF_SYSMENU|FCF_MINMAX|FCF_SIZEBORDER|FCF_NOBYTEALIGN|FCF_MENU);
         if ((hwndMainFrame = WinCreateStdWindow (HWND_DESKTOP, 0, &flFrame, "MAIN_WINDOW", NULL, 0, NULLHANDLE, 257, &hwndMainClient)) != NULLHANDLE) {
            sprintf (Title, "%s Internet Server v%s%s", NAME, VERSION, "");
            WinSetWindowText (hwndMainFrame, Title);

            aptl[ 0 ].x = 60;
            aptl[ 0 ].y = 100;
            aptl[ 1 ].x = 250;
            aptl[ 1 ].y = 100;
            WinMapDlgPoints (HWND_DESKTOP, aptl, 2, TRUE);
            WinSetWindowPos (hwndMainFrame, NULLHANDLE, aptl[0].x, aptl[0].y, aptl[1].x, aptl[1].y, SWP_SIZE|SWP_MOVE|SWP_SHOW|SWP_ACTIVATE);

            WinStartTimer (hab, hwndMainClient, 1, 50);

            while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0)) {
               WinDispatchMsg (hab, &qmsg);
            }

            WinStopTimer (hab, hwndMainClient, 1);
            WinDestroyWindow (hwndMainFrame);
         }

         WinDestroyMsgQueue (hmq);
      }
      WinTerminate (hab);
   }

   if (Cfg != NULL)
      delete Cfg;
}
#elif defined(__NT__)
int PASCAL WinMain (HINSTANCE hinstCurrent, HINSTANCE hinstPrevious, LPSTR lpszCmdLine, int nCmdShow)
{
   MSG msg;
   WNDCLASS wc;

   Log = NULL;
   Modem = NULL;
   Events = NULL;
   hwndModemClient = NULL;
   lpszCmdLine = lpszCmdLine;

   if ((Cfg = new TConfig) != NULL) {
      if (Cfg->Load () == FALSE)
         Cfg->Default ();
      if ((Events = new TEvents (Cfg->SchedulerFile)) != NULL)
         Events->Load ();
      Outbound = new TOutbound (Cfg->Outbound);
   }

   if (hinstPrevious == NULL) {
      wc.style         = 0;
      wc.lpfnWndProc   = MainWinProc;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.hInstance     = hinstCurrent;
      wc.hIcon         = NULL;
      wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
      wc.lpszMenuName  = "MENU_1";
      wc.lpszClassName = "MAIN_WINDOW";

      if (!RegisterClass (&wc))
         return (FALSE);

      wc.style         = 0;
      wc.lpfnWndProc   = ModemWinProc;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.hInstance     = hinstCurrent;
      wc.hIcon         = NULL;
      wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
      wc.lpszMenuName  = NULL;
      wc.lpszClassName = "MODEM_WINDOW";

      if (!RegisterClass (&wc))
         return (FALSE);
   }

   hinst = hinstCurrent;
   msg.wParam = FALSE;

   if ((hwndMainClient = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW, "MAIN_WINDOW", "LoraBBS v3.00 (Unregistered)", WS_OVERLAPPEDWINDOW, 170, 240, 400, 200, NULL, NULL, hinstCurrent, NULL)) != NULL) {
      ShowWindow (hwndMainClient, nCmdShow);

      Status = 0;
      SetTimer (hwndMainClient, 1, MODEM_DELAY, NULL);
      if (Events != NULL)
         SetTimer (hwndMainClient, 2, EVENTS_DELAY, NULL);

      while (GetMessage (&msg, NULL, 0, 0)) {
         TranslateMessage (&msg);
         DispatchMessage (&msg);
      }

      if (Events != NULL)
         KillTimer (hwndMainClient, 2);
      KillTimer (hwndMainClient, 1);
   }

   if (Cfg != NULL)
      delete Cfg;
   if (Events != NULL)
      delete Events;
   if (Outbound != NULL)
      delete Outbound;

   return ((int)msg.wParam);
}
#endif

