
#if defined(__OS2__)
MRESULT EXPENTRY MessageDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
#elif defined(__NT__)
BOOL CALLBACK MessageDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
{
   static USHORT status = 0;
   static class TMsgData *Data = NULL;

   switch (msg) {
#if defined(__OS2__)
      case WM_INITDLG: {
         USHORT i;
         ULONG Test, nFields;
         HWND hwndContainer;
         CNRINFO cnrInfo;
         PFIELDINFO pFieldInfo, pTopField;
         AREALISTDATA *pRecord;
         FIELDINFOINSERT fieldinsert;
         RECTL rc;
         class TEchoLink *Link;

         if (status == 0) {
            WinQueryWindowRect (hwnd, &rc);
            rc.yTop = (WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN) - (rc.yTop - rc.yBottom)) / 2;
            rc.xLeft = (WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN) - (rc.xRight - rc.xLeft)) / 2;
            WinSetWindowPos (hwnd, NULL, rc.xLeft, rc.yTop, 0, 0, SWP_MOVE);

            Data = new TMsgData (Cfg->SystemPath);
            WinSendDlgItemMsg (hwnd, 102, EM_SETTEXTLIMIT, MPFROMSHORT (sizeof (Data->Key) - 1), 0L);
            WinSendDlgItemMsg (hwnd, 104, EM_SETTEXTLIMIT, MPFROMSHORT (sizeof (Data->Display) - 1), 0L);
            WinSendDlgItemMsg (hwnd, 106, EM_SETTEXTLIMIT, MPFROMSHORT (sizeof (Data->Path) - 1), 0L);
            WinSendDlgItemMsg (hwnd, 126, EM_SETTEXTLIMIT, MPFROMSHORT (sizeof (Data->EchoTag) - 1), 0L);
            WinSendDlgItemMsg (hwnd, 128, EM_SETTEXTLIMIT, MPFROMSHORT (sizeof (Data->NewsGroup) - 1), 0L);

            WinSendDlgItemMsg (hwnd, 107, LM_INSERTITEM, MPFROMSHORT (LIT_END), MPFROMP ("Squish<tm>"));
            WinSendDlgItemMsg (hwnd, 107, LM_INSERTITEM, MPFROMSHORT (LIT_END), MPFROMP ("JAM"));
            WinSendDlgItemMsg (hwnd, 107, LM_INSERTITEM, MPFROMSHORT (LIT_END), MPFROMP ("Fido (*.msg)"));
            WinSendDlgItemMsg (hwnd, 107, LM_INSERTITEM, MPFROMSHORT (LIT_END), MPFROMP ("AdeptXBBS"));
            WinSendDlgItemMsg (hwnd, 107, LM_INSERTITEM, MPFROMSHORT (LIT_END), MPFROMP ("USENET Newsgroup"));

            WinSendDlgItemMsg (hwnd, 138, SPBM_SETLIMITS, MPFROMSHORT (5000U), MPFROMSHORT (0L));
            WinSendDlgItemMsg (hwnd, 136, SPBM_SETLIMITS, MPFROMSHORT (366U), MPFROMSHORT (0L));

            if (Data->First () == FALSE)
               Data->New ();
            WinPostMsg (hwnd, WM_USER, 0L, 0L);
         }
         else if (status == 1) {
            WinSetDlgItemText (hwnd, 128, "");
            if ((Link = new TEchoLink (Cfg->SystemPath)) != NULL) {
               Link->Load (Data->EchoTag);
               if (Link->First () == TRUE)
                  do {
                     WinSendDlgItemMsg (hwnd, 101, LM_INSERTITEM, MPFROMSHORT (LIT_END), MPFROMP (Link->Address));
                  } while (Link->Next () == TRUE);
               delete Link;
            }
         }
         else if (status == 2) {
            WinSendDlgItemMsg (hwnd, 102, SPBM_SETLIMITS, MPFROMSHORT (65535U), MPFROMSHORT (0L));
            WinSendDlgItemMsg (hwnd, 102, SPBM_SETCURRENTVALUE, MPFROMSHORT (Data->Level), 0L);

            for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
               if (Data->AccessFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }
            for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
               if (Data->DenyFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }

            WinSendDlgItemMsg (hwnd, 173, SPBM_SETLIMITS, MPFROMSHORT (65535U), MPFROMSHORT (0L));
            WinSendDlgItemMsg (hwnd, 173, SPBM_SETCURRENTVALUE, MPFROMSHORT (Data->WriteLevel), 0L);

            for (i = 174, Test = 0x80000000L; i <= 205; i++, Test >>= 1) {
               if (Data->WriteFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }
            for (i = 207, Test = 0x80000000L; i <= 238; i++, Test >>= 1) {
               if (Data->DenyWriteFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }
         }
         else if (status == 3) {
            WinSetWindowText (hwnd, "Message Areas List");
            if ((hwndContainer = WinWindowFromID (hwnd, 101)) != NULLHANDLE) {
               memset (&cnrInfo, 0, sizeof (CNRINFO));
               cnrInfo.cb = sizeof (CNRINFO);
               cnrInfo.flWindowAttr = CV_DETAIL|CA_DETAILSVIEWTITLES;
               WinSendMsg (hwndContainer, CM_SETCNRINFO, MPFROMP (&cnrInfo), MPFROMLONG (CMA_FLWINDOWATTR));

               nFields = 4;
               pFieldInfo = (PFIELDINFO)WinSendMsg (hwndContainer, CM_ALLOCDETAILFIELDINFO, (MPARAM)nFields, (MPARAM)0);
               pTopField = pFieldInfo;

               pFieldInfo->cb = sizeof (FIELDINFO);
               pFieldInfo->pTitleData = "Key";
               pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_LEFT|CFA_SEPARATOR|CFA_HORZSEPARATOR;
               pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
               pFieldInfo->offStruct = (ULONG)&pRecord->pszKey - (ULONG)pRecord;
               pFieldInfo->pUserData = NULL;
               pFieldInfo->cxWidth = 0L;

               pFieldInfo = pFieldInfo->pNextFieldInfo;
               pFieldInfo->cb = sizeof (FIELDINFO);
               pFieldInfo->pTitleData = "Level";
               pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR|CFA_HORZSEPARATOR;
               pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
               pFieldInfo->offStruct = (ULONG)&pRecord->pszNumber - (ULONG)pRecord;
               pFieldInfo->pUserData = NULL;
               pFieldInfo->cxWidth = 0L;

               pFieldInfo = pFieldInfo->pNextFieldInfo;
               pFieldInfo->cb = sizeof (FIELDINFO);
               pFieldInfo->pTitleData = "Write";
               pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR|CFA_HORZSEPARATOR;
               pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
               pFieldInfo->offStruct = (ULONG)&pRecord->pszWrite - (ULONG)pRecord;
               pFieldInfo->pUserData = NULL;
               pFieldInfo->cxWidth = 0L;

               pFieldInfo = pFieldInfo->pNextFieldInfo;
               pFieldInfo->cb = sizeof (FIELDINFO);
               pFieldInfo->pTitleData = "Description";
               pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_LEFT|CFA_SEPARATOR|CFA_HORZSEPARATOR;
               pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
               pFieldInfo->offStruct = (ULONG)&pRecord->pszDescription - (ULONG)pRecord;
               pFieldInfo->pUserData = NULL;
               pFieldInfo->cxWidth = 0L;

               fieldinsert.cb = sizeof (FIELDINFOINSERT);
               fieldinsert.pFieldInfoOrder = (PFIELDINFO)CMA_FIRST;
               fieldinsert.fInvalidateFieldInfo = TRUE;
               fieldinsert.cFieldInfoInsert = nFields;
               WinSendMsg (hwndContainer, CM_INSERTDETAILFIELDINFO, MPFROMP (pTopField), MPFROMP (&fieldinsert));
            }
            WinPostMsg (hwnd, WM_USER + 2, 0L, 0L);
         }
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

      case WM_USER:
         if (Data != NULL) {
#if defined(__OS2__)
            WinSetDlgItemText (hwnd, 102, Data->Key);
            WinSetDlgItemText (hwnd, 104, Data->Display);
            WinSetDlgItemText (hwnd, 106, Data->Path);
            switch (Data->Storage) {
               case ST_SQUISH:
                  WinSendDlgItemMsg (hwnd, 107, LM_SELECTITEM, MPFROMSHORT (0), MPFROMSHORT (TRUE));
                  break;
               case ST_JAM:
                  WinSendDlgItemMsg (hwnd, 107, LM_SELECTITEM, MPFROMSHORT (1), MPFROMSHORT (TRUE));
                  break;
               case ST_FIDO:
                  WinSendDlgItemMsg (hwnd, 107, LM_SELECTITEM, MPFROMSHORT (2), MPFROMSHORT (TRUE));
                  break;
               case ST_ADEPT:
                  WinSendDlgItemMsg (hwnd, 107, LM_SELECTITEM, MPFROMSHORT (3), MPFROMSHORT (TRUE));
                  break;
               case ST_USENET:
                  WinSendDlgItemMsg (hwnd, 107, LM_SELECTITEM, MPFROMSHORT (4), MPFROMSHORT (TRUE));
                  break;
            }
            WinSendDlgItemMsg (hwnd, 113, BM_SETCHECK, MPFROMSHORT (Data->ShowGlobal), 0L);
            WinSendDlgItemMsg (hwnd, 114, BM_SETCHECK, MPFROMSHORT (Data->Offline), 0L);
            WinSendDlgItemMsg (hwnd, 129, BM_SETCHECK, MPFROMSHORT (Data->EchoMail), 0L);
            WinSendDlgItemMsg (hwnd, 130, BM_SETCHECK, MPFROMSHORT (Data->UpdateNews), 0L);
            WinSetDlgItemText (hwnd, 126, Data->EchoTag);
            WinSetDlgItemText (hwnd, 128, Data->NewsGroup);
            WinSendDlgItemMsg (hwnd, 138, SPBM_SETCURRENTVALUE, MPFROMSHORT (Data->MaxMessages), 0L);
            WinSendDlgItemMsg (hwnd, 136, SPBM_SETCURRENTVALUE, MPFROMSHORT (Data->DaysOld), 0L);
#elif defined(__NT__)
#endif
         }
         break;

      case WM_USER + 1:
         if (Data != NULL) {
#if defined(__OS2__)
            ULONG Value;

            WinQueryDlgItemText (hwnd, 102, WinQueryDlgItemTextLength (hwnd, 102) + 1, Data->Key);
            WinQueryDlgItemText (hwnd, 104, WinQueryDlgItemTextLength (hwnd, 104) + 1, Data->Display);
            WinQueryDlgItemText (hwnd, 106, WinQueryDlgItemTextLength (hwnd, 106) + 1, Data->Path);
            switch ((USHORT)WinSendDlgItemMsg (hwnd, 107, LM_QUERYSELECTION, MPFROMSHORT (LIT_FIRST), 0L)) {
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
            }
            Data->ShowGlobal = (CHAR)WinSendDlgItemMsg (hwnd, 113, BM_QUERYCHECK, 0L, 0L);
            Data->Offline = (CHAR)WinSendDlgItemMsg (hwnd, 114, BM_QUERYCHECK, 0L, 0L);
            Data->EchoMail = (CHAR)WinSendDlgItemMsg (hwnd, 129, BM_QUERYCHECK, 0L, 0L);
            Data->UpdateNews = (CHAR)WinSendDlgItemMsg (hwnd, 130, BM_QUERYCHECK, 0L, 0L);
            WinQueryDlgItemText (hwnd, 126, WinQueryDlgItemTextLength (hwnd, 126) + 1, Data->EchoTag);
            WinQueryDlgItemText (hwnd, 128, WinQueryDlgItemTextLength (hwnd, 128) + 1, Data->NewsGroup);
            WinSendDlgItemMsg (hwnd, 138, SPBM_QUERYVALUE, MPFROMP (&Value), MPFROM2SHORT (0, SPBQ_DONOTUPDATE));
            Data->MaxMessages = (USHORT)Value;
            WinSendDlgItemMsg (hwnd, 136, SPBM_QUERYVALUE, MPFROMP (&Value), MPFROM2SHORT (0, SPBQ_DONOTUPDATE));
            Data->DaysOld = (USHORT)Value;
#elif defined(__NT__)
#endif
         }
         break;

      case WM_USER + 2: {
#if defined(__OS2__)
         HWND hwndContainer;
         AREALISTDATA *pRecord;
         RECORDINSERT recordInsert;
         class TMsgData *Msg;

         hwndContainer = WinWindowFromID (hwnd, 101);
         WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));

         if ((Msg = new TMsgData (Cfg->SystemPath)) != NULL) {
            if (Msg->First () == TRUE)
               do {
                  if ((pRecord = (AREALISTDATA *)WinSendMsg (hwndContainer, CM_ALLOCRECORD, (MPARAM)(sizeof (AREALISTDATA) - sizeof (RECORDCORE)), (MPARAM)1)) != NULL) {
                     pRecord->RecordCore.hptrIcon = NULLHANDLE;
                     pRecord->RecordCore.pszIcon = "";

                     strcpy (pRecord->Key, Msg->Key);
                     strcpy (pRecord->Description, Msg->Display);
                     sprintf (pRecord->Number, "%u", Msg->Level);
                     sprintf (pRecord->Write, "%u", Msg->WriteLevel);

                     pRecord->pszKey = pRecord->Key;
                     pRecord->pszDescription = pRecord->Description;
                     pRecord->pszNumber = pRecord->Number;
                     pRecord->pszWrite = pRecord->Write;

                     recordInsert.cb = sizeof (RECORDINSERT);
                     recordInsert.pRecordOrder = (PRECORDCORE)CMA_END;
                     recordInsert.zOrder = (ULONG)CMA_TOP;
                     recordInsert.cRecordsInsert = 1;
                     recordInsert.fInvalidateRecord = FALSE;
                     recordInsert.pRecordParent = NULL;
                     WinSendMsg (hwndContainer, CM_INSERTRECORD, MPFROMP (pRecord), MPFROMP (&recordInsert));
                  }
               } while (Msg->Next () == TRUE);
            delete Msg;
         }

         WinSendMsg (hwndContainer, CM_INVALIDATEDETAILFIELDINFO, 0L, 0L);
         WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE));
