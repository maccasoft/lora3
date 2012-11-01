
#include "_ldefs.h"
#include "lora_api.h"
#include "lora240.h"

PSZ KeyName[] = {
   "TWIT", "DISGRACE", "LIMITED", "NORMAL", "WORTHY", "PRIVEL", "FAVORED",
   "EXTRA", "CLERK", "ASST.SYSOP", "SYSOP"
};

PSZ LimitName[] = {
   "Twit", "Disgrace", "Limited", "Normal", "Worthy", "Privel", "Favored",
   "Extra", "Clerk", "Asst.Sysop", "Sysop"
};

/*
PSZ AdjustPath (PSZ pszPath)
{
#if defined(__LINUX__)
   PSZ p;

   p = pszPath;
   while ((p = strchr (p, '\\')) != NULL)
      *p++ = '/';
#endif

   return (pszPath);
}
*/

char *strcode(char *str,char *key)
{
    char *p,*q;

    q=key;
    for(p=str;*p;p++) {
        if(*p!=*q) *p=*p^*q;
        q++;
        if(!*q) q=key;
    }
    return(str);
}

VOID ConvertEchoLink (PSZ pszFrom, PSZ pszTo)
{
   int fd;
   CHAR Temp[128];
   class TEchoLink *Data;
   struct _sys_msg msg;

   if ((Data = new TEchoLink (pszTo)) != NULL) {
      sprintf (Temp, "%ssysmsg.dat", pszFrom);
      if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &msg, sizeof (msg)) == sizeof (msg)) {
            if (!msg.echomail || msg.echotag[0] == '\0')
               continue;
            Data->Load (msg.echotag);
            Data->Clear ();
            Data->AddString (msg.forward1);
            Data->AddString (msg.forward2);
            Data->AddString (msg.forward3);
            Data->Save ();
         }
         close (fd);
      }
      delete Data;
   }
}

VOID ConvertFileLink (PSZ pszFrom, PSZ pszTo)
{
   int fd;
   CHAR Temp[128];
   class TFilechoLink *Data;
   struct _sys_file file;

   if ((Data = new TFilechoLink (pszTo)) != NULL) {
      sprintf (Temp, "%ssysfile.dat", pszFrom);
      if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &file, sizeof (file)) == sizeof (file)) {
            if (file.tic_tag[0] == '\0')
               continue;
            Data->Load (file.tic_tag);
            Data->Clear ();
            Data->AddString (file.tic_forward1);
            Data->AddString (file.tic_forward2);
            Data->AddString (file.tic_forward3);
            Data->Save ();
         }
         close (fd);
      }
      delete Data;
   }
}

