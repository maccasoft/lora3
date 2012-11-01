
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "tools.h"

TLanguage::TLanguage (void)
{
   Translate = Buffer = NULL;
   Default ();
}

TLanguage::TLanguage (PSZ pszLanguage)
{
   Translate = Buffer = NULL;
   Default ();
   Read (pszLanguage);
}

TLanguage::~TLanguage (void)
{
   if (Buffer != NULL)
      free (Buffer);
   if (Translate != NULL)
      free (Translate);
}

PSZ TLanguage::CheckCharItem (PSZ pszItem)
{
   if (!stricmp (pszItem, "Yes"))
      return (&Yes);
   if (!stricmp (pszItem, "No"))
      return (&No);
   if (!stricmp (pszItem, "Help"))
      return (&Help);
   if (!stricmp (pszItem, "NonStopKey"))
      return (&NonStopKey);
   if (!stricmp (pszItem, "QuitKey"))
      return (&QuitKey);
   if (!stricmp (pszItem, "ContinueKey"))
      return (&ContinueKey);
   if (!stricmp (pszItem, "MailFromYouKey"))
      return (&MailFromYouKey);
   if (!stricmp (pszItem, "MailToYouKey"))
      return (&MailToYouKey);
   if (!stricmp (pszItem, "WhichMailHelpKey"))
      return (&WhichMailHelpKey);
   if (!stricmp (pszItem, "FirstMessageKey"))
      return (&FirstMessageKey);
   if (!stricmp (pszItem, "LastMessageKey"))
      return (&LastMessageKey);
   if (!stricmp (pszItem, "MailStartHelpKey"))
      return (&MailStartHelpKey);
   if (!stricmp (pszItem, "ReadMessageKey"))
      return (&ReadMessageKey);
   if (!stricmp (pszItem, "NextMessageKey"))
      return (&NextMessageKey);
   if (!stricmp (pszItem, "PreviousMessageKey"))
      return (&PreviousMessageKey);
   if (!stricmp (pszItem, "ExitKey"))
      return (&ExitKey);
   if (!stricmp (pszItem, "ReplyKey"))
      return (&ReplyKey);
   if (!stricmp (pszItem, "RereadKey"))
      return (&RereadKey);
   if (!stricmp (pszItem, "EraseKey"))
      return (&EraseKey);
   if (!stricmp (pszItem, "ForwardKey"))
      return (&ForwardKey);
   if (!stricmp (pszItem, "CopyKey"))
      return (&CopyKey);

   return (NULL);
}

