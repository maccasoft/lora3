
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

#define MAX_TEXT     2048

JAM::JAM (void)
{
   fdHdr = fdJdt = fdJdx = -1;
   pSubfield = NULL;
   Id = 0L;
}

JAM::JAM (PSZ pszName)
{
   fdHdr = fdJdt = fdJdx = -1;
   pSubfield = NULL;
   Id = 0L;

   Open (pszName);
}

JAM::~JAM (void)
{
   Close ();
}

USHORT JAM::Add (VOID)
{
   return (Add (Text));
}

USHORT JAM::Add (class TCollection &MsgText)
{
   USHORT RetVal = FALSE;
   ULONG ulMsg = Highest () + 1L;
   PSZ pszText, pszSign = HEADERSIGNATURE;
   JAMIDXREC jamIdx;
   JAMSUBFIELD jamSubfield;
   struct tm mktm;

   RetVal = TRUE;

   memset (&jamHdr, 0, sizeof (JAMHDR));
   jamHdr.Signature[0] = pszSign[0];
   jamHdr.Signature[1] = pszSign[1];
   jamHdr.Signature[2] = pszSign[2];
   jamHdr.Signature[3] = pszSign[3];
   jamHdr.Revision = CURRENTREVLEV;
   jamHdr.MsgNum = ulMsg;

   mktm.tm_year = Written.Year - 1900;
   mktm.tm_mon = Written.Month - 1;
   mktm.tm_mday = Written.Day;
   mktm.tm_hour = Written.Hour;
   mktm.tm_min = Written.Minute;
   mktm.tm_sec = Written.Second;
   mktm.tm_wday = 0;
   mktm.tm_yday = 0;
   mktm.tm_isdst = 0;
   jamHdr.DateWritten = mktime (&mktm);

   mktm.tm_year = Arrived.Year - 1900;
   mktm.tm_mon = Arrived.Month - 1;
   mktm.tm_mday = Arrived.Day;
   mktm.tm_hour = Arrived.Hour;
   mktm.tm_min = Arrived.Minute;
   mktm.tm_sec = Arrived.Second;
   mktm.tm_wday = 0;
   mktm.tm_yday = 0;
   mktm.tm_isdst = 0;
   jamHdr.DateProcessed = mktime (&mktm);

   lseek (fdHdr, 0L, SEEK_END);

   jamIdx.UserCRC = 0;
   jamIdx.HdrOffset = tell (fdHdr);
   lseek (fdJdx, 0L, SEEK_END);
   write (fdJdx, &jamIdx, sizeof (JAMIDXREC));

   write (fdHdr, &jamHdr, sizeof (JAMHDR));

   jamSubfield.HiID = 0;
   jamSubfield.LoID = JAMSFLD_SENDERNAME;
   jamSubfield.DatLen = strlen (From) + 1;
   jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
   write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
   write (fdHdr, From, strlen (From) + 1);
   if (To[0]) {
      jamSubfield.LoID = JAMSFLD_RECVRNAME;
      jamSubfield.DatLen = strlen (To) + 1;
      jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
      write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
      write (fdHdr, To, strlen (To) + 1);
   }
   jamSubfield.LoID = JAMSFLD_SUBJECT;
   jamSubfield.DatLen = strlen (Subject) + 1;
   jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
   write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
   write (fdHdr, Subject, strlen (Subject) + 1);
   if (FromAddress[0] != '\0') {
      jamSubfield.LoID = JAMSFLD_OADDRESS;
      jamSubfield.DatLen = strlen (FromAddress) + 1;
      jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
      write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
      write (fdHdr, FromAddress, strlen (FromAddress) + 1);
   }
   if (ToAddress[0] != '\0') {
      jamSubfield.LoID = JAMSFLD_DADDRESS;
      jamSubfield.DatLen = strlen (ToAddress) + 1;
      jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
      write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
      write (fdHdr, ToAddress, strlen (ToAddress) + 1);
   }

   if ((pszText = (PSZ)MsgText.First ()) != NULL)
      do {
         if (pszText[0] == 0x01) {
            jamHdr.TxtLen += strlen (pszText);
            jamSubfield.LoID = JAMSFLD_UNKNOWN;
            jamSubfield.DatLen = strlen (pszText) + 1;
            jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
            write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
            write (fdHdr, pszText, strlen (pszText) + 1);
         }
         else if (!strnicmp (pszText, "SEEN-BY:", 8)) {
            pszText += 8;
            jamHdr.TxtLen += strlen (pszText);
            jamSubfield.LoID = JAMSFLD_SEENBY2D;
            jamSubfield.DatLen = strlen (pszText) + 1;
            jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
            write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
            write (fdHdr, pszText, strlen (pszText) + 1);
         }
      } while ((pszText = (PSZ)MsgText.Next ()) != NULL);

   lseek (fdHdr, 0L, SEEK_SET);
   read (fdHdr, &jamHdrInfo, sizeof (JAMHDRINFO));
   jamHdrInfo.ActiveMsgs++;
   lseek (fdHdr, 0L, SEEK_SET);
   write (fdHdr, &jamHdrInfo, sizeof (JAMHDRINFO));

   lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
   write (fdHdr, &jamHdr, sizeof (JAMHDR));

   if (RetVal == TRUE) {
      Id = jamHdr.MsgNum;

      lseek (fdJdt, 0L, SEEK_END);
      jamHdr.TxtOffset = tell (fdJdt);
      jamHdr.TxtLen = 0;

      if ((pszText = (PSZ)MsgText.First ()) != NULL)
         do {
            if (pszText[0] != 0x01 && strnicmp (pszText, "SEEN-BY:", 8)) {
               write (fdJdt, pszText, strlen (pszText));
               jamHdr.TxtLen += strlen (pszText);
               write (fdJdt, "\r\n", 2);
               jamHdr.TxtLen += 2;
            }
         } while ((pszText = (PSZ)MsgText.Next ()) != NULL);

      lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
      write (fdHdr, &jamHdr, sizeof (JAMHDR));

      WriteHeader (ulMsg);
   }

   return (RetVal);
}

