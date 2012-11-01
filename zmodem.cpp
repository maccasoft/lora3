
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "ftrans.h"

TZModem::TZModem (class TBbs *bbs)
{
   Com = bbs->Com;
   Log = bbs->Log;
   Bbs = bbs;

   FileSent = 0;
   Wantfcs32 = TRUE;
   Txfcs32 = FALSE;
   ZCtlesc = FALSE;
   Znulls = 0;
   lastsent = -1;
   Rxtimeout = 200;
   Tframlen = 1024;
   TxBuffer = RxBuffer = (char *)malloc (KSIZE);
   TryZHdrType = ZRINIT;
}

TZModem::~TZModem (void)
{
   if (TxBuffer == RxBuffer) {
      if (TxBuffer != NULL)
         free (TxBuffer);
   }
   else {
      if (TxBuffer != NULL)
         free (TxBuffer);
      if (TxBuffer != NULL)
         free (RxBuffer);
   }
}

SHORT TZModem::TimedRead (SHORT hSec)
{
   LONG Tout = TimerSet (hSec);

   while (!TimeUp (Tout) && Bbs->AbortSession () == FALSE) {
      if (Com->BytesReady () == TRUE)
         return (Com->ReadByte ());
      Bbs->ReleaseTimeSlice ();
   }

   return ((Bbs->AbortSession () == TRUE) ? RCDO : TIMEOUT);
}

VOID TZModem::ZAckBiBi (VOID)
{
   int n;

   ZPutLong (Txhdr, 0L);

   for (n = 4; --n >= 0; ) {
      ZSendHexHeader (ZFIN, Txhdr);
      for (;;) {
         switch (TimedRead ((SHORT)Rxtimeout)) {
            case 'O':
               TimedRead ((SHORT)Rxtimeout);
            /* ***** FALL THRU TO ***** */
            case TIMEOUT:
            case RCDO:
               return;
         }
      }
   }
}

short TZModem::ZDLRead (void)
{
   short c;

   if ((c = TimedRead ((SHORT)Rxtimeout)) != ZDLE)
      return (c);
   if ((c = TimedRead ((SHORT)Rxtimeout)) < 0)
      return (c);
   if (c == CAN && (c = TimedRead ((SHORT)Rxtimeout)) < 0)
      return (c);
   if (c == CAN && (c = TimedRead ((SHORT)Rxtimeout)) < 0)
      return (c);
   if (c == CAN && (c = TimedRead ((SHORT)Rxtimeout)) < 0)
      return (c);
   switch (c) {
      case CAN:
         return (GOTCAN);
      case ZCRCE:
      case ZCRCG:
      case ZCRCQ:
      case ZCRCW:
         return ((short)(c | GOTOR));
      case ZRUB0:
         return (0177);
      case ZRUB1:
         return (0377);
      default:
         if ((c & 0140) ==  0100)
            return ((short)(c ^ 0100));
         break;
   }
   return (ZERROR);
}

LONG TZModem::ZGetLong (char *hdr)
{
   long l;

   l = (hdr[ZP3] & 0377);
   l = (l << 8) | (hdr[ZP2] & 0377);
   l = (l << 8) | (hdr[ZP1] & 0377);
   l = (l << 8) | (hdr[ZP0] & 0377);

   return (l);
}

