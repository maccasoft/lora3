
// ----------------------------------------------------------------------
// Common Presentation Classes - Version 1.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/08/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "cpc.h"

#if defined(__OS2__)
extern HAB hab;
HWND hwndHelpInstance = NULL;
#elif defined(__NT__)
extern HINSTANCE hinst;
#endif

typedef struct {
   USHORT cbSize;
   class CDialog *Dlg;
} CDIALOG_DATA;

// ----------------------------------------------------------------------
// Dialog window procedure
// ----------------------------------------------------------------------

#if defined(__OS2__)
MRESULT EXPENTRY CDialogDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
#elif defined(__NT__)
BOOL CALLBACK CDialogDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
{
   int i;
   class CDialog *dlgClass;
   CPC_MSGMAP_ENTRY *msgMap = NULL;

#if defined(__OS2__)
   dlgClass = (class CDialog *)WinQueryWindowULong (hwnd, QWL_USER);
#elif defined(__NT__)
   dlgClass = (class CDialog *)GetWindowLong (hwnd, GWL_USERDATA);
#endif

   if (dlgClass != NULL) {
      dlgClass->m_msg = msg;
#if defined(__OS2__)
      dlgClass->m_mp1 = mp1;
      dlgClass->m_mp2 = mp2;
#elif defined(__NT__)
      dlgClass->m_wParam = wParam;
      dlgClass->m_lParam = lParam;
#endif

      msgMap = dlgClass->GetMessageMap ();
   }

   switch (msg) {
#if defined(__OS2__)
      case WM_INITDLG: {
         CDIALOG_DATA *cData = (CDIALOG_DATA *)mp2;
#elif defined(__NT__)
      case WM_INITDIALOG: {
         CDIALOG_DATA *cData = (CDIALOG_DATA *)lParam;
#endif

         dlgClass = cData->Dlg;
         dlgClass->m_hWnd = hwnd;
         dlgClass->lv_Selected = -1;
#if defined(__OS2__)
         WinSetWindowULong (hwnd, QWL_USER, (ULONG)dlgClass);
#elif defined(__NT__)
         SetWindowLong (hwnd, GWL_USERDATA, (LONG)dlgClass);
#endif

         dlgClass->OnInitDialog ();
#if defined(__OS2__)
         break;
#elif defined(__NT__)
         return ((BOOL)TRUE);
#endif
      }

#if defined(__OS2__)
      case WM_CONTROL:
         for (i = 0; msgMap[i].pfn != NULL; i++) {
            if (msgMap[i].nMessage == msg) {
               if (msgMap[i].nID >= SHORT1FROMMP (mp1) && msgMap[i].nLastID <= SHORT1FROMMP (mp1)) {
                  if (msgMap[i].nCode == SHORT2FROMMP (mp1)) {
                     (dlgClass->*msgMap[i].pfn) ();
                     break;
                  }
               }
            }
         }
         return (0);
#endif

#if defined(__NT__)
      case WM_NOTIFY: {
         LPNMHDR pnmh = (LPNMHDR)lParam;
         NM_LISTVIEW *pnmv = (NM_LISTVIEW *)lParam;

         if (pnmh->code == NM_DBLCLK)
            dlgClass->OnOK ();
         else if (pnmh->code == LVN_ITEMCHANGED) {
            if (!(pnmv->uNewState & LVIS_SELECTED) && (pnmv->uOldState & LVIS_SELECTED))
               dlgClass->lv_Selected = -1;
            if (pnmv->uNewState & LVIS_SELECTED)
               dlgClass->lv_Selected = (SHORT)pnmv->iItem;
         }
         else {
            for (i = 0; msgMap[i].pfn != NULL; i++) {
               if (msgMap[i].nMessage == msg && msgMap[i].nCode == pnmh->code) {
                  if (msgMap[i].nID >= pnmh->idFrom && msgMap[i].nLastID <= pnmh->idFrom) {
                     (dlgClass->*msgMap[i].pfn) ();
                     break;
                  }
               }
            }
         }
         return (0);
      }
#endif

#if defined(__OS2__)
      case WM_CHAR:
         if (SHORT1FROMMP (mp1) & KC_VIRTUALKEY && SHORT2FROMMP (mp2) == VK_F1)
            dlgClass->OnHelp ();
         break;
#endif

      case WM_HELP:
         dlgClass->OnHelp ();
         break;
/*
#if defined(__OS2__)
      case WM_HELP: {
         USHORT id;
         HWND hwndHelpInstance;

         id = WinQueryWindowUShort (hwnd, QWS_ID);
         hwndHelpInstance = WinQueryHelpInstance (dlgClass->owner_hWnd);
         WinSendMsg (hwndHelpInstance, HM_DISPLAY_HELP, MPFROM2SHORT (id, 0), MPFROMSHORT (HM_RESOURCEID));
         break;
      }
#endif
*/

      case WM_COMMAND:
#if defined(__OS2__)
         switch (SHORT1FROMMP (mp1)) {
#elif defined(__NT__)
         switch (LOWORD (wParam)) {
#endif
            case IDOK:
               dlgClass->OnOK ();
               break;
            case IDCANCEL:
               dlgClass->OnCancel ();
               break;
            case IDHELP:
               dlgClass->OnHelp ();
               break;
            default:
               for (i = 0; msgMap[i].pfn != NULL; i++) {
#if defined(__OS2__)
                  if (msgMap[i].nMessage == msg) {
                     if (msgMap[i].nID >= SHORT1FROMMP (mp1) && msgMap[i].nLastID <= SHORT1FROMMP (mp1)) {
#elif defined(__NT__)
                  if (msgMap[i].nMessage == msg && msgMap[i].nCode == HIWORD (wParam)) {
                     if (msgMap[i].nID >= LOWORD (wParam) && msgMap[i].nLastID <= LOWORD (wParam)) {
#endif
                        (dlgClass->*msgMap[i].pfn) ();
                        break;
                     }
                  }
               }
               break;
         }
         return (0);

#if defined(__NT__)
      case WM_CLOSE:
         EndDialog (hwnd, FALSE);
         break;
#endif

      default:
         if (msgMap != NULL) {
            for (i = 0; msgMap[i].pfn != NULL; i++) {
               if (msgMap[i].nMessage == msg) {
                  (dlgClass->*msgMap[i].pfn) ();
                  return (0);
               }
            }
         }
         break;
   }

#if defined(__OS2__)
   return (WinDefDlgProc (hwnd, msg, mp1, mp2));
#elif defined(__NT__)
   return ((BOOL)FALSE);
#endif
}

CDialog::CDialog (PSZ dialogTemplate, HWND p_hWnd)
{
   strcpy (dlgTemplate, dialogTemplate);
   owner_hWnd = p_hWnd;
   help_hWnd = NULL;
#if defined(__OS2__)
   WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));
#elif defined(__NT__)
#endif
}

CDialog::~CDialog (void)
{
#if defined(__OS2__)
   if (help_hWnd != NULL)
      WinDestroyHelpInstance (help_hWnd);
#elif defined(__NT__)
#endif
}

VOID CDialog::Center (VOID)
{
#if defined(__OS2__)
   RECTL rc;
#elif defined(__NT__)
   RECT rc;
#endif

#if defined(__OS2__)
   WinQueryWindowRect (m_hWnd, &rc);
   rc.yTop = (WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN) - (rc.yTop - rc.yBottom)) / 2;
   rc.xLeft = (WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN) - (rc.xRight - rc.xLeft)) / 2;
   WinSetWindowPos (m_hWnd, NULL, rc.xLeft, rc.yTop, 0, 0, SWP_MOVE);
#elif defined(__NT__)
   GetWindowRect (m_hWnd, &rc);
   rc.top = (GetSystemMetrics (SM_CYSCREEN) - (rc.bottom - rc.top)) / 2;
   rc.left = (GetSystemMetrics (SM_CXSCREEN) - (rc.right - rc.left)) / 2;
   SetWindowPos (m_hWnd, NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
#endif
}

LONG CDialog::DoModal (VOID)
{
   LONG ulResult = 0L;
   CDIALOG_DATA cDialog;

   cDialog.cbSize = sizeof (CDIALOG_DATA);
   cDialog.Dlg = this;

#if defined(__OS2__)
   WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE));
   ulResult = WinDlgBox (HWND_DESKTOP, owner_hWnd, CDialogDlgProc, NULLHANDLE, atoi (dlgTemplate), &cDialog);