VOID JAM::Close (VOID)
{
   if (fdJdx != -1)
      close (fdJdx);
   if (fdJdt != -1)
      close (fdJdt);
   if (fdHdr != -1)
      close (fdHdr);

   if (pSubfield != NULL)
      free (pSubfield);

   fdHdr = fdJdt = fdJdx = -1;
   pSubfield = NULL;
   Id = 0L;
}

USHORT JAM::Delete (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   JAMIDXREC jamIdx;

   if (ReadHeader (ulMsg) == TRUE) {
      jamHdr.Attribute |= MSG_DELETED;

      lseek (fdJdx, tell (fdJdx) - sizeof (jamIdx), SEEK_SET);
      if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
         lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
         write (fdHdr, &jamHdr, sizeof (JAMHDR));

         lseek (fdHdr, 0L, SEEK_SET);
         read (fdHdr, &jamHdrInfo, sizeof (JAMHDRINFO));
         jamHdrInfo.ActiveMsgs--;
         lseek (fdHdr, 0L, SEEK_SET);
         write (fdHdr, &jamHdrInfo, sizeof (JAMHDRINFO));
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

ULONG JAM::Highest (VOID)
{
   ULONG RetVal = 0L;
   JAMIDXREC jamIdx;

   if (jamHdrInfo.ActiveMsgs > 0L) {
      lseek (fdJdx, filelength (fdJdx) - sizeof (jamIdx), SEEK_SET);
      if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
         lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
         read (fdHdr, &jamHdr, sizeof (JAMHDR));
         RetVal = jamHdr.MsgNum;
      }
   }

   Id = RetVal;

   return (RetVal);
}

USHORT JAM::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG JAM::Lowest (VOID)
{
   ULONG RetVal = 0L;
   JAMIDXREC jamIdx;

   if (jamHdrInfo.ActiveMsgs > 0L) {
      lseek (fdJdx, 0L, SEEK_SET);
      if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
         lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
         read (fdHdr, &jamHdr, sizeof (JAMHDR));
         RetVal = jamHdr.MsgNum;
      }
   }

   Id = RetVal;

   return (RetVal);
}

VOID JAM::New (VOID)
{
   From[0] = To[0] = Subject[0] = '\0';
   Crash = Direct = FileAttach = FileRequest = Hold = Immediate = FALSE;
   Intransit = KillSent = Local = Private = ReceiptRequest = Received = FALSE;
   Sent = 0;
   memset (&Written, 0, sizeof (Written));
   memset (&Arrived, 0, sizeof (Arrived));
   FromAddress[0] = ToAddress[0] = '\0';
   Text.Clear ();
}

