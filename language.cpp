
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    09/02/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

PSZ Keywords[] = {
   "",

   "LanguageName", "Yes", "No", "Help", "Male", "Female", "January", "February",
   "March", "April", "May", "Juni", "July", "August", "September", "October",
   "November", "December", "PressEnter", "DefYesNo", "YesDefNo", "DefYesNoHelp",
   "YesDefNoHelp", "AskAddress", "AskAnsi", "AskCity", "AskCompanyName", "AskDayPhone",
   "AskPassword", "AskRealName", "AskSex", "EnterName", "EnterNameOrNew", "EnterPassword",
   "InvalidPassword", "HaveTagged", "TerminateConnection", "YouSure", "UserFromCity",
   "MenuError", "MessageHdr", "MessageFrom", "MessageTo", "MessageSubject",
   "MessageText", "MessageQuote", "MessageKludge", "MessageOrigin", "EndOfMessages", "ReadMenu",

   "EndReadMenu", "NextMessage", "ExitReadMessage", "RereadMessage", "PreviousMessage",
   "ReplyMessage", "EMailReplyMessage", "SelectConference", "ConferenceListHdr",
   "ConferenceList", "MenuName", "ConferenceNotAvailable", "StartWithMessage",
   "NewMessages", "TextFiles", "ForumName", "ForumOperator", "ForumTopic", "MenuPath",
   "MoreQuestion", "DeleteMoreQuestion", "NonStop", "Quit", "Continue", "NameNotFound",

   NULL
};

TLanguage::TLanguage (void)
{
   Default ();
}

TLanguage::~TLanguage (void)
{
   if (TextMemory != NULL)
      free (TextMemory);
}

USHORT TLanguage::CheckKeyword (PSZ pszKey)
{
   USHORT RetVal = 0, i = 0;

   while (Keywords[i] != NULL && RetVal == 0) {
      if (!stricmp (Keywords[i], pszKey))
         RetVal = i;
      i++;
   }

   return (RetVal);
}

PSZ TLanguage::CopyString (USHORT Key, PSZ Arg, PSZ Pointer)
{
   UCHAR c, c1;

   switch (Key) {
      case 1:
         strcpy (Comment, Arg);
         Key = 0;
         break;
      case 2:
         Yes = *Arg;
         break;
      case 3:
         No = *Arg;
         break;
      case 4:
         Help = *Arg;
         break;
      case 5:
         Male = *Arg;
         break;
      case 6:
         Female = *Arg;
         break;
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
         Months[Key - 7] = Pointer;
         break;
      case 19:
         PressEnter = Pointer;
         break;
      case 20:
         DefYesNo = Pointer;
         break;
      case 21:
         YesDefNo = Pointer;
         break;
      case 22:
         DefYesNoHelp = Pointer;
         break;
      case 23:
         YesDefNoHelp = Pointer;
         break;
      case 24:
         AskAddress = Pointer;
         break;
      case 25:
         AskAnsi = Pointer;
         break;
      case 26:
         AskCity = Pointer;
         break;
      case 27:
         AskCompanyName = Pointer;
         break;
      case 28:
         AskDayPhone = Pointer;
         break;
      case 29:
         AskPassword = Pointer;
         break;
      case 30:
         AskRealName = Pointer;
         break;
      case 31:
         AskSex = Pointer;
         break;
      case 32:
         EnterName = Pointer;
         break;
      case 33:
         EnterNameOrNew = Pointer;
         break;
      case 34:
         EnterPassword = Pointer;
         break;
      case 35:
         InvalidPassword = Pointer;
         break;
      case 36:
         HaveTagged = Pointer;
         break;
      case 37:
         TerminateConnection = Pointer;
         break;
      case 38:
         YouSure = Pointer;
         break;
      case 39:
         UserFromCity = Pointer;
         break;
      case 40:
         MenuError = Pointer;
         break;
      case 41:
         MessageHdr = Pointer;
         break;
      case 42:
         MessageFrom = Pointer;
         break;
      case 43:
         MessageTo = Pointer;
         break;
      case 44:
         MessageSubject = Pointer;
         break;
      case 45:
         MessageText = Pointer;
         break;
      case 46:
         MessageQuote = Pointer;
         break;
      case 47:
         MessageKludge = Pointer;
         break;
      case 48:
         MessageOrigin = Pointer;
         break;
      case 49:
         EndOfMessages = Pointer;
         break;
      case 50:
         ReadMenu = Pointer;
         break;
      case 51:
         EndReadMenu = Pointer;
         break;
      case 52:
         NextMessage = *Arg;
         break;
      case 53:
         ExitReadMessage = *Arg;
         break;
      case 54:
         RereadMessage = *Arg;
         break;
      case 55:
         PreviousMessage = *Arg;
         break;
      case 56:
         ReplyMessage = *Arg;
         break;
      case 57:
         EMailReplyMessage = *Arg;
         break;
      case 58:
         SelectConference = Pointer;
         break;
      case 59:
         ConferenceListHdr = Pointer;
         break;
      case 60:
         ConferenceList = Pointer;
         break;
      case 61:
         strcpy (MenuName, Arg);
         Key = 0;
         break;
      case 62:
         ConferenceNotAvailable = Pointer;
         break;
      case 63:
         StartWithMessage = Pointer;
         break;
      case 64:
         NewMessages = Pointer;
         break;
      case 65:
         strcpy (TextFiles, Arg);
         Key = 0;
         break;
      case 66:
         ForumName = Pointer;
         break;
      case 67:
         ForumOperator = Pointer;
         break;
      case 68:
         ForumTopic = Pointer;
         break;
      case 69:
         strcpy (MenuPath, Arg);
         Key = 0;
         break;
      case 70:
         MoreQuestion = Pointer;
         break;
      case 71:
         DeleteMoreQuestion = Pointer;
         break;
      case 72:
         NonStop = *Arg;
         break;
      case 73:
         Quit = *Arg;
         break;
      case 74:
         Continue = *Arg;
         break;
      case 75:
         NameNotFound = Pointer;
         break;
   }

   if (Key != 0) {
      while (*Arg != '\0') {
         if (*Arg == '\\' && *(Arg + 1) == 'x') {
            Arg += 2;
            if (*Arg != '\0' && *(Arg + 1) != '\0') {
               if ((c = (UCHAR)(toupper (*Arg) - '0')) > 9)
                  c -= 7;
               c <<= 4;
               if ((c1 = (UCHAR)(toupper (*(Arg + 1)) - '0')) > 9)
                  c1 -= 7;
               c |= c1;
               *Pointer++ = (CHAR)c;
               Arg += 2;
            }
         }
         else if (*Arg == '\\') {
            Arg++;
            if (*Arg == 'a')
               *Pointer++ = '\a';
            else if (*Arg == 'n')
               *Pointer++ = '\n';
            else if (*Arg == 'r')
               *Pointer++ = '\r';
            else if (*Arg == 't')
               *Pointer++ = '\t';
            else if (isdigit (*Arg)) {
               if (*(Arg + 1) != '\0' && *(Arg + 2) != '\0') {
                  c = (UCHAR)((*Arg - '0') * 64);
                  c += (UCHAR)((*(Arg + 1) - '0') * 8);
                  c += (UCHAR)(*(Arg + 2) - '0');
                  *Pointer++ = (CHAR)c;
                  Arg += 2;
               }
            }
            Arg++;
         }
         else
            *Pointer++ = *Arg++;
      }
      *Pointer++ = *Arg++;
   }

   return (Pointer);
}

