
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

// ----------------------------------------------------------------------

typedef struct {
   USHORT Task;
   CHAR   Status[32];
   CHAR   Name[48];
   CHAR   City[48];
   ULONG  Speed;
   USHORT NoDisturb;
} USERON;

class TUseron
{
public:
   TUseron (PSZ pszDataPath);
   ~TUseron ();

   USHORT Task;
   CHAR   Status[32];
   CHAR   Name[48];
   CHAR   City[48];
   ULONG  Speed;
   USHORT NoDisturb;

   VOID   Disable ();
   VOID   Enable ();
   USHORT First ();
   USHORT Next ();
   VOID   Read (USHORT nTask);
   VOID   Update ();

private:
   CHAR   DataFile[128];
   USERON User;
};

TUseron::TUseron (PSZ pszDataPath)
{
   strcpy (DataFile, pszDataPath);
   strcat (DataFile, "useron.dat");
   AdjustPath (strlwr (DataFile));
}

TUseron::~TUseron ()
{
}

VOID TUseron::Enable ()
{
   int fd;
   USHORT i;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      if (lseek (fd, (Task - 1) * sizeof (USERON), SEEK_SET) == -1) {
         lseek (fd, 0L, SEEK_SET);
         for (i = 0; i < Task; i++) {
            if (read (fd, &User, sizeof (USERON)) != sizeof (USERON)) {
               memset (&User, 0, sizeof (USERON));
               write (fd, &User, sizeof (USERON));
            }
         }
      }
      else
         read (fd, &User, sizeof (USERON));

      memset (&User, 0, sizeof (USERON));
      User.Task = Task;
      strcpy (User.Status, Status);
      strcpy (User.Name, Name);
      strcpy (User.City, City);
      User.Speed = Speed;
      User.NoDisturb = NoDisturb;

      lseek (fd, tell (fd) - sizeof (USERON), SEEK_SET);
      write (fd, &User, sizeof (USERON));

      close (fd);
   }
}

USHORT TUseron::First ()
{
   Task = 0;

   return (Next ());
}

VOID TUseron::Disable ()
{
   int fd;
   USHORT i;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      if (lseek (fd, (Task - 1) * sizeof (USERON), SEEK_SET) == -1) {
         lseek (fd, 0L, SEEK_SET);
         for (i = 0; i < Task; i++) {
            if (read (fd, &User, sizeof (USERON)) != sizeof (USERON)) {
               memset (&User, 0, sizeof (USERON));
               write (fd, &User, sizeof (USERON));
            }
         }
      }
      else
         read (fd, &User, sizeof (USERON));

      memset (&User, 0, sizeof (USERON));

      lseek (fd, tell (fd) - sizeof (USERON), SEEK_SET);
      write (fd, &User, sizeof (USERON));

      close (fd);
   }
}

USHORT TUseron::Next ()
{
   int fd;
   USHORT RetVal = FALSE;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      while (read (fd, &User, sizeof (USERON)) == sizeof (USERON)) {
         if (User.Task > Task) {
            RetVal = TRUE;
            break;
         }
      }
      close (fd);
   }

   if (RetVal == TRUE) {
      Task = User.Task;
      strcpy (Status, User.Status);
      strcpy (Name, User.Name);
      strcpy (City, User.City);
      Speed = User.Speed;
      NoDisturb = User.NoDisturb;
   }

   return (RetVal);
}

VOID TUseron::Update ()
{
   int fd;
   USHORT i;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      if (lseek (fd, (Task - 1) * sizeof (USERON), SEEK_SET) == -1) {
         lseek (fd, 0L, SEEK_SET);
         for (i = 0; i < Task; i++) {
            if (read (fd, &User, sizeof (USERON)) != sizeof (USERON)) {
               memset (&User, 0, sizeof (USERON));
               write (fd, &User, sizeof (USERON));
            }
         }
      }
      else
         read (fd, &User, sizeof (USERON));

      memset (&User, 0, sizeof (USERON));
      User.Task = Task;
      strcpy (User.Status, Status);
      strcpy (User.Name, Name);
      strcpy (User.City, City);
      User.Speed = Speed;
      User.NoDisturb = NoDisturb;

      lseek (fd, tell (fd) - sizeof (USERON), SEEK_SET);
      write (fd, &User, sizeof (USERON));

      close (fd);
   }
}