USHORT JAM::Next (ULONG &ulMsg)
{
   USHORT RetVal = FALSE, MayBeNext = FALSE;
   JAMIDXREC jamIdx;

   if (jamHdrInfo.ActiveMsgs > 0L) {
// --------------------------------------------------------------------
// The first attempt to retrive the next message number suppose that
// the file pointers are located after the current message number.
// Usually this is the 99% of the situations because the messages are
// often readed sequentially.
// --------------------------------------------------------------------
      if (tell (fdJdx) >= sizeof (jamIdx))
         lseek (fdJdx, tell (fdJdx) - sizeof (jamIdx), SEEK_SET);
      do {
         if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
            lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
            read (fdHdr, &jamHdr, sizeof (JAMHDR));
            if (MayBeNext == TRUE) {
               if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum > ulMsg) {
                  ulMsg = jamHdr.MsgNum;
                  RetVal = TRUE;
               }
            }
            if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum == ulMsg)
               MayBeNext = TRUE;
         }
      } while (RetVal == FALSE && tell (fdJdx) < filelength (fdJdx));

      if (RetVal == FALSE && MayBeNext == FALSE) {
// --------------------------------------------------------------------
// It seems that the file pointers are not located where they should be
// so our next attempt is to scan the database from the beginning to
// find the next message number.
// --------------------------------------------------------------------
         lseek (fdJdx, 0L, SEEK_SET);
         do {
            if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
               lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
               read (fdHdr, &jamHdr, sizeof (JAMHDR));
               if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum > ulMsg) {
                  ulMsg = jamHdr.MsgNum;
                  RetVal = TRUE;
               }
            }
         } while (RetVal == FALSE && tell (fdJdx) < filelength (fdJdx));
      }

      Id = 0L;
      if (RetVal == TRUE)
         Id = ulMsg;
   }

   return (RetVal);
}

ULONG JAM::Number (VOID)
{
   return (jamHdrInfo.ActiveMsgs);
}

