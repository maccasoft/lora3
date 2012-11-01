
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.5
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/06/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"
#include "lorawin.h"

enum {
   ALREADY_LINKED = 2,
   NOT_FOUND
};

TAreaManager::TAreaManager ()
{
   Log = NULL;
   EchoLink = NULL;
}

TAreaManager::~TAreaManager ()
{
}

VOID TAreaManager::MsgFooter ()
{
   CHAR Temp[96];

   Text.Add ("");
   sprintf (Temp, "--- %s v%s", NAME, VERSION);
   Text.Add (Temp);
}

VOID TAreaManager::MsgHeader ()
{
   CHAR Temp[96];
   class TAddress From, To;

   From.Parse (Msg->FromAddress);
   To.Parse (Msg->ToAddress);

   if (From.Zone != To.Zone) {
      sprintf (Temp, "\001INTL %u:%u/%u %u:%u/%u", To.Zone, To.Net, To.Node, From.Zone, From.Net, From.Node);
      Text.Add (Temp);
   }
   if (From.Point != 0) {
      sprintf (Temp, "\001FMPT %u", From.Point);
      Text.Add (Temp);
   }
   if (To.Point != 0) {
      sprintf (Temp, "\001TOPT %u", To.Point);
      Text.Add (Temp);
   }

   sprintf (Temp, "\001MSGID: %s %08lx", Msg->ToAddress, time (NULL));
   Text.Add (Temp);
   sprintf (Temp, "\001PID: %s", NAME);
   Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
}

USHORT TAreaManager::SetPacker (PSZ Cmd)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128];
   class TPacker *Packer;
   class TNodes *Nodes;

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (Msg->FromAddress) == TRUE) {
         if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
            if (Packer->Read (Cmd) == TRUE) {
               strcpy (Nodes->Packer, Cmd);
               Nodes->Update ();
               sprintf (Temp, "Compressor set to %s (%s).", Packer->Display, strupr (Cmd));
               RetVal = TRUE;
            }
            delete Packer;
         }
      }
      delete Nodes;
   }

   if (RetVal == FALSE)
      sprintf (Temp, "The compressor %s was not found.", strupr (Cmd));
   Text.Add (Temp, (USHORT)(strlen (Temp) + 1));

   return (RetVal);
}

VOID TAreaManager::ImportAreasBBS ()
{
   #define MAX_LINECHAR 2048
   int i, counter, existing;
   FILE *fp = fopen (Cfg->AreasBBS, "rt");
   CHAR *lpTemp = (CHAR *)malloc (MAX_LINECHAR + 1);
   CHAR *lpPath, *lpTag, *lpLink, Name[16];
   class TMsgData *MsgData = new TMsgData (Cfg->SystemPath);
   class TEchoLink *EchoLink = new TEchoLink (Cfg->SystemPath);

   counter = 0;
   existing = 0;
   if (Log != NULL)
      Log->Write ("+Import areas from %s", Cfg->AreasBBS);

   if (fp != NULL && lpTemp != NULL && MsgData != NULL && EchoLink != NULL) {
      fgets (lpTemp, MAX_LINECHAR, fp);
      while (fgets (lpTemp, MAX_LINECHAR, fp) != NULL) {
         lpTemp[strlen (lpTemp) - 1] = '\0';
         if (lpTemp[0] == ';' || lpTemp[0] == '\0')
            continue;
         if ((lpPath = strtok (lpTemp, " ")) != NULL) {
            if ((lpTag = strtok (NULL, " ")) != NULL) {
               if (MsgData->ReadEcho (lpTag) == FALSE) {
                  i = 1;
                  do {
                     sprintf (Name, "newarea%d", i++);
                  } while (MsgData->Read (Name) == TRUE);

                  MsgData->New ();
                  strcpy (MsgData->Key, Name);
                  sprintf (MsgData->Display, "Imported area %s", lpTag);
                  strcpy (MsgData->EchoTag, lpTag);
                  if (lpPath[0] == '$') {
                     MsgData->Storage = ST_SQUISH;
                     lpPath++;
                  }
                  else if (lpPath[0] == '!') {
                     MsgData->Storage = ST_JAM;
                     lpPath++;
                  }
                  else if (lpPath[0] == '=') {
                     MsgData->Storage = ST_SQUISH;
                     lpPath++;
                  }
                  else if (isdigit (lpPath[0]))
                     MsgData->Storage = ST_HUDSON;
                  else
                     MsgData->Storage = ST_FIDO;
                  MsgData->EchoMail = TRUE;
                  MsgData->Add ();
               }
               else
                  existing++;

               EchoLink->Load (lpTag);
               EchoLink->Clear ();
               if ((lpLink = strtok (NULL, "")) != NULL)
                  EchoLink->AddString (lpLink);
               EchoLink->Save ();

               counter++;
            }
         }
      }
   }

   if (Log != NULL)
      Log->Write (":%d area(s) readed, %d new", counter, counter - existing);

   if (EchoLink != NULL)
      delete EchoLink;
   if (MsgData != NULL)
      delete MsgData;
   if (lpTemp != NULL)
      free (lpTemp);
   if (fp != NULL)
      fclose (fp);
}

