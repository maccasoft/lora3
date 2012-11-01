
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.5
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    04/22/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

#define MAX_TEXT     2048

PSZ pktMonths[] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

typedef struct {
   ULONG Number;
   ULONG Position;
} PKTINDEX;

PACKET::PACKET (void)
{
   Current = Id = 0L;
   TotalMsgs = 0L;
   Index.Clear ();
   fp = NULL;
   memset (Password, 0, sizeof (Password));
}

PACKET::PACKET (PSZ pszName)
{
   Id = 0L;
   TotalMsgs = 0L;
   Index.Clear ();
   fp = NULL;
   memset (Password, 0, sizeof (Password));

   Open (pszName);
}

PACKET::~PACKET (void)
{
   Close ();
}

USHORT PACKET::Add (VOID)
{
   return (Add (Text));
}

USHORT PACKET::Add (class TMsgBase *MsgBase)
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

USHORT PACKET::Add (class TCollection &MsgText)
{
   int f1, f2, f3, f4, t1, t2, t3, t4;
   USHORT RetVal = TRUE, AddedIntl = FALSE, IsEchomail = FALSE;
   CHAR Temp[64], *pszText, *pszAddress, *p;
   PKTINDEX pktIndex;
   struct dosdate_t d_date;
   struct dostime_t d_time;

   memset (&pktIndex, 0, sizeof (PKTINDEX));
   fflush (fp);
   fseek (fp, filelength (fileno (fp)) - 2L, SEEK_SET);

   pktIndex.Number = TotalMsgs + 1;
   pktIndex.Position = ftell (fp);

   memset (&msgHdr, 0, sizeof (msgHdr));
   msgHdr.Version = 2;

   f1 = f2 = f3 = f4 = 0;
   pszAddress = FromAddress;
   if (strchr (pszAddress, ':') != NULL) {
      f1 = atoi (pszAddress);
      pszAddress = strchr (pszAddress, ':') + 1;
   }
   if (strchr (pszAddress, '/') != NULL) {
      f2 = (USHORT)atoi (pszAddress);
      pszAddress = strchr (pszAddress, '/') + 1;
   }
   f3 = (USHORT)atoi (pszAddress);
   if ((p = strchr (pszAddress, '@')) != NULL)
      *p++ = '\0';
   if (strchr (pszAddress, '.') != NULL) {
      pszAddress = strchr (pszAddress, '.') + 1;
      f4 = (USHORT)atoi (pszAddress);
   }

   msgHdr.OrigNet = (USHORT)f2;
   msgHdr.OrigNode = (USHORT)f3;

   t1 = t2 = t3 = t4 = 0;
   pszAddress = ToAddress;
   if (strchr (pszAddress, ':') != NULL) {
      t1 = atoi (pszAddress);
      pszAddress = strchr (pszAddress, ':') + 1;
   }
   if (strchr (pszAddress, '/') != NULL) {
      t2 = (USHORT)atoi (pszAddress);
      pszAddress = strchr (pszAddress, '/') + 1;
   }
   t3 = (USHORT)atoi (pszAddress);
   if ((p = strchr (pszAddress, '@')) != NULL)
      *p++ = '\0';
   if (strchr (pszAddress, '.') != NULL) {
      pszAddress = strchr (pszAddress, '.') + 1;
      t4 = (USHORT)atoi (pszAddress);
   }

   msgHdr.DestNet = (USHORT)t2;
   msgHdr.DestNode = (USHORT)t3;

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

   fwrite (&msgHdr, sizeof (MSGHDR), 1, fp);

   _dos_getdate (&d_date);
   _dos_gettime (&d_time);

   if (Written.Day == 0)
      Written.Day = d_date.day;
   if (Written.Month == 0)
      Written.Month = d_date.month;
   if (Written.Year == 0)
      Written.Year = (USHORT)d_date.year;
   if (Written.Hour == 0)
      Written.Hour = d_time.hour;
   if (Written.Minute == 0)
      Written.Minute = d_time.minute;
   if (Written.Second == 0)
      Written.Second = d_time.second;

   sprintf (Line, "%2d %3.3s %02d  %02d:%02d:%02d", Written.Day, pktMonths[Written.Month - 1], Written.Year % 100, Written.Hour, Written.Minute, 0);
   fwrite (Line, strlen (Line) + 1, 1, fp);

   fwrite (To, strlen (To) + 1, 1, fp);
   fwrite (From, strlen (From) + 1, 1, fp);
   fwrite (Subject, strlen (Subject) + 1, 1, fp);

   if ((pszText = (PSZ)MsgText.First ()) != NULL) {
      if (!strncmp (pszText, "AREA:", 5))
         IsEchomail = TRUE;
   }

   if (IsEchomail == FALSE) {
      if (f1 != t1) {
         sprintf (Temp, "\001INTL %d:%d/%d %d:%d/%d\r", t1, t2, t3, f1, f2, f3);
         fwrite (Temp, strlen (Temp), 1, fp);
         AddedIntl = TRUE;
      }
      if (f4 != 0) {
         sprintf (Temp, "\001FMPT %u\r", f4);
         fwrite (Temp, strlen (Temp), 1, fp);
      }
      if (t4 != 0) {
         sprintf (Temp, "\001TOPT %u\r", t4);
         fwrite (Temp, strlen (Temp), 1, fp);
      }
   }

   if ((pszText = (PSZ)MsgText.First ()) != NULL)
      do {
         if (IsEchomail == FALSE) {
            if (!strncmp (pszText, "\001FMPT ", 6))
               continue;
            else if (!strncmp (pszText, "\001TOPT ", 6))
               continue;
            else if (!strncmp (pszText, "\001INTL ", 6) && AddedIntl == TRUE)
               continue;
         }
         fwrite (pszText, strlen (pszText), 1, fp);
         fwrite ("\r", 1, 1, fp);
      } while ((pszText = (PSZ)MsgText.Next ()) != NULL);

   fwrite ("\000\000\000", 3, 1, fp);

   Index.Add (&pktIndex, sizeof (PKTINDEX));

   return (RetVal);
}

