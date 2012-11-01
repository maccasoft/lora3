
#include "_ldefs.h"
#include "files.h"
#include "mailer.h"
#include "msgdata.h"
#include "schedule.h"

VOID ReadEventText (PSZ pszPath, PSZ pszText)
{
   FILE *fp;
   CHAR Temp[256], *p, *arg, PendingWrite = FALSE;
   class TScheduler *Sched;

   sprintf (Temp, "%sEvents.Dat", pszPath);
   unlink (Temp);

   if (pszText == NULL)
      pszText = "Schedule.Cfg";

   if ((Sched = new TScheduler (pszPath)) != NULL) {
      if ((fp = fopen (pszText, "rt")) != NULL) {
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            if (Temp[0] != '%') {
               if (Temp[strlen (Temp) - 1] == '\n')
                  Temp[strlen (Temp) - 1] = '\0';

               if ((p = strtok (Temp, " ")) != NULL) {
                  if (!stricmp (p, "Event")) {
                     if (PendingWrite == TRUE)
                        Sched->Add ();
                     Sched->New ();
                     if ((p = strtok (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (Sched->Label, p);
                        PendingWrite = TRUE;
                     }
                  }
                  else if (!stricmp (p, "Start")) {
                     if ((p = strtok (NULL, " :")) != NULL) {
                        Sched->Hour = (UCHAR)atoi (p);
                        if ((p = strtok (NULL, " ")) != NULL)
                           Sched->Minute = (UCHAR)atoi (p);
                     }
                  }
                  else if (!stricmp (p, "External")) {
                     if ((p = strtok (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (Sched->External, p);
                     }
                  }
                  else if ((arg = strtok (NULL, " ")) != NULL) {
                     if (!stricmp (p, "Length"))
                        Sched->Length = (USHORT)atoi (arg);
                     else if (!stricmp (p, "Force"))
                        Sched->Force = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "Channel"))
                        Sched->Channel = (USHORT)atoi (arg);
                     else if (!stricmp (p, "CallNode"))
                        Sched->CallNode = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "ForceCall"))
                        Sched->ForceCall = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "Address"))
                        strcpy (Sched->Address, arg);
                     else if (!stricmp (p, "Delay"))
                        Sched->Delay = (USHORT)atoi (arg);
                     else if (!stricmp (p, "Retries"))
                        Sched->Retries = (USHORT)atoi (arg);
                     else if (!stricmp (p, "Failures"))
                        Sched->Failures = (USHORT)atoi (arg);
                     else if (!stricmp (p, "ImportMail"))
                        Sched->ImportMail = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "ExportMail"))
                        Sched->ExportMail = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "StartImport"))
                        Sched->StartImport = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "StartExport"))
                        Sched->StartExport = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "EndImport"))
                        Sched->EndImport = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "EndExport"))
                        Sched->EndExport = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "UserMaint"))
                        Sched->UserMaint = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "MessageMaint"))
                        Sched->MessageMaint = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "UpdateNewsgroups"))
                        Sched->UpdateNewsgroups = (UCHAR)atoi (arg);
                  }
               }
            }
         }
         fclose (fp);

         if (PendingWrite == TRUE)
            Sched->Add ();
      }
      delete Sched;
   }
}

VOID WriteEventText (PSZ pszPath, PSZ pszText)
{
   FILE *fp;
   class TScheduler *Sched;

   if (pszText == NULL)
      pszText = "Schedule.Cfg";

   if ((Sched = new TScheduler (pszPath)) != NULL) {
      if (Sched->First () == TRUE) {
         if ((fp = fopen (pszText, "wt")) != NULL) {
            fprintf (fp, "%%\n%%\n%%\n");
            do {
               fprintf (fp, "Event %s\n", Sched->Label);
               fprintf (fp, "    Start            %2d:%02d\n", Sched->Hour, Sched->Minute);
               fprintf (fp, "    Length           %u\n", Sched->Length);
               fprintf (fp, "    Force            %u\n", Sched->Force);
               fprintf (fp, "    Channel          %u\n", Sched->Channel);
               fprintf (fp, "    CallNode         %u\n", Sched->CallNode);
               fprintf (fp, "    ForceCall        %u\n", Sched->ForceCall);
               fprintf (fp, "    Address          %s\n", Sched->Address);
               fprintf (fp, "    Delay            %u\n", Sched->Delay);
               fprintf (fp, "    Retries          %u\n", Sched->Retries);
               fprintf (fp, "    Failures         %u\n", Sched->Failures);
               fprintf (fp, "    ImportMail       %u\n", Sched->ImportMail);
               fprintf (fp, "    ExportMail       %u\n", Sched->ExportMail);
               fprintf (fp, "    StartImport      %u\n", Sched->StartImport);
               fprintf (fp, "    StartExport      %u\n", Sched->StartExport);
               fprintf (fp, "    EndImport        %u\n", Sched->EndImport);
               fprintf (fp, "    EndExport        %u\n", Sched->EndExport);
               fprintf (fp, "    UserMaint        %u\n", Sched->UserMaint);
               fprintf (fp, "    MessageMaint     %u\n", Sched->MessageMaint);
               fprintf (fp, "    UpdateNewsgroups %u\n", Sched->UpdateNewsgroups);
               fprintf (fp, "    External         %s\n", Sched->External);
               fprintf (fp, "%%\n%%\n%%\n");
            } while (Sched->Next () == TRUE);
            fclose (fp);
         }
      }

      delete Sched;
   }
}

