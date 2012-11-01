
// --------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.13
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// BBS-spcific class definitions. Used internally only.
//
// History list:
//    23/04/95 - Initial coding
// --------------------------------------------------------------------

#include "_ldefs.h"
#include "menu.h"
#include "version.h"

TMenu::TMenu (class TBbs *bbs)
{
   class TMenuEmbedded *ME;

   Lang = bbs->Lang;
   Log = bbs->Log;
   User = bbs->User;
   Cfg = bbs->Cfg;
   Com = bbs->Com;
   Snoop = bbs->Snoop;

   Bbs = bbs;

   usStackPos = 0;
   Library = new TLibrary (Bbs);
   Message = new TMessage (Bbs);
   Editor = NULL;

   if ((ME = new TMenuEmbedded (Bbs)) != NULL) {
      ME->Library = Library;
      ME->Message = Message;
   }
   PS = ME;
}

TMenu::~TMenu (void)
{
   if (PS != NULL)
      delete PS;
   if (Message != NULL)
      delete Message;
   if (Library != NULL)
      delete Library;
   if (Editor!= NULL)
      delete Editor;
}

// --------------------------------------------------------------------
// Descrizione:
//    Esegue il contenuto di un menu'.
//
// Argomenti:
//    pszName = Nome del menu' da eseguire senza l'estenzione .MNU
//
// Valori di ritorno:
//    Nessuno.
// --------------------------------------------------------------------

USHORT TMenu::Run (PSZ pszName)
{
   int fd;
   USHORT i, fExecuted, mRet, OkDisplay;
   CHAR szName[128];

   strcpy (szMenu, pszName);
   item = NULL;
   usDoRead = TRUE;
   usFullMenu = TRUE;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Browsing");
      Bbs->Status->Update ();
   }

   while (Bbs->AbortSession () == FALSE) {
      if (usDoRead == TRUE) {
         sprintf (szName, "%s%s.MNU", Cfg->MenuPath, szMenu);
         if ((fd = sopen (szName, O_RDONLY|O_BINARY, SH_DENYNO)) == -1) {
            Log->Write (Log->MenuNotFound, strupr (szName));
            return (FALSE);
         }

         read (fd, &menuhdr, sizeof (menuhdr));
         if (item != NULL)
            free (item);
         item = (ITEM *)malloc (sizeof (ITEM) * menuhdr.usItems);
         read (fd, item, sizeof (ITEM) * menuhdr.usItems);

         close (fd);
      }

//      if (usFullMenu == TRUE) {
         for (i = 0; i < menuhdr.usItems; i++) {
            if (item[i].ucColor == 0)
               item[i].ucColor = menuhdr.ucColor;
            if (item[i].ucHilight == 0)
               item[i].ucHilight = menuhdr.ucHilight;
            if (User->Level >= item[i].usLevel) {
               if ((User->AccessFlags & item[i].ulAccessFlags) == item[i].ulAccessFlags) {
                  OkDisplay = TRUE;
                  if (item[i].Flags & MF_DSPNORIP && Bbs->Rip == TRUE)
                     OkDisplay = FALSE;
                  if (item[i].Flags & MF_DSPRIP && Bbs->Rip == FALSE)
                     OkDisplay = FALSE;
                  if (item[i].Flags & MF_DSPUSRREMOTE && Bbs->Remote == FALSE)
                     OkDisplay = FALSE;
                  if (item[i].Flags & MF_DSPUSRLOCAL && Bbs->Remote == TRUE)
                     OkDisplay = FALSE;
                  if (item[i].Flags & MF_ONENTER && usDoRead == FALSE)
                     OkDisplay = FALSE;

                  if (OkDisplay == TRUE) {
                     DisplayItem (&item[i]);
                     if (item[i].Flags & (MF_AUTOEXEC|MF_ONENTER))
                        mRet = ExecuteOption (&item[i]);
                  }
               }
            }
         }
//      }

      usDoRead = FALSE;
      fExecuted = FALSE;
      DisplayPrompt (menuhdr.szPrompt, menuhdr.ucColor, menuhdr.ucHilight);
      GetCommand ();

      for (i = 0; i < menuhdr.usItems; i++) {
         OkDisplay = TRUE;
         if (item[i].Flags & MF_DSPNORIP && Bbs->Rip == TRUE)
            OkDisplay = FALSE;
         if (item[i].Flags & MF_DSPRIP && Bbs->Rip == FALSE)
            OkDisplay = FALSE;
         if (item[i].Flags & MF_DSPUSRREMOTE && Bbs->Remote == FALSE)
            OkDisplay = FALSE;
         if (item[i].Flags & MF_DSPUSRLOCAL && Bbs->Remote == TRUE)
            OkDisplay = FALSE;

         if (OkDisplay == TRUE) {
            if (item[i].szKey[0] && !stricmp (szCommand, item[i].szKey)) {
               usFullMenu = FALSE;
               if (User->Level >= item[i].usLevel) {
                  if ((User->AccessFlags & item[i].ulAccessFlags) == item[i].ulAccessFlags) {
                     mRet = ExecuteOption (&item[i]);
                     fExecuted = TRUE;
                  }
               }
            }
            else if (isdigit (item[i].szKey[0]) && isdigit (szCommand[0]) && item[i].szKey[0] == szCommand[0]) {
               usFullMenu = FALSE;
               if (User->Level >= item[i].usLevel) {
                  if ((User->AccessFlags & item[i].ulAccessFlags) == item[i].ulAccessFlags) {
                     mRet = ExecuteOption (&item[i]);
                     fExecuted = TRUE;
                  }
               }
            }
            else if (szCommand[0] == '\0' && !stricmp ("|", item[i].szKey)) {
               usFullMenu = FALSE;
               if (User->Level >= item[i].usLevel) {
                  if ((User->AccessFlags & item[i].ulAccessFlags) == item[i].ulAccessFlags) {
                     mRet = ExecuteOption (&item[i]);
                     fExecuted = TRUE;
                  }
               }
            }
         }
      }

      if (fExecuted == FALSE) {
         if (szCommand[0] == '\0') {
            if (usFullMenu == FALSE)
               usFullMenu = TRUE;
            else {
               sprintf (szName, "%s.HLP", szMenu);
               Bbs->ReadFile (szName);
               usFullMenu = FALSE;
            }
         }
         else {
            Bbs->Printf ("\n\x16\x01\x0DPlease select one of the choices presented.\n");
            usFullMenu = TRUE;
         }
      }
      else {
         if (Bbs->Status != NULL) {
            strcpy (Bbs->Status->Status, "Browsing");
            Bbs->Status->Update ();
         }
         if (mRet != MENU_OK)
            return (mRet);
      }
   }

   return (FALSE);
}

