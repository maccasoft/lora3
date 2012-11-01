
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.11
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/11/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#if !defined(__LINUX__)
#include <errno.h>
#endif
#include "msgbase.h"

FILE *sh_fopen (char *filename, char *access, int shmode)
{
   FILE *fp;
#if !defined(__LINUX__)
   long t1, t2;
#endif

#if defined(__LINUX__)
   fp = fopen (filename, access);
   shmode = shmode;
#else
   t1 = time (NULL);

   while (time (NULL) < t1 + 20) {
      if ((fp = _fsopen (filename, access, shmode)) != NULL)
         break;
#if !defined(__LINUX__)
      if (errno != EACCES)
         break;
#endif
      t2 = time (NULL);
      while (time (NULL) < t2 + 1)
         ;
   }
#endif

   return (fp);
}

SQUISH::SQUISH (void)
{
   fpDat = fpIdx = NULL;
   Current = Id = 0L;
   Locked = FALSE;
   pSqIdx = NULL;
}

SQUISH::SQUISH (PSZ pszName)
{
   fpDat = fpIdx = NULL;
   Id = 0L;
   Locked = FALSE;
   pSqIdx = NULL;

   Open (pszName);
}

SQUISH::~SQUISH (void)
{
   if (Locked == TRUE)
      UnLock ();
   Close ();
}

USHORT SQUISH::Add (VOID)
{
   return (Add (Text));
}

USHORT SQUISH::Add (class TMsgBase *MsgBase)
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

   Original = MsgBase->Original;
   Reply = MsgBase->Reply;

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

USHORT SQUISH::Add (class TCollection &MsgText)
{
   CHAR File[128], NoMore;
   PSZ Text, pszAddress, p;
   ULONG EndFrame;
   SQHDR SqHdr;
   SQIDX SqIdx;
   XMSG XMsg;

   if (Locked == FALSE || fpDat == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      fpDat = sh_fopen (File, "r+b", SH_DENYNO);
      if (Locked == FALSE && fpDat != NULL)
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
   }

   if (Locked == FALSE || fpIdx == NULL) {
      sprintf (File, "%s.sqi", SqBase.Base);
      fpIdx = sh_fopen (File, "ab", SH_DENYNO);
   }

   if (fpDat != NULL) {
      // Allocate a frame at the end of the file, using the 'end_frame' value.
      memset (&SqHdr, 0, sizeof (SQHDR));
      SqHdr.Id = SQHDRID;
      SqHdr.FrameType = FRAME_NORMAL;
      SqHdr.PrevFrame = SqBase.LastFrame;
      SqHdr.NextFrame = 0L;

      NoMore = FALSE;
      SqHdr.CLen = 1;
      if ((Text = (PSZ)MsgText.First ()) != NULL)
         do {
            if (Text[0] == 0x01 && NoMore == FALSE)
               SqHdr.CLen += strlen (Text);
            else {
               SqHdr.MsgLength += strlen (Text) + 1;
               NoMore = TRUE;
            }
         } while ((Text = (PSZ)MsgText.Next ()) != NULL);

      if (SqHdr.CLen == 1)
         SqHdr.CLen++;

      SqHdr.FrameLength = SqHdr.CLen + SqHdr.MsgLength + sizeof (XMSG);
      SqHdr.MsgLength = SqHdr.FrameLength;

      fseek (fpDat, SqBase.EndFrame, SEEK_SET);
      fwrite (&SqHdr, sizeof (SQHDR), 1, fpDat);

      // Write the XMSG header.
      memset (&XMsg, 0, sizeof (XMSG));
      strcpy (XMsg.From, From);
      strcpy (XMsg.To, To);
      strcpy (XMsg.Subject, Subject);
      XMsg.MsgId = SqBase.Uid;

      pszAddress = FromAddress;
      if (strchr (pszAddress, ':') != NULL) {
         XMsg.Orig.Zone = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, ':') + 1;
      }
      if (strchr (pszAddress, '/') != NULL) {
         XMsg.Orig.Net = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, '/') + 1;
      }
      XMsg.Orig.Node = (USHORT)atoi (pszAddress);
      if ((p = strchr (pszAddress, '@')) != NULL)
         *p++ = '\0';
      if (strchr (pszAddress, '.') != NULL) {
         pszAddress = strchr (pszAddress, '.') + 1;
         XMsg.Orig.Point = (USHORT)atoi (pszAddress);
      }

      pszAddress = ToAddress;
      if (strchr (pszAddress, ':') != NULL) {
         XMsg.Dest.Zone = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, ':') + 1;
      }
      if (strchr (pszAddress, '/') != NULL) {
         XMsg.Dest.Net = (USHORT)atoi (pszAddress);
         pszAddress = strchr (pszAddress, '/') + 1;
      }
      XMsg.Dest.Node = (USHORT)atoi (pszAddress);
      if ((p = strchr (pszAddress, '@')) != NULL)
         *p++ = '\0';
      if (strchr (pszAddress, '.') != NULL) {
         pszAddress = strchr (pszAddress, '.') + 1;
         XMsg.Dest.Point = (USHORT)atoi (pszAddress);
      }

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

      XMsg.Attr = MSGUID;
      XMsg.Attr |= (Crash == TRUE) ? MSGCRASH : 0;
      XMsg.Attr |= (FileAttach == TRUE) ? MSGFILE : 0;
      XMsg.Attr |= (FileRequest == TRUE) ? MSGFRQ : 0;
      XMsg.Attr |= (Hold == TRUE) ? MSGHOLD : 0;
      XMsg.Attr |= (KillSent == TRUE) ? MSGKILL : 0;
      XMsg.Attr |= (Local == TRUE) ? MSGLOCAL : 0;
      XMsg.Attr |= (Private == TRUE) ? MSGPRIVATE : 0;
      XMsg.Attr |= (ReceiptRequest == TRUE) ? MSGRRQ : 0;
      XMsg.Attr |= (Received == TRUE) ? MSGREAD : 0;
      XMsg.Attr |= (Sent == TRUE) ? MSGSENT : 0;

      XMsg.ReplyTo = Original;
      XMsg.Replies[0] = Reply;

      fwrite (&XMsg, sizeof (XMsg), 1, fpDat);

      // Write the message's control information and body.
      if (SqHdr.CLen > 2) {
         if ((Text = (PSZ)MsgText.First ()) != NULL)
            do {
               if (Text[0] == 0x01)
                  fwrite (Text, strlen (Text), 1, fpDat);
               else
                  break;
            } while ((Text = (PSZ)MsgText.Next ()) != NULL);
      }
      else if (SqHdr.CLen == 2)
         fwrite ("\001", 1, 1, fpDat);
      fwrite ("", 1, 1, fpDat);

      NoMore = FALSE;
      if ((Text = (PSZ)MsgText.First ()) != NULL)
         do {
            if (Text[0] != 0x01 || NoMore == TRUE) {
               fwrite (Text, strlen (Text), 1, fpDat);
               fwrite ("\r", 1, 1, fpDat);
               NoMore = TRUE;
            }
         } while ((Text = (PSZ)MsgText.Next ()) != NULL);

      fflush (fpDat);
      EndFrame = ftell (fpDat);

      // Link the new SQHDR frame into the end of the message chain.
