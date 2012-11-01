
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.10
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "ftrans.h"

#define CAN          ('X'&037)

#define ZFILE        4              /* File name from sender */
#define ZEOF         11             /* End of file */
#define ZCOMPL       15             /* Request is complete */

typedef struct {
   ULONG ulSize;
   ULONG ulTime;
   CHAR  szName[17];
   CHAR  szProgram[15];
   CHAR  fNoAcks;
   CHAR  cbFiller[87];
} ZEROBLK;

TTransfer::TTransfer (class TBbs *bbs) : TZModem (bbs)
{
   Bbs = bbs;
   Cfg = bbs->Cfg;
   Com = bbs->Com;
   Lang = bbs->Lang;
   Log = bbs->Log;

   pktSize = 128;
   Soh = 0x01;
   DoCrc = FALSE;
   UseAck = TRUE;
   FinalName[0] = '\0';
   pktNumber = 0;
}

TTransfer::~TTransfer (void)
{
}

PSZ TTransfer::Receive1kXModem (PSZ pszPath)
{
   UseAck = TRUE;
   return (ReceiveXFile (pszPath));
}

SHORT TTransfer::ReceivePacket (UCHAR *lpBuffer)
{
   SHORT c;
   USHORT i, crc, recvcrc;
   UCHAR checksum;

   checksum = 0;
   crc = 0;

   if ((c = TimedRead (100)) == -1 || c != pktNumber)
      return (FALSE);
   if ((c = TimedRead (100)) == -1 || c != (UCHAR)(pktNumber ^ 0xFF))
      return (FALSE);

   if (DoCrc == TRUE) {
      for (i = 0; i < pktSize; i++, lpBuffer++) {
         if ((c = TimedRead (100)) == -1)
            return (FALSE);
         *lpBuffer = (UCHAR)c;
         crc = Crc16 (*lpBuffer, crc);
      }
      if ((c = TimedRead (100)) == -1)
         return (FALSE);
      recvcrc = (USHORT)(c << 8);
      if ((c = TimedRead (100)) == -1)
         return (FALSE);
      recvcrc = (USHORT)(recvcrc | c);
      if (recvcrc != crc)
         return (FALSE);
   }
   else {
      for (i = 0; i < pktSize; i++, lpBuffer++) {
         if ((c = TimedRead (100)) == -1)
            return (FALSE);
         *lpBuffer = (UCHAR)c;
         checksum += *lpBuffer;
      }
      if ((c = TimedRead (100)) == -1 || c != checksum)
         return (FALSE);
   }

   return (TRUE);
}

