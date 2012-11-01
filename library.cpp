
// --------------------------------------------------------------------
// LoraBBS Professional Edition - Version 0.18
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History list:
//    23/04/95 - Initial coding
// --------------------------------------------------------------------

#include "_ldefs.h"
#include "menu.h"

#define MAX_INDEX    32

TLibrary::TLibrary (class TBbs *bbs)
{
   Cfg = bbs->Cfg;
   Lang = bbs->Lang;
   Log = bbs->Log;
   User = bbs->User;

   Bbs = bbs;
   if ((Current = new FileData (Cfg->SystemPath)) != NULL) {
      if (Current->Read (Cfg->FirstFileArea) == FALSE)
         Current->First ();
   }
}

TLibrary::~TLibrary (void)
{
   if (Current != NULL)
      delete Current;
}

VOID TLibrary::AddFiles (VOID)
{
   CHAR FileSpec[128], Local[32];
   struct find_t blk;
   struct dosdate_t date;
   struct dostime_t time;
   class TFile *Data;

   Bbs->ReadFile ("ADDFILES");

   Bbs->Printf ("\n\x16\x01\013Enter source path to copy from, or * to add current files: ");
   Bbs->GetString (FileSpec, (USHORT)(sizeof (FileSpec) - 1), 0);

   if (Bbs->AbortSession () == FALSE && FileSpec[0] != '\0') {
      if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
         if (!stricmp (FileSpec, "*"))
            sprintf (FileSpec, "%s*.*", Current->Download);
         else if (strchr (FileSpec, '\\') == NULL && strchr (FileSpec, ':') == NULL) {
            strcpy (Local, FileSpec);
            sprintf (FileSpec, "%s%s", Current->Download, Local);
         }

         if (!_dos_findfirst (FileSpec, 0, &blk)) {
            Bbs->Printf ("\n\x16\x01\012Now searching, please wait ...\n\n");
            do {
               if (Data->Read (blk.name) == FALSE) {
                  Bbs->Printf ("\x16\x01\x0D%s in library %s not present, adding.\n", strupr (blk.name), Current->Key);

                  Data->Clear ();
                  strcpy (Data->Area, Current->Key);
                  strcpy (Data->Name, strupr (blk.name));
                  Data->Size = blk.size;
                  _dos_getdate (&date);
                  _dos_gettime (&time);
                  Data->UplDate.Day = date.day;
                  Data->UplDate.Month = date.month;
                  Data->UplDate.Year = date.year;
                  Data->UplDate.Hour = time.hour;
                  Data->UplDate.Minute = time.minute;
                  Data->Date.Day = (UCHAR)(blk.wr_date & 0x1F);
                  Data->Date.Month = (UCHAR)((blk.wr_date & 0x1E0) >> 5);
                  Data->Date.Year = (USHORT)(((blk.wr_date & 0xFE00) >> 9) + 1980);
                  Data->Date.Hour = (UCHAR)((blk.wr_time & 0xF800) >> 11);
                  Data->Date.Minute = (UCHAR)((blk.wr_time & 0x7E0) >> 5);
                  Data->Uploader = User->Name;

                  Data->Add ();
                  Current->ActiveFiles++;
               }
            } while (!_dos_findnext (&blk));

            Current->Update ();
         }

         delete Data;
      }
   }

   if (Bbs->AbortSession () == FALSE) {
      Bbs->Printf ("\n\x16\x01\x0E<< Press \x16\x01\x0BRETURN \x16\x01\x0Eto continue >> ");
      Bbs->GetString (FileSpec, 0, 0);
   }
}