VOID TAreaManager::UpdateAreasBBS ()
{
   FILE *fp;
   int counter = 0;
   class TMsgData *MsgData;
   class TEchoLink *EchoLink;

   if (Log != NULL)
      Log->Write ("+Updating %s", Cfg->AreasBBS);

   if ((fp = fopen (Cfg->AreasBBS, "wt")) != NULL) {
      fprintf (fp, "%s ! %s\n;\n", Cfg->SystemName, Cfg->SysopName);
      if ((EchoLink = new TEchoLink (Cfg->SystemPath)) != NULL) {
         if ((MsgData = new TMsgData (Cfg->SystemPath)) != NULL) {
            if (MsgData->First () == TRUE)
               do {
                  if (MsgData->EchoMail == TRUE && MsgData->EchoTag[0] != '\0') {
                     if (MsgData->Storage == ST_SQUISH)
                        fprintf (fp, "$%-30s", MsgData->Path);
                     else if (MsgData->Storage == ST_JAM)
                        fprintf (fp, "!%-30s", MsgData->Path);
                     else if (MsgData->Storage == ST_ADEPT)
                        fprintf (fp, "=%-30s", MsgData->Path);
                     else if (MsgData->Storage == ST_FIDO)
                        fprintf (fp, "%-30s ", MsgData->Path);
                     else if (MsgData->Storage == ST_HUDSON)
                        fprintf (fp, "%-30d ", MsgData->Board);
                     EchoLink->Load (MsgData->EchoTag);
                     fprintf (fp, " %-30s", MsgData->EchoTag);
                     if (EchoLink->First () == TRUE)
                        do {
                           fprintf (fp, " %s", EchoLink->Address);
                        } while (EchoLink->Next () == TRUE);
                     fprintf (fp, "\n");
                     counter++;
                  }
               } while (MsgData->Next () == TRUE);
            delete MsgData;
         }
         delete EchoLink;
      }
      fprintf (fp, ";\n");
      fclose (fp);
   }

   if (Log != NULL)
      Log->Write (":%d area(s) exported", counter);
}

USHORT TAreaManager::Passive (PSZ address, USHORT flag)
{
   USHORT RetVal = FALSE;
   CHAR Temp[96];

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (Data->EchoMail == TRUE && Data->EchoTag[0] != '\0') {
               EchoLink->Load (Data->EchoTag);
               if (EchoLink->Check (address) == TRUE) {
                  if (EchoLink->Passive != flag) {
                     EchoLink->Passive = (UCHAR)flag;
                     EchoLink->Update ();
                     EchoLink->Save ();
                     sprintf (Temp, "Area %s is now %s.", strupr (Data->EchoTag), (flag == TRUE) ? "passive" : "active");
                  }
                  else
                     sprintf (Temp, "Area %s already %s.", strupr (Data->EchoTag), (flag == TRUE) ? "passive" : "active");
                  Text.Add (Temp);
                  RetVal = TRUE;
               }
            }
         } while (Data->Next () == TRUE);
      delete Data;
   }

   return (RetVal);
}

///////////////////////////////////////////////////////////////////////////////
// Rimuove tutte le aree agganciate ad un nodo                               //
///////////////////////////////////////////////////////////////////////////////
USHORT TAreaManager::RemoveAll (PSZ address)
{
   USHORT RetVal = FALSE, DoDelete = FALSE;
   CHAR Temp[96];

   if (EchoLink == NULL) {
      EchoLink = new TEchoLink (Cfg->SystemPath);
      DoDelete = TRUE;
   }

   if (EchoLink != NULL) {
      if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
         if (Data->First () == TRUE)
            do {
               if (Data->EchoMail == TRUE && Data->EchoTag[0] != '\0') {
                  EchoLink->Load (Data->EchoTag);
                  if (EchoLink->Check (address) == TRUE) {
                     EchoLink->Delete ();
                     EchoLink->Save ();
                     sprintf (Temp, "Area %s has been removed.", strupr (Data->EchoTag));
                     Text.Add (Temp);
                     RetVal = TRUE;
                  }
               }
            } while (Data->Next () == TRUE);
         delete Data;
      }
   }

   if (EchoLink != NULL && DoDelete == TRUE) {
      delete EchoLink;
      EchoLink = NULL;
   }

   return (RetVal);
}

///////////////////////////////////////////////////////////////////////////////
// Imposta la password di sessione                                           //
///////////////////////////////////////////////////////////////////////////////
USHORT TAreaManager::SetSessionPwd (PSZ address, PSZ pwd)
{
   USHORT RetVal = FALSE;

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (address) == TRUE) {
         strcpy (Nodes->SessionPwd, pwd);
         Nodes->Update ();
         RetVal = TRUE;
      }
      delete Nodes;
   }

   return (RetVal);
}

USHORT TAreaManager::SetPwd (PSZ address, PSZ pwd)
{
   USHORT RetVal = FALSE;

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (address) == TRUE) {
         strcpy (Nodes->AreaMgrPwd, pwd);
         Nodes->Update ();
         RetVal = TRUE;
      }
      delete Nodes;
   }

   return (RetVal);
}

USHORT TAreaManager::SetPacketPwd (PSZ address, PSZ pwd)
{
   USHORT RetVal = FALSE;

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (address) == TRUE) {
         if (pwd == NULL)
            Nodes->InPktPwd[0] = Nodes->OutPktPwd[0] = '\0';
         else {
            strcpy (Nodes->InPktPwd, pwd);
            strcpy (Nodes->OutPktPwd, pwd);
         }
         Nodes->Update ();
         RetVal = TRUE;
      }
      delete Nodes;
   }

   return (RetVal);
}

USHORT TAreaManager::SetInPacketPwd (PSZ address, PSZ pwd)
{
   USHORT RetVal = FALSE;

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (address) == TRUE) {
         if (pwd == NULL)
            Nodes->OutPktPwd[0] = '\0';
         else
            strcpy (Nodes->OutPktPwd, pwd);
         Nodes->Update ();
         RetVal = TRUE;
      }
      delete Nodes;
   }

   return (RetVal);
}

USHORT TAreaManager::SetOutPacketPwd (PSZ address, PSZ pwd)
{
   USHORT RetVal = FALSE;

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (address) == TRUE) {
         if (pwd == NULL)
            Nodes->InPktPwd[0] = '\0';
         else
            strcpy (Nodes->InPktPwd, pwd);
         Nodes->Update ();
         RetVal = TRUE;
      }
      delete Nodes;
   }

   return (RetVal);
}

