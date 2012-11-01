
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_defs.h"
#include "config.h"
#include "files.h"
#include "forum.h"
#include "menu.h"
#include "mailer.h"
#include "msgdata.h"
#include "schedule.h"

void set_menu (void);
void set_menu_ita (void);

void set_config (void)
{
   class TConfig *Cfg;

   unlink ("config.dat");
   unlink ("channels.dat");

   if ((Cfg = new TConfig ("Config.Dat", "Channels.Dat")) != 0) {
      Cfg->Clear ();
      strcpy (Cfg->SystemName, "Arci Computer Club BBS");
      strcpy (Cfg->SysopName, "Marco Maccaferri");
      strcpy (Cfg->SystemPath, "C:\\Bbs\\");
      strcpy (Cfg->MiscPath, "C:\Bbs\\Misc\\");
      strcpy (Cfg->UserPath, "C:\Bbs\\");
      strcpy (Cfg->HomePath, "C:\Bbs\\Home\\");
      strcpy (Cfg->LogPath, "C:\Bbs\\Logs\\");
      Cfg->LogLevel = 0xFF;
      strcpy (Cfg->MailSpool, "C:\Bbs\\EMail\\");
      strcpy (Cfg->SentMail, "C:\Bbs\\EMail\\Sent\\");
      strcpy (Cfg->HostName, "arcibbs.bo.cnr.it");
      strcpy (Cfg->NewsServer, "news.uni-stuttgart.de");
      strcpy (Cfg->SMTPServer, "arci01.bo.cnr.it");
      strcpy (Cfg->Inbound, "C:\Bbs\\Inbound\\");
      strcpy (Cfg->InboundProt, "C:\Bbs\\Inbound\\");
      strcpy (Cfg->InboundKnown, "C:\Bbs\\Inbound\\");
      strcpy (Cfg->Outbound, "C:\\Bbs\\Outbound\\");

      Cfg->Address.Add (2, 332, 402, 1, "fidonet.org");

      strcpy (Cfg->Language.Key, "1");
      strcpy (Cfg->Language.Description, "English");
      strcpy (Cfg->Language.File, "English.Dat");
      strcpy (Cfg->Language.MenuPath, "C:\\Bbs\\English\\");
      strcpy (Cfg->Language.MiscPath, "C:\\Bbs\\English\\");
      Cfg->Language.Add ();

      strcpy (Cfg->Language.Key, "2");
      strcpy (Cfg->Language.Description, "Italiano");
      strcpy (Cfg->Language.File, "Italian.Dat");
      strcpy (Cfg->Language.MenuPath, "C:\\Bbs\\Italian\\");
      strcpy (Cfg->Language.MiscPath, "C:\\Bbs\\Italian\\");
      Cfg->Language.Add ();

      Cfg->ClearChannel ();
      Cfg->Type = CH_LOCAL;
      Cfg->Channel = 0;
      Cfg->MaxChannels = 1;
      strcpy (Cfg->PipeName, "\\PIPE\\LOCAL");
      Cfg->NewLevel = 10;
      strcpy (Cfg->NewLimitClass, "NORMAL");
      Cfg->RookieCalls = 7;
      Cfg->AskRealName = Q_REQUIRED;
      Cfg->AskCompany = Q_YES;
      Cfg->AskAddress = Q_YES;
      Cfg->AskZipCity = Q_REQUIRED;
      Cfg->AskState = Q_YES;
      Cfg->AskPhone = Q_YES;
      Cfg->AskSex = Q_REQUIRED;
      Cfg->InactivityTime = 5;
      Cfg->LoginTime = 15;
      Cfg->Write (Cfg->Channel);

      Cfg->ClearChannel ();
      Cfg->Type = CH_MODEM;
      Cfg->Channel = 1;
      Cfg->MaxChannels = 1;
      strcpy (Cfg->Device, "COM2");
      Cfg->Speed = 38400L;
      strcpy (Cfg->Initialize[0], "ATZ\r");
      strcpy (Cfg->Answer, "ATA\r");
      strcpy (Cfg->Dial, "ATDT");
      strcpy (Cfg->Hangup, "v~~^~+++~~ATH0\r");
      Cfg->NewLevel = 10;
      strcpy (Cfg->NewLimitClass, "NORMAL");
      Cfg->RookieCalls = 7;
      Cfg->AskRealName = Q_REQUIRED;
      Cfg->AskCompany = Q_YES;
      Cfg->AskAddress = Q_YES;
      Cfg->AskZipCity = Q_REQUIRED;
      Cfg->AskState = Q_YES;
      Cfg->AskPhone = Q_YES;
      Cfg->AskSex = Q_REQUIRED;
      Cfg->InactivityTime = 5;
      Cfg->LoginTime = 15;
      Cfg->Write (Cfg->Channel);

      Cfg->ClearChannel ();
      Cfg->Type = CH_MODEM;
      Cfg->Channel = 2;
      Cfg->MaxChannels = 1;
      strcpy (Cfg->Device, "COM3");
      Cfg->Speed = 19200L;
      strcpy (Cfg->Initialize[0], "ATZ\r");
      strcpy (Cfg->Answer, "ATA\r");
      strcpy (Cfg->Dial, "ATDT");
      strcpy (Cfg->Hangup, "v~~^~+++~~ATH0\r");
      Cfg->NewLevel = 10;
      strcpy (Cfg->NewLimitClass, "NORMAL");
      Cfg->RookieCalls = 7;
      Cfg->AskRealName = Q_REQUIRED;
      Cfg->AskCompany = Q_YES;
      Cfg->AskAddress = Q_YES;
      Cfg->AskZipCity = Q_REQUIRED;
      Cfg->AskState = Q_YES;
      Cfg->AskPhone = Q_YES;
      Cfg->AskSex = Q_REQUIRED;
      Cfg->InactivityTime = 5;
      Cfg->LoginTime = 15;
      Cfg->Write (Cfg->Channel);

      Cfg->ClearChannel ();
      Cfg->Type = CH_MODEM;
      Cfg->Channel = 3;
      Cfg->MaxChannels = 1;
      strcpy (Cfg->Device, "COM4");
      Cfg->Speed = 57600L;
      strcpy (Cfg->Initialize[0], "ATZ\r");
      strcpy (Cfg->Answer, "ATA\r");
      strcpy (Cfg->Dial, "ATDT");
      strcpy (Cfg->Hangup, "v~~^~+++~~ATH0\r");
      Cfg->NewLevel = 10;
      strcpy (Cfg->NewLimitClass, "NORMAL");
      Cfg->RookieCalls = 7;
      Cfg->AskRealName = Q_REQUIRED;
      Cfg->AskCompany = Q_YES;
      Cfg->AskAddress = Q_YES;
      Cfg->AskZipCity = Q_REQUIRED;
      Cfg->AskState = Q_YES;
      Cfg->AskPhone = Q_YES;
      Cfg->AskSex = Q_REQUIRED;
      Cfg->InactivityTime = 5;
      Cfg->LoginTime = 15;
      Cfg->Write (Cfg->Channel);

      Cfg->ClearChannel ();
      Cfg->Type = CH_TELNET;
      Cfg->Channel = 4;
      Cfg->MaxChannels = 16;
      Cfg->Port = 23;
      Cfg->NewLevel = 10;
      strcpy (Cfg->NewLimitClass, "NORMAL");
      Cfg->RookieCalls = 7;
      Cfg->AskRealName = Q_REQUIRED;
      Cfg->AskCompany = Q_YES;
      Cfg->AskAddress = Q_YES;
      Cfg->AskZipCity = Q_REQUIRED;
      Cfg->AskState = Q_YES;
      Cfg->AskPhone = Q_YES;
      Cfg->AskSex = Q_REQUIRED;
      Cfg->InactivityTime = 5;
      Cfg->LoginTime = 15;
      Cfg->Write (Cfg->Channel);

      delete Cfg;
   }
}

