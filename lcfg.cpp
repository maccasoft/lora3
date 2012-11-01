
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

#if defined(__OS2__)
#define IDOK            1
#define IDCANCEL        2
#endif

PSZ SpeedArray[] = { "300", "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

typedef struct {
   USHORT cbData;
   class  TUser *Data;
} USERCFG;

// ----------------------------------------------------------------------
// File requests
// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (COkFileDlg, CDialog)
   ON_COMMAND (106, Add)
   ON_COMMAND (107, Remove)
   ON_COMMAND (108, Change)
#if defined(__OS2__)
   ON_CONTROL (CN_EMPHASIS, 101, SelChanged)
#elif defined(__NT__)
   ON_NOTIFY (NM_CLICK, 101, SelChanged)
#endif
END_MESSAGE_MAP ()

COkFileDlg::COkFileDlg (HWND p_hWnd) : CDialog ("53", p_hWnd)
{
}

VOID COkFileDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 6);
}

USHORT COkFileDlg::OnInitDialog (VOID)
{
   CHAR Temp[16];
   class TOkFile *Data;

   Center ();

   EM_SetTextLimit (103, sizeof (Data->Name) - 1);
   EM_SetTextLimit (105, sizeof (Data->Path) - 1);
   EM_SetTextLimit (114, sizeof (Data->Pwd) - 1);

   LVM_AllocateColumns (101, 4);
   LVM_InsertColumn (101, "File", LVC_LEFT);
   LVM_InsertColumn (101, "Flags", LVC_LEFT);
   LVM_InsertColumn (101, "Path", LVC_LEFT);
   LVM_InsertColumn (101, "Password", LVC_LEFT);

   if ((Data = new TOkFile (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            LVM_InsertItem (101);

            LVM_SetItemText (101, 0, Data->Name);
            Temp[0] = '\0';
            if (Data->Normal == TRUE)
               strcat (Temp, "N");
            if (Data->Known == TRUE)
               strcat (Temp, "K");
            if (Data->Protected == TRUE)
               strcat (Temp, "P");
            LVM_SetItemText (101, 1, Temp);
            LVM_SetItemText (101, 2, Data->Path);
            LVM_SetItemText (101, 3, Data->Pwd);
         } while (Data->Next () == TRUE);
      delete Data;
   }

   LVM_InvalidateView (101);

   return (TRUE);
}

VOID COkFileDlg::SelChanged (VOID)
{
   USHORT item;
   CHAR Temp[128];

   if ((item = (USHORT)LVM_QuerySelectedItem (101)) != (USHORT)-1) {
      LVM_QueryItemText (101, item, 0, Temp);
      SetDlgItemText (103, Temp);
      LVM_QueryItemText (101, item, 1, Temp);
      if (strchr (Temp, 'N') != NULL)
         BM_SetCheck (110, TRUE);
      else
         BM_SetCheck (110, FALSE);
      if (strchr (Temp, 'K') != NULL)
         BM_SetCheck (111, TRUE);
      else
         BM_SetCheck (111, FALSE);
      if (strchr (Temp, 'P') != NULL)
         BM_SetCheck (112, TRUE);
      else
         BM_SetCheck (112, FALSE);
      LVM_QueryItemText (101, item, 2, Temp);
      SetDlgItemText (105, Temp);
      LVM_QueryItemText (101, item, 3, Temp);
      SetDlgItemText (114, Temp);
   }
   else {
      // Se non e' stato selezionato un elemento, pulisce i campi della dialog.
      SetDlgItemText (103, "");
      BM_SetCheck (110, FALSE);
      BM_SetCheck (111, FALSE);
      BM_SetCheck (112, FALSE);
      SetDlgItemText (105, "");
      SetDlgItemText (114, "");
   }
}

VOID COkFileDlg::OnOK (VOID)
{
   USHORT i, count;
   CHAR Temp[128];
   class TOkFile *Data;

   if ((Data = new TOkFile (Cfg->SystemPath)) != NULL) {
      Data->DeleteAll ();
      count = (USHORT)LVM_QueryItemCount (101);
      for (i = 0; i < count; i++) {
         LVM_QueryItemText (101, i, 0, Temp);
         strcpy (Data->Name, Temp);
         LVM_QueryItemText (101, i, 1, Temp);
         if (strchr (Temp, 'N') != NULL)
            Data->Normal = TRUE;
         if (strchr (Temp, 'K') != NULL)
            Data->Known = TRUE;
         if (strchr (Temp, 'P') != NULL)
            Data->Protected = TRUE;
         LVM_QueryItemText (101, i, 2, Temp);
         strcpy (Data->Path, Temp);
         LVM_QueryItemText (101, i, 3, Temp);
         strcpy (Data->Pwd, Temp);
         Data->Add ();
      }
      delete Data;
   }

   EndDialog (TRUE);
}

VOID COkFileDlg::Add (VOID)
{
   CHAR Temp[128];

   LVM_InsertItem (101);

   GetDlgItemText (103, GetDlgItemTextLength (103), Temp);
   LVM_SetItemText (101, 0, Temp);
   Temp[0] = '\0';
   if (BM_QueryCheck (110) == TRUE)
      strcat (Temp, "N");
   if (BM_QueryCheck (111) == TRUE)
      strcat (Temp, "K");
   if (BM_QueryCheck (112) == TRUE)
      strcat (Temp, "P");
   LVM_SetItemText (101, 1, Temp);
   GetDlgItemText (105, GetDlgItemTextLength (105), Temp);
   LVM_SetItemText (101, 2, Temp);
   GetDlgItemText (114, GetDlgItemTextLength (114), Temp);
   LVM_SetItemText (101, 3, Temp);

   LVM_InvalidateView (101);
}

VOID COkFileDlg::Change (VOID)
{
   USHORT item;
   CHAR Temp[128];

   if ((item = (USHORT)LVM_QuerySelectedItem (101)) != -1) {
      GetDlgItemText (103, GetDlgItemTextLength (103), Temp);
      LVM_SetItemText (101, item, 0, Temp);
      Temp[0] = '\0';
      if (BM_QueryCheck (110) == TRUE)
         strcat (Temp, "N");
      if (BM_QueryCheck (111) == TRUE)
         strcat (Temp, "K");
      if (BM_QueryCheck (112) == TRUE)
         strcat (Temp, "P");
      LVM_SetItemText (101, item, 1, Temp);
      GetDlgItemText (105, GetDlgItemTextLength (105), Temp);
      LVM_SetItemText (101, item, 2, Temp);
      GetDlgItemText (114, GetDlgItemTextLength (114), Temp);
      LVM_SetItemText (101, item, 3, Temp);

      LVM_InvalidateView (101);
   }
}

VOID COkFileDlg::Remove (VOID)
{
   LVM_DeleteItem (101, LVM_QuerySelectedItem (101));
}

// ----------------------------------------------------------------------
// Mailer addresses
// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CAddressDlg, CDialog)
   ON_COMMAND (102, Add)
   ON_COMMAND (103, Remove)
   ON_COMMAND (104, Change)
#if defined(__OS2__)
   ON_CONTROL (LN_SELECT, 101, SelChanged)
#elif defined(__NT__)
   ON_NOTIFY (LBN_SELCHANGE, 101, SelChanged)
#endif
END_MESSAGE_MAP ()

CAddressDlg::CAddressDlg (HWND p_hWnd) : CDialog ("6", p_hWnd)
{
}

VOID CAddressDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 6);
}

USHORT CAddressDlg::OnInitDialog (VOID)
{
   CHAR Temp[64];

   Center ();

   EM_SetTextLimit (108, sizeof (Cfg->MailAddress.String) - 1);
   if (Cfg->MailAddress.First () == TRUE)
      do {
         if (Cfg->MailAddress.FakeNet != 0)
            sprintf (Temp, "%s (%u)", Cfg->MailAddress.String, Cfg->MailAddress.FakeNet);
         else
            strcpy (Temp, Cfg->MailAddress.String);
         LM_AddString (101, Temp);
      } while (Cfg->MailAddress.Next () == TRUE);

   return (TRUE);
}

VOID CAddressDlg::SelChanged (VOID)
{
   USHORT item;
   CHAR Temp[64], Fake[32], *p;
   class TAddress Addr;

   if ((item = LM_QuerySelection (101)) >= 0) {
      LM_QueryItemText (101, item, sizeof (Temp) - 1, Temp);
      Addr.Parse (Temp);
      SetDlgItemText (108, Addr.String);
      if ((p = strchr (Temp, '(')) != NULL) {
         sprintf (Fake, "%u", atoi (++p));
         SetDlgItemText (109, Fake);
      }
      else
         SetDlgItemText (109, "0");
   }
   else {
      SetDlgItemText (108, "");
      SetDlgItemText (109, "");
   }
}

VOID CAddressDlg::OnOK (VOID)
{
   USHORT i, Max;
   CHAR Temp[64], *p;

   Cfg->MailAddress.Clear ();
   if ((Max = LM_QueryItemCount (101)) > 0) {
      for (i = 0; i < Max; i++) {
         LM_QueryItemText (101, i, sizeof (Temp) - 1, Temp);
         Cfg->MailAddress.Add (Temp);
         if ((p = strchr (Temp, '(')) != NULL) {
            Cfg->MailAddress.FakeNet = (USHORT)atoi (++p);
            Cfg->MailAddress.Update ();
         }
      }
   }

   EndDialog (TRUE);
}

VOID CAddressDlg::Add (VOID)
{
   CHAR Temp[64];
   class TAddress Addr;

   GetDlgItemText (108, GetDlgItemTextLength (108), Temp);
   if (Temp[0] != '\0') {
      Addr.Parse (Temp);
      GetDlgItemText (109, GetDlgItemTextLength (109), Temp);
      Addr.FakeNet = (USHORT)atoi (Temp);
      if (Addr.FakeNet != 0)
         sprintf (Temp, "%s (%u)", Addr.String, Addr.FakeNet);
      else
         strcpy (Temp, Addr.String);
      LM_AddString (101, Temp);
   }

   SetFocus (108);
   SelChanged ();
}

VOID CAddressDlg::Change (VOID)
{
   USHORT item;
   CHAR Temp[64];
   class TAddress Addr;

   if ((item = LM_QuerySelection (101)) >= 0) {
      GetDlgItemText (108, GetDlgItemTextLength (108), Temp);
      if (Temp[0] != '\0') {
         Addr.Parse (Temp);
         GetDlgItemText (109, GetDlgItemTextLength (109), Temp);
         Addr.FakeNet = (USHORT)atoi (Temp);
         if (Addr.FakeNet != 0)
            sprintf (Temp, "%s (%u)", Addr.String, Addr.FakeNet);
         else
            strcpy (Temp, Addr.String);
         LM_SetItemText (101, item, Temp);
      }

      SelChanged ();
      SetFocus (108);
   }
}

VOID CAddressDlg::Remove (VOID)
{
   LM_DeleteItem (101, LM_QuerySelection (101));
   SelChanged ();
}

// ----------------------------------------------------------------------
// Directories / Paths
// ----------------------------------------------------------------------

CDirectoriesDlg::CDirectoriesDlg (HWND p_hWnd) : CDialog ("5", p_hWnd)
{
}

VOID CDirectoriesDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 5);
}

