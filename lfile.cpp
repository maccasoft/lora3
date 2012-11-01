
#include "_ldefs.h"
#include "lora_api.h"

class TConfig *Cfg;

VOID ExportFilesBBS (VOID)
{
   FILE *fp;
   CHAR Path[128], *p;
   ULONG Total;
   class TFileData *Data;
   class TFileBase *File;

   printf (" * Export to FILES.BBS\r\n");

   if ((Data = new TFileData (".\\")) != NULL) {
      if (Data->First () == TRUE)
         do {
            Total = 0L;
            printf (" +-- %-15.15s %-45.45s ", Data->Key, Data->Display);
            if ((File = new TFileBase (".\\", Data->Key)) != NULL) {
               sprintf (Path, "%sfiles.bbs", Data->Download);
               if ((fp = fopen (Path, "wt")) != NULL) {
                  File->SortByName ();
                  if (File->First () == TRUE)
                     do {
                        Total++;
                        if ((p = (CHAR *)File->Description->First ()) == NULL)
                           p = "";
                        fprintf (fp, "%-12.12s (%3lu) %s\n", File->Name, File->DlTimes, p);
                        while ((p = (CHAR *)File->Description->Next ()) != NULL)
                           fprintf (fp, " >%s\n", p);
                     } while (File->Next () == TRUE);
                  fclose (fp);
               }
               delete File;
            }
            printf ("Total: %5lu\r\n", Total);
         } while (Data->Next () == TRUE);
      delete Data;
   }
}

VOID ImportFilesBBS (VOID)
{
   FILE *fp;
   USHORT PendingWrite;
   ULONG Total;
   CHAR Path[128], Temp[128], *p, *Name;
   struct stat statbuf;
   struct tm *ltm;
   class TFileData *Data;
   class TFileBase *File;

   printf (" * Import from FILES.BBS\r\n");

   unlink ("filebase.dat");
   unlink ("filebase.idx");

   if ((Data = new TFileData (".\\")) != NULL) {
      if (Data->First () == TRUE)
         do {
            PendingWrite = FALSE;
            Total = 0L;
            printf (" +-- %-15.15s %-45.45s ", Data->Key, Data->Display);
            if ((File = new TFileBase (".\\", Data->Key)) != NULL) {
               sprintf (Path, "%sfiles.bbs", Data->Download);
               if ((fp = fopen (Path, "rt")) != NULL) {
                  while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                     if ((p = strchr (Temp, 0x0A)) != NULL)
                        *p = '\0';
                     if (Temp[1] == '>') {
                        if (PendingWrite == TRUE)
                           File->Description->Add (&Temp[2], (USHORT)(strlen (&Temp[2]) + 1));
                     }
                     else {
                        if (PendingWrite == TRUE) {
                           File->Add ();
                           Total++;
                           File->Clear ();
                           PendingWrite = FALSE;
                        }
                        if ((Name = strtok (Temp, " ")) != NULL) {
                           if ((p = strtok (NULL, "")) != NULL) {
                              while (*p == ' ')
                                 p++;
                              if (*p == '(' || *p == '[') {
                                 while (*p != ')' && *p != ']' && *p != '\0') {
                                    if (isdigit (*p)) {
                                       File->DlTimes *= 10;
                                       File->DlTimes += *p - '0';
                                    }
                                    p++;
                                 }
                                 if (*p == ')' || *p == ']') {
                                    p++;
                                    while (*p == ' ')
                                       p++;
                                 }
                              }
                              if (*p != '\0')
                                 File->Description->Add (p, (USHORT)(strlen (p) + 1));
                           }
                           sprintf (Path, "%s%s", Data->Download, Name);
#if defined(__LINUX__)
                           strlwr (Path);
#endif
                           if (!stat (Path, &statbuf)) {
                              strcpy (File->Area, Data->Key);
                              strcpy (File->Name, Name);
                              sprintf (File->Complete, "%s%s", Data->Download, Name);
                              File->Size = statbuf.st_size;
                              ltm = localtime ((time_t *)&statbuf.st_mtime);
                              File->UplDate.Day = File->Date.Day = (UCHAR)ltm->tm_mday;
                              File->UplDate.Month = File->Date.Month = (UCHAR)(ltm->tm_mon + 1);
                              File->UplDate.Year = File->Date.Year = (USHORT)(ltm->tm_year + 1900);
                              File->UplDate.Hour = File->Date.Hour = (UCHAR)ltm->tm_hour;
                              File->UplDate.Minute = File->Date.Minute = (UCHAR)ltm->tm_min;
                              File->Uploader = "Sysop";
                              File->CdRom = Data->CdRom;
                              PendingWrite = TRUE;
                           }
                        }
                     }
                  }
                  fclose (fp);

                  if (PendingWrite == TRUE) {
                     File->Add ();
                     File->Clear ();
                     Total++;
                     PendingWrite = FALSE;
                  }
               }
               else
                  printf ("Error\r\n");

               File->Close ();
               delete File;

               Data->ActiveFiles = Total;
               Data->Update ();
            }
            printf ("Total: %5lu\r\n", Total);
         } while (Data->Next () == TRUE);
      delete Data;
   }
}

