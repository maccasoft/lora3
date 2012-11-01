
#include "_ldefs.h"
#include "lorawin.h"
#include "msgbase.h"
#include "uulib.h"

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

// ----------------------------------------------------------------------

typedef struct {
   CHAR   Key[16];
   CHAR   Description[64];
   ULONG  Messages;
   ULONG  New;
   CHAR   Tag[64];
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
         Scan->Add ();
      }

      delete Msg;
   }

   if (Data != NULL) {
      if (Data->Next () == TRUE)
#if defined(__OS2__)
         WinPostMsg (m_hWnd, WM_USER, 0L, 0L);
#elif defined(__NT__)
         PostMessage (m_hWnd, WM_USER, 0, 0L);
#endif
      else
         EndDialog (TRUE);
   }
}

// ----------------------------------------------------------------------

class CMsgHeaderDlg : public CDialog
{
public:
   CMsgHeaderDlg (HWND p_hWnd);

   CHAR   From[48], To[48];
   CHAR   Subject[64];
   CHAR   FromAddress[64];
   CHAR   ToAddress[64];
   USHORT Private;
   USHORT FileAttach;
   USHORT FileRequest;

   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);
};

CMsgHeaderDlg::CMsgHeaderDlg (HWND p_hWnd) : CDialog ("204", p_hWnd)
{
}

USHORT CMsgHeaderDlg::OnInitDialog (VOID)
{
   Center ();

   SetDlgItemText (102, From);
   SetDlgItemText (108, FromAddress);
   SetDlgItemText (104, To);
   SetDlgItemText (110, ToAddress);
   SetDlgItemText (106, Subject);

   BM_SetCheck (107, FALSE);
   BM_SetCheck (109, FALSE);
   BM_SetCheck (111, FALSE);

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

   Private = (USHORT)BM_QueryCheck (107);
   FileAttach = (USHORT)BM_QueryCheck (109);
   FileRequest = (USHORT)BM_QueryCheck (111);

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
   VOID   OnChanged (VOID);
   USHORT OnInitDialog (VOID);
   VOID   OnOK (VOID);

private:
   int    Selected;
   DECLARE_MESSAGE_MAP ()
};

BEGIN_MESSAGE_MAP (CMsgListDlg, CDialog)
   ON_CONTROL (CN_ENTER, 101, OnOK)
#if defined(__NT__)
   ON_NOTIFY (LVN_ITEMCHANGED, 101, OnChanged)
#endif
END_MESSAGE_MAP ()

CMsgListDlg::CMsgListDlg (HWND p_hWnd) : CDialog ("202", p_hWnd)
{
   Selected = -1;
}

USHORT CMsgListDlg::OnInitDialog (VOID)
{
   CHAR Temp[32];
   ULONG Number;

   Center ();

   LVM_AllocateColumns (101, 4);
   LVM_InsertColumn (101, "Num,", LVC_RIGHT);
   LVM_InsertColumn (101, "From", LVC_LEFT);
   LVM_InsertColumn (101, "To", LVC_LEFT);
   LVM_InsertColumn (101, "Subject", LVC_LEFT);

   Msg->Lock (0L);
   Number = Msg->Lowest ();

   do {
      if (Msg->ReadHeader (Number) == TRUE) {
         LVM_InsertItem (101);

         sprintf (Temp, "%lu", Number);
         LVM_SetItemText (101, 0, Temp);
         LVM_SetItemText (101, 1, Msg->From);
         LVM_SetItemText (101, 2, Msg->To);
         LVM_SetItemText (101, 3, Msg->Subject);
      }
   } while (Msg->Next (Number) == TRUE);

   LVM_InvalidateView (101);

   Msg->UnLock ();

   return (TRUE);
}

VOID CMsgListDlg::OnChanged (VOID)
{
#if defined(__NT__)
   NM_LISTVIEW *pnmv = (NM_LISTVIEW *)m_lParam;

   if (!(pnmv->uNewState & LVIS_SELECTED) && (pnmv->uOldState & LVIS_SELECTED))
      Selected = -1;
   if (pnmv->uNewState & LVIS_SELECTED)
      Selected = pnmv->iItem;
#endif
}