VOID TMenu::DisplayItem (ITEM *pItem)
{
   if (PS != NULL)
      PS->DisplayPrompt (pItem->szDisplay, pItem->ucColor, pItem->ucHilight);
}

VOID TMenu::DisplayPrompt (PSZ pszDisplay, UCHAR ucColor, UCHAR ucHilight)
{
   if (PS != NULL) {
      PS->DisplayPrompt (pszDisplay, ucColor, ucHilight);
      if (Com != NULL)
         Com->UnbufferBytes ();
      if (Snoop != NULL)
         Snoop->UnbufferBytes ();
   }
}

VOID TMenu::GetCommand (VOID)
{
   USHORT i, c, len, matching;
   CHAR *p;
   USHORT usMaxlen = sizeof (szCommand) - 1;

   p = szCommand;
   len = 0;

   while (Bbs->AbortSession () == FALSE) {
      if (Bbs->KBHit ()) {
         Bbs->LastActivity = time (NULL);
         if ((c = Bbs->Getch ()) == 0)
            c = (short)(Bbs->Getch () << 8);

         if (c == 13)
            break;
         else if (c == 8 || c == 127) {
            if (len > 0) {
               Bbs->Printf ("%c %c", 8, 8);
               p--;
               len--;
            }
         }
         else if (c >= 32 && c < 127) {
            if (len < usMaxlen) {
               *p++ = (char)c;
               len++;
               Bbs->Putch ((unsigned char)c);

               if (Bbs->HotKey == TRUE && !isdigit (c)) {
                  *p = '\0';
                  matching = FALSE;
                  for (i = 0; i < menuhdr.usItems; i++) {
                     if (item[i].szKey[0] == '\0')
                        continue;
                     if (!stricmp (szCommand, item[i].szKey)) {
                        matching = TRUE;
                        break;
                     }
                  }
                  if (matching == TRUE)
                     break;
               }
            }
         }
      }
      else {
         if (Bbs->TimeWarning == FALSE && Bbs->TimeLeft () <= 1) {
            Bbs->TimeWarning = TRUE;
            Bbs->Printf ("\x16\x01\x0D\n\n\x07You're just about all out of time for today! Please finish up and\nlog off... (This is your one minute warning).\n");
            DisplayPrompt (menuhdr.szPrompt, menuhdr.ucColor, menuhdr.ucHilight);
         }
         else {
            if ((i = Bbs->TimerExpired ()) != TE_NONE) {
               if (i == TE_CALL)
                  Bbs->Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for this call! Please feel free to call\nback again later...\n");
               else if (i == TE_DAY)
                  Bbs->Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for today! Please feel free to call back\nagain tomorrow...\n");
               else if (i == TE_WEEK)
                  Bbs->Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for this week! Please feel free to call\nback again next week...\n");
               else if (i == TE_MONTH)
                  Bbs->Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for this month! Please feel free to call\nback again next month...\n");
               else if (i == TE_YEAR)
                  Bbs->Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for this year! Please feel free to call\nback again next year...\n");
               Bbs->Hangup = TRUE;
            }
            else
               Bbs->ReleaseTimeSlice ();
         }
      }
   }

   Bbs->Printf ("\x16\x01\x07\n");
   *p = '\0';
}