VOID TLibrary::Download (class TFileTag *Files)
{
   CHAR Names[64], Cmd[10], *p;
   USHORT SelectOK, ClearAfter = FALSE;
   ULONG DlTime;
   class TFile *Data;
   class TTransfer *Transfer;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Downloading");
      Bbs->Status->Update ();
   }

   if (Files == NULL && User->FileTag->TotalNumber > 0) {
      Bbs->Printf ("\n\x16\x01\x0D>> You have pending files that you <T>agged for later download.\n>> Do you want to download them now");
      if (Bbs->GetAnswer (ASK_DEFYES) == ANSWER_YES)
         Files = User->FileTag;
   }

   if (Files == NULL) {
      Files = new TFileTag;
      ClearAfter = TRUE;

      Bbs->Printf ("\n\x16\x01\013Enter file name to download, RETURN to exit, or ? for help: ");
      Bbs->GetString (Names, (USHORT)(sizeof (Names) - 1), 0);
      Bbs->Printf ("\n");

      Files->Clear ();

      if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
         if ((p = strtok (Names, " ")) != NULL)
            do {
               if (Data->First () == TRUE) {
                  do {
                     if (!stricmp (Data->Name, p)) {
                        strcpy (Files->Name, Data->Name);
                        strcpy (Files->Library, Data->Area);
                        Files->Size = Data->Size;
                        strcpy (Files->Complete, Data->Complete);
                        if (Files->Add () == TRUE) {
                           if ((p = (PSZ)Data->Description->First ()) == NULL)
                              p = "";
                           Bbs->Printf ("\x16\x01\x0E %-12.12s %4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                        }
                     }
                  } while (Data->Next () == TRUE);
               }
               else
                  Bbs->Printf ("File not found in this library");
            } while ((p = strtok (NULL, " ")) != NULL);
         delete Data;
      }
   }
   else
      Log->Write (":Download of %d tagged files", Files->TotalNumber);

   if (Files != NULL && Files->TotalNumber != 0) {
      DlTime = (Files->TotalSize / (Bbs->CarrierSpeed / 10L)) / 60L;
      if (DlTime < 1)
         Bbs->Printf ("\n\x16\x01\012Approximate download time: < 1 minute.\n\n");
      else
         Bbs->Printf ("\n\x16\x01\012Approximate download time: %ld minutes.\n\n", DlTime);

      if (Files->TotalNumber == 1) {
         Bbs->Printf ("  \x16\x01\013A ... \x16\x01\016ASCII (continuos dump)\n");
         Bbs->Printf ("  \x16\x01\013M ... \x16\x01\016XMODEM (Checksum/CRC)\n");
         Bbs->Printf ("  \x16\x01\0131 ... \x16\x01\016XMODEM-1K\n");
      }
      Bbs->Printf ("  \x16\x01\013Z ... \x16\x01\016ZMODEM\n");
//      Bbs->Printf ("  \x16\x01\013T ... \x16\x01\016FTP to Internet host\n");
//      Bbs->Printf ("  \x16\x01\013F ... \x16\x01\016File Export (to any DOS path)\n");
      if (ClearAfter == TRUE)
         Bbs->Printf ("  \x16\x01\013T ... \x16\x01\016Tag file(s) for later download\n");

      SelectOK = FALSE;

      do {
         Bbs->Printf ("\n\x16\x01\013Choose a download option (or RETURN to exit): ");
         if (Bbs->HotKey == TRUE)
            Bbs->GetString (Cmd, 1, INP_HOTKEY);
         else
            Bbs->GetString (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
         Cmd[0] = (CHAR)toupper (Cmd[0]);
         if (Files->TotalNumber == 1) {
            if (Cmd[0] == 'M' || Cmd[0] == '1')
               SelectOK = TRUE;
         }
         if (Cmd[0] == '\0' || Cmd[0] == 'Z' || Cmd[0] == 'T' || Cmd[0] == 'F')
            SelectOK = TRUE;
      } while (Bbs->AbortSession () == FALSE && SelectOK == FALSE);

      if (Cmd[0] != '\0' && (Transfer = new TTransfer (Bbs)) != NULL) {
         while (Files->TotalNumber > 0 && Bbs->AbortSession () == FALSE) {
            if (Cmd[0] != 'T')
               Bbs->Printf ("\n\x16\x01\012(Hit \x16\x01\013CTRL-X \x16\x01\012a few times to abort)\n");

            if (Cmd[0] == '1') {
               Files->First ();
               Bbs->Printf ("\x16\x01\012Beginning %s download of the file %s\n", "XMODEM-1K", Files->Name);
               if (Transfer->Send1kXModem (Files->Complete) == TRUE) {
                  if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
                     if (Data->Read (Files->Name) == TRUE) {
                        Data->DlTimes++;
                        Data->ReplaceHeader ();
                     }
                     delete Data;
                  }
                  Files->Remove ();
               }
            }
            else if (Cmd[0] == 'A') {
               Files->First ();
               if (Transfer->SendASCIIDump (Files->Complete) == TRUE) {
                  if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
                     if (Data->Read (Files->Name) == TRUE) {
                        Data->DlTimes++;
                        Data->ReplaceHeader ();
                     }
                     delete Data;
                  }
                  Files->Remove ();
               }
            }
            else if (Cmd[0] == 'M') {
               Files->First ();
               Bbs->Printf ("\x16\x01\012Beginning %s download of the file %s\n", "XMODEM", Files->Name);
               if (Transfer->SendXModem (Files->Complete) == TRUE) {
                  if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
                     if (Data->Read (Files->Name) == TRUE) {
                        Data->DlTimes++;
                        Data->ReplaceHeader ();
                     }
                     delete Data;
                  }
                  Files->Remove ();
               }
            }
            else if (Cmd[0] == 'Z') {
               Bbs->Printf ("\x16\x01\012Beginning %s download of the file(s)\n", "ZMODEM");
               while (Files->First () == TRUE && Bbs->AbortSession () == FALSE) {
                  if (Transfer->SendZModem (Files->Complete) == FALSE)
                     break;
                  if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
                     if (Data->Read (Files->Name) == TRUE) {
                        Data->DlTimes++;
                        Data->ReplaceHeader ();
                     }
                     delete Data;
                  }
                  Files->Remove ();
               }
               if (Bbs->AbortSession () == FALSE && Files->TotalNumber == 0L)
                  Transfer->SendZModem (NULL);
            }
            else if (Cmd[0] == 'T') {
               if (ClearAfter == TRUE) {
                  Bbs->Printf ("\n\x16\x01\012You have just tagged:\n\n");
                  if (Files->First () == TRUE)
                     do {
                        User->FileTag->New ();
                        strcpy (User->FileTag->Name, Files->Name);
                        strcpy (User->FileTag->Library, Files->Library);
                        strcpy (User->FileTag->Complete, Files->Complete);
                        User->FileTag->Size = Files->Size;
                        User->FileTag->DeleteAfter = Files->DeleteAfter;
                        User->FileTag->Add ();
                        Bbs->Printf ("\x16\x01\x0A%5d. The file %s in the %s Library\n", User->FileTag->Index, Files->Name, Files->Library);
                        Log->Write (":Tagged file %s, library %s", Files->Name, Files->Library);
                     } while (Files->Next () == TRUE);
                  Files->Clear ();
               }
            }

            if (Cmd[0] != 'T' && Bbs->AbortSession () == FALSE) {
               if (Files->TotalNumber > 0) {
                  Bbs->Printf ("\n\n\x16\x01\015*** ERROR DOWNLOADING FILES ***\n");
                  Bbs->Printf ("\n\x16\x01\013Do you want to try to download the file(s) again");
                  if (Bbs->GetAnswer (ASK_DEFYES) == ANSWER_NO)
                     Files->Clear ();
               }
               else
                  Bbs->Printf ("\n\n\x16\x01\016*** DOWNLOAD COMPLETE ***\n");
            }
         }

         delete Transfer;
      }
   }

   if (ClearAfter == TRUE) {
      Files->Clear ();
      delete Files;
   }
}

