
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"
#include "lorawin.h"

PSZ CheckExts[] = { ".su", ".mo", ".tu", ".we", ".th", ".fr", ".sa", NULL };

VOID TMailProcessor::Import (VOID)
{
   int fd;
   USHORT SkipPacket, Empty;
   CHAR *Line, *Base;
   ULONG Number, DestNumber;
   class TUser *User;
   class TMsgBase *Extra;

   if (Inbound[0] != '\0') {
      if (Inbound[strlen (Inbound) - 1] == '\\' || Inbound[strlen (Inbound) - 1] == '/')
         Inbound[strlen (Inbound) - 1] = '\0';
#if defined(__LINUX__)
      strcat (Inbound, "/");
#else
      strcat (Inbound, "\\");
#endif
   }

   strcpy (Outbound, Cfg->Outbound);
   if (Outbound[strlen (Outbound) - 1] == '\\' || Outbound[strlen (Outbound) - 1] == '/')
      Outbound[strlen (Outbound) - 1] = '\0';

   if (Log != NULL)
      Log->Write ("+Import From %s", Inbound);

   Nodes = new TNodes (Cfg->NodelistPath);
   Data = new TMsgData (Cfg->SystemPath);
   Forward = new TEchoLink (Cfg->SystemPath);
   Dupes = new TDupes (Cfg->SystemPath);
   if ((Echo = new TEchotoss (Cfg->SystemPath)) != NULL)
      Echo->Load ();

   SeenBy = new TKludges;
   Path = new TKludges;

   Msg = NULL;
   TossedArea = 0L;
   Packet = NULL;
   BadArea = FALSE;
   Started = time (NULL);
   BytesProcessed = 0L;

   while (OpenNextPacket () == TRUE) {
      LastTag[0] = '\0';
      Packets++;

      if (Output != NULL)
         Output->Clear ();

      if (Log != NULL) {
         Log->Write ("+%s, %02d/%02d/%d, %02d:%02d", strupr (PktName), Packet->Date.Month, Packet->Date.Day, Packet->Date.Year, Packet->Date.Hour, Packet->Date.Minute);
         Log->Write (":Orig: %s, Dest: %s", Packet->FromAddress, Packet->ToAddress);
      }

      SkipPacket = FALSE;
      if (Nodes != NULL && Cfg->Secure == TRUE) {
         if (Nodes->Read (Packet->FromAddress) == TRUE) {
            if (Nodes->InPktPwd[0] != '\0' && strcmp (Nodes->InPktPwd, Packet->Password)) {
               SkipPacket = TRUE;
               if (Log != NULL)
                  Log->Write ("!Invalid Packet Password, His: %s, Ours: %s", Packet->Password, Nodes->InPktPwd);
            }
         }
      }

      Number = Packet->Lowest ();

      do {
         if (SkipPacket == TRUE)
            break;

         if (Packet->Read (Number, MAX_LINE_LENGTH) == TRUE) {
            // Vengono resettati i flags che non devono essere mantenuti durante il
            // passaggio da un nodo all'altro
            Packet->Sent = Packet->Received = FALSE;
            Packet->Crash = Packet->Direct = Packet->Hold = FALSE;

            if ((Line = (PSZ)Packet->Text.First ()) != NULL) {
               if (!strncmp (Line, "AREA:", 5)) {
                  ImportEchoMail (&Line[5]);
                  TossedArea++;
               }
               else {
                  if (Msg != NULL) {
                     if (TossedArea != 0L) {
                        if (Data != NULL && IsAreasBBS == FALSE) {
                           Data->ActiveMsgs = Msg->Number ();
                           Data->Update ();
                        }
                        if (Dupes != NULL)
                           Dupes->Save ();
                        if (Log != NULL)
                           Log->Write (":   %-20.20s (Toss=%04d)", LastTag, TossedArea);
                        if (Echo != NULL)
                           Echo->Add (LastTag);
                     }
                     TossedArea = 0L;
                     Msg->UnLock ();
                     delete Msg;
                  }
                  strcpy (LastTag, "NetMail");

                  Empty = TRUE;
                  if ((Line = (PSZ)Packet->Text.First ()) != NULL)
                     do {
                        if (*Line != '\001' && *Line != '\0') {
                           Empty = FALSE;
                           break;
                        }
                     } while ((Line = (PSZ)Packet->Text.Next ()) != NULL);

                  if (Log != NULL) {
                     if (Empty == TRUE)
                        Log->Write ("*Netmail: %s -> %s (Empty)", Packet->From, Packet->To);
                     else
                        Log->Write ("*Netmail: %s -> %s", Packet->From, Packet->To);
                     Log->Write ("*         %s -> %s", Packet->FromAddress, Packet->ToAddress);
                  }

                  if (Empty == FALSE || Cfg->ImportEmpty == TRUE) {
                     Msg = NULL;
                     if (Cfg->NetMailStorage == ST_JAM)
                        Msg = new JAM (Cfg->NetMailPath);
                     else if (Cfg->NetMailStorage == ST_SQUISH)
                        Msg = new SQUISH (Cfg->NetMailPath);
                     else if (Cfg->NetMailStorage == ST_FIDO)
                        Msg = new FIDOSDM (Cfg->NetMailPath);
                     else if (Cfg->NetMailStorage == ST_ADEPT)
                        Msg = new ADEPT (Cfg->NetMailPath);
                     else if (Cfg->NetMailStorage == ST_HUDSON)
                        Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->NetMailBoard);

                     if (Msg != NULL) {
                        Msg->New ();
                        Msg->Add (Packet);
                        DestNumber = Msg->Highest ();

                        if (Output != NULL) {
                           Base = "";
                           if (Cfg->NetMailStorage == ST_SQUISH)
                              Base = "Squish<tm>";
                           else if (Cfg->NetMailStorage == ST_JAM)
                              Base = "JAM";
                           else if (Cfg->NetMailStorage == ST_FIDO)
                              Base = "Fido *.MSG";
                           else if (Cfg->NetMailStorage == ST_ADEPT)
                              Base = "AdeptXBBS";
                           else if (Cfg->NetMailStorage == ST_HUDSON)
                              Base = "Hudson";
                           sprintf (Display, "%6lu %-32.32s %-12.12s %6lu", Number + 1L, LastTag, Base, Msg->UidToMsgn (DestNumber));
                           Output->Add (Display);
                        }

                        delete Msg;
                        Msg = NULL;
                     }
                     NetMail++;

                     if ((User = new TUser (Cfg->UserFile)) != NULL) {
                        if (User->GetData (Packet->To) == TRUE) {
                           Extra = NULL;
                           if (Cfg->MailStorage == ST_JAM)
                              Extra = new JAM (Cfg->MailPath);
                           else if (Cfg->MailStorage == ST_SQUISH)
                              Extra = new SQUISH (Cfg->MailPath);
                           else if (Cfg->MailStorage == ST_FIDO)
                              Extra = new FIDOSDM (Cfg->MailPath);
                           else if (Cfg->MailStorage == ST_ADEPT)
                              Extra = new ADEPT (Cfg->MailPath);
                           else if (Cfg->MailStorage == ST_HUDSON)
                              Extra = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->MailBoard);
                           if (Extra != NULL) {
                              Extra->Add (Packet);
                              delete Extra;
                           }
                        }
                        delete User;
                     }
                  }
                  else if (Output != NULL) {
                     sprintf (Display, "%6lu %-32.32s %-12.12s Empty, skipped", Number + 1L, LastTag, "");
                     Output->Add (Display);
                  }
               }
            }

            MsgTossed++;
         }
      } while (Packet->Next (Number) == TRUE);

      if (Msg != NULL) {
         if (TossedArea != 0L) {
            if (Data != NULL && IsAreasBBS == FALSE) {
               Data->ActiveMsgs = Msg->Number ();
               Data->Update ();
            }
            if (Dupes != NULL)
               Dupes->Save ();
            if (Log != NULL)
               Log->Write (":   %-20.20s (Toss=%04d)", LastTag, TossedArea);
            if (Echo != NULL)
               Echo->Add (LastTag);
         }
         TossedArea = 0L;
         Msg->UnLock ();
         delete Msg;
      }
      Msg = NULL;

      if (Packet != NULL) {
         Packet->Kill ();
         delete Packet;
         Packet = NULL;
      }
   }

   if (MsgTossed > 0L) {
      sprintf (Temp, "%sarealist.rsn", Cfg->SystemPath);
      if ((fd = open (Temp, O_BINARY|O_WRONLY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1)
         close (fd);
   }

   if ((Started = time (NULL) - Started) == 0L)
      Started = 1L;
   if (Log != NULL)
      Log->Write (" Throughput: %lu kB/sec, %lu mail/sec", (BytesProcessed / 1024L) / Started, MsgTossed / Started);

   if (Path != NULL)
      delete Path;
   if (SeenBy != NULL)
      delete SeenBy;

   if (Echo != NULL) {
      Echo->Save ();
      delete Echo;
   }
   if (Dupes != NULL)
      delete Dupes;
   if (Nodes != NULL)
      delete Nodes;
   if (Forward != NULL)
      delete Forward;
   if (Data != NULL)
      delete Data;
}

VOID TMailProcessor::ImportBad (VOID)
{
   int fd;
   CHAR *Line;
   ULONG Number;
   class TMsgBase *Packet;

   if (Log != NULL)
      Log->Write ("+Import From %s", Cfg->BadPath);

   Nodes = NULL;
   Data = new TMsgData (Cfg->SystemPath);
   Forward = NULL;
   Dupes = NULL;
   if ((Echo = new TEchotoss (Cfg->SystemPath)) != NULL)
      Echo->Load ();

   Msg = NULL;
   TossedArea = 0L;
   BadArea = FALSE;

   Packet = NULL;
   if (Cfg->BadStorage == ST_JAM)
      Packet = new JAM (Cfg->BadPath);
   else if (Cfg->BadStorage == ST_SQUISH)
      Packet = new SQUISH (Cfg->BadPath);
   else if (Cfg->BadStorage == ST_FIDO)
      Packet = new FIDOSDM (Cfg->BadPath);
   else if (Cfg->BadStorage == ST_ADEPT)
      Packet = new ADEPT (Cfg->BadPath);
   else if (Cfg->BadStorage == ST_HUDSON)
      Packet = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->BadBoard);

   LastTag[0] = '\0';

   if (Output != NULL)
      Output->Clear ();

   Number = Packet->Lowest ();

   do {
      if (Packet->Read (Number, MAX_LINE_LENGTH) == TRUE) {
         // Vengono resettati i flags che non devono essere mantenuti durante il
         // passaggio da un nodo all'altro
         Packet->Sent = Packet->Received = FALSE;
         Packet->Crash = Packet->Direct = Packet->Hold = FALSE;

         if ((Line = (PSZ)Packet->Text.First ()) != NULL) {
            if (!strncmp (Line, "AREA:", 5)) {
               ImportEchoMail (&Line[5], Packet);
               TossedArea++;
               Packet->Delete (Number);
            }
         }

         MsgTossed++;
      }
   } while (Packet->Next (Number) == TRUE);

   if (Msg != NULL) {
      if (TossedArea != 0L) {
         if (Data != NULL && IsAreasBBS == FALSE) {
            Data->ActiveMsgs = Msg->Number ();
            Data->Update ();
         }
         if (Log != NULL)
            Log->Write (":   %-20.20s (Toss=%04d)", LastTag, TossedArea);
         if (Echo != NULL)
            Echo->Add (LastTag);
      }
      TossedArea = 0L;
      Msg->UnLock ();
      delete Msg;
   }
   Msg = NULL;

   if (Packet != NULL) {
      delete Packet;
      Packet = NULL;
   }

   if (MsgTossed > 0L) {
      sprintf (Temp, "%sarealist.rsn", Cfg->SystemPath);
      if ((fd = open (Temp, O_BINARY|O_WRONLY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1)
         close (fd);
   }

   if (Echo != NULL) {
      Echo->Save ();
      delete Echo;
   }
   if (Data != NULL)
      delete Data;
}

ULONG TMailProcessor::ImportEchoMail (PSZ EchoTag, class TMsgBase *InBase)
{
   USHORT BadMessage = FALSE, DupeMessage = FALSE;
   CHAR *Text, *Base;
   ULONG RetVal = 0L;
   class TMsgBase *Extra = NULL;
   class TCollection *MsgText;

   Base = "";
   if (InBase == NULL)
      InBase = Packet;
   MsgText = &InBase->Text;

   if (stricmp (EchoTag, LastTag)) {
      if (TossedArea != 0L) {
         if (Log != NULL)
            Log->Write (":   %-20.20s (Toss=%04d)", LastTag, TossedArea);
         if (Echo != NULL)
            Echo->Add (LastTag);
      }

      TossedArea = 0L;
      strcpy (LastTag, strupr (EchoTag));
      BadArea = FALSE;

      if (OpenArea (LastTag) == FALSE) {
         BadArea = TRUE;
         BadMessage = TRUE;
      }
   }

   if (BadArea == TRUE) {
      Extra = NULL;
      Bad++;
      Base = "(Bad area)";
      BadMessage = TRUE;
   }

   if (BadMessage == FALSE && Forward != NULL) {
      if (Forward->Check (InBase->FromAddress) == FALSE) {
         if (Cfg->Secure == TRUE) {
            BadMessage = TRUE;
            Extra = Msg;
            Bad++;
            Base = "(Bad origin)";

            if (Cfg->BadStorage == ST_JAM)
               Msg = new JAM (Cfg->BadPath);
            else if (Cfg->BadStorage == ST_SQUISH)
               Msg = new SQUISH (Cfg->BadPath);
            else if (Cfg->BadStorage == ST_FIDO)
               Msg = new FIDOSDM (Cfg->BadPath);
            else if (Cfg->BadStorage == ST_ADEPT)
               Msg = new ADEPT (Cfg->BadPath);
            else if (Cfg->BadStorage == ST_HUDSON)
               Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->BadBoard);
         }
      }
      else {
         if (Forward->ReceiveOnly == TRUE) {
            BadMessage = TRUE;
            Extra = Msg;
            Bad++;
            Base = "(Receive only)";

            if (Cfg->BadStorage == ST_JAM)
               Msg = new JAM (Cfg->BadPath);
            else if (Cfg->BadStorage == ST_SQUISH)
               Msg = new SQUISH (Cfg->BadPath);
            else if (Cfg->BadStorage == ST_FIDO)
               Msg = new FIDOSDM (Cfg->BadPath);
            else if (Cfg->BadStorage == ST_ADEPT)
               Msg = new ADEPT (Cfg->BadPath);
            else if (Cfg->BadStorage == ST_HUDSON)
               Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->BadBoard);
         }
      }
   }

   if (BadMessage == FALSE && Dupes != NULL) {
      if (Dupes->Check (Data->EchoTag, InBase) == TRUE) {
         DupeMessage = TRUE;
         Extra = Msg;
         Duplicate++;
         Base = "(Duplicate)";

         if (Cfg->DupeStorage == ST_JAM)
            Msg = new JAM (Cfg->DupePath);
         else if (Cfg->DupeStorage == ST_SQUISH)
            Msg = new SQUISH (Cfg->DupePath);
         else if (Cfg->DupeStorage == ST_FIDO)
            Msg = new FIDOSDM (Cfg->DupePath);
         else if (Cfg->DupeStorage == ST_ADEPT)
            Msg = new ADEPT (Cfg->DupePath);
         else if (Cfg->BadStorage == ST_HUDSON)
            Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->BadBoard);
      }
      else
         Dupes->Add (Data->EchoTag, InBase);
   }

   if (Msg != NULL) {
      if (BadMessage == FALSE && DupeMessage == FALSE) {
         InBase->Text.First ();
         InBase->Text.Remove ();
      }

      Msg->New ();

      if (SeenBy != NULL && Path != NULL) {
         SeenBy->Clear ();
         Path->Clear ();

         Path->Sort = FALSE;
         SeenBy->Sort = FALSE;

         if ((Text = (PSZ)MsgText->Last ()) != NULL)
            do {
               if (strncmp (Text, "SEEN-BY: ", 9) && strncmp (Text, "\001PATH: ", 7) && *Text != '\0')
                  break;
               if (!strncmp (Text, "SEEN-BY: ", 9) && SeenBy != NULL) {
                  SeenBy->AddString (&Text[9]);
                  MsgText->Remove ();
                  Text = (PSZ)MsgText->Value ();
               }
               else if (!strncmp (Text, "\001PATH: ", 7) && Path != NULL) {
                  Path->AddString (&Text[7]);
                  MsgText->Remove ();
                  Text = (PSZ)MsgText->Value ();
               }
               else
                  Text = (PSZ)MsgText->Previous ();
            } while (Text != NULL);

         SeenBy->Sort = TRUE;
         SeenBy->AddString (InBase->FromAddress);
         Path->AddString (InBase->FromAddress);

         if (Data->Address[0] != '\0') {
            SeenBy->AddString (Data->Address);
            Path->AddString (Data->Address);
         }
         else if (Cfg->MailAddress.First () == TRUE) {
            SeenBy->AddString (Cfg->MailAddress.String);
            Path->AddString (Cfg->MailAddress.String);
         }

         if (SeenBy->First () == TRUE) {
            strcpy (Temp, "SEEN-BY:");
            do {
               if (strlen (Temp) + strlen (SeenBy->ShortAddress) + 1 > 70) {
                  MsgText->Add (Temp);
                  strcpy (Temp, "SEEN-BY:");
                  strcpy (SeenBy->ShortAddress, SeenBy->Address);
               }
               strcat (Temp, " ");
               strcat (Temp, SeenBy->ShortAddress);
            } while (SeenBy->Next () == TRUE);
            if (strlen (Temp) > 9)
               MsgText->Add (Temp);
         }

         if (Path->First () == TRUE) {
            strcpy (Temp, "\001PATH:");
            do {
               if (strlen (Temp) + strlen (Path->ShortAddress) + 1 > 70) {
                  MsgText->Add (Temp);
                  strcpy (Temp, "\001PATH:");
                  strcpy (Path->ShortAddress, Path->Address);
               }
               strcat (Temp, " ");
               strcat (Temp, Path->ShortAddress);
            } while (Path->Next () == TRUE);
            if (strlen (Temp) > 7)
               MsgText->Add (Temp);
         }
      }

      Msg->Add (InBase);
      RetVal = Msg->Highest ();

#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif

      if (Data->Storage == ST_PASSTHROUGH && BadMessage == FALSE && DupeMessage == FALSE)
         ExportEchoMail (InBase->Current + 1L, Data->EchoTag);
   }

   if (Output != NULL && Data->Storage != ST_PASSTHROUGH) {
      if (BadMessage == FALSE && DupeMessage == FALSE) {
         Base = "???";
         if (Data->Storage == ST_SQUISH)
            Base = "Squish<tm>";
         else if (Data->Storage == ST_JAM)
            Base = "JAM";
         else if (Data->Storage == ST_FIDO)
            Base = "Fido *.MSG";
         else if (Data->Storage == ST_ADEPT)
            Base = "AdeptXBBS";
         else if (Data->Storage == ST_USENET)
            Base = "Newsgroup";
         else if (Data->Storage == ST_HUDSON)
            Base = "Hudson";
      }
      sprintf (Display, "%6lu %-32.32s %-12.12s %6lu", InBase->Current + 1L, LastTag, Base, Msg->UidToMsgn (RetVal));
      Output->Add (Display);
   }

   if (BadMessage == TRUE || DupeMessage == TRUE) {
      if (Extra != NULL) {
         if (Msg != NULL)
            delete Msg;
         Msg = Extra;
      }
   }

   return (RetVal);
}

