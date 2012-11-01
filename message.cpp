
// --------------------------------------------------------------------
// LoraBBS Professional Edition - Version 0.18
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// BBS-spcific class definitions. Used internally only.
//
// History list:
//    23/04/95 - Initial coding
// --------------------------------------------------------------------

#include "_ldefs.h"
#include "menu.h"

TMessage::TMessage (class TBbs *bbs)
{
   Cfg = bbs->Cfg;
   Lang = bbs->Lang;
   Log = bbs->Log;
   User = bbs->User;

   Bbs = bbs;
   Msg = NULL;

   if ((Current = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Current->Read (Cfg->FirstMessageArea) == FALSE)
         Current->First ();

      if (Current->Storage == ST_JAM)
         Msg = new JAM (Current->Path);
      else if (Current->Storage == ST_SQUISH)
         Msg = new SQUISH (Current->Path);
      else if (Current->Storage == ST_USENET)
         Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);

      Log->Write (":Message Area: %s - %s", Current->Key, Current->Display);
      Current->ActiveMsgs = Msg->Number ();
      Current->Update ();

      if ((LastRead = User->LastRead) != NULL) {
         if (LastRead->Read (Current->Key) == FALSE) {
            LastRead->New ();
            strcpy (LastRead->Key, Current->Key);
            LastRead->Add ();
         }
      }
   }

   ReadForward = TRUE;
   ShowKludges = FALSE;
}

TMessage::~TMessage (void)
{
   if (Current != NULL)
      delete Current;
   if (Msg != NULL)
      delete Msg;
}

VOID TMessage::BriefList (VOID)
{
   USHORT NewMessages, Forward, Continue;
   SHORT Line;
   CHAR Temp[10];
   ULONG Number, First, Last;

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      Number = 0L;
      NewMessages = TRUE;
      if (User->LastRead->Read (Current->Key) == TRUE) {
         Number = User->LastRead->Number;
         if (Number >= Last)
            NewMessages = FALSE;
      }

      if (Msg->Number () != 0L) {
         StartMessageQuestion (First, Last, NewMessages, Number, Forward);

         Line = 3;
         Bbs->Printf ("\n\x0C\x16\x01\x0FMsg#   From                 To                   Subject\n\026\001\017\031Ä\006 \031Ä\024 \031Ä\024 \031Ä\034\n");

         sprintf (Temp, "%lu", Last);
         if (Forward == TRUE)
            Continue = Msg->Next (Number);
         else
            Continue = Msg->Previous (Number);

         while (Bbs->AbortSession () == FALSE && Line != 0 && Continue == TRUE) {
            if (Msg->ReadHeader (Number) == TRUE) {
               Bbs->Printf ("\x16\x01\x07%6lu \x16\x01\x0E%-20.20s \x16\x01\x0A%-20.20s \x16\x01\x0B%.28s\n", Number, Msg->From, Msg->To, Msg->Subject);
               if ((Line = Bbs->MoreQuestion (Line)) == 1) {
                  Bbs->Printf ("\n\x0C\x16\x01\x0FMsg#   From                 To                   Subject\n\026\001\017\031Ä\006 \031Ä\024 \031Ä\024 \031Ä\034\n");
                  Line = 3;
               }
            }
            if (Forward == TRUE)
               Continue = Msg->Next (Number);
            else
               Continue = Msg->Previous (Number);
         }
      }
   }
}

VOID TMessage::DeleteMessage (VOID)
{
   CHAR szTemp[20];
   ULONG ulMsg, ulNumber, First;

   if (Msg != NULL) {
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
            if (!stricmp (Msg->From, User->Name) || !stricmp (Msg->To, User->Name)) {
               if (Msg->Delete (ulMsg) == TRUE)
                  Bbs->Printf (Lang->ConfirmedErase, ulMsg);
            }
            else
               Bbs->Printf (Lang->CantErase, ulMsg);
         }
         else
            Bbs->Printf (Lang->CantErase, ulMsg);
      }
   }
   else
      Log->Write (Log->SeriousError, __FILE__, __LINE__);
}