VOID TLanguage::Default (VOID)
{
   TextMemory = NULL;
   strcpy (File, "default.lng");
   strcpy (Comment, "Default Language");
   TextFiles[0] = MenuName[0] = '\0';
   MenuPath[0] = '\0';

   Yes = 'Y';
   No = 'N';
   Help = '?';
   Male = 'M';
   Female = 'F';

   Months[0] = "January";
   Months[1] = "February";
   Months[2] = "March";
   Months[3] = "April";
   Months[4] = "May";
   Months[5] = "Juni";
   Months[6] = "July";
   Months[7] = "August";
   Months[8] = "September";
   Months[9] = "October";
   Months[10] = "November";
   Months[11] = "December";

   DefYesNo = " (Y,n)? \x16\x01\x1E";
   YesDefNo = " (y,N)? \x16\x01\x1E";
   DefYesNoHelp = " (Y,n,?)? \x16\x01\x1E";
   YesDefNoHelp = " (y,N,?)? \x16\x01\x1E";

   MenuError = "\n\x16\x01\x0DPlease select one of the choices presented.\n\006\007\006\007";
   PressEnter = "Premi [Enter] per continuare: ";
   MoreQuestion = "(N)onStop, (Q)uit or (C)ontinue? ";
   DeleteMoreQuestion = "\r                                   \r";
   NonStop = 'N';
   Quit = 'Q';
   Continue = 'C';

   AskAddress = "\n\x16\x01\013Enter your street address or P.O. Box:\n\x16\x01\x1E";
   AskAnsi = "\nŠQuesta parola potrebbe lampeggiare o no: ŽANSI‹\nSta lampeggiando";
   AskCity = "\n\x16\x01\013Enter your city, state and ZIP code:\n\x16\x01\x1E";
   AskCompanyName = "\n\x16\x01\013Now enter your company name, or just RETURN if none:\n\x16\x01\x1E";
   AskDayPhone = "\n\x16\x01\013Now enter the telephone number where you can be reached during the day:\n\x16\x01\x1E";
   AskPassword = "\nŠInserisci la Password: ";
   AskRealName = "\n\x16\x01\013Please enter your real first and last name:\n\x16\x01\x1E";
   AskSex = "\n\x16\x01\013Are you male or female? \x16\x01\x1E";
   EnterName = "\nŠInserisci Nome e Cognome: ";
   EnterNameOrNew = "\nŠInserisci Nome e Cognome (oppure \"New\"): ";
   EnterPassword = "\nŠPassword: ";
   InvalidPassword = "\nNome o password non validi.\a\n";
   NameNotFound = "\n\026\001\012Your name was not found in our database.\n\x16\x01\013Do you wish to continue as a new user";
   UserFromCity = "\n\x16\x01\x0E%s from %s";

   MessageHdr = "\x0C\x16\x01\x0A   Date: %d %s %d %2d:%02d\n";
   MessageFrom = "\x16\x01\x0A   From: %-40.40s Msg. #%ld of %ld\n";
   MessageTo = "\x16\x01\x0A     To: %s\n";
   MessageSubject = "\x16\x01\x0ASubject: %s\n\n";
   MessageText = "\026\001\012%s\n";
   MessageQuote = "\026\001\012%s\n";
   MessageKludge = "\026\001\012%s\n";
   MessageOrigin = "\026\001\012%s\n";

   EndOfMessages = "\n\x16\x01\x0DYou have reached the end of these messages.\n\006\007\006\007";
   ReadMenu = "\n\x16\x01\013(R)eply, (E)mail reply, (P)revious, (N)ext or e(X)it: ";
   EndReadMenu = "\n\x16\x01\013(P)revious message, (R)e-Read last message or e(X)it: ";
   NextMessage = 'N';
   ExitReadMessage = 'X';
   PreviousMessage = 'P';
   RereadMessage = 'R';
   ReplyMessage = 'R';
   EMailReplyMessage = 'R';
   SelectConference = "\n\026\001\013Enter the name of the Conference, or ? for a list: \026\001\x1E";
   ConferenceListHdr = "\n\026\001\012Conference       Msgs   Description\n\026\001\017\031Ä\017  \031Ä\005  \031Ä\067\n";
   ConferenceList = "\026\001\013%-15.15s  \026\001\016%5ld  %.55s\n";
   ConferenceNotAvailable = "\n\026\001\014This Conference isn't available to you.\n";
   StartWithMessage = "\n\x16\x01\013Enter message number to start with, F for the\nfirst message, L for the last, or ? for help";
   NewMessages = ".\n(Just hit RETURN to start with new messages)";

   ForumName = "\014\026\001\012FORUM: %s\n";
   ForumOperator = "\026\001\012Forum-Op: %s\n";
   ForumTopic = "\026\001\012Forum Topic: %s\n\n";

   HaveTagged = "\n\026\001\015You have %u tagged files.\n";
   TerminateConnection = "\n\x16\x01\x0FYou are about to terminate this connection!\n";
   YouSure = "\n\x16\x01\013Are you sure (Y/N)? ";
}