VOID ConvertUsers (PSZ pszFrom, PSZ pszTo)
{
   int fd;
   CHAR Temp[128], *p;
   ULONG TotalCalls = 0L;
   class TUser *Data;
   class TStatistics *Stats;
   struct _usr usr;

   sprintf (Temp, "%susers", pszTo);
   if ((Data = new TUser (Temp)) != NULL) {
      sprintf (Temp, "%susers.bbs", pszFrom);
      if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &usr, sizeof (usr)) == sizeof (usr)) {
            Data->Clear ();
            strcpy (Data->Name, usr.name);
            strcpy (Data->RealName, usr.handle);
            strcpy (Data->City, usr.city);
            if (usr.ansi)
               Data->Ansi = TRUE;
            if (usr.avatar)
               Data->Avatar = TRUE;
            if (usr.color)
               Data->Color = TRUE;
            if (usr.hotkey)
               Data->HotKey = TRUE;
            Data->ScreenHeight = usr.len;
            Data->ScreenWidth = usr.width;
            Data->Level = usr.priv;
            strcpy (Data->LimitClass, "NORMAL");
            switch (usr.priv) {
               case TWIT:
                  strcpy (Data->LimitClass, "TWIT");
                  break;
               case DISGRACE:
                  strcpy (Data->LimitClass, "DISGRACE");
                  break;
               case LIMITED:
                  strcpy (Data->LimitClass, "LIMITED");
                  break;
               case NORMAL:
                  strcpy (Data->LimitClass, "NORMAL");
                  break;
               case WORTHY:
                  strcpy (Data->LimitClass, "WORTHY");
                  break;
               case PRIVIL:
                  strcpy (Data->LimitClass, "PRIVEL");
                  break;
               case FAVORED:
                  strcpy (Data->LimitClass, "FAVORED");
                  break;
               case EXTRA:
                  strcpy (Data->LimitClass, "EXTRA");
                  break;
               case CLERK:
                  strcpy (Data->LimitClass, "CLERK");
                  break;
               case ASSTSYSOP:
                  strcpy (Data->LimitClass, "ASSTSYSOP");
                  break;
               case SYSOP:
                  strcpy (Data->LimitClass, "SYSOP");
                  break;
            }
            Data->AccessFlags = usr.flags;
            Data->SetPassword (strcode (usr.pwd, usr.name));
            strcpy (Data->DayPhone, usr.voicephone);

            strcpy (Temp, Data->Name);
            p = strtok (strlwr (Temp), " ");
            Data->MailBox[0] = *p;                 // La prima lettera della mailbox e'
            Data->MailBox[1] = '\0';               // la prima lettera del nome
            if ((p = strtok (NULL, " ")) == NULL)  // Cerca il cognome
               p = &Temp[1];
            if (strlen (p) > 7)                    // Se la mailbox risultasse piu' di
               p[7] = '\0';                        // otto caratteri, forza gli otto caratteri
            strcat (Data->MailBox, p);

            Data->LastCall = Data->CreationDate = time (NULL);
            Data->TotalCalls = usr.times;
            TotalCalls += Data->TotalCalls;
            Data->UploadFiles = usr.n_upld;
            Data->DownloadFiles = usr.n_dnld;
            Data->UploadBytes = usr.upld;
            Data->DownloadBytes = usr.dnld;
            Data->Add ();
         }
         close (fd);
      }
      delete Data;
   }

   if ((Stats = new TStatistics) != NULL) {
      Stats->Read (1);
      Stats->Calls = 0;
      Stats->TodayCalls = 0;
      Stats->TotalCalls = TotalCalls;
      Stats->Update ();
      delete Stats;
   }
}

VOID ConvertMessageAreas (PSZ pszFrom, PSZ pszTo, USHORT flUseName)
{
   int fd;
   CHAR Temp[128], *p;
   class TMsgData *Data;
   struct _sys_msg msg;
   struct _configuration *cfg;

   if ((cfg = (struct _configuration *)malloc (sizeof (struct _configuration))) != NULL) {
      sprintf (Temp, "%sconfig.dat", pszFrom);
      if ((fd = open (Temp, O_RDONLY|O_BINARY, S_IREAD|S_IWRITE)) != -1) {
         read (fd, cfg, sizeof (struct _configuration));
         close (fd);
      }
   }

   if ((Data = new TMsgData (pszTo)) != NULL) {
      sprintf (Temp, "%ssysmsg.dat", pszFrom);
      if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &msg, sizeof (msg)) == sizeof (msg)) {
            if (msg.quick_board != 0 || msg.gold_board != 0 || msg.pip_board != 0)
               printf ("Can't convert area %d (Not a squish/fido base)\n", msg.msg_num);
            Data->New ();
            if (flUseName == TRUE && msg.qwk_name[0] != '\0') {
               strcpy (Data->Key, msg.qwk_name);
               if ((p = strchr (Data->Key, '\0')) != NULL) {
                  if (p > Data->Key) {
                     p--;
                     while (p > Data->Key && *p == ' ')
                        *p-- = '\0';
                  }
               }
            }
            else
               sprintf (Data->Key, "%d", msg.msg_num);
            strcpy (Data->Display, msg.msg_name);
            if (msg.echomail)
               Data->EchoMail = TRUE;
            strcpy (Data->EchoTag, msg.echotag);
            Data->Level = msg.msg_priv;
            Data->AccessFlags = msg.msg_flags;
            Data->WriteLevel = msg.write_priv;
            Data->WriteFlags = msg.write_flags;
            if (msg.squish != 0) {
               Data->Storage = ST_SQUISH;
               strcpy (Data->Path, msg.msg_path);
            }
            else if (msg.quick_board != 0) {
               Data->Storage = ST_HUDSON;
               strcpy (Data->Path, cfg->quick_msgpath);
            }
            else {
               Data->Storage = ST_FIDO;
               strcpy (Data->Path, msg.msg_path);
            }
            Data->MaxMessages = msg.max_msgs;
            Data->DaysOld = msg.max_age;
            sprintf (Data->Address, "%d:%d/%d.%d", cfg->alias[msg.use_alias].zone, cfg->alias[msg.use_alias].net, cfg->alias[msg.use_alias].node, cfg->alias[msg.use_alias].point);
            Data->Add ();
         }
         close (fd);
      }
      delete Data;
   }

   if (cfg != NULL)
      free (cfg);
}

