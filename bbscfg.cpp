
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
   ON_COMMAND (201, NewUserSecurity)
END_MESSAGE_MAP ()

CNewUserDlg::CNewUserDlg (HWND p_hWnd) : CDialog ("21", p_hWnd)
{
}

VOID CNewUserDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 21);
}

USHORT CNewUserDlg::OnInitDialog (VOID)
{
   class TLimits *Limits;

   Center ();
   SetFocus (101);

   BM_SetCheck (102 + Cfg->UseAnsi, TRUE);
   BM_SetCheck (107 + Cfg->UseAvatar, TRUE);
   BM_SetCheck (112 + Cfg->UseColor, TRUE);
   BM_SetCheck (116 + Cfg->UseFullScreenEditor, TRUE);
   BM_SetCheck (120 + Cfg->UseFullScreenReader, TRUE);
   BM_SetCheck (124 + Cfg->UseFullScreenLists, TRUE);
   BM_SetCheck (128 + Cfg->UseHotKey, TRUE);
   BM_SetCheck (132 + Cfg->UseIBMChars, TRUE);
   BM_SetCheck (136 + Cfg->AskLines, TRUE);
   BM_SetCheck (139 + Cfg->UsePause, TRUE);
   BM_SetCheck (143 + Cfg->UseScreenClear, TRUE);
   BM_SetCheck (147 + Cfg->AskBirthDate, TRUE);
   BM_SetCheck (150 + Cfg->AskMailCheck, TRUE);
   BM_SetCheck (154 + Cfg->AskFileCheck, TRUE);
   BM_SetCheck (158 + Cfg->AskAlias, TRUE);
   BM_SetCheck (162 + Cfg->AskCompanyName, TRUE);
   BM_SetCheck (166 + Cfg->AskAddress, TRUE);
   BM_SetCheck (170 + Cfg->AskCity, TRUE);
   BM_SetCheck (174 + Cfg->AskPhoneNumber, TRUE);
   BM_SetCheck (178 + Cfg->AskGender, TRUE);

   if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
      if (Limits->First () == TRUE)
         do {
            LM_AddString (182, Limits->Key);
         } while (Limits->Next () == TRUE);
         delete Limits;
   }
   SetDlgItemText (182, Cfg->NewUserLimits);

   return (TRUE);
}

UCHAR CNewUserDlg::GetSelection (USHORT id, USHORT num)
{
   UCHAR i;

   for (i = 0; i < num; i++) {
      if (BM_QueryCheck (id + i) == TRUE)
         return (i);
   }

   return (0);
}

