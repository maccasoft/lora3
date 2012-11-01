
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
#include "ftrans.h"
#include "lora.h"
#include "msgbase.h"

#define YOOHOO             0xF1
#define TSYNC              0xAE

#define isOPUS             0x05
#define isLORA             0x4E
#define isMAX_PRODUCT      0xE2

#define EMSI_NONE          0
#define EMSI_ACK           1
#define EMSI_NAK           2
#define EMSI_ICI           3
#define EMSI_INQ           4
#define EMSI_REQ           5
#define EMSI_DAT           6

typedef struct {
   USHORT Signal;
   USHORT Version;
   USHORT Product;
   USHORT ProductMaj;
   USHORT ProductMin;
   CHAR   Name[60];
   CHAR   Sysop[20];
   USHORT Zone;
   USHORT Net;
   USHORT Node;
   USHORT Point;
   CHAR   Password[8];
   UCHAR  Reserved1[4];
   USHORT PktFiles;           // Used only by LoraBBS >= 2.20
   USHORT DataFiles;          // Used only by LoraBBS >= 2.20
   USHORT Capabilities;
   ULONG  Tranx;              // Used only by LoraBBS >= 2.10
   ULONG  PktBytes;           // Used only by LoraBBS >= 2.20
   ULONG  DataBytes;          // Used only by LoraBBS >= 2.20
} HELLO;

#define Y_DIETIFNA         0x0001
#define FTB_USER           0x0002
#define ZED_ZIPPER         0x0004
#define ZED_ZAPPER         0x0008
#define DOES_IANUS         0x0010
#define DO_DOMAIN          0x4000
#define WZ_FREQ            0x8000

char *ProductNames[] = {
   "Fido",
   "Rover",
   "SEAdog",
   "",
   "Slick/150",
   "Opus",
   "Dutchie",
   "",
   "Tabby",
   "Hoster",
   "Wolf/68k",
   "QMM",
   "FrontDoor",
   "",
   "",
   "",
   "",
   "MailMan",
   "OOPS",
   "GS-Point",
   "BGMail",
   "CrossBow",
   "",
   "",
   "",
   "BinkScan",
   "D'Bridge",
   "BinkleyTerm",
   "Yankee",
   "FGet/FSend",
   "Daisy,Apple ][",
   "Polar Bear",
   "The-Box",
   "STARgate/2",
   "TMail",
   "TCOMMail",
   "Bananna",
   "RBBSMail",
   "Apple-Netmail",
   "Chameleon",
   "Majik Board",
   "QMail",
   "Point And Click",
   "Aurora Three Bundler",
   "FourDog",
   "MSG-PACK",
   "AMAX",
   "Domain Communication System",
   "LesRobot",
   "Rose",
   "Paragon",
   "BinkleyTerm",
   "StarNet",
   "ZzyZx",
   "QEcho",
   "BOOM",
   "PBBS",
   "TrapDoor",
   "Welmat",
   "NetGate",
   "Odie",
   "Quick Gimme",
   "dbLink",
   "TosScan",
   "Beagle",
   "Igor",
   "TIMS",
   "Isis",
   "AirMail",
   "XRS",
   "Juliet Mail System",
   "Jabberwocky",
   "XST",
   "MailStorm",
   "BIX-Mail",
   "IMAIL",
   "FTNGate",
   "RealMail",
   "LoraBBS",
   "TDCS",
   "InterMail",
   "RFD",
   "Yuppie!",
   "EMMA",
   "QBoxMail",
   "Number 4",
   "Number 5",
   "GSBBS",
   "Merlin",
   "TPCS",
   "Raid",
   "Outpost",
   "Nizze",
   "Armadillo",
   "rfmail",
   "Msgtoss",
   "InfoTex",
   "GEcho",
   "CDEhost",
   "Pktize",
   "PC-RAIN",
   "Truffle",
   "Foozle",
   "White Pointer",
   "GateWorks",
   "Portal of Power",
   "MacWoof",
   "Mosaic",
   "TPBEcho",
   "HandyMail",
   "EchoSmith",
   "FileHost",
   "SFScan",
   "Benjamin",
   "RiBBS",
   "MP",
   "Ping",
   "Door2Europe",
   "SWIFT",
   "WMAIL",
   "RATS",
   "Harry the Dirty Dog",
   "Maximus-CBCS",
   "SwifEcho",
   "GCChost",
   "RPX-Mail",
   "Tosser",
   "TCL",
   "MsgTrack",
   "FMail",
   "Scantoss",
   "Point Manager",
   "Dropped",
   "Simplex",
   "UMTP",
   "Indaba",
   "Echomail Engine",
   "DragonMail",
   "Prox",
   "Tick",
   "RA-Echo",
   "TrapToss",
   "Babel",
   "UMS",
   "RWMail",
   "WildMail",
   "AlMAIL",
   "XCS",
   "Fone-Link",
   "Dogfight",
   "Ascan",
   "FastMail",
   "DoorMan",
   "PhaedoZap",
   "SCREAM",
   "MoonMail",
   "Backdoor",
   "MailLink",
   "Mail Manager",
   "Black Star",
   "Bermuda",
   "PT",
   "UltiMail",
   "GMD",
   "FreeMail",
   "Meliora",
   "Foodo",
   "MSBBS",
   "Boston BBS",
   "XenoMail",
   "XenoLink",
   "ObjectMatrix",
   "Milquetoast",
   "PipBase",
   "EzyMail",
   "FastEcho",
   "IOS",
   "Communique",
   "PointMail",
   "Harvey's Robot",
   "2daPoint",
   "CommLink",
   "fronttoss",
   "SysopPoint",
   "PTMAIL",
   "AECHO",
   "DLGMail",
   "GatePrep",
   "Spoint",
   "TurboMail",
   "FXMAIL",
   "NextBBS",
   "EchoToss",
   "SilverBox",
   "MBMail",
   "SkyFreq",
   "ProMailer",
   "Mega Mail",
   "YaBom",
   "TachEcho",
   "XAP",
   "EZMAIL",
   "Arc-Binkley",
   "Roser",
   "UU2",
   "NMS",
   "BBCSCAN",
   "XBBS",
   "LoTek Vzrul",
   "Private Point Project",
   "NoSnail",
   "SmlNet",
   "STIR",
   "RiscBBS",
   "Hercules",
   "AMPRGATE",
   "BinkEMSI",
   "EditMsg",
   "Roof",
   "QwkPkt",
   "MARISCAN",
   "NewsFlash",
   "Paradise",
   "DogMatic-ACB",
   "T-Mail",
   "JetMail",
   "MainDoor"
};

TDetect::TDetect ()
{
   Ansi = Avatar = Rip = FALSE;
   Remote = REMOTE_NONE;
   EMSI = YooHoo = IEMSI = FALSE;
   Com = NULL;
   Cfg = NULL;
   Progress = NULL;
   MailerStatus = NULL;
   Status = NULL;
   LastPktName = 0L;
   Speed = 57600L;

   ReceiveEMSI = ReceiveIEMSI;
   SendEMSI = SendIEMSI;
   Address.Clear ();
}

TDetect::~TDetect ()
{
}