USHORT JAM::Open (PSZ pszName)
{
   USHORT RetVal = FALSE;
   CHAR File[128];
   PSZ Signature = HEADERSIGNATURE;

   sprintf (File, "%s%s", pszName, EXT_HDRFILE);
   if ((fdHdr = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if (read (fdHdr, &jamHdrInfo, sizeof (JAMHDRINFO)) != sizeof (JAMHDRINFO)) {
         memset (&jamHdrInfo, 0, sizeof (JAMHDRINFO));
         jamHdrInfo.Signature[0] = Signature[0];
         jamHdrInfo.Signature[1] = Signature[1];
         jamHdrInfo.Signature[2] = Signature[2];
         jamHdrInfo.Signature[3] = Signature[3];
         jamHdrInfo.DateCreated = time (NULL);
         jamHdrInfo.BaseMsgNum = 1;

         lseek (fdHdr, 0L, SEEK_SET);
         write (fdHdr, &jamHdrInfo, sizeof (JAMHDRINFO));
      }

      if (jamHdrInfo.Signature[0] == Signature[0] && jamHdrInfo.Signature[1] == Signature[1] && jamHdrInfo.Signature[2] == Signature[2] && jamHdrInfo.Signature[3] == Signature[3]) {
         sprintf (File, "%s%s", pszName, EXT_TXTFILE);
         fdJdt = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
         sprintf (File, "%s%s", pszName, EXT_IDXFILE);
         fdJdx = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
         RetVal = TRUE;

         strcpy (BaseName, pszName);
      }
      else {
         close (fdHdr);
         fdHdr = -1;
      }
   }

   Id = 0L;

   return (RetVal);
}

VOID JAM::Pack (VOID)
{
   int fdnHdr, fdnJdx, fdnJdt;
   USHORT ToRead, Readed;
   CHAR File[128], New[128], *Subfield, *Temp;
   JAMIDXREC jamIdx;

   sprintf (File, "%s%s", BaseName, ".$dr");
   fdnHdr = open (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
   sprintf (File, "%s%s", BaseName, ".$dt");
   fdnJdt = open (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
   sprintf (File, "%s%s", BaseName, ".$dx");
   fdnJdx = open (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   if (fdnHdr != -1 && fdnJdt != -1 && fdnJdx != -1) {
      lseek (fdHdr, 0L, SEEK_SET);
      if (read (fdHdr, &jamHdrInfo, sizeof (JAMHDRINFO)) == sizeof (JAMHDRINFO)) {
         write (fdnHdr, &jamHdrInfo, sizeof (JAMHDRINFO));
         while (read (fdHdr, &jamHdr, sizeof (JAMHDR)) == sizeof (JAMHDR)) {
            if (jamHdr.Attribute & MSG_DELETED) {
               if (jamHdr.SubfieldLen > 0L)
                  lseek (fdHdr, jamHdr.SubfieldLen, SEEK_CUR);
            }
            else {
               jamIdx.UserCRC = 0;
               jamIdx.HdrOffset = tell (fdnHdr);
               write (fdnJdx, &jamIdx, sizeof (JAMIDXREC));

               lseek (fdJdt, jamHdr.TxtOffset, SEEK_SET);
               jamHdr.TxtOffset = tell (fdnJdt);
               write (fdnHdr, &jamHdr, sizeof (JAMHDR));

               if (jamHdr.SubfieldLen > 0L) {
                  if ((Subfield = (CHAR *)malloc (jamHdr.SubfieldLen + 1)) != NULL) {
                     read (fdHdr, Subfield, jamHdr.SubfieldLen);
                     write (fdnHdr, Subfield, jamHdr.SubfieldLen);
                     free (Subfield);
                  }
               }

               if ((Temp = (CHAR *)malloc (MAX_TEXT)) != NULL) {
                  do {
                     if ((ToRead = MAX_TEXT) > jamHdr.TxtLen)
                        ToRead = (USHORT)jamHdr.TxtLen;
                     Readed = (USHORT)read (fdJdt, Temp, ToRead);
                     write (fdnJdt, Temp, Readed);
                     jamHdr.TxtLen -= Readed;
                  } while (jamHdr.TxtLen > 0);
                  free (Temp);
               }
            }
         }
      }

      close (fdnHdr);
      close (fdnJdt);
      close (fdnJdx);
      fdnHdr = fdnJdt = fdnJdx = -1;

      close (fdHdr);
      close (fdJdt);
      close (fdJdx);
      fdHdr = fdJdt = fdJdx = -1;

      sprintf (File, "%s%s", BaseName, ".$dr");
      sprintf (New, "%s%s", BaseName, EXT_HDRFILE);
      unlink (New);
      rename (File, New);
      sprintf (File, "%s%s", BaseName, ".$dt");
      sprintf (New, "%s%s", BaseName, EXT_TXTFILE);
      unlink (New);
      rename (File, New);
      sprintf (File, "%s%s", BaseName, ".$dx");
      sprintf (New, "%s%s", BaseName, EXT_IDXFILE);
      unlink (New);
      rename (File, New);

      Open (BaseName);
   }

   if (fdnHdr != -1) {
      close (fdnHdr);
      sprintf (File, "%s%s", BaseName, ".$dr");
      unlink (File);
   }
   if (fdnJdt != -1) {
      close (fdnJdt);
      sprintf (File, "%s%s", BaseName, ".$dt");
      unlink (File);
   }
   if (fdnJdx != -1) {
      close (fdnJdx);
      sprintf (File, "%s%s", BaseName, ".$dx");
      unlink (File);
   }
}

USHORT JAM::Previous (ULONG &ulMsg)
{
   USHORT RetVal = FALSE, MayBeNext = FALSE;
   LONG Pos;
   JAMIDXREC jamIdx;

   if (jamHdrInfo.ActiveMsgs > 0L) {
// --------------------------------------------------------------------
// The first attempt to retrive the next message number suppose that
// the file pointers are located after the current message number.
// Usually this is the 99% of the situations because the messages are
// often readed sequentially.
// --------------------------------------------------------------------
      if (tell (fdJdx) >= sizeof (jamIdx)) {
         Pos = tell (fdJdx) - sizeof (jamIdx);
         do {
            lseek (fdJdx, Pos, SEEK_SET);
            if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
               lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
               read (fdHdr, &jamHdr, sizeof (JAMHDR));
               if (MayBeNext == TRUE) {
                  if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum < ulMsg) {
                     ulMsg = jamHdr.MsgNum;
                     RetVal = TRUE;
                  }
               }
               if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum == ulMsg)
                  MayBeNext = TRUE;
            }
            Pos -= sizeof (jamIdx);
         } while (RetVal == FALSE && Pos >= 0L);
      }

      if (RetVal == FALSE && MayBeNext == FALSE) {
// --------------------------------------------------------------------
// It seems that the file pointers are not located where they should be
// so our next attempt is to scan the database from the end to find
// the next message number.
// --------------------------------------------------------------------
         Pos = filelength (fdJdx) - sizeof (jamIdx);
         do {
            lseek (fdJdx, Pos, SEEK_SET);
            if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
               lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
               read (fdHdr, &jamHdr, sizeof (JAMHDR));
               if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum < ulMsg) {
                  ulMsg = jamHdr.MsgNum;
                  RetVal = TRUE;
               }
            }
            Pos -= sizeof (jamIdx);
         } while (RetVal == FALSE && Pos >= 0L);
      }

      Id = 0L;
      if (RetVal == TRUE)
         Id = ulMsg;
   }

   return (RetVal);
}

