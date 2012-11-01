
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

VOID TMailProcessor::Export (VOID)
{
   USHORT i, UseEchotoss;
   ULONG Number, SentArea, Highest;
   class TEchotoss *Echo;

   UseEchotoss = FALSE;
   Data = new TMsgData (Cfg->SystemPath);
   Forward = new TEchoLink (Cfg->SystemPath);
   Dupes = new TDupes (Cfg->SystemPath);

   SeenBy = new TKludges;
   Path = new TKludges;

   strcpy (Outbound, Cfg->Outbound);
   if (Outbound[strlen (Outbound) - 1] == '\\' || Outbound[strlen (Outbound) - 1] == '/')
      Outbound[strlen (Outbound) - 1] = '\0';

   if (Output != NULL)
      Output->Clear ();
   if (Status != NULL)
      Status->Clear ();

   if (Log != NULL)
      Log->Write ("#Scanning messages");

   SentArea = MsgSent = 0L;

   if (Data != NULL && Forward != NULL) {
      if ((Echo = new TEchotoss (Cfg->SystemPath)) != NULL) {
         Echo->Load ();
         Echo->Delete ();
         if (Echo->First () == TRUE) {
            UseEchotoss = TRUE;

            do {
               Data->ReadEcho (Echo->Tag);

               if (Status != NULL)
                  Status->SetLine (0, "Scanning %s", Data->EchoTag);

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
#if !defined(__POINT__)
               else if (Data->Storage == ST_USENET)
                  Msg = new USENET (Cfg->NewsServer, Data->NewsGroup);
#endif

               if (Msg != NULL) {
                  Msg->Lock (0L);
                  Highest = Msg->Highest ();

                  if (Msg->GetHWM (Number) == FALSE)
                     Number = Data->HighWaterMark;
                  if (Number > Highest) {
                     if (Log != NULL)
                        Log->Write ("!Found HWM error: %lu / %lu (%s)", Number, Msg->Highest (), Data->Path);
                     Number = Highest;
                  }

                  if (Status != NULL)
                     Status->SetLine (1, "   %lu / %lu", Msg->UidToMsgn (Number), Msg->Number ());

                  if (Msg->Next (Number) == TRUE) {
                     Forward->Load (Data->EchoTag);
                     if (Forward->First () == TRUE) {
                        SentArea = 0L;
                        Dupes->Load (Data->EchoTag);

                        do {
                           if (Status != NULL)
                              Status->SetLine (1, "   %lu / %lu", Msg->UidToMsgn (Number), Msg->Number ());
                           i = ExportEchoMail (Number, Data->EchoTag);
                           SentArea += i;
                           MsgSent += i;
                        } while (Msg->Next (Number) == TRUE);

                        Dupes->Save ();

                        if (Log != NULL && SentArea != 0L)
                           Log->Write (":   %-20.20s (Sent=%04lu)", Data->EchoTag, SentArea);
                     }
                     else
                        Number = Msg->Highest ();
                  }

                  Msg->SetHWM (Number);
                  Data->HighWaterMark = Number;
                  Data->Update ();

                  Msg->UnLock ();
                  delete Msg;
               }
            } while (Echo->Next () == TRUE);
         }
         delete Echo;
      }

      if (UseEchotoss == FALSE && Data->First () == TRUE)
         do {
            if (Data->EchoMail == TRUE && Data->EchoTag[0] != '\0' && Data->Storage != ST_PASSTHROUGH) {
               if (Status != NULL)
                  Status->SetLine (0, "Scanning %s", Data->EchoTag);

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
#if !defined(__POINT__)
               else if (Data->Storage == ST_USENET)
                  Msg = new USENET (Cfg->NewsServer, Data->NewsGroup);
#endif

               if (Msg != NULL) {
                  Msg->Lock (0L);
                  Highest = Msg->Highest ();

                  if (Msg->GetHWM (Number) == FALSE)
                     Number = Data->HighWaterMark;
                  if (Number > Highest) {
                     if (Log != NULL)
                        Log->Write ("!Found HWM error: %lu / %lu (%s)", Number, Msg->Highest (), Data->Path);
                     Number = Highest;
                  }

                  if (Status != NULL)
                     Status->SetLine (1, "   %lu / %lu", Number, Highest);

                  if (Msg->Next (Number) == TRUE) {
                     Forward->Load (Data->EchoTag);
                     if (Forward->First () == TRUE) {
                        SentArea = 0L;
                        Dupes->Load (Data->EchoTag);

                        do {
                           if (Status != NULL)
                              Status->SetLine (1, "   %lu / %lu", Msg->UidToMsgn (Number), Msg->Number ());
                           i = ExportEchoMail (Number, Data->EchoTag);
                           SentArea += i;
                           MsgSent += i;
                        } while (Msg->Next (Number) == TRUE);

                        Dupes->Save ();

                        if (Log != NULL && SentArea != 0L)
                           Log->Write (":   %-20.20s (Sent=%04lu)", Data->EchoTag, SentArea);
                     }
                     else
                        Number = Msg->Highest ();
                  }

                  Msg->SetHWM (Number);
                  Data->HighWaterMark = Number;
                  Data->Update ();

                  Msg->UnLock ();
                  delete Msg;
               }
            }
         } while (Data->Next () == TRUE);
   }

   if (Log != NULL) {
      if (MsgSent == 0L)
         Log->Write ("+No ECHOmail messages forwarded");
      else
         Log->Write ("+%lu ECHOmail message(s) forwarded", MsgSent);
   }

   Msg = NULL;
   Packet = NULL;

   if (Status != NULL)
      Status->Clear ();

   if (Path != NULL)
      delete Path;
   if (SeenBy != NULL)
      delete SeenBy;

   if (Dupes != NULL)
      delete Dupes;
   if (Forward != NULL)
      delete Forward;
   if (Data != NULL)
      delete Data;
}

