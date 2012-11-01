
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.06
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "mailer.h"

TDetect::TDetect (class TBbs *bbs)
{
   Cfg = bbs->Cfg;
   Com = bbs->Com;
   Log = bbs->Log;
   User = bbs->User;

   Bbs = bbs;
}

TDetect::~TDetect (void)
{
}

VOID TDetect::DetectRemote (VOID)
{
   USHORT key, prev;
   ULONG Timer, Retry;
   class TMailer *Mailer;

   Timer = TimerSet (4000);
   Retry = TimerSet (500);

   while (Bbs->AbortSession () == FALSE && !TimeUp (Timer)) {
      if (Com->BytesReady () == TRUE) {
         key = Com->ReadByte ();
         switch (key) {
            case '*':
               if (prev == '*') {
                  switch (CheckEMSIPacket ()) {
                     case EMSI_REQ:
                        if ((Mailer = new TMailer (Bbs)) != NULL) {
                           if (Address.First () == TRUE) {
                              Mailer->Address.Clear ();
                              Mailer->Address.Add (Address.String);
                           }
                           Com->SendBytes ((UCHAR *)"**EMSI_INQC816\r", 15);
                           if (Mailer->EMSISender () == TRUE)
                              Bbs->Hangup = TRUE;
                           delete Mailer;
                        }
                        break;
                  }
               }
               break;

            case ENQ:
               if ((Mailer = new TMailer (Bbs)) != NULL) {
                  if (Address.First () == TRUE) {
                     Mailer->Address.Clear ();
                     Mailer->Address.Add (Address.String);
                  }
                  if (Mailer->WaZOOSender () == TRUE)
                     Bbs->Hangup = TRUE;
                  delete Mailer;
               }
               break;
         }
         prev = key;
      }

      if (TimeUp (Retry)) {
         Com->SendBytes ((UCHAR *)" \r \r", 4);
         Com->SendBytes ((UCHAR *)"**EMSI_INQC816\r**EMSI_INQC816\r", 30);
         Com->SendByte (YOOHOO);
         Retry = TimerSet (500);
      }
   }
}

VOID TDetect::DetectTerminal (VOID)
{
   USHORT key, prev, pos, i, gotAnswer, IsEMSI;
   ULONG tout;
   PSZ pszAnnounce = "\rAuto-Sensing Terminal...\r";
   class TMailer *Mailer;

   Com->ClearInbound ();
   gotAnswer = FALSE;
   tout = TimerSet (200);
   prev = 0;
   IsEMSI = FALSE;

   for (i = 0; i < 5 && gotAnswer == FALSE && Bbs->AbortSession () == FALSE; i++) {
      while (!TimeUp (tout) && Bbs->AbortSession () == FALSE) {
         if (Com->BytesReady () == TRUE) {
            key = Com->ReadByte ();
            switch (key) {
               case '*':
                  if (prev == '*') {
                     switch (CheckEMSIPacket ()) {
                        case EMSI_ICI:
                           IEMSIReceiver ();
                           gotAnswer = TRUE;
                           break;
                        case EMSI_INQ:
                           Com->SendBytes ((UCHAR *)"**EMSI_REQA77E\r", 15);
                           IsEMSI = TRUE;
                           break;
                        case EMSI_DAT:
                           if ((Mailer = new TMailer (Bbs)) != NULL) {
                              if (Mailer->EMSIReceiver () == TRUE)
                                 Bbs->Hangup = TRUE;
                              delete Mailer;
                           }
                           break;
                     }
                  }
                  break;

               case 'R':
               case 'I':
                  break;
               case 'P':
                  if (prev == 'I') {
                     Bbs->Rip = TRUE;
                     gotAnswer = TRUE;
                  }
                  break;

               case 0x1B:
                  if (prev == 0x1B) {
                     Bbs->Printf ("\n\n");
                     return;
                  }
                  break;

               case '[':
                  if (prev == 0x1B) {
                     Bbs->Ansi = TRUE;
                     Bbs->Color = TRUE;
                     pos = 0;
                     gotAnswer = TRUE;
                  }
                  break;

               case ';':
                  if (pos == 12) {
                     Bbs->Avatar = TRUE;
                     Bbs->Color = TRUE;
                     gotAnswer = TRUE;
                  }
                  break;

               case YOOHOO:
                  if (IsEMSI == FALSE) {
                     if ((Mailer = new TMailer (Bbs)) != NULL) {
                        if (Mailer->WaZOOReceiver () == TRUE)
                           Bbs->Hangup = TRUE;
                        delete Mailer;
                     }
                  }
                  break;

               default:
                  if (isdigit (key)) {
                     pos = (USHORT)(pos * 10);
                     pos += (USHORT)(key - '0');
                  }
                  break;
            }
            prev = key;
         }
      }

      if (gotAnswer == FALSE && Bbs->AbortSession () == FALSE) {
         Com->BufferBytes ((UCHAR *)" \r", 2);

         Com->BufferBytes ((UCHAR *)"\r\x19\x20\x0B", 4);
         Com->BufferBytes ((UCHAR *)"\r**EMSI_REQA77E\r", 16);
         Com->BufferBytes ((UCHAR *)"\r**EMSI_IRQ8E08\r", 16);
//         Com->BufferBytes ((UCHAR *)"\x1B[!", 3);
         Com->BufferBytes ((UCHAR *)"\x1B[6n", 4);

         Com->BufferBytes ((UCHAR *)pszAnnounce, (USHORT)strlen (pszAnnounce));
         Com->UnbufferBytes ();

         tout = TimerSet (400);
      }
   }

   if (Bbs->AbortSession () == FALSE)
      Bbs->Printf ("\n\n");
}

