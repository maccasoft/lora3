
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

class TMsgAreaSelect : public TListings
{
public:
   class  TConfig *Cfg;
   class  TLanguage *Language;

   VOID   Begin (VOID);
   VOID   PrintCursor (USHORT y);
   VOID   PrintLine (VOID);
   VOID   PrintTitles (VOID);
   VOID   RemoveCursor (USHORT y);
   VOID   Select (VOID);
   VOID   Tag (VOID);

private:
};

// ----------------------------------------------------------------------

TOffline::TOffline (void)
{
   Cfg = NULL;
   Log = NULL;
   Embedded = NULL;
   User = NULL;
   Language = NULL;
   Progress = NULL;

   CarrierSpeed = 19200L;

   Total = TotalPersonal = 0L;
   Limit = 0L;
   strcpy (Id, "offline");
   Msg = NULL;
}

TOffline::~TOffline (void)
{
   if (User != NULL) {
      sprintf (Work, "%s%s", Cfg->TempPath, User->MailBox);
      rmdir (AdjustPath (Work));
   }
   if (Cfg != NULL && User != NULL) {
      sprintf (Path, "%s%s", Cfg->UsersHomePath, User->MailBox);
      rmdir (AdjustPath (Path));
   }
}

VOID TOffline::AddKludges (class TCollection &Text, class TMsgData *Data)
{
   FILE *fp;
   int i, max;
   CHAR Temp[128], Origin[128], *p;

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

   if (Data->EchoMail == TRUE) {
      if (Data->Address[0] == '\0') {
         Cfg->MailAddress.First ();
         sprintf (Temp, "\001MSGID: %s %08lx", Cfg->MailAddress.String, time (NULL));
      }
      else
         sprintf (Temp, "\001MSGID: %s %08lx", Data->Address, time (NULL));
      p = (PSZ)Text.First ();
      Text.Insert (Temp);
      if (p != NULL) {
         Text.Insert (p, (USHORT)(strlen (p) + 1));
         Text.First ();
         Text.Remove ();
      }

      sprintf (Temp, "\001PID: %s", NAME_OS);
      Text.Insert (Temp);

      Text.Add ("");
      sprintf (Temp, "--- %s v%s", NAME, VERSION);
      Text.Add (Temp);
      if (Data->Address[0] == '\0')
         sprintf (Temp, " * Origin: %s (%s)", Origin, Cfg->MailAddress.String);
      else
         sprintf (Temp, " * Origin: %s (%s)", Origin, Data->Address);
      Text.Add (Temp);
   }
}

VOID TOffline::ManageTagged (VOID)
{
   USHORT Line, Tagged;
   CHAR Areas[32], *p;
   class TMsgData *Data;
   class TMsgTag *MsgTag = User->MsgTag;
   class TMsgAreaSelect *List;

   Embedded->BufferedPrintf ("\n\026\001\017Please enter the area numbers to tag/untag.\n");

   while (Embedded->AbortSession () == FALSE) {
      Embedded->BufferedPrintf ("\026\001\013[Enter area(s) to tag, \"?\"=list areas, \"-\"=delete all, <enter>=Quit]\n");
      Embedded->BufferedPrintf ("\026\001\017Tag area: ");
      Embedded->Input (Areas, (USHORT)(sizeof (Areas) - 1), 0);

      if (Areas[0] == '-') {
         if (MsgTag->First () == TRUE)
            do {
               MsgTag->Tagged = FALSE;
               MsgTag->Update ();
            } while (MsgTag->Next () == TRUE);
         Embedded->Printf ("\026\001\017Done!\n");
      }
      else if (Areas[0] == '?') {
         if (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE) {
            if ((List = new TMsgAreaSelect) != NULL) {
               List->Cfg = Cfg;
               List->Embedded = Embedded;
               List->Log = Log;
               List->User = User;
               List->Language = Language;
               List->Run ();
               delete List;
            }
         }
         else {
            if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
               if (Data->First () == TRUE) {
                  Embedded->BufferedPrintf ("\x0C");
                  Embedded->BufferedPrintf ("\n\026\001\017 Area             Msgs   Description\n \031Ф\017  \031Ф\005  \031Ф\066\n");
                  Line = 4;

                  do {
                     Tagged = FALSE;
                     if (User->MsgTag->Read (Data->Key) == TRUE)
                        Tagged = User->MsgTag->Tagged;

                     if (Tagged == FALSE)
                        Embedded->BufferedPrintf ("\026\001\015 %-15.15s  \026\001\002%5ld  \026\001\003%.54s\n", Data->Key, Data->ActiveMsgs, Data->Display);
                     else
                        Embedded->BufferedPrintf ("\026\001\016*\026\001\015%-15.15s\026\001\016* \026\001\002%5ld  \026\001\003%.54s\n", Data->Key, Data->ActiveMsgs, Data->Display);
                     if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                        Embedded->BufferedPrintf ("\x0C");
                        Embedded->BufferedPrintf ("\n\026\001\017 Area             Msgs   Description\n \031Ф\017  \031Ф\005  \031Ф\066\n");
                        Line = 4;
                     }
                  } while (Data->Next () == TRUE && Line != 0);

                  if (Line > 4)
                     Embedded->MoreQuestion (99);
               }
               delete Data;
            }
            Embedded->Printf ("\n\026\001\016Areas marked with an \"*\" are currently tagged.\n");
         }
      }
      else if (Areas[0] == '\0')
         break;
      else if ((p = strtok (Areas, " ")) != NULL)
         do {
            if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
               if (Data->Read (p) == TRUE) {
                  if (User->Level >= Data->Level) {
                     if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                        if (MsgTag->Read (Data->Key) == FALSE) {
                           MsgTag->New ();
                           strcpy (MsgTag->Area, Data->Key);
                           MsgTag->Tagged = TRUE;
                           MsgTag->Add ();
                           Embedded->Printf ("\026\001\016Area \026\001\012%s \026\001\016tagged.\n", MsgTag->Area);
                        }
                        else {
                           MsgTag->Tagged = (MsgTag->Tagged == TRUE) ? FALSE : TRUE;
                           MsgTag->Update ();
                           if (MsgTag->Tagged == FALSE)
                              Embedded->Printf ("\026\001\016Area \026\001\012%s \026\001\016untagged.\n", MsgTag->Area);
                           else
                              Embedded->Printf ("\026\001\016Area \026\001\012%s \026\001\016tagged.\n", MsgTag->Area);
                        }
                     }
                  }
               }
               else
                  Embedded->Printf ("\026\001\014That area doesn't exist!.\n");
               delete Data;
            }
         } while ((p = strtok (NULL, " ")) != NULL);
   }
}

VOID TOffline::AddConference (VOID)
{
   USHORT Found = FALSE, Line;
   CHAR Area[16];
   class TMsgData *Data;
   class TMsgAreaSelect *List;

   if (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE) {
      if ((List = new TMsgAreaSelect) != NULL) {
         List->Cfg = Cfg;
         List->Embedded = Embedded;
         List->Log = Log;
         List->User = User;
         List->Language = Language;
         List->Run ();
         delete List;
      }
   }
   else {
      if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
         do {
            Embedded->Printf ("\n\026\001\013Enter the name of the Conference, or ? for a list: \026\001\x1E");
            Embedded->Input (Area, (USHORT)(sizeof (Area) - 1), INP_FIELD);

            if (!stricmp (Area, "?")) {
               if (Data->First () == TRUE) {
                  Embedded->Printf ("\n\026\001\012Conference       Msgs   Description\n\026\001\017\031Ф\017  \031Ф\005  \031Ф\067\n");
                  Line = 3;
                  do {
                     if (Data->Offline == TRUE && User->Level >= Data->Level) {
                        if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                           Embedded->Printf ("\026\001\013%-15.15s  \026\001\016%5ld  %.55s\n", Data->Key, Data->ActiveMsgs, Data->Display);
                           Line = Embedded->MoreQuestion (Line);
                        }
                     }
                  } while (Embedded->AbortSession () == FALSE && Data->Next () == TRUE && Line != 0);
               }
            }
            else if (Area[0] != '\0') {
               if (Data->Read (Area) == TRUE) {
                  if (User->Level >= Data->Level) {
                     if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                        if (User->MsgTag->Read (Area) == FALSE) {
                           User->MsgTag->New ();
                           strcpy (User->MsgTag->Area, Area);
                           User->MsgTag->Tagged = TRUE;
                           User->MsgTag->Add ();
                        }
                        else {
                           User->MsgTag->Tagged = TRUE;
                           User->MsgTag->Update ();
                        }
                        Found = TRUE;
                     }
                  }
               }

               if (Found == FALSE)
                  Embedded->Printf ("\n\x16\x01\x0DSorry, no such Conference is accessible to you.\n");
            }
         } while (Area[0] != '\0' && Found == FALSE && Embedded->AbortSession () == FALSE);

         delete Data;
      }
   }
}

USHORT TOffline::Compress (PSZ pszPacket)
{
   USHORT RetVal = FALSE;
   CHAR Cmd[16], Temp[128];
   class TPacker *Packer;

   Embedded->Printf ("\n\n");

   if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
      if (Packer->First () == TRUE)
         do {
#if defined(__OS2__)
            if (Packer->OS2 == TRUE)
#elif defined(__NT__)
            if (Packer->Windows == TRUE)
#elif defined(__LINUX__)
            if (Packer->Linux == TRUE)
#else
            if (Packer->Dos == TRUE)
#endif
               Embedded->Printf ("  \x16\x01\013%s ... \x16\x01\016%s\n", Packer->Key, Packer->Display);
         } while (Packer->Next () == TRUE);

      do {
         Embedded->Printf ("\n\x16\x01\013Choose a compression option (or RETURN to exit): ");
         Embedded->Input (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
      } while (Cmd[0] != '\0' && Embedded->AbortSession () == FALSE && Packer->Read (Cmd) == FALSE);

      if (Cmd[0] != '\0' && Packer->Read (Cmd) == TRUE) {
         Embedded->Printf ("\n\x16\x01\016Please wait while compressing your mail packet.\n");
         strcpy (Temp, Work);
         strcat (Temp, "*.*");
         if (Log != NULL)
            Log->Write ("#Executing %s", Packer->PackCmd);
         if (Packer->DoPack (pszPacket, Temp) == TRUE)
            RetVal = TRUE;
      }

      delete Packer;
   }

   if (RetVal == TRUE) {
      MsgTag = User->MsgTag;
      if (NewMsgTag.First () == TRUE)
         do {
            if (MsgTag->Read (NewMsgTag.Area) == TRUE) {
               MsgTag->LastRead = NewMsgTag.LastRead;
               MsgTag->Update ();
            }
         } while (NewMsgTag.Next () == TRUE);

      MsgTag->Save ();
   }

   return (RetVal);
}