USHORT CDirectoriesDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (110, sizeof (Cfg->SystemPath) - 1);
   EM_SetTextLimit (102, sizeof (Cfg->NormalInbound) - 1);
   EM_SetTextLimit (118, sizeof (Cfg->KnownInbound) - 1);
   EM_SetTextLimit (120, sizeof (Cfg->ProtectedInbound) - 1);
   EM_SetTextLimit (114, sizeof (Cfg->Outbound) - 1);
   EM_SetTextLimit (106, sizeof (Cfg->NodelistPath) - 1L);
   EM_SetTextLimit (122, sizeof (Cfg->HudsonPath) - 1L);

   SetDlgItemText (110, Cfg->SystemPath);
   SetDlgItemText (102, Cfg->NormalInbound);
   SetDlgItemText (118, Cfg->KnownInbound);
   SetDlgItemText (120, Cfg->ProtectedInbound);
   SetDlgItemText (114, Cfg->Outbound);
   SetDlgItemText (106, Cfg->NodelistPath);
   SetDlgItemText (122, Cfg->HudsonPath);

   return (TRUE);
}

VOID CDirectoriesDlg::OnOK (VOID)
{
   GetDlgItemText (110, GetDlgItemTextLength (110), Cfg->SystemPath);
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->NormalInbound);
   GetDlgItemText (118, GetDlgItemTextLength (118), Cfg->KnownInbound);
   GetDlgItemText (120, GetDlgItemTextLength (120), Cfg->ProtectedInbound);
   GetDlgItemText (114, GetDlgItemTextLength (114), Cfg->Outbound);
   GetDlgItemText (106, GetDlgItemTextLength (106), Cfg->NodelistPath);
   GetDlgItemText (122, GetDlgItemTextLength (122), Cfg->HudsonPath);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Modem commands
// ----------------------------------------------------------------------

CCommandsDlg::CCommandsDlg (HWND p_hWnd) : CDialog ("2", p_hWnd)
{
}

VOID CCommandsDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 2);
}

USHORT CCommandsDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Cfg->Initialize[0]) - 1);
   EM_SetTextLimit (103, sizeof (Cfg->Initialize[1]) - 1);
   EM_SetTextLimit (104, sizeof (Cfg->Initialize[2]) - 1);
   EM_SetTextLimit (105, sizeof (Cfg->Dial) - 1);
   EM_SetTextLimit (106, sizeof (Cfg->Hangup) - 1);
   EM_SetTextLimit (107, sizeof (Cfg->OffHook) - 1);

   SetDlgItemText (102, Cfg->Initialize[0]);
   SetDlgItemText (103, Cfg->Initialize[1]);
   SetDlgItemText (104, Cfg->Initialize[2]);
   SetDlgItemText (105, Cfg->Dial);
   SetDlgItemText (106, Cfg->Hangup);
   SetDlgItemText (107, Cfg->OffHook);

   return (TRUE);
}

VOID CCommandsDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->Initialize[0]);
   GetDlgItemText (103, GetDlgItemTextLength (103), Cfg->Initialize[1]);
   GetDlgItemText (104, GetDlgItemTextLength (104), Cfg->Initialize[2]);
   GetDlgItemText (105, GetDlgItemTextLength (105), Cfg->Dial);
   GetDlgItemText (106, GetDlgItemTextLength (106), Cfg->Hangup);
   GetDlgItemText (107, GetDlgItemTextLength (107), Cfg->OffHook);

   EndDialog (TRUE);
}

#if !defined(__POINT__)
// ----------------------------------------------------------------------
// Answer control
// ----------------------------------------------------------------------

CAnswerDlg::CAnswerDlg (HWND p_hWnd) : CDialog ("43", p_hWnd)
{
}

VOID CAnswerDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 43);
}

USHORT CAnswerDlg::OnInitDialog (VOID)
{
   CHAR Temp[16];

   Center ();

   EM_SetTextLimit (103, sizeof (Cfg->Answer) - 1);
   EM_SetTextLimit (106, 5);
   EM_SetTextLimit (108, 5);

   BM_SetCheck (101, Cfg->ManualAnswer);
   SetDlgItemText (103, Cfg->Answer);
   BM_SetCheck (104, Cfg->LimitedHours);
   sprintf (Temp, "%02d:%02d", Cfg->StartTime / 60, Cfg->StartTime % 60);
   SetDlgItemText (106, Temp);
   sprintf (Temp, "%02d:%02d", Cfg->EndTime / 60, Cfg->EndTime % 60);
   SetDlgItemText (108, Temp);

   return (TRUE);
}

VOID CAnswerDlg::OnOK (VOID)
{
   CHAR Temp[16], *p;

   Cfg->ManualAnswer = (UCHAR)BM_QueryCheck (101);
   GetDlgItemText (103, GetDlgItemTextLength (103), Cfg->Answer);
   Cfg->LimitedHours = (UCHAR)BM_QueryCheck (104);
   GetDlgItemText (106, GetDlgItemTextLength (106), Temp);
   if ((p = strtok (Temp, ":")) != NULL) {
      Cfg->StartTime = (USHORT)(atoi (p) * 60);
      if ((p = strtok (NULL, ":")) != NULL)
         Cfg->StartTime += (USHORT)atoi (p);
   }
   GetDlgItemText (108, GetDlgItemTextLength (108), Temp);
   if ((p = strtok (Temp, ":")) != NULL) {
      Cfg->EndTime = (USHORT)(atoi (p) * 60);
      if ((p = strtok (NULL, ":")) != NULL)
         Cfg->EndTime += (USHORT)atoi (p);
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Events editor
// ----------------------------------------------------------------------

class CEventsListDlg : public CDialog
{
public:
   CEventsListDlg (HWND p_hWnd);

   int    Selected;
   class  TEvents *Data;

   VOID   OnCancel (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   DECLARE_MESSAGE_MAP ()
};

class CAddEventDlg : public CDialog
{
public:
   CAddEventDlg (HWND p_hWnd);

   CHAR   Label[32];
   UCHAR  Hour, Minute;
   UCHAR  Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday;
   USHORT Length, LastDay;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

BEGIN_MESSAGE_MAP (CEventsDlg, CDialog)
   ON_COMMAND (146, Add)
   ON_COMMAND (147, Delete)
   ON_COMMAND (149, Previous)
   ON_COMMAND (148, List)
   ON_COMMAND (150, Next)
END_MESSAGE_MAP ()

CEventsDlg::CEventsDlg (HWND p_hWnd) : CDialog ("11", p_hWnd)
{
   Data = NULL;
}

CEventsDlg::~CEventsDlg (void)
{
   if (Data != NULL) {
      Data->Save ();
      delete Data;
   }
}

VOID CEventsDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 11);
}

USHORT CEventsDlg::OnInitDialog (VOID)
{
   Center ();

   if ((Data = new TEvents (Cfg->SchedulerFile)) != NULL)
      Data->Load ();

   EM_SetTextLimit (102, sizeof (Data->Label) - 1);
   EM_SetTextLimit (130, sizeof (Data->Address) - 1);
   EM_SetTextLimit (132, sizeof (Data->RouteCmd) - 1);
   EM_SetTextLimit (152, sizeof (Data->Command) - 1);

   SPBM_SetLimits (112, 23, 0);
   SPBM_SetLimits (114, 59, 0);
   SPBM_SetLimits (116, 1440, 0);
   SPBM_SetLimits (127, 60, 0);
   SPBM_SetLimits (123, 100, 0);
   SPBM_SetLimits (125, 100, 0);

   if (Data->First () == FALSE)
      Data->New ();

   DisplayData ();

   return (TRUE);
}

VOID CEventsDlg::OnOK (VOID)
{
   Data->Delete ();
   Data->New ();
   ReadData ();
   Data->Add ();
   SetFocus (102);
}

VOID CEventsDlg::DisplayData (VOID)
{
   SetDlgItemText (102, Data->Label);
   SPBM_SetCurrentValue (112, Data->Hour);
   SPBM_SetCurrentValue (114, Data->Minute);
   SPBM_SetCurrentValue (116, Data->Length);
   BM_SetCheck (104, Data->Sunday);
   BM_SetCheck (105, Data->Monday);
   BM_SetCheck (106, Data->Tuesday);
   BM_SetCheck (107, Data->Wednesday);
   BM_SetCheck (108, Data->Thursday);
   BM_SetCheck (109, Data->Friday);
   BM_SetCheck (110, Data->Saturday);
   BM_SetCheck (118, Data->Force);
   BM_SetCheck (119, Data->MailOnly);
   BM_SetCheck (117, Data->Dynamic);
   BM_SetCheck (133, Data->AllowRequests);
   BM_SetCheck (134, Data->MakeRequests);
   BM_SetCheck (135, Data->SendNormal);
   BM_SetCheck (137, Data->SendCrash);
   BM_SetCheck (136, Data->SendDirect);
   BM_SetCheck (138, Data->SendImmediate);
   SPBM_SetCurrentValue (127, Data->CallDelay);
   SPBM_SetCurrentValue (123, Data->MaxCalls);
   SPBM_SetCurrentValue (125, Data->MaxConnects);
   SetDlgItemText (130, Data->Address);
   BM_SetCheck (128, Data->ForceCall);
   BM_SetCheck (142, Data->ExportMail);
   SetDlgItemText (132, Data->RouteCmd);
   SetDlgItemText (152, Data->Command);
   BM_SetCheck (139, Data->ImportNormal);
   BM_SetCheck (140, Data->ImportProtected);
   BM_SetCheck (141, Data->ImportKnown);
   BM_SetCheck (143, Data->StartImport);
   BM_SetCheck (144, Data->StartExport);
   BM_SetCheck (145, Data->ProcessTIC);
   BM_SetCheck (120, Data->ClockAdjustment);
}

VOID CEventsDlg::ReadData (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Data->Label);
   Data->Hour = (UCHAR)SPBM_QueryValue (112);
   Data->Minute = (UCHAR)SPBM_QueryValue (114);
   Data->Length = (USHORT)SPBM_QueryValue (116);
   Data->Sunday = (UCHAR)BM_QueryCheck (104);
   Data->Monday = (UCHAR)BM_QueryCheck (105);
   Data->Tuesday = (UCHAR)BM_QueryCheck (106);
   Data->Wednesday = (UCHAR)BM_QueryCheck (107);
   Data->Thursday = (UCHAR)BM_QueryCheck (108);
   Data->Friday = (UCHAR)BM_QueryCheck (109);
   Data->Saturday = (UCHAR)BM_QueryCheck (110);
   Data->Force = (UCHAR)BM_QueryCheck (118);
   Data->MailOnly = (UCHAR)BM_QueryCheck (119);
   Data->Dynamic = (UCHAR)BM_QueryCheck (117);
   Data->AllowRequests = (UCHAR)BM_QueryCheck (133);
   Data->MakeRequests = (UCHAR)BM_QueryCheck (134);
   Data->SendNormal = (UCHAR)BM_QueryCheck (135);
   Data->SendCrash = (UCHAR)BM_QueryCheck (137);
   Data->SendDirect = (UCHAR)BM_QueryCheck (136);
   Data->SendImmediate = (UCHAR)BM_QueryCheck (138);
   Data->CallDelay = (UCHAR)SPBM_QueryValue (127);
   Data->MaxCalls = (UCHAR)SPBM_QueryValue (123);
   Data->MaxConnects = (UCHAR)SPBM_QueryValue (125);
   GetDlgItemText (130, GetDlgItemTextLength (130), Data->Address);
   Data->ForceCall = (UCHAR)BM_QueryCheck (128);
   Data->ExportMail = (UCHAR)BM_QueryCheck (142);
   GetDlgItemText (132, GetDlgItemTextLength (132), Data->RouteCmd);
   GetDlgItemText (152, GetDlgItemTextLength (152), Data->Command);
   Data->ImportNormal = (UCHAR)BM_QueryCheck (139);
   Data->ImportProtected = (UCHAR)BM_QueryCheck (140);
   Data->ImportKnown = (UCHAR)BM_QueryCheck (141);
   Data->StartImport = (UCHAR)BM_QueryCheck (143);
   Data->StartExport = (UCHAR)BM_QueryCheck (144);
   Data->ProcessTIC = (UCHAR)BM_QueryCheck (145);
   Data->ClockAdjustment = (UCHAR)BM_QueryCheck (120);
}

VOID CEventsDlg::Add (VOID)
{
   class CAddEventDlg *Dlg;

   Data->Delete ();
   Data->New ();
   ReadData ();
   Data->Add ();

   if ((Dlg = new CAddEventDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         Data->New ();
         strcpy (Data->Label, Dlg->Label);
         Data->Hour = Dlg->Hour;
         Data->Minute = Dlg->Minute;
         Data->Length = Dlg->Length;
         Data->Sunday = Dlg->Sunday;
         Data->Monday = Dlg->Monday;
         Data->Tuesday = Dlg->Tuesday;
         Data->Wednesday = Dlg->Wednesday;
         Data->Thursday = Dlg->Thursday;
         Data->Friday = Dlg->Friday;
         Data->Saturday = Dlg->Saturday;
         Data->Add ();
         DisplayData ();
      }
      delete Dlg;
   }
   SetFocus (102);
}

VOID CEventsDlg::Delete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      Data->Delete ();
      DisplayData ();
   }
   SetFocus (102);
}