///////////////////////////////////////////////////////////////////////////////
// Aggancia un'area ad un nuodo                                              //
///////////////////////////////////////////////////////////////////////////////
USHORT TAreaManager::AddArea (PSZ address, PSZ area)
{
   USHORT RetVal = FALSE, DoDelete = FALSE;
   class TNodes *Nodes;
   class TMsgData *Data;

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (Msg->FromAddress) == TRUE) {
         if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
            if (Data->ReadEcho (area) == TRUE) {
               if (Nodes->Level < Data->Level)
                  RetVal = NOT_FOUND;
               if ((Nodes->AccessFlags & Data->Flags) != Data->Flags)
                  RetVal = NOT_FOUND;
               if ((Nodes->DenyFlags & Data->DenyFlags) != Data->DenyFlags && Data->DenyFlags != 0L)
                  RetVal = NOT_FOUND;
               if (Log != NULL && RetVal == NOT_FOUND)
                  Log->Write ("!Area %s not accessible to node", area);
            }
            else
               RetVal = NOT_FOUND;
            delete Data;
         }
      }
      delete Nodes;
   }

   if (RetVal == NOT_FOUND)
      return (RetVal);

   ///////////////////////////////////////////////////////////////////////////////
   // Si assicura che la classe EchoLink sia accessibile                        //
   ///////////////////////////////////////////////////////////////////////////////
   if (EchoLink == NULL) {
      EchoLink = new TEchoLink (Cfg->SystemPath);
      DoDelete = TRUE;
   }

   if (EchoLink != NULL) {
      EchoLink->Load (area);
      if (EchoLink->First () == TRUE) {
         if (EchoLink->Check (address) == TRUE)
            RetVal = ALREADY_LINKED;
         else {
            EchoLink->AddString (address);
            EchoLink->Save ();
            RetVal = TRUE;
         }
      }
      else {
         ///////////////////////////////////////////////////////////////////////////////
         // Se non trova l'area negli echolink, ne verifica l'esistenza               //
         ///////////////////////////////////////////////////////////////////////////////
         RetVal = NOT_FOUND;
         if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
            if (Data->ReadEcho (area) == TRUE) {
               EchoLink->Load (area);
               EchoLink->AddString (address);
               EchoLink->Save ();
               RetVal = TRUE;
            }
            delete Data;
         }
      }
   }

   if (EchoLink != NULL && DoDelete == TRUE) {
      delete EchoLink;
      EchoLink = NULL;
   }

   return (RetVal);
}

USHORT TAreaManager::RemoveArea (PSZ address, PSZ area)
{
   USHORT RetVal = FALSE, DoDelete = FALSE;

   if (EchoLink == NULL) {
      EchoLink = new TEchoLink (Cfg->SystemPath);
      DoDelete = TRUE;
   }

   if (EchoLink != NULL) {
      EchoLink->Load (area);
      if (EchoLink->First () == TRUE) {
         if (EchoLink->Check (address) == TRUE) {
            EchoLink->Delete ();
            EchoLink->Save ();
            RetVal = TRUE;
         }
      }
   }

   if (EchoLink != NULL && DoDelete == TRUE) {
      delete EchoLink;
      EchoLink = NULL;
   }

   return (RetVal);
}