USHORT TOffline::Create (VOID)
{
   return (FALSE);
}

VOID TOffline::Display (VOID)
{
   class TMsgData *Data;

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      Embedded->Printf ("\n\x16\x01\012You have selected the following Conference(s):\n\x16\x01\x0E");

      if (User->MsgTag->First () == TRUE) {
         Embedded->Printf ("\n\026\001\012Conference       Msgs   Description\n\026\001\017\031Ф\017  \031Ф\005  \031Ф\067\n");
         do {
            if (User->MsgTag->Tagged == TRUE) {
               if (Data->Read (User->MsgTag->Area) == TRUE)
                  Embedded->Printf ("\026\001\013%-15.15s  \026\001\016%5ld  %.55s\n", Data->Key, Data->ActiveMsgs, Data->Display);
            }
         } while (User->MsgTag->Next () == TRUE);
      }

      Embedded->Printf ("\n");
      delete Data;
   }
}

VOID TOffline::Download (PSZ pszFile, PSZ pszName)
{
   USHORT RetVal = FALSE, SelectOK, Loop;
   USHORT WasTagged = FALSE;
   CHAR Cmd[10];
   ULONG DlTime;
   struct stat statbuf;
   class TTransfer *Transfer;

   if (stat (pszFile, &statbuf) == 0) {
      DlTime = (statbuf.st_size / (CarrierSpeed / 10L) + 30L) / 60L;
      if (DlTime < 1)
         Embedded->Printf ("\n\x16\x01\012Approximate download time: < 1 minute.\n\n");
      else
         Embedded->Printf ("\n\x16\x01\012Approximate download time: %ld minutes.\n\n", DlTime);

      if (DlTime < Embedded->TimeRemain ()) {
         Embedded->Printf ("  \x16\x01\013M ... \x16\x01\016XMODEM (Checksum/CRC)\n");
         Embedded->Printf ("  \x16\x01\0131 ... \x16\x01\016XMODEM-1K\n");
         Embedded->Printf ("  \x16\x01\013Z ... \x16\x01\016ZMODEM\n");
      }
      Embedded->Printf ("  \x16\x01\013T ... \x16\x01\016Tag file(s) for later download\n");

      SelectOK = FALSE;

      do {
         Embedded->Printf ("\n\x16\x01\013Choose a download option (or RETURN to exit): ");
         if (Embedded->HotKey == TRUE)
            Embedded->Input (Cmd, 1, INP_HOTKEY);
         else
            Embedded->Input (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
         Cmd[0] = (CHAR)toupper (Cmd[0]);
         if (Cmd[0] == 'M' || Cmd[0] == '1')
            SelectOK = TRUE;
         else if (Cmd[0] == '\0' || Cmd[0] == 'Z' || Cmd[0] == 'T')
            SelectOK = TRUE;
      } while (Embedded->AbortSession () == FALSE && SelectOK == FALSE);

      if (Cmd[0] != '\0' && (Transfer = new TTransfer) != NULL) {
         Transfer->Com = Embedded->Com;
         Transfer->Log = Log;
         Transfer->Speed = CarrierSpeed;
         Transfer->Progress = Progress;
         Transfer->Telnet = Cfg->ZModemTelnet;

         Loop = TRUE;
         while (Loop == TRUE && RetVal == FALSE && Embedded->AbortSession () == FALSE) {
            if (DlTime < Embedded->TimeRemain ()) {
               if (Cmd[0] != 'T')
                  Embedded->Printf ("\n\x16\x01\012(Hit \x16\x01\013CTRL-X \x16\x01\012a few times to abort)\n");

               if (Cmd[0] == '1') {
                  Embedded->Printf ("\x16\x01\012Beginning %s download of the file %s\n", "XMODEM-1K", pszName);
                  RetVal = Transfer->Send1kXModem (pszFile);
               }
               else if (Cmd[0] == 'M') {
                  Embedded->Printf ("\x16\x01\012Beginning %s download of the file %s\n", "XMODEM", pszName);
                  RetVal = Transfer->SendXModem (pszFile);
               }
               else if (Cmd[0] == 'Z') {
                  Embedded->Printf ("\x16\x01\012Beginning %s download of the file %s\n", "ZMODEM", pszName);
                  if ((RetVal = Transfer->SendZModem (pszFile)) == TRUE)
                     Transfer->SendZModem (NULL);
               }
            }

            if (Cmd[0] == 'T') {
               Embedded->Printf ("\n\x16\x01\012You have just tagged:\n\n");
               User->FileTag->New ();
               strcpy (User->FileTag->Name, pszName);
               strcpy (User->FileTag->Area, "Offline-Reader");
               strcpy (User->FileTag->Complete, pszFile);
               User->FileTag->Size = statbuf.st_size;
               User->FileTag->DeleteAfter = TRUE;
               User->FileTag->Add ();
               Embedded->Printf ("\x16\x01\x0A%5d. The file %s in the %s Library\n", User->FileTag->Index, pszName, User->FileTag->Area);
               if (Log != NULL)
                  Log->Write (":Tagged file %s, library %s", pszName, User->FileTag->Area);
               RetVal = TRUE;
               WasTagged = TRUE;
            }

            if (Cmd[0] != 'T' && Embedded->AbortSession () == FALSE) {
               if (RetVal == FALSE) {
                  Embedded->Printf ("\n\n\x16\x01\015*** NO FILES DOWNLOADED ***\n\006\007\006\007");
                  Embedded->Printf ("\n\x16\x01\013Do you want to try to download the file again");
                  if (Embedded->GetAnswer (ASK_DEFYES) == ANSWER_NO)
                     Loop = FALSE;
               }
               else
                  Embedded->Printf ("\n\n\x16\x01\016*** DOWNLOAD COMPLETE ***\n\006\007\006\007");
            }
         }

         delete Transfer;
      }

      if (WasTagged == FALSE)
         unlink (pszFile);
   }
}

USHORT TOffline::FetchReply (VOID)
{
   return (FALSE);
}

VOID TOffline::PackArea (ULONG &ulLast)
{
   ulLast = ulLast;
}

VOID TOffline::PackEMail (ULONG &ulLast)
{
   ulLast = ulLast;
}

USHORT TOffline::Prescan (VOID)
{
   USHORT RetVal = FALSE, Areas;
   ULONG Number;
   class TMsgTag *MsgTag;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   BuildPath (Work);
   sprintf (Path, "%s%s\\", Cfg->UsersHomePath, User->MailBox);
   BuildPath (Path);

   Areas = 0;
   MsgTag = User->MsgTag;

   if (MsgTag->First () == TRUE) {
      Embedded->Printf ("\n\x16\x01\012Forum \x0A Description & Msgs. Pers.\n\x16\x01\x0FФ Ф1 Ф Ф\n");

      MsgTag->First ();
      do {
         if (MsgTag->Tagged == TRUE) {
            if ((MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
               if (MsgArea->Read (MsgTag->Area) == TRUE) {
                  Msg = NULL;
                  if (MsgArea->Storage == ST_JAM)
                     Msg = new JAM (MsgArea->Path);
                  else if (MsgArea->Storage == ST_SQUISH)
                     Msg = new SQUISH (MsgArea->Path);
                  else if (MsgArea->Storage == ST_FIDO)
                     Msg = new FIDOSDM (MsgArea->Path);
                  else if (MsgArea->Storage == ST_ADEPT)
                     Msg = new ADEPT (MsgArea->Path);
                  else if (MsgArea->Storage == ST_HUDSON)
                     Msg = new HUDSON (MsgArea->Path, (UCHAR)MsgArea->Board);

                  Areas++;
                  Current = Personal = 0L;
                  Embedded->Printf ("\x16\x01\x0B%-15.15s \x16\x01\x0E%-49.49s ", MsgArea->Key, MsgArea->Display);

                  if (Msg != NULL) {
                     Number = MsgTag->LastRead;
                     if (Msg->Next (Number) == TRUE) {
                        do {
                           Msg->ReadHeader (Number);
                           if (TooOld (MsgTag->OlderMsg, Msg) == FALSE) {
                              Current++;
                              Total++;
                              if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                                 Personal++;
                                 TotalPersonal++;
                              }
                           }
                           else if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                              Current++;
                              Total++;
                              Personal++;
                              TotalPersonal++;
                           }
#if defined(__OS2__)
                           if ((Total % 16L) == 0L)
                              DosSleep (1L);
#elif defined(__NT__)
                           if ((Total % 16L) == 0L)
                              Sleep (1L);
#endif
                        } while (Msg->Next (Number) == TRUE);
                     }
                  }

                  Embedded->Printf ("%5lu %5lu\n", Current, Personal);

                  if (Msg != NULL) {
                     Msg->Close ();
                     delete Msg;
                  }
               }
               delete MsgArea;
            }
         }
      } while (MsgTag->Next () == TRUE);
   }

   if (Log != NULL)
      Log->Write (":OLR Prescan: %u Areas, %lu Messages", Areas, Total);

   Embedded->Printf ("\n\x16\x01\x0ATotal messages found: %lu\n\n", Total);

   if (Total == 0L)
      Embedded->Printf ("\x16\x01\015No messages found to download!\n\006\007\006\007");
   else {
      if (Limit != 0 && Total > Limit) {
         Embedded->Printf ("\x16\x01\015Too much messages. Only the first %lu will be packed!\n\n\006\007\006\007", Limit);
         Total = Limit;
      }

      Embedded->Printf ("\x16\x01\013Do you want to download these messages");
      if (Embedded->GetAnswer (ASK_DEFYES) == ANSWER_YES) {
         RetVal = TRUE;
         TotalPack = Total;
         BarWidth = 0;
      }
   }

   return (RetVal);
}

