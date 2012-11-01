
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.9
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    01/08/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"

TStdio::TStdio (void)
{
   RxBytes = 0;
   RxPosition = 0;
#if defined(__LINUX__)
   tty_fd = -1;
#endif
}

TStdio::~TStdio (void)
{
#if defined(__LINUX__)
   struct vt_mode vtm;

   if (tty_fd != -1) {
      ioctl (tty_fd, VT_GETMODE, &vtm);
      vtm.mode = VT_AUTO;
      ioctl (tty_fd, VT_SETMODE, &vtm);
      ioctl (tty_fd, TCSETSW, &old_termio);
      ioctl (tty_fd, KDSKBMODE, K_XLATE);
      close (tty_fd);
      tty_fd = -1;
   }
#endif
}

USHORT TStdio::BytesReady (VOID)
{
#if defined(__LINUX__)
   int i;
#endif
   USHORT RetVal = TRUE;

#if defined(__LINUX__)
   if (RxBytes == 0) {
      RetVal = FALSE;
      if ((i = read (tty_fd, RxBuffer, sizeof (RxBuffer))) > 0) {
         RxBytes = (USHORT)i;
         RxPosition = 0;
         RetVal = TRUE;
      }
   }
#else
   if (!kbhit ())
      RetVal = FALSE;
#endif

#if defined(__OS2__)
   if (RetVal == FALSE)
      DosSleep (1L);
#elif defined(__NT__)
   if (RetVal == FALSE)
      Sleep (1L);
#endif

   return (RetVal);
}

VOID TStdio::BufferByte (UCHAR byte)
{
#if defined(__LINUX__)
   write (tty_fd, &byte, 1);
#else
   fwrite (&byte, 1, 1, stdout);
#endif
}

VOID TStdio::BufferBytes (UCHAR *bytes, USHORT len)
{
#if defined(__LINUX__)
   write (tty_fd, bytes, len);
#else
   fwrite (bytes, len, 1, stdout);
#endif
}

USHORT TStdio::Carrier (VOID)
{
   return (TRUE);
}

VOID TStdio::ClearOutbound (VOID)
{
}

VOID TStdio::ClearInbound (VOID)
{
#if defined(__LINUX__)
   RxBytes = 0;
#endif
}

USHORT TStdio::Initialize (VOID)
{
   USHORT RetVal = TRUE;
#if defined(__LINUX__)
   struct vt_mode vtm;
#endif

#if defined(__LINUX__)
   tty_fd = fileno (stdin);
//   tty_fd = open ("/dev/ttyS1", O_RDWR);
   fcntl (tty_fd, F_SETFL, O_NONBLOCK);

   ioctl (tty_fd, TCGETS, &old_termio);
   new_termio = old_termio;
   new_termio.c_iflag &= ~(ICRNL);
   new_termio.c_lflag &= ~(ISIG|ICANON|ECHO);
   ioctl (tty_fd, TCSETSW, &new_termio);

//   ioctl (tty_fd, KDSKBMODE, K_RAW);
//   ioctl (tty_fd, VT_GETMODE, &vtm);
//   vtm.mode = VT_PROCESS;
//   vtm.relsig = SIGUSR1;
//   vtm.acqsig = SIGUSR2;
//   ioctl (tty_fd, VT_SETMODE, &vtm);
#endif

   return (RetVal);
}

UCHAR TStdio::ReadByte (VOID)
{
   UCHAR c;

#if defined(__LINUX__)
   if (RxBytes > 0) {
      c = RxBuffer[RxPosition++];
      RxBytes--;
   }
#else
   c = (UCHAR)getch ();
#endif

   return (c);
}

USHORT TStdio::ReadBytes (UCHAR *bytes, USHORT len)
{
   USHORT Max = 0;

   while (len > 0 && BytesReady () == TRUE) {
      *bytes++ = ReadByte ();
      len--;
      Max++;
   }

   return (Max);
}

VOID TStdio::SendByte (UCHAR byte)
{
#if defined(__LINUX__)
   write (tty_fd, &byte, 1);
#else
   fwrite (&byte, 1, 1, stdout);
   fflush (stdout);
#endif
}

VOID TStdio::SendBytes (UCHAR *bytes, USHORT len)
{
#if defined(__LINUX__)
   write (tty_fd, bytes, len);
#else
   fwrite (bytes, len, 1, stdout);
   fflush (stdout);
#endif
}

VOID TStdio::UnbufferBytes (VOID)
{
#if !defined(__LINUX__)
   fflush (stdout);
#endif
}

VOID TStdio::SetName (PSZ name)
{
   name = name;
}

VOID TStdio::SetCity (PSZ name)
{
   name = name;
}

VOID TStdio::SetLevel (PSZ level)
{
   level = level;
}

VOID TStdio::SetTimeLeft (ULONG seconds)
{
   seconds = seconds;
}

VOID TStdio::SetTime (ULONG seconds)
{
   seconds = seconds;
}