VOID TAreaManager::ProcessAreafix ()
{
   FILE *fp;
   USHORT i, Ok, DoList, DoRescan, DoLinked, DoUnlinked, CanMaint;
   USHORT DoListPacker, DoReport, DoHelp, Level, CanRename;
   ULONG AccessFlags, DenyFlags;
   CHAR Address[48], Temp[96], *Password, *p;
   ULONG Total;
//   class TPacker *Packer;

   CanMaint = FALSE;

   strcpy (Address, Msg->FromAddress);
   if (Log != NULL)
      Log->Write ("#Process AreaMgr requests for %s", Address);

   strcpy (Temp, Msg->Subject);
   if ((Password = strtok (Temp, " ")) == NULL)
      Password = "";

   Ok = FALSE;
   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (Address) == TRUE) {
         Level = Nodes->Level;
         AccessFlags = Nodes->AccessFlags;
         DenyFlags = Nodes->DenyFlags;
         CanMaint = Nodes->EchoMaint;
         CanRename = Nodes->ChangeEchoTag;
         if (!stricmp (Password, Nodes->AreaMgrPwd))
            Ok = TRUE;
      }
      delete Nodes;
   }

   if (Ok == TRUE) {
      DoList = DoRescan = DoLinked = DoUnlinked = FALSE;
      DoHelp = DoListPacker = FALSE;
      DoReport = FALSE;

      strcpy (Temp, Msg->Subject);
      if ((p = strtok (Temp, " ")) != NULL) {
         while ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "-l"))
               DoList = TRUE;
            else if (!stricmp (p, "-r"))
               DoRescan = TRUE;
         }
      }

      Text.Clear ();
      MsgHeader ();

      sprintf (Temp, "Following is a summary from %s of changes in Echomail topology:", Msg->ToAddress);
      Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      Text.Add ("");

      if ((EchoLink = new TEchoLink (Cfg->SystemPath)) != NULL) {
         if ((p = (CHAR *)Msg->Text.First ()) != NULL)
            do {
               if ((p = strtok (p, " ")) != NULL) {
                  if (*p == '%') {
                     if (!stricmp (p, "%rescan"))
                        DoRescan = TRUE;
                     else if (!stricmp (p, "%help"))
                        DoHelp = TRUE;
                     else if (!stricmp (p, "%list"))
                        DoList = TRUE;
                     else if (!stricmp (p, "%unlinked"))
                        DoUnlinked = TRUE;
                     else if (!stricmp (p, "%linked") || !stricmp (p, "%query"))
                        DoLinked = TRUE;
                     else if (!stricmp (p, "%from")) {
                        if (CanMaint == TRUE) {
                           strcpy (Address, &p[6]);
                           if (Log != NULL)
                              Log->Write ("#Remote maintenance for %s", Address);
                           sprintf (Temp, "Remote maintenance for %s", Address);
                        }
                        else
                           sprintf (Temp, "Remote maintenance not allowed");
                        Text.Add (Temp);
                     }
                     else if (!stricmp (p, "%packer")) {
                        if ((p = strtok (NULL, " ")) != NULL)
                           SetPacker (p);
                        else
                           DoListPacker = TRUE;
                     }
                     else if (!stricmp (p, "%passive")) {
                        if (Passive (Address, TRUE) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%active")) {
                        if (Passive (Address, FALSE) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%-all")) {
                        if (RemoveAll (Address) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%pwd")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           if (SetPwd (Address, p) == TRUE)
                              DoReport = TRUE;
                        }
                     }
                     else if (!stricmp (p, "%sessionpwd")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           if (SetSessionPwd (Address, p) == TRUE)
                              DoReport = TRUE;
                        }
                     }
                     else if (!stricmp (p, "%pktpwd")) {
                        p = strtok (NULL, " ");
                        if (SetPacketPwd (Address, p) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%inpktpwd")) {
                        p = strtok (NULL, " ");
                        if (SetInPacketPwd (Address, p) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%outpktpwd")) {
                        p = strtok (NULL, " ");
                        if (SetOutPacketPwd (Address, p) == TRUE)
                           DoReport = TRUE;
                     }
                  }
                  else if (*p == '#') {
                     if (CanRename == TRUE) {
                        if ((p = strtok (NULL, " :")) != NULL) {
                           if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
                              if (Data->ReadEcho (p) == TRUE) {
                                 if ((p = strtok (NULL, " :")) != NULL) {
                                    if (Log != NULL)
                                       Log->Write ("#Area %s changed to %s", Data->EchoTag, p);
                                    sprintf (Temp, "Area %s changed to %s", Data->EchoTag, p);
                                    Text.Add (Temp);
                                    // Cambia il tag nel file dei forward
                                    EchoLink->Change (Data->EchoTag, p);
                                    // Cambia il tag nella configurazione dell'area
                                    strcpy (Data->EchoTag, p);
                                    Data->Update ();
                                 }
                              }
                              delete Data;
                           }
                        }
                     }
                     else
                        Text.Add ("Remote maintenance not allowed");
                  }
                  else if (*p == '-' && strcmp (p, "---")) {
                     p++;
                     if (RemoveArea (Address, p) == FALSE)
                        sprintf (Temp, "Area %s never linked.", strupr (p));
                     else
                        sprintf (Temp, "Area %s has been removed.", strupr (p));
                     Text.Add (Temp);
                     DoReport = TRUE;
                  }
                  else if (*p != '\0' && strcmp (p, "---") && *p != '\001') {
                     if (*p == '+')
                        p++;
                     if ((i = AddArea (Address, p)) == TRUE)
                        sprintf (Temp, "Area %s has been added.\n", strupr (p));
                     else if (i == NOT_FOUND)
                        sprintf (Temp, "Area %s not found.\n", strupr (p));
                     else if (i == ALREADY_LINKED)
                        sprintf (Temp, "Area %s already linked.\n", strupr (p));
                     Text.Add (Temp);
                     DoReport = TRUE;
                  }
               }
            } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);
         delete EchoLink;
      }

      strcpy (Temp, Msg->FromAddress);
      strcpy (Msg->FromAddress, Msg->ToAddress);
      strcpy (Msg->ToAddress, Temp);
      strcpy (Msg->To, Msg->From);
      strcpy (Msg->From, "Echo Manager");

      if (DoReport == TRUE) {
         MsgFooter ();
         strcpy (Msg->Subject, "EchoMail changes report");
         Msg->Text.Clear ();
         Msg->Local = TRUE;
         Msg->Crash = Msg->Direct = Msg->Hold = FALSE;
         Msg->Sent = FALSE;
         Msg->Add (Text);

         // Invia le notifiche dei cambiamenti ai nodi indicati
         if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
            strcpy (Temp, Msg->ToAddress);
            if (Nodes->First () == TRUE)
               do {
                  if (Nodes->NotifyAreafix == TRUE && stricmp (Temp, Nodes->Address)) {
                     strcpy (Msg->To, Nodes->SysopName);
                     if (Msg->To[0] == '\0')
                        strcpy (Msg->To, "Sysop");
                     strcpy (Msg->ToAddress, Nodes->Address);
                     Msg->Add (Text);
                     if (Log != NULL)
                        Log->Write ("-Sent notification to %s (%s)", Msg->To, Msg->ToAddress);
                  }
               } while (Nodes->Next () == TRUE);
            delete Nodes;
         }

         if (Cfg->UpdateAreasBBS == TRUE)
            UpdateAreasBBS ();
      }

      if (DoList == TRUE) {
         Total = 0L;
         strcpy (Msg->Subject, "List of available echomail areas");
         Text.Clear ();
         MsgHeader ();

         sprintf (Temp, "Area(s) available to %s:", Msg->ToAddress);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         Text.Add ("");
         if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
            if (Data->First () == TRUE)
               do {
                  if (Level < Data->Level)
                     continue;
                  if ((AccessFlags & Data->Flags) != Data->Flags)
                     continue;
                  if ((DenyFlags & Data->DenyFlags) != Data->DenyFlags && Data->DenyFlags != 0L)
                     continue;
                  if (Data->EchoMail == TRUE && Data->EchoTag[0] != '\0') {
                     sprintf (Temp, "%-30.30s %.48s", Data->EchoTag, Data->Display);
                     Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                     Total++;
                  }
               } while (Data->Next () == TRUE);
            delete Data;
         }
         Text.Add ("");
         sprintf (Temp, "%lu available area(s).", Total);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));

         MsgFooter ();
         Msg->Add (Text);
      }

      if (DoLinked == TRUE) {
         strcpy (Msg->Subject, "List of linked echomail areas");
         Text.Clear ();
         MsgHeader ();

         sprintf (Temp, "%s is now linked to the following area(s):", Msg->ToAddress);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         Text.Add ("");
         Total = 0L;
         if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
            EchoLink = new TEchoLink (Cfg->SystemPath);
            if (Data->First () == TRUE)
               do {
                  if (Data->EchoMail == TRUE && Data->EchoTag[0] != '\0' && EchoLink != NULL) {
                     EchoLink->Load (Data->EchoTag);
                     if (EchoLink->Check (Msg->ToAddress) == TRUE) {
                        sprintf (Temp, "%-30.30s %.48s", Data->EchoTag, Data->Display);
                        Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                        Total++;
                     }
                  }
               } while (Data->Next () == TRUE);
            if (EchoLink != NULL)
               delete EchoLink;
            delete Data;
         }
         Text.Add ("");
         sprintf (Temp, "%lu linked area(s).", Total);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));

         MsgFooter ();
         Msg->Add (Text);
      }

      if (DoUnlinked == TRUE) {
         strcpy (Msg->Subject, "List of unlinked echomail areas");
         Text.Clear ();
         MsgHeader ();

         sprintf (Temp, "Area(s) not linked to %s:", Msg->ToAddress);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         Text.Add ("");
         Total = 0L;
         if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
            EchoLink = new TEchoLink (Cfg->SystemPath);
            if (Data->First () == TRUE)
               do {
                  if (Level < Data->Level)
                     continue;
                  if ((AccessFlags & Data->Flags) != Data->Flags)
                     continue;
                  if ((DenyFlags & Data->DenyFlags) != Data->DenyFlags && Data->DenyFlags != 0L)
                     continue;
                  if (Data->EchoMail == TRUE && Data->EchoTag[0] != '\0' && EchoLink != NULL) {
                     EchoLink->Load (Data->EchoTag);
                     if (EchoLink->Check (Msg->ToAddress) == FALSE) {
                        sprintf (Temp, "%-30.30s %.48s", Data->EchoTag, Data->Display);
                        Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                        Total++;
                     }
                  }
               } while (Data->Next () == TRUE);
            if (EchoLink != NULL)
               delete EchoLink;
            delete Data;
         }
         Text.Add ("");
         sprintf (Temp, "%lu unlinked area(s).", Total);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         MsgFooter ();

         Msg->Add (Text);
      }

      if (DoHelp == TRUE) {
         strcpy (Msg->Subject, "Areafix help");
         Text.Clear ();
         MsgHeader ();

         sprintf (Temp, "%safxhelp.txt", Cfg->SystemPath);
         if ((fp = fopen (Temp, "rt")) != NULL) {
            while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
               Temp[strlen (Temp) - 1] = '\0';
               Text.Add (Temp);
            }
            fclose (fp);
         }
         else {
            Text.Add ("");
            Text.Add ("No help file available. Contact the Sysop.");
            Text.Add ("");
         }

         MsgFooter ();
         Msg->Add (Text);
      }
   }
   else {
      Text.Clear ();
      MsgHeader ();

      if (Log != NULL)
         Log->Write ("!Node %s is not authorized for echomanager", Msg->FromAddress);

      Text.Add ("");
      sprintf (Temp, "Node %s isn't authorized to use areafix at %s", Msg->FromAddress, Msg->ToAddress);
      Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      Text.Add ("");

      strcpy (Temp, Msg->FromAddress);
      strcpy (Msg->FromAddress, Msg->ToAddress);
      strcpy (Msg->ToAddress, Temp);
      strcpy (Msg->To, Msg->From);
      strcpy (Msg->From, "Echo Manager");
      strcpy (Msg->Subject, "EchoMail changes report");
      Msg->Text.Clear ();
      Msg->Local = TRUE;
      Msg->Crash = Msg->Direct = Msg->Hold = FALSE;

      MsgFooter ();
      Msg->Add (Text);

      // Invia le notifiche dei cambiamenti ai nodi indicati
      if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
         strcpy (Temp, Msg->ToAddress);
         if (Nodes->First () == TRUE)
            do {
               if (Nodes->NotifyAreafix == TRUE && stricmp (Temp, Nodes->Address)) {
                  strcpy (Msg->To, Nodes->SysopName);
                  if (Msg->To[0] == '\0')
                     strcpy (Msg->To, "Sysop");
                  strcpy (Msg->ToAddress, Nodes->Address);
                  Msg->Add (Text);
                  if (Log != NULL)
                     Log->Write ("-Sent notification to %s (%s)", Msg->To, Msg->ToAddress);
               }
            } while (Nodes->Next () == TRUE);
         delete Nodes;
      }
   }
}