//      setvbuf (fpDat, NULL, _IONBF, 0L);
      if (SqBase.LastFrame != 0L) {
         fseek (fpDat, SqBase.LastFrame, SEEK_SET);
         fread (&SqHdr, sizeof (SQHDR), 1, fpDat);
         SqHdr.NextFrame = SqBase.EndFrame;
         fseek (fpDat, SqBase.LastFrame, SEEK_SET);
         fwrite (&SqHdr, sizeof (SQHDR), 1, fpDat);
      }
      SqBase.LastFrame = SqBase.EndFrame;
      SqBase.EndFrame = EndFrame;
      if (SqBase.BeginFrame == 0L)
         SqBase.BeginFrame = SqBase.LastFrame;

      SqBase.Uid++;
      SqBase.NumMsg++;
      SqBase.HighMsg++;

      if (Locked == FALSE) {
         fseek (fpDat, 0L, SEEK_SET);
         fwrite (&SqBase, sizeof (SQBASE), 1, fpDat);
      }
//      setvbuf (fpDat, NULL, _IOFBF, 4096L);
   }

   if (fpIdx != NULL) {
      // Write a SQIDX header for the new message.
      if (Locked == FALSE) {
         SqIdx.Ofs = SqBase.LastFrame;
         SqIdx.MsgId = SqBase.Uid - 1L;
         SqIdx.Hash = Hash (To);
         fwrite (&SqIdx, sizeof (SQIDX), 1, fpIdx);
      }
      else {
         pSqIdx[(int)(SqBase.NumMsg - 1L)].Ofs = SqBase.LastFrame;
         pSqIdx[(int)(SqBase.NumMsg - 1L)].MsgId = SqBase.Uid - 1L;
         pSqIdx[(int)(SqBase.NumMsg - 1L)].Hash = Hash (To);
      }
   }

   if (Locked == FALSE) {
      if (fpDat != NULL)
         fclose (fpDat);
      if (fpIdx != NULL)
         fclose (fpIdx);
      fpDat = fpIdx = NULL;

      if (pSqIdx != NULL) {
         free (pSqIdx);
         pSqIdx = NULL;
      }

      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }

      if (SqBase.NumMsg != 0L) {
         sprintf (File, "%s.sqi", SqBase.Base);
         if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
            fclose (fpIdx);
            fpIdx = NULL;
         }
      }
   }

   return (TRUE);
}

VOID SQUISH::Close (VOID)
{
   if (fpIdx != NULL)
      fclose (fpIdx);
   if (fpDat != NULL)
      fclose (fpDat);
   if (pSqIdx != NULL)
      pSqIdx = NULL;

   fpDat = fpIdx = NULL;
   Id = 0L;
   Locked = FALSE;
   pSqIdx = NULL;
}

