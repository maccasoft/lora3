
// LoraBBS Version 2.99 Free Edition
// Copyright (C) 1987-98 Marco Maccaferri
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "_ldefs.h"
#include "combase.h"

TPipe::TPipe (void)
{
   EndRun = FALSE;
   TxBytes = 0;
   CtlConnect = PipeConnect = FALSE;
}

TPipe::~TPipe (void)
{
   Name[0] = City[0] = Level[0] = '\0';
   TimeLeft = Time = 0L;

#if defined(__OS2__)
   if (hFileCtl != NULL) {
      DosDisConnectNPipe (hFileCtl);
      DosClose (hFileCtl);
   }
   if (hFile != NULL) {
      DosDisConnectNPipe (hFile);
      DosClose (hFile);
   }
#elif defined(__NT__)
   if (hFileCtl != INVALID_HANDLE_VALUE)
      CloseHandle (hFileCtl);
   if (hFile != INVALID_HANDLE_VALUE)
      CloseHandle (hFile);
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
   CHAR c, *p;
   ULONG data, Temp, pipeState;
   struct _AVAILDATA Available;

   if (hFile != NULLHANDLE) {
      data = 0L;
      DosPeekNPipe (hFile, (PVOID)&Temp, sizeof (Temp), &data, &Available, &pipeState);
      if (data > 0L)
         RetVal = TRUE;
      EndRun = FALSE;
      if (pipeState == NP_STATE_CLOSING)
         EndRun = TRUE;
   }

   if (hFileCtl != NULLHANDLE) {
      data = 0L;
      DosPeekNPipe (hFileCtl, (PVOID)&Temp, sizeof (Temp), &data, &Available, &pipeState);
      if (data >= 2L) {
         c = 0;
         DosRead (hFileCtl, (PVOID)&c, 1L, &data);
         switch (c) {
            case 1:
               p = Name;
               do {
                  c = '\0';
                  DosRead (hFileCtl, (PVOID)&c, 1L, &data);
                  *p++ = c;
               } while (c != '\0');
               break;
            case 2:
               p = City;
               do {
                  c = '\0';
                  DosRead (hFileCtl, (PVOID)&c, 1L, &data);
                  *p++ = c;
               } while (c != '\0');
               break;
            case 3:
               p = Level;
               do {
                  c = '\0';
                  DosRead (hFileCtl, (PVOID)&c, 1L, &data);
                  *p++ = c;
               } while (c != '\0');
               break;
            case 4:
               DosRead (hFileCtl, (PVOID)&TimeLeft, sizeof (ULONG), &data);
               break;
            case 5:
               DosRead (hFileCtl, (PVOID)&Time, sizeof (ULONG), &data);
               break;
         }
      }
   }

   if (RetVal == FALSE)
      DosSleep (1L);

#elif defined(__NT__)
   ULONG Available;

   if (hFile != INVALID_HANDLE_VALUE) {
      EndRun = FALSE;
      if (PeekNamedPipe (hFile, NULL, 0L, NULL, &Available, NULL) == FALSE)
         EndRun = TRUE;
      if (Available > 0)
         RetVal = TRUE;
   }

   if (RetVal == FALSE)
      Sleep (1L);
#endif

   return (RetVal);
}

USHORT TPipe::Carrier (VOID)
{
   USHORT RetVal = TRUE;
#if defined(__OS2__)
   ULONG data, Temp, pipeState;
   struct _AVAILDATA Available;

   if (hFileCtl != NULLHANDLE) {
      data = 0L;
      DosPeekNPipe (hFileCtl, (PVOID)&Temp, sizeof (Temp), &data, &Available, &pipeState);
      if (pipeState == NP_STATE_CLOSING)
         RetVal = FALSE;
   }
   if (hFile != NULLHANDLE) {
      data = 0L;
      DosPeekNPipe (hFile, (PVOID)&Temp, sizeof (Temp), &data, &Available, &pipeState);
      if (pipeState == NP_STATE_CLOSING)
         RetVal = FALSE;
   }

#elif defined(__NT__)
   ULONG Available;

   if (hFileCtl != INVALID_HANDLE_VALUE) {
      if (PeekNamedPipe (hFileCtl, NULL, 0L, NULL, &Available, NULL) == FALSE)
         RetVal = FALSE;
   }
   if (hFile != INVALID_HANDLE_VALUE) {
      if (PeekNamedPipe (hFile, NULL, 0L, NULL, &Available, NULL) == FALSE)
         RetVal = FALSE;
   }
#endif

   return (RetVal);
}

VOID TPipe::ClearInbound (VOID)
{
}

VOID TPipe::ClearOutbound (VOID)
{
   TxBytes = 0;
}

