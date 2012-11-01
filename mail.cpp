
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.11
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/17/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "mail.h"

PSZ CheckExts[] = { ".su", ".mo", ".tu", ".we", ".th", ".fr", ".sa", NULL };

TKludges::TKludges (void)
{
   Data.Clear ();
   Sort = TRUE;
}

TKludges::~TKludges (void)
{
   Data.Clear ();
}

USHORT TKludges::Add (VOID)
{
   USHORT Insert = FALSE;
   CHAR Temp[64];
   KLUDGES Buffer, *Current;

   sprintf (Temp, "%u:%u/%u.%u", Zone, Net, Node, Point);

   if (Check (Temp) == FALSE) {
      memset (&Buffer, 0, sizeof (KLUDGES));

      Buffer.Zone = Zone;
      Buffer.Net = Net;
      Buffer.Node = Node;
      Buffer.Point = Point;

      if (Sort == TRUE) {
         if ((Current = (KLUDGES *)Data.First ()) != NULL) {
            if (Current->Net > Net)
               Insert = TRUE;
            else if (Current->Net == Net && Current->Node > Node)
               Insert = TRUE;
            else if (Current->Net == Net && Current->Node == Node && Current->Point > Point)
               Insert = TRUE;

            if (Insert == TRUE) {
               Data.Insert (&Buffer, sizeof (KLUDGES));
               Data.Insert (Current, sizeof (KLUDGES));
               Data.First ();
               Data.Remove ();
               Data.First ();
            }
            else {
               while ((Current = (KLUDGES *)Data.Next ()) != NULL) {
                  if (Current->Net > Net)
                     Insert = TRUE;
                  else if (Current->Net == Net && Current->Node > Node)
                     Insert = TRUE;
                  else if (Current->Net == Net && Current->Node == Node && Current->Point > Point)
                     Insert = TRUE;

                  if (Insert == TRUE) {
                     Data.Previous ();
                     Data.Insert (&Buffer, sizeof (KLUDGES));
                     break;
                  }
               }
               if (Insert == FALSE) {
                  Data.Add (&Buffer, sizeof (KLUDGES));
                  Insert = TRUE;
               }
            }
         }
         else {
            if (Insert == FALSE) {
               Data.Add (&Buffer, sizeof (KLUDGES));
               Insert = TRUE;
            }
         }
      }
      else
         Data.Add (&Buffer, sizeof (KLUDGES));
   }

   return (Insert);
}

USHORT TKludges::AddString (PSZ pszString)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], *p;
   class TAddress Address;

   strcpy (Temp, pszString);
   if ((p = strtok (Temp, " ")) != NULL)
      do {
         if (Check (p) == FALSE) {
            Address.Parse (p);
            Zone = Address.Zone;
            if (Address.Net != 0)
               Net = Address.Net;
            if (Address.Node != 0)
               Node = Address.Node;
            Point = Address.Point;
            RetVal = Add ();
         }
      } while ((p = strtok (NULL, " ")) != NULL);

   return (RetVal);
}

