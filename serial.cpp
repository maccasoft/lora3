
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.19
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"

#if defined(__OS2__)
USHORT DevIOCtl (PVOID pData, USHORT cbData, PVOID pParms, USHORT cbParms, USHORT usFunction, USHORT usCategory, HFILE hDevice)
{
   ULONG ulParmLengthInOut = cbParms, ulDataLengthInOut = cbData;
   return ((USHORT)DosDevIOCtl (hDevice, usCategory, usFunction, pParms, cbParms, &ulParmLengthInOut, pData, cbData, &ulDataLengthInOut));
}
#endif

TSerial::TSerial (void)
{
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
         ToCopy = len;
         if (ToCopy > TSIZE - TxBytes)
            ToCopy = (USHORT)(TSIZE - TxBytes);
         memcpy (&TxBuffer[TxBytes], bytes, ToCopy);
         bytes += ToCopy;
         TxBytes += ToCopy;
         len -= ToCopy;
         if (TxBytes >= TSIZE)
            UnbufferBytes ();
      } while (len > 0 && EndRun == FALSE);
   }
}

SHORT TSerial::BytesReady (VOID)
{
   SHORT RetVal = FALSE;
#if defined(__OS2__)
   UINT data = 0;
   ULONG readed;

   if (hFile != NULLHANDLE) {
      Carrier ();

      if (RxBytes > 0)
         RetVal = TRUE;
      else {
         DevIOCtl ((VOID *)&data, sizeof (UINT), NULL, 0, ASYNC_GETINQUECOUNT, IOCTL_ASYNC, hFile);
         if ((data & 0xFFFF) > 0) {
            RetVal = TRUE;
            Rxd = TRUE;
         }
      }
   }

   return (RetVal);
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
            Rxd = TRUE;
         }
      }
   }

   return (RetVal);
#else
   if (hPort != NULL) {
      if (RxBytes > 0)
         RetVal = TRUE;
      else {
         if (PeekChar (hPort) >= 0) {
            RetVal = TRUE;
            Rxd = TRUE;
         }
      }
   }

   return (RetVal);
#endif
}

SHORT TSerial::Carrier (VOID)
{
#if defined(__OS2__)
   UCHAR data = 0;
   UINT cdata;
   ULONG readed;

   Txd = FALSE;
   DevIOCtl ((VOID *)&cdata, sizeof (UINT), NULL, 0, ASYNC_GETOUTQUECOUNT, IOCTL_ASYNC, hFile);
   if ((cdata & 0xFFFF) > 0)
      Txd = TRUE;

   Rxd = FALSE;
   DevIOCtl ((VOID *)&cdata, sizeof (UINT), NULL, 0, ASYNC_GETINQUECOUNT, IOCTL_ASYNC, hFile);
   if ((cdata & 0xFFFF) > 0)
      Rxd = TRUE;

   if (hFile != NULLHANDLE && EndRun == FALSE)
      DevIOCtl ((VOID *)&data, sizeof (UCHAR), NULL, 0, ASYNC_GETMODEMINPUT, IOCTL_ASYNC, hFile);

   Dcd = (data & DCD) ? TRUE : FALSE;
   Ri = (data & RI) ? TRUE : FALSE;
   Dsr = (data & DSR) ? TRUE : FALSE;
   Cts = (data & CTS) ? TRUE : FALSE;

   return (Dcd);
#elif defined(__NT__)
   DWORD Status = 0L;

   if (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE)
      GetCommModemStatus (hFile, &Status);

   Dcd = (Status & MS_RLSD_ON) ? TRUE : FALSE;

   return (Dcd);
#else
   int data = 0;

   Txd = FALSE;
   if (SpaceUsedInTXBuffer (hPort) > 0)
      Txd = TRUE;

   Rxd = FALSE;
   if (SpaceUsedInRXBuffer (hPort) > 0)
      Rxd = TRUE;

   if (hPort != NULL)
      data = GetModemStatus (hPort);

   Dcd = (data & CD_SET) ? TRUE : FALSE;
   Dsr = (data & DSR_SET) ? TRUE : FALSE;
   Cts = (data & CTS_SET) ? TRUE : FALSE;
   Ri = (data & RI_SET) ? TRUE : FALSE;

   return (Dcd);
#endif
}

VOID TSerial::ClearInbound (VOID)
{
#if defined(__OS2__)
   UINT data;
   CHAR parm = 0;

   Rxd = FALSE;
   if (hFile != NULLHANDLE) {
      RxBytes = 0;
      DevIOCtl (&data, sizeof (data), &parm, sizeof (parm), DEV_FLUSHINPUT, IOCTL_GENERAL, hFile);
   }
#elif defined(__NT__)
   Rxd = FALSE;
   if (hFile != INVALID_HANDLE_VALUE) {
      RxBytes = 0;
      PurgeComm (hFile, PURGE_RXCLEAR);
   }
#else
   Rxd = FALSE;
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

   Txd = FALSE;
   if (hFile != NULLHANDLE) {
      TxBytes = 0;
      DevIOCtl (&data, sizeof (data), &parm, sizeof (parm), DEV_FLUSHOUTPUT, IOCTL_GENERAL, hFile);
   }
#elif defined(__NT__)
   Txd = FALSE;
   if (hFile != INVALID_HANDLE_VALUE) {
      TxBytes = 0;
      PurgeComm (hFile, PURGE_TXCLEAR);
   }
#else
   Txd = FALSE;
   if (hPort != NULL) {
      TxBytes = 0;
      ClearTXBuffer (hPort);
   }
#endif
}