USHORT TMailProcessor::ExportEchoMail (ULONG Number, PSZ pszEchoTag)
{
   USHORT SentArea = 0, First = TRUE, DoExport;
   CHAR *Text, *Base;
   ULONG Msgn;
   struct stat statbuf;
   class TAddress ToAddr;
   class PACKET *Packet;
   class TCollection *MsgText;

   Base = "???";
   if (Data->Storage == ST_SQUISH)
      Base = "Squish<tm>";
   else if (Data->Storage == ST_JAM)
      Base = "JAM";
   else if (Data->Storage == ST_FIDO)
      Base = "Fido *.MSG";
   else if (Data->Storage == ST_ADEPT)
      Base = "AdeptXBBS";
   else if (Data->Storage == ST_HUDSON)
      Base = "Hudson";
#if !defined(__POINT__)
   else if (Data->Storage == ST_PASSTHROUGH)
      Base = "Passthrough";
   else if (Data->Storage == ST_USENET)
      Base = "Newsgroup";
#endif

   if (Msg != NULL && (Data->Storage != ST_FIDO || Number != 1L)) {
      if (Msg->Read (Number, MAX_LINE_LENGTH) == TRUE) {
         DoExport = FALSE;

         if (SeenBy != NULL && Path != NULL) {
            SeenBy->Clear ();
            Path->Clear ();

            Path->Sort = FALSE;
            SeenBy->Sort = FALSE;

            //////////////////////////////////////////////////////////////////
            // Analizza il testo e crea la lista di seen-by e path          //
            //////////////////////////////////////////////////////////////////
            MsgText = &Msg->Text;
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

            //////////////////////////////////////////////////////////////////
            // Controlla che il messaggio sia da esportare                  //
            //////////////////////////////////////////////////////////////////
            if (Forward->First () == TRUE)
               do {
                  if (SeenBy->Check (Forward->Address) == FALSE && Forward->SendOnly == FALSE) {
                     DoExport = TRUE;
                     break;
                  }
               } while (Forward->Next () == TRUE);

            //////////////////////////////////////////////////////////////////
            // Se il messaggio e' locale rimpiazza la tearline              //
            //////////////////////////////////////////////////////////////////
            if (DoExport == TRUE && Msg->Local == TRUE && Cfg->ReplaceTear == TRUE) {
               if ((Text = (PSZ)MsgText->Last ()) != NULL)
                  do {
                     if (!strncmp (Text, "---", 3)) {
                        sprintf (Temp, "--- %s", Cfg->TearLine);
                        strsrep (Temp, "%1", VERSION);
                        MsgText->Replace (Temp);
                        break;
                     }
                  } while ((Text = (PSZ)MsgText->Previous ()) != NULL);
            }

#if defined(__OS2__)
            DosSleep (1L);
#elif defined(__NT__)
            Sleep (1L);
#endif

            if (DoExport == TRUE) {
               if (Forward->First () == TRUE)
                  do {
                     Forward->Skip = FALSE;
                     Forward->Update ();
                  } while (Forward->Next () == TRUE);

               //////////////////////////////////////////////////////////////////
               // Verifica per quali nodi deve essere esportato                //
               //////////////////////////////////////////////////////////////////
               SeenBy->Sort = TRUE;
               if (Forward->First () == TRUE)
                  do {
                     if (SeenBy->Check (Forward->Address) == TRUE) {
                        Forward->Skip = TRUE;
                        Forward->Update ();
                     }
                     else
                        SeenBy->AddString (Forward->Address);
                  } while (Forward->Next () == TRUE);

               //////////////////////////////////////////////////////////////////
               // Aggiunge il proprio indirizzo ai seen-by e path              //
               //////////////////////////////////////////////////////////////////
               if (Data->Address[0] != '\0') {
                  SeenBy->AddString (Data->Address);
                  Path->AddString (Data->Address);
               }
               else if (Cfg->MailAddress.First () == TRUE) {
                  SeenBy->AddString (Cfg->MailAddress.String);
                  Path->AddString (Cfg->MailAddress.String);
               }

               //////////////////////////////////////////////////////////////////
               // Ricostruisce le linee seen-by                                //
               //////////////////////////////////////////////////////////////////
               if (SeenBy->First () == TRUE) {
                  strcpy (Temp, "SEEN-BY:");
                  do {
                     if (strlen (Temp) + strlen (SeenBy->ShortAddress) + 1 > 70) {
                        MsgText->Add (Temp);
                        strcpy (Temp, "SEEN-BY:");
                        strcpy (SeenBy->ShortAddress, SeenBy->Address);
                     }
                     if (SeenBy->Net != Cfg->FakeNet && SeenBy->Point == 0) {
                        strcat (Temp, " ");
                        strcat (Temp, SeenBy->ShortAddress);
                     }
                  } while (SeenBy->Next () == TRUE);
                  if (strlen (Temp) > 8)
                     MsgText->Add (Temp);
               }

               //////////////////////////////////////////////////////////////////
               // Ricostruisce le linee path                                   //
               //////////////////////////////////////////////////////////////////
               if (Path->First () == TRUE) {
                  strcpy (Temp, "\001PATH:");
                  do {
                     if (strlen (Temp) + strlen (Path->ShortAddress) + 1 > 70) {
                        MsgText->Add (Temp);
                        strcpy (Temp, "\001PATH:");
                        strcpy (Path->ShortAddress, SeenBy->Address);
                     }
                     if (Path->Point == 0) {
                        strcat (Temp, " ");
                        strcat (Temp, Path->ShortAddress);
                     }
                  } while (Path->Next () == TRUE);
                  if (strlen (Temp) > 6)
                     MsgText->Add (Temp);
               }
            }
         }

         if (DoExport == TRUE) {
            //////////////////////////////////////////////////////////////////
            // Aggiunge la linea AREA: in testa al messaggio                //
            //////////////////////////////////////////////////////////////////
            sprintf (Temp, "AREA:%s", pszEchoTag);
            if ((Text = (PSZ)Msg->Text.First ()) != NULL) {
               Msg->Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
               if (*Text != '\0')
                  Msg->Text.Insert (Text, (USHORT)(strlen (Text) + 1));
               Msg->Text.First ();
               Msg->Text.Remove ();
            }
            else
               Msg->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));

            //////////////////////////////////////////////////////////////////
            // Esporta il messaggio verso i destinatari                     //
            //////////////////////////////////////////////////////////////////
            if (Forward->First () == TRUE)
               do {
                  if (Forward->SendOnly == TRUE)
                     Forward->Skip = TRUE;
                  if (Forward->PersonalOnly == TRUE || Forward->Passive == TRUE) {
                     if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                        if (Nodes->Read (Forward->Address) == TRUE) {
                           if (stricmp (Nodes->SysopName, Msg->To))
                              Forward->Skip = TRUE;
                        }
                        delete Nodes;
                        Nodes = NULL;
                     }
                  }

                  if (Forward->Skip == FALSE) {
                     Msgn = Msg->UidToMsgn (Number);
                     if (Status != NULL && (Msgn % 10L) != 0L)
                        Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());

                     if (Output != NULL) {
                        if (First == TRUE) {
                           sprintf (Display, "%6lu %-22.22s %-12.12s %s", Msg->UidToMsgn (Number), pszEchoTag, Base, Forward->Address);
                           Output->Add (Display);
                           First = FALSE;
                        }
                        else {
                           if ((strlen (Display) + strlen (Forward->ShortAddress)) > 68) {
                              sprintf (Display, "       %-22.22s %-12.12s %s", "", "", Forward->Address);
                              Output->Add (Display);
                           }
                           else {
                              strcat (Display, " ");
                              strcat (Display, Forward->ShortAddress);
                              Output->Update (Display);
                           }
                        }
                     }
                     if ((Packet = new PACKET) != NULL) {
                        strcpy (Packet->ToAddress, Forward->Address);

                        if (Data->Address[0] != '\0')
                           strcpy (Packet->FromAddress, Data->Address);
                        else {
                           ToAddr.Parse (Packet->ToAddress);
                           if (Cfg->MailAddress.First () == TRUE) {
                              strcpy (Packet->FromAddress, Cfg->MailAddress.String);
                              do {
                                 if (Cfg->MailAddress.Zone == ToAddr.Zone) {
                                    strcpy (Packet->FromAddress, Cfg->MailAddress.String);
                                    break;
                                 }
                              } while (Cfg->MailAddress.Next () == TRUE);
                           }
                        }

                        Cfg->MailAddress.First ();
                        if (Cfg->MailAddress.Zone == Forward->Zone) {
                           if (Forward->Point != 0) {
#if defined(__LINUX__)
                              sprintf (Temp, "%s/%04x%04x.pnt", Outbound, Forward->Net, Forward->Node);
                              mkdir (Temp, 0666);
                              sprintf (Temp, "%s/%04x%04x.pnt/%08x.xpr", Outbound, Forward->Net, Forward->Node, Forward->Point);
#else
                              sprintf (Temp, "%s\\%04x%04x.pnt", Outbound, Forward->Net, Forward->Node);
                              mkdir (Temp);
                              sprintf (Temp, "%s\\%04x%04x.pnt\\%08x.xpr", Outbound, Forward->Net, Forward->Node, Forward->Point);
#endif
                           }
                           else
#if defined(__LINUX__)
                              sprintf (Temp, "%s/%04x%04x.xpr", Outbound, Forward->Net, Forward->Node);
#else
                              sprintf (Temp, "%s\\%04x%04x.xpr", Outbound, Forward->Net, Forward->Node);
#endif
                        }
                        else {
                           sprintf (Temp, "%s.%03x", Outbound, Forward->Zone);
#if defined(__LINUX__)
                           mkdir (Temp, 0666);
#else
                           mkdir (Temp);
#endif
                           if (Forward->Point != 0) {
#if defined(__LINUX__)
                              sprintf (Temp, "%s.%03x/%04x%04x.pnt", Outbound, Forward->Zone, Forward->Net, Forward->Node);
                              mkdir (Temp, 0666);
                              sprintf (Temp, "%s.%03x/%04x%04x.pnt/%08x.xpr", Outbound, Forward->Zone, Forward->Net, Forward->Node, Forward->Point);
#else
                              sprintf (Temp, "%s.%03x\\%04x%04x.pnt", Outbound, Forward->Zone, Forward->Net, Forward->Node);
                              mkdir (Temp);
                              sprintf (Temp, "%s.%03x\\%04x%04x.pnt\\%08x.xpr", Outbound, Forward->Zone, Forward->Net, Forward->Node, Forward->Point);
#endif
                           }
                           else
#if defined(__LINUX__)
                              sprintf (Temp, "%s.%03x/%04x%04x.xpr", Outbound, Forward->Zone, Forward->Net, Forward->Node);
#else
                              sprintf (Temp, "%s.%03x\\%04x%04x.xpr", Outbound, Forward->Zone, Forward->Net, Forward->Node);
#endif
                        }
                        if (stat (Temp, &statbuf) != 0) {
                           if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                              if (Nodes->Read (Forward->Address) == TRUE)
                                 strcpy (Packet->Password, Nodes->OutPktPwd);
                              delete Nodes;
                              Nodes = NULL;
                           }
                        }
                        if (Packet->Open (Temp, FALSE) == TRUE)
                           Packet->Add (Msg);
                        delete Packet;

#if defined(__OS2__)
                        DosSleep (1L);
#elif defined(__NT__)
                        Sleep (1L);
#endif

                        SentArea++;
                     }
                  }
               } while (Forward->Next () == TRUE);
         }

         //////////////////////////////////////////////////////////////////
         // Se il messaggio e' locale aggiorna il database dei duplicati //
         //////////////////////////////////////////////////////////////////
         if (Dupes != NULL && Msg->Local == TRUE) {
            if (Dupes->Check (Data->EchoTag, Msg) == FALSE)
               Dupes->Add (Data->EchoTag, Msg);
         }
      }
   }

   return (SentArea);
}