USHORT TKludges::Check (PSZ pszAddress)
{
   USHORT RetVal = FALSE;
   KLUDGES *El;
   class TAddress Addr;

   Addr.Parse (pszAddress);

   if ((El = (KLUDGES *)Data.First ()) != NULL)
      do {
         if (El->Net == Addr.Net && El->Node == Addr.Node && El->Point == Addr.Point) {
            Zone = El->Zone;
            Net = El->Net;
            Node = El->Node;
            Point = El->Point;
            sprintf (Address, "%u/%u", Net, Node);
            RetVal = TRUE;
            break;
         }
      } while ((El = (KLUDGES *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TKludges::Clear (VOID)
{
   Data.Clear ();
   New ();
}

VOID TKludges::Delete (VOID)
{
   if (Data.Value () != NULL)
      Data.Remove ();
}

USHORT TKludges::First (VOID)
{
   USHORT RetVal = FALSE;
   KLUDGES *El;

   if ((El = (KLUDGES *)Data.First ()) != NULL) {
      Zone = El->Zone;
      Net = El->Net;
      Node = El->Node;
      Point = El->Point;
      if (Point != 0)
         sprintf (Address, "%u/%u.%u", Net, Node, Point);
      else
         sprintf (Address, "%u/%u", Net, Node);
      strcpy (ShortAddress, Address);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TKludges::New (VOID)
{
   Zone = 0;
   Net = 0;
   Node = 0;
   Point = 0;
}

USHORT TKludges::Next (VOID)
{
   USHORT RetVal = FALSE;
   KLUDGES *El;

   if ((El = (KLUDGES *)Data.Next ()) != NULL) {
      if (Net != El->Net) {
         if (El->Point != 0)
            sprintf (ShortAddress, "%u/%u.%u", El->Net, El->Node, El->Point);
         else
            sprintf (ShortAddress, "%u/%u", El->Net, El->Node);
      }
      else {
         if (Node == El->Node && El->Point != 0)
            sprintf (ShortAddress, ".%u", El->Point);
         else {
            if (El->Point != 0)
               sprintf (ShortAddress, "%u.%u", El->Point);
            else
               sprintf (ShortAddress, "%u", El->Node);
         }
      }

      Zone = El->Zone;
      Net = El->Net;
      Node = El->Node;
      Point = El->Point;
      if (Point != 0)
         sprintf (Address, "%u/%u.%u", Net, Node, Point);
      else
         sprintf (Address, "%u/%u", Net, Node);
      RetVal = TRUE;
   }

   return (RetVal);
}

// ----------------------------------------------------------------------

TMail::TMail (void)
{
#if defined(__LINUX__)
   strcpy (Inbound, "./");
#else
   strcpy (Inbound, ".\\");
#endif
   Log = NULL;

   Msg = NULL;
   Packet = NULL;
   PktName[0] = LastTag[0] = '\0';
   MsgTossed = Duplicate = Bad = NetMail = 0L;
   Packets = 0;
}

TMail::~TMail (void)
{
   if (Msg != NULL) {
      Msg->UnLock ();
      delete Msg;
      Msg = NULL;
   }
   if (Packet != NULL) {
      delete Packet;
      Packet = NULL;
   }
}

USHORT TMail::OpenNextPacket (VOID)
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

// We have found a valid packet. To avoid system lockups due to an invalid
// packet file we now rename the file to something that prevents the same
// file to be read two times.
      sprintf (Filename, "%s%s", Inbound, PktName);
      sprintf (OpenFileName, "%sbad_pkt.%03d", Inbound, ++MaxBad);
      rename (Filename, OpenFileName);

      if (Packet != NULL)
         delete Packet;

      if ((Packet = new PACKET) != NULL) {
         if (Packet->Open (OpenFileName, FALSE) == FALSE) {
            delete Packet;
            Packet = NULL;
            RetVal = FALSE;
         }
      }
   }

   return (RetVal);
}

USHORT TMail::ExportEchoMail (ULONG Number, PSZ pszEchoTag)
{
   USHORT SentArea = 0;
   CHAR *Text;
   class TAddress ToAddr;

   if (Msg != NULL) {
      if (Msg->Read (Number, MAX_LINE_LENGTH) == TRUE) {
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

            if (Forward->First () == TRUE)
               do {
                  Forward->Skip = FALSE;
                  Forward->Update ();
               } while (Forward->Next () == TRUE);

            if (Forward->First () == TRUE)
               do {
                  if (SeenBy->Check (Forward->Address) == TRUE) {
                     Forward->Skip = TRUE;
                     Forward->Update ();
                  }
                  else
                     SeenBy->AddString (Forward->Address);
               } while (Forward->Next () == TRUE);

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

         if (Forward->First () == TRUE)
            do {
               if (Forward->Skip == FALSE) {
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

                     SentArea++;
                  }
               }
            } while (Forward->Next () == TRUE);

         if (Dupes->Check (Data->EchoTag, Msg) == FALSE)
            Dupes->Add (Data->EchoTag, Msg);
      }
   }

   return (SentArea);
}

VOID TMail::Export (VOID)
{
   USHORT i;
   ULONG Number, SentArea;

   Data = new TMsgData (Cfg->SystemPath);
   Forward = new TEchoLink (Cfg->SystemPath);
   Dupes = new TDupes (Cfg->SystemPath);

   strcpy (Outbound, Cfg->Outbound);
   if (Outbound[strlen (Outbound) - 1] == '\\' || Outbound[strlen (Outbound) - 1] == '/')
      Outbound[strlen (Outbound) - 1] = '\0';

   if (Log != NULL)
      Log->Write ("#Scanning messages");

   SentArea = MsgSent = 0L;

   if (Data != NULL && Forward != NULL) {
      if (Data->First () == TRUE)
         do {
            if (Data->EchoMail == TRUE && Data->EchoTag[0] != '\0') {
               Dupes->Load (Data->EchoTag);

               Msg = NULL;
               if (Data->Storage == ST_JAM)
                  Msg = new JAM (Data->Path);
               else if (Data->Storage == ST_SQUISH)
                  Msg = new SQUISH (Data->Path);
               else if (Data->Storage == ST_FIDO)
                  Msg = new FIDOSDM (Data->Path);
               else if (Data->Storage == ST_ADEPT)
                  Msg = new ADEPT (Data->Path);

               if (Msg != NULL) {
                  Msg->Lock (0L);
                  memset (Temp, '.', sizeof (Temp));
                  memcpy (Temp, Data->EchoTag, strlen (Data->EchoTag));
                  Temp[strlen (Data->EchoTag)] = ' ';
                  cprintf ("Scanning %-40.40s ", Temp);

                  Forward->Load (Data->EchoTag);
                  if (Forward->First () == TRUE) {
                     if (Msg->GetHWM (Number) == FALSE)
                        Number = Data->HighWaterMark;

                     cprintf ("#%06lu to #%06lu ", Number, Msg->Highest ());

                     SentArea = 0L;
                     if (Msg->Next (Number) == TRUE) {
                        do {
                           cprintf ("<#%06lu>\b\b\b\b\b\b\b\b\b", Number);
                           i = ExportEchoMail (Number, Data->EchoTag);
                           SentArea += i;
                           MsgSent += i;
                        } while (Msg->Next (Number) == TRUE);

                        Msg->SetHWM (Number);
                        Data->HighWaterMark = Number;
                        Data->Update ();

                        if (Log != NULL && SentArea != 0L)
                           Log->Write (":   %-30.30s (Sent=%04lu)", Data->EchoTag, SentArea);
                     }
                  }
                  else {
                     Number = Msg->Highest ();
                     Msg->SetHWM (Number);
                     Data->HighWaterMark = Number;
                     Data->Update ();
                  }

                  Msg->UnLock ();
                  delete Msg;
                  cprintf ("\r\n");
               }

               Dupes->Save ();
            }
         } while (Data->Next () == TRUE);
   }

   Msg = NULL;
   Packet = NULL;

   if (Dupes != NULL)
      delete Dupes;
   if (Forward != NULL)
      delete Forward;
   if (Data != NULL)
      delete Data;

   cprintf ("\r\n");
}

VOID TMail::ExportNetMail (VOID)
{
   USHORT OurPoint, FoundIntl;
   CHAR *Flag, *Text;
   ULONG Number, Sent;
   class TAddress Address, FromAddr, ToAddr;
   class TAreaMgr *AreaMgr;

   strcpy (Outbound, Cfg->Outbound);
   if (Outbound[strlen (Outbound) - 1] == '\\' || Outbound[strlen (Outbound) - 1] == '/')
      Outbound[strlen (Outbound) - 1] = '\0';

   cprintf ("Searching path '%s'...\r\n", Cfg->NetMailPath);

   Msg = NULL;
   if (Cfg->NetMailStorage == ST_JAM)
      Msg = new JAM (Cfg->NetMailPath);
   else if (Cfg->NetMailStorage == ST_SQUISH)
      Msg = new SQUISH (Cfg->NetMailPath);
   else if (Cfg->NetMailStorage == ST_FIDO)
      Msg = new FIDOSDM (Cfg->NetMailPath);
   else if (Cfg->NetMailStorage == ST_ADEPT)
      Msg = new ADEPT (Cfg->NetMailPath);

   if ((AreaMgr = new TAreaMgr) != NULL) {
      AreaMgr->Cfg = Cfg;
      AreaMgr->Log = Log;
   }

   if (Msg != NULL) {
      Msg->Lock (0L);
      Sent = 0L;
      if (Log != NULL)
         Log->Write ("#Packing from %s (%lu msgs)", Cfg->NetMailPath, Msg->Number ());
      Number = Msg->Lowest ();
      do {
         if (Msg->Read (Number, MAX_LINE_LENGTH) == TRUE) {
            Address.Clear ();
            Address.Parse (Msg->ToAddress);

            FoundIntl = FALSE;
            if ((Text = (PSZ)Msg->Text.First ()) != NULL)
               do {
                  if (!strncmp (Text, "\001INTL ", 6)) {
                     FoundIntl = TRUE;
                     break;
                  }
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
                        if (Cfg->MailAddress.Point == Address.Point)
                           Msg->Sent = TRUE;
                     }
                  }
               } while (Cfg->MailAddress.Next () == TRUE);

            if (Msg->Sent == FALSE) {
               // Se non si tratta di un nostro point, il messaggio viene indirizzato
               // sempre al boss, cioe' all'indirizzo con point = 0
               if (OurPoint == FALSE) {
                  Address.Point = 0;
                  Address.Add ();
                  Address.First ();
               }
               else {
                  if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                     if (Nodes->First () == TRUE)
                        do {
                           if (Nodes->Zone == Address.Zone && !stricmp (Msg->To, Nodes->SysopName) && Nodes->RemapMail == TRUE) {
                              if (Log != NULL)
                                 Log->Write ("#  Remap %s => %s", Address.String, Nodes->Address);
                              sprintf (Temp, "\001PointMap %s => %s", Address.String, Nodes->Address);
                              Msg->Text.Add (Temp);
                              Address.Parse (Nodes->Address);
                              OurPoint = FALSE;
                              break;
                           }
                        } while (Nodes->Next () == TRUE);
                     delete Nodes;
                     Nodes = NULL;
                  }
               }

               Flag = "NORMAL";
               if (Msg->Crash == TRUE)
                  Flag = "CRASH";
               else if (Msg->Direct == TRUE)
                  Flag = "DIRECT";
               else if (Msg->Hold == TRUE)
                  Flag = "HOLD";
//               sprintf (Temp, "Sending: #%06lu to %s (%s)", Number, Address.String, Flag);
//               cprintf ("%-52.52s\r", Temp);
               cprintf ("Sending: #%06lu to %s (%s)\r\n", Number, Msg->ToAddress, Flag);

               Cfg->MailAddress.First ();
               if (Address.Zone == 0 || Cfg->MailAddress.Zone == Address.Zone) {
                  if (Address.Point != 0) {
#if defined(__LINUX__)
                     sprintf (Temp, "%s/%04x%04x.pnt", Outbound, Address.Net, Address.Node);
                     mkdir (Temp, 0666);
                     sprintf (Temp, "%s/%04x%04x.pnt/%08x.xpr", Outbound, Address.Net, Address.Node, Address.Point);
#else
                     sprintf (Temp, "%s\\%04x%04x.pnt", Outbound, Address.Net, Address.Node);
                     mkdir (Temp);
                     sprintf (Temp, "%s\\%04x%04x.pnt\\%08x.xpr", Outbound, Address.Net, Address.Node, Address.Point);
#endif
                  }
                  else
#if defined(__LINUX__)
                     sprintf (Temp, "%s/%04x%04x.xpr", Outbound, Address.Net, Address.Node);
#else
                     sprintf (Temp, "%s\\%04x%04x.xpr", Outbound, Address.Net, Address.Node);
#endif
               }
               else {
                  sprintf (Temp, "%s.%03x", Outbound, Address.Zone);
#if defined(__LINUX__)
                  mkdir (Temp, 0666);
#else
                  mkdir (Temp);
#endif
                  if (Address.Point != 0) {
#if defined(__LINUX__)
                     sprintf (Temp, "%s.%03x/%04x%04x.pnt", Outbound, Address.Zone, Address.Net, Address.Node);
                     mkdir (Temp, 0666);
                     sprintf (Temp, "%s.%03x/%04x%04x.pnt/%08x.xpr", Outbound, Address.Zone, Address.Net, Address.Node, Address.Point);
#else
                     sprintf (Temp, "%s.%03x\\%04x%04x.pnt", Outbound, Address.Zone, Address.Net, Address.Node);
                     mkdir (Temp);
                     sprintf (Temp, "%s.%03x\\%04x%04x.pnt\\%08x.xpr", Outbound, Address.Zone, Address.Net, Address.Node, Address.Point);
#endif
                  }
                  else
#if defined(__LINUX__)
                     sprintf (Temp, "%s.%03x/%04x%04x.xpr", Outbound, Address.Zone, Address.Net, Address.Node);
#else
                     sprintf (Temp, "%s.%03x\\%04x%04x.xpr", Outbound, Address.Zone, Address.Net, Address.Node);
#endif
               }

               if (Msg->Crash == TRUE)
                  strcpy (&Temp[strlen (Temp) - 3], "cut");
               else if (Msg->Direct == TRUE)
                  strcpy (&Temp[strlen (Temp) - 3], "dut");
               else if (Msg->Hold == TRUE)
                  strcpy (&Temp[strlen (Temp) - 3], "hut");

               if ((Packet = new PACKET) != NULL) {
                  if (OurPoint == TRUE)
                     strcpy (Packet->ToAddress, Msg->ToAddress);
                  else
                     strcpy (Packet->ToAddress, Address.String);
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
                  else
                     strcpy (Packet->FromAddress, Msg->FromAddress);
                  FromAddr.Parse (Packet->FromAddress);

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
                        if (Nodes->Read (Msg->ToAddress) == TRUE)
                           strcpy (Packet->Password, Nodes->OutPktPwd);
                        delete Nodes;
                        Nodes = NULL;
                     }
                  }

                  // Apre il pacchetto .xpr senza effettuare lo scan dei messaggi
                  if (Packet->Open (Temp, FALSE) == TRUE) {
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
            else if (Msg->Received == FALSE) {
               if (!stricmp (Msg->To, "Areafix") || !stricmp (Msg->To, "AreaMgr")) {
                  if (AreaMgr != NULL) {
                     Msg->Received = TRUE;
                     Msg->WriteHeader (Number);

                     AreaMgr->Msg = Msg;
                     cprintf ("Processing: #%06lu from %s\r\n", Number, Msg->FromAddress);
                     AreaMgr->Process ();
                  }
               }
            }
         }
      } while (Msg->Next (Number) == TRUE);

      if (Log != NULL)
         Log->Write (":  Packed=%lu", Sent);

      Msg->UnLock ();
      delete Msg;
      Msg = NULL;
   }

   if (AreaMgr != NULL)
      delete AreaMgr;

   cprintf ("\r\n");
}

USHORT TMail::OpenArea (PSZ pszEchoTag)
{
   FILE *fp;
   USHORT Found = FALSE, Create;
   CHAR Temp[64], *p, *path, *tag, *links;

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
                     Msg = new SQUISH (++path);
                  else if (*path == '&')
                     Msg = new JAM (++path);
                  else if (*path == '=')
                     Msg = new ADEPT (++path);
                  else
                     Msg = new FIDOSDM (path);
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
                     if (strstr (pszEchoTag, strupr (p)) != NULL) {
                        Create = TRUE;
                        break;
                     }
                  } while ((p = strtok (NULL, " ")) != NULL);
               }
            }
         }
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

         Found = TRUE;

         if (Forward != NULL) {
            Forward->Load (pszEchoTag);
            Forward->AddString (Packet->FromAddress);
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
   }

   if (Msg != NULL)
      Msg->Lock (0L);
   if (Dupes != NULL)
      Dupes->Load (pszEchoTag);

   return (Found);
}

