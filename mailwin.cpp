
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
#include "msgbase.h"
#include "lorawin.h"

PSZ Exts[] = { "su0", "mo0", "tu0", "we0", "th0", "fr0", "sa0", NULL };

// ----------------------------------------------------------------------------
// Mail processor output filter
// ----------------------------------------------------------------------------

#if defined(__OS2__) || defined(__NT__)
TPMList::TPMList (HWND hwnd)
{
   time_t t;
   struct tm *ltm;

   fp = NULL;
   hwndList = hwnd;

   if (Cfg->MakeProcessLog == TRUE) {
      if ((fp = fopen ("process.log", "at")) != NULL) {
         t = time (NULL);
         ltm = localtime (&t);
         fprintf (fp, "\nProcessing date/time: %s", asctime (ltm));
         fprintf (fp, "-------------------------------------------------------------------------------\n");
      }
   }

   DoWrite = FALSE;
}
#elif defined(__LINUX__) || defined(__DOS__)
TPMList::TPMList ()
{
   time_t t;
   struct tm *ltm;

   fp = NULL;
   window = wopen (13, 1, 20, 78, 5, LGREY|_BLACK, LCYAN|_BLACK);
   First = TRUE;

   if (Cfg->MakeProcessLog == TRUE) {
      if ((fp = fopen ("process.log", "at")) != NULL) {
         t = time (NULL);
         ltm = localtime (&t);
         fprintf (fp, "\nProcessing date/time: %s", asctime (ltm));
         fprintf (fp, "-------------------------------------------------------------------------------\n");
      }
   }

   DoWrite = FALSE;
}
#endif

TPMList::~TPMList ()
{
#if defined(__DOS__) || defined(__LINUX__)
   if (window != 0) {
      wactiv (window);
      wclose ();
   }
#endif
   if (fp != NULL && Cfg->MakeProcessLog == TRUE) {
      if (DoWrite == TRUE) {
         fprintf (fp, "%s\n", Line);
         DoWrite = FALSE;
      }
      fprintf (fp, "-------------------------------------------------------------------------------\n");
      fclose (fp);
   }
}

VOID TPMList::Add (PSZ Text)
{
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_ADDOUTBOUNDLINE), MPFROMP (Text));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_ADDOUTBOUNDLINE, (LPARAM)Text);
#elif defined(__LINUX__) || defined(__DOS__)
   if (window != 0) {
      wactiv (window);
      if (First == TRUE) {
         wprintf ("%.78s", Text);
         First = FALSE;
      }
      else
         wprintf ("\n%.78s", Text);
      videoupdate ();
   }
#endif
   if (fp != NULL && Cfg->MakeProcessLog == TRUE) {
      if (DoWrite == TRUE) {
         fprintf (fp, "%s\n", Line);
         DoWrite = FALSE;
      }
      strcpy (Line, Text);
      DoWrite = TRUE;
   }
}

VOID TPMList::Update (PSZ Text)
{
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_REPLACEOUTBOUNDLINE), MPFROMP (Text));
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_REPLACEOUTBOUNDLINE, (LPARAM)Text);
#elif defined(__DOS__) || defined(__LINUX__)
   if (window != 0) {
      wactiv (window);
      wprintf ("\r%.78s", Text);
      videoupdate ();
   }
#endif
   if (fp != NULL && Cfg->MakeProcessLog == TRUE) {
      strcpy (Line, Text);
      DoWrite = TRUE;
   }
}

VOID TPMList::Clear ()
{
#if defined(__OS2__)
   WinSendMsg (hwndList, WM_USER, MPFROMSHORT (WMU_CLEAROUTBOUND), 0L);
#elif defined(__NT__)
   SendMessage (hwndList, WM_USER, (WPARAM)WMU_CLEAROUTBOUND, 0L);
#elif defined(__DOS__) || defined(__LINUX__)
   if (window != 0) {
      wactiv (window);
      wclear ();
      videoupdate ();
      First = TRUE;
   }
#endif
}

// ----------------------------------------------------------------------------
// Mail kludges
// ----------------------------------------------------------------------------

TKludges::TKludges ()
{
   Data.Clear ();
   Sort = TRUE;
}

TKludges::~TKludges ()
{
   Data.Clear ();
}

USHORT TKludges::Add ()
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

VOID TKludges::Clear ()
{
   Data.Clear ();
   New ();
}

VOID TKludges::Delete ()
{
   if (Data.Value () != NULL)
      Data.Remove ();
}

USHORT TKludges::First ()
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

VOID TKludges::New ()
{
   Zone = 0;
   Net = 0;
   Node = 0;
   Point = 0;
}

USHORT TKludges::Next ()
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

// ----------------------------------------------------------------------------
// Mail processor
// ----------------------------------------------------------------------------

TMailProcessor::TMailProcessor ()
{
   Output = NULL;
   Status = NULL;
   Packets = 0;
   Bad = Duplicate = NetMail = MsgTossed = 0L;
}

TMailProcessor::~TMailProcessor ()
{
}