VOID CMsgListDlg::OnOK (VOID)
{
#if defined(__OS2__)
   LV_PLISTDATA pRecord;

   if ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, 101, CM_QUERYRECORDEMPHASIS, MPFROMSHORT (CMA_FIRST), MPFROMSHORT (CRA_SELECTED))) != NULL)
      NewNumber = atol (pRecord->Column[0].Data);
#elif defined(__NT__)
   CHAR Temp[32];

   if (Selected != -1) {
      ListView_GetItemText (GetDlgItem (m_hWnd, 101), Selected, 0, Temp, sizeof (Temp));
      NewNumber = atol (Temp);
   }
#endif

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
            strcpy (name, Temp);
            strcpy (Temp, text);
            if ((p = strtok (Temp, " ")) != NULL)
               strcpy (address, p);
         }
         else {
            strcpy (Temp, text);
            if ((p = strtok (Temp, " ")) != NULL)
               strcpy (name, p);
         }
      }
   }
   else if ((p = strchr (Temp, '<')) != NULL) {
      *p++ = '\0';
      if ((a = strchr (p, '>')) != NULL)
         *a = '\0';
      strcpy (address, p);
      p = Temp;
      if (*p == '"')
         strcpy (Temp, ++p);
      p = strchr (Temp, '\0');
      while (--p > Temp) {
         if (*p != ' ' && *p != '"')
            break;
         *p = '\0';
      }
      strcpy (name, Temp);
      if (*name == '\0' && *address != '\0') {
         strcpy (name, address);
         *address = '\0';
      }
   }
}

VOID DisplayMessage (HWND hwnd)
{
   CHAR *p, Temp[128], *Buffer, *a;
   USHORT gotFrom = FALSE, gotTo = FALSE;
   ULONG bytes, Msgn;
#if defined(__OS2__)
   ULONG Value;

   WinSetDlgItemText (hwnd, 1113, "");
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
      WinSetDlgItemText (hwnd, 1113, Temp);
      WinSetDlgItemText (hwnd, 1106, Msg->From);
      WinSetDlgItemText (hwnd, 1109, Msg->FromAddress);
      if (Msgn != 0L) {
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Written.Day, Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Minute);
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
      SetWindowText (GetDlgItem (hwnd, 1106), Msg->From);
      SetWindowText (GetDlgItem (hwnd, 1109), Msg->FromAddress);
      if (Msgn != 0L) {
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Written.Day, Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Minute);
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

VOID EditMessage (HWND hwnd, USHORT Reply, USHORT DoQuote)
{
   int i;
   USHORT Continue = FALSE;
   CHAR *p, Quote[16], Temp[128], *Buffer;
   CHAR Header[128], Footer[128], FidoAddress[64];
   ULONG bytes;
#if defined(__OS2__)
   IPT ipt = 0L, cursor = 0L;
   ULONG Value;
#endif
   class CMsgHeaderDlg *Dlg;

   if (Reply == TRUE) {
      if (Msg->Read (Number, 72) == FALSE) {
         Msg->New ();
         Reply = FALSE;
      }
   }

   _dos_getdate (&d_date);
   _dos_gettime (&d_time);

   if (Scan->Tag[0] != '\0') {
      Cfg->MailAddress.First ();
#if defined(__OS2__)
      sprintf (Footer, " \n--- %s v%s\n * Origin: %s (%s)\n", NAME, VERSION, Cfg->SystemName, Cfg->MailAddress.String);
#elif defined(__NT__)
      sprintf (Footer, " \r\n--- %s v%s\r\n * Origin: %s (%s)\r\n", NAME, VERSION, Cfg->SystemName, Cfg->MailAddress.String);
#endif
   }
   else {
#if defined(__OS2__)
      sprintf (Footer, " \n--- %s v%s\n", NAME, VERSION);
#elif defined(__NT__)
      sprintf (Footer, " \r\n--- %s v%s\r\n", NAME, VERSION);
#endif
   }

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
                  Msg->FromAddress[0] = '\0';
                  Msg->ToAddress[0] = '\0';
                  ParseAddress (&p[7], Msg->From, Msg->FromAddress);
                  break;
               }
            } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

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

         Cfg->MailAddress.First ();

         if ((Dlg = new CMsgHeaderDlg (hwnd)) != NULL) {
            strcpy (Dlg->From, Cfg->SysopName);
            strcpy (Dlg->FromAddress, Cfg->MailAddress.String);
            strcpy (Dlg->ToAddress, FidoAddress);
            if (strchr (Msg->FromAddress, '@') != NULL && strchr (Msg->FromAddress, '/') == NULL && strchr (Msg->FromAddress, ':') == NULL)
               strcpy (Dlg->To, Msg->FromAddress);
            else
               strcpy (Dlg->To, Msg->From);
            sprintf (Temp, "Re: %s", Msg->Subject);
            strcpy (Dlg->Subject, Temp);

            if ((Continue = (USHORT)Dlg->DoModal ()) == TRUE) {
#if defined(__OS2__)
               WinSetDlgItemText (hwnd, 1106, Dlg->From);
               WinSetDlgItemText (hwnd, 1109, Dlg->FromAddress);
               sprintf (Temp, "%02d %s %d %2d:%02d:%02d", d_date.day, Months[d_date.month - 1], d_date.year, d_time.hour, d_time.minute, d_time.second);
               WinSetDlgItemText (hwnd, 1111, Temp);
               WinSetDlgItemText (hwnd, 1107, Dlg->To);
               WinSetDlgItemText (hwnd, 1110, Dlg->ToAddress);
               WinSetDlgItemText (hwnd, 1112, "");
               WinSetDlgItemText (hwnd, 1108, Dlg->Subject);
#elif defined(__NT__)
               SetDlgItemText (hwnd, 1106, Dlg->From);
               SetDlgItemText (hwnd, 1109, Dlg->FromAddress);
               sprintf (Temp, "%02d %s %d %2d:%02d:%02d", d_date.day, Months[d_date.month - 1], d_date.year, d_time.hour, d_time.minute, d_time.second);
               SetDlgItemText (hwnd, 1111, Temp);
               SetDlgItemText (hwnd, 1107, Dlg->To);
               SetDlgItemText (hwnd, 1110, Dlg->ToAddress);
               SetDlgItemText (hwnd, 1112, "");
               SetDlgItemText (hwnd, 1108, Dlg->Subject);
#endif
            }
            delete Dlg;
         }

         if (Continue == FALSE)
            return;

         hAccel = hAccEditor;