USHORT TDetect::AbortSession ()
{
   USHORT RetVal = FALSE;

   if (Com != NULL) {
      if (Com->Carrier () == FALSE)
         RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TDetect::CheckEMSIPacket ()
{
   USHORT key, RetVal = EMSI_NONE;
   CHAR pkt[4];

   do {
      key = TimedRead ();
      if (key != 'E' && key != '*')
         return (RetVal);
   } while (key != 'E');

   if (TimedRead () != 'M')
      return (RetVal);
   if (TimedRead () != 'S')
      return (RetVal);
   if (TimedRead () != 'I')
      return (RetVal);
   if (TimedRead () != '_')
      return (RetVal);

   if ((pkt[0] = (CHAR)TimedRead ()) == -1)
      return (RetVal);
   if ((pkt[1] = (CHAR)TimedRead ()) == -1)
      return (RetVal);
   if ((pkt[2] = (CHAR)TimedRead ()) == -1)
      return (RetVal);
   pkt[3] = '\0';

   if (!stricmp (pkt, "ICI"))
      RetVal = EMSI_ICI;
   else if (!stricmp (pkt, "INQ")) {
      if (TimedRead () != 'C')
         return (RetVal);
      if (TimedRead () != '8')
         return (RetVal);
      if (TimedRead () != '1')
         return (RetVal);
      if (TimedRead () != '6')
         return (RetVal);
//      TimedRead ();
      RetVal = EMSI_INQ;
   }
   else if (!stricmp (pkt, "DAT"))
      RetVal = EMSI_DAT;
   else if (!stricmp (pkt, "REQ")) {
      if (TimedRead () != 'A')
         return (RetVal);
      if (TimedRead () != '7')
         return (RetVal);
      if (TimedRead () != '7')
         return (RetVal);
      if (TimedRead () != 'E')
         return (RetVal);
//      TimedRead ();
      RetVal = EMSI_REQ;
   }
   else if (!stricmp (pkt, "NAK")) {
      if (TimedRead () != 'E')
         return (RetVal);
      if (TimedRead () != 'E')
         return (RetVal);
      if (TimedRead () != 'C')
         return (RetVal);
      if (TimedRead () != '3')
         return (RetVal);
//      TimedRead ();
      RetVal = EMSI_NAK;
   }
   else if (!stricmp (pkt, "ACK")) {
      if (TimedRead () == -1)
         return (RetVal);
      if (TimedRead () == -1)
         return (RetVal);
      if (TimedRead () == -1)
         return (RetVal);
      if (TimedRead () == -1)
         return (RetVal);
//      TimedRead ();
      RetVal = EMSI_ACK;
   }
   else if (!stricmp (pkt, "HBT")) {
      if (TimedRead () == -1)
         return (RetVal);
      if (TimedRead () == -1)
         return (RetVal);
      if (TimedRead () == -1)
         return (RetVal);
      if (TimedRead () == -1)
         return (RetVal);
//      TimedRead ();
      RetVal = EMSI_NONE;
   }

   return (RetVal);
}

VOID TDetect::IEMSIReceiver ()
{
   if (Status != NULL)
      Status->SetLine (0, "Receiving IEMSI packet");

   if (ReceiveIEMSIPacket () == TRUE) {
      if (Status != NULL)
         Status->SetLine (0, "Sending IEMSI packet");
      SendIEMSIPacket ();
      ParseIEMSIPacket ();
   }
}

USHORT TDetect::RemoteMailer ()
{
   USHORT key, prev, i, IsEMSI, IsYOOHOO;
   ULONG tout;

   StartCall = time (NULL);

   if (Cfg != NULL)
      strcpy (Inbound, Cfg->NormalInbound);
   if (Com != NULL)
      Com->ClearInbound ();
   tout = TimerSet (400);
   prev = 0;
   Remote = REMOTE_NONE;
   IsYOOHOO = IsEMSI = FALSE;

   if (Status != NULL)
      Status->Clear ();

   for (i = 0; i < 5 && Remote == REMOTE_NONE && AbortSession () == FALSE; i++) {
      Com->SendBytes ((UCHAR *)" \r \r", 4);
      while (!TimeUp (tout) && AbortSession () == FALSE) {
         if (Com->BytesReady () == TRUE) {
            key = Com->ReadByte ();
            switch (key) {
               case '*':
                  if (prev == '*') {
                     switch (CheckEMSIPacket ()) {
                        case EMSI_REQ:
                           if (IsYOOHOO == FALSE && Cfg->EMSI == TRUE) {
                              Com->SendBytes ((UCHAR *)"**EMSI_INQC816\r", 15);
                              Remote = REMOTE_MAILER;
                              EMSISender ();
                              IsEMSI = TRUE;
                           }
                           break;
                     }
                  }
                  break;

               case ENQ:
                  if (IsEMSI == FALSE && Cfg->WaZoo == TRUE) {
                     Remote = REMOTE_MAILER;
                     WaZOOSender ();
                     IsYOOHOO = TRUE;
                  }
                  break;
            }
            prev = key;
         }

#if defined(__OS2__)
         DosSleep (1L);
#elif defined(__NT__)
         Sleep (1L);
#endif
      }

      if (Remote == REMOTE_NONE && AbortSession () == FALSE && Com != NULL) {
         if (Cfg->EMSI == TRUE)
            Com->BufferBytes ((UCHAR *)"\r**EMSI_INQC816\r**EMSI_INQC816\r", 31);
         if (Cfg->WaZoo == TRUE)
            Com->BufferByte (YOOHOO);
         Com->BufferByte (TSYNC);
         Com->UnbufferBytes ();

         tout = TimerSet (400);
      }
   }

   if (Remote == REMOTE_NONE && Log != NULL)
      Log->Write ("*Sensor doesn't report intelligent life");

   return (Remote);
}

USHORT TDetect::ReceiveEMSIPacket ()
{
   FILE *fp;
   SHORT key;
   USHORT RetVal = FALSE, i, len, Crc, OtherCrc;

   if (Status != NULL)
      Status->SetLine (0, "Receiving EMSI Packet");

   Crc = StringCrc16 ("EMSI_DAT", 0);

   if ((fp = fopen ("lastemsi.dat", "wb")) != NULL)
      fprintf (fp, "**EMSI_DAT");

   len = 0;
   for (i = 0; i < 4; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
      if (fp != NULL)
         fputc (key, fp);
      Crc = Crc16 ((UCHAR)key, Crc);
      len <<= 4;
      key = (USHORT)(toupper (key) - 48);
      if (key > 9)
         key = (SHORT)(key - 7);
      len |= key;
   }

   for (i = 0; i < len; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
      if (fp != NULL)
         fputc (key, fp);
      ReceiveEMSI[i] = (CHAR)key;
      Crc = Crc16 ((UCHAR)key, Crc);
   }
   ReceiveEMSI[i] = '\0';

   OtherCrc = 0;
   for (i = 0; i < 4; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
      if (fp != NULL)
         fputc (key, fp);
      OtherCrc <<= 4;
      key = (USHORT)(toupper (key) - 48);
      if (key > 9)
         key = (SHORT)(key - 7);
      OtherCrc |= key;
   }

   TimedRead ();

   if (fp != NULL)
      fclose (fp);

   if (Crc != OtherCrc) {
      Log->Write (":  CRC Error (%08lX / %08lX)", Crc, OtherCrc);
      Com->SendBytes ((UCHAR *)"**EMSI_NAKEEC3\r", 15);
   }
   else {
      Com->SendBytes ((UCHAR *)"**EMSI_ACKA490\r**EMSI_ACKA490\r", 30);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TDetect::ReceiveIEMSIPacket ()
{
   SHORT key;
   USHORT RetVal = TRUE, i, len;
   ULONG Crc, OtherCrc;

   Crc = StringCrc32 ("EMSI_ICI", 0xFFFFFFFFL);

   len = 0;
   for (i = 0; i < 4; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
      Crc = Crc32 ((UCHAR)key, Crc);
      len <<= 4;
      key = (USHORT)(toupper (key) - 48);
      if (key > 9)
         key = (SHORT)(key - 7);
      len |= key;
   }

   for (i = 0; i < len; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
      ReceiveIEMSI[i] = (CHAR)key;
      Crc = Crc32 ((UCHAR)key, Crc);
   }
   ReceiveIEMSI[i] = '\0';

   OtherCrc = 0;
   for (i = 0; i < 8; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
      OtherCrc <<= 4;
      key = (USHORT)(toupper (key) - 48);
      if (key > 9)
         key = (SHORT)(key - 7);
      OtherCrc |= key;
   }

   TimedRead ();

   if (Crc != OtherCrc) {
      Com->SendBytes ((UCHAR *)"**EMSI_NAKEEC3\r", 15);
      RetVal = FALSE;
   }

   return (RetVal);
}

VOID TDetect::SendEMSIPacket ()
{
   CHAR Temp[64], FoundPw = FALSE;
   USHORT Crc, OutPktFiles, OutDataFiles;
   ULONG OutPktBytes, OutDataBytes;
   class TNodes *Nodes;
   class TOutbound *Outbound;

   if (Status != NULL)
      Status->SetLine (0, "Sending EMSI Packet");

   OutPktFiles = OutDataFiles = 0L;
   OutPktBytes = OutDataBytes = 0L;

   strcpy (SendEMSI, "{EMSI}{");
   if (Cfg->MailAddress.First () == TRUE) {
      strcat (SendEMSI, Cfg->MailAddress.String);
      while (Cfg->MailAddress.Next () == TRUE) {
         strcat (SendEMSI, " ");
         strcat (SendEMSI, Cfg->MailAddress.String);
      }
   }
   strcat (SendEMSI, "}{");

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Address.First () == TRUE)
         do {
            if (Nodes->Read (Address) == TRUE) {
               if (Nodes->SessionPwd[0] != '\0') {
                  strcat (SendEMSI, Nodes->SessionPwd);
                  FoundPw = TRUE;
               }
            }
         } while (FoundPw == FALSE && Address.Next () == TRUE);
      delete Nodes;
   }

   if (Cfg->Janus == TRUE)
      strcat (SendEMSI, "}{8N1,PUA}{JAN,ZAP,ZMO,ARC,XMA");
   else
      strcat (SendEMSI, "}{8N1,PUA}{ZAP,ZMO,ARC,XMA");
   if (Events->AllowRequests == FALSE)
      strcat (SendEMSI, ",NRQ");
   strcat (SendEMSI, "}");

#if defined(__OS2__)
   sprintf (Temp, "{%02X}{%s/OS2}{%s}{}{IDENT}{", PRODUCT_ID, NAME, VERSION);
#elif defined(__NT__)
   sprintf (Temp, "{%02X}{%s/NT}{%s}{}{IDENT}{", PRODUCT_ID, NAME, VERSION);
#elif defined(__LINUX__)
   sprintf (Temp, "{%02X}{%s/Linux}{%s}{}{IDENT}{", PRODUCT_ID, NAME, VERSION);
#else
   sprintf (Temp, "{%02X}{%s/DOS}{%s}{}{IDENT}{", PRODUCT_ID, NAME, VERSION);
#endif
   strcat (SendEMSI, Temp);
   sprintf (Temp, "[%s][%s]", Cfg->SystemName, Cfg->Location);
   strcat (SendEMSI, Temp);
   sprintf (Temp, "[%s][%s]", Cfg->SysopName, Cfg->Phone);
   strcat (SendEMSI, Temp);
   sprintf (Temp, "[9600][%s]}", Cfg->NodelistFlags);
   strcat (SendEMSI, Temp);

   sprintf (Temp, "{TRX#}{[%08lX]}", time (NULL));
   strcat (SendEMSI, Temp);

   if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL) {
      if (Cfg->MailAddress.First () == TRUE)
         Outbound->DefaultZone = Cfg->MailAddress.Zone;

      if (Address.First () == TRUE) {
         do {
            Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
         } while (Address.Next () == TRUE);
      }

      if (Outbound->First () == TRUE)
         do {
            if (Outbound->ArcMail == TRUE || Outbound->MailPKT == TRUE) {
               OutPktFiles++;
               OutPktBytes += Outbound->Size;
            }
            else if (Outbound->Poll == FALSE) {
               OutDataFiles++;
               OutDataBytes += Outbound->Size;
            }
         } while (Outbound->Next () == TRUE);

      delete Outbound;
   }

   sprintf (Temp, "{MTX#}{[%u][%lu][%u][%lu]}", OutPktFiles, OutPktBytes, OutDataFiles, OutDataBytes);
   strcat (SendEMSI, Temp);

   Com->BufferBytes ((UCHAR *)"**", 2);

   sprintf (Temp, "EMSI_DAT%04X", strlen (SendEMSI));
   Com->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   Crc = StringCrc16 (Temp, 0);

   Com->BufferBytes ((UCHAR *)SendEMSI, (USHORT)strlen (SendEMSI));
   Crc = StringCrc16 (SendEMSI, Crc);
   Com->UnbufferBytes ();

   Com->ClearInbound ();
   while (Com->BytesReady () == TRUE)
      Com->ReadByte ();

   sprintf (Temp, "%04X\r", Crc);
   Com->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
}

VOID TDetect::SendIEMSIPacket ()
{
   CHAR Temp[64];
   ULONG Crc;

   tzset ();

#if defined(__OS2__)
   sprintf (SendIEMSI, "{%s/OS2,%s,Unregistered}{", NAME, VERSION);
#elif defined(__NT__)
   sprintf (SendIEMSI, "{%s/NT,%s,Unregistered}{", NAME, VERSION);
#elif defined(__LINUX__)
   sprintf (SendIEMSI, "{%s/Linux,%s,Unregistered}{", NAME, VERSION);
#else
   sprintf (SendIEMSI, "{%s/DOS,%s,Unregistered}{", NAME, VERSION);
#endif
   strcat (SendIEMSI, Cfg->SystemName);
   strcat (SendIEMSI, "}{");
   strcat (SendIEMSI, Cfg->Location);
   strcat (SendIEMSI, "}{");
   strcat (SendIEMSI, Cfg->SysopName);
   sprintf (Temp, "}{%08lX}{}{}{}{}", time (NULL));
   strcat (SendIEMSI, Temp);

   Com->BufferBytes ((UCHAR *)"**", 2);

   sprintf (Temp, "EMSI_ISI%04X", strlen (SendIEMSI));
   Com->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   Crc = StringCrc32 (Temp, 0xFFFFFFFFL);

   Com->BufferBytes ((UCHAR *)SendIEMSI, (USHORT)strlen (SendIEMSI));
   Com->UnbufferBytes ();
   Crc = StringCrc32 (SendIEMSI, Crc);
   Com->ClearInbound ();

   sprintf (Temp, "%08lX\r", Crc);
   Com->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
}

USHORT TDetect::EMSIReceiver ()
{
   USHORT RetVal = FALSE;
   SHORT key, prev;
   ULONG tout, tout1;

   if (ReceiveEMSIPacket () == TRUE) {
      Remote = REMOTE_MAILER;
      ParseEMSIPacket ();

      SendEMSIPacket ();

      prev = 0;
      tout = TimerSet (6000);
      tout1 = TimerSet (1000);

      while (!TimeUp (tout) && AbortSession () == FALSE) {
         if (Com->BytesReady () == TRUE) {
            key = Com->ReadByte ();
            switch (key) {
               case '*':
                  if (prev == '*') {
                     switch (CheckEMSIPacket ()) {
                        case EMSI_ACK:
                           RetVal = TRUE;
                           tout = TimerSet (100);
                           tout1 = TimerSet (1000);
                           break;

                        case EMSI_NAK:
                           SendEMSIPacket ();
                           tout1 = TimerSet (1000);
                           break;
                     }
                  }
                  break;
            }
            prev = key;
         }
         if (TimeUp (tout1)) {
            SendEMSIPacket ();
            tout1 = TimerSet (1000);
         }
      }
   }

   if (RetVal == TRUE)
      Receiver ();

   return (RetVal);
}

USHORT TDetect::EMSISender ()
{
   USHORT RetVal = FALSE, MaybeError;
   SHORT key, prev;
   ULONG tout, acktout;

   SendEMSIPacket ();

   prev = 0;
   tout = TimerSet (3000);
   acktout = TimerSet (500);
   MaybeError = FALSE;

   while (!TimeUp (tout) && AbortSession () == FALSE) {
      if (Com->BytesReady () == TRUE) {
         key = Com->ReadByte ();
         switch (key) {
            case '*':
               if (prev == '*') {
                  switch (CheckEMSIPacket ()) {
                     case EMSI_DAT:
                        RetVal = TRUE;
                        tout = TimerSet (100);
                        if (ReceiveEMSIPacket () == TRUE) {
                           Remote = REMOTE_MAILER;
                           ParseEMSIPacket ();
                        }
                        break;

                     case EMSI_REQ:
                        if (MaybeError == TRUE) {
                           Com->SendBytes ((UCHAR *)"**EMSI_INQC816\r", 15);
                           SendEMSIPacket ();
                           acktout = TimerSet (500);
                        }
                        break;

                     case EMSI_NAK:
                        SendEMSIPacket ();
                        acktout = TimerSet (500);
                        break;
                     default:
                        break;
                  }
               }
               break;
         }
         prev = key;
      }
      if (TimeUp (acktout)) {
         acktout = TimerSet (500);
         Com->SendBytes ((UCHAR *)"**EMSI_NAKEEC3\r", 15);
         MaybeError = TRUE;
      }
   }

   if (RetVal == TRUE)
      Sender ();

   return (RetVal);
}

VOID TDetect::Terminal ()
{
   USHORT key, prev, pos, i, gotAnswer, IsEMSI, ripos;
   USHORT canexit, MailOnly;
   CHAR Temp[128], RipTemp[32];
   ULONG tout;
   PSZ Announce = "\rAuto-Sensing Terminal...\r";

   StartCall = time (NULL);
   canexit = FALSE;
   ripos = 0;

   if (Cfg != NULL)
      strcpy (Inbound, Cfg->NormalInbound);
   if (Com != NULL)
      Com->ClearInbound ();
   gotAnswer = FALSE;
   tout = TimerSet (400);
   prev = 0;
   IsEMSI = FALSE;

   if (Cfg->Ansi == NO) {
      Ansi = FALSE;
      gotAnswer = TRUE;
   }
   else if (Cfg->Ansi == YES) {
      Ansi = TRUE;
      gotAnswer = TRUE;
   }
   if (Cfg->UseAvatar == NO) {
      Avatar = FALSE;
      gotAnswer = TRUE;
   }
   else if (Cfg->UseAvatar == YES) {
      Avatar = TRUE;
      gotAnswer = TRUE;
   }

   MailOnly = Events->MailOnly;
   Remote = (MailOnly == TRUE) ? REMOTE_NONE : REMOTE_USER;

   for (i = 0; i < 5 && (gotAnswer == FALSE || i == 0) && AbortSession () == FALSE; i++) {
      while (!TimeUp (tout) && AbortSession () == FALSE) {
         if (Com->BytesReady () == TRUE) {
            key = Com->ReadByte ();
            switch (ripos) {
               case 0:
                  if (key == 'R')
                     RipTemp[ripos++] = (CHAR)key;
                  else
                     ripos = 0;
                  break;
               case 1:
                  if (key == 'I')
                     RipTemp[ripos++] = (CHAR)key;
                  else
                     ripos = 0;
                  break;
               case 2:
                  if (key == 'P')
                     RipTemp[ripos++] = (CHAR)key;
                  else
                     ripos = 0;
                  break;
               case 3:
                  if (key == 'S')
                     RipTemp[ripos++] = (CHAR)key;
                  else
                     ripos = 0;
                  break;
               case 4:
                  if (key == 'C')
                     RipTemp[ripos++] = (CHAR)key;
                  else
                     ripos = 0;
                  break;
               case 5:
                  if (key == 'R')
                     RipTemp[ripos++] = (CHAR)key;
                  else
                     ripos = 0;
                  break;
               case 6:
                  if (key == 'I')
                     RipTemp[ripos++] = (CHAR)key;
                  else
                     ripos = 0;
                  break;
               case 7:
                  if (key == 'P')
                     RipTemp[ripos++] = (CHAR)key;
                  else
                     ripos = 0;
                  break;
               default:
                  if (ripos > 7 && ripos < 14) {
                     if (isdigit (key))
                        RipTemp[ripos++] = (CHAR)key;
                     else
                        ripos = 0;
                     if (ripos == 13) {
                        if (RipTemp[12] == '0' && RipTemp[13] == '0')
                           ;
//                           Max = TRUE;
                        else
                           Rip = TRUE;
                        Ansi = TRUE;
                        Remote = REMOTE_USER;
                        gotAnswer = TRUE;
                     }
                  }
                  break;
            }

            switch (key) {
               case '~': {
                  CHAR Temp[64];

                  if (MailOnly == FALSE && Cfg->EnablePPP == TRUE && Cfg->PPPCmd[0]) {
                     if (Log != NULL)
                        Log->Write ("+Detected PPP connection");

                     sprintf (Temp, Cfg->PPPCmd, Task);
                     if (Log != NULL)
                        Log->Write (":Running %s", Temp);

                     RunExternal (Temp, Cfg->PPPTimeLimit);

                     tout = TimerSet (0L);
                     Remote = REMOTE_PPP;
                     gotAnswer = TRUE;
                  }
                  break;
               }
               case '*':
                  if (prev == '*') {
                     switch (CheckEMSIPacket ()) {
                        case EMSI_ICI:
                           if (MailOnly == FALSE && Cfg->IEMSI == YES) {
                              IEMSIReceiver ();
                              tout = TimerSet (200);
                              Remote = REMOTE_USER;
                              gotAnswer = TRUE;
                           }
                           break;
                        case EMSI_INQ:
                           if (Com != NULL)
                              Com->SendBytes ((UCHAR *)"**EMSI_REQA77E\r", 15);
                           IsEMSI = TRUE;
                           break;
                        case EMSI_DAT:
                           if (Cfg->EMSI == TRUE) {
                              EMSIReceiver ();
                              gotAnswer = TRUE;
                           }
                           break;
                     }
                  }
                  break;

/*
               case 'I':
                  if (prev == 'R')
                     mayberip = TRUE;
                  break;
               case 'P':
                  if (mayberip == TRUE) {
                     Rip = TRUE;
                     Ansi = TRUE;
                     Remote = REMOTE_USER;
                     gotAnswer = TRUE;
                  }
                  break;
*/

               case 0x1B:
                  if (prev == 0x1B) {
                     if (canexit == TRUE) {
                        if (MailOnly == FALSE) {
                           if (Com != NULL)
                              Com->SendBytes ((UCHAR *)"\r\n\r\n", 4);
                           Remote = REMOTE_USER;
                           return;
                        }
                        else {
                           if (Cfg->MailOnly[0] != '\0')
                              sprintf (Temp, "%s\r", Cfg->MailOnly);
                           else
                              sprintf (Temp, "%s\r", "Processing mail only. Please call later.");
                           Com->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           key = 0;
                        }
                     }
                  }
                  if (canexit == FALSE)
                     key = 0;
                  break;

               case '[':
                  if (prev == 0x1B && Cfg->Ansi == AUTO) {
                     Ansi = TRUE;
                     pos = 0;
                     Remote = REMOTE_USER;
                     gotAnswer = TRUE;
                  }
                  break;

               case ';':
                  if (pos == 12) {
                     Avatar = TRUE;
                     gotAnswer = TRUE;
                  }
                  break;

               case YOOHOO:
                  if (IsEMSI == FALSE && Cfg->WaZoo == TRUE) {
                     WaZOOReceiver ();
                     Remote = REMOTE_MAILER;
                     gotAnswer = TRUE;
                  }
                  break;

               default:
//                  mayberip = FALSE;
                  if (isdigit (key)) {
                     pos = (USHORT)(pos * 10);
                     pos += (USHORT)(key - '0');
                  }
                  break;
            }
            prev = key;
         }

         if (gotAnswer == TRUE && MailOnly == TRUE) {
            if (Cfg->MailOnly[0] != '\0')
               sprintf (Temp, "%s\r", Cfg->MailOnly);
            else
               sprintf (Temp, "%s\r", "Processing mail only. Please call later.");
            Com->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            gotAnswer = FALSE;
         }

#if defined(__OS2__)
         DosSleep (1L);
#elif defined(__NT__)
         Sleep (1L);
#endif
      }

      if (gotAnswer == FALSE && AbortSession () == FALSE && Com != NULL) {
         Com->BufferBytes ((UCHAR *)" \r", 2);

         if (Cfg->EMSI == TRUE)
            Com->BufferBytes ((UCHAR *)"\r**EMSI_REQA77E\r", 16);
         if (Cfg->IEMSI == TRUE && MailOnly == FALSE)
            Com->BufferBytes ((UCHAR *)"\r**EMSI_IRQ8E08\r", 16);
         if (MailOnly == FALSE)
            Com->BufferBytes ((UCHAR *)"\x1B[!", 3);
         if (Cfg->Ansi == AUTO && MailOnly == FALSE) {
            Com->BufferBytes ((UCHAR *)"\r\x19\x20\x0B", 4);
            Com->BufferBytes ((UCHAR *)"\x1B[6n", 4);
         }

         Com->BufferBytes ((UCHAR *)Announce, (USHORT)strlen (Announce));
         if (MailOnly == TRUE) {
            if (Cfg->MailOnly[0] != '\0')
               sprintf (Temp, "%s\r", Cfg->MailOnly);
            else
               sprintf (Temp, "%s\r", "Processing mail only. Please call later.");
            Com->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (Cfg->EnterBBS[0] != '\0') {
            sprintf (Temp, "%s\r", Cfg->EnterBBS);
            Com->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         Com->UnbufferBytes ();

         tout = TimerSet (400);
         canexit = TRUE;
      }
   }

   if (AbortSession () == FALSE && Com != NULL) {
      if (MailOnly == FALSE)
         Com->SendBytes ((UCHAR *)"\r\n\r\n", 4);
   }
}

SHORT TDetect::TimedRead ()
{
   SHORT RetVal = -1;
   ULONG tout;

   if (Com->BytesReady () == FALSE) {
      tout = TimerSet (100);
      do {
         if (Com->BytesReady () == TRUE) {
            RetVal = (SHORT)Com->ReadByte ();
            break;
         }
#if defined(__OS2__)
         DosSleep (1L);
#elif defined(__NT__)
         Sleep (1L);
#endif
      } while (!TimeUp (tout) && AbortSession () == FALSE && RetVal == -1);
   }
   else
      RetVal = (SHORT)Com->ReadByte ();


   return (RetVal);
}

#define IEMSI_NAME         0
#define IEMSI_ALIAS        1
#define IEMSI_LOCATION     2
#define IEMSI_DATAPHONE    3
#define IEMSI_VOICEPHONE   4
#define IEMSI_PASSWORD     5
#define IEMSI_BIRTHDATE    6
#define IEMSI_CRTDEF       7
#define IEMSI_PROTOCOLS    8
#define IEMSI_CAPS         9
#define IEMSI_REQUESTS     10
#define IEMSI_SOFTWARE     11
#define IEMSI_XLAT         12

#define MAX_IEMSITOKEN     14

#define EMSI_FINGERPRINT   0
#define EMSI_ADDRESS       1
#define EMSI_PASSWORD      2
#define EMSI_LINKCODES     3
#define EMSI_COMPATIBILITY 4
#define EMSI_MAILERCODE    5
#define EMSI_MAILERNAME    6
#define EMSI_MAILERVERSION 7
#define EMSI_MAILERSERIAL  8
#define EMSI_SYSTEMNAME    20
#define EMSI_LOCATION      21
#define EMSI_SYSOPNAME     22
#define EMSI_PHONE         23
#define EMSI_SPEED         24
#define EMSI_FLAGS         25
#define EMSI_TRANX         26
#define EMSI_MAILFILES     27
#define EMSI_MAILBYTES     28
#define EMSI_DATAFILES     29
#define EMSI_DATABYTES     30

#define MAX_EMSITOKEN      40

VOID TDetect::ParseEMSIPacket ()
{
   USHORT i, t, idents, firstEntry;
   UCHAR Byte, *Read, *Write, c, FoundKnown, FoundProt;
   CHAR *Tokens[MAX_EMSITOKEN], Temp[64], *p, List[128];
   class TNodes *Nodes;
   class TOutbound *Outbound;

   for (i = 0; i < MAX_EMSITOKEN; i++)
      Tokens[i] = NULL;

   Read = Write = (UCHAR *)ReceiveEMSI;
   i = EMSI_FINGERPRINT;
   Capabilities = 0;

   while (*Read) {
      if (*Read == '\\') {
         Read++;
         if (*Read != '\\') {
            Byte = (UCHAR)(*Read - 48);
            if (Byte > 9)
               Byte = (UCHAR)(Byte - 7);
            Byte <<= 4;
            Read++;
            c = (UCHAR)(*Read - 48);
            if (c > 9)
               c = (UCHAR)(c - 7);
            Byte |= c;
            *Write++ = Byte;
         }
         else
            *Write++ = *Read;
      }
      else {
         *Write = *Read;
         if (*Write == '{') {
            if (i < MAX_EMSITOKEN)
               Tokens[i++] = (CHAR *)(Write + 1);
         }
         else if (*Write == '}')
            *Write = '\0';
         Write++;
      }

      Read++;
   }

   Tokens[i] = NULL;

   if (strstr (Tokens[EMSI_COMPATIBILITY], "DZA") != NULL || strstr (Tokens[EMSI_COMPATIBILITY], "ZMO") != NULL)
      Capabilities |= ZED_ZIPPER;
   if (strstr (Tokens[EMSI_COMPATIBILITY], "ZAP") != NULL)
      Capabilities |= ZED_ZAPPER;
   if (strstr (Tokens[EMSI_COMPATIBILITY], "JAN") != NULL)
      Capabilities |= DOES_IANUS;

   if ((p = strtok (Tokens[EMSI_ADDRESS], " ,")) != NULL)
      do {
         Address.Add (p);
      } while ((p = strtok (NULL, " ,")) != NULL);

   idents = i;
   for (t = 0; t < idents; t++) {
      if (!stricmp (Tokens[t], "IDENT") || !stricmp (Tokens[t], "TRX#") || !stricmp (Tokens[t], "MTX#")) {
         if ((Read = (UCHAR *)Tokens[t + 1]) != NULL) {
            i = MAX_EMSITOKEN;
            strcpy (Temp, Tokens[t]);
            if (!stricmp (Tokens[t], "IDENT"))
               i = EMSI_SYSTEMNAME;
            else if (!stricmp (Tokens[t], "TRX#"))
               i = EMSI_TRANX;
            else if (!stricmp (Tokens[t], "MTX#"))
               i = EMSI_MAILFILES;

            while (*Read) {
               if (*Read == '[') {
                  if (i < MAX_EMSITOKEN)
                     Tokens[i++] = (CHAR *)(Read + 1);
               }
               else if (*Read == ']')
                  *Read = '\0';
               Read++;
            }

            if (!stricmp (Temp, "IDENT")) {
               if (strlen (Tokens[EMSI_SYSTEMNAME]) > sizeof (RemoteSystem) - 1)
                  Tokens[EMSI_SYSTEMNAME][sizeof (RemoteSystem) - 1] = '\0';
               strcpy (RemoteSystem, Tokens[EMSI_SYSTEMNAME]);
               if (strlen (Tokens[EMSI_SYSOPNAME]) > sizeof (RemoteSysop) - 1)
                  Tokens[EMSI_SYSOPNAME][sizeof (RemoteSysop) - 1] = '\0';
               strcpy (RemoteSysop, Tokens[EMSI_SYSOPNAME]);
               if (strlen (Tokens[EMSI_LOCATION]) > sizeof (RemoteLocation) - 1)
                  Tokens[EMSI_LOCATION][sizeof (RemoteLocation) - 1] = '\0';
               strcpy (RemoteLocation, Tokens[EMSI_LOCATION]);
               if (Tokens[EMSI_MAILERSERIAL][0] != '\0')
                  sprintf (RemoteProgram, "%s %s/%s", Tokens[EMSI_MAILERNAME], Tokens[EMSI_MAILERVERSION], Tokens[EMSI_MAILERSERIAL]);
               else
                  sprintf (RemoteProgram, "%s %s", Tokens[EMSI_MAILERNAME], Tokens[EMSI_MAILERVERSION]);
            }
            else if (!stricmp (Temp, "MTX#")) {
               if (MailerStatus != NULL) {
                  MailerStatus->InPktFiles = (USHORT)atoi (Tokens[EMSI_MAILFILES]);
                  MailerStatus->InPktBytes = (ULONG)atol (Tokens[EMSI_MAILBYTES]);
                  MailerStatus->InDataFiles = (USHORT)atoi (Tokens[EMSI_DATAFILES]);
                  MailerStatus->InDataBytes = (ULONG)atol (Tokens[EMSI_DATABYTES]);
               }
            }
         }
      }
   }

   if (MailerStatus != NULL) {
      if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL) {
         if (Cfg->MailAddress.First () == TRUE)
            Outbound->DefaultZone = Cfg->MailAddress.Zone;
      }

      if (Address.First () == TRUE) {
         if (Outbound != NULL)
            Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
         strcpy (MailerStatus->Address, Address.String);
         if (Address.Next () == TRUE) {
            MailerStatus->Akas[0] = '\0';
            do {
               if (Outbound != NULL)
                  Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
               sprintf (Temp, "%s ", Address.String);
               if ((strlen (MailerStatus->Akas) + strlen (Temp) + 1) <= sizeof (MailerStatus->Akas))
                  strcat (MailerStatus->Akas, Temp);
            } while (Address.Next () == TRUE);
         }
      }

      if (Outbound != NULL) {
         if (Outbound->First () == TRUE)
            do {
               if (Outbound->ArcMail == TRUE || Outbound->MailPKT == TRUE) {
                  MailerStatus->OutPktFiles++;
                  MailerStatus->OutPktBytes += Outbound->Size;
               }
               else if (Outbound->Poll == FALSE) {
                  MailerStatus->OutDataFiles++;
                  MailerStatus->OutDataBytes += Outbound->Size;
               }
            } while (Outbound->Next () == TRUE);
         delete Outbound;
      }

      if (strlen (RemoteSystem) > sizeof (MailerStatus->SystemName) - 1)
         RemoteSystem[sizeof (MailerStatus->SystemName) - 1] = '\0';
      strcpy (MailerStatus->SystemName, RemoteSystem);
      if (strlen (RemoteSysop) > sizeof (MailerStatus->SysopName) - 1)
         RemoteSysop[sizeof (MailerStatus->SysopName) - 1] = '\0';
      strcpy (MailerStatus->SysopName, RemoteSysop);
      if (strlen (RemoteLocation) > sizeof (MailerStatus->Location) - 1)
         RemoteLocation[sizeof (MailerStatus->Location) - 1] = '\0';
      strcpy (MailerStatus->Location, RemoteLocation);
      strcpy (MailerStatus->Program, RemoteProgram);
      MailerStatus->Speed = Speed;
      MailerStatus->Update ();
   }

   if (Address.First () == TRUE) {
      Log->Write ("*%s (%s)", RemoteSystem, Address.String);
      if (Address.Next () == TRUE) {
         List[0] = '\0';
         firstEntry = TRUE;
         do {
            sprintf (Temp, "%s ", Address.String);
            if ((strlen (List) + strlen (Temp)) > 64) {
               if (firstEntry == TRUE)
                  Log->Write ("*Aka(s): %s", List);
               else
                  Log->Write ("*        %s", List);
               List[0] = '\0';
               firstEntry = FALSE;
            }
            else
               strcat (List, Temp);
         } while (Address.Next () == TRUE);

         if (List[0] != '\0') {
            if (firstEntry == TRUE)
               Log->Write ("*Aka(s): %s", List);
            else
               Log->Write ("*        %s", List);
         }
      }
   }
   else
      Log->Write ("*%s", RemoteSystem);

   Log->Write ("*Remote Uses %s", RemoteProgram);
   Log->Write (":SysOp: %s", RemoteSysop);

   if (Tokens[EMSI_TRANX] != NULL)
      Log->Write (":Tranx: %08lX / %s", time (NULL), Tokens[EMSI_TRANX]);
   else
      Log->Write (":No transaction number presented");

   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      strupr (Tokens[EMSI_PASSWORD]);
      FoundKnown = FoundProt = FALSE;
      if (Address.First () == TRUE)
         do {
/*
            if (Nodes->Read (Address) == FALSE) {
               Nodes->New ();
               strcpy (Nodes->Address, Address.String);
               strcpy (Nodes->SystemName, Tokens[EMSI_SYSTEMNAME]);
               strcpy (Nodes->SysopName, Tokens[EMSI_SYSOPNAME]);
               strcpy (Nodes->Location, Tokens[EMSI_LOCATION]);
               strcpy (Nodes->Flags, Tokens[EMSI_FLAGS]);
               strcpy (Nodes->Phone, Tokens[EMSI_PHONE]);
               Nodes->Speed = atol (Tokens[EMSI_SPEED]);
               strcpy (Nodes->SessionPwd, Tokens[EMSI_PASSWORD]);
               Nodes->Add ();
            }
            else {
*/
            if (Nodes->Read (Address) == TRUE) {
               if (FoundKnown == FALSE) {
                  strcpy (Inbound, Cfg->KnownInbound);
                  FoundKnown = TRUE;
               }
               if (FoundProt == FALSE) {
                  strupr (Nodes->SessionPwd);
                  if (Nodes->SessionPwd[0] != '\0' && !strcmp (Tokens[EMSI_PASSWORD], Nodes->SessionPwd)) {
                     strcpy (Inbound, Cfg->ProtectedInbound);
                     FoundProt = TRUE;
                  }
               }
            }
         } while (Address.Next () == TRUE);
      delete Nodes;

      if (FoundProt == TRUE)
         Log->Write ("*Password-protected session");
   }

   Temp[0] = '\0';
   if (Capabilities & WZ_FREQ)
      strcat (Temp, "FReqs ");
   if (Capabilities & ZED_ZIPPER)
      strcat (Temp, "ZedZIP ");
   if (Capabilities & ZED_ZAPPER)
      strcat (Temp, "ZedZAP ");
   if (Capabilities & DOES_IANUS)
      strcat (Temp, "Janus ");
   Log->Write (":Offer: %s", Temp);
}

#define IEMSI_NAME         0
#define IEMSI_ALIAS        1
#define IEMSI_LOCATION     2
#define IEMSI_DATAPHONE    3
#define IEMSI_VOICEPHONE   4
#define IEMSI_PASSWORD     5
#define IEMSI_BIRTHDATE    6
#define IEMSI_CRTDEF       7
#define IEMSI_PROTOCOLS    8
#define IEMSI_CAPS         9
#define IEMSI_REQUESTS     10
#define IEMSI_SOFTWARE     11
#define IEMSI_XLAT         12

#define MAX_IEMSITOKEN     14

VOID TDetect::ParseIEMSIPacket ()
{
   USHORT i;
   UCHAR Byte, *Read, *Write, c;
   CHAR *Tokens[MAX_IEMSITOKEN];

   IEMSI = TRUE;
   Read = Write = (UCHAR *)ReceiveIEMSI;
   i = 0;

   while (*Read != '\0') {
      if (*Read == '\\') {
         Read++;
         if (*Read != '\\') {
            Byte = (UCHAR)(*Read - 48);
            if (Byte > 9)
               Byte = (UCHAR)(Byte - 7);
            Byte <<= 4;
            Read++;
            c = (UCHAR)(*Read - 48);
            if (c > 9)
               c = (UCHAR)(c - 7);
            Byte |= c;
            *Write++ = Byte;
         }
         else
            *Write++ = *Read;
      }
      else {
         *Write = *Read;
         if (*Write == '{') {
            if (i < MAX_IEMSITOKEN)
               Tokens[i++] = (CHAR *)(Write + 1);
         }
         else if (*Write == '}')
            *Write = '\0';
         Write++;
      }

      Read++;
   }

   Tokens[i] = NULL;

   strupr (Tokens[IEMSI_CRTDEF]);
   if (strstr (Tokens[IEMSI_CRTDEF], "AVT0") != NULL) {
      Ansi = TRUE;
      Avatar = TRUE;
   }
   if (strstr (Tokens[IEMSI_CRTDEF], "ANSI") != NULL)
      Ansi = TRUE;
   if (strstr (Tokens[IEMSI_CRTDEF], "VT52") != NULL)
      Ansi = TRUE;
   if (strstr (Tokens[IEMSI_CRTDEF], "VT100") != NULL)
      Ansi = TRUE;

   strupr (Tokens[IEMSI_CAPS]);
   if (strstr (Tokens[IEMSI_CAPS], "ASCII8") != NULL)
      IBMChars = TRUE;

   strupr (Tokens[IEMSI_REQUESTS]);
   if (strstr (Tokens[IEMSI_REQUESTS], "MORE") != NULL)
      MorePrompt = TRUE;
   if (strstr (Tokens[IEMSI_REQUESTS], "HOT") != NULL)
      HotKeys = TRUE;
   if (strstr (Tokens[IEMSI_REQUESTS], "CLR") != NULL)
      ScreenClear = TRUE;
   if (strstr (Tokens[IEMSI_REQUESTS], "FSED") != NULL)
      FullEd = TRUE;
   if (strstr (Tokens[IEMSI_REQUESTS], "MAIL") != NULL)
      MailCheck = TRUE;
   if (strstr (Tokens[IEMSI_REQUESTS], "FILE") != NULL)
      FileCheck = TRUE;

   strcpy (Name, Tokens[IEMSI_NAME]);
   strcpy (RealName, Tokens[IEMSI_ALIAS]);
   strcpy (City, Tokens[IEMSI_LOCATION]);
   strcpy (Password, Tokens[IEMSI_PASSWORD]);
}

USHORT TDetect::ReceiveHello ()
{
   USHORT RetVal = FALSE, Crc, IsPacket, FoundKnown, FoundProt;
   USHORT Received, RemoteCrc, CrcCount;
   UCHAR c, *p;
   CHAR Temp[40];
   LONG Timer, Retry;
   HELLO Hello;
   class TNodes *Nodes;
   class TOutbound *Outbound;

   p = (UCHAR *)&Hello;
   IsPacket = FALSE;
   CrcCount = Received = 0;
   RemoteCrc = 0xFFFFU;
   Crc = 0;

   Timer = TimerSet (4000);

   do {
      Com->SendByte (ENQ);
      Retry = TimerSet (800);
      do {
         if (Com->BytesReady () == TRUE) {
            c = (UCHAR)Com->ReadByte ();
            if (IsPacket == FALSE && c == 0x1F) {
               p = (UCHAR *)&Hello;
               IsPacket = TRUE;
               CrcCount = Received = 0;
               RemoteCrc = 0xFFFFU;
               Crc = 0;
               Retry = TimerSet (800);
            }
            else {
               if (Received < 128) {
                  *p++ = c;
                  Received++;
                  Crc = Crc16 (c, Crc);
               }
               else if (CrcCount < 2) {
                  RemoteCrc <<= 8;
                  RemoteCrc |= c;
                  CrcCount++;
               }
            }
         }
      } while (!TimeUp (Retry) && CrcCount < 2 && Com->Carrier () == TRUE);
   } while (AbortSession () == FALSE && !TimeUp (Timer) && Crc != RemoteCrc);

   if (AbortSession () == FALSE && Crc == RemoteCrc) {
      Address.Add (Hello.Zone, Hello.Net, Hello.Node, Hello.Point);
      Address.First ();

      if (MailerStatus != NULL) {
         if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL) {
            if (Cfg->MailAddress.First () == TRUE)
               Outbound->DefaultZone = Cfg->MailAddress.Zone;

            Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
            if (Outbound->First () == TRUE)
               do {
                  if (Outbound->ArcMail == TRUE || Outbound->MailPKT == TRUE) {
                     MailerStatus->OutPktFiles++;
                     MailerStatus->OutPktBytes += Outbound->Size;
                  }
                  else if (Outbound->Poll == FALSE) {
                     MailerStatus->OutDataFiles++;
                     MailerStatus->OutDataBytes += Outbound->Size;
                  }
               } while (Outbound->Next () == TRUE);
            delete Outbound;
         }
         strcpy (MailerStatus->Address, Address.String);
         MailerStatus->Akas[0] = '\0';
         strcpy (MailerStatus->SystemName, Hello.Name);
         strcpy (MailerStatus->SysopName, Hello.Sysop);
         if (Hello.Product == isOPUS)
            sprintf (MailerStatus->Program, "Opus Version %d.%02d", Hello.ProductMaj, (Hello.ProductMin == 48) ? 0 : Hello.ProductMin);
         else if (Hello.Product <= isMAX_PRODUCT)
            sprintf (MailerStatus->Program, "%s Version %d.%02d", ProductNames[Hello.Product], Hello.ProductMaj, Hello.ProductMin);
         else
            sprintf (MailerStatus->Program, "Program '%02x' Version %d.%02d", Hello.Product, Hello.ProductMaj, Hello.ProductMin);
         MailerStatus->Speed = Speed;

         if (Hello.Product == isLORA) {
            MailerStatus->InPktFiles = Hello.PktFiles;
            MailerStatus->InDataFiles = Hello.DataFiles;
            MailerStatus->InPktBytes = Hello.PktBytes;
            MailerStatus->InDataBytes = Hello.DataBytes;
         }

         MailerStatus->Update ();
      }

      Log->Write ("*%s (%s)", Hello.Name, Address.String);
      if (Hello.Product == isOPUS)
         Log->Write ("*Remote Uses Opus Version %d.%02d", Hello.ProductMaj, (Hello.ProductMin == 48) ? 0 : Hello.ProductMin);
      else if (Hello.Product <= isMAX_PRODUCT)
         Log->Write ("*Remote Uses %s Version %d.%02d", ProductNames[Hello.Product], Hello.ProductMaj, Hello.ProductMin);
      else
         Log->Write ("*Remote Uses Program '%02x' Version %d.%02d", Hello.Product, Hello.ProductMaj, Hello.ProductMin);
      Log->Write (":SysOp: %s", strcpy (RemoteSysop, Hello.Sysop));

      if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
         FoundKnown = FoundProt = FALSE;
         if (Address.First () == TRUE)
            do {
/*
               if (Nodes->Read (Address) == FALSE) {
                  Nodes->New ();
                  strcpy (Nodes->Address, Address.String);
                  strcpy (Nodes->SystemName, Hello.Name);
                  strcpy (Nodes->SysopName, Hello.Sysop);
                  strcpy (Nodes->SessionPwd, Hello.Password);
                  Nodes->Add ();
               }
               else {
*/
               if (Nodes->Read (Address) == TRUE) {
                  if (FoundKnown == FALSE) {
                     strcpy (Inbound, Cfg->KnownInbound);
                     FoundKnown = TRUE;
                  }
                  if (FoundProt == FALSE) {
                     if (Nodes->SessionPwd[0] != '\0' && !stricmp (Hello.Password, Nodes->SessionPwd)) {
                        strcpy (Inbound, Cfg->ProtectedInbound);
                        FoundProt = TRUE;
                     }
                  }
               }
            } while (Address.Next () == TRUE);
         delete Nodes;
      }

      Temp[0] = '\0';
      Capabilities = (USHORT)(Hello.Capabilities|Y_DIETIFNA);
      if (Capabilities & WZ_FREQ)
         strcat (Temp, "FReqs ");
      if (Capabilities & ZED_ZIPPER)
         strcat (Temp, "ZedZIP ");
      if (Capabilities & ZED_ZAPPER)
         strcat (Temp, "ZedZAP ");
      if (Capabilities & DOES_IANUS)
         strcat (Temp, "Janus ");
      Log->Write (":Offer: %s", Temp);

      if (Hello.Product == isLORA) {
         if (Hello.Tranx) {
            if (Log != NULL)
               Log->Write (":Tranx: %08lX / %08lX", time (NULL), Hello.Tranx);
         }
         else {
            if (Log != NULL)
               Log->Write (":No transaction number presented");
         }
      }
      else {
         if (Log != NULL)
            Log->Write (":No transaction number presented");
      }

      Com->SendByte (ACK);
      Com->SendByte (YOOHOO);

      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TDetect::Receiver ()
{
   FILE *fp;
   USHORT FileReceived, FileSent, RetVal, RequestFiles;
   CHAR FinalPhase = FALSE, *p, Stop, Temp[128], Name[32], *rname;
   ULONG ReceivedSize, SentSize, Length, RequestSize;
   struct stat statbuf;
   class TTransfer *Transfer;
#if !defined(__LINUX__)
   class TJanus *Janus;
#endif
   class TOutbound *Outbound;
#if !defined(__POINT__)
   class TFileBase *File;
   class TOkFile *OkFile;
#endif
   class TCollection Request;
   class PACKET *Packet;

   if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL) {
      if (Cfg->MailAddress.First () == TRUE)
         Outbound->DefaultZone = Cfg->MailAddress.Zone;

      FileSent = FileReceived = 0;
      SentSize = ReceivedSize = 0L;

      if (Cfg->Janus == TRUE && (Capabilities & DOES_IANUS)) {
         Log->Write (":Session method: %s", "Janus");

#if !defined(__LINUX__)
         if ((Janus = new TJanus ()) != NULL) {
            Janus->Com = Com;
            Janus->Log = Log;
            Janus->Speed = Speed;
            Janus->AllowRequests = Events->AllowRequests;
            strcpy (Janus->RxPath, Inbound);

            if (AbortSession () == FALSE) {
               Stop = FALSE;
               if (Address.First () == TRUE)
                  do {
                     Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
                  } while (Address.Next () == TRUE);

               Janus->Outbound = Outbound;
               Janus->Transfer ();
            }

            delete Janus;
         }
#endif
      }
      else if (Capabilities & (ZED_ZIPPER|ZED_ZAPPER)) {
         Log->Write (":Session method: %s", "ZedZap");

         if ((Transfer = new TTransfer ()) != NULL) {
            Transfer->Com = Com;
            Transfer->Log = Log;
            Transfer->Progress = Progress;
            Transfer->Speed = Speed;
            Transfer->Telnet = Cfg->ZModemTelnet;
// ----------------------------------------------------------------------
// The first phase of a ZedZap/ZedZip transfer in answering mode is
// the receipt of the files sent by the remote.
// ----------------------------------------------------------------------
            Request.Clear ();
            if ((rname = Transfer->ReceiveZModem (Inbound)) != NULL)
               do {
                  FileReceived++;
                  if (stat (rname, &statbuf) == 0)
                     ReceivedSize += statbuf.st_size;
#if !defined(__LINUX__)
                  strlwr (rname);
#endif
                  if (strstr (rname, ".req") != NULL) {
                     if (Events->AllowRequests == FALSE)
                        unlink (rname);
                     if ((fp = fopen (rname, "rt")) != NULL) {
                        while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                           if ((p = strchr (Temp, '\r')) != NULL)
                              *p = '\0';
                           if ((p = strchr (Temp, '\n')) != NULL)
                              *p = '\0';
                           Request.Add (Temp, (USHORT)(strlen (Temp) + 1));
                        }
                        fclose (fp);
                        unlink (rname);
                     }
                  }
               } while ((rname = Transfer->ReceiveZModem (Inbound)) != NULL && AbortSession () == FALSE);

            if (AbortSession () == FALSE) {
// ----------------------------------------------------------------------
// Second phase, now it is our turn to send our files to the remote
// system.
// ----------------------------------------------------------------------
               Stop = FALSE;
               if (Address.First () == TRUE) {
                  do {
                     Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
                     if (Outbound->First () == TRUE) {
                        do {
                           if (Outbound->Poll == FALSE) {
                              if (Outbound->MailPKT == TRUE) {
                                 while (time (NULL) == LastPktName)
                                    ;
                                 sprintf (Name, "%08lx.pkt", time (NULL));
                              }
                              else
                                 strcpy (Name, Outbound->Name);
                              if (Capabilities & ZED_ZAPPER)
                                 RetVal = Transfer->SendZModem8K (Outbound->Complete, Name);
                              else
                                 RetVal = Transfer->SendZModem (Outbound->Complete, Name);

                              if (RetVal == FALSE)
                                 Stop = TRUE;
                              else {
                                 FileSent++;
                                 SentSize += Outbound->Size;
                                 if (Outbound->Request == TRUE)
                                    FinalPhase = TRUE;
                                 Outbound->Remove ();
                              }
                           }
                           else
                              Outbound->Remove ();
                        } while (Stop == FALSE && Outbound->First () == TRUE);
                        Outbound->Update ();
                     }
                     else
                        Log->Write ("+Nothing to send to %s", Address.String);
                  } while (Stop == FALSE && Address.Next () == TRUE && AbortSession () == FALSE);
               }

#if !defined(__POINT__)
               if ((File = new TFileBase (Cfg->SystemPath, "")) != NULL) {
                  if ((rname = (CHAR *)Request.First ()) != NULL) {
                     Log->Write ("+Processing file requests");
                     RequestFiles = 0;
                     RequestSize = 0L;

                     // Crea il pkt contenente il messaggio riassuntivo del file-request
                     if ((Packet = new PACKET) != NULL) {
                        if (Address.First () == TRUE)
                           strcpy (Packet->ToAddress, Address.String);
                        if (Cfg->MailAddress.First () == TRUE)
                           strcpy (Packet->FromAddress, Cfg->MailAddress.String);
                        sprintf (Name, "%08lx.pkt", time (NULL));

                        Packet->New ();
                        if (Packet->Open (Name) == TRUE) {
                           sprintf (Packet->From, "%s", NAME);
                           strcpy (Packet->To, "Sysop");
                           strcpy (Packet->Subject, "Report on file-request");

                           sprintf (Temp, "The following is the result of a file-request to %s ...", Packet->FromAddress);
                           Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
//kj                           Packet->Text.Add ("", 1);
                           Packet->Text.Add ("");
                           p = "Filename        Bytes   Description";
                           Packet->Text.Add (p, (USHORT)(strlen (p) + 1));
                           p = "--------      -------   -----------";
                           Packet->Text.Add (p, (USHORT)(strlen (p) + 1));
                        }
                     }

                     do {
                        RetVal = TRUE;
                        if (File->First (rname) == TRUE) {
                           do {
                              if (stat (File->Complete, &statbuf) == 0) {
                                 if (Capabilities & ZED_ZAPPER)
                                    RetVal = Transfer->SendZModem8K (File->Complete, File->Name);
                                 else
                                    RetVal = Transfer->SendZModem (File->Complete, File->Name);
                                 if (RetVal == TRUE) {
                                    File->DlTimes++;
                                    File->ReplaceHeader ();
                                    RequestFiles++;
                                    RequestSize += File->Size;

                                    // Aggiunge la descrizione al messaggio riassuntivo
                                    if (Packet != NULL) {
                                       if ((p = (CHAR *)File->Description->First ()) == NULL)
                                          p = "";
                                       sprintf (Temp, "%-12s %8lu   %s", File->Name, File->Size, p);
                                       Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                                       while ((p = (CHAR *)File->Description->Next ()) != NULL)
                                          Packet->Text.Add (p, (USHORT)(strlen (p) + 1));
                                    }
                                 }
                              }
                              else if (Packet != NULL) {
                                 sprintf (Temp, "%-12s            %s", File->Name, "* Removed from filebase *");
                                 Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                              }
                           } while (RetVal == TRUE && File->Next (rname) == TRUE);
                        }
                        else if (Packet != NULL) {
                           // Il file non e' stato trovato nel database dei files, adesso
                           // verifica se fa parte dei magic names.
                           if ((OkFile = new TOkFile (Cfg->SystemPath)) != NULL) {
                              if (OkFile->Read (rname) == TRUE) {
                                 if (stat (OkFile->Path, &statbuf) == 0) {
                                    if (Capabilities & ZED_ZAPPER)
                                       RetVal = Transfer->SendZModem8K (OkFile->Path);
                                    else
                                       RetVal = Transfer->SendZModem (OkFile->Path);
                                    if (RetVal == TRUE) {
                                       if (Packet != NULL) {
                                          sprintf (Temp, "%-12s %8lu   %s", OkFile->Name, statbuf.st_size, "");
                                          Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                                       }
                                    }
                                 }
                              }
                              else {
                                 sprintf (Temp, "%-12s            %s", rname, "* Not found *");
                                 Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                              }
                              delete OkFile;
                           }
                        }
                     } while ((rname = (CHAR *)Request.Next ()) != NULL && RetVal == TRUE && AbortSession () == FALSE);

                     if (Packet != NULL) {
                        p = "              -------";
                        Packet->Text.Add (p, (USHORT)(strlen (p) + 1));
                        sprintf (Temp, "Total        %8lu", RequestSize);
                        Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));

                        // In fondo al messaggio viene appeso il nome del sysop,
                        // l'indirizzo primario e il nome del BBS.
//kj                        Packet->Text.Add ("", 1);
                        Packet->Text.Add ("");
                        sprintf (Temp, "%s, %s", Cfg->SysopName, Cfg->MailAddress.String);
                        Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
                        sprintf (Temp, "%s", Cfg->SystemName);
                        Packet->Text.Add (Temp, (USHORT)(strlen (Temp) + 1));

                        Packet->Add (Packet->Text);
                        delete Packet;

                        if (AbortSession () == FALSE && RetVal == TRUE) {
                           if (Capabilities & ZED_ZAPPER)
                              RetVal = Transfer->SendZModem8K (Name, Name);
                           else
                              RetVal = Transfer->SendZModem (Name, Name);
                        }
                        unlink (Name);
                     }
                  }
                  delete File;

                  Transfer->SendZModem (NULL);
               }
#endif
               Request.Clear ();
            }

            if (FinalPhase == TRUE) {
               if ((p = Transfer->ReceiveZModem (Inbound)) != NULL)
                  do {
                     FileReceived++;
                     if (stat (p, &statbuf) == 0)
                        ReceivedSize += statbuf.st_size;
                  } while ((p = Transfer->ReceiveZModem (Inbound)) != NULL && AbortSession () == FALSE);
            }
            delete Transfer;
         }
      }

      delete Outbound;
      Length = time (NULL) - StartCall;

      if (FileReceived > 0)
         Remote = REMOTE_MAILRECEIVED;

      Log->Write ("*End of WaZOO/EMSI Session");
      Log->Write (" Received: %u/%lu Sent: %u/%lu Total: %u/%lu", FileReceived, ReceivedSize, FileSent, SentSize, FileReceived + FileSent, ReceivedSize + SentSize);
      Address.First ();
      if (Length < 60)
         Log->Write ("*Seconds: %lu  System: %s", Length, Address.String);
      else
         Log->Write ("*Minutes: %lu:%02lu  System: %s", Length / 60L, Length % 60L, Address.String);
   }
}