USHORT TMailProcessor::IsArcmail (VOID)
{
   DIR *dir;
   USHORT RetVal = FALSE, i;
   CHAR Filename[128];
   struct dirent *ent;

   strcpy (Filename, Inbound);
   if (Filename[strlen (Filename) - 1] == '\\' || Filename[strlen (Filename) - 1] == '/')
      Filename[strlen (Filename) - 1] = '\0';

   if ((dir = opendir (Filename)) != NULL) {
      while ((ent = readdir (dir)) != NULL && RetVal == FALSE) {
#if !defined(__LINUX__)
         strlwr (ent->d_name);
#endif
         i = 0;
         while (CheckExts[i] != NULL && RetVal == FALSE) {
            if (strstr (ent->d_name, CheckExts[i]) != NULL)
               RetVal = TRUE;
            i++;
         }
      }
      closedir (dir);
   }

   return (RetVal);
}

USHORT TMailProcessor::CheckEchoList (PSZ pszFile, PSZ pszEchoTag)
{
   #define MAX_LINECHAR 2048
   FILE *fp;
   USHORT RetVal = FALSE;
   CHAR *lpTemp = (CHAR *)malloc (MAX_LINECHAR + 1);
   CHAR *lpTag;

   if (*pszFile == '@')
      pszFile++;
   fp = fopen (pszFile, "rt");

   if (fp != NULL && lpTemp != NULL) {
      while (fgets (lpTemp, MAX_LINECHAR, fp) != NULL) {
         lpTemp[strlen (lpTemp) - 1] = '\0';
         if (lpTemp[0] == ';' || lpTemp[0] == '\0')
            continue;
         if ((lpTag = strtok (lpTemp, " ")) != NULL) {
            if (!stricmp (lpTag, pszEchoTag)) {
               RetVal = TRUE;
               break;
            }
         }
      }
   }

   if (lpTemp != NULL)
      free (lpTemp);
   if (fp != NULL)
      fclose (fp);

   return (RetVal);
}