VOID TOffline::RemoveArea (VOID)
{
   CHAR Area[16];

   Embedded->Printf ("\n\026\001\013Enter the name of the Conference, or ? for a list: \026\001\x1E");
   Embedded->Input (Area, (USHORT)(sizeof (Area) - 1), INP_FIELD);

   if (Embedded->AbortSession () == FALSE) {
      if (User->MsgTag->Read (Area) == TRUE) {
         User->MsgTag->Tagged = FALSE;
         User->MsgTag->Update ();
      }
      else
         Embedded->Printf ("\n\x16\x01\x0DThere is no such Conference in your offline reader configuration.\n");
   }
}

VOID TOffline::RestrictDate (VOID)
{
   int dd, mm, yy;
   CHAR Temp[32];
   ULONG Restrict;
   struct tm ltm;
   class TMsgTag *MsgTag = User->MsgTag;

   memcpy (&ltm, localtime ((time_t *)&User->LastCall), sizeof (struct tm));
   Embedded->Printf ("\n\026\001\017Enter date of oldest message to pack, or press <enter> for %d-%02d-%d: ", ltm.tm_mday, ltm.tm_mon + 1, ltm.tm_year % 100);
   Embedded->Input (Temp, 10);

   if (Embedded->AbortSession () == FALSE) {
      Restrict = User->LastCall;
      if (Temp[0] != '\0') {
         sscanf (Temp, "%d-%d-%d", &dd, &mm, &yy);
         if (yy < 90)
            yy += 100;
         memset (&ltm, 0, sizeof (struct tm));
         ltm.tm_mday = dd;
         ltm.tm_mon = mm - 1;
         ltm.tm_year = yy;
         Restrict = mktime (&ltm);
      }

      if (MsgTag->First () == TRUE)
         do {
            if (MsgTag->Tagged == TRUE) {
               MsgTag->LastRead = 0L;
               MsgTag->OlderMsg = Restrict;
               MsgTag->Update ();
            }
         } while (MsgTag->Next () == TRUE);
   }
}

USHORT TOffline::TooOld (ULONG Restrict, class TMsgBase *Msg)
{
   USHORT RetVal = FALSE;
   struct tm ltm;

   memset (&ltm, 0, sizeof (struct tm));
   ltm.tm_mday = Msg->Written.Day;
   ltm.tm_mon = Msg->Written.Month - 1;
   ltm.tm_year = Msg->Written.Year - 1900;
   if (mktime (&ltm) < Restrict)
      RetVal = TRUE;

   return (RetVal);
}

VOID TOffline::Scan (PSZ pszKey, ULONG ulLast)
{
   class TMsgData *MsgArea;
   class TMsgBase *Msg = NULL;

   Current = Personal = 0L;

   if ((Limit == 0 || Total < Limit) && (MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (MsgArea->Read (pszKey) == TRUE) {
         if (MsgArea->Storage == ST_JAM)
            Msg = new JAM (MsgArea->Path);
         else if (MsgArea->Storage == ST_SQUISH)
            Msg = new SQUISH (MsgArea->Path);
         else if (MsgArea->Storage == ST_FIDO)
            Msg = new FIDOSDM (MsgArea->Path);
         else if (MsgArea->Storage == ST_ADEPT)
            Msg = new ADEPT (MsgArea->Path);
         else if (MsgArea->Storage == ST_HUDSON)
            Msg = new HUDSON (MsgArea->Path, (UCHAR)MsgArea->Board);

         if (Msg != NULL) {
            if (Msg->Next (ulLast) == TRUE) {
               do {
                  Msg->ReadHeader (ulLast);

                  Current++;
                  Total++;
                  if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
// ----------------------------------------------------------------------
// If the message is directed to the user, then writes the message
// number and area number in the personal index file too.
// ----------------------------------------------------------------------
                     Personal++;
                     TotalPersonal++;
                  }
               } while (Msg->Next (ulLast) == TRUE);
            }
            Msg->Close ();
            delete Msg;
         }
      }
      delete MsgArea;
   }
}

VOID TOffline::Upload (VOID)
{
   DIR *dir;
   USHORT RetVal = FALSE, i;
   CHAR Protocol[10], File[128], *p;
   PSZ Extensions[] = { ".su", ".mo", ".tu", ".we", ".th", ".fr", ".sa" };
   class TTransfer *Transfer;
   class TPacker *Packer;
   struct dirent *ent;
   struct stat statbuf;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   BuildPath (Work);
   sprintf (Path, "%s%s\\", Cfg->UsersHomePath, User->MailBox);
   BuildPath (Path);

   if (RetVal == FALSE) {
      sprintf (File, "%s%s.rep", Path, Id);
      if (stat (File, &statbuf) == 0)
         RetVal = TRUE;
   }
   if (RetVal == FALSE) {
      sprintf (File, "%s%s.new", Path, Id);
      if (stat (File, &statbuf) == 0)
         RetVal = TRUE;
   }

   if (RetVal == FALSE) {
      Embedded->Printf ("\n\x16\x01\012To start uploading %s.REP, type:\n\n", Id);

      Embedded->Printf ("  \x16\x01\013A ... \x16\x01\016ASCII\n");
      Embedded->Printf ("  \x16\x01\013M ... \x16\x01\016XMODEM (Checksum/CRC)\n");
      Embedded->Printf ("  \x16\x01\0131 ... \x16\x01\016XMODEM-1K\n");
      Embedded->Printf ("  \x16\x01\013Z ... \x16\x01\016ZMODEM\n");
//      Embedded->Printf ("  \x16\x01\013F ... \x16\x01\016File Import (existing file)\n");

      Embedded->Printf ("\n\x16\x01\013Choose an upload option, or RETURN to exit: ");
      Embedded->Input (Protocol, 1, INP_HOTKEY);

      if ((Transfer = new TTransfer) != NULL) {
         Transfer->Com = Embedded->Com;
         Transfer->Log = Log;
         Transfer->Speed = CarrierSpeed;
         Transfer->Progress = Progress;
         Transfer->Telnet = Cfg->ZModemTelnet;

         sprintf (File, "%s%s.REP", Path, Id);

         switch (toupper (Protocol[0])) {
            case '1':
               if (Transfer->Receive1kXModem (File) != NULL)
                  RetVal = TRUE;
               break;

            case 'M':
               if (Transfer->ReceiveXModem (File) != NULL)
                  RetVal = TRUE;
               break;

            case 'Z':
               if ((p = Transfer->ReceiveZModem (Path)) != NULL) {
                  RetVal = TRUE;
                  while ((p = Transfer->ReceiveZModem (Path)) != NULL)
                     ;
               }
               break;
         }

         delete Transfer;
      }
   }

   Embedded->Printf ("\n\006\007");

   if (RetVal == TRUE) {
      if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
         sprintf (File, "%s%s.rep", Path, Id);
         if (Packer->CheckArc (File) == TRUE) {
            if (Log != NULL)
               Log->Write ("+Unpacking %s", File);
            if (Packer->DoUnpack (File, Work, "*.*") == FALSE)
               Log->Write ("!  Command returned error: %s", Packer->Error);
            unlink (File);
         }
         sprintf (File, "%s%s.new", Path, Id);
         if (Packer->CheckArc (File) == TRUE) {
            if (Log != NULL)
               Log->Write ("+Unpacking %s", File);
            if (Packer->DoUnpack (File, Work, "*.*") == FALSE)
               Log->Write ("!  Command returned error: %s", Packer->Error);
            unlink (File);
         }
         if ((dir = opendir (Path)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
               for (i = 0; i < 7; i++) {
                  if (strstr (strlwr (ent->d_name), Extensions[i]) != NULL) {
                     sprintf (File, "%s%s", Path, ent->d_name);
                     if (Packer->CheckArc (File) == TRUE) {
                        if (Log != NULL)
                           Log->Write ("+Unpacking %s", File);
                        if (Packer->DoUnpack (File, Work, "*.*") == FALSE)
                           Log->Write ("!  Command returned error: %s", Packer->Error);
                        unlink (File);
                     }
                     break;
                  }
               }
            }
            closedir (dir);
         }
         delete Packer;
      }
   }
}

// ----------------------------------------------------------------------

VOID TMsgAreaSelect::Begin (VOID)
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
                  strcpy (ld.Key, MsgData->Key);
                  ld.ActiveMsgs = MsgData->ActiveMsgs;
                  strcpy (ld.Display, MsgData->Display);
                  Data.Add (&ld, sizeof (LISTDATA));
               }
            }
         } while (MsgData->Next () == TRUE);

      delete MsgData;
   }

   if ((pld = (LISTDATA *)Data.First ()) != NULL) {
      do {
         List.Add (pld->Key, (USHORT)(strlen (pld->Key) + 1));
         i++;
         if (i >= (User->ScreenHeight - 6))
            break;
      } while ((pld = (LISTDATA *)Data.Next ()) != NULL);
   }
}

VOID TMsgAreaSelect::PrintTitles (VOID)
{
   Embedded->Printf ("\x0C");
   Embedded->Printf ("\n\026\001\017 Area             Msgs   Description\n \031Ф\017  \031Ф\005  \031Ф\066\n");

   Embedded->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, " \031Ф\017  \031Ф\005  \031Ф\066\n");

   Embedded->Printf (Language->Text (LNG_FILEAREADESCRIPTION1));
   Embedded->Printf (Language->Text (LNG_FILEAREADESCRIPTION2));

   Embedded->PrintfAt (4, 1, "");
}

