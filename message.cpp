
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
#include "lora.h"

class TMsgAreaListing : public TListings
{
public:
   TMsgAreaListing (void);

   CHAR   Area[16];
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

   CHAR   Area[16];

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
   USHORT ShowKludges;
   CHAR   LastRead[16];
   ULONG  Number;
   class  TConfig *Cfg;
   class  TMsgBase *Msg;
   class  TMsgData *Current;
   class  TMessage *Parent;

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
   USHORT i, Total = 0;
   ULONG Msgn;
   LISTDATA ld;

   i = 0;
   y = 4;
   Found = FALSE;
   List.Clear ();
   Data.Clear ();

   if (Msg  != NULL) {
      Msg->Lock (0L);

      do {
         if (Msg->ReadHeader (Number) == TRUE) {
            if (Msg->Private == FALSE || !stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To) || !stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From)) {
               Msgn = Msg->UidToMsgn (Number);
               sprintf (ld.Key, "%5lu", Msgn);
               if (!stricmp (Msg->From, User->Name))
                  sprintf (ld.Display, "\026\001\007%5lu  \026\001\014%-20.20s  \026\001\012%-20.20s  \026\001\013%-.28s", Msgn, Msg->From, Msg->To, Msg->Subject);
               else if (!stricmp (Msg->To, User->Name))
                  sprintf (ld.Display, "\026\001\007%5lu  \026\001\016%-20.20s  \026\001\014%-20.20s  \026\001\013%-.28s", Msgn, Msg->From, Msg->To, Msg->Subject);
               else
                  sprintf (ld.Display, "\026\001\007%5lu  \026\001\016%-20.20s  \026\001\012%-20.20s  \026\001\013%-.28s", Msgn, Msg->From, Msg->To, Msg->Subject);
               Data.Add (&ld, sizeof (LISTDATA));
            }
         }
         Total++;
#if defined(__OS2__)
         if ((Total % 16) == 0L)
            DosSleep (1L);
#elif defined(__NT__)
         if ((Total % 16) == 0L)
            Sleep (1L);
#endif
      } while (Embedded->AbortSession () == FALSE && Msg->Next (Number) == TRUE);

      Msg->UnLock ();
   }

   if ((pld = (LISTDATA *)Data.First ()) != NULL) {
      do {
         List.Add (pld->Key);
         i++;
         if (i >= (User->ScreenHeight - 6))
            break;
      } while ((pld = (LISTDATA *)Data.Next ()) != NULL);
   }

/*
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
            List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
            i++;
            if (i >= (User->ScreenHeight - 6))
               break;
         } while ((pld = (LISTDATA *)Data.Next ()) != NULL);
      }
   }
*/
}

USHORT TMessageList::DrawScreen (VOID)
{
   USHORT i;

   i = 0;
   do {
      List.Next ();
      pld = (LISTDATA *)Data.Value ();
//      if (Found == TRUE && !strcmp (pld->Key, LastRead)) {
//         y = (USHORT)(i + 4);
//         Found = FALSE;
//      }
      PrintLine ();
      i++;
   } while (Data.Next () != NULL && i < (User->ScreenHeight - 6));

   return (i);
}

VOID TMessageList::PrintTitles (VOID)
{
   Embedded->Printf ("\x0C\x16\x01\x0B%s \x16\x01\x0A- \x16\x01\x0B%s\026\007\n", Current->Key, Current->Display);
   Embedded->Printf ("\026\001\017    #  From                  To                    Subject\n\031Ä\005  \031Ä\024  \031Ä\024  \031Ä\034\n");

   Embedded->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, "\026\001\017\031Ä\005  \031Ä\024  \031Ä\024  \031Ä\034\n");

   Embedded->Printf ("\026\001\016Use your arrow keys or CTRL-X / CTRL-E to hilight an area, RETURN selects it.\n");
   Embedded->Printf ("\026\001\016Hit CTRL-V for next page, CTRL-Y for previous page, or X to exit.");

   Embedded->PrintfAt (4, 1, "");
}

VOID TMessageList::PrintLine (VOID)
{
   Embedded->Printf ("%s\n", pld->Display);
}

VOID TMessageList::PrintCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x70%-5.5s\x16\x01\x07", (PSZ)List.Value ());
}

VOID TMessageList::RemoveCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, "\x16\x01\x07%-5.5s\x16\x01\x07", (PSZ)List.Value ());
}

