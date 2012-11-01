
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora.h"

class TFileAreaListing : public TListings
{
public:
   TFileAreaListing (void);

   CHAR   Command[16];
   class  TConfig *Cfg;
   class  TFileData *Current;
   class  TLanguage *Language;

   VOID   Begin (VOID);
   USHORT DrawScreen (VOID);
   VOID   PrintCursor (USHORT y);
   VOID   PrintLine (VOID);
   VOID   PrintTitles (VOID);
   VOID   RemoveCursor (USHORT y);
   VOID   Select (VOID);

private:
};

// ----------------------------------------------------------------------

TLibrary::TLibrary (PSZ pszDataPath)
{
   Embedded = NULL;
   Log = NULL;
   User = NULL;

   strcpy (DataPath, pszDataPath);
   Current = new TFileData (DataPath);
}

TLibrary::~TLibrary (void)
{
   if (Current != NULL)
      delete Current;
}

VOID TLibrary::ExternalProtocols (USHORT Batch)
{
   class TProtocol *Protocol;

   if ((Protocol = new TProtocol (Cfg->SystemPath)) != NULL) {
      if (Protocol->First () == TRUE)
         do {
            if (Protocol->Active == TRUE && (Batch == FALSE || Protocol->Batch == TRUE))
               Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), Protocol->Key, Protocol->Description);
         } while (Protocol->Next () == TRUE);
      delete Protocol;
   }
}

VOID TLibrary::Download (class TFileTag *Files, USHORT AnyLibrary)
{
   CHAR Names[64], Cmd[10], *p;
   USHORT SelectOK, ClearAfter = FALSE, Loop = TRUE, Continue = TRUE;
   ULONG DlTime;
   class TFileBase *Data;
   class TTransfer *Transfer;
   class TLimits *Limits;
   class TProtocol *Protocol;

   if (Files == NULL) {
      Files = new TFileTag;
      ClearAfter = TRUE;

      if (User->FileTag->TotalFiles != 0) {
         Embedded->Printf (Language->Text (LNG_FILETAGGEDWARNING), User->FileTag->TotalFiles);
         if (Embedded->GetAnswer (ASK_DEFYES) == ANSWER_YES) {
            if (User->FileTag->First () == TRUE)
               do {
                  Files->New ();
                  strcpy (Files->Name, User->FileTag->Name);
                  strcpy (Files->Area, User->FileTag->Area);
                  Files->Size = User->FileTag->Size;
                  strcpy (Files->Complete, User->FileTag->Complete);
                  Files->CdRom = User->FileTag->CdRom;
                  Files->Add ();
               } while (User->FileTag->Next () == TRUE);
            User->FileTag->Clear ();
         }
      }

      do {
         Embedded->Printf (Language->Text (LNG_FILEDOWNLOADNAME), Files->TotalFiles + 1);
         Embedded->Input (Names, (USHORT)(sizeof (Names) - 1), 0);

         if ((Data = new TFileBase (Cfg->SystemPath, (AnyLibrary == TRUE) ? "" : Current->Key)) != NULL) {
            if ((p = strtok (Names, " ")) != NULL)
               do {
                  if (Data->Read (p) == TRUE) {
                     Files->New ();
                     strcpy (Files->Name, Data->Name);
                     strcpy (Files->Area, Data->Area);
                     Files->Size = Data->Size;
                     if (Data->Complete[0] == '\0')
                        sprintf (Files->Complete, "%s%s", Current->Download, Files->Name);
                     else
                        strcpy (Files->Complete, Data->Complete);
                     Files->CdRom = Data->CdRom;
                     if (Files->Add () == TRUE) {
                        DlTime = Data->Size / (CarrierSpeed / 10L);
                        Embedded->Printf (Language->Text (LNG_DOWNLOADFILENAME), Files->Index, Data->Name, DlTime / 60L, DlTime % 60L, Data->Size);
                     }
                  }
                  else
                     Embedded->Printf (Language->Text (LNG_NOFILEHERE), p);
               } while ((p = strtok (NULL, " ")) != NULL);
            Data->Close ();
            delete Data;
         }
      } while (Names[0] != '\0');
   }
   else
      Log->Write (":Download of %d tagged files", Files->TotalFiles);

   DlTime = (Files->TotalBytes / (CarrierSpeed / 10L)) / 60L;

   if (DlTime >= Embedded->TimeRemain ()) {
      Embedded->Printf (Language->Text (LNG_FILENOTIMEWARNING));
      Continue = FALSE;
   }
   if ((Limits = new TLimits (Cfg->SystemPath)) != NULL) {
      if (Limits->Read (User->LimitClass) == TRUE) {
         if ((Files->TotalBytes + 1023L)/ 1024L > (Limits->DayDownloadLimit - (User->BytesToday + 1023L) / 1024L)) {
            Embedded->Printf (Language->Text (LNG_FILENOBYTESWARNING));
            Continue = FALSE;
         }
      }
      delete Limits;
   }

   if (Files != NULL && Files->TotalFiles != 0 && Continue == TRUE) {
      if (DlTime < 1)
         Embedded->Printf ("\n\x16\x01\012Approximate download time: < 1 minute.\n\n");
      else
         Embedded->Printf ("\n\x16\x01\012Approximate download time: %ld minutes.\n\n", DlTime);

      if (Files->TotalFiles == 1) {
         Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), "A", "ASCII (continuos dump)");
         Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), "M", "XMODEM (Checksum/CRC)");
         Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), "1", "XMODEM-1K");
      }
      Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), "Z", "ZMODEM");
      ExternalProtocols ((Files->TotalFiles == 1) ? FALSE : TRUE);