VOID CEventsDlg::Next (VOID)
{
   if (Data->Next () == TRUE)
      DisplayData ();
   SetFocus (102);
}

VOID CEventsDlg::Previous (VOID)
{
   if (Data->Previous () == TRUE)
      DisplayData ();
   SetFocus (102);
}

VOID CEventsDlg::List (VOID)
{
   class CEventsListDlg *Dlg;

   Data->Save ();
   if ((Dlg = new CEventsListDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      if (Dlg->DoModal () == TRUE)
         DisplayData ();
      delete Dlg;
   }
   SetFocus (102);
}

// ----------------------------------------------------------------------

CAddEventDlg::CAddEventDlg (HWND p_hWnd) : CDialog ("51", p_hWnd)
{
}

USHORT CAddEventDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Label) - 1);
   SPBM_SetLimits (112, 23, 0);
   SPBM_SetLimits (114, 59, 0);
   SPBM_SetLimits (116, 1440, 0);

   return (TRUE);
}

VOID CAddEventDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Label);
   Hour = (UCHAR)SPBM_QueryValue (112);
   Minute = (UCHAR)SPBM_QueryValue (114);
   Length = (USHORT)SPBM_QueryValue (116);
   Sunday = (UCHAR)BM_QueryCheck (104);
   Monday = (UCHAR)BM_QueryCheck (105);
   Tuesday = (UCHAR)BM_QueryCheck (106);
   Wednesday = (UCHAR)BM_QueryCheck (107);
   Thursday = (UCHAR)BM_QueryCheck (108);
   Friday = (UCHAR)BM_QueryCheck (109);
   Saturday = (UCHAR)BM_QueryCheck (110);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CEventsListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
END_MESSAGE_MAP ()

CEventsListDlg::CEventsListDlg (HWND p_hWnd) : CDialog ("25", p_hWnd)
{
   Selected = -1;
}

USHORT CEventsListDlg::OnInitDialog (VOID)
{
   int i = 1;
   CHAR Temp[64];

   SetWindowTitle ("Events list");

   LVM_AllocateColumns (101, 6);
   LVM_InsertColumn (101, "#", LVC_RIGHT);
   LVM_InsertColumn (101, "Title", LVC_LEFT);
   LVM_InsertColumn (101, "Days", LVC_LEFT);
   LVM_InsertColumn (101, "Start", LVC_RIGHT);
   LVM_InsertColumn (101, "Length", LVC_RIGHT);
   LVM_InsertColumn (101, "Command", LVC_LEFT);

   if (Data != NULL) {
      Selected = Data->Number;

      Data->Load ();
      if (Data->First () == TRUE)
         do {
            LVM_InsertItem (101);

            sprintf (Temp, "%d", i++);
            LVM_SetItemText (101, 0, Temp);
            LVM_SetItemText (101, 1, Data->Label);
            strcpy (Temp, "-------");
            if (Data->Sunday == TRUE)
               Temp[0] = 'S';
            if (Data->Monday == TRUE)
               Temp[1] = 'M';
            if (Data->Tuesday == TRUE)
               Temp[2] = 'T';
            if (Data->Wednesday == TRUE)
               Temp[3] = 'W';
            if (Data->Thursday == TRUE)
               Temp[4] = 'T';
            if (Data->Friday == TRUE)
               Temp[5] = 'F';
            if (Data->Saturday == TRUE)
               Temp[6] = 'S';
            LVM_SetItemText (101, 2, Temp);
            sprintf (Temp, "%d:%02d", Data->Hour, Data->Minute);
            LVM_SetItemText (101, 3, Temp);
            sprintf (Temp, "%d", Data->Length);
            LVM_SetItemText (101, 4, Temp);
            LVM_SetItemText (101, 5, Data->Command);
         } while (Data->Next () == TRUE);
   }

   LVM_InvalidateView (101);

   if (Selected > 0)
      LVM_SelectItem (101, Selected - 1);

   return (TRUE);
}

VOID CEventsListDlg::OnCancel (VOID)
{
   Data->Read ((USHORT)Selected);
   EndDialog (FALSE);
}

VOID CEventsListDlg::OnOK (VOID)
{
   int item;

   Data->Read ((USHORT)Selected);
   if ((item = LVM_QuerySelectedItem (101)) != -1)
      Data->Read ((USHORT)(item + 1));

   EndDialog (TRUE);
}
#endif

// ----------------------------------------------------------------------
// General options
// ----------------------------------------------------------------------

CGeneralDlg::CGeneralDlg (HWND p_hWnd) : CDialog ("7", p_hWnd)
{
}

VOID CGeneralDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 7);
}

USHORT CGeneralDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Cfg->LogFile) - 1);
   EM_SetTextLimit (109, sizeof (Cfg->SchedulerFile) - 1);
   SPBM_SetLimits (104, 255, 1);
   EM_SetTextLimit (112, sizeof (Cfg->FaxCommand) - 1);
   EM_SetTextLimit (106, sizeof (Cfg->AfterCallerCmd) - 1);
   EM_SetTextLimit (110, sizeof (Cfg->AfterMailCmd) - 1);

   SetDlgItemText (102, Cfg->LogFile);
   SPBM_SetCurrentValue (104, Cfg->TaskNumber);
   SetDlgItemText (109, Cfg->SchedulerFile);
   SetDlgItemText (112, Cfg->FaxCommand);
   SetDlgItemText (106, Cfg->AfterCallerCmd);
   SetDlgItemText (110, Cfg->AfterMailCmd);
   BM_SetCheck (113, Cfg->ZModemTelnet);
   BM_SetCheck (114, Cfg->ReloadLog);
   BM_SetCheck (115, Cfg->MakeProcessLog);

   return (TRUE);
}

VOID CGeneralDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->LogFile);
   Cfg->TaskNumber = (USHORT)SPBM_QueryValue (104);
   GetDlgItemText (109, GetDlgItemTextLength (109), Cfg->SchedulerFile);
   GetDlgItemText (112, GetDlgItemTextLength (112), Cfg->FaxCommand);
   GetDlgItemText (106, GetDlgItemTextLength (106), Cfg->AfterCallerCmd);
   GetDlgItemText (110, GetDlgItemTextLength (110), Cfg->AfterMailCmd);
   Cfg->ZModemTelnet = (UCHAR)BM_QueryCheck (113);
   Cfg->ReloadLog = (UCHAR)BM_QueryCheck (114);
   Cfg->MakeProcessLog = (UCHAR)BM_QueryCheck (115);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Fax Options
// ----------------------------------------------------------------------

CFaxOptDlg::CFaxOptDlg (HWND p_hWnd) : CDialog ("48", p_hWnd)
{
}

VOID CFaxOptDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 48);
}

USHORT CFaxOptDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (112, sizeof (Cfg->FaxCommand) - 1);
   EM_SetTextLimit (103, sizeof (Cfg->FaxPath) - 1);
   EM_SetTextLimit (105, sizeof (Cfg->AfterFaxCmd) - 1);

   BM_SetCheck (101, Cfg->ExternalFax);
   SetDlgItemText (112, Cfg->FaxCommand);
   SetDlgItemText (103, Cfg->FaxPath);
   SetDlgItemText (105, Cfg->AfterFaxCmd);

   return (TRUE);
}

VOID CFaxOptDlg::OnOK (VOID)
{
   Cfg->ExternalFax = (UCHAR)BM_QueryCheck (101);
   GetDlgItemText (112, GetDlgItemTextLength (112), Cfg->FaxCommand);
   GetDlgItemText (103, GetDlgItemTextLength (103), Cfg->FaxPath);
   GetDlgItemText (105, GetDlgItemTextLength (105), Cfg->AfterFaxCmd);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Modem hardware
// ----------------------------------------------------------------------

CHardwareDlg::CHardwareDlg (HWND p_hWnd) : CDialog ("1", p_hWnd)
{
}

USHORT CHardwareDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Cfg->Device) - 1);
   EM_SetTextLimit (108, sizeof (Cfg->FaxMessage) - 1);
   EM_SetTextLimit (116, sizeof (Cfg->Ring) - 1);

   CB_AddString (104, "300");
   CB_AddString (104, "1200");
   CB_AddString (104, "2400");
   CB_AddString (104, "4800");
   CB_AddString (104, "9600");
   CB_AddString (104, "19200");
   CB_AddString (104, "38400");
   CB_AddString (104, "57600");
   CB_AddString (104, "115200");

   SPBM_SetLimits (112, 120, 1);
   SPBM_SetLimits (114, 5, 1);

   SetDlgItemText (102, Cfg->Device);
   switch (Cfg->Speed) {
      case 300L:
         CB_SelectItem (104, 0);
         break;
      case 1200L:
         CB_SelectItem (104, 1);
         break;
      case 2400L:
         CB_SelectItem (104, 2);
         break;
      case 4800L:
         CB_SelectItem (104, 3);
         break;
      case 9600L:
         CB_SelectItem (104, 4);
         break;
      case 19200L:
         CB_SelectItem (104, 5);
         break;
      case 38400L:
         CB_SelectItem (104, 6);
         break;
      case 57600L:
         CB_SelectItem (104, 7);
         break;
      case 115200L:
         CB_SelectItem (104, 8);
         break;
   }
   SetDlgItemText (108, Cfg->FaxMessage);
   SetDlgItemText (116, Cfg->Ring);
   SPBM_SetCurrentValue (112, Cfg->DialTimeout);
   SPBM_SetCurrentValue (114, Cfg->CarrierDropTimeout);
   BM_SetCheck (105, Cfg->LockSpeed);
   BM_SetCheck (110, Cfg->StripDashes);

   return (TRUE);
}

