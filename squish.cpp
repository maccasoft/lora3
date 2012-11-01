
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.18
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// Squish<tm> Message Base handling class
//
// History:
//    07/11/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"

SQUISH::SQUISH (void)
{
   fdHdr = fdIdx = -1;
   Id = 0L;
}

SQUISH::SQUISH (PSZ pszName)
{
   fdHdr = fdIdx = -1;
   Id = 0L;

   Open (pszName);
}

SQUISH::~SQUISH (void)
{
   Close ();
}

USHORT SQUISH::Add (VOID)
{
   return (Add (Text));
}

USHORT SQUISH::Add (class TCollection &MsgText)
{
   PSZ Text;
   ULONG LastFrame;
   SQHDR SqHdr;
   SQIDX SqIdx;
   XMSG XMsg;

   lseek (fdHdr, 0L, SEEK_SET);
   read (fdHdr, &SqBase, sizeof (SQBASE));
   SqBase.Uid++;
   SqBase.NumMsg++;
   SqBase.HighMsg++;

   if (SqBase.LastFrame != 0L) {
      lseek (fdHdr, SqBase.LastFrame, SEEK_SET);
      read (fdHdr, &SqHdr, sizeof (SQHDR));
      SqHdr.NextFrame = filelength (fdHdr);
      lseek (fdHdr, SqBase.LastFrame, SEEK_SET);
      write (fdHdr, &SqHdr, sizeof (SQHDR));
   }

   if (SqBase.BeginFrame == 0L)
      SqBase.BeginFrame = filelength (fdHdr);
   LastFrame = SqBase.LastFrame;
   SqBase.LastFrame = filelength (fdHdr);
   lseek (fdHdr, 0L, SEEK_SET);
   write (fdHdr, &SqBase, sizeof (SQBASE));

   lseek (fdHdr, 0L, SEEK_END);
   memset (&SqHdr, 0, sizeof (SQHDR));
   SqHdr.Id = SQHDRID;
   SqHdr.PrevFrame = LastFrame;
   SqHdr.FrameType = FRAME_NORMAL;

   SqHdr.CLen = 1;
   if ((Text = (PSZ)MsgText.First ()) != NULL)
      do {
         if (Text[0] == 0x01)
            SqHdr.CLen += strlen (Text);
         else
            SqHdr.MsgLength += strlen (Text) + 1;
      } while ((Text = (PSZ)MsgText.Next ()) != NULL);

   if (SqHdr.CLen == 1)
      SqHdr.CLen++;
   SqHdr.FrameLength = SqHdr.CLen + SqHdr.MsgLength + sizeof (XMSG);
   SqHdr.MsgLength = SqHdr.FrameLength;

   memset (&SqIdx, 0, sizeof (SQIDX));
   SqIdx.Ofs = tell (fdHdr);
   SqIdx.MsgId = SqBase.Uid - 1L;
   lseek (fdIdx, 0L, SEEK_END);
   write (fdIdx, &SqIdx, sizeof (SQIDX));

   write (fdHdr, &SqHdr, sizeof (SQHDR));

   memset (&XMsg, 0, sizeof (XMSG));
   strcpy (XMsg.From, From);
   strcpy (XMsg.To, To);
   strcpy (XMsg.Subject, Subject);

   XMsg.DateWritten = Written.Day & 0x1F;
   XMsg.DateWritten |= Written.Month << 5;
   XMsg.DateWritten |= (Written.Year - 1980) << 9;
   XMsg.DateWritten |= (Written.Second / 2) << 16;
   XMsg.DateWritten |= Written.Minute << 21;
   XMsg.DateWritten |= Written.Hour << 27;

   XMsg.DateArrived = Arrived.Day & 0x1F;
   XMsg.DateArrived |= Arrived.Month << 5;
   XMsg.DateArrived |= (Arrived.Year - 1980) << 9;
   XMsg.DateArrived |= (Arrived.Second / 2) << 16;
   XMsg.DateArrived |= Arrived.Minute << 21;
   XMsg.DateArrived |= Arrived.Hour << 27;

   write (fdHdr, &XMsg, sizeof (XMsg));

   if ((Text = (PSZ)MsgText.First ()) != NULL)
      do {
         if (Text[0] == 0x01) {
            write (fdHdr, Text, strlen (Text));
            write (fdHdr, "\r", 1);
         }
      } while ((Text = (PSZ)MsgText.Next ()) != NULL);

   if (SqHdr.CLen == 2)
      write (fdHdr, "\x01", 1);
   write (fdHdr, "", 1);

   if ((Text = (PSZ)MsgText.First ()) != NULL)
      do {
         if (Text[0] != 0x01) {
            write (fdHdr, Text, strlen (Text));
            write (fdHdr, "\r", 1);
         }
      } while ((Text = (PSZ)MsgText.Next ()) != NULL);

   lseek (fdHdr, 0L, SEEK_SET);
   read (fdHdr, &SqBase, sizeof (SQBASE));
   SqBase.EndFrame = filelength (fdHdr);
   lseek (fdHdr, 0L, SEEK_SET);
   write (fdHdr, &SqBase, sizeof (SQBASE));

   return (TRUE);
}

