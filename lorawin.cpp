
// LoraBBS Version 2.99 Free Edition
// Copyright (C) 1987-98 Marco Maccaferri
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "_ldefs.h"
#include "lorawin.h"

#define MAIL_IMPORTKNOWN         0x0001L
#define MAIL_IMPORTPROTECTED     0x0002L
#define MAIL_IMPORTNORMAL        0x0004L
#define MAIL_EXPORT              0x0008L
#define MAIL_PACK                0x0010L
#define MAIL_NEWSGROUP           0x0020L
#define MAIL_TIC                 0x0040L
#define MAIL_EMAIL               0x0080L
#define MAIL_IMPORTBAD           0x0100L
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
#define BBSEXIT            7
#define UNDEFINED          99

PSZ statusText[] = {
   "Startup",
   "Initialize",
   "WaitForOK",
   "WaitForCall",
   "Answering",
   "Hangup",
   "WaitForConnect",
   "BBSExit"
};

#define MODEM_DELAY        200
#define EVENTS_DELAY       1000
#define REINIT_DELAY       60000L

#if defined(__OS2__)
HAB  hab;
HWND hwndMainFrame, hwndMainClient;
HWND help_hWnd;
FILEDLG fild;
#elif defined(__NT__)
HINSTANCE hinst;
HWND hwndMainClient, hwndMainList, hwndModemList, hwndStatusList, hwndOutboundList;
#endif

USHORT Status, Current, gotPort, gotSpeed, timeLimit = 0;
CHAR   PollNode[64], ExternalProgram[128];
LONG   TimeOut, CallTimer = 0L;
ULONG  comHandle = 0L, connectSpeed = 0L;
UCHAR  ErrorLevel = 1;
class  TConfig *Cfg;
class  TModem *Modem;
class  TPMLog *Log;
class  TEvents *Events;
class  TOutbound *Outbound;

#define WMU_STARTTIMER              10

VOID StartTimer (HWND hwnd, int id, ULONG msec)
{
#if defined(__OS2__)
   WinStartTimer (hab, hwnd, id, msec);
#elif defined(__NT__)
   SetTimer (hwnd, id, msec, NULL);
#endif
}

VOID StopTimer (HWND hwnd, int id)
{
#if defined(__OS2__)
   WinStopTimer (hab, hwnd, id);
#elif defined(__NT__)
   KillTimer (hwnd, id);
#endif
}

// ----------------------------------------------------------------------
// Product informations dialog
// ----------------------------------------------------------------------

#if !defined(__POINT__)

class CProductDlg : public CDialog
{
public:
   CProductDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
};

CProductDlg::CProductDlg (HWND p_hWnd) : CDialog ("100", p_hWnd)
{
}

USHORT CProductDlg::OnInitDialog ()
{
   CHAR Temp[128], RegName[64], RegNumber[16];

   Center ();

   return (TRUE);
}

// ----------------------------------------------------------------------
// Outbound details dialog
// ----------------------------------------------------------------------

class CDetailsDlg : public CDialog
{
public:
   CDetailsDlg (HWND p_hWnd);
   ~CDetailsDlg ();

   USHORT DoRebuild;
   CHAR   Address[64];

   VOID   OnHelp ();
   USHORT OnInitDialog ();

private:
   CHAR   Temp[1024];
   class  TOutbound *Out;
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   ChangeToNormal ();
   VOID   ChangeToDirect ();
   VOID   ChangeToCrash ();
   VOID   ChangeToHold ();
   VOID   Delete ();
   VOID   DeleteEntry ();
   VOID   Refresh ();
   VOID   ToggleImmediate ();
};

BEGIN_MESSAGE_MAP (CDetailsDlg, CDialog)
   ON_COMMAND (48, ChangeToNormal)
   ON_COMMAND (49, ChangeToDirect)
   ON_COMMAND (50, ChangeToCrash)
   ON_COMMAND (51, ChangeToHold)
   ON_COMMAND (52, ToggleImmediate)
   ON_COMMAND (115, Add)
   ON_COMMAND (116, Delete)
   ON_COMMAND (117, DeleteEntry)
END_MESSAGE_MAP ()

CDetailsDlg::CDetailsDlg (HWND p_hWnd) : CDialog ("47", p_hWnd)
{
   DoRebuild = FALSE;
   if ((Out = new TOutbound (Cfg->Outbound)) != NULL) {
      Cfg->MailAddress.First ();
      Out->DefaultZone = Cfg->MailAddress.Zone;
   }
}

CDetailsDlg::~CDetailsDlg ()
{
   if (Out != NULL)
      delete Out;
}

VOID CDetailsDlg::OnHelp ()
{
   WinHelp ("lora.hlp>h_ref", 47);
}

USHORT CDetailsDlg::OnInitDialog ()
{
   class TNodes *Nodes;

   Center ();

   LVM_AllocateColumns (101, 4);
   LVM_InsertColumn (101, "Name", LVC_LEFT);
   LVM_InsertColumn (101, "Size", LVC_RIGHT);
   LVM_InsertColumn (101, "Flag", LVC_CENTER);
   LVM_InsertColumn (101, "Location", LVC_LEFT);

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (Address) == TRUE) {
         SetDlgItemText (102, Nodes->SystemName);
         SetDlgItemText (103, Nodes->Address);
         SetDlgItemText (109, Nodes->Location);
         SetDlgItemText (102, Nodes->SysopName);
      }
      delete Nodes;
   }

   Refresh ();

   return (TRUE);
}

VOID CDetailsDlg::Delete ()
{
   int item;

   if ((item = LVM_QuerySelectedItem (101)) >= 0 && Out != NULL) {
      if (MessageBox ("Do you really want to delete the selected file ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
         if (Out->First () == TRUE)
            do {
               if (item == 0) {
                  Out->Remove ();
                  Out->Update ();
                  LVM_DeleteItem (101, LVM_QuerySelectedItem (101));
                  DoRebuild = TRUE;
                  break;
               }
               item--;
            } while (Out->Next () == TRUE);
      }
   }
}

VOID CDetailsDlg::DeleteEntry ()
{
   if (Out != NULL) {
      if (MessageBox ("Do you really want to delete this node's entry ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
         while (Out->First () == TRUE)
            Out->Remove ();
         Out->Update ();
         LVM_DeleteAll (101);
         DoRebuild = TRUE;
         EndDialog (FALSE);
      }
   }
}

VOID CDetailsDlg::Add ()
{
   class TAddress Addr;
   struct stat statbuf;
#if defined(__OS2__)
   ULONG i;
   FILEDLG fild;

   Addr.Parse (Address);

   memset (&fild, 0, sizeof (FILEDLG));
   fild.cbSize = sizeof (FILEDLG);
   fild.fl = FDS_CENTER|FDS_OPEN_DIALOG|FDS_MULTIPLESEL;
   fild.pszTitle = "Add File";
   sprintf (fild.szFullFile, "*.*");

   WinFileDlg (HWND_DESKTOP, m_hWnd, &fild);
   if (fild.lReturn == DID_OK) {
      if (fild.papszFQFilename != NULL) {
         for (i = 0; i < fild.ulFQFCount; i++) {
            Out->New ();
            Out->Zone = Addr.Zone;
            Out->Net = Addr.Net;
            Out->Node = Addr.Node;
            Out->Point = Addr.Point;
            strcpy (Out->Complete, fild.papszFQFilename[i][0]);
            if (stat (Out->Complete, &statbuf) == 0)
               Out->Size = statbuf.st_size;
            Out->Status = 'h';
            Out->Add ();
         }
      }
      else {
         Out->New ();
         Out->Zone = Addr.Zone;
         Out->Net = Addr.Net;
         Out->Node = Addr.Node;
         Out->Point = Addr.Point;
         strcpy (Out->Complete, fild.szFullFile);
         if (stat (Out->Complete, &statbuf) == 0)
            Out->Size = statbuf.st_size;
         Out->Status = 'h';
         Out->Add ();
      }
      Out->Update ();
      DoRebuild = TRUE;
      Refresh ();
   }
   if (fild.papszFQFilename != NULL)
      WinFreeFileDlgList (fild.papszFQFilename);
#elif defined(__NT__)
   CHAR Path[256], *p;
   OPENFILENAME OpenFileName;

   Addr.Parse (Address);

   Temp[0] = '\0';
   OpenFileName.lStructSize = sizeof (OPENFILENAME);
   OpenFileName.hwndOwner = m_hWnd;
   OpenFileName.hInstance = NULL;
   OpenFileName.lpstrFilter = "All files (*.*)\0*.*";
   OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
   OpenFileName.nMaxCustFilter = 0L;
   OpenFileName.nFilterIndex = 1L;
   OpenFileName.lpstrFile = Temp;
   OpenFileName.nMaxFile = sizeof (Temp) - 1;
   OpenFileName.lpstrFileTitle = NULL;
   OpenFileName.nMaxFileTitle = 0;
   OpenFileName.lpstrInitialDir = NULL;
   OpenFileName.lpstrTitle = "Add File";
   OpenFileName.nFileOffset = 0;
   OpenFileName.nFileExtension = 0;
   OpenFileName.lpstrDefExt = "";
   OpenFileName.lCustData = 0;
   OpenFileName.Flags = OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_CREATEPROMPT|OFN_NOCHANGEDIR|OFN_ALLOWMULTISELECT;

   if (GetOpenFileName (&OpenFileName) == TRUE) {
      if ((p = strtok (Temp, " ")) != NULL) {
         strcpy (Path, p);
         if (Path[strlen (Path) - 1] != '\\')
            strcat (Path, "\\");
         do {
            Out->New ();
            Out->Zone = Addr.Zone;
            Out->Net = Addr.Net;
            Out->Node = Addr.Node;
            Out->Point = Addr.Point;
            sprintf (Out->Complete, "%s%s", Path, p);
            if (stat (Out->Complete, &statbuf) == 0)
               Out->Size = statbuf.st_size;
            Out->Status = 'h';
            Out->Add ();
         } while ((p = strtok (NULL, " ")) != NULL);

         Out->Update ();
         DoRebuild = TRUE;
         Refresh ();
      }
   }
#endif
}

VOID CDetailsDlg::ToggleImmediate ()
{
   USHORT Found = FALSE;

   if (Out->First () == TRUE)
      do {
         if (Out->Poll == TRUE && toupper (Out->Status) == 'I') {
            Out->Remove ();
            Out->Update ();
            Found = TRUE;
            break;
         }
      } while (Out->Next () == TRUE);

   if (Found == FALSE)
      Out->PollNode (Address, 'i');

   Refresh ();
   DoRebuild = TRUE;
}

VOID CDetailsDlg::Refresh ()
{
   class TAddress Addr;

   LVM_DeleteAll (101);

   if (Out != NULL) {
      Out->Clear ();
      Addr.Parse (Address);
      Out->Add (Addr.Zone, Addr.Net, Addr.Node, Addr.Point);
      if (Out->First () == TRUE)
         do {
            LVM_InsertItem (101);

            if (Out->Poll == FALSE) {
               LVM_SetItemText (101, 0, Out->Name);
               sprintf (Temp, "%lu", Out->Size);
               LVM_SetItemText (101, 1, Temp);
               sprintf (Temp, "%c", toupper (Out->Status));
               LVM_SetItemText (101, 2, Temp);
               LVM_SetItemText (101, 3, Out->Complete);
            }
            else {
               LVM_SetItemText (101, 0, "<POLL>");
               LVM_SetItemText (101, 1, "0");
               sprintf (Temp, "%c", toupper (Out->Status));
               LVM_SetItemText (101, 2, Temp);
               LVM_SetItemText (101, 3, "");
            }
         } while (Out->Next () == TRUE);
   }

   LVM_InvalidateView (101);
}

VOID CDetailsDlg::ChangeToHold ()
{
   class TMailProcessor *Processor;

   if ((Processor = new TMailProcessor) != NULL) {
      Processor->Cfg = Cfg;
      Processor->Log = NULL;
      Processor->Output = NULL;
      Processor->Status = NULL;
      strcpy (Processor->Outbound, Cfg->Outbound);
      if (Processor->Outbound[strlen (Processor->Outbound) - 1] == '\\' || Processor->Outbound[strlen (Processor->Outbound) - 1] == '/')
         Processor->Outbound[strlen (Processor->Outbound) - 1] = '\0';

      sprintf (Temp, "Change Normal Hold %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      sprintf (Temp, "Change Crash Hold %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      sprintf (Temp, "Change Direct Hold %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      delete Processor;
   }

   Refresh ();
   DoRebuild = TRUE;
}

VOID CDetailsDlg::ChangeToCrash ()
{
   class TMailProcessor *Processor;

   if ((Processor = new TMailProcessor) != NULL) {
      Processor->Cfg = Cfg;
      Processor->Log = NULL;
      Processor->Output = NULL;
      Processor->Status = NULL;
      strcpy (Processor->Outbound, Cfg->Outbound);
      if (Processor->Outbound[strlen (Processor->Outbound) - 1] == '\\' || Processor->Outbound[strlen (Processor->Outbound) - 1] == '/')
         Processor->Outbound[strlen (Processor->Outbound) - 1] = '\0';

      sprintf (Temp, "Change Normal Crash %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      sprintf (Temp, "Change Hold Crash %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      sprintf (Temp, "Change Direct Crash %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      delete Processor;
   }

   Refresh ();
   DoRebuild = TRUE;
}

VOID CDetailsDlg::ChangeToDirect ()
{
   class TMailProcessor *Processor;

   if ((Processor = new TMailProcessor) != NULL) {
      Processor->Cfg = Cfg;
      Processor->Log = NULL;
      Processor->Output = NULL;
      Processor->Status = NULL;
      strcpy (Processor->Outbound, Cfg->Outbound);
      if (Processor->Outbound[strlen (Processor->Outbound) - 1] == '\\' || Processor->Outbound[strlen (Processor->Outbound) - 1] == '/')
         Processor->Outbound[strlen (Processor->Outbound) - 1] = '\0';

      sprintf (Temp, "Change Normal Direct %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      sprintf (Temp, "Change Crash Direct %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      sprintf (Temp, "Change Hold Direct %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      delete Processor;
   }

   Refresh ();
   DoRebuild = TRUE;
}

VOID CDetailsDlg::ChangeToNormal ()
{
   class TMailProcessor *Processor;

   if ((Processor = new TMailProcessor) != NULL) {
      Processor->Cfg = Cfg;
      Processor->Log = NULL;
      Processor->Output = NULL;
      Processor->Status = NULL;
      strcpy (Processor->Outbound, Cfg->Outbound);
      if (Processor->Outbound[strlen (Processor->Outbound) - 1] == '\\' || Processor->Outbound[strlen (Processor->Outbound) - 1] == '/')
         Processor->Outbound[strlen (Processor->Outbound) - 1] = '\0';

      sprintf (Temp, "Change Hold Normal %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      sprintf (Temp, "Change Crash Normal %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      sprintf (Temp, "Change Direct Normal %s", Address);
      strtok (Temp, " ");
      Processor->Change ();

      delete Processor;
   }

   Refresh ();
   DoRebuild = TRUE;
}

#endif

// ---------------------------------------------------------------------------
// Windowed log file display
// ---------------------------------------------------------------------------

TPMLog::TPMLog (HWND hwnd)
{
   hwndList = hwnd;
   First = TRUE;
}

TPMLog::~TPMLog ()
{
}

VOID TPMLog::Write (PSZ pszFormat, ...)
{
   va_list arglist;
   time_t t;
   struct tm *timep;
   struct stat statbuf;

   if (First == TRUE && fp != NULL) {
      if (Cfg->ReloadLog == TRUE) {
         fstat (fileno (fp), &statbuf);
         if (statbuf.st_size >= 5192L) {
            fseek (fp, statbuf.st_size - 5192L, SEEK_SET);
            fgets (Temp, sizeof (Temp) - 1, fp);
         }
         else
            fseek (fp, 0L, SEEK_SET);

         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            Temp[strlen (Temp) - 1] = '\0';
            if (Temp[0] != '\0') {
               strcpy (&Temp[14], &Temp[17]);
               strcpy (&Temp[2], &Temp[9]);
               strcpy (&Temp[8], &Temp[13]);
#if defined(__OS2__)
               WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDLOGITEM), MPFROMP (Temp));
#elif defined(__NT__)
               SendMessage (hwndList, WM_USER, WMU_ADDLOGITEM, (LPARAM)Temp);
#endif
            }
         }
      }
      First = FALSE;
   }

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

   if (hwndList != NULL) {
      sprintf (Temp, "%c %02d:%02d %s", Buffer[0], timep->tm_hour, timep->tm_min, &Buffer[1]);
#if defined(__OS2__)
      WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDLOGITEM), MPFROMP (Temp));
#elif defined(__NT__)
      SendMessage (hwndList, WM_USER, WMU_ADDLOGITEM, (LPARAM)Temp);
#endif
   }
}

// ----------------------------------------------------------------------------
// Status window manager
// ----------------------------------------------------------------------------

TPMStatus::TPMStatus (HWND hwnd)
{
   hwndList = hwnd;
}

TPMStatus::~TPMStatus ()
{
}

VOID TPMStatus::Clear ()
{
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 0), MPFROMP (""));
   WinSendMsg (hwndList, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 1), MPFROMP (""));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_SETSTATUSLINE0, (LPARAM)"");
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_SETSTATUSLINE1, (LPARAM)"");
#endif
}

