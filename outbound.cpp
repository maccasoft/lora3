
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.19
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/22/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "outbound.h"

PSZ ArcFlags[] = { ".mo", ".tu", ".we", ".th", ".fr", ".sa", ".su" };

TOutbound::TOutbound (PSZ pszPath)
{
   DefaultZone = 2;
   Files.Clear ();
   TotalFiles = 0;
   TotalSize = 0L;

   if (pszPath != NULL) {
      strcpy (Path, pszPath);
      if (Path[strlen (Path) - 1] != '\\')
         strcat (Path, "\\");
   }
}

TOutbound::TOutbound (PSZ pszPath, USHORT usZone, USHORT usNet, USHORT usNode, USHORT usPoint, PSZ pszDomain)
{
   DefaultZone = 2;
   Files.Clear ();
   TotalFiles = 0;
   TotalSize = 0L;

   if (pszPath != NULL) {
      strcpy (Path, pszPath);
      if (Path[strlen (Path) - 1] != '\\')
         strcat (Path, "\\");
   }

   Add (usZone, usNet, usNode, usPoint, pszDomain);
}

TOutbound::~TOutbound (void)
{
   Files.Clear ();
   Nodes.Clear ();
}

USHORT TOutbound::Add (VOID)
{
   USHORT RetVal = FALSE;
   OUTFILE Out;

   memset (&Out, 0, sizeof (Out));
   Out.Zone = Zone;
   Out.Net = Net;
   Out.Node = Node;
   Out.Point = Point;
   strcpy (Out.Domain, Domain);
   strcpy (Out.Name, strlwr (Name));
   strcpy (Out.Complete, strlwr (Complete));
   Out.Size = Size;
   Out.ArcMail = ArcMail;
   Out.MailPKT = MailPKT;
   Out.Request = Request;
   Out.Poll = Poll;
   Out.DeleteAfter = DeleteAfter;
   Out.TruncateAfter = TruncateAfter;
   Out.Status = Status;
   if (Files.Add (&Out, sizeof (Out)) == TRUE) {
      if (AddQueue (Out) == TRUE) {
         TotalSize += Out.Size;
         TotalFiles++;
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT TOutbound::Add (USHORT usZone, USHORT usNet, USHORT usNode, USHORT usPoint, PSZ pszDomain)
{
   FILE *fp;
   USHORT i, x, RetVal = FALSE, Readed;
   CHAR FileName[128], String[128], *pFile, Flags[] = { 'H', 'C', 'D', 'F', 'O' };
   OUTFILE Out;
   struct find_t ffblk;

   pszDomain = pszDomain;

// ----------------------------------------------------------------------
// The first thing to do is create an outbound path that is conforming
// with the Bink/Opus outbound directory specifications.
// ----------------------------------------------------------------------
   strcpy (Outbound, Path);
   if (usZone != DefaultZone) {
      Outbound[strlen (Outbound) - 1] = '\0';
      sprintf (String, ".%03x\\", usZone);
      strcat (Outbound, String);
   }

// ----------------------------------------------------------------------
// Check for the mail files for the node address being added
// ----------------------------------------------------------------------
   if (usPoint != 0)
      sprintf (FileName, "%s%04x%04x.PNT\\%08x.HUT", Outbound, usNet, usNode, usPoint);
   else
      sprintf (FileName, "%s%04x%04x.HUT", Outbound, usNet, usNode);

   for (i = 0; i < 5; i++) {
      FileName[strlen (FileName) - 3] = Flags[i];
      if (_dos_findfirst (FileName, 0, &ffblk) == 0) {
         memset (&Out, 0, sizeof (Out));
         Out.Zone = usZone;
         Out.Net = usNet;
         Out.Node = usNode;
         Out.Point = usPoint;
         if (pszDomain != NULL)
            strcpy (Out.Domain, pszDomain);
         strcpy (Out.Name, strlwr (ffblk.name));
         strcpy (Out.Complete, strlwr (FileName));
         Out.Size = ffblk.size;
         Out.MailPKT = TRUE;
         Out.DeleteAfter = TRUE;
         Out.Status = Flags[i];
         if (Files.Add (&Out, sizeof (Out)) == TRUE) {
            if (AddQueue (Out) == TRUE) {
               TotalSize += Out.Size;
               TotalFiles++;
               RetVal = TRUE;
            }
         }
      }
   }

// ----------------------------------------------------------------------
// Check for the file attachments for the node address being added
// ----------------------------------------------------------------------
   if (usPoint != 0)
      sprintf (FileName, "%s%04x%04x.PNT\\%08x.HLO", Outbound, usNet, usNode, usPoint);
   else
      sprintf (FileName, "%s%04x%04x.HLO", Outbound, usNet, usNode);

   for (i = 0; i < 5; i++) {
      FileName[strlen (FileName) - 3] = Flags[i];
      if ((fp = _fsopen (FileName, "rt", SH_DENYNO)) != NULL) {
         Readed = FALSE;
         while (fgets (String, sizeof (String) - 1, fp) != NULL) {
            if (String[strlen (String) - 1] == '\n')
               String[strlen (String) - 1] = '\0';
            pFile = String;
            if (String[0] == '^' || String[0] == '#')
               pFile++;
            if (pFile[0] != '~' && _dos_findfirst (pFile, 0, &ffblk) == 0) {
               memset (&Out, 0, sizeof (Out));
               Out.Zone = usZone;
               Out.Net = usNet;
               Out.Node = usNode;
               Out.Point = usPoint;
               if (pszDomain != NULL)
                  strcpy (Out.Domain, pszDomain);
               strcpy (Out.Name, strlwr (ffblk.name));
               strcpy (Out.Complete, strlwr (pFile));
               Out.Size = ffblk.size;
               if (strstr (Out.Name, ".pk") != NULL)
                  Out.MailPKT = TRUE;
               else {
                  for (x = 0; x < 7 && Out.ArcMail == FALSE; x++) {
                     if (strstr (Out.Name, ArcFlags[x]) != NULL)
                        Out.ArcMail = TRUE;
                  }
               }
               if (String[0] == '^')
                  Out.DeleteAfter = TRUE;
               else if (String[0] == '#')
                  Out.TruncateAfter = TRUE;
               Out.Status = Flags[i];
               if (Files.Add (&Out, sizeof (Out)) == TRUE) {
                  if (AddQueue (Out) == TRUE) {
                     TotalSize += Out.Size;
                     TotalFiles++;
                     RetVal = TRUE;
                     Readed = TRUE;
                  }
               }
            }
         }
         fclose (fp);

         if (Readed == FALSE) {
            memset (&Out, 0, sizeof (Out));
            Out.Zone = usZone;
            Out.Net = usNet;
            Out.Node = usNode;
            Out.Point = usPoint;
            if (pszDomain != NULL)
               strcpy (Out.Domain, pszDomain);
            Out.Status = Flags[i];
            Out.Poll = TRUE;
            if (Files.Add (&Out, sizeof (Out)) == TRUE) {
               if (AddQueue (Out) == TRUE) {
                  TotalSize += Out.Size;
                  TotalFiles++;
                  RetVal = TRUE;
               }
            }
         }
      }
   }

// ----------------------------------------------------------------------
// Check for file requests directed to the node being added
// ----------------------------------------------------------------------
   if (usPoint != 0)
      sprintf (FileName, "%s%04x%04x.PNT\\%08x.REQ", Outbound, usNet, usNode, usPoint);
   else
      sprintf (FileName, "%s%04x%04x.REQ", Outbound, usNet, usNode);

   for (i = 0; i < 4; i++) {
      FileName[strlen (FileName) - 3] = Flags[i];
      if (_dos_findfirst (FileName, 0, &ffblk) == 0) {
         memset (&Out, 0, sizeof (Out));
         Out.Zone = usZone;
         Out.Net = usNet;
         Out.Node = usNode;
         Out.Point = usPoint;
         if (pszDomain != NULL)
            strcpy (Out.Domain, pszDomain);
         strcpy (Out.Name, strlwr (ffblk.name));
         strcpy (Out.Complete, strlwr (FileName));
         Out.Size = ffblk.size;
         Out.Request = TRUE;
         Out.DeleteAfter = TRUE;
         if (Files.Add (&Out, sizeof (Out)) == TRUE) {
            if (AddQueue (Out) == TRUE) {
               TotalSize += Out.Size;
               TotalFiles++;
               RetVal = TRUE;
            }
         }
      }
   }

   return (RetVal);
}

USHORT TOutbound::AddQueue (OUTFILE &Out)
{
   USHORT RetVal = FALSE;
   QUEUE Queue, *Temp;

   if ((Temp = (QUEUE *)Nodes.First ()) != NULL)
      do {
         if (Temp->Zone == Out.Zone && Temp->Net == Out.Net && Temp->Node == Out.Node && Temp->Point == Out.Point && !stricmp (Temp->Domain, Out.Domain)) {
            Temp->Files++;
            Temp->Size += Out.Size;
            if (Out.Status == 'C')
               Temp->Crash = TRUE;
            else if (Out.Status == 'D')
               Temp->Direct = TRUE;
            else if (Out.Status == 'H')
               Temp->Hold = TRUE;
            else if (Out.Status == 'I')
               Temp->Immediate = TRUE;
            else if (Out.Status == 'O' || Out.Status == 'F')
               Temp->Normal = TRUE;
            RetVal = TRUE;
         }
      } while ((Temp = (QUEUE *)Nodes.Next ()) != NULL && RetVal == FALSE);

   if (RetVal == FALSE) {
      memset (&Queue, 0, sizeof (Queue));
      Queue.Zone = Out.Zone;
      Queue.Net = Out.Net;
      Queue.Node = Out.Node;
      Queue.Point = Out.Point;
      strcpy (Queue.Domain, Out.Domain);
      Queue.Files++;
      Queue.Size += Out.Size;
      if (Out.Status == 'C')
         Queue.Crash = TRUE;
      else if (Out.Status == 'D')
         Queue.Direct = TRUE;
      else if (Out.Status == 'H')
         Queue.Hold = TRUE;
      else if (Out.Status == 'I')
         Queue.Immediate = TRUE;
      else if (Out.Status == 'N')
         Queue.Normal = TRUE;
      if (Nodes.Add (&Queue, sizeof (Queue)) == TRUE)
         RetVal = TRUE;
   }

   return (RetVal);
}

VOID TOutbound::BuildQueue (PSZ pszPath)
{
   FILE *fp;
   USHORT x;
   CHAR BasePath[128], Temp[128], *p;
   OUTFILE Out;
   struct find_t ffblk, ffblk1, ffblk2;

   Files.Clear ();
   Nodes.Clear ();
   TotalFiles = 0;
   TotalSize = 0L;
   Number = 0;

   strcpy (BasePath, pszPath);
   if (BasePath[strlen (BasePath) - 1] == '\\')
      BasePath[strlen (BasePath) - 1] = '\0';

   strcpy (Temp, BasePath);
   strcat (Temp, ".*");

   // Il primo loop findfirst/findnext cerca le directory di outbound aventi come
   // nome del file la directory di base passata come argomento e, se di zone diversa
   // da quella di default, una estensione di tre caratteri esadecimali rappresentanti
   // il numero di zona.
   if (_dos_findfirst (Temp, _A_SUBDIR, &ffblk) == 0)
      do {
         memset (&Out, 0, sizeof (Out));
         if ((p = strchr (ffblk.name, '.')) != NULL) {
            sscanf (p, ".%03hx", &Out.Zone);
            sprintf (Temp, "%s.%03x\\*.*", BasePath, Out.Zone);
         }
         else {
            Out.Zone = DefaultZone;
            sprintf (Temp, "%s\\*.*", BasePath);
         }

         // Ricerca all'interno della directory di outbound dei files
         if (_dos_findfirst (Temp, 0, &ffblk1) == 0)
            do {
               if ((p = strchr (ffblk1.name, '.')) != NULL) {
                  if (toupper (p[2]) == 'L' && toupper (p[3]) == 'O') {
                     // Trovato un file attach. La prima lettera rappresenta il tipo
                     // di priorita'.
                     sscanf (ffblk1.name, "%04hx%04hx", &Out.Net, &Out.Node);
                     Out.Status = (CHAR)toupper (p[1]);
                     if (Out.Zone != DefaultZone)
                        sprintf (Temp, "%s.%03x\\%s", BasePath, Out.Zone, ffblk1.name);
                     else
                        sprintf (Temp, "%s\\%s", BasePath, ffblk1.name);
                     if ((fp = _fsopen (Temp, "rt", SH_DENYNO)) != NULL) {
                        while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                           if (Temp[strlen (Temp) - 1] == '\n')
                              Temp[strlen (Temp) - 1] = '\0';
                           p = Temp;
                           if (Temp[0] == '^' || Temp[0] == '#')
                              p++;
                           if (p[0] != '~' && _dos_findfirst (p, 0, &ffblk2) == 0) {
                              strcpy (Out.Name, strlwr (ffblk2.name));
                              strcpy (Out.Complete, strlwr (p));
                              Out.Size = ffblk.size;
                              if (strstr (Out.Name, ".pk") != NULL)
                                 Out.MailPKT = TRUE;
                              else {
                                 for (x = 0; x < 7 && Out.ArcMail == FALSE; x++) {
                                    if (strstr (Out.Name, ArcFlags[x]) != NULL)
                                       Out.ArcMail = TRUE;
                                 }
                              }
                              if (Temp[0] == '^')
                                 Out.DeleteAfter = TRUE;
                              else if (Temp[0] == '#')
                                 Out.TruncateAfter = TRUE;
                              if (Files.Add (&Out, sizeof (Out)) == TRUE) {
                                 if (AddQueue (Out) == TRUE) {
                                    TotalSize += Out.Size;
                                    TotalFiles++;
                                 }
                              }

                              // Azzera i flags pericolosi nel caso in cui il file
                              // seguente non li abbia settati nello stesso modo.
                              Out.DeleteAfter = Out.TruncateAfter = Out.ArcMail = FALSE;
                              Out.MailPKT = FALSE;
                           }
                        }
                        fclose (fp);
                     }
                  }
                  else if (toupper (p[2]) == 'U' && toupper (p[3]) == 'T') {
                     // Trovato un pacchetto di posta. La prima lettera rappresenta
                     // il tipo di priorita'. Il file va aggiunto cosi' come si trova.
                     sscanf (ffblk1.name, "%04hx%04hx", &Out.Net, &Out.Node);
                     Out.Status = (CHAR)toupper (p[1]);
                     if (Out.Zone != DefaultZone)
                        sprintf (Out.Complete, "%s.%03x\\%s", BasePath, Out.Zone, ffblk1.name);
                     else
                        sprintf (Out.Complete, "%s\\%s", BasePath, ffblk1.name);
                     _dos_findfirst (Out.Complete, 0, &ffblk2);
                     strcpy (Out.Name, strlwr (ffblk2.name));
                     Out.Size = ffblk2.size;
                     Out.DeleteAfter = TRUE;
                     Out.MailPKT = TRUE;
                     if (Files.Add (&Out, sizeof (Out)) == TRUE) {
                        if (AddQueue (Out) == TRUE) {
                           TotalSize += Out.Size;
                           TotalFiles++;
                        }
                     }
                  }
                  else if (toupper (p[1] == 'R') && toupper (p[2]) == 'E' && toupper (p[3]) == 'Q') {
                     // Trovato un file request diretto al nodo.
                     sscanf (ffblk1.name, "%04hx%04hx", &Out.Net, &Out.Node);
                     Out.Status = (CHAR)toupper (p[1]);
                     if (Out.Zone != DefaultZone)
                        sprintf (Out.Complete, "%s.%03x\\%s", BasePath, Out.Zone, ffblk1.name);
                     else
                        sprintf (Out.Complete, "%s\\%s", BasePath, ffblk1.name);
                     _dos_findfirst (Out.Complete, 0, &ffblk2);
                     strcpy (Out.Name, strlwr (ffblk2.name));
                     Out.Size = ffblk2.size;
                     Out.DeleteAfter = TRUE;
                     Out.Request = TRUE;
                     if (Files.Add (&Out, sizeof (Out)) == TRUE) {
                        if (AddQueue (Out) == TRUE) {
                           TotalSize += Out.Size;
                           TotalFiles++;
                        }
                     }
                  }
               }
            } while (_dos_findnext (&ffblk1) == 0);
      } while (_dos_findnext (&ffblk) == 0);
}

USHORT TOutbound::First (VOID)
{
   USHORT RetVal = FALSE;
   OUTFILE *Out;

   if ((Out = (OUTFILE *)Files.First ()) != NULL) {
      Zone = Out->Zone;
      Net = Out->Net;
      Node = Out->Node;
      Point = Out->Point;
      strcpy (Domain, Out->Domain);
      if (Point)
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      else
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      if (Domain[0]) {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
      strcpy (Name, Out->Name);
      strcpy (Complete, Out->Complete);
      Size = Out->Size;
      ArcMail = Out->ArcMail;
      MailPKT = Out->MailPKT;
      Request = Out->Request;
      Poll = Out->Poll;
      DeleteAfter = Out->DeleteAfter;
      TruncateAfter = Out->TruncateAfter;
      Status = Out->Status;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TOutbound::FirstNode (VOID)
{
   USHORT RetVal = FALSE;
   QUEUE *Queue;

   if ((Queue = (QUEUE *)Nodes.First ()) != NULL) {
      Zone = Queue->Zone;
      Net = Queue->Net;
      Node = Queue->Node;
      Point = Queue->Point;
      strcpy (Domain, Queue->Domain);
      if (Point)
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      else
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      if (Domain[0]) {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
      Number = Queue->Files;
      Size = Queue->Size;
      Crash = Queue->Crash;
      Direct = Queue->Direct;
      Hold = Queue->Hold;
      Immediate = Queue->Immediate;
      Normal = Queue->Normal;
      Attempts = Queue->Attempts;
      Failed = Queue->Failed;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TOutbound::New (VOID)
{
   Zone = Net = Node = Point = 0;
   memset (Domain, 0, sizeof (Domain));
   memset (Name, 0, sizeof (Name));
   memset (Complete, 0, sizeof (Complete));
   Size = 0L;
   ArcMail = MailPKT = Request = DeleteAfter = TruncateAfter = FALSE;
   Status = '\0';
}

USHORT TOutbound::Next (VOID)
{
   USHORT RetVal = FALSE;
   OUTFILE *Out;

   if ((Out = (OUTFILE *)Files.Next ()) != NULL) {
      Zone = Out->Zone;
      Net = Out->Net;
      Node = Out->Node;
      Point = Out->Point;
      strcpy (Domain, Out->Domain);
      strcpy (Name, Out->Name);
      strcpy (Complete, Out->Complete);
      Size = Out->Size;
      ArcMail = Out->ArcMail;
      MailPKT = Out->MailPKT;
      Request = Out->Request;
      Poll = Out->Poll;
      DeleteAfter = Out->DeleteAfter;
      TruncateAfter = Out->TruncateAfter;
      Status = Out->Status;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TOutbound::NextNode (VOID)
{
   USHORT RetVal = FALSE;
   QUEUE *Queue;

   if ((Queue = (QUEUE *)Nodes.Next ()) != NULL) {
      Zone = Queue->Zone;
      Net = Queue->Net;
      Node = Queue->Node;
      Point = Queue->Point;
      strcpy (Domain, Queue->Domain);
      if (Point)
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      else
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      if (Domain[0]) {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
      Number = Queue->Files;
      Size = Queue->Size;
      Crash = Queue->Crash;
      Direct = Queue->Direct;
      Hold = Queue->Hold;
      Immediate = Queue->Immediate;
      Normal = Queue->Normal;
      Attempts = Queue->Attempts;
      Failed = Queue->Failed;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TOutbound::Remove (VOID)
{
   int fd;
   USHORT More = FALSE, Stop = FALSE;
   CHAR FileName[128];
   OUTFILE *Out;
   QUEUE *Temp;

   if ((Out = (OUTFILE *)Files.Value ()) != NULL) {
      if (Out->DeleteAfter == TRUE)
         unlink (Out->Complete);
      else if (Out->TruncateAfter == TRUE) {
         if ((fd = open (Out->Complete, O_WRONLY|O_BINARY|O_TRUNC)) != -1)
            close (fd);
      }

      TotalFiles--;
      TotalSize -= Out->Size;

      if ((Temp = (QUEUE *)Nodes.First ()) != NULL)
         do {
            if (Temp->Zone == Zone && Temp->Net == Net && Temp->Node == Node && Temp->Point == Point && !stricmp (Temp->Domain, Domain)) {
               Temp->Files--;
               Temp->Size -= Out->Size;
               if (Temp->Files == 0)
                  Nodes.Remove ();
               Stop = TRUE;
            }
         } while (Stop == FALSE && (Temp = (QUEUE *)Nodes.Next ()) != NULL);

      Files.Remove ();

      if (MailPKT == FALSE && Request == FALSE) {
         if ((Out = (OUTFILE *)Files.First ()) != NULL)
            do {
               if (Out->MailPKT == FALSE && Out->Request == FALSE && Out->Status == Status) {
                  if (Out->Zone == Zone && Out->Net == Net && Out->Node == Node && Out->Point == Point) {
                     if (!stricmp (Out->Domain, Domain))
                        More = TRUE;
                  }
               }
            } while (More == FALSE && (Out = (OUTFILE *)Files.Next ()) != NULL);
         if (More == FALSE) {
            strcpy (Outbound, Path);
            if (Zone != DefaultZone) {
               Outbound[strlen (Outbound) - 1] = '\0';
               sprintf (FileName, ".%03x\\", Zone);
               strcat (Outbound, FileName);
            }
            if (Point != 0)
               sprintf (FileName, "%s%04x%04x.PNT\\%08x.%cLO", Outbound, Net, Node, Point, Status);
            else
               sprintf (FileName, "%s%04x%04x.%cLO", Outbound, Net, Node, Status);
            unlink (FileName);
         }
      }
   }
}

VOID TOutbound::Update (VOID)
{
   FILE *fp;
   CHAR FileName[128];

   if (First () == TRUE)
      do {
         if (MailPKT == FALSE && Request == FALSE) {
            if (Zone != DefaultZone) {
               Outbound[strlen (Outbound) - 1] = '\0';
               sprintf (FileName, ".%03x\\", Zone);
               strcat (Outbound, FileName);
            }
            if (Point != 0)
               sprintf (FileName, "%s%04x%04x.PNT\\%08x.%cLO", Outbound, Net, Node, Point, Status);
            else
               sprintf (FileName, "%s%04x%04x.%cLO", Outbound, Net, Node, Status);
            unlink (FileName);
         }
      } while (Next () == TRUE);

   if (First () == TRUE)
      do {
         if (MailPKT == FALSE && Request == FALSE) {
            if (Zone != DefaultZone) {
               Outbound[strlen (Outbound) - 1] = '\0';
               sprintf (FileName, ".%03x\\", Zone);
               strcat (Outbound, FileName);
            }

            if (Point != 0) {
               sprintf (FileName, "%s%04x%04x.PNT", Outbound, Net, Node);
               mkdir (FileName);
               sprintf (FileName, "%s%04x%04x.PNT\\%08x.%cLO", Outbound, Net, Node, Point, Status);
            }
            else
               sprintf (FileName, "%s%04x%04x.%cLO", Outbound, Net, Node, Status);

            if ((fp = fopen (FileName, "ab")) != NULL) {
               if (TruncateAfter == TRUE)
                  fprintf (fp, "#%s\r\n", Complete);
               else if (DeleteAfter == TRUE)
                  fprintf (fp, "^%s\r\n", Complete);
               else if (Poll == FALSE)
                  fprintf (fp, "%s\r\n", Complete);
               fclose (fp);
            }
         }
      } while (Next () == TRUE);
}