SHORT TZModem::ZReceiveData (char *buf, short length)
{
   short d, c;
   unsigned short crc;
   unsigned long ulCrc;

   if (Rxframeind == ZBIN32) {
      ulCrc = 0xFFFFFFFFL;
      Rxcount = 0;
      for (;;) {
         if ((c = ZDLRead()) & ~0377) {
crcfoo2:
            switch (c) {
               case GOTCRCE:
               case GOTCRCG:
               case GOTCRCQ:
               case GOTCRCW:
                  d = c;
                  ulCrc = Crc32 ((UCHAR)(c & 0377), ulCrc);
                  if ((c = ZDLRead ()) & ~0377)
                     goto crcfoo2;
                  ulCrc = Crc32 ((UCHAR)c, ulCrc);
                  if ((c = ZDLRead ()) & ~0377)
                     goto crcfoo2;
                  ulCrc = Crc32 ((UCHAR)c, ulCrc);
                  if ((c = ZDLRead ()) & ~0377)
                     goto crcfoo2;
                  ulCrc = Crc32 ((UCHAR)c, ulCrc);
                  if ((c = ZDLRead ()) & ~0377)
                     goto crcfoo2;
                  ulCrc = Crc32 ((UCHAR)c, ulCrc);
                  if (ulCrc != 0xDEBB20E3L)
                     return (ZERROR);
                  return (d);
               case GOTCAN:
                  return (ZCAN);
               case RCDO:
               case TIMEOUT:
                  return (c);
               default:
                  return (c);
            }
         }
         if (--length < 0)
            return (ZERROR);
         ++Rxcount;
         *buf++ = (char)c;
         ulCrc = Crc32 ((UCHAR)c, ulCrc);
      }
   }
   else {
      crc = Rxcount = 0;
      for (;;) {
         if ((c = ZDLRead()) & ~0377) {
crcfoo1:
            switch (c) {
               case GOTCRCE:
               case GOTCRCG:
               case GOTCRCQ:
               case GOTCRCW:
                  d = c;
                  crc = Crc16 ((UCHAR)(c & 0377), crc);
                  if ((c = ZDLRead ()) & ~0377)
                     goto crcfoo1;
                  crc = Crc16 ((UCHAR)c, crc);
                  if ((c = ZDLRead ()) & ~0377)
                     goto crcfoo1;
                  crc = Crc16 ((UCHAR)c, crc);
                  if (crc & 0xFFFF)
                     return (ZERROR);
                  return (d);
               case GOTCAN:
                  return (ZCAN);
               case RCDO:
               case TIMEOUT:
                  return (c);
               default:
                  return (c);
            }
         }
         if (--length < 0)
            return (ZERROR);
         ++Rxcount;
         *buf++ = (char)c;
         crc = Crc16 ((UCHAR)c, crc);
      }
   }
}

void TZModem::ZSendLine (short c)
{
   switch (c &= 0377) {
      case ZDLE:
         Com->BufferByte (ZDLE);
         Com->BufferByte ((UCHAR)(lastsent = (SHORT)(c ^= 0100)));
         break;
      case 015:
      case 0215:
         if (!ZCtlesc && (lastsent & 0177) != '@')
            goto sendit;
      /* **** FALL THRU TO **** */
      case 020:
      case 021:
      case 023:
      case 0220:
      case 0221:
      case 0223:
      case 0377:
         Com->BufferByte (ZDLE);
         c ^= 0100;
sendit:
         Com->BufferByte ((UCHAR)(lastsent = c));
         break;
      default:
         if (ZCtlesc && !(c & 0140)) {
            Com->BufferByte (ZDLE);
            c ^= 0100;
         }
         Com->BufferByte ((UCHAR)(lastsent = c));
         break;
   }
}

// Invia un header binario
void TZModem::ZSendBinaryHeader (short type, char *hdr)
{
   short n;
   unsigned short crc;
   unsigned long  ulCrc;

   if (Bbs->AbortSession () == FALSE) {
      if (type == ZDATA) {
         for (n = Znulls; --n >= 0; )
            Com->BufferByte ((char)0);
      }

      Com->BufferByte (ZPAD);
      Com->BufferByte (ZDLE);

      if (Txfcs32 == TRUE) {
         Com->BufferByte (ZBIN32);
         ZSendLine (type);
         ulCrc = Crc32 ((unsigned char)type, 0xFFFFFFFFL);

         for (n = 4; --n >= 0; ) {
            ZSendLine (*hdr);
            ulCrc = Crc32 ((unsigned char)((0377& *hdr++)), ulCrc);
         }
         ulCrc = ~ulCrc;
         for (n = 4; --n >= 0;) {
            ZSendLine ((short)ulCrc);
            ulCrc >>= 8;
         }
      }
      else {
         Com->BufferByte (ZBIN);
         ZSendLine (type);
         crc = Crc16 ((unsigned char)type, 0);

         for (n = 4; --n >= 0; ) {
            ZSendLine (*hdr);
            crc = Crc16 ((unsigned char)((0377& *hdr++)), crc);
         }
         ZSendLine ((char)(crc >> 8));
         ZSendLine (crc);
      }

      Com->UnbufferBytes ();
   }
}