PSZ *TLanguage::CheckStringItem (PSZ pszItem)
{
   if (!stricmp (pszItem, "DefYesNo"))
      return (&DefYesNo);
   if (!stricmp (pszItem, "YesDefNo"))
      return (&YesDefNo);
   if (!stricmp (pszItem, "DefYesNoHelp"))
      return (&DefYesNoHelp);
   if (!stricmp (pszItem, "YesDefNoHelp"))
      return (&YesDefNoHelp);

   if (!stricmp (pszItem, "AskAnsi"))
      return (&AskAnsi);
   if (!stricmp (pszItem, "AskRealName"))
      return (&AskRealName);
   if (!stricmp (pszItem, "AskSex"))
      return (&AskSex);
   if (!stricmp (pszItem, "City"))
      return (&City);
   if (!stricmp (pszItem, "Country"))
      return (&Country);
   if (!stricmp (pszItem, "EnterCompanyName"))
      return (&EnterCompanyName);
   if (!stricmp (pszItem, "NameNotFound"))
      return (&NameNotFound);
   if (!stricmp (pszItem, "PhoneNumber"))
      return (&PhoneNumber);
   if (!stricmp (pszItem, "PleaseReenter"))
      return (&PleaseReenter);
   if (!stricmp (pszItem, "SelectPassword"))
      return (&SelectPassword);
   if (!stricmp (pszItem, "StreetAddress"))
      return (&StreetAddress);
   if (!stricmp (pszItem, "Wrong"))
      return (&Wrong);
   if (!stricmp (pszItem, "WrongPassword"))
      return (&WrongPassword);

   if (!stricmp (pszItem, "AvailableFileAreas"))
      return (&AvailableFileAreas);
   if (!stricmp (pszItem, "AvailableForums"))
      return (&AvailableForums);
   if (!stricmp (pszItem, "AvailableMsgAreas"))
      return (&AvailableMsgAreas);
   if (!stricmp (pszItem, "EnterName"))
      return (&EnterName);
   if (!stricmp (pszItem, "ForumNotAvailable"))
      return (&ForumNotAvailable);
   if (!stricmp (pszItem, "Password"))
      return (&Password);
   if (!stricmp (pszItem, "PleaseAnswer2"))
      return (&PleaseAnswer2);
   if (!stricmp (pszItem, "PleaseAnswer3"))
      return (&PleaseAnswer3);

   if (!stricmp (pszItem, "MoreQuestion"))
      return (&MoreQuestion);
   if (!stricmp (pszItem, "DeleteMoreQuestion"))
      return (&DeleteMoreQuestion);

   if (!stricmp (pszItem, "LogoffWarning"))
      return (&LogoffWarning);
   if (!stricmp (pszItem, "LogoffConfirm"))
      return (&LogoffConfirm);

   if (!stricmp (pszItem, "ReadMailMenu"))
      return (&ReadMailMenu);
   if (!stricmp (pszItem, "ReadMessageMenu"))
      return (&ReadMessageMenu);
   if (!stricmp (pszItem, "EndOfMessagesMenu"))
      return (&EndOfMessagesMenu);
   if (!stricmp (pszItem, "ReadWhichMail"))
      return (&ReadWhichMail);
   if (!stricmp (pszItem, "ThreadMenu"))
      return (&ThreadMenu);
   if (!stricmp (pszItem, "NoMessagesInMailBox"))
      return (&NoMessagesInMailBox);
   if (!stricmp (pszItem, "NoMessagesFromYou"))
      return (&NoMessagesFromYou);
   if (!stricmp (pszItem, "StartMessageNumber"))
      return (&StartMessageNumber);
   if (!stricmp (pszItem, "StartNewMessages"))
      return (&StartNewMessages);
   if (!stricmp (pszItem, "StartPrompt"))
      return (&StartPrompt);
   if (!stricmp (pszItem, "EndOfMessages"))
      return (&EndOfMessages);
   if (!stricmp (pszItem, "StartOfMessages"))
      return (&StartOfMessages);
   if (!stricmp (pszItem, "MessageHdr"))
      return (&MessageHdr);
   if (!stricmp (pszItem, "MessageFrom"))
      return (&MessageFrom);
   if (!stricmp (pszItem, "MessageTo"))
      return (&MessageTo);
   if (!stricmp (pszItem, "MessageSubject"))
      return (&MessageSubject);
   if (!stricmp (pszItem, "MessageText"))
      return (&MessageText);
   if (!stricmp (pszItem, "MessageQuote"))
      return (&MessageQuote);
   if (!stricmp (pszItem, "MessageKludge"))
      return (&MessageKludge);
   if (!stricmp (pszItem, "ReadThisMessage"))
      return (&ReadThisMessage);
   if (!stricmp (pszItem, "MessageToErase"))
      return (&MessageToErase);
   if (!stricmp (pszItem, "ConfirmedErase"))
      return (&ConfirmedErase);
   if (!stricmp (pszItem, "CantErase"))
      return (&CantErase);
   if (!stricmp (pszItem, "EnterPreference"))
      return (&EnterPreference);
   if (!stricmp (pszItem, "ModifyNotice"))
      return (&ModifyNotice);
   if (!stricmp (pszItem, "ModifyNumber"))
      return (&ModifyNumber);
   if (!stricmp (pszItem, "MessageOutOfRange"))
      return (&MessageOutOfRange);
   if (!stricmp (pszItem, "CantModify"))
      return (&CantModify);
   if (!stricmp (pszItem, "EditorMenu"))
      return (&EditorMenu);

   if (!stricmp (pszItem, "WhoToSend"))
      return (&WhoToSend);
   if (!stricmp (pszItem, "NoSuchName"))
      return (&NoSuchName);
   if (!stricmp (pszItem, "TypeMessageNow"))
      return (&TypeMessageNow);
   if (!stricmp (pszItem, "ContinueEntering"))
      return (&ContinueEntering);
   if (!stricmp (pszItem, "InsertAfterLine"))
      return (&InsertAfterLine);
   if (!stricmp (pszItem, "LineOutOfRange"))
      return (&LineOutOfRange);
   if (!stricmp (pszItem, "ConfirmedCopy"))
      return (&ConfirmedCopy);
   if (!stricmp (pszItem, "ConfirmedSend"))
      return (&ConfirmedSend);
   if (!stricmp (pszItem, "SendToAnother"))
      return (&SendToAnother);
   if (!stricmp (pszItem, "SendToWho"))
      return (&SendToWho);
   if (!stricmp (pszItem, "TextList"))
      return (&TextList);
   if (!stricmp (pszItem, "ChangeLine"))
      return (&ChangeLine);
   if (!stricmp (pszItem, "CurrentLineReads"))
      return (&CurrentLineReads);
   if (!stricmp (pszItem, "TextToChange"))
      return (&TextToChange);
   if (!stricmp (pszItem, "NewText"))
      return (&NewText);
   if (!stricmp (pszItem, "LineNowReads"))
      return (&LineNowReads);
   if (!stricmp (pszItem, "EnterSubject"))
      return (&EnterSubject);
   if (!stricmp (pszItem, "DeleteLine"))
      return (&DeleteLine);
   if (!stricmp (pszItem, "OkToDelete"))
      return (&OkToDelete);
   if (!stricmp (pszItem, "RetypeLine"))
      return (&RetypeLine);
   if (!stricmp (pszItem, "EnterNewLine"))
      return (&EnterNewLine);
   if (!stricmp (pszItem, "EditorExited"))
      return (&EditorExited);
   if (!stricmp (pszItem, "MailWhoToSend"))
      return (&MailWhoToSend);
   if (!stricmp (pszItem, "ToAll"))
      return (&ToAll);

   if (!stricmp (pszItem, "ForumList"))
      return (&ForumList);
   if (!stricmp (pszItem, "ForumListHeader"))
      return (&ForumListHeader);
   if (!stricmp (pszItem, "ForumNotAvailable"))
      return (&ForumNotAvailable);
   if (!stricmp (pszItem, "ForumSelect"))
      return (&ForumSelect);
   if (!stricmp (pszItem, "ListKey"))
      return (&ListKey);
   if (!stricmp (pszItem, "MessageList"))
      return (&MessageList);
   if (!stricmp (pszItem, "MessageListHeader"))
      return (&MessageListHeader);
   if (!stricmp (pszItem, "MessageNotAvailable"))
      return (&MessageNotAvailable);
   if (!stricmp (pszItem, "MessageSelect"))
      return (&MessageSelect);
   if (!stricmp (pszItem, "NoMessagesInArea"))
      return (&NoMessagesInArea);

   return (NULL);
}