VOID TMsgAreaSelect::PrintLine (VOID)
{
   USHORT Tagged = FALSE;

   if (User->MsgTag->Read (pld->Key) == TRUE)
      Tagged = User->MsgTag->Tagged;

   if (Tagged == FALSE)
      Embedded->Printf ("\026\001\015 %-15.15s  \026\001\002%5ld  \026\001\003%.54s\n", pld->Key, pld->ActiveMsgs, pld->Display);
   else
      Embedded->Printf ("\026\001\016*\026\001\015%-15.15s\026\001\016* \026\001\002%5ld  \026\001\003%.54s\n", pld->Key, pld->ActiveMsgs, pld->Display);
}

VOID TMsgAreaSelect::PrintCursor (USHORT y)
{
   Embedded->PrintfAt (y, 2, Language->Text (LNG_MESSAGEAREACURSOR), (PSZ)List.Value ());
}

VOID TMsgAreaSelect::RemoveCursor (USHORT y)
{
   Embedded->PrintfAt (y, 2, Language->Text (LNG_MESSAGEAREAKEY), (PSZ)List.Value ());
}

VOID TMsgAreaSelect::Select (VOID)
{
   RetVal = End = TRUE;
}

VOID TMsgAreaSelect::Tag (VOID)
{
   if (User->MsgTag->Read ((PSZ)List.Value ()) == FALSE) {
      User->MsgTag->New ();
      strcpy (User->MsgTag->Area, (PSZ)List.Value ());
      User->MsgTag->Tagged = TRUE;
      User->MsgTag->Add ();
   }
   else {
      if (User->MsgTag->Tagged == TRUE)
         User->MsgTag->Tagged = FALSE;
      else
         User->MsgTag->Tagged = TRUE;
      User->MsgTag->Update ();
   }

   if (User->MsgTag->Tagged == TRUE)
      Embedded->PrintfAt (y, 1, "\026\001\016*\x16\x01\x70%-15.15s\x16\x01\x0E*", (PSZ)List.Value ());
   else
      Embedded->PrintfAt (y, 1, "\026\001\016 \x16\x01\x70%-15.15s\x16\x01\x0E ", (PSZ)List.Value ());
}

// ----------------------------------------------------------------------

PSZ Extensions[] = {
   ".su0", ".mo0", ".tu0", ".we0", ".th0", ".fr0", ".sa0"
};

TBlueWave::TBlueWave (void)
{
}

TBlueWave::~TBlueWave (void)
{
}

USHORT TBlueWave::Create (VOID)
{
   int fd;
   USHORT RetVal = FALSE;
   CHAR Temp[128], PktName[32];
   struct dosdate_t date;

   Log->Write ("+Preparing BlueWave packet");
   MsgTag = User->MsgTag;
   _dos_getdate (&date);
   sprintf (PktName, "%s%s", Id, Extensions[date.dayofweek]);

   Total = 0L;
   TotalPersonal = 0L;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   sprintf (Path, "%s%s\\", Cfg->UsersHomePath, User->MailBox);

   NewMsgTag.Clear ();

   if (BuildPath (Work) == TRUE) {
      sprintf (Temp, "%s%s.INF", Work, Id);
      if ((fd = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         memset (&Inf, 0, sizeof (Inf));
         Inf.ver = PACKET_LEVEL;
         strcpy ((PSZ)Inf.loginname, User->Name);
         strcpy ((PSZ)Inf.aliasname, User->RealName);
         if (Cfg->MailAddress.First () == TRUE) {
            Inf.zone = Cfg->MailAddress.Zone;
            Inf.net = Cfg->MailAddress.Net;
            Inf.node = Cfg->MailAddress.Node;
            Inf.point = Cfg->MailAddress.Point;
         }
         strcpy ((PSZ)Inf.sysop, Cfg->SysopName);
         strcpy ((PSZ)Inf.systemname, Cfg->SystemName);
         Inf.inf_header_len = sizeof (INF_HEADER);
         Inf.inf_areainfo_len = sizeof (INF_AREA_INFO);
         Inf.mix_structlen = sizeof (MIX_REC);
         Inf.fti_structlen = sizeof (FTI_REC);
         Inf.uses_upl_file = TRUE;
         Inf.can_forward = TRUE;
         strcpy ((PSZ)Inf.packet_id, Id);
         write (fd, &Inf, sizeof (INF_HEADER));

         memset (&AreaInf, 0, sizeof (INF_AREA_INFO));

         if ((MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
            Area = 1;
            if (MsgArea->First () == TRUE)
               do {
                  memset (&AreaInf, 0, sizeof (AreaInf));
                  sprintf ((PSZ)AreaInf.areanum, "%u", Area++);
                  if (strlen (MsgArea->Key) > sizeof (AreaInf.echotag))
                     MsgArea->Key[sizeof (AreaInf.echotag) - 1] = '\0';
                  strcpy ((PSZ)AreaInf.echotag, MsgArea->Key);
                  MsgArea->Display[sizeof (AreaInf.title) - 1] = '\0';
                  strcpy ((PSZ)AreaInf.title, MsgArea->Display);
                  AreaInf.area_flags |= INF_POST|INF_NO_PRIVATE;
                  if (MsgTag->Read (MsgArea->Key) == TRUE)
                     AreaInf.area_flags |= INF_SCANNING;
                  write (fd, &AreaInf, sizeof (AreaInf));
               } while (MsgArea->Next () == TRUE);
            delete MsgArea;
         }

         close (fd);
      }

      if (MsgTag->First () == TRUE)
         do {
            if (MsgTag->Tagged == TRUE)
               RetVal = TRUE;
         } while (RetVal == FALSE && MsgTag->Next () == TRUE);

      if (RetVal == TRUE) {
         Embedded->Printf ("\n\x16\x01\012Preparing %s packet...\n\n", PktName);

         Embedded->Printf ("\x16\x01\0170%%    10%%  20%%   30%%   40%%   50%%   60%%   70%%   80%%   90%%   100%%\n|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|\r");

         if ((MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
            Area = 1;
            if (MsgArea->First () == TRUE)
               do {
                  if (MsgTag->First () == TRUE)
                     do {
                        if (!stricmp (MsgArea->Key, MsgTag->Area) && MsgTag->Tagged == TRUE) {
                           NewMsgTag.New ();
                           strcpy (NewMsgTag.Area, MsgTag->Area);
                           NewMsgTag.LastRead = MsgTag->LastRead;
                           NewMsgTag.Add ();

                           PackArea (NewMsgTag.LastRead);
                           if (Log != NULL)
                              Log->Write (":  Area %s, %ld msgs. (%ld personal)", NewMsgTag.Area, Current, Personal);

                           NewMsgTag.Update ();
                        }
                     } while (MsgTag->Next () == TRUE);
                  Area++;
               } while (MsgArea->Next () == TRUE);
            delete MsgArea;
         }

         if (Log != NULL)
            Log->Write ("*Packed %ld messages (%ld personal)", Total, TotalPersonal);

         if (Total > 0L) {
            if (BuildPath (Path) == TRUE) {
               sprintf (Temp, "%s%s", Path, PktName);
               if (Compress (Temp) == TRUE)
                  Download (Temp, PktName);
            }
         }
      }
   }
   else
      Log->Write ("!Path Error: %s", Work);

   return (RetVal);
}

USHORT TBlueWave::FetchReply (VOID)
{
   FILE *fp;
   int fdh;
   USHORT RetVal = FALSE;
   CHAR Temp[128], *p;
   ULONG t, Number;
   struct tm *ltm;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   BuildPath (Work);

   MsgArea = new TMsgData (Cfg->SystemPath);

   sprintf (Temp, "%s%s.UPL", Work, Id);
   if ((fdh = sopen (Temp, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      read (fdh, &Uplh, sizeof (UPL_HEADER));
      while (read (fdh, &Uplr, sizeof (UPL_REC)) == sizeof (UPL_REC)) {
         if (MsgArea->Read ((PSZ)Uplr.echotag) == TRUE) {
            if (Log != NULL)
               Log->Write (":Message Area: %s - %s", MsgArea->Key, MsgArea->Display);

            Msg = NULL;
            if (MsgArea->Storage == ST_JAM)
               Msg = new JAM (MsgArea->Path);
            else if (MsgArea->Storage == ST_SQUISH)
               Msg = new SQUISH (MsgArea->Path);
            else if (MsgArea->Storage == ST_FIDO)
               Msg = new FIDOSDM (MsgArea->Path);
            else if (MsgArea->Storage == ST_ADEPT)
               Msg = new ADEPT (MsgArea->Path);
            else if (MsgArea->Storage == ST_HUDSON)
               Msg = new HUDSON (MsgArea->Path, (UCHAR)MsgArea->Board);

            if (Msg != NULL) {
               Msg->New ();
               if (!stricmp ((PSZ)Uplr.from, User->Name) || !stricmp ((PSZ)Uplr.from, User->RealName))
                  strcpy (Msg->From, (PSZ)Uplr.from);
               else
                  strcpy (Msg->From, User->Name);
               strcpy (Msg->To, (PSZ)Uplr.to);
               strcpy (Msg->Subject, (PSZ)Uplr.subj);
               ltm = localtime ((time_t *)&Uplr.unix_date);
               Msg->Written.Day = (UCHAR)ltm->tm_mday;
               Msg->Written.Month = (UCHAR)(ltm->tm_mon + 1);
               Msg->Written.Year = (USHORT)(ltm->tm_year + 1900);
               Msg->Written.Hour = (UCHAR)ltm->tm_hour;
               Msg->Written.Minute = (UCHAR)ltm->tm_min;
               Msg->Written.Second = (UCHAR)ltm->tm_sec;
               t = time (NULL);
               ltm = localtime ((time_t *)&t);
               Msg->Arrived.Day = (UCHAR)ltm->tm_mday;
               Msg->Arrived.Month = (UCHAR)(ltm->tm_mon + 1);
               Msg->Arrived.Year = (USHORT)(ltm->tm_year + 1900);
               Msg->Arrived.Hour = (UCHAR)ltm->tm_hour;
               Msg->Arrived.Minute = (UCHAR)ltm->tm_min;
               Msg->Arrived.Second = (UCHAR)ltm->tm_sec;

               sprintf (Temp, "%s%s", Work, Uplr.filename);
               if ((fp = _fsopen (Temp, "rt", SH_DENYNO)) != NULL) {
                  while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                     while ((p = strchr (Temp, '\n')) != NULL)
                        *p = '\0';
                     while ((p = strchr (Temp, '\r')) != NULL)
                        *p = '\0';
                     Msg->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                  }
                  fclose (fp);

                  if (MsgArea->EchoMail == TRUE)
                     AddKludges (Msg->Text, MsgArea);

                  Msg->Add ();
                  MsgArea->ActiveMsgs++;
                  MsgArea->Update ();

                  Number = Msg->Highest ();
                  Log->Write (":Written message #%lu (%lu)", Msg->UidToMsgn (Number), Number);
                  Embedded->Printf ("\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%lu WRITTEN TO DISK >>>\n\006\007", Msg->UidToMsgn (Number));
               }

               Msg->Close ();
               delete Msg;

               sprintf (Temp, "%s%s", Work, Uplr.filename);
               unlink (Temp);
            }
         }
      }
      close (fdh);
   }

   if (MsgArea != NULL)
      delete MsgArea;

   sprintf (Temp, "%s%s.UPL", Work, Id);
   unlink (Temp);

   return (RetVal);
}

VOID TBlueWave::PackArea (ULONG &ulLast)
{
   int fdm, fdfti, fdmix;
   CHAR Temp[128], *Text;
   ULONG Number;

   Number = ulLast;
   Current = Personal = 0L;

   if (Limit == 0 || Total < Limit) {
      if (MsgArea->Storage == ST_JAM)
         Msg = new JAM (MsgArea->Path);
      else if (MsgArea->Storage == ST_SQUISH)
         Msg = new SQUISH (MsgArea->Path);
      else if (MsgArea->Storage == ST_FIDO)
         Msg = new FIDOSDM (MsgArea->Path);
      else if (MsgArea->Storage == ST_ADEPT)
         Msg = new ADEPT (MsgArea->Path);
      else if (MsgArea->Storage == ST_HUDSON)
         Msg = new HUDSON (MsgArea->Path, (UCHAR)MsgArea->Board);

      sprintf (Temp, "%s%s.fti", Work, Id);
      fdfti = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, SH_DENYNO, S_IREAD|S_IWRITE);
      lseek (fdfti, 0L, SEEK_END);

      sprintf (Temp, "%s%s.mix", Work, Id);
      fdmix = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, SH_DENYNO, S_IREAD|S_IWRITE);
      lseek (fdmix, 0L, SEEK_END);

      sprintf (Temp, "%s%s.dat", Work, Id);
      fdm = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, SH_DENYNO, S_IREAD|S_IWRITE);
      lseek (fdm, 0L, SEEK_END);

      memset (&Mix, 0, sizeof (MIX_REC));
      sprintf ((PSZ)Mix.areanum, "%u", Area);
      Mix.msghptr = tell (fdfti);

      if (Msg != NULL && fdfti != -1 && fdmix != -1 && fdm != -1) {
         Msg->Lock (0L);
         if (Msg->Next (Number) == TRUE) {
            do {
               Msg->ReadHeader (Number);
               if (TooOld (MsgTag->OlderMsg, Msg) == FALSE || !stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                  Msg->Read (Number);

                  Current++;
                  Total++;
                  if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                     Personal++;
                     TotalPersonal++;
                  }

                  memset (&Fti, 0, sizeof (FTI_REC));

                  Msg->From[sizeof (Fti.from) - 1] = '\0';
                  strcpy ((PSZ)Fti.from, Msg->From);
                  Msg->To[sizeof (Fti.to) - 1] = '\0';
                  strcpy ((PSZ)Fti.to, Msg->To);
                  Msg->Subject[sizeof (Fti.subject) - 1] = '\0';
                  strcpy ((PSZ)Fti.subject, Msg->Subject);
                  sprintf ((PSZ)Fti.date, "%2d %.3s %2d %2d:%02d:%02d", Msg->Written.Day, Language->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Second);
                  Fti.msgnum = (tWORD)Msg->UidToMsgn (Number);
                  Fti.msgptr = tell (fdm);
                  if (Cfg->MailAddress.First () == TRUE) {
                     Fti.orig_zone = Cfg->MailAddress.Zone;
                     Fti.orig_net = Cfg->MailAddress.Net;
                     Fti.orig_node = Cfg->MailAddress.Node;
                  }

                  Fti.msglength += write (fdm, " ", 1);

                  if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                     do {
                        if (Text[0] != 0x01 && strnicmp (Text, "SEEN-BY: ", 9)) {
                           Fti.msglength += write (fdm, Text, strlen (Text));
                           Fti.msglength += write (fdm, "\r\n", 2);
                        }
                     } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);

                  write (fdfti, &Fti, sizeof (Fti));
#if defined(__OS2__)
                  if ((Total % 16L) == 0L)
                     DosSleep (1L);
#elif defined(__NT__)
                  if ((Total % 16L) == 0L)
                     Sleep (1L);
#endif
                  if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                     BarWidth = (USHORT)((Total * 61L) / TotalPack);
                     Embedded->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
                  }
               }
            } while ((Limit == 0 || Total < Limit) && Msg->Next (Number) == TRUE);
         }
         Msg->UnLock ();
      }

      Mix.totmsgs = (tWORD)Current;
      Mix.numpers = (tWORD)Personal;
      write (fdmix, &Mix, sizeof (Mix));

      if (Msg != NULL) {
         Msg->Close ();
         delete Msg;
      }
      if (fdfti != -1)
         close (fdfti);
      if (fdmix != -1)
         close (fdmix);
      if (fdm != -1)
         close (fdm);
   }

   ulLast = Number;
}