VOID TPMStatus::SetLine (USHORT line, PSZ text, ...)
{
   va_list arglist;
   CHAR Temp[128];

   va_start (arglist, text);
   vsprintf (Temp, text, arglist);
   va_end (arglist);

#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, line), MPFROMP (Temp));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)(WMU_SETSTATUSLINE0 + line), (LPARAM)Temp);
#endif
}

// ----------------------------------------------------------------------------
// File transfer progress indicator
// ----------------------------------------------------------------------------

class TPMProgress : public TProgress
{
public:
   TPMProgress (HWND hwnd);
   ~TPMProgress ();

   VOID   Begin ();
   VOID   End ();
   VOID   Update ();

private:
   HWND   hwndList;
};

TPMProgress::TPMProgress (HWND hwnd)
{
   hwndList = hwnd;
}

TPMProgress::~TPMProgress ()
{
}

VOID TPMProgress::Begin ()
{
   CHAR Temp[128];

   if (Type == FILE_RECEIVING)
      sprintf (Temp, "Receiving %s; %lu bytes", RxFileName, RxSize);
   else if (Type == FILE_SENDING)
      sprintf (Temp, "Sending %s; %lu bytes", TxFileName, TxSize);
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 0), MPFROMP (Temp));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_SETSTATUSLINE0, (LPARAM)Temp);
#endif
}

VOID TPMProgress::Update ()
{
   CHAR Temp[128];

   if (Type == FILE_RECEIVING)
      sprintf (Temp, "%8lu %5u", RxPosition, RxBlockSize);
   else if (Type == FILE_SENDING)
      sprintf (Temp, "%8lu %5u", TxPosition, TxBlockSize);
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 1), MPFROMP (Temp));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_SETSTATUSLINE1, (LPARAM)Temp);
#endif
}

VOID TPMProgress::End ()
{
   CHAR Temp[128];

   sprintf (Temp, "");
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 0), MPFROMP (Temp));
   WinSendMsg (hwndList, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 1), MPFROMP (Temp));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_SETSTATUSLINE0, (LPARAM)Temp);
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_SETSTATUSLINE1, (LPARAM)Temp);
#endif
}

// ----------------------------------------------------------------------------
// Mailer status
// ----------------------------------------------------------------------------

#define CENTER_LINE     70

class TPMMailStatus : public TMailerStatus
{
public:
#if defined(__OS2__) || defined(__NT__)
   TPMMailStatus (HWND hwnd);
#elif defined(__DOS__)
   TPMMailStatus ();
#endif
   ~TPMMailStatus ();

   VOID   Update ();

private:
#if defined(__OS2__) || defined(__NT__)
   HWND   hwndList;
#endif
   CHAR   Temp[256];
};

#if defined(__OS2__) || defined(__NT__)
TPMMailStatus::TPMMailStatus (HWND hwnd)
{
   hwndList = hwnd;
}
#elif defined(__DOS__)
TPMMailStatus::TPMMailStatus ()
{
}
#endif

TPMMailStatus::~TPMMailStatus ()
{
}

VOID TPMMailStatus::Update ()
{
   CHAR Line[128], Num[16];

#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_CLEAROUTBOUND), 0L);
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_CLEAROUTBOUND, 0L);
#elif defined(__DOS__)
   fill_ (13, 1, 20, 78, ' ', LGREY|_BLACK);
#endif

   sprintf (Temp, "%s, %s, %s, %s", Address, SysopName, SystemName, Location);
   if (strlen (Temp) > CENTER_LINE)
      Temp[CENTER_LINE] = '\0';
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDOUTBOUNDLINE), MPFROMP (Line));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_ADDOUTBOUNDLINE, (LPARAM)Line);
#elif defined(__DOS__)
   prints (13, 4, LGREY|_BLACK, Line);
#endif

   sprintf (Temp, "Connected at %lu baud with %s", Speed, Program);
   if (strlen (Temp) > CENTER_LINE)
      Temp[CENTER_LINE] = '\0';
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDOUTBOUNDLINE), MPFROMP (Line));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_ADDOUTBOUNDLINE, (LPARAM)Line);
#elif defined(__DOS__)
   prints (14, 4, LGREY|_BLACK, Line);
#endif

   if (Akas[0] == '\0')
      strcpy (Temp, "AKAs: No aka presented");
   else
      sprintf (Temp, "AKAs: %s", Akas);
   if (strlen (Temp) > CENTER_LINE)
      Temp[CENTER_LINE] = '\0';
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDOUTBOUNDLINE), MPFROMP (Line));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_ADDOUTBOUNDLINE, (LPARAM)Line);
#elif defined(__DOS__)
   prints (15, 4, LGREY|_BLACK, Line);
#endif

#if defined(__OS2__) || defined(__DOS__)
   strcpy (Temp, " �����MailPKT�������Data����Ŀ     �����MailPKT�������Data����Ŀ ");
#elif defined(__NT__)
   strcpy (Temp, " +----MailPKT-------Data-----+     +----MailPKT-------Data-----+ ");
#endif
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDOUTBOUNDLINE), MPFROMP (Line));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_ADDOUTBOUNDLINE, (LPARAM)Line);
#elif defined(__DOS__)
   prints (17, 4, LCYAN|_BLACK, Line);
#endif

#if defined(__OS2__) || defined(__DOS__)
   strcpy (Temp, "�������������������������������   �������������������������������");
#elif defined(__NT__)
   strcpy (Temp, "�������������������������������   �������������������������������");
#endif
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
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDOUTBOUNDLINE), MPFROMP (Line));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_ADDOUTBOUNDLINE, (LPARAM)Line);
#elif defined(__DOS__)
   prints (18, 4, LCYAN|_BLACK, Line);
#endif

#if defined(__OS2__) || defined(__DOS__)
   strcpy (Temp, "�������������������������������   �������������������������������");
#elif defined(__NT__)
   strcpy (Temp, "�������������������������������   �������������������������������");
#endif
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
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDOUTBOUNDLINE), MPFROMP (Line));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_ADDOUTBOUNDLINE, (LPARAM)Line);
#elif defined(__DOS__)
   prints (19, 4, LCYAN|_BLACK, Line);
#endif

#if defined(__OS2__) || defined(__DOS__)
   strcpy (Temp, " �������INBOUND TRAFFIC�������     ������OUTBOUND TRAFFIC������� ");
#elif defined(__NT__)
   strcpy (Temp, " +------INBOUND TRAFFIC------+     +-----OUTBOUND TRAFFIC------+ ");
#endif
   memset (Line, ' ', sizeof (Line));
   memcpy (&Line[(CENTER_LINE - strlen (Temp)) / 2], Temp, strlen (Temp) + 1);
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDOUTBOUNDLINE), MPFROMP (Line));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_ADDOUTBOUNDLINE, (LPARAM)Line);
#elif defined(__DOS__)
   prints (20, 4, LCYAN|_BLACK, Line);
#endif
}

#if defined(__OS2__) || defined(__NT__)

// ----------------------------------------------------------------------
// Ask a FidoNet address
// ----------------------------------------------------------------------

class CAskAddressDlg : public CDialog
{
public:
   CAskAddressDlg (HWND p_hWnd);

   CHAR   Title[128];
   CHAR   String[128];

   USHORT OnInitDialog ();
   VOID   OnOK ();
};

CAskAddressDlg::CAskAddressDlg (HWND p_hWnd) : CDialog ("26", p_hWnd)
{
}

USHORT CAskAddressDlg::OnInitDialog ()
{
   Center ();

   SetDlgItemText (101, "Address:");
   SetWindowTitle (Title);
   EM_SetTextLimit (128, sizeof (String) - 1);

   return (TRUE);
}

VOID CAskAddressDlg::OnOK ()
{
   GetDlgItemText (128, GetDlgItemTextLength (128), String);
   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Forced poll
// ----------------------------------------------------------------------

class CPollDlg : public CDialog
{
public:
   CPollDlg (HWND p_hWnd);

   CHAR   Address[128];

   USHORT OnInitDialog ();
   VOID   OnOK ();
};

CPollDlg::CPollDlg (HWND p_hWnd) : CDialog ("13", p_hWnd)
{
}

USHORT CPollDlg::OnInitDialog ()
{
   CHAR Temp[128];
   class CAskAddressDlg *Dlg;
   class TNodes *Nodes;
   class TAddress Addr;

   Center ();

   if ((Dlg = new CAskAddressDlg (m_hWnd)) != NULL) {
      strcpy (Dlg->Title, "Forced poll");
      if (Dlg->DoModal () == FALSE)
         EndDialog (FALSE);
      else {
         if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
            Addr.Parse (Dlg->String);
            if (Cfg->MailAddress.First () == TRUE) {
               if (Addr.Zone == 0)
                  Addr.Zone = Cfg->MailAddress.Zone;
               if (Addr.Net == 0)
                  Addr.Net = Cfg->MailAddress.Net;
               Addr.Add ();
               Addr.First ();
            }
            strcpy (Address, Addr.String);
            if (Nodes->Read (Addr) == TRUE) {
               SetDlgItemText (102, Nodes->SystemName);
               SetDlgItemText (103, Nodes->Address);
               SetDlgItemText (109, Nodes->Location);
               SetDlgItemText (104, Nodes->SysopName);
               BM_SetCheck (105, TRUE);
            }
            else {
               sprintf (Temp, "Node %s not found !", Address);
               MessageBox (Temp, "Forced poll", MB_OK);
               EndDialog (FALSE);
            }
            delete Nodes;
         }
      }
      delete Dlg;
   }

   return (TRUE);
}

VOID CPollDlg::OnOK ()
{
   CHAR Flag;

   Flag = 'f';
   if (BM_QueryCheck (105) == TRUE)
      Flag = 'i';
   else if (BM_QueryCheck (106) == TRUE)
      Flag = 'c';
   else if (BM_QueryCheck (107) == TRUE)
      Flag = 'd';

   if (Outbound != NULL) {
      Outbound->PollNode (Address, Flag);

      if (Log != NULL)
         Log->Write ("+Building the outbound queue");
      Outbound->BuildQueue (Cfg->Outbound);
      unlink ("rescan.now");
      if (Log != NULL)
         Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);

#if defined(__OS2__)
      WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
      PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// File request
// ----------------------------------------------------------------------

class CRequestDlg : public CDialog
{
public:
   CRequestDlg (HWND p_hWnd);

   CHAR   Address[128];

   USHORT OnInitDialog ();
   VOID   OnOK ();
};

CRequestDlg::CRequestDlg (HWND p_hWnd) : CDialog ("32", p_hWnd)
{
}

USHORT CRequestDlg::OnInitDialog ()
{
   CHAR Temp[128];
   class CAskAddressDlg *Dlg;
   class TNodes *Nodes;
   class TAddress Addr;

   Center ();

   if ((Dlg = new CAskAddressDlg (m_hWnd)) != NULL) {
      strcpy (Dlg->Title, "File request");
      if (Dlg->DoModal () == FALSE)
         EndDialog (FALSE);
      else {
         if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
            Addr.Parse (Dlg->String);
            if (Cfg->MailAddress.First () == TRUE) {
               if (Addr.Zone == 0)
                  Addr.Zone = Cfg->MailAddress.Zone;
               if (Addr.Net == 0)
                  Addr.Net = Cfg->MailAddress.Net;
               Addr.Add ();
               Addr.First ();
            }
            strcpy (Address, Addr.String);
            if (Nodes->Read (Addr) == TRUE) {
               SetDlgItemText (102, Nodes->SystemName);
               SetDlgItemText (103, Nodes->Address);
               SetDlgItemText (109, Nodes->Location);
               SetDlgItemText (104, Nodes->SysopName);
               BM_SetCheck (105, TRUE);
            }
            else {
               sprintf (Temp, "Node %s not found !", Address);
               MessageBox (Temp, "File request", MB_OK);
               EndDialog (FALSE);
            }
            delete Nodes;
         }
      }
      delete Dlg;
   }

   return (TRUE);
}

VOID CRequestDlg::OnOK ()
{
   FILE *fp;
   CHAR File[128], Temp[256], *p, Flag;
   class TAddress Addr;

   Flag = 'f';
   if (BM_QueryCheck (105) == TRUE)
      Flag = 'i';
   else if (BM_QueryCheck (106) == TRUE)
      Flag = 'c';
   else if (BM_QueryCheck (107) == TRUE)
      Flag = 'd';

   Addr.Parse (Address);

   Cfg->MailAddress.First ();
   strcpy (Temp, Cfg->Outbound);
   Temp[strlen (Temp) - 1] = '\0';

   if (Cfg->MailAddress.Zone != Addr.Zone) {
      sprintf (File, "%s.%03x", Temp, Addr.Zone);
      mkdir (File);
      if (Addr.Point != 0) {
         sprintf (File, "%s.%03x\\%04x%04x.pnt", Temp, Addr.Zone, Addr.Net, Addr.Node);
         mkdir (File);
         sprintf (File, "%s.%03x\\%04x%04x.pnt\\%08x.req", Temp, Addr.Zone, Addr.Net, Addr.Node, Addr.Point);
      }
      else
         sprintf (File, "%s.%03x\\%04x%04x.req", Temp, Addr.Zone, Addr.Net, Addr.Node);
   }
   else {
      if (Addr.Point != 0) {
         sprintf (File, "%s\\%04x%04x.pnt", Temp, Addr.Net, Addr.Node);
         mkdir (File);
         sprintf (File, "%s\\%04x%04x.pnt\\%08x.req", Temp, Addr.Net, Addr.Node, Addr.Point);
      }
      else
         sprintf (File, "%s\\%04x%04x.req", Temp, Addr.Net, Addr.Node);
   }

   if ((fp = fopen (File, "at")) != NULL) {
      GetDlgItemText (112, GetDlgItemTextLength (112), Temp);
      if ((p = strtok (Temp, " ")) != NULL)
         do {
            fprintf (fp, "%s\n", p);
         } while ((p = strtok (NULL, " ")) != NULL);
      fclose (fp);
   }

   if (Outbound != NULL) {
      Outbound->PollNode (Address, Flag);

      if (Log != NULL)
         Log->Write ("+Building the outbound queue");
      Outbound->BuildQueue (Cfg->Outbound);
      unlink ("rescan.now");
      if (Log != NULL)
         Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);

#if defined(__OS2__)
      WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
      PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// New ECHOmaill Link
// ----------------------------------------------------------------------

class CNewEchoLinkDlg : public CDialog
{
public:
   CNewEchoLinkDlg (HWND p_hWnd);

   CHAR   Address[128];

   USHORT OnInitDialog ();
   VOID   OnOK ();

private:
   CHAR   Command[512];
};

CNewEchoLinkDlg::CNewEchoLinkDlg (HWND p_hWnd) : CDialog ("52", p_hWnd)
{
}

USHORT CNewEchoLinkDlg::OnInitDialog ()
{
   CHAR Temp[128];
   class CAskAddressDlg *Dlg;
   class TNodes *Nodes;
   class TAddress Addr;

   Center ();

   EM_SetTextLimit (112, sizeof (Command) - 1);

   if ((Dlg = new CAskAddressDlg (m_hWnd)) != NULL) {
      strcpy (Dlg->Title, "New Echomail Link");
      if (Dlg->DoModal () == FALSE)
         EndDialog (FALSE);
      else {
         if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
            Addr.Parse (Dlg->String);
            if (Cfg->MailAddress.First () == TRUE) {
               if (Addr.Zone == 0)
                  Addr.Zone = Cfg->MailAddress.Zone;
               if (Addr.Net == 0)
                  Addr.Net = Cfg->MailAddress.Net;
               Addr.Add ();
               Addr.First ();
            }
            strcpy (Address, Addr.String);
            if (Nodes->Read (Addr) == TRUE) {
               SetDlgItemText (102, Nodes->SystemName);
               SetDlgItemText (103, Nodes->Address);
               SetDlgItemText (109, Nodes->Location);
               SetDlgItemText (104, Nodes->SysopName);
               strcpy (Address, Nodes->Address);
            }
            else {
               sprintf (Temp, "Node %s not found !", Address);
               MessageBox (Temp, "New Echomail Link", MB_OK);
               EndDialog (FALSE);
            }
            delete Nodes;
         }
      }
      delete Dlg;
   }

   return (TRUE);
}