VOID CHardwareDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 1);
}

VOID CHardwareDlg::OnOK (VOID)
{
   CHAR Temp[32];

   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->Device);
   GetDlgItemText (104, GetDlgItemTextLength (104), Temp);
   Cfg->Speed = atol (Temp);
   GetDlgItemText (108, GetDlgItemTextLength (108), Cfg->FaxMessage);
   GetDlgItemText (116, GetDlgItemTextLength (116), Cfg->Ring);
   Cfg->DialTimeout = (USHORT)SPBM_QueryValue (112);
   Cfg->CarrierDropTimeout = (USHORT)SPBM_QueryValue (114);
   Cfg->LockSpeed = BM_QueryCheck (105);
   Cfg->StripDashes = BM_QueryCheck (110);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Nodes editor
// ----------------------------------------------------------------------

class CAddNodeDlg : public CDialog
{
public:
   CAddNodeDlg (HWND p_hWnd);

   CHAR   Address[64];
   CHAR   SystemName[64];
   CHAR   SysopName[48];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CNodesListDlg : public CDialog
{
public:
   CNodesListDlg (HWND p_hWnd);

   int    Selected;
   class  TNodes *Data;

   VOID   OnChanged (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   DECLARE_MESSAGE_MAP ()
};

class CNodeSecurityDlg : public CDialog
{
public:
   CNodeSecurityDlg (HWND p_hWnd);

   class  TNodes *Data;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CNodeOtherDlg : public CDialog
{
public:
   CNodeOtherDlg (HWND p_hWnd);

   class  TNodes *Data;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CNodeEchoDlg : public CDialog
{
public:
   CNodeEchoDlg (HWND p_hWnd);

   class  TNodes *Nodes;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CNodeTicDlg : public CDialog
{
public:
   CNodeTicDlg (HWND p_hWnd);

   class  TNodes *Nodes;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

BEGIN_MESSAGE_MAP (CNodesDlg, CDialog)
   ON_COMMAND (13,  Copy)
   ON_COMMAND (14,  NodeTic)
   ON_COMMAND (114, NodeEcho)
   ON_COMMAND (115, Add)
   ON_COMMAND (116, Delete)
   ON_COMMAND (117, List)
   ON_COMMAND (118, Previous)
   ON_COMMAND (119, Next)
   ON_COMMAND (123, Security)
   ON_COMMAND (114, Other)
END_MESSAGE_MAP ()

CNodesDlg::CNodesDlg (HWND p_hWnd) : CDialog ("12", p_hWnd)
{
   Data = NULL;
}

CNodesDlg::~CNodesDlg (void)
{
   if (Data != NULL)
      delete Data;
}

VOID CNodesDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 12);
}

USHORT CNodesDlg::OnInitDialog (VOID)
{
   class TPacker *Packer;

   Center ();

   Data = new TNodes (Cfg->NodelistPath);

   EM_SetTextLimit (102, sizeof (Data->Address) - 1);
   EM_SetTextLimit (104, sizeof (Data->SystemName) - 1);
   EM_SetTextLimit (108, sizeof (Data->SysopName) - 1);
   EM_SetTextLimit (106, sizeof (Data->Location) - 1);
   EM_SetTextLimit (125, sizeof (Data->Phone) - 1);
   EM_SetTextLimit (127, sizeof (Data->Flags) - 1);
   EM_SetTextLimit (19,  sizeof (Data->SessionPwd) - 1);
   EM_SetTextLimit (21,  sizeof (Data->InPktPwd) - 1);
   EM_SetTextLimit (121, sizeof (Data->OutPktPwd) - 1);
   EM_SetTextLimit (23,  sizeof (Data->AreaMgrPwd) - 1);
   EM_SetTextLimit (25,  sizeof (Data->TicPwd) - 1);

   if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
      if (Packer->First () == TRUE)
         do {
            CB_AddString (112, Packer->Display);
         } while (Packer->Next ());
      delete Packer;
   }

   if (Cfg->MailAddress.First () == TRUE)
      do {
         CB_AddString (17, Cfg->MailAddress.String);
      } while (Cfg->MailAddress.Next ());

   if (Data->First () == FALSE)
      Data->New ();

   DisplayData ();

   return (TRUE);
}

VOID CNodesDlg::OnOK (VOID)
{
   CHAR NewAddress[64];
   class TNodes *Back;

   GetDlgItemText (102, GetDlgItemTextLength (102), NewAddress);
   if (stricmp (NewAddress, Data->Address)) {
      if ((Back = new TNodes (Cfg->NodelistPath)) != NULL) {
         Back->Read (Data->Address);
         Back->Delete ();
         delete Back;
      }
      ReadData ();
      Data->Add ();
   }
   else {
      ReadData ();
      Data->Update ();
   }
}

VOID CNodesDlg::DisplayData (VOID)
{
   class TPacker *Packer;

   SetDlgItemText (102, Data->Address);
   SetDlgItemText (104, Data->SystemName);
   SetDlgItemText (108, Data->SysopName);
   BM_SetCheck (122, Data->RemapMail);
   SetDlgItemText (106, Data->Location);
   SetDlgItemText (125, Data->Phone);
   SetDlgItemText (127, Data->Flags);
   SetDlgItemText (17,  Data->MailerAka);
   SetDlgItemText (19,  Data->SessionPwd);
   SetDlgItemText (21,  Data->InPktPwd);
   SetDlgItemText (121, Data->OutPktPwd);
   SetDlgItemText (23,  Data->AreaMgrPwd);
   SetDlgItemText (25,  Data->TicPwd);
   SetDlgItemText (27,  Data->DialCmd);

   if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
      if (Packer->Read (Data->Packer) == TRUE)
         SetDlgItemText (112, Packer->Display);
      delete Packer;
   }
}

VOID CNodesDlg::ReadData (VOID)
{
   CHAR Temp[128];
   class TPacker *Packer;

   GetDlgItemText (102, GetDlgItemTextLength (102), Data->Address);
   GetDlgItemText (104, GetDlgItemTextLength (104), Data->SystemName);
   GetDlgItemText (108, GetDlgItemTextLength (108), Data->SysopName);
   Data->RemapMail = (UCHAR)BM_QueryCheck (122);
   GetDlgItemText (106, GetDlgItemTextLength (106), Data->Location);
   GetDlgItemText (125, GetDlgItemTextLength (125), Data->Phone);
   GetDlgItemText (127, GetDlgItemTextLength (127), Data->Flags);
   GetDlgItemText (17,  GetDlgItemTextLength (17),  Data->MailerAka);
   GetDlgItemText (19,  GetDlgItemTextLength (19),  Data->SessionPwd);
   GetDlgItemText (21,  GetDlgItemTextLength (21),  Data->InPktPwd);
   GetDlgItemText (121, GetDlgItemTextLength (121), Data->OutPktPwd);
   GetDlgItemText (23,  GetDlgItemTextLength (23),  Data->AreaMgrPwd);
   GetDlgItemText (25,  GetDlgItemTextLength (25),  Data->TicPwd);
   GetDlgItemText (27,  GetDlgItemTextLength (27),  Data->DialCmd);

   GetDlgItemText (112, GetDlgItemTextLength (112), Temp);
   if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
      if (Packer->First () == TRUE)
         do {
            if (!strcmp (Temp, Packer->Display)) {
               strcpy (Data->Packer, Packer->Key);
               break;
            }
         } while (Packer->Next ());
      delete Packer;
   }
}

VOID CNodesDlg::Add (VOID)
{
   class CAddNodeDlg *Dlg;

   if ((Dlg = new CAddNodeDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         Data->New ();
         strcpy (Data->Address, Dlg->Address);
         strcpy (Data->SystemName, Dlg->SystemName);
         strcpy (Data->SysopName, Dlg->SysopName);
         Data->Add ();
         DisplayData ();
      }
      delete Dlg;
   }
}

VOID CNodesDlg::Copy (VOID)
{
   class CAddNodeDlg *Dlg;

   if ((Dlg = new CAddNodeDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         strcpy (Data->Address, Dlg->Address);
         strcpy (Data->SystemName, Dlg->SystemName);
         strcpy (Data->SysopName, Dlg->SysopName);
         Data->Add ();
         DisplayData ();
      }
      delete Dlg;
   }
}

VOID CNodesDlg::Delete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      Data->Delete ();
      DisplayData ();
   }
}