VOID TUseron::Read (USHORT nTask)
{
   int fd;
   USHORT i;

   if ((fd = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
      if (lseek (fd, (Task - 1) * sizeof (USERON), SEEK_SET) == -1) {
         lseek (fd, 0L, SEEK_SET);
         for (i = 0; i < Task; i++) {
            if (read (fd, &User, sizeof (USERON)) != sizeof (USERON)) {
               memset (&User, 0, sizeof (USERON));
               write (fd, &User, sizeof (USERON));
            }
         }
      }
      else
         read (fd, &User, sizeof (USERON));

      User.Task = nTask;
      strcpy (User.Status, Status);
      strcpy (User.Name, Name);
      strcpy (User.City, City);
      User.Speed = Speed;
      User.NoDisturb = NoDisturb;

      close (fd);
   }
}

// ----------------------------------------------------------------------

TBbs::TBbs ()
{
   StartCall = 0L;
   Com = Snoop = NULL;
   Log = NULL;
   Cfg = NULL;
   Progress = NULL;
   MailerStatus = NULL;
   Status = NULL;
   Remote = REMOTE_NONE;
   Speed = 57600L;

   Task = 1;
   AutoDetect = TRUE;
   FancyNames = TRUE;
   Logoff = FALSE;
   Local = FALSE;
   Stack.Clear ();
   TimeLimit = 0;

   Snoop = NULL;
   Menu = new TMenu;
   Language = new TLanguage;
   Embedded = new TEmbedded;
}

TBbs::~TBbs ()
{
   if (Embedded != NULL)
      delete Embedded;
   if (Language != NULL)
      delete Language;
   if (User != NULL)
      delete User;
   if (Menu != NULL)
      delete Menu;
   if (Snoop != NULL)
      delete Snoop;
}

VOID TBbs::DisableUseronRecord ()
{
   class TUseron *Useron;

   if ((Useron = new TUseron (Cfg->SystemPath)) != NULL) {
      Useron->Task = Task;
      Useron->Disable ();
      delete Useron;
   }
}

VOID TBbs::SetUseronRecord (PSZ pszStatus)
{
   class TUseron *Useron;

   if ((Useron = new TUseron (Cfg->SystemPath)) != NULL) {
      Useron->Task = Task;
      strcpy (Useron->Status, pszStatus);
      if (User != NULL && User->Name[0] != '\0') {
         strcpy (Useron->Name, User->Name);
         strcpy (Useron->City, User->City);
         Useron->NoDisturb = FALSE;
      }
      else
         Useron->NoDisturb = TRUE;
      Useron->Speed = Speed;
      Useron->Enable ();
      delete Useron;
   }
}

VOID TBbs::ToggleNoDisturb ()
{
   class TUseron *Useron;

   if ((Useron = new TUseron (Cfg->SystemPath)) != NULL) {
      Useron->Read (Task);
      Useron->NoDisturb = (Useron->NoDisturb == TRUE) ? FALSE : TRUE;
      Useron->Update ();
      delete Useron;
   }
}

VOID TBbs::SetBirthDate ()
{
   CHAR Temp[16], *p;

   do {
      Embedded->Printf (Language->Text (LNG_ASKBIRTHDATE));
      Embedded->Input (Temp, 10, INP_FIELD);
   } while (strlen (Temp) < 10 && Embedded->AbortSession () == FALSE);
   if ((p = strtok (Temp, "-")) != NULL) {
      User->BirthMonth = (UCHAR)atoi (p);
      if ((p = strtok (NULL, "-")) != NULL)
         User->BirthDay = (UCHAR)atoi (p);
      if ((p = strtok (NULL, "-")) != NULL)
         User->BirthYear = (USHORT)atoi (p);
   }
   CheckBirthday ();
}

VOID TBbs::ExecuteCommand (class TMenu *Menu)
{
   CHAR Temp[32], *p;

   switch (Menu->Command) {
      case MNU_APPENDMENU:
         if (Menu->Load (Menu->Argument, TRUE) == FALSE) {
            if (Log != NULL)
               Log->Write ("!Can't append menu %s", Menu->Argument);
         }
         break;
      case MNU_ONLINEUSERS: {
         class TUseron *Useron;

         if ((Useron = new TUseron (Cfg->SystemPath)) != NULL) {
            Embedded->BufferedPrintf (Language->Text (LNG_ONLINETITLE), Cfg->SystemName);
            Embedded->BufferedPrintf (Language->Text (LNG_ONLINEHEADER));
            if (Useron->First () == TRUE)
               do {
                  Embedded->BufferedPrintf (Language->Text (LNG_ONLINEENTRY), Useron->Name, Useron->Task, Useron->Speed, Useron->Status, Useron->City);
               } while (Useron->Next () == TRUE);
            Embedded->Printf ("\n");
            delete Useron;
         }
         break;
      }
      case MNU_CLEARSTACK:
         Stack.Clear ();
         break;
      case MNU_DISPLAY:
         Embedded->DisplayFile (Menu->Argument);
         break;
      case MNU_FILEDATELIST:
         SetUseronRecord ("Browsing files");
         if (Library != NULL)
            Library->ListRecentFiles ();
         break;
      case MNU_FILEDELETE:
         if (Library != NULL)
            Library->RemoveFiles ();
         break;
      case MNU_FILEDOWNLOAD:
      case MNU_FILEDOWNLOADANY:
         SetUseronRecord ("Download");
         if (Library != NULL) {
            if ((p = strstr (Menu->Argument, "/f=")) == NULL)
               p = strstr (Menu->Argument, "/F=");
            if (p != NULL) {
               p += 3;
               Library->DownloadFile (p, NULL, 0L);
            }
            else
               Library->Download (NULL, (Menu->Command == MNU_FILEDOWNLOADANY) ? TRUE : FALSE);
         }
         break;
      case MNU_FILEDOWNLOADLIST:
         if (Library != NULL)
            Library->DownloadList ();
         break;
      case MNU_FILEKEYWORDLIST:
         SetUseronRecord ("Browsing files");
         if (Library != NULL)
            Library->SearchKeyword ();
         break;
      case MNU_FILENAMELIST:
         SetUseronRecord ("Browsing files");
         if (Library != NULL)
            Library->ListFiles ();
         break;
      case MNU_FILENEWLIST:
         SetUseronRecord ("Browsing files");
         if (Library != NULL)
            Library->SearchNewFiles ();
         break;
      case MNU_FILESELECT:
         if (Library != NULL)
            Library->SelectArea (Menu->Argument);
         break;
      case MNU_ADDTAGGED:
         if (Library != NULL)
            Library->AddTagged ();
         break;
      case MNU_LISTTAGGED:
         if (Library != NULL)
            Library->ListTagged ();
         break;
      case MNU_DELETETAGGED:
         if (Library != NULL)
            Library->DeleteTagged ();
         break;
      case MNU_DELETEALLTAGGED:
         if (Library != NULL)
            Library->DeleteAllTagged ();
         break;
      case MNU_FILETEXTLIST:
         SetUseronRecord ("Browsing files");
         if (Library != NULL)
            Library->SearchText ();
         break;
      case MNU_FILEUPLOAD:
         SetUseronRecord ("Upload");
         if (Library != NULL)
            Library->Upload ();
         break;
      case MNU_FILEUPLOADUSER:
         SetUseronRecord ("Upload");
         if (Library != NULL)
            Library->UploadUser (Menu->Argument);
         break;
      case MNU_FILEDISPLAY:
         if (Library != NULL)
            Library->TypeFile ( );
         break;
      case MNU_FINGER: {
         class TInternet *Inet;

         SetUseronRecord ("Finger");
         if ((Inet = new TInternet) != NULL) {
            Inet->Cfg = Cfg;
            Inet->Com = Com;
            Inet->Snoop = Snoop;
            Inet->Log = Log;
            Inet->Embedded = Embedded;
            Inet->Finger (Menu->Argument);
            delete Inet;
         }
         break;
      }
      case MNU_FTP: {
         class TInternet *Inet;

         SetUseronRecord ("FTP");
         if ((Inet = new TInternet) != NULL) {
            Inet->Cfg = Cfg;
            Inet->Com = Com;
            Inet->Snoop = Snoop;
            Inet->Log = Log;
            Inet->Embedded = Embedded;
            Inet->User = User;
            Inet->FTP (Menu->Argument);
            delete Inet;
         }
         break;
      }
      case MNU_IRC: {
         class TInternet *Inet;

         SetUseronRecord ("IRC");
         if ((Inet = new TInternet) != NULL) {
            Inet->Cfg = Cfg;
            Inet->Com = Com;
            Inet->Snoop = Snoop;
            Inet->Log = Log;
            Inet->Embedded = Embedded;
            Inet->User = User;
            Inet->IRC (Menu->Argument);
            delete Inet;
         }
         break;
      }
      case MNU_CLEARGOSUB:
         Stack.Clear ();
         // Fall-through
      case MNU_GOSUB:
         if (Menu->Argument[0] != '\0') {
            Stack.Add (MenuName, (USHORT)(strlen (MenuName) + 1));
            strcpy (MenuName, Menu->Argument);
            Reload = TRUE;
         }
         break;
      case MNU_CLEARGOTO:
         Stack.Clear ();
         // Fall-through
      case MNU_GOTO:
         if (Menu->Argument[0] != '\0') {
            strcpy (MenuName, Menu->Argument);
            Reload = TRUE;
         }
         break;
      case MNU_INQUIREPERSONAL: {
         class TInquire *Inquire;

         if ((Inquire = new TInquire) != NULL) {
            Inquire->Cfg = Cfg;
            Inquire->Embedded = Embedded;
            Inquire->User = User;
            Inquire->Language = Language;
            Inquire->Current = Message->Current;
            Inquire->Log = Log;
            if (!stricmp (Menu->Argument, "/all"))
               Inquire->Type = TYPE_PERSONAL;
            else
               Inquire->Type = TYPE_PERSONALNEW;
            Inquire->Query ();
         }
         break;
      }
      case MNU_INQUIRETEXT: {
         class TInquire *Inquire;

         if ((Inquire = new TInquire) != NULL) {
            Inquire->Cfg = Cfg;
            Inquire->Embedded = Embedded;
            Inquire->User = User;
            Inquire->Language = Language;
            Inquire->Current = Message->Current;
            Inquire->Log = Log;
            Inquire->Type = TYPE_KEYWORD;
            Inquire->Query ();
         }
         break;
      }
      case MNU_LOGOFF: {
         USHORT Answer;

         SetUseronRecord ("Logoff");

         if (User != NULL) {
            if (User->FileTag != NULL) {
               if (User->FileTag->TotalFiles > 0)
                  Embedded->Printf (Language->Text(LNG_HAVETAGGED), User->FileTag->TotalFiles);
            }
         }

         do {
            Embedded->Printf (Language->Text (LNG_DISCONNECT));
            if ((Answer = Embedded->GetAnswer (ASK_DEFNO|ASK_HELP)) == ANSWER_YES)
               Logoff = TRUE;
            if (Answer == ANSWER_HELP)
               Embedded->DisplayFile ("why_hu");
         } while (Answer == ANSWER_HELP && Embedded->AbortSession () == FALSE);
         break;
      }
      case MNU_MAILCHECK:
         if (EMail != NULL)
            EMail->CheckUnread ();
         break;
      case MNU_MAILDELETE:
         if (EMail != NULL)
            EMail->Delete ();
         break;
      case MNU_MAILLIST:
         if (EMail != NULL)
            EMail->BriefList ();
         break;
      case MNU_MAILREAD:
         if (EMail != NULL)
            EMail->ReadMessages (TRUE);
         break;
      case MNU_MAILWRITELOCAL:
      case MNU_MAILWRITEFIDONET:
      case MNU_MAILWRITEINTERNET:
         if (EMail != NULL) {
            if (Menu->Command == MNU_MAILWRITELOCAL)
               EMail->Write (MAIL_LOCAL, Menu->Argument);
            else if (Menu->Command == MNU_MAILWRITEFIDONET)
               EMail->Write (MAIL_FIDONET, Menu->Argument);
            else if (Menu->Command == MNU_MAILWRITEINTERNET)
               EMail->Write (MAIL_INTERNET, Menu->Argument);
         }
         break;
      case MNU_MAILNEXT:
         SetUseronRecord ("Reading e-mail");
         if (EMail != NULL)
            EMail->ReadNext ();
         break;
      case MNU_MAILPREVIOUS:
         SetUseronRecord ("Reading e-mail");
         if (EMail != NULL)
            EMail->ReadPrevious ();
         break;
      case MNU_MAILNONSTOP:
         SetUseronRecord ("Reading e-mail");
         if (EMail != NULL)
            EMail->ReadNonStop ();
         break;
      case MNU_MAILINDIVIDUAL: {
         ULONG Number;

         SetUseronRecord ("Reading e-mail");
         Number = atol (Cmd);
         if (EMail != NULL)
            EMail->Read (Number);
         break;
      }
      case MNU_MAILREPLY:
         SetUseronRecord ("Message editor");
         if (EMail != NULL)
            EMail->Reply ();
         break;
      case MNU_MSGBRIEFLIST:
         if (Message != NULL)
            Message->BriefList ();
         break;
      case MNU_MSGDELETE:
         if (Message != NULL)
            Message->Delete ();
         break;
      case MNU_MSGFORWARD:
         SetUseronRecord ("Message editor");
         if (Message != NULL)
            Message->ReadNext ();
         break;
      case MNU_MSGBACKWARD:
         SetUseronRecord ("Reading messages");
         if (Message != NULL)
            Message->ReadPrevious ();
         break;
      case MNU_MSGREADREPLY:
         SetUseronRecord ("Reading messages");
         if (Message != NULL)
            Message->ReadReply ();
         break;
      case MNU_MSGREADORIGINAL:
         SetUseronRecord ("Reading messages");
         if (Message != NULL)
            Message->ReadOriginal ();
         break;
      case MNU_MSGINDIVIDUAL: {
         ULONG Number;

         SetUseronRecord ("Reading messages");
         Number = atol (Cmd);
         if (Message != NULL && Message->Msg != NULL)
            Message->Read (Message->Msg->MsgnToUid (Number));
         break;
      }
      case MNU_MSGLISTNEWAREAS:
         if (Message != NULL)
            Message->SelectNewArea (Menu->Argument);
         break;
      case MNU_MSGREAD:
         SetUseronRecord ("Reading messages");
         if (Message != NULL)
            Message->ReadMessages ();
         break;
      case MNU_MSGREADCURRENT:
         SetUseronRecord ("Reading messages");
         if (Message != NULL)
            Message->DisplayCurrent ();
         break;
      case MNU_MSGREPLY:
         SetUseronRecord ("Message editor");
         if (Message != NULL)
            Message->Reply ();
         break;
      case MNU_MSGSELECT:
         if (Message != NULL)
            Message->SelectArea (Menu->Argument);
         break;
      case MNU_MSGREADNONSTOP:
         SetUseronRecord ("Reading messages");
         if (Message != NULL)
            Message->ReadNonStop ();
         break;
      case MNU_MSGTITLELIST:
         if (Message != NULL)
            Message->TitleList ();
         break;
      case MNU_MSGWRITE:
         SetUseronRecord ("Message editor");
         if (Message != NULL)
            Message->Write ();
         break;
      case MNU_MSGUNRECEIVE:
         if (Message != NULL)
            Message->Unreceive ();
         break;
      case MNU_OLRDOWNLOADASCII:
      case MNU_OLRDOWNLOADBW:
      case MNU_OLRDOWNLOADPNT:
      case MNU_OLRDOWNLOADQWK: {
         class TOffline *Olr = NULL;

         SetUseronRecord ("Offline reader");
         if (Menu->Command == MNU_OLRDOWNLOADASCII)
            Olr = new TAscii;
         else if (Menu->Command == MNU_OLRDOWNLOADBW)
            Olr = new TBlueWave;
         else if (Menu->Command == MNU_OLRDOWNLOADPNT)
            Olr = new TPoint;
         else if (Menu->Command == MNU_OLRDOWNLOADQWK)
            Olr = new TQWK;

         if (Olr != NULL) {
            Olr->Cfg = Cfg;
            Olr->Embedded = Embedded;
            Olr->Log = Log;
            Olr->User = User;
            Olr->Language = Language;
            Olr->Progress = Progress;
            strcpy (Olr->Id, Cfg->OLRPacketName);
            Olr->Limit = Cfg->OLRMaxMessages;
            if (Olr->Prescan () == TRUE)
               Olr->Create ();
            delete Olr;
         }
         break;
      }
      case MNU_OLRRESTRICTDATE: {
         class TOffline *Olr;

         SetUseronRecord ("Offline reader");
         if ((Olr = new TOffline) != NULL) {
            Olr->Cfg = Cfg;
            Olr->Embedded = Embedded;
            Olr->User = User;
            Olr->Language = Language;
            Olr->RestrictDate ();
            delete Olr;
         }
         break;
      }
      case MNU_OLRREMOVEAREA: {
         class TOffline *Olr;

         SetUseronRecord ("Offline reader");
         if ((Olr = new TOffline) != NULL) {
            Olr->Cfg = Cfg;
            Olr->Embedded = Embedded;
            Olr->User = User;
            Olr->Language = Language;
            Olr->RemoveArea ();
            delete Olr;
         }
         break;
      }
      case MNU_OLRTAGAREA: {
         class TOffline *Olr;

         SetUseronRecord ("Offline reader");
         if ((Olr = new TOffline) != NULL) {
            Olr->Cfg = Cfg;
            Olr->Embedded = Embedded;
            Olr->User = User;
            Olr->Language = Language;
            Olr->ManageTagged ();
            delete Olr;
         }
         break;
      }
      case MNU_OLRUPLOAD: {
         class TOffline *Olr;

         SetUseronRecord ("Offline reader");
         if ((Olr = new TBlueWave) != NULL) {
            Olr->Cfg = Cfg;
            Olr->Embedded = Embedded;
            Olr->Log = Log;
            Olr->User = User;
            Olr->Language = Language;
            Olr->Progress = Progress;
            strcpy (Olr->Id, Cfg->OLRPacketName);
            Olr->Limit = Cfg->OLRMaxMessages;
            Olr->Upload ();
            Olr->FetchReply ();
            delete Olr;
         }
         if ((Olr = new TQWK) != NULL) {
            Olr->Cfg = Cfg;
            Olr->Embedded = Embedded;
            Olr->Log = Log;
            Olr->User = User;
            Olr->Language = Language;
            Olr->Progress = Progress;
            strcpy (Olr->Id, Cfg->OLRPacketName);
            Olr->Limit = Cfg->OLRMaxMessages;
            Olr->FetchReply ();
            delete Olr;
         }
         if ((Olr = new TPoint) != NULL) {
            Olr->Cfg = Cfg;
            Olr->Embedded = Embedded;
            Olr->Log = Log;
            Olr->User = User;
            Olr->Language = Language;
            Olr->Progress = Progress;
            strcpy (Olr->Id, Cfg->OLRPacketName);
            Olr->Limit = Cfg->OLRMaxMessages;
            Olr->FetchReply ();
            delete Olr;
         }
         break;
      }
      case MNU_OLRVIEWTAGGED: {
         class TOffline *Olr;

         SetUseronRecord ("Offline reader");
         if ((Olr = new TOffline) != NULL) {
            Olr->Cfg = Cfg;
            Olr->Embedded = Embedded;
            Olr->User = User;
            Olr->Language = Language;
            Olr->Display ();
            delete Olr;
         }
         break;
      }
      case MNU_PRESSENTER:
         Embedded->PressEnter ();
         break;
      case MNU_RETURN:
         if (Stack.Last () != NULL) {
            strcpy (MenuName, (PSZ)Stack.Value ());
            Reload = TRUE;
            Stack.Remove ();
         }
         break;
      case MNU_RETURNMAIN:
         Stack.Clear ();
         strcpy (MenuName, Cfg->MainMenu);
         Reload = TRUE;
         break;
      case MNU_RUNEXTERNAL:
         SetUseronRecord ("External door");
         Embedded->RunExternal (Menu->Argument);
         break;
      case MNU_SEARCHFILENAME:
         if (Library != NULL)
            Library->SearchFileName ();
         break;
      case MNU_SETCOMPANY:
         do {
            Embedded->Printf (Language->Text(LNG_ASKCOMPANYNAME));
            Embedded->Input (User->Company, (USHORT)(sizeof (User->Company) - 1), INP_FIELD);
         } while (User->Company[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskCompanyName == REQUIRED);
         break;
      case MNU_SETADDRESS:
         do {
            Embedded->Printf (Language->Text(LNG_ASKADDRESS));
            Embedded->Input (User->Address, (USHORT)(sizeof (User->Address) - 1), INP_FIELD);
         } while (User->Address[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskAddress == REQUIRED);
         break;
      case MNU_SETCITY:
         do {
            Embedded->Printf (Language->Text(LNG_ASKCITY));
            Embedded->Input (User->City, (USHORT)(sizeof (User->City) - 1), 0);
         } while (User->City[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskCity == REQUIRED);
         break;
      case MNU_SETPHONE:
         do {
            Embedded->Printf (Language->Text(LNG_ASKDAYPHONE));
            Embedded->Input (User->DayPhone, (USHORT)(sizeof (User->DayPhone) - 1), INP_FIELD);
         } while (User->DayPhone[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskPhoneNumber == REQUIRED);
         break;
      case MNU_SETBIRTHDATE:
         SetBirthDate ();
         break;
      case MNU_SETGENDER:
         do {
            Embedded->Printf (Language->Text(LNG_ASKSEX));
            Embedded->Input (Temp, 1, INP_FIELD);
            User->Sex = (UCHAR)toupper (Temp[0]);
         } while (User->Sex != Language->Male && User->Sex != Language->Female && Embedded->AbortSession () == FALSE && Cfg->AskGender == REQUIRED);

         if (User->Sex == Language->Female)
            User->Sex = 1;
         else if (User->Sex == Language->Male)
            User->Sex = 0;
         else
            User->Sex = 0;
         break;
      case MNU_SETLANGUAGE:
         if (Language->Load (Menu->Argument) == TRUE) {
            Log->Write (":Loaded language %s", Menu->Argument);
            strcpy (Menu->AltPath, Language->MenuPath);
            strcpy (Embedded->AltPath, Language->TextFiles);
            Reload = TRUE;
            if (Menu->AltPath[0] != '\0')
               Log->Write ("   Language Menu Path = %s", Menu->AltPath);
            if (Embedded->AltPath[0] != '\0')
               Log->Write ("   Language Text Files Path = %s", Embedded->AltPath);
            strcpy (User->Language, Menu->Argument);
         }
         else
            Log->Write ("!Failed to load language %s", Menu->Argument);
         break;
      case MNU_SETPASSWORD: {
         CHAR Temp[48];

         Embedded->Printf (Language->Text (LNG_CURRENTPASSWORD));
         Embedded->Input (Temp, (USHORT)(sizeof (Password) - 1), INP_PWD);

         if (User->CheckPassword (Temp) == TRUE) {
            if (Embedded->AbortSession () == FALSE)
               do {
                  Embedded->Printf (Language->Text (LNG_ASKPASSWORD));
                  Embedded->Input (Password, (USHORT)(sizeof (Password) - 1), INP_PWD);
                  if (strlen (Password) < 4)
                     Embedded->Printf (Language->Text (LNG_WHYPASSWORD), sizeof (Password) - 1);
               } while (strlen (Password) < 4 && Embedded->AbortSession () == FALSE);
            if (Embedded->AbortSession () == FALSE)
               do {
                  Embedded->Printf (Language->Text (LNG_REENTERPASSWORD));
                  Embedded->Input (Temp, (USHORT)(sizeof (Password) - 1), INP_FIELD|INP_PWD);
               } while (Temp[0] == '\0' && Embedded->AbortSession () == FALSE);
            if (Embedded->AbortSession () == FALSE) {
               if (stricmp (Temp, Password))
                  Embedded->Printf (Language->Text (LNG_PASSWORDNOMATCH));
               else {
                  User->SetPassword (Password);
                  User->Update ();
               }
            }
         }
         else {
            if (Log != NULL)
               Log->Write ("!Invalid current pwd \"%s\"", Temp);
            Embedded->Printf (Language->Text (LNG_WRONGPASSWORD));
         }
         break;
      }
      case MNU_SETSCREENLENGTH:
         do {
            Embedded->Printf (Language->Text (LNG_ASKLINES));
            Embedded->Input (Temp, 2, INP_FIELD);
         } while ((atoi (Temp) < 10 || atoi (Temp) > 66) && Embedded->AbortSession () == FALSE);
         User->ScreenHeight = (USHORT)atoi (Temp);
         break;
      case MNU_TELNET: {
         class TInternet *Inet;

         SetUseronRecord ("Telnet");
         if ((Inet = new TInternet) != NULL) {
            Inet->Cfg = Cfg;
            Inet->Com = Com;
            Inet->Snoop = Snoop;
            Inet->Log = Log;
            Inet->Embedded = Embedded;
            Inet->Telnet (Menu->Argument);
            delete Inet;
         }
         break;
      }
      case MNU_TOGGLEKLUDGES:
         User->Kludges = (User->Kludges == TRUE) ? FALSE : TRUE;
         Message->ShowKludges = User->Kludges;
         break;
      case MNU_TOGGLEFULLED:
         User->FullEd = (User->FullEd == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLEMAILCHECK:
         User->MailCheck = (User->MailCheck == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLEFILECHECK:
         User->NewFileCheck = (User->NewFileCheck == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLEFULLREAD:
         User->FullReader = (User->FullReader == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLENODISTURB:
         User->NoDisturb = (User->NoDisturb == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLEMOREPROMPT:
         User->MorePrompt = (User->MorePrompt == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLEFULLSCREEN:
         User->FullScreen = (User->FullScreen == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLEIBMCHARS:
         User->IBMChars = (User->IBMChars == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLERIP:
         Embedded->Rip = (Embedded->Rip == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLESCREENCLEAR:
         User->ScreenClear = (User->ScreenClear == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLEINUSERLIST:
         User->InUserList = (User->InUserList == TRUE) ? FALSE : TRUE;
         break;
      case MNU_TOGGLECOLOR:
         User->Color = (User->Color == TRUE) ? FALSE : TRUE;
         Embedded->Color = User->Color;
         if (User->Color == FALSE)
            Embedded->Printf ("\nColor codes will NOT be used.\n\006\007\006\007");
         else
            Embedded->Printf ("\n\026\001\015Color codes now active.\n\006\007\006\007");
         break;
      case MNU_TOGGLEANSI:
         User->Ansi = (User->Ansi == TRUE) ? FALSE : TRUE;
         if (User->Ansi == TRUE)
            User->Avatar = FALSE;
         if (User->Ansi == FALSE && User->Avatar == FALSE)
            User->Color = FALSE;
         Embedded->Ansi = User->Ansi;
         Embedded->Avatar = User->Avatar;
         Embedded->Color = User->Color;
         if (User->Ansi == FALSE)
            Embedded->Printf ("\nANSI Color/Graphics will NOT be used.\n\006\007\006\007");
         else {
            if (User->Color == TRUE)
               Embedded->Printf ("\n\026\001\015ANSI Color/Graphics now active.\n\006\007\006\007");
            else
               Embedded->Printf ("\nANSI Graphics now active.\n\006\007\006\007");
         }
         break;
      case MNU_TOGGLEAVATAR:
         User->Avatar = (User->Avatar == TRUE) ? FALSE : TRUE;
         if (User->Avatar == TRUE)
            User->Ansi = FALSE;
         if (User->Ansi == FALSE && User->Avatar == FALSE)
            User->Color = FALSE;
         Embedded->Ansi = User->Ansi;
         Embedded->Avatar = User->Avatar;
         Embedded->Color = User->Color;
         if (User->Avatar == FALSE)
            Embedded->Printf ("\nAVATAR Color/Graphics will NOT be used.\n\006\007\006\007");
         else {
            if (User->Color == TRUE)
               Embedded->Printf ("\n\026\001\015AVATAR Color/Graphics now active.\n\006\007\006\007");
            else
               Embedded->Printf ("\nAVATAR Graphics now active.\n\006\007\006\007");
         }
         break;
      case MNU_TOGGLEHOTKEY:
         User->HotKey = (User->HotKey == TRUE) ? FALSE : TRUE;
         Embedded->HotKey = User->HotKey;
         if (User->HotKey == FALSE)
            Embedded->Printf ("\nHot keys will NOT be used.\n\006\007\006\007");
         else
            Embedded->Printf ("\n\026\001\015Hot keys now active.\n\006\007\006\007");
         break;
      case MNU_SETVIDEOMODE:
         if (!stricmp (Menu->Argument, "ANSI")) {
            User->Avatar = FALSE;
            User->Ansi = User->Color = TRUE;
         }
         else if (!stricmp (Menu->Argument, "AVATAR")) {
            User->Ansi = FALSE;
            User->Avatar = User->Color = TRUE;
         }
         else if (!stricmp (Menu->Argument, "TTY"))
            User->Avatar = User->Ansi = User->Color = FALSE;
         Embedded->Ansi = User->Ansi;
         Embedded->Avatar = User->Avatar;
         Embedded->Color = User->Color;
         break;
      case MNU_VERSION: {
#if defined(__OS2__)
         CHAR Temp[16];
         ULONG Value[3];
#elif defined(__NT__)
         CHAR *Processor;
         OSVERSIONINFO OsInfo;
         SYSTEM_INFO SysInfo;
#elif defined(__BORLANDC__) || defined(_MSC_VER)
         union REGS inregs, outregs;
#endif

         Embedded->Printf ("\n�%s Professional Edition - Version %s\n", NAME, VERSION);
         Embedded->Printf ("Copyright (c) 1996-97 by Marco Maccaferri. All rights reserved.\n");
         Embedded->Printf ("\026\001\013BlueWave Offline Mail System. Copyright 1990-94 by Cutting Edge Computing\n\n");

         Embedded->Printf ("�Design and Development by Marco Maccaferri.\n\n");

         Embedded->Printf ("\026\001\014For technical support or for more informations on LoraBBS, send mail\nto macca@arci02.bo.cnr.it or \"Marco Maccaferri\" at 2:332/402.\n\n");

#if defined(__BORLANDC__)
         Embedded->Printf ("\x16\x01\x0A Version: Compiled on %s at %s (Borland C++ v%d.%02X)\n", __DATE__, __TIME__, __BCPLUSPLUS__ / 0x100, __BCPLUSPLUS__ % 0x100);
#elif defined(__LINUX__)
         Embedded->Printf ("\x16\x01\x0A Version: Compiled on %s at %s (GCC/32 v%s)\n", __DATE__, __TIME__, "2.7.2");
#else
         Embedded->Printf ("\x16\x01\x0A Version: Compiled on %s at %s (WATCOM C/32 v%d.%d)\n", __DATE__, __TIME__, __WATCOMC__ / 100, (__WATCOMC__ % 100) / 10);
#endif
#if !defined(__NT__)
         Embedded->Printf ("Computer: AT-class\n");
#endif

#if defined(__OS2__)
         DosQuerySysInfo (11, 13, (UCHAR *)Value, sizeof (Value));
         if (Value[0] == 20L && Value[1] >= 30L)
            Embedded->Printf ("      OS: OS/2 Warp v%lu.%lu\n", Value[1] / 10L, Value[1] % 10L);
         else
            Embedded->Printf ("      OS: OS/2 Version %lu.%lu\n", Value[0], Value[1]);

         Value[0] = 0L;
         DosQuerySysInfo (17, 17, (UCHAR *)Value, sizeof (Value));
         if (Value[0] >= 1000000L)
            sprintf (Temp, "%lu,%03lu,%03lu", Value[0] / 1000000L, (Value[0] % 1000000L) / 1000L, (Value[0] % 1000000L) % 1000L);
         else if (Value[0] >= 1000L)
            sprintf (Temp, "%lu,%03lu", Value[0] / 1000L, Value[0] % 1000L);
         else
            sprintf (Temp, "%lu", Value[0]);
         Embedded->Printf ("  Memory: %s bytes of physical memory\n", Temp);

         Value[0] = 0L;
         DosQuerySysInfo (20, 20, (UCHAR *)Value, sizeof (Value));
         if (Value[0] >= 1000000L)
            sprintf (Temp, "%lu,%03lu,%03lu", Value[0] / 1000000L, (Value[0] % 1000000L) / 1000L, (Value[0] % 1000000L) % 1000L);
         else if (Value[0] >= 1000L)
            sprintf (Temp, "%lu,%03lu", Value[0] / 1000L, Value[0] % 1000L);
         else
            sprintf (Temp, "%lu", Value[0]);
         Embedded->Printf ("          %s bytes remain in heap\n\n\026\001\017", Temp);
#elif defined(__NT__)
         OsInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
         GetVersionEx (&OsInfo);
         GetSystemInfo (&SysInfo);

         Processor = "Unknown";
         switch (SysInfo.dwProcessorType) {
            case PROCESSOR_INTEL_386:
               Processor = "i386";
               break;
            case PROCESSOR_INTEL_486:
               Processor = "i486";
               break;
            case PROCESSOR_INTEL_PENTIUM:
               Processor = "Pentium(r)";
               break;
            case PROCESSOR_MIPS_R4000:
               Processor = "MIPS R4000";
               break;
            case PROCESSOR_ALPHA_21064:
               Processor = "Alpha 21064";
               break;
         }
         switch (SysInfo.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_INTEL:
               if (SysInfo.wProcessorLevel == 3)
                  Processor = "i386";
               else if (SysInfo.wProcessorLevel == 4)
                  Processor = "i486";
               else if (SysInfo.wProcessorLevel == 5)
                  Processor = "Pentium(r)";
               break;
            case PROCESSOR_ARCHITECTURE_MIPS:
               if (SysInfo.wProcessorLevel == 0x0004)
                  Processor = "MIPS R4000";
               break;
            case PROCESSOR_ARCHITECTURE_ALPHA:
               if (SysInfo.wProcessorLevel == 21064)
                  Processor = "Alpha 21046";
               else if (SysInfo.wProcessorLevel == 21066)
                  Processor = "Alpha 21066";
               else if (SysInfo.wProcessorLevel == 21164)
                  Processor = "Alpha 21164";
               break;
            case PROCESSOR_ARCHITECTURE_PPC:
               if (SysInfo.wProcessorLevel == 1)
                  Processor = "PPC 601";
               else if (SysInfo.wProcessorLevel == 3)
                  Processor = "PPC 603";
               else if (SysInfo.wProcessorLevel == 4)
                  Processor = "PPC 604";
               else if (SysInfo.wProcessorLevel == 6)
                  Processor = "PPC 603+";
               else if (SysInfo.wProcessorLevel == 9)
                  Processor = "PPC 604+";
               else if (SysInfo.wProcessorLevel == 20)
                  Processor = "PPC 620";
               break;
         }
         Embedded->Printf ("Computer: %s (%d processors detected)\n", Processor, SysInfo.dwNumberOfProcessors);

         if (OsInfo.dwPlatformId == VER_PLATFORM_WIN32s)
            Embedded->Printf ("      OS: Windows %ld.%02ld w/Win32s\n", OsInfo.dwMajorVersion, OsInfo.dwMinorVersion);
         else if (OsInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
            Embedded->Printf ("      OS: Windows 95 %ld.%02ld (build %d)\n", OsInfo.dwMajorVersion, OsInfo.dwMinorVersion, LOWORD (OsInfo.dwBuildNumber));
         else if (OsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
            Embedded->Printf ("      OS: Windows NT %ld.%02ld (build %d)\n", OsInfo.dwMajorVersion, OsInfo.dwMinorVersion, LOWORD (OsInfo.dwBuildNumber));
         else
            Embedded->Printf ("      OS: Windows 95/NT\n");

         Embedded->Printf ("  Memory: %lu bytes remain in heap\n\n\026\001\017", GetFreeSpace (0));
#elif defined(__LINUX__)
         Embedded->Printf ("      OS: Linux (ELF)\n\n");
#elif defined(__BORLANDC__) || defined(_MSC_VER)
         inregs.h.ah = 0x30;
         intdos (&inregs, &outregs);
         if (outregs.h.al >= 20)
            Embedded->Printf ("      OS: OS/2 %d.%02d DOS Box (16 bit)\n", outregs.h.ah / 10, outregs.h.ah % 10);
         else
            Embedded->Printf ("      OS: DOS %d.%02d (16 bit)\n", outregs.h.al, outregs.h.ah);
         Embedded->Printf ("  Memory: %lu bytes remain in heap\n\n\026\001\017", coreleft ());
#else
         Embedded->Printf ("      OS: DOS/32\n\n");
#endif

         Embedded->DisplayFile ("version");
         break;
      }
   }
}

USHORT TBbs::FileExist (PSZ FileName)
{
   USHORT RetVal = FALSE;
   struct stat statbuf;

   if (stat (FileName, &statbuf) == 0)
      RetVal = TRUE;

   return (RetVal);
}

USHORT TBbs::Login ()
{
   USHORT RetVal = FALSE, Counter, Flags, Answer;
   CHAR Temp[48], *p;

   Counter = 0;
   Flags = (USHORT)((FancyNames == TRUE) ? INP_FANCY|INP_FIELD : INP_FIELD);

   while (RetVal == FALSE && Embedded->AbortSession () == FALSE && Counter < 3) {
      if (Cfg->LoginType != 0) {
         Embedded->Printf (Language->Text(LNG_ENTERNAMEORNEW));
         Embedded->Input (Name, (USHORT)(sizeof (Name) - 1), Flags);
         if (Embedded->AbortSession () == TRUE)
            break;
         if (stricmp (Name, "New")) {
            Log->Write ("+%s is calling", Name);
            if (Status != NULL)
               Status->SetLine (0, "%s", Name);
         }
      }
      else {
         do {
            Embedded->Printf (Language->Text(LNG_ENTERNAME));
            Embedded->Input (Name, (USHORT)(sizeof (Name) - 1), Flags);
         } while (strlen (Name) < 3 && Embedded->AbortSession () == FALSE);
         if (Embedded->AbortSession () == FALSE) {
            Log->Write ("+%s is calling", Name);
            if (Status != NULL)
               Status->SetLine (0, "%s", Name);
         }
      }

      if (Snoop != NULL)
         Snoop->SetName (Name);
      else if (Com != NULL)
         Com->SetName (Name);

      RetVal = FALSE;
      if (Embedded->AbortSession () == FALSE && (Cfg->LoginType != 0 && !stricmp (Name, "New")) || (Cfg->LoginType == 0 && User->GetData (Name) == FALSE)) {
         if (Cfg->LoginType == 0 && Name[0] != '\0') {
            Log->Write ("+%s isn't in user list", Name);
            if (Status != NULL)
               Status->SetLine (0, "%s (New user)", Name);
         }

         if (Embedded->DisplayFile ("notfound") == FALSE)
            Embedded->Printf (Language->Text (LNG_NAMENOTFOUND), Name);
         Embedded->Printf (Language->Text (LNG_CONTINUEASNEW));
         if (Embedded->GetAnswer (ASK_DEFNO) == ANSWER_NO)
            continue;

         User->Clear ();
         strcpy (User->Name, Name);
         strcpy (User->RealName, Name);
         User->CreationDate = time (NULL);
         User->HotKey = FALSE;
         User->Level = Cfg->NewUserLevel;
         User->AccessFlags = Cfg->NewUserFlags;
         User->DenyFlags = Cfg->NewUserDenyFlags;
         strcpy (User->LimitClass, Cfg->NewUserLimits);
         User->ScreenWidth = 80;
         User->ScreenHeight = 24;
         User->Ansi = (UCHAR)Embedded->Ansi;
         User->Avatar = (UCHAR)Embedded->Avatar;
         User->Color = (UCHAR)Embedded->Color;
         User->InUserList = TRUE;

         strcpy (Temp, User->Name);
         if ((p = strtok (strlwr (Temp), " ")) != NULL) {
            User->MailBox[0] = *p;                 // La prima lettera della mailbox e'
            User->MailBox[1] = '\0';               // la prima lettera del nome
            if ((p = strtok (NULL, " ")) == NULL)  // Cerca il cognome
               p = &Temp[1];
            if (strlen (p) > 7)                    // Se la mailbox risultasse piu' di
               p[7] = '\0';                        // otto caratteri, forza gli otto caratteri
            strcat (User->MailBox, p);
         }

         SetUseronRecord ("New User");
         Embedded->DisplayFile ("applic");

         if (Embedded->AbortSession () == FALSE && Cfg->AskCity != NO)
            do {
               Embedded->Printf (Language->Text (LNG_ASKCITY));
               Embedded->Input (User->City, (USHORT)(sizeof (User->City) - 1), INP_FIELD);
            } while (User->City[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskCity == REQUIRED);
         if (Snoop != NULL)
            Snoop->SetCity (User->City);
         else if (Com != NULL)
            Com->SetCity (User->City);

         if (Embedded->AbortSession () == FALSE && Cfg->AskCompanyName != NO)
            do {
               Embedded->Printf (Language->Text(LNG_ASKCOMPANYNAME));
               Embedded->Input (User->Company, (USHORT)(sizeof (User->Company) - 1), INP_FIELD);
            } while (User->Company[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskCompanyName == REQUIRED);

         if (Embedded->AbortSession () == FALSE && Cfg->AskAddress != NO)
            do {
               Embedded->Printf (Language->Text(LNG_ASKADDRESS));
               Embedded->Input (User->Address, (USHORT)(sizeof (User->Address) - 1), INP_FIELD);
            } while (User->Address[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskAddress == REQUIRED);

         if (Embedded->AbortSession () == FALSE && Cfg->AskPhoneNumber != NO)
            do {
               Embedded->Printf (Language->Text (LNG_ASKDAYPHONE));
               Embedded->Input (User->DayPhone, (USHORT)(sizeof (User->DayPhone) - 1), INP_FIELD);
            } while (User->DayPhone[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskPhoneNumber == REQUIRED);

         if (Embedded->AbortSession () == FALSE)
            Embedded->DisplayFile ("newuser1");

         do {
            if (Embedded->AbortSession () == FALSE)
               do {
                  Embedded->Printf (Language->Text(LNG_ASKPASSWORD));
                  Embedded->Input (Password, (USHORT)(sizeof (Password) - 1), INP_FIELD|INP_PWD);
               } while (Password[0] == '\0' && Embedded->AbortSession () == FALSE);

            if (Embedded->AbortSession () == FALSE)
               do {
                  Embedded->Printf (Language->Text (LNG_REENTERPASSWORD));
                  Embedded->Input (Temp, (USHORT)(sizeof (Password) - 1), INP_FIELD|INP_PWD);
               } while (Temp[0] == '\0' && Embedded->AbortSession () == FALSE);
            if (stricmp (Temp, Password))
               Embedded->Printf (Language->Text (LNG_PASSWORDNOMATCH));
         } while (Embedded->AbortSession () == FALSE && stricmp (Temp, Password));

         if (Embedded->AbortSession () == FALSE) {
            User->SetPassword (Password);
            User->Add ();
            RetVal = TRUE;
         }

         if (Cfg->UseAnsi == YES) {
            User->Ansi = User->Color = TRUE;
            Embedded->Ansi = User->Ansi;
            Embedded->Color = User->Color;
         }
         else if (Cfg->UseAnsi == NO) {
            User->Ansi = User->Color = FALSE;
            Embedded->Ansi = User->Ansi;
            Embedded->Color = User->Color;
         }
         else if (Embedded->AbortSession () == FALSE && Cfg->UseAnsi == ASK)
            do {
               Embedded->Printf (Language->Text (LNG_ASKANSI));
               if ((Answer = Embedded->GetAnswer (ASK_DEFNO|ASK_HELP)) == ANSWER_YES) {
                  User->Ansi = User->Color = TRUE;
                  Embedded->Ansi = User->Ansi;
                  Embedded->Color = User->Color;
               }
               else if (Answer == ANSWER_HELP)
                  Embedded->DisplayFile ("why_ansi");
            } while (Answer == ANSWER_HELP && Embedded->AbortSession () == FALSE);

         if (User->Ansi == FALSE) {
            if (Cfg->UseAvatar == YES) {
               User->Avatar = User->Color = TRUE;
               Embedded->Avatar = User->Avatar;
               Embedded->Color = User->Color;
            }
            else if (Cfg->UseAvatar == NO) {
               User->Avatar = User->Color = FALSE;
               Embedded->Avatar = User->Avatar;
               Embedded->Color = User->Color;
            }
            else if (Embedded->AbortSession () == FALSE && Cfg->UseAvatar == ASK)
               do {
                  Embedded->Printf (Language->Text (LNG_ASKAVATAR));
                  if ((Answer = Embedded->GetAnswer (ASK_DEFNO|ASK_HELP)) == ANSWER_YES) {
                     User->Avatar = User->Color = TRUE;
                     Embedded->Avatar = User->Avatar;
                     Embedded->Color = User->Color;
                  }
                  else if (Answer == ANSWER_HELP)
                     Embedded->DisplayFile ("why_avt");
               } while (Answer == ANSWER_HELP && Embedded->AbortSession () == FALSE);
         }

         if (User->Ansi == TRUE || User->Avatar == TRUE) {
            if (Cfg->UseColor == YES)
               User->Color = TRUE;
            else if (Cfg->UseColor == NO)
               User->Color = FALSE;
            else if (Embedded->AbortSession () == FALSE && Cfg->UseColor == ASK)
               do {
                  Embedded->Printf (Language->Text (LNG_ASKCOLOR));
                  if ((Answer = Embedded->GetAnswer (ASK_DEFYES|ASK_HELP)) == ANSWER_YES)
                     User->Color = TRUE;
                  else if (Answer == ANSWER_HELP)
                     Embedded->DisplayFile ("why_col");
               } while (Answer == ANSWER_HELP && Embedded->AbortSession () == FALSE);
            Embedded->Color = User->Color;

            if (Cfg->UseFullScreenEditor == YES)
               User->FullEd = TRUE;
            else if (Cfg->UseFullScreenEditor == NO)
               User->FullEd = FALSE;
            else if (Embedded->AbortSession () == FALSE && Cfg->UseFullScreenEditor == ASK)
               do {
                  Embedded->Printf ("\n\026\001\012Do you want to use the full-screen editor");
                  if ((Answer = Embedded->GetAnswer (ASK_DEFYES|ASK_HELP)) == ANSWER_YES)
                     User->FullEd = TRUE;
                  else if (Answer == ANSWER_HELP)
                     Embedded->DisplayFile ("why_fsed");
               } while (Answer == ANSWER_HELP && Embedded->AbortSession () == FALSE);

            if (Cfg->UseFullScreenReader == YES)
               User->FullReader = TRUE;
            else if (Cfg->UseFullScreenReader == NO)
               User->FullReader = FALSE;
            else if (Embedded->AbortSession () == FALSE && Cfg->UseFullScreenReader == ASK)
               do {
                  Embedded->Printf ("\n\026\001\012Do you want to use the full-screen reader");
                  if ((Answer = Embedded->GetAnswer (ASK_DEFYES|ASK_HELP)) == ANSWER_YES)
                     User->FullReader = TRUE;
                  else if (Answer == ANSWER_HELP)
                     Embedded->DisplayFile ("why_fsrd");
               } while (Answer == ANSWER_HELP && Embedded->AbortSession () == FALSE);

            if (Cfg->UseFullScreenLists == YES)
               User->FullScreen = TRUE;
            else if (Cfg->UseFullScreenLists == NO)
               User->FullScreen = FALSE;
            if (Embedded->AbortSession () == FALSE && Cfg->UseFullScreenLists == ASK)
               do {
                  Embedded->Printf ("\n\026\001\012Do you want to use the full-screen lists");
                  if ((Answer = Embedded->GetAnswer (ASK_DEFYES|ASK_HELP)) == ANSWER_YES)
                     User->FullScreen = TRUE;
                  else if (Answer == ANSWER_HELP)
                     Embedded->DisplayFile ("why_fsls");
               } while (Answer == ANSWER_HELP && Embedded->AbortSession () == FALSE);
         }

         if (Embedded->AbortSession () == FALSE && Cfg->AskAlias != NO) {
            do {
               Embedded->Printf (Language->Text (LNG_ASKALIAS));
               Embedded->Input (User->RealName, (USHORT)(sizeof (User->RealName) - 1), INP_FIELD);
            } while (strlen (User->RealName) < 3 && Embedded->AbortSession () == FALSE && Cfg->AskAlias == REQUIRED);
            if (User->RealName[0] == '\0')
               strcpy (User->RealName, User->Name);
         }

         if (Cfg->UseHotKey == YES)
            User->HotKey = TRUE;
         else if (Cfg->UseHotKey == NO)
            User->HotKey = FALSE;
         else if (Embedded->AbortSession () == FALSE && Cfg->UseHotKey == ASK)
            do {
               Embedded->Printf (Language->Text (LNG_ASKHOTKEY));
               if ((Answer = Embedded->GetAnswer (ASK_DEFYES|ASK_HELP)) == ANSWER_YES)
                  User->HotKey = TRUE;
               else if (Answer == ANSWER_HELP)
                  Embedded->DisplayFile ("why_hot");
            } while (Answer == ANSWER_HELP && Embedded->AbortSession () == FALSE);

         if (Cfg->UseIBMChars == YES)
            User->IBMChars = TRUE;
         else if (Cfg->UseIBMChars == NO)
            User->IBMChars = FALSE;
         else if (Embedded->AbortSession () == FALSE && Cfg->UseIBMChars == ASK)
            do {
               Embedded->Printf (Language->Text (LNG_ASKIBMCHARS));
               if ((Answer = Embedded->GetAnswer (ASK_DEFYES|ASK_HELP)) == ANSWER_YES)
                  User->IBMChars = TRUE;
               else if (Answer == ANSWER_HELP)
                  Embedded->DisplayFile ("why_ibm");
            } while (Answer == ANSWER_HELP && Embedded->AbortSession () == FALSE);

         if (Embedded->AbortSession () == FALSE && Cfg->AskLines == YES)
            do {
               Embedded->Printf (Language->Text (LNG_ASKLINES));
               Embedded->Input (Temp, 2, INP_FIELD);
            } while ((atoi (Temp) < 10 || atoi (Temp) > 66) && Embedded->AbortSession () == FALSE);
         User->ScreenHeight = (USHORT)atoi (Temp);

         if (Cfg->UsePause == YES)
            User->MorePrompt = TRUE;
         else if (Cfg->UsePause == NO)
            User->MorePrompt = FALSE;
         else if (Embedded->AbortSession () == FALSE && Cfg->UsePause == ASK) {
            Embedded->Printf (Language->Text (LNG_ASKPAUSE));
            if ((Answer = Embedded->GetAnswer (ASK_DEFYES)) == ANSWER_YES)
               User->MorePrompt = TRUE;
         }

         if (Cfg->UseScreenClear == YES)
            User->ScreenClear = TRUE;
         else if (Cfg->UseScreenClear == NO)
            User->ScreenClear = FALSE;
         else if (Embedded->AbortSession () == FALSE && Cfg->UseScreenClear == ASK) {
            Embedded->Printf (Language->Text (LNG_ASKSCREENCLEAR));
            if ((Answer = Embedded->GetAnswer (ASK_DEFYES)) == ANSWER_YES)
               User->ScreenClear = TRUE;
         }

         if (Embedded->AbortSession () == FALSE && Cfg->AskBirthDate == YES)
            SetBirthDate ();

         User->MailCheck = Cfg->AskMailCheck;
         if (Embedded->AbortSession () == FALSE && Cfg->AskMailCheck == ASK) {
            Embedded->Printf (Language->Text (LNG_ASKMAILCHECK));
            if ((Answer = Embedded->GetAnswer (ASK_DEFYES)) == ANSWER_YES)
               User->MailCheck = TRUE;
         }

         User->NewFileCheck = Cfg->AskFileCheck;
         if (Embedded->AbortSession () == FALSE && Cfg->AskFileCheck == YES) {
            Embedded->Printf (Language->Text (LNG_ASKFILECHECK));
            if ((Answer = Embedded->GetAnswer (ASK_DEFYES)) == ANSWER_YES)
               User->NewFileCheck = TRUE;
         }

         if (Embedded->AbortSession () == FALSE && Cfg->AskGender != NO) {
            do {
               Embedded->Printf (Language->Text(LNG_ASKSEX));
               Embedded->Input (Cmd, 1, INP_FIELD);
               User->Sex = (UCHAR)toupper (Cmd[0]);
            } while (User->Sex != Language->Male && User->Sex != Language->Female && Embedded->AbortSession () == FALSE && Cfg->AskGender == REQUIRED);

            if (User->Sex == Language->Female)
               User->Sex = 1;
            else if (User->Sex == Language->Male)
               User->Sex = 0;
            else
               User->Sex = 0;
         }


/*
         if (Embedded->AbortSession () == FALSE && Cfg->CheckAnsi == YES) {
            Embedded->Printf (Language->Text(LNG_ASKANSI));
            if (Embedded->GetAnswer (ASK_DEFYES) == ANSWER_YES)
               User->Ansi = User->Color = TRUE;
         }

         if (Cfg->LoginType != 0 && Embedded->AbortSession () == FALSE)
            do {
               Embedded->Printf (Language->Text(LNG_ENTERNAME));
               Embedded->Input (Name, (USHORT)(sizeof (Name) - 1), Flags);
               if (Embedded->AbortSession () == FALSE && strlen (Name) >= 2) {
                  Log->Write ("+%s is calling", Name);
                  Log->Write ("+%s isn't in user list", Name);
               }
            } while (Embedded->AbortSession () == FALSE && strlen (Name) < 3);

         strcpy (User->Name, Name);

         Embedded->DisplayFile ("applic");

         if (Embedded->AbortSession () == FALSE && Cfg->RealName != NO) {
            do {
               Embedded->Printf (Language->Text(LNG_ASKREALNAME));
               Embedded->Input (User->RealName, (USHORT)(sizeof (User->RealName) - 1), INP_FIELD|INP_FANCY);
            } while (strlen (User->RealName) < 3 && Embedded->AbortSession () == FALSE && Cfg->RealName == REQUIRED);
            if (User->RealName[0] == '\0')
               strcpy (User->RealName, User->Name);
         }

         if (Embedded->AbortSession () == FALSE && Cfg->CompanyName != NO)
            do {
               Embedded->Printf (Language->Text(LNG_ASKCOMPANYNAME));
               Embedded->Input (User->Company, (USHORT)(sizeof (User->Company) - 1), INP_FIELD);
            } while (User->Company[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->CompanyName == REQUIRED);

         if (Embedded->AbortSession () == FALSE && Cfg->Address != NO)
            do {
               Embedded->Printf (Language->Text(LNG_ASKADDRESS));
               Embedded->Input (User->Address, (USHORT)(sizeof (User->Address) - 1), INP_FIELD);
            } while (User->Address[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->Address == REQUIRED);

         if (Embedded->AbortSession () == FALSE && Cfg->City != NO)
            do {
               Embedded->Printf (Language->Text(LNG_ASKCITY));
               Embedded->Input (User->City, (USHORT)(sizeof (User->City) - 1), INP_FIELD);
            } while (User->City[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->City == REQUIRED);
         if (Snoop != NULL)
            Snoop->SetCity (User->City);
         else if (Com != NULL)
            Com->SetCity (User->City);

         if (Embedded->AbortSession () == FALSE && Cfg->PhoneNumber != NO)
            do {
               Embedded->Printf (Language->Text(LNG_ASKDAYPHONE));
               Embedded->Input (User->DayPhone, (USHORT)(sizeof (User->DayPhone) - 1), INP_FIELD);
            } while (User->DayPhone[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->PhoneNumber == REQUIRED);

         if (Embedded->AbortSession () == FALSE && Cfg->Gender != NO) {
            do {
               Embedded->Printf (Language->Text(LNG_ASKSEX));
               Embedded->Input (Cmd, 1, INP_FIELD);
               User->Sex = (UCHAR)toupper (Cmd[0]);
            } while (User->Sex != Language->Male && User->Sex != Language->Female && Embedded->AbortSession () == FALSE && Cfg->Gender == REQUIRED);

            if (User->Sex == Language->Female)
               User->Sex = 1;
            else if (User->Sex == Language->Male)
               User->Sex = 0;
            else
               User->Sex = 0;
         }
*/

         User->Update ();
         if (Embedded->AbortSession () == FALSE)
            Embedded->DisplayFile ("newuser2");
      }
      else {
         if (Snoop != NULL)
            Snoop->SetCity (User->City);
         else if (Com != NULL)
            Com->SetCity (User->City);

         SetUseronRecord ("Login");

         if (User->Language[0] != '\0') {
            if (Language->Load (User->Language) == TRUE) {
               Log->Write (":Loaded language %s", User->Language);
               strcpy (Menu->AltPath, Language->MenuPath);
               strcpy (Embedded->AltPath, Language->TextFiles);
               Reload = TRUE;
               if (Menu->AltPath[0] != '\0')
                  Log->Write ("   Language Menu Path = %s", Menu->AltPath);
               if (Embedded->AltPath[0] != '\0')
                  Log->Write ("   Language Text Files Path = %s", Embedded->AltPath);
            }
            else {
               Log->Write ("!Failed to load language %s", User->Language);
               Language->Load ("default.lng");
               strcpy (Menu->AltPath, Language->MenuPath);
               strcpy (Embedded->AltPath, Language->TextFiles);
            }
         }

         if (Cfg->LoginType == 0) {
            Embedded->Printf (Language->Text(LNG_USERFROMCITY), Name, User->City);
            Embedded->Printf (Language->Text(LNG_DEFYESNO));
            Embedded->Input (Cmd, 1, INP_FIELD);
            if (toupper (Cmd[0]) == 'N' && Log != NULL)
               Log->Write ("!User temporarily brain-lapsed");
         }

         Password[0] = '\0';
         if (Cfg->LoginType != 0 || toupper (Cmd[0]) != 'N') {
            Embedded->Printf (Language->Text(LNG_ENTERPASSWORD));
            Embedded->Input (Password, (USHORT)(sizeof (Password) - 1), INP_FIELD|INP_PWD);
         }
      }

      if (Embedded->AbortSession () == FALSE && Password[0] != '\0') {
         if (User->CheckPassword (Password) == FALSE) {
            Embedded->Printf (Language->Text(LNG_INVALIDPASSWORD));
            Log->Write ("!Invalid Password (%s)", strlwr (Password));
            Counter++;
         }
         else
            RetVal = TRUE;
      }
   }

   if (RetVal == TRUE && Status != NULL)
      Status->SetLine (0, "%s from %s", User->Name, User->City);

   return (RetVal);
}

VOID TBbs::IEMSILogin ()
{
   CHAR Temp[48], *p;

   User->CreationDate = time (NULL);
   User->HotKey = FALSE;
   User->Level = Cfg->NewUserLevel;
   User->AccessFlags = Cfg->NewUserFlags;
   User->DenyFlags = Cfg->NewUserDenyFlags;
   strcpy (User->LimitClass, Cfg->NewUserLimits);
   User->ScreenWidth = 80;
   User->ScreenHeight = 24;
   if (User->Ansi == TRUE || User->Avatar == TRUE)
      User->Color = TRUE;

   Embedded->DisplayFile ("applic");

   if (Embedded->AbortSession () == FALSE && Cfg->AskCity != NO)
      do {
         Embedded->Printf (Language->Text (LNG_ASKCITY));
         Embedded->Input (User->City, (USHORT)(sizeof (User->City) - 1), INP_FIELD);
      } while (User->City[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskCity == REQUIRED);
   if (Snoop != NULL)
      Snoop->SetCity (User->City);
   else if (Com != NULL)
      Com->SetCity (User->City);

   if (Embedded->AbortSession () == FALSE && Cfg->AskCompanyName != NO)
      do {
         Embedded->Printf (Language->Text(LNG_ASKCOMPANYNAME));
         Embedded->Input (User->Company, (USHORT)(sizeof (User->Company) - 1), INP_FIELD);
      } while (User->Company[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskCompanyName == REQUIRED);

   if (Embedded->AbortSession () == FALSE && Cfg->AskAddress != NO)
      do {
         Embedded->Printf (Language->Text(LNG_ASKADDRESS));
         Embedded->Input (User->Address, (USHORT)(sizeof (User->Address) - 1), INP_FIELD);
      } while (User->Address[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskAddress == REQUIRED);

   if (Embedded->AbortSession () == FALSE && Cfg->AskPhoneNumber != NO)
      do {
         Embedded->Printf (Language->Text (LNG_ASKDAYPHONE));
         Embedded->Input (User->DayPhone, (USHORT)(sizeof (User->DayPhone) - 1), INP_FIELD);
      } while (User->DayPhone[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->AskPhoneNumber == REQUIRED);

   if (Embedded->AbortSession () == FALSE && Cfg->AskAlias != NO) {
      do {
         Embedded->Printf (Language->Text (LNG_ASKALIAS));
         Embedded->Input (User->RealName, (USHORT)(sizeof (User->RealName) - 1), INP_FIELD);
      } while (strlen (User->RealName) < 3 && Embedded->AbortSession () == FALSE && Cfg->AskAlias == REQUIRED);
      if (User->RealName[0] == '\0')
         strcpy (User->RealName, User->Name);
   }

/*
   if (Embedded->AbortSession () == FALSE && Cfg->CompanyName != NO)
      do {
         Embedded->Printf (Language->Text(LNG_ASKCOMPANYNAME));
         Embedded->Input (User->Company, (USHORT)(sizeof (User->Company) - 1), INP_FIELD);
      } while (User->Company[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->CompanyName == REQUIRED);

   if (Embedded->AbortSession () == FALSE && Cfg->Address != NO)
      do {
         Embedded->Printf (Language->Text(LNG_ASKADDRESS));
         Embedded->Input (User->Address, (USHORT)(sizeof (User->Address) - 1), INP_FIELD);
      } while (User->Address[0] == '\0' && Embedded->AbortSession () == FALSE && Cfg->Address == REQUIRED);

   if (Embedded->AbortSession () == FALSE && Cfg->Gender != NO) {
      do {
         Embedded->Printf (Language->Text(LNG_ASKSEX));
         Embedded->Input (Cmd, 1, INP_FIELD);
         User->Sex = (UCHAR)toupper (Cmd[0]);
      } while (User->Sex != Language->Male && User->Sex != Language->Female && Embedded->AbortSession () == FALSE && Cfg->Gender == REQUIRED);

      if (User->Sex == Language->Female)
         User->Sex = 1;
      else if (User->Sex == Language->Male)
         User->Sex = 0;
      else
         User->Sex = 0;
   }
*/

   strcpy (Temp, User->Name);
   if ((p = strtok (strlwr (Temp), " ")) != NULL) {
      User->MailBox[0] = *p;                 // La prima lettera della mailbox e'
      User->MailBox[1] = '\0';               // la prima lettera del nome
      if ((p = strtok (NULL, " ")) == NULL)  // Cerca il cognome
         p = &Temp[1];
      if (strlen (p) > 7)                    // Se la mailbox risultasse piu' di
         p[7] = '\0';                        // otto caratteri, forza gli otto caratteri
      strcat (User->MailBox, p);
   }
}

VOID TBbs::CheckBirthday ()
{
   struct dosdate_t d_date;

   _dos_getdate (&d_date);
   if (User->BirthDay == d_date.day && User->BirthMonth == d_date.month)
      Embedded->DisplayFile ("birthday");
}

VOID TBbs::Run ()
{
   USHORT Logged, Executed, Manual, Hangup;
   USHORT FirstTime, DoTimeWarn, Flags;
   ULONG CallLen;
   class TStatistics *Stats;
   class TDetect *Detect;
   class TEvents *Events;
   class TLimits *Limits;

   Hangup = FALSE;
   Logged = Manual = FALSE;
   StartCall = time (NULL);
   User = new TUser (Cfg->UserFile);
//   Task = Cfg->TaskNumber;
   SetUseronRecord ("Login");

   Language->Load ("default.lng");
   strcpy (Embedded->AltPath, Language->TextFiles);

   if (Status != NULL)
      Status->Clear ();

   Pause (50);

   if (User != NULL && Language != NULL && Embedded != NULL) {
      Embedded->Task = Task;
      Embedded->Com = Com;
      Embedded->Snoop = Snoop;
      Embedded->User = User;
      Embedded->Language = Language;
      Embedded->Cfg = Cfg;
      Embedded->Log = Log;
      strcpy (Embedded->Path, Cfg->TextFiles);
      Embedded->StartCall = StartCall;
      Embedded->CarrierSpeed = Speed;

      if (Local == FALSE) {
         if ((Detect = new TDetect) != NULL) {
            Detect->Task = Task;
            Detect->Com = Com;
            Detect->Cfg = Cfg;
            Detect->Log = Log;
            Detect->Events = Events;
            Detect->Speed = Speed;
            Detect->Progress = Progress;
            Detect->MailerStatus = MailerStatus;
            Detect->Status = Status;

            Detect->Terminal ();
            if (Embedded->AbortSession () == TRUE && Log != NULL)
               Log->Write ("!Carrier lost");

            Embedded->Ansi = Detect->Ansi;
            Embedded->Avatar = Detect->Avatar;
            Embedded->Rip = Detect->Rip;
            if (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE)
               Embedded->Color = TRUE;
            Remote = Detect->Remote;

            if (Detect->IEMSI == TRUE) {
               Log->Write ("*Detected IEMSI login");
               Log->Write (":  %s from %s", Detect->Name, Detect->City);
               strcpy (Name, Detect->Name);

               Log->Write ("+%s is calling", Name);
               if (Status != NULL)
                  Status->SetLine (0, "%s from %s", Detect->Name, Detect->City);

               if (User->GetData (Name) == FALSE) {
                  Log->Write ("+%s isn't in user list", Name);
                  User->Clear ();
                  strcpy (User->Name, Name);
                  strcpy (User->RealName, Detect->RealName);
                  strcpy (User->City, Detect->City);

                  User->Ansi = Detect->Ansi;
                  User->Avatar = Detect->Avatar;
                  if (User->Ansi == TRUE && User->Avatar == TRUE)
                     User->Color = TRUE;
                  User->SetPassword (Detect->Password);
                  User->Add ();

                  Embedded->DisplayFile ("Logo");
                  IEMSILogin ();
                  if (Embedded->AbortSession () == TRUE && Log != NULL)
                     Log->Write ("!Carrier lost");

                  User->Update ();
                  Logged = TRUE;
               }
               else {
                  Logged = TRUE;
                  if (User->Language[0] != '\0') {
                     if (Language->Load (User->Language) == TRUE) {
                        Log->Write (":Loaded language %s", User->Language);
                        strcpy (Menu->AltPath, Language->MenuPath);
                        strcpy (Embedded->AltPath, Language->TextFiles);
                        Reload = TRUE;
                        if (Menu->AltPath[0] != '\0')
                           Log->Write ("   Language Menu Path = %s", Menu->AltPath);
                        if (Embedded->AltPath[0] != '\0')
                           Log->Write ("   Language Text Files Path = %s", Embedded->AltPath);
                     }
                     else {
                        Log->Write ("!Failed to load language %s", User->Language);
                        Language->Load ("default.lng");
                        strcpy (Menu->AltPath, Language->MenuPath);
                        strcpy (Embedded->AltPath, Language->TextFiles);
                     }
                  }
                  if (User->CheckPassword (Detect->Password) == FALSE) {
                     Log->Write ("!Invalid Password (%s)", strlwr (Detect->Password));
                     Logged = Manual = FALSE;
                  }
                  else
                     Embedded->DisplayFile ("logo");
               }

               User->Ansi = Detect->Ansi;
               User->Avatar = Detect->Avatar;
               if (User->Ansi == TRUE && User->Avatar == TRUE)
                  User->Color = TRUE;
               User->FullEd = Detect->FullEd;
               User->IBMChars = Detect->IBMChars;
               User->MorePrompt = Detect->MorePrompt;
               User->HotKey = Detect->HotKeys;
               User->ScreenClear = Detect->ScreenClear;
               User->MailCheck = Detect->MailCheck;
               User->NewFileCheck = Detect->FileCheck;
            }
            else if (Detect->Remote != REMOTE_USER)
               Hangup = TRUE;
            delete Detect;
         }
      }
      else {
         // In locale viene forzata l'emulazione ANSI, visto che tutti i componenti
         // locali possono visualizzare i codici ANSI senza problemi.
         Embedded->Ansi = Embedded->Color = TRUE;
         Remote = REMOTE_USER;
      }

      if (Hangup == FALSE && Remote == REMOTE_USER) {
         Embedded->TimeLimit = 10;
         if ((Events = new TEvents (Cfg->SchedulerFile)) != NULL) {
            Events->Load ();
            Events->TimeToNext ();
            if (Embedded->TimeLimit > Events->TimeRemain)
               Embedded->TimeLimit = Events->TimeRemain;
            delete Events;
         }

         if (Logged == FALSE && Manual == FALSE) {
            Embedded->DisplayFile ("logo");
            Logged = Login ();
            if (Embedded->AbortSession () == TRUE && Log != NULL)
               Log->Write ("!Carrier lost");
            Manual = TRUE;
         }

         if (Logged == TRUE) {
            Embedded->Ansi = User->Ansi;
            Embedded->Avatar = User->Avatar;
            Embedded->Color = User->Color;
            Embedded->HotKey = User->HotKey;

            if (Snoop != NULL) {
               Snoop->SetName (User->Name);
               Snoop->SetCity (User->City);
            }
            else if (Com != NULL) {
               Com->SetName (User->Name);
               Com->SetCity (User->City);
            }

            // Determina se e' passato un giorno calcolando quanti giorni
            // sono passati secondo la unix date, se il numero di giorni
            // dell'ultima chiamata e' diverso da oggi significa che si deve
            // azzerare tutto.
            if ((User->LastCall / 86400L) != (time (NULL) / 86400L)) {
               User->TodayTime = 0;
               User->FilesToday = 0;
               User->BytesToday = 0L;
               Log->Write (":Daily Time/DL Zeroed");
            }

            SetUseronRecord ("Login");
            User->TotalCalls++;
            User->Update ();

//            if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
//               if (Limits->Read (User->LimitClass) == TRUE) {
//                  if (Limits->CallTimeLimit != 0 && Embedded->TimeLimit > Limits->CallTimeLimit)
//                     Embedded->TimeLimit = Limits->CallTimeLimit;
//                  if (Limits->DayTimeLimit != 0 && (Embedded->TimeLimit + User->TodayTime) > Limits->DayTimeLimit)
//                     Embedded->TimeLimit = (USHORT)(Limits->DayTimeLimit - User->TodayTime);
//               }
//               delete Limits;
//            }
            DoTimeWarn = FALSE;
            if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
               if (Limits->Read (User->LimitClass) == TRUE) {
                  User->Level = Limits->Level;
                  User->AccessFlags = Limits->Flags;
                  User->DenyFlags = Limits->DenyFlags;
                  Embedded->TimeLimit = Limits->CallTimeLimit;
                  if (Limits->DayTimeLimit != 0 && (Embedded->TimeLimit + User->TodayTime) > Limits->DayTimeLimit)
                     Embedded->TimeLimit = (USHORT)(Limits->DayTimeLimit - User->TodayTime);
               }
               delete Limits;
            }
            if (TimeLimit != 0) {
               if (Embedded->TimeLimit > TimeLimit) {
                  DoTimeWarn = TRUE;
                  Embedded->TimeLimit = TimeLimit;
               }
            }
            else if ((Events = new TEvents (Cfg->SchedulerFile)) != NULL) {
               Events->Load ();
               Events->TimeToNext ();
               if (Embedded->TimeLimit > Events->TimeRemain) {
                  DoTimeWarn = TRUE;
                  Embedded->TimeLimit = Events->TimeRemain;
               }
               delete Events;
            }

            if (Log != NULL && Embedded->TimeLimit != 0)
               Log->Write ("#Given %u mins (%s)", Embedded->TimeLimit, User->LimitClass);

            if (Status != NULL)
               Status->SetLine (1, "Level: %u (%s)  Time: %lu", User->Level, User->LimitClass, Embedded->TimeRemain ());

            if ((EMail = new TEMail) != NULL) {
               EMail->Cfg = Cfg;
               EMail->Embedded = Embedded;
               EMail->Language = Language;
               EMail->Log = Log;
               EMail->User = User;
               EMail->Storage = Cfg->MailStorage;
               if (Cfg->MailPath[0] != '\0')
                  strcpy (EMail->BasePath, Cfg->MailPath);
            }

            if ((Message = new TMessage (Cfg->SystemPath)) != NULL) {
               Message->Cfg = Cfg;
               Message->Embedded = Embedded;
               Message->Language = Language;
               Message->Log = Log;
               Message->User = User;
               Message->ShowKludges = User->Kludges;
               Message->OpenArea (User->LastMsgArea);
               Embedded->MsgArea = Message->Current;
            }

            if ((Library = new TLibrary (Cfg->SystemPath)) != NULL) {
               Library->Task = Task;
               Library->Cfg = Cfg;
               Library->Embedded = Embedded;
               Library->Log = Log;
               Library->User = User;
               Library->CarrierSpeed = Speed;
               Library->Progress = Progress;
               Library->Language = Language;
               if (Library->Current != NULL) {
                  if (Library->Current->Read (User->LastFileArea) == FALSE) {
                     if (Library->Current->First () == FALSE)
                        Library->Current->New ();
                  }
                  if (Library->Current->Key[0] != '\0') {
                     Log->Write (":File Area: %s - %s", Library->Current->Key, Library->Current->Display);
                     if (Library->Current->Download[strlen (Library->Current->Download) - 1] != '\\')
                        strcat (Library->Current->Download, "\\");
                     if (Library->Current->Upload[strlen (Library->Current->Upload) - 1] != '\\')
                        strcat (Library->Current->Upload, "\\");
                  }
               }
               Embedded->FileArea = Library->Current;
            }

            if ((Stats = new TStatistics (Cfg->SystemPath)) != NULL) {
               Stats->Read (Task);
               Stats->Calls++;
               Stats->TodayCalls++;
               Stats->TotalCalls++;
               Stats->Update ();
               delete Stats;
            }

            Embedded->DisplayFile ("welcome");

            if (EMail != NULL && User->MailCheck == TRUE)
               EMail->CheckUnread ();

            CheckBirthday ();
            Embedded->DisplayFile ("news");
            if (DoTimeWarn == TRUE)
               Embedded->DisplayFile ("timewarn");

            FirstTime = TRUE;
            if (Menu != NULL) {
               strcpy (Menu->Path, Cfg->MenuPath);
               Reload = TRUE;
               strcpy (MenuName, Cfg->MainMenu);

               strcpy (Menu->AltPath, Language->MenuPath);
               strcpy (Embedded->AltPath, Language->TextFiles);

               while (Embedded->AbortSession () == FALSE && Logoff == FALSE) {
                  SetUseronRecord ("Browsing");

                  if (Reload == TRUE) {
                     FirstTime = TRUE;
                     if (Menu->Load (MenuName) == FALSE) {
                        Log->Write ("!Can't load menu: %s", MenuName);
                        if (!stricmp (MenuName, Cfg->MainMenu))
                           Logoff = TRUE;
                        if (Stack.Last () != NULL) {
                           if (!strcmp (MenuName, (PSZ)Stack.Value ()))
                              Stack.Remove ();
                        }
                     }
                     Reload = FALSE;
                  }

                  if (Logoff == FALSE) {
                     // Imposta di default il modo hotkey normale
                     Flags = INP_HOTKEY;

                     // Esegue il loop per la visualizzazione del menu' correntemente
                     // caricato in memoria.
                     if (Menu->First () == TRUE)
                        do {
                           if (User->Level >= Menu->Level) {
                              if ((User->AccessFlags & Menu->AccessFlags) == Menu->AccessFlags) {
                                 if (Menu->FirstTime == FALSE || FirstTime == TRUE) {
                                    if (Menu->Display[0] != '\0')
                                       Embedded->DisplayPrompt (Menu->Display, Menu->Color, Menu->Hilight, FALSE);
                                 }
                                 if (Menu->Command == MNU_APPENDMENU)
                                    Menu->Automatic = Menu->FirstTime = TRUE;
                                 if (Menu->Automatic == TRUE && (Menu->FirstTime == FALSE || FirstTime == TRUE)) {
                                    if (Com != NULL)
                                       Com->UnbufferBytes ();
                                    if (Snoop != NULL)
                                       Snoop->UnbufferBytes ();
                                    ExecuteCommand (Menu);
                                 }
                                 // Verifica se c'e' il comando di lettura di messaggi
                                 // individuali associato ad un numero, nel qual caso
                                 // disabilita l'hotkey per gli input numerici.
                                 if ((Menu->Command == MNU_MSGINDIVIDUAL || Menu->Command == MNU_MAILINDIVIDUAL) && isdigit (Menu->Key[0]))
                                    Flags |= INP_NONUMHOT;
                              }
                           }
                        } while (Menu->Next () == TRUE);

                     // Se il tempo rimasto e' minore di 3 minuti, visualizza un
                     // avvertimento per l'utente.
                     if (Embedded->TimeRemain () <= 3)
                        Embedded->Printf ("\n\026\001\015You only have %lu minute(s) left\n", Embedded->TimeRemain ());

                     FirstTime = FALSE;
                     if (Status != NULL) {
                        Status->SetLine (0, "%s from %s", User->Name, User->City);
                        Status->SetLine (1, "Level: %u (%s)  Time: %lu", User->Level, User->LimitClass, Embedded->TimeRemain ());
                     }

                     Embedded->DisplayPrompt (Menu->Prompt, Menu->PromptColor, Menu->PromptHilight, TRUE);
                     Embedded->Input (Cmd, (USHORT)(sizeof (Cmd) - 1), Flags);

                     if (Embedded->AbortSession () == FALSE) {
                        Executed = FALSE;
                        if (Menu->First () == TRUE)
                           do {
                              if (User->Level >= Menu->Level && Menu->Key[0] != '\0') {
                                 if ((User->AccessFlags & Menu->AccessFlags) == Menu->AccessFlags) {
                                    if (!stricmp (Menu->Key, Cmd) || (Cmd[0] == '\0' && !strcmp (Menu->Key, "|")) || Menu->Key[0] == Cmd[0]) {
                                       ExecuteCommand (Menu);
                                       Executed = TRUE;
                                    }
                                 }
                              }
                           } while (Menu->Next () == TRUE);

                        if (Executed == FALSE)
                           Embedded->Printf (Language->Text(LNG_MENUERROR));
                     }
                  }
               }
               if (Embedded->AbortSession () == TRUE && Log != NULL)
                  Log->Write ("!Carrier lost");
            }

            if (Embedded->AbortSession () == FALSE && Logoff == TRUE) {
               if (Embedded->DisplayFile ("logoff") == FALSE)
                  Embedded->DisplayFile ("byebye");
            }

            if (Library != NULL) {
               delete Library;
               Embedded->FileArea = NULL;
            }
            if (Message != NULL) {
               delete Message;
               Embedded->MsgArea = NULL;
            }
         }

         if (Name[0] != '\0') {
            CallLen = (time (NULL) - StartCall) / 60L;

            User->LastCall = time (NULL);
            User->TodayTime += CallLen;
            User->WeekTime += CallLen;
            User->MonthTime += CallLen;
            User->YearTime += CallLen;

            if (User->Update () == FALSE)
               Log->Write ("|Can't update user's record");
            Log->Write ("+%s off-line. Calls=%ld, Len=%ld", Name, User->TotalCalls, CallLen);
         }
      }
   }

   DisableUseronRecord ();
}

// ----------------------------------------------------------------------

TListings::TListings ()
{
}

TListings::~TListings ()
{
}

VOID TListings::Begin ()
{
}

USHORT TListings::DrawScreen ()
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

VOID TListings::Down ()
{
   USHORT i;

   if (List.Next () != NULL) {
      List.Previous ();
      RemoveCursor (y);
      List.Next ();
      PrintCursor (++y);
      Data.Next ();
   }
   else if (Data.Next () != NULL) {
      Embedded->PrintfAt (4, 1, "");
      for (i = 0; i < (User->ScreenHeight - 6); i++)
         Embedded->Printf ("\x1B[K\n");
      List.Clear ();
      Embedded->PrintfAt (4, 1, "");
      i = 0;
      do {
         pld = (LISTDATA *)Data.Value ();
         PrintLine ();
         List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
      } while (Data.Next () != NULL && ++i < (User->ScreenHeight - 6));
      for (y = 0; y < i; y++)
         Data.Previous ();
      y = 4;
      List.First ();
      PrintCursor (y);
   }
}

VOID TListings::PageDown ()
{
   USHORT i;

   RemoveCursor (y);
   while (List.Next () != NULL) {
      Data.Next ();
      y++;
   }

   if (Data.Next () != NULL) {
      Embedded->PrintfAt (4, 1, "");
      for (i = 0; i < (User->ScreenHeight - 6); i++)
         Embedded->Printf ("\x1B[K\n");
      List.Clear ();
      Embedded->PrintfAt (4, 1, "");
      i = 0;
      do {
         pld = (LISTDATA *)Data.Value ();
         PrintLine ();
         List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
      } while (Data.Next () != NULL && ++i < (User->ScreenHeight - 6));
      for (y = 0; y < i; y++)
         Data.Previous ();
      y = 4;
      List.First ();
   }

   PrintCursor (y);
}

VOID TListings::DownloadTag ()
{
}

VOID TListings::Exit ()
{
   End = TRUE;
}

VOID TListings::Tag ()
{
}

VOID TListings::PageUp ()
{
   USHORT i;

   RemoveCursor (y);
   while (List.Previous () != NULL) {
      Data.Previous ();
      y--;
   }
   if (Data.Previous () != NULL) {
      Embedded->PrintfAt (4, 1, "");
      for (i = 0; i < (User->ScreenHeight - 6); i++)
         Embedded->Printf ("\x1B[K\n");
      for (i = 0; i < (User->ScreenHeight - 6 - 1); i++)
         Data.Previous ();
      List.Clear ();
      Embedded->PrintfAt (4, 1, "");
      i = 0;
      do {
         pld = (LISTDATA *)Data.Value ();
         PrintLine ();
         List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
         i++;
      } while (Data.Next () != NULL && i < (User->ScreenHeight - 6));
      Data.Previous ();
      y = (USHORT)(4 + User->ScreenHeight - 6 - 1);
      List.Last ();
   }

   PrintCursor (y);
}

VOID TListings::PrintTitles ()
{
}

VOID TListings::PrintLine ()
{
}

VOID TListings::PrintCursor (USHORT y)
{
   y = y;
}

VOID TListings::RemoveCursor (USHORT y)
{
   y = y;
}

VOID TListings::Select ()
{
   RetVal = End = TRUE;
}

USHORT TListings::Run ()
{
   USHORT i, t;

   RetVal = FALSE;
   Found = FALSE;
   y = 4;

   Begin ();

   if ((pld = (LISTDATA *)Data.Value ()) != NULL) {
      End = FALSE;
      Titles = Redraw = TRUE;

      while (End == FALSE && Embedded->AbortSession () == FALSE) {
         if (Redraw == TRUE) {
            while (List.Previous () != NULL)
               Data.Previous ();

            if (Titles == TRUE) {
               PrintTitles ();
               Titles = FALSE;
            }

            i = DrawScreen ();

            for (t = (USHORT)(i + 4 - 1); t > y; t--) {
               List.Previous ();
               Data.Previous ();
            }

            if (i < (User->ScreenHeight - 6)) {
               do {
                  Embedded->Printf ("\n");
                  i++;
               } while (i < (User->ScreenHeight - 6));
            }
            else
               Data.Previous ();

            PrintCursor (y);
            Redraw = Titles = FALSE;
         }

         if (Embedded->KBHit () == TRUE) {
            if ((i = Embedded->Getch ()) == 0)
               i = (USHORT)(Embedded->Getch () << 8);
            else
               i = (USHORT)toupper (i);

            if (i == ESC) {
               if ((i = Embedded->Getch ()) == '[') {
                  while ((i = Embedded->Getch ()) == ';' || isdigit (i))
                     ;
                  if (i == 'A')
                     i = CTRLE;
                  else if (i == 'B')
                     i = CTRLX;
               }
            }

            switch (i) {
               case CTRLY:
                  PageUp ();
                  break;

               case CTRLE:
               case 0x4800:
                  Up ();
                  break;

               case '\r':
                  Select ();
                  break;

               case ' ':
                  Tag ();
                  break;

               case CTRLV:
                  PageDown ();
                  break;

               case CTRLX:
               case 0x5000:
                  Down ();
                  break;

               case 'X':
                  Exit ();
                  break;
            }
         }
      }

      Embedded->Printf ("\x0C");
   }

   return (RetVal);
}

VOID TListings::Up ()
{
   USHORT i;

   if (List.Previous () != NULL) {
      List.Next ();
      RemoveCursor (y);
      List.Previous ();
      PrintCursor (--y);
      Data.Previous ();
   }
   else if (Data.Previous () != NULL) {
      Embedded->PrintfAt (4, 1, "");
      for (i = 0; i < (User->ScreenHeight - 6); i++)
         Embedded->Printf ("\x1B[K\n");
      for (i = 0; i < (User->ScreenHeight - 6 - 1); i++)
         Data.Previous ();
      List.Clear ();
      Embedded->PrintfAt (4, 1, "");
      i = 0;
      do {
         pld = (LISTDATA *)Data.Value ();
         PrintLine ();
         List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
         i++;
      } while (Data.Next () != NULL && i < (User->ScreenHeight - 6));
      Data.Previous ();
      y = (USHORT)(4 + User->ScreenHeight - 6 - 1);
      List.Last ();
      PrintCursor (y);
   }
}

// ----------------------------------------------------------------------

TStatus::TStatus ()
{
}

TStatus::~TStatus ()
{
}

VOID TStatus::Clear ()
{
}

VOID TStatus::SetLine (USHORT line, PSZ text, ...)
{
   line = line;
   text = text;
}

// ----------------------------------------------------------------------

VOID ParseAddress (PSZ text, PSZ name, PSZ address)
{
   CHAR Temp[128], *p, *a;

   strcpy (Temp, text);
   if (strchr (Temp, '(') != NULL) {
      if ((p = strtok (Temp, " ")) != NULL) {
         p = strtok (NULL, "");
         while (*p == ' ')
            p++;
         if (*p == '(') {
            strcpy (Temp, ++p);
            p = strchr (Temp, '\0');
            while (--p > Temp) {
               if (*p == ')') {
                  *p = '\0';
                  break;
               }
            }
            strcpy (name, Temp);
            strcpy (Temp, text);
            if ((p = strtok (Temp, " ")) != NULL)
               strcpy (address, p);
         }
         else {
            strcpy (Temp, text);
            if ((p = strtok (Temp, " ")) != NULL)
               strcpy (name, p);
         }
      }
   }
   else if ((p = strchr (Temp, '<')) != NULL) {
      *p++ = '\0';
      if ((a = strchr (p, '>')) != NULL)
         *a = '\0';
      strcpy (address, p);
      p = Temp;
      while (*p == ' ')
         p++;
      if (*p == '"')
         strcpy (Temp, ++p);
      p = strchr (Temp, '\0');
      while (--p > Temp) {
         if (*p != ' ' && *p != '"')
            break;
         *p = '\0';
      }
      strcpy (name, Temp);
   }
   else if (strchr (Temp, '@') != NULL) {
      if ((p = strtok (Temp, " ,")) != NULL) {
         if (address != NULL)
            strcpy (address, p);
      }
   }
}