#elif defined(__NT__)
#endif
         break;
      }

#if defined(__OS2__)
      case WM_CONTROL:
         if (SHORT1FROMMP (mp1) == 101 && status == 3 && SHORT2FROMMP (mp1) == CN_ENTER)
            WinPostMsg (hwnd, WM_COMMAND, MPFROMSHORT (IDOK), 0L);
         break;
#elif defined(__NT__)
#endif

      case WM_COMMAND:
#if defined(__OS2__)
         switch (SHORT1FROMMP (mp1)) {
            case 115:      // Add
               Data->New ();
               WinSendMsg (hwnd, WM_USER + 1, 0L, 0L);
               Data->Add ();
               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               break;
            case 116:      // Insert
               Data->New ();
               WinSendMsg (hwnd, WM_USER + 1, 0L, 0L);
               Data->Insert ();
               WinPostMsg (hwnd, WM_USER, 0L, 0L);
               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               break;
            case 118:      // Previous
               if (Data->Previous () == TRUE)
                  WinPostMsg (hwnd, WM_USER, 0L, 0L);
               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               break;
            case 119:      // Next
               if (Data->Next () == TRUE)
                  WinPostMsg (hwnd, WM_USER, 0L, 0L);
               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               break;
            case 120:      // Delete
               if (WinMessageBox (HWND_DESKTOP, hwnd, "Are you sure ?", "Delete", 0, MB_YESNO|MB_ICONQUESTION|MB_MOVEABLE) == MBID_YES) {
                  Data->Delete ();
                  WinPostMsg (hwnd, WM_USER, 0L, 0L);
                  WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               }
               break;
            case 123:      // Security
               status = 2;
#if defined(__OS2__)
               WinDlgBox (HWND_DESKTOP, NULLHANDLE, MessageDlgProc, NULLHANDLE, 15, NULL);
#elif defined(__NT__)
               Win95DialogBox (hinst, "MSGSECURITY", hwnd, (DLGPROC)MessageDlgProc);
#endif
               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               status = 0;
               break;
            case 117:      // List
               status = 3;
#if defined(__OS2__)
               if ((USHORT)WinDlgBox (HWND_DESKTOP, NULLHANDLE, MessageDlgProc, NULLHANDLE, 25, NULL) == TRUE)
#elif defined(__NT__)
               if (Win95DialogBox (hinst, "LIST", hwnd, (DLGPROC)MessageDlgProc) == TRUE)
#endif
                  WinPostMsg (hwnd, WM_USER, 0L, 0L);
               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               status = 0;
               break;
            case 121:      // Links
               status = 1;
#if defined(__OS2__)
               WinDlgBox (HWND_DESKTOP, NULLHANDLE, MessageDlgProc, NULLHANDLE, 24, NULL);
#elif defined(__NT__)
               Win95DialogBox (hinst, "MSGLINKS", hwnd, (DLGPROC)MessageDlgProc);
#endif
               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               status = 0;
               break;
            case 102: {    // Links / Add
               CHAR Temp[128];

               if (status == 1) {
                  WinQueryDlgItemText (hwnd, 128, WinQueryDlgItemTextLength (hwnd, 128) + 1, Temp);
                  if (Temp[0] != '\0')
                     WinSendDlgItemMsg (hwnd, 101, LM_INSERTITEM, MPFROMSHORT (LIT_END), MPFROMP (Temp));
                  WinSetDlgItemText (hwnd, 128, "");
                  WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 128));
               }
               break;
            }
            case 103: {    // Links / Delete
               int item;

               if (status == 1) {
                  if (WinMessageBox (HWND_DESKTOP, hwnd, "Are you sure ?", "Delete", 0, MB_YESNO|MB_ICONQUESTION|MB_MOVEABLE) == MBID_YES) {
                     if ((item = (int)WinSendDlgItemMsg (hwnd, 101, LM_QUERYSELECTION, MPFROMSHORT (LIT_FIRST), 0L)) != LIT_NONE)
                        WinSendDlgItemMsg (hwnd, 101, LM_DELETEITEM, MPFROMSHORT (item), 0L);
                  }
               }
               break;
            }
            case IDOK: {
               USHORT i, Items;
               CHAR OldKey[16], Temp[128];
               ULONG Test, Value;
               AREALISTDATA *pRecord;
               class TMsgTag *MsgTag;
               class TEchoLink *Link;

               if (status == 0) {
                  strcpy (OldKey, Data->Key);
                  WinSendMsg (hwnd, WM_USER + 1, 0L, 0L);
                  Data->Update ();
                  if (strcmp (OldKey, Data->Key)) {
                     if ((MsgTag = new TMsgTag (Cfg->UserFile)) != NULL) {
                        MsgTag->Change (OldKey, Data->Key);
                        delete MsgTag;
                     }
                  }
                  WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               }
               else if (status == 1) {
                  if ((Link = new TEchoLink (Cfg->SystemPath)) != NULL) {
                     Link->Load (Data->EchoTag);
                     Link->Clear ();
                  }

                  if ((Items = (USHORT)WinSendDlgItemMsg (hwnd, 101, LM_QUERYITEMCOUNT, 0L, 0L)) != 0) {
                     for (i = 0; i < Items; i++) {
                        WinSendDlgItemMsg (hwnd, 101, LM_QUERYITEMTEXT, MPFROM2SHORT (i, sizeof (Temp)), MPFROMP (Temp));
                        Link->AddString (Temp);
                     }
                  }

                  if (Link != NULL) {
                     Link->Save ();
                     delete Link;
                  }

                  WinDismissDlg (hwnd, TRUE);
               }
               else if (status == 2) {
                  WinSendDlgItemMsg (hwnd, 102, SPBM_QUERYVALUE, MPFROMP (&Value), MPFROM2SHORT (0, SPBQ_DONOTUPDATE));
                  Data->Level = (USHORT)Value;
                  WinSendDlgItemMsg (hwnd, 173, SPBM_QUERYVALUE, MPFROMP (&Value), MPFROM2SHORT (0, SPBQ_DONOTUPDATE));
                  Data->WriteLevel = (USHORT)Value;

                  Data->AccessFlags = 0L;
                  for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->AccessFlags |= Test;
                  }
                  Data->DenyFlags = 0L;
                  for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->DenyFlags |= Test;
                  }

                  Data->WriteFlags = 0L;
                  for (i = 174, Test = 0x80000000L; i <= 205; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->WriteFlags |= Test;
                  }
                  Data->DenyWriteFlags = 0L;
                  for (i = 207, Test = 0x80000000L; i <= 238; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->DenyWriteFlags |= Test;
                  }

                  WinDismissDlg (hwnd, TRUE);
               }
               else if (status == 3) {
                  if ((pRecord = (AREALISTDATA *)WinSendDlgItemMsg (hwnd, 101, CM_QUERYRECORDEMPHASIS, MPFROMSHORT (CMA_FIRST), MPFROMSHORT (CRA_SELECTED))) != NULL)
                     Data->Read (pRecord->Key, FALSE);
/*
                  if ((i = (USHORT)WinSendDlgItemMsg (hwnd, 101, LM_QUERYSELECTION, MPFROMSHORT (LIT_FIRST), 0L)) != LIT_NONE) {
                     WinSendDlgItemMsg (hwnd, 101, LM_QUERYITEMTEXT, MPFROM2SHORT (i, sizeof (Temp)), MPFROMP (Temp));
                     if (Data->First () == TRUE)
                        do {
                           sprintf (Check, "%-15.15s %s", Data->Key, Data->Display);
                           if (!strcmp (Temp, Check))
                              break;
                        } while (Data->Next () == TRUE);
                  }
*/
                  WinDismissDlg (hwnd, TRUE);
               }
               break;
            }
            case IDCANCEL:
               if (status != 0)
                  WinDismissDlg (hwnd, FALSE);
               break;
         }
         return ((MRESULT)FALSE);
