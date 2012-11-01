
#include "_ldefs.h"
#include "lorawin.h"
#include "msgbase.h"
#include "uulib.h"

class CMsgHeaderDlg : public CDialog
{
public:
   CMsgHeaderDlg (HWND p_hWnd);

   CHAR   From[48], To[48];
   CHAR   Subject[64];
   CHAR   FromAddress[64];
   CHAR   ToAddress[64];
   UCHAR  Crash, Direct, FileAttach, FileRequest, Hold, Immediate;
   UCHAR  Intransit, KillSent, Local, Private, ReceiptRequest, Received;
   UCHAR  Sent;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

#if defined(__OS2__)
HAB  hab;
HWND hwndReaderFrame, hwndReaderClient;
FILEDLG fild;
#elif defined(__NT__)
HINSTANCE hinst;
HWND hwndReaderClient;
#endif
HACCEL hAccel, hAccReader, hAccEditor;

#define STATIC_HEIGHT      18

#define ID_SUBJECT         1108

PSZ Months[] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

USHORT DoRescan = TRUE, Status = 0, ShowKludges = FALSE;
CHAR   AreaKey[16];
ULONG  Number;
struct dosdate_t d_date;
struct dostime_t d_time;
class  TMsgBase *Msg = NULL;
class  TScan *Scan = NULL;
class  TConfig *Cfg = NULL;
class  TPMLog *Log = NULL;
class  TModem *Modem = NULL;
class  CMsgHeaderDlg *HeaderDlg = NULL;

// ----------------------------------------------------------------------

typedef struct {
   CHAR   Key[16];
   CHAR   Description[64];
   ULONG  Messages;
   ULONG  New;
   CHAR   Tag[64];
   CHAR   Address[64];
} SCANDATA;

class TScan
{
public:
   TScan (void);
   ~TScan (void);

   CHAR   Key[16];
   CHAR   Description[64];
   ULONG  Messages;
   ULONG  New;
   CHAR   Tag[64];
   CHAR   Address[64];

   VOID   Add (VOID);
   VOID   Clear (VOID);
   USHORT First (VOID);
   USHORT Next (VOID);
   USHORT Read (PSZ key);
   VOID   Update (VOID);

private:
   class  TCollection Data;
};

TScan::TScan (void)
{
   Data.Clear ();
}

TScan::~TScan (void)
{
   Data.Clear ();
}

VOID TScan::Add (VOID)
{
   SCANDATA sd;

   strcpy (sd.Key, Key);
   strcpy (sd.Description, Description);
   sd.Messages = Messages;
   sd.New = New;
   strcpy (sd.Tag, Tag);
   strcpy (sd.Address, Address);

   Data.Add (&sd, sizeof (SCANDATA));
}

VOID TScan::Clear (VOID)
{
   Data.Clear ();
}

USHORT TScan::First (VOID)
{
   USHORT RetVal = FALSE;
   SCANDATA *sd;

   if ((sd = (SCANDATA *)Data.First ()) != NULL) {
      strcpy (Key, sd->Key);
      strcpy (Description, sd->Description);
      Messages = sd->Messages;
      New = sd->New;
      strcpy (Tag, sd->Tag);
      strcpy (Address, sd->Address);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TScan::Next (VOID)
{
   USHORT RetVal = FALSE;
   SCANDATA *sd;

   if ((sd = (SCANDATA *)Data.Next ()) != NULL) {
      strcpy (Key, sd->Key);
      strcpy (Description, sd->Description);
      Messages = sd->Messages;
      New = sd->New;
      strcpy (Tag, sd->Tag);
      strcpy (Address, sd->Address);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TScan::Read (PSZ key)
{
   USHORT RetVal = FALSE;
   SCANDATA *sd;

   if ((sd = (SCANDATA *)Data.First ()) != NULL)
      do {
         if (!stricmp (sd->Key, key)) {
            strcpy (Key, sd->Key);
            strcpy (Description, sd->Description);
            Messages = sd->Messages;
            New = sd->New;
            strcpy (Tag, sd->Tag);
            strcpy (Address, sd->Address);
            RetVal = TRUE;
            break;
         }
      } while ((sd = (SCANDATA *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TScan::Update (VOID)
{
   SCANDATA *sd;

   if ((sd = (SCANDATA *)Data.Value ()) != NULL) {
      sd->Messages = Messages;
      sd->New = New;
   }
}

// ----------------------------------------------------------------------

class CScanDlg : public CDialog
{
public:
   CScanDlg (HWND p_hWnd);
   ~CScanDlg (void);

   VOID   OnCancel (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnUser (VOID);

private:
   USHORT IsUser;
   class  TMsgData *Data;
   class  TMsgBase *Msg;
   class  TUser *User;
   DECLARE_MESSAGE_MAP ()
};

BEGIN_MESSAGE_MAP (CScanDlg, CDialog)
   ON_MESSAGE (WM_USER, OnUser)
END_MESSAGE_MAP ()

CScanDlg::CScanDlg (HWND p_hWnd) : CDialog ("203", p_hWnd)
{
   IsUser = FALSE;
   Data = NULL;
   User = NULL;
}

CScanDlg::~CScanDlg (void)
{
   if (Data != NULL)
      delete Data;
   if (User != NULL)
      delete User;
}

USHORT CScanDlg::OnInitDialog (VOID)
{
   ULONG LastRead;

   Center ();

   if (Scan != NULL)
      Scan->Clear ();

   if ((User = new TUser (Cfg->UserFile)) != NULL) {
     if (User->GetData (Cfg->SysopName) == TRUE)
        IsUser = TRUE;
   }

   Msg = NULL;
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
   }
   if (Msg != NULL) {
      LastRead = 0L;
      if (IsUser == TRUE && User != NULL) {
         if (User->MsgTag->Read ("NetMail") == TRUE)
            LastRead = User->MsgTag->LastRead;
      }

      if (Scan != NULL) {
         strcpy (Scan->Key, "NetMail");
         strcpy (Scan->Description, "FidoNet E-Mail");
         Scan->Messages = Msg->Number ();
         Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
         Scan->Tag[0] = '\0';
         Scan->Address[0] = '\0';
         Scan->Add ();
      }

      delete Msg;
   }

#if !defined(__POINT__)
   Msg = NULL;
   switch (Cfg->MailStorage) {
      case ST_JAM:
         Msg = new JAM (Cfg->MailPath);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Cfg->MailPath);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Cfg->MailPath);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Cfg->MailPath);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->MailBoard);
         break;
   }
   if (Msg != NULL) {
      LastRead = 0L;
      if (IsUser == TRUE && User != NULL) {
         if (User->MsgTag->Read ("EMail") == TRUE)
            LastRead = User->MsgTag->LastRead;
      }

      if (Scan != NULL) {
         strcpy (Scan->Key, "EMail");
         strcpy (Scan->Description, "Personal E-Mail");
         Scan->Messages = Msg->Number ();
         Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
         Scan->Tag[0] = '\0';
         Scan->Address[0] = '\0';
         Scan->Add ();
      }

      delete Msg;
   }
#endif

   Msg = NULL;
   switch (Cfg->BadStorage) {
      case ST_JAM:
         Msg = new JAM (Cfg->BadPath);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Cfg->BadPath);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Cfg->BadPath);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Cfg->BadPath);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->BadBoard);
         break;
   }
   if (Msg != NULL) {
      LastRead = 0L;
      if (IsUser == TRUE && User != NULL) {
         if (User->MsgTag->Read ("BadMsgs") == TRUE)
            LastRead = User->MsgTag->LastRead;
      }

      if (Scan != NULL) {
         strcpy (Scan->Key, "BadMsgs");
         strcpy (Scan->Description, "Bad Messages");
         Scan->Messages = Msg->Number ();
         Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
         Scan->Tag[0] = '\0';
         Scan->Address[0] = '\0';
         Scan->Add ();
      }

      delete Msg;
   }

   Msg = NULL;
   switch (Cfg->DupeStorage) {
      case ST_JAM:
         Msg = new JAM (Cfg->DupePath);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Cfg->DupePath);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Cfg->DupePath);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Cfg->DupePath);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->DupeBoard);
         break;
   }
   if (Msg != NULL) {
      LastRead = 0L;
      if (IsUser == TRUE && User != NULL) {
         if (User->MsgTag->Read ("Dupes") == TRUE)
            LastRead = User->MsgTag->LastRead;
      }

      if (Scan != NULL) {
         strcpy (Scan->Key, "Dupes");
         strcpy (Scan->Description, "Duplicate Messages");
         Scan->Messages = Msg->Number ();
         Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
         Scan->Tag[0] = '\0';
         Scan->Address[0] = '\0';
         Scan->Add ();
      }

      delete Msg;
   }

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
#if defined(__OS2__)
         WinPostMsg (m_hWnd, WM_USER, 0L, 0L);
#elif defined(__NT__)
         PostMessage (m_hWnd, WM_USER, 0, 0L);
#endif
      else
         EndDialog (TRUE);
   }

   return (TRUE);
}

VOID CScanDlg::OnUser (VOID)
{
   ULONG LastRead;

   SetDlgItemText (101, Data->Display);

   Msg = NULL;
   switch (Data->Storage) {
      case ST_JAM:
         Msg = new JAM (Data->Path);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Data->Path);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Data->Path);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Data->Path);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Data->Path, (UCHAR)Data->Board);
         break;
   }

   if (Msg != NULL) {
      LastRead = 0L;
      if (IsUser == TRUE && User != NULL) {
         if (User->MsgTag->Read (Data->Key) == TRUE)
            LastRead = User->MsgTag->LastRead;
      }

      if (Scan != NULL) {
         strcpy (Scan->Key, Data->Key);
         strcpy (Scan->Description, Data->Display);
         Scan->Messages = Msg->Number ();
         Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
         strcpy (Scan->Tag, Data->EchoTag);
         strcpy (Scan->Address, Data->Address);
         Scan->Add ();
      }

      delete Msg;
   }

   if (Data != NULL) {
      if (Data->Next () == TRUE) {
#if defined(__OS2__)
         WinPostMsg (m_hWnd, WM_USER, 0L, 0L);
#elif defined(__NT__)
         PostMessage (m_hWnd, WM_USER, 0, 0L);
#endif
      }
      else
         EndDialog (TRUE);
   }
}