VOID TMailProcessor::MakeArcMailName (PSZ pszAddress, CHAR Flag)
{
   USHORT i, Found;
   CHAR *p;
   ULONG Crc32;
   class TAddress Addr;
   class TOutbound *Out;
   struct stat statbuf;
   struct dosdate_t d_date;

   _dos_getdate (&d_date);

   Addr.Parse (pszAddress);
   Crc32 = StringCrc32 (Cfg->MailAddress.String, 0xFFFFFFFFL);
   Crc32 = StringCrc32 (Addr.String, Crc32);

   if (Addr.Zone == 0 || Cfg->MailAddress.Zone == Addr.Zone) {
      if (Addr.Point != 0)
         sprintf (ArcMailName, "%s\\%04x%04x.pnt\\%08lx.%s", Outbound, Addr.Net, Addr.Node, Crc32, Exts[d_date.dayofweek]);
      else
         sprintf (ArcMailName, "%s\\%08lx.%s", Outbound, Crc32, Exts[d_date.dayofweek]);
   }
   else {
      if (Addr.Point != 0)
         sprintf (ArcMailName, "%s.%03x\\%04x%04x.pnt\\%08lx.%s", Outbound, Addr.Zone, Addr.Net, Addr.Node, Crc32, Exts[d_date.dayofweek]);
      else
         sprintf (ArcMailName, "%s.%03x\\%08lx.%s", Outbound, Addr.Zone, Crc32, Exts[d_date.dayofweek]);
   }

   AdjustPath (ArcMailName);

   Found = FALSE;
   if ((p = strchr (ArcMailName, '\0')) != NULL) {
      p--;
      for (i = '0'; i <= '9' && Found == FALSE; i++) {
         *p = (CHAR)i;
         if (stat (ArcMailName, &statbuf) == 0) {
            Found = TRUE;
            if (statbuf.st_size == 0L) {
               unlink (ArcMailName);
               if (++i > '9')
                  i = '0';
               *p = (CHAR)i;
            }
         }
      }
      if (Found == FALSE)
         *p = '0';
   }

   if ((Out = new TOutbound (Cfg->Outbound)) != NULL) {
      if (Cfg->MailAddress.First () == TRUE)
         Out->DefaultZone = Cfg->MailAddress.Zone;
      Found = FALSE;
      Out->Add (Addr.Zone, Addr.Net, Addr.Node, Addr.Point, Addr.Domain);
      if (Out->First () == TRUE)
         do {
            if (!stricmp (Out->Complete, ArcMailName)) {
               Found = TRUE;
               break;
            }
         } while (Out->Next () == TRUE);

      if (Found == FALSE) {
         Out->New ();
         Out->Zone = Addr.Zone;
         Out->Net = Addr.Net;
         Out->Node = Addr.Node;
         Out->Point = Addr.Point;
         strcpy (Out->Domain, Addr.Domain);
         strcpy (Out->Name, &ArcMailName[strlen (ArcMailName) - 12]);
         strcpy (Out->Complete, ArcMailName);
         Out->Status = Flag;
         Out->ArcMail = TRUE;
         Out->TruncateAfter = TRUE;
         Out->Add ();
         Out->Update ();
      }

      delete Out;
   }
}

