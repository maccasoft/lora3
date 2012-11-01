
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    09/02/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

typedef struct {
   USHORT Id;
   PSZ    Text;
} KEYWORDS;

static KEYWORDS Keywords[] = {
   { LNG_LANGUAGE_NAME, "LanguageName" },
   { LNG_MENUNAME, "MenuName" },
   { LNG_TEXTFILES, "TextFiles" },
   { LNG_MENUPATH, "MenuPath" },
   { LNG_YES, "Yes" },
   { LNG_NO, "No" },
   { LNG_NONE, "None" },
   { LNG_MALE, "Male" },
   { LNG_FEMALE, "Female" },
   { LNG_YESKEY, "YesKey" },
   { LNG_NOKEY, "NoKey" },
   { LNG_HELPKEY, "HelpKey" },
   { LNG_MALEKEY, "MaleKey" },
   { LNG_FEMALEKEY, "FemaleKey" },
   { LNG_JANUARY, "January" },
   { LNG_FEBRUARY, "February" },
   { LNG_MARCH, "March" },
   { LNG_APRIL, "April" },
   { LNG_MAY, "May" },
   { LNG_JUNI, "Juni" },
   { LNG_JULY, "July" },
   { LNG_AUGUST, "August" },
   { LNG_SEPTEMBER, "September" },
   { LNG_OCTOBER, "October" },
   { LNG_NOVEMBER, "November" },
   { LNG_DECEMBER, "December" },
   { LNG_MOREQUESTION, "MoreQuestion" },
   { LNG_DELETEMOREQUESTION, "DeleteMoreQuestion" },
   { LNG_NONSTOP, "NonStop" },
   { LNG_QUIT, "Quit" },
   { LNG_CONTINUE, "Continue" },
   { LNG_PRESSENTER, "PressEnter" },
   { LNG_DEFYESNO, "DefYesNo" },
   { LNG_YESDEFNO, "YesDefNo" },
   { LNG_DEFYESNOHELP, "DefYesNoHelp" },
   { LNG_YESDEFNOHELP, "YesDefNoHelp" },
   { LNG_ENTERNAME, "EnterName" },
   { LNG_NAMENOTFOUND, "NameNotFound" },
   { LNG_CONTINUEASNEW, "ContinueAsNew" },
   { LNG_READERFROM, "ReaderFrom" },
   { LNG_READERTO, "ReaderTo" },
   { LNG_READERSUBJECT, "ReaderSubject" },
   { LNG_READERFILE, "ReaderFile" },
   { LNG_MESSAGETEXT, "MessageText" },
   { LNG_MESSAGEQUOTE, "MessageQuote" },
   { LNG_MESSAGEKLUDGE, "MessageKludge" },
   { LNG_MESSAGEORIGIN, "MessageOrigin" },
   { LNG_MESSAGEDATE, "MessageDate" },
   { LNG_MESSAGEISREPLY, "MessageIsReply" },
   { LNG_MESSAGESEEALSO, "MessageSeeAlso" },
   { LNG_MESSAGEISBOTH, "MessageIsBoth" },
   { LNG_MESSAGEHDR, "MessageHdr" },
   { LNG_MESSAGENUMBER, "MessageNumber" },
   { LNG_MESSAGENUMBER1, "MessageNumber1" },
   { LNG_MESSAGENUMBER2, "MessageNumber2" },
   { LNG_MESSAGENUMBER3, "MessageNumber3" },
   { LNG_MESSAGEFLAGS, "MessageFlags" },
   { LNG_MESSAGEFROM, "MessageFrom" },
   { LNG_MESSAGETO, "MessageTo" },
   { LNG_MESSAGESUBJECT, "MessageSubject" },
   { LNG_MESSAGEFILE, "MessageFile" },
   { LNG_ASKANSI, "AskAnsi" },
   { LNG_ASKAVATAR, "AskAvatar" },
   { LNG_ASKCOLOR, "AskColor" },
   { LNG_ASKFULLSCREEN, "AskFullScreen" },
   { LNG_ASKALIAS, "AskAlias" },
   { LNG_ASKHOTKEY, "AskHotkey" },
   { LNG_ASKIBMCHARS, "AskIBMChars" },
   { LNG_ASKLINES, "AskLines" },
   { LNG_ASKPAUSE, "AskPause" },
   { LNG_ASKSCREENCLEAR, "AskScreenClear" },
   { LNG_ASKBIRTHDATE, "AskBirthDate" },
   { LNG_ASKMAILCHECK, "AskMailCheck" },
   { LNG_ASKFILECHECK, "AskFileCheck" },
   { LNG_DISCONNECT, "Disconnect" },
   { LNG_CURRENTPASSWORD, "CurrentPassword" },
   { LNG_WHYPASSWORD, "WhyPassword" },
   { LNG_INVALIDPASSWORD, "InvalidPassword" },
   { LNG_WRONGPASSWORD, "WrongPassword" },
   { LNG_MESSAGEAREAREQUEST, "MessageAreaRequest" },
   { LNG_MESSAGEAREAHEADER, "MessageAreaHeader" },
   { LNG_MESSAGEAREACURSOR, "MessageAreaCursor" },
   { LNG_MESSAGEAREAKEY, "MessageAreaKey" },
   { LNG_MESSAGEAREASEPARATOR, "MessageAreaSeparator" },
   { LNG_MESSAGEAREADESCRIPTION1, "MessageAreaDescription1" },
   { LNG_MESSAGEAREADESCRIPTION2, "MessageAreaDescription2" },
   { LNG_MESSAGEAREALIST, "MessageAreaList" },
   { LNG_FILEPROTOCOLLIST, "FileProtocolList" },
   { LNG_FILETAGGEDWARNING, "FileTaggedWarning" },
   { LNG_FILENOBYTESWARNING, "FileBytesWarning" },
   { LNG_FILENOTIMEWARNING, "FileNoTimeWarning" },
   { LNG_FILEBEGINDOWNLOAD, "FileBeginDownload" },
   { LNG_FILEBEGINDOWNLOAD2, "FileBeginDownload2" },
   { LNG_FILETAGGEDHEADER, "FileTaggedHeader" },
   { LNG_FILETAGGEDLIST, "FileTaggedList" },
   { LNG_FILETAGGEDTOTAL, "FileTaggedTotal" },
   { LNG_FILEDOWNLOADERROR, "FileDownloadError" },
   { LNG_FILEDOWNLOADCOMPLETE, "FileDownloadComplete" },
   { LNG_FILEBUILDLIST, "FileBuildList" },
   { LNG_FILENOTAGGED, "FileNoTagged" },
   { LNG_FILETAGLISTED, "FileTagListed" },
   { LNG_FILELISTTAGCONFIRM, "FileListTagConfirm" },
   { LNG_FILELISTNOTFOUND, "FileListNotFound" },
   { LNG_FILELISTHEADER, "FileListHeader" },
   { LNG_FILELISTSEPARATOR, "FileListSeparator" },
   { LNG_FILELISTDESCRIPTION1, "FileListDescription1" },
   { LNG_FILELISTDESCRIPTION2, "FileListDescription2" },
   { LNG_FILELISTTAGGED, "FileListTagged" },
   { LNG_FILELISTNORMAL, "FileListNormal" },
   { LNG_FILELISTMOREQUESTION, "FileListMoreQuestion" },
   { LNG_FILELISTDELETEMOREQUESTION, "FileListDeleteMoreQuestion" },
   { LNG_FILELISTTAGKEY, "FileListTagKey" },
   { LNG_FILELISTNOFILESFOUND, "FileListNoFilesFound" },
   { LNG_FILELISTCOMMENT, "FileListComment" },
   { LNG_FILEAREAREQUEST, "FileAreaRequest" },
   { LNG_FILEAREAHEADER, "FileAreaHeader" },
   { LNG_FILEAREASEPARATOR, "FileAreaSeparator" },
   { LNG_FILEAREADESCRIPTION1, "FileAreaDescription1" },
   { LNG_FILEAREADESCRIPTION2, "FileAreaDescription2" },
   { LNG_FILEAREACURSOR, "FileAreaCursor" },
   { LNG_FILEAREAKEY, "FileAreaKey" },
   { LNG_FILEAREALIST, "FileAreaList" },
   { LNG_FILEAREANOTAVAILABLE, "FileAreaNotAvailable" },
   { LNG_FILENAMETODELETE, "FileNameToDelete" },
   { LNG_FILEDELETED, "FileDeleted" },
   { LNG_FILETODETAG, "FileToDetag" },
   { LNG_FILEDETAGGED, "FileDetagged" },
   { LNG_FILETAGEMPTY, "FileTagEmpty" },
   { LNG_FILETOTAG, "FileToTag" },
   { LNG_FILETAGCONFIRM, "FileTagConfirm" },
   { LNG_FILENOTFOUND, "FileNotFound" },
   { LNG_REENTERPASSWORD, "ReenterPassword" },


   { LNG_ASKADDRESS, "AskAddress" },
   { LNG_ASKCITY, "AskCity" },
   { LNG_ASKCOMPANYNAME, "AskCompanyName" },
   { LNG_ASKDAYPHONE, "AskDayPhone" },
   { LNG_ASKPASSWORD, "AskPassword" },
   { LNG_ASKSEX, "AskSex" },
   { LNG_ENTERNAMEORNEW, "EnterNameOrNew" },
   { LNG_ENTERPASSWORD, "EnterPassword" },
   { LNG_HAVETAGGED, "HaveTagged" },
   { LNG_YOUSURE, "YouSure" },
   { LNG_USERFROMCITY, "UserFromCity" },
   { LNG_MENUERROR, "MenuError" },
   { LNG_MSGFLAG_RCV, "MessageFlagRcv" },
   { LNG_MSGFLAG_SNT, "MessageFlagSnt" },
   { LNG_MSGFLAG_PVT, "MessageFlagPvt" },
   { LNG_MSGFLAG_CRA, "MessageFlagCra" },
   { LNG_MSGFLAG_KS, "MessageFlagKS" },
   { LNG_MSGFLAG_LOC, "MessageFlagLoc" },
   { LNG_MSGFLAG_HLD, "MessageFlagHld" },
   { LNG_MSGFLAG_ATT, "MessageFlagAtt" },
   { LNG_MSGFLAG_FRQ, "MessageFlagFrq" },
   { LNG_MSGFLAG_TRS, "MessageFlagTrs" },
   { LNG_ENDOFMESSAGES, "EndOfMessages" },
   { LNG_READMENU, "ReadMenu" },
   { LNG_ENDREADMENU, "EndReadMenu" },
   { LNG_NEXTMESSAGE, "NextMessage" },
   { LNG_EXITREADMESSAGE, "ExitReadMessage" },
   { LNG_REREADMESSAGE, "RereadMessage" },
   { LNG_PREVIOUSMESSAGE, "PreviousMessage" },
   { LNG_REPLYMESSAGE, "ReplyMessage" },
   { LNG_EMAILREPLYMESSAGE, "EMailReplyMessage" },
   { LNG_CONFERENCENOTAVAILABLE, "ConferenceNotAvailable" },
   { LNG_STARTWITHMESSAGE, "StartWithMessage" },
   { LNG_NEWMESSAGES, "NewMessages" },
   { LNG_FORUMNAME, "ForumName" },
   { LNG_FORUMOPERATOR, "ForumOperator" },
   { LNG_FORUMTOPIC, "ForumTopic" },
   { LNG_FILENOTFOUNDINAREA, "FileNotFoundInArea" },
   { LNG_FILEDESCRIPTION, "FileDescription" },
   { LNG_FILEDOWNLOADNAME, "FileDownloadName" },
   { LNG_DOWNLOADFILENAME, "DownloadFileName" },
   { LNG_NOFILEHERE, "NoFileHere" },
   { LNG_DISPLAYWHICHFILE, "DisplayWhichFile" },
   { 0, NULL }
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

   while (Keywords[i].Text != NULL && RetVal == 0) {
      if (!stricmp (Keywords[i].Text, pszKey))
         RetVal = Keywords[i].Id;
      i++;
   }

   return (RetVal);
}

