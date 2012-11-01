
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.03
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

static PSZ BODY  = "BODY\r\n";
static PSZ GROUP = "GROUP %s\r\n";
static PSZ HEAD  = "HEAD\r\n";
static PSZ LAST  = "LAST\r\n";
static PSZ NEXT  = "NEXT\r\n";
static PSZ POST  = "POST\r\n";
static PSZ QUIT  = "QUIT\r\n";
static PSZ STAT  = "STAT %ld\r\n";
static PSZ MONTHS[] = {
   "January", "February", "March", "April", "Maj", "Juni",
   "July", "August", "September", "October", "November", "December"
};

USENET::USENET (void)
{
   Tcp = new TTcpip;
   ulHighest = ulFirst = ulTotal = 0L;
   LastReaded = 0L;
   strcpy (HostName, "unknown.host");
   strcpy (NewsGroup, "");
   strcpy (Organization, "No Organization");
   strcpy (ProgramID, "Macca's NNTP Client/Server");
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

USENET::~USENET (void)
{
   if (Tcp != NULL)
      delete Tcp;
}

USHORT USENET::Add (VOID)
{
   return (Add (Text));
}

USHORT USENET::Add (class TCollection &MsgText)
{
   USHORT retVal = FALSE, Lines;
   CHAR *pszText;

   if (Tcp != NULL) {
      Tcp->BufferBytes ((UCHAR *)POST, (USHORT)strlen (POST));
      if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 340) {
         sprintf (szBuffer, "From %s\r", HostName);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "Path: %s\r", HostName);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "From: %s\r", From);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "Newsgroups: %s\r", NewsGroup);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "Subject: %s\r", Subject);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "Organization: %s\r", Organization);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));

         Lines = 1;
         if (MsgText.First () != NULL)
            do {
               Lines++;
            } while (MsgText.Next () != NULL);
         sprintf (szBuffer, "Lines: %d\r", Lines);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));

         sprintf (szBuffer, "NNTP-Posting-Host: %s\r", HostName);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "X-Newsreader: %s\r", ProgramID);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         Tcp->BufferBytes ((UCHAR *)"\r", 1);

         if ((pszText = (CHAR *)MsgText.First ()) != NULL)
            do {
               if (!strcmp (pszText, "."))
                  Tcp->BufferBytes ((UCHAR *)"..", 2);
               else
                  Tcp->BufferBytes ((UCHAR *)pszText, (USHORT)strlen (pszText));
               Tcp->BufferBytes ((UCHAR *)"\r", 1);
            } while ((pszText = (CHAR *)MsgText.Next ()) != NULL);
         Tcp->BufferBytes ((UCHAR *)".\r", 1);
         if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 240)
            retVal = TRUE;
      }
   }

   return (retVal);
}

VOID USENET::Close (VOID)
{
   CHAR String[50];

   Tcp->BufferBytes ((UCHAR *)QUIT, (USHORT)strlen (QUIT));
   GetResponse (String, (USHORT)(sizeof (String) - 1));
   ulHighest = ulFirst = ulTotal = 0L;
}

USHORT USENET::Delete (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

USHORT USENET::GetResponse (PSZ pszResponse, USHORT usMaxLen)
{
   USHORT retVal = FALSE, len = 0;
   CHAR c, *pszResp = pszResponse;

   do {
      if ((c = (CHAR)Tcp->ReadByte ()) != '\r') {
         if (c != '\n') {
            *pszResp++ = c;
            if (++len >= usMaxLen)
               c = '\r';
         }
      }
   } while (c != '\r');

   *pszResp = '\0';
   if (pszResponse[3] == ' ')
      retVal = (USHORT)atoi (pszResponse);

   return (retVal);
}

ULONG USENET::Highest (VOID)
{
   return (ulHighest);
}

USHORT USENET::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG USENET::Lowest (VOID)
{
   return (ulFirst);
}

VOID USENET::New (VOID)
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

USHORT USENET::Next (ULONG &ulMsg)
{
   USHORT RetVal = FALSE;
   PSZ p;

   if (LastReaded != ulMsg) {
      sprintf (szBuffer, STAT, ulMsg);
      Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
      GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1));
   }
   Tcp->BufferBytes ((UCHAR *)NEXT, (USHORT)strlen (NEXT));
   if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 223) {
      if ((p = strtok (szBuffer, " ")) != NULL) {
         if ((p = strtok (NULL, " ")) != NULL) {
            LastReaded = ulMsg = atol (p);
            RetVal = TRUE;
         }
      }
   }

   return (RetVal);
}

ULONG USENET::Number (VOID)
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
         Tcp->BufferBytes ((UCHAR *)String, (USHORT)strlen (String));
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