void set_limits (void)
{
   class TLimits *Limits;

   printf ("Limits\n");

   unlink ("Limits.Dat");

   if ((Limits = new TLimits) != 0) {
      Limits->Clear ();
      strcpy (Limits->Key, "NEWUSER");
      strcpy (Limits->Description, "New User");
      Limits->CallTimeLimit = 30;
      Limits->DayTimeLimit = 30;
      Limits->DayDownloadLimit = 1000;
      Limits->Add ();

      Limits->Clear ();
      strcpy (Limits->Key, "DEMO");
      strcpy (Limits->Description, "Demo User");
      Limits->CallTimeLimit = 30;
      Limits->DayDownloadLimit = 1000;
      Limits->Add ();

      Limits->Clear ();
      strcpy (Limits->Key, "1MONTH-SUB");
      strcpy (Limits->Description, "1 Month Subscriber");
      Limits->ExpireDays = 30;
      strcpy (Limits->ExpireClass, "DEMO");
      Limits->WeekTimeLimit = 400;
      Limits->WeekTimeLimit = 400;
      Limits->WeekDownloadLimit = 5000;
      Limits->Add ();

      Limits->Clear ();
      strcpy (Limits->Key, "3MONTH-SUB");
      strcpy (Limits->Description, "3 Month Subscriber");
      Limits->ExpireDays = 90;
      strcpy (Limits->ExpireClass, "DEMO");
      Limits->WeekTimeLimit = 400;
      Limits->WeekTimeLimit = 400;
      Limits->WeekDownloadLimit = 5000;
      Limits->Add ();

      Limits->Clear ();
      strcpy (Limits->Key, "YEARLY-SUB");
      strcpy (Limits->Description, "Yearly Subscriber");
      Limits->ExpireDays = 365;
      strcpy (Limits->ExpireClass, "DEMO");
      Limits->WeekTimeLimit = 400;
      Limits->WeekTimeLimit = 400;
      Limits->WeekDownloadLimit = 5000;
      Limits->Add ();

      Limits->Clear ();
      strcpy (Limits->Key, "NORMAL");
      strcpy (Limits->Description, "Normal User");
      Limits->ExpireDays = 0;
      Limits->DayTimeLimit = 45;
      Limits->DayDownloadLimit = 5000;
      Limits->Add ();

      Limits->Clear ();
      strcpy (Limits->Key, "SYSOP");
      strcpy (Limits->Description, "System Operator");
      Limits->ShowLevel = 65000U;
      Limits->Add ();

      delete Limits;
   }
}