VOID TLanguage::Default (VOID)
{
   // Yes/No questions answers
   Yes = 'Y';
   No = 'N';
   Help = '?';

   // Months
   Months[0] = "January";
   Months[1] = "February";
   Months[2] = "March";
   Months[3] = "April";
   Months[4] = "Maj";
   Months[5] = "Juni";
   Months[6] = "July";
   Months[7] = "August";
   Months[8] = "September";
   Months[9] = "October";
   Months[10] = "November";
   Months[11] = "December";

   // Yes/No-type questions
   DefYesNo = " (Y,n)? \x16\x01\x1E";
   YesDefNo = " (y,N)? \x16\x01\x1E";
   DefYesNoHelp = " (Y,n,?)? \x16\x01\x1E";
   YesDefNoHelp = " (y,N,?)? \x16\x01\x1E";

   // End of page question
   MoreQuestion = "\026\001\017(N)onStop, (Q)uit or (C)ontinue? ";
   DeleteMoreQuestion = "\r                                  \r";
   NonStopKey = 'N';
   QuitKey = 'Q';
   ContinueKey = 'C';

   // Login
   EnterName = "\n\026\001\x0APlease enter your full name : \026\001\x1E";
   Password = "\n\026\001\x0APassword: \x16\x01\x1E";
   Wrong = "\n\026\001\x0CWrong !\n\n\x07";
   NameNotFound = "\n\026\001\012Name not found in our database.\n\x16\x01\013Do you want to continue as a new user";

   // New users default questions
   AskAnsi = "\n\x16\x01\x0AThe following word may or may not be blinking: \x16\x01\216ANSI\x16\x01\x0B\nIs it blinking";
   AskRealName = "\n\x16\x01\013Please enter your real first and last name:\n\x16\x01\x1E";
   EnterCompanyName = "\n\x16\x01\013Now enter your company name, or just RETURN if none:\n\x16\x01\x1E";
   StreetAddress = "\n\x16\x01\013Enter your street address or P.O. Bpx:\n\x16\x01\x1E";
   City = "\n\x16\x01\013Enter your city, state and ZIP code:\n\x16\x01\x1E";
   Country = "\n\x16\x01\013Enter your Country:\n\x16\x01\x1E";
   PhoneNumber = "\n\x16\x01\013Now enter the telephone number where you can be reached during the day:\n\x16\x01\x1E";
   AskSex = "\n\x16\x01\013Are you male or female? \x16\x01\x1E";
   SelectPassword = "\n\026\001\013Select your password to use [minimum 4 chars] : \x16\x01\x1E";
   PleaseReenter = "\n\x16\x01\013Please reenter your password for verification : \x16\x01\x1E";
   WrongPassword = "\nWrong! The password was `%s'\nYou wrote `%s'.\n\n";

   // Read mail / messages
   MessageSelect = "\n\026\001\013Enter the name of the Conference, or ? for a list: \026\001\x1E";
   MessageListHeader = "\n\026\001\012Conference       Msgs   Description\n\026\001\017\031Ä\017  \031Ä\005  \031Ä\067\n";
   MessageList = "\026\001\013%-15.15s  \026\001\016%5ld  %.55s\n";
   MessageNotAvailable = "\n\026\001\014This Conference isn't available to you.\n";
   ReadWhichMail = "\n\x16\x01\x0BRead Message (F)rom you or (T)o you (? for help): ";
   MailFromYouKey = 'F';
   MailToYouKey = 'T';
   WhichMailHelpKey = '?';
   NoMessagesInMailBox = "\n\x16\x01\x0DSorry, there are no messages in your mailbox.\n";
   NoMessagesFromYou = "\n\x16\x01\x0DSorry, there are no messages from you in the database.\n";
   NoMessagesInArea = "\n\x16\x01\x0DSorry, there are no messages in this Conference.\n";
   StartMessageNumber = "\n\x16\x01\013Enter message number to start with, F for the\nfirst message, L for the last, or ? for help";
   StartNewMessages = ".\n(Just hit RETURN to start with new messages)";
   StartPrompt = ": ";
   FirstMessageKey = 'F';
   LastMessageKey = 'L';
   MailStartHelpKey = '?';
   MessageHdr = "\026\001\161%s * %s\026\007\n";
   MessageFrom = "\026\001\016From    : \026\001\003%-.35s\n";
   MessageTo = "\026\001\016To      : \026\001\003%-35.35s  \026\001\017Msg: #%lu, %d-%.3s-%d %2d:%02d\n";
   MessageSubject = "\026\001\016Subject : \026\001\003%-.69s\n";
   MessageFooter = "\026\001\037\026\007\n";
   ReadThisMessage = "\x16\x01\x0B(N)ext, (P)revious, (R)ead this message or e(X)it? ";
   ReadMessageKey = 'R';
   MessageText = "\x16\x01\x07%s\n";
   MessageQuote = "\x16\x01\x0E%s\n";
   MessageKludge = "\x16\x01\x1E%s\026\007\n";
   ReadMailMenu = "\n\x16\x01\013(R)eply, (E)rase, (F)orward, (C)opy, (P)revious, (N)ext or e(X)it: ";
   ReadMessageMenu = "\n\x16\x01\013(R)eply, (E)mail reply, (P)revious, (N)ext or e(X)it: ";
   ReplyKey = 'R';
   EraseKey = 'E';
   ForwardKey = 'F';
   CopyKey = 'C';
   PreviousMessageKey = 'P';
   NextMessageKey = 'N';
   ExitKey = 'X';
   EndOfMessages = "\n\x16\x01\x0DYou have reached the end of these messages.\n";
   StartOfMessages = "\n\x16\x01\x0DYou have reached the start of these messages.\n";
   EndOfMessagesMenu = "\n\x16\x01\013(P)revious message, (R)e-Read last message or e(X)it: ";
   RereadKey = 'R';

   // Logoff
   LogoffWarning = "\n\x16\x01\x0FYou are about to terminate this connection!\n";
   LogoffConfirm = "\n\x16\x01\013Are you sure (Y/N)? ";



   AvailableMsgAreas = "\014\026\001\017List of Available Message Areas:\n\n";
   AvailableFileAreas = "\014\026\001\017List of Available File Areas:\n\n";
   AvailableForums = "\n\n\026\001\012Forum           Description              Charge      \n\x16\x01\x02--------------- ------------------------ ------------\n";
   ForumNotAvailable = "\n\026\001\014This Forum isn't available to you.\n";
   PleaseAnswer2 = "\n\x16\x01\x0CPlease answer '%c' or '%c'.\n";
   PleaseAnswer3 = "\n\x16\x01\x0CPlease answer '%c', '%c' or '%c'.\n";

   ThreadMenu = "\n\x16\x01\013Thread (F)orward or (B)ackward (? for help): ";
   MessageToErase = "\n\x16\x01\013Enter the message number to be erased (%lu-%lu): ";
   ConfirmedErase = "\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%ld ERASED >>>\n";
   CantErase = "\n\x16\x01\x0DSorry, message number %ld is neither FROM you nor a PRIVATE message TO you, so you can't erase it.\n";
   EnterPreference = "\n\x16\x01\013Enter your preference: ";
   ModifyNotice = "\n\x16\x01\x0E(Note: Only the text portion of a message can be modified).\n";
   ModifyNumber = "\n\x16\x01\013Enter the message number to modify (1-%ld): ";
   MessageOutOfRange = "\n\x16\x01\x0DThe message #%ld is out of range.\n";
   CantModify = "\n\x16\x01\x0DSorry, you didn't write message #%ld, so you can't modify it.\n";
   EditorMenu = "EDITOR";

   WhoToSend = "\n\x16\x01\x0AWho do you wish to send this message to?\n\x16\x01\013Enter User-Name, ? for help, or RETURN for '\x16\x01\013All\x16\x01\013': ";
   NoSuchName = "\n\x16\x01\x0DSorry, no such User-Name exists...  maybe you mistyped it?\n";
   TypeMessageNow = "\n\x16\x01\012Type your message now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n";
   ContinueEntering = "\n\x16\x01\012Continue entering text. Type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself when you are\ndone. (Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n";
   InsertAfterLine = "\n\x16\x01\013Insert after which line (1-%d)? ";
   LineOutOfRange = "\n\x16\x01\x0DSorry, that line number is out of range!\n";
   ConfirmedCopy = "\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%-10ld COPIED TO #%-10ld >>>\n<<<         SENT TO: %-35.35s >>>\n";
   ConfirmedSend = "\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%ld WRITTEN TO DISK >>>\n";
   SendToAnother = "\n\x16\x01\012Do you want to send a copy of this message to anyone else (Y/N)? ";
   SendToWho = "\n\x16\x01\x0BWho do you wish to send a copy of this message (cc:) to?\n\x16\x01\013Enter User-Name or ? for help: ";
   TextList = "\x16\x01\x0E%3d: %s\n";
   ChangeLine = "\n\x16\x01\013Change text in what line (1-%d)? ";
   CurrentLineReads = "\n\x16\x01\012The current line reads:\n\n\x16\x01\x0E%3d: \x16\x01\013%s\n";
   TextToChange = "\n\x16\x01\013Change what text?\n: ";
   NewText = "\n\x16\x01\013Enter new text now (just RETURN to delete)\n: ";
   LineNowReads = "\n\x16\x01\012New line now reads:\n\n\x16\x01\016%3d: \x16\x01\013%s\n";
   EnterSubject = "\n\x16\x01\013Enter the subject of this message (%d chars.): ";
   DeleteLine = "\n\x16\x01\013Delete what line (1-%d)? ";
   OkToDelete = "\n\x16\x01\013Okay to delete this line (Y/N)? ";
   RetypeLine = "\n\x16\x01\013Retype what line (1-%d)? ";
   EnterNewLine = "\n\x16\x01\013Enter new line\n: ";
   EditorExited = "\n\x16\x01\x0E<<< EDITOR EXITED, MESSAGE NOT SAVED >>>\n";
   MailWhoToSend = "\n\x16\x01\x0AWho do you wish to send this message to?\n\x16\x01\013Enter User-Name, ? for help, or RETURN for '\x16\x01\013Sysop\x16\x01\013': ";
   ToAll = "All";

   ForumList = "\026\001\013%-15.15s  \026\001\016%5ld  %5ld  %s\n";
   ForumListHeader = "\n\026\001\012Forum            Msgs   Files  Description\n---------------  -----  -----  ------------------------------------------------\n";
   ForumNotAvailable = "\n\026\001\014This Forum isn't available to you.\n";
   ForumSelect = "\n\026\001\013Enter the name of new Forum, or ? for a list: \026\001\x1E";
   ListKey = "?";
}

