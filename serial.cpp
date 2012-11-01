
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"

#if defined(__OS2__)
#define ASYNC_EXTSETBAUDRATE              0x0043

USHORT DevIOCtl (PVOID pData, USHORT cbData, PVOID pParms, USHORT cbParms, USHORT usFunction, USHORT usCategory, HFILE hDevice)
{
   ULONG ulParmLengthInOut = cbParms, ulDataLengthInOut = cbData;
   return ((USHORT)DosDevIOCtl (hDevice, usCategory, usFunction, pParms, cbParms, &ulParmLengthInOut, pData, cbData, &ulDataLengthInOut));
}
#endif

TSerial::TSerial (void)
{
#if defined(__OS2__)
   hFile = NULLHANDLE;
#elif defined(__NT__)
   hFile = INVALID_HANDLE_VALUE;
#elif defined(__LINUX__)
   hFile = -1;
#else
   hPort = NULL;
#endif

#if defined(__OS2__) || defined(__NT__)
   strcpy (Device, "COM2");
#elif defined(__LINUX__)
   strcpy (Device, "/dev/modem");
#else
   Com = 2;
#endif
   Speed = 19200L;
   DataBits = 8;
   StopBits = 1;
   Parity = 'N';

   EndRun = FALSE;
   TxBytes = RxBytes = 0;
}

TSerial::~TSerial (void)
{
#if defined(__OS2__)
   if (hFile != NULLHANDLE)
      DosClose (hFile);
#elif defined(__NT__)
   if (hFile != INVALID_HANDLE_VALUE)
      CloseHandle (hFile);
#elif defined(__LINUX__)
   if (hFile >= 0) {
      ioctl (hFile, TCSETSW, &old_termio);
      close (hFile);
   }
#else
   if (hPort != NULL)
      PortClose (hPort);
#endif
}

VOID TSerial::BufferByte (UCHAR byte)
{
   TxBuffer[TxBytes++] = byte;
   if (TxBytes >= TSIZE)
      UnbufferBytes ();
}

VOID TSerial::BufferBytes (UCHAR *bytes, USHORT len)
{
   USHORT ToCopy;

   if (len > 0 && EndRun == FALSE) {
      do {
         if (TxBytes < TSIZE) {
            ToCopy = len;
            if (ToCopy > TSIZE - TxBytes)
               ToCopy = (USHORT)(TSIZE - TxBytes);
            memcpy (&TxBuffer[TxBytes], bytes, ToCopy);
            bytes += ToCopy;
            TxBytes += ToCopy;
            len -= ToCopy;
         }
         if (TxBytes >= TSIZE)
            UnbufferBytes ();
      } while (len > 0 && EndRun == FALSE);
   }
}

USHORT TSerial::BytesReady (VOID)
{
   USHORT RetVal = FALSE;

#if defined(__OS2__)
   UINT data = 0;

   if (hFile != NULLHANDLE) {
      if (RxBytes > 0)
         RetVal = TRUE;
      else {
         DevIOCtl ((VOID *)&data, sizeof (UINT), NULL, 0, ASYNC_GETINQUECOUNT, IOCTL_ASYNC, hFile);
         if ((data & 0xFFFF) > 0)
            RetVal = TRUE;
      }
   }

   if (RetVal == FALSE)
      DosSleep (1L);
#elif defined(__NT__)
   ULONG Available;

   if (hFile != INVALID_HANDLE_VALUE) {
      if (RxBytes > 0)
         RetVal = TRUE;
      else {
         ReadFile (hFile, (PVOID)RxBuffer, sizeof (RxBuffer), &Available, NULL);
         if (Available > 0) {
            RxBytes = (USHORT)Available;
            NextByte = RxBuffer;
            RetVal = TRUE;
         }
      }
   }

   if (RetVal == FALSE)
      Sleep (1L);
#elif defined(__LINUX__)
   int i;

   if (hFile >= 0) {
      if (RxBytes > 0)
         RetVal = TRUE;
      else {
         if ((i = read (hFile, RxBuffer, sizeof (RxBuffer))) > 0) {
            RxBytes = (USHORT)i;
            NextByte = RxBuffer;
            RetVal = TRUE;
         }
      }
   }
#else
   if (hPort != NULL) {
      if (RxBytes > 0)
         RetVal = TRUE;
      else {
         if (PeekChar (hPort) >= 0)
            RetVal = TRUE;
      }
   }
#endif

   return (RetVal);
}