VOID TMessageList::Select (VOID)
{
   USHORT Line, MaxLine, gotFrom = FALSE, gotTo = FALSE;
   CHAR *Text, Temp[96], *p, Flags[96];
   ULONG Msgn;

   if (Msg != NULL) {
      Msgn = atol ((PSZ)List.Value ());
      Msg->Read (Msg->MsgnToUid (Msgn));

      if (Log != NULL)
         Log->Write (":Display Msg. #%lu, area %s (%lu)", Msgn, Current->Key, Msg->Current);

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

      Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEHDR), Current->Display, (CHAR)(80 - strlen (Current->Display) - 3));
      if (Msg->Original != 0L && Msg->Reply == 0L)
         sprintf (Temp, Language->Text (LNG_MESSAGENUMBER1), Msgn, Msg->Number (), Msg->UidToMsgn (Msg->Original));
      else if (Msg->Original == 0L && Msg->Reply != 0L)
         sprintf (Temp, Language->Text (LNG_MESSAGENUMBER2), Msgn, Msg->Number (), Msg->UidToMsgn (Msg->Reply));
      else if (Msg->Original != 0L && Msg->Reply != 0L)
         sprintf (Temp, Language->Text (LNG_MESSAGENUMBER3), Msgn, Msg->Number (), Msg->UidToMsgn (Msg->Original), Msg->UidToMsgn (Msg->Reply));
      else
         sprintf (Temp, Language->Text (LNG_MESSAGENUMBER), Msgn, Msg->Number ());
      Flags[0] = '\0';
      if (Msg->Received == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_RCV));
      if (Msg->Sent == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_SNT));
      if (Msg->Private == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_PVT));
      if (Msg->Crash == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_CRA));
      if (Msg->KillSent == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_KS));
      if (Msg->Local == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_LOC));
      if (Msg->Hold == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_HLD));
      if (Msg->FileAttach == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_ATT));
      if (Msg->FileRequest == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_FRQ));
      if (Msg->Intransit == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_TRS));
      Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEFLAGS), Temp, Flags);

      Parent->BuildDate (Language->Text (LNG_MESSAGEDATE), Temp, &Msg->Written);
      Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEFROM), Msg->From, Msg->FromAddress, Temp);
      Parent->BuildDate (Language->Text (LNG_MESSAGEDATE), Temp, &Msg->Arrived);
      Embedded->BufferedPrintf (Language->Text (LNG_MESSAGETO), Msg->To, Msg->ToAddress, Temp);
      if (Msg->FileAttach == TRUE || Msg->FileRequest == TRUE)
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEFILE), Msg->Subject);
      else
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGESUBJECT), Msg->Subject);
      Embedded->BufferedPrintf ("\x16\x01\x13\031Ä\120");

      Line = 6;

      if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
               if (!strncmp (Text, "SEEN-BY: ", 9) || Text[0] == 1) {
                  if (Text[0] == 1)
                     Text++;
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEKLUDGE), Text);
               }
               else if (!strncmp (Text, " * Origin", 9) || !strncmp (Text, "---", 3))
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEORIGIN), Text);
               else if (strchr (Text, '>') != NULL)
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEQUOTE), Text);
               else
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGETEXT), Text);

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

      if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
         Msg->Received = TRUE;
         Msg->WriteHeader (Msg->Current);
      }

      if (Line > 6)
         Embedded->MoreQuestion (99);

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
   CHAR Temp[16];
   SHORT Line, Continue;
   ULONG Number;
   class TMessageList *List;

   if (Msg != NULL) {
      Number = 0L;
      if (User != NULL) {
         if (User->MsgTag->Read (Current->Key) == TRUE)
            Number = User->MsgTag->LastRead;
      }

      if (Msg->Number () != 0L) {
         Embedded->Printf ("\n\026\001\017Start displaying at which msg# (\"=\" for current): ");
         Embedded->Input (Temp, (USHORT)(sizeof (Temp) - 1), 0);

         if (Temp[0] != '\0') {
            if (Temp[0] != '=')
               Number = Msg->MsgnToUid (atol (Temp));

            if ((Embedded->Ansi == TRUE || Embedded->Avatar == TRUE) && User->FullScreen == TRUE) {
               if ((List = new TMessageList) != NULL) {
                  List->Cfg = Cfg;
                  List->Embedded = Embedded;
                  List->Log = Log;
                  List->User = User;
                  List->Language = Language;
                  List->Current = Current;
                  List->Msg = Msg;
                  List->Number = Number;
                  List->ShowKludges = ShowKludges;
                  List->Parent = this;
                  sprintf (List->LastRead, "%5lu", atol (Temp));
                  List->Run ();
                  delete List;
               }
            }
            else {
               Line = 1;
               Embedded->Printf ("\x0C");
               Embedded->Printf ("\026\001\017    #  From                    To                     Subject\n\031Ä\005  \031Ä\026  \031Ä\025  \031Ä\027\n");

               Continue = Msg->Next (Number);
               while (Embedded->AbortSession () == FALSE && Line != 0 && Continue == TRUE) {
                  if (Msg->ReadHeader (Number) == TRUE) {
                     if (Msg->Private == FALSE || !stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To) || !stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From)) {
                        if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                           Embedded->Printf ("\x0C");
                           Embedded->Printf ("\026\001\017    #  From                    To                     Subject\n\031Ä\005  \031Ä\026  \031Ä\025  \031Ä\027\n");
                           Line = 2;
                        }
                        Embedded->Printf ("\026\001\007%5lu  \026\001\016%-22.22s  \026\001\012%-21.21s  \026\001\013%-.23s\n", Msg->UidToMsgn (Msg->Current), Msg->From, Msg->To, Msg->Subject);
                     }
                  }
                  Continue = Msg->Next (Number);
               }

               if (Line > 2)
                  Embedded->PressEnter ();
            }
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
         Current->ActiveMsgs = Msg->Number ();
      }
   }
}

VOID TMessage::BuildDate (PSZ format, PSZ dest, MDATE *date)
{
   CHAR Temp[16];

   while (*format != '\0') {
      if (*format == '%') {
         format++;
         switch (*format) {
            case 'A':
               if (date->Hour >= 12)
                  strcpy (dest, "pm");
               else
                  strcpy (dest, "am");
               dest += 2;
               format++;
               break;
            case 'B':
               sprintf (Temp, "%2d", date->Month);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'C':
               sprintf (Temp, "%-3.3s", Language->Months[date->Month - 1]);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'D':
               sprintf (Temp, "%2d", date->Day);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'E':
               if (date->Hour > 12)
                  sprintf (Temp, "%2d", date->Hour - 12);
               else
                  sprintf (Temp, "%2d", date->Hour);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'H':
               sprintf (Temp, "%2d", date->Hour);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'M':
               sprintf (Temp, "%02d", date->Minute);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'S':
               sprintf (Temp, "%02d", date->Second);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'Y':
               sprintf (Temp, "%2d", date->Year % 100);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'Z':
               sprintf (Temp, "%4d", date->Year);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            default:
               *dest++ = *format++;
               break;
         }
      }
      else
         *dest++ = *format++;
   }
   *dest = '\0';
}