VOID CNodesDlg::Other (VOID)
{
   class CNodeOtherDlg *Dlg;

   if ((Dlg = new CNodeOtherDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CNodesDlg::NodeEcho (VOID)
{
   class CNodeEchoDlg *Dlg;

   if ((Dlg = new CNodeEchoDlg (m_hWnd)) != NULL) {
      Dlg->Nodes = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CNodesDlg::NodeTic (VOID)
{
   class CNodeTicDlg *Dlg;

   if ((Dlg = new CNodeTicDlg (m_hWnd)) != NULL) {
      Dlg->Nodes = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CNodesDlg::List (VOID)
{
   class CNodesListDlg *Dlg;

   if ((Dlg = new CNodesListDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      if (Dlg->DoModal () == TRUE)
         DisplayData ();
      delete Dlg;
   }
}

VOID CNodesDlg::Next (VOID)
{
   if (Data->Next () == TRUE)
      DisplayData ();
}

VOID CNodesDlg::Previous (VOID)
{
   if (Data->Previous () == TRUE)
      DisplayData ();
}

VOID CNodesDlg::Security (VOID)
{
   class CNodeSecurityDlg *Dlg;

   if ((Dlg = new CNodeSecurityDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

// ----------------------------------------------------------------------

CAddNodeDlg::CAddNodeDlg (HWND p_hWnd) : CDialog ("46", p_hWnd)
{
}

USHORT CAddNodeDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Address) - 1);
   EM_SetTextLimit (104, sizeof (SystemName) - 1);
   EM_SetTextLimit (108, sizeof (SysopName) - 1);

   return (TRUE);
}

VOID CAddNodeDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Address);
   GetDlgItemText (104, GetDlgItemTextLength (104), SystemName);
   GetDlgItemText (108, GetDlgItemTextLength (108), SysopName);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CNodesListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
#if defined(__NT__)
   ON_NOTIFY (LVN_ITEMCHANGED, 101, OnChanged)
#endif
END_MESSAGE_MAP ()

CNodesListDlg::CNodesListDlg (HWND p_hWnd) : CDialog ("25", p_hWnd)
{
   Selected = -1;
}

USHORT CNodesListDlg::OnInitDialog (VOID)
{
   class TNodes *Nodes;

   SetWindowTitle ("Nodes List");

   LVM_AllocateColumns (101, 3);
   LVM_InsertColumn (101, "Address", LVC_LEFT);
   LVM_InsertColumn (101, "System Name", LVC_LEFT);
   LVM_InsertColumn (101, "Sysop Name", LVC_LEFT);

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->First () == TRUE)
         do {
            LVM_InsertItem (101);

            LVM_SetItemText (101, 0, Nodes->Address);
            LVM_SetItemText (101, 1, Nodes->SystemName);
            LVM_SetItemText (101, 2, Nodes->SysopName);
         } while (Nodes->Next () == TRUE);
      delete Nodes;
   }

   LVM_InvalidateView (101);

   return (TRUE);
}

VOID CNodesListDlg::OnChanged (VOID)
{
#if defined(__NT__)
   NM_LISTVIEW *pnmv = (NM_LISTVIEW *)m_lParam;

   if (!(pnmv->uNewState & LVIS_SELECTED) && (pnmv->uOldState & LVIS_SELECTED))
      Selected = -1;
   if (pnmv->uNewState & LVIS_SELECTED)
      Selected = pnmv->iItem;
#endif
}

VOID CNodesListDlg::OnOK (VOID)
{
#if defined(__OS2__)
   LV_PLISTDATA pRecord;

   if ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, 101, CM_QUERYRECORDEMPHASIS, MPFROMSHORT (CMA_FIRST), MPFROMSHORT (CRA_SELECTED))) != NULL)
      Data->Read (pRecord->Column[0].Data);
#elif defined(__NT__)
   CHAR Temp[32];

   if (Selected != -1) {
      ListView_GetItemText (GetDlgItem (m_hWnd, 101), Selected, 0, Temp, sizeof (Temp));
      Data->Read (Temp);
   }
#endif

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CNodeSecurityDlg::CNodeSecurityDlg (HWND p_hWnd) : CDialog ("27", p_hWnd)
{
}

USHORT CNodeSecurityDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (110, sizeof (Data->SessionPwd) - 1);
   EM_SetTextLimit (112, sizeof (Data->InPktPwd) - 1);
   EM_SetTextLimit (121, sizeof (Data->OutPktPwd) - 1);
   EM_SetTextLimit (114, sizeof (Data->AreaMgrPwd) - 1);
   EM_SetTextLimit (102, sizeof (Data->TicPwd) - 1);

   SetDlgItemText (110, Data->SessionPwd);
   SetDlgItemText (112, Data->InPktPwd);
   SetDlgItemText (121, Data->OutPktPwd);
   SetDlgItemText (114, Data->AreaMgrPwd);
   SetDlgItemText (102, Data->TicPwd);

   return (TRUE);
}

VOID CNodeSecurityDlg::OnOK (VOID)
{
   GetDlgItemText (110, GetDlgItemTextLength (110), Data->SessionPwd);
   GetDlgItemText (112, GetDlgItemTextLength (112), Data->InPktPwd);
   GetDlgItemText (121, GetDlgItemTextLength (121), Data->OutPktPwd);
   GetDlgItemText (114, GetDlgItemTextLength (114), Data->AreaMgrPwd);
   GetDlgItemText (102, GetDlgItemTextLength (102), Data->TicPwd);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CNodeOtherDlg::CNodeOtherDlg (HWND p_hWnd) : CDialog ("39", p_hWnd)
{
}

USHORT CNodeOtherDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (108, sizeof (Data->DialCmd) - 1);
   EM_SetTextLimit (102, sizeof (Data->InetAddress) - 1);
   EM_SetTextLimit (106, sizeof (Data->Pop3Pwd) - 1);

   SetDlgItemText (108, Data->DialCmd);
   SetDlgItemText (102, Data->InetAddress);
   BM_SetCheck (103, Data->ImportPOP3Mail);
   BM_SetCheck (104, Data->UseInetAddress);
   SetDlgItemText (106, Data->Pop3Pwd);

   return (TRUE);
}

VOID CNodeOtherDlg::OnOK (VOID)
{
   GetDlgItemText (108, GetDlgItemTextLength (108), Data->DialCmd);
   GetDlgItemText (102, GetDlgItemTextLength (102), Data->InetAddress);
   Data->ImportPOP3Mail = (UCHAR)BM_QueryCheck (103);
   Data->UseInetAddress = (UCHAR)BM_QueryCheck (104);
   GetDlgItemText (106, GetDlgItemTextLength (106), Data->Pop3Pwd);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CNodeEchoDlg::CNodeEchoDlg (HWND p_hWnd) : CDialog ("59", p_hWnd)
{
}

USHORT CNodeEchoDlg::OnInitDialog (VOID)
{
   USHORT i;
   ULONG Test;

   Center ();

   SPBM_SetLimits (103, 65535L, 0L);
   EM_SetTextLimit (175, sizeof (Nodes->NewAreasFilter) - 1);

   SPBM_SetCurrentValue (103, Nodes->Level);

   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (Nodes->AccessFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (Nodes->DenyFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   SetDlgItemText (175, Nodes->NewAreasFilter);
   BM_SetCheck (169, Nodes->CreateNewAreas);
   BM_SetCheck (170, Nodes->LinkNewEcho);
   BM_SetCheck (171, Nodes->EchoMaint);
   BM_SetCheck (172, Nodes->ChangeEchoTag);

   return (TRUE);
}

VOID CNodeEchoDlg::OnOK (VOID)
{
   USHORT i;
   ULONG Test;

   Nodes->Level = (USHORT)SPBM_QueryValue (103);

   Nodes->AccessFlags = 0L;
   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Nodes->AccessFlags |= Test;
   }
   Nodes->DenyFlags = 0L;
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Nodes->DenyFlags |= Test;
   }

   GetDlgItemText (175, GetDlgItemTextLength (175), Nodes->NewAreasFilter);
   Nodes->CreateNewAreas = (UCHAR)BM_QueryCheck (169);
   Nodes->LinkNewEcho = (UCHAR)BM_QueryCheck (170);
   Nodes->EchoMaint = (UCHAR)BM_QueryCheck (171);
   Nodes->ChangeEchoTag = (UCHAR)BM_QueryCheck (172);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CNodeTicDlg::CNodeTicDlg (HWND p_hWnd) : CDialog ("60", p_hWnd)
{
}

USHORT CNodeTicDlg::OnInitDialog (VOID)
{
   USHORT i;
   ULONG Test;

   Center ();

   SPBM_SetLimits (103, 65535L, 0L);
   EM_SetTextLimit (175, sizeof (Nodes->NewTicFilter) - 1);

   SPBM_SetCurrentValue (103, Nodes->TicLevel);

   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (Nodes->TicAccessFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (Nodes->TicDenyFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   SetDlgItemText (175, Nodes->NewTicFilter);
   BM_SetCheck (169, Nodes->CreateNewTic);
   BM_SetCheck (170, Nodes->LinkNewTic);
   BM_SetCheck (171, Nodes->TicMaint);
   BM_SetCheck (172, Nodes->ChangeTicTag);

   return (TRUE);
}

VOID CNodeTicDlg::OnOK (VOID)
{
   USHORT i;
   ULONG Test;

   Nodes->TicLevel = (USHORT)SPBM_QueryValue (103);

   Nodes->TicAccessFlags = 0L;
   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Nodes->TicAccessFlags |= Test;
   }
   Nodes->TicDenyFlags = 0L;
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Nodes->TicDenyFlags |= Test;
   }

   GetDlgItemText (175, GetDlgItemTextLength (175), Nodes->NewTicFilter);
   Nodes->CreateNewTic = (UCHAR)BM_QueryCheck (169);
   Nodes->LinkNewTic = (UCHAR)BM_QueryCheck (170);
   Nodes->TicMaint = (UCHAR)BM_QueryCheck (171);
   Nodes->ChangeTicTag = (UCHAR)BM_QueryCheck (172);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

#if defined(__OS2__)
MRESULT EXPENTRY RegistrationDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
#elif defined(__NT__)
BOOL CALLBACK RegistrationDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
{
   switch (msg) {
#if defined(__OS2__)
      case WM_INITDLG:
         break;
#elif defined(__NT__)
      case WM_INITDIALOG:
         return ((BOOL)TRUE);

      case WM_CTLCOLORBTN:
      case WM_CTLCOLORSTATIC:
      case WM_CTLCOLORDLG: {
         LOGBRUSH logBrush;

         lParam = lParam;
         SetBkColor ((HDC)wParam, GetSysColor (COLOR_MENU));

         logBrush.lbStyle = BS_SOLID;
         logBrush.lbColor = GetSysColor (COLOR_MENU);
         logBrush.lbHatch = 0;

         return ((BOOL)CreateBrushIndirect (&logBrush));
      }
#endif

      case WM_COMMAND:
#if defined(__OS2__)
         switch (SHORT1FROMMP (mp1)) {
            case IDOK:
               WinDismissDlg (hwnd, TRUE);
               break;

            case IDCANCEL:
               WinDismissDlg (hwnd, FALSE);
               break;
         }
         return ((MRESULT)FALSE);
#elif defined(__NT__)
         switch (wParam) {
            case IDOK:
               EndDialog (hwnd, TRUE);
               break;

            case IDCANCEL:
               EndDialog (hwnd, FALSE);
               break;
         }
         break;
#endif

#if defined(__NT__)
      case WM_CLOSE:
         EndDialog (hwnd, FALSE);
         break;
#endif
   }

#if defined(__OS2__)
   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
#elif defined(__NT__)
   return ((BOOL)FALSE);
#endif
}

// ----------------------------------------------------------------------
// Site info
// ----------------------------------------------------------------------

CSiteInfoDlg::CSiteInfoDlg (HWND p_hWnd) : CDialog ("3", p_hWnd)
{
}

VOID CSiteInfoDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 3);
}

USHORT CSiteInfoDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Cfg->SystemName) - 1);
   EM_SetTextLimit (104, sizeof (Cfg->SysopName) - 1);
   EM_SetTextLimit (106, sizeof (Cfg->Location) - 1);
   EM_SetTextLimit (108, sizeof (Cfg->Phone) - 1);
   EM_SetTextLimit (110, sizeof (Cfg->NodelistFlags) - 1);

   SetDlgItemText (102, Cfg->SystemName);
   SetDlgItemText (104, Cfg->SysopName);
   SetDlgItemText (106, Cfg->Location);
   SetDlgItemText (108, Cfg->Phone);
   SetDlgItemText (110, Cfg->NodelistFlags);

   return (TRUE);
}

VOID CSiteInfoDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->SystemName);
   GetDlgItemText (104, GetDlgItemTextLength (104), Cfg->SysopName);
   GetDlgItemText (106, GetDlgItemTextLength (106), Cfg->Location);
   GetDlgItemText (108, GetDlgItemTextLength (108), Cfg->Phone);
   GetDlgItemText (110, GetDlgItemTextLength (110), Cfg->NodelistFlags);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Internet options
// ----------------------------------------------------------------------

CInternetDlg::CInternetDlg (HWND p_hWnd) : CDialog ("23", p_hWnd)
{
}

VOID CInternetDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 23);
}

USHORT CInternetDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Cfg->HostName) - 1);
   EM_SetTextLimit (128, sizeof (Cfg->NewsServer) - 1);
   EM_SetTextLimit (104, sizeof (Cfg->MailServer) - 1);
   SPBM_SetLimits (107, 1440, 0);
   EM_SetTextLimit (109, sizeof (Cfg->PPPCmd) - 1);
   SPBM_SetLimits (111, 1000, 0);

   SetDlgItemText (102, Cfg->HostName);
   SetDlgItemText (128, Cfg->NewsServer);
   SetDlgItemText (104, Cfg->MailServer);
   BM_SetCheck (105, Cfg->EnablePPP);
   SPBM_SetCurrentValue (107, Cfg->PPPTimeLimit);
   SetDlgItemText (109, Cfg->PPPCmd);
   SPBM_SetCurrentValue (111, Cfg->RetriveMaxMessages);

   return (TRUE);
}