void set_messages (void)
{
   class TMsgData *Msg;

   printf ("Message Areas Definitions\n");

   unlink ("msg.dat");
   unlink ("msg.idx");

   if ((Msg = new TMsgData) != 0) {
      Msg->New ();
      strcpy (Msg->Key, "NEWS");
      strcpy (Msg->Display, "News and Announcements");
      Msg->Storage = ST_JAM;
      strcpy (Msg->Path, "MSG\\NEWS");
      strcpy (Msg->Moderator, "Marco Maccaferri");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "LORA");
      strcpy (Msg->Display, "Technical Support");
      Msg->Storage = ST_JAM;
      strcpy (Msg->Path, "MSG\\LORA");
      strcpy (Msg->Moderator, "Marco Maccaferri");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "ADDONS");
      strcpy (Msg->Display, "Third Party Software Addons");
      Msg->Storage = ST_JAM;
      strcpy (Msg->Path, "MSG\\ADDONS");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "HARDWARE");
      strcpy (Msg->Display, "Supported Hardware");
      Msg->Storage = ST_JAM;
      strcpy (Msg->Path, "MSG\\HARDWARE");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "ADVOCACY");
      strcpy (Msg->Display, "comp.os.os2.advocacy");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.advocacy");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "ANNOUNCE");
      strcpy (Msg->Display, "comp.os.os2.announce");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.announce");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "APPS");
      strcpy (Msg->Display, "comp.os.os2.apps");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.apps");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "BETA");
      strcpy (Msg->Display, "comp.os.os2.beta");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.beta");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "BUGS");
      strcpy (Msg->Display, "comp.os.os2.bugs");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.bugs");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "GAMES");
      strcpy (Msg->Display, "comp.os.os2.games");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.games");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "MISC");
      strcpy (Msg->Display, "comp.os.os2.misc");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.misc");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "MULTIMEDIA");
      strcpy (Msg->Display, "comp.os.os2.multimedia");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.multimedia");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "NETMISC");
      strcpy (Msg->Display, "comp.os.os2.networking.misc");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.networking.misc");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "NETTCPIP");
      strcpy (Msg->Display, "comp.os.os2.networking.tcp-ip");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.networking.tcp-ip");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "PROGMISC");
      strcpy (Msg->Display, "comp.os.os2.programmer.misc");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.programmer.misc");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "PROGOOP");
      strcpy (Msg->Display, "comp.os.os2.programmer.oop");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.programmer.oop");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "PROGPORT");
      strcpy (Msg->Display, "comp.os.os2.programmer.porting");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.programmer.porting");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "PROGTOOLS");
      strcpy (Msg->Display, "comp.os.os2.programmer.tools");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.programmer.tools");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      Msg->New ();
      strcpy (Msg->Key, "SETUP");
      strcpy (Msg->Display, "comp.os.os2.setup");
      Msg->Storage = ST_USENET;
      strcpy (Msg->NewsGroup, "comp.os.os2.setup");
      Msg->ShowGlobal = TRUE;
      Msg->Add ();

      delete Msg;
   }
}