VOID TMessage::ReadCurrent (USHORT fCls)
{
   USHORT Line, MaxLine;
   CHAR Temp[32], *Text;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Message reading");
      Bbs->Status->Update ();
   }

   if (LastRead != NULL && Msg != NULL) {
      if (Msg->Read (LastRead->Number, (USHORT)(User->ScreenWidth - 1)) == TRUE) {
         Line = 5;
         Bbs->Printf (Lang->MessageHdr, (fCls == TRUE) ? "\x0C" : "\n", Current->Display);
         Bbs->Printf (Lang->MessageFrom, Msg->From);
         Bbs->Printf (Lang->MessageTo, Msg->To, LastRead->Number, Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
         Bbs->Printf (Lang->MessageSubject, Msg->Subject);
         Bbs->Printf (Lang->MessageFooter);

         MaxLine = Line;
         if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
            do {
               if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
                  if (strchr (Text, '>') != NULL)
                     Bbs->Printf (Lang->MessageQuote, Text);
                  else if (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)
                     Bbs->Printf (Lang->MessageText, Text);
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
               }
            } while ((Text = (PSZ)Msg->Text.Next ()) != NULL && Bbs->AbortSession () == FALSE && Line != 0);

         if (Line > 1)
            Bbs->Printf ("\001\001");
      }
   }
}

VOID TMessage::Read (ULONG ulNumber, USHORT fCls)
{
   USHORT Line, MaxLine;
   CHAR Temp[32], *Text;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Message reading");
      Bbs->Status->Update ();
   }

   if (LastRead != NULL && Msg != NULL) {
      if (Msg->Read (ulNumber, (USHORT)(User->ScreenWidth - 1)) == TRUE) {
         LastRead->Number = ulNumber;
         LastRead->Update ();

         Line = 5;
         Bbs->Printf (Lang->MessageHdr, (fCls == TRUE) ? "\x0C" : "\n", Current->Display);
         Bbs->Printf (Lang->MessageFrom, Msg->From);
         Bbs->Printf (Lang->MessageTo, Msg->To, LastRead->Number, Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
         Bbs->Printf (Lang->MessageSubject, Msg->Subject);
         Bbs->Printf (Lang->MessageFooter);

         MaxLine = Line;
         if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
            do {
               if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
                  if (strchr (Text, '>') != NULL)
                     Bbs->Printf (Lang->MessageQuote, Text);
                  else if (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)
                     Bbs->Printf (Lang->MessageText, Text);
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
               }
            } while ((Text = (PSZ)Msg->Text.Next ()) != NULL && Bbs->AbortSession () == FALSE && Line != 0);
      }
   }
}

VOID TMessage::ReadMessages (VOID)
{
   USHORT Line, Forward;
   CHAR Cmd, NewMessages, End, DoRead, Temp[40], *Text;
   ULONG First, Last, Number;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Message reading");
      Bbs->Status->Update ();
   }

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      Number = 0L;
      NewMessages = TRUE;
      if (User->LastRead->Read (Current->Key) == TRUE) {
         Number = User->LastRead->Number;
         if (Number >= Last)
            NewMessages = FALSE;
      }

      if (Msg->Number () != 0L) {
         StartMessageQuestion (First, Last, NewMessages, Number, Forward);

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

            if (End == TRUE) {
               Bbs->Printf (Lang->EndOfMessages);
               Cmd = Lang->ExitKey;
            }
         }

         while (Bbs->AbortSession () == FALSE && Cmd != Lang->ExitKey) {
            if (End == FALSE && DoRead == TRUE) {
               if (Msg->ReadHeader (Number) == TRUE) {
                  if (User->ReadHeader == FALSE)
                     Bbs->Printf (Lang->MessageHdr);
                  else
                     Bbs->Printf ("\n");

                  Line = 4;
                  sprintf (Temp, "%d %s %d %2d:%02d", Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
//                  Bbs->Printf (Lang->MessageDate, Temp, Current->Display);
                  Bbs->Printf (Lang->MessageFrom, Msg->From, Number, Last);
                  if (Msg->To[0]) {
                     Bbs->Printf (Lang->MessageTo, Msg->To);
                     Line++;
                  }
                  Bbs->Printf (Lang->MessageSubject, Msg->Subject);
               }

               Cmd = '\0';
               if (Bbs->AbortSession () == FALSE && User->ReadHeader == TRUE) {
                  Line = 1;
                  do {
                     Bbs->Printf (Lang->ReadThisMessage);
                     Bbs->GetString (Temp, 1, INP_HOTKEY);
                     if ((Cmd = (CHAR)toupper (Temp[0])) == '\0')
                        Cmd = Lang->ReadMessageKey;
                  } while (Bbs->AbortSession () == FALSE && Cmd != Lang->NextMessageKey && Cmd != Lang->PreviousMessageKey && Cmd != Lang->ReadMessageKey && Cmd != Lang->ExitKey);
               }

               if (User->ReadHeader == FALSE || Cmd == Lang->ReadMessageKey) {
                  if (Msg->Read (Number, (USHORT)(User->ScreenWidth - 1)) == TRUE) {
                     if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
                        do {
                           if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
                              if (strchr (Text, '>') != NULL)
                                 Bbs->Printf (Lang->MessageQuote, Text);
                              else if (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)
                                 Bbs->Printf (Lang->MessageText, Text);
                              else
                                 Bbs->Printf (Lang->MessageText, Text);
                              Line = Bbs->MoreQuestion (Line);
                           }
                        } while ((Text = (PSZ)Msg->Text.Next ()) != NULL && Bbs->AbortSession () == FALSE && Line != 0);
                  }
               }

               DoRead = FALSE;
            }

            if (End == FALSE)
               Bbs->Printf (Lang->ReadMessageMenu);
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
               Bbs->Printf ("\n\x16\x01\015Sorry, command not yet implemented!\n");
            }
            else if (Cmd == 'E' && End == FALSE) {
               Bbs->Printf ("\n\x16\x01\015Sorry, command not yet implemented!\n");
            }
         }
      }

      if (User->LastRead->Read (Current->Key) == TRUE) {
         User->LastRead->Number = Number;
         User->LastRead->Update ();
      }
      else {
         strcpy (User->LastRead->Key, Current->Key);
         User->LastRead->Number = Number;
         User->LastRead->QuickScan = FALSE;
         User->LastRead->PersonalOnly = FALSE;
         User->LastRead->ExcludeOwn = FALSE;
         User->LastRead->Add ();
      }
   }
}

