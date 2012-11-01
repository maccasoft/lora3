
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.5
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/21/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

TConfig::TConfig (void)
{
   strcpy (ConfigFile, "config.dat");
   strcpy (ChannelFile, "channel.dat");
   TaskNumber = 1;
}

TConfig::~TConfig (void)
{
}

VOID TConfig::Default (VOID)
{
   strcpy (Device, "COM2");
   Speed = 19200L;
   strcpy (Initialize[0], "ATZ");
   Initialize[1][0] = Initialize[2][0] = '\0';
   strcpy (Answer, "ATA");
   strcpy (Dial, "ATDT%s");
   strcpy (Hangup, "v~^~+++~~ATH");
   strcpy (OffHook, "ATM0H1");
   LockSpeed = TRUE;
   StripDashes = FALSE;
   ManualAnswer = TRUE;
   strcpy (FaxMessage, "FAX");
   strcpy (FaxCommand, "");
   strcpy (SystemName, "LoraBBS Test System");
   strcpy (SysopName, "LoraBBS Tester");
   strcpy (Location, "Nowhere");
   strcpy (Phone, "-Unpublished-");
   strcpy (NodelistFlags, "CM,XA");
   DialTimeout = 45;
   CarrierDropTimeout = 1;
   LoginType = 0;
   NewUserLevel = 10;
   NewUserFlags = 0L;
   NewUserDenyFlags = 0L;
   NewUserLimits[0] = '\0';
   CheckAnsi = YES;
   RealName = REQUIRED;
   CompanyName = YES;
   Address = YES;
   City = REQUIRED;
   PhoneNumber = YES;
   Gender = REQUIRED;
#if defined(__LINUX__)
   strcpy (SystemPath, "./");
   strcpy (UserFile, "users");
   strcpy (NormalInbound, "inbound/");
   strcpy (KnownInbound, "inbound/");
   strcpy (ProtectedInbound, "inbound/");
   strcpy (Outbound, "./outbound/");
   strcpy (SchedulerFile, "events");
   strcpy (NodelistPath, "nodes/");
   strcpy (UsersHomePath, "home/");
   strcpy (MenuPath, "menu/");
   strcpy (LanguageFile, "language");
   strcpy (TextFiles, "misc/");
   strcpy (NewAreasPath, "./");
   strcpy (TempPath, "temp/");
#else
   strcpy (SystemPath, ".\\");
   strcpy (UserFile, "users");
   strcpy (NormalInbound, "inbound\\");
   strcpy (KnownInbound, "inbound\\");
   strcpy (ProtectedInbound, "inbound\\");
   strcpy (Outbound, ".\\outbound\\");
   strcpy (SchedulerFile, "events");
   strcpy (NodelistPath, "nodes\\");
   strcpy (UsersHomePath, "home\\");
   strcpy (MenuPath, "menu\\");
   strcpy (LanguageFile, "language");
   strcpy (TextFiles, "misc\\");
   strcpy (NewAreasPath, ".\\");
   strcpy (TempPath, "temp\\");
#endif
   strcpy (MainMenu, "MAIN");
   strcpy (HostName, "unknown.host");
   strcpy (NewsServer, "news");
   strcpy (MailServer, "mail");
   strcpy (PopServer, "mail");
   strcpy (LogFile, "lora%u.log");
   FakeNet = 0;
   MailStorage = ST_SQUISH;
   NetMailStorage = BadStorage = DupeStorage = ST_SQUISH;
   strcpy (MailPath, "email");
   strcpy (NetMailPath, "netmail");
   strcpy (BadPath, "bad_msgs");
   strcpy (DupePath, "dupes");
   TelnetPort = 23;
   FtpPort = 21;
   WebPort = 80;
   SMTPPort = 25;
   POP3Port = 109;
   NNTPPort = 119;
   WaZoo = EMSI = TRUE;
   Janus = FALSE;
   NewAreasStorage = ST_SQUISH;
   Ansi = IEMSI = TRUE;
   UseSinglePass = FALSE;
   SeparateNetMail = TRUE;
   ZModemTelnet = FALSE;
   strcpy (OLRPacketName, "offline");

   MailAddress.Load (SystemPath);
}

