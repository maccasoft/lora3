
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
PSZ Exts[] = { "su0", "mo0", "tu0", "we0", "th0", "fr0", "sa0", NULL };

// ----------------------------------------------------------------------------
// Mail processor output filter
// ----------------------------------------------------------------------------

#if defined(__OS2__) || defined(__NT__)
TPMList::TPMList (HWND hwnd)
{
   time_t t;
   struct tm *ltm;

   hwndList = hwnd;
   if ((fp = fopen ("process.log", "at")) != NULL) {
      t = time (NULL);
      ltm = localtime (&t);
      fprintf (fp, "\nProcessing date/time: %s", asctime (ltm));
      fprintf (fp, "-------------------------------------------------------------------------------\n");
   }
   DoWrite = FALSE;
}
#elif defined(__LINUX__) || defined(__DOS__)
TPMList::TPMList (void)
{
   time_t t;
   struct tm *ltm;

   window = wopen (13, 1, 20, 78, 5, LGREY|_BLACK, LCYAN|_BLACK);
   First = TRUE;
   if ((fp = fopen ("process.log", "at")) != NULL) {
      t = time (NULL);
      ltm = localtime (&t);
      fprintf (fp, "\nProcessing date/time: %s", asctime (ltm));
      fprintf (fp, "-------------------------------------------------------------------------------\n");
   }
   DoWrite = FALSE;
}
#endif

TPMList::~TPMList (void)
{
#if defined(__DOS__) || defined(__LINUX__)
   if (window != 0) {
      wactiv (window);
      wclose ();
   }
#endif
   if (fp != NULL) {
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
   if (fp != NULL) {
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
   if (fp != NULL) {
      strcpy (Line, Text);
      DoWrite = TRUE;
   }
}

VOID TPMList::Clear (VOID)
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

// ----------------------------------------------------------------------------
// Mail processor
// ----------------------------------------------------------------------------

TMailProcessor::TMailProcessor (void)
{
   Output = NULL;
   Status = NULL;
   Packets = 0;
   Bad = Duplicate = NetMail = MsgTossed = 0L;
}

TMailProcessor::~TMailProcessor (void)
{
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
         if ((ToFlag = (CHAR)toupper (*p)) == 'n')
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

VOID TMailProcessor::Export (VOID)
{
   USHORT i, UseEchotoss;
   ULONG Number, SentArea, Highest, Msgn;
   class TEchotoss *Echo;

   UseEchotoss = FALSE;
   Data = new TMsgData (Cfg->SystemPath);
   Forward = new TEchoLink (Cfg->SystemPath);
   Dupes = new TDupes (Cfg->SystemPath);

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
               Dupes->Load (Data->EchoTag);

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

                  Forward->Load (Data->EchoTag);
                  if (Forward->First () == TRUE) {
                     SentArea = 0L;
                     if (Msg->Next (Number) == TRUE) {
                        do {
                           Msgn = Msg->UidToMsgn (Number);
                           if (Status != NULL && (Msgn % 10L) == 0L)
                              Status->SetLine (1, "   %lu / %lu", Msgn, Msg->Number ());
                           i = ExportEchoMail (Number, Data->EchoTag);
                           SentArea += i;
                           MsgSent += i;
#if defined(__OS2__)
                           DosSleep (1L);
#elif defined(__NT__)
                           Sleep (1L);
#endif
                        } while (Msg->Next (Number) == TRUE);

                        if (Log != NULL && SentArea != 0L)
                           Log->Write (":   %-20.20s (Sent=%04lu)", Data->EchoTag, SentArea);
                     }
                  }
                  else
                     Number = Msg->Highest ();

                  Msg->SetHWM (Number);
                  Data->HighWaterMark = Number;
                  Data->Update ();

                  Msg->UnLock ();
                  delete Msg;
               }

               Dupes->Save ();
            } while (Echo->Next () == TRUE);
         }
         delete Echo;
      }

      if (UseEchotoss == FALSE && Data->First () == TRUE)
         do {
            if (Data->EchoMail == TRUE && Data->EchoTag[0] != '\0' && Data->Storage != ST_PASSTHROUGH) {
               Dupes->Load (Data->EchoTag);

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

                  Forward->Load (Data->EchoTag);
                  if (Forward->First () == TRUE) {
                     SentArea = 0L;
                     if (Msg->Next (Number) == TRUE) {
                        do {
                           if (Status != NULL)
                              Status->SetLine (1, "   %lu / %lu", Number, Highest);
                           i = ExportEchoMail (Number, Data->EchoTag);
                           SentArea += i;
                           MsgSent += i;
#if defined(__OS2__)
                           DosSleep (1L);
#elif defined(__NT__)
                           Sleep (1L);
#endif
                        } while (Msg->Next (Number) == TRUE);

                        if (Log != NULL && SentArea != 0L)
                           Log->Write (":   %-20.20s (Sent=%04lu)", Data->EchoTag, SentArea);
                     }
                  }
                  else
                     Number = Msg->Highest ();

                  Msg->SetHWM (Number);
                  Data->HighWaterMark = Number;
                  Data->Update ();

                  Msg->UnLock ();
                  delete Msg;
               }

               Dupes->Save ();
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

   if (Dupes != NULL)
      delete Dupes;
   if (Forward != NULL)
      delete Forward;
   if (Data != NULL)
      delete Data;
}