VOID TMessage::ReadNext (VOID)
{
   ULONG Number;

   if (LastRead != NULL && Msg != NULL) {
      Number = LastRead->Number;
      if (Msg->Next (Number) == TRUE) {
         LastRead->Number = Number;
         LastRead->Update ();
         ReadCurrent (TRUE);
         ReadForward = TRUE;
      }
      else
         Bbs->Printf (Lang->EndOfMessages);
   }
}

VOID TMessage::ReadNonStop (VOID)
{
   USHORT Continue = TRUE;
   ULONG Number;

   if (LastRead != NULL && Msg != NULL) {
      Number = LastRead->Number;

      while (Bbs->AbortSession () == FALSE && Continue == TRUE) {
         if (ReadForward == TRUE)
            Continue = Msg->Next (Number);
         else
            Continue = Msg->Previous (Number);

         if (Continue == TRUE) {
            LastRead->Number = Number;
            ReadCurrent (FALSE);
         }
      }

      LastRead->Update ();
   }
}

VOID TMessage::ReadPrevious (VOID)
{
   ULONG Number;

   if (LastRead != NULL && Msg != NULL) {
      Number = LastRead->Number;
      if (Msg->Previous (Number) == TRUE) {
         LastRead->Number = Number;
         LastRead->Update ();
         ReadCurrent (TRUE);
         ReadForward = FALSE;
      }
      else
         Bbs->Printf (Lang->EndOfMessages);
   }
}

