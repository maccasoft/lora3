
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.7
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora.h"

class TMsgAreaListing : public TListings
{
public:
   TMsgAreaListing (void);

   CHAR   Command[16];
   class  TConfig *Cfg;
   class  TMsgData *Current;

   VOID   Begin (VOID);
   USHORT DrawScreen (VOID);
   VOID   PrintCursor (USHORT y);
   VOID   PrintLine (VOID);
   VOID   PrintTitles (VOID);
   VOID   RemoveCursor (USHORT y);
   VOID   Select (VOID);
};

class TMsgNewAreaListing : public TListings
{
public:
   class  TConfig *Cfg;
   class  TMsgData *Current;

   VOID   Begin (VOID);
   USHORT DrawScreen (VOID);
   VOID   PrintCursor (USHORT y);
   VOID   PrintLine (VOID);
   VOID   PrintTitles (VOID);
   VOID   RemoveCursor (USHORT y);
   VOID   Select (VOID);
};

class TMessageList : public TListings
{
public:
   USHORT Forward, ShowKludges;
   CHAR   LastRead[16];
   ULONG  Number;
   class  TConfig *Cfg;
   class  TMsgBase *Msg;
   class  TMsgData *Current;

   VOID   Begin (VOID);
   USHORT DrawScreen (VOID);
   VOID   PrintCursor (USHORT y);
   VOID   PrintLine (VOID);
   VOID   PrintTitles (VOID);
   VOID   RemoveCursor (USHORT y);
   VOID   Select (VOID);
};

// ----------------------------------------------------------------------

VOID TMessageList::Begin (VOID)
{
   USHORT i, Continue;
   LISTDATA ld;

   i = 0;
   y = 4;
   Found = FALSE;
   List.Clear ();
   Data.Clear ();

   if (Msg  != NULL) {
      if (Forward == TRUE)
         Continue = Msg->Next (Number);
      else
         Continue = Msg->Previous (Number);

      while (Embedded->AbortSession () == FALSE && Continue == TRUE) {
         if (Msg->ReadHeader (Number) == TRUE) {
            sprintf (ld.Key, "%lu", Number);
            sprintf (ld.Display, "%-22.22s  %-21.21s  %-.23s", Msg->From, Msg->To, Msg->Subject);
            Data.Add (&ld, sizeof (LISTDATA));
         }
         if (Forward == TRUE)
            Continue = Msg->Next (Number);
         else
            Continue = Msg->Previous (Number);
      }
   }

   if ((pld = (LISTDATA *)Data.First ()) != NULL) {
      do {
         if (!strcmp (pld->Key, LastRead))
            Found = TRUE;
         List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
         i++;
         if (i >= (User->ScreenHeight - 6)) {
            if (Found == TRUE)
               break;
            List.Clear ();
            i = 0;
         }
      } while ((pld = (LISTDATA *)Data.Next ()) != NULL);
   }

   if (Found == FALSE) {
      List.Clear ();
      if ((pld = (LISTDATA *)Data.First ()) != NULL) {
         do {
            if (!strcmp (pld->Key, LastRead))
               Found = TRUE;
            List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
            i++;
            if (i >= (User->ScreenHeight - 6))
               break;
         } while ((pld = (LISTDATA *)Data.Next ()) != NULL);
      }
   }
}

USHORT TMessageList::DrawScreen (VOID)
{
   USHORT i;

   i = 0;
   do {
      pld = (LISTDATA *)Data.Value ();
      if (Found == TRUE && !strcmp (pld->Key, LastRead)) {
         y = (USHORT)(i + 4);
         Found = FALSE;
      }
      PrintLine ();
      i++;
   } while (Data.Next () != NULL && i < (User->ScreenHeight - 6));

   return (i);
}

VOID TMessageList::PrintTitles (VOID)
{
   Embedded->Printf ("\x0C\x16\x01\x0B%s \x16\x01\x0A- \x16\x01\x0B%s\026\007\n", Current->Key, Current->Display);
   Embedded->Printf ("\026\001\012    #  From                    To                     Subject\n");
   Embedded->Printf ("=====  ======================  =====================  =======================\n");

   Embedded->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, "=====  ======================  =====================  =======================\n");

   Embedded->Printf ("\026\001\012Hit \026\001\013CTRL-V \026\001\012for next page, \026\001\013CTRL-Y \026\001\012for previous page, \026\001\013? \026\001\012for help, or \026\001\013X \026\001\012to exit.\n");
   Embedded->Printf ("\026\001\012To highlight a message, use your \026\001\013arrow keys\026\001\012, \026\001\013RETURN \026\001\012reads it.");

   Embedded->PrintfAt (4, 1, "");
}

VOID TMessageList::PrintLine (VOID)
{
   Embedded->Printf ("\026\001\013%-5.5s  \026\001\016%s\n", pld->Key, pld->Display);
}

VOID TMessageList::PrintCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x70%-5.5s\x16\x01\x0E", (PSZ)List.Value ());
}

VOID TMessageList::RemoveCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x0B%-5.5s\x16\x01\x0E", (PSZ)List.Value ());
}

VOID TMessageList::Select (VOID)
{
   USHORT Line, MaxLine, gotFrom = FALSE, gotTo = FALSE;
   CHAR *Text, Temp[96], *p, *a;
   ULONG Msgn;

   if (Msg != NULL) {
      Msg->Read (atoi ((PSZ)List.Value ()));
      Msgn = Msg->Current;

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
                  *p++;
               strcpy (Msg->FromAddress, p);
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

      strcpy (Temp, "===============================================================================");
      Temp[79 - strlen (Current->Display) - 3] = '\0';
      Embedded->BufferedPrintf ("\x0C\x16\x01\x09= \x16\x01\x0E%s \x16\x01\x09%s\n", Current->Display, Temp);

      Embedded->BufferedPrintf ("\x16\x01\x0A    Msg: \x16\x01\x0E%lu of %lu (%lu left)\n", Msgn, Current->LastMessage, Current->LastMessage - Msgn);
      sprintf (Temp, "%02d %3.3s %d %2d:%02d", Msg->Written.Day, Language->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
      Embedded->BufferedPrintf ("\x16\x01\x0A   From: \x16\x01\x0E%-35.35s \x16\x01\x0F%-16.16s \x16\x01\x07%s\n", Msg->From, Msg->FromAddress, Temp);
      sprintf (Temp, "%02d %3.3s %d %2d:%02d", Msg->Arrived.Day, Language->Months[Msg->Arrived.Month - 1], Msg->Arrived.Year, Msg->Arrived.Hour, Msg->Arrived.Minute);
      Embedded->BufferedPrintf ("\x16\x01\x0A     To: \x16\x01\x0E%-35.35s \x16\x01\x0F%-16.16s \x16\x01\x07%s\n", Msg->To, Msg->ToAddress, Temp);
      Embedded->BufferedPrintf ("\x16\x01\x0ASubject: \x16\x01\x0E%s\n", Msg->Subject);
      Embedded->BufferedPrintf ("\x16\x01\x09===============================================================================\n");
      Line = 6;

      if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
               if (strchr (Text, '>') != NULL)
                  Embedded->BufferedPrintf (Language->MessageQuote, Text);
               else if (!strncmp (Text, "SEEN-BY: ", 9) || Text[0] == 1)
                  Embedded->BufferedPrintf (Language->MessageKludge, Text);
               else if (!strncmp (Text, " * Origin", 9) || !strncmp (Text, "---", 3))
                  Embedded->BufferedPrintf (Language->MessageOrigin, Text);
               else
                  Embedded->BufferedPrintf (Language->MessageText, Text);

               MaxLine = Line;
               if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                  MaxLine++;
                  while (MaxLine > 6)
                     Embedded->BufferedPrintfAt (MaxLine--, 1, "\026\007");
                  Line = 6;
               }
            }
         } while ((Text = (CHAR *)Msg->Text.Next ()) != NULL && Embedded->AbortSession () == FALSE && Line != 0);

      Embedded->UnbufferBytes ();

      if (Line > 6) {
         Embedded->Printf ("\n\026\001\013");
         if (Embedded->MoreQuestion (Line) != 1)
            Embedded->PressEnter ();
      }

      Redraw = Titles = TRUE;
   }
}

// ----------------------------------------------------------------------

TMessage::TMessage (PSZ pszDataPath)
{
   Log = NULL;
   Language = NULL;
   Embedded = NULL;
   Msg = NULL;

   strcpy (DataPath, pszDataPath);
   Current = new TMsgData (DataPath);

   ShowKludges = FALSE;
   Width = 79;
   Height = 24;
   DoCls = More = TRUE;
}

TMessage::~TMessage (void)
{
   if (Msg != NULL) {
      Msg->Close ();
      delete Msg;
   }
   if (Current != NULL)
      delete Current;
}

VOID TMessage::BriefList (VOID)
{
   USHORT NewMessages, Forward, Continue;
   SHORT Line;
   ULONG Number, First, Last, LastRead;
   class TMessageList *List;

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      Number = 0L;
      LastRead = 0L;
      NewMessages = TRUE;

      if (User != NULL) {
         if (User->MsgTag->Read (Current->Key) == TRUE) {
            LastRead = Number = User->MsgTag->LastRead;
            if (Number >= Last)
               NewMessages = FALSE;
         }
      }

      if (Msg->Number () != 0L) {
         StartMessageQuestion (First, Last, NewMessages, Number, Forward);

         if (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE) {
            if ((List = new TMessageList) != NULL) {
               List->Cfg = Cfg;
               List->Embedded = Embedded;
               List->Log = Log;
               List->User = User;
               List->Language = Language;
               List->Current = Current;
               List->Msg = Msg;
               List->Forward = Forward;
               List->Number = Number;
               List->ShowKludges = ShowKludges;
               sprintf (List->LastRead, "%lu", LastRead);
               List->Run ();
               delete List;
            }
         }
         else {
            Line = 2;
            Embedded->Printf ("\x0C\026\001\012    #  From                    To                     Subject\n");
            Embedded->Printf ("-----  ----------------------  ---------------------  -----------------------\n");

            if (Forward == TRUE)
               Continue = Msg->Next (Number);
            else
               Continue = Msg->Previous (Number);

            while (Embedded->AbortSession () == FALSE && Line != 0 && Continue == TRUE) {
               if (Msg->ReadHeader (Number) == TRUE) {
                  if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                     Embedded->Printf ("\x0C    #  From                    To                     Subject\n");
                     Embedded->Printf ("-----  ----------------------  ---------------------  -----------------------\n");
                     Line = 2;
                  }
                  Embedded->Printf ("\026\001\016%5d  \026\001\012%-22.22s  %-21.21s  %-.23s\n", Msg->Current, Msg->From, Msg->To, Msg->Subject);
               }
               if (Forward == TRUE)
                  Continue = Msg->Next (Number);
               else
                  Continue = Msg->Previous (Number);
            }

            Embedded->Printf ("\n\026\001\016End of list! \001\001");
         }
      }
   }
}

