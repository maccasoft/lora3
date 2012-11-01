
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
#include "msgbase.h"

static PSZ GROUP = "GROUP %s\r\n";
static PSZ HEAD  = "HEAD\r\n";
static PSZ LAST  = "LAST\r\n";
static PSZ NEXT  = "NEXT\r\n";
static PSZ POST  = "POST\r\n";
static PSZ QUIT  = "QUIT\r\n";
static PSZ STAT  = "STAT %ld\r\n";
static PSZ ARTICLE = "ARTICLE %ld\r\n";
static PSZ MONTHS[] = {
   "January", "February", "March", "April", "Maj", "Juni",
   "July", "August", "September", "October", "November", "December"
};

USENET::USENET ()
{
   Tcp = new TTcpip;
   ulHighest = ulFirst = ulTotal = 0L;
   LastReaded = 0L;
   strcpy (HostName, "unknown.host");
   strcpy (User, "anonymous");
   strcpy (NewsGroup, "");
   strcpy (Organization, "No Organization");
   strcpy (ProgramID, "LoraBBS");
}

USENET::USENET (PSZ pszServer, PSZ pszGroup)
{
   Tcp = new TTcpip;
   ulHighest = ulFirst = ulTotal = 0L;
   LastReaded = 0L;
   strcpy (HostName, "unknown.host");
   strcpy (NewsGroup, "");
   strcpy (Organization, "No Organization");
   strcpy (ProgramID, "Macca's NNTP Client/Server");
   Open (pszServer, pszGroup);
}

USENET::~USENET ()
{
   if (Tcp != NULL)
      delete Tcp;
}

USHORT USENET::Add ()
{
   return (Add (Text));
}

USHORT USENET::Add (class TMsgBase *MsgBase)
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

USHORT USENET::Add (class TCollection &MsgText)
{
   USHORT retVal = FALSE, Lines;
   USHORT GotPath, GotFrom, GotNews, GotSubject, GotOrg, GotLines;
   USHORT GotXNews;
   CHAR *pszText;

   GotPath = GotFrom = GotNews = GotSubject = GotOrg = FALSE;
   GotLines = GotXNews = FALSE;

   if (Tcp->Carrier () == TRUE) {
      Tcp->SendBytes ((UCHAR *)POST, (USHORT)strlen (POST));
      if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 340) {
         if ((pszText = (CHAR *)MsgText.First ()) != NULL)
            do {
               if (!strncmp (pszText, "\001Path: ", 7)) {
                  Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                  Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                  GotPath = TRUE;
               }
               else if (!strncmp (pszText, "\001From: ", 7)) {
                  Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                  Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                  GotFrom = TRUE;
               }
//               else if (!strncmp (pszText, "\001Newsgroups: ", 13)) {
//                  Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
//                  Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
//                  GotNews = TRUE;
//               }
               else if (!strncmp (pszText, "\001Subject: ", 10)) {
                  Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                  Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                  GotSubject = TRUE;
               }
               else if (!strncmp (pszText, "\001Organization: ", 15)) {
                  Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                  Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                  GotOrg = TRUE;
               }
               else if (!strncmp (pszText, "\001X-Newsreader: ", 15)) {
                  Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                  Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
                  GotXNews = TRUE;
               }
               else if (!strncmp (pszText, "\001References: ", 13) || !strncmp (pszText, "\001Sender: ", 9) || !strncmp (pszText, "\001X-To: ", 7)) {
                  Tcp->BufferBytes ((UCHAR *)&pszText[1], (USHORT)(strlen (pszText) - 1));
                  Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
               }
            } while ((pszText = (CHAR *)MsgText.Next ()) != NULL);

         if (GotPath == FALSE) {
            sprintf (szBuffer, "Path: %s\r\n", HostName);
            Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         }
         if (GotFrom == FALSE) {
            sprintf (szBuffer, "From: %s <%s@%s>\r\n", From, User, HostName);
            Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         }
         if (GotNews == FALSE) {
            sprintf (szBuffer, "Newsgroups: %s\r\n", NewsGroup);
            Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         }
         if (GotSubject == FALSE) {
            sprintf (szBuffer, "Subject: %s\r\n", Subject);
            Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         }
         if (GotOrg == FALSE) {
            sprintf (szBuffer, "Organization: %s\r\n", Organization);
            Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         }
         sprintf (szBuffer, "Date: %d %3.3s %d %02d:%02d:%02d GMT\r\n", Written.Day, MONTHS[Written.Month - 1], Written.Year, Written.Hour, Written.Minute, Written.Second);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));

         if (GotLines == FALSE) {
            Lines = 1;
            if ((pszText = (CHAR *)MsgText.First ()) != NULL)
               do {
                  if (*pszText != 0x01 && strncmp (pszText, "SEEN-BY:", 8))
                     Lines++;
               } while ((pszText = (CHAR *)MsgText.Next ()) != NULL);
            sprintf (szBuffer, "Lines: %d\r\n", Lines);
            Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         }