#elif defined(__NT__)
   ulResult = DialogBoxParam (hinst, MAKEINTRESOURCE (atoi (dlgTemplate)), owner_hWnd, (DLGPROC)CDialogDlgProc, (LPARAM)&cDialog);
#endif

   return (ulResult);
}

// ----------------------------------------------------------------------
// Windowing API replacement
// ----------------------------------------------------------------------

VOID CDialog::EndDialog (USHORT Result)
{
#if defined(__OS2__)
   WinDismissDlg (m_hWnd, Result);
#elif defined(__NT__)
   ::EndDialog (m_hWnd, Result);
#endif
}

VOID CDialog::GetDlgItemText (int id, PSZ text, USHORT size)
{
#if defined(__OS2__)
   WinQueryDlgItemText (m_hWnd, id, size, text);
#elif defined(__NT__)
   ::GetDlgItemText (m_hWnd, id, text, size);
#endif
}

VOID CDialog::GetDlgItemText (int id, USHORT size, PSZ text)
{
#if defined(__OS2__)
   WinQueryDlgItemText (m_hWnd, id, size, text);
#elif defined(__NT__)
   ::GetDlgItemText (m_hWnd, id, text, size);
#endif
}

USHORT CDialog::GetDlgItemTextLength (int id)
{
#if defined(__OS2__)
   return ((USHORT)(WinQueryDlgItemTextLength (m_hWnd, id) + 1));
#elif defined(__NT__)
   return ((USHORT)(::GetWindowTextLength (GetDlgItem (m_hWnd, id)) + 1));
#endif
}