#elif defined(__NT__)
         switch (wParam) {
            case IDOK:
               Data->Update ();
               break;
         }
         break;
#endif

      case WM_CLOSE:
#if defined(__NT__)
         EndDialog (hwnd, FALSE);
#endif
         if (status == 0) {
            if (Data != NULL) {
               delete Data;
               Data = NULL;
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

#if defined(__OS2__)
MRESULT EXPENTRY FileDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
#elif defined(__NT__)
BOOL CALLBACK FileDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
{
   static USHORT status = 0;
   static class TFileData *Data = NULL;

   switch (msg) {
#if defined(__OS2__)
      case WM_INITDLG: {
         int i;
         LONG nFields;
         ULONG Test;
         HWND hwndContainer;
         CNRINFO cnrInfo;
         PFIELDINFO pFieldInfo, pTopField;
         AREALISTDATA *pRecord;
         FIELDINFOINSERT fieldinsert;
         RECTL rc;

         if (status == 0) {
         }
         else if (status == 2) {
            WinSendDlgItemMsg (hwnd, 102, SPBM_SETLIMITS, MPFROMSHORT (65535U), MPFROMSHORT (0L));
            WinSendDlgItemMsg (hwnd, 102, SPBM_SETCURRENTVALUE, MPFROMSHORT (Data->Level), 0L);

            for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
               if (Data->AccessFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }
            for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
               if (Data->DenyFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }

            WinSendDlgItemMsg (hwnd, 173, SPBM_SETLIMITS, MPFROMSHORT (65535U), MPFROMSHORT (0L));
            WinSendDlgItemMsg (hwnd, 173, SPBM_SETCURRENTVALUE, MPFROMSHORT (Data->DownloadLevel), 0L);

            for (i = 174, Test = 0x80000000L; i <= 205; i++, Test >>= 1) {
               if (Data->DownloadFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }
            for (i = 207, Test = 0x80000000L; i <= 238; i++, Test >>= 1) {
               if (Data->DownloadDenyFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }

            WinSendDlgItemMsg (hwnd, 242, SPBM_SETLIMITS, MPFROMSHORT (65535U), MPFROMSHORT (0L));
            WinSendDlgItemMsg (hwnd, 242, SPBM_SETCURRENTVALUE, MPFROMSHORT (Data->UploadLevel), 0L);

            for (i = 243, Test = 0x80000000L; i <= 274; i++, Test >>= 1) {
               if (Data->UploadFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }
            for (i = 276, Test = 0x80000000L; i <= 307; i++, Test >>= 1) {
               if (Data->UploadDenyFlags & Test)
                  WinSendDlgItemMsg (hwnd, i, BM_SETCHECK, MPFROMSHORT (TRUE), 0L);
            }
         }
         else if (status == 3) {
            WinSetWindowText (hwnd, "File Areas List");

            if ((hwndContainer = WinWindowFromID (hwnd, 101)) != NULLHANDLE) {
               memset (&cnrInfo, 0, sizeof (CNRINFO));
               cnrInfo.cb = sizeof (CNRINFO);
               cnrInfo.flWindowAttr = CV_DETAIL|CA_DETAILSVIEWTITLES;
               WinSendMsg (hwndContainer, CM_SETCNRINFO, MPFROMP (&cnrInfo), MPFROMLONG (CMA_FLWINDOWATTR));

               nFields = 5;
               pFieldInfo = (PFIELDINFO)WinSendMsg (hwndContainer, CM_ALLOCDETAILFIELDINFO, (MPARAM)nFields, (MPARAM)0);
               pTopField = pFieldInfo;

               pFieldInfo->cb = sizeof (FIELDINFO);
               pFieldInfo->pTitleData = "Key";
               pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_LEFT|CFA_SEPARATOR|CFA_HORZSEPARATOR;
               pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
               pFieldInfo->offStruct = (ULONG)&pRecord->pszKey - (ULONG)pRecord;
               pFieldInfo->pUserData = NULL;
               pFieldInfo->cxWidth = 0L;

               pFieldInfo = pFieldInfo->pNextFieldInfo;
               pFieldInfo->cb = sizeof (FIELDINFO);
               pFieldInfo->pTitleData = "Level";
               pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR|CFA_HORZSEPARATOR;
               pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
               pFieldInfo->offStruct = (ULONG)&pRecord->pszNumber - (ULONG)pRecord;
               pFieldInfo->pUserData = NULL;
               pFieldInfo->cxWidth = 0L;

               pFieldInfo = pFieldInfo->pNextFieldInfo;
               pFieldInfo->cb = sizeof (FIELDINFO);
               pFieldInfo->pTitleData = "Download";
               pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR|CFA_HORZSEPARATOR;
               pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
               pFieldInfo->offStruct = (ULONG)&pRecord->pszDownload - (ULONG)pRecord;
               pFieldInfo->pUserData = NULL;
               pFieldInfo->cxWidth = 0L;

               pFieldInfo = pFieldInfo->pNextFieldInfo;
               pFieldInfo->cb = sizeof (FIELDINFO);
               pFieldInfo->pTitleData = "Upload";
               pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR|CFA_HORZSEPARATOR;
               pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
               pFieldInfo->offStruct = (ULONG)&pRecord->pszUpload - (ULONG)pRecord;
               pFieldInfo->pUserData = NULL;
               pFieldInfo->cxWidth = 0L;

               pFieldInfo = pFieldInfo->pNextFieldInfo;
               pFieldInfo->cb = sizeof (FIELDINFO);
               pFieldInfo->pTitleData = "Description";
               pFieldInfo->flData = CFA_STRING|CFA_VCENTER|CFA_LEFT|CFA_SEPARATOR|CFA_HORZSEPARATOR;
               pFieldInfo->flTitle = CFA_FITITLEREADONLY|CFA_VCENTER|CFA_CENTER|CFA_SEPARATOR;
               pFieldInfo->offStruct = (ULONG)&pRecord->pszDescription - (ULONG)pRecord;
               pFieldInfo->pUserData = NULL;
               pFieldInfo->cxWidth = 0L;

               fieldinsert.cb = sizeof (FIELDINFOINSERT);
               fieldinsert.pFieldInfoOrder = (PFIELDINFO)CMA_FIRST;
               fieldinsert.fInvalidateFieldInfo = TRUE;
               fieldinsert.cFieldInfoInsert = nFields;
               WinSendMsg (hwndContainer, CM_INSERTDETAILFIELDINFO, MPFROMP (pTopField), MPFROMP (&fieldinsert));
            }
            WinPostMsg (hwnd, WM_USER + 2, 0L, 0L);
         }
         break;
      }
#elif defined(__NT__)
      case WM_INITDIALOG: {
         RECT rc;

         lParam = lParam;
         if (status == 0) {
         }
         else if (status == 3) {
            SetWindowText (hwnd, "File Areas List");
            PostMessage (hwnd, WM_USER + 2, 0L, 0L);
         }
         return ((BOOL)TRUE);
      }

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
      case WM_USER + 2: {
         HWND hwndContainer;
         AREALISTDATA *pRecord;
         RECORDINSERT recordInsert;
         class TFileData *File;

         hwndContainer = WinWindowFromID (hwnd, 101);
         WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));

         if ((File = new TFileData (Cfg->SystemPath)) != NULL) {
            if (File->First () == TRUE)
               do {
                  if ((pRecord = (AREALISTDATA *)WinSendMsg (hwndContainer, CM_ALLOCRECORD, (MPARAM)(sizeof (AREALISTDATA) - sizeof (RECORDCORE)), (MPARAM)1)) != NULL) {
                     pRecord->RecordCore.hptrIcon = NULLHANDLE;
                     pRecord->RecordCore.pszIcon = "";

                     strcpy (pRecord->Key, File->Key);
                     strcpy (pRecord->Description, File->Display);
                     sprintf (pRecord->Number, "%u", File->Level);
                     sprintf (pRecord->Download, "%u", File->DownloadLevel);
                     sprintf (pRecord->Upload, "%u", File->UploadLevel);

                     pRecord->pszKey = pRecord->Key;
                     pRecord->pszDescription = pRecord->Description;
                     pRecord->pszNumber = pRecord->Number;
                     pRecord->pszUpload = pRecord->Upload;
                     pRecord->pszDownload = pRecord->Download;

                     recordInsert.cb = sizeof (RECORDINSERT);
                     recordInsert.pRecordOrder = (PRECORDCORE)CMA_END;
                     recordInsert.zOrder = (ULONG)CMA_TOP;
                     recordInsert.cRecordsInsert = 1;
                     recordInsert.fInvalidateRecord = FALSE;
                     recordInsert.pRecordParent = NULL;
                     WinSendMsg (hwndContainer, CM_INSERTRECORD, MPFROMP (pRecord), MPFROMP (&recordInsert));
                  }
               } while (File->Next () == TRUE);
            delete File;
         }

         WinSendMsg (hwndContainer, CM_INVALIDATEDETAILFIELDINFO, 0L, 0L);
         WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE));
         break;
      }
#endif

#if defined(__OS2__)
      case WM_CONTROL:
         if (SHORT1FROMMP (mp1) == 101 && status == 3 && SHORT2FROMMP (mp1) == CN_ENTER)
            WinPostMsg (hwnd, WM_COMMAND, MPFROMSHORT (IDOK), 0L);
         break;
#elif defined(__NT__)
#endif

      case WM_COMMAND:
#if defined(__OS2__)
         switch (SHORT1FROMMP (mp1)) {
#elif defined(__NT__)
         switch (wParam) {
#endif
            case 120:      // Delete
#if defined(__OS2__)
               if (WinMessageBox (HWND_DESKTOP, hwnd, "Are you sure ?", "Delete", 0, MB_YESNO|MB_ICONQUESTION|MB_MOVEABLE) == MBID_YES) {
#elif defined(__NT__)
               if (MessageBox (hwnd, "Are you sure ?", "Delete", MB_YESNO|MB_ICONQUESTION) == IDYES) {
#endif
                  Data->Delete ();
#if defined(__OS2__)
                  WinPostMsg (hwnd, WM_USER, 0L, 0L);
                  WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
#elif defined(__NT__)
                  PostMessage (hwnd, WM_USER, 0, 0L);
#endif
               }
               break;
            case 109:      // Security
               status = 2;
#if defined(__OS2__)
               WinDlgBox (HWND_DESKTOP, NULLHANDLE, FileDlgProc, NULLHANDLE, 16, NULL);
               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
#elif defined(__NT__)
               DialogBox (hinst, "FILESECURITY", hwnd, (DLGPROC)FileDlgProc);
#endif
               status = 0;
               break;
            case 117:      // List
               status = 3;
#if defined(__OS2__)
               if ((USHORT)WinDlgBox (HWND_DESKTOP, NULLHANDLE, FileDlgProc, NULLHANDLE, 25, NULL) == TRUE)
                  WinPostMsg (hwnd, WM_USER, 0L, 0L);
               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
#elif defined(__NT__)
               if (DialogBox (hinst, "LIST", hwnd, (DLGPROC)FileDlgProc) == TRUE)
                  PostMessage (hwnd, WM_USER, 0L, 0L);
#endif
               status = 0;
               break;
            case IDOK: {
#if defined(__OS2__)
               USHORT i;
               ULONG Test, Value;
               AREALISTDATA *pRecord;

               if (status == 0) {
                  WinSendMsg (hwnd, WM_USER + 1, 0L, 0L);
                  Data->Update ();
                  WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, 102));
               }
               else if (status == 2) {
                  WinSendDlgItemMsg (hwnd, 102, SPBM_QUERYVALUE, MPFROMP (&Value), MPFROM2SHORT (0, SPBQ_DONOTUPDATE));
                  Data->Level = (USHORT)Value;
                  WinSendDlgItemMsg (hwnd, 173, SPBM_QUERYVALUE, MPFROMP (&Value), MPFROM2SHORT (0, SPBQ_DONOTUPDATE));
                  Data->DownloadLevel = (USHORT)Value;
                  WinSendDlgItemMsg (hwnd, 242, SPBM_QUERYVALUE, MPFROMP (&Value), MPFROM2SHORT (0, SPBQ_DONOTUPDATE));
                  Data->UploadLevel = (USHORT)Value;

                  Data->AccessFlags = 0L;
                  for (i = 104, Test = 0x80000000L; i <= 135; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->AccessFlags |= Test;
                  }
                  Data->DenyFlags = 0L;
                  for (i = 137, Test = 0x80000000L; i <= 168; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->DenyFlags |= Test;
                  }

                  Data->DownloadFlags = 0L;
                  for (i = 174, Test = 0x80000000L; i <= 205; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->DownloadFlags |= Test;
                  }
                  Data->DownloadDenyFlags = 0L;
                  for (i = 207, Test = 0x80000000L; i <= 238; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->DownloadDenyFlags |= Test;
                  }

                  Data->UploadFlags = 0L;
                  for (i = 243, Test = 0x80000000L; i <= 274; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->UploadFlags |= Test;
                  }
                  Data->UploadDenyFlags = 0L;
                  for (i = 276, Test = 0x80000000L; i <= 307; i++, Test >>= 1) {
                     if ((CHAR)WinSendDlgItemMsg (hwnd, i, BM_QUERYCHECK, 0L, 0L) == TRUE)
                        Data->UploadDenyFlags |= Test;
                  }

                  WinDismissDlg (hwnd, TRUE);
               }
               else if (status == 3) {
                  if ((pRecord = (AREALISTDATA *)WinSendDlgItemMsg (hwnd, 101, CM_QUERYRECORDEMPHASIS, MPFROMSHORT (CMA_FIRST), MPFROMSHORT (CRA_SELECTED))) != NULL)
                     Data->Read (pRecord->Key, FALSE);
                  WinDismissDlg (hwnd, TRUE);
               }
#elif defined(__NT__)
               if (status == 0) {
                  SendMessage (hwnd, WM_USER + 1, 0, 0L);
                  Data->Update ();
               }
#endif
               break;
            }
            case IDCANCEL:
               if (status != 0)
#if defined(__OS2__)
                  WinDismissDlg (hwnd, FALSE);
#elif defined(__NT__)
                  EndDialog (hwnd, FALSE);
#endif
               break;
         }
#if defined(__OS2__)
         return ((MRESULT)FALSE);
#elif defined(__NT__)
         break;
#endif

      case WM_CLOSE:
         if (status == 0) {
            if (Data != NULL) {
               delete Data;
               Data = NULL;
            }
         }
#if defined(__NT__)
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