USHORT TMessage::SelectArea (VOID)
{
   USHORT fRet = FALSE, FirstHit;
   SHORT Line;
   CHAR szCommand[16], Temp[128];
   class TMsgData *Data;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Change conference");
      Bbs->Status->Update ();
   }

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      do {
         Bbs->Printf (Lang->MessageSelect);
         Bbs->GetString (szCommand, (USHORT)(sizeof (szCommand) - 1), INP_FIELD);

         if (!stricmp (szCommand, Lang->ListKey)) {
            if (Data->First () == TRUE) {
               Bbs->Printf (Lang->MessageListHeader);
               Line = 3;
               do {
                  if (Data->ShowGlobal == TRUE && User->Level >= Data->Level) {
                     if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                        Bbs->Printf (Lang->MessageList, Data->Key, Data->ActiveMsgs, Data->Display);
                        Line = Bbs->MoreQuestion (Line);
                     }
                  }
               } while (Line != 0 && Bbs->AbortSession () == FALSE && Data->Next () == TRUE);
            }
         }
         else if (szCommand[0] != '\0') {
            if (Data->Read (szCommand) == TRUE) {
               if (Data->ShowGlobal == TRUE && User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     Current->Read (szCommand);
                     if (Msg != NULL)
                        delete Msg;

                     Msg = NULL;
                     if (Current->Storage == ST_JAM)
                        Msg = new JAM (Current->Path);
                     else if (Current->Storage == ST_SQUISH)
                        Msg = new SQUISH (Current->Path);
                     else if (Current->Storage == ST_USENET)
                        Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);

                     Log->Write (":Message Area: %s - %s", Current->Key, Current->Display);
                     Current->ActiveMsgs = Msg->Number ();
                     Current->Update ();

                     if ((LastRead = User->LastRead) != NULL) {
                        if (LastRead->Read (Current->Key) == FALSE) {
                           LastRead->New ();
                           strcpy (LastRead->Key, Current->Key);
                           LastRead->Add ();
                        }
                     }

                     ReadForward = TRUE;
                     fRet = TRUE;
                  }
               }
            }

            if (fRet == FALSE) {
               FirstHit = TRUE;
               strupr (szCommand);
               Line = 3;

               if (Data->First () == TRUE) {
                  do {
                     if (Data->ShowGlobal == TRUE && User->Level >= Data->Level) {
                        if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                           strcpy (Temp, Data->Key);
                           if (strstr (strupr (Temp), szCommand) != NULL) {
                              if (FirstHit == TRUE)
                                 Bbs->Printf (Lang->MessageListHeader);
                              Bbs->Printf (Lang->MessageList, Data->Key, Data->ActiveMsgs, Data->Display);
                              Line = Bbs->MoreQuestion (Line);
                              FirstHit = FALSE;
                           }
                           else {
                              strcpy (Temp, Data->Display);
                              if (strstr (strupr (Temp), szCommand) != NULL) {
                                 if (FirstHit == TRUE)
                                    Bbs->Printf (Lang->MessageListHeader);
                                 Bbs->Printf (Lang->MessageList, Data->Key, Data->ActiveMsgs, Data->Display);
                                 Line = Bbs->MoreQuestion (Line);
                                 FirstHit = FALSE;
                              }
                              else {
                                 strcpy (Temp, Data->EchoTag);
                                 if (strstr (strupr (Temp), szCommand) != NULL) {
                                    if (FirstHit == TRUE)
                                       Bbs->Printf (Lang->MessageListHeader);
                                    Bbs->Printf (Lang->MessageList, Data->Key, Data->ActiveMsgs, Data->Display);
                                    Line = Bbs->MoreQuestion (Line);
                                    FirstHit = FALSE;
                                 }
                                 else {
                                    strcpy (Temp, Data->NewsGroup);
                                    if (strstr (strupr (Temp), szCommand) != NULL) {
                                       if (FirstHit == TRUE)
                                          Bbs->Printf (Lang->MessageListHeader);
                                       Bbs->Printf (Lang->MessageList, Data->Key, Data->ActiveMsgs, Data->Display);
                                       Line = Bbs->MoreQuestion (Line);
                                       FirstHit = FALSE;
                                    }
                                 }
                              }
                           }
                        }
                     }
                  } while (Line != 0 && Bbs->AbortSession () == FALSE && Data->Next () == TRUE);
               }

               if (FirstHit == TRUE)
                  Bbs->Printf (Lang->ForumNotAvailable);
            }
         }
      } while (szCommand[0] != '\0' && fRet == FALSE && Bbs->AbortSession () == FALSE);

      delete Data;
   }

   return (fRet);
}

VOID TMessage::SelectNext (VOID)
{
   USHORT Found = FALSE;
   class TMsgData *Data;

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      Data->Read (Current->Key, FALSE);
      do {
         if (Data->Next () == FALSE)
            Data->First ();
         if (!stricmp (Data->Key, Current->Key))
            Found = TRUE;
         else {
            if (Data->ShowGlobal == TRUE && User->Level >= Data->Level) {
               if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags)
                  Found = TRUE;
            }
         }
      } while (Found == FALSE);

      if (stricmp (Current->Key, Data->Key)) {
         Current->Read (Data->Key);
         if (Msg != NULL)
            delete Msg;

         Msg = NULL;
         if (Current->Storage == ST_JAM)
            Msg = new JAM (Current->Path);
         else if (Current->Storage == ST_SQUISH)
            Msg = new SQUISH (Current->Path);
         else if (Current->Storage == ST_USENET)
            Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);

         Log->Write (":Message Area: %s - %s", Current->Key, Current->Display);
         Current->ActiveMsgs = Msg->Number ();
         Current->Update ();

         if ((LastRead = User->LastRead) != NULL) {
            if (LastRead->Read (Current->Key) == FALSE) {
               LastRead->New ();
               strcpy (LastRead->Key, Current->Key);
               LastRead->Add ();
            }
         }

         ReadForward = TRUE;
      }

      delete Data;
   }
}

