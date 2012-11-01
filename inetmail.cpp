
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 2.99.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

static PSZ MONTHS[] = {
   "January", "February", "March", "April", "Maj", "Juni",
   "July", "August", "September", "October", "November", "December"
};

INETMAIL::INETMAIL (void)
{
   Tcp = new TTcpip;
   TotalMsgs = 0L;
   LastReaded = 0L;
   strcpy (HostName, "unknown.host");
}

INETMAIL::INETMAIL (PSZ pszServer, PSZ pszUser, PSZ pszPwd)
{
   Tcp = new TTcpip;
   TotalMsgs = 0L;
   LastReaded = 0L;
   strcpy (HostName, "unknown.host");

   Open (pszServer, pszUser, pszPwd);
}

INETMAIL::~INETMAIL (void)
{
   if (Tcp != NULL)
      delete Tcp;
}

USHORT INETMAIL::Add (VOID)
{
   return (Add (Text));
}

USHORT INETMAIL::Add (class TMsgBase *MsgBase)
{
   New ();

   strcpy (From, MsgBase->From);
   strcpy (To, MsgBase->To);
   strcpy (Subject, MsgBase->Subject);

   strcpy (FromAddress, MsgBase->FromAddress);
   strcpy (ToAddress, MsgBase->ToAddress);

   Written.Day = MsgBase->Written.Day;
   Written.Month = MsgBase->Written.Month;
   Written.Year = MsgBase->Written.Year;
   Written.Hour = MsgBase->Written.Hour;
   Written.Minute = MsgBase->Written.Minute;
   Written.Second = MsgBase->Written.Second;

   Arrived.Day = MsgBase->Arrived.Day;
   Arrived.Month = MsgBase->Arrived.Month;
   Arrived.Year = MsgBase->Arrived.Year;
   Arrived.Hour = MsgBase->Arrived.Hour;
   Arrived.Minute = MsgBase->Arrived.Minute;
   Arrived.Second = MsgBase->Arrived.Second;

   Crash = MsgBase->Crash;
   Direct = MsgBase->Direct;
   FileAttach = MsgBase->FileAttach;
   FileRequest = MsgBase->FileRequest;
   Hold = MsgBase->Hold;
   Immediate = MsgBase->Immediate;
   Intransit = MsgBase->Intransit;
   KillSent = MsgBase->KillSent;
   Local = MsgBase->Local;
   Private = MsgBase->Private;
   ReceiptRequest = MsgBase->ReceiptRequest;
   Received = MsgBase->Received;
   Sent = MsgBase->Sent;

   return (Add (MsgBase->Text));
}