PSZ TTransfer::ReceiveXFile (PSZ pszFile)
{
   int fd;
   SHORT c, errs, fStop = FALSE, fStart = FALSE;
   UCHAR *buffer;
   PSZ pszReturn = NULL;

   if ((fd = open (pszFile, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) == -1)
      return (NULL);

   if ((buffer = (UCHAR *)malloc (1024)) != NULL) {
      errs = 0;
      DoCrc = TRUE;
      pktNumber = 1;

      while (Bbs->AbortSession () == FALSE && fStop == FALSE) {
         if ((c = TimedRead (1000)) != -1) {
            switch (c) {
               case SOH:
                  pktSize = 128;
                  if (ReceivePacket (buffer) == TRUE) {
                     if (UseAck == TRUE)
                        Com->BufferByte (ACK);
                     write (fd, buffer, pktSize);
                     errs = 0;
                     fStart = TRUE;
                     pktNumber++;
                  }
                  else {
                     Com->BufferByte (NAK);
                     if (++errs >= 10) {
                        Com->BufferBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                        fStop = TRUE;
                     }
                  }
                  break;

               case STX:
                  pktSize = 1024;
                  if (ReceivePacket (buffer) == TRUE) {
                     if (UseAck == TRUE)
                        Com->BufferByte (ACK);
                     write (fd, buffer, pktSize);
                     errs = 0;
                     fStart = TRUE;
                     pktNumber++;
                  }
                  else {
                     Com->BufferByte (NAK);
                     if (++errs >= 10) {
                        Com->BufferBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                        fStop = TRUE;
                     }
                  }
                  break;

               case EOT:
                  if (UseAck == TRUE)
                     Com->BufferByte (ACK);
                  fStop = TRUE;
                  pszReturn = pszFile;
                  break;

               case CAN:
                  if (TimedRead (100) == CAN) {
                     Com->BufferBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                     fStop = TRUE;
                  }
                  break;
            }
         }
         else {
            errs++;
            if (fStart == FALSE && errs < 5)
               Com->BufferByte ('C');
            else {
               if (errs >= 10) {
                  Com->BufferBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                  fStop = TRUE;
               }
               Com->BufferByte (NAK);
               if (fStart == FALSE)
                  DoCrc = FALSE;
            }
         }
      }

      free (buffer);
   }

   close (fd);

   return (pszReturn);
}

PSZ TTransfer::ReceiveXModem (PSZ pszPath)
{
   UseAck = TRUE;
   return (ReceiveXFile (pszPath));
}

PSZ TTransfer::ReceiveZModem (PSZ pszPath)
{
   PSZ retVal = NULL;

   if (ZInitReceiver () == ZFILE) {
      if (ZReceiveFile (pszPath) == ZEOF)
         retVal = Pathname;
   }

   return (retVal);
}

USHORT TTransfer::Send1kXModem (PSZ pszFile)
{
   Soh = STX;
   pktSize = 1024;
   pktNumber = 1;
   UseAck = TRUE;

   return (SendXFile (pszFile));
}

USHORT TTransfer::SendASCIIDump (PSZ pszFile)
{
   FILE *fp;
   int c;
   USHORT fRet, nChars;

   fRet = FALSE;
   nChars = 0;

   if ((fp = fopen (pszFile, "rb")) != NULL) {
      while (Bbs->AbortSession () == FALSE) {
         if (nChars == 0) {
            if ((c = fgetc (fp)) == EOF) {
               fRet = TRUE;
               break;
            }
            Com->BufferByte ((UCHAR)c);
         }

         if (Com->BytesReady () == TRUE) {
            if ((c = Com->ReadByte ()) == 8) {
               Com->BufferBytes ((UCHAR *)"\x08 \x08", 3);
               nChars--;
            }
            else if (c < 32)
               break;
            else {
               Com->SendByte ((UCHAR)c);
               nChars++;
            }
         }
      }

      fclose (fp);
      Com->UnbufferBytes ();
   }

   return (fRet);
}

SHORT TTransfer::SendPacket (UCHAR *lpBuffer)
{
   USHORT i, crc;
   UCHAR checksum;

   checksum = 0;
   crc = 0;

   Com->BufferByte (Soh);

   Com->BufferByte (pktNumber);
   Com->BufferByte ((UCHAR)(pktNumber ^ 0xFF));

   if (DoCrc == TRUE) {
      for (i = 0; i < pktSize; i++, lpBuffer++) {
         crc = Crc16 (*lpBuffer, crc);
         Com->BufferByte (*lpBuffer);
      }
      Com->BufferByte ((UCHAR)(crc >> 8));
      Com->BufferByte ((UCHAR)(crc & 0xFF));
   }
   else {
      for (i = 0; i < pktSize; i++, lpBuffer++) {
         checksum += *lpBuffer;
         Com->BufferByte (*lpBuffer);
      }
      Com->BufferByte (checksum);
   }

   Com->UnbufferBytes ();

   return (Bbs->AbortSession ());
}

USHORT TTransfer::SendXFile (PSZ pszFile)
{
   int fd;
   SHORT c, errs;
   USHORT fRet, fStarted;
   UCHAR *buffer;

   DoCrc = FALSE;
   errs = 0;
   fRet = FALSE;
   fStarted = FALSE;

   if ((buffer = (UCHAR *)malloc (pktSize)) == NULL)
      return (FALSE);

   if ((fd = open (pszFile, O_RDONLY|O_BINARY)) == -1) {
      free (buffer);
      return (FALSE);
   }

   memset (buffer, 26, pktSize);
   read (fd, buffer, pktSize);

   while (Bbs->AbortSession () == FALSE) {
      if ((c = TimedRead (1000)) != -1) {
         if (c == 'C' || c == NAK) {
            if (c == 'C')
               DoCrc = TRUE;
            SendPacket (buffer);
            errs++;
            fStarted = TRUE;
         }
         if (c == ACK || UseAck == FALSE) {
            pktNumber++;
            errs = 0;
            memset (buffer, 26, pktSize);
            if (read (fd, buffer, pktSize) == 0) {
               Com->SendByte ((UCHAR)EOT);
               while (Bbs->AbortSession () == FALSE) {
                  if ((c = TimedRead (100)) != -1) {
                     if (c == ACK) {
                        fRet = TRUE;
                        break;
                     }
                     else if (c == 'C' || c == NAK)
                        Com->SendByte ((UCHAR)EOT);
                  }
                  else if (++errs > 10) {
                     Com->SendByte ((UCHAR)EOT);
                     Com->SendBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
                     break;
                  }
               }
               break;
            }
            SendPacket (buffer);
            fStarted = TRUE;
         }
         if (c == CAN) {
            if (TimedRead (100) == CAN)
               break;
         }
      }
      else {
         if (++errs > 10) {
            Com->SendBytes ((UCHAR *)"\x18\x18\x18\x18\x18\x08\x08\x08\x08\x08", 10);
            break;
         }
         else if (fStarted == TRUE)
            SendPacket (buffer);
      }
   }

   free (buffer);
   close (fd);

   return (fRet);
}

USHORT TTransfer::SendXModem (PSZ pszFile)
{
   Soh = SOH;
   pktSize = 128;
   pktNumber = 1;
   UseAck = TRUE;

   return (SendXFile (pszFile));
}

USHORT TTransfer::SendYModem (PSZ pszFile)
{
   short c;
   USHORT fZeroSent, fDone, fRet, errs;
   CHAR ZeroBlock[128];
   PSZ p;
   struct find_t blk;

   Soh = SOH;
   pktSize = 128;
   pktNumber = 0;
   fZeroSent = FALSE;
   fDone = FALSE;
   fRet = FALSE;
   errs = 0;

   p = strchr (pszFile, '\0');
   while (p > pszFile && *p != ':' && *p != '\\' && *p != '/')
      p--;
   if (*p == ':' || *p == '\\' || *p == '/')
      p++;
   _dos_findfirst (pszFile, 0, &blk);

   memset (ZeroBlock, 0, sizeof (ZeroBlock));
   sprintf (ZeroBlock, "%s%c%ld %ld", p, '\0', blk.size, (ULONG)((ULONG)blk.wr_date << 16) | blk.wr_time);

   while (fDone == FALSE && Bbs->AbortSession () == FALSE) {
      if ((c = TimedRead (1000)) != -1) {
         if (c == 'C' || c == NAK) {
            if (c == 'C')
               DoCrc = TRUE;
            SendPacket ((UCHAR *)ZeroBlock);
            fZeroSent = TRUE;
         }
         if (c == ACK && fZeroSent == TRUE) {
            Soh = STX;
            pktSize = 1024;
            pktNumber = 1;
            UseAck = TRUE;
            fRet = SendXFile (pszFile);
            fDone = TRUE;
         }
      }
      else if (++errs >= 10)
         fDone = TRUE;
   }

   return (fRet);
}

USHORT TTransfer::SendYModemG (PSZ pszFile)
{
   short c;
   USHORT fZeroSent, fDone, fRet, errs;
   CHAR ZeroBlock[128];
   PSZ p;
   struct find_t blk;

   Soh = SOH;
   pktSize = 128;
   pktNumber = 0;
   fZeroSent = FALSE;
   fDone = FALSE;
   fRet = FALSE;
   errs = 0;

   p = strchr (pszFile, '\0');
   while (p > pszFile && *p != ':' && *p != '\\' && *p != '/')
      p--;
   if (*p == ':' || *p == '\\' || *p == '/')
      p++;
   _dos_findfirst (pszFile, 0, &blk);

   memset (ZeroBlock, 0, sizeof (ZeroBlock));
   sprintf (ZeroBlock, "%s%c%ld %ld", p, '\0', blk.size, (ULONG)((ULONG)blk.wr_date << 16) | blk.wr_time);

   while (fDone == FALSE && Bbs->AbortSession () == FALSE) {
      if ((c = TimedRead (1000)) != -1) {
         if (c == 'C' || c == NAK) {
            if (fZeroSent == TRUE)
               fDone = TRUE;
            else {
               if (c == 'C')
                  DoCrc = TRUE;
               SendPacket ((UCHAR *)ZeroBlock);
               fZeroSent = TRUE;
            }
         }
         if (c == ACK && fZeroSent == TRUE) {
            Soh = STX;
            pktSize = 1024;
            pktNumber = 1;
            UseAck = TRUE;
            fRet = SendXFile (pszFile);
            fDone = TRUE;
         }
      }
      else if (++errs >= 10)
         fDone = TRUE;
   }

   return (fRet);
}

USHORT TTransfer::SendZModem (PSZ pszFile)
{
   USHORT RetVal = FALSE, i;

   if (FileSent == 0 && pszFile == NULL)
      ZInitSender (TRUE);
   else
      ZInitSender (FALSE);
   if (pszFile != NULL) {
      if ((i = ZSendFile (pszFile)) == 0 || i == 5) {
         RetVal = TRUE;
         FileSent++;
      }
   }
   else
      ZEndSender ();

   return (RetVal);
}

SHORT TTransfer::TimedRead (SHORT hSec)
{
   LONG Tout = TimerSet (hSec);

   while (!TimeUp (Tout)) {
      if (Com->Carrier () == FALSE)
         break;
      if (Com->BytesReady () == TRUE)
         return (Com->ReadByte ());
      Bbs->ReleaseTimeSlice ();
   }

   return (-1);
}