#if defined(__OS2__) || defined(__NT__)
SHORT TSerial::Initialize (PSZ pszDevice, ULONG ulSpeed, UCHAR nData, UCHAR nParity, UCHAR nStop)
{
   SHORT RetVal = FALSE;
#if defined(__OS2__)
   UINT data;
   ULONG action;
   MODEMSTATUS ms;
   DCBINFO dcbCom;

   hFile = NULLHANDLE;

   if (DosOpen (pszDevice, &hFile, &action, 0L, FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE, 0L) == 0) {
      RetVal = TRUE;

      DevIOCtl ((VOID *)&dcbCom, sizeof (DCBINFO), NULL, 0, ASYNC_GETDCBINFO, IOCTL_ASYNC, hFile);
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

      SetParameters (ulSpeed, nData, nParity, nStop);
   }
#else
   COMMTIMEOUTS cto;

   if ((hFile = CreateFile (pszDevice, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE) {
      GetCommTimeouts (hFile, &cto);
      cto.ReadIntervalTimeout = MAXDWORD;
      cto.ReadTotalTimeoutMultiplier = 0;
      cto.ReadTotalTimeoutConstant = 0;
      SetCommTimeouts (hFile, &cto);

      SetParameters (ulSpeed, nData, nParity, nStop);
      RetVal = TRUE;
   }
#endif

   return (RetVal);
}
#else
SHORT TSerial::Initialize (USHORT nPort, ULONG ulSpeed, UCHAR nData, UCHAR nParity, UCHAR nStop)
{
   breakchk (0);
   hPort = PortOpenGreenleafFast ((USHORT)(nPort - 1), ulSpeed, nParity, nData, nStop);
   if (hPort->status < ASSUCCESS) {
      hPort = NULL;
      return (FALSE);
   }
   else
      return (TRUE);
}
#endif

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
      if (RxBytes > 0) {
         c = *NextByte++;
         RxBytes--;
      }
   }

   return (c);
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
      if (RxBytes > 0) {
         c = *NextByte++;
         RxBytes--;
      }
   }

   return (c);
#else
   if (hPort != NULL) {
      if (RxBytes == 0) {
         do {
            ReadBuffer (hPort, (char *)RxBuffer, sizeof (RxBuffer));
         } while (hPort->count <= 0 && EndRun == FALSE);
         RxBytes = (USHORT)hPort->count;
         NextByte = RxBuffer;
      }
      if (RxBytes > 0) {
         c = *NextByte++;
         RxBytes--;
      }
   }

   return (c);
#endif
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
      if (RxBytes > 0) {
         if ((Max = len) > RxBytes)
            Max = RxBytes;
         memcpy (bytes, NextByte, Max);
         RxBytes -= Max;
         NextByte += Max;
      }
   }

   return (Max);
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
      if (RxBytes > 0) {
         if ((Max = len) > RxBytes)
            Max = RxBytes;
         memcpy (bytes, NextByte, Max);
         RxBytes -= Max;
         NextByte += Max;
      }
   }

   return (Max);
#else
   if (hPort != NULL) {
      if (RxBytes == 0) {
         do {
            ReadBuffer (hPort, (char *)RxBuffer, sizeof (RxBuffer));
         } while (hPort->count <= 0 && EndRun == FALSE);
         RxBytes = (USHORT)hPort->count;
         NextByte = RxBuffer;
      }
      if (RxBytes > 0) {
         if ((Max = len) > RxBytes)
            Max = RxBytes;
         memcpy (bytes, NextByte, Max);
         RxBytes -= Max;
         NextByte += Max;
      }
   }

   return (Max);
#endif
}

VOID TSerial::SetDTR (USHORT fStatus)
{
#if defined(__OS2__)
   MODEMSTATUS ms;
   UINT data;

   Dtr = fStatus;

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
   DCB dcb;

   Dtr = fStatus;

   if (hFile != INVALID_HANDLE_VALUE) {
      GetCommState (hFile, &dcb);
      dcb.fDtrControl = (fStatus == TRUE) ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE;
      SetCommState (hFile, &dcb);
   }
#else
   Dtr = fStatus;

   if (hPort != NULL) {
      if (fStatus == TRUE)
         SetDtr (hPort, ON);
      else
         SetDtr (hPort, OFF);
   }
#endif
}

VOID TSerial::SetRTS (USHORT fStatus)
{
#if defined(__OS2__)
   MODEMSTATUS ms;
   UINT data;

   Rts = fStatus;

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
   DCB dcb;

   Rts = fStatus;

   if (hFile != INVALID_HANDLE_VALUE) {
      GetCommState (hFile, &dcb);
      dcb.fDtrControl = (fStatus == TRUE) ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE;
      SetCommState (hFile, &dcb);
   }
#else
   Rts = fStatus;

   if (hPort != NULL) {
      if (fStatus == TRUE)
	 SetRts (hPort, ON);
      else
	 SetRts (hPort, OFF);
   }
#endif
}

