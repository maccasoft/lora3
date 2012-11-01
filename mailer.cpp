
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.06
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "mailer.h"
#include "nodes.h"
#include "version.h"

#define EMSI_FINGERPRINT   0
#define EMSI_ADDRESS       1
#define EMSI_PASSWORD      2
#define EMSI_LINKCODES     3
#define EMSI_COMPATIBILITY 4
#define EMSI_MAILERCODE    5
#define EMSI_MAILERNAME    6
#define EMSI_MAILERVERSION 7
#define EMSI_MAILERSERIAL  8
#define EMSI_SYSTEMNAME    9
#define EMSI_LOCATION      10
#define EMSI_SYSOPNAME     11
#define EMSI_PHONE         12
#define EMSI_SPEED         13
#define EMSI_FLAGS         14
#define EMSI_TRANX         15

#define MAX_EMSITOKEN      32

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
   UCHAR  Reserved1[8];
   USHORT Capabilities;
   UCHAR  Reserved2[12];
} HELLO;

#define Y_DIETIFNA         0x0001
#define FTB_USER           0x0002
#define ZED_ZIPPER         0x0004
#define ZED_ZAPPER         0x0008
#define DOES_IANUS         0x0010
#define DO_DOMAIN          0x4000
#define WZ_FREQ            0x8000

TMailer::TMailer (class TBbs *lpBbs)
{
   Bbs = lpBbs;
   Cfg = lpBbs->Cfg;
   Com = lpBbs->Com;
   Log = lpBbs->Log;

   strcpy (Name, Cfg->SystemName);
   strcpy (Sysop, Cfg->SysopName);
   Zone = 2;
   Net = Node = Point = 0;
   strcpy (Inbound, Cfg->Inbound);
   Address.Clear ();
}

TMailer::~TMailer (void)
{
}

USHORT TMailer::EMSIReceiver (VOID)
{
   USHORT RetVal = FALSE, Retry;
   SHORT key, prev;
   CHAR pkt[4];

   if (ReceiveEMSIPacket () == TRUE) {
      ParseEMSIPacket ();

      SendEMSIPacket ();

      prev = 0;
      Retry = 0;
      do {
         if ((key = TimedRead ()) != -1) {
            if (key == '*' && prev == '*') {
               if ((key = TimedRead ()) == 'E') {
                  if ((key = TimedRead ()) == 'M') {
                     if ((key = TimedRead ()) == 'S') {
                        if ((key = TimedRead ()) == 'I') {
                           if ((key = TimedRead ()) == '_') {
                              if ((key = TimedRead ()) != -1)
                                 pkt[0] = (CHAR)key;
                              if ((key = TimedRead ()) != -1)
                                 pkt[1] = (CHAR)key;
                              if ((key = TimedRead ()) != -1)
                                 pkt[2] = (CHAR)key;
                              pkt[3] = '\0';
                              if (!stricmp (pkt, "ACK") || !stricmp (pkt, "NAK")) {
                                 if (TimedRead () != -1) {
                                    if (TimedRead () != -1) {
                                       if (TimedRead () != -1) {
                                          if (TimedRead () != -1) {
                                             TimedRead ();
                                          }
                                       }
                                    }
                                 }
                                 if (!stricmp (pkt, "ACK"))
                                    RetVal = TRUE;
                                 else if (++Retry < 10)
                                    SendEMSIPacket ();
                              }
                           }
                        }
                     }
                  }
               }
            }
            prev = key;
         }
      } while (Com->Carrier () == TRUE && RetVal == FALSE && Retry < 10);
   }

   if (RetVal == TRUE)
      Receiver ();

   return (RetVal);
}