VOID CNewEchoLinkDlg::OnOK ()
{
   CHAR *p, *t;
   class TAddress Addr;
   class TAreaManager *AreaMgr;

   if ((AreaMgr = new TAreaManager) != NULL) {
      AreaMgr->Cfg = Cfg;
      AreaMgr->Log = Log;

      GetDlgItemText (112, GetDlgItemTextLength (112), Command);
      t = Command;

      while ((p = strtok (t, " ")) != NULL) {
         t = strtok (NULL, "");
         if (!stricmp (p, "%-ALL"))
            AreaMgr->RemoveAll (Address);
         else if (*p == '-')
            AreaMgr->RemoveArea (Address, ++p);
         else {
            if (*p == '+')
               p++;
            AreaMgr->AddArea (Address, p);
         }
      }

      delete AreaMgr;
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Request ECHOlink
// ----------------------------------------------------------------------

class CRequestEchoLinkDlg : public CDialog
{
public:
   CRequestEchoLinkDlg (HWND p_hWnd);

   CHAR   Address[128];

   USHORT OnInitDialog ();
   VOID   OnOK ();

private:
   CHAR   Command[512];
};

CRequestEchoLinkDlg::CRequestEchoLinkDlg (HWND p_hWnd) : CDialog ("52", p_hWnd)
{
}

USHORT CRequestEchoLinkDlg::OnInitDialog ()
{
   CHAR Temp[128];
   class CAskAddressDlg *Dlg;
   class TNodes *Nodes;
   class TAddress Addr;

   Center ();

   EM_SetTextLimit (112, sizeof (Command) - 1);

   if ((Dlg = new CAskAddressDlg (m_hWnd)) != NULL) {
      strcpy (Dlg->Title, "Request Echomail Link");
      if (Dlg->DoModal () == FALSE)
         EndDialog (FALSE);
      else {
         if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
            Addr.Parse (Dlg->String);
            if (Cfg->MailAddress.First () == TRUE) {
               if (Addr.Zone == 0)
                  Addr.Zone = Cfg->MailAddress.Zone;
               if (Addr.Net == 0)
                  Addr.Net = Cfg->MailAddress.Net;
               Addr.Add ();
               Addr.First ();
            }
            strcpy (Address, Addr.String);
            if (Nodes->Read (Addr) == TRUE) {
               SetDlgItemText (102, Nodes->SystemName);
               SetDlgItemText (103, Nodes->Address);
               SetDlgItemText (109, Nodes->Location);
               SetDlgItemText (104, Nodes->SysopName);
               strcpy (Address, Nodes->Address);
            }
            else {
               sprintf (Temp, "Node %s not found !", Address);
               MessageBox (Temp, "Request Echomail Link", MB_OK);
               EndDialog (FALSE);
            }
            delete Nodes;
         }
      }
      delete Dlg;
   }

   return (TRUE);
}

VOID CRequestEchoLinkDlg::OnOK ()
{
   CHAR *p;
   struct dosdate_t d_date;
   struct dostime_t d_time;
   class TAddress Addr;
   class TNodes *Nodes;
   class TMsgBase *Msg;

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      Nodes->Read (Address);
      switch (Cfg->NetMailStorage) {
         case ST_JAM:
            Msg = new JAM (Cfg->NetMailPath);
            break;
         case ST_SQUISH:
            Msg = new SQUISH (Cfg->NetMailPath);
            break;
         case ST_FIDO:
            Msg = new FIDOSDM (Cfg->NetMailPath);
            break;
         case ST_ADEPT:
            Msg = new ADEPT (Cfg->NetMailPath);
            break;
         case ST_HUDSON:
            Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->NetMailBoard);
            break;
         default:
            Msg = NULL;
            break;
      }
      if (Msg != NULL) {
         Msg->New ();
         Cfg->MailAddress.First ();

         strcpy (Msg->From, Cfg->SysopName);
         strcpy (Msg->FromAddress, Cfg->MailAddress.String);
         strcpy (Msg->To, "Areafix");
         strcpy (Msg->ToAddress, Address);
         strcpy (Msg->Subject, Nodes->AreaMgrPwd);

         _dos_getdate (&d_date);
         _dos_gettime (&d_time);

         Msg->Arrived.Day = Msg->Written.Day = d_date.day;
         Msg->Arrived.Month = Msg->Written.Month = d_date.month;
         Msg->Arrived.Year = Msg->Written.Year = (USHORT)d_date.year;
         Msg->Arrived.Hour = Msg->Written.Hour = d_time.hour;
         Msg->Arrived.Minute = Msg->Written.Minute = d_time.minute;
         Msg->Arrived.Second = Msg->Written.Second = d_time.second;

         GetDlgItemText (112, GetDlgItemTextLength (112), Command);
         if ((p = strtok (Command, " ")) != NULL)
            do {
               Msg->Text.Add (p);
            } while ((p = strtok (NULL, " ")) != NULL);

         Msg->Text.Add ("---");
         Msg->Add ();

         delete Msg;
      }
      delete Nodes;
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Rescan ECHOmail
// ----------------------------------------------------------------------

class CRescanDlg : public CDialog
{
public:
   CRescanDlg (HWND p_hWnd);

   CHAR   Address[128];

   USHORT OnInitDialog ();
   VOID   OnOK ();

private:
   CHAR   Command[512];
};

CRescanDlg::CRescanDlg (HWND p_hWnd) : CDialog ("52", p_hWnd)
{
}

USHORT CRescanDlg::OnInitDialog ()
{
   CHAR Temp[128];
   class CAskAddressDlg *Dlg;
   class TNodes *Nodes;
   class TAddress Addr;

   SetWindowTitle ("Rescan Area(s)");
   Center ();

   EM_SetTextLimit (112, sizeof (Command) - 1);

   if ((Dlg = new CAskAddressDlg (m_hWnd)) != NULL) {
      strcpy (Dlg->Title, "Rescan EchoMail");
      if (Dlg->DoModal () == FALSE)
         EndDialog (FALSE);
      else {
         if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
            Addr.Parse (Dlg->String);
            if (Cfg->MailAddress.First () == TRUE) {
               if (Addr.Zone == 0)
                  Addr.Zone = Cfg->MailAddress.Zone;
               if (Addr.Net == 0)
                  Addr.Net = Cfg->MailAddress.Net;
               Addr.Add ();
               Addr.First ();
            }
            strcpy (Address, Addr.String);
            if (Nodes->Read (Addr) == TRUE) {
               SetDlgItemText (102, Nodes->SystemName);
               SetDlgItemText (103, Nodes->Address);
               SetDlgItemText (109, Nodes->Location);
               SetDlgItemText (104, Nodes->SysopName);
               strcpy (Address, Nodes->Address);
            }
            else {
               sprintf (Temp, "Node %s not found !", Address);
               MessageBox (Temp, "File request", MB_OK);
               EndDialog (FALSE);
            }
            delete Nodes;
         }
      }
      delete Dlg;
   }

   return (TRUE);
}

VOID CRescanDlg::OnOK ()
{
   CHAR *p, *t;
   class TAddress Addr;
   class TAreaManager *AreaMgr;

   if ((AreaMgr = new TAreaManager) != NULL) {
      AreaMgr->Cfg = Cfg;
      AreaMgr->Log = Log;

      GetDlgItemText (112, GetDlgItemTextLength (112), Command);
      t = Command;

      while ((p = strtok (t, " ")) != NULL) {
         t = strtok (NULL, "");
         AreaMgr->Rescan (p, Address);
      }

      delete AreaMgr;
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// File attach
// ----------------------------------------------------------------------

class CAttachDlg : public CDialog
{
public:
   CAttachDlg (HWND p_hWnd);

   CHAR   Address[128];

   USHORT OnInitDialog ();
   VOID   OnOK ();

private:
   CHAR   Temp[4096];
   DECLARE_MESSAGE_MAP ()

   VOID   Browse ();
};

BEGIN_MESSAGE_MAP (CAttachDlg, CDialog)
   ON_COMMAND (113, Browse)
END_MESSAGE_MAP ()

CAttachDlg::CAttachDlg (HWND p_hWnd) : CDialog ("49", p_hWnd)
{
}

USHORT CAttachDlg::OnInitDialog ()
{
   class CAskAddressDlg *Dlg;
   class TNodes *Nodes;
   class TAddress Addr;

   Center ();

   EM_SetTextLimit (112, sizeof (Temp) - 1);

   if ((Dlg = new CAskAddressDlg (m_hWnd)) != NULL) {
      strcpy (Dlg->Title, "File attach");
      if (Dlg->DoModal () == FALSE)
         EndDialog (FALSE);
      else {
         if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
            Addr.Parse (Dlg->String);
            if (Cfg->MailAddress.First () == TRUE) {
               if (Addr.Zone == 0)
                  Addr.Zone = Cfg->MailAddress.Zone;
               if (Addr.Net == 0)
                  Addr.Net = Cfg->MailAddress.Net;
               Addr.Add ();
               Addr.First ();
            }
            strcpy (Address, Addr.String);
            if (Nodes->Read (Addr) == TRUE) {
               SetDlgItemText (102, Nodes->SystemName);
               SetDlgItemText (103, Nodes->Address);
               SetDlgItemText (109, Nodes->Location);
               SetDlgItemText (104, Nodes->SysopName);
               BM_SetCheck (105, TRUE);
            }
            else {
               sprintf (Temp, "Node %s not found !", Address);
               MessageBox (Temp, "File attach", MB_OK);
               EndDialog (FALSE);
            }
            delete Nodes;
         }
      }
      delete Dlg;
   }

   return (TRUE);
}

VOID CAttachDlg::Browse ()
{
#if defined(__OS2__)
   ULONG i;
   FILEDLG fild;

   Temp[0] = '\0';

   memset (&fild, 0, sizeof (FILEDLG));
   fild.cbSize = sizeof (FILEDLG);
   fild.fl = FDS_CENTER|FDS_OPEN_DIALOG|FDS_MULTIPLESEL;
   fild.pszTitle = "Attach files";
   sprintf (fild.szFullFile, "*.*");

   WinFileDlg (HWND_DESKTOP, m_hWnd, &fild);
   if (fild.lReturn == DID_OK) {
      if (fild.papszFQFilename != NULL) {
         for (i = 0; i < fild.ulFQFCount; i++) {
            if (i != 0)
               strcat (Temp, " ");
            strcat (Temp, fild.papszFQFilename[i][0]);
         }
      }
      else
         strcpy (Temp, fild.szFullFile);
   }
   if (fild.papszFQFilename != NULL)
      WinFreeFileDlgList (fild.papszFQFilename);
   SetDlgItemText (112, Temp);

#elif defined(__NT__)
   CHAR Path[256], File[512], *p;
   OPENFILENAME OpenFileName;

   File[0] = '\0';
   OpenFileName.lStructSize = sizeof (OPENFILENAME);
   OpenFileName.hwndOwner = m_hWnd;
   OpenFileName.hInstance = NULL;
   OpenFileName.lpstrFilter = "All files (*.*)\0*.*";
   OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
   OpenFileName.nMaxCustFilter = 0L;
   OpenFileName.nFilterIndex = 1L;
   OpenFileName.lpstrFile = File;
   OpenFileName.nMaxFile = sizeof (File) - 1;
   OpenFileName.lpstrFileTitle = NULL;
   OpenFileName.nMaxFileTitle = 0;
   OpenFileName.lpstrInitialDir = NULL;
   OpenFileName.lpstrTitle = "Attach Files";
   OpenFileName.nFileOffset = 0;
   OpenFileName.nFileExtension = 0;
   OpenFileName.lpstrDefExt = "";
   OpenFileName.lCustData = 0;
   OpenFileName.Flags = OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_CREATEPROMPT|OFN_NOCHANGEDIR|OFN_ALLOWMULTISELECT;

   if (GetOpenFileName (&OpenFileName) == TRUE) {
      if ((p = strtok (File, " ")) != NULL) {
         strcpy (Path, p);
         if (Path[strlen (Path) - 1] != '\\')
            strcat (Path, "\\");
         Temp[0] = '\0';
         while ((p = strtok (NULL, " ")) != NULL) {
            if (Temp[0] != '\0')
               strcat (Temp, " ");
            strcat (Temp, Path);
            strcat (Temp, p);
         }
      }
      SetDlgItemText (112, Temp);
   }
#endif
}

VOID CAttachDlg::OnOK ()
{
   FILE *fp;
   CHAR File[128], *p, Flag;
   class TAddress Addr;

   Flag = 'f';
   if (BM_QueryCheck (105) == TRUE)
      Flag = 'h';
   else if (BM_QueryCheck (106) == TRUE)
      Flag = 'c';
   else if (BM_QueryCheck (107) == TRUE)
      Flag = 'd';

   Addr.Parse (Address);

   Cfg->MailAddress.First ();
   strcpy (Temp, Cfg->Outbound);
   Temp[strlen (Temp) - 1] = '\0';

   if (Cfg->MailAddress.Zone != Addr.Zone) {
      sprintf (File, "%s.%03x", Temp, Addr.Zone);
      mkdir (File);
      if (Addr.Point != 0) {
         sprintf (File, "%s.%03x\\%04x%04x.pnt", Temp, Addr.Zone, Addr.Net, Addr.Node);
         mkdir (File);
         sprintf (File, "%s.%03x\\%04x%04x.pnt\\%08x.%clo", Temp, Addr.Zone, Addr.Net, Addr.Node, Addr.Point, Flag);
      }
      else
         sprintf (File, "%s.%03x\\%04x%04x.%clo", Temp, Addr.Zone, Addr.Net, Addr.Node, Flag);
   }
   else {
      if (Addr.Point != 0) {
         sprintf (File, "%s\\%04x%04x.pnt", Temp, Addr.Net, Addr.Node);
         mkdir (File);
         sprintf (File, "%s\\%04x%04x.pnt\\%08x.%clo", Temp, Addr.Net, Addr.Node, Addr.Point, Flag);
      }
      else
         sprintf (File, "%s\\%04x%04x.%clo", Temp, Addr.Net, Addr.Node, Flag);
   }

   if ((fp = fopen (File, "at")) != NULL) {
      GetDlgItemText (112, GetDlgItemTextLength (112), Temp);
      if ((p = strtok (Temp, " ")) != NULL)
         do {
            fprintf (fp, "%s\n", p);
         } while ((p = strtok (NULL, " ")) != NULL);
      fclose (fp);
   }

   if (Outbound != NULL) {
      if (Log != NULL)
         Log->Write ("+Building the outbound queue");
      Outbound->BuildQueue (Cfg->Outbound);
      unlink ("rescan.now");
      if (Log != NULL)
         Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);

#if defined(__OS2__)
      WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
      PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
   }

   EndDialog (TRUE);
}

#endif

// ----------------------------------------------------------------------------
// Threads
// ----------------------------------------------------------------------------

VOID NodelistThread (PVOID args)
{
   USHORT OldStatus;

#if defined(__OS2__)
   HAB hab;
   HMQ hmq;
#endif

#if defined(__OS2__)
   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);
   WinCancelShutdown (hmq, TRUE);
#endif

#if defined(__OS2__)
   DosExitCritSec ();
#endif

   OldStatus = Status;
   Status = UNDEFINED;

   CompileNodelist ((USHORT)args);

   Status = OldStatus;

#if defined(__OS2__)
   if (hmq != NULL)
      WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
#endif

   StartTimer (hwndMainClient, 1, MODEM_DELAY);

#if defined(__OS2__) || defined(__NT__)
   _endthread ();
#endif
}

VOID MailProcessorThread (PVOID Args)
{
#if defined(__OS2__)
   HAB hab;
   HMQ hmq;
#endif
   USHORT OldStatus;
   ULONG Actions = (ULONG)Args;
   CHAR Temp[128], *tag;
   class TMailProcessor *Processor;
   class TTicProcessor *Tic;

   Args = Args;
#if defined(__OS2__)
   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);
   WinCancelShutdown (hmq, TRUE);
#endif

#if defined(__OS2__)
   DosExitCritSec ();
//   DosSetPriority ((USHORT)1, (USHORT)0, (SHORT)0, (USHORT)0);
#endif

   OldStatus = Status;
   Status = UNDEFINED;

   if (Actions & MAIL_TIC) {
      if ((Tic = new TTicProcessor) != NULL) {
         Tic->Cfg = Cfg;
         Tic->Log = Log;
         Tic->Output = new TPMList (hwndMainClient);
         if (Tic->Output != NULL)
            Tic->Output->Clear ();
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
      Processor->Output = new TPMList (hwndMainClient);
      Processor->Status = new TPMStatus (hwndMainClient);

      if (Actions & MAIL_IMPORTBAD)
         Processor->ImportBad ();

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
         Log->Write ("+%lu message(s): %lu Sent, %lu Received", Processor->MsgSent + Processor->MsgTossed, Processor->MsgSent, Processor->MsgTossed);
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
            tag = NULL;
            if (Events != NULL)
               tag = Events->RouteCmd;
            Processor->Pack (Temp, tag);
            if (Processor->DoRescan () == TRUE)
               Processor->Pack (Temp, tag);
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
            tag = NULL;
            if (Events != NULL)
               tag = Events->RouteCmd;
            Processor->Pack (Temp, tag);
            if (Processor->DoRescan () == TRUE)
               Processor->Pack (Temp, tag);
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

#if defined(__OS2__)
      WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
      PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif

      if (Actions & MAIL_STARTTIMER)
         StartTimer (hwndMainClient, 1, MODEM_DELAY);
   }
#if defined(__OS2__)
   else
      WinPostMsg (hwndMainClient, WM_CLOSE, 0L, 0L);
#elif defined(__NT__)
   else
      PostMessage (hwndMainClient, WM_CLOSE, 0, 0L);
#endif

#if defined(__OS2__)
   if (hmq != NULL)
      WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
#endif

   Status = OldStatus;

#if defined(__OS2__) || defined(__NT__)
   _endthread ();
#endif
}

#if !defined(__POINT__)

VOID BbsThread (PVOID Args)
{
   USHORT Remote;
   CHAR Title[64];
   ULONG Flags;
#if defined(__OS2__)
   CHAR ObjBuf[64], PipeName[64], CtlName[64], Temp[128];
   ULONG id;
   STARTDATA StartData;
   PID Pid;
   HAB hab;
   HMQ hmq;
   class TPipe *Pipe = NULL;
#elif defined(__NT__)
   class TScreen *Screen;
#endif
   class TBbs *Bbs;

   Args = Args;
   Remote = REMOTE_NONE;
#if defined(__OS2__)
   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);
   WinCancelShutdown (hmq, TRUE);
#endif

#if defined(__OS2__)
   DosExitCritSec ();
