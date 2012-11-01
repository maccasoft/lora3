
// --------------------------------------------------------------------
// LoraBBS Professional Edition - Version 0.14
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History list:
//    23/04/95 - Initial coding
// --------------------------------------------------------------------

#include "_ldefs.h"
#include "menu.h"

TEMail::TEMail (class TBbs *bbs)
{
   Bbs = bbs;
   Cfg = bbs->Cfg;
   Lang = bbs->Lang;
   Log = bbs->Log;
   User = bbs->User;
   sprintf (Path, "%s%s", Cfg->MailSpool, User->MailBox);
   Storage = ST_SQUISH;
}

TEMail::~TEMail (void)
{
}

USHORT TEMail::Check (VOID)
{
   USHORT NewMessages = FALSE;
   ULONG Number;
   class MsgBase *Msg = NULL;

   if (Storage == ST_JAM)
      Msg = new JAM (Path);
   else if (Storage == ST_SQUISH)
      Msg = new SQUISH (Path);

   if (Msg != NULL) {
      NewMessages = TRUE;
      if (User->LastRead->Read ("e-mail") == TRUE) {
         Number = User->LastRead->Number;
         if (Number >= Msg->Highest ())
            NewMessages = FALSE;
      }
      if (Msg->Number () != 0L) {
         if (NewMessages == TRUE)
            Bbs->Printf ("\x16\x01\016\007There is \x16\x01\017new \x16\x01\016mail in your mailbox!\n");
         else
            Bbs->Printf ("\x16\x01\016There is mail in your mailbox!\n");
      }
      delete Msg;
   }

   return (NewMessages);
}