USHORT SQUISH::Delete (ULONG ulMsg)
{
   int i;
   USHORT RetVal = TRUE;
   CHAR File[128];
   ULONG Position;
   SQHDR SqHdr, SqHdrPrev, SqHdrNext;

   if (Locked == FALSE || fpDat == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      fpDat = sh_fopen (File, "r+b", SH_DENYNO);
      if (Locked == FALSE && fpDat != NULL)
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
   }

   if (Locked == FALSE) {
      if (pSqIdx != NULL) {
         free (pSqIdx);
         pSqIdx = NULL;
      }
      sprintf (File, "%s.sqi", SqBase.Base);
      if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         if ((pSqIdx = (SQIDX *)malloc (sizeof (SQIDX) * 4500)) != NULL) {
            if (SqBase.NumMsg > 0L)
               fread (pSqIdx, (int)filelength (fileno (fpIdx)), 1, fpIdx);
         }
         fclose (fpIdx);
         fpIdx = NULL;
      }
   }

   if (pSqIdx != NULL) {
      // Cerca il messaggio da cancellare all'interno dell'indice
      for (i = 0; i < SqBase.NumMsg; i++) {
         if (pSqIdx[i].MsgId == ulMsg) {
            Position = pSqIdx[i].Ofs;
            // Se il messaggio non e' l'ultimo dell'indice, elimina il record
            // spostando l'indice in memoria.
            if ((i + 1) < SqBase.NumMsg)
               memmove (&pSqIdx[i], &pSqIdx[i + 1], (int)((SqBase.NumMsg - i - 1) * sizeof (SQIDX)));
            RetVal = TRUE;
            break;
         }
      }
   }

   if (RetVal == TRUE && fpDat != NULL) {
      fseek (fpDat, Position, SEEK_SET);
      fread (&SqHdr, sizeof (SqHdr), 1, fpDat);
      SqHdr.FrameType = FRAME_FREE;

      if (SqHdr.PrevFrame != 0L) {
         fseek (fpDat, SqHdr.PrevFrame, SEEK_SET);
         fread (&SqHdrPrev, sizeof (SqHdr), 1, fpDat);
         SqHdrPrev.NextFrame = SqHdr.NextFrame;
         fseek (fpDat, SqHdr.PrevFrame, SEEK_SET);
         fwrite (&SqHdrPrev, sizeof (SqHdr), 1, fpDat);
      }
      if (SqHdr.NextFrame != 0L) {
         fseek (fpDat, SqHdr.NextFrame, SEEK_SET);
         fread (&SqHdrNext, sizeof (SqHdr), 1, fpDat);
         SqHdrNext.PrevFrame = SqHdr.PrevFrame;
         fseek (fpDat, SqHdr.NextFrame, SEEK_SET);
         fwrite (&SqHdrNext, sizeof (SqHdr), 1, fpDat);
      }

      SqHdr.NextFrame = 0L;
      fseek (fpDat, Position, SEEK_SET);
      fwrite (&SqHdr, sizeof (SqHdr), 1, fpDat);

//      fseek (fpDat, 0L, SEEK_SET);
      SqBase.NumMsg--;
      SqBase.HighMsg--;

      if (SqBase.FreeFrame == 0L)
         SqBase.FreeFrame = Position;
      if (SqBase.LastFreeFrame == 0L) {
         SqBase.LastFreeFrame = Position;
         SqHdr.PrevFrame = 0L;
      }
      else {
         SqHdr.PrevFrame = SqBase.LastFreeFrame;

         fseek (fpDat, SqBase.LastFreeFrame, SEEK_SET);
         fread (&SqHdrNext, sizeof (SqHdr), 1, fpDat);
         SqHdrNext.NextFrame = Position;
         fseek (fpDat, SqBase.LastFreeFrame, SEEK_SET);
         fwrite (&SqHdrNext, sizeof (SqHdr), 1, fpDat);

         SqBase.LastFreeFrame = Position;
      }

      fseek (fpDat, Position, SEEK_SET);
      fwrite (&SqHdr, sizeof (SqHdr), 1, fpDat);

      fseek (fpDat, 0L, SEEK_SET);
      fwrite (&SqBase, sizeof (SQBASE), 1, fpDat);
   }

   if (Locked == FALSE) {
      sprintf (File, "%s.sqi", SqBase.Base);
      if ((fpIdx = sh_fopen (File, "w+b", SH_DENYNO)) != NULL) {
         if (SqBase.NumMsg > 0L)
            fwrite (pSqIdx, (int)(SqBase.NumMsg * sizeof (SQIDX)), 1, fpIdx);

         free (pSqIdx);
         pSqIdx = NULL;

         if (SqBase.NumMsg != 0L) {
            fseek (fpIdx, 0L, SEEK_SET);
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
         }

         fclose (fpIdx);
         fpIdx = NULL;
      }

      if (fpDat != NULL)
         fclose (fpDat);

      fpDat = fpIdx = NULL;
   }

   return (RetVal);
}

USHORT SQUISH::GetHWM (ULONG &ulMsg)
{
   CHAR File[128];

   if (Locked == FALSE) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }
   }

   ulMsg = SqBase.HighWater;

   return (TRUE);
}

ULONG SQUISH::Hash (PSZ f)
{
   PSZ p;
   ULONG hash = 0, g;

   for (p = f; *p; p++) {
      hash = (hash << 4) + (ULONG)tolower (*p);
      if ((g = (hash & 0xF0000000L)) != 0L) {
         hash |= g >> 24;
         hash |= g;
      }
   }

   return (hash & 0x7FFFFFFFL);
}