VOID CInternetDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->HostName);
   GetDlgItemText (128, GetDlgItemTextLength (128), Cfg->NewsServer);
   GetDlgItemText (104, GetDlgItemTextLength (104), Cfg->MailServer);
   Cfg->EnablePPP = (UCHAR)BM_QueryCheck (105);
   Cfg->PPPTimeLimit = (USHORT)SPBM_QueryValue (107);
   GetDlgItemText (109, GetDlgItemTextLength (109), Cfg->PPPCmd);
   Cfg->RetriveMaxMessages = (USHORT)SPBM_QueryValue (111);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Compressors editor
// ----------------------------------------------------------------------

class CPackerListDlg : public CDialog
{
public:
   CPackerListDlg (HWND p_hWnd);

   int    Selected;
   class  TPacker *Data;

   VOID   OnChanged (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   DECLARE_MESSAGE_MAP ()
};

BEGIN_MESSAGE_MAP (CPackerDlg, CDialog)
   ON_COMMAND (117, Add)
   ON_COMMAND (118, Delete)
   ON_COMMAND (119, List)
   ON_COMMAND (120, Previous)
   ON_COMMAND (121, Next)
END_MESSAGE_MAP ()

CPackerDlg::CPackerDlg (HWND p_hWnd) : CDialog ("28", p_hWnd)
{
   Data = NULL;
}

CPackerDlg::~CPackerDlg (void)
{
   if (Data != NULL)
      delete Data;
}

VOID CPackerDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 28);
}

USHORT CPackerDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (108, sizeof (Data->Key) - 1);
   EM_SetTextLimit (102, sizeof (Data->Display) - 1);
   EM_SetTextLimit (106, sizeof (Data->Id) - 1);
   EM_SetTextLimit (110, sizeof (Data->PackCmd) - 1);
   EM_SetTextLimit (112, sizeof (Data->UnpackCmd) - 1);

   if ((Data = new TPacker (Cfg->SystemPath)) != NULL) {
      if (Data->First (FALSE) == FALSE)
         Data->New ();
   }

   DisplayData ();

   return (TRUE);
}

VOID CPackerDlg::OnOK (VOID)
{
   ReadData ();
   Data->Update ();
   SetFocus (108);
}

VOID CPackerDlg::DisplayData (VOID)
{
   CHAR Temp[32];

   SetDlgItemText (108, Data->Key);
   SetDlgItemText (102, Data->Display);
   sprintf (Temp, "%ld", Data->Position);
   SetDlgItemText (104, Temp);
   SetDlgItemText (106, Data->Id);
   SetDlgItemText (110, Data->PackCmd);
   SetDlgItemText (112, Data->UnpackCmd);
   BM_SetCheck (113, Data->Dos);
   BM_SetCheck (114, Data->OS2);
   BM_SetCheck (115, Data->Windows);
   BM_SetCheck (116, Data->Linux);
}

VOID CPackerDlg::ReadData (VOID)
{
   CHAR Temp[32];

   GetDlgItemText (108, GetDlgItemTextLength (108), Data->Key);
   GetDlgItemText (102, GetDlgItemTextLength (102), Data->Display);
   GetDlgItemText (104, GetDlgItemTextLength (104), Temp);
   Data->Position = atol (Temp);
   GetDlgItemText (106, GetDlgItemTextLength (106), Data->Id);
   GetDlgItemText (110, GetDlgItemTextLength (110), Data->PackCmd);
   GetDlgItemText (112, GetDlgItemTextLength (112), Data->UnpackCmd);
   Data->Dos = (UCHAR)BM_QueryCheck (113);
   Data->OS2 = (UCHAR)BM_QueryCheck (114);
   Data->Windows = (UCHAR)BM_QueryCheck (115);
   Data->Linux = (UCHAR)BM_QueryCheck (116);
}

VOID CPackerDlg::Add (VOID)
{
   Data->New ();
   ReadData ();
   Data->Add ();
   SetFocus (108);
}

VOID CPackerDlg::Delete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      Data->Delete ();
      DisplayData ();
   }
   SetFocus (108);
}

VOID CPackerDlg::List (VOID)
{
   class CPackerListDlg *Dlg;

   if ((Dlg = new CPackerListDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      if (Dlg->DoModal () == TRUE)
         DisplayData ();
      delete Dlg;
   }
   SetFocus (108);
}

VOID CPackerDlg::Next (VOID)
{
   if (Data->Next (FALSE) == TRUE)
      DisplayData ();
   SetFocus (108);
}

VOID CPackerDlg::Previous (VOID)
{
   if (Data->Previous (FALSE) == TRUE)
      DisplayData ();
   SetFocus (108);
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CPackerListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
#if defined(__NT__)
   ON_NOTIFY (LVN_ITEMCHANGED, 101, OnChanged)
#endif
END_MESSAGE_MAP ()

CPackerListDlg::CPackerListDlg (HWND p_hWnd) : CDialog ("25", p_hWnd)
{
   Selected = -1;
}

USHORT CPackerListDlg::OnInitDialog (VOID)
{
   CHAR Temp[128];
   class TPacker *Packer;

   SetWindowTitle ("Compressors List");

   LVM_AllocateColumns (101, 3);
   LVM_InsertColumn (101, "Key", LVC_LEFT);
   LVM_InsertColumn (101, "Display", LVC_LEFT);
   LVM_InsertColumn (101, "Op.Systems", LVC_LEFT);

   if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
      if (Packer->First (FALSE) == TRUE)
         do {
            LVM_InsertItem (101);

            LVM_SetItemText (101, 0, Packer->Key);
            LVM_SetItemText (101, 1, Packer->Display);
            Temp[0] = '\0';
            if (Packer->Dos == TRUE)
               strcat (Temp, "DOS ");
            if (Packer->OS2 == TRUE)
               strcat (Temp, "OS/2 ");
            if (Packer->Windows == TRUE)
               strcat (Temp, "Windows ");
            if (Packer->Linux == TRUE)
               strcat (Temp, "Linux ");
            LVM_SetItemText (101, 2, Temp);
         } while (Packer->Next (FALSE) == TRUE);
      delete Packer;
   }

   LVM_InvalidateView (101);

   return (TRUE);
}

VOID CPackerListDlg::OnChanged (VOID)
{
#if defined(__NT__)
   NM_LISTVIEW *pnmv = (NM_LISTVIEW *)m_lParam;

   if (!(pnmv->uNewState & LVIS_SELECTED) && (pnmv->uOldState & LVIS_SELECTED))
      Selected = -1;
   if (pnmv->uNewState & LVIS_SELECTED)
      Selected = pnmv->iItem;
#endif
}

VOID CPackerListDlg::OnOK (VOID)
{
#if defined(__OS2__)
   LV_PLISTDATA pRecord;

   if ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, 101, CM_QUERYRECORDEMPHASIS, MPFROMSHORT (CMA_FIRST), MPFROMSHORT (CRA_SELECTED))) != NULL)
      Data->Read (pRecord->Column[0].Data, FALSE);
#elif defined(__NT__)
   CHAR Temp[32];

   if (Selected != -1) {
      ListView_GetItemText (GetDlgItem (m_hWnd, 101), Selected, 0, Temp, sizeof (Temp));
      Data->Read (Temp, FALSE);
   }
#endif

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Mail processing
// ----------------------------------------------------------------------

CMailprocDlg::CMailprocDlg (HWND p_hWnd) : CDialog ("30", p_hWnd)
{
}

VOID CMailprocDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 30);
}

USHORT CMailprocDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (106, sizeof (Cfg->NetMailPath) - 1);
   EM_SetTextLimit (104, sizeof (Cfg->DupePath) - 1);
   EM_SetTextLimit (112, sizeof (Cfg->BadPath) - 1);
   EM_SetTextLimit (117, sizeof (Cfg->TearLine) - 1);
   SPBM_SetLimits (131, 255L, 1L);
   SPBM_SetLimits (132, 255L, 1L);
   SPBM_SetLimits (135, 255L, 1L);

   CB_AddString (107, "Squish<tm>");
   CB_AddString (107, "JAM");
   CB_AddString (107, "Fido (*.msg)");
   CB_AddString (107, "AdeptXBBS");
   CB_AddString (107, "Hudson");

   CB_AddString (102, "Squish<tm>");
   CB_AddString (102, "JAM");
   CB_AddString (102, "Fido (*.msg)");
   CB_AddString (102, "AdeptXBBS");
   CB_AddString (102, "Hudson");

   CB_AddString (110, "Squish<tm>");
   CB_AddString (110, "JAM");
   CB_AddString (110, "Fido (*.msg)");
   CB_AddString (110, "AdeptXBBS");
   CB_AddString (110, "Hudson");

   SetDlgItemText (106, Cfg->NetMailPath);
   SetDlgItemText (104, Cfg->DupePath);
   SetDlgItemText (112, Cfg->BadPath);

   switch (Cfg->NetMailStorage) {
      case ST_SQUISH:
         CB_SelectItem (107, 0);
         break;
      case ST_JAM:
         CB_SelectItem (107, 1);
         break;
      case ST_FIDO:
         CB_SelectItem (107, 2);
         break;
      case ST_ADEPT:
         CB_SelectItem (107, 3);
         break;
      case ST_HUDSON:
         CB_SelectItem (107, 4);
         break;
   }

   switch (Cfg->DupeStorage) {
      case ST_SQUISH:
         CB_SelectItem (102, 0);
         break;
      case ST_JAM:
         CB_SelectItem (102, 1);
         break;
      case ST_FIDO:
         CB_SelectItem (102, 2);
         break;
      case ST_ADEPT:
         CB_SelectItem (102, 3);
         break;
      case ST_HUDSON:
         CB_SelectItem (102, 4);
         break;
   }

   switch (Cfg->BadStorage) {
      case ST_SQUISH:
         CB_SelectItem (110, 0);
         break;
      case ST_JAM:
         CB_SelectItem (110, 1);
         break;
      case ST_FIDO:
         CB_SelectItem (110, 2);
         break;
      case ST_ADEPT:
         CB_SelectItem (110, 3);
         break;
      case ST_HUDSON:
         CB_SelectItem (110, 4);
         break;
   }

   BM_SetCheck (113, Cfg->ImportEmpty);
   BM_SetCheck (114, Cfg->ForceIntl);
   BM_SetCheck (115, Cfg->ReplaceTear);
   SetDlgItemText (117, Cfg->TearLine);

   BM_SetCheck (118, Cfg->SeparateNetMail);
   BM_SetCheck (119, Cfg->UseSinglePass);

   SPBM_SetCurrentValue (131, Cfg->NetMailBoard);
   SPBM_SetCurrentValue (133, Cfg->DupeBoard);
   SPBM_SetCurrentValue (135, Cfg->BadBoard);

   return (TRUE);
}

