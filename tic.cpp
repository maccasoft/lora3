
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.25
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    06/06/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"
#include "lorawin.h"

TTicProcessor::TTicProcessor (void)
{
   Output = NULL;
   Log = NULL;
}

TTicProcessor::~TTicProcessor (void)
{
}

USHORT TTicProcessor::Check (VOID)
{
   USHORT RetVal = FALSE;
   class TNodes *Nodes;

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (From) == TRUE) {
         if (!stricmp (Nodes->TicPwd, Password))
            RetVal = TRUE;
      }
      delete Nodes;
   }

   return (RetVal);
}

VOID TTicProcessor::Delete (VOID)
{
   if (Log != NULL)
   Log->Write ("+Deleting %s", CurrentFile);
   unlink (CurrentFile);
   sprintf (Temp, "%s%s", Inbound, Name);
   unlink (Temp);
}

VOID TTicProcessor::Hatch (class TAddress *Dest)
{
   class TAddress Addr;

   Addr.Parse (Dest->String);
   Hatch (Addr);
}

VOID TTicProcessor::Hatch (PSZ address)
{
   class TAddress Addr;

   Addr.Parse (address);
   Hatch (Addr);
}

VOID TTicProcessor::Hatch (class TAddress &Dest)
{
   FILE *fp;
   CHAR TicName[32], TicFile[128], *p;
   struct dostime_t dtime;
   struct stat statbuf;
   class TOutbound *Out;

   Cfg->MailAddress.First ();

   do {
      _dos_gettime (&dtime);
      sprintf (TicName, "lb%02d%02d%02d.tic", dtime.minute, dtime.second, dtime.hsecond);
      strcpy (TicFile, Cfg->Outbound);
      if (Dest.Zone != Cfg->MailAddress.Zone) {
         TicFile[strlen (TicFile) - 1] = '\0';
         sprintf (Temp, ".%03x", Dest.Zone);
         strcat (TicFile, Temp);
#if defined(__LINUX__)
         mkdir (TicFile, 0666);
         strcat (TicFile, "/");
#else
         mkdir (TicFile);
         strcat (TicFile, "\\");
#endif
      }
      if (Dest.Point != 0) {
         sprintf (Temp, "%s%04x%04x.pnt", TicFile, Dest.Net, Dest.Node);
#if defined(__LINUX__)
         mkdir (Temp, 0666);
         strcat (Temp, "/");
#else
         mkdir (Temp);
         strcat (Temp, "\\");
#endif
         strcpy (TicFile, Temp);
      }
      strcat (TicFile, TicName);
   } while (stat (TicFile, &statbuf) == 0);

   if ((fp = fopen (TicFile, "wt")) != NULL) {
      fprintf (fp, "Area %s\n", strupr (Area));
      fprintf (fp, "Origin %s\n", Origin.String);
      if (Cfg->MailAddress.First () == TRUE)
         fprintf (fp, "From %s\n", Cfg->MailAddress.String);
      fprintf (fp, "File %s\n", Name);
      if (Replace[0] != '\0')
         fprintf (fp, "Replaces %s\n", Replace);
      if ((p = (PSZ)Description->First ()) == NULL)
         p = "";
      fprintf (fp, "Desc %s\n", p);
      fprintf (fp, "CRC %08lX\n", Crc);
      fprintf (fp, "Created by %s v%s\n", NAME, VERSION);
      if (Size != 0L)
         fprintf (fp, "Size %lu\n", Size);
      while ((p = (PSZ)Description->Next ()) != NULL)
         fprintf (fp, "Ldesc %s\n", p);

      if ((p = (PSZ)Path->First ()) != NULL)
         do {
            fprintf (fp, "Path %s\n", p);
         } while ((p = (PSZ)Path->Next ()) != NULL);

      if ((p = (PSZ)SeenBy->First ()) != NULL)
         do {
            fprintf (fp, "Seenby %s\n", p);
         } while ((p = (PSZ)SeenBy->Next ()) != NULL);

      fprintf (fp, "Pw %s\n", Password);
      fclose (fp);
   }

   if ((Out = new TOutbound (Cfg->Outbound)) != NULL) {
      Cfg->MailAddress.First ();
      Out->DefaultZone = Cfg->MailAddress.Zone;
      Out->Add (Dest.Zone, Dest.Net, Dest.Node, Dest.Point, Dest.Domain);

      Out->New ();
      Out->Zone = Dest.Zone;
      Out->Net = Dest.Net;
      Out->Node = Dest.Node;
      Out->Point = Dest.Point;
      strcpy (Out->Domain, Dest.Domain);
      strcpy (Out->Name, TicName);
      strcpy (Out->Complete, TicFile);
      Out->Status = 'F';
      Out->DeleteAfter = TRUE;
      Out->Add ();

      Out->New ();
      Out->Zone = Dest.Zone;
      Out->Net = Dest.Net;
      Out->Node = Dest.Node;
      Out->Point = Dest.Point;
      strcpy (Out->Domain, Dest.Domain);
      strcpy (Out->Name, Name);
      strcpy (Out->Complete, Complete);
      Out->Status = 'F';
      Out->Add ();

      Out->Update ();
      delete Out;
   }
}