/*
USHORT INETMAIL::Add (class TCollection &MsgText)
{
   FILE *fp;
   USHORT RetVal = FALSE;
   USHORT GotFrom, GotSubject, GotTo;
   CHAR szBuffer[128], *pszText;

   GotFrom = GotSubject = GotTo = FALSE;

   if ((fp = fopen ("outmail.log", "ab")) != NULL) {
            fprintf (fp, "HELO %s\r\n", HostName);

            if (strchr (From, '@') != NULL)
               fprintf (fp, "MAIL FROM:<%s>\r\n", From);
            else
               fprintf (fp, "MAIL FROM:<%s>\r\n", FromAddress);
               if (strchr (To, '@') != NULL)
                  fprintf (fp, "RCPT TO:<%s>\r\n", To);
               else
                  fprintf (fp, "RCPT TO:<%s>\r\n", ToAddress);
                  fprintf (fp, "DATA\r\n");
                     if ((pszText = (CHAR *)MsgText.First ()) != NULL)
                        do {
                           if (!strncmp (pszText, "\001From: ", 7)) {
                              fprintf (fp, "%s\r\n", &pszText[1]);
                              GotFrom = TRUE;
                           }
                           else if (!strncmp (pszText, "\001To: ", 5)) {
                              fprintf (fp, "%s\r\n", &pszText[1]);
                              GotTo = TRUE;
                           }
                           else if (!strncmp (pszText, "\001Subject: ", 10)) {
                              fprintf (fp, "%s\r\n", &pszText[1]);
                              GotSubject = TRUE;
                           }
                           else if (!strncmp (pszText, "\001X-Mailreader: ", 15)) {
                              fprintf (fp, "%s\r\n", &pszText[1]);
                           }
                           else if (!strncmp (pszText, "\001In-Reply-To: ", 14)) {
                              fprintf (fp, "%s\r\n", &pszText[1]);
                           }
                           else if (!strncmp (pszText, "\001Sender: ", 9)) {
                              fprintf (fp, "%s\r\n", &pszText[1]);
                           }
                        } while ((pszText = (CHAR *)MsgText.Next ()) != NULL);

                     if (GotFrom == FALSE) {
                        if (strchr (From, '@') != NULL)
                           fprintf (fp, "From: %s\r\n", From);
                        else
                           fprintf (fp, "From: %s <%s>\r\n", From, FromAddress);
                     }
                     if (GotTo == FALSE) {
                        if (strchr (To, '@') != NULL)
                           fprintf (fp, "To: %s\r\n", To);
                        else
                           fprintf (fp, "To: %s <%s>\r\n", To, ToAddress);
                     }
                     if (GotSubject == FALSE) {
                        fprintf (fp, "Subject: %s\r\n", Subject);
                     }
                     fprintf (fp, "Date: %d %3.3s %d %02d:%02d:%02d GMT\r\n", Written.Day, MONTHS[Written.Month - 1], Written.Year, Written.Hour, Written.Minute, Written.Second);

                     fprintf (fp, "\r\n");

                     if ((pszText = (CHAR *)MsgText.First ()) != NULL)
                        do {
                           if (*pszText != 0x01 && strncmp (pszText, "SEEN-BY:", 8)) {
                              if (!strcmp (pszText, "."))
                                 fprintf (fp, "..\r\n");
                              else
                                 fprintf (fp, "%s\r\n", pszText);
                           }
                        } while ((pszText = (CHAR *)MsgText.Next ()) != NULL);

                     fprintf (fp, ".\r\n");

                        RetVal = TRUE;

            fprintf (fp, "QUIT\r\n");

      fclose (fp);
   }

   return (RetVal);
}
*/

