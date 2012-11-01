
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
   if (hFile != NULL)
      DosClose (hFile);
#elif defined(__NT__)
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

SHORT TPipe::BytesReady (VOID)
{
   SHORT RetVal = FALSE;
#if defined(__OS2__)
   ULONG data, Temp, bytesRead;
   struct _AVAILDATA Available;

   if (hFile != NULLHANDLE) {
      DosPeekNPipe (hFile, (PVOID)&Temp, sizeof (Temp), &data, &Available, &bytesRead);
      if (data > 0)
         RetVal = TRUE;
   }
#elif defined(__NT__)
   ULONG Available;

   if (hFile != INVALID_HANDLE_VALUE) {
      PeekNamedPipe (hFile, NULL, 0L, NULL, &Available, NULL);
      if (Available > 0)
         RetVal = TRUE;
   }
#endif

   return (RetVal);
}

SHORT TPipe::Carrier (VOID)
{
   return ((EndRun == FALSE) ? TRUE : FALSE);
}

VOID TPipe::ClearInbound (VOID)
{
}

VOID TPipe::ClearOutbound (VOID)
{
}

SHORT TPipe::Initialize (PSZ pszPipeName, USHORT usInstances)
{
   SHORT RetVal = FALSE;

#if defined(__OS2__)
   hFile = NULLHANDLE;

   if (DosCreateNPipe (pszPipeName, &hFile, NP_ACCESS_DUPLEX, NP_NOWAIT|usInstances, TSIZE, RSIZE, 1000) == 0) {
      RetVal = TRUE;
      while (DosConnectNPipe (hFile))
         DosSleep (1L);
   }
#elif defined(__NT__)
   if ((hFile = CreateNamedPipe (pszPipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE|PIPE_NOWAIT, usInstances, TSIZE, RSIZE, 1000, NULL)) != INVALID_HANDLE_VALUE) {
      RetVal = TRUE;
      while (ConnectNamedPipe (hFile, NULL))
         ;
   }
#endif

   return (RetVal);
}

UCHAR TPipe::ReadByte (VOID)
{
   UCHAR c = 0;
   ULONG bytesRead;

#if defined(__OS2__)
   if (hFile != NULLHANDLE)
      DosRead (hFile, (PVOID)&c, 1L, &bytesRead);
#elif defined(__NT__)
   if (hFile != INVALID_HANDLE_VALUE)
      ReadFile (hFile, (PVOID)&c, 1L, &bytesRead, NULL);
#endif

   return ((UCHAR)c);
}

USHORT TPipe::ReadBytes (UCHAR *bytes, USHORT len)
{
   ULONG bytesRead;

#if defined(__OS2__)
   if (hFile != NULLHANDLE)
      DosRead (hFile, (PVOID)&bytes, len, &bytesRead);
#elif defined(__NT__)
   if (hFile != INVALID_HANDLE_VALUE)
      ReadFile (hFile, (PVOID)&bytes, len, &bytesRead, NULL);
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
   if (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE)
      do {
         WriteFile (hFile, (LPCVOID)&byte, 1L, &written, NULL);
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
   if (hFile != INVALID_HANDLE_VALUE && EndRun == FALSE)
      do {
         WriteFile (hFile, (LPCVOID)bytes, (long)len, &written, NULL);
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
   if (hFile != INVALID_HANDLE_VALUE && TxBytes > 0 && EndRun == FALSE) {
      p = TxBuffer;
      do {
         WriteFile (hFile, (LPCVOID)p, (long)TxBytes, &Written, NULL);
         p += Written;
         TxBytes -= (USHORT)Written;
      } while (TxBytes > 0 && EndRun == FALSE);
#endif
}