VOID ReadFileText (PSZ pszPath, PSZ pszText)
{
   FILE *fp;
   CHAR Temp[256], *p, *arg, PendingWrite = FALSE;
   class FileData *File;

   sprintf (Temp, "%sFile.Dat", pszPath);
   unlink (Temp);
   sprintf (Temp, "%sFile.Idx", pszPath);
   unlink (Temp);

   if (pszText == NULL)
      pszText = "File.Cfg";

   if ((File = new FileData (pszPath)) != NULL) {
      if ((fp = fopen (pszText, "rt")) != NULL) {
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            if (Temp[0] != '%') {
               if (Temp[strlen (Temp) - 1] == '\n')
                  Temp[strlen (Temp) - 1] = '\0';

               if ((p = strtok (Temp, " ")) != NULL) {
                  if (!stricmp (p, "Area")) {
                     if (PendingWrite == TRUE)
                        File->Add ();
                     File->Reset ();
                     if ((p = strtok (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (File->Key, p);
                        PendingWrite = TRUE;
                     }
                  }
                  else if (!stricmp (p, "Description")) {
                     if ((p = strtok (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (File->Display, p);
                     }
                  }
                  else if (!stricmp (p, "Moderator")) {
                     if ((p = strtok (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (File->Moderator, p);
                     }
                  }
                  else if ((arg = strtok (NULL, " ")) != NULL) {
                     if (!stricmp (p, "Level"))
                        File->Level = (USHORT)atoi (arg);
                     else if (!stricmp (p, "AccessFlags"))
                        File->AccessFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "DenyFlags"))
                        File->DenyFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "UploadLevel"))
                        File->UploadLevel = (USHORT)atoi (arg);
                     else if (!stricmp (p, "UploadFlags"))
                        File->UploadFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "UploadDenyFlags"))
                        File->UploadDenyFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "DownloadLevel"))
                        File->DownloadLevel = (USHORT)atoi (arg);
                     else if (!stricmp (p, "DownloadFlags"))
                        File->DownloadFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "DownloadDenyFlags"))
                        File->DownloadDenyFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "Age"))
                        File->Age = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "Download"))
                        strcpy (File->Download, arg);
                     else if (!stricmp (p, "Upload"))
                        strcpy (File->Upload, arg);
                     else if (!stricmp (p, "CdRom"))
                        File->CdRom = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "FreeDownload"))
                        File->FreeDownload = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "ShowGlobal"))
                        File->ShowGlobal = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "MenuName"))
                        strcpy (File->MenuName, arg);
                     else if (!stricmp (p, "Cost"))
                        File->Cost = (USHORT)atoi (arg);
                     else if (!stricmp (p, "ActiveFiles"))
                        File->ActiveFiles = (ULONG)atoi (arg);
                     else if (!stricmp (p, "UnapprovedFiles"))
                        File->UnapprovedFiles = (ULONG)atoi (arg);
                     else if (!stricmp (p, "EchoTag"))
                        strcpy (File->EchoTag, arg);
                  }
               }
            }
         }
         fclose (fp);

         if (PendingWrite == TRUE)
            File->Add ();
      }
      delete File;
   }
}