VOID TMessage::SelectPrevious (VOID)
{
   USHORT Found = FALSE;
   class TMsgData *Data;

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      Data->Read (Current->Key, FALSE);
      do {
         if (Data->Previous () == FALSE)
            Data->Last ();
         if (!stricmp (Data->Key, Current->Key))
            Found = TRUE;
         else {
            if (Data->ShowGlobal == TRUE && User->Level >= Data->Level) {
               if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags)
                  Found = TRUE;
            }
         }
      } while (Found == FALSE);

      if (stricmp (Current->Key, Data->Key)) {
         Current->Read (Data->Key);
         if (Msg != NULL)
            delete Msg;

         Msg = NULL;
         if (Current->Storage == ST_JAM)
            Msg = new JAM (Current->Path);
         else if (Current->Storage == ST_SQUISH)
            Msg = new SQUISH (Current->Path);
         else if (Current->Storage == ST_USENET)
            Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);

         Log->Write (":Message Area: %s - %s", Current->Key, Current->Display);
         Current->ActiveMsgs = Msg->Number ();
         Current->Update ();

         if ((LastRead = User->LastRead) != NULL) {
            if (LastRead->Read (Current->Key) == FALSE) {
               LastRead->New ();
               strcpy (LastRead->Key, Current->Key);
               LastRead->Add ();
            }
         }

         ReadForward = TRUE;
      }

      delete Data;
   }
}

VOID TMessage::StartMessageQuestion (ULONG ulFirst, ULONG ulLast, USHORT fNewMessages, ULONG &ulMsg, USHORT &fForward)
{
   CHAR Cmd, Temp[20];

   do {
      Bbs->Printf (Lang->StartMessageNumber);
      if (fNewMessages == TRUE)
         Bbs->Printf (Lang->StartNewMessages);
      Bbs->Printf (Lang->StartPrompt);
      Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), INP_HOTKEY|INP_NUMERIC);
      Cmd = (CHAR)toupper (Temp[0]);
      if (Cmd == Lang->MailStartHelpKey)
         Bbs->ReadFile ("MAILSTRT");
   } while (Cmd != Lang->FirstMessageKey && Cmd != Lang->LastMessageKey && Cmd != '\0' && !isdigit (Cmd));

   fForward = TRUE;
   if (Cmd == Lang->FirstMessageKey) {
      if ((ulMsg = ulFirst) > 0L)
         ulMsg--;
   }
   else if (Cmd == Lang->LastMessageKey) {
      ulMsg = ulLast + 1L;
      fForward = FALSE;
   }
   else if (isdigit (Cmd)) {
      if ((ulMsg = atol (Temp)) > 0L)
         ulMsg--;
   }
}