VOID TMailProcessor::News ()
{
   CHAR *Base;
   ULONG Number, Tossed, SentArea, Msgn;
   class USENET *Usenet;
   class TUser *User;
   class TEchotoss *EchoToss;

   if (Output != NULL)
      Output->Clear ();
   if (Log != NULL)
      Log->Write ("+Processing NEWSgroups");

   MsgTossed = MsgSent = 0L;

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (Data->Storage != ST_USENET && Data->Storage != ST_PASSTHROUGH && Data->NewsGroup[0] != '\0' && Data->UpdateNews == TRUE) {
               if (Data->Storage == ST_JAM) {
                  Msg = new JAM (Data->Path);
                  Base = "JAM";
               }
               else if (Data->Storage == ST_SQUISH) {
                  Msg = new SQUISH (Data->Path);
                  Base = "Squish<tm>";
               }
               else if (Data->Storage == ST_FIDO) {
                  Msg = new FIDOSDM (Data->Path);
                  Base = "Fido *.MSG";
               }
               else if (Data->Storage == ST_ADEPT) {
                  Msg = new ADEPT (Data->Path);
                  Base = "AdeptXBBS";
               }
               else if (Data->Storage == ST_HUDSON) {
                  Msg = new HUDSON (Data->Path, (UCHAR)Data->Board);
                  Base = "Hudson";
               }

               if (Msg != NULL) {
                  if (Status != NULL) {
                     Status->Clear ();
                     Status->SetLine (0, "Processing %s ", Data->NewsGroup);
                  }

                  if ((Usenet = new USENET (Cfg->NewsServer, Data->NewsGroup)) != NULL) {
                     strcpy (Usenet->HostName, Cfg->HostName);
                     strcpy (Usenet->Organization, Cfg->SystemName);
                     sprintf (Usenet->ProgramID, "%s Version %s", NAME, VERSION);

                     if (Status != NULL)
                        Status->SetLine (0, "Sending messages to %s", Data->NewsGroup);

                     Msg->Lock (0L);
                     Number = 0L;
                     if (Data->NewsHWM < Msg->Highest ())
                        Number = Data->NewsHWM;
                     SentArea = 0L;

                     if (Msg->Next (Number) == TRUE)
                        do {
                           if (Msg->Read (Number, MAX_LINE_LENGTH) == TRUE) {
                              Msgn = Msg->UidToMsgn (Number);
                              if (Status != NULL && (Msgn % 10) == 0L)
                                 Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());

                              if (Msg->Sent == FALSE) {
                                 if (Status != NULL && (Msgn % 10) != 0L)
                                    Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());
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
                                    MsgSent++;
                                 }
                                 else if (Log != NULL) {
                                    Log->Write (":Sending msg# %lu in %s", Number, Data->NewsGroup);
                                    Log->Write ("!Server error: %s", Usenet->Error);
                                 }
                              }
                           }
                        } while (Msg->Next (Number) == TRUE);

                     Msg->UnLock ();

                     // Aggiorna subito l'high water mark per le aree newsgroup
                     Data->NewsHWM = Number;
                     Data->Update ();

                     Tossed = 0L;
                     Number = Data->Highest;
                     // Se e' stato impostato un limite di messaggi si assicura di ricevere gli ultimi n messaggi
                     if (Cfg->RetriveMaxMessages != 0 && (Usenet->Highest () - Number) > Cfg->RetriveMaxMessages)
                        Number = Usenet->Highest () - Cfg->RetriveMaxMessages;

                     if (Status != NULL) {
                        Status->Clear ();
                        Status->SetLine (0, "Receiving message from %s (%lu msgs)", Data->NewsGroup, Usenet->Number ());
                     }

                     while (Usenet->Next (Number) == TRUE) {
                        if (Status != NULL)
                           Status->SetLine (1, "   %lu / %lu", Usenet->UidToMsgn (Number), Usenet->UidToMsgn (Usenet->Highest ()));
                        if (Output != NULL) {
                           sprintf (Display, "%6lu %-32.32s", Usenet->UidToMsgn (Number), Data->NewsGroup);
                           Output->Add (Display);
                        }
                        if (Usenet->Read (Number, MAX_LINE_LENGTH) == TRUE) {
                           Usenet->Sent = TRUE;
                           Msg->Add (Usenet);
                           if (Output != NULL) {
                              sprintf (Display, "%6lu %-32.32s %-12.12s %6lu", Usenet->UidToMsgn (Number), Data->NewsGroup, Base, Msg->UidToMsgn (Msg->Highest ()));
                              Output->Update (Display);
                           }
                           Tossed++;
                           MsgTossed++;
                           //////////////////////////////////////////////////////////////////////////////////////////////////
                           // Verifica il raggiungimento del limite di messaggi da ricevere in una volta sola              //
                           //////////////////////////////////////////////////////////////////////////////////////////////////
                           if (Cfg->RetriveMaxMessages != 0 && Tossed >= Cfg->RetriveMaxMessages)
                              break;
                        }
                     }

                     Data->Highest = Number;

                     if (Log != NULL && (Tossed != 0L || SentArea != 0L)) {
                        Log->Write (":   %-20.20s (Toss=%04d,Sent=%04lu)", Data->NewsGroup, Tossed, SentArea);
                        if ((EchoToss = new TEchotoss (Cfg->SystemPath)) != NULL) {
                           EchoToss->Load ();
                           if (Data->EchoTag[0] != '\0')
                              EchoToss->Add (Data->EchoTag);
                           EchoToss->Save ();
                           delete EchoToss;
                        }
                     }

                     delete Usenet;

                     Data->ActiveMsgs = Msg->Number ();
                     Data->Update ();
                  }
                  delete Msg;
                  Msg = NULL;
               }
            }
         } while (Data->Next () == TRUE);

      delete Data;
   }
}

