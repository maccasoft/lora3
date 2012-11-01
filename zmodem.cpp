
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "ftrans.h"

TZModem::TZModem (void)
{
   Com = NULL;
   Log = NULL;
   Progress = NULL;
   EndRun = Hangup = FALSE;

   TxBuffer = RxBuffer;
   FileSent = 0;
   Wantfcs32 = TRUE;
   Txfcs32 = FALSE;
   ZCtlesc = FALSE;
   Znulls = 0;
   lastsent = -1;
   Rxtimeout = 200;
   Tframlen = 1024;
   TryZHdrType = ZRINIT;
   Maxblklen = 1024;
   RxTempSize = 0;
   Telnet = FALSE;
}

TZModem::~TZModem (void)
{
}

USHORT TZModem::AbortSession (VOID)
{
   USHORT RetVal = FALSE;

   if (EndRun == TRUE || Hangup == TRUE)
      RetVal = TRUE;
   else if (Com != NULL && Com->Carrier () == FALSE)
      RetVal = TRUE;

   return (RetVal);
}

/*
SHORT TZModem::TimedRead (LONG hSec)
{
   SHORT RetVal = -1;
   ULONG tout;

   if (RxTempSize == 0) {
      if (Com->BytesReady () == FALSE) {
         tout = TimerSet (hSec);
         do {
            if (Com->BytesReady () == TRUE)
               break;
         } while (!TimeUp (tout) && AbortSession () == FALSE);
      }
      if (Com->BytesReady () == TRUE) {
         RxTempSize = (SHORT)Com->ReadBytes ((UCHAR *)RxTemp, sizeof (RxTemp));
         RxTempPos = RxTemp;
      }
   }

   if (RxTempSize > 0) {
      RetVal = *RxTempPos++;
      RxTempSize--;
   }

   return (RetVal);
}
*/

SHORT TZModem::TimedRead (LONG hSec)
{
   SHORT RetVal = -1;
   ULONG tout;

   if (Com->BytesReady () == FALSE) {
      tout = TimerSet (hSec);
      do {
         if (Com->BytesReady () == TRUE) {
            RetVal = Com->ReadByte ();
            break;
         }
      } while (!TimeUp (tout) && AbortSession () == FALSE);
   }
   else
      RetVal = Com->ReadByte ();

   return (RetVal);
}