//      Embedded->Printf ("  \x16\x01\013T ... \x16\x01\016FTP to Internet host\n");
//      Embedded->Printf ("  \x16\x01\013F ... \x16\x01\016File Export (to any DOS path)\n");
      if (ClearAfter == TRUE)
         Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), "T", "Tag file(s) for later download");

      SelectOK = FALSE;

      do {
         Embedded->Printf ("\n\x16\x01\013Choose a download option (or RETURN to exit): ");
         if (Embedded->HotKey == TRUE)
            Embedded->Input (Cmd, 1, INP_HOTKEY);
         else
            Embedded->Input (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
         Cmd[0] = (CHAR)toupper (Cmd[0]);
         if ((Protocol = new TProtocol (Cfg->SystemPath)) != NULL) {
            if (Protocol->Read (Cmd) == TRUE)
               SelectOK = TRUE;
            delete Protocol;
         }
         if (Files->TotalFiles == 1) {
            if (Cmd[0] == 'M' || Cmd[0] == '1' || Cmd[0] == 'A')
               SelectOK = TRUE;
         }
         if (Cmd[0] == '\0' || Cmd[0] == 'Z' || Cmd[0] == 'T' || Cmd[0] == 'F')
            SelectOK = TRUE;
      } while (Embedded->AbortSession () == FALSE && SelectOK == FALSE);

      if (Cmd[0] != '\0' && (Transfer = new TTransfer) != NULL) {
         Transfer->Task = Task;
         Transfer->Com = Embedded->Com;
         Transfer->Log = Log;
         Transfer->Speed = CarrierSpeed;
         Transfer->Progress = Progress;
         Transfer->Telnet = Cfg->ZModemTelnet;
         strcpy (Transfer->Device, Cfg->Device);
         while (Loop == TRUE && Files->TotalFiles > 0 && Embedded->AbortSession () == FALSE) {
            if (Cmd[0] == '1') {
               Files->First ();
               Embedded->Printf (Language->Text (LNG_FILEBEGINDOWNLOAD), "XMODEM-1K", Files->Name);
               if (Transfer->Send1kXModem (Files->Complete) == TRUE) {
                  if ((Data = new TFileBase (Cfg->SystemPath, Files->Area)) != NULL) {
                     if (Data->Read (Files->Name) == TRUE) {
                        Data->DlTimes++;
                        Data->ReplaceHeader ();
                        if (User != NULL) {
                           User->FilesToday++;
                           User->DownloadFiles++;
                           User->BytesToday += Data->Size;
                           User->DownloadBytes += Data->Size;
                        }
                     }
                     Data->Close ();
                     delete Data;
                  }
                  Files->Remove ();
               }
            }
            else if (Cmd[0] == 'A') {
               Files->First ();
               if (Transfer->SendASCIIDump (Files->Complete) == TRUE) {
                  if ((Data = new TFileBase (Cfg->SystemPath, Files->Area)) != NULL) {
                     if (Data->Read (Files->Name) == TRUE) {
                        Data->DlTimes++;
                        Data->ReplaceHeader ();
                        if (User != NULL) {
                           User->FilesToday++;
                           User->DownloadFiles++;
                           User->BytesToday += Data->Size;
                           User->DownloadBytes += Data->Size;
                        }
                     }
                     Data->Close ();
                     delete Data;
                  }
                  Files->Remove ();
               }
            }
            else if (Cmd[0] == 'M') {
               Files->First ();
               Embedded->Printf (Language->Text (LNG_FILEBEGINDOWNLOAD), "XMODEM", Files->Name);
               if (Transfer->SendXModem (Files->Complete) == TRUE) {
                  if ((Data = new TFileBase (Cfg->SystemPath, Files->Area)) != NULL) {
                     if (Data->Read (Files->Name) == TRUE) {
                        Data->DlTimes++;
                        Data->ReplaceHeader ();
                        if (User != NULL) {
                           User->FilesToday++;
                           User->DownloadFiles++;
                           User->BytesToday += Data->Size;
                           User->DownloadBytes += Data->Size;
                        }
                     }
                     Data->Close ();
                     delete Data;
                  }
                  Files->Remove ();
               }
            }
            else if (Cmd[0] == 'Z') {
               Embedded->Printf (Language->Text (LNG_FILEBEGINDOWNLOAD2), "ZMODEM");
               while (Files->First () == TRUE && Embedded->AbortSession () == FALSE) {
                  if (Transfer->SendZModem (Files->Complete) == FALSE)
                     break;
                  if ((Data = new TFileBase (Cfg->SystemPath, Files->Area)) != NULL) {
                     if (Data->Read (Files->Name) == TRUE) {
                        Data->DlTimes++;
                        Data->ReplaceHeader ();
                        if (User != NULL) {
                           User->FilesToday++;
                           User->DownloadFiles++;
                           User->BytesToday += Data->Size;
                           User->DownloadBytes += Data->Size;
                        }
                     }
                     Data->Close ();
                     delete Data;
                  }
                  Files->Remove ();
               }
               if (Embedded->AbortSession () == FALSE && Files->TotalFiles == 0L)
                  Transfer->SendZModem (NULL);
            }
            else if (Cmd[0] == 'T') {
               if (ClearAfter == TRUE) {
                  Embedded->Printf (Language->Text (LNG_FILETAGGEDHEADER));

                  if (Files->First () == TRUE)
                     do {
                        User->FileTag->New ();
                        strcpy (User->FileTag->Name, Files->Name);
                        strcpy (User->FileTag->Area, Files->Area);
                        strcpy (User->FileTag->Complete, Files->Complete);
                        User->FileTag->Size = Files->Size;
                        User->FileTag->DeleteAfter = Files->DeleteAfter;
                        User->FileTag->CdRom = Files->CdRom;
                        User->FileTag->Add ();

                        DlTime = User->FileTag->Size / (CarrierSpeed / 10L);
                        Embedded->Printf (Language->Text (LNG_FILETAGGEDLIST), User->FileTag->Index, User->FileTag->Name, User->FileTag->Area, DlTime / 60L, DlTime % 60L, User->FileTag->Size);

                        Log->Write (":Tagged file %s, library %s", Files->Name, Files->Area);
                     } while (Files->Next () == TRUE);

                  DlTime = User->FileTag->TotalBytes / (CarrierSpeed / 10L);
                  Embedded->Printf (Language->Text (LNG_FILETAGGEDTOTAL), User->FileTag->TotalBytes, DlTime / 60L, DlTime % 60L);

                  Files->Clear ();
               }
            }
            else {
               if ((Protocol = new TProtocol (Cfg->SystemPath)) != NULL) {
                  Transfer->TxQueue.Clear ();
                  if (Files->First () == TRUE)
                     do {
                        Transfer->TxQueue.New ();
                        strcpy (Transfer->TxQueue.Name, Files->Name);
                        strcpy (Transfer->TxQueue.Complete, Files->Complete);
                        Transfer->TxQueue.Size = Files->Size;
                        Transfer->TxQueue.Add ();
                     } while (Files->Next () == TRUE);

                  Transfer->RunExternalProtocol (TRUE, Cmd, Protocol);

                  if (Transfer->TxQueue.First () == TRUE)
                     do {
                        if (Transfer->TxQueue.Sent == TRUE) {
                           if (Files->First () == TRUE)
                              do {
                                 if (!stricmp (Files->Complete, Transfer->TxQueue.Complete)) {
                                    if ((Data = new TFileBase (Cfg->SystemPath, Files->Area)) != NULL) {
                                       if (Data->Read (Files->Name) == TRUE) {
                                          Data->DlTimes++;
                                          Data->ReplaceHeader ();
                                          if (User != NULL) {
                                             User->FilesToday++;
                                             User->DownloadFiles++;
                                             User->BytesToday += Data->Size;
                                             User->DownloadBytes += Data->Size;
                                          }
                                       }
                                       Data->Close ();
                                       delete Data;
                                    }
                                    Files->Remove ();
                                    break;
                                 }
                              } while (Files->Next () == TRUE);
                        }
                     } while (Transfer->TxQueue.Next () == TRUE);

                  delete Protocol;
               }
            }

            if (Cmd[0] != 'T' && Embedded->AbortSession () == FALSE) {
               if (Files->TotalFiles > 0) {
                  Embedded->Printf (Language->Text (LNG_FILEDOWNLOADERROR));
//                  Embedded->Printf ("\n\x16\x01\013Do you want to try to download the file(s) again");
//                  if (Embedded->GetAnswer (ASK_DEFYES) == ANSWER_NO)
//                  Files->Clear ();
                  Loop = FALSE;
               }
               else
                  Embedded->Printf (Language->Text (LNG_FILEDOWNLOADCOMPLETE));
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

VOID TLibrary::TypeFile (VOID)
{
   CHAR File[128], Names[32], *p;
   class TFileBase *Data;

   Embedded->Printf (Language->Text (LNG_DISPLAYWHICHFILE));
   Embedded->Input (Names, (USHORT)(sizeof (Names) - 1), 0);
   Embedded->Printf ("\n");

   if ((Data = new TFileBase (Cfg->SystemPath, Current->Key)) != NULL) {
      if ((p = strtok (Names, " ")) != NULL) {
         if (Data->Read (p) == TRUE) {
            if (Embedded->DisplayFile (Data->Complete) == FALSE) {
               if (Log != NULL)
                  Log->Write ("!File %s not found", Data->Complete);
               sprintf (File, "%s%s", Current->Download, Data->Name);
               Embedded->DisplayFile (Data->Complete);
            }
         }
         else
            Embedded->Printf (Language->Text (LNG_NOFILEHERE), Names);
      }
      delete Data;
   }
}

USHORT TLibrary::DownloadFile (PSZ pszFile, PSZ pszName, ULONG ulSize)
{
   USHORT RetVal = FALSE, SelectOK, Loop;
   CHAR Cmd[10];
   ULONG DlTime;
   class TTransfer *Transfer;
   class TProtocol *Protocol;

   DlTime = (ulSize / (CarrierSpeed / 10L) + 30L) / 60L;
   if (ulSize != 0L) {
      if (DlTime < 1)
         Embedded->Printf ("\n\x16\x01\012Approximate download time: < 1 minute.\n\n");
      else
         Embedded->Printf ("\n\x16\x01\012Approximate download time: %ld minutes.\n\n", DlTime);
   }
   else
      Embedded->Printf ("\n");

   Embedded->Printf ("  \x16\x01\013A ... \x16\x01\016ASCII (continuos dump)\n");
   if (DlTime < Embedded->TimeRemain ()) {
      Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), "M", "XMODEM (Checksum/CRC)");
      Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), "1", "XMODEM-1K");
      Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), "Z", "ZMODEM");
      ExternalProtocols (FALSE);
   }
   Embedded->Printf (Language->Text (LNG_FILEPROTOCOLLIST), "T", "Tag file(s) for later download");

   SelectOK = FALSE;

   do {
      Embedded->Printf ("\n\x16\x01\013Choose a download option (or RETURN to exit): ");
      if (Embedded->HotKey == TRUE)
         Embedded->Input (Cmd, 1, INP_HOTKEY);
      else
         Embedded->Input (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
      Cmd[0] = (CHAR)toupper (Cmd[0]);
      if (Cmd[0] == 'M' || Cmd[0] == '1' || Cmd[0] == 'A')
         SelectOK = TRUE;
      else if (Cmd[0] == '\0' || Cmd[0] == 'Z' || Cmd[0] == 'T' || Cmd[0] == 'F')
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
            if (Cmd[0] == 'A') {
               RetVal = Transfer->SendASCIIDump (pszFile);
               if (User != NULL) {
                  User->FilesToday++;
                  User->DownloadFiles++;
                  User->BytesToday += ulSize;
                  User->DownloadBytes += ulSize;
               }
            }
            else if (Cmd[0] == '1') {
               Embedded->Printf (Language->Text (LNG_FILEBEGINDOWNLOAD), "XMODEM-1K", pszName);
               if ((RetVal = Transfer->Send1kXModem (pszFile)) == TRUE) {
                  if (User != NULL) {
                     User->FilesToday++;
                     User->DownloadFiles++;
                     User->BytesToday += ulSize;
                     User->DownloadBytes += ulSize;
                  }
               }
            }
            else if (Cmd[0] == 'M') {
               Embedded->Printf (Language->Text (LNG_FILEBEGINDOWNLOAD), "XMODEM", pszName);
               if ((RetVal = Transfer->SendXModem (pszFile)) == TRUE) {
                  if (User != NULL) {
                     User->FilesToday++;
                     User->DownloadFiles++;
                     User->BytesToday += ulSize;
                     User->DownloadBytes += ulSize;
                  }
               }
            }
            else if (Cmd[0] == 'Z') {
               Embedded->Printf (Language->Text (LNG_FILEBEGINDOWNLOAD), "ZMODEM", pszName);
               if ((RetVal = Transfer->SendZModem (pszFile)) == TRUE) {
                  if (User != NULL) {
                     User->FilesToday++;
                     User->DownloadFiles++;
                     User->BytesToday += ulSize;
                     User->DownloadBytes += ulSize;
                  }
                  Transfer->SendZModem (NULL);
               }
            }
            else {
               if ((Protocol = new TProtocol (Cfg->SystemPath)) != NULL) {
                  Transfer->TxQueue.Clear ();
                  Transfer->TxQueue.New ();
                  strcpy (Transfer->TxQueue.Name, pszName);
                  strcpy (Transfer->TxQueue.Complete, pszFile);
                  Transfer->TxQueue.Size = ulSize;
                  Transfer->TxQueue.Add ();

                  Transfer->RunExternalProtocol (TRUE, Cmd, Protocol);

                  if (Transfer->TxQueue.First () == TRUE) {
                     if (Transfer->TxQueue.Sent == TRUE) {
                        if (User != NULL) {
                           User->FilesToday++;
                           User->DownloadFiles++;
                           User->BytesToday += ulSize;
                           User->DownloadBytes += ulSize;
                        }
                     }
                  }

                  delete Protocol;
               }
            }
         }

         if (Cmd[0] == 'T') {
            Embedded->Printf ("\n\x16\x01\012You have just tagged:\n\n");
            User->FileTag->New ();
            strcpy (User->FileTag->Name, pszName);
            strcpy (User->FileTag->Complete, pszFile);
            User->FileTag->Size = ulSize;
            User->FileTag->Add ();
            Embedded->Printf ("\x16\x01\x0A%5d. The file %s\n", User->FileTag->Index, pszName);
            RetVal = Loop = FALSE;
         }

         if (Cmd[0] != 'T') {
            if (RetVal == FALSE) {
               Embedded->Printf (Language->Text (LNG_FILEDOWNLOADERROR));
//               Embedded->Printf ("\n\x16\x01\013Do you want to try to download the file again");
//               if (Embedded->GetAnswer (ASK_DEFYES) == ANSWER_NO)
               Loop = FALSE;
            }
            else
               Embedded->Printf (Language->Text (LNG_FILEDOWNLOADCOMPLETE));
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
   CHAR Work[64], Cmd[16], Temp[128], *p;
   struct stat statbuf;
   class TFileData *File;
   class TFileBase *Data;
   class TPacker *Packer;

   sprintf (Work, "%s%s\\", Cfg->UsersHomePath, User->MailBox);
   BuildPath (Work);
   strcat (Work, "bbslist.txt");

   if (Log != NULL)
      Log->Write ("+Building list of files");

   if ((fp = _fsopen (Work, "wt", SH_DENYNO)) != NULL) {
      if ((File = new TFileData (DataPath)) != NULL) {
         if (File->First () == TRUE) {
            Embedded->Printf (Language->Text (LNG_FILEBUILDLIST));
            do {
               if (User->Level >= File->Level) {
                  if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                     if ((Data = new TFileBase (Cfg->SystemPath, File->Key)) != NULL) {
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
                        Data->Close ();
                        delete Data;
                     }
                  }
               }
            } while (File->Next () == TRUE);
            Embedded->Printf ("\n\n");
         }
         delete File;
      }
      fclose (fp);
   }

   if (CanDownload == TRUE) {
      Embedded->Printf ("\n\n");

      if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
         if (Packer->First () == TRUE)
            do {
               Embedded->Printf ("  \x16\x01\013%s ... \x16\x01\016%s\n", Packer->Key, Packer->Display);
            } while (Packer->Next () == TRUE);

         do {
            Embedded->Printf ("\n\x16\x01\013Choose a compression option (or RETURN to exit): ");
            Embedded->Input (Cmd, (USHORT)(sizeof (Cmd) - 1), 0);
         } while (Cmd[0] != '\0' && Embedded->AbortSession () == FALSE && Packer->Read (Cmd) == FALSE);

         if (Cmd[0] != '\0' && Packer->Read (Cmd) == TRUE) {
            Embedded->Printf ("\n\x16\x01\016Please wait while compressing your mail packet.\n");
            sprintf (Temp, "%s%s\\", Cfg->UsersHomePath, User->MailBox);
            strcat (Temp, "filelist.bbs");
            if (Packer->DoPack (Temp, Work) == TRUE) {
               stat (Temp, &statbuf);
               if (DownloadFile (Temp, "filelist.bbs", statbuf.st_size) == TRUE) {
                  unlink (Temp);
                  sprintf (Work, "%s%s", Cfg->UsersHomePath, User->MailBox);
                  rmdir (Work);
               }
               else if (User->FileTag->Check ("filelist.bbs") == TRUE) {
                  User->FileTag->DeleteAfter = TRUE;
                  User->FileTag->Update ();
               }
               else {
                  unlink (Temp);
                  sprintf (Work, "%s%s", Cfg->UsersHomePath, User->MailBox);
                  rmdir (Work);
               }
            }
         }
         else {
            unlink (Work);
            sprintf (Work, "%s%s", Cfg->UsersHomePath, User->MailBox);
            rmdir (Work);
         }

         delete Packer;
      }
   }
}