VOID TMessage::DisplayCurrent (VOID)
{
   USHORT InitialLine;
   USHORT Line, MaxLine, gotFrom = FALSE, gotTo = FALSE;
   CHAR *Text, Temp[96], *p, Flags[96];
   ULONG Msgn;

   if (Msg != NULL) {
      if (Msg->Current == 0L)
         Msg->Read (Msg->MsgnToUid (Current->LastReaded));
      Msgn = Msg->UidToMsgn (Msg->Current);
      Current->LastReaded = Msgn;
      if (Log != NULL && Pause == FALSE)
         Log->Write (":Display Msg. #%lu, area %s (%lu)", Msgn, Current->Key, Msg->Current);

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

      Flags[0] = '\0';
      if (Msg->Received == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_RCV));
      if (Msg->Sent == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_SNT));
      if (Msg->Private == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_PVT));
      if (Msg->Crash == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_CRA));
      if (Msg->KillSent == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_KS));
      if (Msg->Local == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_LOC));
      if (Msg->Hold == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_HLD));
      if (Msg->FileAttach == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_ATT));
      if (Msg->FileRequest == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_FRQ));
      if (Msg->Intransit == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_TRS));

      if (User->FullReader == TRUE && (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE)) {
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEHDR), Current->Display, (CHAR)(80 - strlen (Current->Display) - 3));
         if (Msg->Original != 0L && Msg->Reply == 0L)
            sprintf (Temp, Language->Text (LNG_MESSAGENUMBER1), Msgn, Msg->Number (), Msg->UidToMsgn (Msg->Original));
         else if (Msg->Original == 0L && Msg->Reply != 0L)
            sprintf (Temp, Language->Text (LNG_MESSAGENUMBER2), Msgn, Msg->Number (), Msg->UidToMsgn (Msg->Reply));
         else if (Msg->Original != 0L && Msg->Reply != 0L)
            sprintf (Temp, Language->Text (LNG_MESSAGENUMBER3), Msgn, Msg->Number (), Msg->UidToMsgn (Msg->Original), Msg->UidToMsgn (Msg->Reply));
         else
            sprintf (Temp, Language->Text (LNG_MESSAGENUMBER), Msgn, Msg->Number ());
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEFLAGS), Temp, Flags);

         BuildDate (Language->Text (LNG_MESSAGEDATE), Temp, &Msg->Written);
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEFROM), Msg->From, Msg->FromAddress, Temp);
         BuildDate (Language->Text (LNG_MESSAGEDATE), Temp, &Msg->Arrived);
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGETO), Msg->To, Msg->ToAddress, Temp);
         if (Msg->FileAttach == TRUE || Msg->FileRequest == TRUE)
            Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEFILE), Msg->Subject);
         else
            Embedded->BufferedPrintf (Language->Text (LNG_MESSAGESUBJECT), Msg->Subject);
         Embedded->BufferedPrintf ("\x16\x01\x13\031Ä\120");

         InitialLine = Line = 6;
      }
      else {
         Embedded->BufferedPrintf ("\x0C");
         Embedded->BufferedPrintf ("\026\001\003From:    \026\001\016%-36.36s \026\001\017%-.33s\n", Msg->From, Flags);

         BuildDate (Language->Text (LNG_MESSAGEDATE), Temp, &Msg->Written);
         Embedded->BufferedPrintf ("\026\001\003To:      \026\001\016%-36.36s \026\001\012Msg #%lu, %-.23s\n", Msg->To, Msgn, Temp);

         if (Msg->FileAttach == TRUE || Msg->FileRequest == TRUE)
            Embedded->BufferedPrintf ("\026\001\003File(s): \026\001\016%-.70s\n", Msg->Subject);
         else
            Embedded->BufferedPrintf ("\026\001\003Subject: \026\001\016%-.70s\n\n", Msg->Subject);

         InitialLine = Line = 4;
      }

      if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
               if (!strncmp (Text, "SEEN-BY: ", 9) || Text[0] == 1) {
                  if (Text[0] == 1)
                     Text++;
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEKLUDGE), Text);
               }
               else if (!strncmp (Text, " * Origin", 9) || !strncmp (Text, "---", 3))
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEORIGIN), Text);
               else if (strchr (Text, '>') != NULL)
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEQUOTE), Text);
               else
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGETEXT), Text);

               MaxLine = Line;
               if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                  MaxLine++;
                  while (MaxLine > InitialLine)
                     Embedded->BufferedPrintfAt (MaxLine--, 1, "\026\007");
                  Line = InitialLine;
               }
            }
         } while ((Text = (CHAR *)Msg->Text.Next ()) != NULL && Embedded->AbortSession () == FALSE && Line != 0);

      if (User->FullReader == FALSE || (Embedded->Ansi == FALSE && Embedded->Avatar == FALSE)) {
         if (Msg->Original != 0L && Msg->Reply == 0L)
            Embedded->BufferedPrintf ("\n\026\001\017*** This is a reply to #%lu.\n", Msg->UidToMsgn (Msg->Original));
         else if (Msg->Original == 0L && Msg->Reply != 0L)
            Embedded->BufferedPrintf ("\n\026\001\017*** See also #%lu.\n", Msg->UidToMsgn (Msg->Reply));
         else if (Msg->Original != 0L && Msg->Reply != 0L)
            Embedded->BufferedPrintf ("\n\026\001\017*** This is a reply to #%lu.  *** See also #%lu.\n", Msg->UidToMsgn (Msg->Original), Msg->UidToMsgn (Msg->Reply));
         Line += 2;
      }

      Embedded->UnbufferBytes ();

      if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
         Msg->Received = TRUE;
         Msg->WriteHeader (Msg->Current);
      }

      if (Line > InitialLine) {
         MaxLine = Line;
         Embedded->MoreQuestion (99);
         MaxLine++;
         while (MaxLine > InitialLine)
            Embedded->BufferedPrintfAt (MaxLine--, 1, "\026\007");
      }
   }
}

VOID TMessage::DisplayText (VOID)
{
   USHORT gotFrom = FALSE, gotTo = FALSE;
   CHAR *Text, Temp[96], Flags[96];
   ULONG Msgn;

   if (Msg != NULL) {
      Msgn = Msg->UidToMsgn (Msg->Current);
      Current->LastReaded = Msgn;

      Flags[0] = '\0';
      if (Msg->Received == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_RCV));
      if (Msg->Sent == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_SNT));
      if (Msg->Private == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_PVT));
      if (Msg->Crash == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_CRA));
      if (Msg->KillSent == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_KS));
      if (Msg->Local == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_LOC));
      if (Msg->Hold == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_HLD));
      if (Msg->FileAttach == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_ATT));
      if (Msg->FileRequest == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_FRQ));
      if (Msg->Intransit == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_TRS));
      Embedded->BufferedPrintf ("\n");
      Embedded->BufferedPrintf ("\026\001\003From:    \026\001\016%-36.36s \026\001\017%-.33s\n", Msg->From, Flags);

      BuildDate (Language->Text (LNG_MESSAGEDATE), Temp, &Msg->Written);
      Embedded->BufferedPrintf ("\026\001\003To:      \026\001\016%-36.36s \026\001\012Msg #%lu, %-.23s\n", Msg->To, Msgn, Temp);

      if (Msg->FileAttach == TRUE || Msg->FileRequest == TRUE)
         Embedded->BufferedPrintf ("\026\001\003File(s): \026\001\016%-.70s\n", Msg->Subject);
      else
         Embedded->BufferedPrintf ("\026\001\003Subject: \026\001\016%-.70s\n\n", Msg->Subject);

      if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
               if (!strncmp (Text, "SEEN-BY: ", 9) || Text[0] == 1) {
                  if (Text[0] == 1)
                     Text++;
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEKLUDGE), Text);
               }
               else if (!strncmp (Text, " * Origin", 9) || !strncmp (Text, "---", 3))
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEORIGIN), Text);
               else if (strchr (Text, '>') != NULL)
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEQUOTE), Text);
               else
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGETEXT), Text);
            }
         } while ((Text = (CHAR *)Msg->Text.Next ()) != NULL && Embedded->AbortSession () == FALSE);

      Embedded->UnbufferBytes ();
   }
}

VOID TMessage::GetOrigin (class TMsgData *Data, PSZ Origin)
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