ULONG SQUISH::Highest (VOID)
{
   CHAR File[128];
   ULONG RetVal = 0L;

   if (pSqIdx == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }

      if (SqBase.NumMsg > 0L) {
         sprintf (File, "%s.sqi", SqBase.Base);
         if ((fpIdx = sh_fopen (File, "rb", SH_DENYNO)) != NULL) {
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
            fclose (fpIdx);
            fpIdx = NULL;
         }
      }
   }

   if (SqBase.NumMsg != 0L && pSqIdx != NULL)
      RetVal = pSqIdx[(int)(SqBase.NumMsg - 1L)].MsgId;

   return (RetVal);
}

USHORT SQUISH::Lock (ULONG ulTimeout)
{
   CHAR File[128];

   ulTimeout = ulTimeout;

   if (Locked == TRUE) {
      free (pSqIdx);
      if (fpDat != NULL)
         fclose (fpDat);
      if (fpIdx != NULL)
         fclose (fpIdx);
   }

   sprintf (File, "%s.sqd", SqBase.Base);
   if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
      fread (&SqBase, sizeof (SQBASE), 1, fpDat);

      sprintf (File, "%s.sqi", SqBase.Base);
      if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         if ((pSqIdx = (SQIDX *)malloc (sizeof (SQIDX) * 4500)) != NULL) {
            SqBase.NumMsg = fread (pSqIdx, sizeof (SQIDX), 4500, fpIdx);
            SqBase.HighMsg = SqBase.NumMsg;
         }
         fseek (fpIdx, 0L, SEEK_END);
         Locked = TRUE;
      }
   }

   return (TRUE);
}

ULONG SQUISH::Lowest (VOID)
{
   int i;
   CHAR File[128];
   ULONG RetVal = 0L;

   if (pSqIdx == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }

      if (SqBase.NumMsg > 0L) {
         sprintf (File, "%s.sqi", SqBase.Base);
         if ((fpIdx = sh_fopen (File, "rb", SH_DENYNO)) != NULL) {
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
            fclose (fpIdx);
            fpIdx = NULL;
         }
      }
   }

   if (SqBase.NumMsg != 0L && pSqIdx != NULL) {
      i = 0;
      while (pSqIdx[i].MsgId == 0xFFFFFFFFL) {
         i++;
         if (i >= SqBase.NumMsg)
            break;
      }
      if (pSqIdx[0].MsgId != 0xFFFFFFFFL)
         RetVal = pSqIdx[0].MsgId;
   }

   return (RetVal);
}

ULONG SQUISH::MsgnToUid (ULONG ulMsg)
{
   ULONG RetVal = 0L;
   CHAR File[128];

   if (pSqIdx == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }

      if (SqBase.NumMsg != 0L) {
         sprintf (File, "%s.sqi", SqBase.Base);
         if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
            fclose (fpIdx);
            fpIdx = NULL;
         }
      }
   }

   if (SqBase.NumMsg != 0L && pSqIdx != NULL) {
      if (ulMsg > 0L && ulMsg <= SqBase.NumMsg)
         RetVal = pSqIdx[(size_t)(ulMsg - 1L)].MsgId;
   }

   if (SqBase.NumMsg == 0L)
      RetVal = 0L;

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
   Original = Reply = 0L;
   Text.Clear ();
}

USHORT SQUISH::Next (ULONG &ulMsg)
{
   int i;
   USHORT RetVal = FALSE;
   CHAR File[128];

   if (pSqIdx == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }

      if (SqBase.NumMsg != 0L) {
         sprintf (File, "%s.sqi", SqBase.Base);
         if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
            fclose (fpIdx);
            fpIdx = NULL;
         }
      }
   }

   if (SqBase.NumMsg != 0L && pSqIdx != NULL) {
      if (pSqIdx[0].MsgId != 0xFFFFFFFFL && ulMsg < pSqIdx[0].MsgId) {
         ulMsg = pSqIdx[0].MsgId;
         RetVal = TRUE;
      }
      else {
         for (i = 0; i < SqBase.NumMsg; i++) {
            if (pSqIdx[i].MsgId != 0xFFFFFFFFL && pSqIdx[i].MsgId >= ulMsg) {
               if (pSqIdx[i].MsgId == ulMsg) {
                  while (pSqIdx[i].MsgId == ulMsg && i < SqBase.NumMsg)
                     i++;
               }
               if (i < SqBase.NumMsg) {
                  ulMsg = pSqIdx[i].MsgId;
                  RetVal = TRUE;
               }
               break;
            }
         }
      }
   }

   return (RetVal);
}

ULONG SQUISH::Number (VOID)
{
   return (SqBase.NumMsg);
}