void TZModem::ZSendData (char *buf, short length, short frameend)
{
   short n;
   unsigned short crc;
   unsigned long  ulCrc;

   if (Bbs->AbortSession () == FALSE) {
      if (Txfcs32 == TRUE) {
         ulCrc = 0xFFFFFFFFL;
         for ( ; --length >= 0; ) {
            ZSendLine (*buf);
            ulCrc = Crc32 ((char)((0377 & *buf++)), ulCrc);
         }
         Com->BufferByte (ZDLE);
         Com->BufferByte ((char)frameend);
         ulCrc = Crc32 ((char)frameend, ulCrc);
         ulCrc = ~ulCrc;
         for (n = 4; --n >= 0;) {
            ZSendLine ((short)ulCrc);
            ulCrc >>= 8;
         }

         Com->UnbufferBytes ();
      }
      else {
         crc = 0;
         for ( ; --length >= 0; ) {
            ZSendLine (*buf);
            crc = Crc16 ((char)((0377 & *buf++)), crc);
         }
         Com->BufferByte (ZDLE);
         Com->BufferByte ((char)frameend);
         crc = Crc16 ((char)frameend, crc);

         ZSendLine ((char)(crc >> 8));
         ZSendLine (crc);

         Com->UnbufferBytes ();

         if (frameend == ZCRCW)
            Com->SendByte (XON);
      }
   }
}

short TZModem::ZGetByte (void)
{
   short c;

   while (Bbs->AbortSession () == FALSE) {
      if ((c = TimedRead ((SHORT)Rxtimeout)) < 0)
         return (TIMEOUT);
      switch (c &= 0177) {
         case XON:
         case XOFF:
            continue;
         default:
            return (c);
      }
   }

   return (RCDO);
}

short TZModem::ZGetHex (void)
{
   short c, n;

   if ((c = ZGetByte ()) < 0)
      return (c);
   n = (short)(c - '0');
   if (n > 9)
      n -= ('a' - ':');
   if (n & ~0xF)
      return (ZERROR);
   if ((c = ZGetByte ()) < 0)
      return (c);
   c -= '0';
   if (c > 9)
      c -= ('a' - ':');
   if (c & ~0xF)
      return (ZERROR);
   c += (n << 4);
   return (c);
}

short TZModem::ZReceiveHexHeader (char *hdr)
{
   short n, c;
   unsigned short crc;

   if ((c = ZGetHex ()) < 0)
      return (c);
   Rxtype = c;
   crc = Crc16 ((char)c, 0);

   for (n = 4; --n >= 0;) {
      if ((c = ZGetHex ()) < 0)
         return (c);
      crc = Crc16 ((char)c, crc);
      *hdr++ = (char)c;
   }
   if ((c = ZGetHex ()) < 0)
      return (c);
   crc = Crc16 ((char)c, crc);
   if ((c = ZGetHex ()) < 0)
      return (c);
   crc = Crc16 ((char)c, crc);
   if (crc & 0xFFFF)
      return (ZERROR);
   if (TimedRead (20) == '\r')        /* Throw away possible cr/lf */
      TimedRead (20);
   return (Rxtype);
}