PSZ TLanguage::CopyString (USHORT Key, PSZ Arg, PSZ Text)
{
   USHORT DoCopy = FALSE;
   UCHAR c, c1;

   switch (Key) {
      case LNG_LANGUAGE_NAME:
         strcpy (Comment, Arg);
         break;
      case LNG_MENUPATH:
         strcpy (MenuPath, Arg);
         break;
      case LNG_MENUNAME:
         strcpy (MenuName, Arg);
         break;
      case LNG_TEXTFILES:
         strcpy (TextFiles, Arg);
         break;
      case LNG_YESKEY:
         Yes = *Arg;
         break;
      case LNG_NOKEY:
         No = *Arg;
         break;
      case LNG_HELPKEY:
         Help = *Arg;
         break;
      case LNG_MALEKEY:
         Male = *Arg;
         break;
      case LNG_FEMALEKEY:
         Female = *Arg;
         break;
      case LNG_JANUARY:
      case LNG_FEBRUARY:
      case LNG_MARCH:
      case LNG_APRIL:
      case LNG_MAY:
      case LNG_JUNI:
      case LNG_JULY:
      case LNG_AUGUST:
      case LNG_SEPTEMBER:
      case LNG_OCTOBER:
      case LNG_NOVEMBER:
      case LNG_DECEMBER:
         Months[Key - LNG_JANUARY] = Text;
         DoCopy = TRUE;
         break;
      default:
         Pointer[Key] = Text;
         DoCopy = TRUE;
         break;
   }

   if (DoCopy == TRUE) {
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
               *Text++ = (CHAR)c;
               Arg += 2;
            }
          }
         else if (*Arg == '\\') {
            Arg++;
            if (*Arg == 'a')
               *Text++ = '\a';
            else if (*Arg == 'n')
               *Text++ = '\n';
            else if (*Arg == 'r')
               *Text++ = '\r';
            else if (*Arg == 't')
               *Text++ = '\t';
            else if (isdigit (*Arg)) {
               if (*(Arg + 1) != '\0' && *(Arg + 2) != '\0') {
                  c = (UCHAR)((*Arg - '0') * 64);
                  c += (UCHAR)((*(Arg + 1) - '0') * 8);
                  c += (UCHAR)(*(Arg + 2) - '0');
                  *Text++ = (CHAR)c;
                  Arg += 2;
               }
            }
            Arg++;
         }
         else
            *Text++ = *Arg++;
      }
      *Text++ = *Arg++;
   }

   return (Text);
}