VOID TLibrary::FileDetails (class TFileBase *File)
{
   USHORT Line;
   CHAR Temp[128];
   PSZ Text;
   ULONG DlTime;

   Embedded->Printf ("\x0C\x16\x01\x0A   File Name: \x16\x01\x0E%-32.32s  \x16\x01\x0A       Date: \x16\x01\x0E%02d/%02d/%02d\n", File->Name, File->Date.Day, File->Date.Month, File->Date.Year % 100);
   Embedded->Printf ("\x16\x01\x0A     Library: \x16\x01\x0E%-32.32s  \x16\x01\x0A       Time: \x16\x01\x0E%02d:%02d\n", File->Area, File->Date.Hour, File->Date.Minute);
   Embedded->Printf ("\x16\x01\x0ASize (bytes): \x16\x01\x0E%lu\n", File->Size);
   if (File->Uploader == NULL)
      File->Uploader = "";
   Embedded->Printf ("\x16\x01\x0A Uploaded by: \x16\x01\x0E%-32.32s  \x16\x01\x0A Downloaded: \x16\x01\x0E%lu \x16\x01\x0Atimes\n", File->Uploader, File->DlTimes);
   Embedded->Printf ("\x16\x01\x0A              %32.32s        Added: \x16\x01\x0E%02d/%02d/%02d %02d:%02d\n", "", File->UplDate.Day, File->UplDate.Month, File->UplDate.Year % 100, File->UplDate.Hour, File->UplDate.Minute);

   DlTime = (File->Size / (CarrierSpeed / 10L) + 30L) / 60L;
   if (DlTime < 1)
      Embedded->Printf ("\n\x16\x01\012Approximate download time: \x16\x01\x0E< 1 \x16\x01\x0Aminute.\n\n");
   else
      Embedded->Printf ("\n\x16\x01\012Approximate download time: \x16\x01\x0E%ld \x16\x01\x0Aminutes.\n\n", DlTime);

   if ((Text = (PSZ)File->Description->First ()) != NULL) {
      Embedded->Printf ("\x16\x01\012Description:\n\x16\x01\x0E");
      Line = 10;
      do {
         Embedded->Printf ("%s\n", Text);
         Line = Embedded->MoreQuestion (Line);
      } while (Line != 0 && Embedded->AbortSession () == FALSE && (Text = (PSZ)File->Description->Next ()) != NULL);
   }

   Temp[0] = '\0';
   while (toupper (Temp[0]) != 'X' && Embedded->AbortSession () == FALSE) {
      Embedded->Printf ("\n\x16\x01\013(D)ownload now, (T)ag for download later, or e(X)it? ");
      Embedded->Input (Temp, 1, (Embedded->HotKey == TRUE) ? INP_HOTKEY : 0);

      switch (toupper (Temp[0])) {
         case '\0':
            Temp[0] = 'X';
            break;

         case 'D': {
            class TFileData *fd;

            if ((fd = new TFileData (DataPath)) != NULL) {
               if (fd->Read (File->Area) == TRUE) {
                  sprintf (Temp, "%s%s", fd->Download, File->Name);
                  if (DownloadFile (Temp, File->Name, 0L) == TRUE) {
                     File->DlTimes++;
                     File->ReplaceHeader ();
                  }
                  Temp[0] = 'X';
               }
               delete fd;
            }
            break;
         }

         case 'T': {
            class TFileData *fd;
            class TFileTag *Tag = User->FileTag;

            if ((fd = new TFileData (DataPath)) != NULL) {
               if (fd->Read (File->Area) == TRUE) {
                  Tag->New ();
                  strcpy (Tag->Name, File->Name);
                  strcpy (Tag->Area, File->Area);
                  Tag->Size = File->Size;
                  sprintf (Tag->Complete, "%s%s", fd->Download, File->Name);
                  Tag->CdRom = fd->CdRom;
                  Tag->Add ();
                  Temp[0] = 'X';
               }
               delete fd;
            }
            break;
         }
      }
   }
}