VOID ConvertEchoMail (PSZ pszFrom, PSZ pszTo)
{
   int fd;
   CHAR Temp[128], *p;
   class TMsgData *Data;
   struct _sys_msg msg;

   if ((Data = new TMsgData (pszTo)) != NULL) {
      sprintf (Temp, "%ssysmsg.dat", pszFrom);
      if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &msg, sizeof (msg)) == sizeof (msg)) {
            if (msg.quick_board != 0 || msg.gold_board != 0 || msg.pip_board != 0)
               printf ("Can't convert area %d (Not a squish/fido base)\n", msg.msg_num);
            printf ("Area %d - %-40.40s ", msg.msg_num, msg.msg_name);
            if (msg.echomail && msg.echotag[0] != '\0') {
               if (Data->ReadEcho (msg.echotag) == FALSE) {
                  Data->New ();
                  sprintf (Data->Key, "%d", msg.msg_num);
                  strcpy (Data->Display, msg.msg_name);
                  if (msg.echomail)
                     Data->EchoMail = TRUE;
                  strcpy (Data->EchoTag, msg.echotag);
                  Data->Level = msg.msg_priv;
                  Data->AccessFlags = msg.msg_flags;
                  Data->WriteLevel = msg.write_priv;
                  Data->WriteFlags = msg.write_flags;
                  if (msg.squish != 0)
                     Data->Storage = ST_SQUISH;
                  else
                     Data->Storage = ST_FIDO;
                  strcpy (Data->Path, msg.msg_path);
                  Data->Add ();
                  printf ("added.\n");
               }
               else {
                  Data->MaxMessages = msg.max_msgs;
                  Data->DaysOld = msg.max_age;
                  Data->Update ();
                  printf ("already present, updating.\n");
               }
            }
            else
               printf ("not echomail, skip.\n");
         }
         close (fd);
      }
      delete Data;
   }
}

VOID ConvertFileAreas (PSZ pszFrom, PSZ pszTo, USHORT flUseName)
{
   int fd;
   CHAR Temp[128], *p;
   class TFileData *Data;
   struct _sys_file file;

   if ((Data = new TFileData (pszTo)) != NULL) {
      sprintf (Temp, "%ssysfile.dat", pszFrom);
      if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &file, sizeof (file)) == sizeof (file)) {
            Data->New ();
            if (flUseName == TRUE && file.short_name[0] != '\0') {
               strcpy (Data->Key, file.short_name);
               if ((p = strchr (Data->Key, '\0')) != NULL) {
                  if (p > Data->Key) {
                     p--;
                     while (p > Data->Key && *p == ' ')
                        *p-- = '\0';
                  }
               }
            }
            else
               sprintf (Data->Key, "%d", file.file_num);
            strcpy (Data->Display, file.file_name);
            strcpy (Data->Download, file.filepath);
            strcpy (Data->Upload, file.uppath);
            Data->Level = file.file_priv;
            Data->AccessFlags = file.file_flags;
            Data->DownloadLevel = file.download_priv;
            Data->DownloadFlags = file.download_flags;
            strcpy (Data->EchoTag, file.tic_tag);
            Data->Add ();
         }
         close (fd);
      }
      delete Data;
   }
}