//   DosSetPriority ((USHORT)1, (USHORT)0, (SHORT)31, (USHORT)0);
#endif

   sprintf (Title, "Snoop - Line %u", Cfg->TaskNumber);

   if ((Bbs = new TBbs) != NULL) {
#if defined(__OS2__)
      if ((Pipe = new TPipe) != NULL) {
         sprintf (PipeName, "\\PIPE\\SNOOP%u", Cfg->TaskNumber);
         sprintf (CtlName, "\\PIPE\\CTL%u", Cfg->TaskNumber);

         if (Pipe->Initialize (PipeName, CtlName, 1) == TRUE) {
            Pipe->WaitClient ();
            Bbs->Snoop = Pipe;
         }

         memset (&StartData, 0, sizeof (STARTDATA));
         StartData.Length = sizeof (STARTDATA);
         StartData.Related = SSF_RELATED_CHILD;
         StartData.FgBg = SSF_FGBG_BACK;
         StartData.TraceOpt = SSF_TRACEOPT_NONE;
         StartData.PgmTitle = Title;
         StartData.PgmName = "SNOOP.EXE";
         sprintf (Temp, "%s %s", PipeName, CtlName);
         StartData.PgmInputs = Temp;
         StartData.TermQ = NULL;
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
         DosStartSession (&StartData, &id, &Pid);

         while (Pipe->WaitClient () == FALSE)
            DosSleep (1L);
      }
#elif defined(__NT__)
      if ((Screen = new TScreen) != NULL) {
         if (Screen->Initialize () == TRUE) {
            SetConsoleTitle (Title);
            SetForegroundWindow (hwndMainClient);
            Bbs->Snoop = Screen;
         }
      }
#endif

      Bbs->Log = Log;
      Bbs->Cfg = Cfg;
      Bbs->Events = Events;
      Bbs->Progress = new TPMProgress (hwndMainClient);
      Bbs->MailerStatus = new TPMMailStatus (hwndMainClient);
      Bbs->Status = new TPMStatus (hwndMainClient);
      if (Modem != NULL) {
         Bbs->Com = Modem->Serial;
         Bbs->Speed = Modem->Speed;
         if (connectSpeed != 0L)
            Bbs->Speed = connectSpeed;
      }
      Bbs->Task = Cfg->TaskNumber;
      Bbs->TimeLimit = timeLimit;
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

#if defined(__OS2__)
      if (Pipe != NULL) {
         DosStopSession (STOP_SESSION_SPECIFIED, id);
         delete Pipe;
      }
#elif defined(__NT__)
      if (Screen != NULL)
         delete Screen;
#endif

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

#if defined(__OS2__)
      WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
      PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
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
      if (Events->ProcessTIC == TRUE)
         Flags |= MAIL_TIC;
#if defined(__OS2__)         
      _beginthread (MailProcessorThread, NULL, 8192, (PVOID)Flags);
#elif defined(__NT__)
      _beginthread (MailProcessorThread, NULL, (PVOID)Flags);
#endif      
   }
   else
      StartTimer (hwndMainClient, 1, MODEM_DELAY);

#if defined(__OS2__)
   if (hmq != NULL)
      WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
#endif

#if defined(__OS2__) || defined(__NT__)
   _endthread ();
#endif
}

VOID LocalThread (PVOID Args)
{
   CHAR Title[64];
#if defined(__OS2__)
   CHAR ObjBuf[64], PipeName[64], CtlName[64], Temp[128];
   ULONG id;
   STARTDATA StartData;
   PID Pid;
   HAB hab;
   HMQ hmq;
   class TPipe *Pipe = NULL;
#elif defined(__NT__)
   class TScreen *Screen;
#endif
   class TBbs *Bbs;

   Args = Args;
#if defined(__OS2__)
   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);
   WinCancelShutdown (hmq, TRUE);
#endif

#if defined(__OS2__)
   DosExitCritSec ();
//   DosSetPriority ((USHORT)1, (USHORT)0, (SHORT)31, (USHORT)0);
#endif

   sprintf (Title, "Snoop - Line %u", Cfg->TaskNumber);

   if ((Bbs = new TBbs) != NULL) {
#if defined(__OS2__)
      if ((Pipe = new TPipe) != NULL) {
         sprintf (PipeName, "\\PIPE\\SNOOP%u", Cfg->TaskNumber);
         sprintf (CtlName, "\\PIPE\\CTL%u", Cfg->TaskNumber);

         if (Pipe->Initialize (PipeName, CtlName, 1) == TRUE) {
            Pipe->WaitClient ();
            Bbs->Com = Pipe;
         }

         memset (&StartData, 0, sizeof (STARTDATA));
         StartData.Length = sizeof (STARTDATA);
         StartData.Related = SSF_RELATED_CHILD;
         StartData.FgBg = SSF_FGBG_FORE;
         StartData.TraceOpt = SSF_TRACEOPT_NONE;
         StartData.PgmTitle = Title;
         StartData.PgmName = "SNOOP.EXE";
         sprintf (Temp, "%s %s", PipeName, CtlName);
         StartData.PgmInputs = Temp;
         StartData.TermQ = NULL;
         StartData.Environment = 0;
         StartData.InheritOpt = SSF_INHERTOPT_PARENT;
         StartData.SessionType = SSF_TYPE_WINDOWABLEVIO;
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
         DosStartSession (&StartData, &id, &Pid);

         while (Pipe->WaitClient () == FALSE)
            DosSleep (1L);
      }
#elif defined(__NT__)
      if ((Screen = new TScreen) != NULL) {
         if (Screen->Initialize () == TRUE) {
            SetConsoleTitle (Title);
            Bbs->Com = Screen;
         }
      }
#endif

      if (Log != NULL)
         Log->Write ("+Connect Local");
      Bbs->Log = Log;
      Bbs->Cfg = Cfg;
      Bbs->Events = Events;
      Bbs->Progress = new TPMProgress (hwndMainClient);
      Bbs->Status = new TPMStatus (hwndMainClient);
      Bbs->Speed = 57600L;
      Bbs->Task = Cfg->TaskNumber;
      Bbs->TimeLimit = timeLimit;
      Bbs->Local = TRUE;
      Bbs->Run ();
      if (Bbs->Status != NULL) {
         Bbs->Status->Clear ();
         delete Bbs->Status;
      }
      if (Bbs->Progress != NULL)
         delete Bbs->Progress;

#if defined(__OS2__)
      if (Pipe != NULL) {
         DosStopSession (STOP_SESSION_SPECIFIED, id);
         delete Pipe;
      }
#elif defined(__NT__)
      if (Screen != NULL)
         delete Screen;
#endif

      delete Bbs;
   }

   Modem->SendCommand (Cfg->Hangup);
   if (Modem->Serial != NULL && Log != NULL) {
      if (Modem->Serial->Carrier () == TRUE)
         Log->Write ("!Unable to drop carrier");
   }

   if (Log != NULL) {
      Log->Display = TRUE;
      Log->WriteBlank ();
   }

   StartTimer (hwndMainClient, 1, MODEM_DELAY);

#if defined(__OS2__)
   if (hmq != NULL)
      WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
#endif

#if defined(__OS2__) || defined(__NT__)
   _endthread ();
#endif
}

#endif

VOID MailerThread (PVOID Args)
{
   USHORT RetVal = FALSE;
   ULONG Flags;
#if defined(__OS2__)
   HAB hab;
   HMQ hmq;
#endif
   class TDetect *Detect;

   Args = Args;
#if defined(__OS2__)
   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);
   WinCancelShutdown (hmq, TRUE);
#endif

#if defined(__OS2__)
   DosExitCritSec ();
//   DosSetPriority ((USHORT)1, (USHORT)0, (SHORT)31, (USHORT)0);
#endif

   if ((Detect = new TDetect) != NULL) {
      Detect->Task = Cfg->TaskNumber;
      Detect->Log = Log;
      Detect->Cfg = Cfg;
      Detect->Events = Events;
      Detect->Progress = new TPMProgress (hwndMainClient);
      Detect->MailerStatus = new TPMMailStatus (hwndMainClient);
      Detect->Status = new TPMStatus (hwndMainClient);
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
#if defined(__OS2__)
      WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
      PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
   }

   if (Log != NULL)
      Log->WriteBlank ();

   Flags = MAIL_STARTTIMER;
   if (RetVal == REMOTE_MAILRECEIVED && Events != NULL) {
      if (Events->ImportNormal == TRUE)
         Flags |= MAIL_IMPORTNORMAL;
      if (Events->ImportKnown == TRUE)
         Flags |= MAIL_IMPORTKNOWN;
      if (Events->ImportProtected == TRUE)
         Flags |= MAIL_IMPORTPROTECTED;
      if (Events->ExportMail == TRUE)
         Flags |= (MAIL_EXPORT|MAIL_PACK);
      if (Events->ProcessTIC == TRUE)
         Flags |= MAIL_TIC;
   }
   if (Flags != MAIL_STARTTIMER)
#if defined(__OS2__)   
      _beginthread (MailProcessorThread, NULL, 8192, (PVOID)Flags);
#elif defined(__NT__)
      _beginthread (MailProcessorThread, NULL, (PVOID)Flags);
#endif      
   else
      StartTimer (hwndMainClient, 1, MODEM_DELAY);

#if defined(__OS2__)
   if (hmq != NULL)
      WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
#endif

#if defined(__OS2__) || defined(__NT__)
   _endthread ();
#endif
}

#if !defined(__POINT__)

VOID FaxReceiveThread (PVOID Args)
{
   CHAR Temp[128];
#if defined(__OS2__)
   HAB hab;
   HMQ hmq;
#endif
   class TFax *Fax;

   Args = Args;
#if defined(__OS2__)
   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);
   WinCancelShutdown (hmq, TRUE);
#endif

#if defined(__OS2__)
   DosExitCritSec ();
//   DosSetPriority ((USHORT)1, (USHORT)0, (SHORT)31, (USHORT)0);
#endif

   if (Cfg->ExternalFax == TRUE && Cfg->FaxCommand[0] != '\0') {
#if !defined(__DOS__)
      sprintf (Temp, Cfg->FaxCommand, atoi (&Cfg->Device[3]), Cfg->Speed, Modem->Serial->hFile);
#else
      sprintf (Temp, Cfg->FaxCommand, atoi (&Cfg->Device[3]), Cfg->Speed, atoi (Cfg->Device) - 1);
#endif

      Log->Write ("+Spawning to %s", Temp);
      Log->Suspend ();
      RunExternal (Temp);
      Log->Resume ();
      Log->Write (":Returned from %s", Temp);
   }
   else {
      if ((Fax = new TFax) != NULL) {
         strcpy (Fax->DataPath, Cfg->FaxPath);
         Fax->Format = Cfg->FaxFormat;
         Fax->Com = Modem->Serial;
         Fax->Log = Log;
         if (Fax->faxreceive () == TRUE && Cfg->AfterFaxCmd[0] != '\0')
            SpawnExternal (Cfg->AfterFaxCmd);
         delete Fax;
      }
   }

   Modem->SendCommand (Cfg->Hangup);
   if (Modem->Serial != NULL && Log != NULL) {
      if (Modem->Serial->Carrier () == TRUE)
         Log->Write ("!Unable to drop carrier");
   }

   StartTimer (hwndMainClient, 1, MODEM_DELAY);

#if defined(__OS2__)
   if (hmq != NULL)
      WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
#endif

#if defined(__OS2__) || defined(__NT__)
   _endthread ();
#endif
}

#endif

VOID SendInitThread (PVOID Args)
{
   Args = Args;
   Modem->SendCommand (Cfg->Initialize[Current]);

#if defined(__OS2__) || defined(__NT__)
   _endthread ();
#endif
}

