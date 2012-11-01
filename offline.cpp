
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/13/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "offline.h"

TOffline::TOffline (class TBbs *bbs)
{
   Bbs = bbs;
   Cfg = bbs->Cfg;
   Lang = bbs->Lang;
   Log = bbs->Log;
   User = bbs->User;

   sprintf (Work, "%s%s\\", Cfg->TempPath, User->MailBox);
   sprintf (Path, "%s%s\\", Cfg->HomePath, User->MailBox);
   Total = TotalPersonal = 0L;
   Limit = 1000L;
   strcpy (Id, "OFFLINE");
   Msg = NULL;
   NewLR.Clear ();
}

TOffline::~TOffline (void)
{
   CHAR Temp[128];
   struct find_t blk;

   NewLR.Clear ();

// ----------------------------------------------------------------------
// The destructor is responsible for the cleanup of the temporary work
// directory.
// ----------------------------------------------------------------------
   sprintf (Temp, "%s*.*", Work);
   if (!_dos_findfirst (Temp, 0, &blk))
      do {
         sprintf (Temp, "%s%s", Work, blk.name);
         unlink (Temp);
      } while (!_dos_findnext (&blk));

   rmdir (Work);
}

VOID TOffline::AddConference (VOID)
{
   USHORT Found = FALSE;
   CHAR Area[16];
   class TMsgData *Data;

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      do {
         Bbs->Printf ("\n\x16\x01\013Enter a Conference to add to your offline reader, or ? for a list\n : \026\001\x1E");
         Bbs->GetString (Area, (USHORT)(sizeof (Area) - 1), INP_FIELD);

         if (!stricmp (Area, "?")) {
            if (Data->First () == TRUE) {
               Bbs->Printf (Lang->MessageListHeader);
               do {
                  if (Data->ShowGlobal == TRUE && Data->Offline == TRUE && User->Level >= Data->Level) {
                     if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags)
                        Bbs->Printf (Lang->MessageList, Data->Key, Data->ActiveMsgs, Data->Display);
                  }
               } while (Bbs->AbortSession () == FALSE && Data->Next () == TRUE);
            }
         }
         else if (Area[0] != '\0') {
            if (Data->Read (Area) == TRUE) {
               if (Data->ShowGlobal == TRUE && User->Level >= Data->Level) {
                  if ((Data->AccessFlags & User->AccessFlags) == Data->AccessFlags) {
                     if (User->LastRead->Read (Area) == FALSE) {
                        strcpy (User->LastRead->Key, Area);
                        User->LastRead->Number = 0L;
                        User->LastRead->QuickScan = TRUE;
                        User->LastRead->PersonalOnly = FALSE;
                        User->LastRead->ExcludeOwn = FALSE;
                        User->LastRead->Add ();
                     }
                     else {
                        User->LastRead->QuickScan = TRUE;
                        User->LastRead->Update ();
                     }
                     Found = TRUE;
                  }
               }
            }

            if (Found == FALSE)
               Bbs->Printf ("\n\x16\x01\x0DSorry, no such Conference is accessible to you.\n");
         }
      } while (Area[0] != '\0' && Found == FALSE && Bbs->AbortSession () == FALSE);

      delete Data;
   }
}

USHORT TOffline::BuildPath (PSZ pszPath)
{
   USHORT RetVal = TRUE;
   CHAR Dir[128], *p, Final[128];
   struct find_t blk;

   strcpy (Final, "");
   strcpy (Dir, pszPath);

// ----------------------------------------------------------------------
// Build a complete directory tree by extracting the single directory
// names from the pathname.
// ----------------------------------------------------------------------
   if ((p = strtok (Dir, "\\")) != NULL)
      do {
         strcat (Final, p);
         if (p[1] != ':' && p[0] != '.') {
// ----------------------------------------------------------------------
// If the directory cannot be created, stops the process immediately.
// The global variable errno is set to zero after an error only if the
// directory already exists.
// ----------------------------------------------------------------------
            mkdir (Final);
//            if (mkdir (Final) != 0 && errno != 0)
//               RetVal = FALSE;
         }
         strcat (Final, "\\");
      } while (RetVal == TRUE && (p = strtok (NULL, "\\")) != NULL);

   if (RetVal == TRUE) {
// ----------------------------------------------------------------------
// The directory was successfully created, now we clean the directory
// just in case it was already here with some trash in it.
// ----------------------------------------------------------------------
      sprintf (Dir, "%s*.*", pszPath);
      if (!_dos_findfirst (Dir, 0, &blk))
         do {
            sprintf (Final, "%s%s", pszPath, blk.name);
            unlink (Final);
         } while (!_dos_findnext (&blk));
   }

   return (RetVal);
}