VOID WriteFileText (PSZ pszPath, PSZ pszText)
{
   FILE *fp;
   class FileData *File;

   if (pszText == NULL)
      pszText = "File.Cfg";

   if ((File = new FileData (pszPath)) != NULL) {
      if (File->First () == TRUE) {
         if ((fp = fopen (pszText, "wt")) != NULL) {
            fprintf (fp, "%%\n%%\n%%\n");
            do {
               fprintf (fp, "Area %s\n", File->Key);
               fprintf (fp, "    Description       %s\n", File->Display);
               fprintf (fp, "    Level             %u\n", File->Level);
               fprintf (fp, "    AccessFlags       %lu\n", File->AccessFlags);
               fprintf (fp, "    DenyFlags         %lu\n", File->DenyFlags);
               fprintf (fp, "    UploadLevel       %u\n", File->UploadLevel);
               fprintf (fp, "    UploadFlags       %lu\n", File->UploadFlags);
               fprintf (fp, "    UploadDenyFlags   %lu\n", File->UploadDenyFlags);
               fprintf (fp, "    DownloadLevel     %u\n", File->DownloadLevel);
               fprintf (fp, "    DownloadFlags     %lu\n", File->DownloadFlags);
               fprintf (fp, "    DownloadDenyFlags %lu\n", File->DownloadDenyFlags);
               fprintf (fp, "    Age               %u\n", File->Age);
               fprintf (fp, "    Download          %s\n", File->Download);
               fprintf (fp, "    Upload            %s\n", File->Upload);
               fprintf (fp, "    CdRom             %u\n", File->CdRom);
               fprintf (fp, "    FreeDownload      %u\n", File->FreeDownload);
               fprintf (fp, "    ShowGlobal        %u\n", File->ShowGlobal);
               fprintf (fp, "    MenuName          %s\n", File->MenuName);
               fprintf (fp, "    Moderator         %s\n", File->Moderator);
               fprintf (fp, "    Cost              %d\n", File->Cost);
               fprintf (fp, "    ActiveFiles       %lu\n", File->ActiveFiles);
               fprintf (fp, "    UnapprovedFiles   %lu\n", File->UnapprovedFiles);
               fprintf (fp, "    EchoTag           %s\n", File->EchoTag);
               fprintf (fp, "%%\n%%\n%%\n");
            } while (File->Next () == TRUE);
            fclose (fp);
         }
      }

      delete File;
   }
}

VOID ReadMsgText (PSZ pszPath, PSZ pszText)
{
   FILE *fp;
   CHAR Temp[256], *p, *arg, PendingWrite = FALSE;
   class TMsgData *Msg;

   if (pszText == NULL)
      pszText = "Msg.Cfg";

   if ((Msg = new TMsgData (pszPath)) != NULL) {
      if ((fp = fopen (pszText, "rt")) != NULL) {
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            if (Temp[0] != '%' && Temp[0] != ';') {
               if (Temp[strlen (Temp) - 1] == '\n')
                  Temp[strlen (Temp) - 1] = '\0';

               if ((p = strtok (Temp, " ")) != NULL) {
                  if (!stricmp (p, "Area")) {
                     if (PendingWrite == TRUE)
                        Msg->Add ();
                     Msg->New ();
                     if ((p = strtok (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (Msg->Key, p);
                        PendingWrite = TRUE;
                     }
                  }
                  else if (!stricmp (p, "Description")) {
                     if ((p = strtok (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (Msg->Display, p);
                     }
                  }
                  else if (!stricmp (p, "Moderator")) {
                     if ((p = strtok (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (Msg->Moderator, p);
                     }
                  }
                  else if ((arg = strtok (NULL, " ")) != NULL) {
                     if (!stricmp (p, "Level"))
                        Msg->Level = (USHORT)atoi (arg);
                     else if (!stricmp (p, "AccessFlags"))
                        Msg->AccessFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "DenyFlags"))
                        Msg->DenyFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "WriteLevel"))
                        Msg->WriteLevel = (USHORT)atoi (arg);
                     else if (!stricmp (p, "WriteFlags"))
                        Msg->WriteFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "DenyWriteFlags"))
                        Msg->DenyWriteFlags = (ULONG)atol (arg);
                     else if (!stricmp (p, "Age"))
                        Msg->Age = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "Storage"))
                        Msg->Storage = (USHORT)atoi (arg);
                     else if (!stricmp (p, "Path"))
                        strcpy (Msg->Path, arg);
                     else if (!stricmp (p, "Echomail"))
                        Msg->EchoMail = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "ShowGlobal"))
                        Msg->ShowGlobal = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "UpdateNews"))
                        Msg->UpdateNews = (UCHAR)atoi (arg);
                     else if (!stricmp (p, "MenuName"))
                        strcpy (Msg->MenuName, arg);
                     else if (!stricmp (p, "Cost"))
                        Msg->Cost = (USHORT)atoi (arg);
                     else if (!stricmp (p, "DaysOld"))
                        Msg->DaysOld = (USHORT)atoi (arg);
                     else if (!stricmp (p, "RecvDaysOld"))
                        Msg->RecvDaysOld = (USHORT)atoi (arg);
                     else if (!stricmp (p, "MaxMessages"))
                        Msg->MaxMessages = (USHORT)atoi (arg);
                     else if (!stricmp (p, "ActiveMsgs"))
                        Msg->ActiveMsgs = (ULONG)atoi (arg);
                     else if (!stricmp (p, "NewsGroup"))
                        strcpy (Msg->NewsGroup, arg);
                     else if (!stricmp (p, "Highest"))
                        Msg->Highest = (ULONG)atoi (arg);
                     else if (!stricmp (p, "EchoTag"))
                        strcpy (Msg->EchoTag, arg);
                     else if (!stricmp (p, "Feeder")) {
                        Msg->Feeder.Clear ();
                        Msg->Feeder.Add (arg);
                        Msg->Feeder.First ();
                     }
                     else if (!stricmp (p, "Forward"))
                        Msg->Forward.Add (arg);
                  }
               }
            }
         }
         fclose (fp);

         if (PendingWrite == TRUE)
            Msg->Add ();
      }
      delete Msg;
   }
}