VOID SQUISH::Close (VOID)
{
   if (fdIdx != -1)
      close (fdIdx);
   if (fdHdr != -1)
      close (fdHdr);

   fdHdr = fdIdx = -1;
   Id = 0L;
}

USHORT SQUISH::Delete (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   SQIDX SqIdx;
   SQHDR SqHdr, SqHdrPrev, SqHdrNext;

   if (ReadHeader (ulMsg) == TRUE) {
      lseek (fdIdx, tell (fdIdx) - sizeof (SQIDX), SEEK_SET);
      read (fdIdx, &SqIdx, sizeof (SQIDX));
      lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
      read (fdHdr, &SqHdr, sizeof (SqHdr));
      SqHdr.FrameType = FRAME_FREE;
      lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
      write (fdHdr, &SqHdr, sizeof (SqHdr));

      if (SqHdr.PrevFrame != 0L) {
         lseek (fdHdr, SqHdr.PrevFrame, SEEK_SET);
         read (fdHdr, &SqHdrPrev, sizeof (SqHdr));
         SqHdrPrev.NextFrame = SqHdr.NextFrame;
         lseek (fdHdr, SqHdr.PrevFrame, SEEK_SET);
         write (fdHdr, &SqHdrPrev, sizeof (SqHdr));
      }
      if (SqHdr.PrevFrame != 0L) {
         lseek (fdHdr, SqHdr.NextFrame, SEEK_SET);
         read (fdHdr, &SqHdrNext, sizeof (SqHdr));
         SqHdrPrev.PrevFrame = SqHdr.PrevFrame;
         lseek (fdHdr, SqHdr.NextFrame, SEEK_SET);
         write (fdHdr, &SqHdrNext, sizeof (SqHdr));
      }
   }

   return (RetVal);
}

ULONG SQUISH::Highest (VOID)
{
   ULONG RetVal = 0L, Position, Current;
   SQIDX SqIdx;
   SQHDR SqHdr;

   if (filelength (fdIdx) >= sizeof (SQIDX)) {
      Position = tell (fdIdx);
      Current = filelength (fdIdx) - sizeof (SQIDX);
      do {
         lseek (fdIdx, Current, SEEK_SET);
         if (read (fdIdx, &SqIdx, sizeof (SQIDX)) == sizeof (SQIDX)) {
            lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
            read (fdHdr, &SqHdr, sizeof (SqHdr));
            if (SqHdr.FrameType != FRAME_FREE)
               RetVal = SqIdx.MsgId;
         }
         Current -= sizeof (SQIDX);
      } while (RetVal == 0L && Current > 0L);
      lseek (fdIdx, Position, SEEK_SET);
   }

   return (RetVal);
}

USHORT SQUISH::Lock (ULONG ulTimeout)
{
   ulTimeout = ulTimeout;
   return (TRUE);
}