USHORT TOffline::Compress (PSZ pszPacket)
{
   USHORT RetVal = FALSE;
   CHAR Cmd[16], Temp[128];
   class TPacker *Packer;

   Bbs->Printf ("\n\n");

   if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
      if (Packer->First () == TRUE)
         do {
            Bbs->Printf ("  \x16\x01\013%s ... \x16\x01\016%s\n", Packer->Key, Packer->Display);
         } while (Packer->Next () == TRUE);

      do {
         Bbs->Printf ("\n\x16\x01\013Choose a compression option (or RETURN to exit): ");
         Bbs->GetString (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
      } while (Cmd[0] != '\0' && Bbs->AbortSession () == FALSE && Packer->Read (Cmd) == FALSE);

      if (Cmd[0] != '\0' && Packer->Read (Cmd) == TRUE) {
         Bbs->Printf ("\n\x16\x01\016Please wait while compressing your mail packet.\n");
         strcpy (Temp, Work);
         strcat (Temp, "*.*");
         if (Packer->DoPack (pszPacket, Temp) == TRUE)
            RetVal = TRUE;
      }

      delete Packer;
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
      Bbs->Printf ("\n\x16\x01\012You have selected the following Conference(s):\n\x16\x01\x0E");

      if (User->LastRead->First () == TRUE) {
         Bbs->Printf (Lang->MessageListHeader);
         do {
            if (User->LastRead->QuickScan == TRUE) {
               if (Data->Read (User->LastRead->Key) == TRUE)
                  Bbs->Printf (Lang->MessageList, Data->Key, Data->ActiveMsgs, Data->Display);
            }
         } while (User->LastRead->Next () == TRUE);
      }

      Bbs->Printf ("\n");
      delete Data;
   }
}