//         sprintf (szBuffer, "NNTP-Posting-Host: %s\r\n", HostName);
//         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         if (GotXNews == FALSE) {
            sprintf (szBuffer, "X-Newsreader: %s\r\n", ProgramID);
            Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         }
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
         Tcp->BufferBytes ((UCHAR *)".\r\n", 3);
         Tcp->UnbufferBytes ();
         if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 240)
            retVal = TRUE;
      }
      strcpy (Error, szBuffer);
   }

   return (retVal);
}

VOID USENET::Close ()
{
   CHAR String[50];

   Tcp->SendBytes ((UCHAR *)QUIT, (USHORT)strlen (QUIT));
   GetResponse (String, (USHORT)(sizeof (String) - 1));
   ulHighest = ulFirst = ulTotal = 0L;
}

USHORT USENET::Delete (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

USHORT USENET::GetHWM (ULONG &ulMsg)
{
   ulMsg = 0L;

   return (FALSE);
}

USHORT USENET::GetResponse (PSZ pszResponse, USHORT usMaxLen)
{
   USHORT retVal = FALSE, len = 0;
   CHAR c, *pszResp = pszResponse;
   LONG timeout;

   timeout = time (NULL) + 60L;

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
   } while (c != '\r' && Tcp->Carrier () == TRUE && time (NULL) < timeout);

   *pszResp = '\0';
   if (pszResponse[3] == ' ')
      retVal = (USHORT)atoi (pszResponse);

   return (retVal);
}

ULONG USENET::Highest ()
{
   return (ulHighest);
}

USHORT USENET::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG USENET::Lowest ()
{
   return (ulFirst);
}

ULONG USENET::MsgnToUid (ULONG ulMsg)
{
   if (ulMsg >= 1 && ulMsg <= Number ())
      ulMsg = ulMsg + Lowest () - 1L;

   return (ulMsg);
}

VOID USENET::New ()
{
   LastReaded = 0L;
   From[0] = To[0] = Subject[0] = '\0';
   Crash = Direct = FileAttach = FileRequest = Hold = Immediate = FALSE;
   Intransit = KillSent = Local = Private = ReceiptRequest = Received = FALSE;
   Sent = 0;
   memset (&Written, 0, sizeof (Written));
   memset (&Arrived, 0, sizeof (Arrived));
   Original = Reply = 0L;
   Text.Clear ();
}

USHORT USENET::Next (ULONG &ulMsg)
{
   USHORT RetVal = FALSE;
   PSZ p;

   if (Tcp->Carrier () == TRUE) {
      if (LastReaded != ulMsg) {
         sprintf (szBuffer, STAT, ulMsg);
         Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1));
      }
      Tcp->SendBytes ((UCHAR *)NEXT, (USHORT)strlen (NEXT));
      if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 223) {
         if ((p = strtok (szBuffer, " ")) != NULL) {
            if ((p = strtok (NULL, " ")) != NULL) {
               LastReaded = ulMsg = atol (p);
               RetVal = TRUE;
            }
         }
      }
   }

   return (RetVal);
}

ULONG USENET::Number ()
{
   return (ulTotal);
}