#if defined(__OS2__)
         WinSetDlgItemText (hwnd, 1105, "");
         Value = 0x000000L;
         WinSetPresParam (WinWindowFromID (hwnd, 1107), PP_FOREGROUNDCOLOR, 4, &Value);
#elif defined(__NT__)
         SetWindowText (GetDlgItem (hwnd, 1105), "");
#endif

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

#if defined(__OS2__)
            WinSetDlgItemText (hwnd, 1105, Buffer);
#elif defined(__NT__)
            SetDlgItemText (hwnd, 1105, Buffer);
#endif

            free (Buffer);
         }
      }
   }
   else {
      Cfg->MailAddress.First ();

      if ((Dlg = new CMsgHeaderDlg (hwnd)) != NULL) {
         strcpy (Dlg->From, Cfg->SysopName);
         strcpy (Dlg->FromAddress, Cfg->MailAddress.String);
         strcpy (Dlg->To, "All");
         strcpy (Dlg->ToAddress, Cfg->MailAddress.String);
         strcpy (Dlg->Subject, "");

         if ((Continue = (USHORT)Dlg->DoModal ()) == TRUE) {
#if defined(__OS2__)
            WinSetDlgItemText (hwnd, 1106, Dlg->From);
            WinSetDlgItemText (hwnd, 1109, Dlg->FromAddress);
            sprintf (Temp, "%02d %s %d %2d:%02d:%02d", d_date.day, Months[d_date.month - 1], d_date.year, d_time.hour, d_time.minute, d_time.second);
            WinSetDlgItemText (hwnd, 1111, Temp);
            WinSetDlgItemText (hwnd, 1107, Dlg->To);
            WinSetDlgItemText (hwnd, 1110, Dlg->ToAddress);
            WinSetDlgItemText (hwnd, 1112, "");
            WinSetDlgItemText (hwnd, 1108, Dlg->Subject);
#elif defined(__NT__)
            SetDlgItemText (hwnd, 1106, Dlg->From);
            SetDlgItemText (hwnd, 1109, Dlg->FromAddress);
            sprintf (Temp, "%02d %s %d %2d:%02d:%02d", d_date.day, Months[d_date.month - 1], d_date.year, d_time.hour, d_time.minute, d_time.second);
            SetDlgItemText (hwnd, 1111, Temp);
            SetDlgItemText (hwnd, 1107, Dlg->To);
            SetDlgItemText (hwnd, 1110, Dlg->ToAddress);
            SetDlgItemText (hwnd, 1112, "");
            SetDlgItemText (hwnd, 1108, Dlg->Subject);
#endif
         }
         delete Dlg;
      }

      if (Continue == FALSE)
         return;

      hAccel = hAccEditor;

#if defined(__OS2__)
      WinSetDlgItemText (hwnd, 1105, "");
      Value = 0x000000L;
      WinSetPresParam (WinWindowFromID (hwnd, 1107), PP_FOREGROUNDCOLOR, 4, &Value);
#elif defined(__NT__)
      SetWindowText (GetDlgItem (hwnd, 1105), "");
#endif

#if defined(__OS2__)
      WinQueryDlgItemText (hwnd, 1107, WinQueryDlgItemTextLength (hwnd, 1107) + 1, Temp);
      if ((p = strtok (Temp, " ")) != NULL)
         sprintf (Header, "Hello, %s!\n \n", p);
      else
         sprintf (Header, "Hello!\n \n");
#elif defined(__NT__)
      GetWindowText (GetDlgItem (hwnd, 1107), Temp, GetWindowTextLength (GetDlgItem (hwnd, 1107)) + 1);
      if ((p = strtok (Temp, " ")) != NULL)
         sprintf (Header, "Hello, %s!\r\n \r\n", p);
      else
         sprintf (Header, "Hello!\r\n \r\n");
#endif

      bytes = strlen (Header) + strlen (Footer);
      if ((Buffer = (CHAR *)malloc (bytes + 1)) != NULL) {
         *Buffer = '\0';
         strcat (Buffer, Header);
         strcat (Buffer, Footer);

#if defined(__OS2__)
         WinSetDlgItemText (hwnd, 1105, Buffer);
#elif defined(__NT__)
         SetDlgItemText (hwnd, 1105, Buffer);
#endif

         free (Buffer);
      }
   }

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