VOID TLibrary::ListDownloadedFiles (VOID)
{
   CHAR Range;
   class TFileBase *Data;

   if ((Range = (CHAR)SearchRange ()) != 'X') {
      if ((Data = new TFileBase (Cfg->SystemPath, (Range == 'A') ? "" : Current->Key)) != NULL) {
         Data->SortByDownload ();
         ListFiles (Data);
         Data->Close ();
         delete Data;
      }
   }
}

VOID TLibrary::TagListed (VOID)
{
   USHORT First = TRUE;
   CHAR Names[64], *p;
   ULONG DlTime;
   class TFileBase *Data;
   class TFileTag *Files = User->FileTag;

   Embedded->Printf (Language->Text (LNG_FILETAGLISTED), Files->TotalFiles + 1);
   Embedded->Input (Names, (USHORT)(sizeof (Names) - 1), INP_NOCRLF);

   if ((Data = new TFileBase (Cfg->SystemPath, "")) != NULL) {
      if ((p = strtok (Names, " ")) != NULL)
         do {
            if (First == FALSE)
               Pause (100L);

            First = FALSE;
            if (Data->Read (p) == TRUE) {
               Files->New ();
               strcpy (Files->Name, Data->Name);
               strcpy (Files->Area, Data->Area);
               Files->Size = Data->Size;
               if (Data->Complete[0] == '\0')
                  sprintf (Files->Complete, "%s%s", Current->Download, Files->Name);
               else
                  strcpy (Files->Complete, Data->Complete);
               Files->CdRom = Data->CdRom;
               if (Files->Add () == TRUE) {
                  DlTime = Data->Size / (CarrierSpeed / 10L);
                  Embedded->Printf (Language->Text (LNG_FILELISTTAGCONFIRM), Files->Index, Data->Name, DlTime / 60L, DlTime % 60L, Data->Size);
               }
            }
            else
               Embedded->Printf (Language->Text (LNG_FILELISTNOTFOUND), p);
         } while ((p = strtok (NULL, " ")) != NULL);
      Data->Close ();
      delete Data;
   }
}

SHORT TLibrary::MoreQuestion (SHORT nLine)
{
   CHAR Temp[2];

   if (nLine == -1 || nLine == 0 || Embedded->More == FALSE)
      return (nLine);

   if (++nLine >= (SHORT)(Embedded->ScreenHeight - 1)) {
      while (Embedded->AbortSession () == FALSE) {
         Embedded->Printf (Language->Text (LNG_FILELISTMOREQUESTION));
         Embedded->GetString (Temp, 1, INP_NOCRLF|INP_NOCOLOR|INP_HOTKEY);
         if (toupper (Temp[0]) == Language->Text(LNG_QUIT)[0]) {
            Embedded->Printf (Language->Text(LNG_FILELISTDELETEMOREQUESTION));
            return (0);
         }
         else if (toupper (Temp[0]) == Language->Text(LNG_NONSTOP)[0]) {
            Embedded->Printf (Language->Text(LNG_FILELISTDELETEMOREQUESTION));
            return (-1);
         }
         else if (toupper (Temp[0]) == Language->Text(LNG_CONTINUE)[0] || Temp[0] == '\0') {
            Embedded->Printf (Language->Text(LNG_FILELISTDELETEMOREQUESTION));
            return (1);
         }
         else if (toupper (Temp[0]) == Language->Text(LNG_FILELISTTAGKEY)[0])
            TagListed ();
      }
   }

   return (nLine);
}