USHORT TMenu::ExecuteOption (ITEM *pItem)
{
   switch (pItem->usCommand) {
      case MNU_DISPLAY: {
         CHAR Temp[128];

         if (strchr (pItem->szArgument, '\\') != NULL) {
            if (PS->DisplayFile (pItem->szArgument) == FALSE) {
               sprintf (Temp, "%s.Bbs", pItem->szArgument);
               PS->DisplayFile (Temp);
            }
         }
         else {
            sprintf (Temp, "%s%s", Cfg->MiscPath, pItem->szArgument);
            if (PS->DisplayFile (Temp) == FALSE) {
               sprintf (Temp, "%s%s.Bbs", Cfg->MiscPath, pItem->szArgument);
               PS->DisplayFile (Temp);
            }
         }
         break;
      }

      case MNU_FINGER:
      case MNU_FTP:
      case MNU_TELNET: {
         class TInternet *Tools;

         if ((Tools = new TInternet (Bbs)) != NULL) {
            if (pItem->usCommand == MNU_FINGER)
               Tools->Finger (pItem->szArgument);
            else if (pItem->usCommand == MNU_FTP)
               Tools->FTP (pItem->szArgument);
            else if (pItem->usCommand == MNU_TELNET)
               Tools->Telnet (pItem->szArgument);
            delete Tools;
         }
         break;
      }

      case MNU_GOSUB:
         Gosub (pItem->szArgument);
         break;

      case MNU_GOTO:
         Goto (pItem->szArgument);
         break;

      case MNU_LOGOFF:
         return (Logoff ());

      case MNU_DOWNLOADQWK:
      case MNU_DOWNLOADBW: {
         class TOffline *Olr = NULL;

         if (pItem->usCommand == MNU_DOWNLOADBW)
            Olr = new TBlueWave (Bbs);
         else if (pItem->usCommand == MNU_DOWNLOADQWK)
            Olr = new TQWK (Bbs);

         if (Olr != NULL) {
            if (Cfg->BbsId[0] != '\0')
               strcpy (Olr->Id, Cfg->BbsId);
            if (Olr->Prescan () == TRUE)
               Olr->Create ();
            delete Olr;
         }
         break;
      }

      case MNU_MSGTAG:
      case MNU_MSGVIEWTAG: {
	 class TOffline *Olr;

         if ((Olr = new TOffline (Bbs)) != NULL) {
            if (pItem->usCommand == MNU_MSGTAG)
               Olr->AddConference ();
            else if (pItem->usCommand == MNU_MSGVIEWTAG)
               Olr->Display ();
            delete Olr;
         }
         break;
      }

      case MNU_RETURN:
         Return ();
         break;

      case MNU_RUN:
         RunProgram (pItem->szArgument);
         break;

      case MNU_VERSION: {
//#if defined(__BORLANDC__)
//	 PSZ p, SMM[2] = {"","  [SL Enhanced]"};
//#endif

         Bbs->Printf ("\x0C\x16\x01\x0D%s Professional Edition - Version %s\n", NAME, VERSION);
         Bbs->Printf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\n\n");

         Bbs->Printf ("Design and Development by Marco Maccaferri.\n\n");

#if defined(__BORLANDC__)
         Bbs->Printf ("\x16\x01\x0A Version: Compiled on %s at %s (Borland C++ v%d.%d)\n", __DATE__, __TIME__, __BCPLUSPLUS__ / 0x100, __BCPLUSPLUS__ % 0x100);
#else
         Bbs->Printf ("\x16\x01\x0A Version: Compiled on %s at %s (WATCOM C/32 v%d.%d)\n", __DATE__, __TIME__, __WATCOMC__ / 100, __WATCOMC__ % 100);
#endif
         Bbs->Printf ("Computer: AT-class\n");
//#if defined(__BORLANDC__)
//         p = cpuType_Str ();
//         Bbs->Printf ("     CPU: %s %dMHz%s\n", p, intCPU_Speed (), SMM[isSMMAble ()]);
//         Bbs->Printf ("    Math: %s\n", fpuType_Str ());
//#endif
#if defined(__OS2__)
         Bbs->Printf ("      OS: OS/2\n");
#elif defined(__NT__)
         Bbs->Printf ("      OS: Windows NT/95\n");
#else
         Bbs->Printf ("      OS: DOS\n");
#endif
         break;
      }

      case MNU_WHOISON:
         OnlineUsers ();
         break;

      case MNU_PRESSENTER: {
         CHAR Temp[1];

         Bbs->Printf ("\x16\x01\x0FPress ENTER to continue ");
         Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), 0);
         break;
      }

      case MNU_MAILREAD:
         class TEMail *Mail;

         if ((Mail = new TEMail (Bbs)) != NULL) {
            Mail->ReadMessages ();
            delete Mail;
         }
         break;

      // File libraries-specific commands

      case MNU_DOWNLOAD:
         Library->Download ();
         break;

      case MNU_FILETITLES:
         if (!stricmp (pItem->szArgument, "/D"))
            Library->ListRecentFiles ();
         else if (!stricmp (pItem->szArgument, "/N"))
            Library->ListDownloadedFiles ();
         else
            Library->ListFiles ();
         break;

      case MNU_FILETAG:
         Library->ManageTagged ();
         break;

      case MNU_FILENAMELOCATE:
         Library->SearchText ();
         break;

      case MNU_NEWFILES:
         Library->SearchNewFiles ();
         break;

      case MNU_UPLOAD:
         Library->Upload ();
         break;

      case MNU_FILEAREA:
         Library->SelectArea ();
         usDoRead = TRUE;
         break;

      case MNU_FILENEXTAREA:
         Library->SelectNext ();
         usDoRead = TRUE;
         break;

      case MNU_FILEPREVAREA:
         Library->SelectPrevious ();
         usDoRead = TRUE;
         break;

      case MNU_FILEKILL:
         Library->RemoveFiles ();
         break;

      // Message conferences-specific commands

      case MNU_MSGDELETE:
         Message->DeleteMessage ();
         break;

      case MNU_READCURRENT:
         Message->ReadCurrent (TRUE);
         break;

      case MNU_MSGLIST:
         Message->BriefList ();
         break;

      case MNU_READNEXT:
         Message->ReadNext ();
         break;

      case MNU_READPREVIOUS:
         Message->ReadPrevious ();
         break;

      case MNU_READMSG:
         Message->ReadMessages ();
         break;

      case MNU_SAMEDIRECTION:
         if (Message->ReadForward == TRUE)
            Message->ReadNext ();
         else
            Message->ReadPrevious ();
         break;

      case MNU_READNONSTOP:
         Message->ReadNonStop ();
         break;

      case MNU_MSGAREA:
         Message->SelectArea ();
         usDoRead = TRUE;
         break;

      case MNU_MSGENTER:
         if ((Editor = new TMsgEditor (Bbs)) != NULL) {
            Editor->Msg = Message->Msg;
            if (Editor->Write () == TRUE)
               Gosub ("edit");
         }
         break;

      case MNU_MSGFORWARD:
         if ((Editor = new TMsgEditor (Bbs)) != NULL) {
            if (Message->Msg->Read (Message->LastRead->Number) == TRUE) {
               Editor->Msg = Message->Msg;
               Editor->Forward ();
            }
            delete Editor;
            Editor = NULL;
         }
         break;

      case MNU_MSGINDIVIDUAL:
         Message->Read (atol (szCommand), TRUE);
         break;

      case MNU_MSGKLUDGES:
         if (Message->ShowKludges == TRUE)
            Message->ShowKludges = FALSE;
         else
            Message->ShowKludges = TRUE;
         break;

      case MNU_MSGNEXTAREA:
         Message->SelectNext ();
         usDoRead = TRUE;
         break;

      case MNU_MSGPREVAREA:
         Message->SelectPrevious ();
         usDoRead = TRUE;
         break;

      // Line-oriented text editor

      case MNU_EDITTO:
         if (Editor != NULL)
            Editor->InputTo ();
         break;

      case MNU_EDITSUBJECT:
         if (Editor != NULL)
            Editor->InputSubject ();
         break;

      case MNU_EDITLIST:
         if (Editor != NULL)
            Editor->ListText ();
         break;

      case MNU_EDITEDIT:
         if (Editor != NULL)
            Editor->ChangeText ();
         break;

      case MNU_EDITINSERT:
         if (Editor != NULL)
            Editor->InsertLines ();
         break;

      case MNU_EDITDELETE:
         if (Editor != NULL)
            Editor->DeleteLine ();
         break;

      case MNU_EDITCONTINUE:
         if (Editor != NULL)
            Editor->AppendText ();
         break;

      case MNU_EDITSAVE:
         if (Editor != NULL) {
            Editor->Save ();
            delete Editor;
            Editor = NULL;
            Return ();
         }
         break;

      case MNU_EDITABORT:
         if (Editor != NULL) {
            Editor->Clear ();
            Bbs->Printf (Lang->EditorExited);
            delete Editor;
            Editor = NULL;
            Return ();
         }
         break;

      // User configuration

      case MNU_CHGALIAS: {
         CHAR Temp[48];

         do {
            Bbs->Printf (Lang->AskRealName);
            Bbs->GetString (Temp, (SHORT)(sizeof (Temp) - 1), INP_FIELD|INP_FANCY);
         } while (Temp[0] == '\0' && Bbs->AbortSession () == FALSE);

         if (Temp[0] != '\0')
            strcpy (User->RealName, Temp);
      }

      case MNU_CHGHOTKEYS:
         Bbs->HotKey = (Bbs->HotKey == TRUE) ? FALSE : TRUE;
         User->HotKey = Bbs->HotKey;
         break;

      case MNU_CHGMORE:
         Bbs->More = (Bbs->More == TRUE) ? FALSE : TRUE;
         break;

      case MNU_CHGPASSWORD: {
         CHAR pwd[16], verify[16];

         do {
            verify[0] = '\0';
            Bbs->Printf (Lang->SelectPassword);
            Bbs->GetString (pwd, sizeof (pwd) - 1, INP_FIELD|INP_PWD);
            if (pwd[0] != '\0') {
               Bbs->Printf (Lang->PleaseReenter);
               Bbs->GetString (verify, sizeof (verify) - 1, INP_FIELD|INP_PWD);
               if (stricmp (pwd, verify))
                  Bbs->Printf (Lang->WrongPassword, pwd, verify);
            }
         } while (Bbs->AbortSession () == FALSE && stricmp (pwd, verify));

         if (pwd[0] != '\0')
            User->SetPassword (strlwr (pwd));
         break;
      }

      case MNU_CHGPHONE: {
         CHAR Temp[26];

         do {
            Bbs->Printf (Lang->PhoneNumber);
            Bbs->GetString (Temp, (SHORT)(sizeof (Temp) - 1), INP_FIELD);
         } while (Temp[0] == '\0' && Bbs->AbortSession () == FALSE);

         if (Temp[0] != '\0')
            strcpy (User->DayPhone, Temp);
      }

      case MNU_CHGRIP:
         Bbs->Rip = (Bbs->Rip == TRUE) ? FALSE : TRUE;
         break;

      case MNU_CHGVIDEO: {
         CHAR Temp[2];

         do {
            Bbs->Printf ("\026\001\012Video:\n\n   \026\001\013T \026\001\012... TTY\n   \026\001\013A \026\001\012... ANSI/VT-100\n   \026\001\013V \026\001\012... AVATAR\n\n\026\001\017Select: ");
            Bbs->GetString (Temp, (SHORT)(sizeof (Temp) - 1), INP_HOTKEY);
         } while (Temp[0] == '\0' && Bbs->AbortSession () == FALSE);

         switch (toupper (Temp[0])) {
            case 'T':
               Bbs->Ansi = Bbs->Avatar = Bbs->Color = FALSE;
               User->Ansi = User->Avatar = User->Color = FALSE;
               break;
            case 'A':
               Bbs->Ansi = Bbs->Color = TRUE;
               Bbs->Avatar = FALSE;
               User->Ansi = User->Color = TRUE;
               User->Avatar = FALSE;
               break;
            case 'V':
               Bbs->Avatar = Bbs->Color = TRUE;
               Bbs->Ansi = FALSE;
               User->Avatar = User->Color = TRUE;
               User->Ansi = FALSE;
               break;
         }
         break;
      }

      case MNU_CHGLENGTH: {
         CHAR Temp[3];

         Bbs->Printf ("43\n42\n41\n40\n39\n38\n37\n36\n35\n34\n33\n32\n31\n30\n29\n28\n27\n26\n25\n24\n23\n22\n21\n20\n19\n18\n17\n16\n15\n14\n13\n12\n11\n10\n9\n8\n7\n6\n5\n4\n3\n2\n");
         Bbs->Printf ("\026\001\012Please type the number at the TOP of your display: \026\001\x1E");

         Bbs->GetString (Temp, (SHORT)(sizeof (Temp) - 1), INP_FIELD);
         if (atoi (Temp) != 0)
            User->ScreenHeight = Bbs->ScreenHeight = (USHORT)atoi (Temp);
         break;
      }

      default:
         Bbs->Printf ("\n\x16\x01\015Sorry, command not yet implemented!\n");
         Log->Write (Log->MenuError, pItem->usCommand, szMenu);
         break;
   }

   return (MENU_OK);
}