VOID WriteMsgText (PSZ pszPath, PSZ pszText)
{
   FILE *fp;
   class TMsgData *Msg;

   if (pszText == NULL)
      pszText = "Msg.Cfg";

   if ((Msg = new TMsgData (pszPath)) != NULL) {
      if (Msg->First () == TRUE) {
         if ((fp = fopen (pszText, "wt")) != NULL) {
            fprintf (fp, ";\n; Area <Name>\n");
            fprintf (fp, ";     Description    \n");
            fprintf (fp, ";     Level          - 0-65535\n");
            fprintf (fp, ";     AccessFlags    \n");
            fprintf (fp, ";     DenyFlags      \n");
            fprintf (fp, ";     WriteLevel     - 0-65535\n");
            fprintf (fp, ";     WriteFlags     \n");
            fprintf (fp, ";     DenyWriteFlags \n");
            fprintf (fp, ";     Age            - 10-21\n");
            fprintf (fp, ";     Storage        - 0=Squish<tm>, 1=JAM<tm>, 2=Newsgroup\n");
            fprintf (fp, ";     Path           \n");
            fprintf (fp, ";     Group          - 0-65535\n");
            fprintf (fp, ";     Echomail       - 1=TRUE, 0=FALSE\n");
            fprintf (fp, ";     ShowGlobal     - 1=TRUE, 0=FALSE\n");
            fprintf (fp, ";     UpdateNews     - 1=TRUE, 0=FALSE\n");
            fprintf (fp, ";     MenuName       \n");
            fprintf (fp, ";     Moderator      <Name>\n");
            fprintf (fp, ";     Cost           <Num>\n");
            fprintf (fp, ";     DaysOld        <Num>\n");
            fprintf (fp, ";     RecvDaysOld    <Num>\n");
            fprintf (fp, ";     MaxMessages    <Num>\n");
            fprintf (fp, ";     ActiveMsgs     <Num>\n");
            fprintf (fp, ";     NewsGroup      <Name>\n");
            fprintf (fp, ";     Highest        <Num>\n");
            fprintf (fp, ";     Echotag        <Tag>\n");
            fprintf (fp, ";     Feeder         <Address>\n");
            fprintf (fp, ";     Forward        <Address>\n");
            fprintf (fp, ";\n");

            do {
               fprintf (fp, "Area %s\n", Msg->Key);
               fprintf (fp, "    Description    %s\n", Msg->Display);
               fprintf (fp, "    Level          %u\n", Msg->Level);
               if (Msg->AccessFlags)
                  fprintf (fp, "    AccessFlags    %lu\n", Msg->AccessFlags);
               if (Msg->DenyFlags)
                  fprintf (fp, "    DenyFlags      %lu\n", Msg->DenyFlags);
               if (Msg->WriteLevel)
                  fprintf (fp, "    WriteLevel     %u\n", Msg->WriteLevel);
               if (Msg->WriteFlags)
                  fprintf (fp, "    WriteFlags     %lu\n", Msg->WriteFlags);
               if (Msg->DenyWriteFlags)
                  fprintf (fp, "    DenyWriteFlags %lu\n", Msg->DenyWriteFlags);
               if (Msg->Age)
                  fprintf (fp, "    Age            %d\n", Msg->Age);
               fprintf (fp, "    Storage        %d\n", Msg->Storage);
               if (Msg->Path[0] != '\0')
                  fprintf (fp, "    Path           %s\n", Msg->Path);
               if (Msg->Group)
                  fprintf (fp, "    Group          %d\n", Msg->Group);
               if (Msg->EchoMail == TRUE)
                  fprintf (fp, "    Echomail       %d\n", Msg->EchoMail);
               if (Msg->ShowGlobal == TRUE)
                  fprintf (fp, "    ShowGlobal     %d\n", Msg->ShowGlobal);
               if (Msg->UpdateNews == TRUE)
                  fprintf (fp, "    UpdateNews     %d\n", Msg->UpdateNews);
               if (Msg->MenuName[0] != '\0')
                  fprintf (fp, "    MenuName       %s\n", Msg->MenuName);
               if (Msg->Moderator[0] != '\0')
                  fprintf (fp, "    Moderator      %s\n", Msg->Moderator);
               if (Msg->Cost)
                  fprintf (fp, "    Cost           %d\n", Msg->Cost);
               if (Msg->DaysOld != 0)
                  fprintf (fp, "    DaysOld        %d\n", Msg->DaysOld);
               if (Msg->RecvDaysOld != 0)
                  fprintf (fp, "    RecvDaysOld    %d\n", Msg->RecvDaysOld);
               if (Msg->MaxMessages != 0)
                  fprintf (fp, "    MaxMessages    %d\n", Msg->MaxMessages);
               if (Msg->ActiveMsgs != 0L)
                  fprintf (fp, "    ActiveMsgs     %d\n", Msg->ActiveMsgs);
               if (Msg->NewsGroup[0] != '\0')
                  fprintf (fp, "    NewsGroup      %s\n", Msg->NewsGroup);
               if (Msg->Highest != 0L)
                  fprintf (fp, "    Highest        %lu\n", Msg->Highest);
               if (Msg->EchoTag[0] != '\0')
                  fprintf (fp, "    Echotag        %s\n", Msg->EchoTag);
               if (Msg->Feeder.First () == TRUE)
                  fprintf (fp, "    Feeder         %s\n", Msg->Feeder.String);
               if (Msg->Forward.First () == TRUE)
                  do {
                     fprintf (fp, "    Forward        %s\n", Msg->Forward.String);
                  } while (Msg->Forward.Next () == TRUE);
               fprintf (fp, ";\n");
            } while (Msg->Next () == TRUE);
            fclose (fp);
         }
      }

      delete Msg;
   }
}