VOID TMessage::Delete (VOID)
{
   CHAR szTemp[20];
   ULONG ulMsg, ulNumber, First;

   if (Msg != NULL) {
      ulNumber = Msg->Highest ();
      First = Msg->Lowest ();

      do {
         Embedded->Printf ("\n\x16\x01\013Enter the message number to be erased (%lu-%lu): ", First, ulNumber);
         Embedded->Input (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
         ulMsg = atol (szTemp);
         if (ulMsg < First || ulMsg > ulNumber)
            Embedded->Printf ("\n\x16\x01\x0DThe message #%ld is out of range.\n\006\007\006\007", ulMsg);
      } while (ulMsg > ulNumber && ulMsg != 0 && Embedded->AbortSession () == FALSE);

      if (ulMsg > 0) {
         if (Msg->ReadHeader (ulMsg) == TRUE) {
            if (!stricmp (Msg->From, User->Name) || !stricmp (Msg->To, User->Name)) {
               if (Msg->Delete (ulMsg) == TRUE)
                  Embedded->Printf ("\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%ld ERASED >>>\n", ulMsg);
            }
            else
               Embedded->Printf ("\n\x16\x01\x0DSorry, message number %ld is neither FROM you,\nnor a PRIVATE message TO you, so you can't erase it.\n\006\007\006\007", ulMsg);
         }
         else
            Embedded->Printf ("\n\x16\x01\x0DSorry, message number %ld is neither FROM you,\nnor a PRIVATE message TO you, so you can't erase it.\n\006\007\006\007", ulMsg);

         Current->LastMessage = Msg->Highest ();
         Current->FirstMessage = Msg->Lowest ();
      }
   }
}

VOID TMessage::DisplayCurrent (VOID)
{
   USHORT Line, MaxLine, gotFrom = FALSE, gotTo = FALSE;
   CHAR *Text, Temp[96], *p, *a;
   ULONG Msgn;

   if (Msg != NULL) {
      Msgn = Msg->Current;

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
                  *p++;
               strcpy (Msg->FromAddress, p);
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

      strcpy (Temp, "===============================================================================");
      Temp[79 - strlen (Current->Display) - 3] = '\0';
      Embedded->BufferedPrintf ("\x0C\x16\x01\x09= \x16\x01\x0E%s \x16\x01\x09%s\n", Current->Display, Temp);

      Embedded->BufferedPrintf ("\x16\x01\x0A    Msg: \x16\x01\x0E%lu of %lu (%lu left)\n", Msgn, Current->LastMessage, Current->LastMessage - Msgn);
      sprintf (Temp, "%02d %3.3s %d %2d:%02d", Msg->Written.Day, Language->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
      Embedded->BufferedPrintf ("\x16\x01\x0A   From: \x16\x01\x0E%-35.35s \x16\x01\x0F%-16.16s \x16\x01\x07%s\n", Msg->From, Msg->FromAddress, Temp);
      sprintf (Temp, "%02d %3.3s %d %2d:%02d", Msg->Arrived.Day, Language->Months[Msg->Arrived.Month - 1], Msg->Arrived.Year, Msg->Arrived.Hour, Msg->Arrived.Minute);
      Embedded->BufferedPrintf ("\x16\x01\x0A     To: \x16\x01\x0E%-35.35s \x16\x01\x0F%-16.16s \x16\x01\x07%s\n", Msg->To, Msg->ToAddress, Temp);
      Embedded->BufferedPrintf ("\x16\x01\x0ASubject: \x16\x01\x0E%s\n", Msg->Subject);
      Embedded->BufferedPrintf ("\x16\x01\x09===============================================================================\n");
      Line = 6;

      if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
               if (strchr (Text, '>') != NULL)
                  Embedded->BufferedPrintf (Language->MessageQuote, Text);
               else if (!strncmp (Text, "SEEN-BY: ", 9) || Text[0] == 1)
                  Embedded->BufferedPrintf (Language->MessageKludge, Text);
               else if (!strncmp (Text, " * Origin", 9) || !strncmp (Text, "---", 3))
                  Embedded->BufferedPrintf (Language->MessageOrigin, Text);
               else
                  Embedded->BufferedPrintf (Language->MessageText, Text);

               MaxLine = Line;
               if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                  MaxLine++;
                  while (MaxLine > 6)
                     Embedded->BufferedPrintfAt (MaxLine--, 1, "\026\007");
                  Line = 6;
               }
            }
         } while ((Text = (CHAR *)Msg->Text.Next ()) != NULL && Embedded->AbortSession () == FALSE && Line != 0);

      Embedded->UnbufferBytes ();
   }
}

VOID TMessage::Reply (VOID)
{
   ULONG Number;
   class TMsgEditor *Editor;
   class TFullEditor *FullEditor;

   Number = Msg->Current;

   if (User->Level >= Current->WriteLevel) {
      if ((User->AccessFlags & Current->WriteFlags) == Current->WriteFlags) {
         if (User->Ansi == TRUE || User->Avatar == TRUE) {
            if ((FullEditor = new TFullEditor) != NULL) {
               FullEditor->Embedded = Embedded;
               FullEditor->Msg = Msg;
               FullEditor->Lang = Language;
               FullEditor->Log = Log;
               FullEditor->Width = User->ScreenWidth;
               FullEditor->Height = User->ScreenHeight;
               strcpy (FullEditor->From, User->Name);
               strcpy (FullEditor->AreaTitle, Current->Display);
               if (Current->EchoMail == TRUE) {
                  FullEditor->EchoMail = TRUE;
                  strcpy (FullEditor->Origin, Cfg->SystemName);
                  if (Cfg->MailAddress.First () == TRUE)
                     strcpy (FullEditor->FromAddress, Cfg->MailAddress.String);
               }
               FullEditor->Reply ();
               delete FullEditor;
            }
         }
         else {
            if ((Editor = new TMsgEditor) != NULL) {
               Editor->Cfg = Cfg;
               Editor->Embedded = Embedded;
               Editor->Log = Log;
               Editor->Msg = Msg;
               Editor->Lang = Language;
               Editor->ScreenWidth = User->ScreenWidth;
               strcpy (Editor->UserName, User->Name);
               if (Current->EchoMail == TRUE) {
                  Editor->EchoMail = TRUE;
                  strcpy (Editor->Origin, Cfg->SystemName);
                  if (Cfg->MailAddress.First () == TRUE)
                     strcpy (Editor->Address, Cfg->MailAddress.String);
               }
               if (Editor->Reply () == TRUE)
                  Editor->Menu ();
               delete Editor;
            }
         }

         Current->ActiveMsgs = Msg->Number ();
         Current->FirstMessage = Msg->Lowest ();
         Current->LastMessage = Msg->Highest ();
         Current->Update ();
         Msg->Read (Number);
      }
      else
         Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
   }
   else
      Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
}

VOID TMessage::Read (ULONG Number)
{
   ULONG Last;

   if (Msg != NULL) {
      Last = Msg->Current;
      if (Msg->Read (Number) == TRUE) {
         if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
            Embedded->Printf ("\026\001\015Sorry, you can't read that message.\n\006\007\006\007");
         else {
            DisplayCurrent ();
            Last = Msg->Current;
         }
      }
      else
         Embedded->Printf ("\026\001\015Sorry, you can't read that message.\n\006\007\006\007");

      if (Last != Msg->Current)
         Msg->Read (Last);

      if (User != NULL && Msg->Current != 0L) {
         if (User->MsgTag->Read (Current->Key) == TRUE) {
            User->MsgTag->LastRead = Msg->Current;
            User->MsgTag->Update ();
         }
         else {
            User->MsgTag->New ();
            strcpy (User->MsgTag->Area, Current->Key);
            User->MsgTag->Tagged = FALSE;
            User->MsgTag->LastRead = Msg->Current;
            User->MsgTag->LastPacked = 0L;
            User->MsgTag->Add ();
         }
      }
   }
}