USHORT JAM::ReadHeader (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   UCHAR *pPos;
   ULONG ulSubfieldLen;
   struct tm *local;
   JAMIDXREC jamIdx;
   JAMBINSUBFIELD *jamSubField;

   New ();

   if (Id == ulMsg) {
// --------------------------------------------------------------------
// The user is requesting the header of the last message retrived
// so our first attempt is to read the last index from the file and
// check if this is the correct one.
// --------------------------------------------------------------------
      lseek (fdJdx, tell (fdJdx) - sizeof (jamIdx), SEEK_SET);
      if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
         lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
         read (fdHdr, &jamHdr, sizeof (JAMHDR));
         if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum == ulMsg)
            RetVal = TRUE;
      }
   }

   if (RetVal == FALSE) {
// --------------------------------------------------------------------
// The message request is not the last retrived or the file pointers
// are not positioned where they should be, so now we attempt to
// retrive the message header scanning the database from the beginning.
// --------------------------------------------------------------------
      Id = 0L;
      lseek (fdJdx, 0L, SEEK_SET);
      do {
         if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
            lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
            read (fdHdr, &jamHdr, sizeof (JAMHDR));
            if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum == ulMsg)
               RetVal = TRUE;
         }
      } while (RetVal == FALSE && tell (fdJdx) < filelength (fdJdx));
   }

   if (RetVal == TRUE) {
      Id = ulMsg;
      Crash = (jamHdr.Attribute & MSG_CRASH) ? TRUE : FALSE;
      Direct = (jamHdr.Attribute & MSG_DIRECT) ? TRUE : FALSE;
      FileAttach = (jamHdr.Attribute & MSG_FILEATTACH) ? TRUE : FALSE;
      FileRequest = (jamHdr.Attribute & MSG_FILEREQUEST) ? TRUE : FALSE;
      Hold = (jamHdr.Attribute & MSG_HOLD) ? TRUE : FALSE;
      Immediate = (jamHdr.Attribute & MSG_IMMEDIATE) ? TRUE : FALSE;
      Intransit = (jamHdr.Attribute & MSG_INTRANSIT) ? TRUE : FALSE;
      KillSent = (jamHdr.Attribute & MSG_KILLSENT) ? TRUE : FALSE;
      Local = (jamHdr.Attribute & MSG_LOCAL) ? TRUE : FALSE;
      Private = (jamHdr.Attribute & MSG_PRIVATE) ? TRUE : FALSE;
      ReceiptRequest = (jamHdr.Attribute & MSG_RECEIPTREQ) ? TRUE : FALSE;
      Received = (jamHdr.Attribute & MSG_READ) ? TRUE : FALSE;
      Sent = (jamHdr.Attribute & MSG_SENT) ? TRUE : FALSE;

      local = localtime ((const time_t *)&jamHdr.DateWritten);
      Written.Day = (UCHAR)local->tm_mday;
      Written.Month = (UCHAR)(local->tm_mon + 1);
      Written.Year = (USHORT)(local->tm_year + 1900);
      Written.Hour = (UCHAR)local->tm_hour;
      Written.Minute = (UCHAR)local->tm_min;
      Written.Second = (UCHAR)local->tm_sec;

      local = localtime ((const time_t *)&jamHdr.DateReceived);
      Arrived.Day = (UCHAR)local->tm_mday;
      Arrived.Month = (UCHAR)(local->tm_mon + 1);
      Arrived.Year = (USHORT)(local->tm_year + 1900);
      Arrived.Hour = (UCHAR)local->tm_hour;
      Arrived.Minute = (UCHAR)local->tm_min;
      Arrived.Second = (UCHAR)local->tm_sec;

      if (pSubfield != NULL)
         free (pSubfield);
      pSubfield = NULL;

      if (jamHdr.SubfieldLen > 0L) {
         ulSubfieldLen = jamHdr.SubfieldLen;
         pPos = pSubfield = (UCHAR *)malloc (ulSubfieldLen + 1);
         if (pSubfield == NULL)
            return (FALSE);

         read (fdHdr, pSubfield, jamHdr.SubfieldLen);

         while (ulSubfieldLen > 0L) {
            jamSubField = (JAMBINSUBFIELD *)pPos;
            pPos += sizeof (JAMBINSUBFIELD);

            switch (jamSubField->LoID) {
               case JAMSFLD_SENDERNAME:
                  memcpy (From, pPos, (INT)jamSubField->DatLen);
                  From[(INT)jamSubField->DatLen] = '\0';
                  break;

               case JAMSFLD_RECVRNAME:
                  memcpy (To, pPos, (INT)jamSubField->DatLen);
                  To[(INT)jamSubField->DatLen] = '\0';
                  break;

               case JAMSFLD_SUBJECT:
                  memcpy (Subject, pPos, (INT)jamSubField->DatLen);
                  Subject[(INT)jamSubField->DatLen] = '\0';
                  break;

               case JAMSFLD_OADDRESS:
                  memcpy (FromAddress, pPos, (INT)jamSubField->DatLen);
                  FromAddress[(INT)jamSubField->DatLen] = '\0';
                  break;

               case JAMSFLD_DADDRESS:
                  memcpy (ToAddress, pPos, (INT)jamSubField->DatLen);
                  ToAddress[(INT)jamSubField->DatLen] = '\0';
                  break;

               default:
                  break;
            }

            ulSubfieldLen -= sizeof (JAMBINSUBFIELD) + jamSubField->DatLen;
            if (ulSubfieldLen > 0)
               pPos += jamSubField->DatLen;
         }
      }
   }

   return (RetVal);
}