VOID TMessage::TextListMessages (VOID)
{
   USHORT NewMessages, Forward, Continue;
   SHORT Line;
   CHAR *Text;
   ULONG Number, First, Last;

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      Number = 0L;
      NewMessages = TRUE;
      if (User->LastRead->Read (Current->Key) == TRUE) {
         Number = User->LastRead->Number;
         if (Number >= Last)
            NewMessages = FALSE;
      }

      if (Msg->Number () > 0L) {
         StartMessageQuestion (First, Last, NewMessages, Number, Forward);

         Line = 3;
         Bbs->Printf ("\n\x16\x01\x0AListing %s...\n\n", Current->Display);

         if (Forward == TRUE)
            Continue = Msg->Next (Number);
         else
            Continue = Msg->Previous (Number);

         while (Bbs->AbortSession () == FALSE && Line != 0 && Continue == TRUE) {
            if (Msg->Read (Number) == TRUE) {
               Bbs->Printf ("\x16\x01\x0A   Date: %d %s %d %2d:%02d\n", Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
               if ((Line = Bbs->MoreQuestion (Line)) != 0) {
                  Bbs->Printf ("\x16\x01\x0A   From: %-40.40s Msg. #%ld of %ld\n", Msg->From, Number, Msg->Highest ());
                  if ((Line = Bbs->MoreQuestion (Line)) != 0) {
                     if (Msg->To[0]) {
                        Bbs->Printf ("\x16\x01\x0A     To: %s\n", Msg->To);
                        Line = Bbs->MoreQuestion (Line);
                     }
                     if (Line != 0) {
                        Bbs->Printf ("\x16\x01\x0ASubject: %s\n", Msg->Subject);
                        Line = Bbs->MoreQuestion (Line);
                     }
                  }
               }
               if (Line != 0) {
                  Bbs->Printf ("\n");
                  if ((Line = Bbs->MoreQuestion (Line)) != 0) {
                     if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
                        do {
                           if (strchr (Text, '>') != NULL)
                              Bbs->Printf (Lang->MessageQuote, Text);
                           else
                              Bbs->Printf (Lang->MessageText, Text);
                           Line = Bbs->MoreQuestion (Line);
                        } while ((Text = (PSZ)Msg->Text.Next ()) != NULL && Bbs->AbortSession () == FALSE && Line != 0);
                  }
                  Bbs->Printf ("\n");
                  Line = Bbs->MoreQuestion (Line);
               }
            }

            if (Forward == TRUE)
               Continue = Msg->Next (Number);
            else
               Continue = Msg->Previous (Number);
         }
      }
   }
}

VOID TMessage::TitleListMessages (VOID)
{
   USHORT NewMessages, Forward, Continue;
   SHORT Line;
   ULONG Number, First, Last;

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      Number = 0L;
      NewMessages = TRUE;
      if (User->LastRead->Read (Current->Key) == TRUE) {
         Number = User->LastRead->Number;
         if (Number >= Last)
            NewMessages = FALSE;
      }

      if (Msg->Number () > 0L) {
         StartMessageQuestion (First, Last, NewMessages, Number, Forward);

         Line = 3;
         Bbs->Printf ("\n\x16\x01\x0AListing %s...\n\n", Current->Display);

         if (Forward == TRUE)
            Continue = Msg->Next (Number);
         else
            Continue = Msg->Previous (Number);

         while (Bbs->AbortSession () == FALSE && Line != 0 && Continue == TRUE) {
            if (Msg->ReadHeader (Number) == TRUE) {
               Bbs->Printf ("\x16\x01\x0A   Date: %d %s %d %2d:%02d\n", Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
               if ((Line = Bbs->MoreQuestion (Line)) != 0) {
                  Bbs->Printf ("\x16\x01\x0A   From: %-40.40s Msg. #%ld of %ld\n", Msg->From, Number, Msg->Highest ());
                  if ((Line = Bbs->MoreQuestion (Line)) != 0) {
                     if (Msg->To[0]) {
                        Bbs->Printf ("\x16\x01\x0A     To: %s\n", Msg->To);
                        Line = Bbs->MoreQuestion (Line);
                     }
                     if (Line != 0) {
                        Bbs->Printf ("\x16\x01\x0ASubject: %s\n", Msg->Subject);
                        Line = Bbs->MoreQuestion (Line);
                     }
                  }
               }
               Bbs->Printf ("\n");
               Line = Bbs->MoreQuestion (Line);
            }

            if (Forward == TRUE)
               Continue = Msg->Next (Number);
            else
               Continue = Msg->Previous (Number);
         }
      }
   }
}

// --------------------------------------------------------------------

TMsgEditor::TMsgEditor (class TBbs *bbs) : TEditor (bbs)
{
   Bbs = bbs;
   Lang = bbs->Lang;
   Log = bbs->Log;
   User = bbs->User;
   Number = 0L;
}

TMsgEditor::~TMsgEditor (void)
{
}