/*
USHORT TConfig::Compile (PSZ pszConfig, PSZ pszChannel, CONFIG *Cfg)
{
   FILE *fp;
   CHAR Temp[128], *p;

   strcpy (Temp, strlwr (pszConfig));
   if ((p = strstr (Temp, ".dat")) != NULL)
      strcpy (p, ".cfg");

   if ((fp = fopen (Temp, "rt")) != NULL) {
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Temp[strlen (Temp) - 1] = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (!stricmp (p, "System")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Name")) {
                     p = strtok (NULL, "");
                     while (*p == ' ')
                        *p++;
                     strcpy (Cfg->SystemName, p);
                  }
               }
            }
            else if (!stricmp (p, "Sysop")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Name")) {
                     p = strtok (NULL, "");
                     while (*p == ' ')
                        *p++;
                     strcpy (Cfg->SysopName, p);
                  }
               }
            }
            else if (!stricmp (p, "Janus"))
               Cfg->Janus = TRUE;
            else if (!stricmp (p, "NoJanus"))
               Cfg->Janus = FALSE;
         }
      }
      fclose (fp);
   }
}
*/

USHORT TConfig::Load (PSZ pszConfig, PSZ pszChannel)
{
   if (pszConfig != NULL)
      strcpy (ConfigFile, pszConfig);
   if (pszChannel != NULL)
      strcpy (ChannelFile, pszChannel);

   return (Reload ());
}

VOID TConfig::NewChannel (VOID)
{
   Speed = 19200L;
//   strcpy (Initialize[0], "ATZ");
   Initialize[1][0] = Initialize[1][0] = Initialize[2][0] = '\0';
   strcpy (Answer, "ATA");
   strcpy (Dial, "ATDT%s");
   strcpy (Hangup, "v~^~+++~~ATH");
   strcpy (OffHook, "ATM0H1");
   LockSpeed = FALSE;
   StripDashes = FALSE;
   FaxMessage[0] = FaxCommand[0] = '\0';
   strcpy (Ring, "RING");
   ManualAnswer = TRUE;
   LimitedHours = FALSE;
   StartTime = EndTime = 0;
}

VOID TConfig::New (VOID)
{
   CONFIG *Cfg;

   if ((Cfg = (CONFIG *)malloc (sizeof (CONFIG))) != NULL) {
      memset (Cfg, 0, sizeof (CONFIG));
      Struct2Class (Cfg);
      free (Cfg);
   }
}

