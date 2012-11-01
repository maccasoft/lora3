
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/09/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "ftrans.h"

#define DLE         0x10

/* Protocol capability flags */
#define CANCRC32 0x80
#define CANFREQ  0x40

/* File Transmission States */
#define XDONE        0       /* All done, no more files to transmit          */
#define XSENDFNAME   1       /* Send filename packet                         */
#define XRCVFNACK    2       /* Wait for filename packet ACK                 */
#define XSENDBLK     3       /* Send next block of file data                 */
#define XRCVEOFACK   4       /* Wait for EOF packet ACK                      */
#define XSENDFREQNAK 5       /* Send file request NAK (no matches found)     */
#define XRCVFRNAKACK 6       /* Wait for ACK to file request NAK             */

/* File Reception States */
#define RDONE      0         /* All done, nothing more to receive            */
#define RRCVFNAME  1         /* Wait for filename packet                     */
#define RRCVBLK    2         /* Wait for next block of file data             */

/* Packet Types */
#define NOPKT        0       /* No packet received yet; try again later      */
#define BADPKT      '@'      /* Bad packet received; CRC error, overrun, etc.*/
#define FNAMEPKT    'A'      /* Filename info packet                         */
#define FNACKPKT    'B'      /* Filename packet ACK                          */
#define BLKPKT      'C'      /* File data block packet                       */
#define RPOSPKT     'D'      /* Transmitter reposition packet                */
#define EOFACKPKT   'E'      /* EOF packet ACK                               */
#define HALTPKT     'F'      /* Immediate screeching halt packet             */
#define HALTACKPKT  'G'      /* Halt packet ACK for ending batch             */
#define FREQPKT     'H'      /* File request packet                          */
#define FREQNAKPKT  'I'      /* File request NAK (no matches for your req)   */
#define FRNAKACKPKT 'J'      /* ACK to file request NAK                      */

/* Non-byte values returned by rcvbyte() */
#define BUFEMPTY  (-1)
#define PKTSTRT   (-2)
#define PKTEND    (-3)
#define NOCARRIER (-4)
#define PKTSTRT32 (-5)

/* Bytes we need to watch for */
#define PKTSTRTCHR   'a'
#define PKTENDCHR    'b'
#define PKTSTRTCHR32 'c'

TJanus::TJanus (void)
{
   Com = NULL;
   Log = NULL;
   LastSent = 0;
   CanCrc32 = TRUE;
   Rxbufptr = NULL;
   Rxbufmax = (UCHAR *)(RxBuffer + BUFMAX + 8);
   WaitFlag = FALSE;
   RxPktCrc32 = 0L;
   RxPktCrc16 = 0;
   RxCrc32 = FALSE;
   IsOutbound = FALSE;
   LastPktName = 0L;
   RxTempSize = 0;
   MakeRequests = TRUE;
   AllowRequests = TRUE;

   TimeoutSecs = 10;
   Speed = 19200L;
   RxPath[0] = '\0';
   TxQueue = RxQueue = NULL;
   Outbound = NULL;
}

TJanus::~TJanus (void)
{
}

SHORT TJanus::GetByte (VOID)
{
   SHORT c, w;

   if ((c = GetRawByte ()) == DLE) {
      w = WaitFlag;
      WaitFlag = TRUE;
      if ((c = GetRawByte ()) >= 0) {
         switch (c ^= 0x40) {
            case PKTSTRTCHR:
               c = PKTSTRT;
               break;
            case PKTSTRTCHR32:
               c = PKTSTRT32;
               break;
            case PKTENDCHR:
               c = PKTEND;
               break;
         }
      }
      WaitFlag = w;
   }

   return (c);
}