VOID TTicProcessor::Import (VOID)
{
   Description = new TCollection;
   SeenBy = new TCollection;
   Path = new TCollection;

   while (OpenNext () == TRUE) {
      if (ImportTic () == TRUE)
         Delete ();
   }

   if (Path != NULL)
      delete Path;
   if (SeenBy != NULL)
      delete SeenBy;
   if (Description != NULL)
      delete Description;
}

USHORT TTicProcessor::ImportTic (VOID)
{
   int i, a, fds, fdd;
   USHORT RetVal = FALSE, Found = FALSE, Bad = FALSE;
   CHAR *Buffer, *p;
   ULONG FileCrc;
   class TFilechoLink *Link;
   struct stat statbuf;
   struct dosdate_t datet;
   struct dostime_t timet;
   time_t t;
   struct tm *timep;

   if (Output != NULL) {
      sprintf (Display, " %-12.12s  %7ld  %-19.19s  %-24.24s", Name, Size, Area, From.String);
      Output->Add (Display);
   }

   if ((Data = new TFileData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (!stricmp (Data->EchoTag, Area)) {
               Found = TRUE;
               if ((Link = new TFilechoLink (Cfg->SystemPath)) != NULL) {
                  Link->Load (Area);
                  if (Link->Check (From.String) == FALSE && Cfg->Secure == TRUE) {
                     if (Log != NULL)
                        Log->Write ("!Bad origin node %s", From.String);
                     if (Output != NULL) {
                        strcat (Display, " Bad origin");
                        Output->Update (Display);
                     }
                     Bad = TRUE;
                  }
                  delete Link;
               }
               if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                  if (Nodes->Read (From) == TRUE) {
                     if (Nodes->TicPwd[0] != '\0' && stricmp (Nodes->TicPwd, Password)) {
                        if (Log != NULL)
                           Log->Write ("!Bad password %s from %s", Password, From.String);
                        if (Output != NULL) {
                           strcat (Display, " Bad password");
                           Output->Update (Display);
                        }
                        Bad = TRUE;
                     }
                  }
                  delete Nodes;
               }
               if (Bad == FALSE) {
                  if ((fds = open (Complete, O_RDONLY|O_BINARY)) != -1) {
                     if ((Buffer = (PSZ)malloc (8192)) != NULL) {
                        FileCrc = 0xFFFFFFFFL;
                        do {
                           i = read (fds, Buffer, 8192);
                           for (a = 0; a < i; a++)
                              FileCrc = Crc32 (Buffer[a], FileCrc);
                        } while (i == 8192);
                        FileCrc = ~FileCrc;
                        if (FileCrc == Crc) {
                           sprintf (Temp, "%s%s", Data->Download, Name);
                           if ((fdd = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
                              if (Log != NULL)
                                 Log->Write (" Moving %s to %s", Complete, Temp);
                              lseek (fds, 0L, SEEK_SET);
                              do {
                                 i = read (fds, Buffer, 8192);
                                 write (fdd, Buffer, i);
                              } while (i == 8192);
                              close (fdd);
                              RetVal = TRUE;
                           }
                        }
                        else if (Log != NULL)
                           Log->Write ("!%s bad CRC! (%08lX / %08lX)", Name, Crc, FileCrc);
                        free (Buffer);
                     }
                     close (fds);
                  }
               }
               if (RetVal == TRUE && Bad == FALSE) {
                  if (Replace[0] != '\0') {
                     sprintf (Temp, "%s%s", Data->Download, Replace);
                     if (unlink (Temp) == -1) {
                        if (Log != NULL)
                           Log->Write ("*%s doesn't exist", Temp);
                     }
                     else {
                        if (Log != NULL)
                           Log->Write ("*Replaces: %s", Temp);
                     }
                  }
                  if ((File = new TFileBase (Cfg->SystemPath, Data->Key)) != NULL) {
                     if (Replace[0] != '\0') {
                        if (File->Read (Replace) == TRUE)
                           File->Delete ();
                     }
                     if (File->Read (Name) == TRUE)
                        File->Delete ();
                     File->Clear ();
                     strcpy (File->Area, Data->Key);
                     strcpy (File->Name, Name);
                     sprintf (File->Complete, "%s%s", Data->Download, Name);
                     if ((p = (PSZ)Description->First ()) != NULL)
                        do {
                           File->Description->Add (p, (USHORT)(strlen (p) + 1));
                        } while ((p = (PSZ)Description->Next ()) != NULL);

                     _dos_getdate (&datet);
                     _dos_gettime (&timet);
                     File->UplDate.Day = datet.day;
                     File->UplDate.Month = datet.month;
                     File->UplDate.Year = datet.year;
                     File->UplDate.Hour = timet.hour;
                     File->UplDate.Minute = timet.minute;

                     stat (Complete, &statbuf);
                     File->Size = statbuf.st_size;

                     timep = localtime (&statbuf.st_mtime);
                     File->Date.Day = (UCHAR)timep->tm_mday;
                     File->Date.Month = (UCHAR)(timep->tm_mon + 1);
                     File->Date.Year = (USHORT)(timep->tm_year + 1900);
                     File->Date.Hour = (UCHAR)timep->tm_hour;
                     File->Date.Minute = (UCHAR)timep->tm_min;

                     File->Uploader = "TIC Processor";

                     File->Add ();
                     delete File;

                     Data->ActiveFiles++;
                     Data->Update ();
                  }

                  sprintf (Complete, "%s%s", Data->Download, Name);
                  if ((Link = new TFilechoLink (Cfg->SystemPath)) != NULL) {
                     Link->Load (Area);
                     if (Link->First () == TRUE)
                        do {
                           if (strcmp (Link->Address, From.String))
                              SeenBy->Add (Link->Address);
                        } while (Link->Next () == TRUE);

                     t = time (NULL);
                     timep = gmtime (&t);
                     sprintf (Temp, "%s %lu %s", Cfg->MailAddress.String, t, asctime (timep));
                     Temp[strlen (Temp) - 1] = '\0';
                     strcat (Temp, " GMT");
                     Path->Add (Temp);

                     if (Link->First () == TRUE)
                        do {
                           if (strcmp (Link->Address, From.String)) {
                              if (Log != NULL)
                                 Log->Write ("+Sending %s to %s", Name, Link->Address);
                              Password[0] = '\0';
                              if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                                 if (Nodes->Read (From) == TRUE)
                                    strcpy (Password, Nodes->TicPwd);
                                 delete Nodes;
                              }
                              Hatch (Link->Address);
                           }
                        } while (Link->Next () == TRUE);
                     delete Link;
                  }
               }
               break;
            }
         } while (Data->Next () == TRUE);
      delete Data;
   }

   if (RetVal == FALSE) {
      if (Found == FALSE) {
         if (Log != NULL)
            Log->Write ("!Unknown area \"%s\" in %s", Area, CurrentFile);
         if (Output != NULL) {
            strcat (Display, " Bad area");
            Output->Update (Display);
         }
      }
      strcpy (Temp, CurrentFile);
      strcpy (&Temp[strlen (Temp) - 4], ".bad");
      if (Log != NULL)
         Log->Write (":Renaming %s in %s", CurrentFile, Temp);
      rename (CurrentFile, Temp);
   }

   return (RetVal);
}