VOID TDetect::SelectLanguage (VOID)
{
   CHAR Temp[16], File[64];

   if (Cfg->Language.First () == TRUE) {
      do {
         Bbs->Printf ("\n\x16\x01\012Please choose one of these languages:\n\n");
         if (Cfg->Language.First () == TRUE)
            do {
               Bbs->Printf ("\x16\x01\013%3s \x16\x01\016... %s\n", Cfg->Language.Key, Cfg->Language.Description);
            } while (Cfg->Language.Next () == TRUE);

         Bbs->Printf ("\n\x16\x01\013Choose a number from 1 to 2: ");
         Bbs->GetString (Temp, (USHORT)(sizeof (Temp) - 1), 0);
      } while (Cfg->Language.Check (Temp) == FALSE && Bbs->AbortSession () == FALSE);

      if (Cfg->Language.Check (Temp) == TRUE) {
         sprintf (File, "%s%s", Cfg->SystemPath, Cfg->Language.File);
         Bbs->Lang->Read (File);
         if (Cfg->Language.MenuPath[0])
            strcpy (Cfg->MenuPath, Cfg->Language.MenuPath);
         if (Cfg->Language.MiscPath[0])
            strcpy (Cfg->MiscPath, Cfg->Language.MiscPath);
         Bbs->Lang->Write (File);
      }
   }
}

SHORT TDetect::TimedRead (VOID)
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

USHORT TDetect::CheckEMSIPacket (VOID)
{
   USHORT RetVal = EMSI_NONE;
   CHAR pkt[4];

   if (TimedRead () != 'E')
      return (RetVal);
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
   else if (!stricmp (pkt, "ACK") || !stricmp (pkt, "HBT")) {
      RetVal = EMSI_NONE;
      if (TimedRead () == -1)
         return (RetVal);
      if (TimedRead () == -1)
         return (RetVal);
      if (TimedRead () == -1)
         return (RetVal);
      if (TimedRead () == -1)
         return (RetVal);
//      TimedRead ();
   }

   return (RetVal);
}

VOID TDetect::IEMSIReceiver (VOID)
{
   if (ReceiveIEMSIPacket () == TRUE) {
      SendIEMSIPacket ();
      ParseIEMSIPacket ();
   }
}

VOID TDetect::SendIEMSIPacket (VOID)
{
   CHAR szTemp[64];
   ULONG ulCrc;

   strcpy (szSendIEMSI, "{LoraBBS,0.01,Unregistered}{");
   strcat (szSendIEMSI, Cfg->SystemName);
   strcat (szSendIEMSI, "}{Bologna, Italy}{");
   strcat (szSendIEMSI, Cfg->SysopName);
   strcat (szSendIEMSI, "}{0L}{}{}{!}{}");

   Com->BufferBytes ((UCHAR *)"**", 2);

   sprintf (szTemp, "EMSI_ISI%04X", strlen (szSendIEMSI));
   Com->BufferBytes ((UCHAR *)szTemp, (USHORT)strlen (szTemp));
   ulCrc = StringCrc32 (szTemp);

   Com->BufferBytes ((UCHAR *)szSendIEMSI, (USHORT)strlen (szSendIEMSI));
   ulCrc = StringCrc32 (szSendIEMSI, ulCrc);
   Com->ClearInbound ();

   sprintf (szTemp, "%08lX\r", ulCrc);
   Com->BufferBytes ((UCHAR *)szTemp, (USHORT)strlen (szTemp));
}