ULONG TMail::ImportEchoMail (VOID)
{
   USHORT BadMessage = FALSE;
   CHAR *Text;
   ULONG RetVal = 0L;
   class TMsgBase *Extra;

   if (Forward != NULL) {
      if (Forward->Check (Packet->FromAddress) == FALSE && Cfg->Secure == TRUE) {
         BadMessage = TRUE;
         Extra = Msg;

         if (Cfg->BadStorage == ST_JAM)
            Msg = new JAM (Cfg->BadPath);
         else if (Cfg->BadStorage == ST_SQUISH)
            Msg = new SQUISH (Cfg->BadPath);
         else if (Cfg->BadStorage == ST_FIDO)
            Msg = new FIDOSDM (Cfg->BadPath);
         else if (Cfg->BadStorage == ST_ADEPT)
            Msg = new ADEPT (Cfg->BadPath);
      }
   }

   if (Dupes != NULL) {
      if (Dupes->Check (Data->EchoTag, Packet) == TRUE) {
         BadMessage = TRUE;
         Extra = Msg;

         if (Cfg->DupeStorage == ST_JAM)
            Msg = new JAM (Cfg->DupePath);
         else if (Cfg->DupeStorage == ST_SQUISH)
            Msg = new SQUISH (Cfg->DupePath);
         else if (Cfg->DupeStorage == ST_FIDO)
            Msg = new FIDOSDM (Cfg->DupePath);
         else if (Cfg->DupeStorage == ST_ADEPT)
            Msg = new ADEPT (Cfg->DupePath);
      }
      else
         Dupes->Add (Data->EchoTag, Packet);
   }

   if (Msg != NULL) {
      if (BadMessage == FALSE) {
         Packet->Text.First ();
         Packet->Text.Remove ();
      }

      Msg->New ();

      SeenBy = new TKludges;
      Path = new TKludges;

      if (SeenBy != NULL && Path != NULL) {
         Path->Sort = FALSE;
         if ((Text = (PSZ)Packet->Text.First ()) != NULL)
            do {
               if (!strncmp (Text, "SEEN-BY: ", 9) && SeenBy != NULL) {
                  SeenBy->AddString (&Text[9]);
                  Packet->Text.Remove ();
                  Text = (PSZ)Packet->Text.Value ();
               }
               else if (!strncmp (Text, "\001PATH: ", 7) && Path != NULL) {
                  Path->AddString (&Text[7]);
                  Packet->Text.Remove ();
                  Text = (PSZ)Packet->Text.Value ();
               }
               else
                  Text = (PSZ)Packet->Text.Next ();
            } while (Text != NULL);

         SeenBy->AddString (Packet->FromAddress);
         Path->AddString (Packet->FromAddress);

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
                  Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                  strcpy (Temp, "SEEN-BY:");
                  strcpy (SeenBy->ShortAddress, SeenBy->Address);
               }
               strcat (Temp, " ");
               strcat (Temp, SeenBy->ShortAddress);
            } while (SeenBy->Next () == TRUE);
            if (strlen (Temp) > 9)
               Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         }

         if (Path->First () == TRUE) {
            strcpy (Temp, "\001PATH:");
            do {
               if (strlen (Temp) + strlen (Path->ShortAddress) + 1 > 70) {
                  Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                  strcpy (Temp, "\001PATH:");
                  strcpy (Path->ShortAddress, Path->Address);
               }
               strcat (Temp, " ");
               strcat (Temp, Path->ShortAddress);
            } while (Path->Next () == TRUE);
            if (strlen (Temp) > 7)
               Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         }
      }

      if (Path != NULL)
         delete Path;
      if (SeenBy != NULL)
         delete SeenBy;

      Msg->Add (Packet);
      RetVal = Msg->Highest ();
   }

   if (BadMessage == TRUE) {
      if (Msg != NULL)
         delete Msg;
      Msg = Extra;
   }

   return (RetVal);
}