void set_files (void)
{
   class FileData *File;

   printf ("File Areas Definitions\n");

   unlink ("file.dat");
   unlink ("file.idx");

   if ((File = new FileData) != 0) {
      File->Reset ();
      strcpy (File->Key, "LORA");
      strcpy (File->Display, "LoraBBS Shareware Distribution");
      strcpy (File->Download, "FILE\\DISTRIB\\");
      strcpy (File->Upload, "FILE\\DISTRIB\\");
      strcpy (File->Moderator, "Marco Maccaferri");
      File->ShowGlobal = TRUE;
      File->Add ();

      File->Reset ();
      strcpy (File->Key, "RFC");
      strcpy (File->Display, "Internet Request For Comments");
      strcpy (File->Download, "FILE\\RFC\\");
      strcpy (File->Upload, "FILE\\RFC\\");
      File->ShowGlobal = TRUE;
      File->Add ();

      File->Reset ();
      strcpy (File->Key, "MSD-1");
      strcpy (File->Display, "MS-DOS: Compression Programs");
      strcpy (File->Download, "FILE\\MS_COMP\\");
      strcpy (File->Upload, "FILE\\MS_COMP\\");
      File->ShowGlobal = TRUE;
      File->Add ();

      File->Reset ();
      strcpy (File->Key, "MSD-2");
      strcpy (File->Display, "MS-DOS: Communication Programs");
      strcpy (File->Download, "FILE\\MS_COM\\");
      strcpy (File->Upload, "FILE\\MS_COM\\");
      File->ShowGlobal = TRUE;
      File->Add ();

      File->Reset ();
      strcpy (File->Key, "MSD-3");
      strcpy (File->Display, "MS-DOS: Music Files");
      strcpy (File->Download, "FILE\\MS_MUSIC\\");
      strcpy (File->Upload, "FILE\\MS_MUSIC\\");
      File->ShowGlobal = TRUE;
      File->Add ();

      File->Reset ();
      strcpy (File->Key, "MSD-4");
      strcpy (File->Display, "MS-DOS: Miscellaneous");
      strcpy (File->Download, "FILE\\MS_MISC\\");
      strcpy (File->Upload, "FILE\\MS_MISC\\");
      File->ShowGlobal = TRUE;
      File->Add ();

      File->Reset ();
      strcpy (File->Key, "OS2-1");
      strcpy (File->Display, "OS/2: Editors");
      strcpy (File->Download, "FILE\\OS2_EDIT\\");
      strcpy (File->Upload, "FILE\\OS2_EDIT\\");
      File->ShowGlobal = TRUE;
      File->Add ();

      File->Reset ();
      strcpy (File->Key, "OS2-2");
      strcpy (File->Display, "OS/2: Networking");
      strcpy (File->Download, "FILE\\OS2_LAN\\");
      strcpy (File->Upload, "FILE\\OS2_LAN\\");
      File->ShowGlobal = TRUE;
      File->Add ();

      File->Reset ();
      strcpy (File->Key, "OS2-3");
      strcpy (File->Display, "OS/2: Miscellaneous");
      strcpy (File->Download, "FILE\\OS2_MISC\\");
      strcpy (File->Upload, "FILE\\OS2_MISC\\");
      File->ShowGlobal = TRUE;
      File->Add ();

      delete File;
   }
}