USHORT TTicProcessor::Open (PSZ pszFile)
{
   FILE *fp;
   USHORT RetVal = FALSE, Count;
   CHAR *Temp, *p;

   if (Inbound[strlen (Inbound) - 1] == '\\' || Inbound[strlen (Inbound) - 1] == '/')
      Inbound[strlen (Inbound) - 1] = '\0';
#if defined(__LINUX__)
   if (Inbound[0] != '\0')
      strcat (Inbound, "/");
#else
   if (Inbound[0] != '\0')
      strcat (Inbound, "\\");
#endif

   Count = 0;
   Description->Clear ();
   SeenBy->Clear ();
   Path->Clear ();
   Replace[0] = '\0';

   if ((fp = fopen (pszFile, "rt")) != NULL) {
      strcpy (CurrentFile, pszFile);
      if ((Temp = (CHAR *)malloc (2048)) != NULL) {
         while (fgets (Temp, 2048 - 1, fp) != NULL) {
            Temp[strlen (Temp) - 1] = '\0';
            if ((p = strtok (Temp, " ")) != NULL) {
               if (!stricmp (p, "Area")) {
                  if ((p = strtok (NULL, " ")) != NULL) {
                     strcpy (Area, strupr (p));
                     if (++Count >= 2)
                        RetVal = TRUE;
                  }
               }
               else if (!stricmp (p, "File")) {
                  if ((p = strtok (NULL, " ")) != NULL) {
                     strcpy (Name, strlwr (p));
                     strcpy (Complete, Inbound);
                     strcat (Complete, Name);
                     if (++Count >= 2)
                        RetVal = TRUE;
                  }
               }
               else if (!stricmp (p, "Size")) {
                  if ((p = strtok (NULL, " ")) != NULL)
                     Size = atol (p);
               }
               else if (!stricmp (p, "CRC")) {
                  if ((p = strtok (NULL, " ")) != NULL)
                     sscanf (p, "%08lx", &Crc);
               }
               else if (!stricmp (p, "Desc")) {
                  if ((p = strtok (NULL, "")) != NULL) {
                     while (*p == ' ')
                        p++;
                     Description->Add (p, (USHORT)(strlen (p) + 1));
                  }
               }
               else if (!stricmp (p, "Ldesc")) {
                  if ((p = strtok (NULL, "")) != NULL) {
                     while (*p == ' ')
                        p++;
                     Description->Add (p, (USHORT)(strlen (p) + 1));
                  }
               }
               else if (!stricmp (p, "Pw")) {
                  if ((p = strtok (NULL, " ")) != NULL)
                     strcpy (Password, strupr (p));
               }
               else if (!stricmp (p, "SeenBy")) {
                  if ((p = strtok (NULL, " ")) != NULL)
                     SeenBy->Add (p, (USHORT)(strlen (p) + 1));
               }
               else if (!stricmp (p, "Path")) {
                  if ((p = strtok (NULL, "")) != NULL) {
                     while (*p == ' ')
                        p++;
                     Path->Add (p, (USHORT)(strlen (p) + 1));
                  }
               }
               else if (!stricmp (p, "From")) {
                  if ((p = strtok (NULL, " ")) != NULL)
                     From.Parse (p);
               }
               else if (!stricmp (p, "Origin")) {
                  if ((p = strtok (NULL, " ")) != NULL)
                     Origin.Parse (p);
               }
               else if (!stricmp (p, "Replace") || !stricmp (p, "Replaces")) {
                  if ((p = strtok (NULL, " ")) != NULL)
                     strcpy (Replace, p);
               }
            }
         }
         free (Temp);
      }
      fclose (fp);
   }

   if (RetVal == TRUE && Log != NULL) {
      Log->Write (":AREA: %s", Area);
      if ((p = (PSZ)Description->First ()) == NULL)
         p = "";
      Log->Write (":%s - %s", Name, p);
      Log->Write (":Originated by %s", Origin.String);
   }

   return (RetVal);
}

USHORT TTicProcessor::OpenNext (VOID)
{
   DIR *dir;
   USHORT RetVal = FALSE;
   CHAR Filename[128], OpenFileName[128];
   ULONG PktDate;
   struct dirent *ent;
   struct stat statbuf;

   strcpy (Filename, Inbound);
   if (Filename[strlen (Filename) - 1] == '\\' || Filename[strlen (Filename) - 1] == '/')
      Filename[strlen (Filename) - 1] = '\0';

   if ((dir = opendir (Filename)) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
         strlwr (ent->d_name);
         if (strstr (ent->d_name, ".tic") != NULL) {
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
            if (strstr (ent->d_name, ".tic") != NULL) {
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
      sprintf (OpenFileName, "%s%s", Inbound, PktName);
      Open (OpenFileName);
   }

   return (RetVal);
}