USHORT JAM::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT JAM::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   USHORT RetVal = FALSE;
   SHORT i, nReaded, nCol, nRead;
   CHAR szBuff[128], szLine[132], szWrp[132], *pLine, *pBuff;
   ULONG ulTxtLen;

   MsgText.Clear ();

   if ((RetVal = ReadHeader (ulMsg)) == TRUE) {
      lseek (fdJdt, jamHdr.TxtOffset, SEEK_SET);
      ulTxtLen = jamHdr.TxtLen;
      pLine = szLine;
      nCol = 0;

      do {
         if ((nRead = sizeof (szBuff)) > ulTxtLen)
            nRead = (SHORT)ulTxtLen;

         nReaded = (SHORT)read (fdJdt, szBuff, nRead);

         for (i = 0, pBuff = szBuff; i < nReaded; i++, pBuff++) {
            if (*pBuff == '\r') {
               *pLine = '\0';
               MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
               pLine = szLine;
               nCol = 0;
            }
            else if (*pBuff != '\n') {
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

         ulTxtLen -= nRead;
      } while (ulTxtLen > 0);
   }

   return (RetVal);
}

VOID JAM::UnLock (VOID)
{
}

USHORT JAM::WriteHeader (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   JAMIDXREC jamIdx;

   if (Id == ulMsg) {
// --------------------------------------------------------------------
// The user is requesting to write the header of the last message
// retrived so our first attempt is to read the last index from the
// file and check if this is the correct one.
// --------------------------------------------------------------------
      lseek (fdJdx, tell (fdJdx) - sizeof (jamIdx), SEEK_SET);
      if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
         lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
         read (fdHdr, &jamHdr, sizeof (JAMHDR));
         if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum == ulMsg)
            RetVal = TRUE;
      }
   }

   if (RetVal == FALSE) {
// --------------------------------------------------------------------
// The message requested is not the last retrived or the file pointers
// are not positioned where they should be, so now we attempt to
// retrive the message header scanning the database from the beginning.
// --------------------------------------------------------------------
      Id = 0L;
      lseek (fdJdx, 0L, SEEK_SET);
      do {
         if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
            lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
            read (fdHdr, &jamHdr, sizeof (JAMHDR));
            if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum == ulMsg)
               RetVal = TRUE;
         }
      } while (RetVal == FALSE && tell (fdJdx) < filelength (fdJdx));
   }

   if (RetVal == TRUE) {
      Id = jamHdr.MsgNum;
      jamHdr.Attribute &= MSG_DELETED;
      jamHdr.Attribute |= (Crash == TRUE) ? MSG_CRASH : 0;
      jamHdr.Attribute |= (Direct == TRUE) ? MSG_DIRECT : 0;
      jamHdr.Attribute |= (FileAttach == TRUE) ? MSG_FILEATTACH : 0;
      jamHdr.Attribute |= (FileRequest == TRUE) ? MSG_FILEREQUEST : 0;
      jamHdr.Attribute |= (Hold == TRUE) ? MSG_HOLD : 0;
      jamHdr.Attribute |= (Immediate == TRUE) ? MSG_IMMEDIATE : 0;
      jamHdr.Attribute |= (Intransit == TRUE) ? MSG_INTRANSIT : 0;
      jamHdr.Attribute |= (KillSent == TRUE) ? MSG_KILLSENT : 0;
      jamHdr.Attribute |= (Local == TRUE) ? MSG_LOCAL : 0;
      jamHdr.Attribute |= (Private == TRUE) ? MSG_PRIVATE : 0;
      jamHdr.Attribute |= (ReceiptRequest == TRUE) ? MSG_RECEIPTREQ : 0;
      jamHdr.Attribute |= (Received == TRUE) ? MSG_READ : 0;
      jamHdr.Attribute |= (Sent == TRUE) ? MSG_SENT : 0;

      lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
      write (fdHdr, &jamHdr, sizeof (JAMHDR));
   }

   return (RetVal);
}