VOID ModemTimer (HWND hwnd)
{
   USHORT i;

   switch (Status) {
      case 0:
         if (Status != 200) {
            if (Modem == NULL && Cfg != NULL) {
               if ((Modem = new TModem) != NULL) {
                  Modem->Log = Log;
                  strcpy (Modem->Device, Cfg->Device);
                  Modem->Speed = Cfg->Speed;
                  Modem->LockSpeed = Cfg->LockSpeed;
                  if (Cfg->Ring[0] != '\0')
                     strcpy (Modem->Ring, Cfg->Ring);
                  if (Modem->Initialize (comHandle) == TRUE) {
                     Current = 0;
                     Modem->hwndWindow = hwnd;

                     if (Modem->Serial->Carrier () == TRUE && gotPort == TRUE && gotSpeed == TRUE) {
                        if (Log != NULL)
                           Log->Write ("+Connect %lu", Modem->Speed);
                        StopTimer (hwnd, 1);
#if defined(__OS2__)                        
                        _beginthread (BbsThread, NULL, 32768U, NULL);
#elif defined(__NT__)
                        _beginthread (BbsThread, NULL, NULL);
#endif                        
                        Status = BBSEXIT;
                     }
                     else
                        Status = INITIALIZE;
                  }
                  else if (Log != NULL) {
                     Log->Write ("!Error opening device %s", Modem->Device);
                     Status = BBSEXIT;
                  }
               }
            }
            if (Outbound != NULL && Status != BBSEXIT) {
               if (Log != NULL)
                  Log->Write ("+Building the outbound queue");
               Outbound->BuildQueue (Cfg->Outbound);
               if (Outbound->FirstNode () == FALSE)
                  Outbound->New ();
               if (Log != NULL)
                  Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);
#if defined(__OS2__)
               WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
               PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
            }
         }
         break;

      case INITIALIZE:
         if (Modem != NULL && Cfg != NULL) {
            while (Current < 3 && Cfg->Initialize[Current][0] == '\0')
               Current++;
            if (Current >= 3) {
               Status = WAITFORCALL;
               TimeOut = TimerSet (REINIT_DELAY);
               Modem->Terminal = TRUE;
            }
            else {
               Modem->Terminal = FALSE;
#if defined(__OS2__)
               _beginthread (SendInitThread, NULL, 8192U, NULL);
#elif defined(__NT__)
               _beginthread (SendInitThread, NULL, NULL);
#endif               
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
               TimeOut = TimerSet (REINIT_DELAY);
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

      case WAITFORCALL: {
         USHORT t;
         struct dostime_t dt;

         if ((i = Modem->GetResponse ()) == RING && Cfg != NULL) {
            if (Cfg->ManualAnswer == TRUE) {
               if (Cfg->LimitedHours == FALSE) {
                  Modem->SendCommand (Cfg->Answer);
                  Status = ANSWERING;
                  TimeOut = TimerSet (4500L);
               }
               else {
                  _dos_gettime (&dt);
                  t = (USHORT)(dt.hour * 60 + dt.minute);
                  if (Cfg->StartTime < Cfg->EndTime) {
                     if (t >= Cfg->StartTime && t <= Cfg->EndTime) {
                        Modem->SendCommand (Cfg->Answer);
                        Status = ANSWERING;
                        TimeOut = TimerSet (4500L);
                     }
                  }
                  else {
                     if (t >= Cfg->StartTime) {
                        Modem->SendCommand (Cfg->Answer);
                        Status = ANSWERING;
                        TimeOut = TimerSet (4500L);
                     }
                     else if (t <= Cfg->EndTime) {
                        Modem->SendCommand (Cfg->Answer);
                        Status = ANSWERING;
                        TimeOut = TimerSet (4500L);
                     }
                  }
               }
            }
         }
         else if (i == CONNECT) {
            StopTimer (hwnd, 1);
#if defined(__OS2__)            
            _beginthread (BbsThread, NULL, 32768U, NULL);
#elif defined(__NT__)
            _beginthread (BbsThread, NULL, NULL);
#endif            
            Status = HANGUP;
         }
         else if (i == FAX) {
            StopTimer (hwnd, 1);
#if defined(__OS2__)            
            _beginthread (FaxReceiveThread, NULL, 8192, NULL);
#elif defined(__NT__)
            _beginthread (FaxReceiveThread, NULL, NULL);
#endif            
            Status = HANGUP;
         }
         else if (TimeUp (TimeOut) == TRUE) {
            Status = INITIALIZE;
            Current = 0;
         }
         break;
      }

      case ANSWERING:
         if ((i = Modem->GetResponse ()) == CONNECT) {
            StopTimer (hwnd, 1);
#if defined(__OS2__)            
            _beginthread (BbsThread, NULL, 32768U, NULL);
#elif defined(__NT__)
            _beginthread (BbsThread, NULL, NULL);
#endif            
            Status = HANGUP;
         }
         else if (i == FAX) {
            StopTimer (hwnd, 1);
#if defined(__OS2__)            
            _beginthread (FaxReceiveThread, NULL, 8192, NULL);
#elif defined(__NT__)
            _beginthread (FaxReceiveThread, NULL, NULL);
#endif            
            Status = HANGUP;
         }
         else if (i != NO_RESPONSE && i != RING)
            Status = HANGUP;
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
         CallTimer = 0L;
         break;

      case WAITFORCONNECT:
         if ((i = Modem->GetResponse ()) == CONNECT) {
            if (PollNode[0] != '\0' && Outbound != NULL)
               Outbound->AddAttempt (PollNode, TRUE);
            StopTimer (hwnd, 1);
#if defined(__OS2__)            
            _beginthread (MailerThread, NULL, 32768U, NULL);
#elif defined(__NT__)
            _beginthread (MailerThread, NULL, NULL);
#endif            
            Status = HANGUP;
         }
         else if (i != NO_RESPONSE) {
            if (PollNode[0] != '\0' && Outbound != NULL) {
               Outbound->AddAttempt (PollNode, FALSE, Modem->Response);
//               PollNode[0] = '\0';
#if defined(__OS2__)
               WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
               PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
            }
            Status = INITIALIZE;
            Current = 0;
         }
         else if (TimeUp (TimeOut) == TRUE) {
            if (Log != NULL)
               Log->Write ("!Dialing timer expired");
            if (PollNode[0] != '\0' && Outbound != NULL) {
               Outbound->AddAttempt (PollNode, FALSE, "Timeout");
//               PollNode[0] = '\0';
#if defined(__OS2__)
               WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
               PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
            }
            Status = HANGUP;
         }
         break;

      case BBSEXIT:
#if defined(__OS2__)
         WinPostMsg (hwnd, WM_CLOSE, 0L, 0L);
#elif defined(__NT__)
         PostMessage (hwnd, WM_CLOSE, 0, 0L);
#endif
         break;
   }
}

//////////////////////////////////////////////////////////////////
// Processo in background per controllo eventi e poll           //
//////////////////////////////////////////////////////////////////
VOID BackgroundThread (PVOID Args)
{
   DIR *dir;
   USHORT i, DoCall, Found, DoEvent;
   USHORT OldStatus = 99;
   CHAR Temp[64], *p;
   ULONG EventsTimer;
   time_t t;
   class TAddress Address;
   class TOutbound *Out;
   class TNodes *Nodes;
   struct stat statbuf;
   struct dirent *ent;
   struct tm ltm;

#if defined(__OS2__)
   HAB hab;
   HMQ hmq;
#endif

   Args = Args;
   EventsTimer = 0L;

#if defined(__OS2__)
   hab = WinInitialize (0);
   if ((hmq = WinCreateMsgQueue (hab, 0)) != NULL)
      WinCancelShutdown (hmq, TRUE);
   DosExitCritSec ();
//   DosSetPriority ((USHORT)1, (USHORT)0, (SHORT)31, (USHORT)0);
#endif

   for (;;) {
      if (Status != OldStatus) {
         OldStatus = Status;
//         if (Status != UNDEFINED)
//            Log->Write ("> DEBUG: Status=%d (%s)", Status, statusText[Status]);
         EventsTimer = 0L;
         CallTimer = 0L;
      }
      if (Status == WAITFORCALL) {
         //////////////////////////////////////////////////////////////////
         // Controlla se ci sono eventi da eseguire.                     //
         //////////////////////////////////////////////////////////////////
         if (TimeUp (EventsTimer)) {
            Found = DoCall = FALSE;
            Address.Clear ();

            Events->TimeToNext ();
            if (Events->NextNumber != 0) {
               if ((p = (CHAR *)malloc (128)) != NULL) {
                  sprintf (p, "Event %d starts in %d minute(s)", Events->NextNumber, Events->TimeRemain);
#if defined(__OS2__)
                  WinSendMsg (hwndMainClient, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 0), MPFROMP (p));
#elif defined(__NT__)
                  SendMessage (hwndMainClient, WM_USER, (WPARAM)WMU_SETSTATUSLINE0, (LPARAM)p);
#elif defined(__DOS__)
                  prints (22, 2, WHITE|_BLACK, p);
#endif
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
#if defined(__OS2__)
                  WinSendMsg (hwndMainClient, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 1), MPFROMP (p));
#elif defined(__NT__)
                  SendMessage (hwndMainClient, WM_USER, (WPARAM)WMU_SETSTATUSLINE1, (LPARAM)p);
#elif defined(__DOS__)
                  prints (23, 2, WHITE|_BLACK, p);
#endif
                  free (p);
               }
            }

            DoEvent = FALSE;

            if (Events->First () == TRUE)
               do {
                  if (Events->Dynamic == TRUE && Events->Completed == FALSE) {
                     if (Outbound->FirstNode () == TRUE)
                        do {
                           if (Events->SendCrash == TRUE && Outbound->Crash == TRUE)
                              DoEvent = TRUE;
                           if (Events->SendDirect == TRUE && Outbound->Direct == TRUE)
                              DoEvent = TRUE;
                           if (Events->SendNormal == TRUE && Outbound->Normal == TRUE)
                              DoEvent = TRUE;
                           if (Events->SendImmediate == TRUE && Outbound->Immediate == TRUE)
                              DoEvent = TRUE;
                           if (Events->MaxCalls != 0 && Outbound->Attempts >= Events->MaxCalls)
                              DoEvent = FALSE;
                           else if (Events->MaxConnects != 0 && Outbound->Failed >= Events->MaxConnects)
                              DoEvent = FALSE;
                        } while (Outbound->NextNode () == TRUE);

                     if (DoEvent == TRUE) {
                        t = time (NULL);
                        memcpy (&ltm, localtime (&t), sizeof (struct tm));
                        if (Events->LastDay != ltm.tm_yday)
                           DoEvent = FALSE;
                     }

                     if (DoEvent == TRUE)
                        break;

                     Events->Completed = TRUE;
                     Events->Update ();
                  }
               } while (Events->Next () == TRUE);

            if (DoEvent == FALSE)
               DoEvent = Events->SetCurrent ();

            if (DoEvent == TRUE) {
               if (Events->Started == TRUE) {
                  if (Log != NULL) {
                     if (Events->Label[0] != '\0')
                        Log->Write (":Starting Event %d - %s", Events->Number, Events->Label);
                     else
                        Log->Write (":Starting Event %d", Events->Number);
                  }

                  if (Events->Command[0] != '\0') {
                     Log->Write ("#Executing %s", Events->Command);
                     Log->Suspend ();
                     RunExternal (Events->Command);
                     Log->Resume ();
                  }

                  if (Events->StartImport == TRUE && Events->StartExport == TRUE && Cfg->UseSinglePass == TRUE) {
                     Log->Write ("#Executing %s", Cfg->SinglePassCmd);
                     Log->Suspend ();
                     RunExternal (Cfg->SinglePassCmd);
                     Log->Resume ();
                     Log->Write ("#Executing %s", Cfg->PackCmd);
                     Log->Suspend ();
                     RunExternal (Cfg->PackCmd);
                     Log->Resume ();
                  }
                  else {
                     if (Events->StartImport == TRUE) {
                        if (Events->ImportNormal == TRUE || Events->ImportProtected == TRUE || Events->ImportKnown == TRUE) {
                           Log->Write ("#Executing %s", Cfg->ImportCmd);
                           Log->Suspend ();
                           RunExternal (Cfg->ImportCmd);
                           Log->Resume ();
                        }
                     }
                     if (Events->StartExport == TRUE) {
                        if (Cfg->SeparateNetMail == TRUE) {
                           Log->Write ("#Executing %s", Cfg->PackCmd);
                           Log->Suspend ();
                           RunExternal (Cfg->PackCmd);
                           Log->Resume ();
                        }
                        Log->Write ("#Executing %s", Cfg->ExportCmd);
                        Log->Suspend ();
                        RunExternal (Cfg->ExportCmd);
                        Log->Resume ();
                        Log->Write ("#Executing %s", Cfg->PackCmd);
                        Log->Suspend ();
                        RunExternal (Cfg->PackCmd);
                        Log->Resume ();
                     }
                  }

                  if (Events->ForceCall == TRUE && Events->Address[0] != '\0') {
                     if ((Out = new TOutbound (Cfg->Outbound)) != NULL) {
                        Cfg->MailAddress.First ();
                        Out->DefaultZone = Cfg->MailAddress.Zone;

                        Address.Parse (Events->Address);
                        if (Cfg->MailAddress.First () == TRUE) {
                           if (Address.Zone == 0)
                              Address.Zone = Cfg->MailAddress.Zone;
                           if (Address.Net == 0)
                              Address.Net = Cfg->MailAddress.Net;
                        }
                        Out->Add (Address.Zone, Address.Net, Address.Node, Address.Point);

                        Out->New ();
                        Out->Zone = Address.Zone;
                        Out->Net = Address.Net;
                        Out->Node = Address.Node;
                        Out->Point = Address.Point;
                        Out->Poll = TRUE;
                        Out->Crash = Events->SendCrash;
                        Out->Direct = Events->SendDirect;
                        Out->Normal = Events->SendNormal;
                        Out->Immediate = Events->SendImmediate;
                        Out->Add ();
                        Out->Update ();

                        delete Out;
                     }
                  }

                  if (Log != NULL)
                     Log->Write ("+Building the outbound queue");
                  Outbound->BuildQueue (Cfg->Outbound);
                  unlink ("rescan.now");
                  if (Log != NULL)
                     Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);
#if defined(__OS2__)
                  WinSendMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
                  SendMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
                  CallTimer = 0L;

                  Events->Save ();
               }
            }

            if (stat ("rescan.now", &statbuf) == 0) {
               if (Log != NULL)
                  Log->Write ("+Building the outbound queue");
               Outbound->BuildQueue (Cfg->Outbound);
               unlink ("rescan.now");
               if (Log != NULL)
                  Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);
               CallTimer = 0L;
            }

            if ((dir = opendir (".")) != NULL) {
               sprintf (Temp, "lexit%d", Cfg->TaskNumber);
               while ((ent = readdir (dir)) != NULL) {
                  if ((p = strchr (ent->d_name, '.')) != NULL) {
                     *p++ = '\0';
                     if (!stricmp (ent->d_name, Temp)) {
                        ErrorLevel = (UCHAR)atoi (p);
                        Status = BBSEXIT;
                        if (Log != NULL)
                           Log->Write (":Exit with errorlevel %d", ErrorLevel);
                        break;
                     }
                  }
               }
               closedir (dir);
            }

            EventsTimer = TimerSet (1000L);
         }

         if (CallTimer == 0L)
            CallTimer = TimerSet ((ULONG)Events->CallDelay * 100L);

         //////////////////////////////////////////////////////////////////
         // Controlla se c'e' un poll da eseguire.                       //
         //////////////////////////////////////////////////////////////////
         if (TimeUp (CallTimer)) {
            DoCall = FALSE;
            Address.Clear ();

            if (Outbound != NULL && Outbound->TotalNodes > 0) {
               // Verifica se l'evento corrente e' riservato per il poll verso un
               // certo nodo.
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
                  // Cerca il nodo nel database di outbound e se lo trova lo marca
                  // immediatamente per il poll.
                  DoCall = FALSE;
                  if (Outbound->FirstNode () == TRUE)
                     do {
                        if (Outbound->Zone == Address.Zone && Outbound->Net == Address.Net && Outbound->Node == Address.Node && Outbound->Point == Address.Point) {
                           strcpy (PollNode, Address.String);
                           DoCall = TRUE;
                           break;
                        }
                     } while (Outbound->NextNode () == TRUE);
               }

               // L'evento corrente non e' riservato ad un nodo.
               if (Events->Address[0] == '\0') {
                  Address.Clear ();
                  // Verifica se c'e' gia' stata una chiamata, nel qual caso si posiziona
                  // sull'entry di quel nodo e comincia la ricerca dal nodo sucessivo (se
                  // esiste oppure dal primo.
                  if (PollNode[0] != '\0') {
                     Address.Clear ();
                     Address.Parse (PollNode);
                     if (Cfg->MailAddress.First () == TRUE) {
                        if (Address.Zone == 0)
                           Address.Zone = Cfg->MailAddress.Zone;
                        if (Address.Net == 0)
                           Address.Net = Cfg->MailAddress.Net;
                        Address.Add ();
                        Address.First ();
                     }
                     Found = FALSE;
                     if (Outbound->FirstNode () == TRUE)
                        do {
                           if (Outbound->Zone == Address.Zone && Outbound->Net == Address.Net && Outbound->Node == Address.Node && Outbound->Point == Address.Point) {
                              if (Outbound->NextNode () == TRUE)
                                 Found = TRUE;
                              break;
                           }
                        } while (Outbound->NextNode () == TRUE);
                     if (Found == FALSE)
                        Outbound->FirstNode ();
                  }
                  else
                     Outbound->FirstNode ();

                  // Verifica i flag di chiamata
                  DoCall = FALSE;
                  if (Events->SendCrash == TRUE && Outbound->Crash == TRUE)
                     DoCall = TRUE;
                  if (Events->SendDirect == TRUE && Outbound->Direct == TRUE)
                     DoCall = TRUE;
                  if (Events->SendNormal == TRUE && Outbound->Normal == TRUE)
                     DoCall = TRUE;
                  if (Events->SendImmediate == TRUE && Outbound->Immediate == TRUE)
                     DoCall = TRUE;
                  if (Events->MaxCalls != 0 && Outbound->Attempts >= Events->MaxCalls)
                     DoCall = FALSE;
                  else if (Events->MaxConnects != 0 && Outbound->Failed >= Events->MaxConnects)
                     DoCall = FALSE;
                  if (DoCall == TRUE) {
                     if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                        if (Nodes->Read (Outbound->Address) == FALSE)
                           DoCall = FALSE;
                        strupr (Nodes->Flags);

                        // Verifica i flags con i quali puo' chiamare
                        if (Cfg->CallIf[0] != '\0' && DoCall == TRUE) {
                           strcpy (Temp, Cfg->CallIf);
                           DoCall = FALSE;
                           if ((p = strtok (strupr (Temp), " ,")) != NULL)
                              do {
                                 if (strstr (Nodes->Flags, p) != NULL)
                                    DoCall = TRUE;
                              } while ((p = strtok (NULL, " ,")) != NULL);
                        }

                        // Verifica i flags con i quali non puo' chiamare
                        if (Cfg->DontCallIf[0] != '\0' && DoCall == TRUE) {
                           strcpy (Temp, Cfg->DontCallIf);
                           DoCall = TRUE;
                           if ((p = strtok (strupr (Temp), " ,")) != NULL)
                              do {
                                 if (strstr (Nodes->Flags, p) != NULL)
                                    DoCall = FALSE;
                              } while ((p = strtok (NULL, " ,")) != NULL);
                        }

                        delete Nodes;
                     }
                  }
               }

               if (DoCall == FALSE) {
                  while (Outbound->NextNode () == TRUE) {
                     if (Events->SendCrash == TRUE && Outbound->Crash == TRUE)
                        DoCall = TRUE;
                     if (Events->SendDirect == TRUE && Outbound->Direct == TRUE)
                        DoCall = TRUE;
                     if (Events->SendNormal == TRUE && Outbound->Normal == TRUE)
                        DoCall = TRUE;
                     if (Events->SendImmediate == TRUE && Outbound->Immediate == TRUE)
                        DoCall = TRUE;
                     if (Events->MaxCalls != 0 && Outbound->Attempts >= Events->MaxCalls)
                        DoCall = FALSE;
                     else if (Events->MaxConnects != 0 && Outbound->Failed >= Events->MaxConnects)
                        DoCall = FALSE;
                     if (DoCall == TRUE) {
                        if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                           if (Nodes->Read (Outbound->Address) == FALSE)
                              DoCall = FALSE;
                           strupr (Nodes->Flags);

                           // Verifica i flags con i quali puo' chiamare
                           if (Cfg->CallIf[0] != '\0' && DoCall == TRUE) {
                              strcpy (Temp, Cfg->CallIf);
                              DoCall = FALSE;
                              if ((p = strtok (strupr (Temp), " ,")) != NULL)
                                 do {
                                    if (strstr (Nodes->Flags, p) != NULL)
                                       DoCall = TRUE;
                                 } while ((p = strtok (NULL, " ,")) != NULL);
                           }

                           // Verifica i flags con i quali non puo' chiamare
                           if (Cfg->DontCallIf[0] != '\0' && DoCall == TRUE) {
                              strcpy (Temp, Cfg->DontCallIf);
                              DoCall = TRUE;
                              if ((p = strtok (strupr (Temp), " ,")) != NULL)
                                 do {
                                    if (strstr (Nodes->Flags, p) != NULL)
                                       DoCall = FALSE;
                                 } while ((p = strtok (NULL, " ,")) != NULL);
                           }

                           delete Nodes;
                        }
                     }
                     if (DoCall == TRUE)
                        break;
                  }
               }

               if (DoCall == TRUE) {
                  strcpy (PollNode, Outbound->Address);
                  strcpy (Modem->NodelistPath, Cfg->NodelistPath);
                  strcpy (Modem->DialCmd, Cfg->Dial);
                  Modem->Poll (PollNode);
                  Status = WAITFORCONNECT;
                  TimeOut = TimerSet ((ULONG)Cfg->DialTimeout * 100L);
               }
               else
                  PollNode[0] = '\0';
            }

            CallTimer = 0L;
         }
      }

#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   }

/*
#if defined(__OS2__)
   if (hmq != NULL)
      WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
#endif

#if defined(__OS2__) || defined(__NT__)
   _endthread ();
#endif
*/
}

