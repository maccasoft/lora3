
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.11
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

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
// Mailer addresses
// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CAddressDlg, CDialog)
   ON_COMMAND (102, Add)
   ON_COMMAND (103, Remove)
END_MESSAGE_MAP ()

CAddressDlg::CAddressDlg (HWND p_hWnd) : CDialog ("6", p_hWnd)
{
}

USHORT CAddressDlg::OnInitDialog (VOID)
{
   CHAR Temp[16];

   Center ();

   EM_SetTextLimit (108, sizeof (Cfg->MailAddress.String) - 1);
   if (Cfg->MailAddress.First () == TRUE)
      do {
         LM_AddString (101, Cfg->MailAddress.String);
      } while (Cfg->MailAddress.Next () == TRUE);

   sprintf (Temp, "%u", Cfg->FakeNet);
   SetDlgItemText (109, Temp);

   return (TRUE);
}

VOID CAddressDlg::OnOK (VOID)
{
   USHORT i, Max;
   CHAR Temp[64];

   Cfg->MailAddress.Clear ();
   if ((Max = LM_QueryItemCount (101)) > 0) {
      for (i = 0; i < Max; i++) {
         LM_QueryItemText (101, i, sizeof (Temp) - 1, Temp);
         Cfg->MailAddress.Add (Temp);
      }
   }

   GetDlgItemText (109, GetDlgItemTextLength (109), Temp);
   Cfg->FakeNet = (USHORT)atoi (Temp);

   EndDialog (TRUE);
}

VOID CAddressDlg::Add (VOID)
{
   CHAR Temp[64];

   GetDlgItemText (108, GetDlgItemTextLength (108), Temp);
   if (Temp[0] != '\0') {
      Cfg->MailAddress.Parse (Temp);
      LM_AddString (101, Cfg->MailAddress.String);
   }

   SetDlgItemText (108, "");
   SetFocus (108);
}

VOID CAddressDlg::Remove (VOID)
{
   LM_DeleteItem (101, LM_QuerySelection (101));
}

// ----------------------------------------------------------------------
// Directories / Paths
// ----------------------------------------------------------------------

CDirectoriesDlg::CDirectoriesDlg (HWND p_hWnd) : CDialog ("5", p_hWnd)
{
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

   SetDlgItemText (110, Cfg->SystemPath);
   SetDlgItemText (102, Cfg->NormalInbound);
   SetDlgItemText (118, Cfg->KnownInbound);
   SetDlgItemText (120, Cfg->ProtectedInbound);
   SetDlgItemText (114, Cfg->Outbound);
   SetDlgItemText (106, Cfg->NodelistPath);

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

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Modem commands
// ----------------------------------------------------------------------

CCommandsDlg::CCommandsDlg (HWND p_hWnd) : CDialog ("2", p_hWnd)
{
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
      if ((p = strtok (Temp, ":")) != NULL)
         Cfg->StartTime += (USHORT)atoi (p);
   }
   GetDlgItemText (108, GetDlgItemTextLength (108), Temp);
   if ((p = strtok (Temp, ":")) != NULL) {
      Cfg->EndTime = (USHORT)(atoi (p) * 60);
      if ((p = strtok (Temp, ":")) != NULL)
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

   VOID   OnChanged (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   DECLARE_MESSAGE_MAP ()
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
   Data->New ();
   ReadData ();
   Data->Add ();
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

BEGIN_MESSAGE_MAP (CEventsListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
#if defined(__NT__)
   ON_NOTIFY (LVN_ITEMCHANGED, 101, OnChanged)
#endif
END_MESSAGE_MAP ()

CEventsListDlg::CEventsListDlg (HWND p_hWnd) : CDialog ("25", p_hWnd)
{
   Selected = -1;
}

USHORT CEventsListDlg::OnInitDialog (VOID)
{
   int i = 1;
   CHAR Temp[64];
   class TEvents *Events;

   SetWindowTitle ("Events list");

   LVM_AllocateColumns (101, 6);
   LVM_InsertColumn (101, "#", LVC_RIGHT);
   LVM_InsertColumn (101, "Title", LVC_LEFT);
   LVM_InsertColumn (101, "Days", LVC_LEFT);
   LVM_InsertColumn (101, "Start", LVC_RIGHT);
   LVM_InsertColumn (101, "Length", LVC_RIGHT);
   LVM_InsertColumn (101, "Command", LVC_LEFT);

   if ((Events = new TEvents (Cfg->SchedulerFile)) != NULL) {
      Events->Load ();
      if (Events->First () == TRUE)
         do {
            LVM_InsertItem (101);

            sprintf (Temp, "%d", i++);
            LVM_SetItemText (101, 0, Temp);
            LVM_SetItemText (101, 1, Events->Label);
            strcpy (Temp, "-------");
            if (Events->Sunday == TRUE)
               Temp[0] = 'S';
            if (Events->Monday == TRUE)
               Temp[1] = 'M';
            if (Events->Tuesday == TRUE)
               Temp[2] = 'T';
            if (Events->Wednesday == TRUE)
               Temp[3] = 'W';
            if (Events->Thursday == TRUE)
               Temp[4] = 'T';
            if (Events->Friday == TRUE)
               Temp[5] = 'F';
            if (Events->Saturday == TRUE)
               Temp[6] = 'S';
            LVM_SetItemText (101, 2, Temp);
            sprintf (Temp, "%d:%02d", Events->Hour, Events->Minute);
            LVM_SetItemText (101, 3, Temp);
            sprintf (Temp, "%d", Events->Length);
            LVM_SetItemText (101, 4, Temp);
            LVM_SetItemText (101, 5, Events->Command);
         } while (Events->Next () == TRUE);
      delete Events;
   }

   LVM_InvalidateView (101);

   return (TRUE);
}

VOID CEventsListDlg::OnChanged (VOID)
{
#if defined(__NT__)
   NM_LISTVIEW *pnmv = (NM_LISTVIEW *)m_lParam;

   if (!(pnmv->uNewState & LVIS_SELECTED) && (pnmv->uOldState & LVIS_SELECTED))
      Selected = -1;
   if (pnmv->uNewState & LVIS_SELECTED)
      Selected = pnmv->iItem;
#endif
}

VOID CEventsListDlg::OnOK (VOID)
{
   int i = 1;

#if defined(__OS2__)
   LV_PLISTDATA pRecord;

   if ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, 101, CM_QUERYRECORDEMPHASIS, MPFROMSHORT (CMA_FIRST), MPFROMSHORT (CRA_SELECTED))) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (atoi (pRecord->Column[0].Data) == i++)
               break;
         } while (Data->Next () == TRUE);
   }
#elif defined(__NT__)
   CHAR Temp[32];

   if (Selected != -1) {
      ListView_GetItemText (GetDlgItem (m_hWnd, 101), Selected, 0, Temp, sizeof (Temp));
      if (Data->First () == TRUE)
         do {
            if (atoi (Temp) == i++)
               break;
         } while (Data->Next () == TRUE);
   }
#endif

   EndDialog (TRUE);
}
#endif