VOID PACKET::Close (VOID)
{
   Id = 0L;
   if (fp != NULL) {
      fclose (fp);
      fp = NULL;
   }
   Index.Clear ();
}

USHORT PACKET::Delete (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   PKTINDEX *pktIndex;

   if ((pktIndex = (PKTINDEX *)Index.First ()) != NULL)
      do {
         if (pktIndex->Number == ulMsg) {
            Index.Remove ();
            RetVal = TRUE;
         }
      } while ((pktIndex = (PKTINDEX *)Index.Next ()) != NULL);

   return (RetVal);
}

USHORT PACKET::GetHWM (ULONG &ulMsg)
{
   ulMsg = 0L;

   return (FALSE);
}

USHORT PACKET::GetLine (VOID)
{
   int c;
   USHORT Readed = 0, MaybeEOM = FALSE;
   PSZ Ptr = Line;

   if (LastRead == 13) {
      MaybeEOM = TRUE;
      if ((c = fgetc (fp)) >= ' ' || c == 0x01) {
        *Ptr++ = (CHAR)c;
        Readed++;
      }
      else if (c != 10)
         ungetc (c, fp);
   }

   do {
      if (Readed < sizeof (Line) - 1) {
//         if ((c = fgetc (fp)) >= ' ' || c == 0x01) {
         if ((c = fgetc (fp)) != '\0') {
            *Ptr++ = (CHAR)c;
            Readed++;
         }
      }
   } while (Readed < sizeof (Line) - 1 && c != '\0' && c != EOF);
//   } while (Readed < sizeof (Line) - 1 && (c == 1 || c >= ' ') && c != EOF);

   *Ptr = '\0';
   LastRead = (CHAR)c;

   if (MaybeEOM == TRUE && c == 0)
      ungetc (c, fp);

   return ((Readed == 0 && c == EOF) ? FALSE : TRUE);
}