VOID CScanDlg::OnCancel (VOID)
{
   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

CMsgHeaderDlg::CMsgHeaderDlg (HWND p_hWnd) : CDialog ("204", p_hWnd)
{
   Crash = Direct = FileAttach = FileRequest = Hold = Immediate = FALSE;
   Intransit = KillSent = Local = Private = ReceiptRequest = Received = FALSE;
   Sent = FALSE;
}

USHORT CMsgHeaderDlg::OnInitDialog (VOID)
{
   Center ();

   SetDlgItemText (102, From);
   SetDlgItemText (108, FromAddress);
   SetDlgItemText (104, To);
   SetDlgItemText (110, ToAddress);
   SetDlgItemText (106, Subject);

   BM_SetCheck (109, Crash);
   BM_SetCheck (113, FileAttach);
   BM_SetCheck (117, FileRequest);
   BM_SetCheck (115, Hold);
   BM_SetCheck (114, Intransit);
   BM_SetCheck (123, KillSent);
   BM_SetCheck (111, Local);
   BM_SetCheck (107, Private);
   BM_SetCheck (120, ReceiptRequest);
   BM_SetCheck (121, Received);
   BM_SetCheck (112, Sent);

   SetFocus (104);

   return (TRUE);
}

VOID CMsgHeaderDlg::OnOK (VOID)
{
   GetDlgItemText (102, GetDlgItemTextLength (102), From);
   GetDlgItemText (108, GetDlgItemTextLength (108), FromAddress);
   GetDlgItemText (104, GetDlgItemTextLength (104), To);
   GetDlgItemText (110, GetDlgItemTextLength (110), ToAddress);
   GetDlgItemText (106, GetDlgItemTextLength (106), Subject);

   Crash = (UCHAR)BM_QueryCheck (109);
   FileAttach = (UCHAR)BM_QueryCheck (113);
   FileRequest = (UCHAR)BM_QueryCheck (117);
   Hold = (UCHAR)BM_QueryCheck (115);
   Intransit = (UCHAR)BM_QueryCheck (114);
   KillSent = (UCHAR)BM_QueryCheck (123);
   Local = (UCHAR)BM_QueryCheck (111);
   Private = (UCHAR)BM_QueryCheck (107);
   ReceiptRequest = (UCHAR)BM_QueryCheck (120);
   Received = (UCHAR)BM_QueryCheck (121);
   Sent = (UCHAR)BM_QueryCheck (112);

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

class CAreaListDlg : public CDialog
{
public:
   CAreaListDlg (HWND p_hWnd);

   USHORT NetMail;
   USHORT EMail;
   USHORT BadMsgs;
   USHORT Dupes;
   class  TMsgData *Data;

   VOID   OnChanged (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   int    toSelect;
   CHAR   Temp[128];
   DECLARE_MESSAGE_MAP ()
};

BEGIN_MESSAGE_MAP (CAreaListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
END_MESSAGE_MAP ()

CAreaListDlg::CAreaListDlg (HWND p_hWnd) : CDialog ("201", p_hWnd)
{
   toSelect = -1;
   NetMail = FALSE;
   BadMsgs = FALSE;
   Dupes = FALSE;
   EMail = FALSE;
}

USHORT CAreaListDlg::OnInitDialog (VOID)
{
   int i;
   struct stat statbuf;
   class CScanDlg *Dlg;

   Center ();

   sprintf (Temp, "%sarealist.rsn", Cfg->SystemPath);
   if (stat (Temp, &statbuf) == 0) {
      DoRescan = TRUE;
      unlink (Temp);
   }

   if (DoRescan == TRUE || Scan == NULL) {
      if (Scan == NULL)
         Scan = new TScan;
      if (Scan != NULL) {
         if ((Dlg = new CScanDlg (m_hWnd)) != NULL) {
            Dlg->DoModal ();
            delete Dlg;
         }
         DoRescan = FALSE;
      }
   }

   LVM_AllocateColumns (101, 4);
   LVM_InsertColumn (101, "Key", LVC_LEFT);
   LVM_InsertColumn (101, "Msgs.", LVC_RIGHT);
   LVM_InsertColumn (101, "New", LVC_RIGHT);
   LVM_InsertColumn (101, "Description", LVC_LEFT);

   i = 0;
   if (Scan->First () == TRUE)
      do {
         LVM_InsertItem (101);

         LVM_SetItemText (101, 0, Scan->Key);
         sprintf (Temp, "%lu", Scan->Messages);
         LVM_SetItemText (101, 1, Temp);
         sprintf (Temp, "%lu", Scan->New);
         LVM_SetItemText (101, 2, Temp);
         LVM_SetItemText (101, 3, Scan->Description);

         if (!stricmp (AreaKey, Scan->Key))
            toSelect = i;
         i++;
      } while (Scan->Next () == TRUE);

   LVM_InvalidateView (101);
   if (toSelect != -1)
      LVM_SelectItem (101, toSelect);

   return (TRUE);
}

VOID CAreaListDlg::OnOK (VOID)
{
   int item;
   CHAR Temp[32];

   if ((item = LVM_QuerySelectedItem (101)) != -1) {
      LVM_QueryItemText (101, item, 0, Temp);

      if (Scan != NULL)
         Scan->Read (Temp);
      if (!stricmp (Temp, "NetMail"))
         NetMail = TRUE;
      else if (!stricmp (Temp, "EMail"))
         EMail = TRUE;
      else if (!stricmp (Temp, "BadMsgs"))
         BadMsgs = TRUE;
      else if (!stricmp (Temp, "Dupes"))
         Dupes = TRUE;
      else
         Data->Read (Temp, FALSE);
   }

   EndDialog (TRUE);
}

// ----------------------------------------------------------------------

class CMsgListDlg : public CDialog
{
public:
   CMsgListDlg (HWND p_hWnd);

   ULONG  NewNumber;
   ULONG  OldNumber;
   class  TMsgBase *Msg;

   VOID   OnCancel (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   int    Selected;
   DECLARE_MESSAGE_MAP ()
};

BEGIN_MESSAGE_MAP (CMsgListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
END_MESSAGE_MAP ()

CMsgListDlg::CMsgListDlg (HWND p_hWnd) : CDialog ("202", p_hWnd)
{
   Selected = -1;
}

USHORT CMsgListDlg::OnInitDialog (VOID)
{
   CHAR Temp[32];
   USHORT i, ToSelect = 0;
   ULONG ListNumber;

   Center ();

   LVM_AllocateColumns (101, 4);
   LVM_InsertColumn (101, "Num,", LVC_RIGHT);
   LVM_InsertColumn (101, "From", LVC_LEFT);
   LVM_InsertColumn (101, "To", LVC_LEFT);
   LVM_InsertColumn (101, "Subject", LVC_LEFT);

   Msg->Lock (0L);
   ListNumber = Msg->Lowest ();
   i = 0;

   do {
      if (Msg->ReadHeader (ListNumber) == TRUE) {
         LVM_InsertItem (101);

         sprintf (Temp, "%lu", Msg->UidToMsgn (ListNumber));
         LVM_SetItemText (101, 0, Temp);
         LVM_SetItemText (101, 1, Msg->From);
         LVM_SetItemText (101, 2, Msg->To);
         LVM_SetItemText (101, 3, Msg->Subject);

         if (ListNumber == Number)
            ToSelect = i;
         i++;
      }
   } while (Msg->Next (ListNumber) == TRUE);

   LVM_InvalidateView (101);
   LVM_SelectItem (101, ToSelect);

   Msg->UnLock ();

   return (TRUE);
}

VOID CMsgListDlg::OnOK (VOID)
{
   CHAR Temp[32];

   LVM_QueryItemText (101, LVM_QuerySelectedItem (101), 0, Temp);
   NewNumber = Msg->MsgnToUid (atol (Temp));

   EndDialog (TRUE);
}

VOID CMsgListDlg::OnCancel (VOID)
{
   Msg->Read (OldNumber);

   EndDialog (FALSE);
}

// ----------------------------------------------------------------------
// Product informations dialog
// ----------------------------------------------------------------------

class CProductDlg : public CDialog
{
public:
   CProductDlg (HWND p_hWnd);

   USHORT OnInitDialog (VOID);
};

CProductDlg::CProductDlg (HWND p_hWnd) : CDialog ("100", p_hWnd)
{
}

USHORT CProductDlg::OnInitDialog (VOID)
{
   CHAR Temp[128], RegName[64], RegNumber[16];

   Center ();

#if defined(__POINT__)
   if (ValidateKey ("point", RegName, RegNumber) != KEY_UNREGISTERED) {
#else
   if (ValidateKey ("bbs", RegName, RegNumber) != KEY_UNREGISTERED) {
#endif
      sprintf (Temp, "Registered to %s", RegName);
      SetDlgItemText (104, Temp);
      sprintf (Temp, "Serial Number %s", RegNumber);
      SetDlgItemText (105, Temp);
   }

   return (TRUE);
}

// ----------------------------------------------------------------------

VOID ParseAddress (PSZ text, PSZ name, PSZ address)
{
   CHAR Temp[128], *p, *a;

   strcpy (Temp, text);
   if (strchr (Temp, '(') != NULL) {
      if ((p = strtok (Temp, " ")) != NULL) {
         p = strtok (NULL, "");
         while (*p == ' ')
            p++;
         if (*p == '(') {
            strcpy (Temp, ++p);
            p = strchr (Temp, '\0');
            while (--p > Temp) {
               if (*p == ')') {
                  *p = '\0';
                  break;
               }
            }
            if (name != NULL)
               strcpy (name, Temp);
            strcpy (Temp, text);
            if ((p = strtok (Temp, " ")) != NULL) {
               if (address != NULL)
                  strcpy (address, p);
            }
         }
         else {
            strcpy (Temp, text);
            if ((p = strtok (Temp, " ")) != NULL) {
               if (name != NULL)
                  strcpy (name, p);
            }
         }
      }
   }
   else if ((p = strchr (Temp, '<')) != NULL) {
      *p++ = '\0';
      if ((a = strchr (p, '>')) != NULL)
         *a = '\0';
      if (address != NULL)
         strcpy (address, p);
      p = Temp;
      while (*p == ' ')
         p++;
      if (*p == '"')
         strcpy (Temp, ++p);
      p = strchr (Temp, '\0');
      while (--p > Temp) {
         if (*p != ' ' && *p != '"')
            break;
         *p = '\0';
      }
      if (name != NULL) {
         strcpy (name, Temp);
         if (address != NULL) {
            if (*name == '\0' && *address != '\0') {
               strcpy (name, address);
               *address = '\0';
            }
         }
      }
   }
   else if (strchr (Temp, '@') != NULL) {
      if ((p = strtok (Temp, " ,")) != NULL) {
         if (address != NULL)
            strcpy (address, p);
      }
   }
}

VOID DisplayMessage (HWND hwnd)
{
   CHAR *p, Temp[128], *Buffer, *a;
   USHORT gotFrom = FALSE, gotTo = FALSE;
   ULONG bytes, Msgn;
   class TAddress Address;
#if defined(__OS2__)
   ULONG Value;

   WinSetDlgItemText (hwnd, 1113, "");
   WinSetDlgItemText (hwnd, 1114, "");
   WinSetDlgItemText (hwnd, 1106, "");
   WinSetDlgItemText (hwnd, 1109, "");
   WinSetDlgItemText (hwnd, 1111, "");
   WinSetDlgItemText (hwnd, 1107, "");
   WinSetDlgItemText (hwnd, 1110, "");
   WinSetDlgItemText (hwnd, 1112, "");
   WinSetDlgItemText (hwnd, 1108, "");

   WinSetDlgItemText (hwnd, 1105, "");
#elif defined(__NT__)
   SetWindowText (GetDlgItem (hwnd, 1113), "");
   SetWindowText (GetDlgItem (hwnd, 1114), "");
   SetWindowText (GetDlgItem (hwnd, 1106), "");
   SetWindowText (GetDlgItem (hwnd, 1109), "");
   SetWindowText (GetDlgItem (hwnd, 1111), "");
   SetWindowText (GetDlgItem (hwnd, 1107), "");
   SetWindowText (GetDlgItem (hwnd, 1110), "");
   SetWindowText (GetDlgItem (hwnd, 1112), "");
   SetWindowText (GetDlgItem (hwnd, 1108), "");

   SetWindowText (GetDlgItem (hwnd, 1105), "");
#endif

   if (Msg != NULL) {
      if (Msg->Read (Number, 80) == FALSE) {
         Msg->New ();
         Msgn = 0L;
      }
      else
         Msgn = Msg->UidToMsgn (Number);

      if (Scan != NULL) {
         Scan->New = Msg->Number () - Msgn;
         Scan->Update ();
      }

      Cfg->MailAddress.First ();
      Address.Parse (Msg->FromAddress);
      if (Address.Zone == 0)
         Address.Zone = Cfg->MailAddress.Zone;
      if (Address.Net == 0)
         Address.Net = Cfg->MailAddress.Net;
      Address.Add ();
      Address.First ();
      strcpy (Msg->FromAddress, Address.String);
      Address.Clear ();

      Cfg->MailAddress.First ();
      Address.Parse (Msg->ToAddress);
      if (Address.Zone == 0)
         Address.Zone = Cfg->MailAddress.Zone;
      if (Address.Net == 0)
         Address.Net = Cfg->MailAddress.Net;
      Address.Add ();
      Address.First ();
      strcpy (Msg->ToAddress, Address.String);
      Address.Clear ();

      if ((p = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (!strncmp (p, " * Origin: ", 11)) {
               Msg->ToAddress[0] = '\0';

               strcpy (Temp, &p[11]);
               p = strchr (Temp, '\0');
               while (--p > Temp) {
                  if (*p != ' ' && *p != ')')
                     break;
                  *p = '\0';
               }
               if (p > Temp) {
                  while (--p > Temp) {
                     if (*p == '(' || *p == ' ')
                        break;
                  }
               }
               if (*p == '(' || *p == ' ')
                  p++;
               strcpy (Msg->FromAddress, p);
               break;
            }
            else if (!strncmp (p, "\001MSGID: ", 8)) {
               strcpy (Temp, &p[8]);
               if ((p = strtok (Temp, " ")) != NULL) {
                  if (strchr (p, ':') != NULL && strchr (p, '/') != NULL) {
                     Msg->ToAddress[0] = '\0';
                     strcpy (Msg->FromAddress, p);
                  }
               }
               break;
            }
            else if (!strncmp (p, "\001From: ", 7)) {
               Msg->FromAddress[0] = '\0';
               if (gotTo == FALSE)
                  Msg->ToAddress[0] = '\0';
               ParseAddress (&p[7], Msg->From, Msg->FromAddress);
               gotFrom = TRUE;
            }
            else if (!strncmp (p, "\001To: ", 5)) {
               if (gotFrom == FALSE)
                  Msg->FromAddress[0] = '\0';
               Msg->ToAddress[0] = '\0';
               ParseAddress (&p[5], Msg->To, Msg->ToAddress);
               gotTo = TRUE;
            }
            if (gotFrom == TRUE && gotTo == TRUE)
               break;
         } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

#if defined(__OS2__)
      sprintf (Temp, "%lu of %lu (%lu left)", Msgn, Msg->Number (), Msg->Number () - Msgn);
      if (Msg->Original != 0L)
         sprintf (&Temp[strlen (Temp)], " -%lu", Msg->UidToMsgn (Msg->Original));
      if (Msg->Reply != 0L)
         sprintf (&Temp[strlen (Temp)], " +%lu", Msg->UidToMsgn (Msg->Reply));
      WinSetDlgItemText (hwnd, 1113, Temp);
      Temp[0] = '\0';
      if (Msg->Received == TRUE)
         strcat (Temp, "Rcv ");
      if (Msg->Sent == TRUE)
         strcat (Temp, "Snt ");
      if (Msg->Private == TRUE)
         strcat (Temp, "Pvt ");
      if (Msg->Crash == TRUE)
         strcat (Temp, "Cra ");
      if (Msg->KillSent == TRUE)
         strcat (Temp, "K/s ");
      if (Msg->Local == TRUE)
         strcat (Temp, "Loc ");
      if (Msg->Hold == TRUE)
         strcat (Temp, "Hld ");
      if (Msg->FileAttach == TRUE)
         strcat (Temp, "Att ");
      if (Msg->FileRequest == TRUE)
         strcat (Temp, "Frq ");
      if (Msg->Intransit == TRUE)
         strcat (Temp, "Trs ");
      WinSetDlgItemText (hwnd, 1114, Temp);
      WinSetDlgItemText (hwnd, 1106, Msg->From);
      WinSetDlgItemText (hwnd, 1109, Msg->FromAddress);
      if (Msgn != 0L) {
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Written.Day, Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Second);
         WinSetDlgItemText (hwnd, 1111, Temp);
      }
      WinSetDlgItemText (hwnd, 1107, Msg->To);
      if (!stricmp (Msg->To, Cfg->SysopName)) {
         Value = 0xF00000L;
         WinSetPresParam (WinWindowFromID (hwnd, 1107), PP_FOREGROUNDCOLOR, 4, &Value);
      }
      else {
         Value = 0x000000L;
         WinSetPresParam (WinWindowFromID (hwnd, 1107), PP_FOREGROUNDCOLOR, 4, &Value);
      }
      WinSetDlgItemText (hwnd, 1110, Msg->ToAddress);
      if (Msgn != 0L) {
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Arrived.Day, Months[Msg->Arrived.Month - 1], Msg->Arrived.Year, Msg->Arrived.Hour, Msg->Arrived.Minute, Msg->Arrived.Second);
         WinSetDlgItemText (hwnd, 1112, Temp);
      }
      WinSetDlgItemText (hwnd, 1108, Msg->Subject);
#elif defined(__NT__)
      sprintf (Temp, "%lu of %lu (%lu left)", Msgn, Msg->Number (), Msg->Number () - Msgn);
      SetWindowText (GetDlgItem (hwnd, 1113), Temp);
      Temp[0] = '\0';
      if (Msg->Received == TRUE)
         strcat (Temp, "Rcv ");
      if (Msg->Sent == TRUE)
         strcat (Temp, "Snt ");
      if (Msg->Private == TRUE)
         strcat (Temp, "Pvt ");
      if (Msg->Crash == TRUE)
         strcat (Temp, "Cra ");
      if (Msg->KillSent == TRUE)
         strcat (Temp, "K/s ");
      if (Msg->Local == TRUE)
         strcat (Temp, "Loc ");
      if (Msg->Hold == TRUE)
         strcat (Temp, "Hld ");
      if (Msg->FileAttach == TRUE)
         strcat (Temp, "Att ");
      if (Msg->FileRequest == TRUE)
         strcat (Temp, "Frq ");
      if (Msg->Intransit == TRUE)
         strcat (Temp, "Trs ");
      SetWindowText (GetDlgItem (hwnd, 1114), Temp);
      SetWindowText (GetDlgItem (hwnd, 1106), Msg->From);
      SetWindowText (GetDlgItem (hwnd, 1109), Msg->FromAddress);
      if (Msgn != 0L) {
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Written.Day, Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Second);
         SetWindowText (GetDlgItem (hwnd, 1111), Temp);
      }
      SetWindowText (GetDlgItem (hwnd, 1107), Msg->To);
      SetWindowText (GetDlgItem (hwnd, 1110), Msg->ToAddress);
      if (Msgn != 0L) {
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Arrived.Day, Months[Msg->Arrived.Month - 1], Msg->Arrived.Year, Msg->Arrived.Hour, Msg->Arrived.Minute, Msg->Arrived.Second);
         SetWindowText (GetDlgItem (hwnd, 1112), Temp);
      }
      SetWindowText (GetDlgItem (hwnd, 1108), Msg->Subject);
#endif

      bytes = 0L;
      if ((p = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if ((*p != 0x01 && strncmp (p, "SEEN-BY:", 8)) || ShowKludges == TRUE)
#if defined(__OS2__)
               bytes += strlen (p) + 1;
#elif defined(__NT__)
               bytes += strlen (p) + 2;
#endif
         } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

      if ((Buffer = (CHAR *)malloc (bytes + 1)) != NULL) {
         *Buffer = '\0';
         a = Buffer;
         if ((p = (CHAR *)Msg->Text.First ()) != NULL)
            do {
               if ((*p != 0x01 && strncmp (p, "SEEN-BY:", 8)) || ShowKludges == TRUE) {
                  strcpy (a, p);
                  a = strchr (a, '\0');
#if defined(__OS2__)
                  strcpy (a, "\n");
#elif defined(__NT__)
                  strcpy (a, "\r\n");
#endif
                  a = strchr (a, '\0');
               }
            } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

#if defined(__OS2__)
         WinSetDlgItemText (hwnd, 1105, Buffer);
#elif defined(__NT__)
         SetWindowText (GetDlgItem (hwnd, 1105), Buffer);
#endif
         free (Buffer);
      }

      if (!stricmp (Msg->To, Cfg->SysopName) && Msg->Received == FALSE) {
         Msg->Received = TRUE;
         Msg->WriteHeader (Number);
#if defined(__OS2__)
         WinAlarm (HWND_DESKTOP, WA_NOTE);
#endif
      }

#if defined(__OS2__)
      WinSendDlgItemMsg (hwnd, 1105, MLM_SETREADONLY, MPFROMSHORT (TRUE), 0L);
      WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 1105));
#elif defined(__NT__)
      SendDlgItemMessage (hwnd, 1105, EM_SETREADONLY, (WPARAM)TRUE, 0L);
      SetFocus (GetDlgItem (hwnd, 1105));
#endif

      hAccel = hAccReader;

#if defined(__OS2__)
      WinEnableControl (hwnd, 1201, TRUE);
      WinEnableControl (hwnd, 1202, TRUE);
      WinEnableControl (hwnd, 1203, TRUE);
      WinEnableControl (hwnd, 1204, TRUE);
      WinEnableControl (hwnd, 1205, TRUE);
      WinEnableControl (hwnd, 1206, TRUE);
      WinEnableControl (hwnd, 1207, TRUE);
      WinEnableControl (hwnd, 1208, FALSE);
      WinEnableControl (hwnd, 1209, FALSE);
#elif defined(__NT__)
      EnableWindow (GetDlgItem (hwnd, 1201), TRUE);
      EnableWindow (GetDlgItem (hwnd, 1202), TRUE);
      EnableWindow (GetDlgItem (hwnd, 1203), TRUE);
      EnableWindow (GetDlgItem (hwnd, 1204), TRUE);
      EnableWindow (GetDlgItem (hwnd, 1205), TRUE);
      EnableWindow (GetDlgItem (hwnd, 1206), TRUE);
      EnableWindow (GetDlgItem (hwnd, 1207), TRUE);
      EnableWindow (GetDlgItem (hwnd, 1208), FALSE);
      EnableWindow (GetDlgItem (hwnd, 1209), FALSE);
#endif
   }
}

VOID GetOrigin (class TMsgData *Data, PSZ Origin)
{
   FILE *fp;
   int i, max;
   CHAR Temp[128];

   strcpy (Origin, Cfg->SystemName);
   if (Data->Origin[0] != '\0')
      strcpy (Origin, Data->Origin);
   else if (Data->OriginIndex == OIDX_DEFAULT)
      strcpy (Origin, Cfg->SystemName);
   else if (Data->OriginIndex == OIDX_RANDOM) {
      srand ((unsigned int)time (NULL));
      sprintf (Temp, "%sorigin.txt", Cfg->SystemPath);
      if ((fp = fopen (Temp, "rt")) != NULL) {
         max = 0;
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL)
            max++;
         while ((i = rand ()) > max)
            ;
         fseek (fp, 0L, SEEK_SET);
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            if (i == 0) {
               if (Temp[strlen (Temp) - 1] == '\n')
                  Temp[strlen (Temp) - 1] = '\0';
               strcpy (Origin, Temp);
               break;
            }
            i--;
         }
         fclose (fp);
      }
   }
   else {
      i = 1;
      sprintf (Temp, "%sorigin.txt", Cfg->SystemPath);
      if ((fp = fopen (Temp, "rt")) != NULL) {
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            if (i == Data->OriginIndex) {
               if (Temp[strlen (Temp) - 1] == '\n')
                  Temp[strlen (Temp) - 1] = '\0';
               strcpy (Origin, Temp);
               break;
            }
         }
         fclose (fp);
      }
   }
}