VOID TMail::Import (VOID)
{
   USHORT Found, SkipPacket, Empty;
   CHAR *Line;
   ULONG Number, TossedArea;
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

   if (Log != NULL)
      Log->Write ("+Import From %s", Inbound);

   Nodes = new TNodes (Cfg->NodelistPath);
   Data = new TMsgData (Cfg->SystemPath);
   Forward = new TEchoLink (Cfg->SystemPath);
   Dupes = new TDupes (Cfg->SystemPath);

   TossedArea = 0L;

   while (OpenNextPacket () == TRUE) {
      LastTag[0] = '\0';
      Packets++;

      cprintf ("Tossing mail bundle %s from %s (%lu msgs.)\r\n", strupr (PktName), Packet->FromAddress, Packet->Number ());

      if (Log != NULL) {
         Log->Write ("+%s, %02d/%02d/%d, %02d:%02d (%lu Msgs.)", strupr (PktName), Packet->Date.Month, Packet->Date.Day, Packet->Date.Year, Packet->Date.Hour, Packet->Date.Minute, Packet->Number ());
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
      Found = FALSE;

      do {
         if (SkipPacket == TRUE)
            break;
         cprintf ("%s #%06lu: ", PktName, Number);
         if (Packet->Read (Number, MAX_LINE_LENGTH) == TRUE) {
            // Vengono resettati i flags che non devono essere mantenuti durante il
            // passaggio da un nodo all'altro
            Packet->Sent = Packet->Received = FALSE;
            Packet->Crash = Packet->Direct = Packet->Hold = FALSE;

            if ((Line = (PSZ)Packet->Text.First ()) != NULL) {
               if (!strncmp (Line, "AREA:", 5)) {
                  cprintf ("%s ", strupr (&Line[5]));
                  if (stricmp (&Line[5], LastTag)) {
                     if (Log != NULL && TossedArea != 0L)
                        Log->Write (":   %-30.30s (Toss=%04d)", LastTag, TossedArea);
                     TossedArea = 0L;
                     strcpy (LastTag, strupr (&Line[5]));
                     Found = OpenArea (LastTag);
                  }
                  if (Found == TRUE) {
                     if (Forward->Check (Packet->FromAddress) == TRUE) {
                        if (Dupes->Check (Data->EchoTag, Packet) == FALSE)
                           cprintf ("=> #%06lu", ImportEchoMail ());
                        else {
                           cprintf ("=> Duplicate #%06lu", ImportEchoMail ());
                           Duplicate++;
                        }
                     }
                     else {
                        cprintf ("=> Bad_Msgs #%06lu", ImportEchoMail ());
                        Bad++;
                     }
                  }
                  else {
                     cprintf ("=> Bad_Msgs #%06lu", ImportEchoMail ());
                     Bad++;
                  }
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
                           Log->Write (":   %-30.30s (Toss=%04d)", LastTag, TossedArea);
                     }
                     TossedArea = 0L;
                     Msg->UnLock ();
                     delete Msg;
                  }
                  strcpy (LastTag, "NetMail");

                  cprintf ("NetMail from %s ", Packet->FromAddress);

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

                     if (Msg != NULL) {
                        Msg->New ();
                        Msg->Add (Packet);
                        cprintf ("=> #%06lu", Msg->Highest ());
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
                           if (Extra != NULL) {
                              Extra->Add (Packet);
                              delete Extra;
                           }
                        }
                        delete User;
                     }
                  }
                  else
                     cprintf ("Empty message, skipped");
               }
            }

            MsgTossed++;
         }
         cprintf ("\r\n");
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
               Log->Write (":   %-30.30s (Toss=%04d)", LastTag, TossedArea);
         }
         TossedArea = 0L;
         Msg->UnLock ();
         delete Msg;
      }
      Msg = NULL;

      cprintf ("Toss of bundle %s successful, deleting\r\n\r\n", PktName);
      if (Packet != NULL) {
         Packet->Kill ();
         delete Packet;
         Packet = NULL;
      }
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

