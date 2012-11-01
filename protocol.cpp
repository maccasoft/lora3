
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
#include "lora_api.h"

TProtocol::TProtocol (void)
{
   fdDat = -1;
   strcpy (DataFile, "protocol.dat");
}

TProtocol::TProtocol (PSZ path)
{
   fdDat = -1;
   strcpy (DataFile, path);
   strcat (DataFile, "protocol.dat");
   AdjustPath (strlwr (DataFile));
}

TProtocol::~TProtocol (void)
{
   if (fdDat != -1)
      close (fdDat);
}

VOID TProtocol::Struct2Class (PROTOCOL *proto)
{
   strcpy (Key, proto->Key);
   strcpy (Description, proto->Description);
   Active = proto->Active;
   Batch = proto->Batch;
   DisablePort = proto->DisablePort;
   ChangeToUploadPath = proto->ChangeToUploadPath;
   strcpy (DownloadCmd, proto->DownloadCmd);
   strcpy (UploadCmd, proto->UploadCmd);
   strcpy (LogFileName, proto->LogFileName);
   strcpy (CtlFileName, proto->CtlFileName);
   strcpy (DownloadCtlString, proto->DownloadCtlString);
   strcpy (UploadCtlString, proto->UploadCtlString);
   strcpy (DownloadKeyword, proto->DownloadKeyword);
   strcpy (UploadKeyword, proto->UploadKeyword);
   FileNamePos = proto->FileNamePos;
   SizePos = proto->SizePos;
   CpsPos = proto->CpsPos;
}

VOID TProtocol::Class2Struct (PROTOCOL *proto)
{
   memset (proto, 0, sizeof (PROTOCOL));

   proto->Size = sizeof (PROTOCOL);
   strcpy (proto->Key, Key);
   strcpy (proto->Description, Description);
   proto->Active = Active;
   proto->Batch = Batch;
   proto->DisablePort = DisablePort;
   proto->ChangeToUploadPath = ChangeToUploadPath;
   strcpy (proto->DownloadCmd, DownloadCmd);
   strcpy (proto->UploadCmd, UploadCmd);
   strcpy (proto->LogFileName, LogFileName);
   strcpy (proto->CtlFileName, CtlFileName);
   strcpy (proto->DownloadCtlString, DownloadCtlString);
   strcpy (proto->UploadCtlString, UploadCtlString);
   strcpy (proto->DownloadKeyword, DownloadKeyword);
   strcpy (proto->UploadKeyword, UploadKeyword);
   proto->FileNamePos = FileNamePos;
   proto->SizePos = SizePos;
   proto->CpsPos = CpsPos;
}

VOID TProtocol::Add (VOID)
{
   USHORT DoClose = FALSE;

   if (fdDat == -1) {
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1) {
      Class2Struct (&prot);
      lseek (fdDat, 0L, SEEK_END);
      write (fdDat, &prot, sizeof (PROTOCOL));
   }

   if (fdDat != -1 && DoClose == TRUE) {
      close (fdDat);
      fdDat = -1;
   }
}

VOID TProtocol::Delete (VOID)
{
   int fdNew;
   USHORT DoClose = FALSE;
   ULONG Position;

   if (fdDat == -1) {
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   fdNew = sopen ("temp.dat", O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdNew != -1) {
      if ((Position = tell (fdDat)) > 0L)
         Position -= sizeof (PROTOCOL);

      lseek (fdDat, 0L, SEEK_SET);

      while (read (fdDat, &prot, sizeof (PROTOCOL)) == sizeof (PROTOCOL)) {
         if (strcmp (Key, prot.Key))
            write (fdNew, &prot, sizeof (PROTOCOL));
      }

      lseek (fdDat, 0L, SEEK_SET);
      lseek (fdNew, 0L, SEEK_SET);

      while (read (fdNew, &prot, sizeof (PROTOCOL)) == sizeof (PROTOCOL))
            write (fdDat, &prot, sizeof (PROTOCOL));

      chsize (fdDat, tell (fdDat));

      lseek (fdDat, Position, SEEK_SET);
      if (Next () == FALSE) {
         if (Previous () == FALSE)
            New ();
      }
   }

   if (fdNew != -1) {
      close (fdNew);
      unlink ("temp.dat");
   }
   if (fdDat != -1 && DoClose == TRUE) {
      close (fdDat);
      fdDat = -1;
   }
}

USHORT TProtocol::First (VOID)
{
   USHORT retVal = FALSE;

   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1) {
      lseek (fdDat, 0L, SEEK_SET);
      retVal = Next ();
   }

   return (retVal);
}

VOID TProtocol::New (VOID)
{
   memset (&prot, 0, sizeof (PROTOCOL));
   Struct2Class (&prot);
}

USHORT TProtocol::Next (VOID)
{
   USHORT retVal = FALSE;

   if (fdDat != -1) {
      if (read (fdDat, &prot, sizeof (PROTOCOL)) == sizeof (PROTOCOL)) {
         Struct2Class (&prot);
         retVal = TRUE;
      }
   }

   return (retVal);
}

USHORT TProtocol::Previous (VOID)
{
   USHORT retVal = FALSE;

   if (fdDat != -1) {
      if (tell (fdDat) > sizeof (PROTOCOL)) {
         lseek (fdDat, tell (fdDat) - sizeof (PROTOCOL) * 2, SEEK_SET);
         read (fdDat, &prot, sizeof (PROTOCOL));
         Struct2Class (&prot);
         retVal = TRUE;
      }
   }

   return (retVal);
}

USHORT TProtocol::Read (PSZ key)
{
   USHORT RetVal = FALSE, DoClose = FALSE;

   if (fdDat == -1) {
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1) {
      lseek (fdDat, 0L, SEEK_SET);
      while (read (fdDat, &prot, sizeof (PROTOCOL)) == sizeof (PROTOCOL)) {
         if (!stricmp (key, prot.Key)) {
            Struct2Class (&prot);
            RetVal = TRUE;
            break;
         }
      }
   }

   if (fdDat != -1 && DoClose == TRUE) {
      close (fdDat);
      fdDat = -1;
   }

   return (RetVal);
}

VOID TProtocol::Update (VOID)
{
   USHORT retVal = FALSE;

   if (fdDat != -1) {
      if (tell (fdDat) >= sizeof (PROTOCOL)) {
         lseek (fdDat, tell (fdDat) - sizeof (PROTOCOL), SEEK_SET);
         Class2Struct (&prot);
         write (fdDat, &prot, sizeof (PROTOCOL));
      }
   }
}