VOID TMessage::Reply (VOID)
{
   ULONG Number;
   class TMsgEditor *Editor;

   Number = Msg->Current;

   if (User->Level >= Current->WriteLevel) {
      if ((User->AccessFlags & Current->WriteFlags) == Current->WriteFlags) {
         if (Log != NULL)
            Log->Write (":Reply to Msg. #%lu, area %s", Msg->UidToMsgn (Number), Current->Key);

         if ((Editor = new TMsgEditor) != NULL) {
            Editor->Cfg = Cfg;
            Editor->Embedded = Embedded;
            Editor->Log = Log;
            Editor->Msg = Msg;
            Editor->Language = Language;
            Editor->Width = User->ScreenWidth;
            Editor->Height = User->ScreenHeight;
            strcpy (Editor->UserName, User->Name);
            strcpy (Editor->AreaKey, Current->Key);
            strcpy (Editor->AreaTitle, Current->Display);
            if (Current->EchoMail == TRUE) {
               Editor->EchoMail = TRUE;
               GetOrigin (Current, Editor->Origin);
               if (Cfg->MailAddress.First () == TRUE)
                  strcpy (Editor->Address, Cfg->MailAddress.String);
               if (Current->Address[0] != '\0')
                  strcpy (Editor->Address, Current->Address);
            }
            if (User->FullEd == TRUE && (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE))
               Editor->UseFullScreen = TRUE;
            if (Editor->Reply () == TRUE)
               Editor->Menu ();
            delete Editor;
         }

         Msg->Read (Number);
         Current->ActiveMsgs = Msg->Number ();
         Current->FirstMessage = Msg->Lowest ();
         Current->LastMessage = Msg->Highest ();
         Current->LastReaded = Msg->UidToMsgn (Number);
         Current->Update ();
      }
      else {
         Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
         if (Log != NULL)
            Log->Write ("!User can't write messages (flags)");
      }
   }
   else {
      Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
      if (Log != NULL)
         Log->Write ("!User can't write messages (level)");
   }
}

VOID TMessage::Write (VOID)
{
   class TMsgEditor *Editor;

   if (User->Level >= Current->WriteLevel) {
      if ((User->AccessFlags & Current->WriteFlags) == Current->WriteFlags) {
         if (Log != NULL)
            Log->Write (":Writing Msg. in area %s", Current->Key);

         if ((Editor = new TMsgEditor) != NULL) {
            Editor->Cfg = Cfg;
            Editor->Embedded = Embedded;
            Editor->Log = Log;
            Editor->Msg = Msg;
            Editor->Language = Language;
            Editor->Width = User->ScreenWidth;
            Editor->Height = User->ScreenHeight;
            strcpy (Editor->UserName, User->Name);
            strcpy (Editor->AreaKey, Current->Key);
            strcpy (Editor->AreaTitle, Current->Display);
            if (Current->EchoMail == TRUE) {
               Editor->EchoMail = TRUE;
               GetOrigin (Current, Editor->Origin);
               if (Cfg->MailAddress.First () == TRUE)
                  strcpy (Editor->Address, Cfg->MailAddress.String);
               if (Current->Address[0] != '\0')
                  strcpy (Editor->Address, Current->Address);
            }
            if (User->FullEd == TRUE && (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE))
               Editor->UseFullScreen = TRUE;
            if (Editor->Write () == TRUE)
               Editor->Menu ();
            delete Editor;
         }

         Current->ActiveMsgs = Msg->Number ();
         Current->FirstMessage = Msg->Lowest ();
         Current->LastMessage = Msg->Highest ();
         Current->Update ();
      }
      else {
         Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
         if (Log != NULL)
            Log->Write ("!User can't write messages (flags)");
      }
   }
   else {
      Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
      if (Log != NULL)
         Log->Write ("!User can't write messages (level)");
   }
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
            User->MsgTag->Add ();
         }
      }
   }
}

VOID TMessage::ReadReply (VOID)
{
   if (Msg != NULL) {
      if (Msg->Current == 0L || Msg->Reply == 0L)
         Embedded->Printf ("\n\026\001\017That message isn't available.\n");
      else
         Read (Msg->Reply);
   }
}

VOID TMessage::ReadOriginal (VOID)
{
   if (Msg != NULL) {
      if (Msg->Current == 0L || Msg->Original == 0L)
         Embedded->Printf ("\n\026\001\017That message isn't available.\n");
      else
         Read (Msg->Original);
   }
}

VOID TMessage::ReadMessages (VOID)
{
   USHORT Forward;
   CHAR Cmd, NewMessages, End, DoRead, Temp[40];
   ULONG First, Last, Number, LastReaded;
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
         LastReaded = Number;

         if (Embedded->AbortSession () == FALSE) {
            DoRead = FALSE;
            do {
               End = TRUE;
               if (Forward == TRUE) {
                  if (Msg->Next (Number) == TRUE) {
                     End = FALSE;
                     Msg->ReadHeader (Number);
                     if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                        continue;
                     DoRead = TRUE;
                  }
               }
               else {
                  if (Msg->Previous (Number) == TRUE) {
                     End = FALSE;
                     Msg->ReadHeader (Number);
                     if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                        continue;
                     DoRead = TRUE;
                  }
               }
            } while (End == FALSE && DoRead == FALSE);

            if (End == TRUE) {
               Embedded->Printf (Language->Text(LNG_ENDOFMESSAGES));
               Cmd = Language->Text(LNG_EXITREADMESSAGE)[0];
            }
         }

         while (Embedded->AbortSession () == FALSE && Cmd != Language->Text(LNG_EXITREADMESSAGE)[0]) {
            if (End == FALSE && DoRead == TRUE) {
               LastReaded = Number;
               if (Msg->Read (Number) == TRUE)
                  DisplayCurrent ();
               DoRead = FALSE;
            }

            if (End == FALSE)
               Embedded->Printf (Language->Text(LNG_READMENU));
            else
               Embedded->Printf (Language->Text(LNG_ENDREADMENU));
            Embedded->GetString (Temp, 10, INP_HOTKEY|INP_NUMERIC);
            if ((Cmd = (CHAR)toupper (Temp[0])) == '\0')
               Cmd = Language->Text(LNG_NEXTMESSAGE)[0];

            if (isdigit (Cmd)) {
               if (Msg->ReadHeader (atol (Temp)) == TRUE) {
                  Number = atol (Temp);
                  DoRead = TRUE;
                  End = FALSE;
               }
            }
            else if (Cmd == Language->Text(LNG_NEXTMESSAGE)[0]) {
               do {
                  End = TRUE;
                  if (Forward == TRUE) {
                     if (Msg->Next (Number) == TRUE) {
                        End = FALSE;
                        Msg->ReadHeader (Number);
                        if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                           continue;
                        DoRead = TRUE;
                     }
                  }
                  else {
                     if (Msg->Previous (Number) == TRUE) {
                        End = FALSE;
                        Msg->ReadHeader (Number);
                        if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                           continue;
                        DoRead = TRUE;
                     }
                  }
               } while (End == FALSE && DoRead == FALSE);

               if (End == TRUE) {
                  Embedded->Printf (Language->Text(LNG_ENDOFMESSAGES));
                  Number = LastReaded;
               }
            }
            else if (Cmd == Language->Text(LNG_REREADMESSAGE)[0] && End == TRUE) {
               DoRead = TRUE;
               End = FALSE;
            }
            else if (Cmd == Language->Text(LNG_PREVIOUSMESSAGE)[0]) {
               do {
                  End = TRUE;
                  if (Forward == TRUE) {
                     if (Msg->Previous (Number) == TRUE) {
                        End = FALSE;
                        Msg->ReadHeader (Number);
                        if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                           continue;
                        DoRead = TRUE;
                     }
                  }
                  else {
                     if (Msg->Next (Number) == TRUE) {
                        End = FALSE;
                        Msg->ReadHeader (Number);
                        if (Msg->Private == TRUE && stricmp (User->Name, Msg->To) && stricmp (User->RealName, Msg->To) && stricmp (User->Name, Msg->From) && stricmp (User->RealName, Msg->From))
                           continue;
                        DoRead = TRUE;
                     }
                  }
               } while (End == FALSE && DoRead == FALSE);

               if (End == TRUE) {
                  Embedded->Printf (Language->Text(LNG_ENDOFMESSAGES));
                  Number = LastReaded;
               }
            }
            else if (Cmd == Language->Text(LNG_REPLYMESSAGE)[0]) {
               if (User->Level >= Current->WriteLevel) {
                  if ((User->AccessFlags & Current->WriteFlags) == Current->WriteFlags) {
                     Reply ();
                     Msg->Read (Number);
                     Current->ActiveMsgs = Msg->Number ();
                     Current->FirstMessage = Msg->Lowest ();
                     Current->LastMessage = Msg->Highest ();
                     Current->LastReaded = Msg->UidToMsgn (Number);
                     Current->Update ();
                     Last = Msg->Highest ();
                  }
                  else
                     Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
               }
               else
                  Embedded->Printf ("\026\001\015Sorry, you can't write messages in this Conference.\n\006\007\006\007");
            }
            else if (Cmd == Language->Text(LNG_EMAILREPLYMESSAGE)[0] && End == FALSE) {
               if ((MailEditor = new TMailEditor) != NULL) {
                  MailEditor->Cfg = Cfg;
                  MailEditor->Embedded = Embedded;
                  MailEditor->Log = Log;
                  MailEditor->Msg = Msg;
                  MailEditor->Language = Language;
                  MailEditor->Width = User->ScreenWidth;
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
            User->MsgTag->Add ();
         }
      }
   }
}