USHORT JAM::Write (ULONG ulMsg)
{
   return (Write (ulMsg, Text));
}

USHORT JAM::Write (ULONG ulMsg, class TCollection &MsgText)
{
   USHORT RetVal = FALSE;
   PSZ pszText, pszSign = HEADERSIGNATURE;
   JAMIDXREC jamIdx;
   JAMSUBFIELD jamSubfield;
   struct tm mktm;

   if (ulMsg == (Highest () + 1L)) {
      RetVal = TRUE;

      memset (&jamHdr, 0, sizeof (JAMHDR));
      jamHdr.Signature[0] = pszSign[0];
      jamHdr.Signature[1] = pszSign[1];
      jamHdr.Signature[2] = pszSign[2];
      jamHdr.Signature[3] = pszSign[3];
      jamHdr.Revision = CURRENTREVLEV;
      jamHdr.MsgNum = ulMsg;

      mktm.tm_year = Written.Year - 1900;
      mktm.tm_mon = Written.Month - 1;
      mktm.tm_mday = Written.Day;
      mktm.tm_hour = Written.Hour;
      mktm.tm_min = Written.Minute;
      mktm.tm_sec = Written.Second;
      mktm.tm_wday = 0;
      mktm.tm_yday = 0;
      mktm.tm_isdst = 0;
      jamHdr.DateWritten = mktime (&mktm);

      mktm.tm_year = Arrived.Year - 1900;
      mktm.tm_mon = Arrived.Month - 1;
      mktm.tm_mday = Arrived.Day;
      mktm.tm_hour = Arrived.Hour;
      mktm.tm_min = Arrived.Minute;
      mktm.tm_sec = Arrived.Second;
      mktm.tm_wday = 0;
      mktm.tm_yday = 0;
      mktm.tm_isdst = 0;
      jamHdr.DateProcessed = mktime (&mktm);

      lseek (fdHdr, 0L, SEEK_END);

      jamIdx.UserCRC = 0;
      jamIdx.HdrOffset = tell (fdHdr);
      lseek (fdJdx, 0L, SEEK_END);
      write (fdJdx, &jamIdx, sizeof (JAMIDXREC));

      write (fdHdr, &jamHdr, sizeof (JAMHDR));

      jamSubfield.HiID = 0;
      jamSubfield.LoID = JAMSFLD_SENDERNAME;
      jamSubfield.DatLen = strlen (From) + 1;
      jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
      write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
      write (fdHdr, From, strlen (From) + 1);
      if (To[0]) {
         jamSubfield.LoID = JAMSFLD_RECVRNAME;
         jamSubfield.DatLen = strlen (To) + 1;
         jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
         write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
         write (fdHdr, To, strlen (To) + 1);
      }
      jamSubfield.LoID = JAMSFLD_SUBJECT;
      jamSubfield.DatLen = strlen (Subject) + 1;
      jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
      write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
      write (fdHdr, Subject, strlen (Subject) + 1);
      if (FromAddress[0] != '\0') {
         jamSubfield.LoID = JAMSFLD_OADDRESS;
         jamSubfield.DatLen = strlen (FromAddress) + 1;
         jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
         write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
         write (fdHdr, FromAddress, strlen (FromAddress) + 1);
      }
      if (ToAddress[0] != '\0') {
         jamSubfield.LoID = JAMSFLD_DADDRESS;
         jamSubfield.DatLen = strlen (ToAddress) + 1;
         jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
         write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
         write (fdHdr, ToAddress, strlen (ToAddress) + 1);
      }

      if ((pszText = (PSZ)MsgText.First ()) != NULL)
         do {
            if (pszText[0] == 0x01) {
               jamHdr.TxtLen += strlen (pszText);
               jamSubfield.LoID = JAMSFLD_UNKNOWN;
               jamSubfield.DatLen = strlen (pszText) + 1;
               jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
               write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
               write (fdHdr, pszText, strlen (pszText) + 1);
            }
            else if (!strnicmp (pszText, "SEEN-BY:", 8)) {
               pszText += 8;
               jamHdr.TxtLen += strlen (pszText);
               jamSubfield.LoID = JAMSFLD_SEENBY2D;
               jamSubfield.DatLen = strlen (pszText) + 1;
               jamHdr.SubfieldLen += jamSubfield.DatLen + sizeof (JAMBINSUBFIELD);
               write (fdHdr, &jamSubfield, sizeof (JAMBINSUBFIELD));
               write (fdHdr, pszText, strlen (pszText) + 1);
            }
         } while ((pszText = (PSZ)MsgText.Next ()) != NULL);

      lseek (fdHdr, 0L, SEEK_SET);
      read (fdHdr, &jamHdrInfo, sizeof (JAMHDRINFO));
      jamHdrInfo.ActiveMsgs++;
      lseek (fdHdr, 0L, SEEK_SET);
      write (fdHdr, &jamHdrInfo, sizeof (JAMHDRINFO));

      lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
      write (fdHdr, &jamHdr, sizeof (JAMHDR));
   }
   else {
      Id = 0L;
      lseek (fdJdx, 0L, SEEK_SET);
      do {
         if (read (fdJdx, &jamIdx, sizeof (jamIdx)) == sizeof (jamIdx)) {
            lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
            read (fdHdr, &jamHdr, sizeof (JAMHDR));
            if (!(jamHdr.Attribute & MSG_DELETED) && jamHdr.MsgNum == ulMsg)
               RetVal = TRUE;
         }
      } while (RetVal == FALSE && tell (fdJdx) < filelength (fdJdx));
   }

   if (RetVal == TRUE) {
      Id = jamHdr.MsgNum;

      lseek (fdJdt, 0L, SEEK_END);
      jamHdr.TxtOffset = tell (fdJdt);
      jamHdr.TxtLen = 0;

      if ((pszText = (PSZ)MsgText.First ()) != NULL)
         do {
            if (pszText[0] != 0x01 && strnicmp (pszText, "SEEN-BY:", 8)) {
               write (fdJdt, pszText, strlen (pszText));
               jamHdr.TxtLen += strlen (pszText);
               write (fdJdt, "\r\n", 2);
               jamHdr.TxtLen += 2;
            }
         } while ((pszText = (PSZ)MsgText.Next ()) != NULL);

      lseek (fdHdr, jamIdx.HdrOffset, SEEK_SET);
      write (fdHdr, &jamHdr, sizeof (JAMHDR));

      WriteHeader (ulMsg);
   }

   return (RetVal);
}