USHORT TLibrary::DownloadFile (PSZ pszFile, PSZ pszName, ULONG ulSize)
{
   USHORT RetVal = FALSE, SelectOK, Loop;
   CHAR Cmd[10];
   ULONG DlTime;
   class TTransfer *Transfer;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Downloading");
      Bbs->Status->Update ();
   }

   if (ulSize != 0L) {
      DlTime = (ulSize / (Bbs->CarrierSpeed / 10L) + 30L) / 60L;
      if (DlTime < 1)
         Bbs->Printf ("\n\x16\x01\012Approximate download time: < 1 minute.\n\n");
      else
         Bbs->Printf ("\n\x16\x01\012Approximate download time: %ld minutes.\n\n", DlTime);
   }
   else
      Bbs->Printf ("\n");

   Bbs->Printf ("  \x16\x01\013A ... \x16\x01\016ASCII (continuos dump)\n");
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
      else if (Cmd[0] == '\0' || Cmd[0] == 'Z' || Cmd[0] == 'T' || Cmd[0] == 'F')
         SelectOK = TRUE;
   } while (Bbs->AbortSession () == FALSE && SelectOK == FALSE);

   if (Cmd[0] != '\0' && (Transfer = new TTransfer (Bbs)) != NULL) {
      Loop = TRUE;
      while (Loop == TRUE && RetVal == FALSE && Bbs->AbortSession () == FALSE) {
         Bbs->Printf ("\n\x16\x01\012(Hit \x16\x01\013CTRL-X \x16\x01\012a few times to abort)\n");

         if (Cmd[0] == '1') {
            Bbs->Printf ("\x16\x01\012Beginning %s download of the file %s\n", "XMODEM-1K", pszName);
            RetVal = Transfer->Send1kXModem (pszFile);
         }
         else if (Cmd[0] == 'A')
            RetVal = Transfer->SendASCIIDump (pszFile);
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
            strcpy (User->FileTag->Complete, pszFile);
            User->FileTag->Size = ulSize;
            User->FileTag->Add ();
            Bbs->Printf ("\x16\x01\x0A%5d. The file %s\n", User->FileTag->Index, pszName);
            RetVal = TRUE;
         }

         if (Cmd[0] != 'T') {
            if (RetVal == FALSE) {
               Bbs->Printf ("\n\n\x16\x01\015*** NO FILES DOWNLOADED ***\n");
               Bbs->Printf ("\n\x16\x01\013Do you want to try to download the file again");
               if (Bbs->GetAnswer (ASK_DEFYES) == ANSWER_NO)
                  Loop = FALSE;
            }
            else
               Bbs->Printf ("\n\n\x16\x01\016*** DOWNLOAD COMPLETE ***\n");
         }
      }

      delete Transfer;
   }

   return (RetVal);
}

VOID TLibrary::DownloadList (VOID)
{
   FILE *fp;
   UCHAR CanDownload = FALSE;
   CHAR Work[64], *p;
   ULONG Size;
   class FileData *File;
   class TFile *Data;
   class TOffline *Olr;

   sprintf (Work, "%s%s\\", Cfg->HomePath, User->MailBox);
   if ((Olr = new TOffline (Bbs)) != NULL) {
      Olr->BuildPath (Work);
      delete Olr;
   }
   strcat (Work, "LIST.TXT");

   if ((fp = fopen (Work, "wt")) != NULL) {
      if ((File = new FileData (Cfg->SystemPath)) != NULL) {
         if (File->First () == TRUE) {
            Bbs->Printf ("\n\x16\x01\012Please wait... ");
            do {
               if (File->ShowGlobal == TRUE && User->Level >= File->Level) {
                  if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                     if ((Data = new TFile (Cfg->SystemPath, File->Key)) != NULL) {
                        Data->SortByName ();
                        if (Data->First () == TRUE) {
                           fprintf (fp, "\nLibrary: %s\n", File->Key);
                           fprintf (fp, "Description: %s\n", File->Display);
                           fprintf (fp, "There are %lu files available for download\n\n", File->ActiveFiles);

                           fprintf (fp, "File Name    Size  Date  Description\n============ ===== ===== =====================================================\n");
                           do {
                              if ((p = (PSZ)Data->Description->First ()) == NULL)
                                 p = "";
                              fprintf (fp, "%-12.12s %4ldK %02d/%02d %.53s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                              while ((p = (PSZ)Data->Description->Next ()) != NULL)
                                 fprintf (fp, "                         %.53s\n", p);
                           } while (Data->Next () == TRUE);

                           fprintf (fp, "\n");
                           CanDownload = TRUE;
                        }
                        delete Data;
                     }
                  }
               }
            } while (File->Next () == TRUE);
            Bbs->Printf ("\n\n");
         }
         delete File;
      }
      Size = ftell (fp);
      fclose (fp);
   }

   if (CanDownload == TRUE)
      DownloadFile (Work, "LIST.TXT", Size);
   unlink (Work);
}

VOID TLibrary::EditFileData (VOID)
{
   USHORT Line;
   CHAR FileName[32], Command[10], *p;
   class TFile *Data;

   Bbs->Printf ("\n\x16\x01\013Enter file name to edit, or RETURN to exit: ");
   Bbs->GetString (FileName, sizeof (FileName) - 1, 0);

   if (FileName[0] != '\0') {
      if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
         if (Data->Read (FileName) == TRUE) {
            Bbs->Printf ("\x0C");
            Bbs->PrintfAt ( 2,  1, "\x16\x01\012  File Name: \x16\x01\x0E%s", Data->Name);
            Bbs->PrintfAt ( 3,  1, "\x16\x01\012  File Date: \x16\x01\x0E%02d-%02d-%02d", Data->Date.Day, Data->Date.Month, Data->Date.Year % 100);
            Bbs->PrintfAt ( 4,  1, "\x16\x01\012  File Size: \x16\x01\x0E%lu bytes", Data->Size);

            Bbs->PrintfAt ( 6,  1, "\x16\x01\012  Downloads: \x16\x01\x0E%lu", Data->DlTimes);
            Bbs->PrintfAt ( 7,  1, "\x16\x01\012       Cost: \x16\x01\x0E%lu", Data->Cost);
            Bbs->PrintfAt ( 8,  1, "\x16\x01\012      Level: \x16\x01\x0E%lu", Data->Level);

            Bbs->PrintfAt (10,  1, "\x16\x01\012Upload Date: \x16\x01\x0E%02d-%02d-%02d", Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100);

            Bbs->PrintfAt ( 3, 30, "\x16\x01\x0A-- Short Description ----------------------------");
            Bbs->PrintfAt ( 4, 30, "\x16\x01\x0E%.49s\n", Data->Description->First ());

            Bbs->PrintfAt ( 6, 30, "\x16\x01\x0A-- Description ----------------------------------");
            Bbs->PrintfAt (19, 30, "\x16\x01\x0A-------------------------------------------------");
            Bbs->PrintfAt (20, 30, "\x16\x01\012Uploaded by: \x16\x01\x0E%s", Data->Uploader);
            Bbs->PrintfAt (21, 30, "\x16\x01\012   Keywords: \x16\x01\x0E%s", Data->Keyword);

            Line = 6;
            if ((p = (PSZ)Data->Description->First ()) != NULL)
               do {
                  Bbs->PrintfAt (Line++, 30, "\x16\x01\x0E%.49s\n", p);
               } while ((p = (PSZ)Data->Description->Next ()) != NULL && Bbs->AbortSession () == FALSE);

            Bbs->PrintfAt (21,  1, "\x16\x01\013  SAVE/QUIT:");

            Bbs->GetString (Command, sizeof (Command) - 1, 0);
         }
         else
            Bbs->Printf ("\n\x16\x01\x0DThere is no such file in this Library!\n");

         delete Data;
      }
   }
}