VOID CNewUserDlg::OnOK (VOID)
{
   Cfg->UseAnsi = GetSelection (102, 4);
   Cfg->UseAvatar = GetSelection (107, 4);
   Cfg->UseColor = GetSelection (112, 3);
   Cfg->UseFullScreenEditor = GetSelection (116, 3);
   Cfg->UseFullScreenReader = GetSelection (120, 3);
   Cfg->UseFullScreenLists = GetSelection (124, 3);
   Cfg->UseHotKey = GetSelection (128, 3);
   Cfg->UseIBMChars = GetSelection (132, 3);
   Cfg->AskLines = GetSelection (136, 2);
   Cfg->UsePause = GetSelection (139, 3);
   Cfg->UseScreenClear = GetSelection (143, 3);
   Cfg->AskBirthDate = GetSelection (147, 2);
   Cfg->AskMailCheck = GetSelection (150, 3);
   Cfg->AskFileCheck = GetSelection (154, 3);
   Cfg->AskAlias = GetSelection (158, 3);
   Cfg->AskCompanyName = GetSelection (162, 3);
   Cfg->AskAddress = GetSelection (166, 3);
   Cfg->AskCity = GetSelection (170, 3);
   Cfg->AskPhoneNumber = GetSelection (174, 3);
   Cfg->AskGender = GetSelection (178, 3);

   GetDlgItemText (182, Cfg->NewUserLimits, GetDlgItemTextLength (182));

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

   SPBM_SetLimits (102, 65535L, 0L);
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

   Cfg->NewUserLevel = (USHORT)SPBM_QueryValue (102);

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

// Moving between menu
MENUCMD MenuCmd1[] = {
   MNU_GOTO,       "Goto menu          ",
   MNU_GOSUB,      "Gosub menu         ",
   MNU_RETURN,     "Return to previous ",
   MNU_CLEARSTACK, "Clear menu stack   ",
   MNU_CLEARGOSUB, "Clear gosub menu   ",
   MNU_CLEARGOTO,  "Clear goto menu    ",
   MNU_RETURNMAIN, "Return to MAIN menu",
   0, NULL
};

// Message areas
MENUCMD MenuCmd2[] = {
   MNU_MSGSELECT,       "Change message area      ",
   MNU_MSGDELETE,       "Kill message             ",
   MNU_MSGWRITE,        "Edit new message         ",
   MNU_MSGREPLY,        "Reply to message         ",
   MNU_MSGBRIEFLIST,    "Short message list       ",
   MNU_MSGFORWARD,      "Read next message        ",
   MNU_MSGBACKWARD,     "Read previous message    ",
   MNU_MSGREADNONSTOP,  "Read message non-stop    ",
   MNU_INQUIRETEXT,     "Inquire messages         ",
   MNU_MSGINDIVIDUAL,   "Read individual message  ",
   MNU_MSGTITLELIST,    "Verbose message list     ",
   MNU_MSGLISTNEWAREAS, "List areas w/new messages",
   MNU_MSGREAD,         "Read messages            ",
   MNU_INQUIREPERSONAL, "Inquire personal messages",
   MNU_MSGMODIFY,       "Change message           ",
   MNU_MSGUNRECEIVE,    "Unreceive message        ",
   MNU_MSGREADORIGINAL, "Read original message    ",
   MNU_MSGREADREPLY,    "Read reply message       ",
   MNU_MSGREADCURRENT,  "Read current message     ",
   MNU_TOGGLEKLUDGES,   "Toggle kludges lines     ",
   0, NULL
};

// File areas
MENUCMD MenuCmd3[] = {
   MNU_FILENAMELIST, "File list",
   MNU_FILEDOWNLOAD, "Download file",
   MNU_FILETEXTLIST, "Locate files",
   MNU_FILEUPLOAD, "Upload file",
   MNU_FILENEWLIST, "New files list",
   MNU_FILEDOWNLOADANY, "Download from any area",
   MNU_FILEDELETE, "Kill files",
   MNU_FILESELECT, "Change file area",
   MNU_SEARCHFILENAME, "Locate files by name",
   MNU_FILEKEYWORDLIST, "Locate files by keyword",
   MNU_FILEDATELIST, "File list by date",
   MNU_FILEDOWNLOADLIST, "Download list of files",
   MNU_FILEUPLOADUSER, "Upload file to user",
   MNU_FILEDISPLAY, "File Display",
   MNU_ADDTAGGED, "Tag files",
   MNU_DELETETAGGED, "Delete tagged files",
   MNU_LISTTAGGED, "List tagged files",
   MNU_DELETEALLTAGGED, "Delete all tagged files",
   0, NULL
};

// User configuration
MENUCMD MenuCmd4[] = {
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
   MNU_TOGGLEFULLSCREEN, "Toggle fullscreen lists",
   MNU_TOGGLEIBMCHARS, "Toggle IBM characters",
   MNU_TOGGLEMOREPROMPT, "Toggle More? prompt",
   MNU_TOGGLESCREENCLEAR, "Toggle screen clear",
   MNU_TOGGLEINUSERLIST, "Toggle in user list",
   MNU_SETARCHIVER, "Set default archiver",
   MNU_SETPROTOCOL, "Set default protocol",
   MNU_SETSIGNATURE, "Set personal signature",
   MNU_SETVIDEOMODE, "Set video mode",
   MNU_TOGGLEFULLED, "Toggle fullscreen editor",
   MNU_TOGGLEFULLREAD, "Toggle fullscreen reader",
   MNU_TOGGLENODISTURB, "Toggle do not disturb flag",
   MNU_TOGGLEMAILCHECK, "Toggle logon mail check",
   MNU_TOGGLEFILECHECK, "Toggle new files check",
   MNU_SETBIRTHDATE, "Set birthdate",
   MNU_SETSCREENLENGTH, "Set screen length",
   MNU_TOGGLERIP, "Toggle RIP graphics",
   0, NULL
};

// Personal mail
MENUCMD MenuCmd5[] = {
   MNU_MAILWRITELOCAL,    "Write local mail    ",
   MNU_MAILWRITEINTERNET, "Write Internet mail ",
   MNU_MAILREAD,          "Read mail           ",
   MNU_MAILDELETE,        "Kill mail           ",
   MNU_MAILLIST,          "List mail           ",
   MNU_MAILWRITEFIDONET,  "Write FidoNet mail  ",
   MNU_MAILCHECK,         "Check unread mail   ",
   MNU_MAILNEXT,          "Read next mail      ",
   MNU_MAILPREVIOUS,      "Read previous mail  ",
   MNU_MAILINDIVIDUAL,    "Read individual mail",
   MNU_MAILNONSTOP,       "Read mail non-stop  ",
   MNU_MAILREPLY,         "Reply mail          ",
   0, NULL
};

// Offline reader
MENUCMD MenuCmd6[] = {
   MNU_OLRTAGAREA, "Tag areas",
   MNU_OLRDOWNLOADASCII, "ASCII download",
   MNU_OLRUPLOAD, "Upload replies",
   MNU_OLRDOWNLOADQWK, "QWK download",
   MNU_OLRDOWNLOADBW, "BlueWave download",
   MNU_OLRDOWNLOADPNT, "PointMail download",
   MNU_OLRREMOVEAREA, "Untag areas",
   MNU_OLRVIEWTAGGED, "View tagged areas",
   MNU_OLRRESTRICTDATE, "Restrict date",
   0, NULL
};

   // Miscellaneous
MENUCMD MenuCmd7[] = {
   MNU_DISPLAY, "Display file (anywhere)",
   MNU_NULL, "Display only",
   MNU_LOGOFF, "Logoff",
   MNU_VERSION, "Version information",
   MNU_RUNEXTERNAL, "Run external program",
   MNU_PRESSENTER, "Press Enter to Continue",
   MNU_FINGER, "Finger",
   MNU_FTP, "FTP client",
   MNU_IRC, "IRC client",
   MNU_APPENDMENU, "Append menu",
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

   int    old_item;
   class  TMenu *Data;

   VOID   OnCancel (VOID);
   VOID   OnChanged (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   CHAR   Temp[128];
   DECLARE_MESSAGE_MAP ()
};

class CMenuCommandDlg : public CDialog
{
public:
   CMenuCommandDlg (HWND p_hWnd);

   USHORT id;
   CHAR   text[64];

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   DECLARE_MESSAGE_MAP ()

   VOID   Selection (VOID);
};

BEGIN_MESSAGE_MAP (CMenuDlg, CDialog)
   ON_COMMAND (115, Delete)
   ON_COMMAND (117, Add)
   ON_COMMAND (118, Previous)
   ON_COMMAND (119, Next)
   ON_COMMAND (120, Prompt)
   ON_COMMAND (121, List)
   ON_COMMAND (122, Security)
   ON_COMMAND (123, Insert)
   ON_COMMAND (124, HilightColor)
   ON_COMMAND (125, Color)
   ON_COMMAND (126, Command)
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

VOID CMenuDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 8);
}

