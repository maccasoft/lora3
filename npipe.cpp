
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.13
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"

TPipe::TPipe (void)
{
   EndRun = FALSE;
   TxBytes = 0;
}

TPipe::~TPipe (void)
{
#if defined(__OS2__)
   if (hFile != NULL) {
      DosDisConnectNPipe (hFile);
      DosClose (hFile);
   }
#elif defined(__NT__)
   if (hServerR != INVALID_HANDLE_VALUE)
      CloseHandle (hServerR);
   if (hServerW != INVALID_HANDLE_VALUE)
      CloseHandle (hServerW);
   if (hClientR != INVALID_HANDLE_VALUE)
      CloseHandle (hClientR);
   if (hClientW != INVALID_HANDLE_VALUE)
      CloseHandle (hClientW);
#endif
}

VOID TPipe::BufferByte (UCHAR byte)
{
   TxBuffer[TxBytes++] = byte;
   if (TxBytes >= TSIZE)
      UnbufferBytes ();
}

VOID TPipe::BufferBytes (UCHAR *bytes, USHORT len)
{
   while (len > 0 && EndRun == FALSE) {
      TxBuffer[TxBytes++] = *bytes++;
      if (TxBytes >= TSIZE)
         UnbufferBytes ();
      len--;
   }
}

USHORT TPipe::BytesReady (VOID)
{
   USHORT RetVal = FALSE;
#if defined(__OS2__)
   ULONG data, Temp, pipeState;
   struct _AVAILDATA Available;

   if (hFile != NULLHANDLE) {
      DosPeekNPipe (hFile, (PVOID)&Temp, sizeof (Temp), &data, &Available, &pipeState);
      if (data > 0)
         RetVal = TRUE;
      EndRun = FALSE;
      if (pipeState == NP_STATE_CLOSING)
         EndRun = TRUE;
   }
#elif defined(__NT__)
   ULONG Available;

   if (hServerR != INVALID_HANDLE_VALUE) {
      EndRun = FALSE;
      if (PeekNamedPipe (hServerR, NULL, 0L, NULL, &Available, NULL) == FALSE)
         EndRun = TRUE;
      if (Available > 0)
         RetVal = TRUE;
   }
#endif

   return (RetVal);
}

USHORT TPipe::Carrier (VOID)
{
//   return ((EndRun == FALSE) ? TRUE : FALSE);
   return (TRUE);
}

VOID TPipe::ClearInbound (VOID)
{
}

VOID TPipe::ClearOutbound (VOID)
{
   TxBytes = 0;
}

#if defined(__OS2__)
USHORT TPipe::Initialize (PSZ pszPipeName, USHORT usInstances)
{
   USHORT RetVal = FALSE;

   hFile = NULLHANDLE;

   if (DosCreateNPipe (pszPipeName, &hFile, NP_ACCESS_DUPLEX, NP_NOWAIT|usInstances, TSIZE, RSIZE, 1000) == 0) {
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TPipe::ConnectServer (PSZ pszPipeName)
{
   USHORT RetVal = FALSE;
   ULONG Action;

   hFile = NULLHANDLE;

   if (DosOpen (pszPipeName, &hFile, &Action, 0, FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE, NULL) == 0)
      RetVal = TRUE;

   return (RetVal);
}

#elif defined(__NT__)

USHORT TPipe::Initialize (VOID)
{
   USHORT RetVal = FALSE;
   SECURITY_ATTRIBUTES sa;

   sa.nLength = sizeof (SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = NULL;
   sa.bInheritHandle = TRUE;
   if (CreatePipe (&hServerR, &hClientW, &sa, 0L)) {
      sa.nLength = sizeof (SECURITY_ATTRIBUTES);
      sa.lpSecurityDescriptor = NULL;
      sa.bInheritHandle = TRUE;
      if (CreatePipe (&hClientR, &hServerW, &sa, 0L))
         RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TPipe::ConnectServer (HANDLE hRead, HANDLE hWrite)
{
   USHORT RetVal = TRUE;

   hServerR = hRead;
   hServerW = hWrite;

   return (RetVal);
}
#endif

UCHAR TPipe::ReadByte (VOID)
{
   UCHAR c = 0;
   ULONG bytesRead;

#if defined(__OS2__)
   if (hFile != NULLHANDLE)
      DosRead (hFile, (PVOID)&c, 1L, &bytesRead);
#elif defined(__NT__)
   if (hServerR != INVALID_HANDLE_VALUE)
      ReadFile (hServerR, (PVOID)&c, 1L, &bytesRead, NULL);
#endif

   return ((UCHAR)c);
}

USHORT TPipe::ReadBytes (UCHAR *bytes, USHORT len)
{
   ULONG bytesRead;

#if defined(__OS2__)
   if (hFile != NULLHANDLE)
      DosRead (hFile, (PVOID)bytes, len, &bytesRead);
#elif defined(__NT__)
   if (hServerR != INVALID_HANDLE_VALUE)
      ReadFile (hServerR, (PVOID)&bytes, len, &bytesRead, NULL);
#endif

   return ((USHORT)bytesRead);
}

VOID TPipe::SendByte (UCHAR byte)
{
   ULONG written;

#if defined(__OS2__)
   if (hFile != NULLHANDLE && EndRun == FALSE)
      DosWrite (hFile, (PVOID)&byte, 1L, &written);
#elif defined(__NT__)
   if (hServerW != INVALID_HANDLE_VALUE && EndRun == FALSE)
      do {
         WriteFile (hServerW, (LPCVOID)&byte, 1L, &written, NULL);
      } while (written != 1L);
#endif
}

VOID TPipe::SendBytes (UCHAR *bytes, USHORT len)
{
   ULONG written;

#if defined(__OS2__)
   if (hFile != NULLHANDLE)
      DosWrite (hFile, (PVOID)bytes, (long)len, &written);
#elif defined(__NT__)
   if (hServerW != INVALID_HANDLE_VALUE && EndRun == FALSE)
      do {
         WriteFile (hServerW, (LPCVOID)bytes, (long)len, &written, NULL);
         bytes += written;
         len -= written;
      } while (len > 0 && EndRun == FALSE);
#endif
}

VOID TPipe::UnbufferBytes (VOID)
{
   ULONG Written;
   UCHAR *p;

#if defined(__OS2__)
   if (hFile != NULLHANDLE && TxBytes > 0 && EndRun == FALSE) {
      p = TxBuffer;
      do {
         DosWrite (hFile, (PVOID)p, (long)TxBytes, &Written);
         if (Written < TxBytes)
            DosSleep (10L);
         p += Written;
         TxBytes -= (USHORT)Written;
      } while (TxBytes > 0 && EndRun == FALSE);
   }
#elif defined(__NT__)
   if (hServerW != INVALID_HANDLE_VALUE && TxBytes > 0 && EndRun == FALSE) {
      p = TxBuffer;
      do {
         WriteFile (hServerW, (LPCVOID)p, (long)TxBytes, &Written, NULL);
         p += Written;
         if (Written < TxBytes)
            Sleep (10L);
         TxBytes -= (USHORT)Written;
      } while (TxBytes > 0 && EndRun == FALSE);
   }
#endif
}

USHORT TPipe::WaitClient (VOID)
{
   USHORT RetVal = FALSE;

#if defined(__OS2__)
   if (DosConnectNPipe (hFile) == 0)
      RetVal = TRUE;
#elif defined(__NT__)
#endif

   return (RetVal);
}


