
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "config.h"

TConfig::TConfig (PSZ pszConfig, PSZ pszChannels)
{
   LastChannel = 0xFFFFU;
   strcpy (Config, pszConfig);
   strcpy (Channels, pszChannels);
}

TConfig::TConfig (PSZ pszConfig, PSZ pszChannels, USHORT usChannel)
{
   LastChannel = 0xFFFFU;
   strcpy (Config, pszConfig);
   strcpy (Channels, pszChannels);
   Read (usChannel, pszConfig, pszChannels);
}

TConfig::TConfig (PSZ pszDataPath)
{
   LastChannel = 0xFFFFU;
   strcpy (Config, pszDataPath);
   if (strlen (Config) > 0 && Config[strlen (Config) - 1] != '\\')
      strcat (Config, "\\");
   strcat (Config, "Config.Dat");
   strcpy (Channels, pszDataPath);
   if (strlen (Channels) > 0 && Channels[strlen (Channels) - 1] != '\\')
      strcat (Channels, "\\");
   strcat (Channels, "Channels.Dat");
}

TConfig::~TConfig (void)
{
   Clear ();
}

USHORT TConfig::ReadFirstChannel (VOID)
{
   LastChannel = 0xFFFFU;
   return (ReadNextChannel ());
}

VOID TConfig::Clear (VOID)
{
   Type = 0;
   Channel = 0;
   MaxChannels = 0;
   Cost = 0L;
   memset (Device, 0, sizeof (Device));
   Speed = 0L;
   memset (Initialize, 0, sizeof (Initialize));
   memset (Answer, 0, sizeof (Answer));
   memset (Hangup, 0, sizeof (Hangup));
   memset (OffHook, 0, sizeof (OffHook));
   memset (Dial, 0, sizeof (Dial));
   memset (PipeName, 0, sizeof (PipeName));
   Port = 0;
   NewLevel = 0;
   memset (NewLimitClass, 0, sizeof (NewLimitClass));
   NewAccessFlags = 0L;
   NewDenyFlags = 0L;
   RookieCalls = 0;
   MinimumAge = 0;
   AskRealName = AskCompany = AskAddress = AskZipCity = 0;
   AskState = AskPhone = AskSex = 0;
   InactivityTime = LoginTime = 0;
   memset (Logo, 0, sizeof (Logo));
   Calls = 0L;

   memset (SystemName, 0, sizeof (SystemName));
   memset (SysopName, 0, sizeof (SysopName));
   memset (SystemPath, 0, sizeof (SystemPath));
   memset (MiscPath, 0, sizeof (MiscPath));
   memset (MenuPath, 0, sizeof (MenuPath));
   memset (UserFile, 0, sizeof (UserFile));
   memset (HomePath, 0, sizeof (HomePath));
   memset (LogPath, 0, sizeof (LogPath));
   LogLevel = 0;
   memset (MailSpool, 0, sizeof (MailSpool));
   memset (SentMail, 0, sizeof (SentMail));
   memset (HostName, 0, sizeof (HostName));
   memset (NewsServer, 0, sizeof (NewsServer));
   memset (SMTPServer, 0, sizeof (SMTPServer));
   memset (POP3Server, 0, sizeof (POP3Server));
   memset (Inbound, 0, sizeof (Inbound));
   memset (InboundProt, 0, sizeof (InboundProt));
   memset (InboundKnown, 0, sizeof (InboundKnown));
   memset (Outbound, 0, sizeof (Outbound));
   memset (Welcome, 0, sizeof (Welcome));
   memset (BbsId, 0, sizeof (BbsId));
   memset (CityState, 0, sizeof (CityState));
   memset (PhoneNumber, 0, sizeof (PhoneNumber));
   memset (Bulletin, 0, sizeof (Bulletin));
   memset (Welcome, 0, sizeof (Welcome));
   memset (FirstMenu, 0, sizeof (FirstMenu));
   BaseNode = 0;
   memset (CommonPath, 0, sizeof (CommonPath));
   memset (TempPath, 0, sizeof (TempPath));
   PwdAttempts = 0;
   MaxOfflineMsgs = 0;
   MinAge = MaxAge = 0;
   TotalCalls = MaxBankTime = 0L;
   MaxInactiveUser = 0;
   memset (FirstMessageArea, 0, sizeof (FirstMessageArea));
   memset (FirstFileArea, 0, sizeof (FirstFileArea));
   FtpServer = Pop3Server = FALSE;

   Address.Clear ();
   NodeList.Clear ();
   Language.Clear ();
}