USHORT TMail::IsArcmail (VOID)
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

USHORT TMail::IsMail (VOID)
{
   DIR *dir;
   USHORT RetVal = FALSE;
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
         if (strstr (ent->d_name, ".pk") != NULL)
            RetVal = TRUE;
      }
      closedir (dir);
   }

   return (RetVal);
}

USHORT TMail::UnpackArcmail (VOID)
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
               if (Log != NULL)
                  Log->Write ("+Unpacking %s", Filename);
               if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
                  if (Packer->CheckArc (Filename) == TRUE) {
                     cprintf ("Unpacking %s", Filename);
                     if (Packer->DoUnpack (Filename, Inbound, "*.pk?") == FALSE) {
                        if (Log != NULL)
                           Log->Write ("!  Command returned error: %s", Packer->Error);
                     }
                     unlink (Filename);
                     cprintf ("\r\n");
                     RetVal = TRUE;
                  }
                  else {
                     counter = 1;
                     do {
                        sprintf (BadArc, "%sbad_arc.%03d", Inbound, counter++);
                     } while (rename (Filename, BadArc) == -1);
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

// ----------------------------------------------------------------------
class TConfig *Cfg = NULL;
class TLog *Log = NULL;

VOID ImportFromBadMsgs (VOID)
{
   CHAR *p, Found;
   ULONG Number;
   class TMsgBase *Msg, *Bad;
   class TMsgData *Data;

   if (Cfg != NULL) {
      Bad = NULL;
      if (Cfg->BadStorage == ST_JAM)
         Bad = new JAM (Cfg->BadPath);
      else if (Cfg->BadStorage == ST_SQUISH)
         Bad = new SQUISH (Cfg->BadPath);
      else if (Cfg->BadStorage == ST_FIDO)
         Bad = new FIDOSDM (Cfg->BadPath);
      else if (Cfg->BadStorage == ST_ADEPT)
         Bad = new ADEPT (Cfg->BadPath);

      cprintf ("Sarching path '%s'...\r\n", Cfg->BadPath);

      if (Bad != NULL) {
         Bad->Lock (0L);
         Number = Bad->Lowest ();
         do {
            if (Bad->Read (Number) == TRUE) {
               if ((p = (CHAR *)Bad->Text.First ()) != NULL)
                  do {
                     if (*p != 0x01)
                        break;
                     else
                        Bad->Text.Remove ();
                  } while ((p = (CHAR *)Bad->Text.First ()) != NULL);

               if (p != NULL) {
                  if (!strncmp (p, "AREA:", 5)) {
                     Found = FALSE;
                     cprintf ("Tossing %s #%06lu: %s ", Cfg->BadPath, Number, &p[5]);

                     if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
                        if (Data->First () == TRUE)
                           do {
                              if (!stricmp (&p[5], Data->EchoTag)) {
                                 Msg = NULL;
                                 if (Data->Storage == ST_JAM)
                                    Msg = new JAM (Data->Path);
                                 else if (Data->Storage == ST_SQUISH)
                                    Msg = new SQUISH (Data->Path);
                                 else if (Data->Storage == ST_FIDO)
                                    Msg = new FIDOSDM (Data->Path);
                                 else if (Data->Storage == ST_ADEPT)
                                    Msg = new ADEPT (Data->Path);
                                 if (Msg != NULL) {
                                    Bad->Text.Remove ();
                                    Msg->Add (Bad);
                                    cprintf ("=> #%06lu\r\n", Msg->Highest ());
                                    Bad->Delete (Number);
                                    delete Msg;
                                 }
                                 Found = TRUE;
                                 break;
                              }
                           } while (Data->Next () == TRUE);
                        delete Data;
                     }

                     if (Found == FALSE)
                        cprintf ("(Unknown area)\r\n");
                  }
               }
            }
         } while (Bad->Next (Number) == TRUE);

         Bad->UnLock ();

         cprintf ("Packing '%s'...\r\n", Cfg->BadPath);
         Bad->Pack ();

         delete Bad;
      }

      cprintf ("\r\n");
   }
}