USHORT TMailProcessor::OpenArea (PSZ pszEchoTag)
{
   FILE *fp;
   USHORT Found = FALSE, Create;
   CHAR *p, *path, *tag, *links;

   if (Msg != NULL) {
      if (Data != NULL && IsAreasBBS == FALSE) {
         Data->ActiveMsgs = Msg->Number ();
         Data->Update ();
      }
      if (Dupes != NULL)
         Dupes->Save ();
      Msg->UnLock ();
      delete Msg;
   }

   Msg = NULL;
   strcpy (LastTag, strupr (pszEchoTag));
   IsAreasBBS = FALSE;

   if (Data != NULL) {
      Found = Data->ReadEcho (pszEchoTag);
      if (Found == TRUE) {
         if (Forward != NULL)
            Forward->Load (pszEchoTag);

         if (Data->Storage == ST_JAM)
            Msg = new JAM (Data->Path);
         else if (Data->Storage == ST_SQUISH)
            Msg = new SQUISH (Data->Path);
         else if (Data->Storage == ST_FIDO)
            Msg = new FIDOSDM (Data->Path);
         else if (Data->Storage == ST_ADEPT)
            Msg = new ADEPT (Data->Path);
         else if (Data->Storage == ST_HUDSON)
            Msg = new HUDSON (Data->Path, (UCHAR)Data->Board);
#if !defined(__POINT__)
         else if (Data->Storage == ST_USENET)
            Msg = new USENET (Cfg->NewsServer, Data->NewsGroup);
         else if (Data->Storage == ST_PASSTHROUGH)
            Msg = new PASSTHR;
#endif
      }
   }

   if (Found == FALSE && Cfg->UseAreasBBS == TRUE && Cfg->AreasBBS[0] != '\0') {
      if ((fp = fopen (Cfg->AreasBBS, "rt")) != NULL) {
         fgets (Temp, sizeof (Temp) - 1, fp);
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            if (Temp[0] == ';')
               continue;
            Temp[strlen (Temp) - 1] = '\0';
            if ((path = strtok (Temp, " ")) != NULL) {
               tag = strtok (NULL, " ");
               if ((links = strtok (NULL, "")) != NULL) {
                  while (*links == ' ')
                     links++;
               }
               if (!stricmp (pszEchoTag, tag)) {
                  Found = TRUE;
                  IsAreasBBS = TRUE;
                  if (*path == '$')
                     Msg = new SQUISH (Cfg->AdjustPath (++path));
                  else if (*path == '!')
                     Msg = new JAM (Cfg->AdjustPath (++path));
                  else if (*path == '=')
                     Msg = new ADEPT (Cfg->AdjustPath (++path));
                  else if (isdigit (*path))
                     Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)atoi (path));
                  else
                     Msg = new FIDOSDM (Cfg->FixPath (path));
                  if (Forward != NULL) {
                     Forward->Clear ();
                     Forward->AddString (links);
                  }
               }
            }
         }
         fclose (fp);
      }
   }

   if (Found == FALSE && Data != NULL) {
      Create = FALSE;

      if (Nodes != NULL) {
         if (Nodes->Read (Packet->FromAddress, FALSE) == TRUE) {
            if (Nodes->CreateNewAreas == TRUE) {
               Create = TRUE;
               if ((p = strtok (Nodes->NewAreasFilter, " ")) != NULL) {
                  Create = FALSE;
                  do {
                     if (*p == '@') {
                        if (CheckEchoList (p, pszEchoTag) == FALSE)
                           Create = FALSE;
                     }
                     else if (strstr (pszEchoTag, strupr (p)) != NULL) {
                        Create = TRUE;
                        break;
                     }
                  } while ((p = strtok (NULL, " ")) != NULL);
               }
            }
         }

         if (Create == FALSE && Log != NULL)
            Log->Write ("!Node %s can't create new area %s", Packet->FromAddress, pszEchoTag);
      }

      if (Create == TRUE) {
         strcpy (Temp, pszEchoTag);
         Temp[sizeof (Data->Key) - 1] = '\0';
         while (Data->Read (Temp) == TRUE) {
            if (isdigit (Temp[strlen (Temp) - 1]))
               Temp[strlen (Temp) - 1]++;
            else
               Temp[strlen (Temp) - 1] = '0';
         }

         Data->New ();
         strcpy (Data->Key, Temp);
         sprintf (Data->Display, "New area %s", pszEchoTag);
         Data->Storage = Cfg->NewAreasStorage;
         strcpy (Temp, pszEchoTag);
         Temp[8] = '\0';
         while ((p = strchr (Temp, '.')) != NULL)
            *p = '_';
         if (Temp[strlen (Temp) - 1] == '_')
            Temp[strlen (Temp) - 1] = '\0';
         sprintf (Data->Path, "%s%s", Cfg->NewAreasPath, Temp);
         strlwr (Data->Path);
         if (Data->Storage == ST_FIDO)
#if defined(__LINUX__)
            mkdir (Data->Path, 0666);
#else
            mkdir (Data->Path);
#endif
         Data->Level = Cfg->NewAreasLevel;
         Data->AccessFlags = Cfg->NewAreasFlags;
         Data->DenyFlags = Cfg->NewAreasDenyFlags;
         Data->WriteLevel = Cfg->NewAreasWriteLevel;
         Data->WriteFlags = Cfg->NewAreasWriteFlags;
         Data->DenyWriteFlags = Cfg->NewAreasDenyWriteFlags;
         strcpy (Data->EchoTag, pszEchoTag);
         Data->EchoMail = TRUE;
         Data->DaysOld = 14;
         Data->MaxMessages = 200;
         Data->Add ();
         Data->ReadEcho (pszEchoTag);

         Found = TRUE;
         if (Log != NULL)
            Log->Write ("*Created area [%s] %s from %s", Data->Key, Data->EchoTag, Packet->FromAddress);

         if (Forward != NULL) {
            Forward->Load (pszEchoTag);
            Forward->AddString (Packet->FromAddress);

            if (Nodes != NULL) {
               strcpy (Temp, Packet->FromAddress);
               if (Nodes->First () == TRUE)
                  do {
                     if (Nodes->LinkNewEcho == TRUE && stricmp (Nodes->Address, Packet->FromAddress)) {
                        Forward->AddString (Nodes->Address);
                        if (Log != NULL)
                           Log->Write ("-Area %s auto-linked to %s", Data->EchoTag, Nodes->Address);
                     }
                  } while (Nodes->Next () == TRUE);
            }

            Forward->Save ();
         }

         if (Data->Storage == ST_JAM)
            Msg = new JAM (Data->Path);
         else if (Data->Storage == ST_SQUISH)
            Msg = new SQUISH (Data->Path);
         else if (Data->Storage == ST_FIDO)
            Msg = new FIDOSDM (Data->Path);
         else if (Data->Storage == ST_ADEPT)
            Msg = new ADEPT (Data->Path);
         else if (Data->Storage == ST_HUDSON)
            Msg = new HUDSON (Data->Path, (UCHAR)Data->Board);
#if !defined(__POINT__)
         else if (Data->Storage == ST_USENET)
            Msg = new USENET (Cfg->NewsServer, Data->NewsGroup);
         else if (Data->Storage == ST_PASSTHROUGH)
            Msg = new PASSTHR;
#endif
      }
   }

   if (Found == FALSE) {
      if (Forward != NULL)
         Forward->Load ("");

      if (Cfg->BadStorage == ST_JAM)
         Msg = new JAM (Cfg->BadPath);
      else if (Cfg->BadStorage == ST_SQUISH)
         Msg = new SQUISH (Cfg->BadPath);
      else if (Cfg->BadStorage == ST_FIDO)
         Msg = new FIDOSDM (Cfg->BadPath);
      else if (Cfg->BadStorage == ST_ADEPT)
         Msg = new ADEPT (Cfg->BadPath);
      else if (Cfg->BadStorage == ST_HUDSON)
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->BadBoard);
   }

   if (Msg != NULL)
      Msg->Lock (0L);
   if (Dupes != NULL)
      Dupes->Load (pszEchoTag);

   return (Found);
}