VOID CDialog::SetDlgItemText (int id, PSZ text)
{
#if defined(__OS2__)
   WinSetDlgItemText (m_hWnd, id, text);
#elif defined(__NT__)
   ::SetDlgItemText (m_hWnd, id, text);
#endif
}

VOID CDialog::SetFocus (int id)
{
#if defined(__OS2__)
   WinSetFocus (HWND_DESKTOP, WinWindowFromID (m_hWnd, id));
#elif defined(__NT__)
   ::SetFocus (GetDlgItem (m_hWnd, id));
#endif
}

VOID CDialog::WinHelp (PSZ help_file, int topic_id, PSZ title)
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
         WinAssociateHelpInstance (help_hWnd, owner_hWnd);
   }
   if (help_hWnd != NULL)
      WinSendMsg (help_hWnd, HM_DISPLAY_HELP, MPFROM2SHORT (topic_id, 0), MPFROMSHORT (HM_RESOURCEID));

#elif defined(__NT__)
   title = title;
   ::WinHelp (owner_hWnd, helpFile, HELP_CONTEXT, topic_id);
#endif
}

// ----------------------------------------------------------------------
// Dialog control messages
// ----------------------------------------------------------------------

USHORT CDialog::BM_QueryCheck (int id)
{
#if defined(__OS2__)
   return ((USHORT)WinSendDlgItemMsg (m_hWnd, id, BM_QUERYCHECK, 0L, 0L));
#elif defined(__NT__)
   return ((USHORT)SendDlgItemMessage (m_hWnd, id, BM_GETCHECK, 0, 0L));
#endif
}

VOID CDialog::BM_SetCheck (int id, USHORT value)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, BM_SETCHECK, MPFROMSHORT (value), 0L);
#elif defined(__NT__)
   SendDlgItemMessage (m_hWnd, id, BM_SETCHECK, (WPARAM)value, 0L);
#endif
}

VOID CDialog::CB_AddString (int id, PSZ value)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, LM_INSERTITEM, MPFROMSHORT (LIT_END), MPFROMP (value));
#elif defined(__NT__)
   SendDlgItemMessage (m_hWnd, id, CB_ADDSTRING, 0, (LPARAM)value);
#endif
}

VOID CDialog::CB_SelectItem (int id, USHORT value)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, LM_SELECTITEM, MPFROMSHORT (value), MPFROMSHORT (TRUE));
#elif defined(__NT__)
   SendDlgItemMessage (m_hWnd, id, CB_SETCURSEL, (WPARAM)value, 0L);
#endif
}