VOID PurgeFiles (USHORT DaysOld)
{
   ULONG Total, Deleted;
   time_t Today, FileDate;
   struct tm ltm;
   class TFileData *Data;
   class TFileBase *File;

   printf (" * Purging Files\r\n");
   Today = time (NULL) / 86400L;

   if ((Data = new TFileData (".\\")) != NULL) {
      if (Data->First () == TRUE)
         do {
            Deleted = Total = 0L;
            printf (" +-- %-15.15s %-29.29s ", Data->Key, Data->Display);
            if ((File = new TFileBase (".\\", Data->Key)) != NULL) {
               if (File->First () == TRUE)
                  do {
                     Total++;

                     ltm.tm_mday = File->UplDate.Day;
                     ltm.tm_mon = File->UplDate.Month - 1;
                     ltm.tm_year = File->UplDate.Year - 1900;
                     ltm.tm_hour = File->UplDate.Hour;
                     ltm.tm_min = File->UplDate.Minute;
                     ltm.tm_sec = 0;
                     FileDate = mktime (&ltm) / 86400L;

                     if ((Today - FileDate) > DaysOld) {
                        unlink (File->Complete);
                        File->Delete ();
                        Deleted++;
                     }
                  } while (File->Next () == TRUE);
               delete File;
            }
            printf ("Total: %5lu, Deleted: %5lu\r\n", Total, Deleted);
         } while (Data->Next () == TRUE);
      delete Data;
   }
}

VOID PackFilebase (VOID)
{
   class TFileBase *File;

   printf (" * Packing (Compressing) Filebase\r\n");

   if ((File = new TFileBase (".\\", "")) != NULL) {
      File->Pack ();
      delete File;
   }
}

VOID CreateFilesList (VOID)
{
   FILE *fp;
   CHAR *p;
   class TFileData *Data;
   class TFileBase *File;

   printf (" * Creating List of Files (bbslist.txt)\r\n");

   if ((fp = _fsopen ("bbslist.txt", "wt", SH_DENYNO)) != NULL) {
      if ((Data = new TFileData (".\\")) != NULL) {
         if (Data->First () == TRUE)
            do {
               printf (" +-- %-15.15s %-29.29s ", Data->Key, Data->Display);
               if ((File = new TFileBase (".\\", Data->Key)) != NULL) {
                  File->SortByName ();
                  if (File->First () == TRUE) {
                     fprintf (fp, "\nLibrary: %s\n", Data->Key);
                     fprintf (fp, "Description: %s\n", Data->Display);
                     fprintf (fp, "There are %lu files available for download\n\n", Data->ActiveFiles);

                     fprintf (fp, "File Name    Size  Date  Description\n============ ===== ===== =====================================================\n");
                     do {
                        if ((p = (PSZ)File->Description->First ()) == NULL)
                           p = "";
                        fprintf (fp, "%-12.12s %4ldK %02d/%02d %.53s\n", File->Name, (File->Size + 1023) / 1024, File->UplDate.Month, File->UplDate.Year % 100, p);
                        while ((p = (PSZ)File->Description->Next ()) != NULL)
                           fprintf (fp, "                         %.53s\n", p);
                     } while (File->Next () == TRUE);

                     fprintf (fp, "\n");
                  }
                  delete File;
               }
            } while (Data->Next () == TRUE);

         delete Data;
      }
      fclose (fp);
   }
}