VOID TEMail::DeleteMessage (VOID)
{
   CHAR szTemp[20];
   ULONG ulMsg, ulNumber, First;
   class MsgBase *Msg = NULL;

   if (Storage == ST_JAM)
      Msg = new JAM (Path);
   else if (Storage == ST_SQUISH)
      Msg = new SQUISH (Path);

   if (Msg != NULL) {
      if (Msg->Number () != 0L) {
         ulNumber = Msg->Highest ();
         First = Msg->Lowest ();

         do {
            Bbs->Printf (Lang->MessageToErase, First, ulNumber);
            Bbs->GetString (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
            ulMsg = atol (szTemp);
            if (ulMsg < First || ulMsg > ulNumber)
               Bbs->Printf (Lang->MessageOutOfRange, ulMsg);
         } while (ulMsg > ulNumber && ulMsg != 0 && Bbs->AbortSession () == FALSE);

         if (ulMsg > 0) {
            if (Msg->ReadHeader (ulMsg) == TRUE) {
               if (Msg->Delete (ulMsg) == TRUE)
                  Bbs->Printf (Lang->ConfirmedErase, ulMsg);
            }
            else
               Bbs->Printf (Lang->CantErase, ulMsg);
         }
      }

      delete Msg;
   }
   else
      Log->Write (Log->SeriousError, __FILE__, __LINE__);
}

VOID TEMail::ReadMessages (VOID)
{
   USHORT Line, Forward, MaxLine;
   CHAR Cmd, NewMessages, End, DoRead, Temp[40], *Text, WhichMail;
   ULONG First, Last, Number, HighestRead;
   class MsgBase *Msg = NULL;

   do {
      Bbs->Printf (Lang->ReadWhichMail);
      Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), INP_HOTKEY);
      Cmd = (CHAR)toupper (Temp[0]);
      if (Cmd == Lang->WhichMailHelpKey)
         Bbs->ReadFile ("MAILTYP");
   } while (Cmd != Lang->MailFromYouKey && Cmd != Lang->MailToYouKey && Cmd != '\0');

   if (Cmd == Lang->MailFromYouKey || Cmd == Lang->MailToYouKey) {
      if (Cmd == Lang->MailFromYouKey)
         sprintf (Path, "%s%s", Cfg->SentMail, User->MailBox);
      else if (Cmd == Lang->MailToYouKey)
         sprintf (Path, "%s%s", Cfg->MailSpool, User->MailBox);
      WhichMail = Cmd;

      if (Storage == ST_JAM)
         Msg = new JAM (Path);
      else if (Storage == ST_SQUISH)
         Msg = new SQUISH (Path);
   }

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      Number = 0L;
      NewMessages = TRUE;

      if (WhichMail == Lang->MailToYouKey) {
         if (User->LastRead->Read ("e-mail") == TRUE) {
            Number = User->LastRead->Number;
            if (Number >= Last)
               NewMessages = FALSE;
         }
      }
      HighestRead = Number;

      if (Msg->Number () != 0L) {
         do {
            Bbs->Printf (Lang->StartMessageNumber);
            if (WhichMail == Lang->MailToYouKey) {
               if (NewMessages == TRUE)
                  Bbs->Printf (Lang->StartNewMessages);
            }
            else
               Bbs->Printf (".\n(Just hit RETURN to start with the first)");
            Bbs->Printf (Lang->StartPrompt);
            Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), INP_HOTKEY|INP_NUMERIC);
            Cmd = (CHAR)toupper (Temp[0]);
            if (Cmd == Lang->MailStartHelpKey)
               Bbs->ReadFile ("MAILSTRT");
         } while (Cmd != Lang->FirstMessageKey && Cmd != Lang->LastMessageKey && Cmd != '\0' && !isdigit (Cmd));

         Forward = TRUE;
         if (Cmd == Lang->FirstMessageKey) {
            if ((Number = First) > 0L)
               Number--;
         }
         else if (Cmd == Lang->LastMessageKey) {
            Number = Last + 1L;
            Forward = FALSE;
         }
         else if (isdigit (Cmd)) {
            if ((Number = atol (Temp)) > 0L)
               Number--;
         }

         if (Bbs->AbortSession () == FALSE) {
            End = TRUE;
            DoRead = FALSE;
            if (Forward == TRUE) {
               if (Msg->Next (Number) == TRUE) {
                  DoRead = TRUE;
                  End = FALSE;
               }
            }
            else {
               if (Msg->Previous (Number) == TRUE) {
                  DoRead = TRUE;
                  End = FALSE;
               }
            }

            if (End == TRUE)
               Bbs->Printf (Lang->EndOfMessages);
         }

         while (Bbs->AbortSession () == FALSE && Cmd != Lang->ExitKey) {
            if (End == FALSE && DoRead == TRUE) {
               if (Msg->Read (Number) == TRUE) {
                  Line = 5;
                  Bbs->Printf (Lang->MessageHdr, "\x0C", "E-Mail");
                  Bbs->Printf (Lang->MessageFrom, Msg->From);
                  Bbs->Printf (Lang->MessageTo, Msg->To, Number, Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
                  Bbs->Printf (Lang->MessageSubject, Msg->Subject);
                  Bbs->Printf (Lang->MessageFooter);

                  MaxLine = Line;
                  if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
                     do {
                        if (strchr (Text, '>') != NULL)
                           Bbs->Printf (Lang->MessageQuote, Text);
                        else
                           Bbs->Printf (Lang->MessageText, Text);
                        if ((Line = Bbs->MoreQuestion (Line)) == 1) {
                           if (Bbs->Ansi == TRUE || Bbs->Avatar == TRUE) {
                              do {
                                 Bbs->PrintfAt ((USHORT)(MaxLine + 1), 1, "\026\001\007\026\007");
                              } while (MaxLine-- > 5);
                              MaxLine = Line = 5;
                           }
                        }
                        else
                           MaxLine = Line;
                     } while ((Text = (PSZ)Msg->Text.Next ()) != NULL && Bbs->AbortSession () == FALSE && Line != 0);
               }

               DoRead = FALSE;
            }

            if (End == FALSE) {
               if (WhichMail == Lang->MailFromYouKey)
                  Bbs->Printf ("\n\x16\x01\013(E)rase, (F)orward, (C)opy, (P)revious, (N)ext or e(X)it: ");
               else
                  Bbs->Printf (Lang->ReadMailMenu);
            }
            else
               Bbs->Printf (Lang->EndOfMessagesMenu);

            Bbs->GetString (Temp, 10, INP_HOTKEY|INP_NUMERIC);
            if ((Cmd = (CHAR)toupper (Temp[0])) == '\0')
               Cmd = Lang->NextMessageKey;

            if (isdigit (Cmd)) {
               if (Msg->ReadHeader (atol (Temp)) == TRUE) {
                  Number = atol (Temp);
                  DoRead = TRUE;
                  End = FALSE;
               }
            }
            else if (Cmd == Lang->NextMessageKey) {
               End = TRUE;
               if (Forward == TRUE) {
                  if (Msg->Next (Number) == TRUE) {
                     DoRead = TRUE;
                     End = FALSE;
                  }
               }
               else {
                  if (Msg->Previous (Number) == TRUE) {
                     DoRead = TRUE;
                     End = FALSE;
                  }
               }

               if (End == TRUE)
                  Bbs->Printf (Lang->EndOfMessages);
            }
            else if (Cmd == Lang->RereadKey && End == TRUE) {
               DoRead = TRUE;
               End = FALSE;
            }
            else if (Cmd == Lang->PreviousMessageKey) {
               if (End == FALSE) {
                  End = TRUE;
                  if (Forward == TRUE) {
                     if (Msg->Previous (Number) == TRUE) {
                        DoRead = TRUE;
                        End = FALSE;
                     }
                  }
                  else {
                     if (Msg->Next (Number) == TRUE) {
                        DoRead = TRUE;
                        End = FALSE;
                     }
                  }
               }
               else {
                  if (Forward == TRUE) {
                     if (Msg->Next (Number) == TRUE) {
                        DoRead = TRUE;
                        End = FALSE;
                     }
                  }
                  else {
                     if (Msg->Previous (Number) == TRUE) {
                        DoRead = TRUE;
                        End = FALSE;
                     }
                  }
               }

               if (End == TRUE)
                  Bbs->Printf (Lang->EndOfMessages);
            }
            else if (Cmd == Lang->ReplyKey && End == FALSE) {
               if (WhichMail == Lang->MailToYouKey) {
               }
            }
            else if (Cmd == Lang->ForwardKey && End == FALSE) {
            }
            else if (Cmd == Lang->CopyKey && End == FALSE) {
            }
            else if (Cmd == Lang->EraseKey) {
               if (Msg->Delete (Number) == TRUE)
                  Bbs->Printf (Lang->ConfirmedErase, Number);
            }
         }
      }
      else {
         if (WhichMail == Lang->MailToYouKey)
            Bbs->Printf (Lang->NoMessagesInMailBox);
         else
            Bbs->Printf (Lang->NoMessagesFromYou);
      }

      if (WhichMail == Lang->MailToYouKey) {
         if (User->LastRead->Read ("e-mail") == TRUE) {
            User->LastRead->Number = Number;
            User->LastRead->Update ();
         }
         else {
            strcpy (User->LastRead->Key, "e-mail");
            User->LastRead->Number = Number;
            User->LastRead->QuickScan = FALSE;
            User->LastRead->PersonalOnly = FALSE;
            User->LastRead->ExcludeOwn = FALSE;
            User->LastRead->Add ();
         }
      }

      delete Msg;
   }
}