VOID TMailProcessor::ExportNetMail (VOID)
{
   FILE *fp;
   USHORT OurPoint, FoundIntl;
   CHAR *Text, *Base, PktFile[128], Attach[128], *p;
   ULONG Number, Sent, Msgn;
   struct stat statbuf;
   class TAddress Address, FromAddr, ToAddr;
#if !defined(__POINT__)
   class TAreaManager *AreaMgr;
#endif

   if (Output != NULL)
      Output->Clear ();

   strcpy (Outbound, Cfg->Outbound);
   if (Outbound[strlen (Outbound) - 1] == '\\' || Outbound[strlen (Outbound) - 1] == '/')
      Outbound[strlen (Outbound) - 1] = '\0';

   Msg = NULL;
   Base = "???";
   if (Cfg->NetMailStorage == ST_JAM) {
      Msg = new JAM (Cfg->NetMailPath);
      Base = "JAM";
   }
   else if (Cfg->NetMailStorage == ST_SQUISH) {
      Msg = new SQUISH (Cfg->NetMailPath);
      Base = "Squish<tm>";
   }
   else if (Cfg->NetMailStorage == ST_FIDO) {
      Msg = new FIDOSDM (Cfg->NetMailPath);
      Base = "Fido *.MSG";
   }
   else if (Cfg->NetMailStorage == ST_ADEPT) {
      Msg = new ADEPT (Cfg->NetMailPath);
      Base = "AdeptXBBS";
   }
   else if (Cfg->NetMailStorage == ST_HUDSON) {
      Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->NetMailBoard);
      Base = "Hudson";
   }

#if !defined(__POINT__)
   if ((AreaMgr = new TAreaManager) != NULL) {
      AreaMgr->Cfg = Cfg;
      AreaMgr->Log = Log;
   }