short TZModem::ZReceiveBinaryHeader (char *hdr)
{
   short c, n;
   unsigned short crc;

   if ((c = ZDLRead ()) & ~0377)
      return (c);
   Rxtype = c;
   crc = Crc16 ((char)c, 0);

   for (n = 4; --n >= 0; ) {
      if ((c = ZDLRead ()) & ~0377)
         return (c);
      crc = Crc16 ((char)c, crc);
      *hdr++ = (char)c;
   }
   if ((c = ZDLRead ()) & ~0377)
      return (c);
   crc = Crc16 ((char)c, crc);
   if ((c = ZDLRead ()) & ~0377)
      return (c);
   crc = Crc16 ((char)c, crc);
   if (crc & 0xFFFF)
      return (ZERROR);
   return (Rxtype);
}

short TZModem::ZReceiveBinaryHeader32 (char *hdr)
{
   short c, n;
   unsigned long crc;

   if ((c = ZDLRead ()) & ~0377)
      return (c);
   Rxtype = c;
   crc = Crc32 ((char)c, 0xFFFFFFFFL);

   for (n = 4; --n >= 0; ) {
      if ((c = ZDLRead ()) & ~0377)
         return (c);
      crc = Crc32 ((UCHAR)c, crc);
      *hdr++ = (char)c;
   }
   for (n = 4; --n >= 0;) {
      if ((c = ZDLRead ()) & ~0377)
         return (c);
      crc = Crc32 ((UCHAR)c, crc);
   }
   if (crc != 0xDEBB20E3L)
      return(ZERROR);

   return (Rxtype);
}

short TZModem::ZGetHeader (char *hdr)
{
   short c, n, cancount;

   n = 2400;                     /* Max characters before start of frame */
   cancount = 5;

again:
   Rxframeind = Rxtype = 0;
   switch (c = ZGetByte ()) {
      case RCDO:
      case TIMEOUT:
         goto fifi;
      case CAN:
         if (--cancount <= 0) {
            c = ZCAN;
            goto fifi;
         }
      /* **** FALL THRU TO **** */
      default:
agn2:
         if ( --n == 0)
            return (ZERROR);
         if (c != CAN)
            cancount = 5;
         goto again;
      case ZPAD:              /* This is what we want. */
         break;
   }
   cancount = 5;
splat:
   switch (c = ZGetByte ()) {
      case ZPAD:
         goto splat;
      case RCDO:
      case TIMEOUT:
         goto fifi;
      default:
         goto agn2;
      case ZDLE:              /* This is what we want. */
         break;
   }

   switch (c = ZGetByte ()) {
      case RCDO:
      case TIMEOUT:
         goto fifi;
      case ZBIN:
         Rxframeind = ZBIN;
         c = ZReceiveBinaryHeader (hdr);
         break;
      case ZBIN32:
         Rxframeind = ZBIN32;
         c = ZReceiveBinaryHeader32 (hdr);
         break;
      case ZHEX:
         Rxframeind = ZHEX;
         c = ZReceiveHexHeader (hdr);
         break;
      case CAN:
         if (--cancount <= 0) {
            c = ZCAN;
            goto fifi;
         }
         goto agn2;
      default:
         goto agn2;
   }

   Rxpos = hdr[ZP3] & 0377;
   Rxpos = (Rxpos << 8) + (hdr[ZP2] & 0377);
   Rxpos = (Rxpos << 8) + (hdr[ZP1] & 0377);
   Rxpos = (Rxpos << 8) + (hdr[ZP0] & 0377);

fifi:
   if (c == GOTCAN)
      c = ZCAN;

   return (c);
}

void TZModem::ZPutHex (short c)
{
   char digits[] = "0123456789abcdef";

   Com->BufferByte (digits[(c & 0xF0) >> 4]);
   Com->BufferByte (digits[(c) & 0xF]);
}