VOID ConvertScheduler (PSZ pszFrom, PSZ pszTo)
{
   int fd;
   CHAR Temp[128];
   class TEvents *Data;
   struct _event event;

   sprintf (Temp, "%sevents.dat", pszTo);
   if ((Data = new TEvents (Temp)) != NULL) {
      sprintf (Temp, "%ssched.dat", pszFrom);
      if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &event, sizeof (event)) == sizeof (event)) {
            Data->New ();
            Data->Hour = (UCHAR)(event.minute / 60);
            Data->Minute = (UCHAR)(event.minute % 60);
            Data->Length = event.length;
            if (event.days & DAY_SUNDAY)
               Data->Sunday = TRUE;
            if (event.days & DAY_MONDAY)
               Data->Monday = TRUE;
            if (event.days & DAY_TUESDAY)
               Data->Tuesday = TRUE;
            if (event.days & DAY_WEDNESDAY)
               Data->Wednesday = TRUE;
            if (event.days & DAY_THURSDAY)
               Data->Thursday = TRUE;
            if (event.days & DAY_FRIDAY)
               Data->Friday = TRUE;
            if (event.days & DAY_SATURDAY)
               Data->Saturday = TRUE;
            if (event.behavior & MAT_DYNAM)
               Data->Dynamic = TRUE;
            if (event.behavior & MAT_CM)
               Data->SendCrash = TRUE;
            if (event.behavior & MAT_NOCM)
               Data->SendDirect = Data->SendNormal = TRUE;
            Data->SendImmediate = TRUE;
            if (event.behavior & MAT_FORCED)
               Data->Force = TRUE;
            if (!(event.behavior & MAT_BBS))
               Data->MailOnly = TRUE;
            if (event.res_zone != 0) {
               sprintf (Data->Address, "%d:%d/%d.%d", event.res_zone, event.res_net, event.res_node, 0);
               Data->ForceCall = TRUE;
               Data->SendCrash = TRUE;
               Data->SendDirect = Data->SendNormal = TRUE;
            }
            strcpy (Data->Label, event.cmd);
            Data->MaxCalls = event.no_connect;
            Data->MaxConnects = event.with_connect;
            Data->Add ();
         }
         close (fd);
      }
      Data->Save ();
      delete Data;
   }
}

VOID CreateLimits (PSZ pszFrom)
{
   int fd, i;
   CHAR Temp[128];
   struct _configuration *cfg;
   class TLimits *Limits;

   unlink ("limits.dat");
   if ((Limits = new TLimits) != 0) {
      if ((cfg = (struct _configuration *)malloc (sizeof (struct _configuration))) != NULL) {
         sprintf (Temp, "%sconfig.dat", pszFrom);
         if ((fd = open (Temp, O_RDONLY|O_BINARY, S_IREAD|S_IWRITE)) != -1) {
            read (fd, cfg, sizeof (struct _configuration));
            close (fd);

            for (i = 0; i < 11; i++) {
               Limits->New ();
               strcpy (Limits->Key, KeyName[i]);
               strcpy (Limits->Description, LimitName[i]);
               Limits->CallTimeLimit = cfg->uclass[i].max_call;
               Limits->DayTimeLimit = cfg->uclass[i].max_time;
               Limits->DayDownloadLimit = cfg->uclass[i].max_dl;
               Limits->Add ();
            }
         }
         free (cfg);
      }
      delete Limits;
   }
}