VOID TZModem::ZAckBiBi (VOID)
{
   int n;

   ZPutLong (Txhdr, 0L);

   for (n = 4; --n >= 0 && AbortSession () == FALSE; ) {
      ZSendHexHeader (ZFIN, Txhdr);
      switch (TimedRead (100)) {
         case 'O':
            TimedRead (1);
         /* ***** FALL THRU TO ***** */
         case TIMEOUT:
         case RCDO:
            return;
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
   if (c == CAN) {
      if ((c = TimedRead ((SHORT)Rxtimeout)) < 0)
         return (c);
   }
   if (c == CAN) {
      if ((c = TimedRead ((SHORT)Rxtimeout)) < 0)
         return (c);
   }
   if (c == CAN) {
      if ((c = TimedRead ((SHORT)Rxtimeout)) < 0)
         return (c);
   }
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
   char *endpos;
   unsigned short crc;
   unsigned long ulCrc;

   Rxcount = 0;
   endpos = buf + length;

   if (Rxframeind == ZBIN32) {
      ulCrc = 0xFFFFFFFFL;
      while (buf <= endpos) {
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
         ++Rxcount;
         if (buf < endpos)
            *buf++ = (char)c;
         ulCrc = Crc32 ((UCHAR)c, ulCrc);
      }
   }
   else {
      crc = 0;
      while (buf <= endpos) {
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
         ++Rxcount;
         if (buf < endpos)
            *buf++ = (char)c;
         crc = Crc16 ((UCHAR)c, crc);
      }
   }

   return (ZERROR);
}

void TZModem::ZSendLine (unsigned char c)
{
   switch (c) {
      case 0177:
         if (Telnet == TRUE) {
            Com->BufferByte (ZDLE);
            Com->BufferByte ((UCHAR)(lastsent = ZRUB0));
         }
         else
            Com->BufferByte ((UCHAR)(lastsent = c));
         break;
      case 0377:
         if (Telnet == TRUE) {
            Com->BufferByte (ZDLE);
            Com->BufferByte ((UCHAR)(lastsent = ZRUB1));
         }
         else
            Com->BufferByte ((UCHAR)(lastsent = c));
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
      case ZDLE:
         Com->BufferByte (ZDLE);
         c ^= 0x40;
      default:
sendit:
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

   if (AbortSession () == FALSE) {
      if (type == ZDATA) {
         for (n = Znulls; --n >= 0; )
            Com->BufferByte ((char)0);
      }

      Com->BufferByte (ZPAD);
      Com->BufferByte (ZDLE);

      if (Txfcs32 == TRUE) {
         Com->BufferByte (ZBIN32);
         ZSendLine ((unsigned char)type);
         ulCrc = Crc32 ((unsigned char)type, 0xFFFFFFFFL);

         for (n = 4; --n >= 0; hdr++) {
            ZSendLine ((unsigned char)*hdr);
            ulCrc = Crc32 ((unsigned char)*hdr, ulCrc);
         }
         ulCrc = ~ulCrc;
         for (n = 4; --n >= 0;) {
            ZSendLine ((unsigned char)ulCrc);
            ulCrc >>= 8;
         }
      }
      else {
         Com->BufferByte (ZBIN);
         ZSendLine ((unsigned char)type);
         crc = Crc16 ((unsigned char)type, 0);

         for (n = 4; --n >= 0; hdr++) {
            ZSendLine ((unsigned char)*hdr);
            crc = Crc16 ((unsigned char)*hdr, crc);
         }
         ZSendLine ((unsigned char)(crc >> 8));
         ZSendLine ((unsigned char)crc);
      }

      Com->UnbufferBytes ();
   }
}

void TZModem::ZSendData (char *buf, short length, short frameend)
{
   short n;
   unsigned short crc;
   unsigned long  ulCrc;

   if (AbortSession () == FALSE) {
      if (Txfcs32 == TRUE) {
         ulCrc = 0xFFFFFFFFL;
         for ( ; --length >= 0; buf++) {
            ZSendLine ((unsigned char)*buf);
            ulCrc = Crc32 ((unsigned char)*buf, ulCrc);
         }
         Com->BufferByte (ZDLE);
         Com->BufferByte ((unsigned char)frameend);
         ulCrc = Crc32 ((unsigned char)frameend, ulCrc);
         ulCrc = ~ulCrc;
         for (n = 4; --n >= 0;) {
            ZSendLine ((unsigned char)ulCrc);
            ulCrc >>= 8;
         }

         Com->UnbufferBytes ();
      }
      else {
         crc = 0;
         for ( ; --length >= 0; buf++) {
            ZSendLine ((unsigned char)*buf);
            crc = Crc16 ((unsigned char)*buf, crc);
         }
         Com->BufferByte (ZDLE);
         Com->BufferByte ((unsigned char)frameend);
         crc = Crc16 ((unsigned char)frameend, crc);

         ZSendLine ((unsigned char)(crc >> 8));
         ZSendLine ((unsigned char)(crc & 0xFF));

         Com->UnbufferBytes ();

//         if (frameend == ZCRCW)
//            Com->SendByte (XON);
      }
   }
}

short TZModem::ZGetByte (void)
{
   short c;

   while (AbortSession () == FALSE) {
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

   if (AbortSession () == FALSE) {
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
   short n, errors;

   FileSent = 0;
   Txfcs32 = TRUE;
   errors = 0;
   Com->ClearInbound ();
   RxTempSize = 0;

   for (n = 10; --n >= 0 && AbortSession () == FALSE; ) {
      ZPutLong (Txhdr, 0L);
      Txhdr[ZF0] = CANFC32|CANFDX|CANOVIO;
      ZSendHexHeader (TryZHdrType, Txhdr);

again:
      switch (ZGetHeader (Rxhdr)) {
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
//            cmdzack1flg = Rxhdr[ZF0];
            if (ZReceiveData (RxBuffer, KSIZE) == GOTCRCW) {
               ZPutLong (Txhdr, 0L);
               do {
                  ZSendHexHeader (ZCOMPL, Txhdr);
               } while (++errors < 10 && ZGetHeader (Rxhdr) != ZFIN);
               ZAckBiBi();
               return (ZCOMPL);
            }
            else
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

         default:
            break;
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
      for (i = 0; i < 10 && AbortSession () == FALSE; i++) {
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
   CHAR *p, *q, *fileinfo, *name;
   long length, filesize, filetime;
   struct utimbuf utimes;
   struct stat f;

   for (p = RxBuffer, q = RxBuffer; *p; p++) {
      if (*p == '/' || *p == '\\' || *p == ':')
         q = p + 1;
   }

   name = q;
   sprintf (Pathname, "%s%s", pszPath, q);
   strlwr (Pathname);

   filesize = filetime = 0L;
   fileinfo = RxBuffer + strlen (RxBuffer) + 1;
   if (*fileinfo)
      sscanf (fileinfo, "%ld %lo", &filesize, &filetime);

   Log->Write (" Receiving %s; %ldb, %d min.", q, filesize, (int)((filesize * 10 / Speed + 53) / 54));

   Rxbytes = 0L;
   if ((fout = _fsopen (Pathname, "rb", SH_DENYNO)) != NULL) {
      // If the file already exists, it's date and size are verified in order to
      // prevent other end to send us a file that we have already received.
      fstat (fileno (fout), &f);
      fclose (fout);
      if (filesize == f.st_size && filetime == f.st_mtime) {
         Log->Write ("+Already have %s", Pathname);
         ZSendHexHeader (ZSKIP, Txhdr);
         return (ZEOF);
      }
      else if (filesize > f.st_size) {
         Rxbytes = f.st_size;
         Log->Write ("+Synchronizing to offset %lu", Rxbytes);
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
               if ((fout = _fsopen (Pathname, "rb", SH_DENYNO)) != NULL)
                  fclose (fout);
            } while (fout != NULL);
            Log->Write ("+Renaming dupe file %s", Pathname);
         }
      }
   }

   if (Progress != NULL) {
      Progress->Type = FILE_RECEIVING;
      strcpy (Progress->RxFileName, name);
      Progress->RxBlockSize = 0;
      Progress->RxSize = filesize;
      Progress->Begin ();
      Progress->RxPosition = Rxbytes;
      Progress->Update ();
   }

   n = 10;
   length = time (NULL);

   if ((fout = _fsopen (Pathname, "ab", SH_DENYNO)) == NULL) {
      if (Progress != NULL)
         Progress->End ();
      return (ZERROR);
   }

   while (AbortSession () == FALSE) {
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
               if (Progress != NULL)
                  Progress->End ();
               return (ZERROR);
            }
            break;

         case ZFILE:
            ZReceiveData (RxBuffer, KSIZE);
            continue;

         case ZEOF:
            if (ZGetLong (Rxhdr) != Rxbytes)
               continue;
            if (Progress != NULL)
               Progress->End ();
            if (fout != NULL) {
               fclose (fout);
               fout = NULL;
               if ((length = time (NULL) - length) == 0L)
                  length++;
               Log->Write ("+CPS: %lu (%lu bytes)  Efficiency: %d%%", Rxbytes / length, Rxbytes, ((Rxbytes / length) * 100L) / (Speed / 10));
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
                  if (filetime) {
                     utimes.actime = filetime;
                     utimes.modtime = filetime;
                     utime (Pathname, &utimes);
                  }
               }
               if (Progress != NULL)
                  Progress->End ();
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
                     if (filetime) {
                        utimes.actime = filetime;
                        utimes.modtime = filetime;
                        utime (Pathname, &utimes);
                     }
                  }
                  if (Progress != NULL)
                     Progress->End ();
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
                     if (filetime) {
                        utimes.actime = filetime;
                        utimes.modtime = filetime;
                        utime (Pathname, &utimes);
                     }
                  }
                  if (Progress != NULL)
                     Progress->End ();
                  return (ZERROR);

               case ZERROR:     /* CRC error */
                  if (--n < 0) {
                     if (fout != NULL) {
                        fclose (fout);
                        fout = NULL;
                        if (filetime) {
                           utimes.actime = filetime;
                           utimes.modtime = filetime;
                           utime (Pathname, &utimes);
                        }
                     }
                     if (Progress != NULL)
                        Progress->End ();
                     return (ZERROR);
                  }
                  Com->SendBytes ((unsigned char *)Attn, (USHORT)strlen (Attn));
                  continue;

               case TIMEOUT:
                  if (--n < 0) {
                     if (fout != NULL) {
                        fclose (fout);
                        fout = NULL;
                        if (filetime) {
                           utimes.actime = filetime;
                           utimes.modtime = filetime;
                           utime (Pathname, &utimes);
                        }
                     }
                     if (Progress != NULL)
                        Progress->End ();
                     return (ZERROR);
                  }
                  continue;

               case GOTCRCW:
                  n = 10;
                  fwrite (RxBuffer, Rxcount, 1, fout);
                  Rxbytes += Rxcount;
                  ZPutLong (Txhdr, Rxbytes);
                  ZSendBinaryHeader (ZACK, Txhdr);
                  if (Progress != NULL) {
                     Progress->RxPosition = Rxbytes;
                     Progress->RxBlockSize = Rxcount;
                     Progress->Update ();
                  }
                  goto nxthdr;

               case GOTCRCQ:
                  n = 10;
                  fwrite (RxBuffer, Rxcount, 1, fout);
                  Rxbytes += Rxcount;
                  ZPutLong (Txhdr, Rxbytes);
                  ZSendBinaryHeader (ZACK, Txhdr);
                  if (Progress != NULL) {
                     Progress->RxPosition = Rxbytes;
                     Progress->RxBlockSize = Rxcount;
                     Progress->Update ();
                  }
                  goto moredata;

               case GOTCRCG:
                  n = 10;
                  fwrite (RxBuffer, Rxcount, 1, fout);
                  Rxbytes += Rxcount;
                  if (Progress != NULL) {
                     Progress->RxPosition = Rxbytes;
                     Progress->RxBlockSize = Rxcount;
                     Progress->Update ();
                  }
                  goto moredata;

               case GOTCRCE:
                  n = 10;
                  fwrite (RxBuffer, Rxcount, 1, fout);
                  Rxbytes += Rxcount;
                  if (Progress != NULL) {
                     Progress->RxPosition = Rxbytes;
                     Progress->RxBlockSize = Rxcount;
                     Progress->Update ();
                  }
                  goto nxthdr;
            }
            break;
      }
   }

   if (Progress != NULL)
      Progress->End ();

   if (fout != NULL) {
      fclose (fout);
      fout = NULL;
      if (filetime) {
         utimes.actime = filetime;
         utimes.modtime = filetime;
         utime (Pathname, &utimes);
      }
   }

   return (ZERROR);
}

short TZModem::ZSendFile (char *file, char *name)
{
   int fd;
   short c, filedone, len, goodneeded, goodblks;
   char buf[64], *p, *q;
   unsigned long length;
   struct stat f;

   AdjustPath (file);
   if ((fd = sopen (file, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) == -1)
      return (ZERROR);

   if (name == NULL) {
      for (p = file, q = buf; *p; p++) {
         if ((*q++ = *p) == '/' || *p == '\\' || *p == ':')
            q = buf;
      }
   }
   else {
      strcpy (buf, name);
      q = strchr (buf, '\0');
   }
   *q++ = '\0';

   fstat (fd, &f);
   sprintf (q, "%lu %lo %o", f.st_size, f.st_mtime, f.st_mode);
   length = time (NULL);

   Txpos = Rxpos = 0L;
   Log->Write (" Sending %s; %ldb, %d min.", file, f.st_size, (int)((f.st_size * 10 / Speed + 53) / 54));

   if (Progress != NULL) {
      Progress->Type = FILE_SENDING;
      if (name != NULL)
         strcpy (Progress->TxFileName, name);
      else
         strcpy (Progress->TxFileName, file);
      Progress->TxBlockSize = 0;
      Progress->TxSize = f.st_size;
      Progress->Begin ();
      Progress->Update ();
   }

   while (TimedRead (50) != -1 && AbortSession () == FALSE)
      ;

   do {
      Txhdr[ZF0] = LZCONV;    /* file conversion request */
      Txhdr[ZF1] = LZMANAG;   /* file management request */
      Txhdr[ZF2] = LZTRANS;   /* file transport request */
      Txhdr[ZF3] = 0;
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
            close (fd);
            if (Progress != NULL)
               Progress->End ();
            Log->Write ("!Aborted by remote");
            return (ZERROR);

         case ZSKIP:
            close (fd);
            if (Progress != NULL)
               Progress->End ();
            Log->Write ("+Remote refused %s", buf);
            return (c);

         case ZRPOS:
            break;

         case ZERROR:
         default:
            continue;
      }
   } while (c != ZRPOS && AbortSession () == FALSE);

   lseek (fd, Rxpos, SEEK_SET);
   Txpos = Rxpos;
   filedone = FALSE;
   goodblks = 0;
   goodneeded = 4;
//   if (Rxbuflen == 0)
   Rxbuflen = 1024;

   if (Progress != NULL) {
      Progress->TxPosition = Txpos;
      Progress->TxBlockSize = 0;
      Progress->Update ();
   }

   if (Txpos < f.st_size) {
      ZPutLong (Txhdr, Txpos);
      ZSendBinaryHeader (ZDATA, Txhdr);
   }

   while (AbortSession () == FALSE) {
      if (filedone == FALSE) {
         if ((len = (short)read (fd, TxBuffer, Rxbuflen)) > 0) {
            if (len < Rxbuflen)
               ZSendData (TxBuffer, len, ZCRCE);
            else
               ZSendData (TxBuffer, len, ZCRCG);
            Txpos += len;
            if (Progress != NULL) {
               Progress->TxPosition += len;
               Progress->TxBlockSize = len;
               Progress->Update ();
            }
         }
         else {
            filedone = TRUE;
            ZPutLong (Txhdr, Txpos);
            ZSendBinaryHeader (ZEOF, Txhdr);
         }
      }

      if (Rxbuflen < Maxblklen && ++goodblks > goodneeded) {
         Rxbuflen = (SHORT)(((Rxbuflen << 1) < Maxblklen) ? Rxbuflen << 1 : Maxblklen);
         goodblks = 0;
      }

      while (Com->BytesReady () == TRUE && AbortSession () == FALSE) {
         c = Com->ReadByte ();
         if (c == CAN || c == ZPAD) {
            switch (c = ZGetHeader (Rxhdr)) {
               case ZACK:
                  continue;

               case ZRINIT:
                  close (fd);
                  if (Progress != NULL)
                     Progress->End ();
                  if ((length = time (NULL) - length) == 0L)
                     length++;
                  Log->Write ("+CPS: %lu (%lu bytes)  Efficiency: %d%%", f.st_size / length, f.st_size, ((f.st_size / length) * 100L) / (Speed / 10));
                  Log->Write ("+Sent-Z%s %s", (Txfcs32 == TRUE) ? "/32" : "", strupr (file));
                  return (OK);

               case ZRPOS:
                  Txpos = Rxpos;
                  lseek (fd, Txpos, SEEK_SET);
                  filedone = FALSE;
                  Com->ClearOutbound ();
                  if (Txpos < f.st_size) {
                     ZPutLong (Txhdr, Txpos);
                     ZSendBinaryHeader (ZDATA, Txhdr);
                  }
                  if (Rxpos > 0L) {
                     Rxbuflen = (SHORT)(((Rxbuflen >> 2) > 64) ? Rxbuflen >> 2 : 64);
                     goodblks = 0;
                     goodneeded = (SHORT)(((goodneeded << 1) > 16) ? 16 : goodneeded << 1);
                  }
                  if (Progress != NULL) {
                     Progress->TxPosition = Rxpos;
                     Progress->TxBlockSize = Rxbuflen;
                     Progress->Update ();
                  }
                  break;

               case ZSKIP:
                  close (fd);
                  if (Progress != NULL)
                     Progress->End ();
                  Log->Write ("+Remote refused %s", buf);
                  return (c);
            }
         }
      }
   }

   if (Progress != NULL)
      Progress->End ();

   close (fd);
   return (ZERROR);
}

void TZModem::ZEndSender (void)
{
   ULONG TimeOut;

   FileSent = 0;

//   ZPutLong (Txhdr, 0L);
//   ZSendBinaryHeader (ZFIN, Txhdr);

   TimeOut = TimerSet (200L);
   while (AbortSession () == FALSE && !TimeUp (TimeOut)) {
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

//   Pause (2);
   Com->ClearInbound ();
}