ULONG SQUISH::Lowest (VOID)
{
   ULONG RetVal = 0L, Position;
   SQIDX SqIdx;
   SQHDR SqHdr;

   Position = tell (fdIdx);
   lseek (fdIdx, 0L, SEEK_SET);
   while (RetVal == 0L && read (fdIdx, &SqIdx, sizeof (SQIDX)) == sizeof (SQIDX)) {
      lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
      read (fdHdr, &SqHdr, sizeof (SqHdr));
      if (SqHdr.FrameType != FRAME_FREE)
         RetVal = SqIdx.MsgId;
   }
   lseek (fdIdx, Position, SEEK_SET);

   return (RetVal);
}

VOID SQUISH::New (VOID)
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

USHORT SQUISH::Next (ULONG &ulMsg)
{
   USHORT RetVal = FALSE, MayBeNext = FALSE;
   SQIDX SqIdx;
   SQHDR SqHdr;

   if (SqBase.NumMsg > 0L) {
// --------------------------------------------------------------------
// The first attempt to retrive the next message number suppose that
// the file pointers are located after the current message number.
// Usually this is the 99% of the situations because the messages are
// often readed sequentially.
// --------------------------------------------------------------------
      if (tell (fdIdx) >= sizeof (SQIDX))
         lseek (fdIdx, tell (fdIdx) - sizeof (SQIDX), SEEK_SET);
      do {
         if (read (fdIdx, &SqIdx, sizeof (SQIDX)) == sizeof (SQIDX)) {
            lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
            read (fdHdr, &SqHdr, sizeof (SqHdr));
            if (SqHdr.FrameType == FRAME_NORMAL) {
               if (MayBeNext == TRUE) {
                  if (SqIdx.MsgId > ulMsg) {
                     ulMsg = SqIdx.MsgId;
                     RetVal = TRUE;
                  }
               }
               if (SqIdx.MsgId == ulMsg)
                  MayBeNext = TRUE;
            }
         }
      } while (RetVal == FALSE && tell (fdIdx) < filelength (fdIdx));

      if (RetVal == FALSE && MayBeNext == FALSE) {
// --------------------------------------------------------------------
// It seems that the file pointers are not located where they should be
// so our next attempt is to scan the database from the beginning to
// find the next message number.
// --------------------------------------------------------------------
         lseek (fdIdx, 0L, SEEK_SET);
         do {
            if (read (fdIdx, &SqIdx, sizeof (SQIDX)) == sizeof (SQIDX)) {
               lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
               read (fdHdr, &SqHdr, sizeof (SqHdr));
               if (SqHdr.FrameType == FRAME_NORMAL) {
                  if (SqIdx.MsgId > ulMsg) {
                     ulMsg = SqIdx.MsgId;
                     RetVal = TRUE;
                  }
               }
            }
         } while (RetVal == FALSE && tell (fdIdx) < filelength (fdIdx));
      }

      Id = 0L;
      if (RetVal == TRUE)
         Id = ulMsg;
   }

   return (RetVal);
}

ULONG SQUISH::Number (VOID)
{
   return (SqBase.NumMsg);
}