VOID TLibrary::FileDetails (class TFile *File)
{
   USHORT Line;
   CHAR Temp[128];
   PSZ Text;
   ULONG DlTime;

   Bbs->Printf ("\x0C\x16\x01\x0A   File Name: \x16\x01\x0E%-32.32s  \x16\x01\x0A       Date: \x16\x01\x0E%02d/%02d/%02d\n", File->Name, File->Date.Day, File->Date.Month, File->Date.Year % 100);
   Bbs->Printf ("\x16\x01\x0A     Library: \x16\x01\x0E%-32.32s  \x16\x01\x0A       Time: \x16\x01\x0E%02d:%02d\n", File->Area, File->Date.Hour, File->Date.Minute);
   Bbs->Printf ("\x16\x01\x0ASize (bytes): \x16\x01\x0E%lu\n", File->Size);
   if (File->Uploader == NULL)
      File->Uploader = "";
   Bbs->Printf ("\x16\x01\x0A Uploaded by: \x16\x01\x0E%-32.32s  \x16\x01\x0A Downloaded: \x16\x01\x0E%lu \x16\x01\x0Atimes\n", File->Uploader, File->DlTimes);
   Bbs->Printf ("\x16\x01\x0A              %32.32s        Added: \x16\x01\x0E%02d/%02d/%02d %02d:%02d\n", "", File->UplDate.Day, File->UplDate.Month, File->UplDate.Year % 100, File->UplDate.Hour, File->UplDate.Minute);

   DlTime = (File->Size / (Bbs->CarrierSpeed / 10L) + 30L) / 60L;
   if (DlTime < 1)
      Bbs->Printf ("\n\x16\x01\012Approximate download time: \x16\x01\x0E< 1 \x16\x01\x0Aminute.\n\n");
   else
      Bbs->Printf ("\n\x16\x01\012Approximate download time: \x16\x01\x0E%ld \x16\x01\x0Aminutes.\n\n", DlTime);

   if ((Text = (PSZ)File->Description->First ()) != NULL) {
      Bbs->Printf ("\x16\x01\012Description:\n\x16\x01\x0E");
      Line = 10;
      do {
         Bbs->Printf ("%s\n", Text);
         Line = Bbs->MoreQuestion (Line);
      } while (Line != 0 && Bbs->AbortSession () == FALSE && (Text = (PSZ)File->Description->Next ()) != NULL);
   }

   Temp[0] = '\0';
   while (toupper (Temp[0]) != 'X' && Bbs->AbortSession () == FALSE) {
      Bbs->Printf ("\n\x16\x01\013(D)ownload now, (T)ag for download later, or e(X)it? ");
      Bbs->GetString (Temp, 1, (Bbs->HotKey == TRUE) ? INP_HOTKEY : 0);

      switch (toupper (Temp[0])) {
         case 'D': {
            class FileData *fd;

            if ((fd = new FileData (Cfg->SystemPath)) != NULL) {
               if (fd->Read (File->Area) == TRUE) {
                  sprintf (Temp, "%s%s", fd->Download, File->Name);
                  if (DownloadFile (Temp, File->Name, 0L) == TRUE) {
                     File->DlTimes++;
                     File->ReplaceHeader ();
                  }
                  Temp[0] = 'X';
               }
            }
            break;
         }

         case 'T': {
            class FileData *fd;
            class TFileTag *Tag = User->FileTag;

            if ((fd = new FileData (Cfg->SystemPath)) != NULL) {
               if (fd->Read (File->Area) == TRUE) {
                  Tag->New ();
                  strcpy (Tag->Name, File->Name);
                  strcpy (Tag->Library, File->Area);
                  Tag->Size = File->Size;
                  sprintf (Tag->Complete, "%s%s", fd->Download, File->Name);
                  Tag->Add ();
                  Temp[0] = 'X';
               }
            }
            break;
         }
      }
   }
}