USHORT TMailProcessor::OpenNextPacket (VOID)
{
   DIR *dir;
   USHORT RetVal = FALSE, MaxBad;
   CHAR Filename[128], OpenFileName[128];
   PSZ p;
   ULONG PktDate;
   struct dirent *ent;
   struct stat statbuf;

   strcpy (Filename, Inbound);
   if (Filename[strlen (Filename) - 1] == '\\' || Filename[strlen (Filename) - 1] == '/')
      Filename[strlen (Filename) - 1] = '\0';

   if ((dir = opendir (Filename)) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
         strlwr (ent->d_name);
         if (strstr (ent->d_name, ".pk") != NULL) {
            sprintf (Filename, "%s%s", Inbound, ent->d_name);
            stat (Filename, &statbuf);
            strcpy (PktName, ent->d_name);
            PktDate = statbuf.st_mtime;
            RetVal = TRUE;
            break;
         }
      }

      if (RetVal == TRUE) {
         while ((ent = readdir (dir)) != NULL) {
            strlwr (ent->d_name);
            if (strstr (ent->d_name, ".pk") != NULL) {
               sprintf (Filename, "%s%s", Inbound, ent->d_name);
               stat (Filename, &statbuf);
               if (PktDate > statbuf.st_mtime) {
                  strcpy (PktName, ent->d_name);
                  PktDate = statbuf.st_mtime;
               }
            }
         }
      }

      closedir (dir);
   }

   if (RetVal == TRUE) {
      MaxBad = 0;

      strcpy (Filename, Inbound);
      if (Filename[strlen (Filename) - 1] == '\\' || Filename[strlen (Filename) - 1] == '/')
         Filename[strlen (Filename) - 1] = '\0';

      if ((dir = opendir (Filename)) != NULL) {
         while ((ent = readdir (dir)) != NULL) {
            strlwr (ent->d_name);
            if (strstr (ent->d_name, "bad_pkt.") != NULL) {
               if ((p = strchr (ent->d_name, '.')) != NULL) {
                  p++;
                  if (atoi (p) > MaxBad)
                     MaxBad = (USHORT)atoi (p);
               }
            }
         }
         closedir (dir);
      }

      sprintf (Filename, "%s%s", Inbound, PktName);
      sprintf (OpenFileName, "%sbad_pkt.%03d", Inbound, ++MaxBad);
      rename (Filename, OpenFileName);
      stat (OpenFileName, &statbuf);

      if (Packet != NULL)
         delete Packet;

      if ((Packet = new PACKET) != NULL) {
         if (Packet->Open (OpenFileName, FALSE) == FALSE) {
            delete Packet;
            Packet = NULL;
            RetVal = FALSE;
         }
         else
            BytesProcessed += statbuf.st_size;
      }
   }

   return (RetVal);
}

