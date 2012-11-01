
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.5
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/13/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

PSZ adeptMonths[] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

ADEPT::ADEPT (void)
{
   fdTxt = fdHdr = fdIdx = -1;
   Current = Id = 0L;
}

ADEPT::ADEPT (PSZ pszName)
{
   fdTxt = fdHdr = fdIdx = -1;
   Id = 0L;

   Open (pszName);
}

ADEPT::~ADEPT (void)
{
   Close ();
}

USHORT ADEPT::Add (VOID)
{
   return (Add (Text));
}

USHORT ADEPT::Add (class TMsgBase *MsgBase)
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

USHORT ADEPT::Add (class TCollection &MsgText)
{
   USHORT RetVal = FALSE;
   CHAR *Text, *p, *pszAddress;
   ADEPTINDEXES Index;

   if (fdHdr != -1 && fdIdx != -1 && fdTxt != -1) {
      RetVal = TRUE;

      memset (&Index, 0, sizeof (Index));
      Text = To;
      while (*Text)
         Index.to += (UCHAR)*Text++;
      Text = From;
      while (*Text)
         Index.from += (UCHAR)*Text++;
      Text = Subject;
      while (*Text)
         Index.subj += (UCHAR)*Text++;

      Index.msgidserialno = Highest () + 1L;
      lseek (fdIdx, 0L, SEEK_END);
      write (fdIdx, &Index, sizeof (Index));

      memset (&Data, 0, sizeof (Data));
      Data.StructLen = sizeof (Data);
      strcpy (Data.from, From);
      strcpy (Data.to, To);
      strcpy (Data.subj, Subject);
      Data.msgnum = Index.msgidserialno;

      pszAddress = FromAddress;
      if (strchr (pszAddress, ':') != NULL) {
         Data.o_zone = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, ':') + 1;
      }
      if (strchr (pszAddress, '/') != NULL) {
         Data.o_net = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, '/') + 1;
      }
      Data.o_node = (USHORT)atoi (pszAddress);
      if ((p = strchr (pszAddress, '@')) != NULL)
         *p++ = '\0';
      if (strchr (pszAddress, '.') != NULL) {
         pszAddress = strchr (pszAddress, '.') + 1;
         Data.o_point = (USHORT)atoi (pszAddress);
      }

      pszAddress = ToAddress;
      if (strchr (pszAddress, ':') != NULL) {
         Data.d_zone = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, ':') + 1;
      }
      if (strchr (pszAddress, '/') != NULL) {
         Data.d_net = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, '/') + 1;
      }
      Data.d_node = (USHORT)atoi (pszAddress);
      if ((p = strchr (pszAddress, '@')) != NULL)
         *p++ = '\0';
      if (strchr (pszAddress, '.') != NULL) {
         pszAddress = strchr (pszAddress, '.') + 1;
         Data.d_point = (USHORT)atoi (pszAddress);
      }

      if (Written.Month == 0)
         Written.Month = 1;
      sprintf (Data.date, "%2d %3.3s %02d  %02d:%02d:%02d", Written.Day, adeptMonths[Written.Month - 1], Written.Year % 100, Written.Hour, Written.Minute, Written.Second);

      Data.fflags |= (Sent == TRUE) ? MSGSENT : 0;
      Data.fflags |= (Crash == TRUE) ? MSGCRASH : 0;
      Data.fflags |= (Received == TRUE) ? MSGREAD : 0;
      Data.fflags |= (Private == TRUE) ? MSGPRIVATE : 0;
      Data.fflags |= (KillSent == TRUE) ? MSGKILL : 0;
      Data.fflags |= (Local == TRUE) ? MSGLOCAL : 0;
      Data.fflags |= (FileRequest == TRUE) ? MSGFRQ : 0;

      lseek (fdTxt, 0L, SEEK_END);
      Data.start = tell (fdTxt);

      if ((Text = (PSZ)MsgText.First ()) != NULL)
         do {
            Data.length += strlen (Text) + 1;
            write (fdTxt, Text, strlen (Text));
            write (fdTxt, "\r", 1);
         } while ((Text = (PSZ)MsgText.Next ()) != NULL);

      lseek (fdHdr, 0L, SEEK_END);
      write (fdHdr, &Data, sizeof (Data));

      TotalMsgs++;
   }

   return (RetVal);
}