VOID EditMessage (HWND hwnd, USHORT Reply, USHORT DoQuote)
{
   USHORT i, Continue = FALSE, gotReplyTo = FALSE;
   CHAR *p, Quote[16], Temp[128], *Buffer;
   CHAR Header[128], Footer[128], FidoAddress[64], Origin[128];
   ULONG bytes;
#if defined(__OS2__)
   IPT ipt = 0L, cursor = 0L;
   ULONG Value;
#endif
   class TMsgData *Data;

   if (Reply == TRUE) {
      if (Msg->ReadHeader (Number) == FALSE) {
         Msg->New ();
         Reply = FALSE;
      }
   }

   _dos_getdate (&d_date);
   _dos_gettime (&d_time);

   if (Msg != NULL && Reply == TRUE) {
      if (Msg->Read (Number, 72) == TRUE) {
         strcpy (FidoAddress, Msg->FromAddress);
         if ((p = (CHAR *)Msg->Text.First ()) != NULL)
            do {
               if (!strncmp (p, " * Origin: ", 11)) {
                  Msg->ToAddress[0] = '\0';

                  strcpy (Temp, &p[11]);
                  p = strchr (Temp, '\0');
                  while (--p > Temp) {
                     if (*p != ' ' && *p != ')')
                        break;
                     *p = '\0';
                  }
                  if (p > Temp) {
                     while (--p > Temp) {
                        if (*p == '(' || *p == ' ')
                           break;
                     }
                  }
                  if (*p == '(' || *p == ' ')
                     p++;
                  strcpy (Msg->FromAddress, p);
                  strcpy (FidoAddress, p);
                  break;
               }
               else if (!strncmp (p, "\001MSGID: ", 8)) {
                  strcpy (Temp, &p[8]);
                  if ((p = strtok (Temp, " ")) != NULL) {
                     if (strchr (p, ':') != NULL && strchr (p, '/') != NULL) {
                        Msg->ToAddress[0] = '\0';
                        strcpy (Msg->FromAddress, p);
                        strcpy (FidoAddress, p);
                     }
                  }
                  break;
               }
               else if (!strncmp (p, "\001From: ", 7)) {
                  Msg->ToAddress[0] = '\0';
                  if (gotReplyTo == TRUE)
                     ParseAddress (&p[7], Msg->From, NULL);
                  else {
                     Msg->FromAddress[0] = '\0';
                     ParseAddress (&p[7], Msg->From, Msg->FromAddress);
                  }
               }
               else if (!strncmp (p, "\001Reply-To: ", 11)) {
                  Temp[0] = '\0';
                  ParseAddress (&p[11], Temp, Msg->FromAddress);
                  gotReplyTo = TRUE;
               }
            } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

         Cfg->MailAddress.First ();
         if ((HeaderDlg = new CMsgHeaderDlg (hwnd)) != NULL) {
            strcpy (HeaderDlg->From, Cfg->SysopName);
            if (Scan->Tag[0] == '\0' || Scan->Address[0] == '\0')
               strcpy (HeaderDlg->FromAddress, Cfg->MailAddress.String);
            else
               strcpy (HeaderDlg->FromAddress, Scan->Address);
            strcpy (HeaderDlg->ToAddress, FidoAddress);
            if (strchr (Msg->FromAddress, '@') != NULL && strchr (Msg->FromAddress, '/') == NULL && strchr (Msg->FromAddress, ':') == NULL)
               strcpy (HeaderDlg->To, Msg->FromAddress);
            else
               strcpy (HeaderDlg->To, Msg->From);

            strcpy (Temp, Msg->Subject);
            if (strncmp (strupr (Temp), "RE:", 3))
               sprintf (HeaderDlg->Subject, "Re: %s", Msg->Subject);
            else
               strcpy (HeaderDlg->Subject, Msg->Subject);

            HeaderDlg->Private = Msg->Private;
            HeaderDlg->Local = TRUE;

            if ((Continue = (USHORT)HeaderDlg->DoModal ()) == TRUE) {
#if defined(__OS2__)
               WinSetDlgItemText (hwnd, 1106, HeaderDlg->From);
               WinSetDlgItemText (hwnd, 1109, HeaderDlg->FromAddress);
               sprintf (Temp, "%02d %s %d %2d:%02d:%02d", d_date.day, Months[d_date.month - 1], d_date.year, d_time.hour, d_time.minute, d_time.second);
               WinSetDlgItemText (hwnd, 1111, Temp);
               WinSetDlgItemText (hwnd, 1107, HeaderDlg->To);
               WinSetDlgItemText (hwnd, 1110, HeaderDlg->ToAddress);
               WinSetDlgItemText (hwnd, 1112, "");
               WinSetDlgItemText (hwnd, 1108, HeaderDlg->Subject);
#elif defined(__NT__)
               SetDlgItemText (hwnd, 1106, HeaderDlg->From);
               SetDlgItemText (hwnd, 1109, HeaderDlg->FromAddress);
               sprintf (Temp, "%02d %s %d %2d:%02d:%02d", d_date.day, Months[d_date.month - 1], d_date.year, d_time.hour, d_time.minute, d_time.second);
               SetDlgItemText (hwnd, 1111, Temp);
               SetDlgItemText (hwnd, 1107, HeaderDlg->To);
               SetDlgItemText (hwnd, 1110, HeaderDlg->ToAddress);
               SetDlgItemText (hwnd, 1112, "");
               SetDlgItemText (hwnd, 1108, HeaderDlg->Subject);
#endif
            }
         }
      }
   }
   else if (Msg != NULL) {
      Cfg->MailAddress.First ();
      if ((HeaderDlg = new CMsgHeaderDlg (hwnd)) != NULL) {
         strcpy (HeaderDlg->From, Cfg->SysopName);
         if (Scan->Tag[0] == '\0' || Scan->Address[0] == '\0')
            strcpy (HeaderDlg->FromAddress, Cfg->MailAddress.String);
         else
            strcpy (HeaderDlg->FromAddress, Scan->Address);
         strcpy (HeaderDlg->To, "All");
         strcpy (HeaderDlg->ToAddress, Cfg->MailAddress.String);
         strcpy (HeaderDlg->Subject, "");

         HeaderDlg->Local = TRUE;

         if ((Continue = (USHORT)HeaderDlg->DoModal ()) == TRUE) {
#if defined(__OS2__)
            WinSetDlgItemText (hwnd, 1106, HeaderDlg->From);
            WinSetDlgItemText (hwnd, 1109, HeaderDlg->FromAddress);
            sprintf (Temp, "%02d %s %d %2d:%02d:%02d", d_date.day, Months[d_date.month - 1], d_date.year, d_time.hour, d_time.minute, d_time.second);
            WinSetDlgItemText (hwnd, 1111, Temp);
            WinSetDlgItemText (hwnd, 1107, HeaderDlg->To);
            WinSetDlgItemText (hwnd, 1110, HeaderDlg->ToAddress);
            WinSetDlgItemText (hwnd, 1112, "");
            WinSetDlgItemText (hwnd, 1108, HeaderDlg->Subject);
#elif defined(__NT__)
            SetDlgItemText (hwnd, 1106, HeaderDlg->From);
            SetDlgItemText (hwnd, 1109, HeaderDlg->FromAddress);
            sprintf (Temp, "%02d %s %d %2d:%02d:%02d", d_date.day, Months[d_date.month - 1], d_date.year, d_time.hour, d_time.minute, d_time.second);
            SetDlgItemText (hwnd, 1111, Temp);
            SetDlgItemText (hwnd, 1107, HeaderDlg->To);
            SetDlgItemText (hwnd, 1110, HeaderDlg->ToAddress);
            SetDlgItemText (hwnd, 1112, "");
            SetDlgItemText (hwnd, 1108, HeaderDlg->Subject);
#endif
         }
      }
   }

   if (Msg != NULL && Continue == TRUE && HeaderDlg != NULL) {
      Temp[0] = '\0';
      if (HeaderDlg->Received == TRUE)
         strcat (Temp, "Rcv ");
      if (HeaderDlg->Sent == TRUE)
         strcat (Temp, "Snt ");
      if (HeaderDlg->Private == TRUE)
         strcat (Temp, "Pvt ");
      if (HeaderDlg->Crash == TRUE)
         strcat (Temp, "Cra ");
      if (HeaderDlg->KillSent == TRUE)
         strcat (Temp, "K/s ");
      if (HeaderDlg->Local == TRUE)
         strcat (Temp, "Loc ");
      if (HeaderDlg->Hold == TRUE)
         strcat (Temp, "Hld ");
      if (HeaderDlg->FileAttach == TRUE)
         strcat (Temp, "Att ");
      if (HeaderDlg->FileRequest == TRUE)
         strcat (Temp, "Frq ");
      if (HeaderDlg->Intransit == TRUE)
         strcat (Temp, "Trs ");
#if defined(__OS2__)
      WinSetDlgItemText (hwnd, 1114, Temp);
#elif defined(__NT__)
      SetDlgItemText (hwnd, 1114, Temp);
#endif

      strcpy (Temp, HeaderDlg->To);
#if defined(__OS2__)
      if ((p = strtok (Temp, " ")) != NULL)
         sprintf (Header, "Hello, %s!\n \n", p);
      else
         sprintf (Header, "Hello!\n \n");
#elif defined(__NT__)
      if ((p = strtok (Temp, " ")) != NULL)
         sprintf (Header, "Hello, %s!\r\n \r\n", p);
      else
         sprintf (Header, "Hello!\r\n \r\n");
#endif

      if (Scan->Tag[0] != '\0') {
         strcpy (Origin, Cfg->SystemName);
         if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
            Data->Read (Scan->Key);
            GetOrigin (Data, Origin);
            delete Data;
         }

         Cfg->MailAddress.First ();
#if defined(__OS2__)
         sprintf (Footer, " \n--- %s v%s\n * Origin: %s (%s)\n", NAME, VERSION, Origin, HeaderDlg->FromAddress);
#elif defined(__NT__)
         sprintf (Footer, " \r\n--- %s v%s\r\n * Origin: %s (%s)\r\n", NAME, VERSION, Origin, HeaderDlg->FromAddress);
#endif
      }
      else {
#if defined(__OS2__)
         sprintf (Footer, " \n--- %s v%s\n", NAME, VERSION);
#elif defined(__NT__)
         sprintf (Footer, " \r\n--- %s v%s\r\n", NAME, VERSION);
#endif
      }

      Buffer = NULL;
      if (Reply == TRUE && DoQuote == TRUE) {
         Quote[0] = ' ';
         Quote[1] = '\0';
         i = 1;
         strcpy (Temp, Msg->From);
         if ((p = strtok (Temp, " ")) != NULL)
            do {
               Quote[i++] = *p;
            } while ((p = strtok (NULL, " ")) != NULL);
         Quote[i++] = '>';
         Quote[i++] = ' ';
         Quote[i] = '\0';

         bytes = strlen (Header) + strlen (Footer);
         if ((p = (CHAR *)Msg->Text.First ()) != NULL)
            do {
               if ((*p != 0x01 && strncmp (p, "SEEN-BY:", 8)) || ShowKludges == TRUE)
#if defined(__OS2__)
                  bytes += strlen (Quote) + strlen (p) + 1;
#elif defined(__NT__)
                  bytes += strlen (Quote) + strlen (p) + 2;
#endif
            } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

         if ((Buffer = (CHAR *)malloc (bytes + 1)) != NULL) {
            *Buffer = '\0';
            strcat (Buffer, Header);
            if ((p = (CHAR *)Msg->Text.First ()) != NULL)
               do {
                  if ((*p != 0x01 && strncmp (p, "SEEN-BY:", 8)) || ShowKludges == TRUE) {
                     strcat (Buffer, Quote);
                     strcat (Buffer, p);
#if defined(__OS2__)
                     strcat (Buffer, "\n");
#elif defined(__NT__)
                     strcat (Buffer, "\r\n");
#endif
                  }
               } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);
            strcat (Buffer, Footer);
         }
      }
      else {
         bytes = strlen (Header) + strlen (Footer);
         if ((Buffer = (CHAR *)malloc (bytes + 1)) != NULL) {
            *Buffer = '\0';
            strcat (Buffer, Header);
            strcat (Buffer, Footer);
         }
      }

      if (Buffer != NULL) {
#if defined(__OS2__)
         Value = 0x000000L;
         WinSetPresParam (WinWindowFromID (hwnd, 1107), PP_FOREGROUNDCOLOR, 4, &Value);

         WinSetDlgItemText (hwnd, 1105, Buffer);
#elif defined(__NT__)
         SetDlgItemText (hwnd, 1105, Buffer);
#endif
         free (Buffer);

         hAccel = hAccEditor;

#if defined(__OS2__)
         WinSendDlgItemMsg (hwnd, 1105, MLM_SETREADONLY, MPFROMSHORT (FALSE), 0L);
         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 1105));