// --------------------------------------------------------------------

TMailEditor::TMailEditor (class TBbs *bbs) : TMenu (bbs)
{
   Bbs = bbs;
   Lang = bbs->Lang;
   User = bbs->User;
   Editor = new TEditor (bbs);
}

TMailEditor::~TMailEditor (void)
{
   delete Editor;
}

USHORT TMailEditor::ExecuteOption (ITEM *pItem)
{
   USHORT RetVal = MENU_OK;

/*
   switch (pItem->usCommand) {
      case MNU_RETURN:
         if (Return () == FALSE) {
            Editor->Text.Clear ();
            Bbs->Printf (Lang->EditorExited);
            RetVal = MENU_RETURN;
         }
         break;

      case MNU_SAVEMSG:
         Save ();
         RetVal = MENU_RETURN;
         break;

      case MNU_APPENDMSG:
         if (Editor->AppendText () == FALSE)
            RetVal = MENU_RETURN;
         break;

      case MNU_LISTMSG:
         Editor->ListText ();
         break;

      case MNU_CHANGETEXT:
         Editor->ChangeText ();
         break;

      case MNU_CHANGESUBJ:
         InputSubject ();
         break;

      case MNU_DELETELINE:
         Editor->DeleteLine ();
         break;

      case MNU_RETYPELINE:
         Editor->RetypeLine ();
         break;

      case MNU_INSERTLINE:
         Editor->InsertLines ();
         break;

      default:
         TMenu::ExecuteOption (pItem);
         break;
   }
*/

   return (RetVal);
}

USHORT TMailEditor::Run (PSZ pszName)
{
   return ((TMenu::Run (pszName) != MENU_LOGOFF) ? MENU_OK : MENU_LOGOFF);
}

USHORT TMailEditor::Copy (USHORT flCheckName)
{
   USHORT RetVal = FALSE;
   PSZ Text;

   do {
      Bbs->Printf (Lang->MailWhoToSend);
      Bbs->GetString (To, (USHORT)(sizeof (To) - 1), INP_FANCY);
      if (To[0] == Lang->Help)
         Bbs->ReadFile ("COPYHLP");
      else if (To[0] != '\0') {
         if (flCheckName == TRUE && strchr (To, '/') != NULL && strchr (To, '@') == NULL && User->Exist (To) == FALSE)
            Bbs->Printf (Lang->NoSuchName);
         else
            RetVal = TRUE;
      }
   } while (Bbs->AbortSession () == FALSE && RetVal == FALSE);

   if (RetVal == TRUE) {
      strcpy (Subject, Msg->Subject);

      Editor->Text.Clear ();

      if ((Text = (PSZ)Msg->Text.First ()) != NULL)
         do {
            if (Text[0] == 0x01 || !strnicmp (Text, "SEEN-BY: ", 9)) {
               Msg->Text.Remove ();
               Text = (PSZ)Msg->Text.Value ();
            }
            else {
               Editor->Text.Add (Text, (USHORT)(strlen (Text) + 1));
               Text = (PSZ)Msg->Text.Next ();
            }
         } while (Text != NULL);

      Save ();
   }

   return (RetVal);
}