VOID UpdateFilebase (USHORT KeepDate)
{
   FILE *fp;
   DIR *dir;
   ULONG Total, Added;
   CHAR *p, Path[128], Temp[128];
   time_t today;
   struct stat statbuf;
   struct tm *ltm;
   struct dirent *ent;
   class TFileData *Data;
   class TFileBase *File;
   class TPacker *Packer;

   printf (" * Updating filebase\r\n");

   unlink ("file_id.diz");
   Packer = new TPacker (Cfg->SystemPath);

   if ((Data = new TFileData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            Total = 0L;
            Added = 0L;
            cprintf (" +-- %-15.15s %-32.32s ", Data->Key, Data->Display);
            if ((File = new TFileBase (Cfg->SystemPath, Data->Key)) != NULL) {
               strcpy (Temp, Data->Download);
               if (Temp[strlen (Temp) - 1] == '\\' || Temp[strlen (Temp) - 1] == '/')
                  Temp[strlen (Temp) - 1] = '\0';

               if (File->First () == TRUE)
                  do {
                     sprintf (Path, "%s%s", Data->Download, File->Name);
                     if (stat (AdjustPath (Path), &statbuf))
                        File->Delete ();
                  } while (File->Next () == TRUE);

               File->SortByName ();
               if ((dir = opendir (AdjustPath (Temp))) != NULL) {
                  while ((ent = readdir (dir)) != NULL) {
                     if (!strcmp (ent->d_name, ".") || !strcmp (ent->d_name, ".."))
                        continue;
                     if (File->Read (ent->d_name) == FALSE) {
                        sprintf (Path, "%s%s", Data->Download, ent->d_name);
                        if (!stat (AdjustPath (Path), &statbuf)) {
                           File->Clear ();
                           strcpy (File->Area, Data->Key);
                           strcpy (File->Name, ent->d_name);
                           strcpy (File->Complete, Path);
                           File->Size = statbuf.st_size;
                           ltm = localtime ((time_t *)&statbuf.st_mtime);
                           File->Date.Day = (UCHAR)ltm->tm_mday;
                           File->Date.Month = (UCHAR)(ltm->tm_mon + 1);
                           File->Date.Year = (USHORT)(ltm->tm_year + 1900);
                           File->Date.Hour = (UCHAR)ltm->tm_hour;
                           File->Date.Minute = (UCHAR)ltm->tm_min;
                           if (KeepDate == FALSE) {
                              today = time (NULL);
                              ltm = localtime (&today);
                              File->UplDate.Day = (UCHAR)ltm->tm_mday;
                              File->UplDate.Month = (UCHAR)(ltm->tm_mon + 1);
                              File->UplDate.Year = (USHORT)(ltm->tm_year + 1900);
                              File->UplDate.Hour = (UCHAR)ltm->tm_hour;
                              File->UplDate.Minute = (UCHAR)ltm->tm_min;
                           }
                           else {
                              File->UplDate.Day = File->Date.Day;
                              File->UplDate.Month = File->Date.Month;
                              File->UplDate.Year = File->Date.Year;
                              File->UplDate.Hour = File->Date.Hour;
                              File->UplDate.Minute = File->Date.Minute;
                           }
                           File->Uploader = "Sysop";
                           File->CdRom = Data->CdRom;
                           File->Description->Add ("Description missing");
                           if (Packer != NULL) {
                              if (Packer->CheckArc (Path) == TRUE) {
                                 Packer->DoUnpack (Path, ".\\", "file_id.diz");
                                 if (!stat ("file_id.diz", &statbuf)) {
                                    if ((fp = fopen ("file_id.diz", "rt")) != NULL) {
                                       File->Description->Clear ();
                                       while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                                          if ((p = strchr (Temp, '\n')) != NULL)
                                             *p = '\0';
                                          if ((p = strchr (Temp, '\r')) != NULL)
                                             *p = '\0';
                                          File->Description->Add (Temp);
                                       }
                                       fclose (fp);
                                    }
                                    unlink ("file_id.diz");
                                 }
                              }
                           }
                           File->Add ();
                           Added++;
                        }
                     }
                     Total++;
                  }
                  closedir (dir);
               }
               delete File;
            }
            cprintf ("Total: %5lu Added: %5lu\r\n", Total, Added);
         } while (Data->Next () == TRUE);
      delete Data;
   }

   if (Packer != NULL)
      delete Packer;
}