USHORT INETMAIL::Add (class TCollection &MsgText)
{
   USHORT RetVal = FALSE;
   USHORT GotFrom, GotSubject, GotTo;
   CHAR szBuffer[128], *pszText;
   class TTcpip *OldTcp = Tcp;

   GotFrom = GotSubject = GotTo = FALSE;

   if ((Tcp = new TTcpip) != NULL) {
      if (Tcp->ConnectServer (SMTPHostName, 25) == TRUE) {
         if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 220) {
            sprintf (szBuffer, "HELO %s\r\n", HostName);
            Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
            GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1));

            if (strchr (From, '@') != NULL)
               sprintf (szBuffer, "MAIL FROM:<%s>\r\n", From);
            else
               sprintf (szBuffer, "MAIL FROM:<%s>\r\n", FromAddress);
            Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
            if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 250) {
               if (strchr (To, '@') != NULL)
                  sprintf (szBuffer, "RCPT TO:<%s>\r\n", To);
               else
                  sprintf (szBuffer, "RCPT TO:<%s>\r\n", ToAddress);
               Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
               if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 250) {
                  sprintf (szBuffer, "DATA\r\n");
                  Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
                  if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 354) {
                     if ((pszText = (CHAR *)MsgText.First ()) != NULL)
                        do {
                           if (!strncmp (pszText, "\001From: ", 7)) {
                              Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                              Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                              GotFrom = TRUE;
                           }
                           else if (!strncmp (pszText, "\001To: ", 5)) {
                              Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                              Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                              GotTo = TRUE;
                           }
                           else if (!strncmp (pszText, "\001Subject: ", 10)) {
                              Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                              Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                              GotSubject = TRUE;
                           }
                           else if (!strncmp (pszText, "\001X-Mailreader: ", 15)) {
                              Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                              Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                           }
                           else if (!strncmp (pszText, "\001In-Reply-To: ", 14)) {
                              Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                              Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                           }
                           else if (!strncmp (pszText, "\001Sender: ", 9)) {
                              Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                              Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                           }
                        } while ((pszText = (CHAR *)MsgText.Next ()) != NULL);

                     if (GotFrom == FALSE) {
                        if (strchr (From, '@') != NULL)
                           sprintf (szBuffer, "From: %s\r\n", From);
                        else
                           sprintf (szBuffer, "From: %s <%s>\r\n", From, FromAddress);
                        Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
                     }
                     if (GotTo == FALSE) {
                        if (strchr (To, '@') != NULL)
                           sprintf (szBuffer, "To: %s\r\n", To);
                        else
                           sprintf (szBuffer, "To: %s <%s>\r\n", To, ToAddress);
                        Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
                     }
                     if (GotSubject == FALSE) {
                        sprintf (szBuffer, "Subject: %s\r\n", Subject);
                        Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
                     }
                     sprintf (szBuffer, "Date: %d %3.3s %d %02d:%02d:%02d GMT\r\n", Written.Day, MONTHS[Written.Month - 1], Written.Year, Written.Hour, Written.Minute, Written.Second);
                     Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));

                     Tcp->BufferBytes ((UCHAR *)"\r\n", 2);

                     if ((pszText = (CHAR *)MsgText.First ()) != NULL)
                        do {
                           if (*pszText != 0x01 && strncmp (pszText, "SEEN-BY:", 8)) {
                              if (!strcmp (pszText, "."))
                                 Tcp->BufferBytes ((UCHAR *)"..", 2);
                              else
                                 Tcp->BufferBytes ((UCHAR *)pszText, (USHORT)strlen (pszText));
                              Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                           }
                        } while ((pszText = (CHAR *)MsgText.Next ()) != NULL);

                     sprintf (szBuffer, ".\r\n");
                     Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
                     Tcp->UnbufferBytes ();

                     if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 250)
                        RetVal = TRUE;
                  }
               }
            }

            sprintf (szBuffer, "QUIT\r\n");
            Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
            GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1));
         }
      }
      delete Tcp;
   }

   Tcp = OldTcp;

   return (RetVal);
}

VOID INETMAIL::Close (VOID)
{
   CHAR String[50];

   Tcp->SendBytes ((UCHAR *)"QUIT\r\n", 6);
   GetResponse (String, (USHORT)(sizeof (String) - 1));
   TotalMsgs = 0L;
}

USHORT INETMAIL::Delete (ULONG ulMsg)
{
   CHAR String[50];

   sprintf (String, "DELE %lu\r\n", ulMsg);
   Tcp->SendBytes ((UCHAR *)String, (USHORT)strlen (String));
   return (GetResponse (String, (USHORT)(sizeof (String) - 1)));
}

USHORT INETMAIL::GetHWM (ULONG &ulMsg)
{
   ulMsg = 0L;

   return (FALSE);
}

USHORT INETMAIL::GetResponse (PSZ pszResponse, USHORT usMaxLen)
{
   USHORT retVal = FALSE, len = 0;
   CHAR c, *pszResp = pszResponse;

   do {
      c = '\0';
      if (Tcp->BytesReady () == TRUE) {
         if ((c = (CHAR)Tcp->ReadByte ()) != '\r') {
            if (c != '\n') {
               *pszResp++ = c;
               if (++len >= usMaxLen)
                  c = '\r';
            }
         }
      }
#if defined(__OS2__)
      else
         DosSleep (1L);
#elif defined(__NT__)
      else
         Sleep (1L);
#endif
   } while (c != '\r' && Tcp->Carrier () == TRUE);

   *pszResp = '\0';
   if (pszResponse[0] == '+')
      retVal = TRUE;
   else if (pszResponse[0] == '-')
      retVal = FALSE;
   else if (pszResponse[3] == ' ')
      retVal = (USHORT)atoi (pszResponse);

   return (retVal);
}