VOID TLanguage::Default (VOID)
{
   TextMemory = NULL;

#if !defined (__DOS__)
   strcpy (File, "default.lng");
   strcpy (Comment, "Default");
   TextFiles[0] = MenuName[0] = '\0';
   MenuPath[0] = '\0';

   Pointer[LNG_YES] = "YES";
   Pointer[LNG_NO] = "NO ";
   Pointer[LNG_NONE] = "None";

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

   Pointer[LNG_MOREQUESTION] = "\026\001\017More [Y,n,=]? \026\001\007";
   Pointer[LNG_DELETEMOREQUESTION] = "\r                \r";
   Pointer[LNG_NONSTOP] = "=";
   Pointer[LNG_QUIT] = "N";
   Pointer[LNG_CONTINUE] = "Y";

   Pointer[LNG_PRESSENTER] = "\026\001\017Press [Enter] to continue: ";

   Pointer[LNG_DEFYESNO] = " [Y,n]? \x16\x01\x1E";
   Pointer[LNG_YESDEFNO] = " [y,N]? \x16\x01\x1E";
   Pointer[LNG_DEFYESNOHELP] = " [Y,n,?=help]? \x16\x01\x1E";
   Pointer[LNG_YESDEFNOHELP] = " [y,N,?=help]? \x16\x01\x1E";

   // Login
   Pointer[LNG_ENTERNAME] = "\n\026\001\012Please enter your full name: \026\001\x1E";
   Pointer[LNG_USERFROMCITY] = "\n\x16\x01\x0E%s from %s";
   Pointer[LNG_ENTERPASSWORD] = "\n\026\001\012Password: \026\001\x1E";
   Pointer[LNG_INVALIDPASSWORD] = "\n\026\001\014Invalid name or password!\a\n";
   Pointer[LNG_NAMENOTFOUND] = "\n\026\001\017Your name was not found in the system user file.\n\n\026\001\016Name entered: %s\n";
   Pointer[LNG_CONTINUEASNEW] = "\n\026\001\017Do you want to continue as a new user";
   Pointer[LNG_ASKCITY] = "\n\026\001\012Please enter your location: \026\001\x1E";
   Pointer[LNG_ASKDAYPHONE] = "\n\026\001\012Write your home/voice number: \026\001\x1E";
   Pointer[LNG_ASKPASSWORD] = "\n\x16\x01\012Select your password to use [minimum 4 chars]: \026\001\x1E";
   Pointer[LNG_REENTERPASSWORD] = "\026\001\012Reenter your password for verify: \026\001\x1E";
   Pointer[LNG_PASSWORDNOMATCH] = "\026\001\015\nPasswords do not match. Try again.\n\006\007";
   Pointer[LNG_ASKANSI] = "\n\026\001\017Would you like ANSI graphics";
   Pointer[LNG_ASKAVATAR] = "\n\026\001\017Would you like AVATAR graphics";
   Pointer[LNG_ASKCOLOR] = "\n\026\001\012Do you want color codes to be sent";
   Pointer[LNG_ASKFULLSCREEN] = "\n\026\001\012Do you want to use the full-screen enhancements";
   Pointer[LNG_ASKALIAS] = "\n\026\001\012Choose your alias to use: \026\001\x1E";
   Pointer[LNG_ASKHOTKEY] = "\n\026\001\013Would you like to use hotkeys";
   Pointer[LNG_ASKIBMCHARS] = "\n\026\001\017Use IBM-PC characters";
   Pointer[LNG_ASKLINES] = "\n\026\001\016How many lines does your display have (10-66, 24 recommended)? \026\001\x1E";
   Pointer[LNG_ASKPAUSE] = "\n\026\001\016Do you want to pause after each screen page";
   Pointer[LNG_ASKSCREENCLEAR] = "\n\026\001\015Do you want screen clearing codes to be sent";
   Pointer[LNG_ASKBIRTHDATE] = "\n\026\001\014Write your date of birth (mm-dd-yyyy): \026\001\x1E";
   Pointer[LNG_ASKMAILCHECK] = "\n\026\001\013Would you like to check for mail at every logon";
   Pointer[LNG_ASKFILECHECK] = "\n\026\001\016Would you like to check for new files at every logon";

   // Logout
   Pointer[LNG_DISCONNECT] = "\n\026\001\017Disconnect";

   // Message areas
   Pointer[LNG_MESSAGEAREAREQUEST] = "\n\026\001\017Message area [Area, \"[\"=Prior, \"]\"=Next, \"?\"=List]: \026\001\x1E";
   Pointer[LNG_MESSAGEAREAHEADER] = "\n\026\001\017Area             Msgs   Description\n\031Ä\017  \031Ä\005  \031Ä\067\n";
   Pointer[LNG_MESSAGEAREASEPARATOR] = "\026\001\017\031Ä\017  \031Ä\005  \031Ä\067\n";
   Pointer[LNG_MESSAGEAREADESCRIPTION1] = "\026\001\016Use your arrow keys or CTRL-X / CTRL-E to hilight an area, RETURN selects it.\n";
   Pointer[LNG_MESSAGEAREADESCRIPTION2] = "\026\001\016Hit CTRL-V for next page, CTRL-Y for previous page, or X to exit.";
   Pointer[LNG_MESSAGEAREACURSOR] = "\x16\x01\x70%-15.15s\x16\x01\x07";
   Pointer[LNG_MESSAGEAREAKEY] = "\026\001\015%-15.15s\x16\x01\x07";
   Pointer[LNG_MESSAGEAREALIST] = "\026\001\015%-15.15s  \026\001\002%5ld  \026\001\003%.55s\n";

   // Message reader
   Pointer[LNG_READERFROM] = "\026\001\003From:    \026\001\016%-36.36s \026\001\017%-.33s\n";
   Pointer[LNG_READERTO] = "\026\001\003To:      \026\001\016%-36.36s \026\001\012Msg #%lu, %-.23s\n";
   Pointer[LNG_READERFILE] = "\026\001\003File(s): \026\001\016%-.70s\n";
   Pointer[LNG_READERSUBJECT] = "\026\001\003Subject: \026\001\016%-.70s\n\n";
   Pointer[LNG_MESSAGEDATE] = "%D %C %Z %H:%M";
   Pointer[LNG_MESSAGEISREPLY] = "\n\026\001\017*** This is a reply to #%lu.\n";
   Pointer[LNG_MESSAGESEEALSO] = "\n\026\001\017*** See also #%lu.\n";
   Pointer[LNG_MESSAGEISBOTH] = "\n\026\001\017*** This is a reply to #%lu.  *** See also #%lu.\n";
   Pointer[LNG_MESSAGETEXT] = "\026\001\003%s\n";
   Pointer[LNG_MESSAGEQUOTE] = "\026\001\017%s\n";
   Pointer[LNG_MESSAGEKLUDGE] = "\026\001\003%s\n";
   Pointer[LNG_MESSAGEORIGIN] = "\026\001\002%s\n";

   // Fullscreen message reader
   Pointer[LNG_MESSAGEHDR] = "\x0C\x16\x01\x13Ä \x16\x01\x1E%s \x16\x01\x13\031Ä%c";
   Pointer[LNG_MESSAGENUMBER] = "%lu of %lu";
   Pointer[LNG_MESSAGENUMBER1] = "%lu of %lu -%lu";
   Pointer[LNG_MESSAGENUMBER2] = "%lu of %lu +%lu";
   Pointer[LNG_MESSAGENUMBER3] = "%lu of %lu -%lu +%lu";
   Pointer[LNG_MESSAGEFLAGS] = "\x16\x01\x1F   Msg#: \x16\x01\x1C%-35.35s \x16\x01\x1F%-.35s\x16\x07\n";
   Pointer[LNG_MESSAGEFROM] = "\x16\x01\x1F   From: \x16\x01\x1E%-35.35s \x16\x01\x1F%-16.16s \x16\x01\x1F%-18.18s";
   Pointer[LNG_MESSAGETO] = "\x16\x01\x1F     To: \x16\x01\x1E%-35.35s \x16\x01\x1F%-16.16s \x16\x01\x1F%-18.18s";
   Pointer[LNG_MESSAGESUBJECT] = "\x16\x01\x1FSubject: \x16\x01\x1E%s\x16\007\n";
   Pointer[LNG_MESSAGEFILE] = "\x16\x01\037File(s): \x16\x01\x1E%s\x16\007\n";

   // File areas
   Pointer[LNG_FILEAREAREQUEST] = "\n\026\001\017File area [Area, \"[\"=Prior, \"]\"=Next, \"?\"=List]: \026\001\x1E";
   Pointer[LNG_FILEAREAHEADER] = "\n\026\001\017Area             Files  Description\n\026\001\017\031Ä\017  \031Ä\005  \031Ä\067\n";
   Pointer[LNG_FILEAREASEPARATOR] = "\026\001\017\031Ä\017  \031Ä\005  \031Ä\067\n";
   Pointer[LNG_FILEAREADESCRIPTION1] = "\026\001\016Use your arrow keys or CTRL-X / CTRL-E to hilight an area, RETURN selects it.\n";
   Pointer[LNG_FILEAREADESCRIPTION2] = "\026\001\016Hit CTRL-V for next page, CTRL-Y for previous page, or X to exit.";
   Pointer[LNG_FILEAREACURSOR] = "\x16\x01\x70%-15.15s\x16\x01\x07";
   Pointer[LNG_FILEAREAKEY] = "\026\001\015%-15.15s\x16\x01\x07";
   Pointer[LNG_FILEAREALIST] = "\026\001\015%-15.15s  \026\001\002%5ld  \026\001\003%.55s\n";
   Pointer[LNG_FILEAREANOTAVAILABLE] = "\n\026\001\017That area doesn't exist\n";
   Pointer[LNG_FILEPROTOCOLLIST] = "  \x16\x01\013%s ... \x16\x01\016%s\n";
   Pointer[LNG_FILETAGGEDWARNING] = "\n\026\001\015You have %u files tagged for later download.\nDo you want to download these files now";
   Pointer[LNG_FILENOBYTESWARNING] = "\n\026\001\015You don't have enough bytes to download these files.\n\006\007\006\007";
   Pointer[LNG_FILENOTIMEWARNING] = "\n\026\001\015You don't have enough time to download these files.\n\006\007\006\007";
   Pointer[LNG_FILEBEGINDOWNLOAD] = "\x16\x01\012Beginning %s download of the file %s\n\n\x16\x01\012(Hit \x16\x01\013CTRL-X \x16\x01\012a few times to abort)\n";
   Pointer[LNG_FILEBEGINDOWNLOAD2] = "\x16\x01\012Beginning %s download of the file(s)\n\n\x16\x01\012(Hit \x16\x01\013CTRL-X \x16\x01\012a few times to abort)\n";
   Pointer[LNG_FILETAGGEDHEADER] = "\n\x16\x01\x0FYou have tagged the following files:\n\n";
   Pointer[LNG_FILETAGGEDLIST] = "\026\001\016(%d) \026\001\015%-12.12s \026\001\016in area \026\001\015%s \026\001\012(%02lu:%02lu, %lu bytes)\n";
   Pointer[LNG_FILETAGGEDTOTAL] = "\n\026\001\016Total: \026\001\012%lu bytes \026\001\016(%lu:%02lu)\n";
   Pointer[LNG_FILENOTAGGED] = "\n\026\001\014No files are tagged.\n";
   Pointer[LNG_FILEDOWNLOADERROR] = "\006\007\n\n\x16\x01\015*** ERROR DOWNLOADING FILES ***\n\006\007\006\007";
   Pointer[LNG_FILEDOWNLOADCOMPLETE] = "\006\007\n\n\x16\x01\016*** DOWNLOAD COMPLETE ***\n\006\007\006\007";
   Pointer[LNG_FILEBUILDLIST] = "\n\x16\x01\017Building the list of files. Please wait... ";

   Pointer[LNG_FILETAGLISTED] = "\r\026\007\x16\x01\017Filename(s) to tag (#%d): ";
   Pointer[LNG_FILELISTTAGCONFIRM] = "\r026\007\026\001\016(%d) \026\001\015%-12.12s \026\001\012(%02lu:%02lu, %lu bytes)  ";
   Pointer[LNG_FILELISTNOTFOUND] = "\r\026\007\026\001\014I see no \"%s\" here.  ";
   Pointer[LNG_FILELISTHEADER] = "\026\001\017 File Name    Size  Date     Description\n \031Ä\014 \031Ä\005 \031Ä\010 \031Ä\062\n";
   Pointer[LNG_FILELISTSEPARATOR] = " \031Ä\014 \031Ä\005 \031Ä\010 \031Ä\062\n";
   Pointer[LNG_FILELISTDESCRIPTION1] = "\026\001\016Use your arrow keys or CTRL-X / CTRL-E to hilight a file, RETURN view details.\n";
   Pointer[LNG_FILELISTDESCRIPTION2] = "\026\001\016Hit CTRL-V for next page, CTRL-Y for previous page, or X to exit.";
   Pointer[LNG_FILELISTTAGGED] = "\026\001\016*%-12.12s*\026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n";
   Pointer[LNG_FILELISTNORMAL] = "\026\001\016 %-12.12s \026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n";
   Pointer[LNG_FILELISTMOREQUESTION] = "\r\026\001\017More [Y,n,t,=]? \026\001\007";
   Pointer[LNG_FILELISTDELETEMOREQUESTION] = "\r                  \r";
   Pointer[LNG_FILELISTTAGKEY] = "T";
   Pointer[LNG_FILELISTNOFILESFOUND] = "\n\x16\x01\015Sorry, no files are found to match your search/list parameters.\n\006\007\006\007";
   Pointer[LNG_FILELISTCOMMENT] = "                             \026\001\003%.50s\n";
   Pointer[LNG_FILENAMETODELETE] = "\n\x16\x01\013Enter file name to delete, or RETURN to exit: ";
   Pointer[LNG_FILEDELETED] = "\n\x16\x01\x0D%s deleted from Library records...\n\006\007\006\007";
   Pointer[LNG_FILETODETAG] = "\n\026\001\017File number to detag [1-%d, or A)ll]: ";
   Pointer[LNG_FILEDETAGGED] = "\n\026\001\016File \026\001\015%s \026\001\016detagged.";
   Pointer[LNG_FILETAGEMPTY] = "\n\026\001\016File queue is now empty.\n";
   Pointer[LNG_FILETOTAG] = "\n\x16\x01\017Filename(s) to tag (#%d): ";
   Pointer[LNG_FILETAGCONFIRM] = "\026\001\016(%d) \026\001\015%-12.12s \026\001\012(%02lu:%02lu, %lu bytes)";
   Pointer[LNG_FILENOTFOUND] = "\n\026\007\026\001\014I see no \"%s\" here.\n";

   // Change options
   Pointer[LNG_CURRENTPASSWORD] = "\n\026\001\017Current password: ";
   Pointer[LNG_WHYPASSWORD] = "\nA password is a single word (no spaces).\nIt must be 4 to %d letters (or numbers) long.\n\n";
   Pointer[LNG_WRONGPASSWORD] = "\n\026\001\014Wrong password!\n";

   // Miscellaneous
   Pointer[LNG_MENUERROR] = "\n\x16\x01\x0DPlease select one of the choices presented.\n\006\007\006\007";





   Pointer[LNG_ASKADDRESS] = "\n\x16\x01\013Enter your street address or P.O. Box:\n\x16\x01\x1E";
   Pointer[LNG_ASKCOMPANYNAME] = "\n\x16\x01\013Now enter your company name, or just RETURN if none:\n\x16\x01\x1E";
   Pointer[LNG_ASKSEX] = "\n\x16\x01\013What is your sex? (M)ale or (F)emale? \x16\x01\x1E";
   Pointer[LNG_ENTERNAMEORNEW] = "\nŠPlease enter your full name (or \"New\"): ";

   Pointer[LNG_MSGFLAG_RCV] = "Rcv ";
   Pointer[LNG_MSGFLAG_SNT] = "Snt ";
   Pointer[LNG_MSGFLAG_PVT] = "Pvt ";
   Pointer[LNG_MSGFLAG_CRA] = "Cra ";
   Pointer[LNG_MSGFLAG_KS] = "K/s ";
   Pointer[LNG_MSGFLAG_LOC] = "Loc ";
   Pointer[LNG_MSGFLAG_HLD] = "Hld ";
   Pointer[LNG_MSGFLAG_ATT] = "Att ";
   Pointer[LNG_MSGFLAG_FRQ] = "Frq ";
   Pointer[LNG_MSGFLAG_TRS] = "Trs ";

   Pointer[LNG_ENDOFMESSAGES] = "\n\x16\x01\x0DYou have reached the end of these messages.\n\006\007\006\007";
   Pointer[LNG_READMENU] = "\n\x16\x01\013(R)eply, (E)mail reply, (P)revious, (N)ext or e(X)it: ";
   Pointer[LNG_ENDREADMENU] = "\n\x16\x01\013(P)revious message, (R)e-Read last message or e(X)it: ";
   Pointer[LNG_NEXTMESSAGE] = "N";
   Pointer[LNG_EXITREADMESSAGE] = "X";
   Pointer[LNG_PREVIOUSMESSAGE] = "P";
   Pointer[LNG_REREADMESSAGE] = "R";
   Pointer[LNG_REPLYMESSAGE] = "R";
   Pointer[LNG_EMAILREPLYMESSAGE] = "R";
   Pointer[LNG_CONFERENCENOTAVAILABLE] = "\n\026\001\014This Conference isn't available to you.\n";
   Pointer[LNG_STARTWITHMESSAGE] = "\n\x16\x01\013Enter message number to start with, F for the\nfirst message, L for the last, or ? for help";
   Pointer[LNG_NEWMESSAGES] = ".\n(Just hit RETURN to start with new messages)";

   Pointer[LNG_FORUMNAME] = "\014\026\001\012FORUM: %s\n";
   Pointer[LNG_FORUMOPERATOR] = "\026\001\012Forum-Op: %s\n";
   Pointer[LNG_FORUMTOPIC] = "\026\001\012Forum Topic: %s\n\n";


   Pointer[LNG_FILENOTFOUNDINAREA] = "\n\026\001\015File not found in this library\n\006\007\006\007";
   Pointer[LNG_FILEDESCRIPTION] = "\x16\x01\x0E %-12.12s %4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n";
   Pointer[LNG_FILEDOWNLOADNAME] = "\n\x16\x01\017File(s) to download (#%d): ";
   Pointer[LNG_DOWNLOADFILENAME] = "\026\001\016(%d) \026\001\015%-12.12s \026\001\012(%02lu:%02lu, %lu bytes)";
   Pointer[LNG_NOFILEHERE] = "\n\026\001\014I see no \"%s\" here.\n";
   Pointer[LNG_DISPLAYWHICHFILE] = "\n\x16\x01\017Display which file? ";


   Pointer[LNG_HAVETAGGED] = "\n\026\001\015You have %u tagged files.\n";
   Pointer[LNG_YOUSURE] = "\n\x16\x01\013Are you sure (Y/N)? ";
#endif
}

USHORT TLanguage::Load (PSZ pszFile)
{
   FILE *fp;
   USHORT RetVal = FALSE;
   CHAR *Temp, *Key, *Arg, *p, *Pointer;
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

PSZ TLanguage::Text (USHORT Id)
{
   PSZ RetVal = "";

   if (Id >= 1 && Id < LNG_MAX_ENTRIES)
      RetVal = Pointer[Id];

   return (RetVal);
}