VOID SaveMessage (HWND hwnd)
{
   CHAR *p, *a, *Buffer, Temp[64];
   LONG bytes;
   class TEchotoss *EchoToss;

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

      if (Scan->Tag[0] != '\0') {
         sprintf (Temp, "\001MSGID: %s %08lx", Msg->FromAddress, time (NULL));
         Msg->Text.Add (Temp);
      }

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

   DisplayMessage (hwnd);
}

// ----------------------------------------------------------------------

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

         WinPostMsg (hwnd, WM_USER, 0L, 0L);
         WinPostMsg (hwnd, WM_COMMAND, MPFROMSHORT (1206), 0L);
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

         PostMessage (hwnd, WM_USER, 0, 0L);
         PostMessage (hwnd, WM_COMMAND, 1206, 0L);
#endif
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
         }

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
               break;
            case 1202:
               if (Msg->Next (Number) == TRUE)
                  DisplayMessage (hwnd);
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

            case 1206: {   // Message Areas List
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
                        User->MsgTag->LastPacked = 0L;
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

                     if (Dlg->NetMail == FALSE && Dlg->Dupes == FALSE && Dlg->BadMsgs == FALSE && Dlg->EMail == FALSE) {
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
                           case ST_USENET:
                              Msg = new USENET (Cfg->NewsServer, Data->NewsGroup);
                              break;
                        }
                     }
                     else if (Dlg->NetMail == TRUE) {
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
                        }
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
                        }
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
                        }
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
                        }
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
               break;
            }

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
                  User->MsgTag->LastPacked = 0L;
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

