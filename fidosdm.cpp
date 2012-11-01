
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 2.99.20
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/07/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

#define MAX_TEXT     2048

PSZ fidoMonths[] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

PSZ fidoDays[] = {
   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

typedef struct {
   ULONG Number;
   CHAR  FileName[16];
} MSGINDEX;

FIDOSDM::FIDOSDM (void)
{
   Id = 0L;
   TotalMsgs = 0L;
   Index.Clear ();
}

FIDOSDM::FIDOSDM (PSZ pszName)
{
   Id = 0L;
   TotalMsgs = 0L;
   Index.Clear ();

   Open (pszName);
}

FIDOSDM::~FIDOSDM (void)
{
   Index.Clear ();
}

USHORT FIDOSDM::Add (VOID)
{
   return (Add (Text));
}

USHORT FIDOSDM::Add (class TMsgBase *MsgBase)
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

   Reply = MsgBase->Reply;
   Original = MsgBase->Original;

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

USHORT FIDOSDM::Add (class TCollection &MsgText)
{
   USHORT RetVal = FALSE;
   PSZ pszText, pszAddress;
   CHAR Temp[128];
   MSGINDEX msgIndex;

   if ((msgIndex.Number = Highest () + 1L) == 1L) {
      if ((pszText = (PSZ)MsgText.First ()) != NULL)
         do {
            if (!strncmp (pszText, "\001PATH", 5) || !strncmp (pszText, "SEEN-BY:", 8)) {
               msgIndex.Number++;
               break;
            }
         } while ((pszText = (PSZ)MsgText.Next ()) != NULL);
   }
   sprintf (msgIndex.FileName, "%lu.msg", msgIndex.Number);

   memset (&msgHdr, 0, sizeof (msgHdr));

   pszAddress = FromAddress;
   if (strchr (pszAddress, ':') != NULL)
      pszAddress = strchr (pszAddress, ':') + 1;
   if (strchr (pszAddress, '/') != NULL) {
      msgHdr.OrigNet = (USHORT)atoi (pszAddress);
      pszAddress = strchr (pszAddress, '/') + 1;
   }
   msgHdr.OrigNode = (USHORT)atoi (pszAddress);

   pszAddress = ToAddress;
   if (strchr (pszAddress, ':') != NULL)
      pszAddress = strchr (pszAddress, ':') + 1;
   if (strchr (pszAddress, '/') != NULL) {
      msgHdr.DestNet = (USHORT)atoi (pszAddress);
      pszAddress = strchr (pszAddress, '/') + 1;
   }
   msgHdr.DestNode = (USHORT)atoi (pszAddress);

   if (Crash == TRUE)
      msgHdr.Attrib |= MSGCRASH;
   if (FileAttach == TRUE)
      msgHdr.Attrib |= MSGFILE;
   if (FileRequest == TRUE)
      msgHdr.Attrib |= MSGFRQ;
   if (Hold == TRUE)
      msgHdr.Attrib |= MSGHOLD;
   if (KillSent == TRUE)
      msgHdr.Attrib |= MSGKILL;
   if (Local == TRUE)
      msgHdr.Attrib |= MSGLOCAL;
   if (Private == TRUE)
      msgHdr.Attrib |= MSGPRIVATE;
   if (ReceiptRequest == TRUE)
      msgHdr.Attrib |= MSGRRQ;
   if (Received == TRUE)
      msgHdr.Attrib |= MSGREAD;
   if (Sent == TRUE)
      msgHdr.Attrib |= MSGSENT;

   msgHdr.Reply = (USHORT)Original;
   msgHdr.Up = (USHORT)Reply;

   strcpy (msgHdr.From, From);
   strcpy (msgHdr.To, To);
   strcpy (msgHdr.Subject, Subject);
   sprintf (msgHdr.Date, "%2d %3.3s %02d  %02d:%02d:%02d", Written.Day, fidoMonths[Written.Month - 1], Written.Year % 100, Written.Hour, Written.Minute, Written.Second);

   sprintf (Temp, "%s%s", BasePath, msgIndex.FileName);
   if ((fp = _fsopen (Temp, "wb", SH_DENYNO)) != NULL) {
      fwrite (&msgHdr, sizeof (msgHdr), 1, fp);

      if ((pszText = (PSZ)MsgText.First ()) != NULL)
         do {
            fwrite (pszText, strlen (pszText), 1, fp);
            fwrite ("\r", 1, 1, fp);
         } while ((pszText = (PSZ)MsgText.Next ()) != NULL);

      fwrite ("\000", 1, 1, fp);
      fclose (fp);

      Index.Add (&msgIndex, sizeof (MSGINDEX));
      TotalMsgs++;

      RetVal = TRUE;
   }

   return (RetVal);
}

VOID FIDOSDM::Close (VOID)
{
   Id = 0L;
   if (fp != NULL) {
      fclose (fp);
      fp = NULL;
   }
   Index.Clear ();
}

USHORT FIDOSDM::Delete (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128];
   MSGINDEX *msgIndex;

   if ((msgIndex = (MSGINDEX *)Index.First ()) != NULL)
      do {
         if (msgIndex->Number == ulMsg) {
            sprintf (Temp, "%s%s", BasePath, msgIndex->FileName);
            unlink (Temp);
            Index.Remove ();
            TotalMsgs--;
            RetVal = TRUE;
         }
      } while ((msgIndex = (MSGINDEX *)Index.Next ()) != NULL);

   return (RetVal);
}