USHORT SQUISH::Open (PSZ pszName)
{
   int fd;
   USHORT RetVal = FALSE;
   CHAR File[128];
#if defined(__LINUX__)
   CHAR *p;
#endif

   sprintf (File, "%s.sqd", pszName);
#if defined(__LINUX__)
   while ((p = strchr (File, '\\')) != NULL)
      *p = '/';
#endif

   memset (&SqBase, 0, sizeof (SQBASE));
   strcpy (SqBase.Base, pszName);

   if ((fd = sopen (strlwr (File), O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if (read (fd, &SqBase, sizeof (SQBASE)) != sizeof (SQBASE)) {
         memset (&SqBase, 0, sizeof (SQBASE));
         SqBase.Len = sizeof (SQBASE);
         SqBase.Uid = 1;
         strcpy (SqBase.Base, pszName);
         SqBase.EndFrame = sizeof (SQBASE);
         SqBase.SzSqhdr = sizeof (SQHDR);
      }

      strcpy (SqBase.Base, pszName);
#if defined(__LINUX__)
      while ((p = strchr (SqBase.Base, '\\')) != NULL)
         *p = '/';
#endif

      lseek (fd, 0L, SEEK_SET);
      write (fd, &SqBase, sizeof (SQBASE));
      close (fd);
   }

   if (SqBase.NumMsg != 0L) {
      sprintf (File, "%s.sqi", SqBase.Base);
      if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
            fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
         fclose (fpIdx);
         fpIdx = NULL;
      }
   }

   Id = 0L;

   return (RetVal);
}

VOID SQUISH::Pack (VOID)
{
   int fdHdr, fdIdx, fdNewDat, fdNewIdx;
   USHORT Read;
   CHAR File[128], NewFile[128], *Buffer;
   SQIDX SqIdx;
   SQHDR SqHdr, SqHdr2;

   if (Locked == TRUE)
      UnLock ();
   if (pSqIdx != NULL) {
      free (pSqIdx);
      pSqIdx = NULL;
   }

   sprintf (File, "%s.sqd", SqBase.Base);
   fdHdr = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   sprintf (File, "%s.sqi", SqBase.Base);
   fdIdx = sopen (File, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   sprintf (File, "%s._qd", SqBase.Base);
   fdNewDat = sopen (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);
   sprintf (File, "%s._qi", SqBase.Base);
   fdNewIdx = sopen (File, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);

   Buffer = (CHAR *)malloc (2048);

   if (fdHdr != -1 && fdIdx != -1 && fdNewDat != -1 && fdNewIdx != -1 && Buffer != NULL) {
      lseek (fdHdr, 0L, SEEK_SET);
      read (fdHdr, &SqBase, sizeof (SQBASE));
      SqBase.NumMsg = 0L;
      SqBase.HighMsg = 0L;
      SqBase.BeginFrame = sizeof (SQBASE);
      SqBase.LastFrame = 0L;
      SqBase.FreeFrame = 0L;
      SqBase.LastFreeFrame = 0L;
      SqBase.EndFrame = 0L;
      write (fdNewDat, &SqBase, sizeof (SQBASE));

      lseek (fdIdx, 0L, SEEK_SET);
      while (read (fdIdx, &SqIdx, sizeof (SQIDX)) == sizeof (SQIDX)) {
         lseek (fdHdr, SqIdx.Ofs, SEEK_SET);
         read (fdHdr, &SqHdr, sizeof (SQHDR));
         if (SqHdr.FrameType == FRAME_NORMAL && SqHdr.Id == SQHDRID) {
            SqBase.NumMsg++;
            SqBase.HighMsg++;

            SqHdr.PrevFrame = SqBase.LastFrame;
            SqHdr.NextFrame = 0L;
            SqHdr.FrameLength = SqHdr.MsgLength;
            if (SqBase.LastFrame != 0L) {
               lseek (fdNewDat, SqBase.LastFrame, SEEK_SET);
               read (fdNewDat, &SqHdr2, sizeof (SQHDR));
               SqHdr2.NextFrame = filelength (fdNewDat);
               lseek (fdNewDat, SqBase.LastFrame, SEEK_SET);
               write (fdNewDat, &SqHdr2, sizeof (SQHDR));
            }
            lseek (fdNewDat, 0L, SEEK_END);
            SqBase.LastFrame = filelength (fdNewDat);

            SqIdx.Ofs = filelength (fdNewDat);
            write (fdNewIdx, &SqIdx, sizeof (SQIDX));

            write (fdNewDat, &SqHdr, sizeof (SQHDR));

            while (SqHdr.FrameLength > 0L) {
               if ((Read = 2048) > SqHdr.FrameLength)
                  Read = (USHORT)SqHdr.FrameLength;
               read (fdHdr, Buffer, Read);
               write (fdNewDat, Buffer, Read);
               SqHdr.FrameLength -= Read;
            }
         }
      }

      lseek (fdNewDat, 0L, SEEK_SET);
      SqBase.EndFrame = filelength (fdNewDat);
      write (fdNewDat, &SqBase, sizeof (SQBASE));

      close (fdNewDat);
      close (fdHdr);
      fdHdr = fdNewDat = -1;
      sprintf (File, "%s._qd", SqBase.Base);
      sprintf (NewFile, "%s.sqd", SqBase.Base);
      unlink (NewFile);
      rename (File, NewFile);

      close (fdNewIdx);
      close (fdIdx);
      fdIdx = fdNewIdx = -1;
      sprintf (File, "%s._qi", SqBase.Base);
      sprintf (NewFile, "%s.sqi", SqBase.Base);
      unlink (NewFile);
      rename (File, NewFile);
   }

   if (Buffer != NULL)
      free (Buffer);
   if (fdHdr != -1)
      close (fdHdr);
   if (fdNewDat != -1)
      close (fdNewDat);
   sprintf (File, "%s._qd", SqBase.Base);
   unlink (File);
   if (fdIdx != -1)
      close (fdIdx);
   if (fdNewIdx != -1)
      close (fdNewIdx);
   sprintf (File, "%s._qi", SqBase.Base);
   unlink (File);
}

USHORT SQUISH::Previous (ULONG &ulMsg)
{
   int i;
   USHORT RetVal = FALSE;
   CHAR File[128];

   if (pSqIdx == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }

      if (SqBase.NumMsg != 0L) {
         sprintf (File, "%s.sqi", SqBase.Base);
         if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
            fclose (fpIdx);
            fpIdx = NULL;
         }
      }
   }

   if (SqBase.NumMsg != 0L && pSqIdx != NULL) {
      if (pSqIdx[(int)(SqBase.NumMsg - 1L)].MsgId != 0xFFFFFFFFL && ulMsg > pSqIdx[(int)(SqBase.NumMsg - 1L)].MsgId) {
         ulMsg = pSqIdx[(int)(SqBase.NumMsg - 1L)].MsgId;
         RetVal = TRUE;
      }
      else {
         for (i = SqBase.NumMsg - 1; i >= 0; i--) {
            if (pSqIdx[i].MsgId != 0xFFFFFFFFL && pSqIdx[i].MsgId <= ulMsg) {
               if (pSqIdx[i].MsgId == ulMsg)
                  i--;
               if (i >= 0L) {
                  ulMsg = pSqIdx[i].MsgId;
                  RetVal = TRUE;
               }
               break;
            }
         }
      }
   }

   return (RetVal);
}