VOID TMessage::ReadMessages (VOID)
{
   USHORT Forward;
   CHAR Cmd, NewMessages, End, DoRead, Temp[40];
   ULONG First, Last, Number;
   class TMsgEditor *Editor;
   class TFullEditor *FullEditor;
   class TMailEditor *MailEditor;

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      Number = 0L;
      Cmd = '\0';

      NewMessages = TRUE;
      if (User != NULL) {
         if (User->MsgTag->Read (Current->Key) == TRUE) {
            Number = User->MsgTag->LastRead;
            if (Number >= Last)
               NewMessages = FALSE;
         }
      }

      if (Msg->Number () != 0L) {
         StartMessageQuestion (First, Last, NewMessages, Number, Forward);

         if (Embedded->AbortSession () == FALSE) {
            DoRead = FALSE;
            do {
               End = TRUE;
               if (Forward == TRUE) {
                  if (Msg->Next (Number) == TRUE) {
                     End = FALSE;
                     if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                        continue;
                     DoRead = TRUE;
                  }
               }
               else {
                  if (Msg->Previous (Number) == TRUE) {
                     End = FALSE;
                     if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                        continue;
                     DoRead = TRUE;
                  }
               }
            } while (End == FALSE && DoRead == FALSE);

            if (End == TRUE) {
               Embedded->Printf (Language->EndOfMessages);
               Cmd = Language->ExitReadMessage;
            }
         }

         while (Embedded->AbortSession () == FALSE && Cmd != Language->ExitReadMessage) {
            if (End == FALSE && DoRead == TRUE) {
               if (Msg->Read (Number) == TRUE)
                  DisplayCurrent ();
               DoRead = FALSE;
            }

            if (End == FALSE)
               Embedded->Printf (Language->ReadMenu);
            else
               Embedded->Printf (Language->EndReadMenu);
            Embedded->GetString (Temp, 10, INP_HOTKEY|INP_NUMERIC);
            if ((Cmd = (CHAR)toupper (Temp[0])) == '\0')
               Cmd = Language->NextMessage;

            if (isdigit (Cmd)) {
               if (Msg->ReadHeader (atol (Temp)) == TRUE) {
                  Number = atol (Temp);
                  DoRead = TRUE;
                  End = FALSE;
               }
            }
            else if (Cmd == Language->NextMessage) {
               if (End == FALSE) {
                  do {
                     End = TRUE;
                     if (Forward == TRUE) {
                        if (Msg->Next (Number) == TRUE) {
                           End = FALSE;
                           if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                              continue;
                           DoRead = TRUE;
                        }
                     }
                     else {
                        if (Msg->Previous (Number) == TRUE) {
                           End = FALSE;
                           if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                              continue;
                           DoRead = TRUE;
                        }
                     }
                  } while (End == FALSE && DoRead == FALSE);
               }

               if (End == TRUE)
                  Embedded->Printf (Language->EndOfMessages);
            }
            else if (Cmd == Language->RereadMessage && End == TRUE) {
               DoRead = TRUE;
               End = FALSE;
            }
            else if (Cmd == Language->PreviousMessage) {
               do {
                  End = TRUE;
                  if (Forward == TRUE) {
                     if (Msg->Previous (Number) == TRUE) {
                        End = FALSE;
                        if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                           continue;
                        DoRead = TRUE;
                     }
                  }
                  else {
                     if (Msg->Next (Number) == TRUE) {
                        End = FALSE;
                        if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                           continue;
                        DoRead = TRUE;
                     }
                  }
               } while (End == FALSE && DoRead == FALSE);

               if (End == TRUE)
                  Embedded->Printf (Language->EndOfMessages);
            }
            else if (Cmd == Language->ReplyMessage) {
               if (User->Level >= Current->WriteLevel) {
                  if ((User->AccessFlags & Current->WriteFlags) == Current->WriteFlags) {
                     if (User->Ansi == TRUE || User->Avatar == TRUE) {
                        if ((FullEditor = new TFullEditor) != NULL) {
                           FullEditor->Embedded = Embedded;
                           FullEditor->Msg = Msg;
                           FullEditor->Lang = Language;
                           FullEditor->Log = Log;
                           FullEditor->Width = User->ScreenWidth;
                           FullEditor->Height = User->ScreenHeight;
                           strcpy (FullEditor->From, User->Name);
                           strcpy (FullEditor->AreaTitle, Current->Display);
                           if (Current->EchoMail == TRUE) {
                              FullEditor->EchoMail = TRUE;
                              strcpy (FullEditor->Origin, Cfg->SystemName);
                              if (Cfg->MailAddress.First () == TRUE)
                                 strcpy (FullEditor->FromAddress, Cfg->MailAddress.String);
                           }
                           FullEditor->Reply ();
                           delete FullEditor;
                        }
                     }
                     else {
                        if ((Editor = new TMsgEditor) != NULL) {
                           Editor->Cfg = Cfg;
                           Editor->Embedded = Embedded;
                           Editor->Log = Log;
                           Editor->Msg = Msg;
                           Editor->Lang = Language;
                           Editor->ScreenWidth = User->ScreenWidth;
                           strcpy (Editor->UserName, User->Name);
                           if (Current->EchoMail == TRUE) {
                              Editor->EchoMail = TRUE;
                              strcpy (Editor->Origin, Cfg->SystemName);
                              if (Cfg->MailAddress.First () == TRUE)
                                 strcpy (Editor->Address, Cfg->MailAddress.String);
                           }
                           if (Editor->Reply () == TRUE)
                              Editor->Menu ();
                           delete Editor;
                        }
                     }

                     Current->ActiveMsgs = Msg->Number ();
                     Current->FirstMessage = Msg->Lowest ();
                     Current->LastMessage = Msg->Highest ();
                     Current->Update ();
                     Msg->Read (Number);
                     Last = Msg->Highest ();
                  }
                  else
                     Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
               }
               else
                  Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
            }
            else if (Cmd == Language->EMailReplyMessage && End == FALSE) {
               if ((MailEditor = new TMailEditor) != NULL) {
                  Editor->Cfg = Cfg;
                  MailEditor->Embedded = Embedded;
                  MailEditor->Log = Log;
                  MailEditor->Msg = Msg;
                  MailEditor->Lang = Language;
                  MailEditor->ScreenWidth = User->ScreenWidth;
                  strcpy (MailEditor->UserName, User->Name);
                  if (Cfg->MailAddress.First () == TRUE)
                     strcpy (MailEditor->Address, Cfg->MailAddress.String);
                  if (MailEditor->Reply () == TRUE)
                     MailEditor->Menu ();
                  delete MailEditor;
               }
               Last = Msg->Highest ();
            }
         }
      }

      if (User != NULL) {
         if (User->MsgTag->Read (Current->Key) == TRUE) {
            User->MsgTag->LastRead = Number;
            User->MsgTag->Update ();
         }
         else {
            User->MsgTag->New ();
            strcpy (User->MsgTag->Area, Current->Key);
            User->MsgTag->Tagged = FALSE;
            User->MsgTag->LastRead = Number;
            User->MsgTag->LastPacked = 0L;
            User->MsgTag->Add ();
         }
      }
   }
}

VOID TMessage::ReadNext (VOID)
{
   ULONG Number = 0L;

   if (User != NULL) {
      if (User->MsgTag->Read (Current->Key) == TRUE)
         Number = User->MsgTag->LastRead;
   }

   if (Msg != NULL) {
      if (Msg->Next (Number) == TRUE) {
         Msg->Read (Number);
         DisplayCurrent ();
      }
      else
         Embedded->Printf (Language->EndOfMessages);

      if (User != NULL && Msg->Current != 0L) {
         if (User->MsgTag->Read (Current->Key) == TRUE) {
            User->MsgTag->LastRead = Msg->Current;
            User->MsgTag->Update ();
         }
         else {
            User->MsgTag->New ();
            strcpy (User->MsgTag->Area, Current->Key);
            User->MsgTag->Tagged = FALSE;
            User->MsgTag->LastRead = Msg->Current;
            User->MsgTag->LastPacked = 0L;
            User->MsgTag->Add ();
         }
      }
   }
}

VOID TMessage::ReadPrevious (VOID)
{
   ULONG Number = 0L;

   if (User != NULL) {
      if (User->MsgTag->Read (Current->Key) == TRUE)
         Number = User->MsgTag->LastRead;
   }

   if (Msg != NULL) {
      if (Msg->Previous (Number) == TRUE) {
         Msg->Read (Number);
         DisplayCurrent ();
      }
      else
         Embedded->Printf (Language->EndOfMessages);

      if (User != NULL && Msg->Current != 0L) {
         if (User->MsgTag->Read (Current->Key) == TRUE) {
            User->MsgTag->LastRead = Msg->Current;
            User->MsgTag->Update ();
         }
         else {
            User->MsgTag->New ();
            strcpy (User->MsgTag->Area, Current->Key);
            User->MsgTag->Tagged = FALSE;
            User->MsgTag->LastRead = Msg->Current;
            User->MsgTag->LastPacked = 0L;
            User->MsgTag->Add ();
         }
      }
   }
}

USHORT TMessage::SelectArea (PSZ pszArea)
{
   USHORT RetVal = FALSE, FirstHit, DoList;
   SHORT Line;
   CHAR Command[16], Temp[128];
   class TMsgData *Data;
   class TMsgAreaListing *List;

   DoList = FALSE;
   if (pszArea != NULL && *pszArea != '\0') {
      strcpy (Command, pszArea);
      DoList = TRUE;
   }

   if ((Data = new TMsgData (DataPath)) != NULL) {
      do {
         if (DoList == FALSE) {
            Embedded->Printf (Language->SelectConference);
            Embedded->Input (Command, (USHORT)(sizeof (Command) - 1), INP_FIELD);
         }
         else
            DoList = FALSE;

         if (toupper (Command[0]) == Language->Help) {
            if (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE) {
               if ((List = new TMsgAreaListing) != NULL) {
                  List->Cfg = Cfg;
                  List->Embedded = Embedded;
                  List->Log = Log;
                  List->Current = Current;
                  List->User = User;
                  List->Language = Language;
                  if ((RetVal = List->Run ()) == TRUE) {
                     if (Msg != NULL) {
                        Msg->Close ();
                        delete Msg;
                     }

                     Msg = NULL;
                     if (Current->Storage == ST_JAM)
                        Msg = new JAM (Current->Path);
                     else if (Current->Storage == ST_SQUISH)
                        Msg = new SQUISH (Current->Path);
                     else if (Current->Storage == ST_USENET)
                        Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                     else if (Current->Storage == ST_FIDO)
                        Msg = new FIDOSDM (Current->Path);
                     else if (Current->Storage == ST_ADEPT)
                        Msg = new ADEPT (Current->Path);

                     if (Msg != NULL) {
                        Current->ActiveMsgs = Msg->Number ();
                        Current->FirstMessage = Msg->Lowest ();
                        Current->LastMessage = Msg->Highest ();
                     }
                     Current->Update ();
                  }
                  delete List;
               }
            }
            else {
               if (Data->First () == TRUE) {
                  Embedded->Printf (Language->ConferenceListHdr);
                  Line = 3;
                  do {
                     if (User->Level >= Data->Level) {
                        if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                           Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                           Line = Embedded->MoreQuestion (Line);
                        }
                     }
                  } while (Line != 0 && Embedded->AbortSession () == FALSE && Data->Next () == TRUE);
               }
            }
         }
         else if (Command[0] != '\0') {
            if (Data->Read (Command) == TRUE) {
               if (User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     delete Data;
                     Data = NULL;
                     if (Msg != NULL) {
                        Msg->Close ();
                        delete Msg;
                     }

                     Current->Read (Command);
                     if (User != NULL) {
                        strcpy (User->LastMsgArea, Current->Key);
                        User->Update ();
                     }

                     Msg = NULL;
                     if (Current->Storage == ST_JAM)
                        Msg = new JAM (Current->Path);
                     else if (Current->Storage == ST_SQUISH)
                        Msg = new SQUISH (Current->Path);
                     else if (Current->Storage == ST_USENET)
                        Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                     else if (Current->Storage == ST_FIDO)
                        Msg = new FIDOSDM (Current->Path);
                     else if (Current->Storage == ST_ADEPT)
                        Msg = new ADEPT (Current->Path);

                     Log->Write (":Message Area: %s - %s", Current->Key, Current->Display);
                     if (Msg != NULL) {
                        Current->ActiveMsgs = Msg->Number ();
                        Current->FirstMessage = Msg->Lowest ();
                        Current->LastMessage = Msg->Highest ();
                     }
                     Current->Update ();
                     RetVal = TRUE;
                  }
               }
            }

            if (RetVal == FALSE) {
               if (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE) {
                  if ((List = new TMsgAreaListing) != NULL) {
                     List->Cfg = Cfg;
                     List->Embedded = Embedded;
                     List->Log = Log;
                     List->Current = Current;
                     List->User = User;
                     List->Language = Language;
                     strcpy (List->Command, Command);
                     if ((RetVal = List->Run ()) == TRUE) {
                        if (Msg != NULL) {
                           Msg->Close ();
                           delete Msg;
                        }

                        Msg = NULL;
                        if (Current->Storage == ST_JAM)
                           Msg = new JAM (Current->Path);
                        else if (Current->Storage == ST_SQUISH)
                           Msg = new SQUISH (Current->Path);
                        else if (Current->Storage == ST_USENET)
                           Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                        else if (Current->Storage == ST_FIDO)
                           Msg = new FIDOSDM (Current->Path);
                        else if (Current->Storage == ST_ADEPT)
                           Msg = new ADEPT (Current->Path);

                        if (Msg != NULL) {
                           Current->ActiveMsgs = Msg->Number ();
                           Current->FirstMessage = Msg->Lowest ();
                           Current->LastMessage = Msg->Highest ();
                        }
                        Current->Update ();
                     }
                     delete List;
                  }
               }
               else {
                  FirstHit = TRUE;
                  strupr (Command);
                  Line = 3;

                  if (Data->First () == TRUE) {
                     do {
                        if (User->Level >= Data->Level) {
                           if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                              strcpy (Temp, Data->Key);
                              if (strstr (strupr (Temp), Command) != NULL) {
                                 if (FirstHit == TRUE)
                                    Embedded->Printf (Language->ConferenceListHdr);
                                 Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                                 Line = Embedded->MoreQuestion (Line);
                                 FirstHit = FALSE;
                              }
                              else {
                                 strcpy (Temp, Data->Display);
                                 if (strstr (strupr (Temp), Command) != NULL) {
                                    if (FirstHit == TRUE)
                                       Embedded->Printf (Language->ConferenceListHdr);
                                    Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                                    Line = Embedded->MoreQuestion (Line);
                                    FirstHit = FALSE;
                                 }
                                 else {
                                    strcpy (Temp, Data->EchoTag);
                                    if (strstr (strupr (Temp), Command) != NULL) {
                                       if (FirstHit == TRUE)
                                          Embedded->Printf (Language->ConferenceListHdr);
                                       Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                                       Line = Embedded->MoreQuestion (Line);
                                       FirstHit = FALSE;
                                    }
                                    else {
                                       strcpy (Temp, Data->NewsGroup);
                                       if (strstr (strupr (Temp), Command) != NULL) {
                                          if (FirstHit == TRUE)
                                             Embedded->Printf (Language->ConferenceListHdr);
                                          Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                                          Line = Embedded->MoreQuestion (Line);
                                          FirstHit = FALSE;
                                       }
                                    }
                                 }
                              }
                           }
                        }
                     } while (Line != 0 && Embedded->AbortSession () == FALSE && Data->Next () == TRUE);
                  }

                  if (FirstHit == TRUE)
                     Embedded->Printf (Language->ConferenceNotAvailable);
               }
               if (pszArea != NULL && *pszArea != '\0')
                  Command[0] = '\0';
            }
         }
      } while (Command[0] != '\0' && RetVal == FALSE && Embedded->AbortSession () == FALSE);

      if (Data != NULL)
         delete Data;
   }

   return (RetVal);
}