VOID TOffline::Download (PSZ pszFile, PSZ pszName)
{
   USHORT RetVal = FALSE, SelectOK, Loop;
   CHAR Cmd[10];
   ULONG DlTime;
   struct find_t blk;
   class TTransfer *Transfer;

   if (!_dos_findfirst (pszFile, 0, &blk)) {
      DlTime = (blk.size / (Bbs->CarrierSpeed / 10L) + 30L) / 60L;
      if (DlTime < 1)
         Bbs->Printf ("\n\x16\x01\012Approximate download time: < 1 minute.\n\n");
      else
         Bbs->Printf ("\n\x16\x01\012Approximate download time: %ld minutes.\n\n", DlTime);

      Bbs->Printf ("  \x16\x01\013M ... \x16\x01\016XMODEM (Checksum/CRC)\n");
      Bbs->Printf ("  \x16\x01\0131 ... \x16\x01\016XMODEM-1K\n");
      Bbs->Printf ("  \x16\x01\013Z ... \x16\x01\016ZMODEM\n");
      Bbs->Printf ("  \x16\x01\013T ... \x16\x01\016Tag file(s) for later download\n");

      SelectOK = FALSE;

      do {
         Bbs->Printf ("\n\x16\x01\013Choose a download option (or RETURN to exit): ");
         if (Bbs->HotKey == TRUE)
            Bbs->GetString (Cmd, 1, INP_HOTKEY);
         else
            Bbs->GetString (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
         Cmd[0] = (CHAR)toupper (Cmd[0]);
         if (Cmd[0] == 'M' || Cmd[0] == '1')
            SelectOK = TRUE;
         else if (Cmd[0] == '\0' || Cmd[0] == 'Z' || Cmd[0] == 'T')
            SelectOK = TRUE;
      } while (Bbs->AbortSession () == FALSE && SelectOK == FALSE);

      if (Cmd[0] != '\0' && (Transfer = new TTransfer (Bbs)) != NULL) {
         Loop = TRUE;
         while (Loop == TRUE && RetVal == FALSE) {
            if (Cmd[0] != 'T')
               Bbs->Printf ("\n\x16\x01\012(Hit \x16\x01\013CTRL-X \x16\x01\012a few times to abort)\n");

            if (Cmd[0] == '1') {
               Bbs->Printf ("\x16\x01\012Beginning %s download of the file %s\n", "XMODEM-1K", pszName);
               RetVal = Transfer->Send1kXModem (pszFile);
            }
            else if (Cmd[0] == 'M') {
               Bbs->Printf ("\x16\x01\012Beginning %s download of the file %s\n", "XMODEM", pszName);
               RetVal = Transfer->SendXModem (pszFile);
            }
            else if (Cmd[0] == 'Z') {
               Bbs->Printf ("\x16\x01\012Beginning %s download of the file %s\n", "ZMODEM", pszName);
               if ((RetVal = Transfer->SendZModem (pszFile)) == TRUE)
                  Transfer->SendZModem (NULL);
            }
            else if (Cmd[0] == 'T') {
               Bbs->Printf ("\n\x16\x01\012You have just tagged:\n\n");
               User->FileTag->New ();
               strcpy (User->FileTag->Name, pszName);
               strcpy (User->FileTag->Library, "Offline-Reader");
               strcpy (User->FileTag->Complete, pszFile);
               User->FileTag->Size = blk.size;
               User->FileTag->DeleteAfter = TRUE;
               User->FileTag->Add ();
               Bbs->Printf ("\x16\x01\x0A%5d. The file %s in the %s Library\n", User->FileTag->Index, pszName, User->FileTag->Library);
               Log->Write (":Tagged file %s, library %s", pszName, User->FileTag->Library);
               RetVal = TRUE;
            }

            // After a successfull file transfer, the user's lastread pointers
            // are updated.
            if (RetVal == TRUE) {
               class TLastRead *LastRead = User->LastRead;

               if (NewLR.First () == TRUE)
                  do {
                     if (LastRead->First () == TRUE)
                        do {
                           if (!stricmp (LastRead->Key, NewLR.Key)) {
                              LastRead->Number = NewLR.Number;
                              LastRead->Update ();
                           }
                        } while (LastRead->Next () == TRUE);
                  } while (NewLR.Next () == TRUE);
            }

            // The messages regarding the successful download are displayed only if
            // the user has choose an actual file transfer protocol and not the tag file
            // for later download option.
            if (Cmd[0] != 'T' && Bbs->AbortSession () == FALSE) {
               if (RetVal == FALSE) {
                  Bbs->Printf ("\n\n\x16\x01\015*** NO FILES DOWNLOADED ***\n");
                  Bbs->Printf ("\n\x16\x01\013Do you want to try to download the file again");
                  if (Bbs->GetAnswer (ASK_DEFYES) == ANSWER_NO)
                     Loop = FALSE;
               }
               else {
                  // After a successful download the packet should be deleted in order
                  // to prevent the garbage to wast the disk space.
                  Bbs->Printf ("\n\n\x16\x01\016*** DOWNLOAD COMPLETE ***\n");
                  unlink (pszFile);
               }
            }
         }

         delete Transfer;
      }
   }
}

USHORT TOffline::FetchReply (VOID)
{
   return (FALSE);
}

VOID TOffline::PackArea (PSZ pszKey, ULONG &ulLast)
{
   pszKey = pszKey;
   ulLast = ulLast;
}

VOID TOffline::PackEMail (ULONG &ulLast)
{
   ulLast = ulLast;
}

USHORT TOffline::Prescan (VOID)
{
   USHORT RetVal = FALSE, Areas;
   CHAR Temp[128];
   ULONG Number;
   class TLastRead *LastRead;

   Areas = 0;
   LastRead = User->LastRead;

   if (LastRead->First () == TRUE) {
      Bbs->Printf ("\n\x16\x01\012Forum \x0A Description & Msgs. Pers.\n\x16\x01\x0FÄ Ä1 Ä Ä\n");

      sprintf (Temp, "%s%s", Cfg->MailSpool, User->MailBox);
      if ((Msg = new SQUISH (Temp)) != NULL) {
         Current = Personal = 0L;
         Bbs->Printf ("\x16\x01\x0B%-15.15s \x16\x01\x0E%-49.49s ", "E-MAIL", "Electronic Mail");

         Number = 0L;
         if (LastRead->Read ("e-mail") == TRUE)
            Number = LastRead->Number;

         if (Msg->Next (Number) == TRUE) {
            do {
               Msg->ReadHeader (Number);
               Current++;
               Total++;
               Personal++;
               TotalPersonal++;
            } while (Msg->Next (Number) == TRUE);
         }

         Bbs->Printf ("%5lu %5lu\n", Current, Personal);
         delete Msg;
      }

      LastRead->First ();
      do {
         if (LastRead->QuickScan == TRUE) {
            if ((MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
               if (MsgArea->Read (LastRead->Key) == TRUE) {
                  Msg = NULL;
                  if (MsgArea->Storage == ST_JAM)
                     Msg = new JAM (MsgArea->Path);
                  else if (MsgArea->Storage == ST_SQUISH)
                     Msg = new SQUISH (MsgArea->Path);
                  else if (MsgArea->Storage == ST_USENET)
                     Msg = new USENET (Cfg->NewsServer, MsgArea->NewsGroup);

                  Areas++;
                  Current = Personal = 0L;
                  Bbs->Printf ("\x16\x01\x0B%-15.15s \x16\x01\x0E%-49.49s ", MsgArea->Key, MsgArea->Display);

                  if (Msg != NULL) {
                     Number = LastRead->Number;
                     if (Msg->Next (Number) == TRUE) {
                        do {
                           Msg->ReadHeader (Number);
                           Current++;
                           Total++;
                           if (!stricmp (Msg->To, User->Name) || !stricmp (Msg->To, User->RealName)) {
                              // If the message is directed to the user, then writes the message
                              // number and area number in the personal index file too.
                              Personal++;
                              TotalPersonal++;
                           }
                        } while (Msg->Next (Number) == TRUE);
                     }
                  }

                  Bbs->Printf ("%5lu %5lu\n", Current, Personal);

                  if (Msg != NULL)
                     delete Msg;
               }
               delete MsgArea;
            }
         }
      } while (LastRead->Next () == TRUE);
   }

   Log->Write (":Offline Prescan: %u Areas, %lu Messages", Areas, Total);

   Bbs->Printf ("\n\x16\x01\x0ATotal messages found: %lu\n\n", Total);

   if (Total == 0L)
      Bbs->Printf ("\x16\x01\015No messages found to download!\n");
   else {
      Bbs->Printf ("\x16\x01\013Do you want to download these messages");
      if (Bbs->GetAnswer (ASK_DEFYES) == ANSWER_YES) {
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

   Bbs->Printf ("\n\x16\x01\013Enter a Conference to delete from your offline reader: \026\001\x1E");
   Bbs->GetString (Area, (USHORT)(sizeof (Area) - 1), INP_FIELD);

   if (Bbs->AbortSession () == FALSE) {
      if (User->LastRead->Read (Area) == TRUE) {
         User->LastRead->QuickScan = FALSE;
         User->LastRead->Update ();
      }
      else
         Bbs->Printf ("\n\x16\x01\x0DThere is no such Conference in your offline reader configuration.\n");
   }
}

VOID TOffline::Scan (PSZ pszKey, ULONG ulLast)
{
   class TMsgData *MsgArea;
   class MsgBase *Msg = NULL;

   Current = Personal = 0L;

   if (Total < Limit && (MsgArea = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (MsgArea->Read (pszKey) == TRUE) {
         if (MsgArea->Storage == ST_JAM)
            Msg = new JAM (MsgArea->Path);
         else if (MsgArea->Storage == ST_SQUISH)
            Msg = new SQUISH (MsgArea->Path);
         else if (MsgArea->Storage == ST_USENET)
            Msg = new USENET (Cfg->NewsServer, MsgArea->NewsGroup);

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
            delete Msg;
         }
      }
      delete MsgArea;
   }
}