USHORT CDialog::CB_QuerySelection (int id)
{
#if defined(__OS2__)
   return ((USHORT)WinSendDlgItemMsg (m_hWnd, id, LM_QUERYSELECTION, MPFROMSHORT (LIT_FIRST), 0L));
#elif defined(__NT__)
   return ((USHORT)SendDlgItemMessage (m_hWnd, id, CB_GETCURSEL, 0, 0L));
#endif
}

VOID CDialog::EM_SetTextLimit (int id, USHORT value)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, EM_SETTEXTLIMIT, MPFROMSHORT (value), 0L);
#elif defined(__NT__)
   SendDlgItemMessage (m_hWnd, id, EM_SETLIMITTEXT, (WPARAM)value, 0L);
#endif
}

VOID CDialog::LM_AddString (int id, PSZ value)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, LM_INSERTITEM, MPFROMSHORT (LIT_END), MPFROMP (value));
#elif defined(__NT__)
   SendDlgItemMessage (m_hWnd, id, LB_ADDSTRING, 0, (LPARAM)value);
#endif
}

VOID CDialog::LM_SetItemText (int id, USHORT value, PSZ text)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, LM_SETITEMTEXT, MPFROMSHORT (value), MPFROMP (text));
#elif defined(__NT__)
#endif
}

VOID CDialog::LM_DeleteAll (int id)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, LM_DELETEALL, 0L, 0L);
#elif defined(__NT__)
   SendDlgItemMessage (m_hWnd, id, LB_RESETCONTENT, 0, 0L);
#endif
}

VOID CDialog::LM_DeleteItem (int id, USHORT value)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, LM_DELETEITEM, MPFROMSHORT (value), 0L);
#elif defined(__NT__)
   SendDlgItemMessage (m_hWnd, id, LB_DELETESTRING, (WPARAM)value, 0L);
#endif
}

VOID CDialog::LM_SelectItem (int id, USHORT value)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, LM_SELECTITEM, MPFROMSHORT (value), MPFROMSHORT (TRUE));
#elif defined(__NT__)
   SendDlgItemMessage (m_hWnd, id, LB_SETCURSEL, (WPARAM)value, 0L);
#endif
}

USHORT CDialog::LM_QueryItemCount (int id)
{
#if defined(__OS2__)
   return ((USHORT)WinSendDlgItemMsg (m_hWnd, id, LM_QUERYITEMCOUNT, 0L, 0L));
#elif defined(__NT__)
   return ((USHORT)SendDlgItemMessage (m_hWnd, id, LB_GETCOUNT, 0, 0L));
#endif
}

VOID CDialog::LM_QueryItemText (int id, USHORT value, USHORT size, PSZ text)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, LM_QUERYITEMTEXT, MPFROM2SHORT (value, size), MPFROMP (text));
#elif defined(__NT__)
   SendDlgItemMessage (m_hWnd, id, LB_GETTEXT, (WPARAM)value, (LPARAM)text);
   size = size;
#endif
}

USHORT CDialog::LM_QuerySelection (int id)
{
#if defined(__OS2__)
   return ((USHORT)WinSendDlgItemMsg (m_hWnd, id, LM_QUERYSELECTION, MPFROMSHORT (LIT_FIRST), 0L));
#elif defined(__NT__)
   return ((USHORT)SendDlgItemMessage (m_hWnd, id, LB_GETCURSEL, 0, 0L));
#endif
}

#if defined(__OS2__)

ULONG nFields, listDataSize, subItem;
PFIELDINFO pFieldInfo, pTopField;
LV_PLISTDATA pRecord;

#elif defined(__NT__)

LV_COLUMN lvC;
LV_ITEM lvI;

#endif

VOID CDialog::LVM_AllocateColumns (int id, int value)
{
#if defined(__OS2__)
   HWND hwndContainer;
   CNRINFO cnrInfo;

   hwndContainer = WinWindowFromID (m_hWnd, id);

   memset (&cnrInfo, 0, sizeof (CNRINFO));
   cnrInfo.cb = sizeof (CNRINFO);
   cnrInfo.flWindowAttr = CV_DETAIL|CA_DETAILSVIEWTITLES;
   WinSendMsg (hwndContainer, CM_SETCNRINFO, MPFROMP (&cnrInfo), MPFROMLONG (CMA_FLWINDOWATTR));

   nFields = value;
   pFieldInfo = (PFIELDINFO)WinSendMsg (hwndContainer, CM_ALLOCDETAILFIELDINFO, (MPARAM)nFields, (MPARAM)0);
   pTopField = pFieldInfo;

   subItem = 0L;
   pRecord = NULL;
#elif defined(__NT__)
   id = id;
   value = value;

   InitCommonControls ();

   lvC.mask = LVCF_FMT|LVCF_TEXT|LVCF_SUBITEM|LVCF_WIDTH;
   lvC.iSubItem = 0;
   lvC.pszText = "";
   lvC.cx = 50;
   lvC.fmt = LVCFMT_LEFT;

   lvI.mask = 0;
   lvI.iItem = -1;
   lvI.iSubItem = 0;
#endif
}