ULONG INETMAIL::Highest (VOID)
{
   return (TotalMsgs);
}

USHORT INETMAIL::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG INETMAIL::Lowest (VOID)
{
   return (1L);
}

ULONG INETMAIL::MsgnToUid (ULONG ulMsg)
{
   return (ulMsg);
}

VOID INETMAIL::New (VOID)
{
   LastReaded = 0L;
   From[0] = To[0] = Subject[0] = '\0';
   Crash = Direct = FileAttach = FileRequest = Hold = Immediate = FALSE;
   Intransit = KillSent = Local = Private = ReceiptRequest = Received = FALSE;
   Sent = 0;
   memset (&Written, 0, sizeof (Written));
   memset (&Arrived, 0, sizeof (Arrived));
   Text.Clear ();
}

USHORT INETMAIL::Next (ULONG &ulMsg)
{
   USHORT RetVal = FALSE;

   if (ulMsg < TotalMsgs) {
      ulMsg++;
      RetVal = TRUE;
   }

   return (RetVal);
}

ULONG INETMAIL::Number (VOID)
{
   return (TotalMsgs);
}

USHORT INETMAIL::Open (PSZ pszServer, PSZ pszUser, PSZ pszPwd)
{
   USHORT retVal = FALSE;
   CHAR String[128], *p;

   if (Tcp->ConnectServer (pszServer, 110) == TRUE) {
      if (GetResponse (String, (USHORT)(sizeof (String) - 1)) == TRUE) {
         sprintf (String, "USER %s\r\n", pszUser);
         Tcp->SendBytes ((UCHAR *)String, (USHORT)strlen (String));
         if (GetResponse (String, (USHORT)(sizeof (String) - 1)) == TRUE) {
            sprintf (String, "PASS %s\r\n", pszPwd);
            Tcp->SendBytes ((UCHAR *)String, (USHORT)strlen (String));
            if (GetResponse (String, (USHORT)(sizeof (String) - 1)) == TRUE) {
               sprintf (String, "STAT\r\n");
               Tcp->SendBytes ((UCHAR *)String, (USHORT)strlen (String));
               if (GetResponse (String, (USHORT)(sizeof (String) - 1)) == TRUE) {
                  p = strtok (String, " ");
                  if ((p = strtok (NULL, " ")) != NULL)
                     TotalMsgs = atol (p);
               }
               retVal = TRUE;
               LastReaded = 0L;
            }
         }
      }
   }

   return (retVal);
}

VOID INETMAIL::Pack (VOID)
{
}