VOID TLibrary::ListFiles (class TFile *Data)
{
   USHORT i, y, t, End, DoDelete = FALSE;
   SHORT Line;
   CHAR FileName[128], Redraw, Titles, *p;
   class TCollection List;
   class TFileTag *Tag = User->FileTag;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Files listing");
      Bbs->Status->Update ();
   }

   if (Data == NULL) {
      if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL)
         Data->SortByName ();
      DoDelete = TRUE;
   }

   if (Data != NULL && (User->Ansi == TRUE || User->Avatar == TRUE)) {
      List.Clear ();
      i = 0;
      if (Data->First () == TRUE) {
         do {
            List.Add (Data->Name, (USHORT)(strlen (Data->Name) + 1));
            i++;
         } while (Data->Next () == TRUE && i < (User->ScreenHeight - 6));

         y = 4;
         List.First ();
         Data->First ();
         End = FALSE;
         Titles = Redraw = TRUE;

         while (End == FALSE && Bbs->AbortSession () == FALSE) {
            if (Redraw == TRUE) {
               while (List.Previous () != NULL)
                  Data->Previous ();

               if (Titles == TRUE) {
                  Bbs->Printf ("\x0C\x16\x01\x0B%s \x16\x01\x0A- \x16\x01\x0B%s\x1B[K\n", Current->Key, Current->Display);
                  Bbs->Printf ("\x16\x01\012 File Name    Size  Date     Description\n ============ ===== ======== ==================================================\n");
               }

               if (Tag->TotalNumber != 0)
                  Bbs->PrintfAt (1, 29, " \x16\x01\013CTRL-T \x16\x01\016downloads tagged files (%d tagged).\n\n\n", Tag->TotalNumber);

               if (Titles == TRUE) {
                  Bbs->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, "\x16\x01\x0A ============ ===== ======== ==================================================\n");
                  Bbs->Printf ("\x16\x01\x0A Press \x16\x01\x0B? \x16\x01\012for help, or \x16\x01\x0BX \x16\x01\x0Ato exit. To highlight a file, use your \x16\x01\013arrow keys\x16\x01\x0A.\n \x16\x01\013RETURN \x16\x01\x0Aview details on highlighted file, \x16\x01\013SPACE \x16\x01\x0Atags it.");
                  Bbs->PrintfAt (4, 1, "");
               }

               i = 0;
               do {
                  if ((p = (PSZ)Data->Description->First ()) == NULL)
                     p = "";
                  if (Tag->Check (Data->Name) == TRUE)
                     Bbs->Printf ("\x16\x01\x0E*%-12.12s*%4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                  else
                     Bbs->Printf ("\x16\x01\x0E %-12.12s %4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                  i++;
               } while (Data->Next () == TRUE && i < (User->ScreenHeight - 6));

               List.Last ();
               for (t = (USHORT)(i + 4 - 1); t > y; t--) {
                  List.Previous ();
                  Data->Previous ();
               }

               if (i < (User->ScreenHeight - 6)) {
                  do {
                     Bbs->Printf ("\n");
                     i++;
                  } while (i < (User->ScreenHeight - 6));
               }
               else
                  Data->Previous ();

               Bbs->PrintfAt (y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
               Redraw = Titles = FALSE;
            }

            if (Bbs->KBHit () == TRUE) {
               if ((i = Bbs->Getch ()) == 0)
                  i = (USHORT)(Bbs->Getch () << 8);
               else
                  i = (USHORT)toupper (i);

               if (i == ESC) {
                  if ((i = Bbs->Getch ()) == '[') {
                     while ((i = Bbs->Getch ()) == ';' || isdigit (i))
                        ;
                     if (i == 'A')
                        i = CTRLE;
                     else if (i == 'B')
                        i = CTRLX;
                  }
               }

               switch (i) {
                  case CTRLD:
                     Bbs->Printf ("\x0C\n");
                     if ((p = (PSZ)Data->Description->First ()) == NULL)
                        p = "";
                     Bbs->Printf ("\x16\x01\x0E%-12.12s %4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);

                     sprintf (FileName, "%s%s", Current->Download, Data->Name);
                     if (DownloadFile (FileName, Data->Name, Data->Size) == TRUE) {
                        Data->DlTimes++;
                        Data->ReplaceHeader ();
                     }

                     Titles = Redraw = TRUE;
                     break;

                  case CTRLT:
                     if (Tag->TotalNumber > 0) {
                        Bbs->Printf ("\x0C\n");
                        Download (Tag);
                        Titles = Redraw = TRUE;
                     }
                     break;

                  case ' ':
                     if (Tag->Check (Data->Name) == FALSE) {
                        Tag->New ();
                        strcpy (Tag->Name, Data->Name);
                        strcpy (Tag->Library, Data->Area);
                        strcpy (Tag->Complete, Data->Complete);
                        Tag->Size = Data->Size;
                        Tag->Add ();
                     }
                     else
                        Tag->Remove (Data->Name);
                     Bbs->PrintfAt (1, 29, " \x16\x01\013CTRL-T \x16\x01\016downloads tagged files (%d tagged).", Tag->TotalNumber);
                     if (Tag->TotalNumber == 0)
                        Bbs->PrintfAt (1, 29, "\x1B[K");
                     if (Tag->Check (Data->Name) == TRUE)
                        Bbs->PrintfAt (y, 1, "\x16\x01\x0E*%-12.12s*", Data->Name);
                     else
                        Bbs->PrintfAt (y, 1, "\x16\x01\x0E %-12.12s ", Data->Name);
                     Bbs->PrintfAt (y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                     break;

                  case CTRLE:
                  case 0x4800:
                     if (List.Previous () != NULL) {
                        Bbs->PrintfAt (y, 2, "\x16\x01\x0E%-12.12s\x16\x01\x0E", (PSZ)List.Next ());
                        Bbs->PrintfAt (--y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Previous ());
                        Data->Previous ();
                     }
                     else if (Data->Previous () == TRUE) {
                        Bbs->PrintfAt (4, 1, "");
                        for (i = 0; i < (User->ScreenHeight - 6); i++)
                           Bbs->Printf ("\x1B[K\n");
                        for (i = 0; i < (User->ScreenHeight - 6 - 1); i++)
                           Data->Previous ();
                        List.Clear ();
                        Bbs->PrintfAt (4, 1, "");
                        i = 0;
                        do {
                           if ((p = (PSZ)Data->Description->First ()) == NULL)
                              p = "";
                           if (Tag->Check (Data->Name) == TRUE)
                              Bbs->Printf ("\x16\x01\x0E*%-12.12s*%4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                           else
                              Bbs->Printf ("\x16\x01\x0E %-12.12s %4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                           List.Add (Data->Name, (USHORT)(strlen (Data->Name) + 1));
                           i++;
                        } while (Data->Next () == TRUE && i < (User->ScreenHeight - 6));
                        Data->Previous ();
                        y = (USHORT)(4 + User->ScreenHeight - 6 - 1);
                        List.Last ();
                        Bbs->PrintfAt (y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                     }
                     break;

                  case CR:
                     FileDetails (Data);
                     Titles = Redraw = TRUE;
                     break;

                  case CTRLX:
                  case 0x5000:
                     if (List.Next () != NULL) {
                        Bbs->PrintfAt (y, 2, "\x16\x01\x0E%-12.12s\x16\x01\x0E", (PSZ)List.Previous ());
                        Bbs->PrintfAt (++y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Next ());
                        Data->Next ();
                     }
                     else if (Data->Next () == TRUE) {
                        Bbs->PrintfAt (4, 1, "");
                        for (i = 0; i < (User->ScreenHeight - 6); i++)
                           Bbs->Printf ("\x1B[K\n");
                        List.Clear ();
                        Bbs->PrintfAt (4, 1, "");
                        i = 0;
                        do {
                           if ((p = (PSZ)Data->Description->First ()) == NULL)
                              p = "";
                           if (Tag->Check (Data->Name) == TRUE)
                              Bbs->Printf ("\x16\x01\x0E*%-12.12s*%4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                           else
                              Bbs->Printf ("\x16\x01\x0E %-12.12s %4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                           List.Add (Data->Name, (USHORT)(strlen (Data->Name) + 1));
                        } while (Data->Next () == TRUE && ++i < (User->ScreenHeight - 6));
                        for (y = 0; y < i; y++)
                           Data->Previous ();
                        y = 4;
                        List.First ();
                        Bbs->PrintfAt (y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                     }
                     break;

                  case 'X':
                     End = TRUE;
                     break;
               }
            }

            Bbs->ReleaseTimeSlice ();
         }

         Bbs->Printf ("\x0C");
      }
      else
         Bbs->Printf ("\n\x16\x01\014Sorry, no files are found to match your search/list parameters.\n");
   }
   else if (Data != NULL) {
      if (Data->First () == TRUE) {
         Bbs->Printf ("\x0C\x16\x01\x0B%s \x16\x01\x0A- \x16\x01\x0B%s\x1B[K\n", Current->Key, Current->Display);
         Bbs->Printf ("\x16\x01\012 File Name    Size  Date     Description\n ============ ===== ======== ==================================================\n");
         Line = 3;
         do {
            if ((p = (PSZ)Data->Description->First ()) == NULL)
               p = "";
            Bbs->Printf ("\x16\x01\x0E %-12.12s %4ldK \x16\x01\x0A%2d/%02d/%02d %.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
            if ((Line = Bbs->MoreQuestion (Line)) == 1) {
               Bbs->Printf ("\x0C\x16\x01\x0B%s \x16\x01\x0A- \x16\x01\x0B%s\x1B[K\n", Current->Key, Current->Display);
               Bbs->Printf ("\x16\x01\012 File Name    Size  Date     Description\n ============ ===== ======== ==================================================\n");
               Line = 3;
            }
         } while (Data->Next () == TRUE && Bbs->AbortSession () == FALSE && Line != 0);
      }
   }

   if (Data != NULL && DoDelete == TRUE)
      delete Data;
}

VOID TLibrary::ListDownloadedFiles (VOID)
{
   class TFile *Data;

   if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
      Data->SortByDownload ();
      ListFiles (Data);
      delete Data;
   }
}

VOID TLibrary::ListRecentFiles (VOID)
{
   class TFile *Data;

   if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
      Data->SortByDate ();
      ListFiles (Data);
      delete Data;
   }
}

VOID TLibrary::ManageTagged (VOID)
{
   SHORT Index;
   CHAR Temp[10], End;
   ULONG Size, Minute;
   class TFile *File;
   class TFileTag *Tag = User->FileTag;

   Tag->Reindex ();

   End = FALSE;
   while (Tag->TotalNumber > 0 && Bbs->AbortSession () == FALSE && End == FALSE) {
      if (Tag->First () == TRUE) {
         Bbs->Printf ("\n\x16\x01\x0AYou have tagged the following:\n\n");

         do {
            Size = Tag->Size / 1024L;
            if ((Tag->Size % 1024L) != 0L)
               Size++;
            Minute = (Tag->Size / (Bbs->CarrierSpeed / 10L)) / 60L;
            if (Minute < 1)
               Bbs->Printf ("\x16\x01\x0A%5d. The file %s in the %s Library (%ldK, < 1 minute)\n", Tag->Index, Tag->Name, Tag->Library, Size);
            else
               Bbs->Printf ("\x16\x01\x0A%5d. The file %s in the %s Library (%ldK, %ld minute(s))\n", Tag->Index, Tag->Name, Tag->Library, Size, Minute);
         } while (Tag->Next () == TRUE);

         Size = Tag->TotalSize / 1024L;
         if ((Tag->TotalSize % 1024L) != 0L)
            Size++;
         Bbs->Printf ("\n\x16\x01\x0ATotal size of tagged Library files: \x16\x01\x0E%ldK\n", Size);
         Minute = (Tag->TotalSize / (Bbs->CarrierSpeed / 10L)) / 60L;
         if (Minute < 1)
            Bbs->Printf ("\x16\x01\x0A         Approximate transfer time: \x16\x01\x0E< 1 minute\n\n");
         else
            Bbs->Printf ("\x16\x01\x0A         Approximate transfer time: \x16\x01\x0E%ld minute(s)\n\n", Minute);
      }

      Bbs->Printf ("\x16\x01\013           D \x16\x01\x0E... Download all these files\n");
      Bbs->Printf ("\x16\x01\013%4d \x16\x01\x0Ato \x16\x01\013%4d \x16\x01\x0E... Download one file\n", 1, Tag->TotalNumber);
      Bbs->Printf ("\x16\x01\013%4d \x16\x01\x0Ato \x16\x01\013%4d \x16\x01\x0E... Untag one file\n", -1, -Tag->TotalNumber);
      Bbs->Printf ("\x16\x01\013        -ALL \x16\x01\x0E... Untag all these files\n\n");

      Bbs->Printf ("\x16\x01\013Enter option, X to exit or ? for help: ");
      Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), 0);
      if (Bbs->AbortSession () == FALSE) {
         if (toupper (Temp[0]) == 'X')
            End = TRUE;
         else if (!stricmp (Temp, "-ALL")) {
            Tag->Clear ();
            Bbs->Printf ("\n\x16\x01\x0AOk, all files have been untagged.\n");
         }
         else if (!stricmp (Temp, "D"))
            Download (Tag);
         else {
            Index = (SHORT)atoi (Temp);
            if (Index < 0) {
               if (Tag->Select ((USHORT)abs (Index)) == TRUE) {
                  Tag->Remove ();
                  Tag->Reindex ();
               }
            }
            else if (Index > 0) {
               if (Tag->Select ((USHORT)abs (Index)) == TRUE) {
                  if (DownloadFile (Tag->Complete, Tag->Name, Tag->Size) == TRUE) {
                     if ((File = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
                        if (File->Read (Tag->Name) == TRUE) {
                           File->DlTimes++;
                           File->ReplaceHeader ();
                        }
                        delete File;
                     }
                     Tag->Remove ();
                     Tag->Reindex ();
                  }
               }
            }
         }
      }
   }

   if (Tag->TotalNumber == 0)
      Bbs->Printf ("\n\x16\x01\x0DNo tagged files.\n");
}

VOID TLibrary::RemoveFiles (VOID)
{
   CHAR FileName[32];
   class TFile *Data;

   Bbs->Printf ("\n\x16\x01\013Enter file name to delete, or RETURN to exit: ");
   Bbs->GetString (FileName, sizeof (FileName) - 1, 0);

   if (FileName[0] != '\0') {
      if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
         if (Data->Read (FileName) == TRUE) {
            Data->Delete ();
            Bbs->Printf ("\n\x16\x01\x0D%s deleted from Library records...\n", strupr (FileName));

            Current->ActiveFiles--;
            Current->Update ();
         }
         else
            Bbs->Printf ("\n\x16\x01\x0DThere is no such file in this Library!\n");

         delete Data;
      }
   }
}

VOID TLibrary::SearchKeyword (VOID)
{
   CHAR Keyword[80];
   class TFile *Data;

   Bbs->Printf ("\n\x16\x01\x0BOn the line below, enter your keyword list, ? for help, or RETURN to exit.\n: ");
   Bbs->GetString (Keyword, (USHORT)(sizeof (Keyword) - 1), 0);

   if (Keyword[0] != '\0' && Bbs->AbortSession () == FALSE) {
      if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
         Data->SearchKeyword (Keyword);
         ListFiles (Data);
         delete Data;
      }
   }
}

VOID TLibrary::SearchNewFiles (VOID)
{
   class TFile *Data;

   if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
      Data->SortByDate (User->LastCall);
      ListFiles (Data);
      delete Data;
   }
}

VOID TLibrary::SearchText (VOID)
{
   CHAR Keyword[80];
   class TFile *Data;

   Bbs->Printf ("\n\x16\x01\013Enter the text to search, ? for help, or RETURN to exit.\n: ");
   Bbs->GetString (Keyword, (USHORT)(sizeof (Keyword) - 1), 0);

   if (Keyword[0] != '\0' && Bbs->AbortSession () == FALSE) {
      if ((Data = new TFile (Cfg->SystemPath, Current->Key)) != NULL) {
         Data->SearchText (Keyword);
         ListFiles (Data);
         delete Data;
      }
   }
}

USHORT TLibrary::SelectArea (VOID)
{
   USHORT RetVal = FALSE, FirstHit;
   SHORT Line;
   CHAR Command[16], Temp[128];
   class FileData *File;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Change Library");
      Bbs->Status->Update ();
   }

   if ((File = new FileData (Cfg->SystemPath)) != NULL) {
      do {
         Bbs->Printf ("\n\026\001\013Enter the name of new Library, or ? for a list: \026\001\x1E");
         Bbs->GetString (Command, (USHORT)(sizeof (Command) - 1), INP_FIELD);

         if (!stricmp (Command, Lang->ListKey)) {
            if (File->First () == TRUE) {
               Bbs->Printf ("\n\026\001\012Library          Files  Description\n\026\001\017\031Ä\017  \031Ä\005  \031Ä\067\n");
               Line = 3;
               do {
                  if (File->ShowGlobal == TRUE && User->Level >= File->Level) {
                     if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                        Bbs->Printf ("\026\001\013%-15.15s  \026\001\016%5ld  %.55s\n", File->Key, File->ActiveFiles, File->Display);
                        Line = Bbs->MoreQuestion (Line);
                     }
                  }
               } while (Line != 0 && Bbs->AbortSession () == FALSE && File->Next () == TRUE);
            }
         }
         else if (Command[0] != '\0') {
            if (File->Read (Command) == TRUE) {
               if (File->ShowGlobal == TRUE && User->Level >= File->Level) {
                  if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                     Current->Read (Command);
                     if (Current->Download[strlen (Current->Download) - 1] != '\\')
                        strcat (Current->Download, "\\");
                     if (Current->Upload[strlen (Current->Upload) - 1] != '\\')
                        strcat (Current->Upload, "\\");
                     Log->Write (":File Area: %s - %s", File->Key, File->Display);
                     RetVal = TRUE;
                  }
               }
            }

            if (RetVal == FALSE) {
               FirstHit = TRUE;
               strupr (Command);
               Line = 3;

               if (File->First () == TRUE) {
                  do {
                     if (File->ShowGlobal == TRUE && User->Level >= File->Level) {
                        if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                           strcpy (Temp, File->Key);
                           if (strstr (strupr (Temp), Command) != NULL) {
                              if (FirstHit == TRUE)
                                 Bbs->Printf ("\n\026\001\012Library          Files  Description\n---------------  -----  -------------------------------------------------------\n");
                              Bbs->Printf ("\026\001\013%-15.15s  \026\001\016%5ld  %s\n", File->Key, File->ActiveFiles, File->Display);
                              Line = Bbs->MoreQuestion (Line);
                              FirstHit = FALSE;
                           }
                           else {
                              strcpy (Temp, File->Display);
                              if (strstr (strupr (Temp), Command) != NULL) {
                                 if (FirstHit == TRUE)
                                    Bbs->Printf ("\n\026\001\012Library          Files  Description\n---------------  -----  -------------------------------------------------------\n");
                                 Bbs->Printf ("\026\001\013%-15.15s  \026\001\016%5ld  %s\n", File->Key, File->ActiveFiles, File->Display);
                                 Line = Bbs->MoreQuestion (Line);
                                 FirstHit = FALSE;
                              }
                           }
                        }
                     }
                  } while (Line != 0 && Bbs->AbortSession () == FALSE && File->Next () == TRUE);
               }

               if (FirstHit == TRUE)
                  Bbs->Printf (Lang->ForumNotAvailable);
            }
         }
      } while (Command[0] != '\0' && RetVal == FALSE && Bbs->AbortSession () == FALSE);

      delete File;
   }

   return (RetVal);
}