void set_forums (void)
{
   class ForumData *Forum;

   printf ("Forums Definitions\n");

   unlink ("forum.dat");
   unlink ("forum.idx");

   if ((Forum = new ForumData) != 0) {
      Forum->Reset ();
      strcpy (Forum->Key, "EIC");
      strcpy (Forum->Display, "Erotic Image Center CD-ROM");
      Forum->File.Add ("CATALOG");
      Forum->File.Add ("IMG1");
      Forum->File.Add ("IMG2");
      Forum->File.Add ("IMG3");
      Forum->File.Add ("JAV");
      Forum->File.Add ("JFTYEM");
      Forum->Add ();

      delete Forum;
   }
}

void set_packers (void)
{
   class TPacker *Pack;

   printf ("Compressor Definitions\n");

   unlink ("packer.dat");

   if ((Pack = new TPacker) != 0) {
      Pack->Clear ();
      strcpy (Pack->Key, "Z");
      strcpy (Pack->Display, "PK-Ware's PKZIP/PKUNZIP");
      strcpy (Pack->PackCmd, "zip -mj9 %1 %2");
      strcpy (Pack->UnpackCmd, "unzip -oj %1 %2");
      strcpy (Pack->Id, "504B0304");
      Pack->Position = 0L;
      Pack->Add ();

      delete Pack;
   }
}

void set_mail (void)
{
   class TScheduler *Sched;
//   class TNodes *Nodes;

   printf ("Nodes and scheduler.\n");

   unlink ("..\\data\\nodes.dat");
//   unlink ("..\\data\\events.dat");

/*
   if ((Nodes = new TNodes ("..\\data\\")) != 0) {
      Nodes->New ();
      Nodes->Zone = 2;
      Nodes->Net = 332;
      Nodes->Node = 402;
      Nodes->Point = 0;
      strcpy (Nodes->Phone, "6331746");
      strcpy (Nodes->SessionPwd, "MARVEL");
      Nodes->Add ();

      delete Nodes;
   }
*/

   if ((Sched = new TScheduler (".\\")) != 0) {
      Sched->New ();
      Sched->Hour = 0;
      Sched->Minute = 30;
      Sched->CallNode = TRUE;
      Sched->ForceCall = TRUE;
      Sched->Channel = 1;
      strcpy (Sched->Address, "2:332/402.0");
      Sched->Delay = 10;
      Sched->Length = 25;
      Sched->Add ();

      delete Sched;
   }
}

void main (void)
{
   printf ("\nDemo System Files Generator - Version 0.01\n");
   printf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\n\n");

   set_config ();
   set_limits ();
//   set_forums ();
//   set_messages ();
//   set_files ();
//   set_menu ();
//   set_packers ();
//   set_mail ();

   printf ("Done.\n\n");
}