USHORT TMailProcessor::ExportEchoMail (ULONG Number, PSZ pszEchoTag)
{
   USHORT SentArea = 0, First = TRUE;
   CHAR *Text, *Base;
   ULONG Msgn;
   struct stat statbuf;
   class TAddress ToAddr;
   class TKludges *SeenBy, *Path;
   class PACKET *Packet;

   Base = "???";
   if (Data->Storage == ST_SQUISH)
      Base = "Squish<tm>";
   else if (Data->Storage == ST_JAM)
      Base = "JAM";
   else if (Data->Storage == ST_FIDO)
      Base = "Fido *.MSG";
   else if (Data->Storage == ST_ADEPT)
      Base = "AdeptXBBS";
#if !defined(__POINT__)
   else if (Data->Storage == ST_PASSTHROUGH)
      Base = "Passthrough";
   else if (Data->Storage == ST_USENET)
      Base = "Newsgroup";
#endif

   if (Msg != NULL && (Data->Storage != ST_FIDO || Number != 1L)) {
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

                     SentArea++;
                  }
               }
            } while (Forward->Next () == TRUE);

         if (Dupes != NULL) {
            if (Dupes->Check (Data->EchoTag, Msg) == FALSE)
               Dupes->Add (Data->EchoTag, Msg);
         }
      }
   }

   return (SentArea);
}