USHORT SQUISH::Open (PSZ pszName)
{
   USHORT RetVal = FALSE;
   CHAR File[128];

   sprintf (File, "%s.sqd", pszName);
   if ((fdHdr = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if (read (fdHdr, &SqBase, sizeof (SQBASE)) != sizeof (SQBASE)) {
         memset (&SqBase, 0, sizeof (SQBASE));
         SqBase.Len = sizeof (SQBASE);
         SqBase.Uid = 1;
         strcpy (SqBase.Base, pszName);
         SqBase.EndFrame = sizeof (SQBASE);
         SqBase.SzSqhdr = sizeof (SQHDR);
         lseek (fdHdr, 0L, SEEK_SET);
         write (fdHdr, &SqBase, sizeof (SQBASE));
      }

      sprintf (File, "%s.sqi", pszName);
      fdIdx = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      RetVal = TRUE;

      strcpy (BaseName, pszName);
   }

   Id = 0L;

   return (RetVal);
}

VOID SQUISH::Pack (VOID)
{
}

USHORT SQUISH::Previous (ULONG &ulMsg)
{
   USHORT RetVal = FALSE, MayBeNext = FALSE;
   LONG Pos;
   SQIDX SqIdx;
   SQHDR SqHdr;

   if (SqBase.NumMsg > 0L) {
// --------------------------------------------------------------------
// The first attempt to retrive the next message number suppose that
// the file pointers are located after the current message number.
// Usually this is the 99% of the situations because the messages are
// often readed sequentially.
// --------------------------------------------------------------------
      if (tell (fdIdx) >= sizeof (SQIDX)) {
         Pos = tell (fdIdx) - sizeof (SQIDX);
         do {
            lseek (fdIdx, Pos, SEEK_SET);
            if (read (fdIdx, &SqIdx, sizeof (SQIDX)) == sizeof (SQIDX)) {
               lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
               read (fdHdr, &SqHdr, sizeof (SqHdr));
               if (SqHdr.FrameType == FRAME_NORMAL) {
                  if (MayBeNext == TRUE) {
                     if (SqIdx.MsgId < ulMsg) {
                        ulMsg = SqIdx.MsgId;
                        RetVal = TRUE;
                     }
                  }
                  if (SqIdx.MsgId == ulMsg)
                     MayBeNext = TRUE;
               }
            }
            Pos -= sizeof (SQIDX);
         } while (RetVal == FALSE && Pos >= 0L);
      }

      if (RetVal == FALSE && MayBeNext == FALSE) {
// --------------------------------------------------------------------
// It seems that the file pointers are not located where they should be
// so our next attempt is to scan the database from the end to find
// the next message number.
// --------------------------------------------------------------------
         Pos = filelength (fdIdx) - sizeof (SQIDX);
         do {
            lseek (fdIdx, Pos, SEEK_SET);
            if (read (fdIdx, &SqIdx, sizeof (SQIDX)) == sizeof (SQIDX)) {
               lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
               read (fdHdr, &SqHdr, sizeof (SqHdr));
               if (SqHdr.FrameType == FRAME_NORMAL) {
                  if (SqIdx.MsgId < ulMsg) {
                     ulMsg = SqIdx.MsgId;
                     RetVal = TRUE;
                  }
               }
            }
            Pos -= sizeof (SQIDX);
         } while (RetVal == FALSE && Pos >= 0L);
      }

      Id = 0L;
      if (RetVal == TRUE)
         Id = ulMsg;
   }

   return (RetVal);
}

USHORT SQUISH::ReadHeader (ULONG ulMsg)
{
   USHORT RetVal = FALSE;
   SQIDX SqIdx;
   SQHDR SqHdr;
   XMSG XMsg;

   New ();

   if (Id == ulMsg) {
      lseek (fdIdx, tell (fdIdx) - sizeof (SQIDX), SEEK_SET);
      if (read (fdIdx, &SqIdx, sizeof (SQIDX)) == sizeof (SQIDX)) {
         if (SqIdx.MsgId == ulMsg)
            RetVal = TRUE;
      }
   }

   if (RetVal == FALSE) {
      lseek (fdIdx, 0L, SEEK_SET);
      do {
         if (read (fdIdx, &SqIdx, sizeof (SQIDX)) == sizeof (SQIDX)) {
            if (SqIdx.MsgId == ulMsg)
               RetVal = TRUE;
         }
      } while (RetVal == FALSE && tell (fdIdx) < filelength (fdIdx));
   }

   if (RetVal == TRUE) {
      RetVal = FALSE;
      lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
      read (fdHdr, &SqHdr, sizeof (SQHDR));
      if (SqHdr.FrameType == FRAME_NORMAL) {
         Id = ulMsg;
         RetVal = TRUE;
         read (fdHdr, &XMsg, sizeof (XMSG));
         strcpy (From, XMsg.From);
         strcpy (To, XMsg.To);
         strcpy (Subject, XMsg.Subject);

         Written.Day = (UCHAR)(XMsg.DateWritten & 0x001FL);
         Written.Month = (UCHAR)((XMsg.DateWritten & 0x01E0L) >> 5);
         Written.Year = (USHORT)(((XMsg.DateWritten & 0xFE00L) >> 9) + 1980);
         Written.Second = (UCHAR)(((XMsg.DateWritten & 0x001F0000L) >> 16) * 2);
         Written.Minute = (UCHAR)((XMsg.DateWritten & 0x07E00000L) >> 21);
         Written.Hour = (UCHAR)((XMsg.DateWritten & 0xF8000000L) >> 27);

         Arrived.Day = (UCHAR)(XMsg.DateArrived & 0x001FL);
         Arrived.Month = (UCHAR)((XMsg.DateArrived & 0x01E0L) >> 5);
         Arrived.Year = (USHORT)(((XMsg.DateArrived & 0xFE00L) >> 9) + 1980);
         Arrived.Second = (UCHAR)(((XMsg.DateArrived & 0x001F0000L) >> 16) * 2);
         Arrived.Minute = (UCHAR)((XMsg.DateArrived & 0x07E00000L) >> 21);
         Arrived.Hour = (UCHAR)((XMsg.DateArrived & 0xF8000000L) >> 27);

         Crash = (XMsg.Attr & MSGCRASH) ? TRUE : FALSE;
         FileAttach = (XMsg.Attr & MSGFILE) ? TRUE : FALSE;
         FileRequest = (XMsg.Attr & MSGFRQ) ? TRUE : FALSE;
         Hold = (XMsg.Attr & MSGHOLD) ? TRUE : FALSE;
         KillSent = (XMsg.Attr & MSGKILL) ? TRUE : FALSE;
         Local = (XMsg.Attr & MSGLOCAL) ? TRUE : FALSE;
         Private = (XMsg.Attr & MSGPRIVATE) ? TRUE : FALSE;
         ReceiptRequest = (XMsg.Attr & MSGRRQ) ? TRUE : FALSE;
         Received = (XMsg.Attr & MSGREAD) ? TRUE : FALSE;
         Sent = (XMsg.Attr & MSGSENT) ? TRUE : FALSE;
      }
   }

   return (RetVal);
}

USHORT SQUISH::Read (ULONG ulMsg, SHORT nWidth)
{
   return (Read (ulMsg, Text, nWidth));
}

USHORT SQUISH::Read (ULONG ulMsg, class TCollection &MsgText, SHORT nWidth)
{
   USHORT RetVal = FALSE;
   SHORT i, nReaded, nCol, nRead;
   CHAR szBuff[128], szLine[132], szWrp[132], *pLine, *pBuff;
   LONG TxtLen;
   SQIDX SqIdx;
   SQHDR SqHdr;

   MsgText.Clear ();

   if (ReadHeader (ulMsg) == TRUE) {
      lseek (fdIdx, tell (fdIdx) - sizeof (SQIDX), SEEK_SET);
      read (fdIdx, &SqIdx, sizeof (SQIDX));

      lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
      read (fdHdr, &SqHdr, sizeof (SQHDR));
      lseek (fdHdr, sizeof (XMSG) + SqHdr.CLen, SEEK_CUR);

      TxtLen = (LONG)(SqHdr.MsgLength - sizeof (XMSG) - SqHdr.CLen);
      pLine = szLine;
      nCol = 0;

      if (TxtLen > 0L)
         do {
            if ((nRead = sizeof (szBuff)) > TxtLen)
               nRead = (SHORT)TxtLen;

            nReaded = (SHORT)read (fdHdr, szBuff, nRead);

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
       
            TxtLen -= nReaded;
         } while (TxtLen > 0);

      RetVal = TRUE;
   }

   return (RetVal);
}

VOID SQUISH::UnLock (VOID)
{
}

USHORT SQUISH::WriteHeader (ULONG ulMsg)
{
   ulMsg = ulMsg;
   return (FALSE);
}

USHORT SQUISH::Write (ULONG ulMsg)
{
   return (Write (ulMsg, Text));
}

USHORT SQUISH::Write (ULONG ulMsg, class TCollection &MsgText)
{
   PSZ Text;
   ULONG LastFrame;
   SQHDR SqHdr;
   SQIDX SqIdx;
   XMSG XMsg;

   if (ulMsg == (Highest () + 1L)) {
      lseek (fdHdr, 0L, SEEK_SET);
      read (fdHdr, &SqBase, sizeof (SQBASE));
      SqBase.Uid = ulMsg + 1L;
      SqBase.NumMsg++;
      SqBase.HighMsg++;

      if (SqBase.LastFrame != 0L) {
         lseek (fdHdr, SqBase.LastFrame, SEEK_SET);
         read (fdHdr, &SqHdr, sizeof (SQHDR));
         SqHdr.NextFrame = filelength (fdHdr);
         lseek (fdHdr, SqBase.LastFrame, SEEK_SET);
         write (fdHdr, &SqHdr, sizeof (SQHDR));
      }

      if (SqBase.BeginFrame == 0L)
         SqBase.BeginFrame = filelength (fdHdr);
      LastFrame = SqBase.LastFrame;
      SqBase.LastFrame = filelength (fdHdr);
      lseek (fdHdr, 0L, SEEK_SET);
      write (fdHdr, &SqBase, sizeof (SQBASE));

      lseek (fdHdr, 0L, SEEK_END);
      memset (&SqHdr, 0, sizeof (SQHDR));
      SqHdr.Id = SQHDRID;
      SqHdr.PrevFrame = LastFrame;
      SqHdr.FrameType = FRAME_NORMAL;

      SqHdr.CLen = 1;
      if ((Text = (PSZ)MsgText.First ()) != NULL)
         do {
            if (Text[0] == 0x01)
               SqHdr.CLen += strlen (Text);
            else
               SqHdr.MsgLength += strlen (Text) + 1;
         } while ((Text = (PSZ)MsgText.Next ()) != NULL);

      if (SqHdr.CLen == 1)
         SqHdr.CLen++;
      SqHdr.FrameLength = SqHdr.CLen + SqHdr.MsgLength + sizeof (XMSG);
      SqHdr.MsgLength = SqHdr.FrameLength;

      memset (&SqIdx, 0, sizeof (SQIDX));
      SqIdx.Ofs = tell (fdHdr);
      SqIdx.MsgId = ulMsg;
      lseek (fdIdx, 0L, SEEK_END);
      write (fdIdx, &SqIdx, sizeof (SQIDX));

      write (fdHdr, &SqHdr, sizeof (SQHDR));
   }

   memset (&XMsg, 0, sizeof (XMSG));
   strcpy (XMsg.From, From);
   strcpy (XMsg.To, To);
   strcpy (XMsg.Subject, Subject);

   XMsg.DateWritten = Written.Day & 0x1F;
   XMsg.DateWritten |= Written.Month << 5;
   XMsg.DateWritten |= (Written.Year - 1980) << 9;
   XMsg.DateWritten |= (Written.Second / 2) << 16;
   XMsg.DateWritten |= Written.Minute << 21;
   XMsg.DateWritten |= Written.Hour << 27;

   XMsg.DateArrived = Arrived.Day & 0x1F;
   XMsg.DateArrived |= Arrived.Month << 5;
   XMsg.DateArrived |= (Arrived.Year - 1980) << 9;
   XMsg.DateArrived |= (Arrived.Second / 2) << 16;
   XMsg.DateArrived |= Arrived.Minute << 21;
   XMsg.DateArrived |= Arrived.Hour << 27;

   write (fdHdr, &XMsg, sizeof (XMsg));

   if ((Text = (PSZ)MsgText.First ()) != NULL)
      do {
         if (Text[0] == 0x01) {
            write (fdHdr, Text, strlen (Text));
            write (fdHdr, "\r", 1);
         }
      } while ((Text = (PSZ)MsgText.Next ()) != NULL);

   if (SqHdr.CLen == 2)
      write (fdHdr, "\x01", 1);
   write (fdHdr, "", 1);

   if ((Text = (PSZ)MsgText.First ()) != NULL)
      do {
         if (Text[0] != 0x01) {
            write (fdHdr, Text, strlen (Text));
            write (fdHdr, "\r", 1);
         }
      } while ((Text = (PSZ)MsgText.Next ()) != NULL);

   lseek (fdHdr, 0L, SEEK_SET);
   read (fdHdr, &SqBase, sizeof (SQBASE));
   SqBase.EndFrame = filelength (fdHdr);
   lseek (fdHdr, 0L, SEEK_SET);
   write (fdHdr, &SqBase, sizeof (SQBASE));

   return (TRUE);
}