VOID TMailProcessor::Mail ()
{
   CHAR Host[128], Name[32], *p, ToUs;
   ULONG Number, Msgn;
   class TUser *User;
   class INETMAIL *Mail;
   class TAddress Addr;
   class TMsgBase *EMail;

   if (Status != NULL)
      Status->Clear ();
   if (Log != NULL)
      Log->Write ("+Processing Internet Mail");

   MsgSent = MsgTossed = 0L;

#if !defined(__POINT__)
   if (stricmp (Cfg->MailPath, Cfg->NetMailPath)) {
      Msg = NULL;
      if (Cfg->MailStorage == ST_JAM)
         Msg = new JAM (Cfg->MailPath);
      else if (Cfg->MailStorage == ST_SQUISH)
         Msg = new SQUISH (Cfg->MailPath);
      else if (Cfg->MailStorage == ST_FIDO)
         Msg = new FIDOSDM (Cfg->MailPath);
      else if (Cfg->MailStorage == ST_ADEPT)
         Msg = new ADEPT (Cfg->MailPath);
      else if (Cfg->MailStorage == ST_HUDSON)
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->MailBoard);

      if ((Mail = new INETMAIL) != NULL) {
         strcpy (Mail->SMTPHostName, Cfg->MailServer);
         if (Status != NULL) {
            sprintf (Temp, "Sending e-mail to %s", Mail->SMTPHostName);
            Status->SetLine (0, Temp);
         }
         Number = Msg->Lowest ();
         do {
            if (Msg->ReadHeader (Number) == TRUE) {
               Msgn = Msg->UidToMsgn (Number);
               if (Status != NULL && (Msgn % 10) == 0L)
                  Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());
               if (Msg->Sent == FALSE && strchr (Msg->To, '@') != NULL) {
                  Msg->Read (Number);
                  if (Status != NULL && (Msgn % 10) != 0L)
                     Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());

                  // Verifica se la prima riga del testo del messaggio contiene il kludge
                  // To: per specificare l'indirizzo internet di destinazione.
                  if ((p = (CHAR *)Msg->Text.First ()) != NULL) {
                     strcpy (Host, p);
                     if (!strncmp (strlwr (Host), "to:", 3)) {
                        p += 3;
                        while (*p == ' ')
                           p++;
                        strcpy (Msg->To, p);
                        // La riga con il kludge viene tolta.
                        Msg->Text.Remove ();
                        // Verifica se la riga sucessiva e' vuota, nel qual caso toglie anche quella.
                        if ((p = (CHAR *)Msg->Text.First ()) != NULL) {
                           if (*p == '\0')
                              Msg->Text.Remove ();
                        }
                     }
                  }

                  // Scrive qualche informazione sul log.
                  if (Log != NULL) {
                     Log->Write (":  Msg#: %lu", Msg->UidToMsgn (Number));
                     Log->Write (":    Fm: %s", Msg->From);
                     Log->Write (":    To: %s", Msg->To);
                     Log->Write (":    Sb: %s", Msg->Subject);
                  }

                  if (Msg->Local == TRUE) {
                     // Nel caso di messaggi locali viene impostato di default un indirizzo
                     // di ritorno di convenienza per il nostro host.
                     sprintf (Msg->FromAddress, "anonymous@%s", Cfg->HostName);
                     if ((User = new TUser (Cfg->UserFile)) != NULL) {
                        if (User->GetData (Msg->From, FALSE) == TRUE) {
                           // Se l'utente ha un indirizzo e-mail proprio, viene usato quello come
                           // indirizzo di ritorno, altrimenti viene usata la sua mailbox presso
                           // il nostro host.
                           if (User->UseInetAddress == TRUE && User->InetAddress[0] != '\0')
                              strcpy (Msg->FromAddress, User->InetAddress);
                           else
                              sprintf (Msg->FromAddress, "%s@%s", User->MailBox, Cfg->HostName);
                        }
                        delete User;
                     }
                  }
                  else {
                     // Nel caso in cui il messaggio provenga da un'altro nodo imposta come
                     // indirizzo di ritorno di default quello standard per il gateway FidoNet.
                     Addr.Parse (Msg->FromAddress);
                     if (Addr.Point != 0)
                        sprintf (Msg->FromAddress, "%s@p%u.f%u.n%u.z%u.fidonet.org", Msg->From, Addr.Point, Addr.Node, Addr.Net, Addr.Zone);
                     else
                        sprintf (Msg->FromAddress, "%s@f%u.n%u.z%u.fidonet.org", Msg->From, Addr.Node, Addr.Net, Addr.Zone);
                     while ((p = strchr (Msg->FromAddress, ' ')) != NULL)
                        *p = '.';
                     if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                        if (Nodes->Read (Msg->FromAddress, FALSE) == FALSE) {
                           // Nel caso in cui il nodo (o point) abbia un suo indirizzo e-mail viene
                           // usato quello come indirizzo di ritorno.
                           if (Nodes->UseInetAddress == TRUE && Nodes->InetAddress[0] != '\0')
                              strcpy (Msg->FromAddress, Nodes->InetAddress);
                        }
                        delete Nodes;
                     }
                  }

                  // Invia il mail attraverso la classe INETMAIL.
                  if (Mail->Add (Msg) == TRUE) {
                     Msg->ReadHeader (Number);
                     Msg->Sent = TRUE;
                     Msg->WriteHeader (Number);
                     MsgSent++;
                  }
               }
            }
         } while (Msg->Next (Number) == TRUE);
         delete Mail;
      }

      if (Msg != NULL) {
         delete Msg;
         Msg = NULL;
      }

      if (Status != NULL)
         Status->Clear ();
   }