USHORT INETMAIL::Previous (ULONG &ulMsg)
{
   USHORT RetVal = FALSE;

   if (ulMsg > 1) {
      ulMsg--;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT INETMAIL::ReadHeader (ULONG ulMsg)
{
   ulMsg = ulMsg;

   return (FALSE);
}

USHORT INETMAIL::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT INETMAIL::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   USHORT retVal = FALSE, SkipNext;
   SHORT i, nReaded, nCol;
   PSZ p;
   struct dosdate_t date;
   struct dostime_t time;

   From[0] = To[0] = Subject[0] = '\0';
   MsgText.Clear ();

   if (Tcp != NULL) {
      New ();
      sprintf (szBuffer, "RETR %lu\r\n", ulMsg);
      Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));

      if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == TRUE) {
         retVal = TRUE;

         _dos_getdate (&date);
         _dos_gettime (&time);
         Arrived.Day = Written.Day = date.day;
         Arrived.Month = Written.Month = date.month;
         Arrived.Year = Written.Year = (USHORT)date.year;
         Arrived.Hour = Written.Hour = time.hour;
         Arrived.Minute = Written.Minute = time.minute;
         Arrived.Second = Written.Second = time.second;

         do {
            szBuffer[0] = 1;
            GetResponse (&szBuffer[1], (USHORT)(sizeof (szBuffer) - 2));
            if (szBuffer[1] != '\0' && strcmp (szBuffer, "."))
               Text.Add (szBuffer);

            if (!strncmp (&szBuffer[1], "From: ", 6)) {
               if (strlen (&szBuffer[7]) >= sizeof (From))
                  szBuffer[7 + sizeof (From)] = '\0';
               strcpy (From, &szBuffer[7]);
            }
            else if (!strncmp (&szBuffer[1], "Subject: ", 9)) {
               if (strlen (&szBuffer[10]) >= sizeof (Subject))
                  szBuffer[10 + sizeof (Subject)] = '\0';
               strcpy (Subject, &szBuffer[10]);
            }
            else if (!strncmp (&szBuffer[1], "Date: ", 6)) {
               p = strtok (&szBuffer[7], " ");
               if (p != NULL && !isdigit (p[0]))
                  p = strtok (NULL, " ");
               if (p != NULL) {
                  Written.Day = (UCHAR)atoi (p);
                  if ((p = strtok (NULL, " ")) != NULL) {
                     for (i = 0; i < 12; i++)
                        if (strnicmp (MONTHS[i], p, 3) == 0) {
                           Written.Month = (UCHAR)(i + 1);
                           i = 12;
                        }
                  }
                  if ((p = strtok (NULL, " ")) != NULL) {
                     Written.Year = (USHORT)atoi (p);
                     if (Written.Year >= 80 && Written.Year < 100)
                        Written.Year += 1900;
                     else if (Written.Year < 80)
                        Written.Year += 2000;
                  }
                  if ((p = strtok (NULL, " ")) != NULL) {
                     Written.Hour = (UCHAR)((p[0] - '0') * 10 + (p[1] - '0'));
                     p += 2;
                     if (p[0] == ':')
                        p++;
                     Written.Minute = (UCHAR)((p[0] - '0') * 10 + (p[1] - '0'));
                     p += 2;
                     if (p[0] != '\0') {
                        if (p[0] == ':')
                           p++;
                        if (p[0] != '\0')
                           Written.Second = (UCHAR)((p[0] - '0') * 10 + (p[1] - '0'));
                     }
                  }
               }
            }
         } while (szBuffer[1] != '\0' && strcmp (szBuffer, "."));

         Id = LastReaded = ulMsg;
         pLine = szLine;
         nCol = 0;
         SkipNext = FALSE;

         do {
            GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1));
            nReaded = (USHORT)strlen (szBuffer);

            for (i = 0, pBuff = szBuffer; i < nReaded; i++, pBuff++) {
               if (*pBuff != '\n') {
                  *pLine++ = *pBuff;
                  nCol++;
                  if (nCol >= nWidth) {
                     *pLine = '\0';
                     while (nCol > 1 && *pLine != ' ') {
                        nCol--;
                        pLine--;
                     }
                     if (nCol > 0) {
                        while (*pLine == ' ')
                           pLine++;
                        strcpy (szWrp, pLine);
                     }
                     *pLine = '\0';
                     MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
                     strcpy (szLine, szWrp);
                     pLine = strchr (szLine, '\0');
                     nCol = (SHORT)strlen (szLine);
                     SkipNext = TRUE;
                  }
               }
            }
            *pLine = '\0';
            if (pLine > szLine && SkipNext == TRUE) {
               pLine--;
               while (pLine > szLine && *pLine == ' ')
                  *pLine-- = '\0';
               if (pLine > szLine && strcmp (szLine, "."))
                  MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
            }
            else if (SkipNext == FALSE) {
               if (strcmp (szLine, "."))
                  MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
            }
            SkipNext = FALSE;
            pLine = szLine;
            nCol = 0;
         } while (strcmp (szBuffer, "."));
      }
   }

   return (retVal);
}

VOID INETMAIL::SetHWM (ULONG ulMsg)
{
   ulMsg = ulMsg;
}

ULONG INETMAIL::UidToMsgn (ULONG ulMsg)
{
   return (ulMsg);
}

VOID INETMAIL::UnLock (VOID)
{
}

USHORT INETMAIL::WriteHeader (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