USHORT TPipe::Initialize (PSZ pszPipeName, PSZ pszCtlName, USHORT usInstances)
{
   USHORT RetVal = FALSE;
   CHAR TempFile[128];

   CtlConnect = PipeConnect = FALSE;

#if defined(__OS2__)
   hFileCtl = NULLHANDLE;
   if (!strncmp (pszCtlName, "\\\\.", 3)) {
      strcpy (TempFile, &pszCtlName[3]);
      pszCtlName = TempFile;
   }
   if (DosCreateNPipe (pszCtlName, &hFileCtl, NP_ACCESS_DUPLEX, NP_NOWAIT|usInstances, TSIZE, RSIZE, 1000) != 0) {
      hFileCtl = NULLHANDLE;
      CtlConnect = TRUE;
   }

   hFile = NULLHANDLE;
   if (!strncmp (pszPipeName, "\\\\.", 3)) {
      strcpy (TempFile, &pszPipeName[3]);
      pszPipeName = TempFile;
   }
   if (DosCreateNPipe (pszPipeName, &hFile, NP_ACCESS_DUPLEX, NP_NOWAIT|usInstances, TSIZE, RSIZE, 1000) == 0)
      RetVal = TRUE;

#elif defined(__NT__)
   hFileCtl = INVALID_HANDLE_VALUE;
   if (strncmp (pszCtlName, "\\\\.", 3)) {
      sprintf (TempFile, "\\\\.%s", pszCtlName);
      pszCtlName = TempFile;
   }
   hFileCtl = CreateNamedPipe (pszCtlName, PIPE_ACCESS_DUPLEX|FILE_FLAG_WRITE_THROUGH, PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_NOWAIT, usInstances, TSIZE * 2, RSIZE, 1000, NULL);

   hFile = INVALID_HANDLE_VALUE;
   if (strncmp (pszPipeName, "\\\\.", 3)) {
      sprintf (TempFile, "\\\\.%s", pszPipeName);
      pszPipeName = TempFile;
   }
   if ((hFile = CreateNamedPipe (pszPipeName, PIPE_ACCESS_DUPLEX|FILE_FLAG_WRITE_THROUGH, PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_NOWAIT, usInstances, TSIZE * 2, RSIZE, 1000, NULL)) != INVALID_HANDLE_VALUE)
      RetVal = TRUE;
#endif

   return (RetVal);
}

USHORT TPipe::ConnectServer (PSZ pszPipeName, PSZ pszCtlName)
{
   USHORT RetVal = FALSE;
   CHAR TempFile[128];
#if defined(__OS2__)
   ULONG Action;
#endif

#if defined(__OS2__)
   hFileCtl = NULLHANDLE;
   if (!strncmp (pszCtlName, "\\\\.", 3)) {
      strcpy (TempFile, &pszCtlName[3]);
      pszCtlName = TempFile;
   }
   if (DosOpen (pszCtlName, &hFileCtl, &Action, 0, FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE, NULL) != 0)
      hFileCtl = NULLHANDLE;

   hFile = NULLHANDLE;
   if (!strncmp (pszPipeName, "\\\\.", 3)) {
      strcpy (TempFile, &pszPipeName[3]);
      pszPipeName = TempFile;
   }
   if (DosOpen (pszPipeName, &hFile, &Action, 0, FILE_NORMAL, FILE_OPEN, OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE, NULL) == 0)
      RetVal = TRUE;
#elif defined(__NT__)
   hFileCtl = INVALID_HANDLE_VALUE;
   if (strncmp (pszCtlName, "\\\\.", 3)) {
      sprintf (TempFile, "\\\\.%s", pszCtlName);
      pszCtlName = TempFile;
   }
   hFileCtl = CreateFile (pszCtlName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH, NULL);

   hFile = INVALID_HANDLE_VALUE;
   if (strncmp (pszPipeName, "\\\\.", 3)) {
      sprintf (TempFile, "\\\\.%s", pszPipeName);
      pszPipeName = TempFile;
   }
   if ((hFile = CreateFile (pszPipeName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH, NULL)) != INVALID_HANDLE_VALUE)
      RetVal = TRUE;
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
      DosRead (hFile, (PVOID)bytes, len, &bytesRead);
#elif defined(__NT__)
   if (hFile != INVALID_HANDLE_VALUE)
      ReadFile (hFile, (PVOID)bytes, len, &bytesRead, NULL);
#endif

   return ((USHORT)bytesRead);
}