#endif

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

   if ((Mail = new INETMAIL) != NULL) {
      strcpy (Mail->SMTPHostName, Cfg->MailServer);
      if (Status != NULL) {
         sprintf (Temp, "Sending NetMail to %s", Mail->SMTPHostName);
         Status->SetLine (0, Temp);
      }
      Number = Msg->Lowest ();
      do {
         if (Msg->ReadHeader (Number) == TRUE) {
            Addr.Clear ();
            Addr.Parse (Msg->ToAddress);
            if (Cfg->MailAddress.First () == TRUE && Addr.Zone == 0)
               Addr.Zone = Cfg->MailAddress.Zone;

            Msgn = Msg->UidToMsgn (Number);
            if (Status != NULL && (Msgn % 10) == 0L)
               Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());

            ToUs = FALSE;
            if (Msg->Sent == FALSE && Cfg->MailAddress.First () == TRUE)
               do {
                  if (Cfg->MailAddress.Zone == Addr.Zone || Addr.Zone == 0) {
                     if (Cfg->MailAddress.Net == Addr.Net && Cfg->MailAddress.Node == Addr.Node) {
                        if (Cfg->MailAddress.Point == Addr.Point)
                           ToUs = TRUE;
                     }
                  }
               } while (Cfg->MailAddress.Next () == TRUE);

            if (ToUs == TRUE && Msg->Sent == FALSE && strchr (Msg->To, '@') != NULL) {
               Msg->Read (Number);
               if (Status != NULL && (Msgn % 10) != 0L)
                  Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());
               if (Log != NULL) {
                  Log->Write (":  Msg#: %lu", Msg->UidToMsgn (Number));
                  Log->Write (":    Fm: %s", Msg->From);
                  Log->Write (":    To: %s", Msg->To);
                  Log->Write (":    Sb: %s", Msg->Subject);
               }
               if (Msg->Local == TRUE) {
                  sprintf (Msg->FromAddress, "anonymous@%s", Cfg->HostName);
                  if ((User = new TUser (Cfg->UserFile)) != NULL) {
                     if (User->GetData (Msg->From, FALSE) == TRUE) {
                        if (User->UseInetAddress == TRUE && User->InetAddress[0] != '\0')
                           strcpy (Msg->FromAddress, User->InetAddress);
                        else
                           sprintf (Msg->FromAddress, "%s@%s", User->MailBox, Cfg->HostName);
                     }
                     delete User;
                  }
               }
               else {
                  if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                     if (Nodes->Read (Msg->FromAddress, FALSE) == TRUE) {
                        if (Nodes->UseInetAddress == TRUE && Nodes->InetAddress[0] != '\0')
                           strcpy (Msg->From, Nodes->InetAddress);
                        else {
                           Addr.Parse (Msg->FromAddress);
                           if (Addr.Point != 0)
                              sprintf (Msg->FromAddress, "%s@p%u.f%u.n%u.z%u.fidonet.org", Msg->From, Addr.Point, Addr.Node, Addr.Net, Addr.Zone);
                           else
                              sprintf (Msg->FromAddress, "%s@f%u.n%u.z%u.fidonet.org", Msg->From, Addr.Node, Addr.Net, Addr.Zone);
                        }
                     }
                     else {
                        Addr.Parse (Msg->FromAddress);
                        if (Addr.Point != 0)
                           sprintf (Msg->FromAddress, "%s@p%u.f%u.n%u.z%u.fidonet.org", Msg->From, Addr.Point, Addr.Node, Addr.Net, Addr.Zone);
                        else
                           sprintf (Msg->FromAddress, "%s@f%u.n%u.z%u.fidonet.org", Msg->From, Addr.Node, Addr.Net, Addr.Zone);
                     }
                     delete Nodes;
                  }
               }
               if (Mail->Add (Msg) == TRUE) {
                  Msg->ReadHeader (Number);
                  Msg->Sent = TRUE;
                  Msg->WriteHeader (Number);
                  MsgSent++;
               }
            }
         }
      } while (Msg->Next (Number) == TRUE);
      delete Mail;
   }

   if (Msg != NULL) {
      delete Msg;
      Msg = NULL;
   }

   if (Status != NULL)
      Status->Clear ();

   Msg = NULL;
#if defined(__POINT__)
   if (Cfg->NetMailStorage == ST_JAM)
      Msg = new JAM (Cfg->NetMailPath);
   else if (Cfg->NetMailStorage == ST_SQUISH)
      Msg = new SQUISH (Cfg->NetMailPath);
   else if (Cfg->NetMailStorage == ST_FIDO)
      Msg = new FIDOSDM (Cfg->NetMailPath);
   else if (Cfg->NetMailStorage == ST_ADEPT)
      Msg = new ADEPT (Cfg->NetMailPath);
   else if (Cfg->NetMailStorage == ST_HUDSON)
      Msg = new HUDSON (Cfg->HudsonPath, Cfg->NetMailBoard);
#else
   if (Cfg->MailStorage == ST_JAM)
      Msg = new JAM (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_SQUISH)
      Msg = new SQUISH (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_FIDO)
      Msg = new FIDOSDM (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_ADEPT)
      Msg = new ADEPT (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_HUDSON)
      Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->MailBoard);
#endif

   Cfg->MailAddress.First ();
   if ((User = new TUser (Cfg->UserFile)) != NULL) {
      if (User->First () == TRUE)
         do {
            if (User->ImportPOP3Mail == TRUE && User->InetAddress[0] != '\0') {
               if (Status != NULL) {
                  sprintf (Temp, "Receiving e-mail for %s (%s)", User->Name, User->InetAddress);
                  Status->SetLine (0, Temp);
               }
               if ((p = strchr (User->InetAddress, '@')) != NULL) {
                  *p = '\0';
                  strcpy (Name, User->InetAddress);
                  strcpy (Host, &p[1]);
                  *p = '@';
               }
               if ((Mail = new INETMAIL (Host, Name, User->Pop3Pwd)) != NULL) {
                  if (Mail->Number () > 0L) {
                     if (Log != NULL)
                        Log->Write ("+Receiving e-mail for %s (%s)", User->Name, User->InetAddress);
                     Number = Mail->Lowest ();
                     do {
                        if (Status != NULL) {
                           sprintf (Temp, "%6lu / %-6lu", Number, Mail->Number ());
                           Status->SetLine (1, Temp);
                        }
                        if (Mail->Read (Number, MAX_LINE_LENGTH) == TRUE) {
                           if (Log != NULL) {
                              Log->Write (":  Msg#: %lu", Number);
                              Log->Write (":    Fm: %s", Mail->From);
                              Log->Write (":    Sb: %s", Mail->Subject);
                           }
                           strcpy (Mail->To, User->Name);
                           strcpy (Mail->ToAddress, Cfg->MailAddress.String);
                           strcpy (Mail->FromAddress, Cfg->MailAddress.String);
                           Mail->Sent = TRUE;
                           if (Msg != NULL)
                              Msg->Add (Mail);

                           Mail->Delete (Number);
                           MsgTossed++;
                        }
                     } while (Mail->Next (Number) == TRUE);
                  }
                  delete Mail;
               }
            }
         } while (User->Next () == TRUE);
      delete User;
   }

   if (Status != NULL)
      Status->Clear ();

   if (Msg != NULL) {
      delete Msg;
      Msg = NULL;
   }