USHORT SQUISH::ReadHeader (ULONG ulMsg)
{
   int i;
   USHORT RetVal = FALSE;
   CHAR File[128];
   ULONG Position;

   New ();

   if (pSqIdx == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }

      if (SqBase.NumMsg != 0L) {
         sprintf (File, "%s.sqi", SqBase.Base);
         if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
            fclose (fpIdx);
            fpIdx = NULL;
         }
      }
   }

   if (pSqIdx != NULL && SqBase.NumMsg > 0L) {
      for (i = 0; i < SqBase.NumMsg; i++) {
         if (pSqIdx[i].MsgId != 0xFFFFFFFFL && pSqIdx[i].MsgId == ulMsg) {
            RetVal = TRUE;
            Position = pSqIdx[i].Ofs;
            memcpy (&SqIdx, &pSqIdx[i], sizeof (SqIdx));
            break;
         }
      }
   }

   if (RetVal == TRUE) {
      if (Locked == FALSE || fpDat == NULL) {
         sprintf (File, "%s.sqd", SqBase.Base);
         fpDat = sh_fopen (File, "r+b", SH_DENYNO);
      }

      if (Locked == FALSE || Position != ftell (fpDat))
         fseek (fpDat, Position, SEEK_SET);
      fread (&SqHdr, sizeof (SQHDR), 1, fpDat);

      if (SqHdr.Id == SQHDRID && SqHdr.FrameType == FRAME_NORMAL) {
         Current = Id = ulMsg;
         fread (&XMsg, sizeof (XMSG), 1, fpDat);
         strcpy (From, XMsg.From);
         strcpy (To, XMsg.To);
         strcpy (Subject, XMsg.Subject);

         sprintf (FromAddress, "%u:%u/%u.%u", XMsg.Orig.Zone, XMsg.Orig.Net, XMsg.Orig.Node, XMsg.Orig.Point);
         sprintf (ToAddress, "%u:%u/%u.%u", XMsg.Dest.Zone, XMsg.Dest.Net, XMsg.Dest.Node, XMsg.Dest.Point);

         Written.Day = (UCHAR)(XMsg.DateWritten & 0x001FL);
         Written.Month = (UCHAR)((XMsg.DateWritten & 0x01E0L) >> 5);
         if (Written.Month < 1 || Written.Month > 12)
            Written.Month = 1;
         Written.Year = (USHORT)(((XMsg.DateWritten & 0xFE00L) >> 9) + 1980);
         Written.Second = (UCHAR)(((XMsg.DateWritten & 0x001F0000L) >> 16) * 2);
         Written.Minute = (UCHAR)((XMsg.DateWritten & 0x07E00000L) >> 21);
         Written.Hour = (UCHAR)((XMsg.DateWritten & 0xF8000000L) >> 27);

         Arrived.Day = (UCHAR)(XMsg.DateArrived & 0x001FL);
         Arrived.Month = (UCHAR)((XMsg.DateArrived & 0x01E0L) >> 5);
         if (Arrived.Month < 1 || Arrived.Month > 12)
            Arrived.Month = 1;
         Arrived.Year = (USHORT)(((XMsg.DateArrived & 0xFE00L) >> 9) + 1980);
         Arrived.Second = (UCHAR)(((XMsg.DateArrived & 0x001F0000L) >> 16) * 2);
         Arrived.Minute = (UCHAR)((XMsg.DateArrived & 0x07E00000L) >> 21);
         Arrived.Hour = (UCHAR)((XMsg.DateArrived & 0xF8000000L) >> 27);

         Original = XMsg.ReplyTo;
         Reply = XMsg.Replies[0];

         Crash = (UCHAR)((XMsg.Attr & MSGCRASH) ? TRUE : FALSE);
         FileAttach = (UCHAR)((XMsg.Attr & MSGFILE) ? TRUE : FALSE);
         FileRequest = (UCHAR)((XMsg.Attr & MSGFRQ) ? TRUE : FALSE);
         Hold = (UCHAR)((XMsg.Attr & MSGHOLD) ? TRUE : FALSE);
         KillSent = (UCHAR)((XMsg.Attr & MSGKILL) ? TRUE : FALSE);
         Local = (UCHAR)((XMsg.Attr & MSGLOCAL) ? TRUE : FALSE);
         Private = (UCHAR)((XMsg.Attr & MSGPRIVATE) ? TRUE : FALSE);
         ReceiptRequest = (UCHAR)((XMsg.Attr & MSGRRQ) ? TRUE : FALSE);
         Received = (UCHAR)((XMsg.Attr & MSGREAD) ? TRUE : FALSE);
         Sent = (UCHAR)((XMsg.Attr & MSGSENT) ? TRUE : FALSE);
      }
      else
         RetVal = FALSE;

      if (Locked == FALSE && fpDat != NULL) {
         fclose (fpDat);
         fpDat = NULL;
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
   USHORT RetVal = FALSE, SkipNext;
   SHORT i, nReaded, nCol, nRead;
   CHAR File[128];
   LONG TxtLen;

   MsgText.Clear ();

   if (ReadHeader (ulMsg) == TRUE) {
      if (Locked == FALSE || fpDat == NULL) {
         sprintf (File, "%s.sqd", SqBase.Base);
         if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL)
            fseek (fpDat, SqIdx.Ofs + sizeof (SQHDR) + sizeof (XMSG), SEEK_SET);
      }

      if (SqHdr.CLen > 2 && fpDat != NULL) {
         TxtLen = (LONG)SqHdr.CLen;
         pLine = szLine;
         nCol = 0;

         do {
            if ((nRead = sizeof (szBuff)) > TxtLen)
               nRead = (SHORT)TxtLen;

            nReaded = 0;
            if (nRead > 0)
               nReaded = (SHORT)fread (szBuff, 1, nRead, fpDat);

            for (i = 0, pBuff = szBuff; i < nReaded; i++, pBuff++) {
               if (*pBuff == '\r' || *pBuff == '\001' || *pBuff == '\0' && pLine != szLine) {
                  *pLine = '\0';
                  if (szLine[0] == '\001')
                     MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
                  pLine = szLine;
                  nCol = 0;
                  if (*pBuff == '\001')
                     *pLine++ = '\001';
               }
               else if (*pBuff != '\n' && *pBuff != '\r' && *pBuff != '\0') {
                  *pLine++ = *pBuff;
                  nCol++;
                  if (nCol >= nWidth) {
                     *pLine = '\0';
                     MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
                     pLine = szLine;
                     *pLine++ = '\001';
                     nCol = 1;
                  }
               }
            }

            TxtLen -= nReaded;
         } while (TxtLen > 0);
      }
      else
         fread (szBuff, (size_t)SqHdr.CLen, 1, fpDat);

      TxtLen = (LONG)(SqHdr.MsgLength - sizeof (XMSG) - SqHdr.CLen);
      pLine = szLine;
      nCol = 0;
      SkipNext = FALSE;
      szWrp[0] = '\0';

      if (TxtLen > 0L && fpDat != NULL)
         do {
            if ((nRead = sizeof (szBuff)) > TxtLen)
               nRead = (SHORT)TxtLen;

            nReaded = 0;
            if (nRead > 0)
               nReaded = (SHORT)fread (szBuff, 1, nRead, fpDat);

            for (i = 0, pBuff = szBuff; i < nReaded; i++, pBuff++) {
               if (*pBuff == '\r') {
                  *pLine = '\0';
                  if (pLine > szLine && SkipNext == TRUE) {
                     pLine--;
                     while (pLine > szLine && *pLine == ' ')
                        *pLine-- = '\0';
                     if (pLine > szLine)
                        MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
                  }
                  else if (SkipNext == FALSE)
                     MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
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
                     MsgText.Add (szLine, (USHORT)(strlen (szLine) + 1));
                     strcpy (szLine, szWrp);
                     pLine = strchr (szLine, '\0');
                     nCol = (SHORT)strlen (szLine);
                     SkipNext = TRUE;
                  }
               }
            }

            TxtLen -= nReaded;
         } while (TxtLen > 0);

      if (Locked == FALSE && fpDat != NULL) {
         fclose (fpDat);
         fpDat = NULL;
      }

      RetVal = TRUE;
   }

   return (RetVal);
}