VOID TConfig::ClearChannel (VOID)
{
   Type = 0;
   Channel = 0;
   MaxChannels = 0;
   Cost = 0L;
   memset (Device, 0, sizeof (Device));
   Speed = 0L;
   memset (Initialize, 0, sizeof (Initialize));
   memset (Answer, 0, sizeof (Answer));
   memset (Hangup, 0, sizeof (Hangup));
   memset (OffHook, 0, sizeof (OffHook));
   memset (Dial, 0, sizeof (Dial));
   memset (PipeName, 0, sizeof (PipeName));
   Port = 0;
   NewLevel = 0;
   memset (NewLimitClass, 0, sizeof (NewLimitClass));
   NewAccessFlags = 0L;
   NewDenyFlags = 0L;
   RookieCalls = 0;
   MinimumAge = 0;
   AskRealName = AskCompany = AskAddress = AskZipCity = 0;
   AskState = AskPhone = AskSex = 0;
   InactivityTime = LoginTime = 0;
   memset (Logo, 0, sizeof (Logo));
   Calls = 0L;
}

USHORT TConfig::ReadNextChannel (VOID)
{
   int fd;
   USHORT i, retVal = FALSE;
   CHANNEL *Ch;
   CONFIG *Cfg;
   ADDR Addr;
   NODELIST *Node;
   LANGUAGE *Lang;

   if ((Ch = (CHANNEL *)malloc (sizeof (CHANNEL))) != NULL) {
      memset (Ch, 0, sizeof (CHANNEL));
      if ((fd = open (Channels, O_RDONLY|O_BINARY)) != -1) {
         while (retVal == FALSE && read (fd, Ch, sizeof (CHANNEL)) == sizeof (*Ch)) {
            if (LastChannel == 0xFFFFU || Ch->Channel > LastChannel) {
               Clear ();
               Type = Ch->Type;
               Channel = Ch->Channel;
               MaxChannels = Ch->MaxChannels;
               Cost = Ch->Cost;
               strcpy (Device, Ch->Device);
               Speed = Ch->Speed;
               strcpy (Initialize[0], Ch->Initialize[0]);
               strcpy (Initialize[1], Ch->Initialize[1]);
               strcpy (Initialize[2], Ch->Initialize[2]);
               strcpy (Answer, Ch->Answer);
               strcpy (Hangup, Ch->Hangup);
               strcpy (OffHook, Ch->OffHook);
               strcpy (Dial, Ch->Dial);
               strcpy (PipeName, Ch->PipeName);
               Port = Ch->Port;
               NewLevel = Ch->NewLevel;
               strcpy (NewLimitClass, Ch->NewLimitClass);
               NewAccessFlags = Ch->NewAccessFlags;
               NewDenyFlags = Ch->NewDenyFlags;
               RookieCalls = Ch->RookieCalls;
               MinimumAge = Ch->MinimumAge;
               AskRealName = Ch->AskRealName;
               AskCompany = Ch->AskCompany;
               AskAddress = Ch->AskAddress;
               AskZipCity = Ch->AskZipCity;
               AskState = Ch->AskState;
               AskPhone = Ch->AskPhone;
               AskSex = Ch->AskSex;
               InactivityTime = Ch->InactivityTime;
               LoginTime = Ch->LoginTime;
               strcpy (Logo, Ch->Logo);
               Calls = Ch->Calls;

               LastChannel = Channel;
               retVal = TRUE;
            }
         }
         close (fd);
      }
      free (Ch);
   }

   if (retVal == TRUE) {
      retVal = FALSE;

      Cfg = (CONFIG *)malloc (sizeof (CONFIG));
      Lang = (LANGUAGE *)malloc (sizeof (LANGUAGE));
      Node = (NODELIST *)malloc (sizeof (NODELIST));
      fd = open (Config, O_RDONLY|O_BINARY);

      if (fd != -1 && Cfg != NULL && Lang != NULL && Node != NULL) {
         read (fd, Cfg, sizeof (CONFIG));
         strcpy (SystemName, Cfg->SystemName);
         strcpy (SysopName, Cfg->SysopName);
         strcpy (SystemPath, Cfg->SystemPath);
         strcpy (MiscPath, Cfg->MiscPath);
         strcpy (MenuPath, Cfg->MenuPath);
         strcpy (UserFile, Cfg->UserFile);
         strcpy (HomePath, Cfg->HomePath);
         strcpy (LogPath, Cfg->LogPath);
         LogLevel = Cfg->LogLevel;
         strcpy (MailSpool, Cfg->MailSpool);
         strcpy (SentMail, Cfg->SentMail);
         strcpy (HostName, Cfg->HostName);
         strcpy (NewsServer, Cfg->NewsServer);
         strcpy (SMTPServer, Cfg->SMTPServer);
         strcpy (Inbound, Cfg->Inbound);
         strcpy (InboundProt, Cfg->InboundProt);
         strcpy (InboundKnown, Cfg->InboundKnown);
         strcpy (Outbound, Cfg->Outbound);
         strcpy (BbsId, Cfg->BbsId);
         strcpy (CityState, Cfg->CityState);
         strcpy (PhoneNumber, Cfg->PhoneNumber);
         strcpy (Bulletin, Cfg->Bulletin);
         strcpy (Welcome, Cfg->Welcome);
         strcpy (FirstMenu, Cfg->FirstMenu);
         BaseNode = Cfg->BaseNode;
         strcpy (TempPath, Cfg->TempPath);
         PwdAttempts = Cfg->PwdAttempts;
         MaxOfflineMsgs = Cfg->MaxOfflineMsgs;
         TotalCalls = Cfg->TotalCalls;
         MaxBankTime = Cfg->MaxBankTime;
         MaxInactiveUser = Cfg->MaxInactiveUser;
         strcpy (FirstMessageArea, Cfg->FirstMessageArea);
         strcpy (FirstFileArea, Cfg->FirstFileArea);
         FtpServer = Cfg->FtpServer;
         Pop3Server = Cfg->Pop3Server;

         for (i = 0; i < Cfg->FidoAddresses; i++) {
            memset (&Addr, 0, sizeof (ADDR));
            read (fd, &Addr, sizeof (ADDR));
            Address.Add (Addr.Zone, Addr.Net, Addr.Node, Addr.Point, Addr.Domain);
         }

         for (i = 0; i < Cfg->FidoNodelists; i++) {
            memset (Node, 0, sizeof (NODELIST));
            read (fd, Node, sizeof (NODELIST));
            NodeList.Add (Node, sizeof (NODELIST));
         }

         for (i = 0; i < Cfg->Languages; i++) {
            memset (Lang, 0, sizeof (LANGUAGE));
            read (fd, Lang, sizeof (LANGUAGE));
            Language.Add (Lang);
         }

         retVal = TRUE;
      }

      if (fd != -1)
         close (fd);
      if (Node != NULL)
         free (Node);
      if (Lang != NULL)
         free (Lang);
      if (Cfg != NULL)
         free (Cfg);
   }

   return (retVal);
}