VOID ReadNewsText (PSZ pszPath, PSZ pszText)
{
   FILE *fp;
   CHAR Temp[256], *p, DupeKey[16], Last;
   class TMsgData *Msg;

   DupeKey[0] = '\0';
   Last = '0';
   if (pszText == NULL)
      pszText = "News.Cfg";

   if ((Msg = new TMsgData (pszPath)) != NULL) {
      if ((fp = fopen (pszText, "rt")) != NULL) {
         while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
            if (Temp[0] != '%' && Temp[0] != ';') {
               if (Temp[strlen (Temp) - 1] == '\n')
                  Temp[strlen (Temp) - 1] = '\0';

               if ((p = strtok (Temp, " ")) != NULL) {
                  Msg->New ();
                  sprintf (Msg->Key, "%.15s", p);
                  strcpy (Msg->NewsGroup, p);
                  if ((p = strtok (NULL, "")) != NULL) {
                     while (*p == ' ')
                        p++;
                     strcpy (Msg->Display, p);
                  }
                  if (Msg->Display[0] == '\0')
                     strcpy (Msg->Display, Msg->NewsGroup);
                  Msg->Storage = 2;
                  Msg->ShowGlobal = TRUE;
                  Msg->Age = 10;
                  if (!stricmp (DupeKey, Msg->Key)) {
                     p = strchr (Msg->Key, '\0') - 1;
                     *p = (CHAR)(++Last);
                  }
                  else {
                     strcpy (DupeKey, Msg->Key);
                     Last = '0';
                  }
                  Msg->Add ();
               }
            }
         }
         fclose (fp);
      }
      delete Msg;
   }
}