VOID SQUISH::SetHWM (ULONG ulMsg)
{
   CHAR File[128];

   if (Locked == FALSE) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
      }
   }

   SqBase.HighWater = ulMsg;

   fseek (fpDat, 0L, SEEK_SET);
   fwrite (&SqBase, sizeof (SQBASE), 1, fpDat);

   if (Locked == FALSE) {
      fclose (fpDat);
      fpDat = NULL;
   }
}

ULONG SQUISH::UidToMsgn (ULONG ulMsg)
{
   int i;
   ULONG RetVal = 0L;
   CHAR File[128];

   if (pSqIdx == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }

      if (SqBase.NumMsg != 0L) {
         sprintf (File, "%s.sqi", SqBase.Base);
         if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
            fclose (fpIdx);
            fpIdx = NULL;
         }
      }
   }

   if (SqBase.NumMsg != 0L && pSqIdx != NULL) {
      for (i = 0; i < SqBase.NumMsg; i++) {
         if (pSqIdx[i].MsgId == ulMsg) {
            RetVal = i + 1;
            break;
         }
      }
   }

   if (SqBase.NumMsg == 0L)
      RetVal = 0L;

   return (RetVal);
}

VOID SQUISH::UnLock (VOID)
{
   CHAR File[128];

   if (Locked == TRUE && pSqIdx != NULL) {
      fclose (fpIdx);

      sprintf (File, "%s.sqi", SqBase.Base);
      fpIdx = sh_fopen (File, "wb", SH_DENYNO);
      if (SqBase.NumMsg > 0L)
         fwrite (pSqIdx, (int)(sizeof (SQIDX) * SqBase.NumMsg), 1, fpIdx);
      fclose (fpIdx);

      fseek (fpDat, 0L, SEEK_SET);
      fwrite (&SqBase, sizeof (SQBASE), 1, fpDat);
      fclose (fpDat);

      free (pSqIdx);

      pSqIdx = NULL;
      fpDat = fpIdx = NULL;
      Locked = FALSE;
   }
}

