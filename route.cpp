
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/12/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "mail.h"

PSZ Exts[] = { "su0", "mo0", "tu0", "we0", "th0", "fr0", "sa0", NULL };

TRoute::TRoute (void)
{
   Cfg = NULL;
   Log = NULL;
}

TRoute::~TRoute (void)
{
}

VOID TRoute::MakeArcMailName (PSZ pszAddress, CHAR Flag)
{
   USHORT i, Found;
   CHAR *p;
   class TAddress Addr;
   class TOutbound *Out;
   struct stat statbuf;
   struct dosdate_t d_date;

   _dos_getdate (&d_date);

   Addr.Parse (pszAddress);
   if (Addr.Zone == 0 || Cfg->MailAddress.Zone == Addr.Zone) {
      if (Addr.Point != 0)
         sprintf (ArcMailName, "%s\\%04x%04x.pnt\\%08lx.%s", Outbound, Addr.Net, Addr.Node, StringCrc32 (Addr.String, 0xFFFFFFFFL), Exts[d_date.dayofweek]);
      else
         sprintf (ArcMailName, "%s\\%08lx.%s", Outbound, StringCrc32 (Addr.String, 0xFFFFFFFFL), Exts[d_date.dayofweek]);
   }
   else {
      if (Addr.Point != 0)
         sprintf (ArcMailName, "%s.%03x\\%04x%04x.pnt\\%08lx.%s", Outbound, Addr.Zone, Addr.Net, Addr.Node, StringCrc32 (Addr.String, 0xFFFFFFFFL), Exts[d_date.dayofweek]);
      else
         sprintf (ArcMailName, "%s.%03x\\%08lx.%s", Outbound, Addr.Zone, StringCrc32 (Addr.String, 0xFFFFFFFFL), Exts[d_date.dayofweek]);
   }

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

VOID TRoute::SendTo (VOID)
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
         if ((dir = opendir (Temp)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
               strlwr (ent->d_name);
               if (strstr (ent->d_name, ".xpr") != NULL) {
                  sscanf (ent->d_name, "%08hx", &Point);

                  cprintf ("SEND: %s for %u:%u/%u.%u... ", FlagStr, Zone, Net, Node, Point);
                  if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                     sprintf (Temp, "%s\\%04x%04x.pnt\\%08x.xpr", Outbound, Net, Node, Point);
                  else
                     sprintf (Temp, "%s.%03x\\%04x%04x.pnt\\%08x.xpr", Outbound, Zone, Net, Node, Point);

                  if ((fd = sopen (Temp, O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                     close (fd);
                     do {
                        if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                           sprintf (Name, "%s\\%04x%04x.pnt\\%08lx.pkt", Outbound, Net, Node, time (NULL));
                        else
                           sprintf (Name, "%s.%03x\\%04x%04x.pnt\\%08lx.pkt", Outbound, Zone, Net, Node, time (NULL));
                     } while (rename (Temp, Name) != 0);

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
                        cprintf ("Using: '%s'\r\n", Packer->Display);
                        stat (Name, &statbuf);
                        if (Log != NULL)
                           Log->Write ("#Packing mail for %s (%lu bytes)", Addr.String, statbuf.st_size);
                        if (Log != NULL)
                           Log->Write ("#Executing %s", Packer->PackCmd);
                        cprintf ("Adding %s (%lub) to archive '%s'\r\n", &Name[strlen (Name) - 12], statbuf.st_size, ArcMailName);
                        Packer->DoPack (ArcMailName, Name);
                        delete Packer;
                     }
                  }
                  cprintf ("\r\n");
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
                     cprintf ("SEND: %s for %u:%u/%u.%u... ", FlagStr, Zone, Net, Node, Point);
                     if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                        sprintf (Temp, "%s\\%04x%04x.xpr", Outbound, Net, Node);
                     else
                        sprintf (Temp, "%s.%03x\\%04x%04x.xpr", Outbound, Zone, Net, Node);

                     if ((fd = sopen (Temp, O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                        close (fd);
                        do {
                           if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                              sprintf (Name, "%s\\%08lx.pkt", Outbound, time (NULL));
                           else
                              sprintf (Name, "%s.%03x\\%08lx.pkt", Outbound, Zone, time (NULL));
                        } while (rename (Temp, Name) != 0);

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
                           cprintf ("Using: '%s'\r\n", Packer->Display);
                           stat (Name, &statbuf);
                           if (Log != NULL)
                              Log->Write ("#Packing mail for %s (%lu bytes)", Addr.String, statbuf.st_size);
                           if (Log != NULL)
                              Log->Write ("#Executing %s", Packer->PackCmd);
                           cprintf ("Adding %s (%lub) to archive '%s'\r\n", &Name[strlen (Name) - 12], statbuf.st_size, ArcMailName);
                           Packer->DoPack (ArcMailName, Name);
                           delete Packer;
                        }
                     }
                     cprintf ("\r\n");
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

         if ((fd = sopen (Temp, O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
            close (fd);
            cprintf ("SEND: %s for %u:%u/%u.%u... ", FlagStr, Zone, Net, Node, Point);
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
            } while (rename (Temp, Name) != 0);

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
               cprintf ("Using: '%s'\r\n", Packer->Display);
               stat (Name, &statbuf);
               if (Log != NULL)
                  Log->Write ("#Packing mail for %s (%lu bytes)", Addr.String, statbuf.st_size);
               if (Log != NULL)
                  Log->Write ("#Executing %s", Packer->PackCmd);
               cprintf ("Adding %s (%lub) to archive '%s'\r\n", &Name[strlen (Name) - 12], statbuf.st_size, ArcMailName);
               Packer->DoPack (ArcMailName, Name);
               delete Packer;
            }

            cprintf ("\r\n");
         }
      }
   }
}

VOID TRoute::RouteTo (VOID)
{
   DIR *dir;
   int fd;
   USHORT Zone, Net, Node, Point, DoPack, CheckNet;
   CHAR *p, Flag, Lookup[32], DestPath[128], DestAddr[32], FlagStr[32];
   ULONG TotalSize;
   class TAddress Addr;
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

      sprintf (DestAddr, "%u:%u/%u.%u", Zone, Net, Node, Point);
      MakeArcMailName (DestAddr, Flag);

      if (Point != 0)
         sprintf (Temp, "%s%08x.xpr", DestPath, Point);
      else
         sprintf (Temp, "%s%04x%04x.xpr", DestPath, Net, Node);

      if ((fd = sopen (Temp, O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         close (fd);
         cprintf ("ROUTE: %s %u:%u/%u.%u Via %s\r\n", FlagStr, Zone, Net, Node, Point, DestAddr);
         do {
            sprintf (Name, "%s%08lx.pkt", DestPath, time (NULL));
         } while (rename (Temp, Name) != 0);

         if (stat (Name, &statbuf) == 0)
            TotalSize += statbuf.st_size;
         cprintf ("Adding %s (%lub) to archive '%s'\r\n", &Name[strlen (Name) - 12], statbuf.st_size, ArcMailName);
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
         if ((dir = opendir (Temp)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
               strlwr (ent->d_name);
               if (strstr (ent->d_name, ".xpr") != NULL) {
                  sscanf (ent->d_name, "%08hx", &Point);

                  if (Zone == 0 || Cfg->MailAddress.Zone == Zone)
                     sprintf (Temp, "%s\\%04x%04x.pnt\\%08lx.xpr", Outbound, Net, Node, Point);
                  else
                     sprintf (Temp, "%s.%03x\\%04x%04x.pnt\\%08lx.xpr", Outbound, Zone, Net, Node, Point);

                  if ((fd = sopen (Temp, O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                     close (fd);
                     cprintf ("ROUTE: %s %u:%u/%u.%u Via %s\r\n", FlagStr, Zone, Net, Node, Point, DestAddr);
                     do {
                        sprintf (Name, "%s%08lx.pkt", DestPath, time (NULL));
                     } while (rename (Temp, Name) != 0);

                     if (stat (Name, &statbuf) == 0)
                        TotalSize += statbuf.st_size;
                     cprintf ("Adding %s (%lub) to archive '%s'\r\n", &Name[strlen (Name) - 12], statbuf.st_size, ArcMailName);
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

                     if ((fd = sopen (Temp, O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                        close (fd);
                        cprintf ("ROUTE: %s %u:%u/%u Via %s\r\n", FlagStr, Zone, Net, Node, DestAddr);
                        do {
                           sprintf (Name, "%s%08lx.pkt", DestPath, time (NULL));
                        } while (rename (Temp, Name) != 0);

                        if (stat (Name, &statbuf) == 0)
                           TotalSize += statbuf.st_size;
                        cprintf ("Adding %s (%lub) to archive '%s'\r\n", &Name[strlen (Name) - 12], statbuf.st_size, ArcMailName);
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

         if ((fd = sopen (Temp, O_RDONLY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
            close (fd);
            cprintf ("ROUTE: %s %u:%u/%u Via %s\r\n", FlagStr, Zone, Net, Node, DestAddr);
            do {
               sprintf (Name, "%s%08lx.pkt", DestPath, time (NULL));
            } while (rename (Temp, Name) != 0);

            if (stat (Name, &statbuf) == 0)
               TotalSize += statbuf.st_size;
            cprintf ("Adding %s (%lub) to archive '%s'\r\n", &Name[strlen (Name) - 12], statbuf.st_size, ArcMailName);
            DoPack = TRUE;
         }
      }
   }

   if (DoPack == TRUE) {
      MakeArcMailName (DestAddr, Flag);
      sprintf (Name, "%s*.pkt", DestPath);

      if ((Packer = new TPacker (Cfg->SystemPath)) != NULL) {
         Packer->First ();
         if (Packer->CheckArc (ArcMailName) == FALSE) {
            if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
               if (Nodes->Read (DestAddr) == TRUE)
                  Packer->Read (Nodes->Packer);
               delete Nodes;
            }
         }
         if (Log != NULL) {
            Log->Write ("#Packing mail for %s (%lu bytes)", DestAddr, TotalSize);
            Log->Write ("#Executing %s", Packer->PackCmd);
         }
         Packer->DoPack (ArcMailName, Name);
         delete Packer;
      }
   }
}

VOID TRoute::Run (PSZ pszFile)
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

   cprintf ("Searching outbound areas...\r\n");

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
               else if (*p != '\0' && *p != ';' && *p != '%')
                  Log->Write ("!Unknown keyword '%s' on line #%d", p, LineNum);
            }

            LineNum++;
         }
         fclose (fp);
      }
   }
   else {
      cprintf ("No compressor(s) defined. Routing aborted.\r\n");
      if (Log != NULL)
         Log->Write ("!No compressor(s) defined");
   }
}