#elif defined(__NT__)
         SendDlgItemMessage (hwnd, 1105, EM_SETREADONLY, (WPARAM)FALSE, 0L);
         SetFocus (GetDlgItem (hwnd, 1105));
#endif

#if defined(__OS2__)
         WinEnableControl (hwnd, 1201, FALSE);
         WinEnableControl (hwnd, 1202, FALSE);
         WinEnableControl (hwnd, 1203, FALSE);
         WinEnableControl (hwnd, 1204, FALSE);
         WinEnableControl (hwnd, 1205, FALSE);
         WinEnableControl (hwnd, 1206, FALSE);
         WinEnableControl (hwnd, 1207, FALSE);
         WinEnableControl (hwnd, 1208, TRUE);
         WinEnableControl (hwnd, 1209, TRUE);
#elif defined(__NT__)
         EnableWindow (GetDlgItem (hwnd, 1201), FALSE);
         EnableWindow (GetDlgItem (hwnd, 1202), FALSE);
         EnableWindow (GetDlgItem (hwnd, 1203), FALSE);
         EnableWindow (GetDlgItem (hwnd, 1204), FALSE);
         EnableWindow (GetDlgItem (hwnd, 1205), FALSE);
         EnableWindow (GetDlgItem (hwnd, 1206), FALSE);
         EnableWindow (GetDlgItem (hwnd, 1207), FALSE);
         EnableWindow (GetDlgItem (hwnd, 1208), TRUE);
         EnableWindow (GetDlgItem (hwnd, 1209), TRUE);
#endif
      }
   }
}