USHORT TMailer::EMSISender (VOID)
{
   USHORT RetVal = FALSE, Retry;
   SHORT key, prev;
   CHAR pkt[4];

   SendEMSIPacket ();

   prev = 0;
   Retry = 0;
   do {
      if ((key = TimedRead ()) != -1) {
         if (key == '*' && prev == '*') {
            if ((key = TimedRead ()) == 'E') {
               if ((key = TimedRead ()) == 'M') {
                  if ((key = TimedRead ()) == 'S') {
                     if ((key = TimedRead ()) == 'I') {
                        if ((key = TimedRead ()) == '_') {
                           if ((key = TimedRead ()) != -1)
                              pkt[0] = (CHAR)key;
                           if ((key = TimedRead ()) != -1)
                              pkt[1] = (CHAR)key;
                           if ((key = TimedRead ()) != -1)
                              pkt[2] = (CHAR)key;
                           pkt[3] = '\0';
                           if (!stricmp (pkt, "ACK") || !stricmp (pkt, "NAK")) {
                              if (TimedRead () != -1) {
                                 if (TimedRead () != -1) {
                                    if (TimedRead () != -1)
                                       TimedRead ();
                                 }
                              }
                              if (!stricmp (pkt, "NAK")) {
                                 if (++Retry < 10)
                                    SendEMSIPacket ();
                              }
                           }
                           else if (!stricmp (pkt, "DAT")) {
                              if (ReceiveEMSIPacket () == TRUE)
                                 RetVal = TRUE;
                           }
                        }
                     }
                  }
               }
            }
         }
         prev = key;
      }
   } while (Com->Carrier () == TRUE && RetVal == FALSE && Retry < 10);

   if (RetVal == TRUE) {
      ParseEMSIPacket ();
      Sender ();
   }

   return (RetVal);
}