USHORT SQUISH::WriteHeader (ULONG ulMsg)
{
   int i;
   USHORT RetVal = FALSE;
   CHAR File[128];
   ULONG Position;

   if (pSqIdx == NULL) {
      sprintf (File, "%s.sqd", SqBase.Base);
      if ((fpDat = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
         fread (&SqBase, sizeof (SQBASE), 1, fpDat);
         fclose (fpDat);
         fpDat = NULL;
      }

      if (SqBase.NumMsg != 0L) {
         sprintf (File, "%s.sqi", SqBase.Base);
         if ((fpIdx = sh_fopen (File, "r+b", SH_DENYNO)) != NULL) {
            if ((pSqIdx = (SQIDX *)malloc ((int)(sizeof (SQIDX) * SqBase.NumMsg))) != NULL)
               fread (pSqIdx, sizeof (SQIDX), (size_t)SqBase.NumMsg, fpIdx);
            fclose (fpIdx);
            fpIdx = NULL;
         }
      }
   }

   if (SqBase.NumMsg > 0L && pSqIdx != NULL) {
      for (i = 0; i < SqBase.NumMsg; i++) {
         if (pSqIdx[i].MsgId == ulMsg) {
            RetVal = TRUE;
            Position = pSqIdx[i].Ofs;
            break;
         }
      }
   }

   if (RetVal == TRUE) {
      if (Locked == FALSE || fpDat == NULL) {
         sprintf (File, "%s.sqd", SqBase.Base);
         fpDat = sh_fopen (File, "r+b", SH_DENYNO);
      }

      Position += sizeof (SQHDR);
      if (Locked == FALSE || Position != ftell (fpDat))
         fseek (fpDat, Position, SEEK_SET);

      if (SqHdr.FrameType == FRAME_NORMAL) {
         fread (&XMsg, sizeof (XMSG), 1, fpDat);

         XMsg.Attr = 0;
         XMsg.Attr |= (Crash == TRUE) ? MSGCRASH : 0;
         XMsg.Attr |= (FileAttach == TRUE) ? MSGFILE : 0;
         XMsg.Attr |= (FileRequest == TRUE) ? MSGFRQ : 0;
         XMsg.Attr |= (Hold == TRUE) ? MSGHOLD : 0;
         XMsg.Attr |= (KillSent == TRUE) ? MSGKILL : 0;
         XMsg.Attr |= (Local == TRUE) ? MSGLOCAL : 0;
         XMsg.Attr |= (Private == TRUE) ? MSGPRIVATE : 0;
         XMsg.Attr |= (ReceiptRequest == TRUE) ? MSGRRQ : 0;
         XMsg.Attr |= (Received == TRUE) ? MSGREAD : 0;
         XMsg.Attr |= (Sent == TRUE) ? MSGSENT : 0;

         XMsg.ReplyTo = Original;
         XMsg.Replies[0] = Reply;

         fseek (fpDat, Position, SEEK_SET);
         fwrite (&XMsg, sizeof (XMSG), 1, fpDat);
      }

      if (Locked == FALSE && fpDat != NULL) {
         fclose (fpDat);
         fpDat = NULL;
      }
   }

   return (RetVal);
}