USHORT TMessage::SelectNewArea (PSZ pszArea)
{
   USHORT RetVal = FALSE, FirstHit, DoList;
   SHORT Line;
   CHAR Command[16], Temp[128];
   class TMsgData *Data;
   class TMsgNewAreaListing *List;

   DoList = FALSE;
   if (pszArea != NULL && *pszArea != '\0') {
      strcpy (Command, pszArea);
      DoList = TRUE;
   }

   if ((Data = new TMsgData (DataPath)) != NULL) {
      do {
         if (DoList == FALSE) {
            Embedded->Printf (Language->SelectConference);
            Embedded->Input (Command, (USHORT)(sizeof (Command) - 1), INP_FIELD);
         }
         else
            DoList = FALSE;

         if (toupper (Command[0]) == Language->Help) {
            if (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE) {
               if ((List = new TMsgNewAreaListing) != NULL) {
                  List->Cfg = Cfg;
                  List->Embedded = Embedded;
                  List->Log = Log;
                  List->Current = Current;
                  List->User = User;
                  List->Language = Language;
                  if ((RetVal = List->Run ()) == TRUE) {
                     if (Msg != NULL) {
                        Msg->Close ();
                        delete Msg;
                     }

                     Msg = NULL;
                     if (Current->Storage == ST_JAM)
                        Msg = new JAM (Current->Path);
                     else if (Current->Storage == ST_SQUISH)
                        Msg = new SQUISH (Current->Path);
                     else if (Current->Storage == ST_USENET)
                        Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                     else if (Current->Storage == ST_FIDO)
                        Msg = new FIDOSDM (Current->Path);
                     else if (Current->Storage == ST_ADEPT)
                        Msg = new ADEPT (Current->Path);

                     if (Msg != NULL) {
                        Current->ActiveMsgs = Msg->Number ();
                        Current->FirstMessage = Msg->Lowest ();
                        Current->LastMessage = Msg->Highest ();
                     }
                     Current->Update ();
                  }
                  delete List;
               }
            }
            else {
               if (Data->First () == TRUE) {
                  Embedded->Printf (Language->ConferenceListHdr);
                  Line = 3;
                  do {
                     if (User->Level >= Data->Level) {
                        if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                           if (User->MsgTag->Read (Data->Key) == TRUE) {
                              if (User->MsgTag->LastRead < Data->LastMessage) {
                                 Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                                 Line = Embedded->MoreQuestion (Line);
                              }
                           }
                        }
                     }
                  } while (Line != 0 && Embedded->AbortSession () == FALSE && Data->Next () == TRUE);
               }
            }
         }
         else if (Command[0] != '\0') {
            if (Data->Read (Command) == TRUE) {
               if (User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     delete Data;
                     Data = NULL;
                     if (Msg != NULL) {
                        Msg->Close ();
                        delete Msg;
                     }

                     Current->Read (Command);
                     if (User != NULL) {
                        strcpy (User->LastMsgArea, Current->Key);
                        User->Update ();
                     }

                     Msg = NULL;
                     if (Current->Storage == ST_JAM)
                        Msg = new JAM (Current->Path);
                     else if (Current->Storage == ST_SQUISH)
                        Msg = new SQUISH (Current->Path);
                     else if (Current->Storage == ST_USENET)
                        Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                     else if (Current->Storage == ST_FIDO)
                        Msg = new FIDOSDM (Current->Path);
                     else if (Current->Storage == ST_ADEPT)
                        Msg = new ADEPT (Current->Path);

                     Log->Write (":Message Area: %s - %s", Current->Key, Current->Display);
                     if (Msg != NULL) {
                        Current->ActiveMsgs = Msg->Number ();
                        Current->FirstMessage = Msg->Lowest ();
                        Current->LastMessage = Msg->Highest ();
                     }
                     Current->Update ();
                     RetVal = TRUE;
                  }
               }
            }

            if (RetVal == FALSE) {
               FirstHit = TRUE;
               strupr (Command);
               Line = 3;

               if (Data->First () == TRUE) {
                  do {
                     if (User->Level >= Data->Level) {
                        if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                           if (User->MsgTag->Read (Data->Key) == TRUE) {
                              if (User->MsgTag->LastRead < Data->LastMessage) {
                                 strcpy (Temp, Data->Key);
                                 if (strstr (strupr (Temp), Command) != NULL) {
                                    if (FirstHit == TRUE)
                                       Embedded->Printf (Language->ConferenceListHdr);
                                    Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                                    Line = Embedded->MoreQuestion (Line);
                                    FirstHit = FALSE;
                                 }
                                 else {
                                    strcpy (Temp, Data->Display);
                                    if (strstr (strupr (Temp), Command) != NULL) {
                                       if (FirstHit == TRUE)
                                          Embedded->Printf (Language->ConferenceListHdr);
                                       Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                                       Line = Embedded->MoreQuestion (Line);
                                       FirstHit = FALSE;
                                    }
                                    else {
                                       strcpy (Temp, Data->EchoTag);
                                       if (strstr (strupr (Temp), Command) != NULL) {
                                          if (FirstHit == TRUE)
                                             Embedded->Printf (Language->ConferenceListHdr);
                                          Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                                          Line = Embedded->MoreQuestion (Line);
                                          FirstHit = FALSE;
                                       }
                                       else {
                                          strcpy (Temp, Data->NewsGroup);
                                          if (strstr (strupr (Temp), Command) != NULL) {
                                             if (FirstHit == TRUE)
                                                Embedded->Printf (Language->ConferenceListHdr);
                                             Embedded->Printf (Language->ConferenceList, Data->Key, Data->ActiveMsgs, Data->Display);
                                             Line = Embedded->MoreQuestion (Line);
                                             FirstHit = FALSE;
                                          }
                                       }
                                    }
                                 }
                              }
                           }
                        }
                     }
                  } while (Line != 0 && Embedded->AbortSession () == FALSE && Data->Next () == TRUE);
               }

               if (FirstHit == TRUE)
                  Embedded->Printf (Language->ConferenceNotAvailable);
            }
         }
      } while (Command[0] != '\0' && RetVal == FALSE && Embedded->AbortSession () == FALSE);

      if (Data != NULL)
         delete Data;
   }

   return (RetVal);
}

VOID TMessage::StartMessageQuestion (ULONG ulFirst, ULONG ulLast, USHORT fNewMessages, ULONG &ulMsg, USHORT &fForward)
{
   CHAR Cmd, Temp[20];

   do {
      Embedded->Printf (Language->StartWithMessage);
      if (fNewMessages == TRUE)
         Embedded->Printf (Language->NewMessages);
      Embedded->Printf (": ");
      Embedded->Input (Temp, (USHORT)(sizeof (Temp) - 1), INP_HOTKEY|INP_NUMERIC);
      Cmd = (CHAR)toupper (Temp[0]);
      if (Cmd == Language->Help)
         Embedded->DisplayFile ("MAILSTRT");
   } while (Embedded->AbortSession () == FALSE && Cmd != 'F' && Cmd != 'L' && Cmd != '\0' && !isdigit (Cmd));

   fForward = TRUE;
   if (Cmd == 'F') {
      if ((ulMsg = ulFirst) > 0L)
         ulMsg--;
   }
   else if (Cmd == 'L') {
      ulMsg = ulLast + 1L;
      fForward = FALSE;
   }
   else if (isdigit (Cmd)) {
      if ((ulMsg = atol (Temp)) > 0L)
         ulMsg--;
   }
}