VOID CDialog::LVM_InsertColumn (int id, PSZ text, int align)
{
#if defined(__OS2__)
   FIELDINFOINSERT fieldinsert;

   pFieldInfo->cb = sizeof (FIELDINFO);
   pFieldInfo->pTitleData = text;
   pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_SEPARATOR|CFA_HORZSEPARATOR;
   if (align == LVC_LEFT)
      pFieldInfo->flData |= CFA_LEFT;
   else if (align == LVC_CENTER)
      pFieldInfo->flData |= CFA_CENTER;
   else if (align == LVC_RIGHT)
      pFieldInfo->flData |= CFA_RIGHT;
   pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
   pFieldInfo->offStruct = sizeof (RECORDCORE) + sizeof (COLUMNDATA) * subItem;
   pFieldInfo->pUserData = NULL;
   pFieldInfo->cxWidth = 0L;
   subItem++;

   if ((pFieldInfo = pFieldInfo->pNextFieldInfo) == NULL) {
      fieldinsert.cb = sizeof (FIELDINFOINSERT);
      fieldinsert.pFieldInfoOrder = (PFIELDINFO)CMA_FIRST;
      fieldinsert.fInvalidateFieldInfo = TRUE;
      fieldinsert.cFieldInfoInsert = nFields;
      WinSendMsg (WinWindowFromID (m_hWnd, id), CM_INSERTDETAILFIELDINFO, MPFROMP (pTopField), MPFROMP (&fieldinsert));
   }
#elif defined(__NT__)
   lvC.pszText = text;
   if (align == LVC_LEFT)
      lvC.fmt = LVCFMT_LEFT;
   else if (align == LVC_CENTER)
      lvC.fmt = LVCFMT_CENTER;
   else if (align == LVC_RIGHT)
      lvC.fmt = LVCFMT_RIGHT;
   ListView_InsertColumn (GetDlgItem (m_hWnd, id), lvC.iSubItem++, &lvC);
#endif
}

VOID CDialog::LVM_DeleteItem (int id, int item)
{
#if defined(__OS2__)
   PRECORDCORE pRecord;

   if (item >= 0) {
      if ((pRecord = (PRECORDCORE)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, 0L, MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER))) != NULL)
         do {
            if (item == 0)
               break;
            item--;
         } while ((pRecord = (PRECORDCORE)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, MPFROMP (pRecord), MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER))) != NULL);
      if (pRecord != NULL)
         WinSendDlgItemMsg (m_hWnd, id, CM_REMOVERECORD, MPFROMP (&pRecord), MPFROM2SHORT (1, CMA_FREE|CMA_INVALIDATE));
      lv_Selected = -1;
   }
#elif defined(__NT__)
   ListView_DeleteItem (GetDlgItem (m_hWnd, id), item);
#endif
}

VOID CDialog::LVM_DeleteAll (int id)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, CM_REMOVERECORD, NULL, MPFROM2SHORT (0, CMA_FREE|CMA_INVALIDATE));
   lv_Selected = -1;
#elif defined(__NT__)
   ListView_DeleteAllItems (GetDlgItem (m_hWnd, id));
#endif
}

int CDialog::LVM_QuerySelectedItem (int id)
{
#if defined(__OS2__)
   int RetVal = -1;
   PRECORDCORE pSelected, pRecord;

   if ((pSelected = (PRECORDCORE)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORDEMPHASIS, MPFROMSHORT (CMA_FIRST), MPFROMSHORT (CRA_SELECTED))) != NULL) {
      pRecord = (PRECORDCORE)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, 0L, MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER));
      do {
         RetVal++;
         if (pRecord == pSelected)
            break;
      } while ((pRecord = (PRECORDCORE)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, MPFROMP (pRecord), MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER))) != NULL);
   }

   return (RetVal);