VOID ImportTic (VOID)
{
#if !defined(__LINUX__)
   class TTic *Tic;

   if ((Tic = new TTic) != NULL) {
      strcpy (Tic->Inbound, Cfg->ProtectedInbound);
      while (Tic->OpenNext () == TRUE) {
         if (Tic->Import () == TRUE)
            Tic->Delete ();
      }
      strcpy (Tic->Inbound, Cfg->KnownInbound);
      while (Tic->OpenNext () == TRUE) {
         if (Tic->Import () == TRUE)
            Tic->Delete ();
      }
      strcpy (Tic->Inbound, Cfg->NormalInbound);
      while (Tic->OpenNext () == TRUE) {
         if (Tic->Import () == TRUE)
            Tic->Delete ();
      }
      delete Tic;
   }
#endif
}

VOID ProcessNewsgroups (VOID)
{
   ULONG Number, Tossed, SentArea;
   class TMsgData *Data;
   class USENET *Usenet;
   class TMsgBase *Msg;
   class TUser *User;

   if (Log != NULL)
      Log->Write ("+Processing NEWSgroups");

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (Data->Storage != ST_USENET && Data->NewsGroup[0] != '\0' && Data->UpdateNews == TRUE) {
               if (Data->Storage == ST_JAM)
                  Msg = new JAM (Data->Path);
               else if (Data->Storage == ST_SQUISH)
                  Msg = new SQUISH (Data->Path);
               else if (Data->Storage == ST_FIDO)
                  Msg = new FIDOSDM (Data->Path);
               else if (Data->Storage == ST_ADEPT)
                  Msg = new ADEPT (Data->Path);

               if (Msg != NULL) {
                  cprintf ("Processing %s ", Data->NewsGroup);
                  if ((Usenet = new USENET (Cfg->NewsServer, Data->NewsGroup)) != NULL) {
                     strcpy (Usenet->HostName, Cfg->HostName);
                     strcpy (Usenet->Organization, Cfg->SystemName);
                     sprintf (Usenet->ProgramID, "%s Version %s", NAME, VERSION);

                     cprintf ("(%lu msgs)\r\n", Usenet->Number ());

                     cprintf ("Scanning %-40.40s ", Data->NewsGroup);
                     Number = Msg->Lowest ();
                     cprintf ("#%06lu to #%06lu ", Number, Msg->Highest ());
                     SentArea = 0L;

                     Msg->Lock (0L);

                     do {
                        if (Msg->Read (Number) == TRUE) {
                           cprintf ("<#%06lu>\b\b\b\b\b\b\b\b\b", Number);
                           if (Msg->Sent == FALSE) {
                              strcpy (Usenet->User, "anonymous");
                              if (Msg->Local == TRUE) {
                                 if ((User = new TUser (Cfg->UserFile)) != NULL) {
                                    if (User->GetData (Msg->From) == TRUE)
                                       strcpy (Usenet->User, User->MailBox);
                                    delete User;
                                 }
                              }
                              if (Usenet->Add (Msg) == TRUE) {
                                 Msg->Delete (Number);
                                 SentArea++;
                              }
                              else if (Log != NULL) {
                                 Log->Write (":Sending msg# %lu in %s", Number, Data->NewsGroup);
                                 Log->Write ("!Server error: %s", Usenet->Error);
                              }
                           }
                        }
                     } while (Msg->Next (Number) == TRUE);

                     Msg->UnLock ();

                     cprintf ("\r\n");
                     Tossed = 0L;
                     Number = Data->Highest;

                     while (Usenet->Next (Number) == TRUE) {
                        if (Usenet->Read (Number, MAX_LINE_LENGTH) == TRUE) {
                           cprintf ("%s #%06lu: ", Data->NewsGroup, Number);
                           Usenet->Sent = TRUE;
                           Msg->Add (Usenet);
                           cprintf ("=> #%06lu\r\n", Msg->Highest ());
                           Tossed++;
                        }
                     }

                     Data->Highest = Number;

                     if (Log != NULL && (Tossed != 0L || SentArea != 0L))
                        Log->Write (":   %-30.30s (Toss=%04d,Sent=%04lu)", Data->NewsGroup, Tossed, SentArea);

                     delete Usenet;

                     Data->ActiveMsgs = Msg->Number ();
                     Data->Update ();
                  }
                  delete Msg;
                  Msg = NULL;
                  cprintf ("\r\n");
               }
            }
         } while (Data->Next () == TRUE);

      cprintf ("\r\n");
      delete Data;
   }
}