VOID TMessage::TextList (VOID)
{
   USHORT NewMessages, Forward, Continue;
   SHORT Line;
   CHAR *Text, Temp[64];
   ULONG Number, First, Last;

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      Number = 0L;
      NewMessages = TRUE;
      if (User != NULL) {
         if (User->MsgTag->Read (Current->Key) == TRUE) {
            Number = User->MsgTag->LastRead;
            if (Number >= Last)
               NewMessages = FALSE;
         }
      }

      if (Msg->Number () > 0L) {
         StartMessageQuestion (First, Last, NewMessages, Number, Forward);

         Line = 4;
         Embedded->Printf ("\026\001\012FORUM: %s\n", Current->Key);
         if (Current->Moderator[0] != '\0') {
            Embedded->Printf ("\026\001\012Forum-Op: %s\n", Current->Moderator);
            Line++;
         }
         Embedded->Printf ("\026\001\012Forum Topic: %s\n\n", Current->Display);

         if (Forward == TRUE)
            Continue = Msg->Next (Number);
         else
            Continue = Msg->Previous (Number);

         while (Embedded->AbortSession () == FALSE && Line != 0 && Continue == TRUE) {
            if (Msg->Read (Number) == TRUE) {
               sprintf (Temp, "%d %s %d %2d:%02d", Msg->Written.Day, Language->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
               Embedded->Printf ("\026\001\012   Date: %-30.30s %30s\n", Temp, Current->Display);

               if ((Line = Embedded->MoreQuestion (Line)) != 0) {
                  sprintf (Temp, "Msg#: %lu", Msg->Current);
                  Embedded->Printf ("   From: %-40.40s %20s\n", Msg->From, Temp);

                  if ((Line = Embedded->MoreQuestion (Line)) != 0) {
                     Embedded->Printf ("     To: %.70s\n", Msg->To);
                     if ((Line = Embedded->MoreQuestion (Line)) != 0) {
                        Embedded->Printf ("Subject: %.70s\n\n", Msg->Subject);
                        Line = Embedded->MoreQuestion (Line);
                     }
                  }
               }

               if (Line != 0) {
                  Embedded->Printf ("\n");
                  if ((Line = Embedded->MoreQuestion (Line)) != 0) {
                     if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
                        do {
                           if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
                              if (strchr (Text, '>') != NULL)
                                 Embedded->Printf ("\026\001\012%s\n", Text);
                              else if (!strncmp (Text, "SEEN-BY: ", 9) || Text[0] == 1)
                                 Embedded->Printf ("\026\001\012%s\n", Text);
                              else if (!strncmp (Text, " * Origin", 9) || !strncmp (Text, "---", 3))
                                 Embedded->Printf ("\026\001\012%s\n", Text);
                              else
                                 Embedded->Printf ("\026\001\012%s\n", Text);
                              Line = Embedded->MoreQuestion (Line);
                           }
                        } while ((Text = (PSZ)Msg->Text.Next ()) != NULL && Embedded->AbortSession () == FALSE && Line != 0);
                  }
                  if (Line != 0) {
                     Embedded->Printf ("\n");
                     Line = Embedded->MoreQuestion (Line);
                  }
               }
            }

            if (Line != 0) {
               if (Forward == TRUE)
                  Continue = Msg->Next (Number);
               else
                  Continue = Msg->Previous (Number);
            }
         }

         Embedded->Printf ("\n\026\001\016End of list!\n");
         if (Line > 1)
            Embedded->PressEnter ();
      }
   }
}

VOID TMessage::TitleList (VOID)
{
   USHORT NewMessages, Forward, Continue;
   SHORT Line;
   CHAR Temp[64];
   ULONG Number, First, Last;

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      Number = 0L;
      NewMessages = TRUE;
      if (User != NULL) {
         if (User->MsgTag->Read (Current->Key) == TRUE) {
            Number = User->MsgTag->LastRead;
            if (Number >= Last)
               NewMessages = FALSE;
         }
      }

      if (Msg->Number () > 0L) {
         StartMessageQuestion (First, Last, NewMessages, Number, Forward);

         Line = 3;
         Embedded->Printf ("\026\001\012FORUM: %s\n", Current->Key);
         if (Current->Moderator[0] != '\0') {
            Embedded->Printf ("\026\001\012Forum-Op: %s\n", Current->Moderator);
            Line++;
         }
         Embedded->Printf ("\026\001\012Forum Topic: %s\n\n", Current->Display);

         if (Forward == TRUE)
            Continue = Msg->Next (Number);
         else
            Continue = Msg->Previous (Number);

         while (Embedded->AbortSession () == FALSE && Line != 0 && Continue == TRUE) {
            if (Msg->ReadHeader (Number) == TRUE) {
               sprintf (Temp, "%d %s %d %2d:%02d", Msg->Written.Day, Language->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
               Embedded->Printf ("\026\001\012   Date: %-30.30s %30s\n", Temp, Current->Display);

               if ((Line = Embedded->MoreQuestion (Line)) != 0) {
                  sprintf (Temp, "Msg#: %lu", Msg->Current);
                  Embedded->Printf ("   From: %-40.40s %20s\n", Msg->From, Temp);

                  if ((Line = Embedded->MoreQuestion (Line)) != 0) {
                     Embedded->Printf ("     To: %.70s\n", Msg->To);
                     if ((Line = Embedded->MoreQuestion (Line)) != 0) {
                        Embedded->Printf ("Subject: %.70s\n", Msg->Subject);
                        Line = Embedded->MoreQuestion (Line);
                     }
                  }
               }
               Embedded->Printf ("\n");
               Line = Embedded->MoreQuestion (Line);
            }

            if (Forward == TRUE)
               Continue = Msg->Next (Number);
            else
               Continue = Msg->Previous (Number);
         }

         Embedded->Printf ("\n\026\001\016End of list!\n");
         if (Line > 1)
            Embedded->PressEnter ();
      }
   }
}

// ----------------------------------------------------------------------

TMsgAreaListing::TMsgAreaListing (void)
{
   Command[0] = '\0';
}

VOID TMsgAreaListing::Begin (VOID)
{
   USHORT i, Add;
   CHAR Temp[128];
   LISTDATA ld;
   class TMsgData *MsgData;

   i = 0;
   y = 4;
   Found = FALSE;
   List.Clear ();
   Data.Clear ();
   strupr (Command);

   if ((MsgData = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (MsgData->First () == TRUE)
         do {
            if (User->Level >= MsgData->Level) {
               if ((MsgData->AccessFlags & User->AccessFlags) == MsgData->AccessFlags) {
                  if (Command[0] != '\0') {
                     Add = FALSE;
                     strcpy (Temp, MsgData->Key);
                     if (strstr (strupr (Temp), Command) != NULL)
                        Add = TRUE;
                     else {
                        strcpy (Temp, MsgData->Display);
                        if (strstr (strupr (Temp), Command) != NULL)
                           Add = TRUE;
                        else {
                           strcpy (Temp, MsgData->EchoTag);
                           if (strstr (strupr (Temp), Command) != NULL)
                              Add = TRUE;
                           else {
                              strcpy (Temp, MsgData->NewsGroup);
                              if (strstr (strupr (Temp), Command) != NULL)
                                 Add = TRUE;
                           }
                        }
                     }
                  }

                  if (Command[0] == '\0' || Add == TRUE) {
                     strcpy (ld.Key, MsgData->Key);
                     ld.ActiveMsgs = MsgData->ActiveMsgs;
                     strcpy (ld.Display, MsgData->Display);
                     Data.Add (&ld, sizeof (LISTDATA));
                  }
               }
            }
         } while (MsgData->Next () == TRUE);

      delete MsgData;
   }

   if ((pld = (LISTDATA *)Data.First ()) != NULL) {
      do {
         if (!strcmp (pld->Key, Current->Key))
            Found = TRUE;
         List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
         i++;
         if (i >= (User->ScreenHeight - 6)) {
            if (Found == TRUE)
               break;
            List.Clear ();
            i = 0;
         }
      } while ((pld = (LISTDATA *)Data.Next ()) != NULL);
   }
}

USHORT TMsgAreaListing::DrawScreen (VOID)
{
   USHORT i;

   i = 0;
   do {
      pld = (LISTDATA *)Data.Value ();
      if (Found == TRUE && !strcmp (pld->Key, Current->Key)) {
         y = (USHORT)(i + 4);
         Found = FALSE;
      }
      PrintLine ();
      i++;
   } while (Data.Next () != NULL && i < (User->ScreenHeight - 6));

   return (i);
}

VOID TMsgAreaListing::PrintTitles (VOID)
{
   Embedded->Printf ("\x0C\n");
   Embedded->Printf ("\026\001\012Conference       Msgs   Description\n\026\001\017\031=\017  \031=\005  \031=\067\n");

   Embedded->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, "\026\001\017\031=\017  \031=\005  \031=\067\n");

   Embedded->Printf ("\026\001\012Hit \026\001\013CTRL-V \026\001\012for next page, \026\001\013CTRL-Y \026\001\012for previous page, \026\001\013? \026\001\012for help, or \026\001\013X \026\001\012to exit.\n");
   Embedded->Printf ("\026\001\012To highlight an area, use your \026\001\013arrow keys\026\001\012, \026\001\013RETURN \026\001\012selects it.");

   Embedded->PrintfAt (4, 1, "");
}

VOID TMsgAreaListing::PrintLine (VOID)
{
   Embedded->Printf (Language->ConferenceList, pld->Key, pld->ActiveMsgs, pld->Display);
}

VOID TMsgAreaListing::PrintCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x70%-15.15s\x16\x01\x0E", (PSZ)List.Value ());
}

VOID TMsgAreaListing::RemoveCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x0B%-15.15s\x16\x01\x0E", (PSZ)List.Value ());
}

VOID TMsgAreaListing::Select (VOID)
{
   Current->Read ((PSZ)List.Value ());
   if (User != NULL) {
      strcpy (User->LastMsgArea, Current->Key);
      User->Update ();
   }
   if (Log != NULL)
      Log->Write (":Message Area: %s - %s", Current->Key, Current->Display);

   RetVal = End = TRUE;
}

// ----------------------------------------------------------------------