USHORT TSerial::Carrier (VOID)
{
#if defined(__OS2__)
   UCHAR data = 0;

   if (hFile != NULLHANDLE && EndRun == FALSE)
      DevIOCtl ((VOID *)&data, sizeof (UCHAR), NULL, 0, ASYNC_GETMODEMINPUT, IOCTL_ASYNC, hFile);

   return ((USHORT)((data & DCD) ? TRUE : FALSE));
#elif defined(__NT__)
   DWORD Status = 0L;

   if (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE)
      GetCommModemStatus (hFile, &Status);

   return ((Status & MS_RLSD_ON) ? TRUE : FALSE);
#elif defined(__LINUX__)
   int mcs;

   ioctl (hFile, TIOCMGET, &mcs);
   return ((mcs & TIOCM_CAR) ? TRUE : FALSE);
#else
   int data = 0;

   if (hPort != NULL)
      data = GetModemStatus (hPort);

   return ((data & CD_SET) ? TRUE : FALSE);
#endif
}

VOID TSerial::ClearInbound (VOID)
{
#if defined(__OS2__)
   UINT data;
   CHAR parm = 0;

   if (hFile != NULLHANDLE) {
      RxBytes = 0;
      DevIOCtl (&data, sizeof (data), &parm, sizeof (parm), DEV_FLUSHINPUT, IOCTL_GENERAL, hFile);
   }
#elif defined(__NT__)
   if (hFile != INVALID_HANDLE_VALUE) {
      RxBytes = 0;
      PurgeComm (hFile, PURGE_RXCLEAR);
   }
#elif defined(__LINUX__)
   RxBytes = 0;
#else
   if (hPort != NULL) {
      RxBytes = 0;
      ClearRXBuffer (hPort);
   }
#endif
}

VOID TSerial::ClearOutbound (VOID)
{
#if defined(__OS2__)
   UINT data;
   CHAR parm = 0;

   if (hFile != NULLHANDLE) {
      TxBytes = 0;
      DevIOCtl (&data, sizeof (data), &parm, sizeof (parm), DEV_FLUSHOUTPUT, IOCTL_GENERAL, hFile);
   }
#elif defined(__NT__)
   if (hFile != INVALID_HANDLE_VALUE) {
      TxBytes = 0;
      PurgeComm (hFile, PURGE_TXCLEAR);
   }
#elif defined(__LINUX__)
   TxBytes = 0;
#else
   if (hPort != NULL) {
      TxBytes = 0;
      ClearTXBuffer (hPort);
   }
#endif
}