VOID TMessage::ReadNext (VOID)
{
   USHORT DoRead = FALSE;
   ULONG Number = 0L;

   if (User != NULL) {
      if (User->MsgTag->Read (Current->Key) == TRUE)
         Number = User->MsgTag->LastRead;
   }

   if (Msg != NULL) {
      if (Msg->Next (Number) == TRUE)
         do {
            Msg->ReadHeader (Number);
            if (Msg->Private == FALSE || !stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To) || !stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From)) {
               DoRead = TRUE;
               break;
            }
         } while (Msg->Next (Number) == TRUE);

      if (DoRead == TRUE) {
         Msg->Read (Number);
         DisplayCurrent ();

         if (User != NULL) {
            if (User->MsgTag->Read (Current->Key) == TRUE) {
               User->MsgTag->LastRead = Msg->Current;
               User->MsgTag->Update ();
            }
            else {
               User->MsgTag->New ();
               strcpy (User->MsgTag->Area, Current->Key);
               User->MsgTag->Tagged = FALSE;
               User->MsgTag->LastRead = Msg->Current;
               User->MsgTag->Add ();
            }
         }
      }
      else
         Embedded->Printf (Language->Text(LNG_ENDOFMESSAGES));
   }
}

VOID TMessage::ReadPrevious (VOID)
{
   USHORT DoRead = FALSE;
   ULONG Number = 0L;

   if (User != NULL) {
      if (User->MsgTag->Read (Current->Key) == TRUE)
         Number = User->MsgTag->LastRead;
   }

   if (Msg != NULL) {
      if (Msg->Previous (Number) == TRUE)
         do {
            Msg->ReadHeader (Number);
            if (Msg->Private == FALSE || !stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To) || !stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From)) {
               DoRead = TRUE;
               break;
            }
         } while (Msg->Previous (Number) == TRUE);

      if (DoRead == TRUE) {
         Msg->Read (Number);
         DisplayCurrent ();

         if (User != NULL) {
            if (User->MsgTag->Read (Current->Key) == TRUE) {
               User->MsgTag->LastRead = Msg->Current;
               User->MsgTag->Update ();
            }
            else {
               User->MsgTag->New ();
               strcpy (User->MsgTag->Area, Current->Key);
               User->MsgTag->Tagged = FALSE;
               User->MsgTag->LastRead = Msg->Current;
               User->MsgTag->Add ();
            }
         }
      }
      else
         Embedded->Printf (Language->Text(LNG_ENDOFMESSAGES));
   }
}