VOID TMsgNewAreaListing::Begin (VOID)
{
   USHORT i;
   LISTDATA ld;
   class TMsgData *MsgData;

   i = 0;
   y = 4;
   Found = FALSE;
   List.Clear ();
   Data.Clear ();

   if ((MsgData = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (MsgData->First () == TRUE)
         do {
            if (User->Level >= MsgData->Level) {
               if ((MsgData->AccessFlags & User->AccessFlags) == MsgData->AccessFlags) {
                  if (User->MsgTag->Read (MsgData->Key) == TRUE) {
                     if (User->MsgTag->LastRead < MsgData->LastMessage) {
                        strcpy (ld.Key, MsgData->Key);
                        ld.ActiveMsgs = MsgData->ActiveMsgs;
                        strcpy (ld.Display, MsgData->Display);
                        Data.Add (&ld, sizeof (LISTDATA));
                     }
                  }
               }
            }
         } while (MsgData->Next () == TRUE);

      delete MsgData;
   }

   if ((pld = (LISTDATA *)Data.First ()) != NULL) {
      do {
         if (!strcmp (pld->Key, Current->Key))
            Found = TRUE;
         List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
         i++;
         if (i >= (User->ScreenHeight - 6)) {
            if (Found == TRUE)
               break;
            List.Clear ();
            i = 0;
         }
      } while ((pld = (LISTDATA *)Data.Next ()) != NULL);
   }
}

USHORT TMsgNewAreaListing::DrawScreen (VOID)
{
   USHORT i;

   i = 0;
   do {
      pld = (LISTDATA *)Data.Value ();
      if (Found == TRUE && !strcmp (pld->Key, Current->Key)) {
         y = (USHORT)(i + 4);
         Found = FALSE;
      }
      PrintLine ();
      i++;
   } while (Data.Next () != NULL && i < (User->ScreenHeight - 6));

   return (i);
}

VOID TMsgNewAreaListing::PrintTitles (VOID)
{
   Embedded->Printf ("\x0C\n");
   Embedded->Printf ("\026\001\012Conference       Msgs   Description\n\026\001\017\031=\017  \031=\005  \031=\067\n");

   Embedded->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, "\026\001\017\031=\017  \031=\005  \031=\067\n");

   Embedded->Printf ("\026\001\012Hit \026\001\013CTRL-V \026\001\012for next page, \026\001\013CTRL-Y \026\001\012for previous page, \026\001\013? \026\001\012for help, or \026\001\013X \026\001\012to exit.\n");
   Embedded->Printf ("\026\001\012To highlight an area, use your \026\001\013arrow keys\026\001\012, \026\001\013RETURN \026\001\012selects it.");

   Embedded->PrintfAt (4, 1, "");
}

VOID TMsgNewAreaListing::PrintLine (VOID)
{
   Embedded->Printf (Language->ConferenceList, pld->Key, pld->ActiveMsgs, pld->Display);
}

VOID TMsgNewAreaListing::PrintCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x70%-15.15s\x16\x01\x0E", (PSZ)List.Value ());
}

VOID TMsgNewAreaListing::RemoveCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x0B%-15.15s\x16\x01\x0E", (PSZ)List.Value ());
}

VOID TMsgNewAreaListing::Select (VOID)
{
   Current->Read ((PSZ)List.Value ());
   if (User != NULL) {
      strcpy (User->LastMsgArea, Current->Key);
      User->Update ();
   }
   Log->Write (":Message Area: %s - %s", Current->Key, Current->Display);

   RetVal = End = TRUE;
}

// ---------------------------------------------------------------------------

TInquire::TInquire (void)
{
   Type = TYPE_ALL;
   Action = ACTION_UNDEFINED;
   Range = RANGE_UNDEFINED;
   User = NULL;
   Embedded = NULL;
   Language = NULL;
   ShowKludges = FALSE;
   Current = Data = NULL;
   Log = NULL;
   Stop = FALSE;
}

TInquire::~TInquire (void)
{
}

VOID TInquire::DeleteCurrent (VOID)
{
   if (Msg != NULL && Number > 0) {
      if (Msg->ReadHeader (Number) == TRUE) {
         if (!stricmp (Msg->From, User->Name) || !stricmp (Msg->To, User->Name)) {
            if (Msg->Delete (Number) == TRUE)
               Embedded->Printf ("\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%ld ERASED >>>\n", Number);
         }
         else
            Embedded->Printf ("\n\x16\x01\x0DSorry, message number %ld is neither FROM you,\nnor a PRIVATE message TO you, so you can't erase it.\n\006\007\006\007", Number);
      }
      else
         Embedded->Printf ("\n\x16\x01\x0DSorry, message number %ld is neither FROM you,\nnor a PRIVATE message TO you, so you can't erase it.\n\006\007\006\007", Number);
   }
}

VOID TInquire::DisplayCurrent (VOID)
{
   USHORT Line, MaxLine, gotFrom = FALSE, gotTo = FALSE;
   CHAR *Text, Temp[96], *p, *a;
   ULONG Msgn;

   if (Msg != NULL) {
      Msgn = Msg->Current;

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
                  *p++;
               strcpy (Msg->FromAddress, p);
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

      strcpy (Temp, "===============================================================================");
      Temp[79 - strlen (Current->Display) - 3] = '\0';
      Embedded->BufferedPrintf ("\x0C\x16\x01\x09= \x16\x01\x0E%s \x16\x01\x09%s\n", Current->Display, Temp);

      Embedded->BufferedPrintf ("\x16\x01\x0A    Msg: \x16\x01\x0E%lu of %lu (%lu left)\n", Msgn, Current->LastMessage, Current->LastMessage - Msgn);
      sprintf (Temp, "%02d %3.3s %d %2d:%02d", Msg->Written.Day, Language->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
      Embedded->BufferedPrintf ("\x16\x01\x0A   From: \x16\x01\x0E%-35.35s \x16\x01\x0F%-16.16s \x16\x01\x07%s\n", Msg->From, Msg->FromAddress, Temp);
      sprintf (Temp, "%02d %3.3s %d %2d:%02d", Msg->Arrived.Day, Language->Months[Msg->Arrived.Month - 1], Msg->Arrived.Year, Msg->Arrived.Hour, Msg->Arrived.Minute);
      Embedded->BufferedPrintf ("\x16\x01\x0A     To: \x16\x01\x0E%-35.35s \x16\x01\x0F%-16.16s \x16\x01\x07%s\n", Msg->To, Msg->ToAddress, Temp);
      Embedded->BufferedPrintf ("\x16\x01\x0ASubject: \x16\x01\x0E%s\n", Msg->Subject);
      Embedded->BufferedPrintf ("\x16\x01\x09===============================================================================\n");
      Line = 6;

      if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
               if (strchr (Text, '>') != NULL)
                  Embedded->BufferedPrintf (Language->MessageQuote, Text);
               else if (!strncmp (Text, "SEEN-BY: ", 9) || Text[0] == 1)
                  Embedded->BufferedPrintf (Language->MessageKludge, Text);
               else if (!strncmp (Text, " * Origin", 9) || !strncmp (Text, "---", 3))
                  Embedded->BufferedPrintf (Language->MessageOrigin, Text);
               else
                  Embedded->BufferedPrintf (Language->MessageText, Text);

               MaxLine = Line;
               if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                  MaxLine++;
                  while (MaxLine > 6)
                     Embedded->BufferedPrintfAt (MaxLine--, 1, "\026\007");
                  Line = 6;
               }
            }
         } while ((Text = (CHAR *)Msg->Text.Next ()) != NULL && Embedded->AbortSession () == FALSE && Line != 0);

      Embedded->UnbufferBytes ();
   }
}

USHORT TInquire::FirstMessage (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], *p;

   if (Msg != NULL) {
      Msg->Lock (0L);
      do {
         if (Embedded->KBHit () == TRUE) {
            switch (Embedded->Getch ()) {
               case 0x03:
               case 'c':
               case 'C':
               case 's':
               case 'S':
                  Stop = TRUE;
                  break;
            }
         }
         if (Type == TYPE_KEYWORD) {
            if (Msg->Read (Number) == TRUE) {
               strcpy (Temp, Msg->From);
               if (strstr (strlwr (Temp), Keyword) != NULL)
                  RetVal = TRUE;
               if (RetVal == FALSE) {
                  strcpy (Temp, Msg->To);
                  if (strstr (strlwr (Temp), Keyword) != NULL)
                     RetVal = TRUE;
               }
               if (RetVal == FALSE) {
                  strcpy (Temp, Msg->Subject);
                  if (strstr (strlwr (Temp), Keyword) != NULL)
                     RetVal = TRUE;
               }
               if (RetVal == FALSE) {
                  if ((p = (CHAR *)Msg->Text.First ()) != NULL)
                     do {
                        strcpy (Temp, p);
                        if (strstr (strlwr (Temp), Keyword) != NULL) {
                           RetVal = TRUE;
                           break;
                        }
                     } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);
               }
               if (RetVal == TRUE)
                  break;
            }
         }
         else {
            if (Msg->ReadHeader (Number) == TRUE) {
               if (Type == TYPE_PERSONAL || Type == TYPE_PERSONALNEW) {
                  if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To)) {
                     if (Type == TYPE_PERSONAL || (Type == TYPE_PERSONALNEW && Msg->Received == FALSE)) {
                        RetVal = TRUE;
                        break;
                     }
                  }
               }
               else if (Type == TYPE_ALL) {
                  RetVal = TRUE;
                  break;
               }
            }
         }
      } while (Stop == FALSE && Msg->Next (Number) == TRUE && Embedded->AbortSession () == FALSE);
      Msg->UnLock ();
   }

   return (RetVal);
}

USHORT TInquire::NextMessage (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], *p;

   if (Msg != NULL) {
      Msg->Lock (0L);
      while (Stop == FALSE && Msg->Next (Number) == TRUE && Embedded->AbortSession () == FALSE) {
         if (Embedded->KBHit () == TRUE) {
            switch (Embedded->Getch ()) {
               case 0x03:
               case 'c':
               case 'C':
               case 's':
               case 'S':
                  Stop = TRUE;
                  break;
            }
         }
         if (Type == TYPE_KEYWORD) {
            if (Msg->Read (Number) == TRUE) {
               strcpy (Temp, Msg->From);
               if (strstr (strlwr (Temp), Keyword) != NULL)
                  RetVal = TRUE;
               if (RetVal == FALSE) {
                  strcpy (Temp, Msg->To);
                  if (strstr (strlwr (Temp), Keyword) != NULL)
                     RetVal = TRUE;
               }
               if (RetVal == FALSE) {
                  strcpy (Temp, Msg->Subject);
                  if (strstr (strlwr (Temp), Keyword) != NULL)
                     RetVal = TRUE;
               }
               if (RetVal == FALSE) {
                  if ((p = (CHAR *)Msg->Text.First ()) != NULL)
                     do {
                        strcpy (Temp, p);
                        if (strstr (strlwr (Temp), Keyword) != NULL) {
                           RetVal = TRUE;
                           break;
                        }
                     } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);
               }
               if (RetVal == TRUE)
                  break;
            }
         }
         else {
            if (Msg->ReadHeader (Number) == TRUE) {
               if (Type == TYPE_PERSONAL || Type == TYPE_PERSONALNEW) {
                  if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To)) {
                     if (Type == TYPE_PERSONAL || (Type == TYPE_PERSONALNEW && Msg->Received == FALSE)) {
                        RetVal = TRUE;
                        break;
                     }
                  }
               }
               else if (Type == TYPE_ALL) {
                  RetVal = TRUE;
                  break;
               }
            }
         }
      }
      Msg->UnLock ();
   }

   return (RetVal);
}