USHORT TLanguage::Read (PSZ pszLanguage)
{
   FILE *fp;
   USHORT retVal = FALSE;
   ULONG totalSize;
   PSZ tempString, p, pBuffer, pString;
   PSZ *Dst;

   Translate = (PSZ)malloc (1024);

   if ((tempString = (PSZ)malloc (2048)) != NULL) {
      if ((fp = fopen (pszLanguage, "rt")) != NULL) {
         totalSize = 0L;
         while (fgets (tempString, 2047, fp) != NULL) {
            if ((p = strtok (tempString, "\"")) != NULL) {
               if ((p = strtok (NULL, "\"")) != NULL)
                  totalSize += strlen (p) + 1;
            }
         }

         Default ();
         if (Buffer != NULL)
            free (Buffer);

         if ((Buffer = (PSZ)malloc (totalSize)) != NULL) {
            retVal = TRUE;
            pBuffer = Buffer;
            rewind (fp);
            while (fgets (tempString, 2047, fp) != NULL) {
               if ((p = strtok (tempString, " =")) != NULL) {
                  if ((Dst = CheckStringItem (p)) != NULL) {
                     if ((p = strtok (NULL, "\"")) != NULL) {
                        if ((p = strtok (NULL, "\"")) != NULL) {
                           strcpy (pBuffer, TranslateIn (p));
                           *Dst = pBuffer;
                           pBuffer += strlen (p) + 1;
                        }
                     }
                  }
                  else if ((pString = CheckCharItem (p)) != NULL) {
                     if ((p = strtok (NULL, "\"")) != NULL) {
                        if ((p = strtok (NULL, "\"")) != NULL) {
                           strcpy (pBuffer, p);
                           *pString = *pBuffer;
                           pBuffer += strlen (p) + 1;
                        }
                     }
                  }
               }
            }
         }

         fclose (fp);
      }
      free (tempString);
   }

   if (Translate != NULL) {
      free (Translate);
      Translate = NULL;
   }

   return (retVal);
}