USHORT TConfig::Read (USHORT usChannel, PSZ pszConfig, PSZ pszChannels)
{
   int fd;
   USHORT i, retVal = FALSE;
   CHANNEL *Ch;
   CONFIG *Cfg;
   ADDR Addr;
   NODELIST *Node;
   LANGUAGE *Lang;

   if (pszConfig == NULL)
      pszConfig = Config;
   if (pszChannels == NULL)
      pszChannels = Channels;

   if ((Ch = (CHANNEL *)malloc (sizeof (CHANNEL))) != NULL) {
      memset (Ch, 0, sizeof (CHANNEL));
      if ((fd = open (pszChannels, O_RDONLY|O_BINARY)) != -1) {
         while (retVal == FALSE && read (fd, Ch, sizeof (CHANNEL)) == sizeof (*Ch)) {
            if (Ch->Channel == usChannel) {
               Clear ();
               Type = Ch->Type;
               Channel = Ch->Channel;
               MaxChannels = Ch->MaxChannels;
               Cost = Ch->Cost;
               strcpy (Device, Ch->Device);
               Speed = Ch->Speed;
               strcpy (Initialize[0], Ch->Initialize[0]);
               strcpy (Initialize[1], Ch->Initialize[1]);
               strcpy (Initialize[2], Ch->Initialize[2]);
               strcpy (Answer, Ch->Answer);
               strcpy (Hangup, Ch->Hangup);
               strcpy (OffHook, Ch->OffHook);
               strcpy (Dial, Ch->Dial);
               strcpy (PipeName, Ch->PipeName);
               Port = Ch->Port;
               NewLevel = Ch->NewLevel;
               strcpy (NewLimitClass, Ch->NewLimitClass);
               NewAccessFlags = Ch->NewAccessFlags;
               NewDenyFlags = Ch->NewDenyFlags;
               RookieCalls = Ch->RookieCalls;
               MinimumAge = Ch->MinimumAge;
               AskRealName = Ch->AskRealName;
               AskCompany = Ch->AskCompany;
               AskAddress = Ch->AskAddress;
               AskZipCity = Ch->AskZipCity;
               AskState = Ch->AskState;
               AskPhone = Ch->AskPhone;
               AskSex = Ch->AskSex;
               InactivityTime = Ch->InactivityTime;
               LoginTime = Ch->LoginTime;
               strcpy (Logo, Ch->Logo);
               Calls = Ch->Calls;

               LastChannel = Channel;
               retVal = TRUE;
            }
         }
         close (fd);
      }
      free (Ch);
   }

   if (retVal == TRUE) {
      retVal = FALSE;

      Cfg = (CONFIG *)malloc (sizeof (CONFIG));
      Lang = (LANGUAGE *)malloc (sizeof (LANGUAGE));
      Node = (NODELIST *)malloc (sizeof (NODELIST));
      fd = open (pszConfig, O_RDONLY|O_BINARY);

      if (fd != -1 && Cfg != NULL && Lang != NULL && Node != NULL) {
         read (fd, Cfg, sizeof (CONFIG));
         strcpy (SystemName, Cfg->SystemName);
         strcpy (SysopName, Cfg->SysopName);
         strcpy (SystemPath, Cfg->SystemPath);
         strcpy (MiscPath, Cfg->MiscPath);
         strcpy (MenuPath, Cfg->MenuPath);
         strcpy (UserFile, Cfg->UserFile);
         strcpy (HomePath, Cfg->HomePath);
         strcpy (LogPath, Cfg->LogPath);
         LogLevel = Cfg->LogLevel;
         strcpy (MailSpool, Cfg->MailSpool);
         strcpy (SentMail, Cfg->SentMail);
         strcpy (HostName, Cfg->HostName);
         strcpy (NewsServer, Cfg->NewsServer);
         strcpy (SMTPServer, Cfg->SMTPServer);
         strcpy (Inbound, Cfg->Inbound);
         strcpy (InboundProt, Cfg->InboundProt);
         strcpy (InboundKnown, Cfg->InboundKnown);
         strcpy (Outbound, Cfg->Outbound);
         strcpy (BbsId, Cfg->BbsId);
         strcpy (CityState, Cfg->CityState);
         strcpy (PhoneNumber, Cfg->PhoneNumber);
         strcpy (Bulletin, Cfg->Bulletin);
         strcpy (Welcome, Cfg->Welcome);
         strcpy (FirstMenu, Cfg->FirstMenu);
         BaseNode = Cfg->BaseNode;
         strcpy (TempPath, Cfg->TempPath);
         PwdAttempts = Cfg->PwdAttempts;
         MaxOfflineMsgs = Cfg->MaxOfflineMsgs;
         TotalCalls = Cfg->TotalCalls;
         MaxBankTime = Cfg->MaxBankTime;
         MaxInactiveUser = Cfg->MaxInactiveUser;
         strcpy (FirstMessageArea, Cfg->FirstMessageArea);
         strcpy (FirstFileArea, Cfg->FirstFileArea);
         FtpServer = Cfg->FtpServer;
         Pop3Server = Cfg->Pop3Server;

         for (i = 0; i < Cfg->FidoAddresses; i++) {
            memset (&Addr, 0, sizeof (ADDR));
            read (fd, &Addr, sizeof (ADDR));
            Address.Add (Addr.Zone, Addr.Net, Addr.Node, Addr.Point, Addr.Domain);
         }

         for (i = 0; i < Cfg->FidoNodelists; i++) {
            memset (Node, 0, sizeof (NODELIST));
            read (fd, Node, sizeof (NODELIST));
            NodeList.Add (Node, sizeof (NODELIST));
         }

         for (i = 0; i < Cfg->Languages; i++) {
            memset (Lang, 0, sizeof (LANGUAGE));
            read (fd, Lang, sizeof (LANGUAGE));
            Language.Add (Lang);
         }

         retVal = TRUE;
      }

      if (fd != -1)
         close (fd);
      if (Node != NULL)
         free (Node);
      if (Lang != NULL)
         free (Lang);
      if (Cfg != NULL)
         free (Cfg);
   }

   return (retVal);
}