#if !defined(__POINT__)
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

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->First () == TRUE)
         do {
            if (Nodes->ImportPOP3Mail == TRUE && Nodes->InetAddress[0] != '\0') {
               if (Status != NULL) {
                  sprintf (Temp, "Receiving e-mail for %s (%s)", Nodes->Address, Nodes->InetAddress);
                  Status->SetLine (0, Temp);
               }
               if ((p = strchr (Nodes->InetAddress, '@')) != NULL) {
                  *p = '\0';
                  strcpy (Name, Nodes->InetAddress);
                  strcpy (Host, &p[1]);
                  *p = '@';
               }
               if ((Mail = new INETMAIL (Host, Name, Nodes->Pop3Pwd)) != NULL) {
                  if (Mail->Number () > 0L) {
                     if (Log != NULL)
                        Log->Write ("+Receiving e-mail for %s (%s)", Nodes->Address, Nodes->InetAddress);
                     Number = Mail->Lowest ();
                     do {
                        if (Status != NULL) {
                           sprintf (Temp, "%6lu / %-6lu", Number, Mail->Number ());
                           Status->SetLine (1, Temp);
                        }
                        if (Mail->Read (Number, MAX_LINE_LENGTH) == TRUE) {
                           if (Log != NULL) {
                              Log->Write (":  Msg#: %lu", Number);
                              Log->Write (":    Fm: %s", Mail->From);
                              Log->Write (":    Sb: %s", Mail->Subject);
                           }
                           strcpy (Mail->To, Nodes->SysopName);
                           strcpy (Mail->ToAddress, Nodes->Address);
                           if (Cfg->MailAddress.First () == TRUE) {
                              strcpy (Mail->FromAddress, Cfg->MailAddress.String);
                              do {
                                 if (Cfg->MailAddress.Zone == Nodes->Zone) {
                                    strcpy (Mail->FromAddress, Cfg->MailAddress.String);
                                    break;
                                 }
                              } while (Cfg->MailAddress.Next () == TRUE);
                           }
                           Mail->Sent = FALSE;
                           Mail->Private = TRUE;
                           if (Msg != NULL)
                              Msg->Add (Mail);

                           // Il messaggio viene aggiunto all'email solo se i path
                           // delle aree email e netmail sono diversi.
                           if (stricmp (Cfg->MailPath, Cfg->NetMailPath)) {
                              // Se il Sysop del BBS/Point e' anche nostro utente, inserise il
                              // mail nella base message email locale.
                              if ((User = new TUser (Cfg->UserFile)) != NULL) {
                                 if (User->GetData (Msg->To, FALSE) == TRUE) {
                                    EMail = NULL;
                                    if (Cfg->MailStorage == ST_JAM)
                                       EMail = new JAM (Cfg->MailPath);
                                    else if (Cfg->MailStorage == ST_SQUISH)
                                       EMail = new SQUISH (Cfg->MailPath);
                                    else if (Cfg->MailStorage == ST_FIDO)
                                       EMail = new FIDOSDM (Cfg->MailPath);
                                    else if (Cfg->MailStorage == ST_ADEPT)
                                       EMail = new ADEPT (Cfg->MailPath);
                                    else if (Cfg->MailStorage == ST_HUDSON)
                                       EMail = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->MailBoard);

                                    if (EMail != NULL) {
                                       EMail->Add (Mail);
                                       delete EMail;
                                    }
                                 }
                                 delete User;
                              }
                           }

                           Mail->Delete (Number);
                           MsgTossed++;
                        }
                     } while (Mail->Next (Number) == TRUE);
                  }

                  Mail->Close ();
                  delete Mail;
               }
            }
         } while (Nodes->Next () == TRUE);
      delete Nodes;
   }

   if (Msg != NULL) {
      delete Msg;
      Msg = NULL;
   }

   if (Status != NULL)
      Status->Clear ();
#endif
}

// ----------------------------------------------------------------------------
// Nodelist compiler
// ----------------------------------------------------------------------------

#define isLeap(x) ((x)%1000)?((((x)%100)?(((x)%4)?0:1):(((x)%400)?0:1))):(((x)%4000)?1:0)