USHORT CMenuDlg::OnInitDialog (VOID)
{
   CHAR Temp[128];

   FullFile[0] = '\0';

#if defined(__OS2__)
   memset (&fild, 0, sizeof (FILEDLG));
   fild.cbSize = sizeof (FILEDLG);
   fild.fl = FDS_CENTER|FDS_OPEN_DIALOG;
   fild.pszTitle = "Open Menu File";
   sprintf (fild.szFullFile, "%s*.mnu", Cfg->MenuPath);

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
   sprintf (FullFile, "%s*.mnu", Cfg->MenuPath);
   OpenFileName.lpstrFile = FullFile;
   OpenFileName.nMaxFile = sizeof (FullFile) - 1;
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
      SPBM_SetLimits (104, 255L, 0L);
      SPBM_SetLimits (106, 255L, 0L);

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
   CHAR Temp[64];

   SetDlgItemText (102, Menu->Display);
   SPBM_SetCurrentValue (104, Menu->Color);
   SPBM_SetCurrentValue (106, Menu->Hilight);
   SetDlgItemText (108, Menu->Key);

   for (i = 0; MenuCmd1[i].Text != NULL; i++) {
      if (MenuCmd1[i].Id == Menu->Command) {
         sprintf (Temp, "%u - %s", MenuCmd1[i].Id, MenuCmd1[i].Text);
         SetDlgItemText (110, Temp);
         break;
      }
   }
   for (i = 0; MenuCmd2[i].Text != NULL; i++) {
      if (MenuCmd2[i].Id == Menu->Command) {
         sprintf (Temp, "%u - %s", MenuCmd2[i].Id, MenuCmd2[i].Text);
         SetDlgItemText (110, Temp);
         break;
      }
   }
   for (i = 0; MenuCmd3[i].Text != NULL; i++) {
      if (MenuCmd3[i].Id == Menu->Command) {
         sprintf (Temp, "%u - %s", MenuCmd3[i].Id, MenuCmd3[i].Text);
         SetDlgItemText (110, Temp);
         break;
      }
   }
   for (i = 0; MenuCmd4[i].Text != NULL; i++) {
      if (MenuCmd4[i].Id == Menu->Command) {
         sprintf (Temp, "%u - %s", MenuCmd4[i].Id, MenuCmd4[i].Text);
         SetDlgItemText (110, Temp);
         break;
      }
   }
   for (i = 0; MenuCmd5[i].Text != NULL; i++) {
      if (MenuCmd5[i].Id == Menu->Command) {
         sprintf (Temp, "%u - %s", MenuCmd5[i].Id, MenuCmd5[i].Text);
         SetDlgItemText (110, Temp);
         break;
      }
   }
   for (i = 0; MenuCmd6[i].Text != NULL; i++) {
      if (MenuCmd6[i].Id == Menu->Command) {
         sprintf (Temp, "%u - %s", MenuCmd6[i].Id, MenuCmd6[i].Text);
         SetDlgItemText (110, Temp);
         break;
      }
   }
   for (i = 0; MenuCmd7[i].Text != NULL; i++) {
      if (MenuCmd7[i].Id == Menu->Command) {
         sprintf (Temp, "%u - %s", MenuCmd7[i].Id, MenuCmd7[i].Text);
         SetDlgItemText (110, Temp);
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
//   Menu->Command = MenuCmd[Value].Id;
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
   ReadData ();
   Menu->Update ();
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

   ReadData ();
   Menu->Update ();

   if ((Dlg = new CMenuListDlg (m_hWnd)) != NULL) {
      Dlg->Data = Menu;
      Dlg->DoModal ();
      delete Dlg;

      DisplayData ();
      SetFocus (102);
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
   class CMenuCommandDlg *Dlg;

   if ((Dlg = new CMenuCommandDlg (m_hWnd)) != NULL) {
      Dlg->id = Menu->Command;
      if (Dlg->DoModal () == TRUE) {
         Menu->Command = Dlg->id;
         SetDlgItemText (110, Dlg->text);
      }
      delete Dlg;
   }
}

// ----------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CMenuCommandDlg, CDialog)
#if defined(__OS2__)
   ON_CONTROL (LN_SELECT, 103, Selection)
#elif defined(__NT__)
   ON_NOTIFY (LBN_SELCHANGE, 103, Selection)
#endif
END_MESSAGE_MAP ()

CMenuCommandDlg::CMenuCommandDlg (HWND p_hWnd) : CDialog ("54", p_hWnd)
{
   id = 0;
}

USHORT CMenuCommandDlg::OnInitDialog (VOID)
{
   int i;

   Center ();

   LM_AddString (103, "Moving Between Menus");
   LM_AddString (103, "Message Areas");
   LM_AddString (103, "File Areas");
   LM_AddString (103, "User Configuration");
   LM_AddString (103, "Personal Mail");
   LM_AddString (103, "Offline Reader");
   LM_AddString (103, "Miscellaneous");

   for (i = 0; MenuCmd1[i].Text != NULL; i++) {
      if (MenuCmd1[i].Id == id) {
         LM_SelectItem (103, 0);
         LM_SelectItem (104, (USHORT)i);
         break;
      }
   }
   for (i = 0; MenuCmd2[i].Text != NULL; i++) {
      if (MenuCmd2[i].Id == id) {
         LM_SelectItem (103, 1);
         LM_SelectItem (104, (USHORT)i);
         break;
      }
   }
   for (i = 0; MenuCmd3[i].Text != NULL; i++) {
      if (MenuCmd3[i].Id == id) {
         LM_SelectItem (103, 2);
         LM_SelectItem (104, (USHORT)i);
         break;
      }
   }
   for (i = 0; MenuCmd4[i].Text != NULL; i++) {
      if (MenuCmd4[i].Id == id) {
         LM_SelectItem (103, 3);
         LM_SelectItem (104, (USHORT)i);
         break;
      }
   }
   for (i = 0; MenuCmd5[i].Text != NULL; i++) {
      if (MenuCmd5[i].Id == id) {
         LM_SelectItem (103, 4);
         LM_SelectItem (104, (USHORT)i);
         break;
      }
   }
   for (i = 0; MenuCmd6[i].Text != NULL; i++) {
      if (MenuCmd6[i].Id == id) {
         LM_SelectItem (103, 5);
         LM_SelectItem (104, (USHORT)i);
         break;
      }
   }
   for (i = 0; MenuCmd7[i].Text != NULL; i++) {
      if (MenuCmd7[i].Id == id) {
         LM_SelectItem (103, 6);
         LM_SelectItem (104, (USHORT)i);
         break;
      }
   }

#if defined(__NT__)
   Selection ();
#endif

   return (TRUE);
}

VOID CMenuCommandDlg::Selection (VOID)
{
   int i;
   CHAR Temp[64];

   switch (LM_QuerySelection (103)) {
      case 0:
         LM_DeleteAll (104);
         for (i = 0; MenuCmd1[i].Text != NULL; i++) {
            sprintf (Temp, "%u - %s", MenuCmd1[i].Id, MenuCmd1[i].Text);
            LM_AddString (104, Temp);
         }
         break;
      case 1:
         LM_DeleteAll (104);
         for (i = 0; MenuCmd2[i].Text != NULL; i++) {
            sprintf (Temp, "%u - %s", MenuCmd2[i].Id, MenuCmd2[i].Text);
            LM_AddString (104, Temp);
         }
         break;
      case 2:
         LM_DeleteAll (104);
         for (i = 0; MenuCmd3[i].Text != NULL; i++) {
            sprintf (Temp, "%u - %s", MenuCmd3[i].Id, MenuCmd3[i].Text);
            LM_AddString (104, Temp);
         }
         break;
      case 3:
         LM_DeleteAll (104);
         for (i = 0; MenuCmd4[i].Text != NULL; i++) {
            sprintf (Temp, "%u - %s", MenuCmd4[i].Id, MenuCmd4[i].Text);
            LM_AddString (104, Temp);
         }
         break;
      case 4:
         LM_DeleteAll (104);
         for (i = 0; MenuCmd5[i].Text != NULL; i++) {
            sprintf (Temp, "%u - %s", MenuCmd5[i].Id, MenuCmd5[i].Text);
            LM_AddString (104, Temp);
         }
         break;
      case 5:
         LM_DeleteAll (104);
         for (i = 0; MenuCmd6[i].Text != NULL; i++) {
            sprintf (Temp, "%u - %s", MenuCmd6[i].Id, MenuCmd6[i].Text);
            LM_AddString (104, Temp);
         }
         break;
      case 6:
         LM_DeleteAll (104);
         for (i = 0; MenuCmd7[i].Text != NULL; i++) {
            sprintf (Temp, "%u - %s", MenuCmd7[i].Id, MenuCmd7[i].Text);
            LM_AddString (104, Temp);
         }
         break;
   }
}

VOID CMenuCommandDlg::OnOK (VOID)
{
   USHORT item;

   item = LM_QuerySelection (104);

   switch (LM_QuerySelection (103)) {
      case 0:
         id = MenuCmd1[item].Id;
         sprintf (text, "%u - %s", MenuCmd1[item].Id, MenuCmd1[item].Text);
         break;
      case 1:
         id = MenuCmd2[item].Id;
         sprintf (text, "%u - %s", MenuCmd2[item].Id, MenuCmd2[item].Text);
         break;
      case 2:
         id = MenuCmd3[item].Id;
         sprintf (text, "%u - %s", MenuCmd3[item].Id, MenuCmd3[item].Text);
         break;
      case 3:
         id = MenuCmd4[item].Id;
         sprintf (text, "%u - %s", MenuCmd4[item].Id, MenuCmd4[item].Text);
         break;
      case 4:
         id = MenuCmd5[item].Id;
         sprintf (text, "%u - %s", MenuCmd5[item].Id, MenuCmd5[item].Text);
         break;
      case 5:
         id = MenuCmd6[item].Id;
         sprintf (text, "%u - %s", MenuCmd6[item].Id, MenuCmd6[item].Text);
         break;
      case 6:
         id = MenuCmd7[item].Id;
         sprintf (text, "%u - %s", MenuCmd7[item].Id, MenuCmd7[item].Text);
         break;
   }

   EndDialog (TRUE);
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

   Data->Level = (USHORT)SPBM_QueryValue (102);

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
   old_item = 0;
}

USHORT CMenuListDlg::OnInitDialog (VOID)
{
   int i;
   ULONG CrcCurrent, Crc;

   SetWindowTitle ("Menu List");

   LVM_AllocateColumns (101, 5);
   LVM_InsertColumn (101, "Display", LVC_LEFT);
   LVM_InsertColumn (101, "Key", LVC_LEFT);
   LVM_InsertColumn (101, "Level", LVC_RIGHT);
   LVM_InsertColumn (101, "Command", LVC_LEFT);
   LVM_InsertColumn (101, "Argument", LVC_LEFT);

   // Calcola il CRC di alcuni elementi chiave per determinare l'indice della voce
   // di menu' correntemente selezionata.
   CrcCurrent = StringCrc32 (Data->Display, 0xFFFFFFFFL);
   CrcCurrent = StringCrc32 (Data->Key, CrcCurrent);
   sprintf (Temp, "%u", Data->Command);
   CrcCurrent = StringCrc32 (Temp, CrcCurrent);

   i = 0;
   if (Data->First () == TRUE)
      do {
         LVM_InsertItem (101);

         LVM_SetItemText (101, 0, Data->Display);
         LVM_SetItemText (101, 1, Data->Key);
         sprintf (Temp, "%u", Data->Level);
         LVM_SetItemText (101, 2, Temp);
         LVM_SetItemText (101, 3, "");
         LVM_SetItemText (101, 4, Data->Argument);

         // Calcola il CRC di alcuni elementi chiavi per controllare se la voce
         // inserita e' quella correntemente selezionata nell'editor.
         Crc = StringCrc32 (Data->Display, 0xFFFFFFFFL);
         Crc = StringCrc32 (Data->Key, Crc);
         sprintf (Temp, "%u", Data->Command);
         Crc = StringCrc32 (Temp, Crc);

         // Se e' quella selezionata memorizza l'indice per l'uso mediante il
         // tasto cancel e per la selezione dell'elemento di default.
         if (Crc == CrcCurrent)
            old_item = i;

         i++;
      } while (Data->Next () == TRUE);

   LVM_InvalidateView (101);
   LVM_SelectItem (101, (USHORT)old_item);

   return (TRUE);
}

VOID CMenuListDlg::OnOK (VOID)
{
   int i, item;

   if ((item = LVM_QuerySelectedItem (101)) == -1)
      item = old_item;

   Data->First ();
   for (i = 0; i < item; i++)
      Data->Next ();

   EndDialog (TRUE);
}

VOID CMenuListDlg::OnCancel (VOID)
{
   int i;

   Data->First ();
   for (i = 0; i < old_item; i++)
      Data->Next ();

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
   CHAR   Area[32];
   CHAR   Search[128];

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
   ON_COMMAND (124, Move)
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

VOID CFileDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 10);
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
   GetDlgItemText (104, Data->Display, GetDlgItemTextLength (104));
   GetDlgItemText (106, Data->Upload, GetDlgItemTextLength (106));
   GetDlgItemText (108, Data->Download, GetDlgItemTextLength (108));
   Data->ShowGlobal = (UCHAR)BM_QueryCheck (113);
   Data->FreeDownload = (UCHAR)BM_QueryCheck (123);
   Data->CdRom = (UCHAR)BM_QueryCheck (114);
   GetDlgItemText (127, Data->EchoTag, GetDlgItemTextLength (127));
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
   CHAR Temp[32];
   class TFileBase *File;
   class TUser *User;

   ReadData ();
   GetDlgItemText (102, GetDlgItemTextLength (102), Temp);
   if (strcmp (Temp, Data->Key)) {
      if ((File = new TFileBase (Cfg->SystemPath, "")) != NULL) {
         Data->ActiveFiles = File->ChangeLibrary (Data->Key, Temp);
         delete File;
      }
      if ((User = new TUser (Cfg->UserFile)) != NULL) {
         User->FileTag->Change (Data->Key, Temp);
         delete User;
      }
   }

   Data->Update (Temp);
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
      strcpy (Dlg->Area, Data->Key);
      if (Dlg->DoModal () == TRUE) {
         Data->Read (Dlg->Area, FALSE);
         DisplayData ();
      }
      delete Dlg;
   }
}

VOID CFileDlg::Move (VOID)
{
   CHAR Key[32];
   class CFileListDlg *Dlg;
   class TFileData *NewData;

   Key[0] = '\0';
   if ((Dlg = new CFileListDlg (m_hWnd)) != NULL) {
      strcpy (Dlg->Area, Data->Key);
      if (Dlg->DoModal () == TRUE) {
         if ((NewData = new TFileData (Cfg->SystemPath)) != NULL) {
            NewData->Read (Data->Key);
            NewData->Delete ();
            strcpy (Key, NewData->Key);
            NewData->Read (Dlg->Area);
            NewData->Insert (Data);
            delete NewData;
         }
         if (Data->Read (Key, FALSE) == FALSE)
            Data->Read (Dlg->Area, FALSE);
         DisplayData ();
      }
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
            strcpy (Dlg->Area, Data->Key);
            if (Dlg->DoModal () == TRUE) {
               Data->Read (Dlg->Area, FALSE);
               DisplayData ();
            }
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

   Data->Level = (USHORT)SPBM_QueryValue (102);

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

   Data->DownloadLevel = (USHORT)SPBM_QueryValue (173);

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

   Data->UploadLevel = (USHORT)SPBM_QueryValue (242);

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
   USHORT i = 0, select = 0;
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

            if (!strcmp (File->Key, Area))
               select = i;
            i++;

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
   else
      LVM_SelectItem (101, select);

   return (TRUE);
}

VOID CFileListDlg::OnOK (VOID)
{
   int item;

   if ((item = LVM_QuerySelectedItem (101)) != -1)
      LVM_QueryItemText (101, item, 0, Area);

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
   CHAR   Area[32];
   CHAR   Search[128];

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
   ON_COMMAND (124, Move)
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

VOID CMessageDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 9);
}

USHORT CMessageDlg::OnInitDialog (VOID)
{
   FILE *fp;
   CHAR Temp[128];

   Center ();
   Data = new TMsgData (Cfg->SystemPath);

   EM_SetTextLimit (102, sizeof (Data->Key) - 1);
   EM_SetTextLimit (104, sizeof (Data->Display) - 1);
   EM_SetTextLimit (106, sizeof (Data->Path) - 1);
   EM_SetTextLimit (126, sizeof (Data->EchoTag) - 1);
   EM_SetTextLimit (128, sizeof (Data->NewsGroup) - 1);
   EM_SetTextLimit (111, sizeof (Data->Origin) - 1);

   CB_AddString (107, "Squish<tm>");
   CB_AddString (107, "JAM");
   CB_AddString (107, "Fido (*.msg)");
   CB_AddString (107, "AdeptXBBS");
   CB_AddString (107, "Hudson");
   CB_AddString (107, "USENET Newsgroup");
   CB_AddString (107, "Passthrough");

   SPBM_SetLimits (138, 5000L, 0L);
   SPBM_SetLimits (136, 366L, 0L);
   SPBM_SetLimits (140, 999999L, 0L);
   SPBM_SetLimits (145, 255L, 1L);

   if (Cfg->MailAddress.First () == TRUE)
      do {
         CB_AddString (110, Cfg->MailAddress.String);
      } while (Cfg->MailAddress.Next () == TRUE);

   CB_AddString (111, "<DEFAULT>");
   CB_AddString (111, "<RANDOM>");

   sprintf (Temp, "%sorigin.txt", Cfg->SystemPath);
   if ((fp = fopen (Temp, "rt")) != NULL) {
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         if (Temp[strlen (Temp) - 1] == '\n')
            Temp[strlen (Temp) - 1] = '\0';
         CB_AddString (111, Temp);
      }
      fclose (fp);
   }

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
      case ST_HUDSON:
         CB_SelectItem (107, 4);
         break;
      case ST_USENET:
         CB_SelectItem (107, 5);
         break;
      case ST_PASSTHROUGH:
         CB_SelectItem (107, 6);
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
   SPBM_SetCurrentValue (140, Data->Highest);
   SetDlgItemText (110, Data->Address);
   SPBM_SetCurrentValue (145, Data->Board);
   if (Data->Origin[0] != '\0')
      SetDlgItemText (111, Data->Origin);
   else if (Data->OriginIndex == OIDX_DEFAULT)
      SetDlgItemText (111, "<DEFAULT>");
   else if (Data->OriginIndex == OIDX_RANDOM)
      SetDlgItemText (111, "<RANDOM>");
   else
      CB_SelectItem (111, (USHORT)(Data->OriginIndex + 1));
}