USHORT TConfig::Remove (USHORT usChannel, PSZ pszChannels)
{
   int fd, fdNew;
   USHORT retVal = FALSE;
   CHANNEL *Ch;

   if (pszChannels == NULL)
      pszChannels = Channels;

   if ((Ch = (CHANNEL *)malloc (sizeof (CHANNEL))) != NULL) {
      memset (Ch, 0, sizeof (CHANNEL));
      if ((fdNew = open ("CFGNEW.TMP", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
         if ((fd = open (pszChannels, O_RDONLY|O_BINARY)) != -1) {
            while (read (fd, Ch, sizeof (CHANNEL)) == sizeof (CHANNEL)) {
               if (Ch->Channel == usChannel)
                  retVal = TRUE;
               else
                  write (fdNew, Ch, sizeof (CHANNEL));
            }
            close (fd);
         }

         close (fdNew);

         if (retVal == TRUE) {
            unlink (pszChannels);
            rename ("CFGNEW.TMP", pszChannels);
         }
         else
            unlink ("CFGNEW.TMP");
      }
      free (Ch);
   }

   return (retVal);
}

USHORT TConfig::Write (USHORT usChannel, PSZ pszConfig, PSZ pszChannels)
{
   int fd, fdNew;
   USHORT retVal = FALSE;
   CHANNEL *Ch;
   CONFIG *Cfg;
   ADDR Addr;
   NODELIST *Node;
   LANGUAGE *Lang;

   if (pszConfig == NULL)
      pszConfig = Config;
   if (pszChannels == NULL)
      pszChannels = Channels;

   if ((Ch = (CHANNEL *)malloc (sizeof (CHANNEL))) != NULL) {
      memset (Ch, 0, sizeof (CHANNEL));
      if ((fdNew = open ("CFGNEW.TMP", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
         if ((fd = open (pszChannels, O_RDONLY|O_BINARY)) != -1) {
            while (read (fd, Ch, sizeof (CHANNEL)) == sizeof (*Ch)) {
               if (retVal == FALSE && Ch->Channel == usChannel) {
                  memset (Ch, 0, sizeof (CHANNEL));
                  Ch->Type = Type;
                  Ch->Channel = usChannel;
                  Ch->MaxChannels = MaxChannels;
                  Ch->Cost = Cost;
                  strcpy (Ch->Device, Device);
                  Ch->Speed = Speed;
                  strcpy (Ch->Initialize[0], Initialize[0]);
                  strcpy (Ch->Initialize[1], Initialize[1]);
                  strcpy (Ch->Initialize[2], Initialize[2]);
                  strcpy (Ch->Answer, Answer);
                  strcpy (Ch->Hangup, Hangup);
                  strcpy (Ch->OffHook, OffHook);
                  strcpy (Ch->Dial, Dial);
                  strcpy (Ch->PipeName, PipeName);
                  Ch->Port = Port;
                  Ch->NewLevel = NewLevel;
                  strcpy (Ch->NewLimitClass, NewLimitClass);
                  Ch->NewAccessFlags = NewAccessFlags;
                  Ch->NewDenyFlags = NewDenyFlags;
                  Ch->RookieCalls = RookieCalls;
                  Ch->MinimumAge = MinimumAge;
                  Ch->AskRealName = AskRealName;
                  Ch->AskCompany = AskCompany;
                  Ch->AskAddress = AskAddress;
                  Ch->AskZipCity = AskZipCity;
                  Ch->AskState = AskState;
                  Ch->AskPhone = AskPhone;
                  Ch->AskSex = AskSex;
                  Ch->InactivityTime = InactivityTime;
                  Ch->LoginTime = LoginTime;
                  strcpy (Ch->Logo, Logo);
                  Ch->Calls = Calls;
                  write (fdNew, Ch, sizeof (CHANNEL));

                  retVal = TRUE;
               }
               else if (retVal == FALSE && Ch->Channel > usChannel) {
                  memset (Ch, 0, sizeof (CHANNEL));
                  Ch->Type = Type;
                  Ch->Channel = usChannel;
                  Ch->MaxChannels = MaxChannels;
                  Ch->Cost = Cost;
                  strcpy (Ch->Device, Device);
                  Ch->Speed = Speed;
                  strcpy (Ch->Initialize[0], Initialize[0]);
                  strcpy (Ch->Initialize[1], Initialize[1]);
                  strcpy (Ch->Initialize[2], Initialize[2]);
                  strcpy (Ch->Answer, Answer);
                  strcpy (Ch->Hangup, Hangup);
                  strcpy (Ch->OffHook, OffHook);
                  strcpy (Ch->Dial, Dial);
                  strcpy (Ch->PipeName, PipeName);
                  Ch->Port = Port;
                  Ch->NewLevel = NewLevel;
                  strcpy (Ch->NewLimitClass, NewLimitClass);
                  Ch->NewAccessFlags = NewAccessFlags;
                  Ch->NewDenyFlags = NewDenyFlags;
                  Ch->RookieCalls = RookieCalls;
                  Ch->MinimumAge = MinimumAge;
                  Ch->AskRealName = AskRealName;
                  Ch->AskCompany = AskCompany;
                  Ch->AskAddress = AskAddress;
                  Ch->AskZipCity = AskZipCity;
                  Ch->AskState = AskState;
                  Ch->AskPhone = AskPhone;
                  Ch->AskSex = AskSex;
                  Ch->InactivityTime = InactivityTime;
                  Ch->LoginTime = LoginTime;
                  strcpy (Ch->Logo, Logo);
                  Ch->Calls = Calls;
                  write (fdNew, Ch, sizeof (CHANNEL));

                  lseek (fd, tell (fd) - sizeof (CHANNEL), SEEK_SET);
                  read (fd, Ch, sizeof (CHANNEL));
                  write (fdNew, Ch, sizeof (CHANNEL));

                  retVal = TRUE;
               }
               else
                  write (fdNew, Ch, sizeof (CHANNEL));
            }
            close (fd);
         }

         if (retVal == FALSE) {
            memset (Ch, 0, sizeof (CHANNEL));
            Ch->Type = Type;
            Ch->Channel = usChannel;
            Ch->MaxChannels = MaxChannels;
            Ch->Cost = Cost;
            strcpy (Ch->Device, Device);
            Ch->Speed = Speed;
            strcpy (Ch->Initialize[0], Initialize[0]);
            strcpy (Ch->Initialize[1], Initialize[1]);
            strcpy (Ch->Initialize[2], Initialize[2]);
            strcpy (Ch->Answer, Answer);
            strcpy (Ch->Hangup, Hangup);
            strcpy (Ch->OffHook, OffHook);
            strcpy (Ch->Dial, Dial);
            strcpy (Ch->PipeName, PipeName);
            Ch->Port = Port;
            Ch->NewLevel = NewLevel;
            strcpy (Ch->NewLimitClass, NewLimitClass);
            Ch->NewAccessFlags = NewAccessFlags;
            Ch->NewDenyFlags = NewDenyFlags;
            Ch->RookieCalls = RookieCalls;
            Ch->MinimumAge = MinimumAge;
            Ch->AskRealName = AskRealName;
            Ch->AskCompany = AskCompany;
            Ch->AskAddress = AskAddress;
            Ch->AskZipCity = AskZipCity;
            Ch->AskState = AskState;
            Ch->AskPhone = AskPhone;
            Ch->AskSex = AskSex;
            Ch->InactivityTime = InactivityTime;
            Ch->LoginTime = LoginTime;
            strcpy (Ch->Logo, Logo);
            Ch->Calls = Calls;
            write (fdNew, Ch, sizeof (CHANNEL));

            retVal = TRUE;
         }

         close (fdNew);

         if (retVal == TRUE) {
            unlink (pszChannels);
            rename ("CFGNEW.TMP", pszChannels);
         }
         else
            unlink ("CFGNEW.TMP");
      }
      free (Ch);
   }

   if (retVal == TRUE) {
      retVal = FALSE;
      if ((Cfg = (CONFIG *)malloc (sizeof (CONFIG))) != NULL) {
         memset (Cfg, 0, sizeof (CONFIG));
         if ((fd = open (pszConfig, O_WRONLY|O_BINARY|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE)) != -1) {
            memset (Cfg, 0, sizeof (CONFIG));
            strcpy (Cfg->SystemName, SystemName);
            strcpy (Cfg->SysopName, SysopName);
            strcpy (Cfg->SystemPath, SystemPath);
            strcpy (Cfg->MiscPath, MiscPath);
            strcpy (Cfg->MenuPath, MenuPath);
            strcpy (Cfg->UserFile, UserFile);
            strcpy (Cfg->HomePath, HomePath);
            strcpy (Cfg->LogPath, LogPath);
            Cfg->LogLevel = LogLevel;
            strcpy (Cfg->MailSpool, MailSpool);
            strcpy (Cfg->SentMail, SentMail);
            strcpy (Cfg->HostName, HostName);
            strcpy (Cfg->NewsServer, NewsServer);
            strcpy (Cfg->SMTPServer, SMTPServer);
            strcpy (Cfg->Inbound, Inbound);
            strcpy (Cfg->InboundProt, InboundProt);
            strcpy (Cfg->InboundKnown, InboundKnown);
            strcpy (Cfg->Outbound, Outbound);
            strcpy (Cfg->BbsId, BbsId);
            strcpy (Cfg->CityState, CityState);
            strcpy (Cfg->PhoneNumber, PhoneNumber);
            strcpy (Cfg->Bulletin, Bulletin);
            strcpy (Cfg->Welcome, Welcome);
            strcpy (Cfg->FirstMenu, FirstMenu);
            Cfg->BaseNode = BaseNode;
            strcpy (Cfg->TempPath, TempPath);
            Cfg->PwdAttempts = PwdAttempts;
            Cfg->MaxOfflineMsgs = MaxOfflineMsgs;
            Cfg->TotalCalls = TotalCalls;
            Cfg->MaxBankTime = MaxBankTime;
            Cfg->MaxInactiveUser = MaxInactiveUser;
            strcpy (Cfg->FirstMessageArea, FirstMessageArea);
            strcpy (Cfg->FirstFileArea, FirstFileArea);
            Cfg->FtpServer = FtpServer;
            Cfg->Pop3Server = Pop3Server;
            if (Address.First () == TRUE)
               do {
                  Cfg->FidoAddresses++;
               } while (Address.Next () == TRUE);
            if (NodeList.First () != NULL)
               do {
                  Cfg->FidoNodelists++;
               } while (NodeList.Next () != NULL);
            if (Language.First () != NULL)
               do {
                  Cfg->Languages++;
               } while (Language.Next () != NULL);
            write (fd, Cfg, sizeof (CONFIG));

            if (Address.First () == TRUE)
               do {
                  memset (&Addr, 0, sizeof (Addr));
                  Addr.Zone = Address.Zone;
                  Addr.Net = Address.Net;
                  Addr.Node = Address.Node;
                  Addr.Point = Address.Point;
                  strcpy (Addr.Domain, Address.Domain);
                  write (fd, &Addr, sizeof (ADDR));
               } while (Address.Next () == TRUE);
            if ((Node = (NODELIST *)NodeList.First ()) != NULL)
               do {
                  write (fd, Node, sizeof (NODELIST));
               } while ((Node = (NODELIST *)NodeList.Next ()) != NULL);
            if ((Lang = (LANGUAGE *)malloc (sizeof (LANGUAGE))) != NULL) {
               if (Language.First () == TRUE)
                  do {
                     memset (Lang, 0, sizeof (LANGUAGE));
                     strcpy (Lang->Key, Language.Key);
                     strcpy (Lang->Description, Language.Description);
                     strcpy (Lang->File, Language.File);
                     strcpy (Lang->MenuPath, Language.MenuPath);
                     strcpy (Lang->MiscPath, Language.MiscPath);
                     write (fd, Lang, sizeof (LANGUAGE));
                  } while (Language.Next () == TRUE);
               free (Lang);
            }
            close (fd);
         }
         free (Cfg);
      }
   }

   return (retVal);
}

// ----------------------------------------------------------------------

TLangCfg::TLangCfg (void)
{
}

TLangCfg::~TLangCfg (void)
{
   Data.Clear ();
}

VOID TLangCfg::Add (VOID)
{
   LANGUAGE Lang;

   memset (&Lang, 0, sizeof (Lang));
   strcpy (Lang.Key, Key);
   strcpy (Lang.Description, Description);
   strcpy (Lang.File, File);
   strcpy (Lang.MenuPath, MenuPath);
   strcpy (Lang.MiscPath, MiscPath);
   Data.Add (&Lang, sizeof (Lang));
}

VOID TLangCfg::Add (LANGUAGE *lpLanguage)
{
   Data.Add (lpLanguage, sizeof (LANGUAGE));
}

USHORT TLangCfg::Check (PSZ pszKey)
{
   USHORT RetVal = FALSE;
   LANGUAGE *Lang;

   if ((Lang = (LANGUAGE *)Data.First ()) != NULL)
      do {
         if (!stricmp (Lang->Key, pszKey)) {
            strcpy (Key, Lang->Key);
            strcpy (Description, Lang->Description);
            strcpy (File, Lang->File);
            strcpy (MenuPath, Lang->MenuPath);
            strcpy (MiscPath, Lang->MiscPath);
            RetVal = TRUE;
         }
      } while ((Lang = (LANGUAGE *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TLangCfg::Clear (VOID)
{
   Data.Clear ();
}

USHORT TLangCfg::First (VOID)
{
   USHORT RetVal = FALSE;
   LANGUAGE *Lang;

   if ((Lang = (LANGUAGE *)Data.First ()) != NULL) {
      strcpy (Key, Lang->Key);
      strcpy (Description, Lang->Description);
      strcpy (File, Lang->File);
      strcpy (MenuPath, Lang->MenuPath);
      strcpy (MiscPath, Lang->MiscPath);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TLangCfg::Next (VOID)
{
   USHORT RetVal = FALSE;
   LANGUAGE *Lang;

   if ((Lang = (LANGUAGE *)Data.Next ()) != NULL) {
      strcpy (Key, Lang->Key);
      strcpy (Description, Lang->Description);
      strcpy (File, Lang->File);
      strcpy (MenuPath, Lang->MenuPath);
      strcpy (MiscPath, Lang->MiscPath);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TLangCfg::Remove (VOID)
{
   Data.Remove ();
}

VOID TLangCfg::Update (VOID)
{
   LANGUAGE Lang;

   memset (&Lang, 0, sizeof (Lang));
   strcpy (Lang.Key, Key);
   strcpy (Lang.Description, Description);
   strcpy (Lang.File, File);
   strcpy (Lang.MenuPath, MenuPath);
   strcpy (Lang.MiscPath, MiscPath);
   Data.Replace (&Lang, sizeof (Lang));
}