#endif

   if (Msg != NULL) {
      Msg->Lock (0L);
      Sent = 0L;

      if (Log != NULL)
         Log->Write ("#Packing from %s (%lu msgs)", Cfg->NetMailPath, Msg->Number ());

      if (Status != NULL) {
         Status->Clear ();
         Status->SetLine (0, "Exporting from %s", Cfg->NetMailPath);
      }

      Number = Msg->Lowest ();
      do {
         if (Msg->Read (Number, MAX_LINE_LENGTH) == TRUE) {
            Msgn = Msg->UidToMsgn (Number);
            if (Status != NULL && (Msgn % 10L) == 0L)
               Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());

            Address.Clear ();
            Address.Parse (Msg->ToAddress);
            if (Cfg->MailAddress.First () == TRUE && Address.Zone == 0)
               Address.Zone = Cfg->MailAddress.Zone;

            FoundIntl = FALSE;
            if ((Text = (PSZ)Msg->Text.First ()) != NULL)
               do {
                  if (Text[0] == '\0')
                     continue;
                  if (!strncmp (Text, "\001INTL ", 6)) {
                     FoundIntl = TRUE;
                     if (Msg->Local == FALSE || Cfg->ReplaceTear == FALSE)
                        break;
                  }
                  if (Msg->Local == TRUE && Cfg->ReplaceTear == TRUE) {
                     if (!strncmp (Text, "---", 3)) {
                        sprintf (Temp, "--- %s", Cfg->TearLine);
                        strsrep (Temp, "%1", VERSION);
                        Msg->Text.Replace (Temp, (USHORT)(strlen (Temp) + 1));
                     }
                  }
                  else if (Text[0] != 0x01)
                     break;
               } while ((Text = (PSZ)Msg->Text.Next ()) != NULL);

            // Controlla che non si tratti di un messaggio indirizzato ad uno dei
            // nostri aka e nel contempo se e' indirizzato ad un nostro point
            OurPoint = FALSE;
            if (Msg->Sent == FALSE && Cfg->MailAddress.First () == TRUE)
               do {
                  if (Cfg->MailAddress.Zone == Address.Zone || Address.Zone == 0) {
                     if (Cfg->MailAddress.Net == Address.Net && Cfg->MailAddress.Node == Address.Node) {
                        // Ormai e' sicuro che si tratta di un nostro point
                        OurPoint = TRUE;
                        if (Cfg->MailAddress.Point != 0 && Cfg->MailAddress.Point == Address.Point)
                           Msg->Sent = TRUE;
                     }
                  }
               } while (Cfg->MailAddress.Next () == TRUE);

            if (Msg->Sent == FALSE && OurPoint == TRUE) {
               if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                  if (Nodes->First () == TRUE)
                     do {
                        if (Nodes->Zone == Address.Zone && !stricmp (Msg->To, Nodes->SysopName) && Nodes->RemapMail == TRUE) {
                           if (Log != NULL)
                              Log->Write ("#  Remap %s => %s", Address.String, Nodes->Address);
                           sprintf (Temp, "\001PointMap %s => %s", Address.String, Nodes->Address);
                           Msg->Text.Add (Temp);
                           Address.Parse (Nodes->Address);
                           break;
                        }
                     } while (Nodes->Next () == TRUE);
                  delete Nodes;
                  Nodes = NULL;
               }

               if (Cfg->MailAddress.First () == TRUE)
                  do {
                     if (Cfg->MailAddress.Zone == Address.Zone || Address.Zone == 0) {
                        if (Cfg->MailAddress.Net == Address.Net && Cfg->MailAddress.Node == Address.Node && Cfg->MailAddress.Point == Address.Point)
                           Msg->Sent = TRUE;
                     }
                  } while (Cfg->MailAddress.Next () == TRUE);
            }

            if (Msg->Sent == FALSE) {
               // Se non si tratta di un nostro point, il messaggio viene indirizzato
               // sempre al boss, cioe' all'indirizzo con point = 0
               if (OurPoint == FALSE) {
                  Address.Point = 0;
                  Address.Add ();
                  Address.First ();
               }

               if (Status != NULL && (Msgn % 10L) != 0L)
                  Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());

               if (Output != NULL) {
                  sprintf (Display, "%6lu %-22.22s %-12.12s ", Msg->UidToMsgn (Number), "Netmail", Base);
                  if (OurPoint == TRUE)
                     strcat (Display, Msg->ToAddress);
                  else
                     strcat (Display, Address.String);
                  Output->Add (Display);
               }

               Cfg->MailAddress.First ();
               if (Address.Zone == 0 || Cfg->MailAddress.Zone == Address.Zone) {
                  if (Address.Point != 0) {
#if defined(__LINUX__)
                     sprintf (PktFile, "%s/%04x%04x.pnt", Outbound, Address.Net, Address.Node);
                     mkdir (PktFile, 0666);
                     sprintf (PktFile, "%s/%04x%04x.pnt/%08x.xpr", Outbound, Address.Net, Address.Node, Address.Point);
#else
                     sprintf (PktFile, "%s\\%04x%04x.pnt", Outbound, Address.Net, Address.Node);
                     mkdir (PktFile);
                     sprintf (PktFile, "%s\\%04x%04x.pnt\\%08x.xpr", Outbound, Address.Net, Address.Node, Address.Point);
#endif
                  }
                  else
#if defined(__LINUX__)
                     sprintf (PktFile, "%s/%04x%04x.xpr", Outbound, Address.Net, Address.Node);
#else
                     sprintf (PktFile, "%s\\%04x%04x.xpr", Outbound, Address.Net, Address.Node);
#endif
               }
               else {
                  sprintf (PktFile, "%s.%03x", Outbound, Address.Zone);
#if defined(__LINUX__)
                  mkdir (PktFile, 0666);
#else
                  mkdir (PktFile);
#endif
                  if (Address.Point != 0) {
#if defined(__LINUX__)
                     sprintf (PktFile, "%s.%03x/%04x%04x.pnt", Outbound, Address.Zone, Address.Net, Address.Node);
                     mkdir (PktFile, 0666);
                     sprintf (PktFile, "%s.%03x/%04x%04x.pnt/%08x.xpr", Outbound, Address.Zone, Address.Net, Address.Node, Address.Point);
#else
                     sprintf (PktFile, "%s.%03x\\%04x%04x.pnt", Outbound, Address.Zone, Address.Net, Address.Node);
                     mkdir (PktFile);
                     sprintf (PktFile, "%s.%03x\\%04x%04x.pnt\\%08x.xpr", Outbound, Address.Zone, Address.Net, Address.Node, Address.Point);
#endif
                  }
                  else
#if defined(__LINUX__)
                     sprintf (PktFile, "%s.%03x/%04x%04x.xpr", Outbound, Address.Zone, Address.Net, Address.Node);
#else
                     sprintf (PktFile, "%s.%03x\\%04x%04x.xpr", Outbound, Address.Zone, Address.Net, Address.Node);
#endif
               }

               if (Msg->Crash == TRUE)
                  strcpy (&PktFile[strlen (PktFile) - 3], "cut");
               else if (Msg->Direct == TRUE)
                  strcpy (&PktFile[strlen (PktFile) - 3], "dut");
               else if (Msg->Hold == TRUE)
                  strcpy (&PktFile[strlen (PktFile) - 3], "hut");

               if ((Packet = new PACKET) != NULL) {
                  strcpy (Packet->ToAddress, Address.String);
                  ToAddr.Parse (Packet->ToAddress);
                  if (ToAddr.Zone == 0) {
                     ToAddr.Zone = Cfg->MailAddress.Zone;
                     ToAddr.Add ();
                     ToAddr.First ();
                  }

                  if (Msg->FileAttach == TRUE) {
                     if (Address.Zone == 0 || Cfg->MailAddress.Zone == Address.Zone) {
                        if (Address.Point != 0) {
                           sprintf (Attach, "%s\\%04x%04x.pnt", Outbound, Address.Net, Address.Node);
#if defined(__LINUX__)
                           mkdir (AdjustPath (Attach), 0666);
#else
                           mkdir (AdjustPath (Attach));
#endif
                           sprintf (Attach, "%s\\%04x%04x.pnt\\%08x.flo", Outbound, Address.Net, Address.Node, Address.Point);
                        }
                        else
                           sprintf (Attach, "%s\\%04x%04x.flo", Outbound, Address.Net, Address.Node);
                     }
                     else {
                        sprintf (Attach, "%s.%03x", Outbound, Address.Zone);
#if defined(__LINUX__)
                        mkdir (AdjustPath (Attach), 0666);
#else
                        mkdir (AdjustPath (Attach));
#endif
                        if (Address.Point != 0) {
                           sprintf (Attach, "%s.%03x\\%04x%04x.flo", Outbound, Address.Zone, Address.Net, Address.Node);
#if defined(__LINUX__)
                           mkdir (AdjustPath (Attach), 0666);
#else
                           mkdir (AdjustPath (Attach));
#endif
                           sprintf (Attach, "%s.%03x\\%04x%04x.pnt\\%08x.flo", Outbound, Address.Zone, Address.Net, Address.Node, Address.Point);
                        }
                        else
                           sprintf (Attach, "%s.%03x\\%04x%04x.flo", Outbound, Address.Zone, Address.Net, Address.Node);
                     }

                     if (Msg->Crash == TRUE)
                        strcpy (&Attach[strlen (Attach) - 3], "clo");
                     else if (Msg->Direct == TRUE)
                        strcpy (&Attach[strlen (Attach) - 3], "dlo");
                     else if (Msg->Hold == TRUE)
                        strcpy (&Attach[strlen (Attach) - 3], "hlo");

                     if ((fp = fopen (AdjustPath (Attach), "at")) != NULL) {
                        strcpy (Attach, Msg->Subject);
                        if ((p = strtok (Attach, " ,;")) != NULL)
                           do {
                              fprintf (fp, "%s\n", p);
                              if (Log != NULL)
                                 Log->Write ("+  Sending attach %s to %s", p, Address.String);
                           } while ((p = strtok (NULL, " ,;")) != NULL);
                        fclose (fp);
                     }
                  }

                  strcpy (Packet->FromAddress, Msg->FromAddress);
                  FromAddr.Parse (Packet->FromAddress);
                  if (FromAddr.Zone == 0) {
                     FromAddr.Zone = Cfg->MailAddress.Zone;
                     FromAddr.Add ();
                     FromAddr.First ();
                  }

                  if ((Cfg->ForceIntl == TRUE || FromAddr.Zone != ToAddr.Zone) && FoundIntl == FALSE) {
                     sprintf (Temp, "\001INTL %u:%u/%u %u:%u/%u", ToAddr.Zone, ToAddr.Net, ToAddr.Node, FromAddr.Zone, FromAddr.Net, FromAddr.Node);
                     if ((Text = (PSZ)Msg->Text.First ()) != NULL) {
                        Msg->Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
                        Msg->Text.Insert (Text, (USHORT)(strlen (Text) + 1));
                        Msg->Text.First ();
                        Msg->Text.Remove ();
                     }
                     else
                        Msg->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                  }

                  // Se il file non esiste, allora cerca la password di pacchetto
                  // definita per il nodo di destinazione
                  if (stat (Temp, &statbuf) != 0) {
                     if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                        if (Nodes->Read (Packet->ToAddress) == TRUE)
                           strcpy (Packet->Password, Nodes->OutPktPwd);
                        delete Nodes;
                        Nodes = NULL;
                     }
                  }

                  // Apre il pacchetto .xpr senza effettuare lo scan dei messaggi
                  if (Packet->Open (PktFile, FALSE) == TRUE) {
                     Packet->Add (Msg);
                     Sent++;
                  }

                  if (Cfg->KeepNetMail == FALSE)
                     Msg->Delete (Number);

                  delete Packet;
                  Packet = NULL;
               }

               Msg->Sent = TRUE;
               Msg->WriteHeader (Number);
            }