VOID TMessage::OpenArea (PSZ area)
{
   USHORT DoOpen = TRUE;

   if (Current->Read (area) == FALSE) {
      DoOpen = FALSE;
      if (Current->First () == TRUE)
         do {
            if (User->Level >= Current->Level) {
               if ((Current->AccessFlags & User->AccessFlags) == Current->AccessFlags) {
                  DoOpen = TRUE;
                  break;
               }
            }
         } while (Current->Next () == TRUE);
   }

   if (DoOpen == TRUE) {
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
      else if (Current->Storage == ST_HUDSON)
         Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

      if (Log != NULL)
         Log->Write (":Message Area: %s - %s", Current->Key, Current->Display);
      if (Msg != NULL) {
         Current->LastReaded = 0L;
         if (User->MsgTag->Read (Current->Key) == TRUE)
            Current->LastReaded = Msg->UidToMsgn (User->MsgTag->LastRead);
         Current->ActiveMsgs = Msg->Number ();
         Current->FirstMessage = Msg->Lowest ();
         Current->LastMessage = Msg->Highest ();
      }
      Current->Update ();

      if (User != NULL) {
         strcpy (User->LastMsgArea, Current->Key);
         User->Update ();
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
            Embedded->Printf (Language->Text(LNG_MESSAGEAREAREQUEST));
            Embedded->Input (Command, (USHORT)(sizeof (Command) - 1), INP_FIELD);
         }
         else
            DoList = FALSE;

         if (toupper (Command[0]) == Language->Help) {
            if (User->FullScreen == TRUE && (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE)) {
               if ((List = new TMsgAreaListing) != NULL) {
                  List->Cfg = Cfg;
                  List->Embedded = Embedded;
                  List->Log = Log;
                  List->Current = Current;
                  List->User = User;
                  List->Language = Language;
                  if ((RetVal = List->Run ()) == TRUE)
                     OpenArea (List->Area);
                  delete List;
               }
            }
            else {
               if (Data->First () == TRUE) {
                  Embedded->Printf ("\x0C");
                  Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                  Line = 3;
                  do {
                     if (User->Level >= Data->Level) {
                        if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                           Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
                           if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                              Embedded->Printf ("\x0C");
                              Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                              Line = 3;
                           }
                        }
                     }
                  } while (Line != 0 && Embedded->AbortSession () == FALSE && Data->Next () == TRUE);
               }
            }
         }
         else if (!stricmp (Command, "[")) {
            Data->Read (Current->Key, FALSE);
            if (Data->Previous () == TRUE)
               do {
                  if (User->Level >= Data->Level) {
                     if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                        OpenArea (Data->Key);
                        RetVal = TRUE;
                        break;
                     }
                  }
               } while (Data->Previous () == TRUE);
         }
         else if (!stricmp (Command, "]")) {
            Data->Read (Current->Key, FALSE);
            if (Data->Next () == TRUE)
               do {
                  if (User->Level >= Data->Level) {
                     if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                        OpenArea (Data->Key);
                        RetVal = TRUE;
                        break;
                     }
                  }
               } while (Data->Next () == TRUE);
         }
         else if (Command[0] != '\0') {
            if (Data->Read (Command) == TRUE) {
               if (User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     OpenArea (Data->Key);
                     RetVal = TRUE;
                  }
               }
            }

            if (RetVal == FALSE) {
               if (User->FullScreen == TRUE && (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE)) {
                  if ((List = new TMsgAreaListing) != NULL) {
                     List->Cfg = Cfg;
                     List->Embedded = Embedded;
                     List->Log = Log;
                     List->Current = Current;
                     List->User = User;
                     List->Language = Language;
                     strcpy (List->Command, Command);
                     if ((RetVal = List->Run ()) == TRUE)
                        OpenArea (Current->Key);
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
                                    Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                                 Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
                                 Line = Embedded->MoreQuestion (Line);
                                 FirstHit = FALSE;
                              }
                              else {
                                 strcpy (Temp, Data->Display);
                                 if (strstr (strupr (Temp), Command) != NULL) {
                                    if (FirstHit == TRUE)
                                       Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                                    Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
                                    Line = Embedded->MoreQuestion (Line);
                                    FirstHit = FALSE;
                                 }
                                 else {
                                    strcpy (Temp, Data->EchoTag);
                                    if (strstr (strupr (Temp), Command) != NULL) {
                                       if (FirstHit == TRUE)
                                          Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                                       Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
                                       Line = Embedded->MoreQuestion (Line);
                                       FirstHit = FALSE;
                                    }
                                    else {
                                       strcpy (Temp, Data->NewsGroup);
                                       if (strstr (strupr (Temp), Command) != NULL) {
                                          if (FirstHit == TRUE)
                                             Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                                          Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
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
                     Embedded->Printf (Language->Text(LNG_CONFERENCENOTAVAILABLE));
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
            Embedded->Printf (Language->Text(LNG_MESSAGEAREAREQUEST));
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
                     else if (Current->Storage == ST_HUDSON)
                        Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

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
                  Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                  Line = 3;
                  do {
                     if (User->Level >= Data->Level) {
                        if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                           if (User->MsgTag->Read (Data->Key) == TRUE) {
                              if (User->MsgTag->LastRead < Data->LastMessage) {
                                 Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
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
                     else if (Current->Storage == ST_HUDSON)
                        Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

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
                                       Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                                    Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
                                    Line = Embedded->MoreQuestion (Line);
                                    FirstHit = FALSE;
                                 }
                                 else {
                                    strcpy (Temp, Data->Display);
                                    if (strstr (strupr (Temp), Command) != NULL) {
                                       if (FirstHit == TRUE)
                                          Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                                       Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
                                       Line = Embedded->MoreQuestion (Line);
                                       FirstHit = FALSE;
                                    }
                                    else {
                                       strcpy (Temp, Data->EchoTag);
                                       if (strstr (strupr (Temp), Command) != NULL) {
                                          if (FirstHit == TRUE)
                                             Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                                          Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
                                          Line = Embedded->MoreQuestion (Line);
                                          FirstHit = FALSE;
                                       }
                                       else {
                                          strcpy (Temp, Data->NewsGroup);
                                          if (strstr (strupr (Temp), Command) != NULL) {
                                             if (FirstHit == TRUE)
                                                Embedded->Printf (Language->Text(LNG_MESSAGEAREAHEADER));
                                             Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), Data->Key, Data->ActiveMsgs, Data->Display);
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
                  Embedded->Printf (Language->Text(LNG_CONFERENCENOTAVAILABLE));
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
      Embedded->Printf (Language->Text(LNG_STARTWITHMESSAGE));
      if (fNewMessages == TRUE)
         Embedded->Printf (Language->Text(LNG_NEWMESSAGES));
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

VOID TMessage::ReadNonStop (VOID)
{
   USHORT Forward, Continue;
   ULONG Number;

   if (Msg != NULL) {
      Number = 0L;
      if (User != NULL) {
         if (User->MsgTag->Read (Current->Key) == TRUE)
            Number = User->MsgTag->LastRead;
      }

      Forward = TRUE;
      Embedded->BufferedPrintf ("\x0C");

      if (Msg->Number () > 0L && Number < Msg->Highest ()) {
         if (Forward == TRUE)
            Continue = Msg->Next (Number);
         else
            Continue = Msg->Previous (Number);

         while (Embedded->AbortSession () == FALSE && Continue == TRUE) {
            if (Msg->Read (Number) == TRUE) {
               if (Msg->Private == FALSE || !stricmp (User->Name, Msg->To) || !stricmp (User->RealName, Msg->To) || !stricmp (User->Name, Msg->From) || !stricmp (User->RealName, Msg->From)) {
                  DisplayText ();
                  if (User != NULL) {
                     if (User->MsgTag->Read (Current->Key) == TRUE) {
                        User->MsgTag->LastRead = Msg->Current;
                        User->MsgTag->Update ();
                     }
                     else {
                        User->MsgTag->New ();
                        strcpy (User->MsgTag->Area, Current->Key);
                        User->MsgTag->Tagged = FALSE;
                        User->MsgTag->LastRead = Msg->Current;
                        User->MsgTag->Add ();
                     }
                  }
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

VOID TMessage::Unreceive (VOID)
{
   if (Msg != NULL) {
      if (Msg->Current == 0L)
         Msg->Read (Msg->MsgnToUid (Current->LastReaded));
      if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
         Msg->Received = FALSE;
         Msg->WriteHeader (Msg->Current);
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

   if (Found == FALSE) {
      List.Clear ();
      if ((pld = (LISTDATA *)Data.First ()) != NULL) {
         do {
            List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
            i++;
            if (i >= (User->ScreenHeight - 6))
               break;
         } while ((pld = (LISTDATA *)Data.Next ()) != NULL);
      }
   }
}

USHORT TMsgAreaListing::DrawScreen (VOID)
{
   USHORT i;

   i = 0;
   do {
      List.Next ();
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
   Embedded->Printf ("\x0C");
   Embedded->Printf (Language->Text (LNG_MESSAGEAREAHEADER));

   Embedded->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, Language->Text (LNG_MESSAGEAREASEPARATOR));

   Embedded->Printf (Language->Text (LNG_MESSAGEAREADESCRIPTION1));
   Embedded->Printf (Language->Text (LNG_MESSAGEAREADESCRIPTION2));

   Embedded->PrintfAt (4, 1, "");
}

VOID TMsgAreaListing::PrintLine (VOID)
{
   Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), pld->Key, pld->ActiveMsgs, pld->Display);
}

VOID TMsgAreaListing::PrintCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, Language->Text (LNG_MESSAGEAREACURSOR), (PSZ)List.Value ());
}

VOID TMsgAreaListing::RemoveCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, Language->Text (LNG_MESSAGEAREAKEY), (PSZ)List.Value ());
}

VOID TMsgAreaListing::Select (VOID)
{
   strcpy (Area, (PSZ)List.Value ());
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
   Embedded->Printf (Language->Text(LNG_MESSAGEAREALIST), pld->Key, pld->ActiveMsgs, pld->Display);
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
   strcpy (Area, (PSZ)List.Value ());
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
   USHORT InitialLine;
   USHORT Line, MaxLine, gotFrom = FALSE, gotTo = FALSE;
   CHAR *Text, Temp[96], *p, Flags[96];
   ULONG Msgn;

   if (Msg != NULL) {
      if (Msg->Current == 0L)
         Msg->Read (Msg->MsgnToUid (Current->LastReaded));
      Msgn = Msg->UidToMsgn (Msg->Current);
      Current->LastReaded = Msgn;
      if (Log != NULL && Pause == FALSE)
         Log->Write (":Display Msg. #%lu, area %s (%lu)", Msgn, Current->Key, Msg->Current);

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

      Flags[0] = '\0';
      if (Msg->Received == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_RCV));
      if (Msg->Sent == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_SNT));
      if (Msg->Private == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_PVT));
      if (Msg->Crash == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_CRA));
      if (Msg->KillSent == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_KS));
      if (Msg->Local == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_LOC));
      if (Msg->Hold == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_HLD));
      if (Msg->FileAttach == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_ATT));
      if (Msg->FileRequest == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_FRQ));
      if (Msg->Intransit == TRUE)
         strcat (Flags, Language->Text (LNG_MSGFLAG_TRS));

      if (User->FullReader == TRUE && (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE)) {
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEHDR), Current->Display, (CHAR)(80 - strlen (Current->Display) - 3));
         if (Msg->Original != 0L && Msg->Reply == 0L)
            sprintf (Temp, Language->Text (LNG_MESSAGENUMBER1), Msgn, Msg->Number (), Msg->UidToMsgn (Msg->Original));
         else if (Msg->Original == 0L && Msg->Reply != 0L)
            sprintf (Temp, Language->Text (LNG_MESSAGENUMBER2), Msgn, Msg->Number (), Msg->UidToMsgn (Msg->Reply));
         else if (Msg->Original != 0L && Msg->Reply != 0L)
            sprintf (Temp, Language->Text (LNG_MESSAGENUMBER3), Msgn, Msg->Number (), Msg->UidToMsgn (Msg->Original), Msg->UidToMsgn (Msg->Reply));
         else
            sprintf (Temp, Language->Text (LNG_MESSAGENUMBER), Msgn, Msg->Number ());
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEFLAGS), Temp, Flags);

         BuildDate (Language->Text (LNG_MESSAGEDATE), Temp, &Msg->Written);
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEFROM), Msg->From, Msg->FromAddress, Temp);
         BuildDate (Language->Text (LNG_MESSAGEDATE), Temp, &Msg->Arrived);
         Embedded->BufferedPrintf (Language->Text (LNG_MESSAGETO), Msg->To, Msg->ToAddress, Temp);
         if (Msg->FileAttach == TRUE || Msg->FileRequest == TRUE)
            Embedded->BufferedPrintf (Language->Text (LNG_MESSAGEFILE), Msg->Subject);
         else
            Embedded->BufferedPrintf (Language->Text (LNG_MESSAGESUBJECT), Msg->Subject);
         Embedded->BufferedPrintf ("\x16\x01\x13\031Ä\120");

         InitialLine = Line = 6;
      }
      else {
         Embedded->BufferedPrintf ("\x0C");
         Embedded->BufferedPrintf (Language->Text (LNG_READERFROM), Msg->From, Flags);

         BuildDate (Language->Text (LNG_MESSAGEDATE), Temp, &Msg->Written);
         Embedded->BufferedPrintf (Language->Text (LNG_READERTO), Msg->To, Msgn, Temp);

         if (Msg->FileAttach == TRUE || Msg->FileRequest == TRUE)
            Embedded->BufferedPrintf (Language->Text (LNG_READERFILE), Msg->Subject);
         else
            Embedded->BufferedPrintf (Language->Text (LNG_READERSUBJECT), Msg->Subject);

         InitialLine = Line = 4;
      }

      if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if (ShowKludges == TRUE || (strncmp (Text, "SEEN-BY: ", 9) && Text[0] != 1)) {
               if (!strncmp (Text, "SEEN-BY: ", 9) || Text[0] == 1) {
                  if (Text[0] == 1)
                     Text++;
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEKLUDGE), Text);
               }
               else if (!strncmp (Text, " * Origin", 9) || !strncmp (Text, "---", 3))
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEORIGIN), Text);
               else if (strchr (Text, '>') != NULL)
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGEQUOTE), Text);
               else
                  Embedded->BufferedPrintf (Language->Text(LNG_MESSAGETEXT), Text);

               MaxLine = Line;
               if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                  MaxLine++;
                  while (MaxLine > InitialLine)
                     Embedded->BufferedPrintfAt (MaxLine--, 1, "\026\007");
                  Line = InitialLine;
               }
            }
         } while ((Text = (CHAR *)Msg->Text.Next ()) != NULL && Embedded->AbortSession () == FALSE && Line != 0);

      if (User->FullReader == FALSE || (Embedded->Ansi == FALSE && Embedded->Avatar == FALSE)) {
         if (Msg->Original != 0L && Msg->Reply == 0L)
            Embedded->BufferedPrintf ("\n\026\001\017*** This is a reply to #%lu.\n", Msg->UidToMsgn (Msg->Original));
         else if (Msg->Original == 0L && Msg->Reply != 0L)
            Embedded->BufferedPrintf ("\n\026\001\017*** See also #%lu.\n", Msg->UidToMsgn (Msg->Reply));
         else if (Msg->Original != 0L && Msg->Reply != 0L)
            Embedded->BufferedPrintf ("\n\026\001\017*** This is a reply to #%lu.  *** See also #%lu.\n", Msg->UidToMsgn (Msg->Original), Msg->UidToMsgn (Msg->Reply));
         Line += 2;
      }

      Embedded->UnbufferBytes ();

      if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
         Msg->Received = TRUE;
         Msg->WriteHeader (Msg->Current);
      }

      if (Line > InitialLine) {
         MaxLine = Line;
         Embedded->MoreQuestion (99);
         MaxLine++;
         while (MaxLine > InitialLine)
            Embedded->BufferedPrintfAt (MaxLine--, 1, "\026\007");
      }
   }
}