VOID CMessageDlg::ReadData (VOID)
{
   FILE *fp;
   USHORT index;
   CHAR Temp[128];

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
         Data->Storage = ST_HUDSON;
         break;
      case 5:
         Data->Storage = ST_USENET;
         break;
      case 6:
         Data->Storage = ST_PASSTHROUGH;
         break;
   }
   Data->ShowGlobal = (CHAR)BM_QueryCheck (113);
   Data->Offline = (CHAR)BM_QueryCheck (114);
   Data->EchoMail = (CHAR)BM_QueryCheck (129);
   Data->UpdateNews = (CHAR)BM_QueryCheck (130);
   GetDlgItemText (126, GetDlgItemTextLength (126), Data->EchoTag);
   GetDlgItemText (128, GetDlgItemTextLength (128), Data->NewsGroup);
   Data->MaxMessages = (USHORT)SPBM_QueryValue (138);
   Data->DaysOld = (USHORT)SPBM_QueryValue (136);
   Data->Highest = SPBM_QueryValue (140);
   GetDlgItemText (110, GetDlgItemTextLength (110), Data->Address);
   GetDlgItemText (111, GetDlgItemTextLength (111), Temp);
   Data->Board = (USHORT)SPBM_QueryValue (145);
   Data->Origin[0] = '\0';
   if (!strcmp (Temp, "<DEFAULT>"))
      Data->OriginIndex = OIDX_DEFAULT;
   else if (!strcmp (Temp, "<RANDOM>"))
      Data->OriginIndex = OIDX_RANDOM;
   else {
      strcpy (Data->Origin, Temp);

      sprintf (Temp, "%sorigin.txt", Cfg->SystemPath);
      if ((fp = fopen (Temp, "rt")) != NULL) {
         index = 1;
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            if (Temp[strlen (Temp) - 1] == '\n')
               Temp[strlen (Temp) - 1] = '\0';
            if (!strcmp (Temp, Data->Origin)) {
               Data->OriginIndex = index;
               Data->Origin[0] = '\0';
               break;
            }
            index++;
         }
         fclose (fp);
      }
   }
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
   CHAR Temp[32];
   class TUser *User;

   ReadData ();
   GetDlgItemText (102, GetDlgItemTextLength (102), Temp);
   if (stricmp (Data->Key, Temp)) {
      if ((User = new TUser (Cfg->UserFile)) != NULL) {
         User->MsgTag->Change (Data->Key, Temp);
         delete User;
      }
   }

   Data->Update (Temp);
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
      strcpy (Dlg->Area, Data->Key);
      if (Dlg->DoModal () == TRUE) {
         Data->Read (Dlg->Area, FALSE);
         DisplayData ();
      }
      delete Dlg;
   }
}