UCHAR TJanus::GetPacket (VOID)
{
   SHORT c, i;
   UCHAR *p;
   ULONG PktCrc;

   WaitFlag = FALSE;
   if ((p = Rxbufptr) == NULL) {
      do {
         c = GetByte ();
      } while (c >= 0 || c == PKTEND);

      switch (c) {
         case PKTSTRT:
            RxCrc32 = FALSE;
            p = (UCHAR *)RxBuffer;
            RxPktCrc16 = 0;
            break;
         case PKTSTRT32:
            RxCrc32 = TRUE;
            p = (UCHAR *)RxBuffer;
            RxPktCrc32 = 0xFFFFFFFFL;
            break;
         case NOCARRIER:
            return (HALTPKT);
         default:
            return (NOPKT);
      }
   }

   if (RxCrc32 == TRUE) {
      while ((c = GetByte ()) >= 0 && p < Rxbufmax) {
         *p++ = (UCHAR)c;
         RxPktCrc32 = Crc32 ((UCHAR)c, RxPktCrc32);
      }
   }
   else {
      while ((c = GetByte ()) >= 0 && p < Rxbufmax) {
         *p++ = (UCHAR)c;
         RxPktCrc16 = Crc16 ((UCHAR)c, RxPktCrc16);
      }
   }

   switch (c) {
      case PKTEND:
         WaitFlag = TRUE;
         PktCrc = 0L;
         for (i = (RxCrc32 == TRUE) ? 4 : 2; i; --i) {
            if ((c = GetByte ()) < 0)
               break;
            PktCrc = (PktCrc << 8) | (UCHAR)c;
         }
         if (i == 0) {
            if ((RxCrc32 && PktCrc == RxPktCrc32) || (USHORT)PktCrc == RxPktCrc16) {
               Rxbufptr = NULL;
               Rxblklen = (USHORT)(--p - RxBuffer);
               return (*p);
            }
         }
         /* fallthrough */

      /*---------------------------------------------------------------------*/
      /* Bad CRC, carrier lost, or buffer overflow from munged PKTEND        */
      /*---------------------------------------------------------------------*/
      default:
         if (c == NOCARRIER)
            return (HALTPKT);
         else {
            Rxbufptr = NULL;
            return (BADPKT);
         }

      case BUFEMPTY:
         Rxbufptr = p;
         return (NOPKT);

      case PKTSTRT:
         RxCrc32 = FALSE;
         Rxbufptr = RxBuffer;
         RxPktCrc16 = 0;
         return (BADPKT);

      case PKTSTRT32:
         RxCrc32 = TRUE;
         Rxbufptr = RxBuffer;
         RxPktCrc32 = 0xFFFFFFFFL;
         return (BADPKT);
   }
}

SHORT TJanus::GetRawByte (VOID)
{
   ULONG timeval;

   if (Com->BytesReady () == TRUE)
      return (Com->ReadByte ());
   if (Com->Carrier () == FALSE)
      return (NOCARRIER);

   if (WaitFlag == FALSE)
      return (BUFEMPTY);

   timeval = time (NULL) + TimeoutSecs;

   while (Com->BytesReady () == FALSE) {
      if (Com->Carrier () == FALSE)
         return (NOCARRIER);
      if (time (NULL) > timeval)
         return (BUFEMPTY);
   }

   return (Com->ReadByte ());
}

VOID TJanus::SendByte (UCHAR Byte)
{
   switch (Byte) {
      case 0x0D:
         if (LastSent != '@')
            goto sendit;
         /* fallthrough */
      case DLE:
      case XON:
      case XOFF:
         Com->BufferByte (DLE);
         Byte ^= 0x40;
         /* fallthrough */
      default:
sendit:
         Com->BufferByte (LastSent = Byte);
         break;
   }
}

VOID TJanus::SendPacket (UCHAR *Buffer, USHORT Len, USHORT Type)
{
   SHORT i;
   USHORT crc;
   ULONG crc32;

   Com->BufferByte (DLE);

   if (CanCrc32 == TRUE && Type != FNAMEPKT) {
      Com->BufferByte ((UCHAR)(PKTSTRTCHR32 ^ 0x40));

      crc32 = 0xFFFFFFFFL;
      if (Buffer != NULL) {
         for (i = 0; i < Len; i++) {
            SendByte ((UCHAR)*Buffer);
            crc32 = Crc32 ((UCHAR)((0377 & *Buffer++)), crc32);
         }
      }

      Com->BufferByte ((UCHAR)Type);
      crc32 = Crc32 ((UCHAR)Type, crc32);

      Com->BufferByte (DLE);
      Com->BufferByte ((UCHAR)(PKTENDCHR ^ 0x40));

      SendByte ((UCHAR)(crc32 >> 24));
      SendByte ((UCHAR)((crc32 >> 16) & 0xFF));
      SendByte ((UCHAR)((crc32 >> 8) & 0xFF));
      SendByte ((UCHAR)(crc32 & 0xFF));
   }
   else {
      Com->BufferByte ((UCHAR)(PKTSTRTCHR ^ 0x40));

      crc = 0;
      if (Buffer != NULL) {
         for (i = 0; i < Len; i++) {
            SendByte ((UCHAR)*Buffer);
            crc = Crc16 ((UCHAR)((0377 & *Buffer++)), crc);
         }
      }

      Com->BufferByte ((UCHAR)Type);
      crc = Crc16 ((UCHAR)Type, crc);

      Com->BufferByte (DLE);
      Com->BufferByte ((UCHAR)(PKTENDCHR ^ 0x40));

      SendByte ((UCHAR)(crc >> 8));
      SendByte ((UCHAR)(crc & 0xFF));
   }

   Com->UnbufferBytes ();
}