USHORT TMailEditor::Forward (USHORT flCheckName)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128];
   PSZ Text;

   do {
      Bbs->Printf (Lang->MailWhoToSend);
      Bbs->GetString (To, (USHORT)(sizeof (To) - 1), INP_FANCY);
      if (To[0] == Lang->Help)
         Bbs->ReadFile ("FORWDHLP");
      else if (To[0] != '\0') {
         if (flCheckName == TRUE && strchr (To, '/') != NULL && strchr (To, '@') == NULL && User->Exist (To) == FALSE)
            Bbs->Printf (Lang->NoSuchName);
         else
            RetVal = TRUE;
      }
   } while (Bbs->AbortSession () == FALSE && RetVal == FALSE);

   if (RetVal == TRUE) {
      strcpy (Subject, Msg->Subject);

      Editor->Text.Clear ();
      sprintf (Temp, " * Originally for %s", Msg->To);
      Editor->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      sprintf (Temp, " * Forwarded by %s", User->Name);
      Editor->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      Editor->Text.Add ("", 1);

      if ((Text = (PSZ)Msg->Text.First ()) != NULL)
         do {
            if (Text[0] == 0x01 || !strnicmp (Text, "SEEN-BY: ", 9)) {
               Msg->Text.Remove ();
               Text = (PSZ)Msg->Text.Value ();
            }
            else {
               Editor->Text.Add (Text, (USHORT)(strlen (Text) + 1));
               Text = (PSZ)Msg->Text.Next ();
            }
         } while (Text != NULL);

      Save ();
   }

   return (RetVal);
}

VOID TMailEditor::InputSubject (VOID)
{
   do {
      Bbs->Printf (Lang->EnterSubject, sizeof (Subject) - 1);
      Bbs->GetString (Subject, (USHORT)(sizeof (Subject) - 1), 0);
   } while (Bbs->AbortSession () == FALSE && Subject[0] == '\0');
}

USHORT TMailEditor::Modify (VOID)
{
   USHORT RetVal = FALSE;
   PSZ Text;

   if ((Text = (PSZ)Msg->Text.First ()) != NULL) {
      strcpy (To, Msg->To);
      strcpy (Subject, Msg->Subject);

      Editor->Text.Clear ();

      do {
         if (Text[0] == 0x01 || !strnicmp (Text, "SEEN-BY: ", 9)) {
            Msg->Text.Remove ();
            Text = (PSZ)Msg->Text.Value ();
         }
         else {
            Editor->Text.Add (Text, (USHORT)(strlen (Text) + 1));
            Text = (PSZ)Msg->Text.Next ();
         }
      } while (Text != NULL);

      Bbs->Printf (Lang->TypeMessageNow);
      if ((RetVal = Editor->InputText ()) == FALSE)
         Save ();
   }

   return (RetVal);
}

USHORT TMailEditor::Reply (USHORT flCheckName)
{
   USHORT RetVal = FALSE;

   if (flCheckName == FALSE || strchr (Msg->From, '/') != NULL || strchr (Msg->From, '@') != NULL || User->Exist (Msg->From) == TRUE) {
      strcpy (To, Msg->From);
      strcpy (Subject, Msg->Subject);

      Editor->Text.Clear ();
      Bbs->Printf (Lang->TypeMessageNow);
      if ((RetVal = Editor->InputText ()) == FALSE)
         Save ();
   }

   return (RetVal);
}

