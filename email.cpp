
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 2.99.20
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora.h"

class TMailList : public TListings
{
public:
   USHORT Forward, ShowKludges, ToYou, Unread;
   ULONG  Number;
   class  TConfig *Cfg;
   class  TMsgBase *Msg;

   VOID   Begin (VOID);
   USHORT DrawScreen (VOID);
   VOID   PrintCursor (USHORT y);
   VOID   PrintLine (VOID);
   VOID   PrintTitles (VOID);
   VOID   RemoveCursor (USHORT y);
   VOID   Select (VOID);
};

VOID TMailList::Begin (VOID)
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
            if (Unread == TRUE) {
               if (Msg->Received == FALSE && !stricmp (Msg->To, User->Name)) {
                  sprintf (ld.Key, "%lu", Number);
                  sprintf (ld.Display, "%-26.26s  %-42.42s", Msg->From, Msg->Subject);
                  Data.Add (&ld, sizeof (LISTDATA));
               }
            }
            else if ((ToYou == TRUE && !stricmp (Msg->To, User->Name)) || (ToYou == FALSE && !stricmp (Msg->From, User->Name))) {
               sprintf (ld.Key, "%lu", Number);
               if (ToYou == TRUE)
                  sprintf (ld.Display, "%-26.26s  %-42.42s", Msg->From, Msg->Subject);
               else
                  sprintf (ld.Display, "%-26.26s  %-42.42s", Msg->To, Msg->Subject);
               Data.Add (&ld, sizeof (LISTDATA));
            }
            if (Forward == TRUE)
               Continue = Msg->Next (Number);
            else
               Continue = Msg->Previous (Number);
         }
      }
   }

   Found = TRUE;
   if ((pld = (LISTDATA *)Data.First ()) != NULL) {
      do {
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

USHORT TMailList::DrawScreen (VOID)
{
   USHORT i;

   i = 0;
   do {
      pld = (LISTDATA *)Data.Value ();
      PrintLine ();
      i++;
   } while (Data.Next () != NULL && i < (User->ScreenHeight - 6));

   return (i);
}

VOID TMailList::PrintTitles (VOID)
{
   Embedded->Printf ("\x0C\x16\x01\x0B%s \x16\x01\x0A- \x16\x01\x0B%s\026\007\n", "E-Mail", "Electronic Mail");
   if (ToYou == TRUE)
      Embedded->Printf ("\026\001\012    #  From                        Subject\n");
   else
      Embedded->Printf ("\026\001\012    #  To                          Subject\n");
   Embedded->Printf ("=====  ==========================  ==========================================\n");

   Embedded->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, "=====  ==========================  ==========================================\n");

   Embedded->Printf ("\026\001\012Hit \026\001\013CTRL-V \026\001\012for next page, \026\001\013CTRL-Y \026\001\012for previous page, \026\001\013? \026\001\012for help, or \026\001\013X \026\001\012to exit.\n");
   Embedded->Printf ("\026\001\012To highlight a message, use your \026\001\013arrow keys\026\001\012, \026\001\013RETURN \026\001\012reads it.");

   Embedded->PrintfAt (4, 1, "");
}

VOID TMailList::PrintLine (VOID)
{
   Embedded->Printf ("\026\001\013%-5.5s  \026\001\016%s\n", pld->Key, pld->Display);
}

VOID TMailList::PrintCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x70%-5.5s\x16\x01\x0E", (PSZ)List.Value ());
}

VOID TMailList::RemoveCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x0B%-5.5s\x16\x01\x0E", (PSZ)List.Value ());
}