// ----------------------------------------------------------------------

TQWK::TQWK (void)
{
}

TQWK::~TQWK (void)
{
}

USHORT TQWK::Create (VOID)
{
   int i;
   FILE *fp;
   USHORT RetVal = FALSE;
   CHAR Temp[128], PktName[32];
   struct dosdate_t date;
   struct dostime_t dtime;

   if (Log != NULL)
      Log->Write ("+Preparing QWK packet");
   MsgTag = User->MsgTag;
   sprintf (PktName, "%s.QWK", Id);

   Total = 0L;
   TotalPersonal = 0L;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   sprintf (Path, "%s%s\\", Cfg->UsersHomePath, User->MailBox);

   if (BuildPath (Work) == TRUE) {
      if (MsgTag->First () == TRUE)
         do {
            if (MsgTag->Tagged == TRUE)
               RetVal = TRUE;
         } while (RetVal == FALSE && MsgTag->Next () == TRUE);

      if (RetVal == TRUE) {
         Embedded->Printf ("\n\x16\x01\012Preparing %s packet...\n\n", PktName);

         Embedded->Printf ("\x16\x01\0170%%    10%%  20%%   30%%   40%%   50%%   60%%   70%%   80%%   90%%   100%%\n|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|\r");
         Area = 1;
         NewMsgTag.Clear ();
         Blocks = 2L;

         if ((MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
            if (MsgTag->First () == TRUE)
               do {
                  if (MsgTag->Tagged == TRUE) {
                     if (MsgArea->Read (MsgTag->Area) == TRUE) {
                        strcpy (NewMsgTag.Area, MsgTag->Area);
                        NewMsgTag.LastRead = MsgTag->LastRead;
                        NewMsgTag.Add ();

                        PackArea (NewMsgTag.LastRead);
                        if (Log != NULL)
                           Log->Write (":  Area %s, %ld msgs. (%ld personal)", NewMsgTag.Area, Current, Personal);

                        NewMsgTag.Update ();
                     }
                     Area++;
                  }
               } while (MsgTag->Next () == TRUE);

            delete MsgArea;
         }

         sprintf (Temp, "%sCONTROL.DAT", Work);
         if ((fp = _fsopen (Temp, "wt", SH_DENYNO)) != NULL) {
            fprintf (fp, "%s\n", Cfg->SystemName);
            fprintf (fp, " \n");
            fprintf (fp, " \n");
            fprintf (fp, "%s\n", Cfg->SysopName);
            fprintf (fp, "00000,%s\n", Id);
            _dos_getdate (&date);
            _dos_gettime (&dtime);
            fprintf (fp, "%02d-%02d-%04d,%02d:%02d:%02d\n", date.month, date.day, date.year, dtime.hour, dtime.minute, dtime.second);
            strcpy (Temp, User->Name);
            fprintf (fp, "%s\n", strupr (Temp));
            fprintf (fp, " \n");
            fprintf (fp, "0\n");
            fprintf (fp, "%lu\n", Total);

            i = 0;
            if (MsgTag->First () == TRUE)
               do {
                  if (MsgTag->Tagged == TRUE)
                     i++;
               } while (MsgTag->Next () == TRUE);
            fprintf (fp, "%d\n", i - 1);

            i = 1;
            if (MsgTag->First () == TRUE)
               do {
                  if (MsgTag->Tagged == TRUE)
                     fprintf (fp, "%d\n%s\n", i++, MsgTag->Area);
               } while (MsgTag->Next () == TRUE);

            fprintf (fp, "WELCOME\n");
            fprintf (fp, "NEWS\n");
            fprintf (fp, "GOODBYE\n");

            fclose (fp);
         }

         sprintf (Temp, "%sDOOR.ID", Work);
         if ((fp = _fsopen (Temp, "wt", SH_DENYNO)) != NULL) {
            fprintf (fp, "DOOR = %s\n", NAME);
            fprintf (fp, "VERSION = %s\n", VERSION);
            fprintf (fp, "SYSTEM = %s\n", NAME);
            fprintf (fp, "CONTROLNAME = LoraQWK\n");
            fprintf (fp, "CONTROLTYPE = ADD\n");
            fprintf (fp, "CONTROLTYPE = DROP\n");
            fclose (fp);
         }

         Log->Write ("*Packed %ld messages (%ld personal)", Total, TotalPersonal);

         if (Total > 0L) {
            if (BuildPath (Path) == TRUE) {
               sprintf (Temp, "%s%s", Path, PktName);
               if (Compress (Temp) == TRUE)
                  Download (Temp, PktName);
            }
         }
      }
   }
   else
      Log->Write ("!Path Error: %s", Work);

   return (RetVal);
}

USHORT TQWK::FetchReply (VOID)
{
   int fdh;
   USHORT RetVal = FALSE, i, r, x, nCol, nWidth, nReaded, nRec;
   CHAR Temp[128], szLine[132], szWrp[132], *pLine, *pBuff;
   ULONG t, Number;
   struct tm *ltm;
   class TMsgTag *MsgTag = User->MsgTag;
   class TCollection MsgText;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   BuildPath (Work);

   nWidth = (USHORT)(sizeof (szLine) - 1);
   MsgArea = new TMsgData (Cfg->SystemPath);

   sprintf (Temp, "%s%s.msg", Work, Id);
   if ((fdh = sopen (Temp, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      read (fdh, Temp, 128);
      Temp[8] = '\0';

      while (read (fdh, &Qwk, sizeof (Qwk)) == sizeof (Qwk)) {
         Msg = NULL;

         Area = 1;
         strcpy (Temp, StripSpaces ((PSZ)Qwk.Msgnum, (USHORT)sizeof (Qwk.Msgnum)));

         if (MsgTag->First () == TRUE)
            do {
               if (MsgTag->Tagged == TRUE && Area == (USHORT)atoi (Temp)) {
                  if (MsgArea->Read (MsgTag->Area) == TRUE) {
                     if (Log != NULL)
                        Log->Write (":Message Area: %s - %s", MsgArea->Key, MsgArea->Display);

                     Msg = NULL;
                     if (MsgArea->Storage == ST_JAM)
                        Msg = new JAM (MsgArea->Path);
                     else if (MsgArea->Storage == ST_SQUISH)
                        Msg = new SQUISH (MsgArea->Path);
                     else if (MsgArea->Storage == ST_FIDO)
                        Msg = new FIDOSDM (MsgArea->Path);
                     else if (MsgArea->Storage == ST_ADEPT)
                        Msg = new ADEPT (MsgArea->Path);
                     else if (MsgArea->Storage == ST_HUDSON)
                        Msg = new HUDSON (MsgArea->Path, (UCHAR)MsgArea->Board);
                  }
                  break;
               }
               if (MsgTag->Tagged == TRUE)
                  Area++;
            } while (MsgTag->Next () == TRUE);

         if (Msg == NULL && Log != NULL)
            Log->Write ("!Unknown Forum %s, Skipping", Temp);

         MsgText.Clear ();
         nRec = (USHORT)atoi (StripSpaces ((PSZ)Qwk.Msgrecs, (USHORT)sizeof (Qwk.Msgrecs)));
         pLine = szLine;
         nCol = 0;

         for (r = 1; r < nRec; r++) {
            nReaded = (USHORT)read (fdh, Temp, 128);
            if (r == (USHORT)(nRec - 1)) {
               x = 127;
               while (x > 0 && Temp[x] == ' ') {
                  nReaded--;
                  x--;
               }
            }

            for (i = 0, pBuff = Temp; i < nReaded; i++, pBuff++) {
               if (*pBuff == '\r' || *pBuff == (CHAR)0xE3) {
                  *pLine = '\0';
                  MsgText.Add (szLine);
                  pLine = szLine;
                  nCol = 0;
               }
               else if (*pBuff != '\n') {
                  *pLine++ = *pBuff;
                  nCol++;
                  if (nCol >= nWidth) {
                     *pLine = '\0';
                     while (nCol > 1 && *pLine != ' ') {
                        nCol--;
                        pLine--;
                     }
                     if (nCol > 0) {
                        while (*pLine == ' ')
                           pLine++;
                        strcpy (szWrp, pLine);
                     }
                     *pLine = '\0';
                     MsgText.Add (szLine);
                     strcpy (szLine, szWrp);
                     pLine = strchr (szLine, '\0');
                     nCol = (SHORT)strlen (szLine);
                  }
               }
            }
         }

         if (nCol > 0) {
            *pLine = '\0';
            MsgText.Add (szLine);
         }

         if (MsgArea->EchoMail == TRUE)
            AddKludges (MsgText, MsgArea);

         if (Msg != NULL) {
            Msg->New ();

            strcpy (Msg->From, StripSpaces ((PSZ)Qwk.MsgFrom, (USHORT)sizeof (Qwk.MsgFrom)));
            if (stricmp (Msg->From, User->Name) && stricmp (Msg->From, User->RealName))
               strcpy (Msg->From, User->Name);
            strcpy (Msg->To, StripSpaces ((PSZ)Qwk.MsgTo, (USHORT)sizeof (Qwk.MsgTo)));
            strcpy (Msg->Subject, StripSpaces ((PSZ)Qwk.MsgSubj, (USHORT)sizeof (Qwk.MsgSubj)));

            strcpy (Temp, StripSpaces ((PSZ)Qwk.Msgdate, (USHORT)sizeof (Qwk.Msgdate)));
            Msg->Written.Day = (UCHAR)(atoi (&Temp[3]));
            Msg->Written.Month = (UCHAR)(atoi (Temp));
            Msg->Written.Year = (USHORT)(atoi (&Temp[6]));
            if (Msg->Written.Year < 96)
               Msg->Written.Year += 2000;
            else
               Msg->Written.Year += 1900;

            strcpy (Temp, StripSpaces ((PSZ)Qwk.Msgtime, (USHORT)sizeof (Qwk.Msgtime)));
            Msg->Written.Hour = (UCHAR)(atoi (Temp));
            Msg->Written.Minute = (UCHAR)(atoi (&Temp[3]));
            Msg->Written.Second = 0;

            t = time (NULL);
            ltm = localtime ((time_t *)&t);
            Msg->Arrived.Day = (UCHAR)ltm->tm_mday;
            Msg->Arrived.Month = (UCHAR)(ltm->tm_mon + 1);
            Msg->Arrived.Year = (USHORT)(ltm->tm_year + 1900);
            Msg->Arrived.Hour = (UCHAR)ltm->tm_hour;
            Msg->Arrived.Minute = (UCHAR)ltm->tm_min;
            Msg->Arrived.Second = (UCHAR)ltm->tm_sec;

            Msg->Add (MsgText);
            MsgArea->ActiveMsgs++;
            MsgArea->Update ();

            Number = Msg->Highest ();
            Log->Write (":Written message #%lu (%lu)", Msg->UidToMsgn (Number), Number);
            Embedded->Printf ("\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%lu WRITTEN TO DISK >>>\n\006\007", Msg->UidToMsgn (Number));

            Msg->Close ();
            delete Msg;
         }

         MsgText.Clear ();
      }

      RetVal = TRUE;
      close (fdh);
   }

   if (MsgArea != NULL)
      delete MsgArea;

   sprintf (Temp, "%s%s.msg", Work, Id);
   unlink (Temp);

   return (RetVal);
}

float TQWK::IEEToMSBIN (float f)
{
   short sign, exp;
   QWKCONV t;

   t.f[0] = f;
   sign = (short)(t.uc[3] / 0x80);
   exp = (short)(((t.ui[1] >> 7) - 0x7F + 0x81) & 0xFF);
   t.ui[1] = (USHORT)((t.ui[1] & 0x7F) | (sign << 7) | (exp << 8));

   return (t.f[0]);
}

VOID TQWK::PackArea (ULONG &ulLast)
{
   int fdm, fdi, fdp;
   float out, in;
   CHAR Temp[128], *Text;
   ULONG Number, Pos, Size;

   Number = ulLast;
   Current = Personal = 0L;

   if (Limit == 0 || Total < Limit) {
      Msg = NULL;
      if (MsgArea->Storage == ST_JAM)
         Msg = new JAM (MsgArea->Path);
      else if (MsgArea->Storage == ST_SQUISH)
         Msg = new SQUISH (MsgArea->Path);
      else if (MsgArea->Storage == ST_FIDO)
         Msg = new FIDOSDM (MsgArea->Path);
      else if (MsgArea->Storage == ST_ADEPT)
         Msg = new ADEPT (MsgArea->Path);
      else if (MsgArea->Storage == ST_HUDSON)
         Msg = new HUDSON (MsgArea->Path, (UCHAR)MsgArea->Board);

      sprintf (Temp, "%s%03d.NDX", Work, Area);
      fdi = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);
      lseek (fdi, 0L, SEEK_END);

      sprintf (Temp, "%sPERSONAL.NDX", Work);
      fdp = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, SH_DENYNO, S_IREAD|S_IWRITE);
      lseek (fdp, 0L, SEEK_END);

      sprintf (Temp, "%sMESSAGES.DAT", Work);
      if ((fdm = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         lseek (fdm, 0L, SEEK_END);
         if (tell (fdm) == 0L) {
// ----------------------------------------------------------------------
// The first record of the MESSAGE.DAT file must be the Sparkware id
// block, otherwise some applications may complain.
// ----------------------------------------------------------------------
            write (fdm, "Produced by Qmail...", 20);
            write (fdm, "Copywright (c) 1987 by Sparkware.  ", 35);
            write (fdm, "All Rights Reserved", 19);
            memset (Temp, ' ', 54);
            write (fdm, Temp, 54);
         }
      }

      if (Msg != NULL && fdi != -1 && fdp != -1 && fdm != -1) {
         Msg->Lock (0L);
         if (Msg->Next (Number) == TRUE) {
            do {
               Msg->ReadHeader (Number);
               if (TooOld (MsgTag->OlderMsg, Msg) == FALSE || !stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                  Msg->Read (Number);

                  Current++;
                  Total++;
                  sprintf (Temp, "%lu", Blocks);
                  in = atof (Temp);
                  out = IEEToMSBIN (in);
                  write (fdi, &out, sizeof (float));
                  write (fdi, "", 1);
                  if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                     Personal++;
                     TotalPersonal++;
                     write (fdp, &out, sizeof (float));
                     write (fdp, "", 1);
                  }

                  memset (&Qwk, ' ', sizeof (Qwk));
                  sprintf (Temp, "%-*lu", sizeof (Qwk.Msgnum), Msg->UidToMsgn (Number));
                  memcpy (Qwk.Msgnum, Temp, sizeof (Qwk.Msgnum));
                  sprintf (Temp, "%02d-%02d-%02d", Msg->Written.Month, Msg->Written.Day, Msg->Written.Year % 100);
                  memcpy (Qwk.Msgdate, Temp, sizeof (Qwk.Msgdate));
                  sprintf (Temp, "%02d:%02d", Msg->Written.Hour, Msg->Written.Minute);
                  memcpy (Qwk.Msgtime, Temp, sizeof (Qwk.Msgtime));
                  Msg->From[sizeof (Qwk.MsgFrom) - 1] = '\0';
                  memcpy (Qwk.MsgFrom, Msg->From, strlen (Msg->From));
                  Msg->To[sizeof (Qwk.MsgTo) - 1] = '\0';
                  memcpy (Qwk.MsgTo, Msg->To, strlen (Msg->To));
                  Msg->Subject[sizeof (Qwk.MsgSubj) - 1] = '\0';
                  memcpy (Qwk.MsgSubj, Msg->Subject, strlen (Msg->Subject));
                  Qwk.Msglive = 0xE1;
                  Qwk.Msgarealo = (UCHAR)(Area & 0xFF);
                  Qwk.Msgareahi = (UCHAR)((Area & 0xFF00) >> 8);

                  Pos = tell (fdm);
                  write (fdm, &Qwk, sizeof (Qwk));

                  Size = 128L;
                  if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                     do {
                        if (Text[0] != 0x01 && strnicmp (Text, "SEEN-BY: ", 9)) {
                           Size += (ULONG)write (fdm, Text, strlen (Text));
                           Size += (ULONG)write (fdm, "\xE3", 1);
                        }
                     } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);

                  if ((Size % 128L) != 0) {
                     memset (Temp, ' ', 128);
                     Size += (ULONG)write (fdm, Temp, (int)(128L - (Size % 128L)));
                  }

                  Blocks += (Size / 128L);
                  sprintf (Temp, "%-*lu", sizeof (Qwk.Msgrecs), Size / 128L);
                  memcpy (Qwk.Msgrecs, Temp, sizeof (Qwk.Msgrecs));
                  lseek (fdm, Pos, SEEK_SET);
                  write (fdm, &Qwk, sizeof (Qwk));
                  lseek (fdm, 0L, SEEK_END);
#if defined(__OS2__)
                  if ((Total % 16L) == 0L)
                     DosSleep (1L);
#elif defined(__NT__)
                  if ((Total % 16L) == 0L)
                     Sleep (1L);
#endif
                  if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                     BarWidth = (USHORT)((Total * 61L) / TotalPack);
                     Embedded->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
                  }
               }
            } while ((Limit == 0 || Total < Limit) && Msg->Next (Number) == TRUE);
         }
         Msg->UnLock ();
      }

      if (Msg != NULL) {
         Msg->Close ();
         delete Msg;
      }
      if (fdm != -1)
         close (fdm);
      if (fdi != -1)
         close (fdi);
      if (fdp != -1)
         close (fdp);
   }

   ulLast = Number;
}