USHORT USENET::Open (PSZ pszServer, PSZ pszGroup)
{
   USHORT retVal = FALSE, i;
   CHAR String[128], *p;

   if (Tcp->ConnectServer (pszServer, 119) == TRUE) {
      i = GetResponse (String, (USHORT)(sizeof (String) - 1));
      if (i == 200 || i == 201) {
         sprintf (String, GROUP, pszGroup);
         Tcp->SendBytes ((UCHAR *)String, (USHORT)strlen (String));
         if (GetResponse (String, (USHORT)(sizeof (String) - 1)) == 211) {
            p = strtok (String, " ");
            if ((p = strtok (NULL, " ")) != NULL)
               ulTotal = atoi (p);
            if ((p = strtok (NULL, " ")) != NULL)
               ulFirst = atoi (p);
            if ((p = strtok (NULL, " ")) != NULL)
               ulHighest = atoi (p);
            strcpy (NewsGroup, pszGroup);
            LastReaded = 0L;
            retVal = TRUE;
         }
      }
   }

   return (retVal);
}

VOID USENET::Pack ()
{
}

USHORT USENET::Previous (ULONG &ulMsg)
{
   USHORT RetVal = FALSE;
   PSZ p;

   if (Tcp->Carrier () == TRUE) {
      if (LastReaded != ulMsg) {
         sprintf (szBuffer, STAT, ulMsg);
         Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1));
      }
      Tcp->SendBytes ((UCHAR *)LAST, (USHORT)strlen (LAST));
      if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 223) {
         if ((p = strtok (szBuffer, " ")) != NULL) {
            if ((p = strtok (NULL, " ")) != NULL) {
               LastReaded = ulMsg = atol (p);
               RetVal = TRUE;
            }
         }
      }

      if (RetVal == FALSE) {
         ulMsg--;
         sprintf (szBuffer, STAT, ulMsg);
         Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 223) {
            LastReaded = ulMsg;
            RetVal = TRUE;
         }
      }
   }

   return (RetVal);
}

USHORT USENET::ReadHeader (ULONG ulMsg)
{
   USHORT retVal = FALSE, i;
   UCHAR gotFrom, gotSubject;
   CHAR Temp[128], *p, *a;
   struct dosdate_t date;
   struct dostime_t time;

   if (Tcp->Carrier () == TRUE) {
      New ();
      gotFrom = gotSubject = FALSE;
      Text.Clear ();

      sprintf (szBuffer, STAT, ulMsg);
      Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
      if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 223) {
         Tcp->SendBytes ((UCHAR *)HEAD, (USHORT)strlen (HEAD));
         if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 221) {
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
               if (strcmp (szBuffer, "\001.")) {
                  if (!strncmp (szBuffer, "\001From: ", 7) || !strncmp (szBuffer, "\001To: ", 5))
                     Text.Add (szBuffer);
                  else if (!strncmp (szBuffer, "\001Message-ID: ", 13) || !strncmp (szBuffer, "\001References: ", 13))
                     Text.Add (szBuffer);
               }

               if (!strncmp (&szBuffer[1], "From: ", 6)) {
                  strcpy (Temp, &szBuffer[7]);
                  if (strchr (Temp, '(') != NULL) {
                     if ((p = strtok (Temp, " ")) != NULL) {
                        p = strtok (NULL, "");
                        while (*p == ' ')
                           p++;
                        if (*p == '(') {
                           strcpy (Temp, ++p);
                           p = strchr (Temp, '\0');
                           while (--p > Temp) {
                              if (*p == ')') {
                                 *p = '\0';
                                 break;
                              }
                           }
                           strcpy (From, Temp);
                           strcpy (Temp, &szBuffer[7]);
                           if ((p = strtok (Temp, " ")) != NULL)
                              strcpy (FromAddress, p);
                        }
                        else {
                           strcpy (Temp, &szBuffer[7]);
                           if ((p = strtok (Temp, " ")) != NULL)
                              strcpy (From, p);
                        }
                     }
                  }
                  else if ((p = strchr (Temp, '<')) != NULL) {
                     *p++ = '\0';
                     if ((a = strchr (p, '>')) != NULL)
                        *a = '\0';
                     strcpy (FromAddress, p);
                     p = Temp;
                     if (*p == '"')
                        strcpy (Temp, ++p);
                     p = strchr (Temp, '\0');
                     while (--p > Temp) {
                        if (*p != ' ' && *p != '"')
                           break;
                        *p = '\0';
                     }
                     strcpy (From, Temp);
                  }

                  gotFrom = TRUE;
               }
               else if (!strncmp (&szBuffer[1], "To: ", 4)) {
                  strcpy (Temp, &szBuffer[5]);
                  if (strchr (Temp, '(') != NULL) {
                     if ((p = strtok (Temp, " ")) != NULL) {
                        p = strtok (NULL, "");
                        while (*p == ' ')
                           p++;
                        if (*p == '(') {
                           strcpy (Temp, ++p);
                           p = strchr (Temp, '\0');
                           while (--p > Temp) {
                              if (*p == ')') {
                                 *p = '\0';
                                 break;
                              }
                           }
                           strcpy (To, Temp);
                           strcpy (Temp, &szBuffer[5]);
                           if ((p = strtok (Temp, " ")) != NULL)
                              strcpy (ToAddress, p);
                        }
                        else {
                           strcpy (Temp, &szBuffer[5]);
                           if ((p = strtok (Temp, " ")) != NULL)
                              strcpy (To, p);
                        }
                     }
                  }
                  else if ((p = strchr (Temp, '<')) != NULL) {
                     *p++ = '\0';
                     if ((a = strchr (p, '>')) != NULL)
                        *a = '\0';
                     strcpy (ToAddress, p);
                     p = Temp;
                     if (*p == '"')
                        strcpy (Temp, ++p);
                     p = strchr (Temp, '\0');
                     while (--p > Temp) {
                        if (*p != ' ' && *p != '"')
                           break;
                        *p = '\0';
                     }
                     strcpy (To, Temp);
                  }
               }
               else if (!strncmp (&szBuffer[1], "Subject: ", 9)) {
                  if (strlen (&szBuffer[10]) >= sizeof (Subject))
                     szBuffer[10 + sizeof (Subject)] = '\0';
                  strcpy (Subject, &szBuffer[10]);
                  gotSubject = TRUE;
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
            } while (strcmp (szBuffer, "\001."));
            Id = LastReaded = ulMsg;
         }
      }
      if (gotFrom == FALSE || gotSubject == FALSE)
         retVal = FALSE;
   }

   return (retVal);
}