#if !defined(__POINT__)
            else if (Msg->Received == FALSE) {
               if (!stricmp (Msg->To, "Areafix") || !stricmp (Msg->To, "AreaMgr")) {
                  if (AreaMgr != NULL) {
                     Msg->Received = TRUE;
                     Msg->WriteHeader (Number);

                     AreaMgr->Msg = Msg;
                     AreaMgr->ProcessAreafix ();
                  }
               }
               else if (!stricmp (Msg->To, "Raid")) {
                  if (AreaMgr != NULL) {
                     Msg->Received = TRUE;
                     Msg->WriteHeader (Number);

                     AreaMgr->Msg = Msg;
                     AreaMgr->ProcessRaid ();
                  }
               }
            }
#endif
         }
      } while (Msg->Next (Number) == TRUE);

      if (Log != NULL)
         Log->Write (":  Packed=%lu", Sent);

      if (Status != NULL)
         Status->Clear ();

      Msg->UnLock ();
      delete Msg;
      Msg = NULL;
   }

#if !defined(__POINT__)
   if (AreaMgr != NULL)
      delete AreaMgr;
#endif
}

VOID TMailProcessor::Change (VOID)
{
   FILE *fp;
   DIR *dir;
   USHORT Zone, Net, Node, Point, CheckNet;
   CHAR *p, FromFlag, ToFlag, Lookup[32], *Text;
   CHAR FromStr[16], ToStr[16];
   class TAddress Addr;
   class TCollection Attach;
   struct dirent *ent;

   Cfg->MailAddress.First ();
   Zone = Cfg->MailAddress.Zone;
   Net = Cfg->MailAddress.Net;
   Node = Cfg->MailAddress.Node;
   Point = 0;

   FromFlag = 'h';
   if ((p = strtok (NULL, " ")) != NULL) {
      if (!stricmp (p, "hold") || !stricmp (p, "crash") || !stricmp (p, "direct") || !stricmp (p, "normal")) {
         if ((FromFlag = (CHAR)tolower (*p)) == 'n')
            FromFlag = 'f';
      }
      strcpy (FromStr, strupr (p));
   }

   ToFlag = 'h';
   if ((p = strtok (NULL, " ")) != NULL) {
      if (!stricmp (p, "hold") || !stricmp (p, "crash") || !stricmp (p, "direct") || !stricmp (p, "normal")) {
         if ((ToFlag = (CHAR)tolower (*p)) == 'n')
            ToFlag = 'f';
      }
      strcpy (ToStr, strupr (p));
   }

   if ((p = strtok (NULL, "")) != NULL)
      strcpy (Line, p);

   while ((p = strtok (Line, " ")) != NULL) {
      Addr.Clear ();
      Addr.Parse (p);

      if ((p = strtok (NULL, "")) != NULL)
         strcpy (Line, p);
      else
         Line[0] = '\0';

      if (Addr.Zone != 0)
         Zone = Addr.Zone;
      if (Addr.Net != 0)
         Net = Addr.Net;
      if (Addr.Node != 0)
         Node = Addr.Node;
      Point = Addr.Point;

      // Cambio dei flags per tutti i point
      if (Point == 65535U && Net != 65535U && Node != 65535U) {
         if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
            sprintf (Temp, "%s\\%04x%04x.pnt", Outbound, Net, Node);
         else
            sprintf (Temp, "%s.%03x\\%04x%04x.pnt", Outbound, Zone, Net, Node);
         if ((dir = opendir (Temp)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
               strlwr (ent->d_name);
               sprintf (Name, ".%clo", FromFlag);
               if (strstr (ent->d_name, Name) != NULL) {
                  sscanf (ent->d_name, "%08hx", &Point);

                  Addr.Zone = Zone;
                  Addr.Net = Net;
                  Addr.Node = Node;
                  Addr.Point = Point;
                  Addr.Add ();
                  Addr.First ();
                  if (Log != NULL)
                     Log->Write (":Changing %s from %s to %s", Addr.String, FromStr, ToStr);

                  // Legge i file attach da cambiare di stato.
                  Attach.Clear ();
                  if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                     sprintf (Name, "%s\\%04x%04x.pnt\\%08x.%clo", Outbound, Net, Node, Point, FromFlag);
                  else
                     sprintf (Name, "%s.%03x\\%04x%04x.pnt\\%08x.%clo", Outbound, Zone, Net, Node, Point, FromFlag);
                  if ((fp = fopen (Name, "rt")) != NULL) {
                     while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL)
                        Attach.Add (Temp);
                     fclose (fp);
                     unlink (Name);
                  }

                  // Legge il file attach che ricevera' gli attachment da cambiare
                  // di stato controllando i duplicati.
                  if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                     sprintf (Name, "%s\\%04x%04x.pnt\\%08x.%clo", Outbound, Net, Node, Point, ToFlag);
                  else
                     sprintf (Name, "%s.%03x\\%04x%04x.pnt\\%08x.%clo", Outbound, Zone, Net, Node, Point, ToFlag);
                  if ((fp = fopen (Name, "rt")) != NULL) {
                     while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                        if ((Text = (CHAR *)Attach.First ()) != NULL)
                           do {
                              if (!stricmp (Text, Temp))
                                 break;
                           } while ((Text = (CHAR *)Attach.Next ()) != NULL);
                        if (Text == NULL)
                           Attach.Add (Temp);
                     }
                     fclose (fp);
                     unlink (Name);
                  }

                  // Scrive il nuovo file attach.
                  if ((fp = fopen (Name, "wt")) != NULL) {
                     if ((Text = (CHAR *)Attach.First ()) != NULL)
                        do {
                           fprintf (fp, "%s", Text);
                        } while ((Text = (CHAR *)Attach.Next ()) != NULL);
                     fclose (fp);
                  }
               }
            }
            closedir (dir);
         }
      }
      else if (Net == 65535U || Node == 65535U) {
         CheckNet = (Net != 65535U) ? TRUE : FALSE;
         sprintf (Lookup, "%04x", Net);
         if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
            sprintf (Temp, "%s", Outbound);
         else
            sprintf (Temp, "%s.%03x", Outbound, Zone);
         if ((dir = opendir (Temp)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
               strlwr (ent->d_name);
               sprintf (Name, ".%clo", FromFlag);
               if (strstr (ent->d_name, Name) != NULL && (CheckNet == FALSE || !strncmp (ent->d_name, Lookup, strlen (Lookup)))) {
                  sscanf (ent->d_name, "%04hx%04hx", &Net, &Node);

                  Addr.Zone = Zone;
                  Addr.Net = Net;
                  Addr.Node = Node;
                  Addr.Point = Point;
                  Addr.Add ();
                  Addr.First ();
                  if (Log != NULL)
                     Log->Write (":Changing %s from %s to %s", Addr.String, FromStr, ToStr);

                  // Legge i file attach da cambiare di stato.
                  Attach.Clear ();
                  if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                     sprintf (Name, "%s\\%04x%04x.%clo", Outbound, Net, Node, FromFlag);
                  else
                     sprintf (Name, "%s.%03x\\%04x%04x.%clo", Outbound, Zone, Net, Node, FromFlag);
                  if ((fp = fopen (Name, "rt")) != NULL) {
                     while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL)
                        Attach.Add (Temp);
                     fclose (fp);
                     unlink (Name);
                  }

                  // Legge il file attach che ricevera' gli attachment da cambiare
                  // di stato controllando i duplicati.
                  if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                     sprintf (Name, "%s\\%04x%04x.%clo", Outbound, Net, Node, ToFlag);
                  else
                     sprintf (Name, "%s.%03x\\%04x%04x.%clo", Outbound, Zone, Net, Node, ToFlag);
                  if ((fp = fopen (Name, "rt")) != NULL) {
                     while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                        if ((Text = (CHAR *)Attach.First ()) != NULL)
                           do {
                              if (!stricmp (Text, Temp))
                                 break;
                           } while ((Text = (CHAR *)Attach.Next ()) != NULL);
                        if (Text == NULL)
                           Attach.Add (Temp);
                     }
                     fclose (fp);
                     unlink (Name);
                  }

                  // Scrive il nuovo file attach.
                  if ((fp = fopen (Name, "wt")) != NULL) {
                     if ((Text = (CHAR *)Attach.First ()) != NULL)
                        do {
                           fprintf (fp, "%s", Text);
                        } while ((Text = (CHAR *)Attach.Next ()) != NULL);
                     fclose (fp);
                  }
               }
            }
            closedir (dir);
         }
      }
      else {
         // Legge i file attach da cambiare di stato.
         Attach.Clear ();
         if (Zone == 0 || Cfg->MailAddress.Zone == Zone) {
            if (Point != 0)
               sprintf (Name, "%s\\%04x%04x.pnt\\%08x.%clo", Outbound, Net, Node, Point, FromFlag);
            else
               sprintf (Name, "%s\\%04x%04x.%clo", Outbound, Net, Node, FromFlag);
         }
         else {
            if (Point != 0)
               sprintf (Name, "%s.%03x\\%04x%04x.pnt\\%08x.%clo", Outbound, Zone, Net, Node, Point, FromFlag);
            else
               sprintf (Name, "%s.%03x\\%04x%04x.%clo", Outbound, Zone, Net, Node, FromFlag);
         }
         if ((fp = fopen (Name, "rt")) != NULL) {
            if (Log != NULL)
               Log->Write (":Changing %s from %s to %s", Addr.String, FromStr, ToStr);

            while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL)
               Attach.Add (Temp);
            fclose (fp);
            unlink (Name);

            // Legge il file attach che ricevera' gli attachment da cambiare
            // di stato controllando i duplicati.
            if (Zone == 0 || Cfg->MailAddress.Zone == Zone) {
               if (Point != 0)
                  sprintf (Name, "%s\\%04x%04x.pnt\\%08x.%clo", Outbound, Net, Node, Point, ToFlag);
               else
                  sprintf (Name, "%s\\%04x%04x.%clo", Outbound, Net, Node, ToFlag);
            }
            else {
               if (Point != 0)
                  sprintf (Name, "%s.%03x\\%04x%04x.pnt\\%08x.%clo", Outbound, Zone, Net, Node, Point, ToFlag);
               else
                  sprintf (Name, "%s.%03x\\%04x%04x.%clo", Outbound, Zone, Net, Node, ToFlag);
            }
            if ((fp = fopen (Name, "rt")) != NULL) {
               while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                  if ((Text = (CHAR *)Attach.First ()) != NULL)
                     do {
                        if (!stricmp (Text, Temp))
                           break;
                     } while ((Text = (CHAR *)Attach.Next ()) != NULL);
                  if (Text == NULL)
                     Attach.Add (Temp);
               }
               fclose (fp);
               unlink (Name);
            }

            // Scrive il nuovo file attach.
            if ((fp = fopen (Name, "wt")) != NULL) {
               if ((Text = (CHAR *)Attach.First ()) != NULL)
                  do {
                     fprintf (fp, "%s", Text);
                  } while ((Text = (CHAR *)Attach.Next ()) != NULL);
               fclose (fp);
            }
         }
      }
   }
}