ULONG PACKET::Highest (VOID)
{
   ULONG RetVal = 0L;
   PKTINDEX *pktIndex;

   if ((pktIndex = (PKTINDEX *)Index.Last ()) != NULL)
      RetVal = pktIndex->Number;

   return (RetVal);
}

VOID PACKET::Kill (VOID)
{
   if (fp != NULL) {
      fclose (fp);
      fp = NULL;
   }
   unlink (FileName);
   Index.Clear ();
}

USHORT PACKET::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG PACKET::Lowest (VOID)
{
   ULONG RetVal = 0L;
   PKTINDEX *pktIndex;

   if ((pktIndex = (PKTINDEX *)Index.First ()) != NULL)
      RetVal = pktIndex->Number;

   return (RetVal);
}

ULONG PACKET::MsgnToUid (ULONG ulMsg)
{
   return (ulMsg);
}

VOID PACKET::New (VOID)
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

USHORT PACKET::Next (ULONG &ulMsg)
{
   int c;
   USHORT RetVal = FALSE;
   PKTINDEX *pktIndex;

   if ((pktIndex = (PKTINDEX *)Index.Next ()) != NULL) {
      ulMsg = pktIndex->Number;
      if (ftell (fp) != pktIndex->Position)
         fseek (fp, pktIndex->Position, SEEK_SET);
      RetVal = TRUE;
   }
   else if (Index.Elements == 0) {
      if ((c = fgetc (fp)) != EOF) {
         if (c == 2) {
            Current = ++ulMsg;
            RetVal = TRUE;
         }
         ungetc (c, fp);
      }
/*
      memset (&msgHdr, 0, sizeof (MSGHDR));
      position = ftell (fp);
      fread (&msgHdr, sizeof (MSGHDR), 1, fp);
      fseek (fp, position, SEEK_SET);
      if (msgHdr.Version == 2) {
         Current = ++ulMsg;
         RetVal = TRUE;
      }
*/
   }

   return (RetVal);
}

ULONG PACKET::Number (VOID)
{
   return (TotalMsgs);
}