VOID SaveMessage (HWND hwnd)
{
   CHAR *p, *a, *Buffer, Temp[64];
   LONG bytes;
   class TEchotoss *EchoToss;
   class TAddress FromAddress, ToAddress;

#if defined(__OS2__)
   bytes = (LONG)WinQueryDlgItemTextLength (hwnd, 1105) + 1;
#elif defined(__NT__)
   bytes = GetWindowTextLength (GetDlgItem (hwnd, 1105)) + 1;
#endif

   if (Msg != NULL && bytes != 0L) {
      Msg->New ();

#if defined(__OS2__)
      WinQueryDlgItemText (hwnd, 1106, WinQueryDlgItemTextLength (hwnd, 1106) + 1, Msg->From);
      WinQueryDlgItemText (hwnd, 1107, WinQueryDlgItemTextLength (hwnd, 1107) + 1, Msg->To);
      WinQueryDlgItemText (hwnd, 1108, WinQueryDlgItemTextLength (hwnd, 1108) + 1, Msg->Subject);

      WinQueryDlgItemText (hwnd, 1109, WinQueryDlgItemTextLength (hwnd, 1109) + 1, Msg->FromAddress);
      WinQueryDlgItemText (hwnd, 1110, WinQueryDlgItemTextLength (hwnd, 1110) + 1, Msg->ToAddress);
#elif defined(__NT__)
      GetWindowText (GetDlgItem (hwnd, 1106), Msg->From, GetWindowTextLength (GetDlgItem (hwnd, 1106)) + 1);
      GetWindowText (GetDlgItem (hwnd, 1107), Msg->To, GetWindowTextLength (GetDlgItem (hwnd, 1107)) + 1);
      GetWindowText (GetDlgItem (hwnd, 1108), Msg->Subject, GetWindowTextLength (GetDlgItem (hwnd, 1108)) + 1);

      GetWindowText (GetDlgItem (hwnd, 1109), Msg->FromAddress, GetWindowTextLength (GetDlgItem (hwnd, 1109)) + 1);
      GetWindowText (GetDlgItem (hwnd, 1110), Msg->ToAddress, GetWindowTextLength (GetDlgItem (hwnd, 1110)) + 1);
#endif

      if (HeaderDlg != NULL) {
         Msg->Crash = HeaderDlg->Crash;
         Msg->FileAttach = HeaderDlg->FileAttach;
         Msg->FileRequest = HeaderDlg->FileRequest;
         Msg->Hold = HeaderDlg->Hold;
         Msg->Intransit = HeaderDlg->Intransit;
         Msg->KillSent = HeaderDlg->KillSent;
         Msg->Local = HeaderDlg->Local;
         Msg->Private = HeaderDlg->Private;
         Msg->ReceiptRequest = HeaderDlg->ReceiptRequest;
         Msg->Received = HeaderDlg->Received;
         Msg->Sent = HeaderDlg->Sent;
      }

      if (Scan->Tag[0] == '\0') {
         FromAddress.Parse (Msg->FromAddress);
         ToAddress.Parse (Msg->ToAddress);
         Cfg->MailAddress.First ();
         if ((FromAddress.Zone != ToAddress.Zone) || FromAddress.Zone != Cfg->MailAddress.Zone || ToAddress.Zone != Cfg->MailAddress.Zone || Cfg->ForceIntl == TRUE) {
            sprintf (Temp, "\001INTL %u:%u/%u %u:%u/%u", ToAddress.Zone, ToAddress.Net, ToAddress.Node, FromAddress.Zone, FromAddress.Net, FromAddress.Node);
            Msg->Text.Add (Temp);
         }
         if (FromAddress.Point != 0) {
            sprintf (Temp, "\001FMPT %u", FromAddress.Point);
            Msg->Text.Add (Temp);
         }
         if (ToAddress.Point != 0) {
            sprintf (Temp, "\001TOPT %u", ToAddress.Point);
            Msg->Text.Add (Temp);
         }
      }

      sprintf (Temp, "\001MSGID: %s %08lx", Msg->FromAddress, time (NULL));
      Msg->Text.Add (Temp);
      sprintf (Temp, "\001PID: %s", NAME_OS);
      Msg->Text.Add (Temp);

      if ((Buffer = (CHAR *)malloc (bytes + 1)) != NULL) {
#if defined(__OS2__)
         WinQueryDlgItemText (hwnd, 1105, bytes, Buffer);
#elif defined(__NT__)
         GetWindowText (GetDlgItem (hwnd, 1105), Buffer, bytes);
#endif
         Buffer[bytes] = '\0';

         a = Buffer;
         while ((p = strchr (a, '\n')) != NULL) {
            if (p > a) {
               p--;
               if (*p == '\r')
                  *p = '\0';
               p++;
            }
            *p = '\0';
            Msg->Text.Add (a);
            a = p + 1;
         }
         if (*a != '\0')
            Msg->Text.Add (a);

         free (Buffer);
      }

      Msg->Arrived.Day = Msg->Written.Day = d_date.day;
      Msg->Arrived.Month = Msg->Written.Month = d_date.month;
      Msg->Arrived.Year = Msg->Written.Year = (USHORT)d_date.year;
      Msg->Arrived.Hour = Msg->Written.Hour = d_time.hour;
      Msg->Arrived.Minute = Msg->Written.Minute = d_time.minute;
      Msg->Arrived.Second = Msg->Written.Second = d_time.second;

      Msg->Add ();

      if (Scan != NULL && Scan->Tag[0] != '\0') {
         if ((EchoToss = new TEchotoss (Cfg->SystemPath)) != NULL) {
            EchoToss->Load ();
            EchoToss->Add (Scan->Tag);
            EchoToss->Save ();
            delete EchoToss;
         }
      }
   }

   if (HeaderDlg != NULL) {
      delete HeaderDlg;
      HeaderDlg = NULL;
   }

   DisplayMessage (hwnd);
}

class TMsgBase *OpenArea (USHORT Storage, PSZ Path, PSZ Newsgroup, USHORT Board)
{
   class TMsgBase *Msg = NULL;

   switch (Storage) {
      case ST_JAM:
         Msg = new JAM (Path);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Path);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Path);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Path);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Path, (UCHAR)Board);
         break;
      case ST_USENET:
         Msg = new USENET (Cfg->NewsServer, Newsgroup);
         break;
   }

   return (Msg);
}

VOID CopyMessage (HWND hwnd, USHORT DoDelete)
{
   class CAreaListDlg *Dlg;
   class TMsgBase *CopyMsg = NULL;

   if ((Dlg = new CAreaListDlg (hwnd)) != NULL) {
      Dlg->Data = new TMsgData (Cfg->SystemPath);
      if (Dlg->DoModal () == TRUE) {
         if (Dlg->NetMail == TRUE)
            CopyMsg = OpenArea (Cfg->NetMailStorage, Cfg->NetMailPath, NULL, Cfg->NetMailBoard);
         else if (Dlg->EMail == TRUE)
            CopyMsg = OpenArea (Cfg->MailStorage, Cfg->MailPath, NULL, Cfg->MailBoard);
         else if (Dlg->Dupes == TRUE)
            CopyMsg = OpenArea (Cfg->DupeStorage, Cfg->DupePath, NULL, Cfg->DupeBoard);
         else if (Dlg->BadMsgs == TRUE)
            CopyMsg = OpenArea (Cfg->BadStorage, Cfg->BadPath, NULL, Cfg->BadBoard);
         else
            CopyMsg = OpenArea (Dlg->Data->Storage, Dlg->Data->Path, Dlg->Data->NewsGroup, Dlg->Data->Board);

         if (CopyMsg != NULL) {
            CopyMsg->Add (Msg);
            if (Scan != NULL) {
               Scan->Messages = CopyMsg->Number ();
               Scan->New++;
               Scan->Update ();
            }
            delete CopyMsg;
            if (DoDelete == TRUE) {
               Msg->Delete (Number);
               if (Msg->Next (Number) == FALSE) {
                  if (Msg->Previous (Number) == FALSE)
                     Msg->New ();
               }
               DisplayMessage (hwnd);
            }
         }
      }
      if (Dlg->Data != NULL)
         delete Dlg->Data;
      delete Dlg;
   }

   if (Scan != NULL) {
      Scan->Read (AreaKey);
      Scan->Messages = Msg->Number ();
      if (Scan->New != 0)
         Scan->New--;
      Scan->Update ();
   }
}

VOID ForwardMessage (HWND hwnd)
{
   CHAR Temp[128], CopyKey[32], Origin[128], *p;
   class TAddress FromAddress, ToAddress;
   class TMsgBase *CopyMsg = NULL;
   class CAreaListDlg *alDlg;
   class CMsgHeaderDlg *Dlg = NULL;

   _dos_getdate (&d_date);
   _dos_gettime (&d_time);

   if ((alDlg = new CAreaListDlg (hwnd)) != NULL) {
      alDlg->Data = new TMsgData (Cfg->SystemPath);
      if (alDlg->DoModal () == TRUE) {
         if (alDlg->NetMail == TRUE) {
            CopyMsg = OpenArea (Cfg->NetMailStorage, Cfg->NetMailPath, NULL, Cfg->NetMailBoard);
            strcpy (CopyKey, "NetMail");
         }
         else if (alDlg->EMail == TRUE) {
            CopyMsg = OpenArea (Cfg->MailStorage, Cfg->MailPath, NULL, Cfg->MailBoard);
            strcpy (CopyKey, "EMail");
         }
         else if (alDlg->Dupes == TRUE) {
            CopyMsg = OpenArea (Cfg->DupeStorage, Cfg->DupePath, NULL, Cfg->DupeBoard);
            strcpy (CopyKey, "Dupes");
         }
         else if (alDlg->BadMsgs == TRUE) {
            CopyMsg = OpenArea (Cfg->BadStorage, Cfg->BadPath, NULL, Cfg->BadBoard);
            strcpy (CopyKey, "BadMsgs");
         }
         else
            CopyMsg = OpenArea (alDlg->Data->Storage, alDlg->Data->Path, alDlg->Data->NewsGroup, alDlg->Data->Board);

         if (CopyMsg != NULL) {
            Cfg->MailAddress.First ();
            if ((Dlg = new CMsgHeaderDlg (hwnd)) != NULL) {
               strcpy (Dlg->From, Cfg->SysopName);
               if (Scan->Tag[0] == '\0' || Scan->Address[0] == '\0')
                  strcpy (Dlg->FromAddress, Cfg->MailAddress.String);
               else
                  strcpy (Dlg->FromAddress, Scan->Address);
               strcpy (Dlg->To, "All");
               strcpy (Dlg->ToAddress, Cfg->MailAddress.String);
               strcpy (Dlg->Subject, Msg->Subject);

               if (Dlg->DoModal () == TRUE) {
                  if (Scan->Tag[0] == '\0') {
                     FromAddress.Parse (Dlg->FromAddress);
                     ToAddress.Parse (Dlg->ToAddress);
                     Cfg->MailAddress.First ();
                     if ((FromAddress.Zone != ToAddress.Zone) || FromAddress.Zone != Cfg->MailAddress.Zone || ToAddress.Zone != Cfg->MailAddress.Zone || Cfg->ForceIntl == TRUE) {
                        sprintf (Temp, "\001INTL %u:%u/%u %u:%u/%u", ToAddress.Zone, ToAddress.Net, ToAddress.Node, FromAddress.Zone, FromAddress.Net, FromAddress.Node);
                        CopyMsg->Text.Add (Temp);
                     }
                     if (FromAddress.Point != 0) {
                        sprintf (Temp, "\001FMPT %u", FromAddress.Point);
                        CopyMsg->Text.Add (Temp);
                     }
                     if (ToAddress.Point != 0) {
                        sprintf (Temp, "\001TOPT %u", ToAddress.Point);
                        CopyMsg->Text.Add (Temp);
                     }
                  }

                  CopyMsg->New ();
                  strcpy (CopyMsg->From, Dlg->From);
                  strcpy (CopyMsg->FromAddress, Dlg->FromAddress);
                  strcpy (CopyMsg->To, Dlg->To);
                  strcpy (CopyMsg->ToAddress, Dlg->ToAddress);
                  strcpy (CopyMsg->Subject, Dlg->Subject);

                  sprintf (Temp, "\001MSGID: %s %08lx", CopyMsg->FromAddress, time (NULL));
                  CopyMsg->Text.Add (Temp);
                  sprintf (Temp, "\001PID: %s", NAME_OS);
                  CopyMsg->Text.Add (Temp);

                  CopyMsg->Arrived.Day = Msg->Written.Day = d_date.day;
                  CopyMsg->Arrived.Month = Msg->Written.Month = d_date.month;
                  CopyMsg->Arrived.Year = Msg->Written.Year = (USHORT)d_date.year;
                  CopyMsg->Arrived.Hour = Msg->Written.Hour = d_time.hour;
                  CopyMsg->Arrived.Minute = Msg->Written.Minute = d_time.minute;
                  CopyMsg->Arrived.Second = Msg->Written.Second = d_time.second;

                  CopyMsg->Text.Add ("===============================================================================");
                  sprintf (Temp, "* Forwarded by %s (%s)", Dlg->From, Dlg->FromAddress);
                  CopyMsg->Text.Add (Temp);
                  if (alDlg->NetMail == TRUE || alDlg->EMail == TRUE || alDlg->Dupes == TRUE || alDlg->BadMsgs == TRUE)
                     sprintf (Temp, "* Area : %s", CopyKey);
                  else
                     sprintf (Temp, "* Area : %s (%s)", alDlg->Data->Key, alDlg->Data->Display);
                  CopyMsg->Text.Add (Temp);
                  sprintf (Temp, "* From : %s, %s", Msg->From, Msg->FromAddress);
                  CopyMsg->Text.Add (Temp);
                  sprintf (Temp, "* To   : %s", Msg->To);
                  CopyMsg->Text.Add (Temp);
                  sprintf (Temp, "* Subj : %-.70s", Msg->Subject);
                  CopyMsg->Text.Add (Temp);
                  CopyMsg->Text.Add ("===============================================================================");

                  if ((p = (PSZ)Msg->Text.First ()) != NULL)
                     do {
                        if (*p != 0x01 && strncmp (p, "SEEN-BY: ", 9)) {
                           if (!strcmp (p, "---") || !strncmp (p, "--- ", 4)) {
                              strcpy (Temp, p);
                              Temp[1] = '+';
                           }
                           else if (!strncmp (p, " * Origin: ", 11)) {
                              strcpy (Temp, p);
                              Temp[3] = '0';
                           }
                           else
                              CopyMsg->Text.Add (p);
                        }
                        else if (ShowKludges == TRUE) {
                           strcpy (Temp, p);
                           if (Temp[0] == 0x01)
                              Temp[0] = '@';
                           else if (!strncmp (Temp, "SEEN-BY: ", 9))
                              Temp[4] = '+';
                           CopyMsg->Text.Add (Temp);
                        }
                     } while ((p = (PSZ)Msg->Text.Next ()) != NULL);

                  CopyMsg->Text.Add ("");
                  sprintf (Temp, "--- %s v%s", NAME, VERSION);
                  CopyMsg->Text.Add (Temp);

                  if (Scan->Tag[0] != '\0') {
                     GetOrigin (alDlg->Data, Origin);
                     Cfg->MailAddress.First ();
                     sprintf (Temp, " * Origin: %s (%s)", NAME, VERSION, Origin, Msg->FromAddress);
                     CopyMsg->Text.Add (Temp);
                  }

                  CopyMsg->Add ();

                  if (Scan != NULL) {
                     Scan->Messages = CopyMsg->Number ();
                     Scan->New++;
                     Scan->Update ();
                  }
               }
               delete Dlg;
            }
            delete CopyMsg;
         }
      }
      if (alDlg->Data != NULL)
         delete alDlg->Data;
      delete alDlg;
   }

   if (Scan != NULL)
      Scan->Read (AreaKey);
}