USHORT TAreaManager::FilePassive (PSZ address, USHORT flag)
{
   USHORT RetVal = FALSE;
   CHAR Temp[96];

   if ((FileData = new TFileData (Cfg->SystemPath)) != NULL) {
      if (FileData->First () == TRUE)
         do {
            if (FileData->EchoTag[0] != '\0') {
               FileEchoLink->Load (FileData->EchoTag);
               if (FileEchoLink->Check (address) == TRUE) {
                  if (FileEchoLink->Passive != flag) {
                     FileEchoLink->Passive = (UCHAR)flag;
                     FileEchoLink->Update ();
                     FileEchoLink->Save ();
                     sprintf (Temp, "Area %s is now %s.", strupr (FileData->EchoTag), (flag == TRUE) ? "passive" : "active");
                  }
                  else
                     sprintf (Temp, "Area %s already %s.", strupr (FileData->EchoTag), (flag == TRUE) ? "passive" : "active");
                  Text.Add (Temp);
                  RetVal = TRUE;
               }
            }
         } while (FileData->Next () == TRUE);
      delete FileData;
   }

   return (RetVal);
}

USHORT TAreaManager::FileRemoveAll (PSZ address)
{
   USHORT RetVal = FALSE;
   CHAR Temp[96];

   if ((FileData = new TFileData (Cfg->SystemPath)) != NULL) {
      if (FileData->First () == TRUE)
         do {
            if (FileData->EchoTag[0] != '\0') {
               FileEchoLink->Load (FileData->EchoTag);
               if (FileEchoLink->Check (address) == TRUE) {
                  FileEchoLink->Delete ();
                  FileEchoLink->Save ();
                  sprintf (Temp, "Area %s has been removed.", strupr (FileData->EchoTag));
                  Text.Add (Temp);
                  RetVal = TRUE;
               }
            }
         } while (FileData->Next () == TRUE);
      delete FileData;
   }

   return (RetVal);
}