VOID USENET::Pack (VOID)
{
}

USHORT USENET::Previous (ULONG &ulMsg)
{
   USHORT RetVal = FALSE;
   PSZ p;

   if (LastReaded != ulMsg) {
      sprintf (szBuffer, STAT, ulMsg);
      Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
      GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1));
   }
   Tcp->BufferBytes ((UCHAR *)LAST, (USHORT)strlen (LAST));
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
      Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
      if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 223) {
         LastReaded = ulMsg;
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT USENET::ReadHeader (ULONG ulMsg)
{
   USHORT retVal = FALSE, i;
   UCHAR gotFrom, gotSubject;
   PSZ p;
   struct dosdate_t date;
   struct dostime_t time;

   New ();
   gotFrom = gotSubject = FALSE;

   sprintf (szBuffer, STAT, ulMsg);
   Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
   if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 223) {
      Tcp->BufferBytes ((UCHAR *)HEAD, (USHORT)strlen (HEAD));
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
            GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1));
            if (!strncmp (szBuffer, "From: ", 6)) {
               if (strlen (&szBuffer[6]) >= sizeof (From))
                  szBuffer[6 + sizeof (From)] = '\0';
               strcpy (From, &szBuffer[6]);
               gotFrom = TRUE;
            }
            else if (!strncmp (szBuffer, "Subject: ", 9)) {
               if (strlen (&szBuffer[9]) >= sizeof (Subject))
                  szBuffer[9 + sizeof (Subject)] = '\0';
               strcpy (Subject, &szBuffer[9]);
               gotSubject = TRUE;
            }
            else if (!strncmp (szBuffer, "Date: ", 6)) {
               p = strtok (&szBuffer[6], " ");
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
         } while (strcmp (szBuffer, "."));
         Id = LastReaded = ulMsg;
      }
   }

   if (gotFrom == FALSE || gotSubject == FALSE)
      retVal = FALSE;

   return (retVal);
}

USHORT USENET::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT USENET::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   USHORT retVal = FALSE;
   SHORT i, nReaded, nCol;
   CHAR szLine[132], szWrp[132], *pLine, *pBuff;

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

         Tcp->BufferBytes ((UCHAR *)BODY, (USHORT)strlen (BODY));
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
                     }
                  }
               }
               *pLine = '\0';
               if (strcmp (szLine, ".")) {
                  if (!strcmp (szLine, ".."))
                     strcpy (szLine, ".");
                  MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
               }
               pLine = szLine;
               nCol = 0;
            } while (strcmp (szBuffer, "."));
         }
      }
   }

   return (retVal);
}

VOID USENET::UnLock (VOID)
{
}

USHORT USENET::WriteHeader (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

USHORT USENET::Write (ULONG ulMsg)
{
   return (Write (ulMsg, Text));
}

USHORT USENET::Write (ULONG ulMsg, class TCollection &MsgText)
{
   USHORT retVal = FALSE, Lines;
   CHAR *pszText;

   if (Tcp != NULL && ulMsg == (Highest () + 1)) {
      Tcp->BufferBytes ((UCHAR *)POST, (USHORT)strlen (POST));
      if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 340) {
         sprintf (szBuffer, "From %s\r", HostName);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "Path: %s\r", HostName);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "From: %s\r", From);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "Newsgroups: %s\r", NewsGroup);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "Subject: %s\r", Subject);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "Organization: %s\r", Organization);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));

         Lines = 1;
         if (MsgText.First () != NULL)
            do {
               Lines++;
            } while (MsgText.Next () != NULL);
         sprintf (szBuffer, "Lines: %d\r", Lines);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));

         sprintf (szBuffer, "NNTP-Posting-Host: %s\r", HostName);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         sprintf (szBuffer, "X-Newsreader: %s\r", ProgramID);
         Tcp->BufferBytes ((UCHAR *)szBuffer, (USHORT)strlen (szBuffer));
         Tcp->BufferBytes ((UCHAR *)"\r", 1);

         if ((pszText = (CHAR *)MsgText.First ()) != NULL)
            do {
               if (!strcmp (pszText, "."))
                  Tcp->BufferBytes ((UCHAR *)"..", 2);
               else
                  Tcp->BufferBytes ((UCHAR *)pszText, (USHORT)strlen (pszText));
               Tcp->BufferBytes ((UCHAR *)"\r", 1);
            } while ((pszText = (CHAR *)MsgText.Next ()) != NULL);
         Tcp->BufferBytes ((UCHAR *)".\r", 1);
         if (GetResponse (szBuffer, (USHORT)(sizeof (szBuffer) - 1)) == 240)
            retVal = TRUE;
      }
   }

   return (retVal);
}