VOID TMailProcessor::RouteTo (VOID)
{
   DIR *dir;
   int fd;
   USHORT Zone, Net, Node, Point, DoPack, CheckNet, MakeArc = TRUE;
   CHAR *p, Flag, Lookup[32], DestPath[128], FlagStr[32];
   ULONG TotalSize;
   PKT2HDR pktHdr;
   class TAddress Addr, DestAddr;
   class TPacker *Packer;
   class TNodes *Nodes;
   struct dirent *ent;
   struct stat statbuf;

   Cfg->MailAddress.First ();
   Zone = Cfg->MailAddress.Zone;
   Net = Cfg->MailAddress.Net;
   Node = Cfg->MailAddress.Node;

   DoPack = FALSE;
   DestPath[0] = '\0';
   TotalSize = 0L;

   Flag = 'H';
   if ((p = strtok (NULL, " ")) != NULL) {
      if (!stricmp (p, "hold") || !stricmp (p, "crash") || !stricmp (p, "direct") || !stricmp (p, "normal")) {
         if ((Flag = (CHAR)toupper (*p)) == 'N')
            Flag = 'F';
      }
      strcpy (FlagStr, strupr (p));
   }

   if ((p = strtok (NULL, "")) != NULL)
      strcpy (Line, p);

   if ((p = strtok (Line, " ")) != NULL) {
      Addr.Parse (p);
      if ((p = strtok (NULL, "")) != NULL)
         strcpy (Line, p);
      else
         Line[0] = '\0';

      Point = 0;
      if (Addr.Zone != 0)
         Zone = Addr.Zone;
      if (Addr.Net != 0)
         Net = Addr.Net;
      Node = Addr.Node;
      Point = Addr.Point;

      if (Zone == 0 || Cfg->MailAddress.Zone == Zone) {
         if (Point != 0)
            sprintf (DestPath, "%s\\%04x%04x.pnt\\", Outbound, Net, Node);
         else
            sprintf (DestPath, "%s\\", Outbound);
      }
      else {
         if (Point != 0)
            sprintf (DestPath, "%s.%03x\\%04x%04x.pnt\\", Outbound, Zone, Net, Node);
         else
            sprintf (DestPath, "%s.%03x\\", Outbound, Zone);
      }

      AdjustPath (DestPath);
      DestAddr.Add (Zone, Net, Node, Point);
      DestAddr.First ();

      if (Point != 0)
         sprintf (Temp, "%s%08x.xpr", DestPath, Point);
      else
         sprintf (Temp, "%s%04x%04x.xpr", DestPath, Net, Node);

      if ((fd = sopen (Temp, O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         close (fd);

         if (MakeArc == TRUE) {
            MakeArcMailName (DestAddr.String, Flag);
            MakeArc = FALSE;
         }

         do {
            sprintf (Name, "%s%08lx.pkt", DestPath, time (NULL));
         } while (rename (Temp, Name) != 0);

         if (stat (Name, &statbuf) == 0)
            TotalSize += statbuf.st_size;
         DoPack = TRUE;
      }
   }

   while ((p = strtok (Line, " ")) != NULL) {
      Addr.Parse (p);
      if ((p = strtok (NULL, "")) != NULL)
         strcpy (Line, p);
      else
         Line[0] = '\0';

      if (Addr.Zone != 0)
         Zone = Addr.Zone;
      if (Addr.Net != 0)
         Net = Addr.Net;
      Node = Addr.Node;
      Point = Addr.Point;

      // Controlla se e' stato comando di compattare la posta di
      // tutti i point (send-to 2:332/402.All).
      if (Point == 65535U && Net != 65535U && Node != 65535U) {
         if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
            sprintf (Temp, "%s\\%04x%04x.pnt", Outbound, Net, Node);
         else
            sprintf (Temp, "%s.%03x\\%04x%04x.pnt", Outbound, Zone, Net, Node);
         AdjustPath (Temp);
         if ((dir = opendir (Temp)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
               strlwr (ent->d_name);
               if (strstr (ent->d_name, ".xpr") != NULL) {
                  sscanf (ent->d_name, "%08hx", &Point);

                  if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                     sprintf (Temp, "%s\\%04x%04x.pnt\\%08lx.xpr", Outbound, Net, Node, Point);
                  else
                     sprintf (Temp, "%s.%03x\\%04x%04x.pnt\\%08lx.xpr", Outbound, Zone, Net, Node, Point);

                  AdjustPath (Temp);
                  if ((fd = sopen (Temp, O_RDWR|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                     if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                        if (Nodes->Read (Zone, Net, Node, Point) == TRUE) {
                           read (fd, &pktHdr, sizeof (pktHdr));
                           pktHdr.DestZone = pktHdr.DestZone2 = DestAddr.Zone;
                           pktHdr.DestNet = DestAddr.Net;
                           pktHdr.DestNode = DestAddr.Node;
                           pktHdr.DestPoint = DestAddr.Point;
                           memcpy (pktHdr.Password, Nodes->OutPktPwd, strlen (Nodes->OutPktPwd));
                           lseek (fd, 0L, SEEK_SET);
                           write (fd, &pktHdr, sizeof (pktHdr));
                        }
                        delete Nodes;
                        Nodes = NULL;
                     }
                     close (fd);
                     if (MakeArc == TRUE) {
                        MakeArcMailName (DestAddr.String, Flag);
                        MakeArc = FALSE;
                     }
                     do {
                        sprintf (Name, "%s%08lx.pkt", DestPath, time (NULL));
                     } while (rename (Temp, Name) != 0);

                     if (stat (Name, &statbuf) == 0)
                        TotalSize += statbuf.st_size;
                     DoPack = TRUE;
                  }
               }
            }
            closedir (dir);
         }
      }
      // Controlla se e' stato comando di compattare la posta di
      // tutta una zona (send-to all 2:All) o di un solo net (send-to 2:332/All).
      else if (Net == 65535U || Node == 65535U) {
         CheckNet = (Net != 65535U) ? TRUE : FALSE;
         sprintf (Lookup, "%04x", Net);
         if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
            sprintf (Temp, "%s", Outbound);
         else
            sprintf (Temp, "%s.%03x", Outbound, Zone);
         AdjustPath (Temp);
         if ((dir = opendir (Temp)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
               strlwr (ent->d_name);
               if (strstr (ent->d_name, ".xpr") != NULL) {
                  DoPack = FALSE;
                  if (CheckNet == FALSE) {
                     sscanf (ent->d_name, "%04hx%04hx", &Net, &Node);
                     DoPack = TRUE;
                  }
                  else if (!strncmp (ent->d_name, Lookup, strlen (Lookup))) {
                     sscanf (ent->d_name, "%04hx%04hx", &Net, &Node);
                     DoPack = TRUE;
                  }

                  if (DoPack == TRUE) {
                     if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                        sprintf (Temp, "%s\\%04x%04x.xpr", Outbound, Net, Node);
                     else
                        sprintf (Temp, "%s.%03x\\%04x%04x.xpr", Outbound, Zone, Net, Node);

                     AdjustPath (Temp);
                     if ((fd = sopen (Temp, O_RDWR|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                        if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                           if (Nodes->Read (Zone, Net, Node, Point) == TRUE) {
                              read (fd, &pktHdr, sizeof (pktHdr));
                              pktHdr.DestZone = pktHdr.DestZone2 = DestAddr.Zone;
                              pktHdr.DestNet = DestAddr.Net;
                              pktHdr.DestNode = DestAddr.Node;
                              pktHdr.DestPoint = DestAddr.Point;
                              memcpy (pktHdr.Password, Nodes->OutPktPwd, strlen (Nodes->OutPktPwd));
                              lseek (fd, 0L, SEEK_SET);
                              write (fd, &pktHdr, sizeof (pktHdr));
                           }
                           delete Nodes;
                           Nodes = NULL;
                        }
                        close (fd);
                        if (MakeArc == TRUE) {
                           MakeArcMailName (DestAddr.String, Flag);
                           MakeArc = FALSE;
                        }
                        do {
                           sprintf (Name, "%s%08lx.pkt", DestPath, time (NULL));
                        } while (rename (Temp, Name) != 0);

                        if (stat (Name, &statbuf) == 0)
                           TotalSize += statbuf.st_size;
                     }
                  }
               }
            }
            closedir (dir);
         }
      }
      else {
         if (Zone == 0 || Cfg->MailAddress.Zone == Zone) {
            if (Point != 0)
               sprintf (Temp, "%s\\%04x%04x.pnt\\%08x.xpr", Outbound, Net, Node, Point);
            else
               sprintf (Temp, "%s\\%04x%04x.xpr", Outbound, Net, Node);
         }
         else {
            if (Point != 0)
               sprintf (Temp, "%s.%03x\\%04x%04x.pnt\\%08x.xpr", Outbound, Zone, Net, Node, Point);
            else
               sprintf (Temp, "%s.%03x\\%04x%04x.xpr", Outbound, Zone, Net, Node);
         }

         AdjustPath (Temp);
         if ((fd = sopen (Temp, O_RDWR|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
            if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
               if (Nodes->Read (Zone, Net, Node, Point) == TRUE) {
                  read (fd, &pktHdr, sizeof (pktHdr));
                  pktHdr.DestZone = pktHdr.DestZone2 = DestAddr.Zone;
                  pktHdr.DestNet = DestAddr.Net;
                  pktHdr.DestNode = DestAddr.Node;
                  pktHdr.DestPoint = DestAddr.Point;
                  memcpy (pktHdr.Password, Nodes->OutPktPwd, strlen (Nodes->OutPktPwd));
                  lseek (fd, 0L, SEEK_SET);
                  write (fd, &pktHdr, sizeof (pktHdr));
               }
               delete Nodes;
               Nodes = NULL;
            }
            close (fd);
            if (MakeArc == TRUE) {
               MakeArcMailName (DestAddr.String, Flag);
               MakeArc = FALSE;
            }
            do {
               sprintf (Name, "%s%08lx.pkt", DestPath, time (NULL));
            } while (rename (Temp, Name) != 0);

            if (stat (Name, &statbuf) == 0)
               TotalSize += statbuf.st_size;
            DoPack = TRUE;
         }
      }
   }

   if (DoPack == TRUE) {
      MakeArcMailName (DestAddr.String, Flag);
      sprintf (Name, "%s*.pkt", DestPath);

      if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
         Packer->First ();
         if (Packer->CheckArc (ArcMailName) == FALSE) {
            if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
               if (Nodes->Read (DestAddr.String) == TRUE)
                  Packer->Read (Nodes->Packer);
               delete Nodes;
            }
         }
         if (Log != NULL)
            Log->Write ("#Packing mail for %s (%lu bytes)", DestAddr.String, TotalSize);
         Packer->DoPack (ArcMailName, Name);
         delete Packer;
      }
   }
}

VOID TMailProcessor::SendTo (VOID)
{
   DIR *dir;
   int fd;
   USHORT Zone, Net, Node, Point, DoPack, CheckNet;
   CHAR *p, Flag, Lookup[32], FlagStr[16];
   class TAddress Addr;
   class TPacker *Packer;
   class TNodes *Nodes;
   struct dirent *ent;
   struct stat statbuf;

   Cfg->MailAddress.First ();
   Zone = Cfg->MailAddress.Zone;
   Net = Cfg->MailAddress.Net;
   Node = Cfg->MailAddress.Node;
   Point = 0;

   Flag = 'H';
   if ((p = strtok (NULL, " ")) != NULL) {
      if (!stricmp (p, "hold") || !stricmp (p, "crash") || !stricmp (p, "direct") || !stricmp (p, "normal")) {
         if ((Flag = (CHAR)toupper (*p)) == 'N')
            Flag = 'F';
      }
      strcpy (FlagStr, strupr (p));
   }

   if ((p = strtok (NULL, "")) != NULL)
      strcpy (Line, p);

   while ((p = strtok (Line, " ")) != NULL) {
      Addr.Parse (p);
      if ((p = strtok (NULL, "")) != NULL)
         strcpy (Line, p);
      else
         Line[0] = '\0';

      if (Addr.Zone != 0)
         Zone = Addr.Zone;
      if (Addr.Net != 0)
         Net = Addr.Net;
      if (Addr.Node != 0)
         Node = Addr.Node;
      Point = Addr.Point;

      // Controlla se e' stato comando di compattare la posta di
      // tutti i point (send-to 2:332/402.All).
      if (Point == 65535U && Net != 65535U && Node != 65535U) {
         if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
            sprintf (Temp, "%s\\%04x%04x.pnt", Outbound, Net, Node);
         else
            sprintf (Temp, "%s.%03x\\%04x%04x.pnt", Outbound, Zone, Net, Node);
         if ((dir = opendir (AdjustPath (Temp))) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
               strlwr (ent->d_name);
               if (strstr (ent->d_name, ".xpr") != NULL) {
                  sscanf (ent->d_name, "%08hx", &Point);

                  if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                     sprintf (Temp, "%s\\%04x%04x.pnt\\%08x.xpr", Outbound, Net, Node, Point);
                  else
                     sprintf (Temp, "%s.%03x\\%04x%04x.pnt\\%08x.xpr", Outbound, Zone, Net, Node, Point);

                  if ((fd = sopen (AdjustPath (Temp), O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                     close (fd);
                     do {
                        if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                           sprintf (Name, "%s\\%04x%04x.pnt\\%08lx.pkt", Outbound, Net, Node, time (NULL));
                        else
                           sprintf (Name, "%s.%03x\\%04x%04x.pnt\\%08lx.pkt", Outbound, Zone, Net, Node, time (NULL));
                     } while (rename (Temp, AdjustPath (Name)) != 0);

                     sprintf (Addr.String, "%u:%u/%u.%u", Zone, Net, Node, Point);
                     MakeArcMailName (Addr.String, Flag);

                     if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
                        Packer->First ();
                        if (Packer->CheckArc (ArcMailName) == FALSE) {
                           if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                              if (Nodes->Read (Addr.String) == TRUE)
                                 Packer->Read (Nodes->Packer);
                              delete Nodes;
                           }
                        }
                        stat (Name, &statbuf);
                        if (Log != NULL)
                           Log->Write ("#Packing mail for %s (%lu bytes)", Addr.String, statbuf.st_size);
                        Packer->DoPack (ArcMailName, Name);
                        delete Packer;
                     }
                  }
               }
            }
            closedir (dir);
         }
      }
      // Controlla se e' stato comando di compattare la posta di
      // tutta una zona (send-to all 2:All) o di un solo net (send-to 2:332/All).
      else if (Net == 65535U || Node == 65535U) {
         CheckNet = (Net != 65535U) ? TRUE : FALSE;
         sprintf (Lookup, "%04x", Net);
         if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
            sprintf (Temp, "%s", Outbound);
         else
            sprintf (Temp, "%s.%03x", Outbound, Zone);
         if ((dir = opendir (AdjustPath (Temp))) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
               strlwr (ent->d_name);
               if (strstr (ent->d_name, ".xpr") != NULL) {
                  DoPack = FALSE;
                  if (CheckNet == FALSE) {
                     sscanf (ent->d_name, "%04hx%04hx", &Net, &Node);
                     DoPack = TRUE;
                  }
                  else if (!strncmp (ent->d_name, Lookup, strlen (Lookup))) {
                     sscanf (ent->d_name, "%04hx%04hx", &Net, &Node);
                     DoPack = TRUE;
                  }

                  if (DoPack == TRUE) {
                     if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                        sprintf (Temp, "%s\\%04x%04x.xpr", Outbound, Net, Node);
                     else
                        sprintf (Temp, "%s.%03x\\%04x%04x.xpr", Outbound, Zone, Net, Node);

                     if ((fd = sopen (AdjustPath (Temp), O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                        close (fd);
                        do {
                           if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                              sprintf (Name, "%s\\%08lx.pkt", Outbound, time (NULL));
                           else
                              sprintf (Name, "%s.%03x\\%08lx.pkt", Outbound, Zone, time (NULL));
                        } while (rename (Temp, AdjustPath (Name)) != 0);

                        sprintf (Addr.String, "%u:%u/%u.%u", Zone, Net, Node, Point);
                        MakeArcMailName (Addr.String, Flag);

                        if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
                           Packer->First ();
                           if (Packer->CheckArc (ArcMailName) == FALSE) {
                              if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                                 if (Nodes->Read (Addr.String) == TRUE)
                                    Packer->Read (Nodes->Packer);
                                 delete Nodes;
                              }
                           }
                           stat (Name, &statbuf);
                           if (Log != NULL)
                              Log->Write ("#Packing mail for %s (%lu bytes)", Addr.String, statbuf.st_size);
                           Packer->DoPack (ArcMailName, Name);
                           delete Packer;
                        }
                     }
                  }
               }
            }
            closedir (dir);
         }
      }
      else {
         if (Zone == 0 || Cfg->MailAddress.Zone == Zone) {
            if (Point != 0)
               sprintf (Temp, "%s\\%04x%04x.pnt\\%08x.xpr", Outbound, Net, Node, Point);
            else
               sprintf (Temp, "%s\\%04x%04x.xpr", Outbound, Net, Node);
         }
         else {
            if (Point != 0)
               sprintf (Temp, "%s.%03x\\%04x%04x.pnt\\%08x.xpr", Outbound, Zone, Net, Node, Point);
            else
               sprintf (Temp, "%s.%03x\\%04x%04x.xpr", Outbound, Zone, Net, Node);
         }

         if ((fd = sopen (AdjustPath (Temp), O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
            close (fd);
            do {
               if (Zone == 0 || Cfg->MailAddress.Zone == Zone) {
                  if (Point != 0)
                     sprintf (Name, "%s\\%04x%04x.pnt\\%08lx.pkt", Outbound, Net, Node, time (NULL));
                  else
                     sprintf (Name, "%s\\%08lx.pkt", Outbound, time (NULL));
               }
               else {
                  if (Point != 0)
                     sprintf (Name, "%s.%03x\\%04x%04x.pnt\\%08lx.pkt", Outbound, Zone, Net, Node, time (NULL));
                  else
                     sprintf (Name, "%s.%03x\\%08lx.pkt", Outbound, Zone, time (NULL));
               }
            } while (rename (Temp, AdjustPath (Name)) != 0);

            sprintf (Addr.String, "%u:%u/%u.%u", Zone, Net, Node, Point);
            MakeArcMailName (Addr.String, Flag);

            if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
               Packer->First ();
               if (Packer->CheckArc (ArcMailName) == FALSE) {
                  if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                     if (Nodes->Read (Addr.String) == TRUE)
                        Packer->Read (Nodes->Packer);
                     delete Nodes;
                  }
               }
               stat (Name, &statbuf);
               if (Log != NULL)
                  Log->Write ("#Packing mail for %s (%lu bytes)", Addr.String, statbuf.st_size);
               Packer->DoPack (ArcMailName, Name);
               delete Packer;
            }
         }
      }
   }
}

VOID TMailProcessor::Pack (PSZ pszFile)
{
   FILE *fp;
   USHORT LineNum, DoRoute;
   CHAR *p;
   class TPacker *Packer;

   strcpy (Outbound, Cfg->Outbound);
   if (Outbound[strlen (Outbound) - 1] == '\\' || Outbound[strlen (Outbound) - 1] == '/')
      Outbound[strlen (Outbound) - 1] = '\0';

   if (Log != NULL)
      Log->Write (":Pack outbound mail (%s)", pszFile);

   DoRoute = FALSE;
   if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
      if (Packer->First () == TRUE)
         DoRoute = TRUE;
      delete Packer;
   }

   if (DoRoute == TRUE) {
      if ((fp = _fsopen (pszFile, "rt", SH_DENYNO)) != NULL) {
         LineNum = 1;
         while (fgets (Line, sizeof (Line) - 1, fp) != NULL) {
            if ((p = strchr (Line, '\n')) != NULL)
               *p = '\0';
            if ((p = strtok (Line, " ")) != NULL) {
               if (!stricmp (p, "send-to") || !stricmp (p, "send"))
                  SendTo ();
               else if (!stricmp (p, "route-to") || !stricmp (p, "route"))
                  RouteTo ();
               else if (!stricmp (p, "change"))
                  Change ();
               else if (*p != '\0' && *p != ';' && *p != '%')
                  Log->Write ("!Unknown keyword '%s' on line #%d", p, LineNum);
            }

            LineNum++;
         }
         fclose (fp);
      }
   }
   else {
      if (Log != NULL)
         Log->Write ("!No compressor(s) defined");
   }
}