typedef unsigned char bit;
typedef unsigned short word;
typedef unsigned char byte;

struct _sysmsg {
   char  msg_name[70];
   short msg_num;
   char  msg_path[40];
   char  origin[56];
   bit   echomail  :1;
   bit   netmail   :1;
   bit   mpublic   :1;
   bit   mprivate  :1;
   bit   anon_ok   :1;
   bit   no_matrix :1;
   bit   squish    :1;
   bit   kill_unlisted :1;
   word  msg_sig;
   char  echotag[32];
   word  pip_board;
   byte  quick_board;
   byte  msg_priv;
	long  msg_flags;
	byte  write_priv;
	long  write_flags;
	byte  use_alias;
	short max_msgs;
	short max_age;
	short age_rcvd;
	char  forward1[80];
	char  forward2[80];
	char  forward3[80];
	bit   msg_restricted :1;
	bit   passthrough    :1;
	bit   internet_mail  :1;
	byte  areafix;
	char  qwk_name[14];
	long  afx_flags;
	word  gold_board;
	bit   sendonly 		:1;
	bit   receiveonly	   :1;
	char  filler1[26];
};

VOID ReadOldLoraMsg (PSZ pszPath, PSZ pszText)
{
   int fd;
   CHAR *p, DupeKey[16], Last;
   class TMsgData *Msg;
   struct _sysmsg sysmsg;

   DupeKey[0] = '\0';
   Last = '0';
   if (pszText == NULL)
      pszText = "SysMsg.Dat";

   if ((Msg = new TMsgData (pszPath)) != NULL) {
      if ((fd = open (pszText, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &sysmsg, sizeof (sysmsg)) == sizeof (sysmsg)) {
            if (sysmsg.squish && sysmsg.echomail) {
               Msg->New ();
               if (sysmsg.qwk_name[0])
                  strcpy (Msg->Key, sysmsg.qwk_name);
               else
                  sprintf (Msg->Key, "%.15s", sysmsg.echotag);
               strcpy (Msg->Display, sysmsg.msg_name);
               Msg->Storage = 1;
               strcpy (Msg->Path, sysmsg.msg_path);
               Msg->ShowGlobal = TRUE;
               Msg->EchoMail = TRUE;
               Msg->Age = 10;
               strcpy (Msg->EchoTag, sysmsg.echotag);
               Msg->Level = 10;
               Msg->WriteLevel = 20;
//               Msg->Level = sysmsg.msg_priv;
//               Msg->AccessFlags = sysmsg.msg_flags;
//               Msg->WriteLevel = sysmsg.write_priv;
//               Msg->WriteFlags = sysmsg.write_flags;
               if (!stricmp (DupeKey, Msg->Key)) {
                  p = strchr (Msg->Key, '\0') - 1;
                  *p = (CHAR)(++Last);
               }
               else {
                  strcpy (DupeKey, Msg->Key);
                  Last = '0';
               }
               Msg->Add ();
            }
         }
         close (fd);
      }
      delete Msg;
   }
}

struct _nodeinfo {
   short zone;
   short net;
   short node;
	short point;
   short afx_level;
   char  pw_session[20];
   char  pw_areafix[20];
   char  pw_tic[20];
   char  pw_packet[9];
   short modem_type;
   char  phone[30];
   short packer;
   char  sysop_name[36];
   char  system[36];
   bit   remap4d      :1;
   bit   can_do_22pkt :1;
   bit   wazoo        :1;
   bit   emsi         :1;
   bit   janus        :1;
   bit   pkt_create   :2;
   char  aka;
   short tic_level;
   long  afx_flags;
   long  tic_flags;
   char  tic_aka;
   long  baudrate;
   char  pw_inbound_packet[9];
	char  mailer_aka;
	long  min_baud_rate;
	char  filler[40];
};