USHORT ProcessSimpleDialog (HWND hwnd, USHORT id)
{
   USHORT RetVal = FALSE;
#if !defined(__POINT__)
   class CDialog *Dlg = NULL;

   switch (id) {
      case 107:      // System / File request
         Dlg = new CRequestDlg (hwnd);
         RetVal = TRUE;
         break;
      case 108:      // System / File attach
         Dlg = new CAttachDlg (hwnd);
         RetVal = TRUE;
         break;
      case 111:      // System / Request ECHOmail Link
         Dlg = new CRequestEchoLinkDlg (hwnd);
         RetVal = TRUE;
         break;
      case 112:      // System / New ECHOmail Link
         Dlg = new CNewEchoLinkDlg (hwnd);
         RetVal = TRUE;
         break;
      case 113:      // System / Rescan ECHOmail
         Dlg = new CRescanDlg (hwnd);
         RetVal = TRUE;
         break;
      case 305:      // Mailer / File requests
         Dlg = new COkFileDlg (hwnd);
         RetVal = TRUE;
         break;
      case 402:      // BBS / Message Areas
         Dlg = new CMessageDlg (hwnd);
         RetVal = TRUE;
         break;
      case 403:      // BBS / File Areas
         Dlg = new CFileDlg (hwnd);
         RetVal = TRUE;
         break;
      case 406:      // BBS / User Levels
         Dlg = new CLimitsDlg (hwnd);
         RetVal = TRUE;
         break;
      case 409:      // BBS / External Protocols
         Dlg = new CProtocolDlg (hwnd);
         RetVal = TRUE;
         break;
      case 602:      // Manager / Compressors
         Dlg = new CPackerDlg (hwnd);
         RetVal = TRUE;
         break;
      case 603:
         Dlg = new CMenuDlg (hwnd);
         RetVal = TRUE;
         break;
      case 604:      // Manager / Nodes
         Dlg = new CNodesDlg (hwnd);
         RetVal = TRUE;
         break;
      case 605:      // Manager / User editor
         Dlg = new CUserDlg (hwnd);
         RetVal = TRUE;
         break;
      case 606:      // Manager / Nodelist
         Dlg = new CNodelistDlg (hwnd);
         RetVal = TRUE;
         break;
      case 607:      // Manager / Origin Lines
         Dlg = new COriginDlg (hwnd);
         RetVal = TRUE;
         break;
      case 608:      // Manager / Translations
         Dlg = new CTranslationDlg (hwnd);
         RetVal = TRUE;
         break;
   }

   if (Dlg != NULL) {
      Dlg->DoModal ();
      delete Dlg;
   }
#endif

   return (RetVal);
}

USHORT ProcessSaveDialog (HWND hwnd, USHORT id)
{
   USHORT RetVal = FALSE;
#if !defined(__POINT__)
   class CDialog *Dlg = NULL;

   switch (id) {
      case 202:      // Global / Site Informations
         Dlg = new CSiteInfoDlg (hwnd);
         RetVal = TRUE;
         break;
      case 204:      // Global / Address
         Dlg = new CAddressDlg (hwnd);
         RetVal = TRUE;
         break;
      case 206:      // Global / Internet Options
         Dlg = new CInternetDlg (hwnd);
         RetVal = TRUE;
         break;
      case 207:      // Global / Fax Options
         Dlg = new CFaxOptDlg (hwnd);
         RetVal = TRUE;
         break;
      case 208:      // Global / Directories - Paths
         Dlg = new CDirectoriesDlg (hwnd);
         RetVal = TRUE;
         break;
      case 302:      // Mailer / Mail processing
         Dlg = new CMailprocDlg (hwnd);
         RetVal = TRUE;
         break;
      case 303:      // Mailer / Areafix
         Dlg = new CAreafixDlg (hwnd);
         RetVal = TRUE;
         break;
      case 304:      // Mailer / Raid
         Dlg = new CRaidDlg (hwnd);
         RetVal = TRUE;
         break;
      case 306:      // Mailer / External processing
         Dlg = new CExternalDlg (hwnd);
         RetVal = TRUE;
         break;
      case 308:      // Mailer / Miscellaneous
         Dlg = new CMiscDlg (hwnd);
         RetVal = TRUE;
         break;
      case 404:      // BBS / Offline Reader
         Dlg = new COfflineDlg (hwnd);
         RetVal = TRUE;
         break;
      case 405:      // BBS / New Users
         Dlg = new CNewUserDlg (hwnd);
         RetVal = TRUE;
         break;
      case 411:      // BBS / General options
         Dlg = new CBBSGeneralDlg (hwnd);
         RetVal = TRUE;
         break;
      case 504:      // Modem / Answer control
         Dlg = new CAnswerDlg (hwnd);
         RetVal = TRUE;
         break;
      case 505:      // Modem / Nodelist flags
         Dlg = new CNodeFlagsDlg (hwnd);
         RetVal = TRUE;
         break;
   }

   if (Dlg != NULL) {
      if (Dlg->DoModal () == TRUE) {
         Cfg->Save ();
         if (Cfg->MailAddress.First () == TRUE)
            Outbound->DefaultZone = Cfg->MailAddress.Zone;
      }
      delete Dlg;
   }
#endif

   return (RetVal);
}

VOID WinHelp (PSZ help_file, int topic_id, PSZ title)
{
   CHAR helpFile[128];

   getcwd (helpFile, sizeof (helpFile) - 1);
   if (helpFile[strlen (helpFile) - 1] != '\\')
      strcat (helpFile, "\\");
   strcat (helpFile, help_file);

#if defined(__OS2__)
   CHAR *p;
   HELPINIT hini;

   if ((p = strchr (helpFile, '>')) != NULL)
      *p = '\0';

   if (help_hWnd == NULL) {
      hini.cb = sizeof (HELPINIT);
      hini.ulReturnCode = 0L;
      hini.pszTutorialName = NULL;
      hini.phtHelpTable = (PHELPTABLE)MAKELONG (1, 0xFFFF);
      hini.hmodHelpTableModule = NULL;
      hini.hmodAccelActionBarModule = NULL;
      hini.idAccelTable = 0;
      hini.idActionBar = 0;
      hini.pszHelpWindowTitle = title;
      hini.fShowPanelId = CMIC_HIDE_PANEL_ID;
      hini.pszHelpLibraryName = helpFile;
      if ((help_hWnd = WinCreateHelpInstance (hab, &hini)) != NULL)
         WinAssociateHelpInstance (help_hWnd, hwndMainFrame);
   }
   if (help_hWnd != NULL) {
      if (topic_id == HM_HELP_CONTENTS)
         WinSendMsg (help_hWnd, HM_HELP_CONTENTS, 0L, 0L);
      else if (topic_id == HM_GENERAL_HELP)
         WinSendMsg (help_hWnd, HM_GENERAL_HELP, 0L, 0L);
      else if (topic_id == HM_DISPLAY_HELP)
         WinSendMsg (help_hWnd, HM_DISPLAY_HELP, 0L, 0L);
      else
         WinSendMsg (help_hWnd, HM_DISPLAY_HELP, MPFROM2SHORT (topic_id, 0), MPFROMSHORT (HM_RESOURCEID));
   }

#elif defined(__NT__)
   title = title;
   ::WinHelp (hwndMainClient, helpFile, HELP_CONTEXT, topic_id);
#endif
}

VOID OutboundDetails (HWND hwnd)
{
   USHORT i;
   CHAR Temp[128], *p;
   class CDetailsDlg *Dlg;

#if defined(__OS2__)
   i = (USHORT)WinSendDlgItemMsg (hwnd, 1004, LM_QUERYSELECTION, MPFROMSHORT (LIT_FIRST), 0L);
   WinSendDlgItemMsg (hwnd, 1004, LM_QUERYITEMTEXT, MPFROM2SHORT (i, sizeof (Temp) - 1), MPFROMP (Temp));
#elif defined(__NT__)
   i = (USHORT)SendDlgItemMessage (hwnd, 1004, LB_GETCURSEL, 0, 0L);
   SendDlgItemMessage (hwnd, 1004, LB_GETTEXT, (WPARAM)i, (LPARAM)Temp);
#endif

   if ((p = strtok (Temp, " ")) != NULL) {
      if (Outbound->FirstNode () == TRUE)
         do {
            if (!strcmp (Outbound->Address, p)) {
               if ((Dlg = new CDetailsDlg (hwnd)) != NULL) {
                  strcpy (Dlg->Address, p);
                  Dlg->DoModal ();
                  if (Dlg->DoRebuild == TRUE) {
                     Outbound->BuildQueue (Cfg->Outbound);
                     unlink ("rescan.now");
#if defined(__OS2__)
                     WinPostMsg (hwnd, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
                     PostMessage (hwnd, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
                  }
                  delete Dlg;
               }
               break;
            }
         } while (Outbound->NextNode () == TRUE);
   }
}

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
         HWND hwndMainList, hwndMainModem, hwndMainStatus, hwndMainOutbound;
         ULONG Value;

         if ((hwndMainList = WinCreateWindow (hwnd, WC_LISTBOX, NULL, LS_NOADJUSTPOS|LS_HORZSCROLL|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1001, NULL, NULL)) != NULLHANDLE) {
            WinSetPresParam (hwndMainList, PP_FONTNAMESIZE, 14, "11.System VIO");
            Value = 0x000070L;
            WinSetPresParam (hwndMainList, PP_BACKGROUNDCOLOR, 4, &Value);
            Value = 0xFFFFFFL;
            WinSetPresParam (hwndMainList, PP_FOREGROUNDCOLOR, 4, &Value);
         }
         if ((hwndMainModem = WinCreateWindow (hwnd, WC_LISTBOX, NULL, LS_NOADJUSTPOS|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1002, NULL, NULL)) != NULLHANDLE) {
            WinSetPresParam (hwndMainModem, PP_FONTNAMESIZE, 14, "11.System VIO");
            Value = 0x000000L;
            WinSetPresParam (hwndMainModem, PP_BACKGROUNDCOLOR, 4, &Value);
            Value = 0xFFFFFFL;
            WinSetPresParam (hwndMainModem, PP_FOREGROUNDCOLOR, 4, &Value);
         }
         if ((hwndMainOutbound = WinCreateWindow (hwnd, WC_LISTBOX, NULL, LS_NOADJUSTPOS|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1004, NULL, NULL)) != NULLHANDLE) {
            WinSetPresParam (hwndMainOutbound, PP_FONTNAMESIZE, 14, "11.System VIO");
            Value = 0xFFFFFFL;
         }
         if ((hwndMainStatus = WinCreateWindow (hwnd, WC_LISTBOX, NULL, LS_NOADJUSTPOS|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1003, NULL, NULL)) != NULLHANDLE) {
            WinSetPresParam (hwndMainStatus, PP_FONTNAMESIZE, 14, "11.System VIO");
            Value = 0xFFFFFFL;
            WinSetPresParam (hwndMainStatus, PP_BACKGROUNDCOLOR, 4, &Value);
            Value = 0x000000L;
            WinSetPresParam (hwndMainStatus, PP_FOREGROUNDCOLOR, 4, &Value);
            WinSendMsg (hwndMainStatus, LM_INSERTITEM, MPFROMSHORT (LIT_END), (MPARAM)"");
            WinSendMsg (hwndMainStatus, LM_INSERTITEM, MPFROMSHORT (LIT_END), (MPARAM)"");
         }

#elif defined(__NT__)
         LOGFONT logFont;
         HFONT hFont;

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
         hFont = CreateFontIndirect (&logFont);

         if ((hwndMainList = CreateWindow ("LISTBOX", "", LBS_NOINTEGRALHEIGHT|LBS_NOTIFY|WS_CHILD|WS_VSCROLL|WS_HSCROLL, 0, 0, 100, 50, hwnd, (HMENU)1001, hinst, NULL)) != NULL) {
            if (hFont != NULL)
               SendMessage (hwndMainList, WM_SETFONT, (WPARAM)hFont, MAKELPARAM (FALSE, 0));
            ShowWindow (hwndMainList, SW_SHOW);
         }

         if ((hwndModemList = CreateWindow ("LISTBOX", "", LBS_NOINTEGRALHEIGHT|LBS_NOTIFY|WS_CHILD|WS_VSCROLL, 0, 0, 100, 50, hwnd, (HMENU)1002, hinst, NULL)) != NULL) {
            if (hFont != NULL)
               SendMessage (hwndModemList, WM_SETFONT, (WPARAM)hFont, MAKELPARAM (FALSE, 0));
            ShowWindow (hwndModemList, SW_SHOW);
         }

         if ((hwndOutboundList = CreateWindow ("LISTBOX", "", LBS_NOINTEGRALHEIGHT|LBS_NOTIFY|WS_CHILD|WS_VSCROLL, 0, 0, 100, 50, hwnd, (HMENU)1004, hinst, NULL)) != NULL) {
            if (hFont != NULL)
               SendMessage (hwndOutboundList, WM_SETFONT, (WPARAM)hFont, MAKELPARAM (FALSE, 0));
            ShowWindow (hwndOutboundList, SW_SHOW);
         }

         if ((hwndStatusList = CreateWindow ("LISTBOX", "", LBS_NOSEL|LBS_NOINTEGRALHEIGHT|LBS_NOTIFY|WS_CHILD, 0, 0, 100, 50, hwnd, (HMENU)1003, hinst, NULL)) != NULL) {
            if (hFont != NULL)
               SendMessage (hwndStatusList, WM_SETFONT, (WPARAM)hFont, MAKELPARAM (FALSE, 0));
            ShowWindow (hwndStatusList, SW_SHOW);
         }
#endif
         if (hwndMainList != NULL) {
            if ((Log = new TPMLog (hwnd)) != NULL) {
               if (Cfg->LogFile[0] == '\0')
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

         if ((HWND)lParam == hwndMainList) {
            SetTextColor ((HDC)wParam, RGB (0xFF, 0xFF, 0xFF));
            SetBkMode ((HDC)wParam, TRANSPARENT);
         }
         else if ((HWND)lParam == hwndModemList) {
            SetTextColor ((HDC)wParam, RGB (0xFF, 0xFF, 0xFF));
            SetBkMode ((HDC)wParam, TRANSPARENT);
         }
         else if ((HWND)lParam == hwndStatusList) {
            SetTextColor ((HDC)wParam, RGB (0x00, 0x00, 0x00));
            SetBkMode ((HDC)wParam, TRANSPARENT);
         }
         else if ((HWND)lParam == hwndOutboundList) {
            SetTextColor ((HDC)wParam, RGB (0x00, 0x00, 0x00));
            SetBkMode ((HDC)wParam, TRANSPARENT);
         }

         logBrush.lbStyle = BS_SOLID;
         if ((HWND)lParam == hwndMainList)
            logBrush.lbColor = RGB (0x00, 0x00, 0x70);
         else if ((HWND)lParam == hwndModemList)
            logBrush.lbColor = RGB (0x00, 0x00, 0x00);
         else if ((HWND)lParam == hwndStatusList)
            logBrush.lbColor = RGB (0xFF, 0xFF, 0xFF);
         else if ((HWND)lParam == hwndOutboundList)
            logBrush.lbColor = RGB (0xD0, 0xD0, 0x80);
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
            case WMU_ADDLOGITEM: {
               USHORT Item;
#if defined(__OS2__)
               HWND hwndList;

               hwndList = WinWindowFromID (hwnd, 1001);
               if ((USHORT)WinSendMsg (hwndList, LM_QUERYITEMCOUNT, 0L, 0L) > 50)
                  WinSendMsg (hwndList, LM_DELETEITEM, MPFROMSHORT (0), 0L);
               Item = (USHORT)WinSendMsg (hwndList, LM_INSERTITEM, MPFROMSHORT (LIT_END), mp2);
               WinSendMsg (hwndList, LM_SELECTITEM, MPFROMSHORT (Item), MPFROMSHORT (TRUE));
#elif defined(__NT__)
               if (SendMessage (hwndMainList, LB_GETCOUNT, 0, 0L) > 50)
                  SendMessage (hwndMainList, LB_DELETESTRING, 0, 0L);
               Item = (USHORT)SendMessage (hwndMainList, LB_ADDSTRING, 0, lParam);
               SendMessage (hwndMainList, LB_SETCURSEL, (WPARAM)Item, 0L);
#endif
               break;
            }
            case WMU_ADDMODEMITEM: {
               USHORT Item;
#if defined(__OS2__)
               HWND hwndList;

               hwndList = WinWindowFromID (hwnd, 1002);
               if ((USHORT)WinSendMsg (hwndList, LM_QUERYITEMCOUNT, 0L, 0L) > 50)
                  WinSendMsg (hwndList, LM_DELETEITEM, MPFROMSHORT (0), 0L);
               Item = (USHORT)WinSendMsg (hwndList, LM_INSERTITEM, MPFROMSHORT (LIT_END), mp2);
               WinSendMsg (hwndList, LM_SELECTITEM, MPFROMSHORT (Item), MPFROMSHORT (TRUE));
#elif defined(__NT__)
               if (SendMessage (hwndModemList, LB_GETCOUNT, 0, 0L) > 50)
                  SendMessage (hwndModemList, LB_DELETESTRING, 0, 0L);
               Item = (USHORT)SendMessage (hwndModemList, LB_ADDSTRING, 0, lParam);
               SendMessage (hwndModemList, LB_SETCURSEL, (WPARAM)Item, 0L);
#endif
               break;
            }
#if defined(__OS2__)
            case WMU_SETSTATUSLINE: {
               USHORT Item;
               HWND hwndList;

               hwndList = WinWindowFromID (hwnd, 1003);
               Item = SHORT2FROMMP (mp1);
               WinSendMsg (hwndList, LM_SETITEMTEXT, MPFROMSHORT (Item), mp2);
               break;
            }
#elif defined(__NT__)
            case WMU_SETSTATUSLINE0:
            case WMU_SETSTATUSLINE1: {
               USHORT Item;

               Item = (USHORT)(wParam - WMU_SETSTATUSLINE0);
               SendMessage (hwndStatusList, LB_DELETESTRING, Item, 0L);
               SendMessage (hwndStatusList, LB_INSERTSTRING, Item, lParam);
               break;
            }
#endif
            case WMU_REFRESHOUTBOUND: {
               CHAR String[128], Flags[16], Status[32];
#if defined(__OS2__)
               HWND hwndList;

               hwndList = WinWindowFromID (hwnd, 1004);
               WinSendMsg (hwndList, LM_DELETEALL, 0L, 0L);
#elif defined(__NT__)
               SendMessage (hwndOutboundList, LB_RESETCONTENT, 0, 0L);
#endif

               if (Outbound->FirstNode () == TRUE) {
                  sprintf (String, "Node               Try/Con  Type       Size     Status");
#if defined(__OS2__)
                  WinSendMsg (hwndList, LM_INSERTITEM, MPFROMSHORT (LIT_END), String);
#elif defined(__NT__)
                  SendMessage (hwndOutboundList, LB_ADDSTRING, 0, (LPARAM)String);
#endif
                  do {
                     Flags[0] = (Outbound->Normal == TRUE) ? 'N' : ' ';
                     Flags[1] = (Outbound->Crash == TRUE) ? 'C' : ' ';
                     Flags[2] = (Outbound->Direct == TRUE) ? 'D' : ' ';
                     Flags[3] = (Outbound->Hold == TRUE) ? 'H' : ' ';
                     Flags[4] = (Outbound->Immediate == TRUE) ? 'I' : ' ';
                     Flags[5] = '\0';
                     if (Outbound->Hold == TRUE && Outbound->Normal == FALSE && Outbound->Crash == FALSE && Outbound->Direct == FALSE && Outbound->Immediate == FALSE)
                        strcpy (Status, "Hold");
                     else {
                        strcpy (Status, "Temp. Hold");
                        if (Outbound->Normal == TRUE && Events->SendNormal == TRUE)
                           strcpy (Status, Outbound->LastCall);
                        else if (Outbound->Crash == TRUE && Events->SendCrash == TRUE)
                           strcpy (Status, Outbound->LastCall);
                        else if (Outbound->Direct == TRUE && Events->SendDirect == TRUE)
                           strcpy (Status, Outbound->LastCall);
                        else if (Outbound->Immediate == TRUE && Events->SendImmediate == TRUE)
                           strcpy (Status, Outbound->LastCall);
                        if (Events->MaxCalls != 0 && Outbound->Attempts >= Events->MaxCalls)
                           strcpy (Status, "Undialable (Try)");
                        else if (Events->MaxConnects != 0 && Outbound->Failed >= Events->MaxConnects)
                           strcpy (Status, "Undialable (Con)");
                     }
                     sprintf (String, "%-16.16s  %3d %3d   %s  %8lub    %s", Outbound->Address, Outbound->Attempts, Outbound->Failed, Flags, Outbound->Size, Status);
#if defined(__OS2__)
                     WinSendMsg (hwndList, LM_INSERTITEM, MPFROMSHORT (LIT_END), String);
#elif defined(__NT__)
                     SendMessage (hwndOutboundList, LB_ADDSTRING, 0, (LPARAM)String);
#endif
                  } while (Outbound->NextNode () == TRUE);
               }
               else {
#if defined(__OS2__)
                  WinSendMsg (hwndList, LM_INSERTITEM, MPFROMSHORT (LIT_END), "");
                  WinSendMsg (hwndList, LM_INSERTITEM, MPFROMSHORT (LIT_END), "");
                  WinSendMsg (hwndList, LM_INSERTITEM, MPFROMSHORT (LIT_END), "");
                  WinSendMsg (hwndList, LM_INSERTITEM, MPFROMSHORT (LIT_END), "                      Nothing in outbound area");
#elif defined(__NT__)
                  SendMessage (hwndOutboundList, LB_ADDSTRING, 0, (LPARAM)"");
                  SendMessage (hwndOutboundList, LB_ADDSTRING, 0, (LPARAM)"");
                  SendMessage (hwndOutboundList, LB_ADDSTRING, 0, (LPARAM)"");
                  SendMessage (hwndOutboundList, LB_ADDSTRING, 0, (LPARAM)"                      Nothing in outbound area");
#endif
               }
               break;
            }
            case WMU_CLEAROUTBOUND:
#if defined(__OS2__)
               WinSendMsg (WinWindowFromID (hwnd, 1004), LM_DELETEALL, 0L, 0L);
#elif defined(__NT__)
               SendMessage (hwndOutboundList, LB_RESETCONTENT, 0, 0L);
#endif
               break;
            case WMU_ADDOUTBOUNDLINE: {
               USHORT Item;
#if defined(__OS2__)
               HWND hwndList;

               hwndList = WinWindowFromID (hwnd, 1004);
               if ((USHORT)WinSendMsg (hwndList, LM_QUERYITEMCOUNT, 0L, 0L) > 50)
                  WinSendMsg (hwndList, LM_DELETEITEM, MPFROMSHORT (0), 0L);
               Item = (USHORT)WinSendMsg (hwndList, LM_INSERTITEM, MPFROMSHORT (LIT_END), mp2);
               WinSendMsg (hwndList, LM_SELECTITEM, MPFROMSHORT (Item), MPFROMSHORT (TRUE));
#elif defined(__NT__)
               if (SendMessage (hwndOutboundList, LB_GETCOUNT, 0, 0L) > 50)
                  SendMessage (hwndOutboundList, LB_DELETESTRING, 0, 0L);
               Item = (USHORT)SendMessage (hwndOutboundList, LB_ADDSTRING, 0, lParam);
               SendMessage (hwndOutboundList, LB_SETCURSEL, (WPARAM)Item, 0L);
#endif
               break;
            }
            case WMU_REPLACEOUTBOUNDLINE: {
               USHORT Item;
#if defined(__OS2__)
               HWND hwndList;

               hwndList = WinWindowFromID (hwnd, 1004);
               if ((Item = (USHORT)WinSendMsg (hwndList, LM_QUERYITEMCOUNT, 0L, 0L)) >= 0)
                  WinSendMsg (hwndList, LM_SETITEMTEXT, MPFROMSHORT (Item - 1), mp2);
#elif defined(__NT__)
               if ((Item = (USHORT)SendMessage (hwndOutboundList, LB_GETCOUNT, 0, 0L)) > 0) {
                  SendMessage (hwndOutboundList, LB_DELETESTRING, Item - 1, 0L);
                  Item = (USHORT)SendMessage (hwndOutboundList, LB_ADDSTRING, 0, lParam);
                  SendMessage (hwndOutboundList, LB_SETCURSEL, (WPARAM)Item, 0L);
               }
#endif
               break;
            }
         }
         break;

      case WM_TIMER:
#if defined(__OS2__)
         if (SHORT1FROMMP (mp1) == 99) {
            ULONG Value[3];

            Value[0] = 0L;
            DosQuerySysInfo (20, 20, (UCHAR *)Value, sizeof (Value));
            Log->Write ("+%lu bytes remain in heap", Value[0]);
            break;
         }
#endif

#if defined(__OS2__)
         if (SHORT1FROMMP (mp1) == 1)
            ModemTimer (hwnd);
#elif defined(__NT__)
         if (wParam == 1)
            ModemTimer (hwnd);
#endif
         break;

      case WM_SIZE: {
         USHORT dx, dy, doy;

#if defined(__OS2__)
         dx = (USHORT)(((float)SHORT1FROMMP (mp2) / 100.0) * 65.0);
//         dy = (USHORT)(((float)SHORT2FROMMP (mp2) / 100.0) * 83.0);
         dy = (USHORT)(SHORT2FROMMP (mp2) - 36);
         doy = (USHORT)(((float)dy / 100.0) * 45.0);
         dy -= doy;
         WinSetWindowPos (WinWindowFromID (hwnd, 1001), NULLHANDLE, 0, SHORT2FROMMP (mp2) - dy - 1, dx, dy, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1002), NULLHANDLE, dx + 1, SHORT2FROMMP (mp2) - dy - 1, SHORT1FROMMP (mp2) - dx - 1, dy, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1004), NULLHANDLE, 0, 35, SHORT1FROMMP (mp2), doy, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1003), NULLHANDLE, 0, 0, SHORT1FROMMP (mp2), 35, SWP_SIZE|SWP_MOVE|SWP_SHOW);
#elif defined(__NT__)
         dx = (USHORT)(((float)LOWORD (lParam) / 100.0) * 65.0);
         dy = (USHORT)(HIWORD (lParam) - 34);
         doy = (USHORT)(((float)dy / 100.0) * 50.0);
         dy -= doy;
         MoveWindow (hwndMainList, 0, 0, dx, dy, TRUE);
         MoveWindow (hwndModemList, dx + 1, 0, LOWORD (lParam) - dx - 1, dy, TRUE);
         MoveWindow (hwndOutboundList, 0, dy + 1, LOWORD (lParam), doy, TRUE);
         MoveWindow (hwndStatusList, 0, HIWORD (lParam) - 33, LOWORD (lParam), 34, TRUE);
#endif
         break;
      }