VOID CMailprocDlg::OnOK (VOID)
{
   GetDlgItemText (106, GetDlgItemTextLength (106), Cfg->NetMailPath);
   GetDlgItemText (104, GetDlgItemTextLength (104), Cfg->DupePath);
   GetDlgItemText (112, GetDlgItemTextLength (112), Cfg->BadPath);

   switch (CB_QuerySelection (107)) {
      case 0:
         Cfg->NetMailStorage = ST_SQUISH;
         break;
      case 1:
         Cfg->NetMailStorage = ST_JAM;
         break;
      case 2:
         Cfg->NetMailStorage = ST_FIDO;
         break;
      case 3:
         Cfg->NetMailStorage = ST_ADEPT;
         break;
      case 4:
         Cfg->NetMailStorage = ST_HUDSON;
         break;
   }

   switch (CB_QuerySelection (102)) {
      case 0:
         Cfg->DupeStorage = ST_SQUISH;
         break;
      case 1:
         Cfg->DupeStorage = ST_JAM;
         break;
      case 2:
         Cfg->DupeStorage = ST_FIDO;
         break;
      case 3:
         Cfg->DupeStorage = ST_ADEPT;
         break;
      case 4:
         Cfg->DupeStorage = ST_HUDSON;
         break;
   }

   switch (CB_QuerySelection (110)) {
      case 0:
         Cfg->BadStorage = ST_SQUISH;
         break;
      case 1:
         Cfg->BadStorage = ST_JAM;
         break;
      case 2:
         Cfg->BadStorage = ST_FIDO;
         break;
      case 3:
         Cfg->BadStorage = ST_ADEPT;
         break;
      case 4:
         Cfg->BadStorage = ST_HUDSON;
         break;
   }

   Cfg->ImportEmpty = (UCHAR)BM_QueryCheck (113);
   Cfg->ForceIntl = (UCHAR)BM_QueryCheck (114);
   Cfg->ReplaceTear = (UCHAR)BM_QueryCheck (115);
   GetDlgItemText (117, GetDlgItemTextLength (117), Cfg->TearLine);
   Cfg->SeparateNetMail = (UCHAR)BM_QueryCheck (118);
   Cfg->UseSinglePass = (UCHAR)BM_QueryCheck (119);

   Cfg->NetMailBoard = (USHORT)SPBM_QueryValue (131);
   Cfg->DupeBoard = (USHORT)SPBM_QueryValue (133);
   Cfg->BadBoard = (USHORT)SPBM_QueryValue (135);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Miscellaneous mailer
// ----------------------------------------------------------------------

CMiscDlg::CMiscDlg (HWND p_hWnd) : CDialog ("20", p_hWnd)
{
}

VOID CMiscDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 20);
}

USHORT CMiscDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (109, sizeof (Cfg->MailOnly) - 1);
   EM_SetTextLimit (105, sizeof (Cfg->EnterBBS) - 1);

   SetDlgItemText (109, Cfg->MailOnly);
   SetDlgItemText (105, Cfg->EnterBBS);

   BM_SetCheck (102, Cfg->WaZoo);
   BM_SetCheck (103, Cfg->EMSI);
   BM_SetCheck (104, Cfg->Janus);
   BM_SetCheck (106, Cfg->Secure);
   BM_SetCheck (107, Cfg->KeepNetMail);
   BM_SetCheck (110, Cfg->TrackNetMail);

   return (TRUE);
}

VOID CMiscDlg::OnOK (VOID)
{
   GetDlgItemText (109, GetDlgItemTextLength (109), Cfg->MailOnly);
   GetDlgItemText (105, GetDlgItemTextLength (105), Cfg->EnterBBS);

   Cfg->WaZoo = (UCHAR)BM_QueryCheck (102);
   Cfg->EMSI = (UCHAR)BM_QueryCheck (103);
   Cfg->Janus = (UCHAR)BM_QueryCheck (104);
   Cfg->Secure = (UCHAR)BM_QueryCheck (106);
   Cfg->KeepNetMail = (UCHAR)BM_QueryCheck (107);
   Cfg->TrackNetMail = (UCHAR)BM_QueryCheck (110);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// External processing
// ----------------------------------------------------------------------

CExternalDlg::CExternalDlg (HWND p_hWnd) : CDialog ("31", p_hWnd)
{
}

VOID CExternalDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 31);
}

USHORT CExternalDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (112, sizeof (Cfg->ImportCmd) - 1);
   EM_SetTextLimit (102, sizeof (Cfg->ExportCmd) - 1);
   EM_SetTextLimit (104, sizeof (Cfg->PackCmd) - 1);
   EM_SetTextLimit (106, sizeof (Cfg->SinglePassCmd) - 1);
   EM_SetTextLimit (108, sizeof (Cfg->NewsgroupCmd) - 1);

   SetDlgItemText (112, Cfg->ImportCmd);
   SetDlgItemText (102, Cfg->ExportCmd);
   SetDlgItemText (104, Cfg->PackCmd);
   SetDlgItemText (106, Cfg->SinglePassCmd);
   SetDlgItemText (108, Cfg->NewsgroupCmd);

   return (TRUE);
}

VOID CExternalDlg::OnOK (VOID)
{
   GetDlgItemText (112, GetDlgItemTextLength (112), Cfg->ImportCmd);
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->ExportCmd);
   GetDlgItemText (104, GetDlgItemTextLength (104), Cfg->PackCmd);
   GetDlgItemText (106, GetDlgItemTextLength (106), Cfg->SinglePassCmd);
   GetDlgItemText (108, GetDlgItemTextLength (108), Cfg->NewsgroupCmd);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Areafix
// ----------------------------------------------------------------------

CAreafixDlg::CAreafixDlg (HWND p_hWnd) : CDialog ("33", p_hWnd)
{
}

VOID CAreafixDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 33);
}

USHORT CAreafixDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (104, sizeof (Cfg->AreasBBS) - 1);
   EM_SetTextLimit (112, sizeof (Cfg->NewAreasPath) - 1);
   EM_SetTextLimit (36, sizeof (Cfg->AreafixHelp) - 1);
   EM_SetTextLimit (39, sizeof (Cfg->AreafixNames) - 1);
   CB_AddString (110, "Squish<tm>");
   CB_AddString (110, "JAM");
   CB_AddString (110, "Fido (*.msg)");
   CB_AddString (110, "AdeptXBBS");

   SetDlgItemText (104, Cfg->AreasBBS);
   SetDlgItemText (36, Cfg->AreafixHelp);
   SetDlgItemText (39, Cfg->AreafixNames);
   BM_SetCheck (101, Cfg->UseAreasBBS);
   BM_SetCheck (102, Cfg->UpdateAreasBBS);
   BM_SetCheck (34, Cfg->AreafixActive);
   BM_SetCheck (37, Cfg->AllowRescan);
   BM_SetCheck (40, Cfg->CheckZones);
   switch (Cfg->NewAreasStorage) {
      case ST_SQUISH:
         CB_SelectItem (110, 0);
         break;
      case ST_JAM:
         CB_SelectItem (110, 1);
         break;
      case ST_FIDO:
         CB_SelectItem (110, 2);
         break;
      case ST_ADEPT:
         CB_SelectItem (110, 3);
         break;
   }
   SetDlgItemText (112, Cfg->NewAreasPath);

   return (TRUE);
}

VOID CAreafixDlg::OnOK (VOID)
{
   GetDlgItemText (104, GetDlgItemTextLength (104), Cfg->AreasBBS);
   GetDlgItemText (36, GetDlgItemTextLength (36), Cfg->AreafixHelp);
   GetDlgItemText (39, GetDlgItemTextLength (39), Cfg->AreafixNames);
   Cfg->UseAreasBBS = (UCHAR)BM_QueryCheck (101);
   Cfg->UpdateAreasBBS = (UCHAR)BM_QueryCheck (102);
   Cfg->AreafixActive = (UCHAR)BM_QueryCheck (34);
   Cfg->AllowRescan = (UCHAR)BM_QueryCheck (37);
   Cfg->CheckZones = (UCHAR)BM_QueryCheck (40);
   switch (CB_QuerySelection (110)) {
      case 0:
         Cfg->NewAreasStorage = ST_SQUISH;
         break;
      case 1:
         Cfg->NewAreasStorage = ST_JAM;
         break;
      case 2:
         Cfg->NewAreasStorage = ST_FIDO;
         break;
      case 3:
         Cfg->NewAreasStorage = ST_ADEPT;
         break;
   }
   GetDlgItemText (112, GetDlgItemTextLength (112), Cfg->NewAreasPath);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// TIC Manager (Raid)
// ----------------------------------------------------------------------

CRaidDlg::CRaidDlg (HWND p_hWnd) : CDialog ("58", p_hWnd)
{
}

VOID CRaidDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 58);
}

USHORT CRaidDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (112, sizeof (Cfg->NewTicPath) - 1);
   EM_SetTextLimit (36, sizeof (Cfg->RaidHelp) - 1);
   EM_SetTextLimit (39, sizeof (Cfg->RaidNames) - 1);

   BM_SetCheck (34, Cfg->RaidActive);
   SetDlgItemText (36, Cfg->RaidHelp);
   SetDlgItemText (39, Cfg->RaidNames);
   SetDlgItemText (112, Cfg->NewTicPath);

   return (TRUE);
}

VOID CRaidDlg::OnOK (VOID)
{
   Cfg->RaidActive = (UCHAR)BM_QueryCheck (34);
   GetDlgItemText (36, GetDlgItemTextLength (36), Cfg->RaidHelp);
   GetDlgItemText (39, GetDlgItemTextLength (39), Cfg->RaidNames);
   GetDlgItemText (112, GetDlgItemTextLength (112), Cfg->NewTicPath);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Nodelist configuration
// ----------------------------------------------------------------------

class CNodelistDataDlg : public CDialog
{
public:
   CNodelistDataDlg (HWND p_hWnd);

   USHORT Zone;
   CHAR   Name[64], Diff[64];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

BEGIN_MESSAGE_MAP (CNodelistDlg, CDialog)
   ON_COMMAND (102, Add)
   ON_COMMAND (103, Edit)
   ON_COMMAND (104, Remove)
END_MESSAGE_MAP ()

CNodelistDlg::CNodelistDlg (HWND p_hWnd) : CDialog ("40", p_hWnd)
{
}

VOID CNodelistDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 40);
}

USHORT CNodelistDlg::OnInitDialog (VOID)
{
   CHAR Temp[32];
   class TNodes *Data;

   Center ();

   LVM_AllocateColumns (101, 3);
   LVM_InsertColumn (101, "Zone", LVC_RIGHT);
   LVM_InsertColumn (101, "Nodelist", LVC_LEFT);
   LVM_InsertColumn (101, "Nodediff", LVC_LEFT);

   if ((Data = new TNodes (Cfg->NodelistPath)) != NULL) {
      Data->LoadNodelist ();
      if (Data->FirstNodelist () == TRUE)
         do {
            LVM_InsertItem (101);
            sprintf (Temp, "%u", Data->DefaultZone);
            LVM_SetItemText (101, 0, Temp);
            LVM_SetItemText (101, 1, Data->Nodelist);
            LVM_SetItemText (101, 2, Data->Nodediff);
         } while (Data->NextNodelist () == TRUE);
      delete Data;
   }

   LVM_InvalidateView (101);

   return (TRUE);
}

VOID CNodelistDlg::OnOK (VOID)
{
   int i, count;
   CHAR Temp[16], Name[64], Diff[64];
   class TNodes *Data;

   if ((Data = new TNodes (Cfg->NodelistPath)) != NULL) {
      count = LVM_QueryItemCount (101);
      for (i = 0; i < count; i++) {
         Temp[0] = Name[0] = Diff[0] = '\0';
         LVM_QueryItemText (101, i, 0, Temp);
         LVM_QueryItemText (101, i, 1, Name);
         LVM_QueryItemText (101, i, 2, Diff);
         Data->AddNodelist (Name, Diff, (USHORT)atoi (Temp));
      }
      Data->SaveNodelist ();
      delete Data;
   }

   EndDialog (TRUE);
}