void TZModem::ZSendHexHeader (short type, char *hdr)
{
   short n;
   unsigned short crc;

   if (Bbs->AbortSession () == FALSE) {
      Com->BufferByte (ZPAD);
      Com->BufferByte (ZPAD);
      Com->BufferByte (ZDLE);
      Com->BufferByte (ZHEX);
      ZPutHex (type);

      crc = Crc16 ((char)type, 0);
      for (n = 4; --n >= 0; ) {
         ZPutHex (*hdr);
         crc = Crc16 ((char)((0377& *hdr++)), crc);
      }
      ZPutHex ((unsigned char)(crc >> 8));
      ZPutHex (crc);

      Com->BufferByte (015);
      Com->BufferByte (012);

      if (type != ZFIN)
         Com->BufferByte (021);

      Com->UnbufferBytes ();
   }
}

void TZModem::ZPutLong (char *hdr, long pos)
{
   hdr[ZP0] = (char)pos;
   hdr[ZP1] = (char)(pos >> 8);
   hdr[ZP2] = (char)(pos >> 16);
   hdr[ZP3] = (char)(pos >> 24);
}

SHORT TZModem::ZInitReceiver (VOID)
{
   short i, n, cmdzack1flg, errors;

   FileSent = 0;
   Txfcs32 = TRUE;
   errors = 0;
   Com->ClearInbound ();

   for (n = 10; --n >= 0 && Bbs->AbortSession () == FALSE; ) {
      ZPutLong (Txhdr, 0L);
      Txhdr[ZF0] = CANFC32|CANFDX|CANOVIO;
      ZSendHexHeader (TryZHdrType, Txhdr);

again:
      switch ((i = ZGetHeader (Rxhdr))) {
         case ZRQINIT:
            break;

         case ZFILE:
            TryZHdrType = ZRINIT;
            if (ZReceiveData (RxBuffer, KSIZE) == GOTCRCW)
               return (ZFILE);
            ZSendHexHeader (ZNAK, Txhdr);
            goto again;

         case ZSINIT:
            if (ZReceiveData (Attn, ZATTNLEN) == GOTCRCW) {
               ZPutLong (Txhdr, 1L);
               ZSendHexHeader (ZACK, Txhdr);
            }
            else
               ZSendHexHeader (ZNAK, Txhdr);
            goto again;

         case ZFREECNT:
            ZPutLong (Txhdr, 0xFFFFFFFFL);
            ZSendHexHeader (ZACK, Txhdr);
            goto again;

         case ZCOMMAND:
            cmdzack1flg = Rxhdr[ZF0];
            if (ZReceiveData (RxBuffer, KSIZE) == GOTCRCW) {
               if (cmdzack1flg & ZCACK1)
                  ZPutLong (Txhdr, 0);
               else
                  ZPutLong (Txhdr, 0);
               do {
                  ZSendHexHeader (ZCOMPL, Txhdr);
               } while (++errors < 10 && ZGetHeader (Rxhdr) != ZFIN);
               ZAckBiBi();
               return (ZCOMPL);
            }
            ZSendHexHeader (ZNAK, Txhdr);
            goto again;

         case ZCOMPL:
            goto again;

         case ZFIN:
            ZAckBiBi();
            return (ZCOMPL);

         case RCDO:
         case ZCAN:
            return (ZERROR);
      }
   }

   return (OK);
}