PSZ TQWK::StripSpaces (PSZ pszString, USHORT usSize)
{
   USHORT x;

   memcpy (TempStr, pszString, usSize);
   TempStr[usSize] = '\0';
   if ((x = (USHORT)(usSize - 1)) > 0) {
      while (x > 0 && TempStr[x] == ' ')
         TempStr[x--] = '\0';
   }

   return (TempStr);
}

// ----------------------------------------------------------------------

TAscii::TAscii (void)
{
}

TAscii::~TAscii (void)
{
}

USHORT TAscii::Create (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], PktName[32];

   if (Log != NULL)
      Log->Write ("+Preparing Ascii packet");
   MsgTag = User->MsgTag;
   sprintf (PktName, "%s.MSG", Id);

   Total = 0L;
   TotalPersonal = 0L;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   sprintf (Path, "%s%s\\", Cfg->UsersHomePath, User->MailBox);

   if (BuildPath (Work) == TRUE) {
      if (MsgTag->First () == TRUE)
         do {
            if (MsgTag->Tagged == TRUE)
               RetVal = TRUE;
         } while (RetVal == FALSE && MsgTag->Next () == TRUE);

      if (RetVal == TRUE) {
         Embedded->Printf ("\n\x16\x01\012Preparing %s packet...\n\n", PktName);

         Embedded->Printf ("\x16\x01\0170%%    10%%  20%%   30%%   40%%   50%%   60%%   70%%   80%%   90%%   100%%\n|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|\r");
         Area = 1;
         NewMsgTag.Clear ();

         if ((MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
            if (MsgTag->First () == TRUE)
               do {
                  if (MsgTag->Tagged == TRUE) {
                     if (MsgArea->Read (MsgTag->Area) == TRUE) {
                        strcpy (NewMsgTag.Area, MsgTag->Area);
                        NewMsgTag.LastRead = MsgTag->LastRead;
                        NewMsgTag.Add ();

                        PackArea (NewMsgTag.LastRead);
                        if (Log != NULL)
                           Log->Write (":  Area %s, %ld msgs. (%ld personal)", NewMsgTag.Area, Current, Personal);

                        NewMsgTag.Update ();
                        Area++;
                     }
                  }
               } while (MsgTag->Next () == TRUE);

            delete MsgArea;
         }

         if (Log != NULL)
            Log->Write ("*Packed %ld messages (%ld personal)", Total, TotalPersonal);

         if (Total > 0L) {
            if (BuildPath (Path) == TRUE) {
               sprintf (Temp, "%s%s", Path, PktName);
               if (Compress (Temp) == TRUE)
                  Download (Temp, PktName);
            }
         }
      }
   }
   else if (Log != NULL)
      Log->Write ("!Path Error: %s", Work);

   return (RetVal);
}