VOID CNodelistDlg::Add (VOID)
{
   CHAR Temp[16];
   class CNodelistDataDlg *Dlg;

   if ((Dlg = new CNodelistDataDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         LVM_InsertItem (101);
         sprintf (Temp, "%u", Dlg->Zone);
         LVM_SetItemText (101, 0, Temp);
         LVM_SetItemText (101, 1, Dlg->Name);
         LVM_SetItemText (101, 2, Dlg->Diff);
         LVM_InvalidateView (101);
      }
      delete Dlg;
   }
}

VOID CNodelistDlg::Edit (VOID)
{
   int item;
   CHAR Temp[16];
   class CNodelistDataDlg *Dlg;

   if ((item = LVM_QuerySelectedItem (101)) != -1) {
      if ((Dlg = new CNodelistDataDlg (m_hWnd)) != NULL) {
         LVM_QueryItemText (101, item, 0, Temp);
         Dlg->Zone = (USHORT)atoi (Temp);
         LVM_QueryItemText (101, item, 1, Dlg->Name);
         LVM_QueryItemText (101, item, 2, Dlg->Diff);
         if (Dlg->DoModal () == TRUE) {
            sprintf (Temp, "%u", Dlg->Zone);
            LVM_SetItemText (101, item, 0, Temp);
            LVM_SetItemText (101, item, 1, Dlg->Name);
            LVM_SetItemText (101, item, 2, Dlg->Diff);
            LVM_InvalidateView (101);
         }
         delete Dlg;
      }
   }
}

VOID CNodelistDlg::Remove (VOID)
{
   int item;

   if ((item = LVM_QuerySelectedItem (101)) != -1) {
      if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES)
         LVM_DeleteItem (101, item);
   }
}

// ----------------------------------------------------------------------

CNodelistDataDlg::CNodelistDataDlg (HWND p_hWnd) : CDialog ("41", p_hWnd)
{
   Zone = 0;
   if (Cfg->MailAddress.First () == TRUE)
      Zone = Cfg->MailAddress.Zone;
   Name[0] = Diff[0] = '\0';
}

USHORT CNodelistDataDlg::OnInitDialog (VOID)
{
   CHAR Temp[16];

   EM_SetTextLimit (104, sizeof (Name) - 1);
   EM_SetTextLimit (102, sizeof (Diff) - 1);
   EM_SetTextLimit (106, 5);

   SetDlgItemText (104, Name);
   SetDlgItemText (102, Diff);
   sprintf (Temp, "%u", Zone);
   SetDlgItemText (106, Temp);

   return (TRUE);
}

VOID CNodelistDataDlg::OnOK (VOID)
{
   CHAR Temp[16];

   GetDlgItemText (104, Name, GetDlgItemTextLength (104));
   GetDlgItemText (102, Diff, GetDlgItemTextLength (102));
   GetDlgItemText (106, Temp, GetDlgItemTextLength (106));
   Zone = (USHORT)atoi (Temp);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Origin Lines
// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (COriginDlg, CDialog)
   ON_COMMAND (103, Add)
   ON_COMMAND (104, Replace)
   ON_COMMAND (105, Remove)
#if defined(__OS2__)
   ON_CONTROL (LN_SELECT, 107, ItemSelected)
#elif defined(__NT__)
   ON_NOTIFY (LBN_SELCHANGE, 107, ItemSelected)
#endif
END_MESSAGE_MAP ()

COriginDlg::COriginDlg (HWND p_hWnd) : CDialog ("50", p_hWnd)
{
}

VOID COriginDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 50);
}

USHORT COriginDlg::OnInitDialog (VOID)
{
   FILE *fp;
   CHAR Temp[128];

   Center ();

   sprintf (Temp, "%sorigin.txt", Cfg->SystemPath);
   if ((fp = fopen (Temp, "rt")) != NULL) {
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         if (Temp[strlen (Temp) - 1] == '\n')
            Temp[strlen (Temp) - 1] = '\0';
         LM_AddString (107, Temp);
      }
      fclose (fp);
   }

   EM_SetTextLimit (102, sizeof (Temp) - 1);

   return (TRUE);
}

VOID COriginDlg::Add (VOID)
{
   CHAR Temp[128];

   GetDlgItemText (102, Temp, GetDlgItemTextLength (102));
   LM_AddString (107, Temp);
   SetDlgItemText (102, "");
}

VOID COriginDlg::Replace (VOID)
{
   CHAR Temp[128];

   GetDlgItemText (102, Temp, GetDlgItemTextLength (102));
   LM_SetItemText (107, LM_QuerySelection (107), Temp);
}

VOID COriginDlg::Remove (VOID)
{
   LM_DeleteItem (107, LM_QuerySelection (107));
}

VOID COriginDlg::ItemSelected (VOID)
{
   USHORT item;
   CHAR Temp[128];

   item = LM_QuerySelection (107);
   LM_QueryItemText (107, item, (USHORT)(sizeof (Temp) - 1), Temp);
   SetDlgItemText (102, Temp);
}

VOID COriginDlg::OnOK (VOID)
{
   FILE *fp;
   USHORT i, item;
   CHAR Temp[128];

   Center ();

   sprintf (Temp, "%sorigin.txt", Cfg->SystemPath);
   if ((fp = fopen (Temp, "wt")) != NULL) {
      item = LM_QueryItemCount (107);
      for (i = 0; i < item; i++) {
         LM_QueryItemText (107, i, (USHORT)(sizeof (Temp) - 1), Temp);
         fprintf (fp, "%s\n", Temp);
      }
      fclose (fp);
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Nodelist flags
// ----------------------------------------------------------------------

class CModifyFlagsDlg : public CDialog
{
public:
   CModifyFlagsDlg (HWND p_hWnd);

   CHAR   Flags[64];
   CHAR   Cmd[64];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

BEGIN_MESSAGE_MAP (CNodeFlagsDlg, CDialog)
   ON_COMMAND (102, Add)
   ON_COMMAND (104, Remove)
   ON_COMMAND (103, Change)
END_MESSAGE_MAP ()

CNodeFlagsDlg::CNodeFlagsDlg (HWND p_hWnd) : CDialog ("55", p_hWnd)
{
}

VOID CNodeFlagsDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 55);
}

USHORT CNodeFlagsDlg::OnInitDialog (VOID)
{
   Center ();
   class TNodeFlags *Data;

   EM_SetTextLimit (106, sizeof (Cfg->CallIf) - 1);
   EM_SetTextLimit (108, sizeof (Cfg->DontCallIf) - 1);

   LVM_AllocateColumns (101, 2);
   LVM_InsertColumn (101, "Flags", LVC_LEFT);
   LVM_InsertColumn (101, "Dial command", LVC_LEFT);

   if ((Data = new TNodeFlags (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            LVM_InsertItem (101);

            LVM_SetItemText (101, 0, Data->Flags);
            LVM_SetItemText (101, 1, Data->Cmd);
         } while (Data->Next () == TRUE);
      delete Data;
   }

   LVM_InvalidateView (101);

   SetDlgItemText (106, Cfg->CallIf);
   SetDlgItemText (108, Cfg->DontCallIf);

   return (TRUE);
}

VOID CNodeFlagsDlg::OnOK (VOID)
{
   USHORT i, count;
   CHAR Temp[128];
   class TNodeFlags *Data;

   if ((Data = new TNodeFlags (Cfg->SystemPath)) != NULL) {
      Data->DeleteAll ();
      count = (USHORT)LVM_QueryItemCount (101);
      for (i = 0; i < count; i++) {
         LVM_QueryItemText (101, i, 0, Temp);
         strcpy (Data->Flags, Temp);
         LVM_QueryItemText (101, i, 1, Temp);
         strcpy (Data->Cmd, Temp);
         Data->Add ();
      }
      Data->Save ();
      delete Data;
   }

   GetDlgItemText (106, Cfg->CallIf, GetDlgItemTextLength (106));
   GetDlgItemText (108, Cfg->DontCallIf, GetDlgItemTextLength (108));

   EndDialog (TRUE);
}

VOID CNodeFlagsDlg::Add (VOID)
{
   class CModifyFlagsDlg *Dlg;

   if ((Dlg = new CModifyFlagsDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         LVM_InsertItem (101);
         LVM_SetItemText (101, 0, Dlg->Flags);
         LVM_SetItemText (101, 1, Dlg->Cmd);
         LVM_InvalidateView (101);
      }
      delete Dlg;
   }
}

VOID CNodeFlagsDlg::Change (VOID)
{
   USHORT item;
   CHAR Temp[64];
   class CModifyFlagsDlg *Dlg;

   if ((item = (USHORT)LVM_QuerySelectedItem (101)) != -1) {
      if ((Dlg = new CModifyFlagsDlg (m_hWnd)) != NULL) {
         LVM_QueryItemText (101, item, 0, Temp);
         strcpy (Dlg->Flags, Temp);
         LVM_QueryItemText (101, item, 1, Temp);
         strcpy (Dlg->Cmd, Temp);

         if (Dlg->DoModal () == TRUE) {
            LVM_SetItemText (101, item, 0, Dlg->Flags);
            LVM_SetItemText (101, item, 1, Dlg->Cmd);
            LVM_InvalidateView (101);
         }
         delete Dlg;
      }
   }
}

VOID CNodeFlagsDlg::Remove (VOID)
{
   LVM_DeleteItem (101, LVM_QuerySelectedItem (101));
}

// ----------------------------------------------------------------------

CModifyFlagsDlg::CModifyFlagsDlg (HWND p_hWnd) : CDialog ("56", p_hWnd)
{
   Flags[0] = '\0';
   Cmd[0] = '\0';
}

USHORT CModifyFlagsDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (106, sizeof (Flags) - 1);
   EM_SetTextLimit (102, sizeof (Cmd) - 1);

   SetDlgItemText (106, Flags);
   SetDlgItemText (102, Cmd);

   return (TRUE);
}

VOID CModifyFlagsDlg::OnOK (VOID)
{
   GetDlgItemText (106, GetDlgItemTextLength (106), Flags);
   GetDlgItemText (102, GetDlgItemTextLength (102), Cmd);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Translation table
// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CTranslationDlg, CDialog)
   ON_COMMAND (187, Add)
   ON_COMMAND (188, Delete)
   ON_COMMAND (189, List)
   ON_COMMAND (190, Previous)
   ON_COMMAND (191, Next)
END_MESSAGE_MAP ()

CTranslationDlg::CTranslationDlg (HWND p_hWnd) : CDialog ("61", p_hWnd)
{
   Data = NULL;
}

CTranslationDlg::~CTranslationDlg (void)
{
   if (Data != NULL)
      delete Data;
}

VOID CTranslationDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 61);
}

USHORT CTranslationDlg::OnInitDialog (VOID)
{
   Center ();

   return (TRUE);
}

VOID CTranslationDlg::OnOK (VOID)
{
}

VOID CTranslationDlg::DisplayData (VOID)
{
}

VOID CTranslationDlg::ReadData (VOID)
{
}

VOID CTranslationDlg::Add (VOID)
{
}

VOID CTranslationDlg::Copy (VOID)
{
}

VOID CTranslationDlg::Delete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      DisplayData ();
   }
}

VOID CTranslationDlg::List (VOID)
{
}

VOID CTranslationDlg::Next (VOID)
{
}

VOID CTranslationDlg::Previous (VOID)
{
}