USHORT FIDOSDM::GetHWM (ULONG &ulMsg)
{
   int fd;
   CHAR Temp[128];
   FIDOMSG Hdr;

   ulMsg = 1L;

   sprintf (Temp, "%s1.msg", BasePath);
   if ((fd = sopen (Temp, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      read (fd, &Hdr, sizeof (FIDOMSG));
      ulMsg = Hdr.Up;
      close (fd);
   }

   return (TRUE);
}

ULONG FIDOSDM::Highest (VOID)
{
   ULONG RetVal = 0L;
   MSGINDEX *msgIndex;

   if ((msgIndex = (MSGINDEX *)Index.Last ()) != NULL)
      RetVal = msgIndex->Number;

   return (RetVal);
}

USHORT FIDOSDM::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG FIDOSDM::Lowest (VOID)
{
   ULONG RetVal = 0L;
   MSGINDEX *msgIndex;

   if ((msgIndex = (MSGINDEX *)Index.First ()) != NULL)
      RetVal = msgIndex->Number;

   return (RetVal);
}

ULONG FIDOSDM::MsgnToUid (ULONG ulMsg)
{
   return (ulMsg);
}

VOID FIDOSDM::New (VOID)
{
   From[0] = To[0] = Subject[0] = '\0';
   Crash = Direct = FileAttach = FileRequest = Hold = Immediate = FALSE;
   Intransit = KillSent = Local = Private = ReceiptRequest = Received = FALSE;
   Sent = 0;
   memset (&Written, 0, sizeof (Written));
   Written.Month = 1;
   memset (&Arrived, 0, sizeof (Arrived));
   Arrived.Month = 1;
   Original = Reply = 0L;
   Text.Clear ();
}

USHORT FIDOSDM::Next (ULONG &ulMsg)
{
   USHORT RetVal = FALSE, Found = FALSE;
   MSGINDEX *msgIndex;

   if ((msgIndex = (MSGINDEX *)Index.First ()) != NULL)
      do {
         if (ulMsg == msgIndex->Number) {
            Found = TRUE;
            break;
         }
      } while ((msgIndex = (MSGINDEX *)Index.Next ()) != NULL);

   if (Found == TRUE) {
      if ((msgIndex = (MSGINDEX *)Index.Next ()) != NULL) {
         ulMsg = msgIndex->Number;
         RetVal = TRUE;
      }
   }
   else {
      if ((msgIndex = (MSGINDEX *)Index.First ()) != NULL)
         do {
            if (msgIndex->Number > ulMsg) {
               ulMsg = msgIndex->Number;
               RetVal = TRUE;
               break;
            }
         } while ((msgIndex = (MSGINDEX *)Index.Next ()) != NULL);
   }

   return (RetVal);
}

ULONG FIDOSDM::Number (VOID)
{
   return (TotalMsgs);
}

USHORT FIDOSDM::Open (PSZ pszName)
{
   DIR *dir;
   USHORT RetVal = FALSE, Insert;
   CHAR Temp[128];
   ULONG Current;
   struct dirent *ent;
   MSGINDEX msgIndex, *Check;

   TotalMsgs = 0L;
   Index.Clear ();

   strcpy (Temp, pszName);
   if (Temp[strlen (Temp) - 1] == '\\' || Temp[strlen (Temp) - 1] == '/')
      Temp[strlen (Temp) - 1] = '\0';

   if ((dir = opendir (Temp)) != NULL) {
      RetVal = TRUE;
      while ((ent = readdir (dir)) != NULL) {
         if (strstr (ent->d_name, ".msg") != NULL || strstr (ent->d_name, ".MSG") != NULL) {
            Current = atol (ent->d_name);
            msgIndex.Number = Current;
            strcpy (msgIndex.FileName, ent->d_name);

            if ((Check = (MSGINDEX *)Index.First ()) != NULL) {
               if (Check->Number > msgIndex.Number) {
                  Index.Insert (&msgIndex, sizeof (MSGINDEX));
                  Index.Insert (Check, sizeof (MSGINDEX));
                  Index.First ();
                  Index.Remove ();
               }
               else {
                  Insert = FALSE;
                  while ((Check = (MSGINDEX *)Index.Next ()) != NULL) {
                     if (Check->Number > msgIndex.Number) {
                        Index.Previous ();
                        Index.Insert (&msgIndex, sizeof (MSGINDEX));
                        Insert = TRUE;
                        break;
                     }
                  }
                  if (Insert == FALSE)
                     Index.Add (&msgIndex, sizeof (MSGINDEX));
               }
            }
            else
               Index.Add (&msgIndex, sizeof (MSGINDEX));

            TotalMsgs++;
         }
      }
      closedir (dir);
   }

   strcpy (BasePath, Temp);
#if defined(__LINUX__)
   strcat (BasePath, "/");
#else
   strcat (BasePath, "\\");
#endif

   return (RetVal);
}

VOID FIDOSDM::Pack (VOID)
{
}

USHORT FIDOSDM::Previous (ULONG &ulMsg)
{
   USHORT RetVal = FALSE, Found = FALSE;
   MSGINDEX *msgIndex;

   if ((msgIndex = (MSGINDEX *)Index.First ()) != NULL)
      do {
         if (ulMsg == msgIndex->Number) {
            Found = TRUE;
            break;
         }
      } while ((msgIndex = (MSGINDEX *)Index.Next ()) != NULL);

   if (Found == TRUE) {
      if ((msgIndex = (MSGINDEX *)Index.Previous ()) != NULL) {
         ulMsg = msgIndex->Number;
         RetVal = TRUE;
      }
   }
   else {
      if ((msgIndex = (MSGINDEX *)Index.Last ()) != NULL)
         do {
            if (msgIndex->Number < ulMsg) {
               ulMsg = msgIndex->Number;
               RetVal = TRUE;
               break;
            }
         } while ((msgIndex = (MSGINDEX *)Index.Previous ()) != NULL);
   }

   return (RetVal);
}

USHORT FIDOSDM::ReadHeader (ULONG ulMsg)
{
   int dd, yy, hr, mn, sc;
   USHORT RetVal = FALSE, i;
   CHAR mm[4];
   MSGINDEX *msgIndex;
   struct dosdate_t date;
   struct dostime_t time;

   New ();

   if ((msgIndex = (MSGINDEX *)Index.First ()) != NULL)
      do {
         if (msgIndex->Number == ulMsg) {
            sprintf (LastFile, "%s%s", BasePath, msgIndex->FileName);
            if ((fp = _fsopen (LastFile, "rb", SH_DENYNO)) != NULL) {
               RetVal = TRUE;
               Current = ulMsg;

               memset (&msgHdr, 0, sizeof (msgHdr));
               fread (&msgHdr, sizeof (msgHdr), 1, fp);
               fclose (fp);

               sprintf (FromAddress, "%d/%d", msgHdr.OrigNet, msgHdr.OrigNode);
               sprintf (ToAddress, "%d/%d", msgHdr.DestNet, msgHdr.DestNode);

               for (i = 0; i < 7; i++) {
                  if (!strncmp (fidoDays[i], msgHdr.Date, 3))
                     break;
               }

               if (i < 7) {
                  sscanf (&msgHdr.Date[4], "%2d %3s %2d %2d:%2d", &dd, mm, &yy, &hr, &mn);
                  sc = 0;
               }
               else
                  sscanf (msgHdr.Date, "%2d %3s %2d  %2d:%2d:%02d", &dd, mm, &yy, &hr, &mn, &sc);

               Written.Day = (UCHAR)dd;
               for (i = 0; i < 12; i++) {
                  if (!stricmp (fidoMonths[i], mm)) {
                     Written.Month = (UCHAR)(i + 1);
                     break;
                  }
               }
               if (Written.Month < 1 || Written.Month > 12)
                  Written.Month = 1;
               if ((Written.Year = (USHORT)(yy + 1900)) < 1990)
                  Written.Year += 100;
               Written.Hour = (UCHAR)hr;
               Written.Minute = (UCHAR)mn;
               Written.Second = (UCHAR)sc;

               _dos_getdate (&date);
               _dos_gettime (&time);

               Arrived.Day = date.day;
               Arrived.Month = date.month;
               Arrived.Year = (USHORT)date.year;
               Arrived.Hour = time.hour;
               Arrived.Minute = time.minute;
               Arrived.Second = time.second;

               strcpy (From, msgHdr.From);
               strcpy (To, msgHdr.To);
               strcpy (Subject, msgHdr.Subject);

               Crash = (UCHAR)((msgHdr.Attrib & MSGCRASH) ? TRUE : FALSE);
               FileAttach = (UCHAR)((msgHdr.Attrib & MSGFILE) ? TRUE : FALSE);
               FileRequest = (UCHAR)((msgHdr.Attrib & MSGFRQ) ? TRUE : FALSE);
               Hold = (UCHAR)((msgHdr.Attrib & MSGHOLD) ? TRUE : FALSE);
               KillSent = (UCHAR)((msgHdr.Attrib & MSGKILL) ? TRUE : FALSE);
               Local = (UCHAR)((msgHdr.Attrib & MSGLOCAL) ? TRUE : FALSE);
               Private = (UCHAR)((msgHdr.Attrib & MSGPRIVATE) ? TRUE : FALSE);
               ReceiptRequest = (UCHAR)((msgHdr.Attrib & MSGRRQ) ? TRUE : FALSE);
               Received = (UCHAR)((msgHdr.Attrib & MSGREAD) ? TRUE : FALSE);
               Sent = (UCHAR)((msgHdr.Attrib & MSGSENT) ? TRUE : FALSE);

               Original = msgHdr.Reply;
               Reply = msgHdr.Up;
               break;
            }
         }
      } while ((msgIndex = (MSGINDEX *)Index.Next ()) != NULL);

   return (RetVal);
}

USHORT FIDOSDM::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT FIDOSDM::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   int fz, fn, fo, tz, tn, to, dd, yy, hr, mn, sc;
   USHORT RetVal = FALSE, SkipNext;
   USHORT ToPoint, ToZone, FromPoint, FromZone;
   CHAR mm[4];
   SHORT i, nReaded, nCol;
   MSGINDEX *msgIndex;
   struct dosdate_t date;
   struct dostime_t time;

   New ();
   MsgText.Clear ();
   ToPoint = ToZone = FromPoint = FromZone = 0;
   fp = NULL;

   if ((msgIndex = (MSGINDEX *)Index.First ()) != NULL)
      do {
         if (msgIndex->Number == ulMsg) {
            sprintf (LastFile, "%s%s", BasePath, msgIndex->FileName);
            if ((fp = _fsopen (LastFile, "rb", SH_DENYNO)) != NULL) {
               RetVal = TRUE;
               Current = ulMsg;
               break;
            }
         }
      } while ((msgIndex = (MSGINDEX *)Index.Next ()) != NULL);

   if (RetVal == TRUE && fp != NULL) {
      memset (&msgHdr, 0, sizeof (msgHdr));
      fread (&msgHdr, sizeof (msgHdr), 1, fp);

      for (i = 0; i < 7; i++) {
         if (!strncmp (fidoDays[i], msgHdr.Date, 3))
            break;
      }

      if (i < 7) {
         sscanf (&msgHdr.Date[4], "%2d %3s %2d %2d:%2d", &dd, mm, &yy, &hr, &mn);
         sc = 0;
      }
      else
         sscanf (msgHdr.Date, "%2d %3s %2d  %2d:%2d:%02d", &dd, mm, &yy, &hr, &mn, &sc);

      Written.Day = (UCHAR)dd;
      for (i = 0; i < 12; i++) {
         if (!stricmp (fidoMonths[i], mm)) {
            Written.Month = (UCHAR)(i + 1);
            break;
         }
      }
      if (Written.Month < 1 || Written.Month > 12)
         Written.Month = 1;
      if ((Written.Year = (USHORT)(yy + 1900)) < 1990)
         Written.Year += 100;
      Written.Hour = (UCHAR)hr;
      Written.Minute = (UCHAR)mn;
      Written.Second = (UCHAR)sc;

      _dos_getdate (&date);
      _dos_gettime (&time);

      Arrived.Day = date.day;
      Arrived.Month = date.month;
      Arrived.Year = (USHORT)date.year;
      Arrived.Hour = time.hour;
      Arrived.Minute = time.minute;
      Arrived.Second = time.second;

      strcpy (From, msgHdr.From);
      strcpy (To, msgHdr.To);
      strcpy (Subject, msgHdr.Subject);

      Crash = (UCHAR)((msgHdr.Attrib & MSGCRASH) ? TRUE : FALSE);
      FileAttach = (UCHAR)((msgHdr.Attrib & MSGFILE) ? TRUE : FALSE);
      FileRequest = (UCHAR)((msgHdr.Attrib & MSGFRQ) ? TRUE : FALSE);
      Hold = (UCHAR)((msgHdr.Attrib & MSGHOLD) ? TRUE : FALSE);
      KillSent = (UCHAR)((msgHdr.Attrib & MSGKILL) ? TRUE : FALSE);
      Local = (UCHAR)((msgHdr.Attrib & MSGLOCAL) ? TRUE : FALSE);
      Private = (UCHAR)((msgHdr.Attrib & MSGPRIVATE) ? TRUE : FALSE);
      ReceiptRequest = (UCHAR)((msgHdr.Attrib & MSGRRQ) ? TRUE : FALSE);
      Received = (UCHAR)((msgHdr.Attrib & MSGREAD) ? TRUE : FALSE);
      Sent = (UCHAR)((msgHdr.Attrib & MSGSENT) ? TRUE : FALSE);

      Original = msgHdr.Reply;
      Reply = msgHdr.Up;

      pLine = szLine;
      nCol = 0;
      Current = ulMsg;
      SkipNext = FALSE;

      do {
         nReaded = (SHORT)fread (szBuff, 1, sizeof (szBuff), fp);

         for (i = 0, pBuff = szBuff; i < nReaded && *pBuff != '\0'; i++, pBuff++) {
            if (*pBuff == '\r') {
               *pLine = '\0';
               if (!strncmp (szLine, "\001FMPT ", 6))
                  FromPoint = (USHORT)atoi (&szLine[6]);
               else if (!strncmp (szLine, "\001TOPT ", 6))
                  ToPoint = (USHORT)atoi (&szLine[6]);
               else if (!strncmp (szLine, "\001INTL ", 6)) {
                  sscanf (&szLine[6], "%d:%d/%d %d:%d/%d", &tz, &tn, &to, &fz, &fn, &fo);
                  if (tn == msgHdr.DestNet && to == msgHdr.DestNode)
                     ToZone = (USHORT)tz;
                  if (fn == msgHdr.OrigNet && fo == msgHdr.OrigNode)
                     FromZone = (USHORT)fz;
               }
               else if (!strncmp (szLine, "\001FLAGS ", 7)) {
                  if (strstr (szLine, "DIR"))
                     Direct = TRUE;
               }
               if (pLine > szLine && SkipNext == TRUE) {
                  pLine--;
                  while (pLine > szLine && *pLine == ' ')
                     *pLine-- = '\0';
                  if (pLine > szLine)
                     MsgText.Add (szLine);
               }
               else if (SkipNext == FALSE)
                  MsgText.Add (szLine);
               SkipNext = FALSE;
               pLine = szLine;
               nCol = 0;
            }
            else if (*pBuff != '\n') {
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
      } while (nReaded != 0 && i >= nReaded);

      sprintf (FromAddress, "%u:%u/%u.%u", FromZone, msgHdr.OrigNet, msgHdr.OrigNode, FromPoint);
      sprintf (ToAddress, "%u:%u/%u.%u", ToZone, msgHdr.DestNet, msgHdr.DestNode, ToPoint);

      fclose (fp);
   }

   return (RetVal);
}

VOID FIDOSDM::SetHWM (ULONG ulMsg)
{
   int fd;
   CHAR Temp[128];
   struct dosdate_t d_date;
   struct dostime_t d_time;
   FIDOMSG Hdr;

   sprintf (Temp, "%s1.msg", BasePath);
   if ((fd = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      memset (&Hdr, 0, sizeof (FIDOMSG));
      strcpy (Hdr.From, "MsgBase");
      strcpy (Hdr.To, "Nobody in particular");
      strcpy (Hdr.Subject, "Re: Whatsa high water mark?");
      _dos_getdate (&d_date);
      _dos_gettime (&d_time);
      sprintf (Hdr.Date, "%2d %3.3s %02d  %02d:%02d:%02d", d_date.day, fidoMonths[d_date.month - 1], d_date.year % 100, d_time.hour, d_time.minute, d_time.second);
      Hdr.Up = (USHORT)ulMsg;
      Hdr.Attrib = MSGPRIVATE|MSGSENT|MSGREAD;
      write (fd, &Hdr, sizeof (FIDOMSG));
      strcpy (Temp, "\r\nThis message is used to store the high water mark\r\n");
      write (fd, Temp, strlen (Temp) + 1);
      close (fd);
   }
}

ULONG FIDOSDM::UidToMsgn (ULONG ulMsg)
{
   ULONG RetVal = 0L, Number = 0L;
   MSGINDEX *msgIndex;

   if ((msgIndex = (MSGINDEX *)Index.First ()) != NULL)
      do {
         Number++;
         if (ulMsg == msgIndex->Number) {
            RetVal = Number;
            break;
         }
      } while ((msgIndex = (MSGINDEX *)Index.Next ()) != NULL);

   return (RetVal);
}

VOID FIDOSDM::UnLock (VOID)
{
}

USHORT FIDOSDM::WriteHeader (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   MSGINDEX *msgIndex;

   if ((msgIndex = (MSGINDEX *)Index.First ()) != NULL)
      do {
         if (msgIndex->Number == ulMsg) {
            sprintf (LastFile, "%s%s", BasePath, msgIndex->FileName);
            if ((fp = _fsopen (LastFile, "r+b", SH_DENYNO)) != NULL) {
               RetVal = TRUE;

               memset (&msgHdr, 0, sizeof (msgHdr));
               fread (&msgHdr, sizeof (msgHdr), 1, fp);

               msgHdr.Attrib = 0;
               if (Crash == TRUE)
                  msgHdr.Attrib |= MSGCRASH;
               if (FileAttach == TRUE)
                  msgHdr.Attrib |= MSGFILE;
               if (FileRequest == TRUE)
                  msgHdr.Attrib |= MSGFRQ;
               if (Hold == TRUE)
                  msgHdr.Attrib |= MSGHOLD;
               if (KillSent == TRUE)
                  msgHdr.Attrib |= MSGKILL;
               if (Local == TRUE)
                  msgHdr.Attrib |= MSGLOCAL;
               if (Private == TRUE)
                  msgHdr.Attrib |= MSGPRIVATE;
               if (ReceiptRequest == TRUE)
                  msgHdr.Attrib |= MSGRRQ;
               if (Received == TRUE)
                  msgHdr.Attrib |= MSGREAD;
               if (Sent == TRUE)
                  msgHdr.Attrib |= MSGSENT;

               msgHdr.Reply = (USHORT)Original;
               msgHdr.Up = (USHORT)Reply;

               fseek (fp, 0L, SEEK_SET);
               fwrite (&msgHdr, sizeof (msgHdr), 1, fp);

               fclose (fp);
               break;
            }
         }
      } while ((msgIndex = (MSGINDEX *)Index.Next ()) != NULL);

   return (RetVal);
}