VOID ChangeArea (HWND hwnd)
{
   CHAR Title[128];
   class CAreaListDlg *Dlg;
   class TMsgData *Data;
   class TUser *User;

   if ((User = new TUser (Cfg->UserFile)) != NULL) {
      if (User->GetData (Cfg->SysopName) == TRUE) {
         if (User->MsgTag->Read (AreaKey) == TRUE) {
            User->MsgTag->LastRead = Number;
            User->MsgTag->Update ();
         }
         else {
            User->MsgTag->New ();
            strcpy (User->MsgTag->Area, AreaKey);
            User->MsgTag->Tagged = FALSE;
            User->MsgTag->LastRead = Number;
            User->MsgTag->Add ();
         }
         User->Update ();
      }
      delete User;
   }

   Data = new TMsgData (Cfg->SystemPath);

   if ((Dlg = new CAreaListDlg (hwnd)) != NULL) {
      Dlg->Data = Data;
      if (Dlg->DoModal () == TRUE) {
         if (Msg != NULL)
            delete Msg;
         Msg = NULL;

         if (Dlg->NetMail == TRUE) {
            strcpy (AreaKey, "NetMail");
#if defined(__POINT__)
            sprintf (Title, "%s v%s - %s", NAME, VERSION, AreaKey);
#else
            sprintf (Title, "%s Message Reader v%s - %s", NAME, VERSION, AreaKey);
#endif
#if defined(__OS2__)
            WinSetWindowText (hwndReaderFrame, Title);
#elif defined(__NT__)
            SetWindowText (hwndReaderClient, Title);
#endif
            Msg = OpenArea (Cfg->NetMailStorage, Cfg->NetMailPath, NULL, Cfg->NetMailBoard);
         }
         else if (Dlg->EMail == TRUE) {
            strcpy (AreaKey, "EMail");
#if defined(__POINT__)
            sprintf (Title, "%s v%s - %s", NAME, VERSION, AreaKey);
#else
            sprintf (Title, "%s Message Reader v%s - %s", NAME, VERSION, AreaKey);
#endif
#if defined(__OS2__)
            WinSetWindowText (hwndReaderFrame, Title);
#elif defined(__NT__)
            SetWindowText (hwndReaderClient, Title);
#endif
            Msg = OpenArea (Cfg->MailStorage, Cfg->MailPath, NULL, Cfg->MailBoard);
         }
         else if (Dlg->Dupes == TRUE) {
            strcpy (AreaKey, "Dupes");
#if defined(__POINT__)
            sprintf (Title, "%s v%s - %s", NAME, VERSION, AreaKey);
#else
            sprintf (Title, "%s Message Reader v%s - %s", NAME, VERSION, AreaKey);
#endif
#if defined(__OS2__)
            WinSetWindowText (hwndReaderFrame, Title);
#elif defined(__NT__)
            SetWindowText (hwndReaderClient, Title);
#endif
            Msg = OpenArea (Cfg->DupeStorage, Cfg->DupePath, NULL, Cfg->DupeBoard);
         }
         else if (Dlg->BadMsgs == TRUE) {
            strcpy (AreaKey, "BadMsgs");
#if defined(__POINT__)
            sprintf (Title, "%s v%s - %s", NAME, VERSION, AreaKey);
#else
            sprintf (Title, "%s Message Reader v%s - %s", NAME, VERSION, AreaKey);
#endif
#if defined(__OS2__)
            WinSetWindowText (hwndReaderFrame, Title);
#elif defined(__NT__)
            SetWindowText (hwndReaderClient, Title);
#endif
            Msg = OpenArea (Cfg->BadStorage, Cfg->BadPath, NULL, Cfg->BadBoard);
         }
         else {
            strcpy (AreaKey, Data->Key);
#if defined(__POINT__)
            sprintf (Title, "%s v%s - %s", NAME, VERSION, Data->Display);
#else
            sprintf (Title, "%s Message Reader v%s - %s", NAME, VERSION, Data->Display);
#endif
#if defined(__OS2__)
            WinSetWindowText (hwndReaderFrame, Title);
#elif defined(__NT__)
            SetWindowText (hwndReaderClient, Title);
#endif
            Msg = OpenArea (Data->Storage, Data->Path, Data->NewsGroup, Data->Board);
         }

         if (Msg != NULL && Scan != NULL) {
            Scan->Messages = Msg->Number ();
            Scan->Update ();
         }

         Number = 0L;
         if ((User = new TUser (Cfg->UserFile)) != NULL) {
            if (User->GetData (Cfg->SysopName) == TRUE) {
               if (User->MsgTag->Read (AreaKey) == TRUE)
                  Number = User->MsgTag->LastRead;
            }
            delete User;
         }

         if (Msg != NULL)
            Msg->Next (Number);
         DisplayMessage (hwnd);
      }
      delete Dlg;
   }

   if (Data != NULL)
      delete Data;
}

// ----------------------------------------------------------------------

/*
#if defined(__NT__)

static FARPROC PrevEditProc;

LRESULT CALLBACK EditSubclassProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg) {
      default:
         return (CallWindowProc (PrevEditProc, hwnd, msg, wParam, lParam));
   }
}
#endif
*/

#if defined(__OS2__)
MRESULT EXPENTRY ReaderWinProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
#elif defined(__NT__)
LRESULT CALLBACK ReaderWinProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
{
   switch (msg) {
      case WM_CREATE: {
         class TUser *User;
#if defined(__OS2__)
         ULONG Value;
         HWND hwndCtl;

         if ((hwndCtl = WinCreateWindow (hwnd, WC_STATIC, "Msg: ", SS_TEXT|DT_RIGHT|DT_VCENTER|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1101, NULL, NULL)) != NULLHANDLE) {
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_ENTRYFIELD, NULL, ES_READONLY|ES_LEFT|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1113, NULL, NULL)) != NULLHANDLE) {
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_ENTRYFIELD, NULL, ES_READONLY|ES_LEFT|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1114, NULL, NULL)) != NULLHANDLE) {
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_STATIC, NULL, SS_TEXT|DT_RIGHT|DT_VCENTER|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1102, NULL, NULL)) != NULLHANDLE) {
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
            WinSetWindowText (hwndCtl, "From: ");
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_ENTRYFIELD, NULL, ES_READONLY|ES_LEFT|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1106, NULL, NULL)) != NULLHANDLE) {
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_STATIC, NULL, SS_TEXT|DT_LEFT|DT_VCENTER|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1109, NULL, NULL)) != NULLHANDLE) {
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_STATIC, NULL, SS_TEXT|DT_LEFT|DT_VCENTER|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1111, NULL, NULL)) != NULLHANDLE) {
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_STATIC, NULL, SS_TEXT|DT_RIGHT|DT_VCENTER|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1103, NULL, NULL)) != NULLHANDLE) {
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
            WinSetWindowText (hwndCtl, "To: ");
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_ENTRYFIELD, NULL, ES_READONLY|ES_LEFT|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1107, NULL, NULL)) != NULLHANDLE) {
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_STATIC, NULL, SS_TEXT|DT_LEFT|DT_VCENTER|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1110, NULL, NULL)) != NULLHANDLE) {
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_STATIC, NULL, SS_TEXT|DT_LEFT|DT_VCENTER|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1112, NULL, NULL)) != NULLHANDLE) {
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_STATIC, NULL, SS_TEXT|DT_RIGHT|DT_VCENTER|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1104, NULL, NULL)) != NULLHANDLE) {
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
            WinSetWindowText (hwndCtl, "Subject: ");
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_ENTRYFIELD, NULL, ES_READONLY|ES_LEFT|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1108, NULL, NULL)) != NULLHANDLE) {
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            Value = WinQuerySysColor (HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0L);
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_MLE, NULL, MLS_WORDWRAP|MLS_BORDER|MLS_READONLY|MLS_VSCROLL|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 0, 0, hwnd, HWND_TOP, 1105, NULL, NULL)) != NULLHANDLE) {
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 14, "11.System VIO");
            Value = 0xFFFFFFL;
            WinSetPresParam (hwndCtl, PP_BACKGROUNDCOLOR, 4, &Value);
            Value = 0x000000L;
            WinSetPresParam (hwndCtl, PP_FOREGROUNDCOLOR, 4, &Value);
            WinSendMsg (hwndCtl, MLM_FORMAT, MPFROMSHORT (MLFIE_NOTRANS), 0L);
         }

         if ((hwndCtl = WinCreateWindow (hwnd, WC_BUTTON, "~Previous", BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 60, 25, hwnd, HWND_TOP, 1201, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
         if ((hwndCtl = WinCreateWindow (hwnd, WC_BUTTON, "~Next", BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 60, 25, hwnd, HWND_TOP, 1202, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
         if ((hwndCtl = WinCreateWindow (hwnd, WC_BUTTON, "~Reply", BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 60, 25, hwnd, HWND_TOP, 1203, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
         if ((hwndCtl = WinCreateWindow (hwnd, WC_BUTTON, "~List", BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 60, 25, hwnd, HWND_TOP, 1204, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
         if ((hwndCtl = WinCreateWindow (hwnd, WC_BUTTON, "~Enter", BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 60, 25, hwnd, HWND_TOP, 1205, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
         if ((hwndCtl = WinCreateWindow (hwnd, WC_BUTTON, "~Area", BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 60, 25, hwnd, HWND_TOP, 1206, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
         if ((hwndCtl = WinCreateWindow (hwnd, WC_BUTTON, "~Delete", BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 60, 25, hwnd, HWND_TOP, 1207, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
         if ((hwndCtl = WinCreateWindow (hwnd, WC_BUTTON, "~Abort", BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 60, 25, hwnd, HWND_TOP, 1208, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
         if ((hwndCtl = WinCreateWindow (hwnd, WC_BUTTON, "~Save", BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP|WS_VISIBLE, 0, 0, 60, 25, hwnd, HWND_TOP, 1209, NULL, NULL)) != NULLHANDLE)
            WinSetPresParam (hwndCtl, PP_FONTNAMESIZE, 7, "8.Helv");
#elif defined(__NT__)
         LOGFONT logFont;
         HFONT hFontHelv, hFontFixed;
         HWND hwndCtl;

         logFont.lfHeight = 6;
         logFont.lfWidth = 6;
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
         strcpy (logFont.lfFaceName, "Helv");
         hFontHelv = CreateFontIndirect (&logFont);

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
         hFontFixed = CreateFontIndirect (&logFont);

         CreateWindow ("STATIC", "Msg: ", SS_RIGHT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1101, hinst, NULL);
         CreateWindow ("STATIC", "", SS_LEFT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1113, hinst, NULL);
         CreateWindow ("STATIC", "", SS_LEFT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1114, hinst, NULL);

         CreateWindow ("STATIC", "From: ", SS_RIGHT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1102, hinst, NULL);
         CreateWindow ("STATIC", "", SS_LEFT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1106, hinst, NULL);
         CreateWindow ("STATIC", "", SS_LEFT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1109, hinst, NULL);
         hwndCtl = CreateWindow ("STATIC", "", SS_LEFT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1111, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));

         CreateWindow ("STATIC", "To: ", SS_RIGHT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1103, hinst, NULL);
         CreateWindow ("STATIC", "", SS_LEFT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1107, hinst, NULL);
         CreateWindow ("STATIC", "", SS_LEFT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1110, hinst, NULL);
         hwndCtl = CreateWindow ("STATIC", "", SS_LEFT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1112, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));

         CreateWindow ("STATIC", "Subject: ", SS_RIGHT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1104, hinst, NULL);
         CreateWindow ("STATIC", "", SS_LEFT|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1108, hinst, NULL);

         hwndCtl = CreateWindowEx (WS_EX_CLIENTEDGE, "EDIT", "", ES_MULTILINE|ES_LEFT|WS_CHILD|WS_VISIBLE|WS_VSCROLL, 0, 0, 0, 0, hwnd, (HMENU)1105, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontFixed, MAKELPARAM (FALSE, 0));
//         PrevEditProc = (FARPROC)SetWindowLong (hwndCtl, GWL_WNDPROC, (LONG)EditSubclassProc);

         hwndCtl = CreateWindow ("BUTTON", "&Previous", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1201, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));
         hwndCtl = CreateWindow ("BUTTON", "&Next", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1202, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));
         hwndCtl = CreateWindow ("BUTTON", "&Abort", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1208, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));
         hwndCtl = CreateWindow ("BUTTON", "&Save", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1209, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));
         hwndCtl = CreateWindow ("BUTTON", "&Enter", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1205, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));
         hwndCtl = CreateWindow ("BUTTON", "&Reply", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1203, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));
         hwndCtl = CreateWindow ("BUTTON", "&List", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1204, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));
         hwndCtl = CreateWindow ("BUTTON", "&Area", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1206, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));
         hwndCtl = CreateWindow ("BUTTON", "&Delete", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)1207, hinst, NULL);
         SendMessage (hwndCtl, WM_SETFONT, (WPARAM)hFontHelv, MAKELPARAM (FALSE, 0));
#endif
         Msg = OpenArea (Cfg->NetMailStorage, Cfg->NetMailPath, NULL, Cfg->NetMailBoard);
         strcpy (AreaKey, "NetMail");
         Number = 0L;
         if ((User = new TUser (Cfg->UserFile)) != NULL) {
            if (User->GetData (Cfg->SysopName) == TRUE) {
               if (User->MsgTag->Read (AreaKey) == TRUE)
                  Number = User->MsgTag->LastRead;
            }
            delete User;
         }
         if (Msg->ReadHeader (Number) == FALSE) {
            if (Msg->Next (Number) == FALSE)
               Msg->Previous (Number);
         }

         if ((Scan = new TScan) != NULL) {
            strcpy (Scan->Key, "NetMail");
            strcpy (Scan->Description, "FidoNet E-Mail");
            Scan->Messages = Msg->Number ();
            Scan->New = Msg->Number () - Msg->UidToMsgn (Number);
            Scan->Tag[0] = '\0';
            Scan->Add ();
            Scan->First ();
         }
         break;
      }

#if defined(__NT__)
      case WM_CTLCOLORSTATIC: {
         CHAR Temp[96];
         LOGBRUSH logBrush;

         if ((HWND)lParam == GetDlgItem (hwnd, 1107)) {
            GetWindowText ((HWND)lParam, Temp, GetWindowTextLength ((HWND)lParam) + 1);
            if (!stricmp (Temp, Cfg->SysopName))
               SetTextColor ((HDC)wParam, RGB (0xD0, 0x00, 0x00));
            else
               SetTextColor ((HDC)wParam, RGB (0x00, 0x00, 0x00));
         }

         SetBkColor ((HDC)wParam, RGB (0xC0, 0xC0, 0xC0));

         logBrush.lbStyle = BS_SOLID;
         logBrush.lbColor = RGB (0xC0, 0xC0, 0xC0);
         logBrush.lbHatch = 0;

         return ((BOOL)CreateBrushIndirect (&logBrush));
      }
#endif

      case WM_SIZE: {
         USHORT dx, dy;
         USHORT xcol2, wcol2, wcol3, wcol4;

         xcol2 = 80;
#if defined(__OS2__)
         dx = (USHORT)SHORT1FROMMP (mp2);
         dy = (USHORT)SHORT2FROMMP (mp2);
#elif defined(__NT__)
         dx = (USHORT)LOWORD (lParam);
         dy = (USHORT)HIWORD (lParam);
#endif
         wcol2 = (USHORT)(((float)(dx - xcol2) / 100.0) * 45.0);
         wcol3 = (USHORT)(((float)(dx - xcol2) / 100.0) * 32.0);
         wcol4 = (USHORT)(dx - xcol2 - wcol2 - wcol3);

#if defined(__OS2__)
         WinSetWindowPos (WinWindowFromID (hwnd, 1101), NULLHANDLE, 0, dy - STATIC_HEIGHT * 1, xcol2, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1102), NULLHANDLE, 0, dy - STATIC_HEIGHT * 2, xcol2, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1103), NULLHANDLE, 0, dy - STATIC_HEIGHT * 3, xcol2, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1104), NULLHANDLE, 0, dy - STATIC_HEIGHT * 4, xcol2, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);

         WinSetWindowPos (WinWindowFromID (hwnd, 1113), NULLHANDLE, xcol2, dy - STATIC_HEIGHT * 1, dx - xcol2, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1106), NULLHANDLE, xcol2, dy - STATIC_HEIGHT * 2, wcol2, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1107), NULLHANDLE, xcol2, dy - STATIC_HEIGHT * 3, wcol2, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1108), NULLHANDLE, xcol2, dy - STATIC_HEIGHT * 4, dx - xcol2, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);

         WinSetWindowPos (WinWindowFromID (hwnd, 1114), NULLHANDLE, xcol2 + wcol2, dy - STATIC_HEIGHT * 1, wcol3 + wcol4, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1109), NULLHANDLE, xcol2 + wcol2, dy - STATIC_HEIGHT * 2, wcol3, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1110), NULLHANDLE, xcol2 + wcol2, dy - STATIC_HEIGHT * 3, wcol3, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);

         WinSetWindowPos (WinWindowFromID (hwnd, 1111), NULLHANDLE, xcol2 + wcol2 + wcol3, dy - STATIC_HEIGHT * 2, wcol4, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1112), NULLHANDLE, xcol2 + wcol2 + wcol3, dy - STATIC_HEIGHT * 3, wcol4, STATIC_HEIGHT, SWP_SIZE|SWP_MOVE|SWP_SHOW);

         WinSetWindowPos (WinWindowFromID (hwnd, 1105), NULLHANDLE, 0, 35, dx, dy - STATIC_HEIGHT * 4 - 35, SWP_SIZE|SWP_MOVE|SWP_SHOW);

         WinSetWindowPos (WinWindowFromID (hwnd, 1201), NULLHANDLE, 5, 5, 0, 0, SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1202), NULLHANDLE, 5 + 65 * 1, 5, 0, 0, SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1208), NULLHANDLE, 5 + 65 * 2 + 5, 5, 0, 0, SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1209), NULLHANDLE, 5 + 65 * 3 + 5, 5, 0, 0, SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1205), NULLHANDLE, 5 + 65 * 4 + 10, 5, 0, 0, SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1203), NULLHANDLE, 5 + 65 * 5 + 10, 5, 0, 0, SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1204), NULLHANDLE, 5 + 65 * 6 + 15, 5, 0, 0, SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1206), NULLHANDLE, 5 + 65 * 7 + 15, 5, 0, 0, SWP_MOVE|SWP_SHOW);
         WinSetWindowPos (WinWindowFromID (hwnd, 1207), NULLHANDLE, 5 + 65 * 8 + 20, 5, 0, 0, SWP_MOVE|SWP_SHOW);