VOID TMenu::Gosub (PSZ pszArgument)
{
   if (usStackPos < MAX_STACK) {
      strcpy (szMenuStack[usStackPos], szMenu);
      usStackPos++;
      strcpy (szMenu, pszArgument);
      usDoRead = TRUE;
      usFullMenu = TRUE;
   }
}

VOID TMenu::Goto (PSZ pszArgument)
{
   strcpy (szMenu, pszArgument);
   usDoRead = TRUE;
   usFullMenu = TRUE;
}

/*
   Descrizione:
      Termina il collegamento al BBS. Viene visualizzato il file
      LOGOFF e viene fatta una pausa di due secondi.

   Argomenti:
      Nessuno.
*/

USHORT TMenu::Logoff (VOID)
{
   CHAR szTemp[2];

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Logout");
      Bbs->Status->Update ();
   }

   Bbs->Printf (Lang->LogoffWarning);

   for (;;) {
      if (Bbs->AbortSession () == TRUE)
         return (MENU_LOGOFF);
      Bbs->Printf (Lang->LogoffConfirm);
      Bbs->GetString (szTemp, 1, INP_HOTKEY);
      szTemp[0] = (CHAR)toupper (szTemp[0]);

      if (szTemp[0] == Lang->Yes || szTemp[0] == Lang->No)
         break;
   }

   if (szTemp[0] == Lang->Yes)
      Bbs->ReadFile ("LOGOFF");

   return ((szTemp[0] == Lang->Yes) ? MENU_LOGOFF : MENU_OK);
}