USHORT TDetect::ReceiveIEMSIPacket (VOID)
{
   SHORT key;
   USHORT i, len;
   ULONG ulCrc, ulOtherCrc;

   ulCrc = StringCrc32 ("EMSI_ICI");

   len = 0;
   for (i = 0; i < 4; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
      ulCrc = Crc32 ((UCHAR)key, ulCrc);
      len <<= 4;
      key = (USHORT)(toupper (key) - 48);
      if (key > 9)
         key = (SHORT)(key - 7);
      len |= key;
   }

   for (i = 0; i < len; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
      szReceiveIEMSI[i] = (CHAR)key;
      ulCrc = Crc32 ((UCHAR)key, ulCrc);
   }
   szReceiveIEMSI[i] = '\0';

   ulOtherCrc = 0;
   for (i = 0; i < 8; i++) {
      if ((key = TimedRead ()) == -1)
         return (FALSE);
      ulOtherCrc <<= 4;
      key = (USHORT)(toupper (key) - 48);
      if (key > 9)
         key = (SHORT)(key - 7);
      ulOtherCrc |= key;
   }

   TimedRead ();

   if (ulCrc != ulOtherCrc) {
      Com->BufferBytes ((UCHAR *)"**EMSI_NAKEEC3\r", 15);
      return (FALSE);
   }

   return (TRUE);
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

VOID TDetect::ParseIEMSIPacket (VOID)
{
   USHORT i;
   UCHAR Byte, *pRead, *pWrite, c;
   CHAR *pTokens[MAX_IEMSITOKEN];

   pRead = pWrite = (UCHAR *)szReceiveIEMSI;
   i = 0;

   while (*pRead) {
      if (*pRead == '\\') {
         pRead++;
         if (*pRead != '\\') {
            Byte = (UCHAR)(*pRead - 48);
            if (Byte > 9)
               Byte = (UCHAR)(Byte - 7);
            Byte <<= 4;
            pRead++;
            c = (UCHAR)(*pRead - 48);
            if (c > 9)
               c = (UCHAR)(c - 7);
            Byte |= c;
            *pWrite++ = Byte;
         }
         else
            *pWrite++ = *pRead;
      }
      else {
         *pWrite = *pRead;
         if (*pWrite == '{') {
            if (i < MAX_IEMSITOKEN)
               pTokens[i++] = (CHAR *)(pWrite + 1);
         }
         else if (*pWrite == '}')
            *pWrite = '\0';
         pWrite++;
      }

      pRead++;
   }

   pTokens[i] = NULL;

   if (strstr (pTokens[IEMSI_CRTDEF], "AVT0") != NULL) {
      Bbs->Ansi = TRUE;
      Bbs->Avatar = TRUE;
      Bbs->Color = TRUE;
   }
   if (strstr (pTokens[IEMSI_CRTDEF], "ANSI") != NULL) {
      Bbs->Ansi = TRUE;
      Bbs->Color = TRUE;
   }
   if (strstr (pTokens[IEMSI_CRTDEF], "VT52") != NULL)
      Bbs->Ansi = TRUE;
   if (strstr (pTokens[IEMSI_CRTDEF], "VT100") != NULL)
      Bbs->Ansi = TRUE;
//   if (strstr (pTokens[IEMSI_CAPS], "ASCII8") != NULL)
//      usResult |= VIDEO_IBM;

   if (pTokens[IEMSI_NAME][0] != '\0') {
      if (User->GetData (pTokens[IEMSI_NAME]) == TRUE) {
         User->Validate = TRUE;
         if (User->CheckPassword (pTokens[IEMSI_PASSWORD]) == TRUE)
            User->Validate = FALSE;
      }
      else {
         strcpy (User->Name, pTokens[IEMSI_NAME]);
         strcpy (User->RealName, pTokens[IEMSI_ALIAS]);
         strcpy (User->Address2, pTokens[IEMSI_LOCATION]);
         User->SetPassword (pTokens[IEMSI_PASSWORD]);
         User->Ansi = Bbs->Ansi;
         User->Avatar = Bbs->Avatar;
         User->Color = Bbs->Color;
         User->Update ();
         User->Validate = FALSE;
      }
   }
}