#elif defined(__NT__)
         MoveWindow (GetDlgItem (hwnd, 1101), 0, 0, xcol2, STATIC_HEIGHT, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1102), 0, STATIC_HEIGHT, xcol2, STATIC_HEIGHT, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1103), 0, STATIC_HEIGHT * 2, xcol2, STATIC_HEIGHT, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1104), 0, STATIC_HEIGHT * 3, xcol2, STATIC_HEIGHT, TRUE);

         MoveWindow (GetDlgItem (hwnd, 1113), xcol2, 0, dx - xcol2, STATIC_HEIGHT, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1106), xcol2, STATIC_HEIGHT, wcol2, STATIC_HEIGHT, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1107), xcol2, STATIC_HEIGHT * 2, wcol2, STATIC_HEIGHT, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1108), xcol2, STATIC_HEIGHT * 3, dx - xcol2, STATIC_HEIGHT, TRUE);

         MoveWindow (GetDlgItem (hwnd, 1114), xcol2 + wcol2, 0, wcol3 + wcol4, STATIC_HEIGHT, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1109), xcol2 + wcol2, STATIC_HEIGHT, wcol3, STATIC_HEIGHT, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1110), xcol2 + wcol2, STATIC_HEIGHT * 2, wcol3, STATIC_HEIGHT, TRUE);

         MoveWindow (GetDlgItem (hwnd, 1111), xcol2 + wcol2 + wcol3, STATIC_HEIGHT, wcol4, STATIC_HEIGHT, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1112), xcol2 + wcol2 + wcol3, STATIC_HEIGHT * 2, wcol4, STATIC_HEIGHT, TRUE);

         MoveWindow (GetDlgItem (hwnd, 1105), 0, STATIC_HEIGHT * 4, dx, dy - (STATIC_HEIGHT * 4) - 35, TRUE);

         MoveWindow (GetDlgItem (hwnd, 1201), 5, dy - 25 - 5, 60, 25, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1202), 5 + 65 * 1, dy - 25 - 5, 60, 25, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1208), 5 + 65 * 2 + 5, dy - 25 - 5, 60, 25, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1209), 5 + 65 * 3 + 5, dy - 25 - 5, 60, 25, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1205), 5 + 65 * 4 + 10, dy - 25 - 5, 60, 25, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1203), 5 + 65 * 5 + 10, dy - 25 - 5, 60, 25, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1204), 5 + 65 * 6 + 15, dy - 25 - 5, 60, 25, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1206), 5 + 65 * 7 + 15, dy - 25 - 5, 60, 25, TRUE);
         MoveWindow (GetDlgItem (hwnd, 1207), 5 + 65 * 8 + 20, dy - 25 - 5, 60, 25, TRUE);
#endif
         break;
      }

      case WM_USER:
         DisplayMessage (hwnd);
         break;

      case WM_COMMAND:
#if defined(__OS2__)
         if (SHORT1FROMMP (mp1) >= 1200 && SHORT1FROMMP (mp1) < 1300)
            WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, SHORT1FROMMP (mp1)));
         switch (SHORT1FROMMP (mp1)) {
#elif defined(__NT__)
         if (LOWORD (wParam) >= 1200 && LOWORD (wParam) < 1300)
            SetFocus (GetDlgItem (hwnd, LOWORD (wParam)));
         switch (LOWORD (wParam)) {
#endif
            case 103:
#if defined(__OS2__)
               WinPostMsg (hwnd, WM_CLOSE, 0L, 0L);
#elif defined(__NT__)
               PostMessage (hwnd, WM_CLOSE, 0, 0L);
#endif
               break;
            case 104: {
               class TUULib *UULib;

               if ((UULib = new TUULib) != NULL) {
                  UULib->Decode (Msg->Text);
                  delete UULib;
               }
               break;
            }
            case 201:   // Edit / Undo
#if defined(__OS2__)
               WinSendDlgItemMsg (hwnd, 1105, MLM_UNDO, 0L, 0L);
#endif
               break;
            case 202:   // Edit / Cut
#if defined(__OS2__)
               WinSendDlgItemMsg (hwnd, 1105, MLM_CUT, 0L, 0L);
#endif
               break;
            case 203:   // Edit / Copy
#if defined(__OS2__)
               WinSendDlgItemMsg (hwnd, 1105, MLM_COPY, 0L, 0L);
#endif
               break;
            case 204:   // Edit / Paste
#if defined(__OS2__)
               WinSendDlgItemMsg (hwnd, 1105, MLM_PASTE, 0L, 0L);
#endif
               break;   // Edit / Clear
            case 205:
#if defined(__OS2__)
               WinSendDlgItemMsg (hwnd, 1105, MLM_CLEAR, 0L, 0L);
#endif
               break;
            case 301:   // Message / Move
               CopyMessage (hwnd, TRUE);
               break;
            case 302:   // Message / Copy
               CopyMessage (hwnd, FALSE);
               break;
            case 303:   // Message / Forward
               ForwardMessage (hwnd);
               break;
            case 304:
#if defined(__OS2__)
               WinPostMsg (hwnd, WM_COMMAND, MPFROMSHORT (1205), 0L);
#elif defined(__NT__)
               PostMessage (hwnd, WM_COMMAND, 1205, 0L);
#endif
               break;
            case 305:
#if defined(__OS2__)
               WinPostMsg (hwnd, WM_COMMAND, MPFROMSHORT (1203), 0L);
#elif defined(__NT__)
               PostMessage (hwnd, WM_COMMAND, 1203, 0L);
#endif
               break;
            case 306:
#if defined(__OS2__)
               WinPostMsg (hwnd, WM_COMMAND, MPFROMSHORT (1207), 0L);
#elif defined(__NT__)
               PostMessage (hwnd, WM_COMMAND, 1207, 0L);
#endif
               break;
            case 308:
               if (Msg != NULL) {
                  if (Number != Msg->Lowest ()) {
                     Number = Msg->Lowest ();
                     DisplayMessage (hwnd);
                  }
               }
               break;
            case 309:
               if (Msg != NULL) {
                  if (Number != Msg->Highest ()) {
                     Number = Msg->Highest ();
                     DisplayMessage (hwnd);
                  }
               }
               break;

#if defined(__POINT__)
            case 401: {
               class CHardwareDlg *Dlg;

               if ((Dlg = new CHardwareDlg (hwnd)) != NULL) {
                  if (Dlg->DoModal () == TRUE)
                     Cfg->Save ();
                  delete Dlg;
               }
               break;
            }
            case 402: {
               class CCommandsDlg *Dlg;

               if ((Dlg = new CCommandsDlg (hwnd)) != NULL) {
                  if (Dlg->DoModal () == TRUE)
                     Cfg->Save ();
                  delete Dlg;
               }
               break;
            }
            case 403: {
               class CSiteInfoDlg *Dlg;

               if ((Dlg = new CSiteInfoDlg (hwnd)) != NULL) {
                  if (Dlg->DoModal () == TRUE)
                     Cfg->Save ();
                  delete Dlg;
               }
               break;
            }
            case 404: {
               class CMessageDlg *Dlg;

               if ((Dlg = new CMessageDlg (hwnd)) != NULL) {
                  Dlg->DoModal ();
                  delete Dlg;
               }
               break;
            }
            case 406: {
               class CInternetDlg *Dlg;

               if ((Dlg = new CInternetDlg (hwnd)) != NULL) {
                  Dlg->DoModal ();
                  delete Dlg;
               }
               break;
            }
            case 407: {
               class CNodesDlg *Dlg;

               if ((Dlg = new CNodesDlg (hwnd)) != NULL) {
                  Dlg->DoModal ();
                  delete Dlg;
               }
               break;
            }
#endif
            case 905: {
               class CProductDlg *Dlg;

               if ((Dlg = new CProductDlg (hwnd)) != NULL) {
                  Dlg->DoModal ();
                  delete Dlg;
               }
               break;
            }

            case 1201:
               if (Msg->Previous (Number) == TRUE)
                  DisplayMessage (hwnd);
               else {
#if defined(__OS2__)
                  WinAlarm (HWND_DESKTOP, WA_ERROR);
#endif
                  ChangeArea (hwnd);
               }
               break;
            case 1202:
               if (Msg->Next (Number) == TRUE)
                  DisplayMessage (hwnd);
               else {
#if defined(__OS2__)
                  WinAlarm (HWND_DESKTOP, WA_ERROR);
#endif
                  ChangeArea (hwnd);
               }
               break;
            case 1203:
               EditMessage (hwnd, TRUE, TRUE);
               break;
            case 1204: {
               class CMsgListDlg *Dlg;

               if ((Dlg = new CMsgListDlg (hwnd)) != NULL) {
                  Dlg->OldNumber = Number;
                  Dlg->Msg = Msg;
                  if (Dlg->DoModal () == TRUE) {
                     Number = Dlg->NewNumber;
                     DisplayMessage (hwnd);
                  }
                  delete Dlg;
               }
               break;
            }

            case 1205:     // Enter new message
               EditMessage (hwnd, FALSE, FALSE);
               break;

            case 1206:     // Message Areas List
               ChangeArea (hwnd);
               break;

            case 1207:        // Delete
#if defined(__OS2__)
               if ((int)WinMessageBox (HWND_DESKTOP, hwnd, "Do you want to delete this message ?", "Delete", 0, MB_YESNO|MB_MOVEABLE) == MBID_YES) {
#elif defined(__NT__)
               if (MessageBox (hwnd, "Do you want to delete this message ?", "Delete", MB_YESNO) == IDYES) {
#endif
                  Msg->Delete (Number);

                  if (Msg->Next (Number) == TRUE)
                     DisplayMessage (hwnd);
                  else if (Msg->Previous (Number) == TRUE)
                     DisplayMessage (hwnd);
                  else {
                     Msg->New ();
                     DisplayMessage (hwnd);
                  }
               }
               break;

            case 1208:
               DisplayMessage (hwnd);
               break;

            case 1209:
               SaveMessage (hwnd);
               break;

            case 1300:
               ShowKludges = (ShowKludges == TRUE) ? FALSE : TRUE;
               DisplayMessage (hwnd);
               break;

            case 1301:
               EditMessage (hwnd, TRUE, FALSE);
               break;

            case 1302:
               if (Msg->Original != 0L) {
                  Number = Msg->Original;
                  DisplayMessage (hwnd);
               }
#if defined(__OS2__)
               else
                  WinAlarm (HWND_DESKTOP, WA_ERROR);
#endif
               break;

            case 1303:
               if (Msg->Reply != 0L) {
                  Number = Msg->Reply;
                  DisplayMessage (hwnd);
               }
#if defined(__OS2__)
               else
                  WinAlarm (HWND_DESKTOP, WA_ERROR);
#endif
               break;
         }
         break;

#if defined(__OS2__)
      case WM_ERASEBACKGROUND: {
         POINTL pt;
         RECTL *rc = (RECTL *)mp2;

         pt.x = rc->xLeft;
         pt.y = rc->yTop;
         GpiMove ((HPS)mp1, &pt);
         pt.x = rc->xRight;
         pt.y = rc->yBottom;
         GpiSetColor ((HPS)mp1, CLR_PALEGRAY);
         GpiBox ((HPS)mp1, DRO_FILL, &pt, 0L, 0L);
         return ((MRESULT)FALSE);
      }
#endif

#if defined(__OS2__)
      case WM_CONTROL:
         break;
#endif

#if defined(__OS2__)
      case WM_CLOSE: {
#elif defined(__NT__)
      case WM_DESTROY: {
#endif
         class TUser *User;

         if ((User = new TUser (Cfg->UserFile)) != NULL) {
            if (User->GetData (Cfg->SysopName) == TRUE) {
               if (User->MsgTag->Read (AreaKey) == TRUE) {
                  User->MsgTag->LastRead = Number;
                  User->MsgTag->Update ();
               }
               else {
                  User->MsgTag->New ();
                  strcpy (User->MsgTag->Area, AreaKey);
                  User->MsgTag->Tagged = FALSE;
                  User->MsgTag->LastRead = Number;
                  User->MsgTag->Add ();
               }
               User->Update ();
            }
            delete User;
         }

         if (Scan != NULL) {
            delete Scan;
            Scan = NULL;
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

#if defined(__OS2__)
void main (int argc, char *argv[])
{
   int i, x, y, dx, dy;
   CHAR Title[128], *Config, *Channel;
   HMQ hmq;
   QMSG qmsg;
   FRAMECDATA fd;
   RECTL rc;

   Config = Channel = NULL;

   for (i = 1; i < argc; i++) {
      if (Config == NULL)
         Config = argv[i];
      else if (Channel == NULL)
         Channel = argv[i];
   }

   if (Config == NULL)
      Config = getenv ("LORA_CONFIG");
   if (Channel == NULL)
      Channel = getenv ("LORA_CHANNEL");

   if ((Cfg = new TConfig) != NULL) {
      Cfg->TaskNumber = 1;
      if (Cfg->Load (Config, Channel) == FALSE)
         Cfg->Default ();
   }

   if ((hab = WinInitialize (0)) != 0) {
      if ((hmq = WinCreateMsgQueue (hab, 0)) != 0) {
#if defined(__POINT__)
         WinRegisterClass (hab, "POINT_WINDOW", ReaderWinProc, CS_CLIPCHILDREN|CS_SIZEREDRAW|CS_MOVENOTIFY, 0);
#else
         WinRegisterClass (hab, "READER_WINDOW", ReaderWinProc, CS_CLIPCHILDREN|CS_SIZEREDRAW|CS_MOVENOTIFY, 0);
#endif
         fd.cb = sizeof (fd);
         fd.flCreateFlags = FCF_TASKLIST|FCF_TITLEBAR|FCF_SYSMENU|FCF_MINMAX|FCF_SIZEBORDER|FCF_NOBYTEALIGN|FCF_MENU;
         fd.hmodResources = NULL;
         fd.idResources = 257;

         if ((hwndReaderFrame = WinCreateWindow (HWND_DESKTOP, WC_FRAME, NULL, 0, 0, 0, 0, 0, NULL, HWND_TOP, fd.idResources, &fd, NULL)) != NULLHANDLE)
#if defined(__POINT__)
            hwndReaderClient = WinCreateWindow (hwndReaderFrame, "POINT_WINDOW", NULL, 0, 0, 0, 10, 10, hwndReaderFrame, HWND_BOTTOM, FID_CLIENT, NULL, NULL);
#else
            hwndReaderClient = WinCreateWindow (hwndReaderFrame, "READER_WINDOW", NULL, 0, 0, 0, 10, 10, hwndReaderFrame, HWND_BOTTOM, FID_CLIENT, NULL, NULL);
#endif

         if (hwndReaderFrame != NULLHANDLE && hwndReaderClient != NULLHANDLE) {
#if defined(__POINT__)
            sprintf (Title, "%s v%s - %s", NAME, VERSION, "NetMail");
#else
            sprintf (Title, "%s Message Reader v%s - %s", NAME, VERSION, "NetMail");
#endif
            WinSetWindowText (hwndReaderFrame, Title);

            WinQueryWindowRect (HWND_DESKTOP, &rc);
            dx = 690;
            if ((rc.xRight - rc.xLeft) < dx)
               dx = rc.xRight - rc.xLeft;
            dy = 490;
            if ((rc.yTop - rc.yBottom) < dy)
               dy = rc.yTop - rc.yBottom;

            x = ((rc.xRight - rc.xLeft) - dx) / 2;
            y = ((rc.yTop - rc.yBottom) - dy) / 2;

            WinSetWindowPos (hwndReaderFrame, NULLHANDLE, x, y, dx, dy, SWP_SIZE|SWP_MOVE|SWP_SHOW|SWP_ACTIVATE);

            hAccReader = WinLoadAccelTable (hab, NULLHANDLE, 1);
            hAccEditor = NULLHANDLE;
            hAccel = hAccReader;

            WinPostMsg (hwndReaderClient, WM_USER, 0L, 0L);
            WinPostMsg (hwndReaderClient, WM_COMMAND, MPFROMSHORT (1206), 0L);

            while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0)) {
               if (hAccel != NULLHANDLE)
                  WinTranslateAccel (hab, hwndReaderClient, hAccel, &qmsg);
               WinDispatchMsg (hab, &qmsg);
            }

            if (hAccReader != NULLHANDLE)
               WinDestroyAccelTable (hAccReader);
            if (hAccEditor != NULLHANDLE)
               WinDestroyAccelTable (hAccEditor);

            WinDestroyWindow (hwndReaderFrame);
         }

         WinDestroyMsgQueue (hmq);
      }

      WinTerminate (hab);
   }
}
#elif defined(__NT__)
int PASCAL WinMain (HINSTANCE hinstCurrent, HINSTANCE hinstPrevious, LPSTR lpszCmdLine, int nCmdShow)
{
   int x, y, dx, dy;
   CHAR Title[128], *Config, *Channel, *p;
   MSG msg;
   WNDCLASS wc;
   RECT rc;

   Config = Channel = NULL;

   strcpy (Title, lpszCmdLine);
   if ((p = strtok (Title, " ")) != NULL)
      do {
         if (Config == NULL)
            Config = p;
         else if (Channel == NULL)
            Channel = p;
      } while ((p = strtok (NULL, " ")) != NULL);

   if (Config == NULL)
      Config = getenv ("LORA_CONFIG");
   if (Channel == NULL)
      Channel = getenv ("LORA_CHANNEL");

   if ((Cfg = new TConfig) != NULL) {
      if (Cfg->Load (Config, Channel) == FALSE)
         Cfg->Default ();
   }

   if (hinstPrevious == NULL) {
      wc.style         = 0;
      wc.lpfnWndProc   = ReaderWinProc;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.hInstance     = hinstCurrent;
      wc.hIcon         = NULL;
      wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
      wc.lpszMenuName  = "MENU_1";
#if defined(__POINT__)
      wc.lpszClassName = "POINT_WINDOW";
#else
      wc.lpszClassName = "READER_WINDOW";
#endif

      if (!RegisterClass (&wc))
         return (FALSE);
   }

   hinst = hinstCurrent;
   msg.wParam = FALSE;

   hAccReader = LoadAccelerators (hinstCurrent, "ACCELERATOR_1");
   hAccEditor = LoadAccelerators (hinstCurrent, "ACCELERATOR_2");
   hAccel = hAccReader;

   GetClientRect (GetDesktopWindow (), &rc);
   dx = 690;
   if ((rc.right - rc.left) < dx)
      dx = rc.right - rc.left;
   dy = 490;
   if ((rc.bottom - rc.top) < dy)
      dy = rc.bottom - rc.top;

   x = ((rc.right - rc.left) - dx) / 2;
   y = ((rc.bottom - rc.top) - dy) / 2;

#if defined(__POINT__)
   sprintf (Title, "%s v%s - %s", NAME, VERSION, "NetMail");
#else
   sprintf (Title, "%s Mail Reader v%s - %s", NAME, VERSION, "NetMail");
#endif
#if defined(__POINT__)
   if ((hwndReaderClient = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW, "POINT_WINDOW", Title, WS_OVERLAPPEDWINDOW, x, y, dx, dy, NULL, NULL, hinstCurrent, NULL)) != NULL) {
#else
   if ((hwndReaderClient = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW, "READER_WINDOW", Title, WS_OVERLAPPEDWINDOW, x, y, dx, dy, NULL, NULL, hinstCurrent, NULL)) != NULL) {
#endif
      ShowWindow (hwndReaderClient, nCmdShow);

      PostMessage (hwndReaderClient, WM_USER, 0, 0L);
      PostMessage (hwndReaderClient, WM_COMMAND, 1206, 0L);

      while (GetMessage (&msg, NULL, 0, 0)) {
         if (!TranslateAccelerator (hwndReaderClient, hAccel, &msg)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
         }
      }
   }

   if (Cfg != NULL)
      delete Cfg;

   return ((int)msg.wParam);
}
#endif

