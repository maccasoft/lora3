
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.18
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"

TScreen::TScreen (void)
{
#if defined(__NT__)
   EndRun = FALSE;
   RxBytes = 0;
#else
   wh = -1;
   Ansi = FALSE;
   Prec = 0;
   Attr = BLACK|_LGREY;
#endif
}

TScreen::~TScreen (void)
{
#if defined(__DOS__) || defined(__OS2__)
   if (wh != -1) {
      wactiv (wh);
      wclose ();
   }
   hidecur ();
#endif
}

SHORT TScreen::BytesReady (VOID)
{
   SHORT RetVal = FALSE;

   if (kbhit ())
      RetVal = TRUE;

   return (RetVal);
}

VOID TScreen::BufferByte (UCHAR byte)
{
#if defined(__NT__)
   TxBuffer[TxBytes++] = byte;
   if (TxBytes >= TSIZE)
      UnbufferBytes ();
#else
   USHORT i;

   if (byte == '[' && Prec == ESC) {
      Ansi = TRUE;
      Count = 0;
      Params[Count] = 0;
   }
   else {
      if (Ansi == TRUE) {
         if (isalpha (byte)) {
            if (byte == 'm') {
               for (i = 0; i <= Count; i++) {
                  if (Params[i] >= 30 && Params[i] <= 39)
                     Attr &= 0xF8;
                  else if (Params[i] >= 40 && Params[i] <= 49)
                     Attr &= 0x8F;

                  switch (Params[i]) {
                     case 0:
                        Attr = 0;
                        break;
                     case 1:
                        Attr |= 0x08;
                        break;
                     case 5:
                        Attr |= BLINK;
                        break;
                     case 30:
                        Attr |= BLACK;
                        break;
                     case 34:
                        Attr |= BLUE;
                        break;
                     case 32:
                        Attr |= GREEN;
                        break;
                     case 36:
                        Attr |= CYAN;
                        break;
                     case 31:
                        Attr |= RED;
                        break;
                     case 35:
                        Attr |= MAGENTA;
                        break;
                     case 39:
                        Attr |= BROWN;
                        break;
                     case 37:
                        Attr |= LGREY;
                        break;
                     case 33:
                        Attr |= YELLOW;
                        break;
                     case 40:
                        Attr |= _BLACK;
                        break;
                     case 44:
                        Attr |= _BLUE;
                        break;
                     case 42:
                        Attr |= _GREEN;
                        break;
                     case 46:
                        Attr |= _CYAN;
                        break;
                     case 41:
                        Attr |= _RED;
                        break;
                     case 45:
                        Attr |= _MAGENTA;
                        break;
                     case 49:
                        Attr |= _BROWN;
                        break;
                     case 47:
                        Attr |= _LGREY;
                        break;
                  }
               }
               wtextattr (Attr);
            }
            else if (byte == 'f')
               wgotoxy ((SHORT)(Params[0] - 1), (SHORT)(Params[1] - 1));
            else if (byte == 'J' && Params[0] == 2)
               wclear ();
            else if (byte == 'K')
               wclreol ();
            Ansi = FALSE;
         }
         else if (byte == ';') {
            Count++;
            Params[Count] = 0;
         }
         else if (isdigit (byte)) {
            Params[Count] *= 10;
            Params[Count] += (USHORT)(byte - '0');
         }
      }
      else if (byte == CTRLL)
         wclear ();
      else if (byte != ESC)
         wputc (byte);
   }

   Prec = byte;
#endif
}

VOID TScreen::BufferBytes (UCHAR *bytes, USHORT len)
{
#if defined(__NT__)
   while (len > 0 && EndRun == FALSE) {
      TxBuffer[TxBytes++] = *bytes++;
      if (TxBytes >= TSIZE)
         UnbufferBytes ();
      len--;
   }
#else
   while (len-- > 0)
      BufferByte (*bytes++);
#endif
}

SHORT TScreen::Carrier (VOID)
{
   return (TRUE);
}

VOID TScreen::ClearOutbound (VOID)
{
#if defined(__NT__)
   TxBytes = 0;
#endif
}

VOID TScreen::ClearInbound (VOID)
{
#if defined(__NT__)
   RxBytes = 0;
#endif
}

SHORT TScreen::Initialize (VOID)
{
#if defined(__NT__)
   return (TRUE);
#else
   SHORT RetVal = FALSE;

   if ((wh = wopen (0, 0, (short)(24 - 1), 79, 5, LGREY|_BLACK, LGREY|_BLACK)) != -1) {
      showcur ();
      RetVal = TRUE;
   }

   return (RetVal);
#endif
}

UCHAR TScreen::ReadByte (VOID)
{
   return ((UCHAR)getch ());
}

USHORT TScreen::ReadBytes (UCHAR *bytes, USHORT len)
{
   bytes = bytes;
   len = len;

   return (0);
}

VOID TScreen::SendByte (UCHAR byte)
{
#if defined(__NT__)
   fwrite (&byte, 1, 1, stdout);
   fflush (stdout);
#else
   BufferByte (byte);
#endif
}

VOID TScreen::SendBytes (UCHAR *bytes, USHORT len)
{
#if defined(__NT__)
   fwrite (bytes, len, 1, stdout);
   fflush (stdout);
#else
   while (len-- > 0)
      BufferByte (*bytes++);
#endif
}

VOID TScreen::UnbufferBytes (VOID)
{
#if defined(__NT__)
   fwrite (TxBuffer, TxBytes, 1, stdout);
   fflush (stdout);
   TxBytes = 0;
#endif
}