USHORT PACKET::Open (PSZ pszName, USHORT doScan)
{
   int c;
   USHORT RetVal = FALSE, i;
   PSZ pszAddress;
   ULONG Position;
   PKTINDEX pktIndex;
   struct dosdate_t date;
   struct dostime_t time;

   TotalMsgs = 0L;
   Index.Clear ();

   if (fp != NULL)
      fclose (fp);

   if ((fp = fopen (pszName, "r+b")) == NULL)
      fp = fopen (pszName, "w+b");
   strcpy (FileName, pszName);

   if (fp != NULL) {
      setvbuf (fp, NULL, _IOFBF, 4096L);
      if (filelength (fileno (fp)) == 0L) {
         memset (&pkt2Hdr, 0, sizeof (PKT2HDR));

         _dos_getdate (&date);
         _dos_gettime (&time);

         pkt2Hdr.Version = 2;
         pkt2Hdr.CWValidation = 0x100;
         pkt2Hdr.Capability = 0x0001;
         pkt2Hdr.ProductL = 0x4E;

         pkt2Hdr.Day = date.day;
         pkt2Hdr.Month = (UCHAR)(date.month - 1);
         pkt2Hdr.Year = (USHORT)date.year;
         pkt2Hdr.Hour = time.hour;
         pkt2Hdr.Minute = time.minute;
         pkt2Hdr.Second = time.second;

         pszAddress = ToAddress;
         if (strchr (pszAddress, ':') != NULL) {
            pkt2Hdr.DestZone2 = pkt2Hdr.DestZone = (USHORT)atoi (pszAddress);
            pszAddress = strchr (pszAddress, ':') + 1;
         }
         if (strchr (pszAddress, '/') != NULL) {
            pkt2Hdr.DestNet = (USHORT)atoi (pszAddress);
            pszAddress = strchr (pszAddress, '/') + 1;
         }
         pkt2Hdr.DestNode = (USHORT)atoi (pszAddress);
         if (strchr (pszAddress, '.') != NULL) {
            pszAddress = strchr (pszAddress, '.') + 1;
            pkt2Hdr.DestPoint = (USHORT)atoi (pszAddress);
         }

         pszAddress = FromAddress;
         if (strchr (pszAddress, ':') != NULL) {
            pkt2Hdr.OrigZone2 = pkt2Hdr.OrigZone = (USHORT)atoi (pszAddress);
            pszAddress = strchr (pszAddress, ':') + 1;
         }
         if (strchr (pszAddress, '/') != NULL) {
            pkt2Hdr.OrigNet = (USHORT)atoi (pszAddress);
            pszAddress = strchr (pszAddress, '/') + 1;
         }
         pkt2Hdr.OrigNode = (USHORT)atoi (pszAddress);
         if (strchr (pszAddress, '.') != NULL) {
            pszAddress = strchr (pszAddress, '.') + 1;
            pkt2Hdr.OrigPoint = (USHORT)atoi (pszAddress);
         }

         memcpy (pkt2Hdr.Password, Password, 8);
         fwrite (&pkt2Hdr, sizeof (PKT2HDR), 1, fp);

         memset (&msgHdr, 0, sizeof (MSGHDR));
         fwrite (&msgHdr, 2, 1, fp);

         sprintf (FromAddress, "%d:%d/%d.%d", pkt2Hdr.OrigZone, pkt2Hdr.OrigNet, pkt2Hdr.OrigNode, pkt2Hdr.OrigPoint);
         sprintf (ToAddress, "%d:%d/%d.%d", pkt2Hdr.DestZone, pkt2Hdr.DestNet, pkt2Hdr.DestNode, pkt2Hdr.DestPoint);

         fflush (fp);
         fseek (fp, filelength (fileno (fp)) - sizeof (MSGHDR), SEEK_SET);
         RetVal = TRUE;
      }
      else {
         fread (&pkt2Hdr, sizeof (PKT2HDR), 1, fp);
         if (pkt2Hdr.Version == 2) {
            if (pkt2Hdr.Rate == 2) {
               memcpy (&pkt22Hdr, &pkt2Hdr, sizeof (PKT22HDR));
               sprintf (FromAddress, "%d:%d/%d.%d", pkt22Hdr.OrigZone, pkt22Hdr.OrigNet, pkt22Hdr.OrigNode, pkt22Hdr.OrigPoint);
               if (pkt22Hdr.OrigDomain[0] != '\0') {
                  strcat (FromAddress, "@");
                  strcat (FromAddress, pkt22Hdr.OrigDomain);
               }
               sprintf (ToAddress, "%d:%d/%d.%d", pkt22Hdr.DestZone, pkt22Hdr.DestNet, pkt22Hdr.DestNode, pkt22Hdr.DestPoint);
               if (pkt22Hdr.DestDomain[0] != '\0') {
                  strcat (ToAddress, "@");
                  strcat (ToAddress, pkt22Hdr.DestDomain);
               }
               RetVal = TRUE;
            }
            else {
               swab ((char *)&pkt2Hdr.CWValidation, (char *)&i, 2);
               pkt2Hdr.CWValidation = i;
               if (pkt2Hdr.Capability != pkt2Hdr.CWValidation || !(pkt2Hdr.Capability & 0x0001)) {
                  sprintf (FromAddress, "%d:%d/%d.%d", pkt2Hdr.OrigZone, pkt2Hdr.OrigNet, pkt2Hdr.OrigNode, 0);
                  sprintf (ToAddress, "%d:%d/%d.%d", pkt2Hdr.DestZone, pkt2Hdr.DestNet, pkt2Hdr.DestNode, 0);
               }
               else {
                  sprintf (FromAddress, "%d:%d/%d.%d", pkt2Hdr.OrigZone, pkt2Hdr.OrigNet, pkt2Hdr.OrigNode, pkt2Hdr.OrigPoint);
                  sprintf (ToAddress, "%d:%d/%d.%d", pkt2Hdr.DestZone, pkt2Hdr.DestNet, pkt2Hdr.DestNode, pkt2Hdr.DestPoint);
               }
               memcpy (Password, pkt2Hdr.Password, sizeof (pkt2Hdr.Password));
               Password[sizeof (pkt2Hdr.Password)] = '\0';
               RetVal = TRUE;
            }
         }
      }
   }

   if (RetVal == TRUE) {
      Date.Day = (UCHAR)pkt2Hdr.Day;
      Date.Month = (UCHAR)(pkt2Hdr.Month + 1);
      Date.Year = (USHORT)pkt2Hdr.Year;
      Date.Hour = (UCHAR)pkt2Hdr.Hour;
      Date.Minute = (UCHAR)pkt2Hdr.Minute;
      Date.Second = (UCHAR)pkt2Hdr.Second;
   }

   if (RetVal == TRUE && fp != NULL && doScan == TRUE) {
      do {
         memset (&pktIndex, 0, sizeof (PKTINDEX));
         pktIndex.Position = ftell (fp);

         memset (&msgHdr, 0, sizeof (MSGHDR));
         fread (&msgHdr, sizeof (MSGHDR), 1, fp);

         if (msgHdr.Version == 2) {
            // Date and time
            while ((c = fgetc (fp)) != '\0' && c != EOF)
               ;
            // To
            while ((c = fgetc (fp)) != '\0' && c != EOF)
               ;
            // From
            while ((c = fgetc (fp)) != '\0' && c != EOF)
               ;
            // Subject
            while ((c = fgetc (fp)) != '\0' && c != EOF)
               ;
            // Text body
            while ((c = fgetc (fp)) != '\0' && c != EOF)
               ;

            TotalMsgs++;
            pktIndex.Number = TotalMsgs;
            Index.Add (&pktIndex, sizeof (PKTINDEX));
         }
         else if (msgHdr.Version != 0) {
            while ((c = fgetc (fp)) != EOF && c != 0)
               ;
            if (c == 0) {
               Position = ftell (fp);
               memset (&msgHdr, 0, sizeof (MSGHDR));
               fread (&msgHdr, sizeof (MSGHDR), 1, fp);
               fseek (fp, Position, SEEK_SET);
            }
            else
               msgHdr.Version = 0;
         }
      } while (msgHdr.Version != 0);
   }

   return (RetVal);
}