PSZ TLanguage::TranslateIn (PSZ pszText)
{
   UCHAR c, a;
   PSZ Src, Dst;

   if (Translate != NULL) {
      Src = pszText;
      Dst = Translate;
      while ((c = *Src++) != '\0') {
         if (c == '\\') {
            a = *Src++;
            if (a == '\\')
               *Dst++ = (CHAR)a;
            else if (a == 'n')
               *Dst++ = '\n';
            else if (a == 'r')
               *Dst++ = '\r';
            else if (a == 'a')
               *Dst++ = '\a';
            else {
               a -= 0x30;
               if (a > 9)
                  a -= 7;
               c = *Src++;
               c -= 0x30;
               if (c > 9)
                  c -= 7;
               c |= (UCHAR)(a << 4);
               *Dst++ = (CHAR)c;
            }
         }
         else
            *Dst++ = (CHAR)c;
      }
      *Dst = '\0';
      return (Translate);
   }
   else
      return (pszText);
}

PSZ TLanguage::TranslateOut (PSZ pszText)
{
   UCHAR c, a, Sequence;
   PSZ Src, Dst;

   if (Translate != NULL) {
      Src = pszText;
      Dst = Translate;
      Sequence = 0;
      while ((c = *Src++) != '\0') {
         if (c < 32 || c >= 127) {
            if (Sequence == 0 && (c == '\n' || c == '\r' || c == '\a')) {
               *Dst++ = '\\';
               if (c == '\n')
                  *Dst++ = 'n';
               else if (c == '\r')
                  *Dst++ = 'r';
               else if (c == '\a')
                  *Dst++ = 'a';
            }
            else {
               if (Sequence == 0 && c == '\x16')
                  Sequence++;
               else if (Sequence == 1 && c == '\x01')
                  Sequence++;
               else if (Sequence == 2)
                  Sequence++;
               *Dst++ = '\\';
               a = (UCHAR)((c & 0xF0) >> 4);
               a += 0x30;
               if (a > 0x39)
                  a += 7;
               *Dst++ = (CHAR)a;
               a = (UCHAR)(c & 0x0F);
               a += 0x30;
               if (a > 0x39)
                  a += 7;
               *Dst++ = (CHAR)a;
            }
         }
         else {
            Sequence = 0;
            if (c == '\\')
               *Dst++ = '\\';
            *Dst++ = (CHAR)c;
         }
         if (Sequence == 3)
            Sequence = 0;
      }
      *Dst = '\0';
      return (Translate);
   }
   else
      return (pszText);
}