LONG TJanus::ProcessFileName (VOID)
{
   SHORT gota, SharedCap;
   CHAR *p, *q, *fileinfo;
   LONG Rxbytes;
   struct stat f;

   RxFileName[0] = '\0';
   Rxbytes = -1L;

   fileinfo = strchr ((PSZ)RxBuffer, '\0') + 1;
   p = strchr (fileinfo, '\0') + 1;
   SharedCap = (SHORT)((Rxblklen > p - (PSZ)RxBuffer) ? *p & (CANCRC32|CANFREQ) : 0);
   if (SharedCap & CANCRC32)
      CanCrc32 = TRUE;

   for (p = (PSZ)RxBuffer, q = (PSZ)RxBuffer; *p; p++) {
      if (*p == '/' || *p == '\\' || *p == ':')
         q = p + 1;
   }

   if (*q != '\0') {
      sprintf (RxFileName, "%s%s", RxPath, q);

      RxFilesize = RxFiletime = 0L;
      if (*fileinfo)
         sscanf (fileinfo, "%ld %lo", &RxFilesize, &RxFiletime);

      Log->Write (" Receiving %s; %ldb, %d min.", q, RxFilesize, (int)((RxFilesize * 10 / Speed + 53) / 54));

      if (RxQueue != NULL) {
         RxQueue->New ();
         strcpy (RxQueue->Name, q);
         strcpy (RxQueue->Complete, RxFileName);
         RxQueue->Size = RxFilesize;
      }

      Rxbytes = 0L;
      if ((RxFile = sopen (RxFileName, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
         fstat (RxFile, &f);
         close (RxFile);

         if (RxFilesize == f.st_size && RxFiletime == f.st_mtime) {
            Log->Write ("+Already have %s", RxFileName);
            return (-1L);
         }
         else if (RxFilesize > f.st_size) {
            Rxbytes = f.st_size;
            Log->Write ("+Synchronizing to offset %lu", Rxbytes);
         }
         else {
            if ((p = strchr (RxFileName, '\0')) != NULL) {
               p--;
               gota = FALSE;
               do {
                  if (isdigit (*p) || gota == TRUE) {
                     if (*p == '9') {
                        gota = TRUE;
                        *p = 'A';
                     }
                     else
                        *p = (char)(*p + 1);
                  }
                  else
                     *p = '0';
                  if ((RxFile = sopen (RxFileName, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1)
                     close (RxFile);
               } while (RxFile != -1);
               Log->Write ("+Renaming dupe file %s", RxFileName);
            }
         }
      }

      if ((RxFile = sopen (RxFileName, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, SH_DENYNO, S_IREAD|S_IWRITE)) == -1)
         return (-1L);
   }

   return (Rxbytes);
}

VOID TJanus::GetNextFile (VOID)
{
   CHAR *q;
   struct stat f;

   TxFileName[0] = '\0';
   TxBuffer[0] = TxBuffer[1] = '\0';

   if (TxQueue != NULL) {
      IsOutbound = FALSE;
      while (TxQueue->First () == TRUE) {
         if ((TxFile = sopen (TxQueue->Complete, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
            strcpy (TxFileName, TxQueue->Complete);
            strcpy ((PSZ)TxBuffer, TxQueue->Name);

            q = strchr ((PSZ)TxBuffer, '\0') + 1;
            fstat (TxFile, &f);
            sprintf (q, "%lu %lo %o", f.st_size, f.st_mtime, f.st_mode);

            Log->Write (" Sending %s; %ldb, %d min.", TxQueue->Name, f.st_size, (int)((f.st_size * 10 / Speed + 53) / 54));
            break;
         }
         else
            TxQueue->Remove ();
      }
   }

   if (TxFileName[0] == '\0' && Outbound != NULL) {
      IsOutbound = TRUE;
      while (Outbound->First () == TRUE) {
         if (Outbound->Poll == FALSE && (Outbound->Request == FALSE || MakeRequests == TRUE)) {
            if ((TxFile = sopen (Outbound->Complete, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
               strcpy (TxFileName, Outbound->Complete);

               if (Outbound->MailPKT == TRUE) {
                  while (time (NULL) == LastPktName)
                     ;
                  sprintf ((PSZ)TxBuffer, "%08lx.pkt", time (NULL));
               }
               else
                  strcpy ((PSZ)TxBuffer, Outbound->Name);

               q = strchr ((PSZ)TxBuffer, '\0') + 1;
               fstat (TxFile, &f);
               sprintf (q, "%lu %lo %o", f.st_size, f.st_mtime, f.st_mode);

               Log->Write (" Sending %s; %ldb, %d min.", Outbound->Name, f.st_size, (int)((f.st_size * 10 / Speed + 53) / 54));
               break;
            }
            else
               Outbound->Remove ();
         }
         else
            Outbound->Remove ();
      }
   }
}

VOID TJanus::Transfer (VOID)
{

   RxFile = TxFile = -1;
   if (Speed > 64000L)
      txblkmax = BUFMAX;
   else {
      txblkmax = (USHORT)(Speed / 300 * 128);
      if (txblkmax > BUFMAX)
         txblkmax = BUFMAX;
   }
   txblklen = txblkmax;
   xmit_retry = 0L;
   lasttx = txlength = rxstpos = length = 0L;
   rpos_count = 0;
   rpos_sttime = rpos_retry = last_blkpos = 0L;

   xstate = XSENDFNAME;
   rstate = RRCVFNAME;

   GetNextFile ();

   do {
      if (xmit_retry != 0L) {
         if (time (NULL) > xmit_retry) {
            xmit_retry = 0L;
            switch (xstate) {
               case XRCVFNACK:
                  xstate = XSENDFNAME;
                  break;
               case XRCVFRNAKACK:
                  xstate = XSENDFREQNAK;
                  break;
               case XRCVEOFACK:
                  txpos = lasttx;
                  lseek (TxFile, txpos, SEEK_SET);
                  xstate = XSENDBLK;
                  break;
            }
         }
      }

      switch (xstate) {
         case XSENDFNAME:
            blklen = (USHORT)(strchr (strchr ((PSZ)TxBuffer, '\0') + 1, '\0') - (PSZ)TxBuffer + 1);
            TxBuffer[blklen++] = (UCHAR)(CANCRC32|CANFREQ);
            SendPacket (TxBuffer, blklen, FNAMEPKT);
            xstate = XRCVFNACK;
            xmit_retry = time (NULL) + TimeoutSecs;
//            Log->Write ("> JAN=XSENDFNAME, XS=%d", xstate);
            SendPacket (NULL, 0, EOFACKPKT);
            break;
         case XSENDBLK:
//            Log->Write ("> JAN=XSENDBLK, XS=%d", xstate);
            *((LONG *)TxBuffer) = lasttx = txpos;
            blklen = (USHORT)read (TxFile, TxBuffer + sizeof (txpos), txblklen);
            SendPacket (TxBuffer, (USHORT)(sizeof (txpos) + blklen), BLKPKT);
            txpos += blklen;
            if (txpos >= filelength (TxFile) || blklen < txblklen) {
               xmit_retry = time (NULL) + TimeoutSecs;
               xstate = XRCVEOFACK;
            }
//            Log->Write ("> JAN=XSENDBLK (END), XS=%d", xstate);
            break;
         case XSENDFREQNAK:
            SendPacket (NULL, 0, FREQNAKPKT);
            xmit_retry = time (NULL) + TimeoutSecs;
            xstate = XRCVFRNAKACK;
//            Log->Write ("> JAN=XSENDFREQNAK, XS=%d", xstate);
            break;
      }

      while ((pkttype = GetPacket ()) != 0) {
         switch (pkttype) {
            case BADPKT:
            case BLKPKT:
//               Log->Write ("> JAN=%s, RS=%d", (pkttype == BADPKT) ? "BADPKT" : "BLKPKT", rstate);
               if (rstate == RRCVBLK) {
                  if (pkttype == BADPKT || *((LONG *)RxBuffer) != Rxpos) {
                     if (pkttype == BLKPKT) {
                        if (*((LONG *)RxBuffer) < last_blkpos) {
                           rpos_retry = 0L;
                           rpos_count = 0;
                        }
                        last_blkpos = *((long *)RxBuffer);
                     }
                     if (time (NULL) > rpos_retry) {
                        if (++rpos_count == 1)
                           rpos_sttime = time (NULL);
                        *((LONG *)RxBuffer) = Rxpos;
                        *((LONG *)(RxBuffer + sizeof (Rxpos))) = rpos_sttime;
                        SendPacket (RxBuffer, sizeof (Rxpos) + sizeof (rpos_sttime), RPOSPKT);
                        rpos_retry = time (NULL) + (TimeoutSecs / 2);
//                        Log->Write ("> SEND->RPOSPKT");
                     }
                  }
                  else {
                     last_blkpos = Rxpos;
                     rpos_retry = 0L;
                     rpos_count = 0;
                     Rxblklen -= sizeof (Rxpos);
                     write (RxFile, RxBuffer + sizeof (Rxpos), Rxblklen);
                     Rxpos += Rxblklen;
                     if (Rxpos >= RxFilesize) {
                        close (RxFile);
                        RxFile = -1;
                        if ((length = time (NULL) - length) == 0L)
                           length++;
                        Log->Write ("+CPS: %lu (%lu bytes)  Efficiency: %d%%", (Rxpos - rxstpos) / length, Rxpos, (((Rxpos - rxstpos) / length) * 100L) / (Speed / 10));
                        Log->Write ("+Received-J%s %s", (CanCrc32 == TRUE) ? "/32" : "", strupr (RxFileName));
                        if (RxFiletime) {
                           utimes.actime = RxFiletime;
                           utimes.modtime = RxFiletime;
                           utime (RxFileName, &utimes);
                        }
                        if (RxQueue != NULL)
                           RxQueue->Add ();
                        rstate = RRCVFNAME;
                     }
                  }
               }
               if (rstate == RRCVFNAME)
                  SendPacket (NULL, 0, EOFACKPKT);
               break;
            case FNAMEPKT:
//               Log->Write ("> JAN=FNAMEPKT, RS=%d", rstate);
               if (rstate == RRCVFNAME)
                  Rxpos = rxstpos = ProcessFileName ();
               *((LONG *)RxBuffer) = Rxpos;
               SharedCap = CANFREQ;
               if (CanCrc32 == TRUE)
                  SharedCap |= CANCRC32;
               RxBuffer[sizeof (Rxpos)] = SharedCap;
               SendPacket (RxBuffer, sizeof (Rxpos) + 1, FNACKPKT);
               rstate = (USHORT)((RxFileName[0] != '\0') ? RRCVBLK : RDONE);
               length = time (NULL);
               break;
            case FNACKPKT:
//               Log->Write ("> JAN=FNACKPKT, RS=%d", rstate);
               if (xstate == XRCVFNACK) {
                  xmit_retry = 0L;
                  if (TxFileName[0] != '\0') {
                     SharedCap = (UCHAR)((Rxblklen > sizeof (LONG)) ? RxBuffer[sizeof (LONG)] : 0);
                     if (SharedCap & CANCRC32)
                        CanCrc32 = TRUE;
                     if ((txpos = *((LONG *)RxBuffer)) > -1L) {
                        if (txpos != 0L)
                           Log->Write ("+Synchronizing to offset %lu", txpos);
                        lseek (TxFile, txstpos = txpos, SEEK_SET);
                        xstate = XSENDBLK;
                        txlength = time (NULL);
                     }
                     else {
                        Log->Write ("+Remote refused %s", TxFileName);
                        close (TxFile);
                        if (TxQueue != NULL && IsOutbound == FALSE) {
                           TxQueue->DeleteAfter = TxQueue->TruncateAfter = FALSE;
                           TxQueue->Remove ();
                        }
                        if (Outbound != NULL && IsOutbound == TRUE)
                           Outbound->Remove ();
                        GetNextFile ();
                        xstate = XSENDFNAME;
                     }
                  }
                  else
                     xstate = XDONE;
               }
               break;
            case RPOSPKT:
//               Log->Write ("> JAN=RPOSPKT, RS=%d", rstate);
               if (xstate == XSENDBLK || xstate == XRCVEOFACK) {
                  xmit_retry = 0L;
                  Com->ClearOutbound ();
                  txpos = *((LONG *)RxBuffer);
                  lseek (TxFile, txpos, SEEK_SET);
                  xstate = XSENDBLK;
               }
               break;
            case EOFACKPKT:
//               Log->Write ("> JAN=EOFACKPKT, RS=%d", rstate);
               if (xstate == XRCVEOFACK || xstate == XRCVFNACK) {
                  if (xstate == XRCVEOFACK && TxFile != -1) {
                     close (TxFile);
                     TxFile = -1;
                     xmit_retry = 0L;
                     if ((txlength = time (NULL) - txlength) == 0L)
                        txlength++;
                     if (TxQueue != NULL && IsOutbound == FALSE)
                        Log->Write ("+CPS: %lu (%lu bytes)  Efficiency: %d%%", (TxQueue->Size - txstpos) / txlength, TxQueue->Size, (((TxQueue->Size - txstpos) / txlength) * 100L) / (Speed / 10));
                     if (Outbound != NULL && IsOutbound == TRUE)
                        Log->Write ("+CPS: %lu (%lu bytes)  Efficiency: %d%%", (Outbound->Size - txstpos) / txlength, Outbound->Size, (((Outbound->Size - txstpos) / txlength) * 100L) / (Speed / 10));
                     Log->Write ("+Sent-Z%s %s", (CanCrc32 == TRUE) ? "/32" : "", strupr (TxFileName));
                     if (TxQueue != NULL && IsOutbound == FALSE)
                        TxQueue->Remove ();
                     if (Outbound != NULL && IsOutbound == TRUE)
                        Outbound->Remove ();
                     GetNextFile ();
                  }
                  xstate = XSENDFNAME;
               }
               break;
            case FREQPKT:
//               Log->Write ("> JAN=FREQPKT, RS=%d", rstate);
               if (xstate == XRCVFNACK) {
                  xmit_retry = 0L;
                  SharedCap = *(strchr ((PSZ)RxBuffer, '\0') + 1);
                  xstate = XSENDFREQNAK;
               }
               break;
            case HALTPKT:
//               Log->Write ("> JAN=HALTPKT, RS=%d", rstate);
               SendPacket (NULL, 0, HALTACKPKT);
               xstate = XDONE;
               rstate = RDONE;
               break;
         }
         if (pkttype == HALTPKT || (xstate == XDONE && rstate == RDONE))
            break;
      }
   } while ((xstate != XDONE || rstate != RDONE) && Com->Carrier () == TRUE);

   if (RxFile != -1)
      close (RxFile);
   if (TxFile != -1)
      close (TxFile);

   if (Outbound != NULL)
      Outbound->Update ();

/*
   Done = FALSE;
   SendPacket (NULL, 0, HALTPKT);
   xmit_retry = time (NULL) + (TimeoutSecs / 2);
   timeout = time (NULL) + 2;

   while (Done == FALSE && Com->Carrier () == TRUE && time (NULL) < xmit_retry) {
      switch (GetPacket ()) {
         case NOPKT:
         case BADPKT:
            break;

         case HALTPKT:
         case HALTACKPKT:
            Done = TRUE;
            break;

         default:
            if (time (NULL) >= timeout) {
               SendPacket (NULL, 0, HALTPKT);
               timeout = time (NULL) + 2;
            }
            break;
      }
   }
*/

   if (Com->Carrier () == TRUE) {
      SendPacket (NULL, 0, HALTACKPKT);
      SendPacket (NULL, 0, HALTACKPKT);
      SendPacket (NULL, 0, HALTACKPKT);
      SendPacket (NULL, 0, HALTACKPKT);
      SendPacket (NULL, 0, HALTACKPKT);
   }
}

// ----------------------------------------------------------------------

typedef struct {
   USHORT Sent;
   CHAR   Name[32];
   ULONG  Size;
   CHAR   Complete[128];
   UCHAR  DeleteAfter;
   UCHAR  TruncateAfter;
} FILEQUEUE;

TFileQueue::TFileQueue (void)
{
   Data.Clear ();
   New ();
   TotalFiles = 0L;
}

TFileQueue::~TFileQueue (void)
{
   Data.Clear ();
}

USHORT TFileQueue::Add (VOID)
{
   FILEQUEUE Buffer;

   memset (&Buffer, 0, sizeof (FILEQUEUE));

   Buffer.Sent = Sent;
   strcpy (Buffer.Name, Name);
   strcpy (Buffer.Complete, Complete);
   Buffer.Size = Size;
   Buffer.DeleteAfter = DeleteAfter;
   Buffer.TruncateAfter = TruncateAfter;
   TotalFiles++;

   return (Data.Add (&Buffer, sizeof (FILEQUEUE)));
}

VOID TFileQueue::Clear (VOID)
{
   Data.Clear ();
   New ();
   TotalFiles = 0L;
}

USHORT TFileQueue::First (VOID)
{
   USHORT RetVal = FALSE;
   FILEQUEUE *Buffer;

   if ((Buffer = (FILEQUEUE *)Data.First ()) != NULL) {
      Sent = Buffer->Sent;
      strcpy (Name, Buffer->Name);
      strcpy (Complete, Buffer->Complete);
      Size = Buffer->Size;
      DeleteAfter = Buffer->DeleteAfter;
      TruncateAfter = Buffer->TruncateAfter;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TFileQueue::New (VOID)
{
   Sent = FALSE;
   Name[0] = '\0';
   Complete[0] = '\0';
   Size = 0L;
   TruncateAfter = DeleteAfter = FALSE;
}

USHORT TFileQueue::Next (VOID)
{
   USHORT RetVal = FALSE;
   FILEQUEUE *Buffer;

   if ((Buffer = (FILEQUEUE *)Data.Next ()) != NULL) {
      Sent = Buffer->Sent;
      strcpy (Name, Buffer->Name);
      strcpy (Complete, Buffer->Complete);
      Size = Buffer->Size;
      DeleteAfter = Buffer->DeleteAfter;
      TruncateAfter = Buffer->TruncateAfter;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TFileQueue::Previous (VOID)
{
   USHORT RetVal = FALSE;
   FILEQUEUE *Buffer;

   if ((Buffer = (FILEQUEUE *)Data.Previous ()) != NULL) {
      Sent = Buffer->Sent;
      strcpy (Name, Buffer->Name);
      strcpy (Complete, Buffer->Complete);
      Size = Buffer->Size;
      DeleteAfter = Buffer->DeleteAfter;
      TruncateAfter = Buffer->TruncateAfter;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TFileQueue::Remove (PSZ pszName)
{
   USHORT RetVal = FALSE;
   FILEQUEUE *ft;

   if (pszName != NULL) {
      if ((ft = (FILEQUEUE *)Data.First ()) != NULL) {
         do {
            if (!stricmp (ft->Name, pszName)) {
               RetVal = TRUE;
               break;
            }
         } while ((ft = (FILEQUEUE *)Data.Next ()) != NULL);
      }
      if (RetVal == TRUE) {
         if (ft->DeleteAfter == TRUE)
            unlink (ft->Complete);
         Data.Remove ();
      }
   }
   else {
      if (DeleteAfter == TRUE)
         unlink (Complete);
      Data.Remove ();
   }

   TotalFiles--;
}

VOID TFileQueue::Update (VOID)
{
   FILEQUEUE *Buffer;

   if ((Buffer = (FILEQUEUE *)Data.Value ()) != NULL) {
      Buffer->Sent = Sent;
      strcpy (Buffer->Name, Name);
      strcpy (Buffer->Complete, Complete);
      Buffer->Size = Size;
      Buffer->DeleteAfter = DeleteAfter;
      Buffer->TruncateAfter = TruncateAfter;
   }
}