VOID TAreaManager::ProcessRaid ()
{
   FILE *fp;
   USHORT Ok, DoList, DoLinked, DoUnlinked, Found;
   USHORT DoReport, DoHelp;
   CHAR Temp[96], *Password, *p;
   ULONG Total;

   if (Log != NULL)
      Log->Write ("#Process Raid requests for %s", Msg->FromAddress);

   strcpy (Temp, Msg->Subject);
   if ((Password = strtok (Temp, " ")) == NULL)
      Password = "";

   Ok = FALSE;
   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (Msg->FromAddress) == TRUE) {
         if (!stricmp (Password, Nodes->AreaMgrPwd))
            Ok = TRUE;
      }
      delete Nodes;
   }

   if (Ok == TRUE) {
      DoHelp = DoList = DoLinked = DoUnlinked = FALSE;
      DoReport = FALSE;

      strcpy (Temp, Msg->Subject);
      if ((p = strtok (Temp, " ")) != NULL) {
         while ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "-l"))
               DoList = TRUE;
         }
      }

      Text.Clear ();
      MsgHeader ();

      sprintf (Temp, "Following is a summary from %s of changes in Fileecho topology:", Msg->ToAddress);
      Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      Text.Add ("");

      if ((FileEchoLink = new TFilechoLink (Cfg->SystemPath)) != NULL) {
         if ((p = (CHAR *)Msg->Text.First ()) != NULL)
            do {
               if ((p = strtok (p, " ")) != NULL) {
                  if (*p == '%') {
                     if (!stricmp (p, "%list"))
                        DoList = TRUE;
                     else if (!stricmp (p, "%unlinked"))
                        DoUnlinked = TRUE;
                     else if (!stricmp (p, "%linked"))
                        DoLinked = TRUE;
                     else if (!stricmp (p, "%help"))
                        DoHelp = TRUE;
                     else if (!stricmp (p, "%passive")) {
                        if (FilePassive (Msg->FromAddress, TRUE) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%active")) {
                        if (FilePassive (Msg->FromAddress, FALSE) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%-all")) {
                        if (FileRemoveAll (Msg->FromAddress) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%pwd")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           if (SetPwd (Msg->FromAddress, p) == TRUE)
                              DoReport = TRUE;
                        }
                     }
                     else if (!stricmp (p, "%sessionpwd")) {
                        if ((p = strtok (NULL, " ")) != NULL) {
                           if (SetSessionPwd (Msg->FromAddress, p) == TRUE)
                              DoReport = TRUE;
                        }
                     }
                     else if (!stricmp (p, "%pktpwd")) {
                        p = strtok (NULL, " ");
                        if (SetPacketPwd (Msg->FromAddress, p) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%inpktpwd")) {
                        p = strtok (NULL, " ");
                        if (SetInPacketPwd (Msg->FromAddress, p) == TRUE)
                           DoReport = TRUE;
                     }
                     else if (!stricmp (p, "%outpktpwd")) {
                        p = strtok (NULL, " ");
                        if (SetOutPacketPwd (Msg->FromAddress, p) == TRUE)
                           DoReport = TRUE;
                     }
                  }
                  else if (*p == '-' && strcmp (p, "---")) {
                     p++;
                     FileEchoLink->Load (p);
                     if (FileEchoLink->First () == TRUE) {
                        if (FileEchoLink->Check (Msg->FromAddress) == TRUE) {
                           FileEchoLink->Delete ();
                           FileEchoLink->Save ();
                           sprintf (Temp, "Area %s has been removed.", strupr (p));
                        }
                        else
                           sprintf (Temp, "Area %s never linked.", strupr (p));
                     }
                     else
                        sprintf (Temp, "Area %s never linked.", strupr (p));
                     Text.Add (Temp);
                     DoReport = TRUE;
                  }
                  else if (*p != '\0' && strcmp (p, "---") && *p != '\001') {
                     if (*p == '+')
                        p++;
                     FileEchoLink->Load (p);
                     if (FileEchoLink->First () == TRUE) {
                        if (FileEchoLink->Check (Msg->FromAddress) == TRUE)
                           sprintf (Temp, "Area %s already linked.\n", strupr (p));
                        else {
                           FileEchoLink->AddString (Msg->FromAddress);
                           FileEchoLink->Save ();
                           sprintf (Temp, "Area %s has been added.\n", strupr (p));
                        }
                     }
                     else {
                        Found = FALSE;
                        if ((FileData = new TFileData (Cfg->SystemPath)) != NULL) {
                           if (FileData->ReadEcho (p) == TRUE) {
                              FileEchoLink->Load (FileData->EchoTag);
                              FileEchoLink->AddString (Msg->FromAddress);
                              FileEchoLink->Save ();
                              sprintf (Temp, "Area %s has been added.\n", strupr (p));
                              Found = TRUE;
                           }
                           delete Data;
                        }
                        if (Found == FALSE)
                           sprintf (Temp, "Area %s not found.\n", strupr (p));
                     }
                     Text.Add (Temp);
                     DoReport = TRUE;
                  }
               }
            } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);
         delete FileEchoLink;
      }

      strcpy (Temp, Msg->FromAddress);
      strcpy (Msg->FromAddress, Msg->ToAddress);
      strcpy (Msg->ToAddress, Temp);
      strcpy (Msg->To, Msg->From);
      strcpy (Msg->From, "Tic Manager");

      if (DoReport == TRUE) {
         MsgFooter ();
         strcpy (Msg->Subject, "Tic changes report");
         Msg->Text.Clear ();
         Msg->Local = TRUE;
         Msg->Crash = Msg->Direct = Msg->Hold = FALSE;
         Msg->Sent = FALSE;
         Msg->Add (Text);
      }

      if (DoList == TRUE) {
         Total = 0L;
         strcpy (Msg->Subject, "List of available Tic areas");
         Text.Clear ();
         MsgHeader ();

         sprintf (Temp, "Area(s) available to %s:", Msg->ToAddress);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         Text.Add ("");
         if ((FileData = new TFileData (Cfg->SystemPath)) != NULL) {
            if (FileData->First () == TRUE)
               do {
                  if (FileData->EchoTag[0] != '\0') {
                     sprintf (Temp, "%-30.30s %.48s", FileData->EchoTag, FileData->Display);
                     Text.Add (Temp);
                     Total++;
                  }
               } while (FileData->Next () == TRUE);
            delete FileData;
         }
         Text.Add ("");
         sprintf (Temp, "%lu available area(s).", Total);
         Text.Add (Temp);

         MsgFooter ();
         Msg->Add (Text);
      }

      if (DoLinked == TRUE) {
         strcpy (Msg->Subject, "List of linked Tic areas");
         Text.Clear ();
         MsgHeader ();

         sprintf (Temp, "%s is now linked to the following area(s):", Msg->ToAddress);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         Text.Add ("");
         Total = 0L;
         if ((FileData = new TFileData (Cfg->SystemPath)) != NULL) {
            FileEchoLink = new TFilechoLink (Cfg->SystemPath);
            if (FileData->First () == TRUE)
               do {
                  if (FileData->EchoTag[0] != '\0' && FileEchoLink != NULL) {
                     FileEchoLink->Load (FileData->EchoTag);
                     if (FileEchoLink->Check (Msg->ToAddress) == TRUE) {
                        sprintf (Temp, "%-30.30s %.48s", FileData->EchoTag, FileData->Display);
                        Text.Add (Temp);
                        Total++;
                     }
                  }
               } while (FileData->Next () == TRUE);
            if (FileEchoLink != NULL)
               delete FileEchoLink;
            delete FileData;
         }
         Text.Add ("");
         sprintf (Temp, "%lu linked area(s).", Total);
         Text.Add (Temp);

         MsgFooter ();
         Msg->Add (Text);
      }

      if (DoUnlinked == TRUE) {
         strcpy (Msg->Subject, "List of unlinked Tic areas");
         Text.Clear ();
         MsgHeader ();

         sprintf (Temp, "Area(s) not linked to %s:", Msg->ToAddress);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         Text.Add ("");
         Total = 0L;
         if ((FileData = new TFileData (Cfg->SystemPath)) != NULL) {
            FileEchoLink = new TFilechoLink (Cfg->SystemPath);
            if (FileData->First () == TRUE)
               do {
                  if (FileData->EchoTag[0] != '\0' && FileEchoLink != NULL) {
                     FileEchoLink->Load (FileData->EchoTag);
                     if (FileEchoLink->Check (Msg->ToAddress) == FALSE) {
                        sprintf (Temp, "%-30.30s %.48s", FileData->EchoTag, FileData->Display);
                        Text.Add (Temp);
                        Total++;
                     }
                  }
               } while (FileData->Next () == TRUE);
            if (FileEchoLink != NULL)
               delete FileEchoLink;
            delete FileData;
         }
         Text.Add ("");
         sprintf (Temp, "%lu unlinked area(s).", Total);
         Text.Add (Temp);
         MsgFooter ();

         Msg->Add (Text);
      }

      if (DoHelp == TRUE) {
         strcpy (Msg->Subject, "Raid help");
         Text.Clear ();
         MsgHeader ();

         sprintf (Temp, "%sraidhelp.txt", Cfg->SystemPath);
         if ((fp = fopen (Temp, "rt")) != NULL) {
            while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
               Temp[strlen (Temp) - 1] = '\0';
               Text.Add (Temp);
            }
            fclose (fp);
         }
         else {
            Text.Add ("");
            Text.Add ("No help file available. Contact the Sysop.");
            Text.Add ("");
         }

         MsgFooter ();
         Msg->Add (Text);
      }
   }
   else {
      Text.Clear ();
      MsgHeader ();

      Text.Add ("");
      sprintf (Temp, "Node %s isn't authorized to use raid at %s", Msg->FromAddress, Msg->ToAddress);
      Text.Add (Temp);
      Text.Add ("");

      strcpy (Temp, Msg->FromAddress);
      strcpy (Msg->FromAddress, Msg->ToAddress);
      strcpy (Msg->ToAddress, Temp);
      strcpy (Msg->To, Msg->From);
      strcpy (Msg->From, "Tic Manager");
      strcpy (Msg->Subject, "Tic changes report");
      Msg->Text.Clear ();
      Msg->Local = TRUE;
      Msg->Crash = Msg->Direct = Msg->Hold = FALSE;

      MsgFooter ();
      Msg->Add (Text);
   }
}