USHORT TLanguage::Write (PSZ pszLanguage)
{
   FILE *fp;
   USHORT retVal = FALSE;

   if ((fp = fopen (pszLanguage, "wt")) != NULL) {
      retVal = TRUE;

      Translate = (PSZ)malloc (1024);

      fprintf (fp, "\n; Generic section\n");
      fprintf (fp, "Yes = \"%c\"\n", Yes);
      fprintf (fp, "No = \"%c\"\n", No);
      fprintf (fp, "Help = \"%c\"\n", Help);
      fprintf (fp, "DefYesNo = \"%s\"\n", TranslateOut (DefYesNo));
      fprintf (fp, "YesDefNo = \"%s\"\n", TranslateOut (YesDefNo));
      fprintf (fp, "DefYesNoHelp = \"%s\"\n", TranslateOut (DefYesNoHelp));
      fprintf (fp, "YesDefNoHelp = \"%s\"\n", TranslateOut (YesDefNoHelp));
      fprintf (fp, "MoreQuestion = \"%s\"\n", TranslateOut (MoreQuestion));
      fprintf (fp, "DeleteMoreQuestion = \"%s\"\n", TranslateOut (DeleteMoreQuestion));
      fprintf (fp, "NonStopKey = \"%c\"\n", NonStopKey);
      fprintf (fp, "QuitKey = \"%c\"\n", QuitKey);
      fprintf (fp, "ContinueKey = \"%c\"\n", ContinueKey);

      fprintf (fp, "\n; Login section\n");
      fprintf (fp, "AskAnsi = \"%s\"\n", TranslateOut (AskAnsi));
      fprintf (fp, "AskRealName = \"%s\"\n", TranslateOut (AskRealName));
      fprintf (fp, "AskSex = \"%s\"\n", TranslateOut (AskSex));
      fprintf (fp, "City = \"%s\"\n", TranslateOut (City));
      fprintf (fp, "Country = \"%s\"\n", TranslateOut (Country));
      fprintf (fp, "EnterCompanyName = \"%s\"\n", TranslateOut (EnterCompanyName));
      fprintf (fp, "NameNotFound = \"%s\"\n", TranslateOut (NameNotFound));
      fprintf (fp, "PhoneNumber = \"%s\"\n", TranslateOut (PhoneNumber));
      fprintf (fp, "PleaseReenter = \"%s\"\n", TranslateOut (PleaseReenter));
      fprintf (fp, "SelectPassword = \"%s\"\n", TranslateOut (SelectPassword));
      fprintf (fp, "StreetAddress = \"%s\"\n", TranslateOut (StreetAddress));
      fprintf (fp, "Wrong = \"%s\"\n", TranslateOut (Wrong));
      fprintf (fp, "WrongPassword = \"%s\"\n", TranslateOut (WrongPassword));

      fprintf (fp, "AvailableFileAreas = \"%s\"\n", TranslateOut (AvailableFileAreas));
      fprintf (fp, "AvailableForums = \"%s\"\n", TranslateOut (AvailableForums));
      fprintf (fp, "AvailableMsgAreas = \"%s\"\n", TranslateOut (AvailableMsgAreas));
      fprintf (fp, "EnterName = \"%s\"\n", TranslateOut (EnterName));
      fprintf (fp, "ForumNotAvailable = \"%s\"\n", TranslateOut (ForumNotAvailable));
      fprintf (fp, "Password = \"%s\"\n", TranslateOut (Password));
      fprintf (fp, "PleaseAnswer2 = \"%s\"\n", TranslateOut (PleaseAnswer2));
      fprintf (fp, "PleaseAnswer3 = \"%s\"\n", TranslateOut (PleaseAnswer3));

      fprintf (fp, "LogoffWarning = \"%s\"\n", TranslateOut (LogoffWarning));
      fprintf (fp, "LogoffConfirm = \"%s\"\n", TranslateOut (LogoffConfirm));

      fprintf (fp, "\n; Message section\n");
      fprintf (fp, "ReadMailMenu = \"%s\"\n", TranslateOut (ReadMailMenu));
      fprintf (fp, "ReadMessageMenu = \"%s\"\n", TranslateOut (ReadMessageMenu));
      fprintf (fp, "EndOfMessagesMenu = \"%s\"\n", TranslateOut (EndOfMessagesMenu));
      fprintf (fp, "ReadWhichMail = \"%s\"\n", TranslateOut (ReadWhichMail));
      fprintf (fp, "ThreadMenu = \"%s\"\n", TranslateOut (ThreadMenu));
      fprintf (fp, "MailFromYouKey = \"%c\"\n", MailFromYouKey);
      fprintf (fp, "MailToYouKey = \"%c\"\n", MailToYouKey);
      fprintf (fp, "WhichMailHelpKey = \"%c\"\n", WhichMailHelpKey);
      fprintf (fp, "NoMessagesInMailBox = \"%s\"\n", TranslateOut (NoMessagesInMailBox));
      fprintf (fp, "NoMessagesFromYou = \"%s\"\n", TranslateOut (NoMessagesFromYou));
      fprintf (fp, "StartMessageNumber = \"%s\"\n", TranslateOut (StartMessageNumber));
      fprintf (fp, "StartNewMessages = \"%s\"\n", TranslateOut (StartNewMessages));
      fprintf (fp, "StartPrompt = \"%s\"\n", TranslateOut (StartPrompt));
      fprintf (fp, "FirstMessageKey = \"%c\"\n", FirstMessageKey);
      fprintf (fp, "LastMessageKey = \"%c\"\n", LastMessageKey);
      fprintf (fp, "MailStartHelpKey = \"%c\"\n", MailStartHelpKey);
      fprintf (fp, "EndOfMessages = \"%s\"\n", TranslateOut (EndOfMessages));
      fprintf (fp, "StartOfMessages = \"%s\"\n", TranslateOut (StartOfMessages));
      fprintf (fp, "NextMessageKey = \"%c\"\n", NextMessageKey);
      fprintf (fp, "PreviousMessageKey = \"%c\"\n", PreviousMessageKey);
      fprintf (fp, "ExitKey = \"%c\"\n", ExitKey);
      fprintf (fp, "ReplyKey = \"%c\"\n", ReplyKey);
      fprintf (fp, "RereadKey = \"%c\"\n", RereadKey);
      fprintf (fp, "EraseKey = \"%c\"\n", EraseKey);
      fprintf (fp, "ForwardKey = \"%c\"\n", ForwardKey);
      fprintf (fp, "CopyKey = \"%c\"\n", CopyKey);
      fprintf (fp, "MessageHdr = \"%s\"\n", TranslateOut (MessageHdr));
      fprintf (fp, "MessageFrom = \"%s\"\n", TranslateOut (MessageFrom));
      fprintf (fp, "MessageTo = \"%s\"\n", TranslateOut (MessageTo));
      fprintf (fp, "MessageSubject = \"%s\"\n", TranslateOut (MessageSubject));
      fprintf (fp, "MessageText = \"%s\"\n", TranslateOut (MessageText));
      fprintf (fp, "MessageQuote = \"%s\"\n", TranslateOut (MessageQuote));
      fprintf (fp, "MessageKludge = \"%s\"\n", TranslateOut (MessageKludge));
      fprintf (fp, "ReadThisMessage = \"%s\"\n", TranslateOut (ReadThisMessage));
      fprintf (fp, "ReadMessageKey = \"%c\"\n", ReadMessageKey);
      fprintf (fp, "MessageToErase = \"%s\"\n", TranslateOut (MessageToErase));
      fprintf (fp, "ConfirmedErase = \"%s\"\n", TranslateOut (ConfirmedErase));
      fprintf (fp, "CantErase = \"%s\"\n", TranslateOut (CantErase));
      fprintf (fp, "EnterPreference = \"%s\"\n", TranslateOut (EnterPreference));
      fprintf (fp, "ModifyNotice = \"%s\"\n", TranslateOut (ModifyNotice));
      fprintf (fp, "ModifyNumber = \"%s\"\n", TranslateOut (ModifyNumber));
      fprintf (fp, "MessageOutOfRange = \"%s\"\n", TranslateOut (MessageOutOfRange));
      fprintf (fp, "CantModify = \"%s\"\n", TranslateOut (CantModify));
      fprintf (fp, "EditorMenu = \"%s\"\n", TranslateOut (EditorMenu));
      fprintf (fp, "MessageList = \"%s\"\n", TranslateOut (MessageList));
      fprintf (fp, "MessageListHeader = \"%s\"\n", TranslateOut (MessageListHeader));
      fprintf (fp, "MessageNotAvailable = \"%s\"\n", TranslateOut (MessageNotAvailable));
      fprintf (fp, "MessageSelect = \"%s\"\n", TranslateOut (MessageSelect));
      fprintf (fp, "NoMessagesInArea = \"%s\"\n", TranslateOut (NoMessagesInArea));

      fprintf (fp, "\n; Editor section\n");
      fprintf (fp, "WhoToSend = \"%s\"\n", TranslateOut (WhoToSend));
      fprintf (fp, "NoSuchName = \"%s\"\n", TranslateOut (NoSuchName));
      fprintf (fp, "TypeMessageNow = \"%s\"\n", TranslateOut (TypeMessageNow));
      fprintf (fp, "ContinueEntering = \"%s\"\n", TranslateOut (ContinueEntering));
      fprintf (fp, "InsertAfterLine = \"%s\"\n", TranslateOut (InsertAfterLine));
      fprintf (fp, "LineOutOfRange = \"%s\"\n", TranslateOut (LineOutOfRange));
      fprintf (fp, "ConfirmedCopy = \"%s\"\n", TranslateOut (ConfirmedCopy));
      fprintf (fp, "ConfirmedSend = \"%s\"\n", TranslateOut (ConfirmedSend));
      fprintf (fp, "SendToAnother = \"%s\"\n", TranslateOut (SendToAnother));
      fprintf (fp, "SendToWho = \"%s\"\n", TranslateOut (SendToWho));
      fprintf (fp, "TextList = \"%s\"\n", TranslateOut (TextList));
      fprintf (fp, "ChangeLine = \"%s\"\n", TranslateOut (ChangeLine));
      fprintf (fp, "CurrentLineReads = \"%s\"\n", TranslateOut (CurrentLineReads));
      fprintf (fp, "TextToChange = \"%s\"\n", TranslateOut (TextToChange));
      fprintf (fp, "NewText = \"%s\"\n", TranslateOut (NewText));
      fprintf (fp, "LineNowReads = \"%s\"\n", TranslateOut (LineNowReads));
      fprintf (fp, "EnterSubject = \"%s\"\n", TranslateOut (EnterSubject));
      fprintf (fp, "DeleteLine = \"%s\"\n", TranslateOut (DeleteLine));
      fprintf (fp, "OkToDelete = \"%s\"\n", TranslateOut (OkToDelete));
      fprintf (fp, "RetypeLine = \"%s\"\n", TranslateOut (RetypeLine));
      fprintf (fp, "EnterNewLine = \"%s\"\n", TranslateOut (EnterNewLine));
      fprintf (fp, "EditorExited = \"%s\"\n", TranslateOut (EditorExited));
      fprintf (fp, "MailWhoToSend = \"%s\"\n", TranslateOut (MailWhoToSend));
      fprintf (fp, "ToAll = \"%s\"\n", TranslateOut (ToAll));

      fprintf (fp, "ForumList = \"%s\"\n", TranslateOut (ForumList));
      fprintf (fp, "ForumListHeader = \"%s\"\n", TranslateOut (ForumListHeader));
      fprintf (fp, "ForumNotAvailable = \"%s\"\n", TranslateOut (ForumNotAvailable));
      fprintf (fp, "ForumSelect = \"%s\"\n", TranslateOut (ForumSelect));
      fprintf (fp, "ListKey = \"%c\"\n", ListKey);

      if (Translate != NULL) {
         free (Translate);
         Translate = NULL;
      }

      fclose (fp);
   }

   return (retVal);
}