VOID PACKET::Pack (VOID)
{
}

USHORT PACKET::Previous (ULONG &ulMsg)
{
   USHORT RetVal = FALSE;
   PKTINDEX *pktIndex;

   if ((pktIndex = (PKTINDEX *)Index.Previous ()) != NULL) {
      ulMsg = pktIndex->Number;
      if (ftell (fp) != pktIndex->Position)
         fseek (fp, pktIndex->Position, SEEK_SET);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT PACKET::ReadHeader (ULONG ulMsg)
{
   int dd, yy, hr, mn, sc;
   USHORT RetVal = FALSE, i;
   CHAR mm[4];
   PKTINDEX *pktIndex;
   struct dosdate_t date;
   struct dostime_t time;

   New ();

   if ((pktIndex = (PKTINDEX *)Index.First ()) != NULL) {
      do {
         if (pktIndex->Number == ulMsg) {
            fseek (fp, pktIndex->Position, SEEK_SET);
            break;
         }
      } while ((pktIndex = (PKTINDEX *)Index.Next ()) != NULL);
   }

   memset (&msgHdr, 0, sizeof (MSGHDR));
   fread (&msgHdr, sizeof (MSGHDR), 1, fp);

   if (msgHdr.Version == 2) {
      RetVal = TRUE;

      if (pkt2Hdr.Capability != pkt2Hdr.CWValidation || !(pkt2Hdr.Capability & 0x0001)) {
         sprintf (FromAddress, "%d:%d/%d.%d", pkt2Hdr.OrigZone, msgHdr.OrigNet, msgHdr.OrigNode, 0);
         sprintf (ToAddress, "%d:%d/%d.%d", pkt2Hdr.DestZone, msgHdr.DestNet, msgHdr.DestNode, 0);
      }
      else {
         sprintf (FromAddress, "%d:%d/%d.%d", pkt2Hdr.OrigZone, msgHdr.OrigNet, msgHdr.OrigNode, pkt2Hdr.OrigPoint);
         sprintf (ToAddress, "%d:%d/%d.%d", pkt2Hdr.DestZone, msgHdr.DestNet, msgHdr.DestNode, pkt2Hdr.DestPoint);
      }

      GetLine ();
      sscanf (Line, "%2d %3s %2d  %2d:%2d:%02d", &dd, mm, &yy, &hr, &mn, &sc);

      Written.Day = (UCHAR)dd;
      for (i = 0; i < 12; i++) {
         if (!stricmp (pktMonths[i], mm)) {
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

      GetLine ();
      strcpy (To, Line);
      GetLine ();
      strcpy (From, Line);
      GetLine ();
      strcpy (Subject, Line);
   }

   return (RetVal);
}

USHORT PACKET::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT PACKET::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   int c, f1, f2, f3, f4, t1, t2, t3, t4;
   USHORT RetVal = FALSE;
   SHORT nCol;

   MsgText.Clear ();

   if ((RetVal = ReadHeader (ulMsg)) == TRUE) {
      Current = ulMsg;
      pLine = szLine;
      nCol = 0;

      f1 = pkt2Hdr.OrigZone;
      f2 = msgHdr.OrigNet;
      f3 = msgHdr.OrigNode;
      f4 = pkt2Hdr.OrigPoint;

      t1 = pkt2Hdr.DestZone;
      t2 = msgHdr.DestNet;
      t3 = msgHdr.DestNode;
      t4 = pkt2Hdr.DestPoint;

      sprintf (FromAddress, "%d:%d/%d.%d", f1, f2, f3, f4);
      sprintf (ToAddress, "%d:%d/%d.%d", t1, t2, t3, t4);

      while ((c = fgetc (fp)) != 0 && c != EOF) {
         if (c == '\r') {
            *pLine = '\0';
            if (!strncmp (szLine, "\001FMPT ", 6)) {
               f4 = atoi (&szLine[6]);
               sprintf (FromAddress, "%d:%d/%d.%d", f1, f2, f3, f4);
            }
            else if (!strncmp (szLine, "\001TOPT ", 6)) {
               t4 = atoi (&szLine[6]);
               sprintf (ToAddress, "%d:%d/%d.%d", t1, t2, t3, t4);
            }
            else if (!strncmp (szLine, "\001INTL ", 6)) {
               sscanf (&szLine[6], "%d:%d/%d %d:%d/%d", &t1, &t2, &t3, &f1, &f2, &f3);
               if (t2 != msgHdr.DestNet || t3 != msgHdr.DestNode) {
                  sprintf (szLine, "\001INTL %d:%d/%d %d:%d/%d", t1, t2, t3, f1, f2, f3);
                  MsgText.Add (szLine);
                  t2 = msgHdr.DestNet;
                  t3 = msgHdr.DestNode;
               }
               else {
                  sprintf (FromAddress, "%d:%d/%d.%d", f1, f2, f3, f4);
                  sprintf (ToAddress, "%d:%d/%d.%d", t1, t2, t3, t4);
               }
            }
            else
               MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
            pLine = szLine;
            nCol = 0;
         }
         else if (c != '\n') {
            *pLine++ = (CHAR)c;
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
   }

   return (RetVal);
}

VOID PACKET::SetHWM (ULONG ulMsg)
{
   ulMsg = ulMsg;
}

ULONG PACKET::UidToMsgn (ULONG ulMsg)
{
   return (ulMsg);
}

VOID PACKET::UnLock (VOID)
{
}

USHORT PACKET::WriteHeader (ULONG ulMsg)
{
   USHORT RetVal = FALSE;

   ulMsg = ulMsg;

   return (RetVal);
}