VOID TMenu::OnlineUsers (VOID)
{
   CHAR Speed[16];
   class TStatus *Users;

   if ((Users = new TStatus (Cfg->SystemPath)) != NULL) {
      if (Users->First () == TRUE) {
         Bbs->Printf ("\nŠUser  Ch.  Speed Status \x09 Location\nÄ ÄÄÄ Ä Ä Ä\x1A\n");
         do {
            if (Users->Active == TRUE) {
               ltoa (Users->Speed, Speed, 10);
               if (Users->Speed == 0L)
                  strcpy (Speed, "Local");
               if (Users->User[0] == '\0') {
                  strcpy (Users->User, Users->Status);
                  Users->Status[0] = '\0';
               }
               Bbs->Printf ("%-25.25s %3d Š%6s %-15.15s %.26s\n", Users->User, Users->Task, Speed, Users->Status, Users->Location);
            }
         } while (Users->Next () == TRUE);
         Bbs->Printf ("\n");
      }
      delete Users;
   }
}

USHORT TMenu::Return (VOID)
{
   if (usStackPos > 0) {
      usStackPos--;
      strcpy (szMenu, szMenuStack[usStackPos]);
      usDoRead = TRUE;
      usFullMenu = FALSE;
      return (TRUE);
   }

   return (FALSE);
}