VOID TAscii::PackArea (ULONG &ulLast)
{
   FILE *fp;
   CHAR Temp[128], *Text;
   ULONG Number;

   Number = ulLast;
   Current = Personal = 0L;

   if (Limit == 0 || Total < Limit) {
      if (MsgArea->Storage == ST_JAM)
         Msg = new JAM (MsgArea->Path);
      else if (MsgArea->Storage == ST_SQUISH)
         Msg = new SQUISH (MsgArea->Path);
      else if (MsgArea->Storage == ST_FIDO)
         Msg = new FIDOSDM (MsgArea->Path);
      else if (MsgArea->Storage == ST_ADEPT)
         Msg = new ADEPT (MsgArea->Path);
      else if (MsgArea->Storage == ST_HUDSON)
         Msg = new HUDSON (MsgArea->Path, (UCHAR)MsgArea->Board);

      sprintf (Temp, "%s%03d.TXT", Work, Area);
      fp = _fsopen (Temp, "wt", SH_DENYNO);

      if (Msg != NULL && fp != NULL) {
         Msg->Lock (0L);
         if (Msg->Next (Number) == TRUE) {
            do {
               Msg->ReadHeader (Number);
               if (TooOld (MsgTag->OlderMsg, Msg) == FALSE || !stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                  Msg->Read (Number);

                  fprintf (fp, "\n==============================================\n    Msg. #%ld of %ld (%s)\n", Msg->UidToMsgn (Number), Msg->Number (), MsgArea->Display);
                  fprintf (fp, "   Date: %d %s %d %2d:%02d\n", Msg->Written.Day, Language->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
                  fprintf (fp, "   From: %s\n", Msg->From);
                  if (Msg->To[0])
                     fprintf (fp, "     To: %s\n", Msg->To);
                  fprintf (fp, "Subject: %s\n----------------------------------------------\n", Msg->Subject);

                  Current++;
                  Total++;
                  if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                     Personal++;
                     TotalPersonal++;
                  }

                  if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                     do {
                        if (Text[0] != 0x01 && strnicmp (Text, "SEEN-BY: ", 9))
                           fprintf (fp, "%s\n", Text);
                     } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);
#if defined(__OS2__)
                  if ((Total % 16L) == 0L)
                     DosSleep (1L);
#elif defined(__NT__)
                  if ((Total % 16L) == 0L)
                     Sleep (1L);
#endif
                  if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                     BarWidth = (USHORT)((Total * 61L) / TotalPack);
                     Embedded->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
                  }
               }
            } while ((Limit == 0 || Total < Limit) && Msg->Next (Number) == TRUE);
         }
         Msg->UnLock ();
      }

      if (Msg != NULL) {
         Msg->Close ();
         delete Msg;
      }
      if (fp != NULL)
         fclose (fp);
   }

   ulLast = Number;
}

// ----------------------------------------------------------------------

TPoint::TPoint (void)
{
}

TPoint::~TPoint (void)
{
}