USHORT TLanguage::Load (PSZ pszFile)
{
   FILE *fp;
   USHORT RetVal = FALSE;
   CHAR *Temp, *Key, *Arg, *p;
   ULONG Size = 0L;

   if ((fp = _fsopen (pszFile, "rt", SH_DENYNO)) != NULL) {
      RetVal = TRUE;
      strcpy (File, pszFile);

      if (TextMemory != NULL) {
         free (TextMemory);
         TextMemory = NULL;
      }
      Default ();

      if ((Temp = (CHAR *)malloc (MAX_LINE)) != NULL) {
         while (fgets (Temp, MAX_LINE - 1, fp) != NULL) {
            if ((Key = strtok (Temp, " =")) != NULL) {
               if ((p = strtok (NULL, "")) != NULL) {
                  while (*p != '\0' && *p != '\"')
                     p++;
                  if (*p == '\"') {
                     Arg = p + 1;
                     if ((p = strchr (p, '\0')) != NULL) {
                        while (*p != '\"' && p > Arg)
                           *p-- = '\0';
                        *p-- = '\0';
                     }
                     if (CheckKeyword (Key) != 0)
                        Size += strlen (Arg) + 1;
                  }
               }
            }
         }

#if defined(__DOS__) && defined(__BORLANDC__)
         if ((TextMemory = (PSZ)farmalloc (Size)) != NULL) {
#else
         if ((TextMemory = (PSZ)malloc (Size)) != NULL) {
#endif
            rewind (fp);
            Pointer = TextMemory;

            while (fgets (Temp, MAX_LINE - 1, fp) != NULL) {
               if ((Key = strtok (Temp, " =")) != NULL) {
                  if ((p = strtok (NULL, "")) != NULL) {
                     while (*p != '\0' && *p != '\"')
                        p++;
                     if (*p == '\"') {
                        Arg = p + 1;
                        if ((p = strchr (p, '\0')) != NULL) {
                           while (*p != '\"' && p > Arg)
                              *p-- = '\0';
                           *p-- = '\0';
                        }
                        Pointer = CopyString (CheckKeyword (Key), Arg, Pointer);
                     }
                  }
               }
            }
         }

         free (Temp);
      }

      fclose (fp);
   }

   return (RetVal);
}