VOID TMailProcessor::ExportNetMail (VOID)
{
   USHORT OurPoint, FoundIntl;
   CHAR *Text, *Base, PktFile[128];
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

/*
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
*/
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
#if defined(__OS2__)
         DosSleep (1L);
#elif defined(__NT__)
         Sleep (1L);
#endif
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

   Msg = NULL;
   TossedArea = 0L;
   Packet = NULL;
   BadArea = FALSE;

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
#if defined(__OS2__)
            DosSleep (1L);
#elif defined(__NT__)
            Sleep (1L);
#endif
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

ULONG TMailProcessor::ImportEchoMail (PSZ EchoTag)
{
   USHORT BadMessage = FALSE, DupeMessage = FALSE;
   CHAR *Text, *Base;
   ULONG RetVal = 0L;
   class TMsgBase *Extra = NULL;
   class TKludges *SeenBy, *Path;

   Base = "";

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
      if (Forward->Check (Packet->FromAddress) == FALSE && Cfg->Secure == TRUE) {
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
      }
   }

   if (BadMessage == FALSE && Forward != NULL) {
      if (Forward->Check (Packet->FromAddress) == TRUE) {
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
         }
      }
   }

   if (BadMessage == FALSE && Dupes != NULL) {
      if (Dupes->Check (Data->EchoTag, Packet) == TRUE) {
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
      }
      else
         Dupes->Add (Data->EchoTag, Packet);
   }

   if (Msg != NULL) {
      if (BadMessage == FALSE && DupeMessage == FALSE) {
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

      if (Data->Storage == ST_PASSTHROUGH && BadMessage == FALSE && DupeMessage == FALSE)
         ExportEchoMail (Packet->Current + 1L, Data->EchoTag);
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
      }
      sprintf (Display, "%6lu %-32.32s %-12.12s %6lu", Packet->Current + 1L, LastTag, Base, Msg->UidToMsgn (RetVal));
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


VOID TMailProcessor::News (VOID)
{
   CHAR *Base;
   ULONG Number, Tossed, SentArea, Msgn;
   class USENET *Usenet;
   class TUser *User;

   if (Output != NULL)
      Output->Clear ();
   if (Log != NULL)
      Log->Write ("+Processing NEWSgroups");

   MsgTossed = MsgSent = 0L;

   if (ValidateKey ("bbs", NULL, NULL) == KEY_BASIC) {
      if (Log != NULL) {
         Log->Write ("!WARNING: This function is available in the");
         Log->Write ("!         ADVANCED and PROFESSIONAL versions only");
      }
      return;
   }

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

               if (Msg != NULL) {
                  if (Status != NULL)
                     Status->SetLine (0, "Processing %s ", Data->NewsGroup);

                  if ((Usenet = new USENET (Cfg->NewsServer, Data->NewsGroup)) != NULL) {
                     strcpy (Usenet->HostName, Cfg->HostName);
                     strcpy (Usenet->Organization, Cfg->SystemName);
                     sprintf (Usenet->ProgramID, "%s Version %s", NAME, VERSION);

                     if (Status != NULL)
                        Status->SetLine (0, "Sending messages to %s", Data->NewsGroup);

                     Msg->Lock (0L);
                     Number = Msg->Lowest ();
                     SentArea = 0L;

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
#if defined(__OS2__)
                        DosSleep (1L);
#elif defined(__NT__)
                        Sleep (1L);
#endif
                     } while (Msg->Next (Number) == TRUE);

                     Msg->UnLock ();

                     Tossed = 0L;
                     Number = Data->Highest;
                     if (Status != NULL)
                        Status->SetLine (0, "Receiving message from %s (%lu msgs)", Data->NewsGroup, Usenet->Number ());

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
                        }
#if defined(__OS2__)
                        DosSleep (1L);
#elif defined(__NT__)
                        Sleep (1L);
#endif
                     }

                     Data->Highest = Number;

                     if (Log != NULL && (Tossed != 0L || SentArea != 0L))
                        Log->Write (":   %-20.20s (Toss=%04d,Sent=%04lu)", Data->NewsGroup, Tossed, SentArea);

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

USHORT TMailProcessor::OpenArea (PSZ pszEchoTag)
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
         Data->ReadEcho (pszEchoTag);

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
                     Log->Write ("#Unpacking %s", Filename);
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

VOID TMailProcessor::Mail (VOID)
{
   CHAR Host[64], Name[32], *p, ToUs;
   ULONG Number, Msgn;
   class TUser *User;
   class INETMAIL *Mail;
   class TAddress Addr;

   if (Status != NULL)
      Status->Clear ();
   if (Log != NULL)
      Log->Write ("+Processing Internet Mail");

   MsgSent = MsgTossed = 0L;

   if (ValidateKey ("bbs", NULL, NULL) == KEY_BASIC) {
      if (Log != NULL) {
         Log->Write ("!WARNING: This function is available in the");
         Log->Write ("!         ADVANCED and PROFESSIONAL versions only");
      }
      return;
   }

#if !defined(__POINT__)
   Msg = NULL;
   if (Cfg->MailStorage == ST_JAM)
      Msg = new JAM (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_SQUISH)
      Msg = new SQUISH (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_FIDO)
      Msg = new FIDOSDM (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_ADEPT)
      Msg = new ADEPT (Cfg->MailPath);

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
                     if (Nodes->Read (Msg->FromAddress, FALSE) == FALSE) {
                        if (Nodes->UseInetAddress == TRUE && Nodes->InetAddress[0] != '\0')
                           strcpy (Msg->FromAddress, Nodes->InetAddress);
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
#else
   if (Cfg->MailStorage == ST_JAM)
      Msg = new JAM (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_SQUISH)
      Msg = new SQUISH (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_FIDO)
      Msg = new FIDOSDM (Cfg->MailPath);
   else if (Cfg->MailStorage == ST_ADEPT)
      Msg = new ADEPT (Cfg->MailPath);
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

USHORT UpdateNodelist (PSZ name, PSZ diff)
{
   FILE *fps, *fpd, *fpn;
   DIR *dir;
   SHORT i, max, next, m;
   CHAR Temp[256], Path[64], *p;
   time_t t;
   struct dirent *ent;
   struct tm *ltm;

   t = time (NULL);
   ltm = localtime (&t);

   strcpy (Path, Cfg->NodelistPath);
   p = strchr (Path, '\0');
   while (p > Path && *p != '\\')
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
         if ((fps = fopen (Temp, "rt")) != NULL) {
            sprintf (Temp, "%s\\%s.%03d", Path, name, max);
            fpn = fopen (Temp, "rt");

            sprintf (Temp, "%s\\%s.%03d", Path, name, next);
            fpd = fopen (Temp, "wt");

            if (fpn != NULL && fpd != NULL) {
               fgets (Temp, sizeof (Temp) - 1, fps);

               while (fgets (Temp, sizeof (Temp) - 1, fps) != NULL) {
                  while (Temp[strlen (Temp) -1] == 0x0D || Temp[strlen (Temp) -1] == 0x0A || Temp[strlen (Temp) -1] == ' ')
                     Temp[strlen (Temp) -1] = '\0';

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
               }

               if (fpn != NULL)
                  fclose (fpn);
               if (fpd != NULL)
                  fclose (fpd);

               fclose (fps);

               sprintf (Temp, "%s\\%s.%03d", Path, name, max);
               unlink (Temp);
               sprintf (Temp, "%s\\%s.%03d", Path, diff, next);
               unlink (Temp);
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
                  if ((i = UpdateNodelist (Nodes->Nodelist, Nodes->Nodediff)) != -1) {
                     do {
                        m = UpdateNodelist (Nodes->Nodelist, Nodes->Nodediff);
                     } while (m != -1 && m != i);
                     if (m != -1)
                        i = m;
                     sprintf (Temp, ".%03d", i);
                     strcat (Nodes->Nodelist, Temp);
                  }
               }

#if !defined(__POINT__)
               if (Log != NULL)
                  Log->Write ("+  Compiling %s", Nodes->Nodelist);
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

                     if ((totalnodes % 10) == 0L && Status != NULL) {
                        sprintf (Temp, "%5u:%-5u Total: %lu", nzone, nnet, totalnodes);
                        Status->SetLine (1, Temp);
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