VOID TMsgEditor::Forward (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128];
   PSZ Line;

   do {
      Bbs->Printf (Lang->MailWhoToSend);
      Bbs->GetString (To, (USHORT)(sizeof (To) - 1), INP_FANCY);
      if (To[0] == Lang->Help)
         Bbs->ReadFile ("FORWDHLP");
      else if (To[0] != '\0')
         RetVal = TRUE;
   } while (Bbs->AbortSession () == FALSE && RetVal == FALSE && To[0] != '\0');

   if (RetVal == TRUE) {
      sprintf (Subject, "%s (Fwd)", Msg->Subject);

      Text.Clear ();
      sprintf (Temp, " * Originally for %s", Msg->To);
      Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      sprintf (Temp, " * Forwarded by %s", User->Name);
      Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      Text.Add ("", 1);

      if ((Line = (PSZ)Msg->Text.First ()) != NULL)
         do {
            if (Line[0] != 0x01 && strnicmp (Line, "SEEN-BY: ", 9))
               Text.Add (Line, (USHORT)(strlen (Line) + 1));
         } while ((Line = (PSZ)Msg->Text.Next ()) != NULL);

      Save ();
   }
}

VOID TMsgEditor::InputSubject (VOID)
{
   do {
      Bbs->Printf (Lang->EnterSubject, sizeof (Subject) - 1);
      Bbs->GetString (Subject, (USHORT)(sizeof (Subject) - 1), 0);
   } while (Bbs->AbortSession () == FALSE && Subject[0] == '\0');
}

VOID TMsgEditor::InputTo (VOID)
{
   do {
      Bbs->Printf (Lang->WhoToSend);
      Bbs->GetString (To, (USHORT)(sizeof (To) - 1), INP_FANCY);
      if (To[0] == '\0')
         strcpy (To, Lang->ToAll);
      else if (To[0] == Lang->Help) {
         Bbs->ReadFile ("WRITEHLP");
         To[0] = '\0';
      }
   } while (Bbs->AbortSession () == FALSE && To[0] == '\0');
}

USHORT TMsgEditor::Modify (VOID)
{
   USHORT RetVal = FALSE;
   PSZ Line;

   if ((Line = (PSZ)Msg->Text.First ()) != NULL) {
      do {
         if (Line[0] != 0x01 && strnicmp (Line, "SEEN-BY: ", 9))
            Text.Add (Line, (USHORT)(strlen (Line) + 1));
      } while ((Line = (PSZ)Msg->Text.Next ()) != NULL);

      strcpy (To, Msg->To);
      strcpy (Subject, Msg->Subject);

      Bbs->Printf (Lang->TypeMessageNow);
      if ((RetVal = InputText ()) == FALSE)
         Save ();
   }

   return (RetVal);
}

USHORT TMsgEditor::Reply (VOID)
{
   USHORT RetVal = FALSE;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Writing message");
      Bbs->Status->Update ();
   }

   strcpy (To, Msg->From);
   strcpy (Subject, Msg->Subject);

   Text.Clear ();
   Bbs->Printf (Lang->TypeMessageNow);
   if ((RetVal = InputText ()) == FALSE)
      Save ();

   return (RetVal);
}

VOID TMsgEditor::Save (VOID)
{
   struct dosdate_t date;
   struct dostime_t time;

   if (Msg != NULL) {
      Msg->New ();

      strcpy (Msg->From, User->Name);
      strcpy (Msg->To, To);
      strcpy (Msg->Subject, Subject);

      _dos_getdate (&date);
      _dos_gettime (&time);

      Msg->Arrived.Day = Msg->Written.Day = date.day;
      Msg->Arrived.Month = Msg->Written.Month = date.month;
      Msg->Arrived.Year = Msg->Written.Year = (USHORT)date.year;
      Msg->Arrived.Hour = Msg->Written.Hour = time.hour;
      Msg->Arrived.Minute = Msg->Written.Minute = time.minute;
      Msg->Arrived.Second = Msg->Written.Second = time.second;

      Msg->Add (Text);
      Number = Msg->Highest ();

      Log->Write (":Written message #%lu", Number);
      Bbs->Printf (Lang->ConfirmedSend, Number);
   }
}

USHORT TMsgEditor::Write (VOID)
{
   USHORT RetVal = FALSE;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Writing message");
      Bbs->Status->Update ();
   }

   To[0] = '\0';
   InputTo ();
   if (Bbs->AbortSession () == FALSE)
      InputSubject ();

   if (Bbs->AbortSession () == FALSE) {
      Text.Clear ();
      Bbs->Printf (Lang->TypeMessageNow);
      if ((RetVal = InputText ()) == FALSE)
         Save ();
   }

   return (RetVal);
}