short TZModem::ZInitSender (short nothing_to_do)
{
   short i;

   if (nothing_to_do == FALSE && FileSent == 0) {
      Com->SendBytes ((unsigned char *)"rz\r", 3);
      ZPutLong (Txhdr, 0L);
      ZSendHexHeader (ZRQINIT, Txhdr);
   }

   if (nothing_to_do == TRUE || (nothing_to_do == FALSE && FileSent == 0)) {
      for (i = 0; i < 10 && Bbs->AbortSession () == FALSE; i++) {
         switch (ZGetHeader (Rxhdr)) {
            case ZCHALLENGE:                 /* Echo receiver's challenge numbr */
               ZPutLong (Txhdr, Rxpos);
               ZSendHexHeader (ZACK, Txhdr);
               break;

            case ZCOMMAND:                   /* They didn't see out ZRQINIT */
               ZPutLong (Txhdr, 0L);
               ZSendHexHeader (ZRQINIT, Txhdr);
               break;

            case ZRINIT:
               Rxflags = (short)(0377 & Rxhdr[ZF0]);
               if (Wantfcs32 && (Rxflags & CANFC32))
                  Txfcs32 = TRUE;
               Rxbuflen = (short)((0377 & Rxhdr[ZP0]) + ((0377 & Rxhdr[ZP1]) << 8));
               /* Use 1024 byte frames if no sample/interrupt */
               if (Rxbuflen < 32 || Rxbuflen > 1024)
                  Rxbuflen = 1024;
               /* Override to force shorter frame length */
               if (Rxbuflen && (Rxbuflen > Tframlen) && (Tframlen >= 32))
                  Rxbuflen = Tframlen;
               if (!Rxbuflen && (Tframlen >= 32) && (Tframlen <= 1024))
                  Rxbuflen = Tframlen;
               return (OK);

            case RCDO:
            case ZCAN:
            case TIMEOUT:
               return (ZERROR);

            case ZRQINIT:
               if (Rxhdr[ZF0] == ZCOMMAND)
                  break;

            default:
               ZSendHexHeader (ZNAK, Txhdr);
               break;
         }
      }

      return (ZERROR);
   }

   return (OK);
}