USHORT TInquire::FirstMessage (VOID)
{
   USHORT RetVal = FALSE, Total = 0;
   CHAR Temp[128], *p;

   if (Msg != NULL) {
      Msg->Lock (0L);
      do {
         if ((++Total % 16) == 0) {
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
   USHORT RetVal = FALSE, Total = 0;
   CHAR Temp[128], *p;

   if (Msg != NULL) {
      Msg->Lock (0L);
      while (Stop == FALSE && Msg->Next (Number) == TRUE && Embedded->AbortSession () == FALSE) {
         if ((++Total % 16) == 0) {
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
   USHORT RetVal = FALSE, Total = 0;
   CHAR Temp[128], *p;

   if (Msg != NULL) {
      Msg->Lock (0L);
      while (Stop == FALSE && Msg->Previous (Number) == TRUE && Embedded->AbortSession () == FALSE) {
         if ((++Total % 16) == 0) {
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
                     Embedded->Printf ("\n\026\001\012Searching area %s ...                ", Data->Key);
                     NewLine = FALSE;
                  }
                  else
                     Embedded->Printf ("\r\026\001\012Searching area %s ...                ", Data->Key);
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
                  else if (Current->Storage == ST_HUDSON)
                     Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

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
                        Embedded->Printf ("\n\026\001\012Searching area %s ...                ", Data->Key);
                        NewLine = FALSE;
                     }
                     else
                        Embedded->Printf ("\r\026\001\012Searching area %s ...                ", Data->Key);
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
                     else if (Current->Storage == ST_HUDSON)
                        Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

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
      else if (Current->Storage == ST_HUDSON)
         Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

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
      Embedded->Printf ("\r\026\001\012Searching area %s ...                ", Data->Key);
   else
      Embedded->Printf ("\n\026\001\012Searching area %s ...                ", Data->Key);
   if (Msg != NULL && NextMessage () == TRUE)
      RetVal = TRUE;

   if (RetVal == FALSE) {
      if (Range == RANGE_ALL) {
         if (Data->Next () == TRUE)
            do {
               if (User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     Embedded->Printf ("\r\026\001\012Searching area %s ...                ", Data->Key);
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
                     else if (Current->Storage == ST_HUDSON)
                        Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

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
                        Embedded->Printf ("\r\026\001\012Searching area %s ...                ", Data->Key);
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
                        else if (Current->Storage == ST_HUDSON)
                           Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

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

   Embedded->Printf ("\r\026\001\012Searching area %s ...                \r", Data->Key);
   if (Msg != NULL && PreviousMessage () == TRUE)
      RetVal = TRUE;

   if (RetVal == FALSE) {
      if (Range == RANGE_ALL) {
         if (Data->Previous () == TRUE)
            do {
               if (User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     Embedded->Printf ("\r\026\001\012Searching area %s ...                ", Data->Key);
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
                     else if (Current->Storage == ST_HUDSON)
                        Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

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
                        Embedded->Printf ("\r\026\001\012Searching area %s ...                ", Data->Key);
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
                        else if (Current->Storage == ST_HUDSON)
                           Msg = new HUDSON (Current->Path, (UCHAR)Current->Board);

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
         while (Embedded->AbortSession () == FALSE && Cmd != Language->Text(LNG_EXITREADMESSAGE)[0]) {
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
               Embedded->Printf (Language->Text(LNG_READMENU));
            else
               Embedded->Printf (Language->Text(LNG_ENDREADMENU));
            Embedded->GetString (Temp, 10, INP_HOTKEY|INP_NUMERIC);
            if ((Cmd = (CHAR)toupper (Temp[0])) == '\0')
               Cmd = Language->Text(LNG_NEXTMESSAGE)[0];

            if (Cmd == Language->Text(LNG_NEXTMESSAGE)[0]) {
               End = FALSE;
               if (Next () == FALSE) {
                  Embedded->Printf (Language->Text(LNG_ENDOFMESSAGES));
                  End = TRUE;
               }
               else
                  DoRead = TRUE;
            }
            else if (Cmd == Language->Text(LNG_PREVIOUSMESSAGE)[0]) {
               End = FALSE;
               if (Previous () == FALSE) {
                  Embedded->Printf (Language->Text(LNG_ENDOFMESSAGES));
                  End = TRUE;
               }
               else
                  DoRead = TRUE;
            }
            else if (Cmd == Language->Text(LNG_REPLYMESSAGE)[0]) {
               if (User->Level >= Current->WriteLevel) {
                  if ((User->AccessFlags & Current->WriteFlags) == Current->WriteFlags) {
                     if ((Editor = new TMsgEditor) != NULL) {
                        Editor->Cfg = Cfg;
                        Editor->Embedded = Embedded;
                        Editor->Log = Log;
                        Editor->Msg = Msg;
                        Editor->Language = Language;
                        Editor->Width = User->ScreenWidth;
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
            else if (Cmd == Language->Text(LNG_EMAILREPLYMESSAGE)[0] && End == FALSE) {
               if ((MailEditor = new TMailEditor) != NULL) {
                  Editor->Cfg = Cfg;
                  MailEditor->Embedded = Embedded;
                  MailEditor->Log = Log;
                  MailEditor->Msg = Msg;
                  MailEditor->Language = Language;
                  MailEditor->Width = User->ScreenWidth;
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
               Embedded->Printf ("\026\001\016%5ld  \026\001\012%-22.22s  %-21.21s  %-.23s\n", Msg->UidToMsgn (Msg->Current), Msg->From, Msg->To, Msg->Subject);
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
      Embedded->Printf (Language->Text(LNG_ENDOFMESSAGES));

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

VOID TInquire::BuildDate (PSZ format, PSZ dest, MDATE *date)
{
   CHAR Temp[16];

   while (*format != '\0') {
      if (*format == '%') {
         format++;
         switch (*format) {
            case 'A':
               if (date->Hour >= 12)
                  strcpy (dest, "pm");
               else
                  strcpy (dest, "am");
               dest += 2;
               format++;
               break;
            case 'B':
               sprintf (Temp, "%2d", date->Month);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'C':
               sprintf (Temp, "%-3.3s", Language->Months[date->Month - 1]);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'D':
               sprintf (Temp, "%2d", date->Day);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'E':
               if (date->Hour > 12)
                  sprintf (Temp, "%2d", date->Hour - 12);
               else
                  sprintf (Temp, "%2d", date->Hour);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'H':
               sprintf (Temp, "%2d", date->Hour);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'M':
               sprintf (Temp, "%02d", date->Minute);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'S':
               sprintf (Temp, "%02d", date->Second);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'Y':
               sprintf (Temp, "%2d", date->Year % 100);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            case 'Z':
               sprintf (Temp, "%4d", date->Year);
               strcpy (dest, Temp);
               dest += strlen (Temp);
               format++;
               break;
            default:
               *dest++ = *format++;
               break;
         }
      }
      else
         *dest++ = *format++;
   }
   *dest = '\0';
}