USHORT TSerial::Initialize (VOID)
{
   USHORT RetVal = FALSE;

#if defined(__OS2__)
   UINT data;
   ULONG action;
   MODEMSTATUS ms;
   DCBINFO dcbCom;

   hFile = NULLHANDLE;

   if (DosOpen (Device, &hFile, &action, 0L, FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE, 0L) == 0) {
      DevIOCtl ((VOID *)&dcbCom, sizeof (DCBINFO), NULL, 0, ASYNC_GETDCBINFO, IOCTL_ASYNC, hFile);
      dcbCom.fbCtlHndShake &= ~(MODE_RTS_HANDSHAKE);
      dcbCom.fbCtlHndShake |= MODE_DTR_CONTROL|MODE_CTS_HANDSHAKE;
      dcbCom.fbFlowReplace &= ~(MODE_AUTO_TRANSMIT|MODE_AUTO_RECEIVE|MODE_ERROR_CHAR|MODE_NULL_STRIPPING|MODE_BREAK_CHAR);
      dcbCom.fbTimeout |= MODE_NOWAIT_READ_TIMEOUT;
      dcbCom.fbTimeout &= ~(MODE_NO_WRITE_TIMEOUT);
      dcbCom.usReadTimeout = 1;
      dcbCom.usWriteTimeout = 1;
      DevIOCtl (NULL, 0, (VOID *)&dcbCom, sizeof (DCBINFO), ASYNC_SETDCBINFO, IOCTL_ASYNC, hFile);

      ms.fbModemOn = RTS_ON|DTR_ON;
      ms.fbModemOff = 255;
      DevIOCtl ((VOID *)&data, sizeof (data), (VOID *)&ms, sizeof (ms), ASYNC_SETMODEMCTRL, IOCTL_ASYNC, hFile);

      SetParameters (Speed, DataBits, Parity, StopBits);
      RetVal = TRUE;
   }
#elif defined(__NT__)
   COMMTIMEOUTS cto;

   if ((hFile = CreateFile (Device, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE) {
      GetCommTimeouts (hFile, &cto);
      cto.ReadIntervalTimeout = MAXDWORD;
      cto.ReadTotalTimeoutMultiplier = 0;
      cto.ReadTotalTimeoutConstant = 0;
      SetCommTimeouts (hFile, &cto);

      SetParameters (Speed, DataBits, Parity, StopBits);
      RetVal = TRUE;
   }
#elif defined(__LINUX__)
   FILE *fpd;

   if ((hFile = open (Device, O_RDWR|O_BINARY)) >= 0) {
      fcntl (hFile, F_SETFL, O_NONBLOCK);

      ioctl (hFile, TCGETS, &old_termio);
//      new_termio = old_termio;
      memcpy (&new_termio, &old_termio, sizeof (new_termio));
fpd=fopen("serial.dbg", "wt");
fprintf (fpd, "Before\n");
fprintf (fpd, "  c_iflag=%04o\n", new_termio.c_iflag);
fprintf (fpd, "  c_oflag=%04o\n", new_termio.c_oflag);
fprintf (fpd, "  c_cflag=%04o\n", new_termio.c_cflag);
fprintf (fpd, "  c_lflag=%04o\n", new_termio.c_lflag);
      new_termio.c_iflag = 0;
      new_termio.c_oflag = 0;
      new_termio.c_lflag = 0;
      new_termio.c_cflag = CRTSCTS;
      ioctl (hFile, TCSETSW, &new_termio);
fprintf (fpd, "After\n");
fprintf (fpd, "  c_iflag=%04o\n", new_termio.c_iflag);
fprintf (fpd, "  c_oflag=%04o\n", new_termio.c_oflag);
fprintf (fpd, "  c_cflag=%04o\n", new_termio.c_cflag);
fprintf (fpd, "  c_lflag=%04o\n", new_termio.c_lflag);
fclose (fpd);

      SetParameters (Speed, DataBits, Parity, StopBits);
      RetVal = TRUE;
   }

//   ioctl (tty_fd, KDSKBMODE, K_RAW);
//   ioctl (tty_fd, VT_GETMODE, &vtm);
//   vtm.mode = VT_PROCESS;
//   vtm.relsig = SIGUSR1;
//   vtm.acqsig = SIGUSR2;
//   ioctl (tty_fd, VT_SETMODE, &vtm);
#else
   breakchk (0);

   if ((hPort = PortOpenGreenleafFast ((USHORT)(Com - 1), Speed, Parity, DataBits, StopBits)) != NULL) {
      if (hPort->status != ASSUCCESS) {
         PortClose (hPort);
         hPort = NULL;
         RetVal = FALSE;
      }
   }
#endif

   return (RetVal);
}

UCHAR TSerial::ReadByte (VOID)
{
   UCHAR c = 0;

#if defined(__OS2__)
   ULONG readed;

   if (hFile != NULLHANDLE) {
      if (RxBytes == 0) {
         do {
            DosRead (hFile, (PVOID)RxBuffer, sizeof (RxBuffer), &readed);
         } while (readed == 0L && EndRun == FALSE);
         RxBytes = (USHORT)readed;
         NextByte = RxBuffer;
      }
   }
#elif defined(__NT__)
   ULONG readed;

   if (hFile != INVALID_HANDLE_VALUE) {
      if (RxBytes == 0) {
         do {
            ReadFile (hFile, (PVOID)RxBuffer, sizeof (RxBuffer), &readed, NULL);
         } while (readed == 0L && EndRun == FALSE);
         RxBytes = (USHORT)readed;
         NextByte = RxBuffer;
      }
   }
#elif defined(__LINUX__)
   int i;

   if (hFile >= 0) {
      if (RxBytes == 0) {
         while ((i = read (hFile, RxBuffer, sizeof (RxBuffer))) == 0)
            ;
         RxBytes = (USHORT)i;
         NextByte = RxBuffer;
      }
   }
#else
   if (hPort != NULL) {
      if (RxBytes == 0) {
         do {
            ReadBuffer (hPort, (char *)RxBuffer, sizeof (RxBuffer));
         } while (hPort->count <= 0 && EndRun == FALSE);
         RxBytes = (USHORT)hPort->count;
         NextByte = RxBuffer;
      }
   }
#endif

   if (RxBytes > 0) {
      c = *NextByte++;
      RxBytes--;
   }

   return (c);
}

USHORT TSerial::ReadBytes (UCHAR *bytes, USHORT len)
{
   USHORT Max;

#if defined(__OS2__)
   ULONG readed;

   if (hFile != NULLHANDLE) {
      if (RxBytes == 0) {
         do {
            DosRead (hFile, (PVOID)RxBuffer, sizeof (RxBuffer), &readed);
         } while (readed == 0L && EndRun == FALSE);
         RxBytes = (USHORT)readed;
         NextByte = RxBuffer;
      }
   }
#elif defined(__NT__)
   ULONG readed;

   if (hFile != INVALID_HANDLE_VALUE) {
      if (RxBytes == 0) {
         do {
            ReadFile (hFile, (PVOID)RxBuffer, sizeof (RxBuffer), &readed, NULL);
         } while (readed == 0L && EndRun == FALSE);
         RxBytes = (USHORT)readed;
         NextByte = RxBuffer;
      }
   }
#elif defined(__LINUX__)
   int i;

   if (hFile >= 0) {
      if (RxBytes == 0) {
         while ((i = read (hFile, RxBuffer, sizeof (RxBuffer))) == 0)
            ;
         RxBytes = (USHORT)i;
         NextByte = RxBuffer;
      }
   }
#else
   if (hPort != NULL) {
      if (RxBytes == 0) {
         do {
            ReadBuffer (hPort, (char *)RxBuffer, sizeof (RxBuffer));
         } while (hPort->count <= 0 && EndRun == FALSE);
         RxBytes = (USHORT)hPort->count;
         NextByte = RxBuffer;
      }
   }
#endif

   if (RxBytes > 0) {
      if ((Max = len) > RxBytes)
         Max = RxBytes;
      memcpy (bytes, NextByte, Max);
      RxBytes -= Max;
      NextByte += Max;
   }

   return (Max);
}

VOID TSerial::SetDTR (USHORT fStatus)
{
#if defined(__OS2__)
   MODEMSTATUS ms;
   UINT data;

   if (hFile != NULLHANDLE) {
      if (fStatus == TRUE) {
         ms.fbModemOn = 0x01;
         ms.fbModemOff = 0xFF;
      }
      else {
         ms.fbModemOn = 0x00;
         ms.fbModemOff = 0xFE;
      }
      DevIOCtl ((VOID *)&data, sizeof (data), (VOID *)&ms, sizeof (ms), ASYNC_SETMODEMCTRL, IOCTL_ASYNC, hFile);
   }
#elif defined(__NT__)
   if (hFile != INVALID_HANDLE_VALUE)
      EscapeCommFunction (hFile, (fStatus == TRUE) ? SETDTR : CLRDTR);
#elif defined(__LINUX__)
   int mcs;

   ioctl (hFile, TIOCMGET, &mcs);
   if (fStatus == FALSE)
      mcs &= ~TIOCM_DTR;
   else
      mcs |= TIOCM_DTR;
   ioctl (hFile, TIOCMSET, &mcs);
#else
   if (hPort != NULL)
      SetDtr (hPort, (fStatus == TRUE) ? ON : OFF);
#endif
}

VOID TSerial::SetRTS (USHORT fStatus)
{
#if defined(__OS2__)
   MODEMSTATUS ms;
   UINT data;

   if (hFile != NULLHANDLE) {
      if (fStatus == TRUE) {
         ms.fbModemOn = 0x02;
         ms.fbModemOff = 0xFF;
      }
      else {
         ms.fbModemOn = 0x00;
         ms.fbModemOff = 0xFD;
      }
      DevIOCtl ((VOID *)&data, sizeof (data), (VOID *)&ms, sizeof (ms), ASYNC_SETMODEMCTRL, IOCTL_ASYNC, hFile);
   }
#elif defined(__NT__)
   if (hFile != INVALID_HANDLE_VALUE)
      EscapeCommFunction (hFile, (fStatus == TRUE) ? SETRTS : CLRRTS);
#elif defined(__LINUX__)
   int mcs;

   ioctl (hFile, TIOCMGET, &mcs);
   if (fStatus == FALSE)
      mcs &= ~TIOCM_RTS;
   else
      mcs |= TIOCM_RTS;
   ioctl (hFile, TIOCMSET, &mcs);
#else
   if (hPort != NULL)
      SetRts (hPort, (fStatus == TRUE) ? ON : OFF);
#endif
}

VOID TSerial::SetParameters (ULONG ulSpeed, USHORT nData, UCHAR nParity, USHORT nStop)
{
#if defined(__OS2__)
   ULONG Param[2];
   LINECONTROL lc;

   if (hFile != NULLHANDLE) {
      Param[0] = ulSpeed;
      Param[1] = 0L;
      DevIOCtl (NULL, 0, (VOID *)&Param[0], sizeof (ULONG) + 1, ASYNC_EXTSETBAUDRATE, IOCTL_ASYNC, hFile);

      lc.bDataBits = (BYTE)nData;
      lc.bStopBits = (BYTE)nStop;
      lc.bParity = (BYTE)nParity;

      DevIOCtl (NULL, 0, (VOID *)&lc, sizeof (LINECONTROL), ASYNC_SETLINECTRL, IOCTL_ASYNC, hFile);
   }
#elif defined(__NT__)
   DCB dcb;

   if (hFile != INVALID_HANDLE_VALUE) {
      dcb.DCBlength = sizeof (DCB);
      GetCommState (hFile, &dcb);
      dcb.BaudRate = ulSpeed;
      dcb.ByteSize = (BYTE)nData;
      if (nParity == 'N')
         dcb.Parity = NOPARITY;
      else if (nParity == 'E')
         dcb.Parity = EVENPARITY;
      else if (nParity == 'O')
         dcb.Parity = ODDPARITY;
      else if (nParity == 'M')
         dcb.Parity = MARKPARITY;
      if (nStop == 1)
         dcb.StopBits = ONESTOPBIT;
      dcb.fBinary = TRUE;
      dcb.fOutxCtsFlow = TRUE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fDtrControl = DTR_CONTROL_ENABLE;
      dcb.fRtsControl = RTS_CONTROL_ENABLE;
      SetCommState (hFile, &dcb);
   }
#elif defined(__LINUX__)
   speed_t speed;

   tcgetattr (hFile, &tty);

   switch (ulSpeed) {
      case 300:
         speed = B300;
         break;
      case 1200:
         speed = B1200;
         break;
      case 2400:
         speed = B2400;
         break;
      case 4800:
         speed = B4800;
         break;
      case 9600:
         speed = B9600;
         break;
      case 19200:
         speed = B19200;
         break;
      case 38400:
         speed = B38400;
         break;
      case 57600L:
         speed = B57600;
         break;
      case 115200L:
         speed = B115200;
         break;
   }

   cfsetospeed (&tty, speed);
   cfsetispeed (&tty, speed);

   tty.c_cflag &= ~(CSIZE);
   if (nData == 5)
      tty.c_cflag |= CS5;
   else if (nData == 6)
      tty.c_cflag |= CS6;
   else if (nData == 7)
      tty.c_cflag |= CS7;
   else
      tty.c_cflag |= CS8;

   tty.c_cflag |= CRTSCTS;
   tty.c_cflag |= IXON;

   tty.c_cflag &= ~(PARENB|PARODD);
   if (nParity == 'E')
      tty.c_cflag |= PARENB;
   else if (nParity == 'O')
      tty.c_cflag |= PARODD;

   tcsetattr (hFile, TCSANOW, &tty);
#else
   if (hPort != NULL)
      PortSet (hPort, ulSpeed, nParity, nData, nStop);
#endif
}

VOID TSerial::SendByte (UCHAR byte)
{
#if defined(__OS2__)
   ULONG written;

   if (hFile != NULLHANDLE) {
      do {
         DosWrite (hFile, (PVOID)&byte, 1L, &written);
      } while (written != 1L && EndRun == FALSE && Carrier () == TRUE);
   }
#elif defined(__NT__)
   ULONG written;

   if (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE)
      do {
         WriteFile (hFile, (LPCVOID)&byte, 1L, &written, NULL);
      } while (written != 1L && EndRun == FALSE && Carrier () == TRUE);
#elif defined(__LINUX__)
   while (write (hFile, &byte, 1) != 1)
      ;
#else
   int retval;

   if (hPort != NULL) {
      do {
         retval = WriteChar (hPort, byte);
      } while (retval != ASSUCCESS && hPort->status == ASBUFRFULL && Carrier () == TRUE);
   }
#endif
}

VOID TSerial::SendBytes (UCHAR *bytes, USHORT len)
{
#if defined(__OS2__)
   ULONG written;

   if (hFile != NULLHANDLE) {
      do {
         DosWrite (hFile, (PVOID)bytes, (long)len, &written);
         bytes += written;
         len -= (USHORT)written;
      } while (len > 0 && EndRun == FALSE && Carrier () == TRUE);
   }
#elif defined(__NT__)
   ULONG written;

   if (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE)
      do {
         WriteFile (hFile, (LPCVOID)bytes, (long)len, &written, NULL);
         bytes += written;
         len -= (USHORT)written;
      } while (len > 0 && EndRun == FALSE && Carrier () == TRUE);
#elif defined(__LINUX__)
   write (hFile, bytes, len);
#else
   int retval;

   if (hPort != NULL) {
      do {
         retval = WriteBuffer (hPort, (char *)bytes, len);
         bytes += hPort->count;
         len -= hPort->count;
      } while (retval != ASSUCCESS && hPort->status == ASBUFRFULL && Carrier () == TRUE);
   }
#endif
}

VOID TSerial::UnbufferBytes (VOID)
{
#if defined(__OS2__)
   ULONG Written;
   UCHAR *p;

   while (hFile != NULLHANDLE && TxBytes > 0 && EndRun == FALSE) {
      p = TxBuffer;
      do {
         Written = 0L;
         DosWrite (hFile, (PVOID)p, TxBytes, &Written);
         p += Written;
         TxBytes -= (USHORT)Written;
      } while (TxBytes > 0 && EndRun == FALSE && Carrier () == TRUE);
      if (TxBytes > 0 && Written != 0L)
         memcpy (TxBuffer, p, TxBytes);
   }
#elif defined(__NT__)
   ULONG written;
   UCHAR *p;

   while (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE && TxBytes > 0) {
      p = TxBuffer;
      do {
         WriteFile (hFile, (LPCVOID)p, (long)TxBytes, &written, NULL);
         p += written;
         TxBytes -= (USHORT)written;
      } while (TxBytes > 0 && EndRun == FALSE && Carrier () == TRUE);
   }
#elif defined(__LINUX__)
   int i;

   if (TxBytes > 0) {
      fcntl (hFile, F_SETFL, 0);
      do {
         i = write (hFile, TxBuffer, TxBytes);
         TxBytes -= (USHORT)i;
      } while (TxBytes > 0);
      fcntl (hFile, F_SETFL, O_NONBLOCK);
   }
#else
   char *p;

   while (hPort != NULL && TxBytes > 0) {
      p = (char *)TxBuffer;
      do {
         WriteBuffer (hPort, p, TxBytes);
         p += hPort->count;
         TxBytes -= hPort->count;
      } while (TxBytes > 0 && EndRun == FALSE && Carrier () == TRUE);
   }
#endif
}

VOID TSerial::SetName (PSZ name)
{
   name = name;
}

VOID TSerial::SetCity (PSZ name)
{
   name = name;
}

VOID TSerial::SetLevel (PSZ level)
{
   level = level;
}

VOID TSerial::SetTimeLeft (ULONG seconds)
{
   seconds = seconds;
}

VOID TSerial::SetTime (ULONG seconds)
{
   seconds = seconds;
}