USHORT USENET::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT USENET::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   USHORT retVal = FALSE, SkipNext;
   SHORT i, nReaded, nCol;
   PSZ p;
   struct dosdate_t date;
   struct dostime_t time;

   MsgText.Clear ();

   if (Tcp != NULL) {
      New ();
      sprintf (szBuffer, ARTICLE, ulMsg);
      Tcp->SendBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));

      if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 220) {
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
               MsgText.Add (szBuffer);

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
                     if (strchr (szLine, ' ') != NULL) {
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
                     }
                     else
                        szWrp[0] = '\0';
                     MsgText.Add (szLine);
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
                  MsgText.Add (szLine);
            }
            else if (SkipNext == FALSE) {
               if (strcmp (szLine, "."))
                  MsgText.Add (szLine);
            }
            SkipNext = FALSE;
            pLine = szLine;
            nCol = 0;
         } while (strcmp (szBuffer, "."));
      }
   }

   return (retVal);
}

/*
USHORT USENET::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   USHORT retVal = FALSE, SkipNext;
   SHORT i, nReaded, nCol;

   MsgText.Clear ();

   if (Tcp != NULL) {
      if (ulMsg != Id) {
         if (ReadHeader (ulMsg) == TRUE) {
            Id = LastReaded = ulMsg;
            retVal = TRUE;
         }
      }
      else
         retVal = TRUE;

      if (retVal == TRUE) {
         pLine = szLine;
         nCol = 0;
         retVal = FALSE;
         SkipNext = FALSE;

         Tcp->SendBytes ((UCHAR *)BODY, (USHORT)strlen (BODY));
         if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 222) {
            retVal = TRUE;
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
   }

   return (retVal);
}
*/

VOID USENET::SetHWM (ULONG ulMsg)
{
   ulMsg = ulMsg;
}

ULONG USENET::UidToMsgn (ULONG ulMsg)
{
   if (ulMsg >= Lowest () && ulMsg <= Highest ())
      ulMsg = ulMsg - Lowest () + 1L;

   return (ulMsg);
}

VOID USENET::UnLock ()
{
}

USHORT USENET::WriteHeader (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}