USHORT UpdateNodelist (PSZ name, PSZ diff, class TStatus *Status)
{
   FILE *fps, *fpd, *fpn;
   DIR *dir;
   SHORT i, max, next, m, delay = 0;
   CHAR Temp[256], Path[64], String[32], *p;
   ULONG Perc, OldPerc = 101;
   time_t t;
   struct dirent *ent;
   struct tm *ltm;

   t = time (NULL);
   ltm = localtime (&t);

   strcpy (Path, Cfg->NodelistPath);
   p = strchr (Path, '\0');
   while (p > Path && *p != '\\' && *p != '/')
      p--;
   *p = '\0';

   max = -1;
   if ((dir = opendir (AdjustPath (Path))) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
         if ((p = strchr (ent->d_name, '.')) != NULL) {
            *p++ = '\0';
            if (!stricmp (ent->d_name, name)) {
               i = (SHORT)atoi (p);
               if (i > max)
                  max = i;
            }
         }
      }
      closedir (dir);
   }

   if (max != -1) {
      next = (SHORT)(max + 7);
      if ((isLeap (ltm->tm_year - 1)) && next > 366)
         next -= 366;
      else if (!(isLeap (ltm->tm_year - 1)) && next > 365)
         next -= 365;

      if (diff != NULL && *diff != '\0') {
         sprintf (Temp, "%s\\%s.%03d", Path, diff, next);
         if ((fps = fopen (AdjustPath (Temp), "rt")) != NULL) {
            sprintf (Temp, "%s\\%s.%03d", Path, name, max);
            fpn = fopen (AdjustPath (Temp), "rt");

            sprintf (Temp, "%s\\%s.%03d", Path, name, next);
            fpd = fopen (AdjustPath (Temp), "wt");

#if !defined(__POINT__)
            if (Log != NULL)
               Log->Write (":  Updating %s.%03d with %s.%03d", name, max, diff, next);
#endif
            if (Status != NULL) {
               sprintf (Temp, "Updating %s.%03d with %s.%03d", name, max, diff, next);
               Status->SetLine (0, Temp);
               Status->SetLine (1, "");
            }

            if (fpn != NULL && fpd != NULL) {
               fgets (Temp, sizeof (Temp) - 1, fps);

               while (fgets (Temp, sizeof (Temp) - 1, fps) != NULL) {
                  while (Temp[strlen (Temp) -1] == 0x0D || Temp[strlen (Temp) -1] == 0x0A || Temp[strlen (Temp) -1] == ' ')
                     Temp[strlen (Temp) -1] = '\0';

                  if (Status != NULL) {
                     Perc = (ftell (fps) * 100L) / filelength (fileno (fps));
                     if (Perc != OldPerc) {
                        sprintf (String, "     %02lu%%", Perc);
                        Status->SetLine (1, String);
                        OldPerc = Perc;
                     }
                  }

                  if (Temp[0] == 'A' && isdigit (Temp[1])) {
                     m = (SHORT)atoi (&Temp[1]);
                     for (i = 0; i < m; i++) {
                        fgets (Temp, sizeof (Temp) - 1, fps);
                        fputs (Temp, fpd);
                     }
                  }
                  else if (Temp[0] == 'C' && isdigit (Temp[1])) {
                     m = (SHORT)atoi (&Temp[1]);
                     for (i = 0; i < m; i++) {
                        fgets (Temp, sizeof (Temp) - 1, fpn);
                        fputs (Temp, fpd);
                     }
                  }
                  else if (Temp[0] == 'D' && isdigit (Temp[1])) {
                     m = (SHORT)atoi (&Temp[1]);
                     for (i = 0; i < m; i++)
                        fgets (Temp, sizeof (Temp) - 1, fpn);
                  }

#if defined(__OS2__)
                  if ((delay++ % 16) == 0)
                     DosSleep (1L);
#elif defined(__NT__)
                  if ((delay++ % 16) == 0)
                     Sleep (1L);
#endif
               }

               if (fpn != NULL)
                  fclose (fpn);
               if (fpd != NULL)
                  fclose (fpd);

               fclose (fps);

               sprintf (Temp, "%s\\%s.%03d", Path, name, max);
               unlink (AdjustPath (Temp));
               sprintf (Temp, "%s\\%s.%03d", Path, diff, next);
               unlink (AdjustPath (Temp));
               max = next;
            }
         }
      }
   }

   return (max);
}