VOID TPipe::SendByte (UCHAR byte)
{
   ULONG written;

#if defined(__OS2__)
   if (hFile != NULLHANDLE && EndRun == FALSE) {
      DosSetNPHState (hFile, NP_WAIT|NP_READMODE_BYTE);
      DosWrite (hFile, (PVOID)&byte, (long)1, &written);
      DosSetNPHState (hFile, NP_NOWAIT|NP_READMODE_BYTE);
   }
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
   if (hFile != NULLHANDLE) {
      DosSetNPHState (hFile, NP_WAIT|NP_READMODE_BYTE);
      DosWrite (hFile, (PVOID)bytes, (long)len, &written);
      DosSetNPHState (hFile, NP_NOWAIT|NP_READMODE_BYTE);
   }
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
#if defined(__NT__)
   UCHAR *p;
#endif

#if defined(__OS2__)
   if (hFile != NULLHANDLE && TxBytes > 0 && EndRun == FALSE) {
      DosSetNPHState (hFile, NP_WAIT|NP_READMODE_BYTE);
      DosWrite (hFile, (PVOID)TxBuffer, (long)TxBytes, &Written);
      TxBytes = 0;
      DosSetNPHState (hFile, NP_NOWAIT|NP_READMODE_BYTE);
   }
#elif defined(__NT__)
   if (hFile != INVALID_HANDLE_VALUE && TxBytes > 0 && EndRun == FALSE) {
      p = TxBuffer;
      do {
         WriteFile (hFile, (LPCVOID)p, (long)TxBytes, &Written, NULL);
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
   if (hFileCtl != NULLHANDLE && CtlConnect == FALSE) {
      if (DosConnectNPipe (hFileCtl) == 0)
         CtlConnect = TRUE;
   }
   if (hFile != NULLHANDLE && PipeConnect == FALSE) {
      if (DosConnectNPipe (hFile) == 0)
         PipeConnect = TRUE;
   }
   if (CtlConnect == TRUE && PipeConnect == TRUE)
      RetVal = TRUE;
#elif defined(__NT__)
   if (hFileCtl != INVALID_HANDLE_VALUE)
      ConnectNamedPipe (hFileCtl, NULL);
   ConnectNamedPipe (hFile, NULL);
   if (GetLastError () == ERROR_PIPE_CONNECTED)
      RetVal = TRUE;
#endif

   return (RetVal);
}

VOID TPipe::SetName (PSZ name)
{
   ULONG written;

#if defined(__OS2__)
   if (hFileCtl != NULLHANDLE) {
      DosSetNPHState (hFileCtl, NP_WAIT|NP_READMODE_BYTE);
      DosWrite (hFileCtl, (PVOID)"\x01", 1L, &written);
      DosWrite (hFileCtl, (PVOID)name, (long)(strlen (name) + 1), &written);
      DosSetNPHState (hFileCtl, NP_NOWAIT|NP_READMODE_BYTE);
   }
#elif defined(__NT__)
#endif
}

VOID TPipe::SetCity (PSZ name)
{
   ULONG written;

#if defined(__OS2__)
   if (hFileCtl != NULLHANDLE) {
      DosSetNPHState (hFileCtl, NP_WAIT|NP_READMODE_BYTE);
      DosWrite (hFileCtl, (PVOID)"\x02", 1L, &written);
      DosWrite (hFileCtl, (PVOID)name, (long)(strlen (name) + 1), &written);
      DosSetNPHState (hFileCtl, NP_NOWAIT|NP_READMODE_BYTE);
   }
#elif defined(__NT__)
#endif
}

VOID TPipe::SetLevel (PSZ level)
{
   ULONG written;

#if defined(__OS2__)
   if (hFileCtl != NULLHANDLE) {
      DosSetNPHState (hFileCtl, NP_WAIT|NP_READMODE_BYTE);
      DosWrite (hFileCtl, (PVOID)"\x03", 1L, &written);
      DosWrite (hFileCtl, (PVOID)level, (long)(strlen (level) + 1), &written);
      DosSetNPHState (hFileCtl, NP_NOWAIT|NP_READMODE_BYTE);
   }
#elif defined(__NT__)
#endif
}

VOID TPipe::SetTimeLeft (ULONG seconds)
{
   ULONG written;

#if defined(__OS2__)
   if (hFileCtl != NULLHANDLE) {
      DosSetNPHState (hFileCtl, NP_WAIT|NP_READMODE_BYTE);
      DosWrite (hFileCtl, (PVOID)"\x04", 1L, &written);
      DosWrite (hFileCtl, (PVOID)&seconds, sizeof (ULONG), &written);
      DosSetNPHState (hFileCtl, NP_NOWAIT|NP_READMODE_BYTE);
   }
#elif defined(__NT__)
#endif
}

VOID TPipe::SetTime (ULONG seconds)
{
   ULONG written;

#if defined(__OS2__)
   if (hFileCtl != NULLHANDLE) {
      DosSetNPHState (hFileCtl, NP_WAIT|NP_READMODE_BYTE);
      DosWrite (hFileCtl, (PVOID)"\x05", 1L, &written);
      DosWrite (hFileCtl, (PVOID)&seconds, sizeof (ULONG), &written);
      DosSetNPHState (hFileCtl, NP_NOWAIT|NP_READMODE_BYTE);
   }
#elif defined(__NT__)
#endif
}