VOID TAreaManager::Rescan (PSZ pszEchoTag, PSZ pszAddress)
{
   CHAR Temp[128], Outbound[64], *Text;
   ULONG Number;
   struct stat statbuf;
   class TMsgData *Data;
   class TMsgBase *Msg;
   class TAddress Forward;
   class PACKET *Packet;
   class TKludges *SeenBy, *Path;

   strcpy (Outbound, Cfg->Outbound);
   Outbound[strlen (Outbound) - 1] = '\0';

   Forward.Parse (pszAddress);

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Data->ReadEcho (pszEchoTag) == TRUE) {
         Msg = NULL;
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

         if (Msg != NULL) {
            if ((Packet = new PACKET) != NULL) {
               strcpy (Packet->ToAddress, Forward.String);

               if (Data->Address[0] != '\0')
                  strcpy (Packet->FromAddress, Data->Address);
               else {
                  if (Cfg->MailAddress.First () == TRUE) {
                     strcpy (Packet->FromAddress, Cfg->MailAddress.String);
                     do {
                        if (Cfg->MailAddress.Zone == Forward.Zone) {
                           strcpy (Packet->FromAddress, Cfg->MailAddress.String);
                           break;
                        }
                     } while (Cfg->MailAddress.Next () == TRUE);
                  }
               }

               Cfg->MailAddress.First ();
               if (Cfg->MailAddress.Zone == Forward.Zone) {
                  if (Forward.Point != 0) {
#if defined(__LINUX__)
                     sprintf (Temp, "%s/%04x%04x.pnt", Outbound, Forward.Net, Forward.Node);
                     mkdir (Temp, 0666);
                     sprintf (Temp, "%s/%04x%04x.pnt/%08x.xpr", Outbound, Forward.Net, Forward.Node, Forward.Point);
#else
                     sprintf (Temp, "%s\\%04x%04x.pnt", Outbound, Forward.Net, Forward.Node);
                     mkdir (Temp);
                     sprintf (Temp, "%s\\%04x%04x.pnt\\%08x.xpr", Outbound, Forward.Net, Forward.Node, Forward.Point);
#endif
                  }
                  else
#if defined(__LINUX__)
                     sprintf (Temp, "%s/%04x%04x.xpr", Outbound, Forward.Net, Forward.Node);
#else
                     sprintf (Temp, "%s\\%04x%04x.xpr", Outbound, Forward.Net, Forward.Node);
#endif
               }
               else {
                  sprintf (Temp, "%s.%03x", Outbound, Forward.Zone);
#if defined(__LINUX__)
                  mkdir (Temp, 0666);
#else
                  mkdir (Temp);
#endif
                  if (Forward.Point != 0) {
#if defined(__LINUX__)
                     sprintf (Temp, "%s.%03x/%04x%04x.pnt", Outbound, Forward.Zone, Forward.Net, Forward.Node);
                     mkdir (Temp, 0666);
                     sprintf (Temp, "%s.%03x/%04x%04x.pnt/%08x.xpr", Outbound, Forward.Zone, Forward.Net, Forward.Node, Forward.Point);
#else
                     sprintf (Temp, "%s.%03x\\%04x%04x.pnt", Outbound, Forward.Zone, Forward.Net, Forward.Node);
                     mkdir (Temp);
                     sprintf (Temp, "%s.%03x\\%04x%04x.pnt\\%08x.xpr", Outbound, Forward.Zone, Forward.Net, Forward.Node, Forward.Point);
#endif
                  }
                  else
#if defined(__LINUX__)
                     sprintf (Temp, "%s.%03x/%04x%04x.xpr", Outbound, Forward.Zone, Forward.Net, Forward.Node);
#else
                     sprintf (Temp, "%s.%03x\\%04x%04x.xpr", Outbound, Forward.Zone, Forward.Net, Forward.Node);
#endif
               }

               if (stat (Temp, &statbuf) != 0) {
                  if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                     if (Nodes->Read (Forward.String) == TRUE)
                        strcpy (Packet->Password, Nodes->OutPktPwd);
                     delete Nodes;
                     Nodes = NULL;
                  }
               }

               if (Packet->Open (Temp, FALSE) == TRUE) {
                  Number = Msg->Lowest ();
                  // Evita di esportare il messaggio #1 delle basi Fido (l'high water mark)
                  if (Number == 1L && Data->Storage == ST_FIDO) {
                     if (Msg->Next (Number) == FALSE)
                        Number = 0L;
                  }
                  do {
                     if (Msg->Read (Number) == TRUE) {
                        sprintf (Temp, "AREA:%s", Data->EchoTag);
                        if ((Text = (PSZ)Msg->Text.First ()) != NULL) {
                           Msg->Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
                           if (*Text != '\0')
                              Msg->Text.Insert (Text, (USHORT)(strlen (Text) + 1));
                           Msg->Text.First ();
                           Msg->Text.Remove ();
                        }
                        else
                           Msg->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));

                        SeenBy = new TKludges;
                        Path = new TKludges;

                        if (SeenBy != NULL && Path != NULL) {
                           Path->Sort = FALSE;
                           if ((Text = (PSZ)Msg->Text.First ()) != NULL)
                              do {
                                 if (!strncmp (Text, "SEEN-BY: ", 9) && SeenBy != NULL) {
                                    SeenBy->AddString (&Text[9]);
                                    Msg->Text.Remove ();
                                    Text = (PSZ)Msg->Text.Value ();
                                 }
                                 else if (!strncmp (Text, "\001PATH: ", 7) && Path != NULL) {
                                    Path->AddString (&Text[7]);
                                    Msg->Text.Remove ();
                                    Text = (PSZ)Msg->Text.Value ();
                                 }
                                 else {
                                    if (Msg->Local == TRUE && Cfg->ReplaceTear == TRUE) {
                                       if (!strncmp (Text, "---", 3)) {
                                          sprintf (Temp, "--- %s", Cfg->TearLine);
                                          strsrep (Temp, "%1", VERSION);
                                          Msg->Text.Replace (Temp, (USHORT)(strlen (Temp) + 1));
                                       }
                                    }
                                    Text = (PSZ)Msg->Text.Next ();
                                 }
                              } while (Text != NULL);

                           SeenBy->AddString (Forward.String);

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
                                    Msg->Text.Add (Temp);
                                    strcpy (Temp, "SEEN-BY:");
                                    strcpy (SeenBy->ShortAddress, SeenBy->Address);
                                 }
                                 if (SeenBy->Net != Cfg->FakeNet && SeenBy->Point == 0) {
                                    strcat (Temp, " ");
                                    strcat (Temp, SeenBy->ShortAddress);
                                 }
                              } while (SeenBy->Next () == TRUE);
                              if (strlen (Temp) > 8)
                                 Msg->Text.Add (Temp);
                           }

                           if (Path->First () == TRUE) {
                              strcpy (Temp, "\001PATH:");
                              do {
                                 if (strlen (Temp) + strlen (Path->ShortAddress) + 1 > 70) {
                                    Msg->Text.Add (Temp);
                                    strcpy (Temp, "\001PATH:");
                                    strcpy (Path->ShortAddress, SeenBy->Address);
                                 }
                                 if (Path->Point == 0) {
                                    strcat (Temp, " ");
                                    strcat (Temp, Path->ShortAddress);
                                 }
                              } while (Path->Next () == TRUE);
                              if (strlen (Temp) > 6)
                                 Msg->Text.Add (Temp);
                           }
                        }

                        if (Path != NULL)
                           delete Path;
                        if (SeenBy != NULL)
                           delete SeenBy;

                        strcpy (Msg->ToAddress, Forward.String);
                        Packet->Add (Msg);
                     }
                  } while (Msg->Next (Number) == TRUE);
               }

               delete Packet;
            }

            delete Msg;
         }
      }

      delete Data;
   }
}