VOID TMailEditor::Save (VOID)
{
   USHORT Exit = FALSE, Copy = FALSE, Done = FALSE;
   CHAR Temp[10], Path[128], *ToAddress, *p;
   ULONG Number, Original;
   struct dosdate_t date;
   struct dostime_t time;

   do {
      ToAddress = NULL;
      if ((p = strchr (To, '/')) != NULL) {
         while (p > To && *p != ' ')
            p--;
         *p++ = '\0';
         ToAddress = p;
         sprintf (Path, "%s%s", Cfg->MailSpool, "netmail");
         Msg = new SQUISH (Path);
      }
      else if (strchr (To, '@') != NULL) {
         sprintf (Path, "%s%s", Cfg->MailSpool, "sendmail");
         Msg = new SQUISH (Path);
      }
      else {
         sprintf (Path, "%s%s", Cfg->MailSpool, User->GetMailBox (To));
         Msg = new SQUISH (Path);
      }

      if (Msg != NULL) {
         Msg->New ();
         Number = Msg->Highest () + 1L;

         strcpy (Msg->From, User->Name);
         strcpy (Msg->To, To);
         strcpy (Msg->Subject, Subject);
         if (ToAddress != NULL)
            strcpy (Msg->ToAddress, ToAddress);

         _dos_getdate (&date);
         _dos_gettime (&time);

         Msg->Arrived.Day = Msg->Written.Day = date.day;
         Msg->Arrived.Month = Msg->Written.Month = date.month;
         Msg->Arrived.Year = Msg->Written.Year = (USHORT)date.year;
         Msg->Arrived.Hour = Msg->Written.Hour = time.hour;
         Msg->Arrived.Minute = Msg->Written.Minute = time.minute;
         Msg->Arrived.Second = Msg->Written.Second = time.second;

         Msg->Write (Number, Editor->Text);
         Log->Write (":Mail sent to %s", To);

         if (Copy == TRUE)
            Bbs->Printf (Lang->ConfirmedCopy, Original, Number, To);
         else {
            Bbs->Printf (Lang->ConfirmedSend, Number);
            Original = Number;
            Copy = TRUE;
         }

         delete Msg;

         sprintf (Path, "%s%s", Cfg->SentMail, User->MailBox);
         if ((Msg = new SQUISH (Path)) != NULL) {
            Msg->New ();
            Number = Msg->Highest () + 1L;

            strcpy (Msg->From, User->Name);
            strcpy (Msg->To, To);
            strcpy (Msg->Subject, Subject);
            if (ToAddress != NULL)
               strcpy (Msg->ToAddress, ToAddress);

            Msg->Arrived.Day = Msg->Written.Day = date.day;
            Msg->Arrived.Month = Msg->Written.Month = date.month;
            Msg->Arrived.Year = Msg->Written.Year = (USHORT)date.year;
            Msg->Arrived.Hour = Msg->Written.Hour = time.hour;
            Msg->Arrived.Minute = Msg->Written.Minute = time.minute;
            Msg->Arrived.Second = Msg->Written.Second = time.second;

            Msg->Write (Number, Editor->Text);
            delete Msg;
         }
      }

      Done = TRUE;

      Bbs->Printf (Lang->SendToAnother);
      Bbs->GetString (Temp, 1, INP_HOTKEY);
      if (toupper (Temp[0]) == Lang->Yes) {
         Copy = TRUE;
         Done = FALSE;

         do {
            Bbs->Printf (Lang->SendToWho);
            Bbs->GetString (To, (USHORT)(sizeof (To) - 1), INP_FANCY);
            Exit = TRUE;
            if (To[0] != '\0' && strchr (To, '@') == NULL && User->Exist (To) == FALSE) {
               Bbs->Printf (Lang->NoSuchName);
               Exit = FALSE;
            }
         } while (Exit == FALSE && Bbs->AbortSession () == FALSE);
      }
   } while (Bbs->AbortSession () == FALSE && Done == FALSE && To[0] != '\0');
}

USHORT TMailEditor::Write (USHORT flCheckName)
{
   USHORT RetVal = FALSE;

   do {
      Bbs->Printf (Lang->MailWhoToSend);
      Bbs->GetString (To, (USHORT)(sizeof (To) - 1), INP_FANCY);
      if (To[0] == '\0')
         strcpy (To, Cfg->SysopName);
      else if (To[0] == Lang->Help) {
         Bbs->ReadFile ("WRITEHLP");
         To[0] = '\0';
      }
      else {
         if (flCheckName == TRUE && strchr (To, '/') != NULL && strchr (To, '@') == NULL && User->Exist (To) == FALSE) {
            Bbs->Printf (Lang->NoSuchName);
            To[0] = '\0';
         }
      }
   } while (Bbs->AbortSession () == FALSE && To[0] == '\0');

   if (Bbs->AbortSession () == FALSE)
      InputSubject ();

   if (Bbs->AbortSession () == FALSE) {
      Editor->Text.Clear ();
      Bbs->Printf (Lang->TypeMessageNow);
      if ((RetVal = Editor->InputText ()) == FALSE)
         Save ();
   }

   return (RetVal);
}