SHORT TZModem::ZReceiveFile (PSZ pszPath)
{
   FILE *fout;
   SHORT c, n, gota;
   CHAR *p, *q, *fileinfo;
   long length, filesize, filetime;
   struct utimbuf utimes;
   struct stat f;

   for (p = RxBuffer, q = RxBuffer; *p; p++) {
      if (*p == '/' || *p == '\\' || *p == ':')
         q = p + 1;
   }

   sprintf (Pathname, "%s%s", pszPath, q);
   strupr (Pathname);

   filesize = filetime = 0L;
   fileinfo = RxBuffer + strlen (RxBuffer) + 1;
   if (*fileinfo)
      sscanf (fileinfo, "%ld %lo", &filesize, &filetime);

	Log->Write (" Receiving %s; %ldb, %d min.", q, filesize, (int)((filesize * 10 / Bbs->CarrierSpeed + 53) / 54));

   if ((fout = fopen (Pathname, "rb")) != NULL) {
      // If the file already exists, it's date and size are verified in order to
      // prevent other end to send us a file that we have already received.
      fstat (fileno (fout), &f);
      fclose (fout);
      if (filesize == f.st_size && filetime == f.st_mtime) {
         Log->Write ("+Already have %s", Pathname);
         ZSendHexHeader (ZSKIP, Txhdr);
         return (ZEOF);
      }
      else {
         if ((p = strchr (Pathname, '\0')) != NULL) {
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
               fout = fopen (Pathname, "rb");
               fclose (fout);
            } while (fout != NULL);
	         Log->Write ("+Renaming dupe file %s", Pathname);
         }
      }
   }

   n = 10;
   Rxbytes = 0L;
   length = time (NULL);

   if ((fout = fopen (Pathname, "wb")) == NULL)
      return (ZERROR);

   while (Bbs->AbortSession () == FALSE) {
      ZPutLong (Txhdr, Rxbytes);
      ZSendBinaryHeader (ZRPOS, Txhdr);

nxthdr:
      switch (c = ZGetHeader (Rxhdr)) {
         case ZNAK:
         case TIMEOUT:
            if ( --n < 0) {
               if (fout != NULL) {
                  fclose (fout);
                  fout = NULL;
               }
               return (ZERROR);
            }
            break;

         case ZFILE:
            ZReceiveData (RxBuffer, KSIZE);
            continue;

         case ZEOF:
            if (ZGetLong (Rxhdr) != Rxbytes)
               continue;
            if (fout != NULL) {
               fclose (fout);
               fout = NULL;
               if ((length = time (NULL) - length) == 0L)
                  length++;
               Log->Write ("+CPS: %lu (%lu bytes)  Efficiency: %d%%", Rxbytes / length, Rxbytes, ((Rxbytes / length) * 100L) / (Bbs->CarrierSpeed / 10));
               Log->Write ("+Received-Z%s %s", (Txfcs32 == TRUE) ? "/32" : "", strupr (Pathname));
               if (filetime) {
                  utimes.actime = filetime;
                  utimes.modtime = filetime;
                  utime (Pathname, &utimes);
               }
            }
            return (c);

         case ZERROR:     /* Too much garbage in header search error */
            if (--n < 0) {
               if (fout != NULL) {
                  fclose (fout);
                  fout = NULL;
               }
               return (ZERROR);
            }
            Com->SendBytes ((unsigned char *)Attn, (USHORT)strlen (Attn));
            continue;

         case ZDATA:
            if (ZGetLong (Rxhdr) != Rxbytes) {
               if (--n < 0) {
                  if (fout != NULL) {
                     fclose (fout);
                     fout = NULL;
                  }
                  return (ZERROR);
               }
               Com->SendBytes ((unsigned char *)Attn, (USHORT)strlen (Attn));
               continue;
            }

moredata:
            switch (c = ZReceiveData (RxBuffer, KSIZE)) {
               case ZCAN:
                  if (fout != NULL) {
                     fclose (fout);
                     fout = NULL;
                  }
                  return (ZERROR);

               case ZERROR:     /* CRC error */
                  if (--n < 0) {
                     if (fout != NULL) {
                        fclose (fout);
                        fout = NULL;
                     }
                     return (ZERROR);
                  }
                  Com->SendBytes ((unsigned char *)Attn, (USHORT)strlen (Attn));
                  continue;

               case TIMEOUT:
                  if (--n < 0) {
                     if (fout != NULL) {
                        fclose (fout);
                        fout = NULL;
                     }
                     return (ZERROR);
                  }
                  continue;

               case GOTCRCW:
                  n = 10;
                  fwrite (RxBuffer, Rxcount, 1, fout);
                  Rxbytes += Rxcount;
                  ZPutLong (Txhdr, Rxbytes);
                  ZSendBinaryHeader (ZACK, Txhdr);
                  goto nxthdr;

               case GOTCRCQ:
                  n = 10;
                  fwrite (RxBuffer, Rxcount, 1, fout);
                  Rxbytes += Rxcount;
                  ZPutLong (Txhdr, Rxbytes);
                  ZSendBinaryHeader (ZACK, Txhdr);
                  goto moredata;

               case GOTCRCG:
                  n = 10;
                  fwrite (RxBuffer, Rxcount, 1, fout);
                  Rxbytes += Rxcount;
                  goto moredata;

               case GOTCRCE:
                  n = 10;
                  fwrite (RxBuffer, Rxcount, 1, fout);
                  Rxbytes += Rxcount;
                  goto nxthdr;
            }
            break;
      }
   }

   if (fout != NULL) {
      fclose (fout);
      fout = NULL;
   }

   return (ZERROR);
}