VOID TSerial::SetParameters (ULONG ulSpeed, UCHAR nData, UCHAR nParity, UCHAR nStop)
{
#if defined(__OS2__)
   LINECONTROL lc;

   if (hFile != NULLHANDLE) {
      DevIOCtl (NULL, 0, (VOID *)&ulSpeed, sizeof (ULONG), ASYNC_SETBAUDRATE, IOCTL_ASYNC, hFile);

      lc.bDataBits = (BYTE)nData;
      lc.bStopBits = (BYTE)nStop;
      lc.bParity = (BYTE)nParity;

      DevIOCtl (NULL, 0, (VOID *)&lc, sizeof (LINECONTROL), ASYNC_SETLINECTRL, IOCTL_ASYNC, hFile);
   }
#elif defined(__NT__)
   DCB dcb;

   if (hFile != INVALID_HANDLE_VALUE) {
      GetCommState (hFile, &dcb);
      dcb.BaudRate = ulSpeed;
      dcb.ByteSize = nData;
      dcb.Parity = nParity;
      dcb.StopBits = nStop;
      dcb.fBinary = TRUE;
      dcb.fOutxCtsFlow = dcb.fOutxDsrFlow = TRUE;
      dcb.fDtrControl = DTR_CONTROL_ENABLE;
      SetCommState (hFile, &dcb);
   }
#else
   if (hPort != NULL)
      PortSet (hPort, ulSpeed, nParity, nData, nStop);
#endif
}

VOID TSerial::SendByte (UCHAR byte)
{
#if defined(__OS2__)
   ULONG written;

   Txd = TRUE;

   if (hFile != NULLHANDLE) {
      do {
         DosWrite (hFile, (PVOID)&byte, 1L, &written);
      } while (written != 1L && EndRun == FALSE);
   }
#elif defined(__NT__)
   ULONG written;

   Txd = TRUE;

   if (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE)
      do {
         WriteFile (hFile, (LPCVOID)&byte, 1L, &written, NULL);
      } while (written != 1L && EndRun == FALSE);
#else
   int retval;

   Txd = TRUE;

   if (hPort != NULL) {
      do {
         retval = WriteChar (hPort, byte);
      } while (retval != ASSUCCESS && hPort->status == ASBUFRFULL);
   }
#endif
}

VOID TSerial::SendBytes (UCHAR *bytes, USHORT len)
{
#if defined(__OS2__)
   ULONG written;

   Txd = TRUE;

   if (hFile != NULLHANDLE) {
      do {
         DosWrite (hFile, (PVOID)bytes, (long)len, &written);
         if (written < len)
            DosSleep (10L);
         bytes += written;
         len -= written;
      } while (len > 0 && EndRun == FALSE);
   }
#elif defined(__NT__)
   ULONG written;

   Txd = TRUE;

   if (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE)
      do {
         WriteFile (hFile, (LPCVOID)bytes, (long)len, &written, NULL);
         bytes += written;
         len -= written;
      } while (len > 0 && EndRun == FALSE);
#else
   int retval;

   Txd = TRUE;

   if (hPort != NULL) {
      do {
         retval = WriteBuffer (hPort, (char *)bytes, len);
         bytes += hPort->count;
         len -= hPort->count;
      } while (retval != ASSUCCESS && hPort->status == ASBUFRFULL);
   }
#endif
}

VOID TSerial::UnbufferBytes (VOID)
{
#if defined(__OS2__)
   ULONG Written;
   UCHAR *p;

   if (TxBytes > 0)
      Txd = TRUE;

   while (hFile != NULLHANDLE && TxBytes > 0 && EndRun == FALSE) {
      p = TxBuffer;
      do {
         DosWrite (hFile, (PVOID)p, TxBytes, &Written);
         if (Written < TxBytes)
            DosSleep (10L);
         p += Written;
         TxBytes -= (USHORT)Written;
      } while (TxBytes > 0 && EndRun == FALSE);
   }
#elif defined(__NT__)
   ULONG written;
   UCHAR *p;

   if (TxBytes > 0)
      Txd = TRUE;

   while (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE && TxBytes > 0) {
      p = TxBuffer;
      do {
         WriteFile (hFile, (LPCVOID)p, (long)TxBytes, &written, NULL);
         p += written;
         TxBytes -= (USHORT)written;
      } while (TxBytes > 0 && EndRun == FALSE);
   }
#else
   int retval;
   char *p;

   if (TxBytes > 0)
      Txd = TRUE;

   while (hPort != NULL && TxBytes > 0) {
      p = (char *)TxBuffer;
      do {
         retval = WriteBuffer (hPort, p, TxBytes);
         p += hPort->count;
         TxBytes -= hPort->count;
      } while (TxBytes > 0 && EndRun == FALSE);
   }
#endif
}