VOID TMailList::Select (VOID)
{
   USHORT Line, MaxLine, gotFrom = FALSE, gotTo = FALSE;
   CHAR *Text, Temp[96], *p, Display[32];
   ULONG Msgn;

   if (Msg != NULL) {
      Msg->Read (atoi ((PSZ)List.Value ()));
      Msgn = Msg->Current;

      if ((p = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (!strncmp (p, "\001From: ", 7)) {
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
      strcpy (Display, "Electronic Mail");
      Temp[79 - strlen (Display) - 3] = '\0';
      Embedded->BufferedPrintf ("\x0C\x16\x01\x09= \x16\x01\x0E%s \x16\x01\x09%s\n", Display, Temp);

      Embedded->BufferedPrintf ("\x16\x01\x0A    Msg: \x16\x01\x0E%lu\n", Msgn);
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

TEMail::TEMail (void)
{
   Log = NULL;
   Language = NULL;
   Embedded = NULL;
   Msg = NULL;

   ShowKludges = FALSE;
   Width = 79;
   Height = 24;
   DoCls = More = TRUE;

   Storage = ST_SQUISH;
   strcpy (BasePath, "email");
}

TEMail::~TEMail (void)
{
   if (Msg != NULL)
      delete Msg;
}

VOID TEMail::BriefList (VOID)
{
   USHORT Found, Continue, Line;
   CHAR Which, Start, Temp[40];
   ULONG First, Last, Number;
   class TMailList *List;

   Msg = NULL;
   if (Storage == ST_JAM)
      Msg = new JAM (BasePath);
   else if (Storage == ST_SQUISH)
      Msg = new SQUISH (BasePath);
   else if (Storage == ST_FIDO)
      Msg = new FIDOSDM (BasePath);
   else if (Storage == ST_ADEPT)
      Msg = new ADEPT (BasePath);
   else
      Log->Write ("!Invalid e-mail storage type");

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      if (Msg->Number () != 0L) {
         do {
            Embedded->Printf ("\n\026\001\013Read messages (F)rom you or (T)o you (? for help)? \026\001\007");
            Embedded->Input (Temp, 10, INP_HOTKEY|INP_NUMERIC);
            if ((Which = (CHAR)toupper (Temp[0])) == '\0')
               Which = Language->NextMessage;
         } while (Which != 'T' && Which != 'F' && Which != '\0' && Embedded->AbortSession () == FALSE);

         if ((Which == 'T' || Which == 'F') && Embedded->AbortSession () == FALSE) {
            Found = FALSE;
            Number = Msg->Lowest ();
            do {
               if (Msg->ReadHeader (Number) == TRUE) {
                  if (Which == 'T' && (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName))) {
                     Found = TRUE;
                     break;
                  }
                  else if (Which == 'F' && (!stricmp (Msg->From, User->Name) || !stricmp (Msg->From, User->RealName))) {
                     Found = TRUE;
                     break;
                  }
               }
            } while (Msg->Next (Number) == TRUE);

            if (Found == TRUE) {
               Number = 0L;

               do {
                  Embedded->Printf (Language->StartWithMessage);
                  Embedded->Printf (Language->NewMessages);
                  Embedded->Printf (": ");
                  Embedded->Input (Temp, 10, INP_HOTKEY|INP_NUMERIC);
                  if ((Start = (CHAR)toupper (Temp[0])) == Language->Help)
                     Embedded->DisplayFile ("MAILSTRT");
               } while (Embedded->AbortSession () == FALSE && Start != 'F' && Start != 'L' && Start != '\0' && !isdigit (Start));

               if (isdigit (Start)) {
                  Start = 'F';
                  Number = atol (Temp);
               }
               else if (Start == 'F' || Start == '\0')
                  Number = First - 1L;
               else if (Start == 'L')
                  Number = Last + 1L;

               if (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE) {
                  if ((List = new TMailList) != NULL) {
                     List->Cfg = Cfg;
                     List->Embedded = Embedded;
                     List->Log = Log;
                     List->User = User;
                     List->Language = Language;
                     List->Msg = Msg;
                     List->Forward = TRUE;
                     if (Start == 'L')
                        List->Forward = FALSE;
                     List->ToYou = TRUE;
                     List->Unread = (USHORT)(Start == '\0' ? TRUE : FALSE);
                     if (Which == 'F')
                        List->ToYou = FALSE;
                     List->Number = Number;
                     List->ShowKludges = FALSE;
                     List->Run ();
                     delete List;
                  }
               }
               else {
                  Line = 2;
                  Embedded->Printf ("\x0C\026\001\012    #  From                    To                     Subject\n");
                  Embedded->Printf ("-----  ----------------------  ---------------------  -----------------------\n");

                  if (Start == 'F' || Start == '\0')
                     Continue = Msg->Next (Number);
                  else
                     Continue = Msg->Previous (Number);

                  while (Embedded->AbortSession () == FALSE && Line != 0 && Continue == TRUE) {
                     if (Msg->ReadHeader (Number) == TRUE) {
                        if ((Which == 'T' && !stricmp (Msg->To, User->Name)) || (Which == 'F' && !stricmp (Msg->From, User->Name))) {
                           if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                              Embedded->Printf ("\x0C    #  From                    To                     Subject\n");
                              Embedded->Printf ("-----  ----------------------  ---------------------  -----------------------\n");
                              Line = 2;
                           }
                           Embedded->Printf ("\026\001\016%5d  \026\001\012%-22.22s  %-21.21s  %-.23s\n", Msg->Current, Msg->From, Msg->To, Msg->Subject);
                        }
                     }
                     if (Start == 'F' || Start == '\0')
                        Continue = Msg->Next (Number);
                     else
                        Continue = Msg->Previous (Number);
                  }

                  Embedded->Printf ("\n\026\001\016End of list! \001\001");
               }
            }
            else {
               if (Which == 'T')
                  Embedded->Printf ("\n\026\001\015Sorry, there are no messages in your mailbox.\n\006\007\006\007");
               else
                  Embedded->Printf ("\n\026\001\015Sorry, there are no messages from you in the database.\n\006\007\006\007");
            }
         }
      }

      delete Msg;
      Msg = NULL;
   }
}

VOID TEMail::CheckUnread (VOID)
{
   USHORT i, Unread = 0;
   CHAR *Rotating = "-\\|/";
   ULONG Number;

   Msg = NULL;
   if (Storage == ST_JAM)
      Msg = new JAM (BasePath);
   else if (Storage == ST_SQUISH)
      Msg = new SQUISH (BasePath);
   else if (Storage == ST_FIDO)
      Msg = new FIDOSDM (BasePath);
   else if (Storage == ST_ADEPT)
      Msg = new ADEPT (BasePath);
   else
      Log->Write ("!Invalid e-mail storage type");

   Embedded->Printf ("\n\026\001\012Checking your mailbox ...  ");
   if (Msg != NULL) {
      Msg->Lock (0L);
      Number = Msg->Lowest ();
      i = 0;

      do {
         if (Number > 0L && (Number % 10L) == 0L) {
            Embedded->Printf ("\x08%c", Rotating[i++]);
            if (Rotating[i] == '\0')
               i = 0;
         }
         if (Msg->ReadHeader (Number) == TRUE) {
            if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
               if (Msg->Received == FALSE)
                  Unread++;
            }
         }
      } while (Msg->Next (Number) == TRUE);

      Embedded->Printf (" \010\n");

      Msg->UnLock ();
      delete Msg;
      Msg = NULL;
   }

   if (Unread > 0)
      Embedded->Printf ("\n\026\001\015You have %u unread e-mail message(s)\n\006\007\006\007", Unread);
   else
      Embedded->Printf ("\n\026\001\015You have no new mail in your mailbox\n\006\007\006\007");
}

VOID TEMail::Delete (VOID)
{
   CHAR szTemp[20];
   ULONG ulMsg, ulNumber, First;

   Msg = NULL;
   if (Storage == ST_JAM)
      Msg = new JAM (BasePath);
   else if (Storage == ST_SQUISH)
      Msg = new SQUISH (BasePath);
   else if (Storage == ST_FIDO)
      Msg = new FIDOSDM (BasePath);
   else if (Storage == ST_ADEPT)
      Msg = new ADEPT (BasePath);
   else
      Log->Write ("!Invalid e-mail storage type");

   if (Msg != NULL) {
      ulNumber = Msg->Highest ();
      First = Msg->Lowest ();

      do {
         Embedded->Printf ("\n\x16\x01\013Enter the message number to be erased (%lu-%lu): ", First, ulNumber);
         Embedded->Input (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
         ulMsg = atol (szTemp);
         if (ulMsg < First || ulMsg > ulNumber)
            Embedded->Printf ("\n\x16\x01\x0DThe message #%ld is out of range.\n", ulMsg);
      } while (ulMsg > ulNumber && ulMsg != 0 && Embedded->AbortSession () == FALSE);

      if (ulMsg > 0) {
         if (Msg->ReadHeader (ulMsg) == TRUE) {
            if (!stricmp (Msg->From, User->Name) || !stricmp (Msg->To, User->Name)) {
               if (Msg->Delete (ulMsg) == TRUE)
                  Embedded->Printf ("\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%ld ERASED >>>\n\006\007\006\007", ulMsg);
            }
            else
               Embedded->Printf ("\n\x16\x01\x0DSorry, message number %ld is neither FROM you nor a PRIVATE message TO you, so you can't erase it.\n\006\007\006\007", ulMsg);
         }
         else
            Embedded->Printf ("\n\x16\x01\x0DSorry, message number %ld is neither FROM you nor a PRIVATE message TO you, so you can't erase it.\n\006\007\006\007", ulMsg);
      }

      delete Msg;
      Msg = NULL;
   }
}

/*
   "\026\001\013Do you want to send a copy of this message to anyone (Y/N)? "

   "\026\001\013Who do you whish to send a copy f this message (cc:) to?\nYou may also type ? for help: "
*/

VOID TEMail::DisplayCurrent (VOID)
{
   USHORT Line, MaxLine, gotFrom = FALSE, gotTo = FALSE;
   CHAR *Text, Temp[96], *p, *a, Display[32];
   ULONG Msgn;

   if (Msg != NULL) {
      Msgn = Msg->Current;

      if ((p = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (!strncmp (p, "\001From: ", 7)) {
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
      strcpy (Display, "Electronic Mail");
      Temp[79 - strlen (Display) - 3] = '\0';
      Embedded->BufferedPrintf ("\x0C\x16\x01\x09= \x16\x01\x0E%s \x16\x01\x09%s\n", Display, Temp);

      Embedded->BufferedPrintf ("\x16\x01\x0A    Msg: \x16\x01\x0E%lu\n", Msgn);
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

VOID TEMail::ReadMessages (VOID)
{
   USHORT Found;
   CHAR Cmd, Which, Start, End, DoRead, Temp[40];
   ULONG First, Last, Number;
   class TMailEditor *Editor;

   Msg = NULL;
   if (Storage == ST_JAM)
      Msg = new JAM (BasePath);
   else if (Storage == ST_SQUISH)
      Msg = new SQUISH (BasePath);
   else if (Storage == ST_FIDO)
      Msg = new FIDOSDM (BasePath);
   else if (Storage == ST_ADEPT)
      Msg = new ADEPT (BasePath);
   else
      Log->Write ("!Invalid e-mail storage type");

   if (Msg != NULL) {
      First = Msg->Lowest ();
      Last = Msg->Highest ();

      if (Msg->Number () != 0L) {
         do {
            Embedded->Printf ("\n\026\001\013Read messages (F)rom you or (T)o you (? for help)? \026\001\007");
            Embedded->Input (Temp, 10, INP_HOTKEY|INP_NUMERIC);
            if ((Which = (CHAR)toupper (Temp[0])) == '\0')
               Which = Language->NextMessage;
         } while (Which != 'T' && Which != 'F' && Which != '\0' && Embedded->AbortSession () == FALSE);

         if ((Which == 'T' || Which == 'F') && Embedded->AbortSession () == FALSE) {
            Found = FALSE;
            Number = Msg->Lowest ();
            do {
               if (Msg->ReadHeader (Number) == TRUE) {
                  if (Which == 'T' && (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName))) {
                     Found = TRUE;
                     break;
                  }
                  else if (Which == 'F' && (!stricmp (Msg->From, User->Name) || !stricmp (Msg->From, User->RealName))) {
                     Found = TRUE;
                     break;
                  }
               }
            } while (Msg->Next (Number) == TRUE);

            if (Found == TRUE) {
               Number = 0L;

               do {
                  Embedded->Printf (Language->StartWithMessage);
                  Embedded->Printf (Language->NewMessages);
                  Embedded->Printf (": ");
                  Embedded->Input (Temp, 10, INP_HOTKEY|INP_NUMERIC);
                  if ((Start = (CHAR)toupper (Temp[0])) == Language->Help)
                     Embedded->DisplayFile ("MAILSTRT");
               } while (Embedded->AbortSession () == FALSE && Start != 'F' && Start != 'L' && Start != '\0' && !isdigit (Start));

               if (isdigit (Start)) {
                  Start = 'F';
                  Number = atol (Temp);
               }
               else if (Start == 'F' || Start == '\0')
                  Number = First - 1L;
               else if (Start == 'L')
                  Number = Last + 1L;

               do {
                  End = TRUE;
                  DoRead = FALSE;
                  if (Start == 'F' || Start == '\0') {
                     if (Msg->Next (Number) == TRUE) {
                        Msg->ReadHeader (Number);
                        if (Which == 'T') {
                           if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To)) {
                              if (Start == 'F' || (Start == '\0' && Msg->Received == FALSE))
                                 DoRead = TRUE;
                           }
                        }
                        else {
                           if (!stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From))
                              DoRead = TRUE;
                        }
                        End = FALSE;
                     }
                  }
                  else {
                     if (Msg->Previous (Number) == TRUE) {
                        Msg->ReadHeader (Number);
                        if (Which == 'T') {
                           if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To))
                              DoRead = TRUE;
                        }
                        else {
                           if (!stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From))
                              DoRead = TRUE;
                        }
                        End = FALSE;
                     }
                  }
               } while (DoRead == FALSE && End == FALSE);

               Cmd = '\0';
               if (End == TRUE) {
                  Embedded->Printf (Language->EndOfMessages);
                  Cmd = Language->ExitReadMessage;
               }

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
                  Embedded->Input (Temp, 10, INP_HOTKEY|INP_NUMERIC);
                  if ((Cmd = (CHAR)toupper (Temp[0])) == '\0')
                     Cmd = Language->NextMessage;

                  if (isdigit (Cmd)) {
                     if (Msg->ReadHeader (atol (Temp)) == TRUE) {
                        Number = atol (Temp);
                        if (Which == 'T') {
                           if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To))
                              DoRead = TRUE;
                        }
                        else {
                           if (!stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From))
                              DoRead = TRUE;
                        }
                        End = FALSE;
                     }
                  }
                  else if (Cmd == Language->NextMessage) {
                     if (End == FALSE) {
                        do {
                           End = TRUE;
                           DoRead = FALSE;
                           if (Start == 'F' || Start == '\0') {
                              if (Msg->Next (Number) == TRUE) {
                                 Msg->ReadHeader (Number);
                                 if (Which == 'T') {
                                    if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To)) {
                                       if (Start == 'F' || (Start == '\0' && Msg->Received == FALSE))
                                          DoRead = TRUE;
                                    }
                                 }
                                 else {
                                    if (!stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From))
                                       DoRead = TRUE;
                                 }
                                 End = FALSE;
                              }
                           }
                           else {
                              if (Msg->Previous (Number) == TRUE) {
                                 Msg->ReadHeader (Number);
                                 if (Which == 'T') {
                                    if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To))
                                       DoRead = TRUE;
                                 }
                                 else {
                                    if (!stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From))
                                       DoRead = TRUE;
                                 }
                                 End = FALSE;
                              }
                           }
                        } while (DoRead == FALSE && End == FALSE);
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
                        DoRead = FALSE;
                        if (Start == 'F' || Start == '\0') {
                           if (Msg->Next (Number) == TRUE) {
                              Msg->ReadHeader (Number);
                              if (Which == 'T') {
                                 if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To))
                                    DoRead = TRUE;
                              }
                              else {
                                 if (!stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From))
                                    DoRead = TRUE;
                              }
                              End = FALSE;
                           }
                        }
                        else {
                           if (Msg->Previous (Number) == TRUE) {
                              Msg->ReadHeader (Number);
                              if (Which == 'T') {
                                 if (!stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To))
                                    DoRead = TRUE;
                              }
                              else {
                                 if (!stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From))
                                    DoRead = TRUE;
                              }
                              End = FALSE;
                           }
                        }
                     } while (DoRead == FALSE && End == FALSE);

                     if (End == TRUE)
                        Embedded->Printf (Language->EndOfMessages);
                  }
                  else if ((Cmd == Language->ReplyMessage || Cmd == Language->EMailReplyMessage) && End == FALSE) {
                     if ((Editor = new TMailEditor) != NULL) {
                        Editor->Cfg = Cfg;
                        Editor->Embedded = Embedded;
                        Editor->Log = Log;
                        Editor->Msg = Msg;
                        Editor->Lang = Language;
                        Editor->ScreenWidth = User->ScreenWidth;
                        strcpy (Editor->UserName, User->Name);
                        if (Cfg->MailAddress.First () == TRUE)
                           strcpy (Editor->Address, Cfg->MailAddress.String);
                        if (Cmd == Language->ReplyMessage) {
                           if (Editor->Reply () == TRUE)
                              Editor->Menu ();
                        }
                        else {
                           if (Editor->Write () == TRUE)
                              Editor->Menu ();
                        }
                        Msg->Read (Number);
                     }
                     Last = Msg->Highest ();
                  }
               }
            }
            else {
               if (Which == 'T')
                  Embedded->Printf ("\n\026\001\015Sorry, there are no messages in your mailbox.\n\006\007\006\007");
               else
                  Embedded->Printf ("\n\026\001\015Sorry, there are no messages from you in the database.\n\006\007\006\007");
            }
         }
      }

      delete Msg;
      Msg = NULL;
   }
}

VOID TEMail::StartMessageQuestion (ULONG ulFirst, ULONG ulLast, ULONG &ulMsg, USHORT &fForward)
{
   CHAR Cmd, Temp[20];

   do {
      Embedded->Printf (Language->StartWithMessage);
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