short TZModem::ZSendFile (char *file)
{
   FILE *fp;
   short c, filedone, len;
   char buf[64], *p, *q;
   unsigned long length;
   struct stat f;

   if ((fp = fopen (file, "rb")) == NULL)
      return (ZERROR);

   for (p = file, q = buf; *p; p++) {
      if ((*q++ = *p) == '/' || *p == '\\' || *p == ':')
         q = buf;
   }
   *q++ = 0;

   fstat (fileno (fp), &f);
   sprintf (q, "%lu %lo %o", f.st_size, f.st_mtime, f.st_mode);
   length = time (NULL);

	Log->Write (" Sending %s; %ldb, %d min.", file, f.st_size, (int)((f.st_size * 10 / Bbs->CarrierSpeed + 53) / 54));

   do {
      Txhdr[ZF0] = LZCONV;    /* file conversion request */
      Txhdr[ZF1] = LZMANAG;   /* file management request */
      Txhdr[ZF2] = LZTRANS;   /* file transport request */
      Txhdr[ZF3] = 3;
      ZSendBinaryHeader (ZFILE, Txhdr);
      ZSendData (buf, (short)(strlen (buf) + 1 + strlen (q)), ZCRCW);

      switch (c = ZGetHeader (Rxhdr)) {
         case ZRINIT:
            continue;

         case ZCAN:
         case TIMEOUT:
         case ZABORT:
         case ZFIN:
         case RCDO:
            return (ZERROR);

         case ZSKIP:
            fclose (fp);
            Log->Write ("+Remote refused %s", buf);
            return (c);

         case ZRPOS:
            break;

         case ZERROR:
         default:
            continue;
      }
   } while (c != ZRPOS && Bbs->AbortSession () == FALSE);

   fseek (fp, Rxpos, SEEK_SET);
   Txpos = Rxpos;
   filedone = FALSE;

   if (Txpos < f.st_size) {
      ZPutLong (Txhdr, Txpos);
      ZSendBinaryHeader (ZDATA, Txhdr);
   }

   while (Bbs->AbortSession () == FALSE) {
      if (filedone == FALSE) {
         if ((len = (short)fread (TxBuffer, 1, Rxbuflen, fp)) > 0) {
            if (len < Rxbuflen)
               ZSendData (TxBuffer, len, ZCRCE);
            else
               ZSendData (TxBuffer, len, ZCRCG);
            Txpos += len;
         }
         else {
            filedone = TRUE;
            ZPutLong (Txhdr, Txpos);
            ZSendBinaryHeader (ZEOF, Txhdr);
         }
      }

      while (Com->BytesReady () == TRUE && Bbs->AbortSession () == FALSE) {
         c = Com->ReadByte ();
         if (c == CAN || c == ZPAD) {
            switch (c = ZGetHeader (Rxhdr)) {
               case ZACK:
                  continue;

               case ZRINIT:
                  fclose (fp);
                  if ((length = time (NULL) - length) == 0L)
                     length++;
                  Log->Write ("+CPS: %lu (%lu bytes)  Efficiency: %d%%", f.st_size / length, f.st_size, ((f.st_size / length) * 100L) / (Bbs->CarrierSpeed / 10));
                  Log->Write ("+Sent-Z%s %s", (Txfcs32 == TRUE) ? "/32" : "", strupr (file));
                  return (OK);

               case ZRPOS:
                  Txpos = Rxpos;
                  fseek (fp, Rxpos, SEEK_SET);
                  filedone = FALSE;
                  Com->ClearOutbound ();
                  if (Txpos < f.st_size) {
                     ZPutLong (Txhdr, Txpos);
                     ZSendBinaryHeader (ZDATA, Txhdr);
                  }
                  break;

               case ZSKIP:
                  fclose (fp);
                  Log->Write ("+Remote refused %s", buf);
                  return (c);
            }
         }
      }

      Bbs->ReleaseTimeSlice ();
   }

   fclose (fp);
   return (ZERROR);
}

void TZModem::ZEndSender (void)
{
   FileSent = 0;

   while (Bbs->AbortSession () == FALSE) {
      ZPutLong (Txhdr, 0L);
      ZSendBinaryHeader (ZFIN, Txhdr);

      switch (ZGetHeader (Rxhdr)) {
         case ZFIN:
            Com->SendByte ('O');
            Com->SendByte ('O');

         case ZCAN:
         case TIMEOUT:
         case RCDO:
            return;
      }
   }
}