void main (int argc, char *argv[])
{
   int i;
   USHORT Purge, Pack, Import, Export, DaysOld, List, Update;
   USHORT KeepDate;
   CHAR *Config = NULL;

#if !defined(__LINUX__)
   _grow_handles (64);
#endif

   Update = Purge = Pack = List = Import = Export = FALSE;
   KeepDate = FALSE;
   DaysOld = 65535U;

   cprintf ("\r\nLFILE; %s v%s - File maintenance utility\r\n", NAME, VERSION);
   cprintf ("       Copyright (c) 1991-96 by Marco Maccaferri. All Rights Reserved.\r\n\r\n");

   if (ValidateKey ("bbs", NULL, NULL) == KEY_UNREGISTERED) {
      cprintf ("* * *     WARNING: No license key found    * * *\r\n");
      if ((i = CheckExpiration ()) == 0) {
         cprintf ("* * *   This evaluation copy has expired   * * *\r\n\a\r\n");
         exit (0);
      }
      else
         cprintf ("* * * You have %2d days left for evaluation * * * \r\n\a\r\n", i);
   }

   if (argc <= 1) {
      cprintf (" * Command-line parameters:\r\n\r\n");

      cprintf ("        -U[K]     Update FILEBASE\r\n");
      cprintf ("                  K=Keep file date\r\n");
      cprintf ("        -I        Import from FILES.BBS\r\n");
      cprintf ("        -E        Export to FILES.BBS\r\n");
      cprintf ("        -P[K]     Pack (compress) file base\r\n");
      cprintf ("                  K=Purge\r\n");
      cprintf ("        -K<d>     Purge files that area <d> days old\r\n");
      cprintf ("        -L        Create a list of available files\r\n");

      cprintf ("\r\n * Please refer to the documentation for a more complete command summary\r\n\r\n");
   }
   else {
      for (i = 1; i < argc; i++) {
         if (argv[i][0] == '-' || argv[i][0] == '/') {
            switch (toupper (argv[i][1])) {
               case 'I':
                  Import = TRUE;
                  break;
               case 'E':
                  Export = TRUE;
                  break;
               case 'K':
                  Purge = TRUE;
                  DaysOld = (USHORT)atoi (&argv[i][1]);
                  break;
               case 'L':
                  List = TRUE;
                  break;
               case 'P':
                  Pack = TRUE;
                  if (toupper (argv[i][2]) == 'K')
                     Purge = TRUE;
                  break;
               case 'U':
                  Update = TRUE;
                  if (toupper (argv[i][2]) == 'K')
                     KeepDate = TRUE;
                  break;
            }
         }
         else if (Config == NULL)
            Config = argv[i];
      }

      if ((Cfg = new TConfig) != NULL) {
         if (Cfg->Load (Config, NULL) == FALSE)
            Cfg->Default ();
      }

      if (Update == TRUE)
         UpdateFilebase (KeepDate);
      if (Import == TRUE)
         ImportFilesBBS ();
      if (Purge == TRUE)
         PurgeFiles (DaysOld);
      if (Pack == TRUE)
         PackFilebase ();
      if (Export == TRUE)
         ExportFilesBBS ();
      if (List == TRUE)
         CreateFilesList ();

      if (Import == TRUE || Export == TRUE || Purge == TRUE || Pack == TRUE || List == TRUE || Update == TRUE)
         cprintf (" * Done\r\n\r\n");
      else
         cprintf (" * Nothing to do\r\n\r\n");

      if (Cfg != NULL)
         delete Cfg;
   }
}