USHORT TPoint::Create (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], PktName[32];
   struct dosdate_t date;

   if (Log != NULL)
      Log->Write ("+Preparing PointMail packet");
   MsgTag = User->MsgTag;
   _dos_getdate (&date);
   sprintf (PktName, "%08lX%s", StringCrc32 (Id, 0xFFFFFFFFL), Extensions[date.dayofweek]);

   Total = 0L;
   TotalPersonal = 0L;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   sprintf (Path, "%s%s\\", Cfg->UsersHomePath, User->MailBox);

   if (BuildPath (Work) == TRUE) {
      if (MsgTag->First () == TRUE)
         do {
            if (MsgTag->Tagged == TRUE)
               RetVal = TRUE;
         } while (RetVal == FALSE && MsgTag->Next () == TRUE);

      if (RetVal == TRUE) {
         Embedded->Printf ("\n\x16\x01\012Preparing %s packet...\n\n", PktName);

         Embedded->Printf ("\x16\x01\0170%%    10%%  20%%   30%%   40%%   50%%   60%%   70%%   80%%   90%%   100%%\n|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|\r");
         Area = 1;
         NewMsgTag.Clear ();

         if ((MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
            if (MsgTag->First () == TRUE)
               do {
                  if (MsgTag->Tagged == TRUE) {
                     if (MsgArea->Read (MsgTag->Area) == TRUE) {
                        strcpy (NewMsgTag.Area, MsgTag->Area);
                        NewMsgTag.LastRead = MsgTag->LastRead;
                        NewMsgTag.Add ();

                        PackArea (NewMsgTag.LastRead);
                        if (Log != NULL)
                           Log->Write (":  Area %s, %ld msgs. (%ld personal)", NewMsgTag.Area, Current, Personal);

                        NewMsgTag.Update ();
                        Area++;
                     }
                  }
               } while (MsgTag->Next () == TRUE);

            delete MsgArea;
         }

         if (Log != NULL)
            Log->Write ("*Packed %ld messages (%ld personal)", Total, TotalPersonal);

         if (Total > 0L) {
            if (BuildPath (Path) == TRUE) {
               sprintf (Temp, "%s%s", Path, PktName);
               if (Compress (Temp) == TRUE)
                  Download (Temp, PktName);
            }
         }
      }
   }
   else if (Log != NULL)
      Log->Write ("!Path Error: %s", Work);

   return (RetVal);
}

VOID TPoint::PackArea (ULONG &ulLast)
{
   CHAR Temp[128], *Text;
   ULONG Number;
   class PACKET *Packet;

   Number = ulLast;
   Current = Personal = 0L;

   if (Limit == 0 || Total < Limit) {
      if (MsgArea->Storage == ST_JAM)
         Msg = new JAM (MsgArea->Path);
      else if (MsgArea->Storage == ST_SQUISH)
         Msg = new SQUISH (MsgArea->Path);
      else if (MsgArea->Storage == ST_FIDO)
         Msg = new FIDOSDM (MsgArea->Path);
      else if (MsgArea->Storage == ST_ADEPT)
         Msg = new ADEPT (MsgArea->Path);
      else if (MsgArea->Storage == ST_HUDSON)
         Msg = new HUDSON (MsgArea->Path, (UCHAR)MsgArea->Board);

      if ((Packet = new PACKET) != NULL) {
         if (Cfg->MailAddress.First () == TRUE) {
            strcpy (Packet->FromAddress, Cfg->MailAddress.String);
            strcpy (Packet->ToAddress, Cfg->MailAddress.String);
         }
         sprintf (Temp, "%s%08lX.PKT", Work, time (NULL));
         Packet->Open (Temp);
      }

      if (Msg != NULL && Packet != NULL) {
         Msg->Lock (0L);
         if (Msg->Next (Number) == TRUE) {
            do {
               Msg->ReadHeader (Number);
               if (TooOld (MsgTag->OlderMsg, Msg) == FALSE || !stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                  Msg->Read (Number);

                  Current++;
                  Total++;
                  if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                     Personal++;
                     TotalPersonal++;
                  }

                  if ((Text = (CHAR *)Msg->Text.First ()) != NULL)
                     do {
                        if (!strncmp (Text, "SEEN-BY: ", 9) || !strncmp (Text, "\001PATH: ", 7)) {
                           Msg->Text.Remove ();
                           Text = (CHAR *)Msg->Text.Value ();
                        }
                        else
                           Text = (CHAR *)Msg->Text.Next ();
                     } while (Text != NULL);

                  if (Cfg->MailAddress.First () == TRUE) {
                     sprintf (Temp, "SEEN-BY: %u/%u", Cfg->MailAddress.Net, Cfg->MailAddress.Node);
                     Msg->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                     sprintf (Temp, "\001PATH: %u/%u", Cfg->MailAddress.Net, Cfg->MailAddress.Node);
                     Msg->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                  }

                  if ((Text = (CHAR *)Msg->Text.First ()) != NULL) {
                     if (MsgArea->EchoTag[0] != '\0')
                        sprintf (Temp, "AREA:%s", MsgArea->EchoTag);
                     else
                        sprintf (Temp, "AREA:%s", MsgArea->Key);
                     Msg->Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
                     Msg->Text.Insert (Text, (USHORT)(strlen (Text) + 1));
                     Msg->Text.First ();
                     Msg->Text.Remove ();
                  }

                  Packet->Add (Msg);
#if defined(__OS2__)
                  if ((Total % 16L) == 0L)
                     DosSleep (1L);
#elif defined(__NT__)
                  if ((Total % 16L) == 0L)
                     Sleep (1L);
#endif
                  if (BarWidth != (USHORT)((Total * 61L) / TotalPack)) {
                     BarWidth = (USHORT)((Total * 61L) / TotalPack);
                     Embedded->Printf ("\r%.*s", BarWidth, "ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл");
                  }
               }
            } while ((Limit == 0 || Total < Limit) && Msg->Next (Number) == TRUE);
         }
         Msg->UnLock ();
      }

      if (Msg != NULL) {
         Msg->Close ();
         delete Msg;
      }
      if (Packet != NULL) {
         Packet->Close ();
         delete Packet;
      }
   }

   ulLast = Number;
}

USHORT TPoint::FetchReply (VOID)
{
   DIR *dir;
   USHORT RetVal = FALSE, Found;
   CHAR Temp[128], LastTag[64], *Text;
   ULONG Number, Written;
   class PACKET *Packet;
   class dirent *ent;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   BuildPath (Work);

   MsgArea = new TMsgData (Cfg->SystemPath);
   LastTag[0] = '\0';
   Msg = NULL;

   if ((dir = opendir (Work)) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
         if (strstr (ent->d_name, ".pk") != NULL || strstr (ent->d_name, ".PK") != NULL) {
            sprintf (Temp, "%s%s", Work, ent->d_name);
            if ((Packet = new PACKET) != NULL) {
               if (Packet->Open (Temp) == TRUE) {
                  Number = Packet->Lowest ();
                  do {
                     if (Packet->Read (Number) == TRUE) {
                        if ((Text = (CHAR *)Packet->Text.First ()) != NULL) {
                           if (!strncmp (Text, "AREA:", 5)) {
                              if (stricmp (&Text[5], LastTag)) {
                                 if (Msg != NULL) {
                                    Msg->Close ();
                                    delete Msg;
                                    Msg = NULL;
                                 }
                                 strcpy (LastTag, &Text[5]);
                                 Found = FALSE;
                                 if (MsgArea->First () == TRUE)
                                    do {
                                       if (MsgArea->EchoTag[0] != '\0' && !stricmp (MsgArea->EchoTag, LastTag)) {
                                          Found = TRUE;
                                          break;
                                       }
                                       else if (MsgArea->EchoTag[0] == '\0' && !stricmp (MsgArea->Key, LastTag)) {
                                          Found = TRUE;
                                          break;
                                       }
                                    } while (MsgArea->Next () == TRUE);
                                 if (Found == TRUE) {
                                    if (Log != NULL)
                                       Log->Write (":Message Area: %s - %s", MsgArea->Key, MsgArea->Display);

                                    Msg = NULL;
                                    if (MsgArea->Storage == ST_JAM)
                                       Msg = new JAM (MsgArea->Path);
                                    else if (MsgArea->Storage == ST_SQUISH)
                                       Msg = new SQUISH (MsgArea->Path);
                                    else if (MsgArea->Storage == ST_FIDO)
                                       Msg = new FIDOSDM (MsgArea->Path);
                                    else if (MsgArea->Storage == ST_ADEPT)
                                       Msg = new ADEPT (MsgArea->Path);
                                    else if (MsgArea->Storage == ST_HUDSON)
                                       Msg = new HUDSON (MsgArea->Path, (UCHAR)MsgArea->Board);
                                 }
                              }

                              // Toglie la prima linea che contiene il kludge AREA:
                              Packet->Text.Remove ();

                              // Nel caso di un'area echomail, aggiunge tutti i kludges
                              // Necessari.
                              if (MsgArea->EchoMail == TRUE)
                                 AddKludges (Packet->Text, MsgArea);

                              if (Msg != NULL) {
                                 if (stricmp (Packet->From, User->Name) && stricmp (Packet->From, User->RealName))
                                    strcpy (Packet->From, User->Name);
                                 if ((Text = (CHAR *)Packet->Text.First ()) != NULL)
                                    do {
                                       if (!strncmp (Text, "SEEN-BY: ", 9) || !strncmp (Text, "\001PATH: ", 7)) {
                                          Packet->Text.Remove ();
                                          Text = (CHAR *)Packet->Text.Value ();
                                       }
                                       else
                                          Text = (CHAR *)Packet->Text.Next ();
                                    } while (Text != NULL);

                                 Msg->Add (Packet);
                                 MsgArea->ActiveMsgs++;
                                 MsgArea->Update ();

                                 Written = Msg->Highest ();
                                 Log->Write (":Written message #%lu (%lu)", Msg->UidToMsgn (Number), Number);
                                 Embedded->Printf ("\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%lu WRITTEN TO DISK >>>\n\006\007", Msg->UidToMsgn (Number));
                              }
                           }
                        }
                     }
                  } while (Packet->Next (Number) == TRUE);
               }
               Packet->Close ();
               delete Packet;
            }
            sprintf (Temp, "%s%s", Work, ent->d_name);
            unlink (Temp);
         }
      }
      closedir (dir);
   }

   if (Msg != NULL) {
      Msg->Close ();
      delete Msg;
   }
   if (MsgArea != NULL)
      delete MsgArea;

   return (RetVal);
}