// ----------------------------------------------------------------------
// Modem hardware
// ----------------------------------------------------------------------

CGeneralDlg::CGeneralDlg (HWND p_hWnd) : CDialog ("7", p_hWnd)
{
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

   return (TRUE);
}

VOID CGeneralDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->LogFile);
   Cfg->TaskNumber = SPBM_QueryValue (104);
   GetDlgItemText (109, GetDlgItemTextLength (109), Cfg->SchedulerFile);
   GetDlgItemText (112, GetDlgItemTextLength (112), Cfg->FaxCommand);
   GetDlgItemText (106, GetDlgItemTextLength (106), Cfg->AfterCallerCmd);
   GetDlgItemText (110, GetDlgItemTextLength (110), Cfg->AfterMailCmd);
   Cfg->ZModemTelnet = (UCHAR)BM_QueryCheck (113);

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

VOID CHardwareDlg::OnOK (VOID)
{
   CHAR Temp[32];

   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->Device);
   GetDlgItemText (104, GetDlgItemTextLength (104), Temp);
   Cfg->Speed = atol (Temp);
   GetDlgItemText (108, GetDlgItemTextLength (108), Cfg->FaxMessage);
   GetDlgItemText (116, GetDlgItemTextLength (116), Cfg->Ring);
   Cfg->DialTimeout = SPBM_QueryValue (112);
   Cfg->CarrierDropTimeout = SPBM_QueryValue (114);
   Cfg->LockSpeed = BM_QueryCheck (105);
   Cfg->StripDashes = BM_QueryCheck (110);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Nodes editor
// ----------------------------------------------------------------------

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

BEGIN_MESSAGE_MAP (CNodesDlg, CDialog)
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
   EM_SetTextLimit (111, sizeof (Data->NewAreasFilter) - 1);

   if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
      if (Packer->First () == TRUE)
         do {
            CB_AddString (112, Packer->Display);
         } while (Packer->Next ());
      delete Packer;
   }

   if (Data->First () == FALSE)
      Data->New ();

   DisplayData ();

   return (TRUE);
}

