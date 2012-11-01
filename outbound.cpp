
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/22/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

PSZ ArcFlags[] = { ".mo", ".tu", ".we", ".th", ".fr", ".sa", ".su" };

TOutbound::TOutbound (PSZ pszPath)
{
   DefaultZone = 2;
   Files.Clear ();
   TotalNodes = TotalFiles = 0;
   TotalSize = 0L;

   if (pszPath != NULL) {
      strcpy (Path, pszPath);
      if (Path[strlen (Path) - 1] != '\\' && Path[strlen (Path) - 1] != '/')
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
      if (Path[strlen (Path) - 1] != '\\' && Path[strlen (Path) - 1] != '/')
         strcat (Path, "\\");
   }

   Add (usZone, usNet, usNode, usPoint, pszDomain);
}

TOutbound::~TOutbound (void)
{
   Files.Clear ();
   Nodes.Clear ();
}

VOID TOutbound::Clear (VOID)
{
   Files.Clear ();
   Nodes.Clear ();
   TotalFiles = 0;
   TotalSize = 0L;
   New ();
}

USHORT TOutbound::Add (VOID)
{
   USHORT RetVal = FALSE;
   PSZ p;
   OUTFILE Out;

   memset (&Out, 0, sizeof (Out));
   Out.Zone = Zone;
   Out.Net = Net;
   Out.Node = Node;
   Out.Point = Point;
   strcpy (Out.Domain, Domain);
   if (Name[0] == '\0') {
      if ((p = strchr (Complete, '\0')) != NULL) {
         while (p > Complete && *p != '\\' && *p != ':' && *p != '/')
            p--;
         strcpy (Out.Name, ++p);
      }
   }
   else
      strcpy (Out.Name, Name);
   strcpy (Out.Complete, Complete);
   Out.Size = Size;
   Out.ArcMail = ArcMail;
   Out.MailPKT = MailPKT;
   Out.Request = Request;
   Out.Poll = Poll;
   Out.DeleteAfter = DeleteAfter;
   Out.TruncateAfter = TruncateAfter;
   if (Poll == TRUE) {
      if (Crash == TRUE)
         Status = 'C';
      else if (Direct == TRUE)
         Status = 'D';
      else if (Normal == TRUE)
         Status = 'F';
      else if (Immediate == TRUE)
         Status = 'I';
   }
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
   CHAR FileName[128], String[128], *pFile, *p;
   CHAR Flags[] = { 'h', 'c', 'd', 'f', 'o', 'i' };
   OUTFILE Out;
   struct stat statbuf;

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
      sprintf (FileName, "%s%04x%04x.pnt\\%08x.hut", Outbound, usNet, usNode, usPoint);
   else
      sprintf (FileName, "%s%04x%04x.hut", Outbound, usNet, usNode);

   for (i = 0; i < 6; i++) {
      FileName[strlen (FileName) - 3] = Flags[i];
      if (stat (AdjustPath (FileName), &statbuf) == 0) {
         memset (&Out, 0, sizeof (Out));
         Out.Zone = usZone;
         Out.Net = usNet;
         Out.Node = usNode;
         Out.Point = usPoint;
         if (pszDomain != NULL)
            strcpy (Out.Domain, pszDomain);
         if (usPoint != 0)
            sprintf (Out.Name, "%08x.hut", usPoint);
         else
            sprintf (Out.Name, "%04x%04x.hut", usNet, usNode);
         Out.Name[strlen (Out.Name) - 3] = Flags[i];
         strcpy (Out.Complete, FileName);
         Out.Size = statbuf.st_size;
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
      sprintf (FileName, "%s%04x%04x.pnt\\%08x.hlo", Outbound, usNet, usNode, usPoint);
   else
      sprintf (FileName, "%s%04x%04x.hlo", Outbound, usNet, usNode);

   for (i = 0; i < 6; i++) {
      FileName[strlen (FileName) - 3] = Flags[i];
      if ((fp = _fsopen (AdjustPath (FileName), "rt", SH_DENYNO)) != NULL) {
         Readed = FALSE;
         while (fgets (String, sizeof (String) - 1, fp) != NULL) {
            if (String[strlen (String) - 1] == '\n')
               String[strlen (String) - 1] = '\0';
            pFile = String;
            if (String[0] == '^' || String[0] == '#')
               pFile++;
            if (pFile[0] != '~' && stat (AdjustPath (pFile), &statbuf) == 0) {
               memset (&Out, 0, sizeof (Out));
               Out.Zone = usZone;
               Out.Net = usNet;
               Out.Node = usNode;
               Out.Point = usPoint;
               if (pszDomain != NULL)
                  strcpy (Out.Domain, pszDomain);
               if ((p = strchr (pFile, '!')) != NULL) {
                  *p++ = '\0';
                  strcpy (Out.Name, p);
               }
               else if ((p = strchr (pFile, '\0')) != NULL) {
                  while (p > pFile && *p != '\\' && *p != ':' && *p != '/')
                     p--;
                  strcpy (Out.Name, ++p);
               }
               strcpy (Out.Complete, pFile);
               Out.Size = statbuf.st_size;
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
      sprintf (FileName, "%s%04x%04x.pnt\\%08x.req", Outbound, usNet, usNode, usPoint);
   else
      sprintf (FileName, "%s%04x%04x.req", Outbound, usNet, usNode);

   if (stat (AdjustPath (FileName), &statbuf) == 0) {
      memset (&Out, 0, sizeof (Out));
      Out.Zone = usZone;
      Out.Net = usNet;
      Out.Node = usNode;
      Out.Point = usPoint;
      if (pszDomain != NULL)
         strcpy (Out.Domain, pszDomain);
      if (usPoint != 0)
         sprintf (Out.Name, "%08x.req", usPoint);
      else
         sprintf (Out.Name, "%04x%04x.req", usNet, usNode);
      strcpy (Out.Complete, FileName);
      Out.Size = statbuf.st_size;
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

   return (RetVal);
}

USHORT TOutbound::AddQueue (OUTFILE &Out)
{
   USHORT RetVal = FALSE, Insert;
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
            break;
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

      Insert = FALSE;
      if ((Temp = (QUEUE *)Nodes.First ()) != NULL) {
         if (Temp->Zone > Queue.Zone)
            Insert = TRUE;
         else if (Temp->Zone == Queue.Zone && Temp->Net > Queue.Net)
            Insert = TRUE;
         else if (Temp->Zone == Queue.Zone && Temp->Net == Queue.Net && Temp->Node > Queue.Node)
            Insert = TRUE;
         else if (Temp->Zone == Queue.Zone && Temp->Net == Queue.Net && Temp->Node == Queue.Node && Temp->Point > Queue.Point)
            Insert = TRUE;

         if (Insert == TRUE) {
            Nodes.Insert (&Queue, sizeof (QUEUE));
            Nodes.Insert (Temp, sizeof (QUEUE));
            Nodes.First ();
            Nodes.Remove ();
            Nodes.First ();
            TotalNodes++;
         }
         else {
            while ((Temp = (QUEUE *)Nodes.Next ()) != NULL) {
               if (Temp->Zone > Queue.Zone)
                  Insert = TRUE;
               else if (Temp->Zone == Queue.Zone && Temp->Net > Queue.Net)
                  Insert = TRUE;
               else if (Temp->Zone == Queue.Zone && Temp->Net == Queue.Net && Temp->Node > Queue.Node)
                  Insert = TRUE;
               else if (Temp->Zone == Queue.Zone && Temp->Net == Queue.Net && Temp->Node == Queue.Node && Temp->Point > Queue.Point)
                  Insert = TRUE;

               if (Insert == TRUE) {
                  Nodes.Previous ();
                  Nodes.Insert (&Queue, sizeof (QUEUE));
                  TotalNodes++;
                  break;
               }
            }
            if (Insert == FALSE) {
               Nodes.Add (&Queue, sizeof (QUEUE));
               TotalNodes++;
               Insert = TRUE;
            }
         }
      }
      if (Insert == FALSE) {
         Nodes.Add (&Queue, sizeof (QUEUE));
         TotalNodes++;
      }

      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TOutbound::BuildQueue (PSZ pszPath)
{
   FILE *fp;
   int i;
   USHORT x, Added;
   CHAR BasePath[128], Temp[128], *p, BaseOut[32];
   DIR *dir, *dir1;
   OUTFILE Out;
   QUEUE *Queue;
   struct dirent *ent;
   struct stat statbuf;

   Files.Clear ();
   Nodes.Clear ();
   TotalNodes = TotalFiles = 0;
   TotalSize = 0L;
   Number = 0;
   TotalNodes = 0;

   strcpy (BasePath, pszPath);
   if (BasePath[strlen (BasePath) - 1] == '\\' || BasePath[strlen (BasePath) - 1] == '/')
      BasePath[strlen (BasePath) - 1] = '\0';

   strcpy (Temp, BasePath);
   if ((p = strchr (Temp, '\0')) != NULL) {
      while (p > Temp && *p != '\\' && *p != ':' && *p != '/')
         *p-- = '\0';
   }
   *p-- = '\0';
   if (*p == ':')
      strcat (p, "\\");

   // Il primo loop cerca le directory di outbound aventi come
   // nome del file la directory di base passata come argomento e, se di zone diversa
   // da quella di default, una estensione di tre caratteri esadecimali rappresentanti
   // il numero di zona.
   if ((dir = opendir (AdjustPath (Temp))) != NULL) {
      if ((p = strchr (BasePath, '\0')) != NULL) {
         while (p > BasePath && *p != '\\' && *p != ':' && *p != '/')
            p--;
         strcpy (BaseOut, ++p);
      }

      while ((ent = readdir (dir)) != NULL) {
         if (strnicmp (strupr (ent->d_name), BaseOut, strlen (BaseOut)) == 0) {
            memset (&Out, 0, sizeof (Out));
            p = strchr (ent->d_name, '.');
            if (p != NULL) {
               sscanf (p, ".%03hx", &Out.Zone);
               if (Out.Zone != 0)
                  sprintf (Temp, "%s.%03x", BasePath, Out.Zone);
               else {
                  Out.Zone = DefaultZone;
                  sprintf (Temp, "%s", BasePath);
               }
            }
            else {
               Out.Zone = DefaultZone;
               sprintf (Temp, "%s", BasePath);
            }

            if ((dir1 = opendir (AdjustPath (Temp))) != NULL) {
               while ((ent = readdir (dir1)) != NULL) {
                  if ((p = strchr (ent->d_name, '.')) != NULL) {
                     if (toupper (p[2]) == 'L' && toupper (p[3]) == 'O') {
                        // Trovato un file attach. La prima lettera rappresenta il tipo
                        // di priorita'.
                        Out.Point = 0;
                        sscanf (ent->d_name, "%04hx%04hx", &Out.Net, &Out.Node);
                        Out.Status = (CHAR)toupper (p[1]);
                        if (Out.Zone != DefaultZone)
                           sprintf (Temp, "%s.%03x\\%s", BasePath, Out.Zone, ent->d_name);
                        else
                           sprintf (Temp, "%s\\%s", BasePath, ent->d_name);
                        Out.Size = 0L;
                        if ((fp = _fsopen (AdjustPath (Temp), "rt", SH_DENYNO)) != NULL) {
                           Added = FALSE;
                           while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                              if (Temp[strlen (Temp) - 1] == '\n')
                                 Temp[strlen (Temp) - 1] = '\0';
                              p = Temp;
                              if (Temp[0] == '^' || Temp[0] == '#')
                                 p++;
                              if (p[0] != '~' && stat (AdjustPath (p), &statbuf) == 0) {
                                 strcpy (Out.Complete, p);
                                 if ((p = strchr (Temp, '\0')) != NULL) {
                                    while (p > Temp && *p != '\\' && *p != ':' && *p != '/')
                                       p--;
                                    if (*p == '\\' || *p == ':' || *p == '/' || *p == '^' || *p == '#')
                                       p++;
                                    strcpy (Out.Name, p);
                                 }
                                 Out.Size = statbuf.st_size;
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
                                    Added = TRUE;
                                 }

                                 // Azzera i flags pericolosi nel caso in cui il file
                                 // seguente non li abbia settati nello stesso modo.
                                 Out.DeleteAfter = Out.TruncateAfter = Out.ArcMail = FALSE;
                                 Out.MailPKT = FALSE;
                              }
                           }
                           fclose (fp);

                           if (Added == FALSE) {
                              Out.Poll = TRUE;
                              if (Files.Add (&Out, sizeof (Out)) == TRUE) {
                                 if (AddQueue (Out) == TRUE) {
                                    TotalSize += Out.Size;
                                    TotalFiles++;
                                 }
                                 Added = TRUE;
                              }
                           }
                        }
                     }
                     else if (toupper (p[2]) == 'U' && toupper (p[3]) == 'T') {
                        // Trovato un pacchetto di posta. La prima lettera rappresenta
                        // il tipo di priorita'. Il file va aggiunto cosi' come si trova.
                        Out.Point = 0;
                        sscanf (ent->d_name, "%04hx%04hx", &Out.Net, &Out.Node);
                        Out.Status = (CHAR)toupper (p[1]);
                        if (Out.Zone != DefaultZone)
                           sprintf (Out.Complete, "%s.%03x\\%s", BasePath, Out.Zone, ent->d_name);
                        else
                           sprintf (Out.Complete, "%s\\%s", BasePath, ent->d_name);
                        stat (AdjustPath (Out.Complete), &statbuf);
                        strcpy (Out.Name, ent->d_name);
                        Out.Size = statbuf.st_size;
                        Out.DeleteAfter = TRUE;
                        Out.MailPKT = TRUE;
                        if (Files.Add (&Out, sizeof (Out)) == TRUE) {
                           if (AddQueue (Out) == TRUE) {
                              TotalSize += Out.Size;
                              TotalFiles++;
                           }
                        }
                     }
                     else if (toupper (p[1]) == 'R' && toupper (p[2]) == 'E' && toupper (p[3]) == 'Q') {
                        // Trovato un file request diretto al nodo.
                        Out.Point = 0;
                        sscanf (ent->d_name, "%04hx%04hx", &Out.Net, &Out.Node);
                        Out.Status = (CHAR)toupper (p[1]);
                        if (Out.Zone != DefaultZone)
                           sprintf (Out.Complete, "%s.%03x\\%s", BasePath, Out.Zone, ent->d_name);
                        else
                           sprintf (Out.Complete, "%s\\%s", BasePath, ent->d_name);
                        stat (AdjustPath (Out.Complete), &statbuf);
                        strcpy (Out.Name, ent->d_name);
                        Out.Size = statbuf.st_size;
                        Out.DeleteAfter = TRUE;
                        Out.Request = TRUE;
                        if (Files.Add (&Out, sizeof (Out)) == TRUE) {
                           if (AddQueue (Out) == TRUE) {
                              TotalSize += Out.Size;
                              TotalFiles++;
                           }
                        }
                     }
                     else if (toupper (p[1]) == '$' && toupper (p[2]) == '$') {
                        // Trovato il contatore dei tentativi di chiamata
                        Out.Point = 0;
                        sscanf (ent->d_name, "%04hx%04hx", &Out.Net, &Out.Node);
                        if (AddQueue (Out) == TRUE) {
                           if ((Queue = (QUEUE *)Nodes.Value ()) != NULL) {
                              Queue->Failed = (USHORT)(p[3] - '0');
                              if (Out.Zone != DefaultZone)
                                 sprintf (Out.Complete, "%s.%03x\\%s", BasePath, Out.Zone, ent->d_name);
                              else
                                 sprintf (Out.Complete, "%s\\%s", BasePath, ent->d_name);
                              if ((i = open (AdjustPath (Out.Complete), O_RDONLY|O_BINARY)) != -1) {
                                 read (i, &Queue->Attempts, 2);
                                 read (i, Queue->LastCall, sizeof (LastCall));
                                 close (i);
                              }
                           }
                        }
                     }
                     else if (toupper (p[1]) == 'P' && toupper (p[2]) == 'N' && toupper (p[3]) == 'T') {
                        USHORT pZone, pNet, pNode;
                        DIR *dir3;

                        // Point subdirectory
                        pZone = Out.Zone;
                        sscanf (ent->d_name, "%04hx%04hx", &pNet, &pNode);

                        sprintf (Temp, "%s\\%s", BasePath, ent->d_name);
                        if ((dir3 = opendir (Temp)) != NULL) {
                           while ((ent = readdir (dir3)) != NULL) {
                              if ((p = strchr (ent->d_name, '.')) != NULL) {
                                 memset (&Out, 0, sizeof (Out));
                                 Out.Zone = pZone;
                                 Out.Net = pNet;
                                 Out.Node = pNode;
                                 if (toupper (p[2]) == 'L' && toupper (p[3]) == 'O') {
                                    // Trovato un file attach. La prima lettera rappresenta il tipo
                                    // di priorita'.
                                    Added = FALSE;
                                    sscanf (ent->d_name, "%08hx", &Out.Point);
                                    Out.Status = (CHAR)toupper (p[1]);
                                    sprintf (Temp, "%s\\%04x%04x.pnt\\%s", BasePath, Out.Net, Out.Node, ent->d_name);
                                    Out.Size = 0L;
                                    if ((fp = _fsopen (AdjustPath (Temp), "rt", SH_DENYNO)) != NULL) {
                                       while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                                          if (Temp[strlen (Temp) - 1] == '\n')
                                             Temp[strlen (Temp) - 1] = '\0';
                                          p = Temp;
                                          if (Temp[0] == '^' || Temp[0] == '#')
                                             p++;
                                          if (p[0] != '~' && stat (AdjustPath (p), &statbuf) == 0) {
                                             strcpy (Out.Complete, p);
                                             if ((p = strchr (Temp, '\0')) != NULL) {
                                                while (p > Temp && *p != '\\' && *p != ':' && *p != '/')
                                                   p--;
                                                if (*p == '\\' || *p == ':' || *p == '/' || *p == '^' || *p == '#')
                                                   p++;
                                                strcpy (Out.Name, p);
                                             }
                                             Out.Size = statbuf.st_size;
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

                                    if (Added == FALSE) {
                                       Out.Poll = TRUE;
                                       if (Files.Add (&Out, sizeof (Out)) == TRUE) {
                                          if (AddQueue (Out) == TRUE) {
                                             TotalSize += Out.Size;
                                             TotalFiles++;
                                          }
                                          Added = TRUE;
                                       }
                                    }
                                 }
                                 else if (toupper (p[2]) == 'U' && toupper (p[3]) == 'T') {
                                    // Trovato un pacchetto di posta. La prima lettera rappresenta
                                    // il tipo di priorita'. Il file va aggiunto cosi' come si trova.
                                    sscanf (ent->d_name, "%08hx", &Out.Point);
                                    Out.Status = (CHAR)toupper (p[1]);
                                    sprintf (Out.Complete, "%s\\%04x%04x.pnt\\%s", BasePath, Out.Net, Out.Node, ent->d_name);
                                    stat (AdjustPath (Out.Complete), &statbuf);
                                    strcpy (Out.Name, ent->d_name);
                                    Out.Size = statbuf.st_size;
                                    Out.DeleteAfter = TRUE;
                                    Out.MailPKT = TRUE;
                                    if (Files.Add (&Out, sizeof (Out)) == TRUE) {
                                       if (AddQueue (Out) == TRUE) {
                                          TotalSize += Out.Size;
                                          TotalFiles++;
                                       }
                                    }
                                 }
                                 else if (toupper (p[1]) == 'R' && toupper (p[2]) == 'E' && toupper (p[3]) == 'Q') {
                                    // Trovato un file request diretto al nodo.
                                    sscanf (ent->d_name, "%08hx", &Out.Point);
                                    Out.Status = (CHAR)toupper (p[1]);
                                    sprintf (Out.Complete, "%s\\%04x%04x.pnt\\%s", BasePath, Out.Net, Out.Node, ent->d_name);
                                    stat (AdjustPath (Out.Complete), &statbuf);
                                    strcpy (Out.Name, ent->d_name);
                                    Out.Size = statbuf.st_size;
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
                           }
                           closedir (dir3);
                        }
                     }
                  }
               }
               closedir (dir1);
            }
         }
      }
      closedir (dir);
   }
}

VOID TOutbound::PollNode (PSZ address, CHAR flag)
{
   FILE *fp;
   CHAR PollFile[128], Outb[128];
   class TAddress Addr;

   Addr.Parse (address);

   strcpy (Outb, Path);
   if (Outb[strlen (Outb) - 1] == '\\' || Outb[strlen (Outb) - 1] == '/')
      Outb[strlen (Outb) - 1] = '\0';

   if (Addr.Zone == 0 || DefaultZone == Addr.Zone) {
      if (Addr.Point != 0) {
#if defined(__LINUX__)
         sprintf (PollFile, "%s/%04x%04x.pnt", Outb, Addr.Net, Addr.Node);
         mkdir (PollFile, 0666);
         sprintf (PollFile, "%s/%04x%04x.pnt/%08x.%clo", Outbound, Addr.Net, Addr.Node, Addr.Point, flag);
#else
         sprintf (PollFile, "%s\\%04x%04x.pnt", Outb, Addr.Net, Addr.Node);
         mkdir (PollFile);
         sprintf (PollFile, "%s\\%04x%04x.pnt\\%08x.%clo", Outb, Addr.Net, Addr.Node, Addr.Point, flag);
#endif
      }
      else
#if defined(__LINUX__)
         sprintf (PollFile, "%s/%04x%04x.%clo", Outb, Addr.Net, Addr.Node, flag);
#else
         sprintf (PollFile, "%s\\%04x%04x.%clo", Outb, Addr.Net, Addr.Node, flag);
#endif
   }
   else {
      sprintf (PollFile, "%s.%03x", Outb, Addr.Zone);
#if defined(__LINUX__)
      mkdir (PollFile, 0666);
#else
      mkdir (PollFile);
#endif
      if (Addr.Point != 0) {
#if defined(__LINUX__)
         sprintf (PollFile, "%s.%03x/%04x%04x.pnt", Outb, Addr.Zone, Addr.Net, Addr.Node);
         mkdir (PollFile, 0666);
         sprintf (PollFile, "%s.%03x/%04x%04x.pnt/%08x.%clo", Outb, Addr.Zone, Addr.Net, Addr.Node, Addr.Point, flag);
#else
         sprintf (PollFile, "%s.%03x\\%04x%04x.pnt", Outb, Addr.Zone, Addr.Net, Addr.Node);
         mkdir (PollFile);
         sprintf (PollFile, "%s.%03x\\%04x%04x.pnt\\%08x.%clo", Outb, Addr.Zone, Addr.Net, Addr.Node, Addr.Point, flag);
#endif
      }
      else
#if defined(__LINUX__)
         sprintf (PollFile, "%s.%03x/%04x%04x.%clo", Outb, Addr.Zone, Addr.Net, Addr.Node, flag);
#else
         sprintf (PollFile, "%s.%03x\\%04x%04x.%clo", Outb, Addr.Zone, Addr.Net, Addr.Node, flag);
#endif
   }

   if ((fp = fopen (PollFile, "at")) != NULL)
      fclose (fp);
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
      strcpy (LastCall, Queue->LastCall);
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
   Poll = FALSE;
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
      strcpy (LastCall, Queue->LastCall);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TOutbound::Remove (VOID)
{
   int fd;
   USHORT More = FALSE, Stop = FALSE;
   CHAR FileName[128], i;
   OUTFILE *Out;
   QUEUE *Temp;

   if ((Out = (OUTFILE *)Files.Value ()) != NULL) {
      AdjustPath (Out->Complete);
      if (Out->DeleteAfter == TRUE)
         unlink (Out->Complete);
      else if (Out->TruncateAfter == TRUE) {
         if ((fd = sopen (Out->Complete, O_WRONLY|O_BINARY|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
            close (fd);
      }

      TotalFiles--;
      TotalSize -= Out->Size;

      if ((Temp = (QUEUE *)Nodes.First ()) != NULL)
         do {
            if (Temp->Zone == Zone && Temp->Net == Net && Temp->Node == Node && Temp->Point == Point && !stricmp (Temp->Domain, Domain)) {
               Temp->Files--;
               Temp->Size -= Out->Size;
               if (Temp->Files == 0) {
                  Nodes.Remove ();
                  TotalNodes--;
               }
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

            for (i = '0'; i <= '9'; i++) {
               if (Point != 0)
                  sprintf (FileName, "%s%04x%04x.PNT\\%08x.$$%c", Outbound, Net, Node, Point, i);
               else
                  sprintf (FileName, "%s%04x%04x.$$%c", Outbound, Net, Node, i);
               unlink (FileName);
            }
         }
      }
   }
}

VOID TOutbound::AddAttempt (PSZ address, USHORT failed, PSZ status)
{
   int fd;
   USHORT attempts = 0;
   CHAR i, FileName[128], Status[32];
   QUEUE *Queue, *Current;
   class TAddress Addr;

   Addr.Parse (address);

   strcpy (Outbound, Path);
   if (Addr.Zone != DefaultZone) {
      Outbound[strlen (Outbound) - 1] = '\0';
      sprintf (FileName, ".%03x/", Addr.Zone);
      strcat (Outbound, FileName);
   }

   for (i = '0'; i <= '9'; i++) {
      if (Addr.Point != 0)
         sprintf (FileName, "%s%04x%04x.pnt/%08x.$$%c", Outbound, Addr.Net, Addr.Node, Addr.Point, i);
      else
         sprintf (FileName, "%s%04x%04x.$$%c", Outbound, Addr.Net, Addr.Node, i);
      if ((fd = open (FileName, O_RDWR|O_BINARY)) != -1) {
         read (fd, &attempts, sizeof (attempts));
         read (fd, Status, sizeof (Status));
         close (fd);
         break;
      }
   }

   if (i > '9')
      i = '0';

   if (failed == TRUE && i < '9') {
      unlink (FileName);
      i++;
   }

   if (Addr.Point != 0)
      sprintf (FileName, "%s%04x%04x.pnt/%08x.$$%c", Outbound, Addr.Net, Addr.Node, Addr.Point, i);
   else
      sprintf (FileName, "%s%04x%04x.$$%c", Outbound, Addr.Net, Addr.Node, i);

   memset (Status, 0, sizeof (Status));
   strcpy (Status, status);
   attempts++;

   if ((fd = open (FileName, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
      write (fd, &attempts, sizeof (attempts));
      write (fd, Status, sizeof (Status));
      close (fd);
   }

   Current = (QUEUE *)Nodes.Value ();
   if ((Queue = (QUEUE *)Nodes.First ()) != NULL)
      do {
         if (Addr.Zone == Queue->Zone && Addr.Net == Queue->Net && Addr.Node == Queue->Node && Addr.Point == Queue->Point) {
            Queue->Attempts = attempts;
            Queue->Failed = (USHORT)(i - '0');
            memcpy (Queue->LastCall, Status, sizeof (Queue->LastCall));
            break;
         }
      } while ((Queue = (QUEUE *)Nodes.Next ()) != NULL);

   if ((Queue = (QUEUE *)Nodes.First ()) != NULL)
      do {
         if (Queue == Current)
            break;
      } while ((Queue = (QUEUE *)Nodes.Next ()) != NULL);
}

VOID TOutbound::ClearAttempt (PSZ address)
{
   CHAR i, FileName[128];
   QUEUE *Queue, *Current;
   class TAddress Addr;

   Addr.Parse (address);

   strcpy (Outbound, Path);
   if (Addr.Zone != DefaultZone) {
      Outbound[strlen (Outbound) - 1] = '\0';
      sprintf (FileName, ".%03x/", Addr.Zone);
      strcat (Outbound, FileName);
   }

   for (i = '0'; i <= '9'; i++) {
      if (Addr.Point != 0)
         sprintf (FileName, "%s%04x%04x.pnt/%08x.$$%c", Outbound, Addr.Net, Addr.Node, Addr.Point, i);
      else
         sprintf (FileName, "%s%04x%04x.$$%c", Outbound, Addr.Net, Addr.Node, i);
      unlink (FileName);
   }

   Current = (QUEUE *)Nodes.Value ();
   if ((Queue = (QUEUE *)Nodes.First ()) != NULL)
      do {
         if (Addr.Zone == Queue->Zone && Addr.Net == Queue->Net && Addr.Node == Queue->Node && Addr.Point == Queue->Point) {
            Queue->Attempts = 0;
            Queue->Failed = 0;
            memset (Queue->LastCall, 0, sizeof (Queue->LastCall));
            break;
         }
      } while ((Queue = (QUEUE *)Nodes.Next ()) != NULL);

   if ((Queue = (QUEUE *)Nodes.First ()) != NULL)
      do {
         if (Queue == Current)
            break;
      } while ((Queue = (QUEUE *)Nodes.Next ()) != NULL);
}

VOID TOutbound::Update (VOID)
{
   FILE *fp;
   int fd;
   CHAR FileName[128], i;

   if (FirstNode () == TRUE)
      do {
         strcpy (Outbound, Path);
         if (Zone != DefaultZone) {
            Outbound[strlen (Outbound) - 1] = '\0';
            sprintf (FileName, ".%03x/", Zone);
            strcat (Outbound, FileName);
         }
         for (i = '0'; i <= '9'; i++) {
            if (Point != 0)
               sprintf (FileName, "%s%04x%04x.pnt/%08x.$$%c", Outbound, Net, Node, Point, i);
            else
               sprintf (FileName, "%s%04x%04x.$$%c", Outbound, Net, Node, i);
            unlink (FileName);
         }
      } while (NextNode () == TRUE);

   if (First () == TRUE)
      do {
         if (MailPKT == FALSE && Request == FALSE) {
            strcpy (Outbound, Path);
            if (Zone != DefaultZone) {
               Outbound[strlen (Outbound) - 1] = '\0';
               sprintf (FileName, ".%03x/", Zone);
               strcat (Outbound, FileName);
            }
            if (Point != 0)
               sprintf (FileName, "%s%04x%04x.pnt/%08x.%clo", Outbound, Net, Node, Point, Status);
            else
               sprintf (FileName, "%s%04x%04x.%clo", Outbound, Net, Node, Status);
            unlink (FileName);
         }
      } while (Next () == TRUE);

   if (FirstNode () == TRUE)
      do {
         if (Attempts != 0 || Failed != 0) {
            strcpy (Outbound, Path);
            if (Zone != DefaultZone) {
               Outbound[strlen (Outbound) - 1] = '\0';
               sprintf (FileName, ".%03x/", Zone);
               strcat (Outbound, FileName);
            }
            if (Point != 0)
               sprintf (FileName, "%s%04x%04x.pnt/%08x.$$%c", Outbound, Net, Node, Point, Failed + '0');
            else
               sprintf (FileName, "%s%04x%04x.$$%c", Outbound, Net, Node, Failed + '0');
            if ((fd = open (FileName, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
               write (fd, &Attempts, 2);
               close (fd);
            }
         }
      } while (NextNode () == TRUE);

   if (First () == TRUE)
      do {
         if (MailPKT == FALSE && Request == FALSE) {
            strcpy (Outbound, Path);
            if (Zone != DefaultZone) {
               Outbound[strlen (Outbound) - 1] = '\0';
               sprintf (FileName, ".%03x", Zone);
               strcat (Outbound, FileName);
#if defined(__LINUX__)
               mkdir (Outbound, 0666);
               strcat (Outbound, "/");
#else
               mkdir (Outbound);
               strcat (Outbound, "\\");
#endif
            }

            if (Point != 0) {
               sprintf (FileName, "%s%04x%04x.pnt", Outbound, Net, Node);
#if defined(__LINUX__)
               mkdir (FileName, 0666);
#else
               mkdir (FileName);
#endif
               sprintf (FileName, "%s%04x%04x.pnt/%08x.%clo", Outbound, Net, Node, Point, Status);
            }
            else
               sprintf (FileName, "%s%04x%04x.%clo", Outbound, Net, Node, Status);

            if ((fp = _fsopen (AdjustPath (FileName), "ab", SH_DENYNO)) != NULL) {
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