VOID CMessageDlg::Move (VOID)
{
   CHAR Key[32];
   class CMessageListDlg *Dlg;
   class TMsgData *NewData;

   Key[0] = '\0';
   if ((Dlg = new CMessageListDlg (m_hWnd)) != NULL) {
      strcpy (Dlg->Area, Data->Key);
      if (Dlg->DoModal () == TRUE) {
         if ((NewData = new TMsgData (Cfg->SystemPath)) != NULL) {
            NewData->Read (Data->Key);
            NewData->Delete ();
            strcpy (Key, NewData->Key);
            NewData->Read (Dlg->Area);
            NewData->Insert (Data);
            delete NewData;
         }
         if (Data->Read (Key, FALSE) == FALSE)
            Data->Read (Dlg->Area, FALSE);
         DisplayData ();
      }
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
            strcpy (Dlg->Area, Data->Key);
            if (Dlg->DoModal () == TRUE) {
               Data->Read (Dlg->Area, FALSE);
               DisplayData ();
            }
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

   Data->Level = (USHORT)SPBM_QueryValue (102);

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

   Data->WriteLevel = (USHORT)SPBM_QueryValue (173);

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
   USHORT i =  0, select = 0;
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

            if (!strcmp (Msg->Key, Area))
               select = i;
            i++;

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
   else
      LVM_SelectItem (101, select);

   return (TRUE);
}

VOID CMessageListDlg::OnOK (VOID)
{
   int item;

   if ((item = LVM_QuerySelectedItem (101)) != -1)
      LVM_QueryItemText (101, item, 0, Area);

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

class CSecurityDlg : public CDialog
{
public:
   CSecurityDlg (HWND p_hWnd);

   USHORT Level;
   ULONG  Flags;
   ULONG  DenyFlags;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

BEGIN_MESSAGE_MAP (CLimitsDlg, CDialog)
   ON_COMMAND (198, Add)
   ON_COMMAND (199, Delete)
   ON_COMMAND (200, Previous)
   ON_COMMAND (201, Next)
END_MESSAGE_MAP ()

CLimitsDlg::CLimitsDlg (HWND p_hWnd) : CDialog ("4", p_hWnd)
{
   Limits = NULL;
}

CLimitsDlg::~CLimitsDlg ()
{
   if (Limits != NULL)
      delete Limits;
}

VOID CLimitsDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 4);
}

USHORT CLimitsDlg::OnInitDialog (VOID)
{
   Center ();

   Limits = new TLimits (Cfg->SystemPath);

   EM_SetTextLimit (102, sizeof (Limits->Key) - 1);
   EM_SetTextLimit (104, sizeof (Limits->Description) - 1);

   SPBM_SetLimits (132, 65535L, 0L);
   SPBM_SetLimits (126, 1440L, 0L);
   SPBM_SetLimits (128, 1440L, 0L);
   SPBM_SetLimits (113, 65535L, 0L);
   SPBM_SetLimits (115, 65535L, 0L);
   SPBM_SetLimits (117, 65535L, 0L);
   SPBM_SetLimits (119, 65535L, 0L);
   SPBM_SetLimits (121, 65535L, 0L);
   SPBM_SetLimits (123, 65535L, 0L);
   SPBM_SetLimits (109, 100L, 0L);
   SPBM_SetLimits (111, 65535L, 0L);

   CB_AddString (107, "300");
   CB_AddString (107, "1200");
   CB_AddString (107, "2400");
   CB_AddString (107, "9600");
   CB_AddString (107, "14400");
   CB_AddString (107, "28800");
   CB_AddString (107, "33600");
   CB_AddString (107, "56000");
   CB_AddString (107, "64000");
   CB_AddString (107, "128000");
   CB_AddString (107, "256000");

   if (Limits->First () == TRUE)
      DisplayData ();

   return (TRUE);
}

VOID CLimitsDlg::DisplayData (VOID)
{
   USHORT i;
   ULONG Test;

   SetDlgItemText (102, Limits->Key);
   SetDlgItemText (104, Limits->Description);
   switch (Limits->DownloadSpeed) {
      case 300L:
         CB_SelectItem (107, 0);
         break;
      case 1200L:
         CB_SelectItem (107, 1);
         break;
      case 2400L:
         CB_SelectItem (107, 2);
         break;
      case 9600L:
         CB_SelectItem (107, 3);
         break;
      case 14400L:
         CB_SelectItem (107, 4);
         break;
      case 28800L:
         CB_SelectItem (107, 5);
         break;
      case 33600L:
         CB_SelectItem (107, 6);
         break;
      case 56000L:
         CB_SelectItem (107, 7);
         break;
      case 64000L:
         CB_SelectItem (107, 8);
         break;
      case 128000L:
         CB_SelectItem (107, 9);
         break;
      case 256000L:
         CB_SelectItem (107, 10);
         break;
      default:
         CB_SelectItem (107, 1);
         break;
   }
   SPBM_SetCurrentValue (109, Limits->DownloadRatio);
   SPBM_SetCurrentValue (111, Limits->RatioStart);
   SPBM_SetCurrentValue (113, Limits->DownloadLimit);
   SPBM_SetCurrentValue (115, Limits->DownloadAt2400);
   SPBM_SetCurrentValue (117, Limits->DownloadAt9600);
   SPBM_SetCurrentValue (119, Limits->DownloadAt14400);
   SPBM_SetCurrentValue (121, Limits->DownloadAt28800);
   SPBM_SetCurrentValue (123, Limits->DownloadAt33600);
   SPBM_SetCurrentValue (126, Limits->CallTimeLimit);
   SPBM_SetCurrentValue (128, Limits->DayTimeLimit);
   SPBM_SetCurrentValue (132, Limits->Level);
   for (i = 133, Test = 0x80000000L; i <= 164; i++, Test >>= 1)
      BM_SetCheck (i, (Limits->Flags & Test) ? TRUE : FALSE);
   for (i = 166, Test = 0x80000000L; i <= 197; i++, Test >>= 1)
      BM_SetCheck (i, (Limits->Flags & Test) ? TRUE : FALSE);

   SetFocus (102);
}

VOID CLimitsDlg::OnOK (VOID)
{
   USHORT i;
   CHAR Temp[64];
   ULONG Test;
   class TUser *User;

   strcpy (Temp, Limits->Key);
   GetDlgItemText (102, GetDlgItemTextLength (102), Limits->Key);
   GetDlgItemText (104, GetDlgItemTextLength (104), Limits->Description);
   GetDlgItemText (107, GetDlgItemTextLength (107), Temp);
   Limits->DownloadSpeed = atol (Temp);
   Limits->DownloadRatio = (USHORT)SPBM_QueryValue (109);
   Limits->RatioStart = (USHORT)SPBM_QueryValue (111);
   Limits->DownloadLimit = (USHORT)SPBM_QueryValue (113);
   Limits->DownloadAt2400 = (USHORT)SPBM_QueryValue (115);
   Limits->DownloadAt9600 = (USHORT)SPBM_QueryValue (117);
   Limits->DownloadAt14400 = (USHORT)SPBM_QueryValue (119);
   Limits->DownloadAt28800 = (USHORT)SPBM_QueryValue (121);
   Limits->DownloadAt33600 = (USHORT)SPBM_QueryValue (123);
   Limits->CallTimeLimit = (USHORT)SPBM_QueryValue (126);
   Limits->DayTimeLimit = (USHORT)SPBM_QueryValue (128);
   Limits->Level = (USHORT)SPBM_QueryValue (132);
   Limits->Flags = 0L;
   for (i = 133, Test = 0x80000000L; i <= 164; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Limits->Flags |= Test;
   }
   Limits->DenyFlags = 0L;
   for (i = 166, Test = 0x80000000L; i <= 197; i++, Test >>= 1) {
      if (BM_QueryCheck (i) == TRUE)
         Limits->DenyFlags |= Test;
   }

   Limits->Update ();

   if (stricmp (Temp, Limits->Key)) {
      if ((User = new TUser (Cfg->UserFile)) != NULL) {
         User->ChangeLimitClass (Temp, Limits->Key);
         delete User;
      }
   }
}

VOID CLimitsDlg::Delete (VOID)
{
   if (MessageBox ("Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
      Limits->Delete ();
      DisplayData ();
   }
}

VOID CLimitsDlg::Previous (VOID)
{
   if (Limits->Previous () == TRUE)
      DisplayData ();
}

VOID CLimitsDlg::Next (VOID)
{
   if (Limits->Next () == TRUE)
      DisplayData ();
}

VOID CLimitsDlg::Add (VOID)
{
   Limits->New ();
   Limits->Add ();

   DisplayData ();
}

// ----------------------------------------------------------------------
// General BBS configuration
// ----------------------------------------------------------------------

CBBSGeneralDlg::CBBSGeneralDlg (HWND p_hWnd) : CDialog ("22", p_hWnd)
{
}

VOID CBBSGeneralDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 22);
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
   EM_SetTextLimit (25, sizeof (Cfg->EditorCmd) - 1);
   SPBM_SetLimits (131, 255L, 1L);

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
   BM_SetCheck (23, Cfg->ExternalEditor);
   SetDlgItemText (25, Cfg->EditorCmd);
   SPBM_SetCurrentValue (131, Cfg->MailBoard);

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

   Cfg->ExternalEditor = (UCHAR)BM_QueryCheck (23);
   GetDlgItemText (25, GetDlgItemTextLength (25), Cfg->EditorCmd);
   Cfg->MailBoard = (USHORT)SPBM_QueryValue (131);

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

VOID CUserDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 17);
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
   BM_SetCheck (18, Data->ScreenClear);
   BM_SetCheck (19, Data->IBMChars);
   BM_SetCheck (21, Data->InUserList);
   BM_SetCheck (20, Data->MorePrompt);
   BM_SetCheck (22, Data->FullEd);
   BM_SetCheck (23, Data->FullReader);
   BM_SetCheck (24, Data->FullScreen);
   BM_SetCheck (25, Data->MailCheck);
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
   Data->ScreenClear = (CHAR)BM_QueryCheck (18);
   Data->IBMChars = (CHAR)BM_QueryCheck (19);
   Data->InUserList = (CHAR)BM_QueryCheck (21);
   Data->MorePrompt = (CHAR)BM_QueryCheck (20);
   Data->FullEd = (CHAR)BM_QueryCheck (22);
   Data->FullReader = (CHAR)BM_QueryCheck (23);
   Data->FullScreen = (CHAR)BM_QueryCheck (24);
   Data->MailCheck = (CHAR)BM_QueryCheck (25);
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

   Data->Level = (USHORT)SPBM_QueryValue (102);

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

VOID COfflineDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 42);
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
   Cfg->OLRMaxMessages = (USHORT)SPBM_QueryValue (106);

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

VOID CProtocolDlg::OnHelp (VOID)
{
   WinHelp ("lora.hlp>h_ref", 44);
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
   Data->FileNamePos = (USHORT)SPBM_QueryValue (132);
   Data->SizePos = (USHORT)SPBM_QueryValue (134);
   Data->CpsPos = (USHORT)SPBM_QueryValue (136);
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