#if defined(__OS2__)
      case WM_ERASEBACKGROUND:
         return ((MRESULT)TRUE);
#endif

      case WM_COMMAND:
#if defined(__OS2__)
         if (ProcessSimpleDialog (hwnd, SHORT1FROMMP (mp1)) == TRUE)
            return (0);
         if (ProcessSaveDialog (hwnd, SHORT1FROMMP (mp1)) == TRUE)
            return (0);

         switch (SHORT1FROMMP (mp1)) {
#elif defined(__NT__)
         if (LOWORD (wParam) == 1004 && HIWORD (wParam) == LBN_DBLCLK) {
            OutboundDetails (hwnd);
            return (0);
         }
         if (ProcessSimpleDialog (hwnd, (USHORT)LOWORD (wParam)) == TRUE)
            return (0);
         if (ProcessSaveDialog (hwnd, (USHORT)LOWORD (wParam)) == TRUE)
            return (0);

         switch (LOWORD (wParam)) {
#endif
            case 101:      // System / Import mail
               StopTimer (hwnd, 1);
#if defined(__OS2__)
               WinSendMsg (hwndMainClient, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 0), MPFROMP (""));
               WinSendMsg (hwndMainClient, WM_USER, MPFROM2SHORT (WMU_SETSTATUSLINE, 1), MPFROMP (""));
#elif defined(__NT__)
               SendMessage (hwndMainClient, WM_USER, (WPARAM)WMU_SETSTATUSLINE0, (LPARAM)"");
               SendMessage (hwndMainClient, WM_USER, (WPARAM)WMU_SETSTATUSLINE1, (LPARAM)"");
#endif
               Status = INITIALIZE;
               Current = 0;
#if defined(__OS2__)               
               _beginthread (MailProcessorThread, NULL, 8192, (PVOID)(MAIL_IMPORTKNOWN|MAIL_IMPORTPROTECTED|MAIL_IMPORTNORMAL|MAIL_STARTTIMER));
#elif defined(__NT__)
               _beginthread (MailProcessorThread, NULL, (PVOID)(MAIL_IMPORTKNOWN|MAIL_IMPORTPROTECTED|MAIL_IMPORTNORMAL|MAIL_STARTTIMER));
#endif               
               break;

            case 102:      // System / Export mail
               StopTimer (hwnd, 1);
#if defined(__OS2__)               
               _beginthread (MailProcessorThread, NULL, 8192, (PVOID)(MAIL_EXPORT|MAIL_STARTTIMER));
#elif defined(__NT__)
               _beginthread (MailProcessorThread, NULL, (PVOID)(MAIL_EXPORT|MAIL_STARTTIMER));
#endif               
               break;

            case 103:      // System / Pack mail
               StopTimer (hwnd, 1);
#if defined(__OS2__)               
               _beginthread (MailProcessorThread, NULL, 8192, (PVOID)(MAIL_PACK|MAIL_STARTTIMER));
#elif defined(__NT__)
               _beginthread (MailProcessorThread, NULL, (PVOID)(MAIL_PACK|MAIL_STARTTIMER));
#endif               
               break;

            case 104:      // System / Process ECHOmail
               StopTimer (hwnd, 1);
#if defined(__OS2__)               
               _beginthread (MailProcessorThread, NULL, 8192, (PVOID)(MAIL_IMPORTKNOWN|MAIL_IMPORTPROTECTED|MAIL_IMPORTNORMAL|MAIL_EXPORT|MAIL_PACK|MAIL_STARTTIMER));
#elif defined(__NT__)
               _beginthread (MailProcessorThread, NULL, (PVOID)(MAIL_IMPORTKNOWN|MAIL_IMPORTPROTECTED|MAIL_IMPORTNORMAL|MAIL_EXPORT|MAIL_PACK|MAIL_STARTTIMER));
#endif               
               break;

            case 105:      // System / Process TIC
               StopTimer (hwnd, 1);
#if defined(__OS2__)               
               _beginthread (MailProcessorThread, NULL, 8192, (PVOID)(MAIL_TIC|MAIL_STARTTIMER));
#elif defined(__NT__)
               _beginthread (MailProcessorThread, NULL, (PVOID)(MAIL_TIC|MAIL_STARTTIMER));
#endif               
               break;

            case 106:      // System / Rebuild Queue
               if (Outbound != NULL) {
                  if (Log != NULL)
                     Log->Write ("+Building the outbound queue");
                  Outbound->BuildQueue (Cfg->Outbound);
                  Outbound->FirstNode ();
                  if (Log != NULL)
                     Log->Write ("+%u queue record(s) in database", Outbound->TotalNodes);
#if defined(__OS2__)
                  WinPostMsg (hwndMainClient, WM_USER, MPFROMSHORT (WMU_REFRESHOUTBOUND), 0L);
#elif defined(__NT__)
                  PostMessage (hwndMainClient, WM_USER, (WPARAM)WMU_REFRESHOUTBOUND, 0L);
#endif
               }
               break;

            case 109: {    // System / Forced poll
               class CPollDlg *Dlg;

               if ((Dlg = new CPollDlg (hwnd)) != NULL) {
                  if (Dlg->DoModal () == TRUE) {
/*
                     strcpy (PollNode, Dlg->Address);
                     Modem->Terminal = FALSE;
                     strcpy (Modem->NodelistPath, Cfg->NodelistPath);
                     strcpy (Modem->DialCmd, Cfg->Dial);
                     Modem->Poll (PollNode);
                     Status = WAITFORCONNECT;
                     TimeOut = TimerSet ((ULONG)Cfg->DialTimeout * 100L);
*/
                  }
                  delete Dlg;
               }
               break;
            }

            case 115:      // Local login
               StopTimer (hwnd, 1);
#if defined(__OS2__)
               _beginthread (LocalThread, NULL, 32768U, NULL);
#elif defined(__NT__)
               _beginthread (LocalThread, NULL, NULL);
#endif               
               Status = HANGUP;
               break;

            case 116:      // System / Process NEWSgroups
               StopTimer (hwnd, 1);
#if defined(__OS2__)
               _beginthread (MailProcessorThread, NULL, 32768U, (PVOID)(MAIL_NEWSGROUP|MAIL_STARTTIMER));
#elid defined(__NT__)
               _beginthread (MailProcessorThread, NULL, (PVOID)(MAIL_NEWSGROUP|MAIL_STARTTIMER));
#endif
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
               StopTimer (hwnd, 1);
#if defined(__OS2__)
               _beginthread (MailProcessorThread, NULL, 32768U, (PVOID)(MAIL_EMAIL));
#elif defined(__NT__)
               _beginthread (MailProcessorThread, NULL, (PVOID)(MAIL_EMAIL));
#endif               
               break;

            case 119:      // System / Build nodelist index
               StopTimer (hwnd, 1);
#if defined(__OS2__)               
               _beginthread (NodelistThread, NULL, 8192U, (PVOID)(TRUE));
#elif defined(__NT__)
               _beginthread (NodelistThread, NULL, (PVOID)(TRUE));
#endif               
               break;

            case 120:      // System / Import from bad msgs.
               StopTimer (hwnd, 1);
#if defined(__OS2__)               
               _beginthread (MailProcessorThread, NULL, 32768U, (PVOID)(MAIL_IMPORTBAD|MAIL_STARTTIMER));
#elif defined(__NT__)
               _beginthread (MailProcessorThread, NULL, (PVOID)(MAIL_IMPORTBAD|MAIL_STARTTIMER));
#endif               
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

            case 122: {
#if defined(__OS2__)
               FILEDLG fild;
               class TAreaManager *Mgr;

               memset (&fild, 0, sizeof (FILEDLG));
               fild.cbSize = sizeof (FILEDLG);
               fild.fl = FDS_CENTER|FDS_OPEN_DIALOG;
               fild.pszTitle = "Import Descriptions";
               sprintf (fild.szFullFile, "*.*");

               WinFileDlg (HWND_DESKTOP, hwndMainClient, &fild);
               if (fild.lReturn == DID_OK) {
                  if ((Mgr = new TAreaManager) != NULL) {
                     Mgr->Cfg = Cfg;
                     Mgr->Log = Log;
                     Mgr->ImportDescriptions (fild.szFullFile);
                     delete Mgr;
                  }
               }
#elif defined(__NT__)
               CHAR Temp[128];
               OPENFILENAME OpenFileName;
               class TAreaManager *Mgr;

               OpenFileName.lStructSize = sizeof (OPENFILENAME);
               OpenFileName.hwndOwner = hwndMainClient;
               OpenFileName.hInstance = NULL;
               OpenFileName.lpstrFilter = "All files (*.*)\0*.*";
               OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
               OpenFileName.nMaxCustFilter = 0L;
               OpenFileName.nFilterIndex = 1L;
               OpenFileName.lpstrFile = Temp;
               OpenFileName.nMaxFile = sizeof (Temp) - 1;
               OpenFileName.lpstrFileTitle = NULL;
               OpenFileName.nMaxFileTitle = 0;
               OpenFileName.lpstrInitialDir = NULL;
               OpenFileName.lpstrTitle = "Import Descriptions";
               OpenFileName.nFileOffset = 0;
               OpenFileName.nFileExtension = 0;
               OpenFileName.lpstrDefExt = "";
               OpenFileName.lCustData = 0;
               OpenFileName.Flags = OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_CREATEPROMPT|OFN_NOCHANGEDIR;

               if (GetOpenFileName (&OpenFileName) == TRUE) {
                  if ((Mgr = new TAreaManager) != NULL) {
                     Mgr->Cfg = Cfg;
                     Mgr->Log = Log;
                     Mgr->ImportDescriptions (Temp);
                     delete Mgr;
                  }
               }
#endif
               break;
            }

            case 123: {
#if defined(__OS2__)
               FILEDLG fild;
               class TAreaManager *Mgr;

               memset (&fild, 0, sizeof (FILEDLG));
               fild.cbSize = sizeof (FILEDLG);
               fild.fl = FDS_CENTER|FDS_SAVEAS_DIALOG;
               fild.pszTitle = "Export Descriptions";
               sprintf (fild.szFullFile, "*.*");

               WinFileDlg (HWND_DESKTOP, hwndMainClient, &fild);
               if (fild.lReturn == DID_OK) {
                  if ((Mgr = new TAreaManager) != NULL) {
                     Mgr->Cfg = Cfg;
                     Mgr->Log = Log;
                     Mgr->ExportDescriptions (fild.szFullFile);
                     delete Mgr;
                  }
               }
#elif defined(__NT__)
               CHAR Temp[128];
               OPENFILENAME OpenFileName;
               class TAreaManager *Mgr;

               OpenFileName.lStructSize = sizeof (OPENFILENAME);
               OpenFileName.hwndOwner = hwndMainClient;
               OpenFileName.hInstance = NULL;
               OpenFileName.lpstrFilter = "All files (*.*)\0*.*";
               OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
               OpenFileName.nMaxCustFilter = 0L;
               OpenFileName.nFilterIndex = 1L;
               OpenFileName.lpstrFile = Temp;
               OpenFileName.nMaxFile = sizeof (Temp) - 1;
               OpenFileName.lpstrFileTitle = NULL;
               OpenFileName.nMaxFileTitle = 0;
               OpenFileName.lpstrInitialDir = NULL;
               OpenFileName.lpstrTitle = "Export Descriptions";
               OpenFileName.nFileOffset = 0;
               OpenFileName.nFileExtension = 0;
               OpenFileName.lpstrDefExt = "";
               OpenFileName.lCustData = 0;
               OpenFileName.Flags = OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_CREATEPROMPT|OFN_NOCHANGEDIR;

               if (GetSaveFileName (&OpenFileName) == TRUE) {
                  if ((Mgr = new TAreaManager) != NULL) {
                     Mgr->Cfg = Cfg;
                     Mgr->Log = Log;
                     Mgr->ExportDescriptions (Temp);
                     delete Mgr;
                  }
               }
#endif
               break;
            }

            case 201: {    // Global / General Options
               CHAR Title[128];
               class CGeneralDlg *Dlg;

               if ((Dlg = new CGeneralDlg (hwnd)) != NULL) {
                  if (Dlg->DoModal () == TRUE) {
                     Cfg->Save ();
                     sprintf (Title, "%s v%s%s - Line %u", NAME, VERSION, "", Cfg->TaskNumber);
#if defined(__OS2__)
                     WinSetWindowText (hwndMainFrame, Title);
#elif defined(__NT__)
                     SetWindowText (hwndMainClient, Title);
#endif
                  }
                  delete Dlg;
               }
               break;
            }

            case 501: {    // Modem / Hardware
               class CHardwareDlg *Dlg;

               if ((Dlg = new CHardwareDlg (hwnd)) != NULL) {
                  if (Dlg->DoModal () == TRUE) {
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
                           Modem->hwndWindow = hwnd;
                           Status = INITIALIZE;
                           Current = 0;
                        }
                     }
                  }
                  delete Dlg;
               }
               break;
            }

            case 502: {    // Modem / Command Strings
               class CCommandsDlg *Dlg;

               if ((Dlg = new CCommandsDlg (hwnd)) != NULL) {
                  if (Dlg->DoModal () == TRUE) {
                     Cfg->Save ();
                     Status = INITIALIZE;
                     Current = 0;
                  }
                  delete Dlg;
               }
               break;
            }

            case 506:      // Modem / Hangup
               if (Modem != NULL) {
                  Modem->SendCommand (Cfg->Hangup);
                  if (Modem->Serial != NULL && Log != NULL) {
                     if (Modem->Serial->Carrier () == TRUE)
                        Log->Write ("!Unable to drop carrier");
                  }
               }
               break;

            case 507:      // Modem / Answer Now
               if (Status == WAITFORCALL) {
                  Modem->SendCommand (Cfg->Answer);
                  Status = ANSWERING;
                  TimeOut = TimerSet (4500L);
               }
               break;

            case 601: {    // Manager / Event Scheduler
               class CEventsDlg *Dlg;

               if ((Dlg = new CEventsDlg (hwnd)) != NULL) {
                  Dlg->DoModal ();
                  delete Dlg;
               }
               if (Events != NULL)
                  Events->Load ();
               break;
            }