VOID TDetect::Sender ()
{
   USHORT FileReceived, FileSent, RetVal;
   CHAR *p, Stop, Name[32];
   ULONG ReceivedSize, SentSize, Length;
   struct stat statbuf;
   class TTransfer *Transfer;
   class TJanus *Janus;
   class TOutbound *Outbound;

   if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL) {
      if (Cfg->MailAddress.First () == TRUE)
         Outbound->DefaultZone = Cfg->MailAddress.Zone;

      FileSent = FileReceived = 0;
      SentSize = ReceivedSize = 0L;

      if (Cfg->Janus == TRUE && (Capabilities & DOES_IANUS)) {
         Log->Write (":Session method: %s", "Janus");

         if ((Janus = new TJanus ()) != NULL) {
            Janus->Com = Com;
            Janus->Log = Log;
            Janus->Speed = Speed;
            Janus->MakeRequests = Events->MakeRequests;
            strcpy (Janus->RxPath, Inbound);

            if (AbortSession () == FALSE) {
               Stop = FALSE;
               if (Address.First () == TRUE)
                  do {
                     Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
                     Outbound->ClearAttempt (Address.String);
                  } while (Address.Next () == TRUE);

               Janus->Outbound = Outbound;
               Janus->Transfer ();
            }

            delete Janus;
         }
      }
      else if (Capabilities & (ZED_ZIPPER|ZED_ZAPPER)) {
         Log->Write (":Session method: %s", "ZedZap");

         if ((Transfer = new TTransfer ()) != NULL) {
            Transfer->Com = Com;
            Transfer->Log = Log;
            Transfer->Progress = Progress;
            Transfer->Speed = Speed;
            Transfer->Telnet = Cfg->ZModemTelnet;

            if (AbortSession () == FALSE) {
               Stop = FALSE;
               if (Address.First () == TRUE) {
                  do {
                     Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
                     Outbound->ClearAttempt (Address.String);
                     if (Outbound->First () == TRUE) {
                        do {
                           if (Outbound->Poll == FALSE && (Outbound->Request == FALSE || Events->MakeRequests == TRUE)) {
                              if (Outbound->MailPKT == TRUE) {
                                 while (time (NULL) == LastPktName)
                                    ;
                                 sprintf (Name, "%08lx.pkt", time (NULL));
                              }
                              else
                                 strcpy (Name, Outbound->Name);
                              if (Capabilities & ZED_ZAPPER)
                                 RetVal = Transfer->SendZModem8K (Outbound->Complete, Name);
                              else
                                 RetVal = Transfer->SendZModem (Outbound->Complete, Name);

                              if (RetVal == FALSE)
                                 Stop = TRUE;
                              else {
                                 FileSent++;
                                 SentSize += Outbound->Size;
                                 Outbound->Remove ();
                              }
                           }
                           else if (Outbound->Poll == TRUE)
                              Outbound->Remove ();
                        } while (Stop == FALSE && Outbound->First () == TRUE);
                        Outbound->Update ();
                     }
                     else
                        Log->Write ("+Nothing to send to %s", Address.String);
                  } while (Stop == FALSE && Address.Next () == TRUE);
               }
               Transfer->SendZModem (NULL);
            }

            if ((p = Transfer->ReceiveZModem (Inbound)) != NULL)
               do {
                  FileReceived++;
                  if (stat (p, &statbuf) == 0)
                     ReceivedSize += statbuf.st_size;
               } while ((p = Transfer->ReceiveZModem (Inbound)) != NULL);

            if (AbortSession () == FALSE)
               Transfer->SendZModem (NULL);

            delete Transfer;
         }
      }

      delete Outbound;
      Length = time (NULL) - StartCall;

      if (FileReceived > 0)
         Remote = REMOTE_MAILRECEIVED;

      Log->Write ("*End of WaZOO/EMSI Session");
      Log->Write (" Received: %u/%lu Sent: %u/%lu Total: %u/%lu", FileReceived, ReceivedSize, FileSent, SentSize, FileReceived + FileSent, ReceivedSize + SentSize);
      Address.First ();
      if (Length < 60)
         Log->Write ("*Seconds: %lu  System: %s", Length, Address.String);
      else
         Log->Write ("*Minutes: %lu:%02lu  System: %s", Length / 60L, Length % 60L, Address.String);
   }
}