int main (int argc, char *argv[])
{
   int fd, i;
   USHORT Import, Export, Pack, DoNetmail, NoArcMail;
   USHORT DoBad, Tic, News;
   CHAR Temp[128], *Config = NULL, *Channel = NULL, *Inbound = NULL;
   ULONG Running, Messages = 0L;
   struct stat statbuf;
   class TMail *Mail;
   class TRoute *Route;

   Import = Export = Pack = FALSE;
   DoNetmail = TRUE, DoBad = TRUE;
   Tic = News = NoArcMail = FALSE;

   printf ("\nMAIL; %s Mail Processor - Version %s\n", NAME, VERSION);
   printf ("      Copyright (c) 1991-96 by Marco Maccaferri. All Rights Reserved.\n\n");

   if (ValidateKey ("bbs") == KEY_UNREGISTERED) {
      printf ("* * *     WARNING: No license key found    * * *\n");
      if ((i = CheckExpiration ()) == 0) {
         printf ("* * *   This evaluation copy has expired   * * *\n\a\n");
          exit (0);
      }
      else
         printf ("* * * You have %2d days left for evaluation * * * \n\a\n", i);
   }

   if (argc <= 1) {
      printf (" * Command-line parameters:\n\n");

      printf ("        IMPORT    Import (toss) messages from packets\n");
      printf ("        EXPORT    Export (scan) messages to packets\n");
      printf ("        PACK      Export netmail and route outgoing packets\n\n");
//#if !defined(__LINUX__)
//      printf ("        TIC       Process TIC files\n\n");
//#endif
      printf ("        NEWS      Process newsgroups\n\n");
      printf ("        -o        Only route packets when PACKing\n");
      printf ("        -b        Only toss .PKT files, never ARCmail bundles\n");
      printf ("        -i        Do no import from bad messages\n");
      printf ("        -f<dir>   Use <dir> as inbound directory\n");

      printf ("\n * Please refer to the documentation for a more complete command summary\n\n");
   }
   else {
      for (i = 1; i < argc; i++) {
         if (!stricmp (argv[i], "import") || !stricmp (argv[i], "toss") || !stricmp (argv[i], "in"))
            Import = TRUE;
         else if (!stricmp (argv[i], "export") || !stricmp (argv[i], "scan") || !stricmp (argv[i], "out"))
            Export = TRUE;
         else if (!stricmp (argv[i], "pack") || !stricmp (argv[i], "squash"))
            Pack = TRUE;
//         else if (!stricmp (argv[i], "tic"))
//            Tic = TRUE;
         else if (!stricmp (argv[i], "news"))
            News = TRUE;
         else if (!stricmp (argv[i], "-o"))
            DoNetmail = FALSE;
         else if (!stricmp (argv[i], "-b"))
            NoArcMail = TRUE;
         else if (!stricmp (argv[i], "-i"))
            DoBad = FALSE;
         else if (!stricmp (argv[i], "-f")) {
            if (argv[i][2] != '\0')
               Inbound = &argv[i][2];
         }
         else if (Config == NULL)
            Config = argv[i];
         else if (Channel == NULL)
            Channel = argv[i];
      }

      if ((Cfg = new TConfig) != NULL) {
         if (Cfg->Load (Config, Channel) == FALSE)
            Cfg->Default ();
      }

      Running = time (NULL);

      sprintf (Temp, "%smailproc.flg", Cfg->SystemPath);
      if (stat (Temp, &statbuf) != 0) {
         if ((fd = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
            close (fd);

         if ((Log = new TLog) != NULL) {
            Log->Open ("mail.log");
#if defined(__OS2__)
            Log->Write ("+Begin, v%s (OS/2)", VERSION);
#elif defined(__NT__)
            Log->Write ("+Begin, v%s (Win32)", VERSION);
#endif
            Log->Write ("+Message-base sharing is enabled");
         }

         if ((Mail = new TMail) != NULL) {
            Mail->Cfg = Cfg;
            Mail->Log = Log;

            if (Import == TRUE) {
               if (DoBad == TRUE)
                  ImportFromBadMsgs ();

               if (Inbound == NULL) {
                  strcpy (Mail->Inbound, Cfg->ProtectedInbound);
                  if (NoArcMail == FALSE) {
                     while (Mail->IsArcmail () == TRUE)
                        Mail->UnpackArcmail ();
                  }
                  if (Mail->IsMail () == TRUE)
                     Mail->Import ();
                  strcpy (Mail->Inbound, Cfg->KnownInbound);
                  if (NoArcMail == FALSE) {
                     while (Mail->IsArcmail () == TRUE)
                        Mail->UnpackArcmail ();
                  }
                  if (Mail->IsMail () == TRUE)
                     Mail->Import ();
                  strcpy (Mail->Inbound, Cfg->NormalInbound);
                  if (NoArcMail == FALSE) {
                     while (Mail->IsArcmail () == TRUE)
                        Mail->UnpackArcmail ();
                  }
                  if (Mail->IsMail () == TRUE)
                     Mail->Import ();
               }
               else {
                  strcpy (Mail->Inbound, Inbound);
                  if (NoArcMail == FALSE) {
                     while (Mail->IsArcmail () == TRUE)
                        Mail->UnpackArcmail ();
                  }
                  if (Mail->IsMail () == TRUE)
                     Mail->Import ();
               }

               strcpy (Mail->Inbound, ".\\");
               if (NoArcMail == FALSE) {
                  while (Mail->IsArcmail () == TRUE)
                     Mail->UnpackArcmail ();
               }
               if (Mail->IsMail () == TRUE)
                  Mail->Import ();

               if (Log != NULL) {
                  if (Mail->Packets == 0)
                     Log->Write ("+No ECHOmail processed at this time");
                  else {
                     Messages = Mail->MsgTossed;
                     Log->Write ("+%d packet(s): %lu NETmail, %lu ECHOmail, %lu Dupes, %lu Bad", Mail->Packets, Mail->NetMail, Mail->MsgTossed - Mail->NetMail, Mail->Duplicate, Mail->Bad);
                  }
               }
            }

            if (Tic == TRUE)
               ImportTic ();

            if (News == TRUE)
               ProcessNewsgroups ();

            if (Export == TRUE) {
               Mail->Export ();

               if (Log != NULL) {
                  if (Mail->MsgSent != 0L)
                     Log->Write ("+%lu ECHOmail message(s) forwarded", Mail->MsgSent);
                  else
                     Log->Write ("+No ECHOmail messages forwarded");
               }
            }

            if (Pack == TRUE && DoNetmail == TRUE)
               Mail->ExportNetMail ();

            delete Mail;
         }

         if (Pack == TRUE) {
            if ((Route = new TRoute) != NULL) {
               Route->Cfg = Cfg;
               Route->Log = Log;
               Route->Run ();
               delete Route;
            }
         }

         if (Log != NULL) {
            Log->Write (":End");
            delete Log;
         }

         sprintf (Temp, "%smailproc.flg", Cfg->SystemPath);
         unlink (Temp);

         if ((Running = time (NULL) - Running) == 0L)
            Running = 1L;
         if (Messages != 0L)
            printf ("Run time %lu second(s), %lu msgs/sec, exit\n\n", Running, Messages / Running);
         else
            printf ("Run time %lu second(s), exit\n\n", Running);
      }

      if (Cfg != NULL)
         delete Cfg;
   }

   return (0);
}