VOID TMailer::ParseEMSIPacket (VOID)
{
   USHORT i, t, idents, firstEntry;
   UCHAR Byte, *Read, *Write, c, FoundKnown, FoundProt;
   CHAR *Tokens[MAX_EMSITOKEN], Temp[48], *p, List[128];
   class TNodes *Nodes;

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
      if (!stricmp (Tokens[t], "IDENT") || !stricmp (Tokens[t], "TRX#")) {
         if ((Read = (UCHAR *)Tokens[t + 1]) != NULL) {
            i = MAX_EMSITOKEN;
            strcpy (Temp, Tokens[t]);
            if (!stricmp (Tokens[t], "IDENT"))
               i = EMSI_SYSTEMNAME;
            else if (!stricmp (Tokens[t], "TRX#"))
               i = EMSI_TRANX;
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
               strcpy (RemoteSystem, Tokens[EMSI_SYSTEMNAME]);
               strcpy (RemoteSysop, Tokens[EMSI_SYSOPNAME]);
               if (Tokens[EMSI_MAILERSERIAL][0] != '\0')
                  sprintf (RemoteProgram, "%s %s/%s", Tokens[EMSI_MAILERNAME], Tokens[EMSI_MAILERVERSION], Tokens[EMSI_MAILERSERIAL]);
               else
                  sprintf (RemoteProgram, "%s %s", Tokens[EMSI_MAILERNAME], Tokens[EMSI_MAILERVERSION]);
            }
         }
      }
   }

   if (Address.First () == TRUE) {
      Log->Write ("*%s (%s)", RemoteSystem, Address.String);
      if (Address.Next () == TRUE) {
         List[0] = '\0';
         firstEntry = TRUE;
         do {
            sprintf (Temp, "%s ", Address.String);
            if ((strlen (List) + strlen (Temp)) > (sizeof (List) - 1)) {
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

   if ((Nodes = new TNodes (Cfg->SystemPath)) != NULL) {
      FoundKnown = FoundProt = FALSE;
      if (Address.First () == TRUE)
         do {
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
               if (FoundKnown == FALSE) {
                  strcpy (Inbound, Cfg->InboundKnown);
                  FoundKnown = TRUE;
               }
               if (FoundProt == FALSE) {
                  if (Nodes->SessionPwd[0] != '\0' && !stricmp (Tokens[EMSI_PASSWORD], Nodes->SessionPwd)) {
                     strcpy (Inbound, Cfg->InboundProt);
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

USHORT TMailer::ReceiveEMSIPacket (VOID)
{
   SHORT key;
   USHORT i, len, Crc, OtherCrc;

   Crc = StringCrc16 ("EMSI_DAT");

   len = 0;
   for (i = 0; i < 4; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
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
      ReceiveEMSI[i] = (CHAR)key;
      Crc = Crc16 ((UCHAR)key, Crc);
   }
   ReceiveEMSI[i] = '\0';

   OtherCrc = 0;
   for (i = 0; i < 4; i++) {
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
      return (FALSE);
   }
   else
      Com->SendBytes ((UCHAR *)"**EMSI_ACKA490\r**EMSI_ACKA490\r", 30);

   return (TRUE);
}

USHORT TMailer::ReceiveHello (VOID)
{
   USHORT RetVal = FALSE, Crc, IsPacket, FoundKnown, FoundProt;
   USHORT Received, RemoteCrc, CrcCount;
   UCHAR c, *p;
   CHAR Temp[40];
   LONG Timer, Retry;
   HELLO Hello;
   class TNodes *Nodes;

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
   } while (Com->Carrier () == TRUE && !TimeUp (Timer) && Crc != RemoteCrc);

   if (Com->Carrier () == TRUE && Crc == RemoteCrc) {
      Address.Add (Hello.Zone, Hello.Net, Hello.Node, Hello.Point);
      Address.First ();

      Log->Write ("*%s (%s)", Hello.Name, Address.String);
      if (Hello.Product == isOPUS)
         Log->Write ("*Remote Uses Opus Version %d.%02d", Hello.ProductMaj, (Hello.ProductMin == 48) ? 0 : Hello.ProductMin);
      else if (Hello.Product <= isMAX_PRODUCT)
         Log->Write ("*Remote Uses %s Version %d.%02d", ProductNames[Hello.Product], Hello.ProductMaj, Hello.ProductMin);
      else
         Log->Write ("*Remote Uses Program '%02x' Version %d.%02d", Hello.Product, Hello.ProductMaj, Hello.ProductMin);
      Log->Write (":SysOp: %s", strcpy (RemoteSysop, Hello.Sysop));

      if ((Nodes = new TNodes (Cfg->SystemPath)) != NULL) {
         FoundKnown = FoundProt = FALSE;
         if (Address.First () == TRUE)
            do {
               if (Nodes->Read (Address) == FALSE) {
                  Nodes->New ();
                  strcpy (Nodes->Address, Address.String);
                  strcpy (Nodes->SystemName, Hello.Name);
                  strcpy (Nodes->SysopName, Hello.Sysop);
                  strcpy (Nodes->SessionPwd, Hello.Password);
                  Nodes->Add ();
               }
               else {
                  if (FoundKnown == FALSE) {
                     strcpy (Inbound, Cfg->InboundKnown);
                     FoundKnown = TRUE;
                  }
                  if (FoundProt == FALSE) {
                     if (Nodes->SessionPwd[0] != '\0' && !stricmp (Hello.Password, Nodes->SessionPwd)) {
                        strcpy (Inbound, Cfg->InboundProt);
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
      Com->SendByte (ACK);
      Com->SendByte (YOOHOO);

      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TMailer::SendEMSIPacket (VOID)
{
   CHAR Temp[64], FoundPw = FALSE;
   USHORT Crc;
   class TNodes *Nodes;

   strcpy (SendEMSI, "{EMSI}{");
   if (Cfg->Address.First () == TRUE) {
      strcat (SendEMSI, Cfg->Address.String);
      while (Cfg->Address.Next () == TRUE) {
         strcat (SendEMSI, " ");
         strcat (SendEMSI, Cfg->Address.String);
      }
   }
   strcat (SendEMSI, "}{");

   if ((Nodes = new TNodes (Cfg->SystemPath)) != NULL) {
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

   strcat (SendEMSI, "}{8N1,PUA}{ZAP,ZMO,ARC,XMA}");
#if defined(__DOS__)
   sprintf (Temp, "{%02X}{%s}{%s}{}{IDENT}{", isLORA, STD_NAME, VERSION);
#else
   sprintf (Temp, "{%02X}{%s}{%s}{}{IDENT}{", isLORA, PRO_NAME, VERSION);
#endif
   strcat (SendEMSI, Temp);
   sprintf (Temp, "[%s][]", Cfg->SystemName);
   strcat (SendEMSI, Temp);
   sprintf (Temp, "[%s]", Cfg->SysopName);
   strcat (SendEMSI, Temp);
   strcat (SendEMSI, "[-Unpublished-][9600][XA,V32B,V42B]}");

   Com->BufferBytes ((UCHAR *)"**", 2);

   sprintf (Temp, "EMSI_DAT%04X", strlen (SendEMSI));
   Com->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   Crc = StringCrc16 (Temp);

   Com->BufferBytes ((UCHAR *)SendEMSI, (USHORT)strlen (SendEMSI));
   Crc = StringCrc16 (SendEMSI, Crc);
   Com->ClearInbound ();

   sprintf (Temp, "%04X\r", Crc);
   Com->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   Com->UnbufferBytes ();
}

VOID TMailer::Receiver (VOID)
{
   USHORT FileReceived, FileSent;
   CHAR FinalPhase = FALSE, *p, Stop;
   ULONG ReceivedSize, SentSize, Length;
   struct find_t ffblk;
   class TTransfer *Transfer;
   class TOutbound *Outbound;

   if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL) {
      FileSent = FileReceived = 0;
      SentSize = ReceivedSize = 0L;

      if (Capabilities & (ZED_ZIPPER|ZED_ZAPPER)) {
         Log->Write (":Session method: %s", "ZedZap");

         if ((Transfer = new TTransfer (Bbs)) != NULL) {
// ----------------------------------------------------------------------
// The first phase of a ZedZap/ZedZip transfer in answering mode is
// the receipt of the files sent by the remote.
// ----------------------------------------------------------------------
            if ((p = Transfer->ReceiveZModem (Cfg->Inbound)) != NULL) {
               FileReceived++;
               if (_dos_findfirst (p, 0, &ffblk) == 0)
                  ReceivedSize += ffblk.size;
               while ((p = Transfer->ReceiveZModem (Cfg->Inbound)) != NULL) {
                  FileReceived++;
                  if (_dos_findfirst (p, 0, &ffblk) == 0)
                     ReceivedSize += ffblk.size;
               }
            }

            if (Bbs->AbortSession () == FALSE) {
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
                              if (Transfer->SendZModem (Outbound->Complete) == FALSE)
                                 Stop = TRUE;
                              else {
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
                  } while (Stop == FALSE && Address.Next () == TRUE);
               }
               Transfer->SendZModem (NULL);
            }

            if (Bbs->AbortSession () == FALSE && FinalPhase == TRUE) {
// ----------------------------------------------------------------------
// This is the optional third phase, that is activated only if we
// have sent some file requests to the remote system, in order to
// receive the requested files.
// ----------------------------------------------------------------------
               if ((p = Transfer->ReceiveZModem (Cfg->Inbound)) != NULL) {
                  FileReceived++;
                  if (_dos_findfirst (p, 0, &ffblk) == 0)
                     ReceivedSize += ffblk.size;
                  while ((p = Transfer->ReceiveZModem (Cfg->Inbound)) != NULL) {
                     FileReceived++;
                     if (_dos_findfirst (p, 0, &ffblk) == 0)
                        ReceivedSize += ffblk.size;
                  }
               }
            }
            delete Transfer;
         }
      }

      delete Outbound;
      Length = time (NULL) - Bbs->StartCall;

      Log->Write ("*End of WaZOO/EMSI Session");
      Log->Write (" Received: %u/%lu Sent: %u/%lu Total: %u/%lu", FileReceived, ReceivedSize, FileSent, SentSize, FileReceived + FileSent, ReceivedSize + SentSize);
      Address.First ();
      if (Length < 60)
         Log->Write ("*Seconds: %lu  System: %s", Length, Address.String);
      else
         Log->Write ("*Minutes: %lu:%02lu  System: %s", Length / 60L, Length % 60L, Address.String);
   }
}

VOID TMailer::Sender (VOID)
{
   USHORT FileReceived, FileSent;
   CHAR FinalPhase = FALSE, *p, Stop;
   ULONG ReceivedSize, SentSize, Length;
   struct find_t ffblk;
   class TTransfer *Transfer;
   class TOutbound *Outbound;

   if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL) {
      FileSent = FileReceived = 0;
      SentSize = ReceivedSize = 0L;

      if (Capabilities & (ZED_ZIPPER|ZED_ZAPPER)) {
         Log->Write (":Session method: %s", "ZedZap");

         if ((Transfer = new TTransfer (Bbs)) != NULL) {
// ----------------------------------------------------------------------
// The first phase of a ZedZap/ZedZip transfer in originating mode is
// the transfer of any pending outbound files to the remote system.
// ----------------------------------------------------------------------
            Stop = FALSE;
            if (Address.First () == TRUE) {
               do {
                  Outbound->Add (Address.Zone, Address.Net, Address.Node, Address.Point, Address.Domain);
                  if (Outbound->First () == TRUE) {
                     do {
                        if (Outbound->Poll == FALSE) {
                           if (Transfer->SendZModem (Outbound->Complete) == FALSE)
                              Stop = TRUE;
                           else {
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
               } while (Stop == FALSE && Address.Next () == TRUE);
            }
            Transfer->SendZModem (NULL);

            if (Bbs->AbortSession () == FALSE) {
// ----------------------------------------------------------------------
// Second phase, now it is our turn to receive the files that are
// waiting for us on the remote system.
// ----------------------------------------------------------------------
               if ((p = Transfer->ReceiveZModem (Cfg->Inbound)) != NULL) {
                  FileReceived++;
                  if (_dos_findfirst (p, 0, &ffblk) == 0)
                     ReceivedSize += ffblk.size;
                  while ((p = Transfer->ReceiveZModem (Cfg->Inbound)) != NULL) {
                     FileReceived++;
                     if (_dos_findfirst (p, 0, &ffblk) == 0)
                        ReceivedSize += ffblk.size;
                  }
               }
            }

            if (Bbs->AbortSession () == FALSE && FinalPhase == TRUE) {
// ----------------------------------------------------------------------
// This is the optional third phase, that is activated only if the
// remote system has requested some files from us.
// ----------------------------------------------------------------------
            }
            delete Transfer;
         }
      }

      delete Outbound;
      Length = time (NULL) - Bbs->StartCall;

      Log->Write ("*End of WaZOO/EMSI Session");
      Log->Write (" Received: %u/%lu Sent: %u/%lu Total: %u/%lu", FileReceived, ReceivedSize, FileSent, SentSize, FileReceived + FileSent, ReceivedSize + SentSize);
      Address.First ();
      if (Length < 60)
         Log->Write ("*Seconds: %lu  System: %s", Length, Address.String);
      else
         Log->Write ("*Minutes: %lu:%02lu  System: %s", Length / 60L, Length % 60L, Address.String);
   }
}

USHORT TMailer::SendHello (VOID)
{
   USHORT i, RetVal = FALSE, Crc;
   UCHAR *p, Resp = 0;
   ULONG Timer, Retry;
   HELLO Hello;

   memset (&Hello, 0, sizeof (Hello));
   Hello.Signal = 'o';
   Hello.Version =  1;
   Hello.Product = isLORA;
   Hello.ProductMaj = MAJ_VERSION;
   Hello.ProductMin = MIN_VERSION;
   strcpy (Hello.Name, Name);
   strcpy (Hello.Sysop, Sysop);
   if (Cfg->Address.First () == TRUE) {
      Hello.Zone = Cfg->Address.Zone;
      Hello.Net = Cfg->Address.Net;
      Hello.Node = Cfg->Address.Node;
      Hello.Point = Cfg->Address.Point;
   }
   Hello.Capabilities |= Y_DIETIFNA;
   Hello.Capabilities |= ZED_ZAPPER|ZED_ZIPPER;
   Hello.Capabilities |= WZ_FREQ;

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

SHORT TMailer::TimedRead (VOID)
{
   long tout;

   if (Com->BytesReady () == TRUE)
      return (Com->ReadByte ());

   tout = TimerSet (100);

   do {
      if (Com->BytesReady () == TRUE)
         return (Com->ReadByte ());
   } while (!TimeUp (tout) && Bbs->AbortSession () == FALSE);

   return (-1);
}

USHORT TMailer::WaZOOReceiver (VOID)
{
   USHORT RetVal = FALSE;
   ULONG Timer;

   if (ReceiveHello () == TRUE) {
      Timer = TimerSet (500);
      do {
         if (Com->BytesReady () == TRUE) {
            if (Com->ReadByte () == ENQ)
               RetVal = SendHello ();
         }
      } while (Com->Carrier () == TRUE && RetVal == FALSE && !TimeUp (Timer));
   }

   if (RetVal == TRUE)
      Receiver ();

   return (RetVal);
}

USHORT TMailer::WaZOOSender (VOID)
{
   USHORT RetVal = FALSE;

   if (SendHello () == TRUE && Com->Carrier () == TRUE)
      RetVal = ReceiveHello ();

   if (RetVal == TRUE && Com->Carrier () == TRUE)
      Receiver ();

   return (RetVal);
}