USHORT TConfig::Reload (VOID)
{
   int fd;
   USHORT RetVal = FALSE;
   CONFIG *Cfg;
   CHANNEL *Ch;

   if ((Cfg = (CONFIG *)malloc (sizeof (CONFIG))) != NULL)
      memset (Cfg, 0, sizeof (CONFIG));
   fd = sopen (ConfigFile, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fd != -1 && Cfg != NULL) {
      RetVal = TRUE;
      read (fd, Cfg, sizeof (CONFIG));
      Struct2Class (Cfg);
   }

   if (fd != -1)
      close (fd);
   if (Cfg != NULL)
      free (Cfg);

   if ((Ch = (CHANNEL *)malloc (sizeof (CHANNEL))) != NULL) {
      if ((fd = sopen (ChannelFile, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         while (read (fd, Ch, sizeof (CHANNEL)) == sizeof (CHANNEL)) {
            if (Ch->TaskNumber == TaskNumber) {
               strcpy (Device, Ch->Device);
               Speed = Ch->Speed;
               strcpy (Initialize[0], Ch->Initialize[0]);
               strcpy (Initialize[1], Ch->Initialize[1]);
               strcpy (Initialize[2], Ch->Initialize[2]);
               strcpy (Answer, Ch->Answer);
               strcpy (Dial, Ch->Dial);
               strcpy (Hangup, Ch->Hangup);
               strcpy (OffHook, Ch->OffHook);
               LockSpeed = Ch->LockSpeed;
               StripDashes = Ch->StripDashes;
               strcpy (FaxMessage, Ch->FaxMessage);
               strcpy (FaxCommand, Ch->FaxCommand);
               DialTimeout = Ch->DialTimeout;
               CarrierDropTimeout = Ch->CarrierDropTimeout;
               strcpy (SchedulerFile, Ch->SchedulerFile);
               strcpy (MainMenu, Ch->MainMenu);
               strcpy (Ring, Ch->Ring);
               ManualAnswer = Ch->ManualAnswer;
               LimitedHours = Ch->LimitedHours;
               StartTime = Ch->StartTime;
               EndTime = Ch->EndTime;
               break;
            }
         }
         close (fd);
      }
      free (Ch);
   }

   MailAddress.Load (SystemPath);

   return (RetVal);
}

VOID TConfig::Struct2Class (CONFIG *Cfg)
{
   strcpy (Device, Cfg->Device);
   Speed = Cfg->Speed;
   strcpy (Initialize[0], Cfg->Initialize[0]);
   strcpy (Initialize[1], Cfg->Initialize[1]);
   strcpy (Initialize[2], Cfg->Initialize[2]);
   strcpy (Answer, Cfg->Answer);
   strcpy (Dial, Cfg->Dial);
   strcpy (Hangup, Cfg->Hangup);
   strcpy (OffHook, Cfg->OffHook);
   LockSpeed = Cfg->LockSpeed;
   StripDashes = Cfg->StripDashes;
   strcpy (FaxMessage, Cfg->FaxMessage);
   strcpy (FaxCommand, Cfg->FaxCommand);
   DialTimeout = Cfg->DialTimeout;
   CarrierDropTimeout = Cfg->CarrierDropTimeout;
   strcpy (SystemName, Cfg->SystemName);
   strcpy (SysopName, Cfg->SysopName);
   strcpy (Location, Cfg->Location);
   strcpy (Phone, Cfg->Phone);
   strcpy (NodelistFlags, Cfg->NodelistFlags);
   LoginType = Cfg->LoginType;
   NewUserLevel = Cfg->NewUserLevel;
   NewUserFlags = Cfg->NewUserFlags;
   NewUserDenyFlags = Cfg->NewUserDenyFlags;
   strcpy (NewUserLimits, Cfg->NewUserLimits);
   CheckAnsi = Cfg->CheckAnsi;
   RealName = Cfg->RealName;
   CompanyName = Cfg->CompanyName;
   Address = Cfg->Address;
   City = Cfg->City;
   PhoneNumber = Cfg->PhoneNumber;
   Gender = Cfg->Gender;
   strcpy (SystemPath, FixPath (Cfg->SystemPath));
   strcpy (UserFile, AdjustPath (Cfg->UserFile));
   strcpy (NormalInbound, FixPath (Cfg->NormalInbound));
   strcpy (KnownInbound, FixPath (Cfg->KnownInbound));
   strcpy (ProtectedInbound, FixPath (Cfg->ProtectedInbound));
   strcpy (Outbound, FixPath (Cfg->Outbound));
   strcpy (SchedulerFile, AdjustPath (Cfg->SchedulerFile));
   strcpy (NodelistPath, FixPath (Cfg->NodelistPath));
   strcpy (UsersHomePath, FixPath (Cfg->UsersHomePath));
   strcpy (MenuPath, FixPath (Cfg->MenuPath));
   strcpy (LanguageFile, AdjustPath (Cfg->LanguageFile));
   strcpy (TextFiles, FixPath (Cfg->TextFiles));
   strcpy (MainMenu, Cfg->MainMenu);
   strcpy (HostName, Cfg->HostName);
   strcpy (NewsServer, Cfg->NewsServer);
   strcpy (MailServer, Cfg->MailServer);
   strcpy (PopServer, Cfg->PopServer);
   strcpy (LogFile, AdjustPath (Cfg->LogFile));
   FakeNet = Cfg->FakeNet;
   MailStorage = Cfg->MailStorage;
   NetMailStorage = Cfg->NetMailStorage;
   BadStorage = Cfg->BadStorage;
   DupeStorage = Cfg->DupeStorage;
   if (MailStorage == ST_FIDO)
      strcpy (MailPath, FixPath (Cfg->MailPath));
   else
      strcpy (MailPath, AdjustPath (Cfg->MailPath));
   if (NetMailStorage == ST_FIDO)
      strcpy (NetMailPath, FixPath (Cfg->NetMailPath));
   else
      strcpy (NetMailPath, AdjustPath (Cfg->NetMailPath));
   if (BadStorage == ST_FIDO)
      strcpy (BadPath, FixPath (Cfg->BadPath));
   else
      strcpy (BadPath, AdjustPath (Cfg->BadPath));
   if (DupeStorage == ST_FIDO)
      strcpy (DupePath, FixPath (Cfg->DupePath));
   else
      strcpy (DupePath, AdjustPath (Cfg->DupePath));
   TelnetServer = Cfg->TelnetServer;
   TelnetPort = Cfg->TelnetPort;
   FtpServer = Cfg->FtpServer;
   FtpPort = Cfg->FtpPort;
   WebServer = Cfg->WebServer;
   WebPort = Cfg->WebPort;
   SMTPServer = Cfg->SMTPServer;
   SMTPPort = Cfg->SMTPPort;
   POP3Server = Cfg->POP3Server;
   POP3Port = Cfg->POP3Port;
   NNTPServer = Cfg->NNTPServer;
   NNTPPort = Cfg->NNTPPort;
   WaZoo = Cfg->WaZoo;
   EMSI = Cfg->EMSI;
   Janus = Cfg->Janus;
   NewAreasStorage = Cfg->NewAreasStorage;
   strcpy (NewAreasPath, FixPath (Cfg->NewAreasPath));
   NewAreasLevel = Cfg->NewAreasLevel;
   NewAreasFlags = Cfg->NewAreasFlags;
   NewAreasDenyFlags = Cfg->NewAreasDenyFlags;
   NewAreasWriteLevel = Cfg->NewAreasWriteLevel;
   NewAreasWriteFlags = Cfg->NewAreasWriteFlags;
   NewAreasDenyWriteFlags = Cfg->NewAreasDenyWriteFlags;
   Ansi = Cfg->Ansi;
   IEMSI = Cfg->IEMSI;
   ImportEmpty = Cfg->ImportEmpty;
   ReplaceTear = Cfg->ReplaceTear;
   strcpy (TearLine, Cfg->TearLine);
   ForceIntl = Cfg->ForceIntl;
   Secure = Cfg->Secure;
   KeepNetMail = Cfg->KeepNetMail;
   TrackNetMail = Cfg->TrackNetMail;
   strcpy (MailOnly, Cfg->MailOnly);
   strcpy (EnterBBS, Cfg->EnterBBS);
   strcpy (ImportCmd, Cfg->ImportCmd);
   strcpy (ExportCmd, Cfg->ExportCmd);
   strcpy (PackCmd, Cfg->PackCmd);
   strcpy (SinglePassCmd, Cfg->SinglePassCmd);
   UseSinglePass = Cfg->UseSinglePass;
   SeparateNetMail = Cfg->SeparateNetMail;
   strcpy (AreasBBS, AdjustPath (Cfg->AreasBBS));
   UseAreasBBS = Cfg->UseAreasBBS;
   UpdateAreasBBS = Cfg->UpdateAreasBBS;
   strcpy (AfterCallerCmd, Cfg->AfterCallerCmd);
   strcpy (AfterMailCmd, Cfg->AfterMailCmd);
   ZModemTelnet = Cfg->ZModemTelnet;
   EnablePPP = Cfg->EnablePPP;
   PPPTimeLimit = Cfg->PPPTimeLimit;
   strcpy (PPPCmd, Cfg->PPPCmd);
   strcpy (TempPath, FixPath (Cfg->TempPath));
   strcpy (OLRPacketName, Cfg->OLRPacketName);
   OLRMaxMessages = Cfg->OLRMaxMessages;

}

PSZ TConfig::FixPath (PSZ path)
{
   PSZ p;

   if (path[strlen (path) - 1] != '\\' && path[strlen (path) - 1] != '/')
      strcat (path, "\\");

#if defined(__LINUX__)
   while ((p = strchr (path, '\\')) != NULL)
      *p = '/';
#else
   while ((p = strchr (path, '/')) != NULL)
      *p = '\\';
#endif

   return (path);
}

PSZ TConfig::AdjustPath (PSZ path)
{
   PSZ p;

   if (path[strlen (path) - 1] == '\\' || path[strlen (path) - 1] == '/')
      path[strlen (path) - 1] = '\0';

#if defined(__LINUX__)
   while ((p = strchr (path, '\\')) != NULL)
      *p = '/';
#else
   while ((p = strchr (path, '/')) != NULL)
      *p = '\\';
#endif

   return (path);
}

USHORT TConfig::Save (PSZ pszConfig, PSZ pszChannel)
{
   int fd;
   USHORT RetVal = FALSE, Found;
   CONFIG *Cfg;
   CHANNEL *Ch;

   if (pszConfig == NULL)
      pszConfig = ConfigFile;
   if (pszChannel == NULL)
      pszChannel = ChannelFile;

   Cfg = (CONFIG *)malloc (sizeof (CONFIG));
   fd = sopen (pszConfig, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fd != -1 && Cfg != NULL) {
      memset (Cfg, 0, sizeof (CONFIG));
      RetVal = TRUE;

      Cfg->Speed = Speed;
      sprintf (Cfg->Device, Device);
      strcpy (Cfg->Initialize[0], Initialize[0]);
      strcpy (Cfg->Initialize[1], Initialize[1]);
      strcpy (Cfg->Initialize[2], Initialize[2]);
      strcpy (Cfg->Answer, Answer);
      strcpy (Cfg->Dial, Dial);
      strcpy (Cfg->Hangup, Hangup);
      strcpy (Cfg->OffHook, OffHook);
      Cfg->LockSpeed = LockSpeed;
      Cfg->StripDashes = StripDashes;
      strcpy (Cfg->FaxMessage, FaxMessage);
      strcpy (Cfg->FaxCommand, FaxCommand);
      Cfg->DialTimeout = DialTimeout;
      Cfg->CarrierDropTimeout = CarrierDropTimeout;
      strcpy (Cfg->SystemName, SystemName);
      strcpy (Cfg->SysopName, SysopName);
      strcpy (Cfg->Location, Location);
      strcpy (Cfg->Phone, Phone);
      strcpy (Cfg->NodelistFlags, NodelistFlags);
      Cfg->LoginType = LoginType;
      Cfg->NewUserLevel = NewUserLevel;
      Cfg->NewUserFlags = NewUserFlags;
      Cfg->NewUserDenyFlags = NewUserDenyFlags;
      strcpy (Cfg->NewUserLimits, NewUserLimits);
      Cfg->CheckAnsi = CheckAnsi;
      Cfg->RealName = RealName;
      Cfg->CompanyName = CompanyName;
      Cfg->Address = Address;
      Cfg->City = City;
      Cfg->PhoneNumber = PhoneNumber;
      Cfg->Gender = Gender;
      strcpy (Cfg->SystemPath, SystemPath);
      strcpy (Cfg->UserFile, UserFile);
      strcpy (Cfg->NormalInbound, NormalInbound);
      strcpy (Cfg->KnownInbound, KnownInbound);
      strcpy (Cfg->ProtectedInbound, ProtectedInbound);
      strcpy (Cfg->Outbound, Outbound);
      strcpy (Cfg->SchedulerFile, SchedulerFile);
      strcpy (Cfg->NodelistPath, NodelistPath);
      strcpy (Cfg->UsersHomePath, UsersHomePath);
      strcpy (Cfg->MenuPath, MenuPath);
      strcpy (Cfg->LanguageFile, LanguageFile);
      strcpy (Cfg->TextFiles, TextFiles);
      strcpy (Cfg->MainMenu, MainMenu);
      strcpy (Cfg->HostName, HostName);
      strcpy (Cfg->NewsServer, NewsServer);
      strcpy (Cfg->MailServer, MailServer);
      strcpy (Cfg->PopServer, PopServer);
      strcpy (Cfg->LogFile, LogFile);
      Cfg->FakeNet = FakeNet;
      Cfg->MailStorage = MailStorage;
      Cfg->NetMailStorage = NetMailStorage;
      Cfg->BadStorage = BadStorage;
      Cfg->DupeStorage = DupeStorage;
      strcpy (Cfg->MailPath, MailPath);
      strcpy (Cfg->NetMailPath, NetMailPath);
      strcpy (Cfg->BadPath, BadPath);
      strcpy (Cfg->DupePath, DupePath);
      Cfg->TelnetServer = TelnetServer;
      Cfg->TelnetPort = TelnetPort;
      Cfg->FtpServer = FtpServer;
      Cfg->FtpPort = FtpPort;
      Cfg->WebServer = WebServer;
      Cfg->WebPort = WebPort;
      Cfg->SMTPServer = SMTPServer;
      Cfg->SMTPPort = SMTPPort;
      Cfg->POP3Server = POP3Server;
      Cfg->POP3Port = POP3Port;
      Cfg->NNTPServer = NNTPServer;
      Cfg->NNTPPort = NNTPPort;
      Cfg->WaZoo = WaZoo;
      Cfg->EMSI = EMSI;
      Cfg->Janus = Janus;
      Cfg->NewAreasStorage = NewAreasStorage;
      strcpy (Cfg->NewAreasPath, NewAreasPath);
      Cfg->NewAreasLevel = NewAreasLevel;
      Cfg->NewAreasFlags = NewAreasFlags;
      Cfg->NewAreasDenyFlags = NewAreasDenyFlags;
      Cfg->NewAreasWriteLevel = NewAreasWriteLevel;
      Cfg->NewAreasWriteFlags = NewAreasWriteFlags;
      Cfg->NewAreasDenyWriteFlags = NewAreasDenyWriteFlags;
      Cfg->Ansi = Ansi;
      Cfg->IEMSI = IEMSI;
      Cfg->ImportEmpty = ImportEmpty;
      Cfg->ReplaceTear = ReplaceTear;
      strcpy (Cfg->TearLine, TearLine);
      Cfg->ForceIntl = ForceIntl;
      Cfg->Secure = Secure;
      Cfg->KeepNetMail = KeepNetMail;
      Cfg->TrackNetMail = TrackNetMail;
      strcpy (Cfg->MailOnly, MailOnly);
      strcpy (Cfg->EnterBBS, EnterBBS);
      strcpy (Cfg->ImportCmd, ImportCmd);
      strcpy (Cfg->ExportCmd, ExportCmd);
      strcpy (Cfg->PackCmd, PackCmd);
      strcpy (Cfg->SinglePassCmd, SinglePassCmd);
      Cfg->UseSinglePass = UseSinglePass;
      Cfg->SeparateNetMail = SeparateNetMail;
      strcpy (Cfg->AreasBBS, AreasBBS);
      Cfg->UseAreasBBS = UseAreasBBS;
      Cfg->UpdateAreasBBS = UpdateAreasBBS;
      strcpy (Cfg->AfterCallerCmd, AfterCallerCmd);
      strcpy (Cfg->AfterMailCmd, AfterMailCmd);
      Cfg->ZModemTelnet = ZModemTelnet;
      Cfg->EnablePPP = EnablePPP;
      Cfg->PPPTimeLimit = PPPTimeLimit;
      strcpy (Cfg->PPPCmd, PPPCmd);
      strcpy (Cfg->TempPath, TempPath);
      strcpy (Cfg->OLRPacketName, OLRPacketName);
      Cfg->OLRMaxMessages = OLRMaxMessages;

      write (fd, Cfg, sizeof (CONFIG));
      close (fd);
   }

   if (fd != -1)
      close (fd);
   if (Cfg != NULL)
      free (Cfg);

   if ((Ch = (CHANNEL *)malloc (sizeof (CHANNEL))) != NULL) {
      if ((fd = sopen (pszChannel, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         Found = FALSE;
         while (read (fd, Ch, sizeof (CHANNEL)) == sizeof (CHANNEL)) {
            if (Ch->TaskNumber == TaskNumber) {
               memset (Ch, 0, sizeof (CHANNEL));
               Found = TRUE;

               Ch->TaskNumber = TaskNumber;
               strcpy (Ch->Device, Device);
               Ch->Speed = Speed;
               strcpy (Ch->Initialize[0], Initialize[0]);
               strcpy (Ch->Initialize[1], Initialize[1]);
               strcpy (Ch->Initialize[2], Initialize[2]);
               strcpy (Ch->Answer, Answer);
               strcpy (Ch->Dial, Dial);
               strcpy (Ch->Hangup, Hangup);
               strcpy (Ch->OffHook, OffHook);
               Ch->LockSpeed = LockSpeed;
               Ch->StripDashes = StripDashes;
               strcpy (Ch->FaxMessage, FaxMessage);
               strcpy (Ch->FaxCommand, FaxCommand);
               Ch->DialTimeout = DialTimeout;
               Ch->CarrierDropTimeout = CarrierDropTimeout;
               strcpy (Ch->SchedulerFile, SchedulerFile);
               strcpy (Ch->MainMenu, MainMenu);
               strcpy (Ch->Ring, Ring);
               Ch->ManualAnswer = ManualAnswer;
               Ch->LimitedHours = LimitedHours;
               Ch->StartTime = StartTime;
               Ch->EndTime = EndTime;

               lseek (fd, tell (fd) - sizeof (CHANNEL), SEEK_SET);
               write (fd, Ch, sizeof (CHANNEL));
               break;
            }
         }
         if (Found == FALSE) {
            memset (Ch, 0, sizeof (CHANNEL));

            Ch->TaskNumber = TaskNumber;
            strcpy (Ch->Device, Device);
            Ch->Speed = Speed;
            strcpy (Ch->Initialize[0], Initialize[0]);
            strcpy (Ch->Initialize[1], Initialize[1]);
            strcpy (Ch->Initialize[2], Initialize[2]);
            strcpy (Ch->Answer, Answer);
            strcpy (Ch->Dial, Dial);
            strcpy (Ch->Hangup, Hangup);
            strcpy (Ch->OffHook, OffHook);
            Ch->LockSpeed = LockSpeed;
            Ch->StripDashes = StripDashes;
            strcpy (Ch->FaxMessage, FaxMessage);
            strcpy (Ch->FaxCommand, FaxCommand);
            Ch->DialTimeout = DialTimeout;
            Ch->CarrierDropTimeout = CarrierDropTimeout;
            strcpy (Ch->SchedulerFile, SchedulerFile);
            strcpy (Ch->MainMenu, MainMenu);
            strcpy (Ch->Ring, Ring);

            write (fd, Ch, sizeof (CHANNEL));
         }
         close (fd);
      }
      free (Ch);
   }

   if (SystemPath[0] != '\0')
      MailAddress.Save (SystemPath);

   return (RetVal);
}