USHORT TDetect::SendHello ()
{
   USHORT i, RetVal = FALSE, Crc, FoundPw, OutPktFiles, OutDataFiles;
   UCHAR *p, Resp = 0;
   ULONG Timer, Retry, OutPktBytes, OutDataBytes;
   HELLO Hello;
   class TNodes *Nodes;
   class TOutbound *Outbound;
   class TEvents *Events;

   OutPktFiles = OutDataFiles = 0;
   OutPktBytes = OutDataBytes = 0L;

   memset (&Hello, 0, sizeof (Hello));
   Hello.Signal = 'o';
   Hello.Version =  1;
   Hello.Product = PRODUCT_ID;
   Hello.ProductMaj = VER_MAJOR;
   Hello.ProductMin = VER_MINOR;
   strcpy (Hello.Name, Cfg->SystemName);
   strcpy (Hello.Sysop, Cfg->SysopName);

   FoundPw = FALSE;
   if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
      if (Address.First () == TRUE)
         do {
            if (Nodes->Read (Address) == TRUE) {
               if (Nodes->SessionPwd[0] != '\0') {
                  strcpy (Hello.Password, Nodes->SessionPwd);
                  FoundPw = TRUE;
               }
            }
         } while (FoundPw == FALSE && Address.Next () == TRUE);
      delete Nodes;
   }

   if (Cfg->MailAddress.First () == TRUE) {
      Hello.Zone = Cfg->MailAddress.Zone;
      Hello.Net = Cfg->MailAddress.Net;
      Hello.Node = Cfg->MailAddress.Node;
      Hello.Point = Cfg->MailAddress.Point;
   }
   Hello.Capabilities |= Y_DIETIFNA;
   Hello.Capabilities |= ZED_ZAPPER|ZED_ZIPPER;
   if (Cfg->Janus == TRUE)
      Hello.Capabilities |= DOES_IANUS;