USHORT TInquire::PreviousMessage (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], *p;

   if (Msg != NULL) {
      Msg->Lock (0L);
      while (Stop == FALSE && Msg->Previous (Number) == TRUE && Embedded->AbortSession () == FALSE) {
         if (Embedded->KBHit () == TRUE) {
            switch (Embedded->Getch ()) {
               case 0x03:
               case 'c':
               case 'C':
               case 's':
               case 'S':
                  Stop = TRUE;
                  break;
            }
         }
         if (Type == TYPE_KEYWORD) {
            if (Msg->Read (Number) == TRUE) {
               strcpy (Temp, Msg->From);
               if (strstr (strlwr (Temp), Keyword) != NULL)
                  RetVal = TRUE;
               if (RetVal == FALSE) {
                  strcpy (Temp, Msg->To);
                  if (strstr (strlwr (Temp), Keyword) != NULL)
                     RetVal = TRUE;
               }
               if (RetVal == FALSE) {
                  strcpy (Temp, Msg->Subject);
                  if (strstr (strlwr (Temp), Keyword) != NULL)
                     RetVal = TRUE;
               }
               if (RetVal == FALSE) {
                  if ((p = (CHAR *)Msg->Text.First ()) != NULL)
                     do {
                        strcpy (Temp, p);
                        if (strstr (strlwr (Temp), Keyword) != NULL) {
                           RetVal = TRUE;
                           break;
                        }
                     } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);
               }
               if (RetVal == TRUE)
                  break;
            }
         }
         else {
            if (Msg->ReadHeader (Number) == TRUE) {
               if (Type == TYPE_PERSONAL || Type == TYPE_PERSONALNEW) {
                  if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To)) {
                     if (Type == TYPE_PERSONAL || (Type == TYPE_PERSONALNEW && Msg->Received == FALSE)) {
                        RetVal = TRUE;
                        break;
                     }
                  }
               }
               else if (Type == TYPE_ALL) {
                  RetVal = TRUE;
                  break;
               }
            }
         }
      }
      Msg->UnLock ();
   }

   return (RetVal);
}

USHORT TInquire::First (VOID)
{
   USHORT RetVal = FALSE, NewLine = TRUE;

   Msg = NULL;

   if (Range == RANGE_ALL) {
      if (Data->First () == TRUE)
         do {
            if (User->Level >= Data->Level) {
               if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                  if (NewLine == TRUE) {
                     Embedded->Printf ("\n\026\001\012Searching area %s ... ", Data->Key);
                     NewLine = FALSE;
                  }
                  else
                     Embedded->Printf ("\r\026\001\012Searching area %s ... ", Data->Key);
                  Current = Data;
                  if (Msg != NULL) {
                     Msg->Close ();
                     delete Msg;
                  }
                  Msg = NULL;
                  if (Current->Storage == ST_JAM)
                     Msg = new JAM (Current->Path);
                  else if (Current->Storage == ST_SQUISH)
                     Msg = new SQUISH (Current->Path);
                  else if (Current->Storage == ST_USENET)
                     Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                  else if (Current->Storage == ST_FIDO)
                     Msg = new FIDOSDM (Current->Path);
                  else if (Current->Storage == ST_ADEPT)
                     Msg = new ADEPT (Current->Path);

                  if (Msg != NULL) {
                     Number = Msg->Lowest ();

                     if (FirstMessage () == TRUE) {
                        RetVal = TRUE;
                        break;
                     }

                     Msg->Close ();
                     delete Msg;
                     Msg = NULL;
                  }
               }
            }
         } while (Stop == FALSE && Data->Next () == TRUE && Embedded->AbortSession () == FALSE);
   }
   else if (Range == RANGE_TAGGED) {
      if (User->MsgTag->First () == TRUE)
         do {
            if (User->MsgTag->Tagged == TRUE && Data->Read (User->MsgTag->Area) == TRUE) {
               if (User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     if (NewLine == TRUE) {
                        Embedded->Printf ("\n\026\001\012Searching area %s ... ", Data->Key);
                        NewLine = FALSE;
                     }
                     else
                        Embedded->Printf ("\r\026\001\012Searching area %s ... ", Data->Key);
                     Current = Data;
                     if (Msg != NULL) {
                        Msg->Close ();
                        delete Msg;
                     }
                     Msg = NULL;
                     if (Current->Storage == ST_JAM)
                        Msg = new JAM (Current->Path);
                     else if (Current->Storage == ST_SQUISH)
                        Msg = new SQUISH (Current->Path);
                     else if (Current->Storage == ST_USENET)
                        Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                     else if (Current->Storage == ST_FIDO)
                        Msg = new FIDOSDM (Current->Path);
                     else if (Current->Storage == ST_ADEPT)
                        Msg = new ADEPT (Current->Path);

                     if (Msg != NULL) {
                        Number = Msg->Lowest ();

                        if (FirstMessage () == TRUE) {
                           RetVal = TRUE;
                           break;
                        }

                        Msg->Close ();
                        delete Msg;
                        Msg = NULL;
                     }
                  }
               }
            }
         } while (Stop == FALSE && User->MsgTag->Next () == TRUE && Embedded->AbortSession () == FALSE);
   }
   else if (Range == RANGE_CURRENT) {
      if (Msg != NULL) {
         Msg->Close ();
         delete Msg;
      }
      Msg = NULL;
      if (Current->Storage == ST_JAM)
         Msg = new JAM (Current->Path);
      else if (Current->Storage == ST_SQUISH)
         Msg = new SQUISH (Current->Path);
      else if (Current->Storage == ST_USENET)
         Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
      else if (Current->Storage == ST_FIDO)
         Msg = new FIDOSDM (Current->Path);
      else if (Current->Storage == ST_ADEPT)
         Msg = new ADEPT (Current->Path);

      if (Msg != NULL) {
         Number = Msg->Lowest ();

         if (FirstMessage () == TRUE)
            RetVal = TRUE;
         else {
            Msg->Close ();
            delete Msg;
            Msg = NULL;
         }
      }
   }

   return (RetVal);
}

USHORT TInquire::Next (VOID)
{
   USHORT RetVal = FALSE;

   if (Action == ACTION_LIST)
      Embedded->Printf ("\r\026\001\012Searching area %s ... ", Data->Key);
   else
      Embedded->Printf ("\n\026\001\012Searching area %s ... ", Data->Key);
   if (Msg != NULL && NextMessage () == TRUE)
      RetVal = TRUE;

   if (RetVal == FALSE) {
      if (Range == RANGE_ALL) {
         if (Data->Next () == TRUE)
            do {
               if (User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     Embedded->Printf ("\r\026\001\012Searching area %s ... ", Data->Key);
                     Current = Data;
                     if (Msg != NULL) {
                        Msg->Close ();
                        delete Msg;
                     }
                     Msg = NULL;
                     if (Current->Storage == ST_JAM)
                        Msg = new JAM (Current->Path);
                     else if (Current->Storage == ST_SQUISH)
                        Msg = new SQUISH (Current->Path);
                     else if (Current->Storage == ST_USENET)
                        Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                     else if (Current->Storage == ST_FIDO)
                        Msg = new FIDOSDM (Current->Path);
                     else if (Current->Storage == ST_ADEPT)
                        Msg = new ADEPT (Current->Path);

                     if (Msg != NULL) {
                        Number = Msg->Lowest ();

                        if (FirstMessage () == TRUE) {
                           RetVal = TRUE;
                           break;
                        }

                        Msg->Close ();
                        delete Msg;
                        Msg = NULL;
                     }
                  }
               }
            } while (Stop == FALSE && Data->Next () == TRUE && Embedded->AbortSession () == FALSE);
      }
      else if (Range == RANGE_TAGGED) {
         if (User->MsgTag->Next () == TRUE)
            do {
               if (User->MsgTag->Tagged == TRUE && Data->Read (User->MsgTag->Area) == TRUE) {
                  if (User->Level >= Data->Level) {
                     if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                        Embedded->Printf ("\r\026\001\012Searching area %s ... ", Data->Key);
                        Current = Data;
                        if (Msg != NULL) {
                           Msg->Close ();
                           delete Msg;
                        }
                        Msg = NULL;
                        if (Current->Storage == ST_JAM)
                           Msg = new JAM (Current->Path);
                        else if (Current->Storage == ST_SQUISH)
                           Msg = new SQUISH (Current->Path);
                        else if (Current->Storage == ST_USENET)
                           Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                        else if (Current->Storage == ST_FIDO)
                           Msg = new FIDOSDM (Current->Path);
                        else if (Current->Storage == ST_ADEPT)
                           Msg = new ADEPT (Current->Path);

                        if (Msg != NULL) {
                           Number = Msg->Lowest ();

                           if (FirstMessage () == TRUE) {
                              RetVal = TRUE;
                              break;
                           }

                           Msg->Close ();
                           delete Msg;
                           Msg = NULL;
                        }
                     }
                  }
               }
            } while (Stop == FALSE && User->MsgTag->Next () == TRUE && Embedded->AbortSession () == FALSE);
      }
   }

   return (RetVal);
}