typedef struct {
   USHORT SessionID;
   USHORT ResultCode;
} TERMNOTIFY, *PTERMNOTIFY;

VOID TMenu::RunProgram (PSZ pszArgument)
{
#if defined(__OS2__)
   USHORT Terminated = FALSE;
   CHAR ObjBuf[64], Temp[128], TermQ[32];
   PSZ PrgName, Args, p;
   BYTE Priority;
   ULONG SessID, Size;
   STARTDATA StartData;
   PID pid;
   HQUEUE hSysTermQ = NULLHANDLE;
   HEV hTermQEvent;
   PTERMNOTIFY pNotify;
   REQUESTDATA Request;

   strcpy (Temp, pszArgument);
   if ((PrgName = strtok (Temp, " ")) != NULL) {
      if ((Args = strtok (NULL, "")) == NULL)
         Args = "";

      p = Temp;
      while (*Args != '\0') {
         if (*Args == '%') {
            Args++;
            switch (*Args) {
               case 'P':
                  if (Cfg->Type == CH_MODEM) {
                     sprintf (p, "%s", Cfg->Device);
                     p = strchr (p, '\0');
                  }
                  break;

               case 'k':
                  sprintf (p, "%d", Cfg->Channel);
                  p = strchr (p, '\0');
                  break;

               case '%':
                  *p++ = *Args++;
                  break;
            }
         }
         else
            *p++ = *Args++;
      }
      *p = '\0';

      sprintf (TermQ, "\\QUEUES\\EXT%d.QUE", Cfg->Channel);
      if (DosCreateQueue (&hSysTermQ, QUE_FIFO|QUE_CONVERT_ADDRESS, TermQ) == 0) {
         DosCreateEventSem (NULL, &hTermQEvent, 0L, FALSE);

         StartData.Length = sizeof (STARTDATA);
         StartData.Related = SSF_RELATED_CHILD;
         StartData.FgBg = SSF_FGBG_BACK;
         StartData.TraceOpt = SSF_TRACEOPT_NONE;
         StartData.PgmTitle = PrgName;
         StartData.PgmName = PrgName;
         StartData.PgmInputs = Temp;
         StartData.TermQ = TermQ;
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
         DosStartSession (&StartData, &SessID, &pid);

         while (Bbs->AbortSession () == FALSE && Terminated == FALSE) {
            if (DosReadQueue (hSysTermQ, &Request, &Size, (PPVOID)&pNotify, 0L, DCWW_NOWAIT, &Priority, hTermQEvent) == 0) {
               if (SessID == pNotify->SessionID)
                  Terminated = TRUE;
            }
            else
               Bbs->ReleaseTimeSlice ();
         }

         if (Terminated == FALSE)
            DosStopSession (STOP_SESSION_SPECIFIED, SessID);

         DosCloseEventSem (hTermQEvent);
         DosCloseQueue (hSysTermQ);
      }
   }
#else
   pszArgument = pszArgument;
#endif
}

