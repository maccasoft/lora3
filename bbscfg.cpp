
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lorawin.h"

#if !defined(__POINT__)
class CColorDlg : public CDialog
{
public:
   CColorDlg (HWND p_hWnd);

   UCHAR  color;
   UCHAR  foreground;
   UCHAR  background;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

CColorDlg::CColorDlg (HWND p_hWnd) : CDialog ("29", p_hWnd)
{
   color = 0;
   foreground = background = 0;
}

USHORT CColorDlg::OnInitDialog (VOID)
{
   Center ();

   LM_AddString (103, "Black");
   LM_AddString (103, "Blue");
   LM_AddString (103, "Green");
   LM_AddString (103, "Cyan");
   LM_AddString (103, "Red");
   LM_AddString (103, "Purple");
   LM_AddString (103, "Brown");
   LM_AddString (103, "White");
   LM_AddString (103, "Grey");
   LM_AddString (103, "Bright Blue");
   LM_AddString (103, "Bright Green");
   LM_AddString (103, "Bright Cyan");
   LM_AddString (103, "Bright Red");
   LM_AddString (103, "Bright Purple");
   LM_AddString (103, "Bright Yellow");
   LM_AddString (103, "Bright White");

   LM_AddString (104, "Black");
   LM_AddString (104, "Blue");
   LM_AddString (104, "Green");
   LM_AddString (104, "Cyan");
   LM_AddString (104, "Red");
   LM_AddString (104, "Purple");
   LM_AddString (104, "Brown");
   LM_AddString (104, "White");

   LM_SelectItem (103, (USHORT)(color & 0x0F));
   LM_SelectItem (104, (USHORT)((color & 0x70) >> 4));
   if (color & 0x80)
      BM_SetCheck (105, TRUE);

   return (TRUE);
}

VOID CColorDlg::OnOK (VOID)
{
   foreground = (UCHAR)LM_QuerySelection (103);
   background = (UCHAR)LM_QuerySelection (104);
   if (BM_QueryCheck (105) == TRUE)
      background |= 0x08;

   color = (UCHAR)((background << 4) | foreground);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// New user options
// ----------------------------------------------------------------------

class CNewUserSecurityDlg : public CDialog
{
public:
   CNewUserSecurityDlg (HWND p_hWnd);

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

BEGIN_MESSAGE_MAP (CNewUserDlg, CDialog)
   ON_COMMAND (113, NewUserSecurity)
END_MESSAGE_MAP ()

CNewUserDlg::CNewUserDlg (HWND p_hWnd) : CDialog ("21", p_hWnd)
{
}

USHORT CNewUserDlg::OnInitDialog (VOID)
{
   class TLimits *Limits;

   Center ();
   SetFocus (101);

   BM_SetCheck (102 + Cfg->CheckAnsi, TRUE);
   BM_SetCheck (106 + Cfg->RealName, TRUE);
   BM_SetCheck (110 + Cfg->CompanyName, TRUE);
   BM_SetCheck (115 + Cfg->Address, TRUE);
   BM_SetCheck (119 + Cfg->City, TRUE);
   BM_SetCheck (123 + Cfg->PhoneNumber, TRUE);
   BM_SetCheck (127 + Cfg->Gender, TRUE);

   if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
      if (Limits->First () == TRUE)
         do {
            LM_AddString (130, Limits->Key);
         } while (Limits->Next () == TRUE);
         delete Limits;
   }
   SetDlgItemText (130, Cfg->NewUserLimits);

   return (TRUE);
}

VOID CNewUserDlg::OnOK (VOID)
{
   if (BM_QueryCheck (102) == TRUE)
      Cfg->CheckAnsi = NO;
   else if (BM_QueryCheck (103) == TRUE)
      Cfg->CheckAnsi = YES;
   if (BM_QueryCheck (106) == TRUE)
      Cfg->RealName = NO;
   else if (BM_QueryCheck (107) == TRUE)
      Cfg->RealName = YES;
   else if (BM_QueryCheck (108) == TRUE)
      Cfg->RealName = REQUIRED;
   if (BM_QueryCheck (110) == TRUE)
      Cfg->CompanyName = NO;
   else if (BM_QueryCheck (111) == TRUE)
      Cfg->CompanyName = YES;
   else if (BM_QueryCheck (112) == TRUE)
      Cfg->CompanyName = REQUIRED;
   if (BM_QueryCheck (115) == TRUE)
      Cfg->Address = NO;
   else if (BM_QueryCheck (116) == TRUE)
      Cfg->Address = YES;
   else if (BM_QueryCheck (117) == TRUE)
      Cfg->Address = REQUIRED;
   if (BM_QueryCheck (119) == TRUE)
      Cfg->City = NO;
   else if (BM_QueryCheck (120) == TRUE)
      Cfg->City = YES;
   else if (BM_QueryCheck (121) == TRUE)
      Cfg->City = REQUIRED;
   if (BM_QueryCheck (123) == TRUE)
      Cfg->PhoneNumber = NO;
   else if (BM_QueryCheck (124) == TRUE)
      Cfg->PhoneNumber = YES;
   else if (BM_QueryCheck (125) == TRUE)
      Cfg->PhoneNumber = REQUIRED;
   if (BM_QueryCheck (127) == TRUE)
      Cfg->Gender = NO;
   else if (BM_QueryCheck (128) == TRUE)
      Cfg->Gender = YES;
   else if (BM_QueryCheck (129) == TRUE)
      Cfg->Gender = REQUIRED;

   GetDlgItemText (130, Cfg->NewUserLimits, GetDlgItemTextLength (130));

   EndDialog (TRUE);
}

VOID CNewUserDlg::NewUserSecurity (VOID)
{
   class CNewUserSecurityDlg *Dlg;

   if ((Dlg = new CNewUserSecurityDlg (m_hWnd)) != NULL) {
      Dlg->DoModal ();
      delete Dlg;
   }
}

// ----------------------------------------------------------------------

CNewUserSecurityDlg::CNewUserSecurityDlg (HWND p_hWnd) : CDialog ("18", p_hWnd)
{
}

USHORT CNewUserSecurityDlg::OnInitDialog (VOID)
{
   USHORT i;
   ULONG Test;

   Center ();
   SetWindowTitle ("New Users Security");

   SPBM_SetLimits (102, 65535U, 0L);
   SPBM_SetCurrentValue (102, Cfg->NewUserLevel);

   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (Cfg->NewUserFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (Cfg->NewUserDenyFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   return (TRUE);
}

VOID CNewUserSecurityDlg::OnOK (VOID)
{
   USHORT i;
   ULONG Test;

   Cfg->NewUserLevel = SPBM_QueryValue (102);

   Cfg->NewUserFlags = 0L;
   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Cfg->NewUserFlags |= Test;
   }
   Cfg->NewUserDenyFlags = 0L;
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Cfg->NewUserDenyFlags |= Test;
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// Menu files editor
// ----------------------------------------------------------------------

typedef struct {
   USHORT Id;
   PSZ    Text;
} MENUCMD;

MENUCMD MenuCmd[] = {
   // Moving between menu
   MNU_CLEARGOTO, "Clear goto menu",
   MNU_CLEARGOSUB, "Clear gosub menu",
   MNU_RETURNMAIN, "Return to MAIN menu",
   MNU_GOTO, "Goto menu",
   MNU_GOSUB, "Gosub menu",
   MNU_RETURN, "Return to previous",
   MNU_CLEARSTACK, "Clear menu stack",

   // Message areas
   MNU_MSGDELETE, "Kill message",
   MNU_MSGWRITE, "Edit new message",
   MNU_MSGREPLY, "Reply to message",
   MNU_MSGBRIEFLIST, "Short message list",
   MNU_MSGFORWARD, "Read next message",
   MNU_MSGBACKWARD, "Read previous message",
   MNU_MSGTEXTLIST, "Read message non-stop",
   MNU_INQUIRETEXT, "Inquire messages",
   MNU_MSGINDIVIDUAL, "Read individual message",
   MNU_MSGTITLELIST, "Verbose message list",
   MNU_MSGLISTNEWAREAS, "List areas w/new messages",
   MNU_MSGREAD, "Read messages",
   MNU_INQUIREPERSONAL, "Inquire personal messages",
   MNU_MSGSELECT, "Change message area",
   MNU_MSGMODIFY, "Change message",

   // File areas
   MNU_FILENAMELIST, "File list",
   MNU_FILEDOWNLOAD, "Download file",
   MNU_FILETEXTLIST, "Locate files",
   MNU_FILEUPLOAD, "Upload file",
   MNU_FILENEWLIST, "New files list",
   MNU_FILEDOWNLOADANY, "Download from any area",
   MNU_FILETAGGED, "Manage tagged files",
   MNU_FILEDELETE, "Kill files",
   MNU_FILESELECT, "Change file area",
   MNU_SEARCHFILENAME, "Locate files by name",
   MNU_FILEKEYWORDLIST, "Locate files by keyword",
   MNU_FILEDATELIST, "File list by date",
   MNU_FILEDOWNLOADLIST, "Download list of files",
   MNU_FILEUPLOADUSER, "Upload file to user",
   MNU_FILEDISPLAY, "File Display",

   // User configuration
   MNU_SETLANGUAGE, "Change language",
   MNU_SETPASSWORD, "Set password",
   MNU_TOGGLEANSI, "Toggle ANSI graphics",
   MNU_TOGGLEAVATAR, "Toggle AVATAR graphics",
   MNU_TOGGLECOLOR, "Toggle COLOR codes",
   MNU_TOGGLEHOTKEY, "Toggle hot-keyed menu",
   MNU_SETCOMPANY, "Set company name",
   MNU_SETADDRESS, "Set address",
   MNU_SETCITY, "Set city",
   MNU_SETPHONE, "Set phone number",
   MNU_SETGENDER, "Set gender",

   // Personal mail
   MNU_MAILLIST, "List mail",
   MNU_MAILREAD, "Read mail",
   MNU_MAILWRITE, "Edit new mail",
   MNU_MAILDELETE, "Kill mail",
   MNU_MAILCHECK, "Check unread mail",

   // Offline reader
   MNU_OLRTAGAREA, "Tag areas",
   MNU_OLRDOWNLOADASCII, "ASCII download",
   MNU_OLRUPLOAD, "Upload replies",
   MNU_OLRDOWNLOADQWK, "QWK download",
   MNU_OLRDOWNLOADBW, "BlueWave download",
   MNU_OLRDOWNLOADPNT, "PointMail download",
   MNU_OLRREMOVEAREA, "Untag areas",
   MNU_OLRVIEWTAGGED, "View tagged areas",

   // Miscellaneous
   MNU_DISPLAY, "Display file (anywhere)",
   MNU_NULL, "Display only",
   MNU_LOGOFF, "Logoff",
   MNU_VERSION, "Version information",
   MNU_RUNEXTERNAL, "Run external program",
   MNU_PRESSENTER, "Press Enter to Continue",
   MNU_FINGER, "Finger",
   MNU_FTP, "FTP client",
//   MNU_GOPHER, "Gopher Client",
   MNU_TELNET, "Telnet client",
   MNU_ONLINEUSERS, "Users online",
   0, NULL
};

class CMenuSecurityDlg : public CDialog
{
public:
   CMenuSecurityDlg (HWND p_hWnd);

   class  TMenu *Data;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CMenuPromptDlg : public CDialog
{
public:
   CMenuPromptDlg (HWND p_hWnd);

   DECLARE_MESSAGE_MAP ()
   class  TMenu *Menu;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   VOID   Color (VOID);
   VOID   HilightColor (VOID);
};

class CMenuListDlg : public CDialog
{
public:
   CMenuListDlg (HWND p_hWnd);

   int    Selected;
   CHAR   File[128];
   class  TMenu *Data;

   VOID   OnChanged (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   CHAR   Temp[128];
   DECLARE_MESSAGE_MAP ()
};

BEGIN_MESSAGE_MAP (CMenuDlg, CDialog)
   ON_COMMAND (115, Delete)
   ON_COMMAND (116, Command)
   ON_COMMAND (117, Add)
   ON_COMMAND (118, Previous)
   ON_COMMAND (119, Next)
   ON_COMMAND (120, Prompt)
   ON_COMMAND (121, List)
   ON_COMMAND (122, Security)
   ON_COMMAND (123, Insert)
   ON_COMMAND (124, HilightColor)
   ON_COMMAND (125, Color)
END_MESSAGE_MAP ()

CMenuDlg::CMenuDlg (HWND p_hWnd) : CDialog ("8", p_hWnd)
{
   Menu = NULL;
}

CMenuDlg::~CMenuDlg (void)
{
   if (Menu != NULL)
      delete Menu;
}

USHORT CMenuDlg::OnInitDialog (VOID)
{
   int i;
   CHAR Temp[128];

   FullFile[0] = '\0';

#if defined(__OS2__)
   memset (&fild, 0, sizeof (FILEDLG));
   fild.cbSize = sizeof (FILEDLG);
   fild.fl = FDS_CENTER|FDS_OPEN_DIALOG;
   fild.pszTitle = "Open Menu File";
   sprintf (fild.szFullFile, "*.mnu");

   WinFileDlg (HWND_DESKTOP, m_hWnd, &fild);
   if (fild.lReturn == DID_OK) {
      strcpy (FullFile, fild.szFullFile);
#elif defined(__NT__)
   OpenFileName.lStructSize = sizeof (OPENFILENAME);
   OpenFileName.hwndOwner = m_hWnd;
   OpenFileName.hInstance = NULL;
   OpenFileName.lpstrFilter = "Menu files (*.mnu)\0*.mnu\0All files (*.*)\0*.*";
   OpenFileName.lpstrCustomFilter = (LPTSTR) NULL;
   OpenFileName.nMaxCustFilter = 0L;
   OpenFileName.nFilterIndex = 1L;
   OpenFileName.lpstrFile = FullFile;
   OpenFileName.nMaxFile = sizeof (FullFile);
   OpenFileName.lpstrFileTitle = NULL;
   OpenFileName.nMaxFileTitle = 0;
   OpenFileName.lpstrInitialDir = NULL;
   OpenFileName.lpstrTitle = "Open Menu File";
   OpenFileName.nFileOffset = 0;
   OpenFileName.nFileExtension = 0;
   OpenFileName.lpstrDefExt = "mnu";
   OpenFileName.lCustData = 0;
   OpenFileName.Flags = OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_CREATEPROMPT|OFN_NOCHANGEDIR;

   if (GetOpenFileName (&OpenFileName) == TRUE) {
#endif
      Center ();

      if ((Menu = new TMenu) != NULL) {
         Menu->Load (FullFile);
         if (Menu->First () == FALSE)
            Menu->New (TRUE);
      }

      sprintf (Temp, "Menu Editor - %s", FullFile);
      SetWindowTitle (Temp);

      EM_SetTextLimit (102, sizeof (Menu->Display) - 1);
      EM_SetTextLimit (108, sizeof (Menu->Key) - 1);
      EM_SetTextLimit (112, sizeof (Menu->Argument) - 1);
      SPBM_SetLimits (104, 255, 0);
      SPBM_SetLimits (106, 255, 0);

      for (i = 0; MenuCmd[i].Text != NULL; i++) {
         sprintf (Temp, "%u - %s", MenuCmd[i].Id, MenuCmd[i].Text);
         CB_AddString (110, Temp);
      }

      DisplayData ();
   }
   else
      EndDialog (FALSE);

   return (TRUE);
}

VOID CMenuDlg::OnOK (VOID)
{
   ReadData ();
   Menu->Update ();
   Menu->Save (FullFile);
   SetFocus (102);
}

VOID CMenuDlg::DisplayData (VOID)
{
   USHORT i;

   SetDlgItemText (102, Menu->Display);
   SPBM_SetCurrentValue (104, Menu->Color);
   SPBM_SetCurrentValue (106, Menu->Hilight);
   SetDlgItemText (108, Menu->Key);

   for (i = 0; MenuCmd[i].Text != NULL; i++) {
      if (MenuCmd[i].Id == Menu->Command) {
         CB_SelectItem (110, i);
         break;
      }
   }

   BM_SetCheck (113, Menu->Automatic);
   BM_SetCheck (114, Menu->FirstTime);
   SetDlgItemText (112, Menu->Argument);
}

VOID CMenuDlg::ReadData (VOID)
{
   USHORT Value;

   GetDlgItemText (102, GetDlgItemTextLength (102), Menu->Display);
   Menu->Color = (UCHAR)SPBM_QueryValue (104);
   Menu->Hilight = (UCHAR)SPBM_QueryValue (106);
   GetDlgItemText (108, GetDlgItemTextLength (108), Menu->Key);
   GetDlgItemText (112, GetDlgItemTextLength (112), Menu->Argument);
   Value = CB_QuerySelection (110);
   Menu->Command = MenuCmd[Value].Id;
   Menu->Automatic = (UCHAR)BM_QueryCheck (113);
   Menu->FirstTime = (UCHAR)BM_QueryCheck (114);
}

VOID CMenuDlg::Next (VOID)
{
   ReadData ();
   Menu->Update ();
   if (Menu->Next () == TRUE)
      DisplayData ();
   SetFocus (102);
}

VOID CMenuDlg::Previous (VOID)
{
   ReadData ();
   Menu->Update ();
   if (Menu->Previous () == TRUE)
      DisplayData ();
   SetFocus (102);
}

VOID CMenuDlg::Add (VOID)
{
   ReadData ();
   Menu->Update ();
   Menu->New (FALSE);
   Menu->Add ();
   DisplayData ();
   SetFocus (102);
}

VOID CMenuDlg::Insert (VOID)
{
//   ReadData ();
   Menu->New (FALSE);
   Menu->Insert ();
   DisplayData ();
   SetFocus (102);
}

VOID CMenuDlg::Delete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      Menu->Delete ();
      DisplayData ();
      SetFocus (102);
   }
}

VOID CMenuDlg::Security (VOID)
{
   class CMenuSecurityDlg *Dlg;

   if ((Dlg = new CMenuSecurityDlg (m_hWnd)) != NULL) {
      Dlg->Data = Menu;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CMenuDlg::Prompt (VOID)
{
   class CMenuPromptDlg *Dlg;

   if ((Dlg = new CMenuPromptDlg (m_hWnd)) != NULL) {
      Dlg->Menu = Menu;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CMenuDlg::List (VOID)
{
   class CMenuListDlg *Dlg;

   if ((Dlg = new CMenuListDlg (m_hWnd)) != NULL) {
      Dlg->Data = Menu;
      strcpy (Dlg->File, FullFile);
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CMenuDlg::Color (VOID)
{
   class CColorDlg *Dlg;

   if ((Dlg = new CColorDlg (m_hWnd)) != NULL) {
      Dlg->color = (UCHAR)SPBM_QueryValue (104);
      if (Dlg->DoModal () == TRUE)
         SPBM_SetCurrentValue (104, Dlg->color);
      delete Dlg;
   }
}

VOID CMenuDlg::HilightColor (VOID)
{
   class CColorDlg *Dlg;

   if ((Dlg = new CColorDlg (m_hWnd)) != NULL) {
      Dlg->color = (UCHAR)SPBM_QueryValue (106);
      if (Dlg->DoModal () == TRUE)
         SPBM_SetCurrentValue (106, Dlg->color);
      delete Dlg;
   }
}

VOID CMenuDlg::Command (VOID)
{
}

// ----------------------------------------------------------------------

CMenuSecurityDlg::CMenuSecurityDlg (HWND p_hWnd) : CDialog ("18", p_hWnd)
{
}

USHORT CMenuSecurityDlg::OnInitDialog (VOID)
{
   USHORT i;
   ULONG Test;

   Center ();

   SPBM_SetLimits (102, 65535U, 0L);
   SPBM_SetCurrentValue (102, Data->Level);

   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (Data->AccessFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (Data->DenyFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   return (TRUE);
}

VOID CMenuSecurityDlg::OnOK (VOID)
{
   USHORT i;
   ULONG Test;

   Data->Level = SPBM_QueryValue (102);

   Data->AccessFlags = 0L;
   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->AccessFlags |= Test;
   }
   Data->DenyFlags = 0L;
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->DenyFlags |= Test;
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CMenuPromptDlg, CDialog)
   ON_COMMAND (124, HilightColor)
   ON_COMMAND (125, Color)
END_MESSAGE_MAP ()

CMenuPromptDlg::CMenuPromptDlg (HWND p_hWnd) : CDialog ("19", p_hWnd)
{
}

USHORT CMenuPromptDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Menu->Prompt) - 1);
   SPBM_SetLimits (104, 255, 0);
   SPBM_SetLimits (106, 255, 0);

   SetDlgItemText (102, Menu->Prompt);
   SPBM_SetCurrentValue (104, Menu->PromptColor);
   SPBM_SetCurrentValue (106, Menu->PromptHilight);

   return (TRUE);
}

VOID CMenuPromptDlg::OnOK (VOID)
{
   GetDlgItemText (102, Menu->Prompt, GetDlgItemTextLength (102));
   Menu->PromptColor = (UCHAR)SPBM_QueryValue (104);
   Menu->PromptHilight = (UCHAR)SPBM_QueryValue (106);

   EndDialog (TRUE);
}

VOID CMenuPromptDlg::Color (VOID)
{
   class CColorDlg *Dlg;

   if ((Dlg = new CColorDlg (m_hWnd)) != NULL) {
      Dlg->color = (UCHAR)SPBM_QueryValue (104);
      if (Dlg->DoModal () == TRUE)
         SPBM_SetCurrentValue (104, Dlg->color);
      delete Dlg;
   }
}

VOID CMenuPromptDlg::HilightColor (VOID)
{
   class CColorDlg *Dlg;

   if ((Dlg = new CColorDlg (m_hWnd)) != NULL) {
      Dlg->color = (UCHAR)SPBM_QueryValue (106);
      if (Dlg->DoModal () == TRUE)
         SPBM_SetCurrentValue (106, Dlg->color);
      delete Dlg;
   }
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CMenuListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
END_MESSAGE_MAP ()

CMenuListDlg::CMenuListDlg (HWND p_hWnd) : CDialog ("25", p_hWnd)
{
   Selected = -1;
}

USHORT CMenuListDlg::OnInitDialog (VOID)
{
   int i;
   class TMenu *Menu;

   SetWindowTitle ("Menu List");

   LVM_AllocateColumns (101, 5);
   LVM_InsertColumn (101, "Display", LVC_LEFT);
   LVM_InsertColumn (101, "Key", LVC_LEFT);
   LVM_InsertColumn (101, "Level", LVC_RIGHT);
   LVM_InsertColumn (101, "Command", LVC_LEFT);
   LVM_InsertColumn (101, "Argument", LVC_LEFT);

   if ((Menu = new TMenu) != NULL) {
      Menu->Load (File);
      if (Menu->First () == TRUE)
         do {
            LVM_InsertItem (101);

            LVM_SetItemText (101, 0, Menu->Display);
            LVM_SetItemText (101, 1, Menu->Key);
            sprintf (Temp, "%u", Menu->Level);
            LVM_SetItemText (101, 2, Temp);
            for (i = 0; MenuCmd[i].Text != NULL; i++) {
               if (MenuCmd[i].Id == Menu->Command) {
                  sprintf (Temp, "%u - %s", MenuCmd[i].Id, MenuCmd[i].Text);
                  LVM_SetItemText (101, 3, Temp);
               }
            }
            LVM_SetItemText (101, 4, Menu->Argument);
         } while (Menu->Next () == TRUE);
      delete Menu;
   }

   LVM_InvalidateView (101);

   return (TRUE);
}

VOID CMenuListDlg::OnOK (VOID)
{
   int item;
   CHAR Temp[32];

   if ((item = LVM_QuerySelectedItem (101)) != -1) {
      LVM_QueryItemText (101, item, 0, Temp);
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------
// File areas configuration
// ----------------------------------------------------------------------

class CFileSecurityDlg : public CDialog
{
public:
   CFileSecurityDlg (HWND p_hWnd);

   class  TFileData *Data;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CFileListDlg : public CDialog
{
public:
   CFileListDlg (HWND p_hWnd);

   USHORT Found;
   CHAR   Search[128];
   class  TFileData *Data;

   VOID   OnChanged (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   int    Selected;
   CHAR   Temp[128];
   DECLARE_MESSAGE_MAP ()
};

class CAddFileDlg : public CDialog
{
public:
   CAddFileDlg (HWND p_hWnd);

   CHAR   Key[16];
   CHAR   Description[128];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CFileAskStringDlg : public CDialog
{
public:
   CFileAskStringDlg (HWND p_hWnd);

   CHAR   String[128];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CFileEchoLinkDlg : public CDialog
{
public:
   CFileEchoLinkDlg (HWND p_hWnd);

   class  TFileData *Data;

   VOID   OnCancel (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   CHAR   OldSelect[64];
   class  TFilechoLink *Link;
   DECLARE_MESSAGE_MAP ()

   VOID   Add (VOID);
   VOID   Delete (VOID);
   VOID   SelChanged (VOID);
};

BEGIN_MESSAGE_MAP (CFileDlg, CDialog)
   ON_COMMAND (109, Security)
   ON_COMMAND (115, OnAdd)
   ON_COMMAND (116, OnInsert)
   ON_COMMAND (117, List)
   ON_COMMAND (118, OnPrevious)
   ON_COMMAND (119, OnNext)
   ON_COMMAND (120, OnDelete)
   ON_COMMAND (121, Links)
   ON_COMMAND (122, Search)
END_MESSAGE_MAP()

CFileDlg::CFileDlg (HWND p_hWnd) : CDialog ("10", p_hWnd)
{
   Data = NULL;
}

CFileDlg::~CFileDlg (void)
{
   if (Data != NULL)
      delete Data;
}

VOID CFileDlg::DisplayData (VOID)
{
   SetDlgItemText (102, Data->Key);
   SetDlgItemText (104, Data->Display);
   SetDlgItemText (106, Data->Upload);
   SetDlgItemText (108, Data->Download);
   BM_SetCheck (113, Data->ShowGlobal);
   BM_SetCheck (123, Data->FreeDownload);
   BM_SetCheck (114, Data->CdRom);
   SetDlgItemText (127, Data->EchoTag);
}

VOID CFileDlg::ReadData (VOID)
{
   CHAR OldArea[16];
   class TFileBase *File;

   strcpy (OldArea, Data->Key);

   GetDlgItemText (102, Data->Key, GetDlgItemTextLength (102));
   GetDlgItemText (104, Data->Display, GetDlgItemTextLength (104));
   GetDlgItemText (106, Data->Upload, GetDlgItemTextLength (106));
   GetDlgItemText (108, Data->Download, GetDlgItemTextLength (108));
   Data->ShowGlobal = (UCHAR)BM_QueryCheck (113);
   Data->FreeDownload = (UCHAR)BM_QueryCheck (123);
   Data->CdRom = (UCHAR)BM_QueryCheck (114);
   GetDlgItemText (127, Data->EchoTag, GetDlgItemTextLength (127));

   if (strcmp (OldArea, Data->Key) && OldArea[0] != '\0') {
      if ((File = new TFileBase (".\\", "")) != NULL) {
         Data->ActiveFiles = File->ChangeLibrary (OldArea, Data->Key);
         delete File;
      }
   }
}

VOID CFileDlg::OnAdd (VOID)
{
   class CAddFileDlg *Dlg;

   if ((Dlg = new CAddFileDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         Data->New ();
         strcpy (Data->Key, Dlg->Key);
         strcpy (Data->Display, Dlg->Description);
         Data->Add ();
         DisplayData ();
      }
      delete Dlg;
   }
   SetFocus (102);
}

VOID CFileDlg::OnDelete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      Data->Delete ();
      SetFocus (102);
      DisplayData ();
   }
}

VOID CFileDlg::OnInsert (VOID)
{
   class CAddFileDlg *Dlg;

   if ((Dlg = new CAddFileDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         Data->New ();
         strcpy (Data->Key, Dlg->Key);
         strcpy (Data->Display, Dlg->Description);
         Data->Insert ();
         DisplayData ();
      }
      delete Dlg;
   }
   SetFocus (102);
}

USHORT CFileDlg::OnInitDialog (VOID)
{
   Center ();
   Data = new TFileData (Cfg->SystemPath);

   EM_SetTextLimit (102, sizeof (Data->Key) - 1);
   EM_SetTextLimit (104, sizeof (Data->Display) - 1);
   EM_SetTextLimit (106, sizeof (Data->Upload) - 1);
   EM_SetTextLimit (108, sizeof (Data->Download) - 1);
   EM_SetTextLimit (127, sizeof (Data->EchoTag) - 1);

   if (Data->First () == FALSE)
      Data->New ();

   DisplayData ();

   return (TRUE);
}

VOID CFileDlg::OnPrevious (VOID)
{
   if (Data->Previous () == TRUE)
      DisplayData ();
}

VOID CFileDlg::OnNext (VOID)
{
   if (Data->Next () == TRUE)
      DisplayData ();
}

VOID CFileDlg::OnOK (VOID)
{
   ReadData ();
   Data->Update ();
}

VOID CFileDlg::Security (VOID)
{
   class CFileSecurityDlg *Dlg;

   if ((Dlg = new CFileSecurityDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CFileDlg::List (VOID)
{
   class CFileListDlg *Dlg;

   if ((Dlg = new CFileListDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      if (Dlg->DoModal () == TRUE)
         DisplayData ();
      delete Dlg;
   }
}

VOID CFileDlg::Search (VOID)
{
   class CFileListDlg *Dlg;
   class CFileAskStringDlg *Ask;

   if ((Ask = new CFileAskStringDlg (m_hWnd)) != NULL) {
      if (Ask->DoModal () == TRUE) {
         if ((Dlg = new CFileListDlg (m_hWnd)) != NULL) {
            strcpy (Dlg->Search, Ask->String);
            Dlg->Found = FALSE;
            Dlg->Data = Data;
            if (Dlg->DoModal () == TRUE)
               DisplayData ();
            delete Dlg;
         }
      }
      delete Ask;
   }
}

VOID CFileDlg::Links (VOID)
{
   class CFileEchoLinkDlg *Dlg;

   if ((Dlg = new CFileEchoLinkDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

// ----------------------------------------------------------------------

CFileSecurityDlg::CFileSecurityDlg (HWND p_hWnd) : CDialog ("16", p_hWnd)
{
}

USHORT CFileSecurityDlg::OnInitDialog (VOID)
{
   USHORT i;
   ULONG Test;

   Center ();

   SPBM_SetLimits (102, 65535U, 0L);
   SPBM_SetCurrentValue (102, Data->Level);

   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (Data->AccessFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (Data->DenyFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   SPBM_SetLimits (173, 65535U, 0L);
   SPBM_SetCurrentValue (173, Data->DownloadLevel);

   for (i = 174, Test = 0x80000000L; i <= 205; i++, Test >>= 1) {
      if (Data->DownloadFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 207, Test = 0x80000000L; i <= 238; i++, Test >>= 1) {
      if (Data->DownloadDenyFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   SPBM_SetLimits (242, 65535U, 0L);
   SPBM_SetCurrentValue (242, Data->UploadLevel);

   for (i = 243, Test = 0x80000000L; i <= 274; i++, Test >>= 1) {
      if (Data->UploadFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 276, Test = 0x80000000L; i <= 307; i++, Test >>= 1) {
      if (Data->UploadDenyFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   return (TRUE);
}

VOID CFileSecurityDlg::OnOK (VOID)
{
   USHORT i;
   ULONG Test;

   Data->Level = SPBM_QueryValue (102);

   Data->AccessFlags = 0L;
   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->AccessFlags |= Test;
   }
   Data->DenyFlags = 0L;
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->DenyFlags |= Test;
   }

   Data->DownloadLevel = SPBM_QueryValue (173);

   Data->DownloadFlags = 0L;
   for (i = 174, Test = 0x80000000L; i <= 205; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->DownloadFlags |= Test;
   }
   Data->DownloadDenyFlags = 0L;
   for (i = 207, Test = 0x80000000L; i <= 238; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->DownloadDenyFlags |= Test;
   }

   Data->UploadLevel = SPBM_QueryValue (242);

   Data->UploadFlags = 0L;
   for (i = 243, Test = 0x80000000L; i <= 274; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->UploadFlags |= Test;
   }
   Data->UploadDenyFlags = 0L;
   for (i = 276, Test = 0x80000000L; i <= 307; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->UploadDenyFlags |= Test;
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CFileListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
END_MESSAGE_MAP ()

CFileListDlg::CFileListDlg (HWND p_hWnd) : CDialog ("25", p_hWnd)
{
   Search[0] = '\0';
   Found = TRUE;
   Selected = -1;
}

USHORT CFileListDlg::OnInitDialog (VOID)
{
   class TFileData *File;

   SetWindowTitle ("File Areas List");

   LVM_AllocateColumns (101, 5);
   LVM_InsertColumn (101, "Key", LVC_LEFT);
   LVM_InsertColumn (101, "Level", LVC_RIGHT);
   LVM_InsertColumn (101, "Download", LVC_RIGHT);
   LVM_InsertColumn (101, "Upload", LVC_RIGHT);
   LVM_InsertColumn (101, "Description", LVC_LEFT);

   strupr (Search);

   if ((File = new TFileData (Cfg->SystemPath)) != NULL) {
      if (File->First () == TRUE)
         do {
            if (Search[0] != '\0') {
               strcpy (Temp, File->Key);
               if (strstr (strupr (Temp), Search) == NULL) {
                  strcpy (Temp, File->Display);
                  if (strstr (strupr (Temp), Search) == NULL)
                     continue;
               }
               Found = TRUE;
            }

            LVM_InsertItem (101);

            LVM_SetItemText (101, 0, File->Key);
            sprintf (Temp, "%u", File->Level);
            LVM_SetItemText (101, 1, Temp);
            sprintf (Temp, "%u", File->DownloadLevel);
            LVM_SetItemText (101, 2, Temp);
            sprintf (Temp, "%u", File->UploadLevel);
            LVM_SetItemText (101, 3, Temp);
            LVM_SetItemText (101, 4, File->Display);
         } while (File->Next () == TRUE);
      delete File;
   }

   LVM_InvalidateView (101);

   if (Found == FALSE) {
      MessageBox ("There are no areas that match the search parameters.", "File area search", MB_OK);
      EndDialog (FALSE);
   }

   return (TRUE);
}

VOID CFileListDlg::OnOK (VOID)
{
   int item;
   CHAR Temp[32];

   if ((item = LVM_QuerySelectedItem (101)) != -1) {
      LVM_QueryItemText (101, item, 0, Temp);
      Data->Read (Temp, FALSE);
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CFileAskStringDlg::CFileAskStringDlg (HWND p_hWnd) : CDialog ("26", p_hWnd)
{
}

USHORT CFileAskStringDlg::OnInitDialog (VOID)
{
   Center ();

   SetWindowTitle ("Search File Area");
   EM_SetTextLimit (128, sizeof (String) - 1);

   return (TRUE);
}

VOID CFileAskStringDlg::OnOK (VOID)
{
   GetDlgItemText (128, GetDlgItemTextLength (128), String);
   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CAddFileDlg::CAddFileDlg (HWND p_hWnd) : CDialog ("35", p_hWnd)
{
}

USHORT CAddFileDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Key) - 1);
   EM_SetTextLimit (104, sizeof (Description) - 1);

   return (TRUE);
}

VOID CAddFileDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Key);
   GetDlgItemText (104, GetDlgItemTextLength (104), Description);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CFileEchoLinkDlg, CDialog)
   ON_COMMAND (102, Add)
   ON_COMMAND (103, Delete)
#if defined(__OS2__)
   ON_CONTROL (LN_SELECT, 101, SelChanged)
#elif defined(__NT__)
   ON_NOTIFY (LBN_SELCHANGE, 101, SelChanged)
#endif
END_MESSAGE_MAP ()

CFileEchoLinkDlg::CFileEchoLinkDlg (HWND p_hWnd) : CDialog ("37", p_hWnd)
{
   OldSelect[0] = '\0';
}

USHORT CFileEchoLinkDlg::OnInitDialog (VOID)
{
   Center ();

   SetDlgItemText (128, "");
   if ((Link = new TFilechoLink (Cfg->SystemPath)) != NULL) {
      Link->Load (Data->EchoTag);
      if (Link->First () == TRUE)
         do {
            LM_AddString (101, Link->Address);
         } while (Link->Next () == TRUE);
   }

   return (TRUE);
}

VOID CFileEchoLinkDlg::SelChanged (VOID)
{
   USHORT item;
   CHAR Temp[64];

   if (Link->Check (OldSelect) == TRUE) {
      Link->ReceiveOnly = (UCHAR)BM_QueryCheck (104);
      Link->SendOnly = (UCHAR)BM_QueryCheck (105);
      Link->PersonalOnly = (UCHAR)BM_QueryCheck (106);
      Link->Passive = (UCHAR)BM_QueryCheck (107);
      Link->Update ();
   }

   if ((item = LM_QuerySelection (101)) >= 0) {
      LM_QueryItemText (101, item, sizeof (Temp), Temp);
      if (Link->Check (Temp) == TRUE) {
         BM_SetCheck (104, Link->ReceiveOnly);
         BM_SetCheck (105, Link->SendOnly);
         BM_SetCheck (106, Link->PersonalOnly);
         BM_SetCheck (107, Link->Passive);
      }
      strcpy (OldSelect, Temp);
   }
}

VOID CFileEchoLinkDlg::OnOK (VOID)
{
   if (Link != NULL) {
      Link->Save ();
      delete Link;
   }

   EndDialog (TRUE);
}

VOID CFileEchoLinkDlg::OnCancel (VOID)
{
   if (Link != NULL)
      delete Link;

   EndDialog (FALSE);
}

VOID CFileEchoLinkDlg::Add (VOID)
{
   CHAR Temp[128];

   GetDlgItemText (128, GetDlgItemTextLength (128), Temp);
   if (Temp[0] != '\0') {
      LM_AddString (101, Temp);
      Link->AddString (Temp);
   }
   SetDlgItemText (128, "");
   SetFocus (128);
}

VOID CFileEchoLinkDlg::Delete (VOID)
{
   USHORT item;

   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO) == IDYES) {
      item = LM_QuerySelection (101);
      LM_DeleteItem (101, item);
   }
}
#endif

// ----------------------------------------------------------------------
// Message areas configuration
// ----------------------------------------------------------------------

class CMessageSecurityDlg : public CDialog
{
public:
   CMessageSecurityDlg (HWND p_hWnd);

   class  TMsgData *Data;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CMessageListDlg : public CDialog
{
public:
   CMessageListDlg (HWND p_hWnd);

   USHORT Found;
   CHAR   Search[128];
   class  TMsgData *Data;

   VOID   OnChanged (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   int    Selected;
   CHAR   Temp[128];
   DECLARE_MESSAGE_MAP ()
};

class CAddMessageDlg : public CDialog
{
public:
   CAddMessageDlg (HWND p_hWnd);

   CHAR   Key[16];
   CHAR   Description[128];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CAskStringDlg : public CDialog
{
public:
   CAskStringDlg (HWND p_hWnd);

   CHAR   String[128];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CEchoLinkDlg : public CDialog
{
public:
   CEchoLinkDlg (HWND p_hWnd);

   class  TMsgData *Data;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
   VOID   OnCancel (VOID);

private:
   CHAR   OldSelect[64];
   class  TEchoLink *Link;
   DECLARE_MESSAGE_MAP ()

   VOID   Add (VOID);
   VOID   Delete (VOID);
   VOID   SelChanged (VOID);
};

BEGIN_MESSAGE_MAP (CMessageDlg, CDialog)
   ON_COMMAND (115, OnAdd)
   ON_COMMAND (116, OnInsert)
   ON_COMMAND (117, List)
   ON_COMMAND (118, OnPrevious)
   ON_COMMAND (119, OnNext)
   ON_COMMAND (120, OnDelete)
   ON_COMMAND (122, Search)
   ON_COMMAND (121, Links)
   ON_COMMAND (123, Security)
END_MESSAGE_MAP()

CMessageDlg::CMessageDlg (HWND p_hWnd) : CDialog ("9", p_hWnd)
{
   Data = NULL;
}

CMessageDlg::~CMessageDlg (void)
{
   if (Data != NULL)
      delete Data;
}

USHORT CMessageDlg::OnInitDialog (VOID)
{
   Center ();
   Data = new TMsgData (Cfg->SystemPath);

   EM_SetTextLimit (102, sizeof (Data->Key) - 1);
   EM_SetTextLimit (104, sizeof (Data->Display) - 1);
   EM_SetTextLimit (106, sizeof (Data->Path) - 1);
   EM_SetTextLimit (126, sizeof (Data->EchoTag) - 1);
   EM_SetTextLimit (128, sizeof (Data->NewsGroup) - 1);

   CB_AddString (107, "Squish<tm>");
   CB_AddString (107, "JAM");
   CB_AddString (107, "Fido (*.msg)");
   CB_AddString (107, "AdeptXBBS");
   CB_AddString (107, "USENET Newsgroup");
   CB_AddString (107, "Passthrough");

   SPBM_SetLimits (138, 5000, 0);
   SPBM_SetLimits (136, 366U, 0L);

   if (Cfg->MailAddress.First () == TRUE)
      do {
         CB_AddString (110, Cfg->MailAddress.String);
      } while (Cfg->MailAddress.Next () == TRUE);

   if (Data->First () == FALSE)
      Data->New ();

   DisplayData ();

   return (TRUE);
}

VOID CMessageDlg::DisplayData (VOID)
{
   SetDlgItemText (102, Data->Key);
   SetDlgItemText (104, Data->Display);
   SetDlgItemText (106, Data->Path);
   switch (Data->Storage) {
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
      case ST_USENET:
         CB_SelectItem (107, 4);
         break;
      case ST_PASSTHROUGH:
         CB_SelectItem (107, 5);
         break;
   }
   BM_SetCheck (113, Data->ShowGlobal);
   BM_SetCheck (114, Data->Offline);
   BM_SetCheck (129, Data->EchoMail);
   BM_SetCheck (130, Data->UpdateNews);
   SetDlgItemText (126, Data->EchoTag);
   SetDlgItemText (128, Data->NewsGroup);
   SPBM_SetCurrentValue (138, Data->MaxMessages);
   SPBM_SetCurrentValue (136, Data->DaysOld);
   SetDlgItemText (110, Data->Address);
}

VOID CMessageDlg::ReadData (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Data->Key);
   GetDlgItemText (104, GetDlgItemTextLength (104), Data->Display);
   GetDlgItemText (106, GetDlgItemTextLength (106), Data->Path);
   switch (CB_QuerySelection (107)) {
      case 0:
         Data->Storage = ST_SQUISH;
         break;
      case 1:
         Data->Storage = ST_JAM;
         break;
      case 2:
         Data->Storage = ST_FIDO;
         break;
      case 3:
         Data->Storage = ST_ADEPT;
         break;
      case 4:
         Data->Storage = ST_USENET;
         break;
      case 5:
         Data->Storage = ST_PASSTHROUGH;
         break;
   }
   Data->ShowGlobal = (CHAR)BM_QueryCheck (113);
   Data->Offline = (CHAR)BM_QueryCheck (114);
   Data->EchoMail = (CHAR)BM_QueryCheck (129);
   Data->UpdateNews = (CHAR)BM_QueryCheck (130);
   GetDlgItemText (126, GetDlgItemTextLength (126), Data->EchoTag);
   GetDlgItemText (128, GetDlgItemTextLength (128), Data->NewsGroup);
   Data->MaxMessages = SPBM_QueryValue (138);
   Data->DaysOld = SPBM_QueryValue (136);
   GetDlgItemText (110, GetDlgItemTextLength (110), Data->Address);
}

VOID CMessageDlg::OnAdd (VOID)
{
   class CAddMessageDlg *Dlg;

   if ((Dlg = new CAddMessageDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         Data->New ();
         strcpy (Data->Key, Dlg->Key);
         strcpy (Data->Display, Dlg->Description);
         Data->Storage = Cfg->NewAreasStorage;
         strcpy (Data->Path, Cfg->NewAreasPath);
         strcat (Data->Path, Data->Key);
         Data->Level = Cfg->NewAreasLevel;
         Data->AccessFlags = Cfg->NewAreasFlags;
         Data->DenyFlags = Cfg->NewAreasDenyFlags;
         Data->WriteLevel = Cfg->NewAreasWriteLevel;
         Data->WriteFlags = Cfg->NewAreasWriteFlags;
         Data->DenyWriteFlags = Cfg->NewAreasDenyWriteFlags;
         Data->MaxMessages = 200;
         Data->DaysOld = 14;
         Data->Add ();
         DisplayData ();
      }
      delete Dlg;
   }
   SetFocus (102);
}

VOID CMessageDlg::OnDelete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      Data->Delete ();
      SetFocus (102);
      DisplayData ();
   }
}

VOID CMessageDlg::OnInsert (VOID)
{
   class CAddMessageDlg *Dlg;

   if ((Dlg = new CAddMessageDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         Data->New ();
         strcpy (Data->Key, Dlg->Key);
         strcpy (Data->Display, Dlg->Description);
         Data->Storage = Cfg->NewAreasStorage;
         strcpy (Data->Path, Cfg->NewAreasPath);
         strcat (Data->Path, Data->Key);
         Data->Level = Cfg->NewAreasLevel;
         Data->AccessFlags = Cfg->NewAreasFlags;
         Data->DenyFlags = Cfg->NewAreasDenyFlags;
         Data->WriteLevel = Cfg->NewAreasWriteLevel;
         Data->WriteFlags = Cfg->NewAreasWriteFlags;
         Data->DenyWriteFlags = Cfg->NewAreasDenyWriteFlags;
         Data->MaxMessages = 200;
         Data->DaysOld = 14;
         Data->Insert ();
         DisplayData ();
      }
      delete Dlg;
   }
   SetFocus (102);
}

VOID CMessageDlg::OnPrevious (VOID)
{
   if (Data->Previous () == TRUE)
      DisplayData ();
}

VOID CMessageDlg::OnNext (VOID)
{
   if (Data->Next () == TRUE)
      DisplayData ();
}

VOID CMessageDlg::OnOK (VOID)
{
   ReadData ();
   Data->Update ();
}

VOID CMessageDlg::Security (VOID)
{
   class CMessageSecurityDlg *Dlg;

   if ((Dlg = new CMessageSecurityDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CMessageDlg::List (VOID)
{
   class CMessageListDlg *Dlg;

   if ((Dlg = new CMessageListDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      if (Dlg->DoModal () == TRUE)
         DisplayData ();
      delete Dlg;
   }
}

VOID CMessageDlg::Links (VOID)
{
   class CEchoLinkDlg *Dlg;

   if ((Dlg = new CEchoLinkDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CMessageDlg::Search (VOID)
{
   class CMessageListDlg *Dlg;
   class CAskStringDlg *Ask;

   if ((Ask = new CAskStringDlg (m_hWnd)) != NULL) {
      if (Ask->DoModal () == TRUE) {
         if ((Dlg = new CMessageListDlg (m_hWnd)) != NULL) {
            strcpy (Dlg->Search, Ask->String);
            Dlg->Found = FALSE;
            Dlg->Data = Data;
            if (Dlg->DoModal () == TRUE)
               DisplayData ();
            delete Dlg;
         }
      }
      delete Ask;
   }
}

// ----------------------------------------------------------------------

CMessageSecurityDlg::CMessageSecurityDlg (HWND p_hWnd) : CDialog ("15", p_hWnd)
{
}

USHORT CMessageSecurityDlg::OnInitDialog (VOID)
{
   USHORT i;
   ULONG Test;

   Center ();

   SPBM_SetLimits (102, 65535U, 0L);
   SPBM_SetCurrentValue (102, Data->Level);

   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (Data->AccessFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (Data->DenyFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   SPBM_SetLimits (173, 65535U, 0L);
   SPBM_SetCurrentValue (173, Data->WriteLevel);

   for (i = 174, Test = 0x80000000L; i <= 205; i++, Test >>= 1) {
      if (Data->WriteFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 207, Test = 0x80000000L; i <= 238; i++, Test >>= 1) {
      if (Data->DenyWriteFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   return (TRUE);
}

VOID CMessageSecurityDlg::OnOK (VOID)
{
   USHORT i;
   ULONG Test;

   Data->Level = SPBM_QueryValue (102);

   Data->AccessFlags = 0L;
   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->AccessFlags |= Test;
   }
   Data->DenyFlags = 0L;
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->DenyFlags |= Test;
   }

   Data->WriteLevel = SPBM_QueryValue (173);

   Data->WriteFlags = 0L;
   for (i = 174, Test = 0x80000000L; i <= 205; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->WriteFlags |= Test;
   }
   Data->DenyWriteFlags = 0L;
   for (i = 207, Test = 0x80000000L; i <= 238; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->DenyWriteFlags |= Test;
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CMessageListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
END_MESSAGE_MAP ()

CMessageListDlg::CMessageListDlg (HWND p_hWnd) : CDialog ("25", p_hWnd)
{
   Search[0] = '\0';
   Found = TRUE;
   Selected = -1;
}

USHORT CMessageListDlg::OnInitDialog (VOID)
{
   class TMsgData *Msg;

   SetWindowTitle ("Message Areas List");

   LVM_AllocateColumns (101, 4);
   LVM_InsertColumn (101, "Key", LVC_LEFT);
   LVM_InsertColumn (101, "Level", LVC_RIGHT);
   LVM_InsertColumn (101, "Write", LVC_RIGHT);
   LVM_InsertColumn (101, "Description", LVC_LEFT);

   strupr (Search);

   if ((Msg = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Msg->First () == TRUE)
         do {
            if (Search[0] != '\0') {
               strcpy (Temp, Msg->Key);
               if (strstr (strupr (Temp), Search) == NULL) {
                  strcpy (Temp, Msg->Display);
                  if (strstr (strupr (Temp), Search) == NULL) {
                     strcpy (Temp, Msg->EchoTag);
                     if (strstr (strupr (Temp), Search) == NULL)
                        continue;
                  }
               }
               Found = TRUE;
            }

            LVM_InsertItem (101);

            LVM_SetItemText (101, 0, Msg->Key);
            sprintf (Temp, "%u", Msg->Level);
            LVM_SetItemText (101, 1, Temp);
            sprintf (Temp, "%u", Msg->WriteLevel);
            LVM_SetItemText (101, 2, Temp);
            LVM_SetItemText (101, 3, Msg->Display);
         } while (Msg->Next () == TRUE);
      delete Msg;
   }

   LVM_InvalidateView (101);

   if (Found == FALSE) {
      MessageBox ("There are no areas that match the search parameters.", "Message area search", MB_OK);
      EndDialog (FALSE);
   }

   return (TRUE);
}

VOID CMessageListDlg::OnOK (VOID)
{
   int item;
   CHAR Temp[32];

   if ((item = LVM_QuerySelectedItem (101)) != -1) {
      LVM_QueryItemText (101, item, 0, Temp);
      Data->Read (Temp, FALSE);
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CAskStringDlg::CAskStringDlg (HWND p_hWnd) : CDialog ("26", p_hWnd)
{
}

USHORT CAskStringDlg::OnInitDialog (VOID)
{
   Center ();

   SetWindowTitle ("Search Message Area");
   EM_SetTextLimit (128, sizeof (String) - 1);

   return (TRUE);
}

VOID CAskStringDlg::OnOK (VOID)
{
   GetDlgItemText (128, GetDlgItemTextLength (128), String);
   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CEchoLinkDlg, CDialog)
   ON_COMMAND (102, Add)
   ON_COMMAND (103, Delete)
#if defined(__OS2__)
   ON_CONTROL (LN_SELECT, 101, SelChanged)
#elif defined(__NT__)
   ON_NOTIFY (LBN_SELCHANGE, 101, SelChanged)
#endif
END_MESSAGE_MAP ()

CEchoLinkDlg::CEchoLinkDlg (HWND p_hWnd) : CDialog ("24", p_hWnd)
{
   OldSelect[0] = '\0';
}

USHORT CEchoLinkDlg::OnInitDialog (VOID)
{
   Center ();

   SetDlgItemText (128, "");
   if ((Link = new TEchoLink (Cfg->SystemPath)) != NULL) {
      Link->Load (Data->EchoTag);
      if (Link->First () == TRUE) {
         BM_SetCheck (104, Link->ReceiveOnly);
         BM_SetCheck (105, Link->SendOnly);
         BM_SetCheck (106, Link->PersonalOnly);
         BM_SetCheck (107, Link->Passive);
         strcpy (OldSelect, Link->Address);

         do {
            LM_AddString (101, Link->Address);
         } while (Link->Next () == TRUE);

         LM_SelectItem (101, 0);
      }
   }

   return (TRUE);
}

VOID CEchoLinkDlg::OnOK (VOID)
{
   if (Link->Check (OldSelect) == TRUE) {
      Link->ReceiveOnly = (UCHAR)BM_QueryCheck (104);
      Link->SendOnly = (UCHAR)BM_QueryCheck (105);
      Link->PersonalOnly = (UCHAR)BM_QueryCheck (106);
      Link->Passive = (UCHAR)BM_QueryCheck (107);
      Link->Update ();
   }

   if (Link != NULL) {
      Link->Save ();
      delete Link;
   }

   EndDialog (TRUE);
}

VOID CEchoLinkDlg::OnCancel (VOID)
{
   if (Link != NULL)
      delete Link;

   EndDialog (FALSE);
}

VOID CEchoLinkDlg::Add (VOID)
{
   CHAR Temp[128];

   GetDlgItemText (128, GetDlgItemTextLength (128), Temp);
   if (Temp[0] != '\0') {
      LM_AddString (101, Temp);
      Link->AddString (Temp);
   }
   SetDlgItemText (128, "");
   SetFocus (128);
}

VOID CEchoLinkDlg::Delete (VOID)
{
   USHORT item;
   CHAR Temp[64];

   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO) == IDYES) {
      if ((item = LM_QuerySelection (101)) >= 0) {
         LM_QueryItemText (101, item, sizeof (Temp), Temp);
         if (Link->Check (Temp) == TRUE)
            Link->Delete ();
         LM_DeleteItem (101, item);

         if (LM_QueryItemCount (101) > 0) {
            Link->First ();
            LM_SelectItem (101, 0);
            BM_SetCheck (104, Link->ReceiveOnly);
            BM_SetCheck (105, Link->SendOnly);
            BM_SetCheck (106, Link->PersonalOnly);
            BM_SetCheck (107, Link->Passive);
            strcpy (OldSelect, Link->Address);
         }
         else {
            BM_SetCheck (104, FALSE);
            BM_SetCheck (105, FALSE);
            BM_SetCheck (106, FALSE);
            BM_SetCheck (107, FALSE);
            OldSelect[0] = '\0';
         }
      }
   }
   SetFocus (128);
}

VOID CEchoLinkDlg::SelChanged (VOID)
{
   USHORT item;
   CHAR Temp[64];

   if (Link->Check (OldSelect) == TRUE) {
      Link->ReceiveOnly = (UCHAR)BM_QueryCheck (104);
      Link->SendOnly = (UCHAR)BM_QueryCheck (105);
      Link->PersonalOnly = (UCHAR)BM_QueryCheck (106);
      Link->Passive = (UCHAR)BM_QueryCheck (107);
      Link->Update ();
   }

   if ((item = LM_QuerySelection (101)) >= 0) {
      LM_QueryItemText (101, item, sizeof (Temp), Temp);
      if (Link->Check (Temp) == TRUE) {
         BM_SetCheck (104, Link->ReceiveOnly);
         BM_SetCheck (105, Link->SendOnly);
         BM_SetCheck (106, Link->PersonalOnly);
         BM_SetCheck (107, Link->Passive);
      }
      strcpy (OldSelect, Temp);
   }
}

// ----------------------------------------------------------------------

CAddMessageDlg::CAddMessageDlg (HWND p_hWnd) : CDialog ("35", p_hWnd)
{
}

USHORT CAddMessageDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Key) - 1);
   EM_SetTextLimit (104, sizeof (Description) - 1);

   return (TRUE);
}

VOID CAddMessageDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Key);
   GetDlgItemText (104, GetDlgItemTextLength (104), Description);

   EndDialog (TRUE);
}

#if !defined(__POINT__)
// ----------------------------------------------------------------------
// User limits
// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CLimitsDlg, CDialog)
   ON_CONTROL (LN_SELECT, 101, SelectItem)
   ON_COMMAND (112, Delete)
   ON_COMMAND (113, Add)
END_MESSAGE_MAP ()

CLimitsDlg::CLimitsDlg (HWND p_hWnd) : CDialog ("4", p_hWnd)
{
}

USHORT CLimitsDlg::OnInitDialog (VOID)
{
   class TLimits *Limits;

   Center ();

   EM_SetTextLimit (110, sizeof (Limits->Key) - 1);
   EM_SetTextLimit (103, sizeof (Limits->Description) - 1);

   SPBM_SetLimits (105, 65535U, 0L);
   SPBM_SetLimits (107, 65535U, 0L);
   SPBM_SetLimits (111, 65535U, 0L);

   if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
      if (Limits->First () == TRUE)
         do {
            LM_AddString (101, Limits->Key);
         } while (Limits->Next () == TRUE);
      delete Limits;
   }

   return (TRUE);
}

VOID CLimitsDlg::OnOK (VOID)
{
   USHORT Item;
   CHAR Temp[64];
   class TLimits *Limits;

   Item = LM_QuerySelection (101);
   LM_QueryItemText (101, Item, sizeof (Temp), Temp);

   if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
      if (Limits->Read (Temp) == TRUE) {
         GetDlgItemText (110, GetDlgItemTextLength (110), Limits->Key);
         GetDlgItemText (103, GetDlgItemTextLength (103), Limits->Description);
         Limits->CallTimeLimit = SPBM_QueryValue (105);
         Limits->DayTimeLimit = SPBM_QueryValue (107);
         Limits->DayDownloadLimit = SPBM_QueryValue (111);

         Limits->Update ();

         LM_DeleteAll (101);
         if (Limits->First () == TRUE)
            do {
               LM_AddString (101, Limits->Key);
            } while (Limits->Next () == TRUE);
      }
      delete Limits;
   }
}

VOID CLimitsDlg::Add (VOID)
{
   class TLimits *Limits;

   if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
      Limits->New ();

      GetDlgItemText (110, GetDlgItemTextLength (110), Limits->Key);
      GetDlgItemText (103, GetDlgItemTextLength (103), Limits->Description);
      Limits->CallTimeLimit = SPBM_QueryValue (105);
      Limits->DayTimeLimit = SPBM_QueryValue (107);
      Limits->DayDownloadLimit = SPBM_QueryValue (111);

      Limits->Add ();

      LM_DeleteAll (101);
      if (Limits->First () == TRUE)
         do {
            LM_AddString (101, Limits->Key);
         } while (Limits->Next () == TRUE);

      delete Limits;
   }
}

VOID CLimitsDlg::Delete (VOID)
{
   USHORT Item;
   CHAR Temp[64];
   class TLimits *Limits;

   Item = LM_QuerySelection (101);
   LM_QueryItemText (101, Item, sizeof (Temp), Temp);

   if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
      if (Limits->Read (Temp) == TRUE) {
         Limits->Delete ();
         Limits->New ();

         SetDlgItemText (110, Limits->Key);
         SetDlgItemText (103, Limits->Description);
         SPBM_SetCurrentValue (105, Limits->CallTimeLimit);
         SPBM_SetCurrentValue (107, Limits->DayTimeLimit);
         SPBM_SetCurrentValue (111, (USHORT)Limits->DayDownloadLimit);

         LM_DeleteAll (101);
         if (Limits->First () == TRUE)
            do {
               LM_AddString (101, Limits->Key);
            } while (Limits->Next () == TRUE);
      }
      delete Limits;
   }
}

VOID CLimitsDlg::SelectItem (VOID)
{
   USHORT Item;
   CHAR Temp[64];
   class TLimits *Limits;

   Item = LM_QuerySelection (101);
   LM_QueryItemText (101, Item, sizeof (Temp), Temp);

   if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
      if (Limits->Read (Temp) == TRUE) {
         SetDlgItemText (110, Limits->Key);
         SetDlgItemText (103, Limits->Description);
         SPBM_SetCurrentValue (105, Limits->CallTimeLimit);
         SPBM_SetCurrentValue (107, Limits->DayTimeLimit);
         SPBM_SetCurrentValue (111, (USHORT)Limits->DayDownloadLimit);
      }
      delete Limits;
   }
}

// ----------------------------------------------------------------------
// General BBS configuration
// ----------------------------------------------------------------------

CBBSGeneralDlg::CBBSGeneralDlg (HWND p_hWnd) : CDialog ("22", p_hWnd)
{
}

USHORT CBBSGeneralDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (104, sizeof (Cfg->UserFile) - 1);
   EM_SetTextLimit (116, sizeof (Cfg->MenuPath) - 1);
   EM_SetTextLimit (112, sizeof (Cfg->TextFiles) - 1);
   EM_SetTextLimit (106, sizeof (Cfg->MailPath) - 1);
   EM_SetTextLimit (102, sizeof (Cfg->MainMenu) - 1);
   EM_SetTextLimit (113, sizeof (Cfg->UsersHomePath) - 1);

   CB_AddString (107, "Squish<tm>");
   CB_AddString (107, "JAM");
   CB_AddString (107, "Fido (*.msg)");
   CB_AddString (107, "AdeptXBBS");

   SetDlgItemText (104, Cfg->UserFile);
   SetDlgItemText (116, Cfg->MenuPath);
   SetDlgItemText (112, Cfg->TextFiles);
   SetDlgItemText (113, Cfg->UsersHomePath);

   switch (Cfg->MailStorage) {
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

   SetDlgItemText (106, Cfg->MailPath);
   SetDlgItemText (102, Cfg->MainMenu);
   BM_SetCheck (120 + Cfg->Ansi, TRUE);
   BM_SetCheck (118 + Cfg->IEMSI, TRUE);

   return (TRUE);
}

VOID CBBSGeneralDlg::OnOK (VOID)
{
   GetDlgItemText (104, GetDlgItemTextLength (104), Cfg->UserFile);
   GetDlgItemText (116, GetDlgItemTextLength (116), Cfg->MenuPath);
   GetDlgItemText (112, GetDlgItemTextLength (112), Cfg->TextFiles);
   GetDlgItemText (113, GetDlgItemTextLength (113), Cfg->UsersHomePath);

   switch (CB_QuerySelection (107)) {
      case 0:
         Cfg->MailStorage = ST_SQUISH;
         break;
      case 1:
         Cfg->MailStorage = ST_JAM;
         break;
      case 2:
         Cfg->MailStorage = ST_FIDO;
         break;
      case 3:
         Cfg->MailStorage = ST_ADEPT;
         break;
   }

   GetDlgItemText (106, GetDlgItemTextLength (106), Cfg->MailPath);
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->MainMenu);
   if (BM_QueryCheck (120) == TRUE)
      Cfg->Ansi = NO;
   else if (BM_QueryCheck (121) == TRUE)
      Cfg->Ansi = YES;
   else if (BM_QueryCheck (122) == TRUE)
      Cfg->Ansi = AUTO;
   if (BM_QueryCheck (118) == TRUE)
      Cfg->IEMSI = NO;
   else if (BM_QueryCheck (119) == TRUE)
      Cfg->IEMSI = YES;

   EndDialog (TRUE);
}
#endif

// ----------------------------------------------------------------------
// User editor
// ----------------------------------------------------------------------

class CUserListDlg : public CDialog
{
public:
   CUserListDlg (HWND p_hWnd);

   USHORT Found;
   CHAR   Search[128];
   class  TUser *Data;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
   VOID   OnUser (VOID);

private:
   int i, toSelect;
   class TUser *User;

   DECLARE_MESSAGE_MAP ()
};

class CUserSecurityDlg : public CDialog
{
public:
   CUserSecurityDlg (HWND p_hWnd);

   class  TUser *Data;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CUserAskStringDlg : public CDialog
{
public:
   CUserAskStringDlg (HWND p_hWnd);

   CHAR   String[128];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CAddUserDlg : public CDialog
{
public:
   CAddUserDlg (HWND p_hWnd);

   CHAR   Name[48];
   CHAR   Password[16];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CPasswordDlg : public CDialog
{
public:
   CPasswordDlg (HWND p_hWnd);

   CHAR   Password[16];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

class CUserOtherDlg : public CDialog
{
public:
   CUserOtherDlg (HWND p_hWnd);

   class  TUser *User;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

BEGIN_MESSAGE_MAP (CUserDlg, CDialog)
   ON_COMMAND (113, Security)
   ON_COMMAND (115, Add)
   ON_COMMAND (116, Delete)
   ON_COMMAND (117, List)
   ON_COMMAND (118, Previous)
   ON_COMMAND (119, Next)
   ON_COMMAND (135, Password)
   ON_COMMAND (136, Search)
   ON_COMMAND (139, Other)
END_MESSAGE_MAP ()

CUserDlg::CUserDlg (HWND p_hWnd) : CDialog ("17", p_hWnd)
{
   Data = NULL;
}

CUserDlg::~CUserDlg (void)
{
   if (Data != NULL)
      delete Data;
}

USHORT CUserDlg::OnInitDialog (VOID)
{
#if !defined(__POINT__)
   class TLimits *Limits;
#endif

   Center ();

   Data = new TUser (Cfg->UserFile);

   EM_SetTextLimit (102, sizeof (Data->Name) - 1);
   EM_SetTextLimit (104, sizeof (Data->RealName) - 1);
   EM_SetTextLimit (108, sizeof (Data->Company) - 1);
   EM_SetTextLimit (106, sizeof (Data->Address) - 1);
   EM_SetTextLimit (110, sizeof (Data->City) - 1);
   EM_SetTextLimit (112, sizeof (Data->DayPhone) - 1);
   EM_SetTextLimit (132, sizeof (Data->MailBox) - 1);
   EM_SetTextLimit (134, sizeof (Data->Language) - 1);
   EM_SetTextLimit (124, 1);

#if !defined(__POINT__)
   if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
      if (Limits->First () == TRUE)
         do {
            CB_AddString (130, Limits->Key);
         } while (Limits->Next () == TRUE);
      delete Limits;
   }
#endif

   if (Data->First () == FALSE)
      Data->Clear ();

   DisplayData ();

   return (TRUE);
}

VOID CUserDlg::OnOK (VOID)
{
   ReadData ();
   Data->Update ();
}

VOID CUserDlg::DisplayData (VOID)
{
   CHAR Temp[64];

   SetDlgItemText (102, Data->Name);
   SetDlgItemText (104, Data->RealName);
   SetDlgItemText (108, Data->Company);
   SetDlgItemText (106, Data->Address);
   SetDlgItemText (110, Data->City);
   SetDlgItemText (112, Data->DayPhone);
   SetDlgItemText (132, Data->MailBox);
   if (Data->Sex == 0)
      strcpy (Temp, "M");
   else
      strcpy (Temp, "F");
   SetDlgItemText (124, Temp);
   sprintf (Temp, "%u", Data->ScreenWidth);
   SetDlgItemText (126, Temp);
   sprintf (Temp, "%u", Data->ScreenHeight);
   SetDlgItemText (128, Temp);
   sprintf (Temp, "%lu", Data->TotalCalls);
   SetDlgItemText (138, Temp);
   BM_SetCheck (114, Data->Ansi);
   BM_SetCheck (120, Data->Avatar);
   BM_SetCheck (121, Data->Color);
   BM_SetCheck (122, Data->HotKey);
   SetDlgItemText (130, Data->LimitClass);
}

VOID CUserDlg::ReadData (VOID)
{
   CHAR Temp[64];

   GetDlgItemText (102, GetDlgItemTextLength (102), Data->Name);
   GetDlgItemText (104, GetDlgItemTextLength (104), Data->RealName);
   GetDlgItemText (108, GetDlgItemTextLength (108), Data->Company);
   GetDlgItemText (106, GetDlgItemTextLength (106), Data->Address);
   GetDlgItemText (110, GetDlgItemTextLength (110), Data->City);
   GetDlgItemText (112, GetDlgItemTextLength (112), Data->DayPhone);
   GetDlgItemText (132, GetDlgItemTextLength (132), Data->MailBox);
   GetDlgItemText (124, GetDlgItemTextLength (124), Temp);
   if ((CHAR)toupper (Temp[0]) == 'F')
      Data->Sex = 1;
   else
      Data->Sex = 0;
   GetDlgItemText (126, GetDlgItemTextLength (126), Temp);
   Data->ScreenWidth = (USHORT)atoi (Temp);
   GetDlgItemText (128, GetDlgItemTextLength (128), Temp);
   Data->ScreenHeight = (USHORT)atoi (Temp);
   GetDlgItemText (138, GetDlgItemTextLength (138), Temp);
   Data->TotalCalls = atol (Temp);
   Data->Ansi = (CHAR)BM_QueryCheck (114);
   Data->Avatar = (CHAR)BM_QueryCheck (120);
   Data->Color = (CHAR)BM_QueryCheck (121);
   Data->HotKey = (CHAR)BM_QueryCheck (122);
   GetDlgItemText (130, GetDlgItemTextLength (130), Data->LimitClass);
}

VOID CUserDlg::Add (VOID)
{
   class CAddUserDlg *Dlg;

   if ((Dlg = new CAddUserDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         Data->Clear ();
         Data->ScreenWidth = 80;
         Data->ScreenHeight = 24;
         strcpy (Data->Name, Dlg->Name);
         strcpy (Data->RealName, Dlg->Name);
         Data->SetPassword (Dlg->Password);
         Data->Level = Cfg->NewUserLevel;
         Data->AccessFlags = Cfg->NewUserFlags;
         Data->DenyFlags = Cfg->NewUserDenyFlags;
         strcpy (Data->LimitClass, Cfg->NewUserLimits);
         Data->Add ();
         DisplayData ();
      }
      delete Dlg;
   }
}

VOID CUserDlg::Delete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      Data->Delete ();
      if (Data->Next () == FALSE) {
         if (Data->Previous () == FALSE)
            Data->Clear ();
      }
      DisplayData ();
   }
}

VOID CUserDlg::List (VOID)
{
   class CUserListDlg *Dlg;

   if ((Dlg = new CUserListDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      if (Dlg->DoModal () == TRUE)
         DisplayData ();
      delete Dlg;
   }
}

VOID CUserDlg::Password (VOID)
{
   class CPasswordDlg *Dlg;

   if ((Dlg = new CPasswordDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE)
         Data->SetPassword (Dlg->Password);
      delete Dlg;
   }
}

VOID CUserDlg::Next (VOID)
{
   if (Data->Next () == TRUE)
      DisplayData ();
}

VOID CUserDlg::Previous (VOID)
{
   if (Data->Previous () == TRUE)
      DisplayData ();
}

VOID CUserDlg::Security (VOID)
{
   class CUserSecurityDlg *Dlg;

   if ((Dlg = new CUserSecurityDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CUserDlg::Other (VOID)
{
   class CUserOtherDlg *Dlg;

   if ((Dlg = new CUserOtherDlg (m_hWnd)) != NULL) {
      Dlg->User = Data;
      Dlg->DoModal ();
      delete Dlg;
   }
}

VOID CUserDlg::Search (VOID)
{
   class CUserListDlg *Dlg;
   class CUserAskStringDlg *Ask;

   if ((Ask = new CUserAskStringDlg (m_hWnd)) != NULL) {
      if (Ask->DoModal () == TRUE) {
         if ((Dlg = new CUserListDlg (m_hWnd)) != NULL) {
            strcpy (Dlg->Search, Ask->String);
            Dlg->Found = FALSE;
            Dlg->Data = Data;
            if (Dlg->DoModal () == TRUE)
               DisplayData ();
            delete Dlg;
         }
      }
      delete Ask;
   }
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CUserListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
   ON_MESSAGE (WM_USER, OnUser)
END_MESSAGE_MAP ()

CUserListDlg::CUserListDlg (HWND p_hWnd) : CDialog ("25", p_hWnd)
{
   Search[0] = '\0';
   Found = TRUE;
}

USHORT CUserListDlg::OnInitDialog (VOID)
{
   SetWindowTitle ("Users List");

   LVM_AllocateColumns (101, 5);
   LVM_InsertColumn (101, "Name", LVC_LEFT);
   LVM_InsertColumn (101, "Level", LVC_RIGHT);
   LVM_InsertColumn (101, "Limit class", LVC_LEFT);
   LVM_InsertColumn (101, "City", LVC_LEFT);
   LVM_InsertColumn (101, "Last call", LVC_LEFT);

   toSelect = -1;
   i = 0;
   strupr (Search);

   if ((User = new TUser (Cfg->UserFile)) != NULL) {
      if (User->First () == TRUE) {
#if defined(__OS2__)
         WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));
         WinPostMsg (m_hWnd, WM_USER, 0L, 0L);
#elif defined(__NT__)
         PostMessage (m_hWnd, WM_USER, 0, 0L);
#endif
      }
   }

   return (TRUE);
}

VOID CUserListDlg::OnUser (VOID)
{
   USHORT Found = TRUE;
   CHAR Temp[128];
   struct tm *ltm;

   if (Search[0] != '\0') {
      strcpy (Temp, User->Name);
      if (strstr (strupr (Temp), Search) == NULL) {
         strcpy (Temp, User->RealName);
         if (strstr (strupr (Temp), Search) == NULL)
            Found = FALSE;
      }
   }

   if (Found == TRUE || Search[0] == '\0') {
      LVM_InsertItem (101);

      LVM_SetItemText (101, 0, User->Name);
      sprintf (Temp, "%u", User->Level);
      LVM_SetItemText (101, 1, Temp);
      LVM_SetItemText (101, 2, User->LimitClass);
      LVM_SetItemText (101, 3, User->City);
      ltm = localtime ((time_t *)&User->LastCall);
      sprintf (Temp, "%02d/%02d/%04d %02d:%02d", ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900, ltm->tm_hour, ltm->tm_min);
      LVM_SetItemText (101, 4, Temp);

      if (!strcmp (User->Name, Data->Name))
         toSelect = i;
      i++;
   }

   if (User->Next () == FALSE) {
      delete User;
      LVM_InvalidateView (101);
      if (toSelect != -1)
         LVM_SelectItem (101, toSelect);

#if defined(__OS2__)
      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE));
#elif defined(__NT__)
#endif
   }
   else
#if defined(__OS2__)
      WinPostMsg (m_hWnd, WM_USER, 0L, 0L);
#elif defined(__NT__)
      PostMessage (m_hWnd, WM_USER, 0, 0L);
#endif
}

VOID CUserListDlg::OnOK (VOID)
{
   int item;
   CHAR Temp[64];

   if ((item = LVM_QuerySelectedItem (101)) != -1) {
      LVM_QueryItemText (101, item, 0, Temp);
      Data->GetData (Temp);
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CUserSecurityDlg::CUserSecurityDlg (HWND p_hWnd) : CDialog ("18", p_hWnd)
{
}

USHORT CUserSecurityDlg::OnInitDialog (VOID)
{
   USHORT i;
   ULONG Test;

   Center ();
   SetWindowTitle ("User Security");

   SPBM_SetLimits (102, 65535U, 0L);
   SPBM_SetCurrentValue (102, Data->Level);

   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (Data->AccessFlags & Test)
         BM_SetCheck (i, TRUE);
   }
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (Data->DenyFlags & Test)
         BM_SetCheck (i, TRUE);
   }

   return (TRUE);
}

VOID CUserSecurityDlg::OnOK (VOID)
{
   USHORT i;
   ULONG Test;

   Data->Level = SPBM_QueryValue (102);

   Data->AccessFlags = 0L;
   for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->AccessFlags |= Test;
   }
   Data->DenyFlags = 0L;
   for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Data->DenyFlags |= Test;
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CUserOtherDlg::CUserOtherDlg (HWND p_hWnd) : CDialog ("38", p_hWnd)
{
}

USHORT CUserOtherDlg::OnInitDialog (VOID)
{
   Center ();

   SetDlgItemText (102, User->InetAddress);
   BM_SetCheck (103, User->ImportPOP3Mail);
   BM_SetCheck (104, User->UseInetAddress);
   SetDlgItemText (106, User->Pop3Pwd);

   return (TRUE);
}

VOID CUserOtherDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), User->InetAddress);
   User->ImportPOP3Mail = (UCHAR)BM_QueryCheck (103);
   User->UseInetAddress = (UCHAR)BM_QueryCheck (104);
   GetDlgItemText (106, GetDlgItemTextLength (106), User->Pop3Pwd);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CUserAskStringDlg::CUserAskStringDlg (HWND p_hWnd) : CDialog ("26", p_hWnd)
{
}

USHORT CUserAskStringDlg::OnInitDialog (VOID)
{
   Center ();

   SetWindowTitle ("Search User List");
   EM_SetTextLimit (128, sizeof (String) - 1);

   return (TRUE);
}

VOID CUserAskStringDlg::OnOK (VOID)
{
   GetDlgItemText (128, GetDlgItemTextLength (128), String);
   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CAddUserDlg::CAddUserDlg (HWND p_hWnd) : CDialog ("34", p_hWnd)
{
}

USHORT CAddUserDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Name) - 1);
   EM_SetTextLimit (104, sizeof (Password) - 1);

   return (TRUE);
}

VOID CAddUserDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Name);
   GetDlgItemText (104, GetDlgItemTextLength (104), Password);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CPasswordDlg::CPasswordDlg (HWND p_hWnd) : CDialog ("36", p_hWnd)
{
}

USHORT CPasswordDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Password) - 1);

   return (TRUE);
}

VOID CPasswordDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Password);

   EndDialog (TRUE);
}

#if !defined(__POINT__)
// ----------------------------------------------------------------------
// Offline Reader
// ----------------------------------------------------------------------

COfflineDlg::COfflineDlg (HWND p_hWnd) : CDialog ("42", p_hWnd)
{
}

USHORT COfflineDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Cfg->TempPath) - 1);
   EM_SetTextLimit (104, sizeof (Cfg->OLRPacketName) - 1);
   SPBM_SetLimits (106, 65535U, 0L);

   SetDlgItemText (102, Cfg->TempPath);
   SetDlgItemText (104, Cfg->OLRPacketName);
   SPBM_SetCurrentValue (106, Cfg->OLRMaxMessages);

   return (TRUE);
}

VOID COfflineDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Cfg->TempPath);
   GetDlgItemText (104, GetDlgItemTextLength (104), Cfg->OLRPacketName);
   Cfg->OLRMaxMessages = SPBM_QueryValue (106);

   EndDialog (TRUE);
}

#endif

// ----------------------------------------------------------------------
// External Protocols configuration
// ----------------------------------------------------------------------

class CProtocolListDlg : public CDialog
{
public:
   CProtocolListDlg (HWND p_hWnd);

   class  TProtocol *Data;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   DECLARE_MESSAGE_MAP ()
};

class CAddProtocolDlg : public CDialog
{
public:
   CAddProtocolDlg (HWND p_hWnd);

   CHAR   Key[16];
   CHAR   Description[64];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

BEGIN_MESSAGE_MAP (CProtocolDlg, CDialog)
   ON_COMMAND (115, OnAdd)
   ON_COMMAND (117, List)
   ON_COMMAND (118, OnPrevious)
   ON_COMMAND (119, OnNext)
   ON_COMMAND (116, OnDelete)
END_MESSAGE_MAP()

CProtocolDlg::CProtocolDlg (HWND p_hWnd) : CDialog ("44", p_hWnd)
{
   Data = NULL;
}

CProtocolDlg::~CProtocolDlg (void)
{
   if (Data != NULL)
      delete Data;
}

USHORT CProtocolDlg::OnInitDialog (VOID)
{
   Center ();
   Data = new TProtocol (Cfg->SystemPath);

   EM_SetTextLimit (102, sizeof (Data->Key) - 1);
   EM_SetTextLimit (104, sizeof (Data->Description) - 1);
   EM_SetTextLimit (108, sizeof (Data->DownloadCmd) - 1);
   EM_SetTextLimit (106, sizeof (Data->UploadCmd) - 1);
   EM_SetTextLimit (112, sizeof (Data->LogFileName) - 1);
   EM_SetTextLimit (114, sizeof (Data->CtlFileName) - 1);
   EM_SetTextLimit (124, sizeof (Data->DownloadCtlString) - 1);
   EM_SetTextLimit (126, sizeof (Data->UploadCtlString) - 1);
   EM_SetTextLimit (128, sizeof (Data->DownloadKeyword) - 1);
   EM_SetTextLimit (130, sizeof (Data->UploadKeyword) - 1);

   SPBM_SetLimits (132, 100, 0);
   SPBM_SetLimits (134, 100, 0);
   SPBM_SetLimits (136, 100, 0);

   if (Data->First () == FALSE)
      Data->New ();

   DisplayData ();

   return (TRUE);
}

VOID CProtocolDlg::DisplayData (VOID)
{
   BM_SetCheck (110, Data->Active);
   SetDlgItemText (102, Data->Key);
   SetDlgItemText (104, Data->Description);
   SetDlgItemText (108, Data->DownloadCmd);
   SetDlgItemText (106, Data->UploadCmd);
   SetDlgItemText (112, Data->LogFileName);
   SetDlgItemText (114, Data->CtlFileName);
   SetDlgItemText (124, Data->DownloadCtlString);
   SetDlgItemText (126, Data->UploadCtlString);
   SetDlgItemText (128, Data->DownloadKeyword);
   SetDlgItemText (130, Data->UploadKeyword);
   SPBM_SetCurrentValue (132, Data->FileNamePos);
   SPBM_SetCurrentValue (134, Data->SizePos);
   SPBM_SetCurrentValue (136, Data->CpsPos);
   BM_SetCheck (137, Data->Batch);
   BM_SetCheck (138, Data->DisablePort);
   BM_SetCheck (139, Data->ChangeToUploadPath);
}

VOID CProtocolDlg::ReadData (VOID)
{
   Data->Active = (CHAR)BM_QueryCheck (110);
   GetDlgItemText (102, GetDlgItemTextLength (102), Data->Key);
   GetDlgItemText (104, GetDlgItemTextLength (104), Data->Description);
   GetDlgItemText (108, GetDlgItemTextLength (108), Data->DownloadCmd);
   GetDlgItemText (106, GetDlgItemTextLength (106), Data->UploadCmd);
   GetDlgItemText (112, GetDlgItemTextLength (112), Data->LogFileName);
   GetDlgItemText (114, GetDlgItemTextLength (114), Data->CtlFileName);
   GetDlgItemText (124, GetDlgItemTextLength (124), Data->DownloadCtlString);
   GetDlgItemText (126, GetDlgItemTextLength (126), Data->UploadCtlString);
   GetDlgItemText (128, GetDlgItemTextLength (128), Data->DownloadKeyword);
   GetDlgItemText (130, GetDlgItemTextLength (130), Data->UploadKeyword);
   Data->FileNamePos = SPBM_QueryValue (132);
   Data->SizePos = SPBM_QueryValue (134);
   Data->CpsPos = SPBM_QueryValue (136);
   Data->Batch = (CHAR)BM_QueryCheck (137);
   Data->DisablePort = (CHAR)BM_QueryCheck (138);
   Data->ChangeToUploadPath = (CHAR)BM_QueryCheck (139);
}

VOID CProtocolDlg::OnAdd (VOID)
{
   class CAddProtocolDlg *Dlg;

   if ((Dlg = new CAddProtocolDlg (m_hWnd)) != NULL) {
      if (Dlg->DoModal () == TRUE) {
         Data->New ();
         strcpy (Data->Key, Dlg->Key);
         strcpy (Data->Description, Dlg->Description);
         Data->Add ();
         DisplayData ();
      }
      delete Dlg;
   }
   SetFocus (102);
}

VOID CProtocolDlg::OnDelete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      Data->Delete ();
      SetFocus (102);
      DisplayData ();
   }
}

VOID CProtocolDlg::OnPrevious (VOID)
{
   if (Data->Previous () == TRUE)
      DisplayData ();
}

VOID CProtocolDlg::OnNext (VOID)
{
   if (Data->Next () == TRUE)
      DisplayData ();
}

VOID CProtocolDlg::OnOK (VOID)
{
   ReadData ();
   Data->Update ();
}

VOID CProtocolDlg::List (VOID)
{
   class CProtocolListDlg *Dlg;

   if ((Dlg = new CProtocolListDlg (m_hWnd)) != NULL) {
      Dlg->Data = Data;
      if (Dlg->DoModal () == TRUE)
         DisplayData ();
      delete Dlg;
   }
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CProtocolListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
END_MESSAGE_MAP ()

CProtocolListDlg::CProtocolListDlg (HWND p_hWnd) : CDialog ("25", p_hWnd)
{
}

USHORT CProtocolListDlg::OnInitDialog (VOID)
{
   CHAR Temp[16];
   class TProtocol *Protocol;

   SetWindowTitle ("Protocol List");

   LVM_AllocateColumns (101, 4);
   LVM_InsertColumn (101, "Key", LVC_LEFT);
   LVM_InsertColumn (101, "Active", LVC_CENTER);
   LVM_InsertColumn (101, "Batch", LVC_CENTER);
   LVM_InsertColumn (101, "Description", LVC_LEFT);

   if ((Protocol = new TProtocol (Cfg->SystemPath)) != NULL) {
      if (Protocol->First () == TRUE)
         do {
            LVM_InsertItem (101);

            LVM_SetItemText (101, 0, Protocol->Key);
            sprintf (Temp, "%s", (Protocol->Active == TRUE) ? "Yes" : "No");
            LVM_SetItemText (101, 1, Temp);
            sprintf (Temp, "%s", (Protocol->Batch == TRUE) ? "Yes" : "No");
            LVM_SetItemText (101, 2, Temp);
            LVM_SetItemText (101, 3, Protocol->Description);
         } while (Protocol->Next () == TRUE);
      delete Protocol;
   }

   LVM_InvalidateView (101);

   return (TRUE);
}

VOID CProtocolListDlg::OnOK (VOID)
{
   int item;
   CHAR Temp[32];

   if ((item = LVM_QuerySelectedItem (101)) != -1) {
      LVM_QueryItemText (101, item, 0, Temp);
      Data->Read (Temp);
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CAddProtocolDlg::CAddProtocolDlg (HWND p_hWnd) : CDialog ("45", p_hWnd)
{
}

USHORT CAddProtocolDlg::OnInitDialog (VOID)
{
   Center ();

   EM_SetTextLimit (102, sizeof (Key) - 1);
   EM_SetTextLimit (104, sizeof (Description) - 1);

   return (TRUE);
}

VOID CAddProtocolDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), Key);
   GetDlgItemText (104, GetDlgItemTextLength (104), Description);

   EndDialog (TRUE);
}