#if !defined(__POINT__)
   if ((Events = new TEvents (Cfg->SchedulerFile)) != NULL) {
      Events->Load ();
      Events->SetCurrent ();
      if (Events->AllowRequests == TRUE)
         Hello.Capabilities |= WZ_FREQ;
      delete Events;
   }
#endif

   Hello.Tranx = time (NULL);

   if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL) {
      if (Cfg->MailAddress.First () == TRUE)
         Outbound->DefaultZone = Cfg->MailAddress.Zone;

      if (Address.First () == TRUE) {
         do {
            Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
         } while (Address.Next () == TRUE);
      }

      if (Outbound->First () == TRUE)
         do {
            if (Outbound->ArcMail == TRUE || Outbound->MailPKT == TRUE) {
               OutPktFiles++;
               OutPktBytes += Outbound->Size;
            }
            else if (Outbound->Poll == FALSE) {
               OutDataFiles++;
               OutDataBytes += Outbound->Size;
            }
         } while (Outbound->Next () == TRUE);

      delete Outbound;
   }

   Timer = TimerSet (4000);

   do {
      if (Com->Carrier () == TRUE) {
         Com->BufferByte (0x1F);
         Com->BufferBytes ((UCHAR *)&Hello, sizeof (Hello));
         p = (UCHAR *)&Hello;
         for (i = 0, Crc = 0; i < sizeof (Hello); i++)
            Crc = Crc16 (*p++, Crc);
         Com->UnbufferBytes ();
         Com->SendByte ((UCHAR)(Crc >> 8));
         Com->SendByte ((UCHAR)(Crc & 0xFF));

         Resp = 0;
         Retry = TimerSet (800);
         do {
            if (Com->BytesReady () == TRUE)
               Resp = Com->ReadByte ();
         } while (Com->Carrier () == TRUE && Resp != ACK && Resp != ENQ && Resp != '?' && !TimeUp (Retry));
      }
   } while (Com->Carrier () == TRUE && Resp != ACK && !TimeUp (Timer));

   if (Resp == ACK)
      RetVal = TRUE;

   return (RetVal);
}