USHORT TMailProcessor::UnpackArcmail (VOID)
{
   DIR *dir;
   USHORT RetVal = FALSE, i, counter;
   CHAR Filename[128], BadArc[128];
   struct dirent *ent;
   class TPacker *Packer;

   strcpy (Filename, Inbound);
   if (Filename[strlen (Filename) - 1] == '\\' || Filename[strlen (Filename) - 1] == '/')
      Filename[strlen (Filename) - 1] = '\0';

   if ((dir = opendir (Filename)) != NULL) {
      while ((ent = readdir (dir)) != NULL && RetVal == FALSE) {
         i = 0;
         while (CheckExts[i] != NULL && RetVal == FALSE) {
#if !defined(__LINUX__)
            strlwr (ent->d_name);
#endif
            if (strstr (ent->d_name, CheckExts[i]) != NULL && isdigit (ent->d_name[strlen (ent->d_name) - 1])) {
               sprintf (Filename, "%s%s", Inbound, ent->d_name);
               if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
                  if (Packer->CheckArc (Filename) == TRUE) {
                     Log->Write ("#Unpacking %s (%s)", Filename, Packer->Display);
                     if (Packer->DoUnpack (Filename, Inbound, "*.pk?") == FALSE) {
                        Log->Write ("!Return error: %s", Packer->Error);
                        counter = 1;
                        do {
                           sprintf (BadArc, "%sbad_arc.%03d", Inbound, counter++);
                        } while (rename (Filename, BadArc) == -1);
                        Log->Write (":Renamed %s in %s", Filename, BadArc);
                     }
                     else {
                        unlink (Filename);
                        RetVal = TRUE;
                     }
                  }
                  else {
                     Log->Write ("!Unrecognized method for %s", ent->d_name);
                     counter = 1;
                     do {
                        sprintf (BadArc, "%sbad_arc.%03d", Inbound, counter++);
                     } while (rename (Filename, BadArc) == -1);
                     Log->Write (":Renamed %s in %s", Filename, BadArc);
                  }
                  delete Packer;
               }
            }
            i++;
         }
      }
      closedir (dir);
   }

   return (RetVal);
}

