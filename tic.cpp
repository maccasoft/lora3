
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
   class TNodes *Nodes;

   Password[0] = '\0';
   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Nodes->Read (Dest.String) == TRUE)
         strcpy (Password, Nodes->TicPwd);
      delete Nodes;
   }

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

      if (SeenBy->First () == TRUE)
         do {
            fprintf (fp, "Seenby %s\n", SeenBy->Address);
         } while (SeenBy->Next () == TRUE);

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
   SeenBy = new TKludges;
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

USHORT TTicProcessor::CheckEchoList (PSZ pszFile, PSZ pszEchoTag)
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

USHORT TTicProcessor::ImportTic (VOID)
{
   int i, a, fds, fdd;
   USHORT RetVal = FALSE, Found = FALSE, Bad = FALSE, Create;
   CHAR Temp[32], *Buffer, *p;
   ULONG FileCrc;
   class TFilechoLink *Link;
   class TNodes *Nodes;
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
      if ((Found = Data->ReadEcho (Area)) == FALSE) {
         if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
            if (Nodes->Read (From.String, FALSE) == TRUE) {
               if (Nodes->CreateNewTic == TRUE) {
                  Create = TRUE;
                  if ((p = strtok (Nodes->NewTicFilter, " ")) != NULL) {
                     Create = FALSE;
                     do {
                        if (*p == '@') {
                           if (CheckEchoList (p, Area) == FALSE)
                              Create = FALSE;
                        }
                        else if (strstr (Area, strupr (p)) != NULL) {
                           Create = TRUE;
                           break;
                        }
                     } while ((p = strtok (NULL, " ")) != NULL);
                  }
               }
            }

            delete Nodes;
         }

         if (Create == TRUE) {
            strcpy (Temp, Area);
            Temp[sizeof (Data->Key) - 1] = '\0';
            while (Data->Read (Temp) == TRUE) {
               if (isdigit (Temp[strlen (Temp) - 1]))
                  Temp[strlen (Temp) - 1]++;
               else
                  Temp[strlen (Temp) - 1] = '0';
            }

            Data->New ();
            strcpy (Data->Key, Temp);
            sprintf (Data->Display, "New area %s", Area);
            sprintf (Data->Download, "%s%s", Cfg->NewTicPath, Area);
            mkdir (Data->Download);
            strcat (Data->Download, "\\");
            strcpy (Data->Upload, Data->Download);
            Data->DownloadLevel = Data->Level = Cfg->NewAreasLevel;
            Data->DownloadFlags = Data->AccessFlags = Cfg->NewAreasFlags;
            Data->DownloadDenyFlags = Data->DenyFlags = Cfg->NewAreasDenyFlags;
            strcpy (Data->EchoTag, Area);
            Data->Add ();

            Found = Data->Read (Temp);
            if (Found == TRUE && Log != NULL)
               Log->Write ("*Created area [%s] %s from %s", Data->Key, Data->EchoTag, From.String);

            if ((Link = new TFilechoLink (Cfg->SystemPath)) != NULL) {
               Link->Load (Area);
               Link->AddString (From.String);

               if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                  if (Nodes->First () == TRUE)
                     do {
                        if (Nodes->LinkNewTic == TRUE && stricmp (Nodes->Address, From.String)) {
                           Link->AddString (Nodes->Address);
                           if (Log != NULL)
                              Log->Write ("-Area %s auto-linked to %s", Area, Nodes->Address);
                        }
                     } while (Nodes->Next () == TRUE);
                  delete Nodes;
               }

               Link->Save ();
               delete Link;
            }
         }

         if (Create == FALSE && Log != NULL)
            Log->Write ("!Node %s can't create new TIC area %s", From.String, Area);
      }

      if (Found == TRUE) {
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
#if !defined(__LINUX__)
               strupr (File->Name);
#endif
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
                     if (SeenBy->Check (Link->Address) == TRUE) {
                        Link->Skip = TRUE;
                        Link->Update ();
                     }
                     else if (strcmp (Link->Address, From.String))
                        SeenBy->AddString (Link->Address);
                  } while (Link->Next () == TRUE);

               t = time (NULL);
               timep = gmtime (&t);
               sprintf (Temp, "%s %lu %s", Cfg->MailAddress.String, t, asctime (timep));
               Temp[strlen (Temp) - 1] = '\0';
               strcat (Temp, " GMT");
               Path->Add (Temp);

               if (Link->First () == TRUE)
                  do {
                     if (strcmp (Link->Address, From.String) && Link->Skip == FALSE) {
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
      }

      if (Found == FALSE || Bad == TRUE) {
         if (Log != NULL && Bad == FALSE) {
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

      delete Data;
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
   SeenBy->Sort = TRUE;
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
                     SeenBy->AddString (p);
               }
               else if (!stricmp (p, "Path")) {
                  if ((p = strtok (NULL, "")) != NULL) {
                     while (*p == ' ')
                        p++;
                     Path->Add (p);
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