VOID ConvertFilebase (PSZ pszTo)
{
   FILE *fp;
   USHORT PendingWrite;
   ULONG Total;
   CHAR Path[128], Temp[128], *p, *Name;
   struct find_t blk;
   class TFileData *Data;
   class TFileBase *File;

   if ((Data = new TFileData (pszTo)) != NULL) {
      if (Data->First () == TRUE)
         do {
            PendingWrite = FALSE;
            Total = 0L;
            if ((File = new TFileBase (pszTo, Data->Key)) != NULL) {
               sprintf (Path, "%sfiles.bbs", Data->Download);
               if ((fp = fopen (Path, "rt")) != NULL) {
                  while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                     if ((p = strchr (Temp, 0x0A)) != NULL)
                        *p = '\0';
                     if (Temp[1] == '>') {
                        if (PendingWrite == TRUE)
                           File->Description->Add (&Temp[2], strlen (&Temp[2]) + 1);
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
                                 File->Description->Add (p, strlen (p) + 1);
                           }
                           sprintf (Path, "%s%s", Data->Download, Name);
                           if (!_dos_findfirst (Path, 0, &blk)) {
                              strcpy (File->Area, Data->Key);
                              strcpy (File->Name, Name);
                              sprintf (File->Complete, "%s%s", Data->Download, blk.name);
                              File->Size = blk.size;
                              File->UplDate.Day = File->Date.Day = (UCHAR)(blk.wr_date & 0x1F);
                              File->UplDate.Month = File->Date.Month = (UCHAR)((blk.wr_date & 0x1E0) >> 5);
                              File->UplDate.Year = File->Date.Year = (USHORT)(((blk.wr_date & 0xFE00) >> 9) + 1980);
                              File->UplDate.Hour = File->Date.Hour = (UCHAR)((blk.wr_time & 0xF800) >> 11);
                              File->UplDate.Minute = File->Date.Minute = (UCHAR)((blk.wr_time & 0x7E0) >> 5);
                              File->Uploader = "Sysop";
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
               delete File;

               Data->ActiveFiles = Total;
               Data->Update ();
            }
         } while (Data->Next () == TRUE);
      delete Data;
   }
}

VOID ConvertNodes (PSZ pszFrom, PSZ pszTo)
{
   int fd;
   CHAR Temp[128];
   class TNodes *Data;
   struct _nodeinfo nodes;
   struct _configuration *cfg;

   if ((cfg = (struct _configuration *)malloc (sizeof (struct _configuration))) != NULL) {
      sprintf (Temp, "%sconfig.dat", pszFrom);
      if ((fd = open (Temp, O_RDONLY|O_BINARY, S_IREAD|S_IWRITE)) != -1) {
         read (fd, cfg, sizeof (struct _configuration));
         close (fd);
      }
   }

   if ((Data = new TNodes (pszTo)) != NULL) {
      sprintf (Temp, "%snodes.dat", cfg->net_info);
      if ((fd = open (Temp, O_RDONLY|O_BINARY)) == -1) {
         sprintf (Temp, "%s%snodes.dat", pszFrom, cfg->net_info);
         fd = open (Temp, O_RDONLY|O_BINARY);
      }

      if (fd != -1) {
         while (read (fd, &nodes, sizeof (nodes)) == sizeof (nodes)) {
            Data->New ();
            sprintf (Data->Address, "%d:%d/%d.%d", nodes.zone, nodes.net, nodes.node, nodes.point);
            strcpy (Data->SystemName, nodes.system);
            strcpy (Data->SysopName, nodes.sysop_name);
            strcpy (Data->Phone, nodes.phone);
            strcpy (Data->SessionPwd, nodes.pw_session);
            strcpy (Data->OutPktPwd, nodes.pw_packet);
            strcpy (Data->AreaMgrPwd, nodes.pw_areafix);
            if (nodes.pw_inbound_packet[0] != '\0')
               strcpy (Data->InPktPwd, nodes.pw_inbound_packet);
            else
               strcpy (Data->InPktPwd, nodes.pw_packet);
            Data->RemapMail = nodes.remap4d ? TRUE : FALSE;
            sprintf (Data->Packer, "%c", cfg->packid[nodes.packer].display[0]);
            Data->Add ();
         }
         close (fd);
      }
      delete Data;
   }

   if (cfg != NULL)
      free (cfg);
}

VOID ConvertPackers (PSZ pszFrom, PSZ pszTo)
{
   int fd, i;
   CHAR Temp[128];
   struct _configuration *cfg;
   class TPacker *Packer;

   if ((Packer = new TPacker (pszTo)) != 0) {
      if ((cfg = (struct _configuration *)malloc (sizeof (struct _configuration))) != NULL) {
         sprintf (Temp, "%sconfig.dat", pszFrom);
         if ((fd = open (Temp, O_RDONLY|O_BINARY, S_IREAD|S_IWRITE)) != -1) {
            read (fd, cfg, sizeof (struct _configuration));
            close (fd);

            for (i = 0; i < 10; i++) {
               if (cfg->packid[i].display[0] != '\0') {
                  Packer->New ();
                  sprintf (Packer->Key, "%c", cfg->packid[i].display[0]);
                  strcpy (Packer->Display, &cfg->packid[i].display[1]);
                  strcpy (Packer->Id, cfg->packid[i].ident);
                  strcpy (Packer->PackCmd, cfg->packers[i].packcmd);
                  strcpy (Packer->UnpackCmd, cfg->packers[i].unpackcmd);
                  Packer->Position = cfg->packid[i].offset;
                  Packer->Add ();
               }
            }
         }
         free (cfg);
      }
      delete Packer;
   }
}

void main (int argc, char *argv[])
{
   USHORT i, UseShortName = FALSE;
   PSZ From = NULL, To = NULL, What = NULL;

   printf ("\nLCONV; LoraBBS 2.40 -> 3.00 File Conversion Utility\n");
   printf ("       Copyright (c) 1996 by Marco Maccaferri. All rights reserved.\n");

   for (i = 1; i < argc; i++) {
      if (!stricmp (argv[i], "-N"))
         UseShortName = TRUE;
      else if (What == NULL)
         What = argv[i];
      else if (From == NULL)
         From = argv[i];
      else if (To == NULL)
         To = argv[i];
   }

   if (From == NULL)
      From = ".\\";
   if (To == NULL)
      To = "";

   if (!stricmp (What, "msg") || !stricmp (What, "sysmsg"))
      ConvertMessageAreas (From, To, UseShortName);
//   else if (!stricmp (What, "echomail"))
//      ConvertEchoMail (From, To);
   else if (!stricmp (What, "file") || !stricmp (What, "sysfile"))
      ConvertFileAreas (From, To, UseShortName);
   else if (!stricmp (What, "sched") || !stricmp (What, "events"))
      ConvertScheduler (From, To);
   else if (!stricmp (What, "users"))
      ConvertUsers (From, To);
   else if (!stricmp (What, "limits"))
      CreateLimits (From);
//   else if (!stricmp (What, "filebase"))
//      ConvertFilebase (To);
   else if (!stricmp (What, "nodes"))
      ConvertNodes (From, To);
   else if (!stricmp (What, "echolink"))
      ConvertEchoLink (From, To);
   else if (!stricmp (What, "filelink"))
      ConvertFileLink (From, To);
   else if (!stricmp (What, "packers"))
      ConvertPackers (From, To);
   else {
      printf ("\nUsage:  lconv [what] [from] [to] [-N]\n\n");
      printf ("        what - What to convert: msg, file, sched, users, limits\n");
      printf ("               nodes, echolink, packers, filelink\n");
      printf ("        from - Location of the Lora 2.40 files (default current directory)\n");
      printf ("        to   - Location where to write the new files (default current dir)\n");
      printf ("        -N   - Use short names instead of the area number for the message\n");
      printf ("               and file areas\n");
      printf ("\n");
   }
}