VOID TLibrary::SelectNext (VOID)
{
   USHORT Found = FALSE;
   class FileData *Data;

   if ((Data = new FileData (Cfg->SystemPath)) != NULL) {
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
         if (Current->Download[strlen (Current->Download) - 1] != '\\')
            strcat (Current->Download, "\\");
         if (Current->Upload[strlen (Current->Upload) - 1] != '\\')
            strcat (Current->Upload, "\\");
         Log->Write (":File Area: %s - %s", Current->Key, Current->Display);
      }

      delete Data;
   }
}

VOID TLibrary::SelectPrevious (VOID)
{
   USHORT Found = FALSE;
   class FileData *Data;

   if ((Data = new FileData (Cfg->SystemPath)) != NULL) {
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
         if (Current->Download[strlen (Current->Download) - 1] != '\\')
            strcat (Current->Download, "\\");
         if (Current->Upload[strlen (Current->Upload) - 1] != '\\')
            strcat (Current->Upload, "\\");
         Log->Write (":File Area: %s - %s", Current->Key, Current->Display);
      }

      delete Data;
   }
}

VOID TLibrary::Upload (VOID)
{
   SHORT fRet, BatchTransfer, DoTransfer;
   CHAR szAnswer[32], szProtocol[10], szFile[128], *p, *q, *RxFile;
   struct find_t blk;
   struct dosdate_t date;
   struct dostime_t time;
   class TFile *Data;
   class TTransfer *Transfer;
   class TCollection Received;

   if (Bbs->Status != NULL) {
      strcpy (Bbs->Status->Status, "Uploading");
      Bbs->Status->Update ();
   }

   if (User->Level >= Current->UploadLevel) {
      if ((User->AccessFlags & Current->UploadFlags) == Current->UploadFlags) {
         BatchTransfer = FALSE;

         while (Bbs->AbortSession () == FALSE) {
            DoTransfer = FALSE;

            Bbs->Printf ("\n\x16\x01\012Upload to the %s library\n\n", Current->Key);

            Bbs->Printf ("\x16\x01\013<file name> \x16\x01\016... Upload a file to this library\n");
            Bbs->Printf ("\x16\x01\013          * \x16\x01\016... Upload multiple files at once\n");
//            Bbs->Printf ("\x16\x01\013          M \x16\x01\016... Modify or add descriptions to files you have uploaded\n");

            Bbs->Printf ("\n\x16\x01\013Enter your selection, ? for more help, or RETURN to exit: ");
            Bbs->GetString (szAnswer, (USHORT)(sizeof (szAnswer) - 1), 0);
            if (szAnswer[0] == '\0')
               break;

            if (!stricmp (szAnswer, "?")) {
               Bbs->ReadFile ("UPLOAD");
               DoTransfer = FALSE;
            }
            else if (!stricmp (szAnswer, "*")) {
               BatchTransfer = TRUE;
               DoTransfer = TRUE;
            }
            else if (!stricmp (szAnswer, "M")) {
               DoTransfer = FALSE;
            }
            else {
               BatchTransfer = FALSE;
               DoTransfer = TRUE;
            }

            if (DoTransfer == TRUE) {
               if (BatchTransfer == FALSE) {
                  if ((Data = new TFile (Cfg->SystemPath, Current->Key)) == NULL)
                     return;
                  if (Data->Read (szAnswer) == TRUE) {
                     Bbs->Printf ("\n\x16\x01\013*** File exists!\n");
                     DoTransfer = FALSE;
                  }
                  delete Data;
               }

               if (DoTransfer == TRUE) {
                  Bbs->Printf ("\n\x16\x01\012To start uploading %s, type:\n\n", strupr (szAnswer));

                  if (BatchTransfer == FALSE) {
                     Bbs->Printf ("  \x16\x01\013A ... \x16\x01\016ASCII\n");
                     Bbs->Printf ("  \x16\x01\013M ... \x16\x01\016XMODEM (Checksum/CRC)\n");
                     Bbs->Printf ("  \x16\x01\0131 ... \x16\x01\016XMODEM-1K\n");
                  }
                  Bbs->Printf ("  \x16\x01\013Z ... \x16\x01\016ZMODEM\n");
//                  Bbs->Printf ("  \x16\x01\013F ... \x16\x01\016File Import (existing file)\n");

                  Bbs->Printf ("\n\x16\x01\013Choose an upload option, or RETURN to exit: ");
                  if (Bbs->HotKey == TRUE)
                     Bbs->GetString (szProtocol, 1, INP_HOTKEY);
                  else
                     Bbs->GetString (szProtocol, (USHORT)(sizeof (szProtocol) - 1), 0);

                  fRet = FALSE;

                  if ((Transfer = new TTransfer (Bbs)) != NULL) {
                     sprintf (szFile, "%s%s", szAnswer, Current->Upload);
                     Received.Clear ();

                     switch (toupper (szProtocol[0])) {
                        case '1':
                           if (Transfer->Receive1kXModem (szFile) != NULL) {
                              fRet = TRUE;
                              Received.Add (szFile, (USHORT)(strlen (szFile) + 1));
                           }
                           break;

                        case 'M':
                           if (Transfer->ReceiveXModem (szFile) != NULL) {
                              fRet = TRUE;
                              Received.Add (szFile, (USHORT)(strlen (szFile) + 1));
                           }
                           break;

                        case 'Z':
                           if ((p = Transfer->ReceiveZModem (Current->Upload)) != NULL) {
                              fRet = TRUE;
                              Received.Add (p, (USHORT)(strlen (p) + 1));
                              while ((p = Transfer->ReceiveZModem (Current->Upload)) != NULL)
                                 Received.Add (p, (USHORT)(strlen (p) + 1));
                           }
                           break;
                     }

                     delete Transfer;
                  }

                  Bbs->Printf ("\n");

                  if (fRet == TRUE) {
                     if ((Data = new TFile (Cfg->SystemPath, Current->Key)) == NULL)
                        return;

                     if ((RxFile = (CHAR *)Received.First ()) != NULL)
                        do {
                           for (p = RxFile, q = RxFile; *p; p++) {
                              if (*p == '/' || *p == '\\' || *p == ':')
                                 q = p + 1;
                           }

                           Bbs->Printf ("\x16\x01\012Now adding %s to the %s library.\n", q, Current->Key);

                           _dos_findfirst (RxFile, 0, &blk);
                           Data->Clear ();
                           strcpy (Data->Area, Current->Key);
                           strcpy (Data->Name, strupr (q));
                           Data->Size = blk.size;
                           _dos_getdate (&date);
                           _dos_gettime (&time);
                           Data->UplDate.Day = date.day;
                           Data->UplDate.Month = date.month;
                           Data->UplDate.Year = date.year;
                           Data->UplDate.Hour = time.hour;
                           Data->UplDate.Minute = time.minute;
                           Data->Date.Day = (UCHAR)(blk.wr_date & 0x1F);
                           Data->Date.Month = (UCHAR)((blk.wr_date & 0x1E0) >> 5);
                           Data->Date.Year = (USHORT)(((blk.wr_date & 0xFE00) >> 9) + 1980);
                           Data->Date.Hour = (UCHAR)((blk.wr_time & 0xF800) >> 11);
                           Data->Date.Minute = (UCHAR)((blk.wr_time & 0x7E0) >> 5);
                           Data->Uploader = User->Name;

                           Data->Add ();
                        } while ((RxFile = (CHAR *)Received.Next ()) != NULL);

                     delete Data;

                     Current->ActiveFiles++;
                     Current->Update ();
                  }
               }
            }
         }
      }
      else
         Bbs->Printf ("\n\x16\x01\016   *** Sorry, you can't upload files to this Library! ***\n\n");
   }
   else
      Bbs->Printf ("\n\x16\x01\016   *** Sorry, you can't upload files to this Library! ***\n\n");
}