USHORT TDetect::WaZOOReceiver ()
{
   USHORT RetVal = FALSE;
   ULONG Timer;

   if (Status != NULL)
      Status->SetLine (0, "YooHoo");

   if (ReceiveHello () == TRUE) {
      if (Status != NULL)
         Status->SetLine (0, "YooHoo/2U2");
      Remote = REMOTE_MAILER;
      Timer = TimerSet (500);
      do {
         if (Com->BytesReady () == TRUE) {
            if (Com->ReadByte () == ENQ)
               RetVal = SendHello ();
         }
      } while (RetVal == FALSE && AbortSession () == FALSE && !TimeUp (Timer));
   }

   if (RetVal == TRUE)
      Receiver ();

   return (RetVal);
}

USHORT TDetect::WaZOOSender ()
{
   USHORT RetVal = FALSE;

   if (Status != NULL)
      Status->SetLine (0, "YooHoo");

   if (SendHello () == TRUE) {
      if (Status != NULL)
         Status->SetLine (0, "YooHoo/2U2");
      if (ReceiveHello () == TRUE) {
         Remote = REMOTE_MAILER;
         RetVal = TRUE;
      }
   }

   if (RetVal == TRUE)
      Sender ();

   return (RetVal);
}

// ----------------------------------------------------------------------

TMailerStatus::TMailerStatus ()
{
   SystemName[0] = SysopName[0] = Location[0] = '\0';
   Address[0] = Akas[0] = Program[0] = '\0';
   InPktFiles = InDataFiles = 0xFFFFU;
   OutPktFiles = OutDataFiles = 0;
   InPktBytes = InDataBytes = OutPktBytes = OutDataBytes = 0L;
   Speed = 57600L;
}

TMailerStatus::~TMailerStatus ()
{
}

VOID TMailerStatus::Update ()
{
}