VOID ReadOldLoraNodes (PSZ pszPath, PSZ pszText)
{
   int fd;
   class TNodes *Nodes;
   struct _nodeinfo ni;

   if (pszText == NULL)
      pszText = "Nodes.Dat";

   if ((Nodes = new TNodes (pszPath)) != NULL) {
      if ((fd = open (pszText, O_RDONLY|O_BINARY)) != -1) {
         while (read (fd, &ni, sizeof (ni)) == sizeof (ni)) {
            Nodes->New ();
            Nodes->Zone = ni.zone;
            Nodes->Net = ni.net;
            Nodes->Node = ni.node;
            Nodes->Point = ni.point;
            strcpy (Nodes->SystemName, ni.system);
            strcpy (Nodes->SysopName, ni.sysop_name);
            strcpy (Nodes->Phone, ni.phone);
            strcpy (Nodes->SessionPwd, ni.pw_session);
            strcpy (Nodes->AreaMgrPwd, ni.pw_areafix);
            strcpy (Nodes->OutPktPwd, ni.pw_packet);
            strcpy (Nodes->InPktPwd, ni.pw_inbound_packet);
            Nodes->Add ();
         }
         close (fd);
      }
      delete Nodes;
   }
}

void main (int argc, char *argv[])
{
   USHORT i, Import, Export, Delete;
   USHORT Msg, File, Sched, News, Lora, Nodes;
   CHAR Temp[128];
   PSZ Path, Text;

   Delete = Import = Export = FALSE;
   Msg = File = Sched = News = Lora = Nodes = FALSE;
   Path = Text = NULL;

   for (i = 1; i < argc; i++) {
      if (Export == FALSE && !stricmp (argv[i], "IMPORT"))
         Import = TRUE;
      else if (Import == FALSE && !stricmp (argv[i], "EXPORT"))
         Export = TRUE;
      else if (!stricmp (argv[i], "-M"))
         Msg = TRUE;
      else if (!stricmp (argv[i], "-F"))
         File = TRUE;
      else if (!stricmp (argv[i], "-S"))
         Sched = TRUE;
      else if (!stricmp (argv[i], "-NG"))
         News = TRUE;
      else if (!stricmp (argv[i], "-OL"))
         Lora = TRUE;
      else if (!stricmp (argv[i], "-ON"))
         Nodes = TRUE;
      else if (!stricmp (argv[i], "-D"))
         Delete = TRUE;
      else {
         if (Path == NULL)
            Path = argv[i];
         else
            Text = argv[i];
      }
   }

   if (Path == NULL)
      Path = ".\\";

   if (argc < 2) {
      printf ("\nTEXT; A Text Configuration Files Utility for LoraBBS\n");
      printf ("      Copyright (c) 1995 by Marco Maccaferri. All rigths reserved.\n\n");

      printf ("Usage: TEXT [IMPORT|EXPORT] [-M] [-F] [-S] [-NG] <Path> <Text>\n\n");
      printf ("       IMPORT - Read from a text file into Lora's binary files\n");
      printf ("       EXPORT - Read from Lora's binary files to text files\n");
      printf ("       -M     - Message areas configuration\n");
      printf ("       -F     - File areas configuration\n");
      printf ("       -S     - Event scheduler configuration\n");
      printf ("       -NG    - Usenet newsgroups listing\n");
      printf ("       -OL    - Old Lora 2.4x message configuration\n");
      printf ("       -ON    - Old Lora 2.4x nodes configuration\n");
      printf ("       <Path> - Binary files location\n");
      printf ("       <Text> - Text file to import\n");
   }
   else {
      if (Import == TRUE) {
         if (Delete == TRUE) {
            if (Msg == TRUE || News == TRUE || Lora == TRUE) {
               sprintf (Temp, "%sMsg.Dat", Path);
               unlink (Temp);
               sprintf (Temp, "%sMsg.Idx", Path);
               unlink (Temp);
            }
            if (Nodes == TRUE) {
               sprintf (Temp, "%sNodes.Dat", Path);
               unlink (Temp);
            }
         }
         if (Msg == TRUE)
            ReadMsgText (Path, Text);
         if (File == TRUE)
            ReadFileText (Path, Text);
         if (Sched == TRUE)
            ReadEventText (Path, Text);
         if (News == TRUE)
            ReadNewsText (Path, Text);
         if (Lora == TRUE)
            ReadOldLoraMsg (Path, Text);
         if (Nodes == TRUE)
            ReadOldLoraNodes (Path, Text);
      }

      if (Export == TRUE) {
         if (Msg == TRUE)
            WriteMsgText (Path, Text);
         if (File == TRUE)
            WriteFileText (Path, Text);
         if (Sched == TRUE)
            WriteEventText (Path, Text);
      }
   }
}