#if defined(__OS2__)
            case 901:
               WinHelp ("lora.hlp>h_ref", HM_HELP_CONTENTS, "LoraBBS Online Manual");
               break;
            case 902:
               WinHelp ("lora.hlp>h_ref", HM_GENERAL_HELP, "LoraBBS Online Manual");
               break;
            case 903:
               WinHelp ("lora.hlp>h_ref", HM_DISPLAY_HELP, "LoraBBS Online Manual");
               break;
#endif

            case 905: {
               class CProductDlg *Dlg;

               if ((Dlg = new CProductDlg (hwnd)) != NULL) {
                  Dlg->DoModal ();
                  delete Dlg;
               }
               break;
            }
         }
         return (0);

#if defined(__OS2__)
      case WM_CONTROL:
         if (SHORT1FROMMP (mp1) == 1004 && SHORT2FROMMP (mp1) == LN_ENTER)
            OutboundDetails (hwnd);
         break;
#endif

#if defined(__OS2__)
      case WM_CLOSE: {
#elif defined(__NT__)
      case WM_DESTROY: {
#endif
         class TStatistics *Stats;

         if ((Stats = new TStatistics) != NULL) {
            Stats->Read (Cfg->TaskNumber);
            Stats->Status = STAT_OFFLINE;
            Stats->Update ();
            delete Stats;
         }

         if (Modem != NULL && Status != BBSEXIT)
            delete Modem;
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
   }

#if defined(__OS2__)
   return (WinDefWindowProc (hwnd, msg, mp1, mp2));
#elif defined(__NT__)
   return (DefWindowProc (hwnd, msg, wParam, lParam));
#endif
}

#if !defined(__POINT__)

#if defined(__OS2__)
int main (int argc, char *argv[])
{
   int i, x, y, dx, dy;
   USHORT Task = 1, Interactive, Setup;
   USHORT DoImport, DoExport, DoPack, DoNews, DoTic, DoMail, DoNodelist;
   CHAR Title[128], *Config, *Channel, *Device;
   HMQ hmq;
   QMSG qmsg;
   ULONG flFrame, Flags, Speed;
   RECTL rc;
   HACCEL hAccel = NULLHANDLE;

   Log = NULL;
   Modem = NULL;
   Outbound = NULL;
   Events = NULL;
   Interactive = Setup = FALSE;
   DoImport = DoExport = DoPack = DoNews = DoTic = DoMail = DoNodelist = FALSE;
   Config = Channel = Device = NULL;
   Speed = 0L;
   PollNode[0] = '\0';
   gotPort = gotSpeed = FALSE;

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
      else if (!stricmp (argv[i], "TIC")) {
         DoTic = TRUE;
         Interactive = TRUE;
      }
      else if (!stricmp (argv[i], "MAIL")) {
         DoMail = TRUE;
         Interactive = TRUE;
      }
      else if (!stricmp (argv[i], "NODELIST")) {
         DoNodelist = TRUE;
         Interactive = TRUE;
      }
      else if (!stricmp (argv[i], "SETUP"))
         Setup = TRUE;
      else if (!strncmp (argv[i], "-p", 2)) {
         Device = &argv[i][2];
         gotPort = TRUE;
      }
      else if (!strncmp (argv[i], "-b", 2)) {
         Speed = atol (&argv[i][2]);
         gotSpeed = TRUE;
      }
      else if (!strncmp (argv[i], "-h", 2)) {
         comHandle = atol (&argv[i][2]);
         gotPort = TRUE;
      }
      else if (!strncmp (argv[i], "-s", 2)) {
         connectSpeed = atol (&argv[i][2]);
         gotSpeed = TRUE;
      }
      else if (!strncmp (argv[i], "-t", 2))
         timeLimit = (USHORT)atoi (&argv[i][2]);
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

   if (Device != NULL)
      strcpy (Cfg->Device, Device);
   if (Speed != 0L)
      Cfg->Speed = Speed;

   if ((hab = WinInitialize (0)) != 0) {
      if ((hmq = WinCreateMsgQueue (hab, 0)) != 0) {
         WinRegisterClass (hab, "MAIN_WINDOW", MainWinProc, CS_CLIPCHILDREN|CS_SIZEREDRAW|CS_MOVENOTIFY, 0);

         flFrame = (FCF_TASKLIST|FCF_TITLEBAR|FCF_SYSMENU|FCF_MINMAX|FCF_SIZEBORDER|FCF_NOBYTEALIGN|FCF_MENU);
         if ((hwndMainFrame = WinCreateStdWindow (HWND_DESKTOP, 0, &flFrame, "MAIN_WINDOW", NULL, 0, NULLHANDLE, 256, &hwndMainClient)) != NULLHANDLE) {
            sprintf (Title, "%s v%s%s - Line %u", NAME, VERSION, "", Cfg->TaskNumber);
            WinSetWindowText (hwndMainFrame, Title);

            WinQueryWindowRect (HWND_DESKTOP, &rc);
            dx = 575;
            if ((rc.xRight - rc.xLeft) < dx)
               dx = rc.xRight - rc.xLeft;
            dy = 330;
            if ((rc.yTop - rc.yBottom) < dy)
               dy = rc.yTop - rc.yBottom;

            x = ((rc.xRight - rc.xLeft) - dx) / 2;
            y = ((rc.yTop - rc.yBottom) - dy) / 2;

            WinSetWindowPos (hwndMainFrame, NULLHANDLE, x, y, dx, dy, SWP_SIZE|SWP_MOVE|SWP_SHOW|SWP_ACTIVATE);

            if (Interactive == FALSE) {
               if (Setup == FALSE) {
                  Status = 0;
                  WinStartTimer (hab, hwndMainClient, 1, MODEM_DELAY);
                  hAccel = WinLoadAccelTable (hab, NULLHANDLE, 1);
                  
                  _beginthread (BackgroundThread, NULL, 8192, NULL);
               }
               else
                  Status = 99;
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
               if (DoTic == TRUE)
                  Flags |= MAIL_TIC;
               if (DoMail == TRUE)
                  Flags |= MAIL_EMAIL;
               _beginthread (MailProcessorThread, NULL, 32768U, (PVOID)Flags);
            }

            while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0)) {
               if (hAccel != NULLHANDLE)
                  WinTranslateAccel (hab, hwndMainClient, hAccel, &qmsg);
               WinDispatchMsg (hab, &qmsg);
            }

            if (Interactive == FALSE) {
               if (hAccel != NULLHANDLE)
                  WinDestroyAccelTable (hAccel);
               WinStopTimer (hab, hwndMainClient, 1);
            }

            WinDestroyWindow (hwndMainFrame);
         }

         WinDestroyMsgQueue (hmq);
      }
      WinTerminate (hab);
   }

   sprintf (Title, "lexit%d.%d", Cfg->TaskNumber, ErrorLevel);
   unlink (Title);

   if (Cfg != NULL)
      delete Cfg;
   if (Events != NULL)
      delete Events;
   if (Outbound != NULL)
      delete Outbound;

   return (ErrorLevel);
}
#elif defined(__NT__)
int PASCAL WinMain (HINSTANCE hinstCurrent, HINSTANCE hinstPrevious, LPSTR lpszCmdLine, int nCmdShow)
{
   int x, y, dx, dy;
   USHORT Task = 1, Interactive, Setup;
   USHORT DoImport, DoExport, DoPack, DoNews, DoTic, DoMail, DoNodelist;
   CHAR *p, Title[128], *Config, *Channel, *Device;
   ULONG Flags, Speed;
   MSG msg;
   WNDCLASS wc;
   RECT rc;
   HACCEL hAccel;

   Log = NULL;
   Modem = NULL;
   Events = NULL;
   Outbound = NULL;
   Interactive = Setup = FALSE;
   DoImport = DoExport = DoPack = DoNews = DoTic = DoMail = DoNodelist = FALSE;
   Config = Channel = Device = NULL;
   Speed = 0L;
   lpszCmdLine = lpszCmdLine;
   PollNode[0] = '\0';
   gotPort = gotSpeed = FALSE;

   strcpy (Title, lpszCmdLine);
   if ((p = strtok (Title, " ")) != NULL)
      do {
         if (!stricmp (p, "/LINE")) {
            if ((p = strtok (NULL, " ")) != NULL)
               Task = (USHORT)atoi (p);
         }
         else if (!strncmp (p, "-n", 2))
            Task = (USHORT)atoi (&p[2]);
         else if (!stricmp (p, "IMPORT") || !stricmp (p, "IN") || !stricmp (p, "TOSS")) {
            DoImport = TRUE;
            Interactive = TRUE;
         }
         else if (!stricmp (p, "EXPORT") || !stricmp (p, "OUT") || !stricmp (p, "SCAN")) {
            DoExport = TRUE;
            Interactive = TRUE;
         }
         else if (!stricmp (p, "PACK")) {
            DoPack = TRUE;
            Interactive = TRUE;
         }
         else if (!stricmp (p, "NEWS")) {
            DoNews = TRUE;
            Interactive = TRUE;
         }
         else if (!stricmp (p, "TIC")) {
            DoTic = TRUE;
            Interactive = TRUE;
         }
         else if (!stricmp (p, "MAIL")) {
            DoMail = TRUE;
            Interactive = TRUE;
         }
         else if (!stricmp (p, "NODELIST")) {
            DoNodelist = TRUE;
            Interactive = TRUE;
         }
         else if (!stricmp (p, "SETUP")) {
            Setup = TRUE;
         }
         else if (!strncmp (p, "-p", 2)) {
            Device = &p[2];
            gotPort = TRUE;
         }
         else if (!strncmp (p, "-b", 2)) {
            Speed = atol (&p[2]);
            gotSpeed = TRUE;
         }
         else if (!strncmp (p, "-h", 2)) {
            comHandle = atol (&p[2]);
            gotPort = TRUE;
         }
         else if (!strncmp (p, "-s", 2))
            connectSpeed = atol (&p[2]);
         else if (!strncmp (p, "-t", 2))
            timeLimit = (USHORT)atoi (&p[2]);
         else if (Config == NULL)
            Config = p;
         else if (Channel == NULL)
            Channel = p;
      } while ((p = strtok (NULL, " ")) != NULL);

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

   if (Device != NULL)
      strcpy (Cfg->Device, Device);
   if (Speed != 0L)
      Cfg->Speed = Speed;

   if (hinstPrevious == NULL) {
      wc.style         = CS_DBLCLKS;
      wc.lpfnWndProc   = MainWinProc;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.hInstance     = hinstCurrent;
      wc.hIcon         = LoadIcon (NULL, "100");
      wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
      wc.lpszMenuName  = "MENU_1";
      wc.lpszClassName = "MAIN_WINDOW";

      if (!RegisterClass (&wc))
         return (FALSE);
   }

   hinst = hinstCurrent;
   msg.wParam = FALSE;

   GetClientRect (GetDesktopWindow (), &rc);
   dx = 575;
   if ((rc.right - rc.left) < dx)
      dx = rc.right - rc.left;
   dy = 330;
   if ((rc.bottom - rc.top) < dy)
      dy = rc.bottom - rc.top;

   x = ((rc.right - rc.left) - dx) / 2;
   y = ((rc.bottom - rc.top) - dy) / 2;

   sprintf (Title, "%s v%s%s - Line %u", NAME, VERSION, "", Cfg->TaskNumber);

   if ((hwndMainClient = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW, "MAIN_WINDOW", Title, WS_OVERLAPPEDWINDOW, x, y, dx, dy, NULL, NULL, hinstCurrent, NULL)) != NULL) {
      ShowWindow (hwndMainClient, nCmdShow);

      if (Interactive == FALSE) {
         if (Setup == FALSE) {
            Status = 0;
            SetTimer (hwndMainClient, 1, MODEM_DELAY, NULL);
            _beginthread (BackgroundThread, NULL, NULL);
         }
         else
            Status = 99;
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
         if (DoTic == TRUE)
            Flags |= MAIL_TIC;
         if (DoMail == TRUE)
            Flags |= MAIL_EMAIL;
         _beginthread (MailProcessorThread, NULL, (PVOID)Flags);
      }

      hAccel = LoadAccelerators (hinstCurrent, "ACCELERATOR_1");

      while (GetMessage (&msg, NULL, 0, 0)) {
         if (!TranslateAccelerator (hwndMainClient, hAccel, &msg)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
         }
      }

      if (Interactive == FALSE)
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

#endif