VOID CNodesDlg::OnOK (VOID)
{
   CHAR NewAddress[64];

   GetDlgItemText (102, GetDlgItemTextLength (102), NewAddress);
   if (stricmp (NewAddress, Data->Address)) {
      Data->Delete ();
      Data->New ();
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
   BM_SetCheck (109, Data->CreateNewAreas);
   SetDlgItemText (111, Data->NewAreasFilter);

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
   Data->CreateNewAreas = (UCHAR)BM_QueryCheck (109);
   GetDlgItemText (111, GetDlgItemTextLength (111), Data->NewAreasFilter);

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
   Data->New ();
   ReadData ();
   Data->Add ();
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

USHORT CInternetDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Cfg->HostName) - 1);
   EM_SetTextLimit (128, sizeof (Cfg->NewsServer) - 1);
   EM_SetTextLimit (104, sizeof (Cfg->MailServer) - 1);
   SPBM_SetLimits (107, 1440, 0);
   EM_SetTextLimit (109, sizeof (Cfg->PPPCmd) - 1);

   SetDlgItemText (102, Cfg->HostName);
   SetDlgItemText (128, Cfg->NewsServer);
   SetDlgItemText (104, Cfg->MailServer);
   BM_SetCheck (105, Cfg->EnablePPP);
   SPBM_SetCurrentValue (107, Cfg->PPPTimeLimit);
   SetDlgItemText (109, Cfg->PPPCmd);

   return (TRUE);
}

VOID CInternetDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->HostName);
   GetDlgItemText (128, GetDlgItemTextLength (128), Cfg->NewsServer);
   GetDlgItemText (104, GetDlgItemTextLength (104), Cfg->MailServer);
   Cfg->EnablePPP = (UCHAR)BM_QueryCheck (105);
   Cfg->PPPTimeLimit = SPBM_QueryValue (107);
   GetDlgItemText (109, GetDlgItemTextLength (109), Cfg->PPPCmd);

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

USHORT CMailprocDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (106, sizeof (Cfg->NetMailPath) - 1);
   EM_SetTextLimit (104, sizeof (Cfg->DupePath) - 1);
   EM_SetTextLimit (112, sizeof (Cfg->BadPath) - 1);
   EM_SetTextLimit (117, sizeof (Cfg->TearLine) - 1);

   CB_AddString (107, "Squish<tm>");
   CB_AddString (107, "JAM");
   CB_AddString (107, "Fido (*.msg)");
   CB_AddString (107, "AdeptXBBS");

   CB_AddString (102, "Squish<tm>");
   CB_AddString (102, "JAM");
   CB_AddString (102, "Fido (*.msg)");
   CB_AddString (102, "AdeptXBBS");

   CB_AddString (110, "Squish<tm>");
   CB_AddString (110, "JAM");
   CB_AddString (110, "Fido (*.msg)");
   CB_AddString (110, "AdeptXBBS");

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
   }

   BM_SetCheck (113, Cfg->ImportEmpty);
   BM_SetCheck (114, Cfg->ForceIntl);
   BM_SetCheck (115, Cfg->ReplaceTear);
   SetDlgItemText (117, Cfg->TearLine);

   BM_SetCheck (118, Cfg->SeparateNetMail);
   BM_SetCheck (119, Cfg->UseSinglePass);

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
   }

   Cfg->ImportEmpty = (UCHAR)BM_QueryCheck (113);
   Cfg->ForceIntl = (UCHAR)BM_QueryCheck (114);
   Cfg->ReplaceTear = (UCHAR)BM_QueryCheck (115);
   GetDlgItemText (117, GetDlgItemTextLength (117), Cfg->TearLine);
   Cfg->SeparateNetMail = (UCHAR)BM_QueryCheck (118);
   Cfg->UseSinglePass = (UCHAR)BM_QueryCheck (119);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Miscellaneous mailer
// ----------------------------------------------------------------------

CMiscDlg::CMiscDlg (HWND p_hWnd) : CDialog ("20", p_hWnd)
{
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

USHORT CAreafixDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (104, sizeof (Cfg->AreasBBS) - 1);
   EM_SetTextLimit (112, sizeof (Cfg->NewAreasPath) - 1);
   CB_AddString (110, "Squish<tm>");
   CB_AddString (110, "JAM");
   CB_AddString (110, "Fido (*.msg)");
   CB_AddString (110, "AdeptXBBS");

   SetDlgItemText (104, Cfg->AreasBBS);
   BM_SetCheck (101, Cfg->UseAreasBBS);
   BM_SetCheck (102, Cfg->UpdateAreasBBS);
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
   Cfg->UseAreasBBS = (UCHAR)BM_QueryCheck (101);
   Cfg->UpdateAreasBBS = (UCHAR)BM_QueryCheck (102);
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