#elif defined(__NT__)
   id = id;

   return (lv_Selected);
#endif
}

VOID CDialog::LVM_QueryItemText (int id, int item, int column, PSZ text)
{
#if defined(__OS2__)
   LV_PLISTDATA pRecord;

   if (item >= 0 && text != NULL) {
      if ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, 0L, MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER))) != NULL)
         do {
            if (item == 0)
               break;
            item--;
         } while ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, MPFROMP (pRecord), MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER))) != NULL);
      if (pRecord != NULL) {
         strcpy (text, pRecord->Column[column].Data);
         pRecord->Column[column].pszData = pRecord->Column[column].Data;
      }
   }
#elif defined(__NT__)
   ListView_GetItemText (GetDlgItem (m_hWnd, id), item, column, text, 128);
#endif
}

int CDialog::LVM_QueryItemCount (int id)
{
#if defined(__OS2__)
   int RetVal = 0;
   LV_PLISTDATA pRecord;

   if ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, 0L, MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER))) != NULL)
      do {
         RetVal++;
      } while ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, MPFROMP (pRecord), MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER))) != NULL);

   return (RetVal);
#elif defined(__NT__)
   return (ListView_GetItemCount (GetDlgItem (m_hWnd, id)));
#endif
}

VOID CDialog::LVM_SetItemText (int id, int item, int column, PSZ text)
{
#if defined(__OS2__)
   LV_PLISTDATA pRecord;

   if (item >= 0 && text != NULL) {
      if ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, 0L, MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER))) != NULL)
         do {
            if (item == 0)
               break;
            item--;
         } while ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, MPFROMP (pRecord), MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER))) != NULL);
      if (pRecord != NULL) {
         strcpy (pRecord->Column[column].Data, text);
      }
   }
#elif defined(__NT__)
   ListView_SetItemText (GetDlgItem (m_hWnd, id), item, column, text);
#endif
}

VOID CDialog::LVM_SelectItem (int id, int item)
{
#if defined(__OS2__)
   int i = item;
   LV_PLISTDATA pRecord;

   if (item >= 0) {
      if ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, 0L, MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER))) != NULL)
         do {
            if (i-- == 0)
               break;
         } while ((pRecord = (LV_PLISTDATA)WinSendDlgItemMsg (m_hWnd, id, CM_QUERYRECORD, MPFROMP (pRecord), MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER))) != NULL);
      if (pRecord != NULL) {
         WinSendDlgItemMsg (m_hWnd, id, CM_SETRECORDEMPHASIS, MPFROMP (pRecord), MPFROM2SHORT (TRUE, CRA_SELECTED|CRA_CURSORED));
         if (item == 0)
            WinSendDlgItemMsg (m_hWnd, id, WM_CHAR, MPFROM2SHORT (KC_VIRTUALKEY, 0), MPFROM2SHORT (0, VK_UP));
         else if (item == (LVM_QueryItemCount (id) - 1))
            WinSendDlgItemMsg (m_hWnd, id, WM_CHAR, MPFROM2SHORT (KC_VIRTUALKEY, 0), MPFROM2SHORT (0, VK_DOWN));
         else {
            WinSendDlgItemMsg (m_hWnd, id, WM_CHAR, MPFROM2SHORT (KC_VIRTUALKEY, 0), MPFROM2SHORT (0, VK_DOWN));
            WinSendDlgItemMsg (m_hWnd, id, WM_CHAR, MPFROM2SHORT (KC_VIRTUALKEY, 0), MPFROM2SHORT (0, VK_UP));
         }
      }
   }
#elif defined(__NT__)
   ListView_SetItemState (GetDlgItem (m_hWnd, id), item, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
   ListView_EnsureVisible (GetDlgItem (m_hWnd, id), item, FALSE);
#endif
}