VOID CompileNodelist (USHORT force)
{
   FILE *fps;
   int fd;
   USHORT nzone, nnet, nnode;
   SHORT i, m, Build = force;
   CHAR Temp[256], *p;
   ULONG hdrpos, entrypos, records, totalnodes;
   IDXHEADER idx;
   NODEIDX nidx;
   struct stat statbuf, statidx;
   class TNodes *Nodes;
   class TAddress Addr;
   class TStatus *Status;

   nzone = 2;
   nnet = 0;
   if (Cfg->MailAddress.First () == TRUE) {
      nzone = Cfg->MailAddress.Zone;
      nnet = Cfg->MailAddress.Net;
   }
   records = 0L;
   totalnodes = 0L;

#if !defined(__POINT__)
#if defined(__OS2__) || defined(__NT__)
   if ((Status = new TPMStatus (hwndMainClient)) != NULL)
      Status->Clear ();
#else
   if ((Status = new TPMStatus) != NULL)
      Status->Clear ();
#endif
#endif
   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL)
      Nodes->LoadNodelist ();

   if (Build == FALSE) {
      sprintf (Temp, "%snodes.idx", Cfg->NodelistPath);
      if (stat (Temp, &statidx) != 0)
         Build = TRUE;

      if (Nodes->FirstNodelist () == TRUE) {
         do {
            if (Nodes->Nodelist[1] == ':' || Nodes->Nodelist[0] == '\\')
               strcpy (Temp, Nodes->Nodelist);
            else
               sprintf (Temp, "%s%s", Cfg->NodelistPath, Nodes->Nodelist);
            if (stat (Temp, &statbuf) == 0) {
               if (statbuf.st_mtime > statidx.st_mtime)
                  Build = TRUE;
            }
         } while (Nodes->NextNodelist () == TRUE);
      }

      sprintf (Temp, "%snodes.idx", Cfg->NodelistPath);
      if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &idx, sizeof (IDXHEADER)) == sizeof (IDXHEADER)) {
            if (idx.Name[1] == ':' || idx.Name[0] == '\\')
               strcpy (Temp, idx.Name);
            else
               sprintf (Temp, "%s%s", Cfg->NodelistPath, idx.Name);
            if (stat (Temp, &statbuf) == 0) {
               if (statbuf.st_mtime > statidx.st_mtime) {
                  Build = TRUE;
                  break;
               }
            }
            else {
               Build = TRUE;
               break;
            }

            lseek (fd, idx.Entry * sizeof (NODEIDX), SEEK_CUR);
         }
         close (fd);
      }
   }

   if (Build == TRUE) {
#if !defined(__POINT__)
      if (Log != NULL)
         Log->Write (":Rebuild nodelist index");
#endif

      sprintf (Temp, "%snodes.idx", Cfg->NodelistPath);
      if ((fd = open (Temp, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
         if (Nodes->FirstNodelist () == TRUE)
            do {
               nzone = 2;
               if (Cfg->MailAddress.First () == TRUE)
                  nzone = Cfg->MailAddress.Zone;
               if (Nodes->DefaultZone != 0)
                  nzone = Nodes->DefaultZone;

               if (strchr (Nodes->Nodelist, '.') == NULL) {
                  if ((i = UpdateNodelist (Nodes->Nodelist, Nodes->Nodediff, Status)) != -1) {
                     for (;;) {
                        m = UpdateNodelist (Nodes->Nodelist, Nodes->Nodediff, Status);
                        if (m == i)
                           break;
                        i = m;
                     }
                     sprintf (Temp, ".%03d", i);
                     strcat (Nodes->Nodelist, Temp);
                  }
               }

#if !defined(__POINT__)
               if (Log != NULL)
                  Log->Write ("+Compiling %s", Nodes->Nodelist);
#endif
               if (Status != NULL) {
                  sprintf (Temp, "Compiling %s", Nodes->Nodelist);
                  Status->SetLine (0, Temp);
               }

               if (Nodes->Nodelist[1] == ':' || Nodes->Nodelist[0] == '\\')
                  strcpy (Temp, Nodes->Nodelist);
               else
                  sprintf (Temp, "%s%s", Cfg->NodelistPath, Nodes->Nodelist);
               if ((fps = fopen (Temp, "rt")) != NULL) {
                  memset (&idx, 0, sizeof (IDXHEADER));
                  strcpy (idx.Name, Nodes->Nodelist);
                  hdrpos = tell (fd);
                  write (fd, &idx, sizeof (IDXHEADER));

                  entrypos = ftell (fps);
                  while (fgets (Temp, sizeof (Temp) - 1, fps) != NULL) {
                     if (Temp[0] == ';') {
                        entrypos = ftell (fps);
                        continue;
                     }

                     if (strnicmp (Temp, "Down,", 5))
                        totalnodes++;

                     p = strtok (Temp, ",");
                     if (stricmp (p, "Boss") && stricmp (p, "Zone") && stricmp (p, "Region") && stricmp (p, "Host")) {
                        entrypos = ftell (fps);
                        continue;
                     }

                     nnode = 0;
                     if (!stricmp (p, "Boss")) {
                        p = strtok (NULL, ",");
                        Addr.Parse (p);
                        nzone = Addr.Zone;
                        nnet = Addr.Net;
                        nnode = Addr.Node;
                     }
                     else if (!stricmp (p, "Zone")) {
                        p = strtok (NULL, ",");
                        nzone = nnet = (USHORT)atoi (p);
                     }
                     else {
                        p = strtok (NULL, ",");
                        nnet = (USHORT)atoi (p);
                     }

                     if ((totalnodes % 16) == 0L && Status != NULL) {
                        sprintf (Temp, "%5u:%-5u Total: %lu", nzone, nnet, totalnodes);
                        Status->SetLine (1, Temp);
#if defined(__OS2__)
                        DosSleep (1L);
#elif defined(__NT__)
                        Sleep (1L);
#endif
                     }

                     nidx.Zone = nzone;
                     nidx.Net = nnet;
                     nidx.Node = nnode;
                     nidx.Position = entrypos;
                     write (fd, &nidx, sizeof (NODEIDX));
                     idx.Entry++;
                     records++;
                     entrypos = ftell (fps);
                  }

                  fclose (fps);

                  lseek (fd, hdrpos, SEEK_SET);
                  write (fd, &idx, sizeof (IDXHEADER));
                  lseek (fd, 0L, SEEK_END);
               }
            } while (Nodes->NextNodelist () == TRUE);
         close (fd);
      }

#if !defined(__POINT__)
      if (Log != NULL)
         Log->Write ("+%lu total nodes, %lu record(s) written", totalnodes, records);
#endif
   }

   if (Nodes != NULL)
      delete Nodes;
   if (Status != NULL) {
      Status->Clear ();
      delete Status;
   }
}