VOID TLibrary::ListFiles (class TFileBase *Data)
{
   USHORT i, y, t, End, DoDelete = FALSE;
   SHORT Line, TopLine = 3;
   CHAR FileName[128], Redraw, Titles, *p;
   class TCollection List;
   class TFileTag *Tag = User->FileTag;

   if (Data == NULL) {
      if ((Data = new TFileBase (Cfg->SystemPath, Current->Key)) != NULL)
         Data->SortByName ();
      DoDelete = TRUE;
   }

   if (Data != NULL) {
      if (User->FullScreen == TRUE && (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE)) {
         List.Clear ();
         i = 0;
         if (Data->First () == TRUE) {
            do {
               List.Add (Data->Name);
               i++;
            } while (Data->Next () == TRUE && i < (User->ScreenHeight - TopLine - 2));

            y = TopLine;
            List.First ();
            Data->First ();
            End = FALSE;
            Titles = Redraw = TRUE;

            while (End == FALSE && Embedded->AbortSession () == FALSE) {
               if (Redraw == TRUE) {
                  while (List.Previous () != NULL)
                     Data->Previous ();

                  if (Titles == TRUE) {
                     Embedded->Printf ("\x0C");
                     Embedded->Printf (Language->Text (LNG_FILELISTHEADER));

                     Embedded->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, Language->Text (LNG_FILELISTSEPARATOR));
                     Embedded->Printf (Language->Text (LNG_FILELISTDESCRIPTION1));
                     Embedded->Printf (Language->Text (LNG_FILELISTDESCRIPTION2));

                     Embedded->PrintfAt (TopLine, 1, "");
                  }

                  i = 0;
                  do {
                     if ((p = (PSZ)Data->Description->First ()) == NULL)
                        p = "";
                     if (Tag->Check (Data->Name) == TRUE)
                        Embedded->Printf (Language->Text (LNG_FILELISTTAGGED), Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                     else
                        Embedded->Printf (Language->Text (LNG_FILELISTNORMAL), Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                     i++;
                  } while (Data->Next () == TRUE && i < (User->ScreenHeight - TopLine - 2));

                  List.Last ();
                  for (t = (USHORT)(i + TopLine - 1); t > y; t--) {
                     List.Previous ();
                     Data->Previous ();
                  }

                  if (i < (User->ScreenHeight - 2 - TopLine)) {
                     do {
                        Embedded->Printf ("\n");
                        i++;
                     } while (i < (User->ScreenHeight - 2 - TopLine));
                  }
                  else
                     Data->Previous ();

                  Embedded->PrintfAt (y, 2, "\026\001\x70%-12.12s", (PSZ)List.Value ());
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
                     case CTRLD:
                        Embedded->Printf ("\x0C\n");
                        if ((p = (PSZ)Data->Description->First ()) == NULL)
                           p = "";
                        Embedded->Printf ("\026\001\016 %-12.12s \026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);

                        sprintf (FileName, "%s%s", Current->Download, Data->Name);
                        if (DownloadFile (FileName, Data->Name, Data->Size) == TRUE) {
                           Data->DlTimes++;
                           Data->ReplaceHeader ();
                        }

                        Titles = Redraw = TRUE;
                        break;

                     case CTRLT:
                        if (Tag->TotalFiles > 0) {
                           Embedded->Printf ("\x0C\n");
                           Download (Tag);
                           Titles = Redraw = TRUE;
                        }
                        break;

                     case ' ':
                        if (Tag->Check (Data->Name) == FALSE) {
                           Tag->New ();
                           strcpy (Tag->Name, Data->Name);
                           strcpy (Tag->Area, Data->Area);
                           if (Data->Complete[0] == '\0')
                              sprintf (Tag->Complete, "%s%s", Current->Download, Tag->Name);
                           else
                              strcpy (Tag->Complete, Data->Complete);
                           Tag->Size = Data->Size;
                           Tag->Add ();
                        }
                        else
                           Tag->Remove (Data->Name);
                        Embedded->PrintfAt (1, 29, " \x16\x01\013CTRL-T \x16\x01\016downloads tagged files (%d tagged).", Tag->TotalFiles);
                        if (Tag->TotalFiles == 0)
                           Embedded->PrintfAt (1, 29, "\x1B[K");
                        if (Tag->Check (Data->Name) == TRUE)
                           Embedded->PrintfAt (y, 1, "\026\001\016*%-12.12s*", Data->Name);
                        else
                           Embedded->PrintfAt (y, 1, "\026\001\016 %-12.12s ", Data->Name);
                        Embedded->PrintfAt (y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                        break;

                     case CTRLY:
                        Embedded->PrintfAt (y, 2, "\x16\x01\x0E%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                        while (List.Previous () != NULL) {
                           Data->Previous ();
                           y--;
                        }
                        if (Data->Previous () == TRUE) {
                           Embedded->PrintfAt (TopLine, 1, "");
                           for (i = 0; i < (User->ScreenHeight - TopLine - 2); i++)
                              Embedded->Printf ("\x1B[K\n");
                           for (i = 0; i < (User->ScreenHeight - TopLine - 2 - 1); i++)
                              Data->Previous ();
                           List.Clear ();
                           Embedded->PrintfAt (TopLine, 1, "");
                           i = 0;
                           do {
                              if ((p = (PSZ)Data->Description->First ()) == NULL)
                                 p = "";
                              if (Tag->Check (Data->Name) == TRUE)
                                 Embedded->Printf ("\026\001\016*%-12.12s*\026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                              else
                                 Embedded->Printf ("\026\001\016 %-12.12s \026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                              List.Add (Data->Name, (USHORT)(strlen (Data->Name) + 1));
                              i++;
                           } while (Data->Next () == TRUE && i < (User->ScreenHeight - TopLine - 2));
                           Data->Previous ();
                           y = (USHORT)(TopLine + User->ScreenHeight - TopLine - 2 - 1);
                           List.Last ();
                           Embedded->PrintfAt (y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                        }
                        break;

                     case CTRLE:
                     case 0x4800:
                        if (List.Previous () != NULL) {
                           Embedded->PrintfAt (y, 2, "\x16\x01\x0E%-12.12s\x16\x01\x0E", (PSZ)List.Next ());
                           Embedded->PrintfAt (--y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Previous ());
                           Data->Previous ();
                        }
                        else if (Data->Previous () == TRUE) {
                           Embedded->PrintfAt (TopLine, 1, "");
                           for (i = 0; i < (User->ScreenHeight - TopLine - 2); i++)
                              Embedded->Printf ("\x1B[K\n");
                           for (i = 0; i < (User->ScreenHeight - TopLine - 2 - 1); i++)
                              Data->Previous ();
                           List.Clear ();
                           Embedded->PrintfAt (TopLine, 1, "");
                           i = 0;
                           do {
                              if ((p = (PSZ)Data->Description->First ()) == NULL)
                                 p = "";
                              if (Tag->Check (Data->Name) == TRUE)
                                 Embedded->Printf ("\026\001\016*%-12.12s*\026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                              else
                                 Embedded->Printf ("\026\001\016 %-12.12s \026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                              List.Add (Data->Name, (USHORT)(strlen (Data->Name) + 1));
                              i++;
                           } while (Data->Next () == TRUE && i < (User->ScreenHeight - TopLine - 2));
                           Data->Previous ();
                           y = (USHORT)(TopLine + User->ScreenHeight - TopLine - 2 - 1);
                           List.Last ();
                           Embedded->PrintfAt (y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                        }
                        break;

                     case '\r':
                        FileDetails (Data);
                        Titles = Redraw = TRUE;
                        break;

                     case CTRLV:
                        Embedded->PrintfAt (y, 2, "\x16\x01\x0E%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                        while (List.Next () != NULL) {
                           Data->Next ();
                           y++;
                        }
                        if (Data->Next () == TRUE) {
                           Embedded->PrintfAt (TopLine, 1, "");
                           for (i = 0; i < (User->ScreenHeight - TopLine - 2); i++)
                              Embedded->Printf ("\x1B[K\n");
                           List.Clear ();
                           Embedded->PrintfAt (TopLine, 1, "");
                           i = 0;
                           do {
                              if ((p = (PSZ)Data->Description->First ()) == NULL)
                                 p = "";
                              if (Tag->Check (Data->Name) == TRUE)
                                 Embedded->Printf ("\026\001\016*%-12.12s*\026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                              else
                                 Embedded->Printf ("\026\001\016 %-12.12s \026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                              List.Add (Data->Name, (USHORT)(strlen (Data->Name) + 1));
                           } while (Data->Next () == TRUE && ++i < (User->ScreenHeight - TopLine - 2));
                           for (y = 0; y < i; y++)
                              Data->Previous ();
                           y = TopLine;
                           List.First ();
                           Embedded->PrintfAt (y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                        }
                        break;

                     case CTRLX:
                     case 0x5000:
                        if (List.Next () != NULL) {
                           Embedded->PrintfAt (y, 2, "\x16\x01\x0E%-12.12s\x16\x01\x0E", (PSZ)List.Previous ());
                           Embedded->PrintfAt (++y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Next ());
                           Data->Next ();
                        }
                        else if (Data->Next () == TRUE) {
                           Embedded->PrintfAt (TopLine, 1, "");
                           for (i = 0; i < (User->ScreenHeight - TopLine - 2); i++)
                              Embedded->Printf ("\x1B[K\n");
                           List.Clear ();
                           Embedded->PrintfAt (TopLine, 1, "");
                           i = 0;
                           do {
                              if ((p = (PSZ)Data->Description->First ()) == NULL)
                                 p = "";
                              if (Tag->Check (Data->Name) == TRUE)
                                 Embedded->Printf ("\026\001\016*%-12.12s*\026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                              else
                                 Embedded->Printf ("\026\001\016 %-12.12s \026\001\015%4ldK \026\001\002%2d/%02d/%02d \026\001\003%.50s\n", Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
                              List.Add (Data->Name, (USHORT)(strlen (Data->Name) + 1));
                           } while (Data->Next () == TRUE && ++i < (User->ScreenHeight - TopLine - 2));
                           for (y = 0; y < i; y++)
                              Data->Previous ();
                           y = TopLine;
                           List.First ();
                           Embedded->PrintfAt (y, 2, "\x16\x01\x70%-12.12s\x16\x01\x0E", (PSZ)List.Value ());
                        }
                        break;

                     case 'X':
                        End = TRUE;
                        break;
                  }
               }

#if defined(__OS2__)
               DosSleep (1L);
#elif defined(__NT__)
               Sleep (1L);
#endif
            }

            Embedded->Printf ("\x0C");
         }
         else
            Embedded->Printf (Language->Text (LNG_FILELISTNOFILESFOUND));
      }
      else {
         if (Data->First () == TRUE) {
            Embedded->BufferedPrintf ("\x0C");
            Embedded->BufferedPrintf (Language->Text (LNG_FILELISTHEADER));
            Line = 2;
            do {
               if ((p = (PSZ)Data->Description->First ()) == NULL)
                  p = "";
               if (Tag->Check (Data->Name) == TRUE)
                  Embedded->BufferedPrintf (Language->Text (LNG_FILELISTTAGGED), Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
               else
                  Embedded->BufferedPrintf (Language->Text (LNG_FILELISTNORMAL), Data->Name, (Data->Size + 1023) / 1024, Data->UplDate.Day, Data->UplDate.Month, Data->UplDate.Year % 100, p);
               if ((Line = MoreQuestion (Line)) == 1) {
                  Embedded->BufferedPrintf ("\x0C");
                  Embedded->BufferedPrintf (Language->Text (LNG_FILELISTHEADER));
                  Line = 2;
               }
               while ((p = (PSZ)Data->Description->Next ()) != NULL && Embedded->AbortSession () == FALSE && Line != 0) {
                  Embedded->BufferedPrintf (Language->Text (LNG_FILELISTCOMMENT), p);
                  if ((Line = MoreQuestion (Line)) == 1) {
                     Embedded->BufferedPrintf ("\x0C");
                     Embedded->BufferedPrintf (Language->Text (LNG_FILELISTHEADER));
                     Line = 2;
                  }
               }
            } while (Data->Next () == TRUE && Embedded->AbortSession () == FALSE && Line != 0);

            if (Line > 3)
               MoreQuestion (99);
         }
         else
            Embedded->Printf (Language->Text (LNG_FILELISTNOFILESFOUND));
      }

      if (DoDelete == TRUE) {
         Data->Close ();
         delete Data;
      }
   }
}

VOID TLibrary::ListRecentFiles (VOID)
{
   CHAR Range;
   class TFileBase *Data;

   if ((Range = (CHAR)SearchRange ()) != 'X') {
      if ((Data = new TFileBase (Cfg->SystemPath, (Range == 'A') ? "" : Current->Key)) != NULL) {
         Data->SortByDate ();
         ListFiles (Data);
         Data->Close ();
         delete Data;
      }
   }
}

VOID TLibrary::AddTagged (VOID)
{
   CHAR Names[64], *p;
   ULONG DlTime;
   class TFileBase *Data;
   class TFileTag *Files = User->FileTag;

   do {
      Embedded->Printf (Language->Text (LNG_FILETOTAG), Files->TotalFiles + 1);
      Embedded->Input (Names, (USHORT)(sizeof (Names) - 1), 0);

      if ((Data = new TFileBase (Cfg->SystemPath, "")) != NULL) {
         if ((p = strtok (Names, " ")) != NULL)
            do {
               if (Data->Read (p) == TRUE) {
                  Files->New ();
                  strcpy (Files->Name, Data->Name);
                  strcpy (Files->Area, Data->Area);
                  Files->Size = Data->Size;
                  if (Data->Complete[0] == '\0')
                     sprintf (Files->Complete, "%s%s", Current->Download, Files->Name);
                  else
                     strcpy (Files->Complete, Data->Complete);
                  Files->CdRom = Data->CdRom;
                  if (Files->Add () == TRUE) {
                     DlTime = Data->Size / (CarrierSpeed / 10L);
                     Embedded->Printf (Language->Text (LNG_FILETAGCONFIRM), Files->Index, Data->Name, DlTime / 60L, DlTime % 60L, Data->Size);
                  }
               }
               else
                  Embedded->Printf (Language->Text (LNG_FILENOTFOUND), p);
            } while ((p = strtok (NULL, " ")) != NULL);
         Data->Close ();
         delete Data;
      }
   } while (Names[0] != '\0');
}

VOID TLibrary::ListTagged (VOID)
{
   ULONG DlTime;
   class TFileTag *Tag = User->FileTag;

   Tag->Reindex ();

   if (Tag->First () == TRUE) {
      Embedded->Printf (Language->Text (LNG_FILETAGGEDHEADER));

      do {
         DlTime = Tag->Size / (CarrierSpeed / 10L);
         Embedded->Printf (Language->Text (LNG_FILETAGGEDLIST), Tag->Index, Tag->Name, Tag->Area, DlTime / 60L, DlTime % 60L, Tag->Size);
      } while (Tag->Next () == TRUE);

      DlTime = Tag->TotalBytes / (CarrierSpeed / 10L);
      Embedded->Printf (Language->Text (LNG_FILETAGGEDTOTAL), Tag->TotalBytes, DlTime / 60L, DlTime % 60L);
   }
   else
      Embedded->Printf (Language->Text (LNG_FILENOTAGGED));
}

VOID TLibrary::DeleteTagged (VOID)
{
   SHORT Index;
   CHAR Temp[32];
   class TFileTag *Tag = User->FileTag;

   Tag->Reindex ();

   if (Tag->First () == TRUE) {
      Embedded->Printf (Language->Text (LNG_FILETODETAG), Tag->TotalFiles);
      Embedded->Input (Temp, (USHORT)(sizeof (Temp) - 1), 0);

      if (Embedded->AbortSession () == FALSE) {
         if (toupper (Temp[0] == 'A')) {
            while (Tag->First () == TRUE) {
               Embedded->Printf (Language->Text (LNG_FILEDETAGGED), Tag->Name);
               Tag->Remove ();
            }
            Embedded->Printf (Language->Text (LNG_FILETAGEMPTY));
         }
         else {
            Index = (SHORT)atoi (Temp);
            if (Index > 0) {
               if (Tag->Select ((USHORT)abs (Index)) == TRUE)
                  Tag->Remove ();
            }
         }
      }
   }
}

VOID TLibrary::DeleteAllTagged (VOID)
{
   class TFileTag *Tag = User->FileTag;

   Tag->Reindex ();

   if (Tag->First () == TRUE) {
      while (Tag->First () == TRUE)
         Tag->Remove ();
      Embedded->Printf (Language->Text (LNG_FILETAGEMPTY));
   }
}

VOID TLibrary::RemoveFiles (VOID)
{
   CHAR FileName[32];
   class TFileBase *Data;

   Embedded->Printf (Language->Text (LNG_FILENAMETODELETE));
   Embedded->Input (FileName, sizeof (FileName) - 1, 0);

   if (FileName[0] != '\0') {
      if ((Data = new TFileBase (Cfg->SystemPath, Current->Key)) != NULL) {
         if (Data->Read (FileName) == TRUE) {
            Data->Delete ();
            Embedded->Printf (Language->Text (LNG_FILEDELETED), strupr (FileName));

            Current->ActiveFiles--;
            Current->Update ();
            if (Log != NULL)
               Log->Write ("+Deleted file %s from library %s", FileName, Current->Key);
         }
         else
            Embedded->Printf (Language->Text (LNG_FILENOTFOUND), FileName);

         Data->Close ();
         delete Data;
      }
   }
}

VOID TLibrary::SearchFileName (VOID)
{
   CHAR Keyword[32], Range;
   class TFileBase *Data;

   Embedded->Printf ("\n\026\001\012Enter the file name to find, below.\n\nYou may specify \"wildcards\" such as *.ANS or LA*.ZIP for more exact searches.\n\n\026\001\013Search file name: ");
   Embedded->Input (Keyword, (USHORT)(sizeof (Keyword) - 1), 0);

   if (Keyword[0] != '\0' && Embedded->AbortSession () == FALSE) {
      if ((Range = (CHAR)SearchRange ()) != 'X') {
         if ((Data = new TFileBase (Cfg->SystemPath, (Range == 'A') ? "" : Current->Key)) != NULL) {
            Data->SearchFile (Keyword);
            ListFiles (Data);
            Data->Close ();
            delete Data;
         }
      }
   }
}

VOID TLibrary::SearchKeyword (VOID)
{
   CHAR Keyword[32], Range;
   class TFileBase *Data;

   Embedded->Printf ("\n\x16\x01\x0BOn the line below, enter your keyword list, ? for help, or RETURN to exit.\n: ");
   Embedded->Input (Keyword, (USHORT)(sizeof (Keyword) - 1), 0);

   if (Keyword[0] != '\0' && Embedded->AbortSession () == FALSE) {
      if ((Range = (CHAR)SearchRange ()) != 'X') {
         if ((Data = new TFileBase (Cfg->SystemPath, (Range == 'A') ? "" : Current->Key)) != NULL) {
            Data->SearchKeyword (Keyword);
            ListFiles (Data);
            Data->Close ();
            delete Data;
         }
      }
   }
}

VOID TLibrary::SearchNewFiles (VOID)
{
   CHAR Range;
   class TFileBase *Data;

   if ((Range = (CHAR)SearchRange ()) != 'X') {
      if ((Data = new TFileBase (Cfg->SystemPath, (Range == 'A') ? "" : Current->Key)) != NULL) {
         Data->SortByDate (User->LastCall);
         ListFiles (Data);
         Data->Close ();
         delete Data;
      }
   }
}

USHORT TLibrary::SearchRange (VOID)
{
   CHAR Range[4];

   while (Embedded->AbortSession () == FALSE) {
      Embedded->Printf ("\n\x16\x01\012Which Libraries would you like included in the list of search?\n\n");
      Embedded->Printf ("  \x16\x01\013C \x16\x01\016... Current Library (%s)\n", Current->Key);
      Embedded->Printf ("  \x16\x01\013A \x16\x01\016... All Libraries\n\n");
      Embedded->Printf ("\x16\x01\013Select an option, X to exit, or RETURN for the current Library: ");
      Embedded->Input (Range, 1, INP_HOTKEY);
      Range[0] = (CHAR)toupper (Range[0]);
      if (Range[0] == 'A' || Range[0] == 'C' || Range[0] == 'X' || Range[0] == '\0')
         break;
   }

   return ((USHORT)toupper (Range[0]));
}

VOID TLibrary::SearchText (VOID)
{
   CHAR Keyword[32], Range;
   class TFileBase *Data;

   Embedded->Printf ("\n\x16\x01\013Enter the text to search, ? for help, or RETURN to exit.\n: ");
   Embedded->Input (Keyword, (USHORT)(sizeof (Keyword) - 1), 0);

   if (Keyword[0] != '\0' && Embedded->AbortSession () == FALSE) {
      if ((Range = (CHAR)SearchRange ()) != 'X') {
         if ((Data = new TFileBase (Cfg->SystemPath, (Range == 'A') ? "" : Current->Key)) != NULL) {
            Data->SearchText (Keyword);
            ListFiles (Data);
            Data->Close ();
            delete Data;
         }
      }
   }
}

USHORT TLibrary::SelectArea (PSZ pszArea)
{
   USHORT RetVal = FALSE, FirstHit, DoList;
   SHORT Line;
   CHAR Command[16], Temp[128];
   class TFileData *File;
   class TFileAreaListing *List;

   DoList = FALSE;
   if (pszArea != NULL && *pszArea != '\0') {
      strcpy (Command, pszArea);
      DoList = TRUE;
   }

   if ((File = new TFileData (DataPath)) != NULL) {
      do {
         if (DoList == FALSE) {
            Embedded->Printf (Language->Text (LNG_FILEAREAREQUEST));
            Embedded->Input (Command, (USHORT)(sizeof (Command) - 1), INP_FIELD);
         }
         else
            DoList = FALSE;

         if (!stricmp (Command, "?")) {
            if (User->FullScreen == TRUE && (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE)) {
               if ((List = new TFileAreaListing) != NULL) {
                  List->Cfg = Cfg;
                  List->Embedded = Embedded;
                  List->Log = Log;
                  List->Current = Current;
                  List->User = User;
                  List->Language = Language;
                  RetVal = List->Run ();
                  delete List;
               }
            }
            else {
               if (File->First () == TRUE) {
                  Embedded->Printf ("\x0C");
                  Embedded->Printf (Language->Text (LNG_FILEAREAHEADER));
                  Line = 3;
                  do {
                     if (User->Level >= File->Level) {
                        if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                           Embedded->Printf (Language->Text (LNG_FILEAREALIST), File->Key, File->ActiveFiles, File->Display);
                           if ((Line = Embedded->MoreQuestion (Line)) == 1) {
                              Embedded->Printf ("\x0C");
                              Embedded->Printf (Language->Text (LNG_FILEAREAHEADER));
                              Line = 3;
                           }
                        }
                     }
                  } while (Line != 0 && Embedded->AbortSession () == FALSE && File->Next () == TRUE);
               }
            }
         }
         else if (!stricmp (Command, "[")) {
            File->Read (Current->Key, FALSE);
            if (File->Previous () == TRUE)
               do {
                  if (User->Level >= File->Level) {
                     if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                        Current->Read (File->Key);
                        if (User != NULL) {
                           strcpy (User->LastFileArea, Current->Key);
                           User->Update ();
                        }
                        if (Current->Download[strlen (Current->Download) - 1] != '\\')
                           strcat (Current->Download, "\\");
                        if (Current->Upload[strlen (Current->Upload) - 1] != '\\')
                           strcat (Current->Upload, "\\");
                        Log->Write (":File Area: %s - %s", File->Key, File->Display);
                        RetVal = TRUE;
                        break;
                     }
                  }
               } while (File->Previous () == TRUE);
         }
         else if (!stricmp (Command, "]")) {
            File->Read (Current->Key, FALSE);
            if (File->Next () == TRUE)
               do {
                  if (User->Level >= File->Level) {
                     if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                        Current->Read (File->Key);
                        if (User != NULL) {
                           strcpy (User->LastFileArea, Current->Key);
                           User->Update ();
                        }
                        if (Current->Download[strlen (Current->Download) - 1] != '\\')
                           strcat (Current->Download, "\\");
                        if (Current->Upload[strlen (Current->Upload) - 1] != '\\')
                           strcat (Current->Upload, "\\");
                        Log->Write (":File Area: %s - %s", File->Key, File->Display);
                        RetVal = TRUE;
                        break;
                     }
                  }
               } while (File->Next () == TRUE);
         }
         else if (Command[0] != '\0') {
            if (File->Read (Command) == TRUE) {
               if (User->Level >= File->Level) {
                  if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                     Current->Read (Command);

                     if (User != NULL) {
                        strcpy (User->LastFileArea, Current->Key);
                        User->Update ();
                     }

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
               if (User->FullScreen == TRUE && (Embedded->Ansi == TRUE || Embedded->Avatar == TRUE)) {
                  if ((List = new TFileAreaListing) != NULL) {
                     List->Cfg = Cfg;
                     List->Embedded = Embedded;
                     List->Log = Log;
                     List->Current = Current;
                     List->User = User;
                     strcpy (List->Command, Command);
                     List->Language = Language;
                     RetVal = List->Run ();
                     delete List;
                  }
               }
               else {
                  FirstHit = TRUE;
                  strupr (Command);
                  Line = 3;

                  if (File->First () == TRUE) {
                     do {
                        if (User->Level >= File->Level) {
                           if ((File->AccessFlags & User->AccessFlags) == File->AccessFlags) {
                              strcpy (Temp, File->Key);
                              if (strstr (strupr (Temp), Command) != NULL) {
                                 if (FirstHit == TRUE)
                                    Embedded->Printf (Language->Text (LNG_FILEAREAHEADER));
                                 Embedded->Printf (Language->Text (LNG_FILEAREALIST), File->Key, File->ActiveFiles, File->Display);
                                 Line = Embedded->MoreQuestion (Line);
                                 FirstHit = FALSE;
                              }
                              else {
                                 strcpy (Temp, File->Display);
                                 if (strstr (strupr (Temp), Command) != NULL) {
                                    if (FirstHit == TRUE)
                                       Embedded->Printf (Language->Text (LNG_FILEAREAHEADER));
                                    Embedded->Printf (Language->Text (LNG_FILEAREALIST), File->Key, File->ActiveFiles, File->Display);
                                    Line = Embedded->MoreQuestion (Line);
                                    FirstHit = FALSE;
                                 }
                              }
                           }
                        }
                     } while (Line != 0 && Embedded->AbortSession () == FALSE && File->Next () == TRUE);
                  }

                  if (FirstHit == TRUE)
                     Embedded->Printf (Language->Text (LNG_FILEAREANOTAVAILABLE));
               }
               if (pszArea != NULL && *pszArea != '\0')
                  Command[0] = '\0';
            }
         }
      } while (Command[0] != '\0' && RetVal == FALSE && Embedded->AbortSession () == FALSE);

      delete File;
   }

   return (RetVal);
}

VOID TLibrary::Upload (VOID)
{
   SHORT fRet, BatchTransfer, DoTransfer;
   CHAR szAnswer[32], szProtocol[10], szFile[128], *p, *q, *RxFile;
   struct stat statbuf;
   struct dosdate_t date;
   struct dostime_t time;
   struct tm *timep;
   class TFileBase *Data;
   class TTransfer *Transfer;
   class TCollection Received;
   class TCommentEditor *Editor;

   if (User->Level >= Current->UploadLevel) {
      if ((User->AccessFlags & Current->UploadFlags) == Current->UploadFlags) {
         BatchTransfer = FALSE;

         while (Embedded->AbortSession () == FALSE) {
            DoTransfer = FALSE;

            Embedded->Printf ("\n\x16\x01\012Upload to the %s library\n\n", Current->Key);

            Embedded->Printf ("\x16\x01\013<file name> \x16\x01\016... Upload a file to this library\n");
            Embedded->Printf ("\x16\x01\013          * \x16\x01\016... Upload multiple files at once\n");
//            Embedded->Printf ("\x16\x01\013          M \x16\x01\016... Modify or add descriptions to files you have uploaded\n");

            Embedded->Printf ("\n\x16\x01\013Enter your selection, ? for more help, or RETURN to exit: ");
            Embedded->Input (szAnswer, (USHORT)(sizeof (szAnswer) - 1), 0);
            if (szAnswer[0] == '\0')
               break;

            if (!stricmp (szAnswer, "?")) {
               Embedded->DisplayFile ("UPLOAD");
               DoTransfer = FALSE;
            }
            else if (!stricmp (szAnswer, "*")) {
               BatchTransfer = TRUE;
               DoTransfer = TRUE;
            }
            else if (!stricmp (szAnswer, "M"))
               DoTransfer = FALSE;
            else {
               BatchTransfer = FALSE;
               DoTransfer = TRUE;
            }

            if (DoTransfer == TRUE) {
               if (BatchTransfer == FALSE) {
                  if ((Data = new TFileBase (Cfg->SystemPath, Current->Key)) != NULL) {
                     if (Data->Read (szAnswer) == TRUE) {
                        Embedded->Printf ("\n\x16\x01\013*** File exists!\n");
                        DoTransfer = FALSE;
                     }
                     Data->Close ();
                     delete Data;
                  }
               }

               if (DoTransfer == TRUE) {
                  Embedded->Printf ("\n\x16\x01\012To start uploading %s, type:\n\n", strupr (szAnswer));

                  if (BatchTransfer == FALSE) {
                     Embedded->Printf ("  \x16\x01\013A ... \x16\x01\016ASCII\n");
                     Embedded->Printf ("  \x16\x01\013M ... \x16\x01\016XMODEM (Checksum/CRC)\n");
                     Embedded->Printf ("  \x16\x01\0131 ... \x16\x01\016XMODEM-1K\n");
                  }
                  Embedded->Printf ("  \x16\x01\013Z ... \x16\x01\016ZMODEM\n");
//                  Embedded->Printf ("  \x16\x01\013F ... \x16\x01\016File Import (existing file)\n");

                  Embedded->Printf ("\n\x16\x01\013Choose an upload option, or RETURN to exit: ");
                  if (Embedded->HotKey == TRUE)
                     Embedded->Input (szProtocol, 1, INP_HOTKEY);
                  else
                     Embedded->Input (szProtocol, (USHORT)(sizeof (szProtocol) - 1), 0);

                  fRet = FALSE;

                  if ((Transfer = new TTransfer) != NULL) {
                     Transfer->Com = Embedded->Com;
                     Transfer->Log = Log;
                     Transfer->Speed = CarrierSpeed;
                     Transfer->Progress = Progress;
                     Transfer->Telnet = Cfg->ZModemTelnet;

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

                  Embedded->Printf ("\n");

                  if (fRet == TRUE) {
                     if ((Data = new TFileBase (Cfg->SystemPath, Current->Key)) == NULL)
                        return;

                     if ((RxFile = (CHAR *)Received.First ()) != NULL)
                        do {
                           for (p = RxFile, q = RxFile; *p; p++) {
                              if (*p == '/' || *p == '\\' || *p == ':')
                                 q = p + 1;
                           }

                           Embedded->Printf ("\n\x16\x01\016Now adding %s to the %s library.\n", q, Current->Key);

                           stat (RxFile, &statbuf);
                           if (User != NULL) {
                              User->UploadFiles++;
                              User->UploadBytes += statbuf.st_size;
                           }

                           Data->Clear ();
                           strcpy (Data->Area, Current->Key);
                           strcpy (Data->Name, strupr (q));
                           Data->Size = statbuf.st_size;
                           _dos_getdate (&date);
                           _dos_gettime (&time);
                           Data->UplDate.Day = date.day;
                           Data->UplDate.Month = date.month;
                           Data->UplDate.Year = date.year;
                           Data->UplDate.Hour = time.hour;
                           Data->UplDate.Minute = time.minute;
                           timep = localtime (&statbuf.st_mtime);

                           Data->Date.Day = (UCHAR)timep->tm_mday;
                           Data->Date.Month = (UCHAR)(timep->tm_mon + 1);
                           Data->Date.Year = (USHORT)(timep->tm_year + 1900);
                           Data->Date.Hour = (UCHAR)timep->tm_hour;
                           Data->Date.Minute = (UCHAR)timep->tm_min;

                           Data->Uploader = User->Name;

                           if ((Editor = new TCommentEditor) != NULL) {
                              Editor->Embedded = Embedded;
                              Editor->File = Data;
                              Editor->Width = 60;
                              if (Editor->Write () == TRUE)
                                 Editor->Menu ();
                           }

                           Data->Add ();
                           Current->ActiveFiles++;
                        } while ((RxFile = (CHAR *)Received.Next ()) != NULL);

                     Data->Close ();
                     delete Data;

                     Current->Update ();
                  }
               }
            }
         }
      }
      else
         Embedded->Printf ("\n\x16\x01\016   *** Sorry, you can't upload files to this Library! ***\n\006\007\006\007");
   }
   else
      Embedded->Printf ("\n\x16\x01\016   *** Sorry, you can't upload files to this Library! ***\n\006\007\006\007");
}

VOID TLibrary::UploadUser (PSZ user)
{
   SHORT fRet, BatchTransfer, DoTransfer;
   CHAR Name[48], szAnswer[32], szProtocol[10], szFile[128], *p, *q, *RxFile;
   struct stat statbuf;
   class TTransfer *Transfer;
   class TCollection Received;
   class TUser *DestUser;

   if ((DestUser = new TUser (Cfg->UserFile)) != NULL) {
      if (user == NULL || *user == '\0') {
         Embedded->Printf ("\n\026\001\013Write the user's name or Enter to exit)\n: \026\001\x1E");
         Embedded->Input (Name, (USHORT)(sizeof (Name) - 1), INP_FIELD|INP_FANCY);
      }
      else
         strcpy (Name, user);

      if (Name[0] != '\0' && DestUser->GetData (Name) == TRUE) {
         BatchTransfer = FALSE;
         while (Embedded->AbortSession () == FALSE) {
            DoTransfer = FALSE;

            Embedded->Printf ("\n\x16\x01\012Upload for %s\n\n", Name);

            Embedded->Printf ("\x16\x01\013<file name> \x16\x01\016... Upload a file\n");
            Embedded->Printf ("\x16\x01\013          * \x16\x01\016... Upload multiple files at once\n");

            Embedded->Printf ("\n\x16\x01\013Enter your selection, ? for more help, or RETURN to exit: ");
            Embedded->Input (szAnswer, (USHORT)(sizeof (szAnswer) - 1), 0);
            if (szAnswer[0] == '\0')
               break;

            if (!stricmp (szAnswer, "?")) {
               Embedded->DisplayFile ("USRUPLD");
               DoTransfer = FALSE;
            }
            else if (!stricmp (szAnswer, "*")) {
               BatchTransfer = TRUE;
               DoTransfer = TRUE;
            }
            else {
               BatchTransfer = FALSE;
               DoTransfer = TRUE;
            }

            if (DoTransfer == TRUE) {
               Embedded->Printf ("\n\x16\x01\012To start uploading %s, type:\n\n", strupr (szAnswer));

               if (BatchTransfer == FALSE) {
                  Embedded->Printf ("  \x16\x01\013A ... \x16\x01\016ASCII\n");
                  Embedded->Printf ("  \x16\x01\013M ... \x16\x01\016XMODEM (Checksum/CRC)\n");
                  Embedded->Printf ("  \x16\x01\0131 ... \x16\x01\016XMODEM-1K\n");
               }
               Embedded->Printf ("  \x16\x01\013Z ... \x16\x01\016ZMODEM\n");

               Embedded->Printf ("\n\x16\x01\013Choose an upload option, or RETURN to exit: ");
               if (Embedded->HotKey == TRUE)
                  Embedded->Input (szProtocol, 1, INP_HOTKEY);
               else
                  Embedded->Input (szProtocol, (USHORT)(sizeof (szProtocol) - 1), 0);

               fRet = FALSE;

               if ((Transfer = new TTransfer) != NULL) {
                  Transfer->Com = Embedded->Com;
                  Transfer->Log = Log;
                  Transfer->Speed = CarrierSpeed;
                  Transfer->Progress = Progress;
                  Transfer->Telnet = Cfg->ZModemTelnet;

                  sprintf (szFile, "%s%s\\", Cfg->UsersHomePath, DestUser->MailBox);
                  BuildPath (AdjustPath (szFile));
                  strcat (szFile, szAnswer);
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
                        sprintf (szFile, "%s%s\\", Cfg->UsersHomePath, DestUser->MailBox);
                        AdjustPath (szFile);
                        if ((p = Transfer->ReceiveZModem (szFile)) != NULL) {
                           fRet = TRUE;
                           Received.Add (p, (USHORT)(strlen (p) + 1));
                           while ((p = Transfer->ReceiveZModem (szFile)) != NULL)
                              Received.Add (p, (USHORT)(strlen (p) + 1));
                        }
                        break;
                  }

                  delete Transfer;
               }

               Embedded->Printf ("\n");

               if (fRet == TRUE) {
                  if ((RxFile = (CHAR *)Received.First ()) != NULL) {
                     do {
                        for (p = RxFile, q = RxFile; *p; p++) {
                           if (*p == '/' || *p == '\\' || *p == ':')
                              q = p + 1;
                        }

                        Embedded->Printf ("\n\x16\x01\016Now adding %s to %s.\n", q, Name);

                        sprintf (szFile, "%s%s\\%s", Cfg->UsersHomePath, DestUser->MailBox, q);
                        AdjustPath (szFile);
                        stat (szFile, &statbuf);

                        DestUser->FileTag->New ();
                        strcpy (DestUser->FileTag->Name, q);
                        strcpy (DestUser->FileTag->Area, "PERSONAL");
                        strcpy (DestUser->FileTag->Complete, szFile);
                        DestUser->FileTag->Size = statbuf.st_size;
                        DestUser->FileTag->DeleteAfter = TRUE;
                        DestUser->FileTag->Add ();
                     } while ((RxFile = (CHAR *)Received.Next ()) != NULL);

                     DestUser->Update ();
                  }
               }
            }
         }

         sprintf (szFile, "%s%s", Cfg->UsersHomePath, DestUser->MailBox);
         rmdir (AdjustPath (szFile));
      }
      else if (Name[0] != '\0')
         Embedded->Printf ("\n\x16\x01\016   *** Sorry, you can't upload files to this User! ***\n\006\007\006\007");

      delete DestUser;
   }
}

// ----------------------------------------------------------------------

TFileAreaListing::TFileAreaListing (void)
{
   Command[0] = '\0';
   Language = NULL;
}

VOID TFileAreaListing::Begin (VOID)
{
   USHORT i, Add;
   CHAR Temp[128];
   LISTDATA ld;
   class TFileData *FileData;

   i = 0;
   y = 4;
   Found = FALSE;
   List.Clear ();
   Data.Clear ();
   strupr (Command);

   if ((FileData = new TFileData (Cfg->SystemPath)) != NULL) {
      if (FileData->First () == TRUE)
         do {
            if (User->Level >= FileData->Level) {
               if ((FileData->AccessFlags & User->AccessFlags) == FileData->AccessFlags) {
                  if (Command[0] != '\0') {
                     Add = FALSE;
                     strcpy (Temp, FileData->Key);
                     if (strstr (strupr (Temp), Command) != NULL)
                        Add = TRUE;
                     else {
                        strcpy (Temp, FileData->Display);
                        if (strstr (strupr (Temp), Command) != NULL)
                           Add = TRUE;
                     }
                  }

                  if (Command[0] == '\0' || Add == TRUE) {
                     strcpy (ld.Key, FileData->Key);
                     ld.ActiveFiles = FileData->ActiveFiles;
                     strcpy (ld.Display, FileData->Display);
                     Data.Add (&ld, sizeof (LISTDATA));
                  }
               }
            }
         } while (FileData->Next () == TRUE);

      delete FileData;
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

USHORT TFileAreaListing::DrawScreen (VOID)
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

VOID TFileAreaListing::PrintTitles (VOID)
{
   Embedded->Printf ("\x0C");
   Embedded->Printf (Language->Text (LNG_FILEAREAHEADER));

   Embedded->PrintfAt ((USHORT)(User->ScreenHeight - 2), 1, Language->Text (LNG_FILEAREASEPARATOR));

   Embedded->Printf (Language->Text (LNG_FILEAREADESCRIPTION1));
   Embedded->Printf (Language->Text (LNG_FILEAREADESCRIPTION2));

   Embedded->PrintfAt (4, 1, "");
}

VOID TFileAreaListing::PrintLine (VOID)
{
   Embedded->Printf (Language->Text (LNG_FILEAREALIST), pld->Key, pld->ActiveFiles, pld->Display);
}

VOID TFileAreaListing::PrintCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, Language->Text (LNG_FILEAREACURSOR), (PSZ)List.Value ());
}

VOID TFileAreaListing::RemoveCursor (USHORT y)
{
   Embedded->PrintfAt (y, 1, Language->Text (LNG_FILEAREAKEY), (PSZ)List.Value ());
}

VOID TFileAreaListing::Select (VOID)
{
   Current->Read ((PSZ)List.Value ());
   if (User != NULL) {
      strcpy (User->LastFileArea, Current->Key);
      User->Update ();
   }

   if (Current->Download[strlen (Current->Download) - 1] != '\\')
      strcat (Current->Download, "\\");
   if (Current->Upload[strlen (Current->Upload) - 1] != '\\')
      strcat (Current->Upload, "\\");
   Log->Write (":File Area: %s - %s", Current->Key, Current->Display);

   RetVal = End = TRUE;
}