VOID CDialog::LVM_InsertItem (int id)
{
#if defined(__OS2__)
   RECORDINSERT recordInsert;

   if (pRecord != NULL) {
      recordInsert.cb = sizeof (RECORDINSERT);
      recordInsert.pRecordOrder = (PRECORDCORE)CMA_END;
      recordInsert.zOrder = (ULONG)CMA_TOP;
      recordInsert.cRecordsInsert = 1;
      recordInsert.fInvalidateRecord = FALSE;
      recordInsert.pRecordParent = NULL;
      WinSendMsg (WinWindowFromID (m_hWnd, id), CM_INSERTRECORD, MPFROMP (pRecord), MPFROMP (&recordInsert));
   }

   if ((pRecord = (LV_PLISTDATA)WinSendMsg (WinWindowFromID (m_hWnd, id), CM_ALLOCRECORD, (MPARAM)(sizeof (COLUMNDATA) * nFields), (MPARAM)1)) != NULL) {
      pRecord->RecordCore.hptrIcon = NULLHANDLE;
      pRecord->RecordCore.pszIcon = "";
   }
#elif defined(__NT__)
   lvI.iItem = LVM_QueryItemCount (id);
   ListView_InsertItem (GetDlgItem (m_hWnd, id), &lvI);
#endif
}

VOID CDialog::LVM_SetItemText (int id, int column, PSZ text)
{
#if defined(__OS2__)
   id = id;
   strcpy (pRecord->Column[column].Data, text);
   pRecord->Column[column].pszData = pRecord->Column[column].Data;
#elif defined(__NT__)
   ListView_SetItemText (GetDlgItem (m_hWnd, id), lvI.iItem, column, text);
#endif
}

VOID CDialog::LVM_InvalidateView (int id)
{
#if defined(__OS2__)
   RECORDINSERT recordInsert;

   if (pRecord != NULL) {
      recordInsert.cb = sizeof (RECORDINSERT);
      recordInsert.pRecordOrder = (PRECORDCORE)CMA_END;
      recordInsert.zOrder = (ULONG)CMA_TOP;
      recordInsert.cRecordsInsert = 1;
      recordInsert.fInvalidateRecord = FALSE;
      recordInsert.pRecordParent = NULL;
      WinSendMsg (WinWindowFromID (m_hWnd, id), CM_INSERTRECORD, MPFROMP (pRecord), MPFROMP (&recordInsert));

      pRecord = NULL;
   }

   WinSendMsg (WinWindowFromID (m_hWnd, id), CM_INVALIDATEDETAILFIELDINFO, 0L, 0L);
#elif defined(__NT__)
   int i;

   for (i = 0; i < lvC.iSubItem; i++)
      ListView_SetColumnWidth (GetDlgItem (m_hWnd, id), i, LVSCW_AUTOSIZE_USEHEADER);

   ::ShowWindow (GetDlgItem (m_hWnd, id), SW_SHOW);
#endif
}

LONG CDialog::SPBM_QueryValue (int id)
{
   LONG Value;

#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, SPBM_QUERYVALUE, MPFROMP (&Value), MPFROM2SHORT (0, SPBQ_DONOTUPDATE));
#elif defined(__NT__)
   CHAR Temp[32];

   GetDlgItemText (id, Temp, GetDlgItemTextLength (id));
   Value = atol (Temp);
#endif

   return (Value);
}

VOID CDialog::SPBM_SetCurrentValue (int id, LONG value)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, SPBM_SETCURRENTVALUE, MPFROMLONG (value), 0L);
#elif defined(__NT__)
   CHAR Temp[32];

   sprintf (Temp, "%lu", value);
   SetDlgItemText (id, Temp);
#endif
}

VOID CDialog::SPBM_SetLimits (int id, LONG highest, LONG lowest)
{
#if defined(__OS2__)
   WinSendDlgItemMsg (m_hWnd, id, SPBM_SETLIMITS, MPFROMLONG (highest), MPFROMLONG (lowest));
#elif defined(__NT__)
   id = id;
   highest = highest;
   lowest = lowest;
#endif
}

// ----------------------------------------------------------------------
// Windows message processing
// ----------------------------------------------------------------------

VOID CDialog::OnCancel (VOID)
{
   EndDialog (FALSE);
}

VOID CDialog::OnHelp (VOID)
{
}

USHORT CDialog::OnInitDialog (VOID)
{
   return (TRUE);
}

VOID CDialog::OnOK (VOID)
{
   EndDialog (TRUE);
}

BEGIN_MESSAGE_MAP(CDialog, CDialog)
END_MESSAGE_MAP()