// --------------------------------------------------------------------

TMenuEmbedded::TMenuEmbedded (class TBbs *pBbs) : TEmbedded (pBbs)
{
}

TMenuEmbedded::~TMenuEmbedded (void)
{
}

VOID TMenuEmbedded::BasicControl (UCHAR ucControl)
{
   switch (ucControl) {
      case CTRLF:
         switch (GetNextChar ()) {
            case 'D':
               if (Message->ReadForward == TRUE)
                  OutString ("NEXT");
               else
                  OutString ("PREVIOUS");
               break;
            case 'N':
               OutString ("%lu", Message->LastRead->Number);
               break;
         }
         break;

      case CTRLK:
         switch (GetNextChar ()) {
            case '1':
               OutString ("%s", Message->Current->Key);
               break;
            case '2':
               OutString ("%s", Library->Current->Key);
               break;
            case '7':
               OutString ("%u", User->FileTag->TotalNumber);
               break;
            case '8':
               OutString ("%lu", Library->Current->ActiveFiles);
               break;
            case '9':
               if (Message->Msg != NULL)
                  OutString ("%lu", Message->Msg->Number ());
               break;
            case 'E':
               if (Message->Msg != NULL)
                  OutString ("%lu", Message->Msg->Highest ());
               break;
            case 'Y':
               OutString ("%s", Message->Current->Display);
               break;
            case 'Z':
               OutString ("%s", Library->Current->Display);
               break;
         }
         break;

      default:
         TEmbedded::BasicControl (ucControl);
         break;
   }
}