USHORT TInquire::Previous (VOID)
{
   USHORT RetVal = FALSE;

   Embedded->Printf ("\n\026\001\012Searching area %s ...\r", Data->Key);
   if (Msg != NULL && PreviousMessage () == TRUE)
      RetVal = TRUE;

   if (RetVal == FALSE) {
      if (Range == RANGE_ALL) {
         if (Data->Previous () == TRUE)
            do {
               if (User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     Embedded->Printf ("\r\026\001\012Searching area %s ... ", Data->Key);
                     Current = Data;
                     if (Msg != NULL) {
                        Msg->Close ();
                        delete Msg;
                     }
                     Msg = NULL;
                     if (Current->Storage == ST_JAM)
                        Msg = new JAM (Current->Path);
                     else if (Current->Storage == ST_SQUISH)
                        Msg = new SQUISH (Current->Path);
                     else if (Current->Storage == ST_USENET)
                        Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                     else if (Current->Storage == ST_FIDO)
                        Msg = new FIDOSDM (Current->Path);
                     else if (Current->Storage == ST_ADEPT)
                        Msg = new ADEPT (Current->Path);

                     if (Msg != NULL) {
                        Number = Msg->Highest () + 1L;

                        if (PreviousMessage () == TRUE) {
                           RetVal = TRUE;
                           break;
                        }

                        Msg->Close ();
                        delete Msg;
                        Msg = NULL;
                     }
                  }
               }
            } while (Stop == FALSE && Data->Previous () == TRUE && Embedded->AbortSession () == FALSE);
      }
      else if (Range == RANGE_TAGGED) {
         if (User->MsgTag->Previous () == TRUE)
            do {
               if (User->MsgTag->Tagged == TRUE && Data->Read (User->MsgTag->Area) == TRUE) {
                  if (User->Level >= Data->Level) {
                     if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                        Embedded->Printf ("\r\026\001\012Searching area %s ... ", Data->Key);
                        Current = Data;
                        if (Msg != NULL) {
                           Msg->Close ();
                           delete Msg;
                        }
                        Msg = NULL;
                        if (Current->Storage == ST_JAM)
                           Msg = new JAM (Current->Path);
                        else if (Current->Storage == ST_SQUISH)
                           Msg = new SQUISH (Current->Path);
                        else if (Current->Storage == ST_USENET)
                           Msg = new USENET (Cfg->NewsServer, Current->NewsGroup);
                        else if (Current->Storage == ST_FIDO)
                           Msg = new FIDOSDM (Current->Path);
                        else if (Current->Storage == ST_ADEPT)
                           Msg = new ADEPT (Current->Path);

                        if (Msg != NULL) {
                           Number = Msg->Highest ();

                           if (PreviousMessage () == TRUE) {
                              RetVal = TRUE;
                              break;
                           }

                           Msg->Close ();
                           delete Msg;
                           Msg = NULL;
                        }
                     }
                  }
               }
            } while (Stop == FALSE && User->MsgTag->Previous () == TRUE && Embedded->AbortSession () == FALSE);
      }
   }

   return (RetVal);
}

VOID TInquire::Query (VOID)
{
   USHORT End, DoRead;
   SHORT Line, Continue;
   CHAR Temp[16], LastKey[16], Cmd;
   class TMsgEditor *Editor;
   class TMailEditor *MailEditor;

   Number = 0L;

   Data = new TMsgData (Cfg->SystemPath);
   Msg = NULL;
   LastKey[0] = '\0';

   if (Range == RANGE_UNDEFINED) {
      if (SearchRange () == FALSE)
         return;
   }
   if (Action == ACTION_UNDEFINED) {
      if (SearchAction () == FALSE)
         return;
   }

   if (Type == TYPE_KEYWORD) {
      Embedded->Printf ("\n\x16\x01\013Enter the text to search, ? for help, or RETURN to exit.\n: ");
      Embedded->Input (Keyword, (USHORT)(sizeof (Keyword) - 1), 0);
      if (Keyword[0] == '\0' || Embedded->AbortSession () == TRUE)
         return;
      strlwr (Keyword);
   }

   if (First () == TRUE) {
      End = FALSE;
      DoRead = TRUE;

      if (Action == ACTION_READ) {
         Cmd = '\0';
         while (Embedded->AbortSession () == FALSE && Cmd != Language->ExitReadMessage) {
            if (End == FALSE && DoRead == TRUE) {
               if (Msg->Read (Number) == TRUE) {
                  DisplayCurrent ();
                  if (Msg->Received == FALSE) {
                     Msg->Received = TRUE;
                     Msg->WriteHeader (Number);
                  }
               }
               DoRead = FALSE;
            }

            if (End == FALSE)
               Embedded->Printf (Language->ReadMenu);
            else
               Embedded->Printf (Language->EndReadMenu);
            Embedded->GetString (Temp, 10, INP_HOTKEY|INP_NUMERIC);
            if ((Cmd = (CHAR)toupper (Temp[0])) == '\0')
               Cmd = Language->NextMessage;

            if (Cmd == Language->NextMessage) {
               if (Next () == FALSE) {
                  Embedded->Printf (Language->EndOfMessages);
                  End = TRUE;
               }
               else
                  DoRead = TRUE;
            }
            else if (Cmd == Language->PreviousMessage) {
               if (Previous () == FALSE) {
                  Embedded->Printf (Language->EndOfMessages);
                  End = TRUE;
               }
               else
                  DoRead = TRUE;
            }
            else if (Cmd == Language->ReplyMessage) {
               if (User->Level >= Current->WriteLevel) {
                  if ((User->AccessFlags & Current->WriteFlags) == Current->WriteFlags) {
                     if ((Editor = new TMsgEditor) != NULL) {
                        Editor->Cfg = Cfg;
                        Editor->Embedded = Embedded;
                        Editor->Log = Log;
                        Editor->Msg = Msg;
                        Editor->Lang = Language;
                        Editor->ScreenWidth = User->ScreenWidth;
                        strcpy (Editor->UserName, User->Name);
                        if (Current->EchoMail == TRUE) {
                           Editor->EchoMail = TRUE;
                           strcpy (Editor->Origin, Cfg->SystemName);
                           if (Cfg->MailAddress.First () == TRUE)
                              strcpy (Editor->Address, Cfg->MailAddress.String);
                        }
                        if (Editor->Reply () == TRUE)
                           Editor->Menu ();
                        delete Editor;

                        Current->ActiveMsgs = Msg->Number ();
                        Current->FirstMessage = Msg->Lowest ();
                        Current->LastMessage = Msg->Highest ();
                        Current->Update ();
                        Msg->Read (Number);
                     }
                  }
                  else
                     Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
               }
               else
                  Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
            }
            else if (Cmd == Language->EMailReplyMessage && End == FALSE) {
               if ((MailEditor = new TMailEditor) != NULL) {
                  Editor->Cfg = Cfg;
                  MailEditor->Embedded = Embedded;
                  MailEditor->Log = Log;
                  MailEditor->Msg = Msg;
                  MailEditor->Lang = Language;
                  MailEditor->ScreenWidth = User->ScreenWidth;
                  strcpy (MailEditor->UserName, User->Name);
                  if (Cfg->MailAddress.First () == TRUE)
                     strcpy (MailEditor->Address, Cfg->MailAddress.String);
                  if (MailEditor->Reply () == TRUE)
                     MailEditor->Menu ();
               }
            }
         }
      }
      else if (Action == ACTION_LIST) {
         Line = 2;
         Embedded->Printf ("\x0C\026\001\012    #  From                    To                     Subject\n");
         Embedded->Printf ("-----  ----------------------  ---------------------  -----------------------\n");
         Continue = TRUE;

         while (Embedded->AbortSession () == FALSE && Line != 0 && Continue == TRUE) {
            if (Msg->ReadHeader (Number) == TRUE) {
               Embedded->Printf ("\r");
               if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                  Embedded->Printf ("\x0C    #  From                    To                     Subject\n");
                  Embedded->Printf ("-----  ----------------------  ---------------------  -----------------------\n");
                  Line = 2;
               }
               if (strcmp (LastKey, Current->Key)) {
                  Embedded->Printf ("Area %s - %s\n", Current->Key, Current->Display);
                  strcpy (LastKey, Current->Key);
                  Line++;
               }
               Embedded->Printf ("\026\001\016%5d  \026\001\012%-22.22s  %-21.21s  %-.23s\n", Msg->Current, Msg->From, Msg->To, Msg->Subject);
            }
            if (Line != 0)
               Continue = Next ();
         }

         if (Line != 2) {
            Embedded->Printf ("\r");
            Embedded->PressEnter ();
         }
      }
   }
   else
      Embedded->Printf (Language->EndOfMessages);

   if (Msg != NULL) {
      Msg->Close ();
      delete Msg;
   }
   if (Data != NULL)
      delete Data;
}

USHORT TInquire::SearchRange (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[4];

   while (Embedded->AbortSession () == FALSE) {
      Embedded->Printf ("\n\x16\x01\012Which Conferences would you like included in the list of search?\n\n");
      Embedded->Printf ("  \x16\x01\013C \x16\x01\016... Current Conference (%s)\n", Current->Key);
      Embedded->Printf ("  \x16\x01\013T \x16\x01\016... Tagged Conferences\n");
      Embedded->Printf ("  \x16\x01\013A \x16\x01\016... All Conferences\n\n");
      Embedded->Printf ("\x16\x01\013Select an option, X to exit, or RETURN for the current Conference: ");
      Embedded->Input (Temp, 1, INP_HOTKEY);
      Temp[0] = (CHAR)toupper (Temp[0]);
      if (Temp[0] == 'A' || Temp[0] == 'T' || Temp[0] == 'C' || Temp[0] == 'X' || Temp[0] == '\0')
         break;
   }

   switch (Temp[0]) {
      case 'C':
      case '\0':
         Range = RANGE_CURRENT;
         RetVal = TRUE;
         break;
      case 'A':
         Range = RANGE_ALL;
         RetVal = TRUE;
         break;
      case 'T':
         Range = RANGE_TAGGED;
         RetVal = TRUE;
         break;
   }

   return (RetVal);
}

USHORT TInquire::SearchAction (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[4];

   while (Embedded->AbortSession () == FALSE) {
      Embedded->Printf ("\n\x16\x01\012How do you like to display the messages?\n\n");
      Embedded->Printf ("  \x16\x01\013R \x16\x01\016... Read (show entire message)\n");
      Embedded->Printf ("  \x16\x01\013L \x16\x01\016... List (one per line)\n\n");
      Embedded->Printf ("\x16\x01\013Select an option, X to exit, or RETURN for read: ");
      Embedded->Input (Temp, 1, INP_HOTKEY);
      Temp[0] = (CHAR)toupper (Temp[0]);
      if (Temp[0] == 'R' || Temp[0] == 'L' || Temp[0] == 'X' || Temp[0] == '\0')
         break;
   }

   switch (Temp[0]) {
      case 'R':
      case '\0':
         Action = ACTION_READ;
         RetVal = TRUE;
         break;
      case 'L':
         Action = ACTION_LIST;
         RetVal = TRUE;
         break;
   }

   return (RetVal);
}