VOID ADEPT::Close (VOID)
{
   if (fdIdx != -1)
      close (fdIdx);
   if (fdHdr != -1)
      close (fdHdr);
   if (fdTxt != -1)
      close (fdTxt);

   fdTxt = fdHdr = fdIdx = -1;
   Id = 0L;
}

USHORT ADEPT::Delete (ULONG ulMsg)
{
   USHORT RetVal = FALSE;

   if (ReadHeader (ulMsg) == TRUE) {
      Data.xflags |= MSGDELETED;
      lseek (fdHdr, tell (fdHdr) - sizeof (Data), SEEK_SET);
      write (fdHdr, &Data, sizeof (Data));
      TotalMsgs--;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT ADEPT::GetHWM (ULONG &ulMsg)
{
   ulMsg = 0L;

   return (FALSE);
}

ULONG ADEPT::Highest (VOID)
{
   ULONG RetVal = 0L, Position;
   LONG Current;

   if (filelength (fdHdr) >= sizeof (ADEPTDATA)) {
      Position = tell (fdHdr);
      Current = filelength (fdHdr) - sizeof (ADEPTDATA);
      do {
         lseek (fdHdr, Current, SEEK_SET);
         if (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
            if (!(Data.xflags & MSGDELETED))
               RetVal = Data.msgnum;
         }
         Current -= sizeof (ADEPTDATA);
      } while (RetVal == 0L && Current > 0L);
      lseek (fdHdr, Position, SEEK_SET);
   }

   return (RetVal);
}

USHORT ADEPT::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG ADEPT::Lowest (VOID)
{
   ULONG RetVal = 0L, Position;

   Position = tell (fdHdr);
   lseek (fdHdr, 0L, SEEK_SET);
   while (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
      if (!(Data.xflags & MSGDELETED)) {
         RetVal = Data.msgnum;
         break;
      }
   }
   lseek (fdHdr, Position, SEEK_SET);

   return (RetVal);
}

ULONG ADEPT::MsgnToUid (ULONG ulMsg)
{
   return (ulMsg);
}

VOID ADEPT::New (VOID)
{
   From[0] = To[0] = Subject[0] = '\0';
   Crash = Direct = FileAttach = FileRequest = Hold = Immediate = FALSE;
   Intransit = KillSent = Local = Private = ReceiptRequest = Received = FALSE;
   Sent = 0;
   memset (&Written, 0, sizeof (Written));
   memset (&Arrived, 0, sizeof (Arrived));
   FromAddress[0] = ToAddress[0] = '\0';
   Original = Reply = 0L;
   Text.Clear ();
}

USHORT ADEPT::Next (ULONG &ulMsg)
{
   USHORT RetVal = FALSE, MayBeNext;

   if (tell (fdHdr) >= sizeof (Data)) {
      lseek (fdHdr, tell (fdHdr) - sizeof (Data), SEEK_SET);
      read (fdHdr, &Data, sizeof (Data));
      if (Data.msgnum == ulMsg) {
         while (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
            if (!(Data.xflags & MSGDELETED)) {
               RetVal = TRUE;
               ulMsg = Data.msgnum;
               break;
            }
         }
      }
   }

   if (RetVal == FALSE) {
      lseek (fdHdr, 0L, SEEK_SET);
      MayBeNext = FALSE;
      while (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
         if (!(Data.xflags & MSGDELETED)) {
            if (MayBeNext == TRUE) {
               RetVal = TRUE;
               ulMsg = Data.msgnum;
               break;
            }
            else if (ulMsg == Data.msgnum)
               MayBeNext = TRUE;
            else if (ulMsg < Data.msgnum) {
               RetVal = TRUE;
               ulMsg = Data.msgnum;
               break;
            }
         }
      }
   }

   Id = ulMsg;

   return (RetVal);
}

ULONG ADEPT::Number (VOID)
{
   return (TotalMsgs);
}

USHORT ADEPT::Open (PSZ pszName)
{
   USHORT RetVal = FALSE;
   CHAR File[128];

   TotalMsgs = 0L;

   sprintf (File, "%s.Data", pszName);
   if ((fdHdr = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      sprintf (File, "%s.Index", pszName);
      fdIdx = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      sprintf (File, "%s.Text", pszName);
      fdTxt = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

      while (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
         if (!(Data.xflags & MSGDELETED))
            TotalMsgs++;
      }

      strcpy (BaseName, pszName);
      RetVal = TRUE;
   }

   Current = Id = 0L;

   return (RetVal);
}

VOID ADEPT::Pack (VOID)
{
   int fdHdrNew, fdTxtNew, fdIdxNew;
   USHORT Max;
   CHAR OldName[128], NewName[128], *Text;
   ADEPTINDEXES Index;

   sprintf (NewName, "%s.NewData", BaseName);
   fdHdrNew = sopen (NewName, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);
   sprintf (NewName, "%s.NewIndex", BaseName);
   fdIdxNew = sopen (NewName, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);
   sprintf (NewName, "%s.NewText", BaseName);
   fdTxtNew = sopen (NewName, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);
   Text = (CHAR *)malloc (2048);

   if (fdIdxNew != -1 && fdHdrNew != -1 && fdTxtNew != -1 && Text != NULL) {
      lseek (fdHdr, 0L, SEEK_SET);
      lseek (fdIdx, 0L, SEEK_SET);
      while (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
         read (fdIdx, &Index, sizeof (Index));
         if (!(Data.xflags & MSGDELETED)) {
            lseek (fdTxt, Data.start, SEEK_SET);
            Data.start = tell (fdTxtNew);
            write (fdHdrNew, &Data, sizeof (Data));
            while (Data.length > 0) {
               if ((Max = 2048) > Data.length)
                  Max = (USHORT)Data.length;
               read (fdTxt, Text, Max);
               write (fdTxtNew, Text, Max);
               Data.length -= Max;
            }
            write (fdIdxNew, &Index, sizeof (Index));
         }
      }

      free (Text);
      Text = NULL;
      close (fdHdrNew);
      close (fdTxtNew);
      close (fdIdxNew);
      fdHdrNew = fdIdxNew = fdTxtNew = -1;

      close (fdHdr);
      close (fdTxt);
      close (fdIdx);

      sprintf (NewName, "%s.NewData", BaseName);
      sprintf (OldName, "%s.Data", BaseName);
      unlink (OldName);
      rename (NewName, OldName);
      fdHdr = sopen (OldName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

      sprintf (NewName, "%s.NewIndex", BaseName);
      sprintf (OldName, "%s.Index", BaseName);
      unlink (OldName);
      rename (NewName, OldName);
      fdIdx = sopen (OldName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

      sprintf (NewName, "%s.NewText", BaseName);
      sprintf (OldName, "%s.Text", BaseName);
      unlink (OldName);
      rename (NewName, OldName);
      fdTxt = sopen (OldName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (Text != NULL)
      free (Text);
   if (fdHdrNew != -1)
      close (fdHdrNew);
   if (fdTxtNew != -1)
      close (fdTxtNew);
   if (fdIdxNew != -1)
      close (fdIdxNew);

   sprintf (NewName, "%s.NewData", BaseName);
   unlink (NewName);
   sprintf (NewName, "%s.NewText", BaseName);
   unlink (NewName);
   sprintf (NewName, "%s.NewIndex", BaseName);
   unlink (NewName);
}

USHORT ADEPT::Previous (ULONG &ulMsg)
{
   USHORT RetVal = FALSE, MayBeNext;

   if (tell (fdHdr) >= sizeof (Data) * 2) {
      lseek (fdHdr, tell (fdHdr) - sizeof (Data), SEEK_SET);
      read (fdHdr, &Data, sizeof (Data));
      if (Data.msgnum == ulMsg) {
         while (tell (fdHdr) >= sizeof (Data) * 2) {
            lseek (fdHdr, tell (fdHdr) - sizeof (Data) * 2, SEEK_SET);
            read (fdHdr, &Data, sizeof (Data));
            if (!(Data.xflags & MSGDELETED)) {
               RetVal = TRUE;
               ulMsg = Data.msgnum;
               break;
            }
         }
      }

      if (RetVal == FALSE) {
         lseek (fdHdr, filelength (fdHdr) - sizeof (Data), SEEK_SET);
         MayBeNext = FALSE;

         read (fdHdr, &Data, sizeof (Data));
         if (!(Data.xflags & MSGDELETED)) {
            if (ulMsg == Data.msgnum)
               MayBeNext = TRUE;
            else if (ulMsg > Data.msgnum) {
               RetVal = TRUE;
               ulMsg = Data.msgnum;
            }
         }

         while (tell (fdHdr) >= sizeof (Data) * 2) {
            lseek (fdHdr, tell (fdHdr) - sizeof (Data) * 2, SEEK_SET);
            read (fdHdr, &Data, sizeof (Data));
            if (!(Data.xflags & MSGDELETED)) {
               if (MayBeNext == TRUE) {
                  RetVal = TRUE;
                  ulMsg = Data.msgnum;
                  break;
               }
               else if (ulMsg == Data.msgnum)
                  MayBeNext = TRUE;
               else if (ulMsg > Data.msgnum) {
                  RetVal = TRUE;
                  ulMsg = Data.msgnum;
                  break;
               }
            }
         }
      }
   }

   Id = ulMsg;

   return (RetVal);
}

USHORT ADEPT::ReadHeader (ULONG ulMsg)
{
   int dd, yy, hr, mn, sc;
   USHORT RetVal = FALSE, i;
   CHAR mm[8];

   New ();

   if (Id == ulMsg) {
      lseek (fdHdr, tell (fdHdr) - sizeof (ADEPTDATA), SEEK_SET);
      if (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
         if (Data.msgnum == ulMsg)
            RetVal = TRUE;
      }
   }

   if (RetVal == FALSE) {
      lseek (fdHdr, 0L, SEEK_SET);
      while (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
         if (!(Data.xflags & MSGDELETED)) {
            if (ulMsg == Data.msgnum) {
               RetVal = TRUE;
               break;
            }
         }
      }
   }

   if (RetVal == TRUE) {
      Current = Id = ulMsg;
      strcpy (From, Data.from);
      strcpy (To, Data.to);
      strcpy (Subject, Data.subj);

      sprintf (FromAddress, "%u:%u/%u.%u", Data.o_zone, Data.o_net, Data.o_node, Data.o_point);
      sprintf (ToAddress, "%u:%u/%u.%u", Data.d_zone, Data.d_net, Data.d_node, Data.d_point);

      sscanf (Data.date, "%2d %3s %2d  %2d:%2d:%02d", &dd, mm, &yy, &hr, &mn, &sc);

      Arrived.Day = Written.Day = (UCHAR)dd;
      for (i = 0; i < 12; i++) {
         if (!stricmp (adeptMonths[i], mm)) {
            Written.Month = (UCHAR)(i + 1);
            break;
         }
      }
      if (Written.Month < 1 || Written.Month > 12)
         Written.Month = 1;
      Arrived.Month = Written.Month;
      if ((Written.Year = (USHORT)(yy + 1900)) < 1980)
         Written.Year += 100;
      Arrived.Year = Written.Year;
      Arrived.Hour = Written.Hour = (UCHAR)hr;
      Arrived.Minute = Written.Minute = (UCHAR)mn;
      Arrived.Second = Written.Second = (UCHAR)sc;

      Sent = (UCHAR)((Data.fflags & MSGSENT) ? TRUE : FALSE);
      Crash = (UCHAR)((Data.fflags & MSGCRASH) ? TRUE : FALSE);
      Received = (UCHAR)((Data.fflags & MSGREAD) ? TRUE : FALSE);
      Private = (UCHAR)((Data.fflags & MSGPRIVATE) ? TRUE : FALSE);
      KillSent = (UCHAR)((Data.fflags & MSGKILL) ? TRUE : FALSE);
      Local = (UCHAR)((Data.fflags & MSGLOCAL) ? TRUE : FALSE);
      FileRequest = (UCHAR)((Data.fflags & MSGFRQ) ? TRUE : FALSE);
   }

   return (RetVal);
}

USHORT ADEPT::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT ADEPT::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   USHORT RetVal = FALSE, SkipNext;
   SHORT i, nReaded, nCol, nRead;
   LONG TxtLen;

   MsgText.Clear ();

   if (ReadHeader (ulMsg) == TRUE) {
      lseek (fdTxt, Data.start, SEEK_SET);

      TxtLen = Data.length;
      pLine = szLine;
      nCol = 0;
      SkipNext = FALSE;

      if (TxtLen > 0L)
         do {
            if ((nRead = sizeof (szBuff)) > TxtLen)
               nRead = (SHORT)TxtLen;

            nReaded = (SHORT)read (fdTxt, szBuff, nRead);

            for (i = 0, pBuff = szBuff; i < nReaded; i++, pBuff++) {
               if (*pBuff == '\r') {
                  *pLine = '\0';
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

            TxtLen -= nReaded;
         } while (TxtLen > 0);

      RetVal = TRUE;
   }

   return (RetVal);
}

VOID ADEPT::SetHWM (ULONG ulMsg)
{
   ulMsg = ulMsg;
}

ULONG ADEPT::UidToMsgn (ULONG ulMsg)
{
   return (ulMsg);
}

VOID ADEPT::UnLock (VOID)
{
}

USHORT ADEPT::WriteHeader (ULONG ulMsg)
{
   USHORT RetVal = FALSE;

   if (Id == ulMsg) {
      lseek (fdHdr, tell (fdHdr) - sizeof (ADEPTDATA), SEEK_SET);
      if (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
         if (Data.msgnum == ulMsg)
            RetVal = TRUE;
      }
   }

   if (RetVal == FALSE) {
      lseek (fdHdr, 0L, SEEK_SET);
      while (read (fdHdr, &Data, sizeof (Data)) == sizeof (Data)) {
         if (!(Data.xflags & MSGDELETED)) {
            if (ulMsg == Data.msgnum) {
               RetVal = TRUE;
               break;
            }
         }
      }
   }

   if (RetVal == TRUE) {
      RetVal = TRUE;

      Data.fflags = 0;
      Data.fflags |= (Sent == TRUE) ? MSGSENT : 0;
      Data.fflags |= (Crash == TRUE) ? MSGCRASH : 0;
      Data.fflags |= (Received == TRUE) ? MSGREAD : 0;
      Data.fflags |= (Private == TRUE) ? MSGPRIVATE : 0;
      Data.fflags |= (KillSent == TRUE) ? MSGKILL : 0;
      Data.fflags |= (Local == TRUE) ? MSGLOCAL : 0;
      Data.fflags |= (FileRequest == TRUE) ? MSGFRQ : 0;

      lseek (fdHdr, tell (fdHdr) - sizeof (Data), SEEK_SET);
      write (fdHdr, &Data, sizeof (Data));
   }

   return (RetVal);
}


