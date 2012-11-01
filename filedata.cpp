
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

TFileData::TFileData (void)
{
   fdDat = fdIdx = -1;
   strcpy (DataFile, "file.dat");
   strcpy (IdxFile, "file.idx");
}

TFileData::TFileData (PSZ pszDataPath)
{
   strcpy (DataFile, pszDataPath);
   strcpy (IdxFile, DataFile);

   fdDat = fdIdx = -1;
   strcat (DataFile, "file.dat");
   strcat (IdxFile, "file.idx");

   AdjustPath (strlwr (DataFile));
   AdjustPath (strlwr (IdxFile));
}

TFileData::~TFileData (void)
{
   if (fdDat != -1)
      close (fdDat);
   if (fdIdx != -1)
      close (fdIdx);
}

USHORT TFileData::Add (VOID)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   FILES *File;
   INDEX Idx;

   if (fdIdx == -1) {
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }
   if (fdDat == -1) {
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdDat, 0L, SEEK_END);
      lseek (fdIdx, 0L, SEEK_END);

      if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
         memset (File, 0, sizeof (FILES));
         File->Size = sizeof (FILES);
         Class2Struct (File);

         memset (&Idx, 0, sizeof (Idx));
         strcpy (Idx.Key, Key);
         Idx.Level = Level;
         Idx.AccessFlags = AccessFlags;
         Idx.DenyFlags = DenyFlags;
         Idx.Position = tell (fdDat);

         write (fdDat, File, sizeof (FILES));
         write (fdIdx, &Idx, sizeof (Idx));
         RetVal = TRUE;

         free (File);
      }
   }

   if (DoClose == TRUE) {
      if (fdDat != -1) {
         close (fdDat);
         fdDat = -1;
      }
      if (fdIdx != -1) {
         close (fdIdx);
         fdIdx = -1;
      }
   }

   return (RetVal);
}

VOID TFileData::Class2Struct (FILES *File)
{
   strcpy (File->Display, Display);
   strcpy (File->Key, Key);
   File->Level = Level;
   File->AccessFlags = AccessFlags;
   File->DenyFlags = DenyFlags;
   File->UploadLevel = UploadLevel;
   File->UploadFlags = UploadFlags;
   File->UploadDenyFlags = UploadDenyFlags;
   File->DownloadLevel = DownloadLevel;
   File->DownloadFlags = DownloadFlags;
   File->DownloadDenyFlags = DownloadDenyFlags;
   File->Age = Age;
   strcpy (File->Download, Download);
   strcpy (File->Upload, Upload);
   File->CdRom = CdRom;
   File->FreeDownload = FreeDownload;
   File->ShowGlobal = ShowGlobal;
   strcpy (File->MenuName, MenuName);
   strcpy (File->Moderator, Moderator);
   File->Cost = Cost;
   File->ActiveFiles = ActiveFiles;
   File->UnapprovedFiles = UnapprovedFiles;
   strcpy (File->EchoTag, EchoTag);
   File->UseFilesBBS = UseFilesBBS;
   File->DlCost = DlCost;
   strcpy (File->FileList, FileList);
}

VOID TFileData::Delete (VOID)
{
   int fdNew;
   ULONG Position;
   FILES *File;
   INDEX Idx;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   fdNew = sopen ("Temp2.Dat", O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1 && fdNew != -1) {
      lseek (fdDat, 0L, SEEK_SET);

      if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
         while (read (fdDat, File, sizeof (FILES)) == sizeof (FILES)) {
            if (strcmp (LastKey, File->Key))
               write (fdNew, File, sizeof (FILES));
         }

         if ((Position = tell (fdIdx)) > 0L)
            Position -= sizeof (Idx);

         close (fdDat);
         close (fdIdx);
         fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);
         fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);

         if (fdDat != -1 && fdIdx != -1) {
            lseek (fdNew, 0L, SEEK_SET);

            while (read (fdNew, File, sizeof (FILES)) == sizeof (FILES)) {
               memset (&Idx, 0, sizeof (Idx));
               strcpy (Idx.Key, File->Key);
               Idx.Level = File->Level;
               Idx.AccessFlags = File->AccessFlags;
               Idx.DenyFlags = File->DenyFlags;
               Idx.Position = tell (fdDat);

               write (fdIdx, &Idx, sizeof (Idx));
               write (fdDat, File, sizeof (FILES));
            }

            lseek (fdIdx, Position, SEEK_SET);
            if (Next () == FALSE) {
               if (Previous () == FALSE)
                  New ();
            }
         }

         free (File);
      }
   }

   if (fdNew != -1) {
      close (fdNew);
      unlink ("Temp2.Dat");
   }
}

USHORT TFileData::First (VOID)
{
   USHORT RetVal = FALSE;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);
      RetVal = Next ();
   }

   return (RetVal);
}

USHORT TFileData::Insert (class TFileData *Data)
{
   PSZ p;

   strcpy (Display, Data->Display);
   strcpy (Key, Data->Key);
   Level = Data->Level;
   AccessFlags = Data->AccessFlags;
   DenyFlags = Data->DenyFlags;
   UploadLevel = Data->UploadLevel;
   UploadFlags = Data->UploadFlags;
   UploadDenyFlags = Data->UploadDenyFlags;
   DownloadLevel = Data->DownloadLevel;
   DownloadFlags = Data->DownloadFlags;
   DownloadDenyFlags = Data->DownloadDenyFlags;
   Age = Data->Age;

   strcpy (Download, Data->Download);
   if (Download[strlen (Download) - 1] != '\\' && Download[strlen (Download) - 1] != '/')
      strcat (Download, "\\");
#if defined(__LINUX__)
   while ((p = strchr (Download, '\\')) != NULL)
      *p = '/';
#else
   while ((p = strchr (Download, '/')) != NULL)
      *p = '\\';
#endif

   strcpy (Upload, Data->Upload);
   if (Upload[strlen (Upload) - 1] != '\\' && Upload[strlen (Upload) - 1] != '/')
      strcat (Upload, "\\");
#if defined(__LINUX__)
   while ((p = strchr (Upload, '\\')) != NULL)
      *p = '/';
#else
   while ((p = strchr (Upload, '/')) != NULL)
      *p = '\\';
#endif

   CdRom = Data->CdRom;
   FreeDownload = Data->FreeDownload;
   ShowGlobal = Data->ShowGlobal;
   strcpy (MenuName, Data->MenuName);
   strcpy (Moderator, Data->Moderator);
   Cost = Data->Cost;
   ActiveFiles = Data->ActiveFiles;
   UnapprovedFiles = Data->UnapprovedFiles;
   strcpy (EchoTag, Data->EchoTag);
   UseFilesBBS = Data->UseFilesBBS;
   DlCost = Data->DlCost;
   strcpy (FileList, Data->FileList);

   return (Insert ());
}

USHORT TFileData::Insert (VOID)
{
   int fdNew;
   USHORT retVal = FALSE;
   ULONG Position;
   FILES *File;
   INDEX Idx;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   fdNew = sopen ("Temp2.Dat", O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1 && fdNew != -1) {
      lseek (fdDat, 0L, SEEK_SET);

      if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
         while (read (fdDat, File, sizeof (FILES)) == sizeof (FILES)) {
            write (fdNew, File, sizeof (FILES));
            if (!strcmp (LastKey, File->Key)) {
               memset (File, 0, sizeof (FILES));
               File->Size = sizeof (FILES);
               Class2Struct (File);
               write (fdNew, File, sizeof (FILES));
            }
         }

         lseek (fdDat, 0L, SEEK_SET);
         lseek (fdNew, 0L, SEEK_SET);

         Position = tell (fdIdx);
         lseek (fdIdx, 0L, SEEK_SET);

         while (read (fdNew, File, sizeof (FILES)) == sizeof (FILES)) {
            memset (&Idx, 0, sizeof (Idx));
            strcpy (Idx.Key, File->Key);
            Idx.Level = File->Level;
            Idx.AccessFlags = File->AccessFlags;
            Idx.DenyFlags = File->DenyFlags;
            Idx.Position = tell (fdDat);

            write (fdIdx, &Idx, sizeof (Idx));
            write (fdDat, File, sizeof (FILES));
         }

         lseek (fdIdx, Position, SEEK_SET);
         Next ();

         retVal = TRUE;
         free (File);
      }
   }

   if (fdNew != -1) {
      close (fdNew);
      unlink ("Temp2.Dat");
   }

   return (retVal);
}

USHORT TFileData::Last (VOID)
{
   USHORT RetVal = FALSE, IsValid = FALSE;
   FILES *File;
   INDEX Idx;

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_END);
      if (tell (fdIdx) >= sizeof (INDEX)) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
               if (read (fdDat, File, sizeof (FILES)) == sizeof (FILES)) {
                  RetVal = TRUE;
                  New ();
                  Struct2Class (File);
                  IsValid = TRUE;
               }

               free (File);
            }
         }
      }
      while (IsValid == FALSE && tell (fdIdx) >= sizeof (INDEX) * 2) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX) * 2, SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
               if (read (fdDat, File, sizeof (FILES)) == sizeof (FILES)) {
                  RetVal = TRUE;
                  New ();
                  Struct2Class (File);
                  IsValid = TRUE;
               }

               free (File);
            }
         }
      }
   }

   return (RetVal);
}

VOID TFileData::New (VOID)
{
   memset (Display, 0, sizeof (Display));
   memset (Key, 0, sizeof (Key));
   Level = 0;
   AccessFlags = DenyFlags = 0L;
   UploadLevel = 0;
   UploadFlags = UploadDenyFlags = 0L;
   DownloadLevel = 0;
   DownloadFlags = DownloadDenyFlags = 0L;
   Age = 0;
   memset (Download, 0, sizeof (Download));
   memset (Upload, 0, sizeof (Upload));
   CdRom = FALSE;
   FreeDownload = FALSE;
   ShowGlobal = TRUE;
   memset (MenuName, 0, sizeof (MenuName));
   memset (Moderator, 0, sizeof (Moderator));
   Cost = 0L;
   ActiveFiles = UnapprovedFiles = 0L;
   memset (EchoTag, 0, sizeof (EchoTag));
}

USHORT TFileData::Next (VOID)
{
   USHORT RetVal = FALSE, IsValid = FALSE;
   FILES *File;
   INDEX Idx;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED)) {
            if (tell (fdDat) != Idx.Position)
               lseek (fdDat, Idx.Position, SEEK_SET);
            if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
               if (read (fdDat, File, sizeof (FILES)) == sizeof (FILES)) {
                  RetVal = TRUE;
                  New ();
                  Struct2Class (File);
                  IsValid = TRUE;
               }

               free (File);
            }
         }
      }
   }

   return (RetVal);
}

VOID TFileData::Pack (VOID)
{
   int fdNewIdx, fdNewDat;
   INDEX Idx;
   FILES *File;

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdIdx != -1 && fdDat != -1) {
      lseek (fdIdx, 0L, SEEK_SET);

      if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
         fdNewIdx = open ("File-New.Idx", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
         fdNewDat = open ("File-New.Dat", O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

         if (fdNewIdx != -1 && fdNewDat != -1) {
            while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
               if (!(Idx.Flags & IDX_DELETED)) {
                  lseek (fdDat, Idx.Position, SEEK_SET);
                  read (fdDat, File, sizeof (FILES));
                  Idx.Position = tell (fdNewDat);
                  write (fdNewDat, File, sizeof (FILES));
                  write (fdNewIdx, &Idx, sizeof (Idx));
               }
            }

            close (fdNewIdx);
            close (fdNewDat);

            unlink (DataFile);
            rename ("File-New.Dat", DataFile);
            unlink (IdxFile);
            rename ("File-New.Idx", IdxFile);
         }

         unlink ("File-New.Dat");
         unlink ("File-New.Idx");
         free (File);
      }

      close (fdIdx);
      close (fdDat);
      fdIdx = fdDat = -1;
   }
}

USHORT TFileData::Previous (VOID)
{
   USHORT RetVal = FALSE, IsValid = FALSE;
   FILES *File;
   INDEX Idx;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && tell (fdIdx) >= sizeof (INDEX) * 2) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX) * 2, SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
               if (read (fdDat, File, sizeof (FILES)) == sizeof (FILES)) {
                  RetVal = TRUE;
                  New ();
                  Struct2Class (File);
                  IsValid = TRUE;
               }

               free (File);
            }
         }
      }
   }

   return (RetVal);
}

USHORT TFileData::Read (PSZ pszName, USHORT fCloseFile)
{
   USHORT RetVal = FALSE;
   FILES *File;
   INDEX Idx;

   New ();

   if (fdIdx == -1)
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   if (fdDat == -1)
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);

      while (RetVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && !stricmp (pszName, Idx.Key)) {
            if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
               lseek (fdDat, Idx.Position, SEEK_SET);
               read (fdDat, File, sizeof (FILES));

               RetVal = TRUE;
               Struct2Class (File);

               free (File);
            }
         }
      }
   }

   if (fCloseFile == TRUE) {
      if (fdDat != -1) {
         close (fdDat);
         fdDat = -1;
      }
      if (fdIdx != -1) {
         close (fdIdx);
         fdIdx = -1;
      }
   }

   return (RetVal);
}

USHORT TFileData::ReadEcho (PSZ pszEchoTag)
{
   FILE *fp;
   USHORT retVal = FALSE;
   FILES *File;

   New ();

   fp = fopen (DataFile, "r+b");
   File = (FILES *)malloc (sizeof (FILES));

   if (fp != NULL && File != NULL) {
      setvbuf (fp, NULL, _IOFBF, 2048);
      while (fread (File, sizeof (FILES), 1, fp) == 1) {
         if (!stricmp (pszEchoTag, File->EchoTag)) {
            retVal = TRUE;
            Struct2Class (File);
            break;
         }
      }
   }

   if (File != NULL)
      free (File);
   if (fp != NULL)
      fclose (fp);

   return (retVal);
}

USHORT TFileData::ReRead (VOID)
{
   USHORT RetVal = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      if (tell (fdIdx) >= sizeof (INDEX)) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         RetVal = Next ();
      }
   }

   return (RetVal);
}

VOID TFileData::Struct2Class (FILES *File)
{
   PSZ p;

   strcpy (Display, File->Display);
   strcpy (Key, File->Key);
   Level = File->Level;
   AccessFlags = File->AccessFlags;
   DenyFlags = File->DenyFlags;
   UploadLevel = File->UploadLevel;
   UploadFlags = File->UploadFlags;
   UploadDenyFlags = File->UploadDenyFlags;
   DownloadLevel = File->DownloadLevel;
   DownloadFlags = File->DownloadFlags;
   DownloadDenyFlags = File->DownloadDenyFlags;
   Age = File->Age;

   strcpy (Download, File->Download);
   if (Download[strlen (Download) - 1] != '\\' && Download[strlen (Download) - 1] != '/')
      strcat (Download, "\\");
#if defined(__LINUX__)
   while ((p = strchr (Download, '\\')) != NULL)
      *p = '/';
#else
   while ((p = strchr (Download, '/')) != NULL)
      *p = '\\';
#endif

   strcpy (Upload, File->Upload);
   if (Upload[strlen (Upload) - 1] != '\\' && Upload[strlen (Upload) - 1] != '/')
      strcat (Upload, "\\");
#if defined(__LINUX__)
   while ((p = strchr (Upload, '\\')) != NULL)
      *p = '/';
#else
   while ((p = strchr (Upload, '/')) != NULL)
      *p = '\\';
#endif

   CdRom = File->CdRom;
   FreeDownload = File->FreeDownload;
   ShowGlobal = File->ShowGlobal;
   strcpy (MenuName, File->MenuName);
   strcpy (Moderator, File->Moderator);
   Cost = File->Cost;
   ActiveFiles = File->ActiveFiles;
   UnapprovedFiles = File->UnapprovedFiles;
   strcpy (EchoTag, File->EchoTag);
   UseFilesBBS = File->UseFilesBBS;
   DlCost = File->DlCost;
   strcpy (FileList, File->FileList);

   strcpy (LastKey, File->Key);
}

USHORT TFileData::Update (PSZ pszNewKey)
{
   USHORT RetVal = FALSE, DoClose = FALSE;
   FILES *File;
   INDEX Idx;

   if (fdIdx == -1) {
      fdIdx = sopen (IdxFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }
   if (fdDat == -1) {
      fdDat = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1 && fdIdx != -1) {
      if (DoClose == TRUE) {
         lseek (fdIdx, 0L, SEEK_SET);
         while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
            if (!(Idx.Flags & IDX_DELETED) && !stricmp (Key, Idx.Key)) {
               RetVal = TRUE;
               break;
            }
         }
      }
      else {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));

         if (strcmp (Idx.Key, Key)) {
            lseek (fdIdx, 0L, SEEK_SET);
            while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
               if (!(Idx.Flags & IDX_DELETED) && !stricmp (Key, Idx.Key)) {
                  RetVal = TRUE;
                  break;
               }
            }
         }
         else
            RetVal = TRUE;
      }

      if (RetVal == TRUE && tell (fdIdx) >= sizeof (INDEX)) {
         RetVal = FALSE;

         if ((File = (FILES *)malloc (sizeof (FILES))) != NULL) {
            memset (File, 0, sizeof (FILES));
            File->Size = sizeof (FILES);
            if (pszNewKey != NULL)
               strcpy (Key, pszNewKey);

            Class2Struct (File);

            strcpy (Idx.Key, Key);
            lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
            write (fdIdx, &Idx, sizeof (Idx));

            lseek (fdDat, Idx.Position, SEEK_SET);
            write (fdDat, File, sizeof (FILES));
            free (File);

            RetVal = TRUE;
         }
      }
   }

   if (DoClose == TRUE) {
      if (fdDat != -1) {
         close (fdDat);
         fdDat = -1;
      }
      if (fdIdx != -1) {
         close (fdIdx);
         fdIdx = -1;
      }
   }

   return (RetVal);
}

// --------------------------------------------------------------------------

#define ECHOLINK_INDEX     32

TFilechoLink::TFilechoLink (void)
{
   Data.Clear ();
   strcpy (DataFile, "fecholnk.dat");
   Skip4D = FALSE;
}

TFilechoLink::TFilechoLink (PSZ pszDataPath)
{
   Data.Clear ();

   strcpy (DataFile, pszDataPath);
   if (DataFile[0] != '\0') {
#if defined(__LINUX__)
      if (DataFile[strlen (DataFile) - 1] != '/')
         strcat (DataFile, "/");
#else
      if (DataFile[strlen (DataFile) - 1] != '\\')
         strcat (DataFile, "\\");
#endif
   }
   strcpy (DataFile, DataFile);
   strcat (DataFile, "fecholnk.dat");
   Skip4D = FALSE;
}

TFilechoLink::~TFilechoLink (void)
{
   Data.Clear ();
}

USHORT TFilechoLink::Add (VOID)
{
   USHORT Insert = FALSE;
   ECHOLINK Buffer, *Current;

   memset (&Buffer, 0, sizeof (ECHOLINK));

   Buffer.Free = FALSE;
   Buffer.EchoTag = EchoTag;
   Buffer.Zone = Zone;
   Buffer.Net = Net;
   Buffer.Node = Node;
   Buffer.Point = Point;
   strcpy (Buffer.Domain, Domain);
   Buffer.SendOnly = SendOnly;
   Buffer.ReceiveOnly = ReceiveOnly;
   Buffer.PersonalOnly = PersonalOnly;
   Buffer.Passive = Passive;
   Buffer.Skip = Skip;

   if ((Current = (ECHOLINK *)Data.First ()) != NULL) {
      if (Current->Zone > Zone)
         Insert = TRUE;
      else if (Current->Zone == Zone && Current->Net > Net)
         Insert = TRUE;
      else if (Current->Zone == Zone && Current->Net == Net && Current->Node > Node)
         Insert = TRUE;
      else if (Current->Zone == Zone && Current->Net == Net && Current->Node == Node && Current->Point > Point)
         Insert = TRUE;

      if (Insert == TRUE) {
         Data.Insert (&Buffer, sizeof (ECHOLINK));
         Data.Insert (Current, sizeof (ECHOLINK));
         Data.First ();
         Data.Remove ();
         Data.First ();
      }
      else {
         while ((Current = (ECHOLINK *)Data.Next ()) != NULL) {
            if (Current->Zone > Zone)
               Insert = TRUE;
            else if (Current->Zone == Zone && Current->Net > Net)
               Insert = TRUE;
            else if (Current->Zone == Zone && Current->Net == Net && Current->Node > Node)
               Insert = TRUE;
            else if (Current->Zone == Zone && Current->Net == Net && Current->Node == Node && Current->Point > Point)
               Insert = TRUE;

            if (Insert == TRUE) {
               Data.Previous ();
               Data.Insert (&Buffer, sizeof (ECHOLINK));
               break;
            }
         }
         if (Insert == FALSE) {
            Data.Add (&Buffer, sizeof (ECHOLINK));
            Insert = TRUE;
         }
      }
   }
   else {
      if (Insert == FALSE) {
         Data.Add (&Buffer, sizeof (ECHOLINK));
         Insert = TRUE;
      }
   }

   return (Insert);
}

USHORT TFilechoLink::AddString (PSZ pszString)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], *p;
   class TAddress Address;

   strcpy (Temp, pszString);
   if ((p = strtok (Temp, " ")) != NULL)
      do {
         Skip = FALSE;
         ReceiveOnly = SendOnly = PersonalOnly = FALSE;
         if (Check (p) == FALSE) {
            while (isdigit (*p) == 0 && *p != '.') {
               if (*p == '>')
                  ReceiveOnly = TRUE;
               if (*p == '<')
                  SendOnly = TRUE;
               if (*p == '!')
                  PersonalOnly = TRUE;
               p++;
            }
            Address.Parse (p);
            if (Address.Zone != 0)
               Zone = Address.Zone;
            if (Address.Net != 0)
               Net = Address.Net;
            if (Address.Node != 0)
               Node = Address.Node;
            Point = Address.Point;
            strcpy (Domain, Address.Domain);
            RetVal = Add ();
         }
      } while ((p = strtok (NULL, " ")) != NULL);

   return (RetVal);
}

VOID TFilechoLink::Change (PSZ pszFrom, PSZ pszTo)
{
   int fd, i, Count, Changed;
   ULONG CrcFrom, CrcTo, Position;
   ECHOLINK *Buffer;

   CrcFrom = StringCrc32 (pszFrom, 0xFFFFFFFFL);
   CrcTo = StringCrc32 (pszTo, 0xFFFFFFFFL);

   if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if ((Buffer = (ECHOLINK *)malloc (sizeof (ECHOLINK) * ECHOLINK_INDEX)) != NULL) {
         do {
            Changed = FALSE;

            Position = tell (fd);
            Count = read (fd, Buffer, sizeof (ECHOLINK) * ECHOLINK_INDEX) / sizeof (ECHOLINK);
            for (i = 0; i < Count; i++) {
               if (Buffer[i].EchoTag == CrcFrom) {
                  Buffer[i].EchoTag = CrcTo;
                  Changed = TRUE;
               }
            }

            if (Changed == TRUE) {
               lseek (fd, Position, SEEK_SET);
               write (fd, Buffer, sizeof (ECHOLINK) * Count);
            }
         } while (Count == ECHOLINK_INDEX);
         free (Buffer);
      }

      close (fd);
   }
}

USHORT TFilechoLink::Check (PSZ pszAddress)
{
   USHORT RetVal = FALSE;
   ECHOLINK *El;
   class TAddress Addr;

   while (isdigit (*pszAddress) == 0 && *pszAddress != '.')
      pszAddress++;
   Addr.Parse (pszAddress);

   if ((El = (ECHOLINK *)Data.First ()) != NULL)
      do {
         if (El->Zone == Addr.Zone && El->Net == Addr.Net && El->Node == Addr.Node && El->Point == Addr.Point) {
            EchoTag = El->EchoTag;
            Zone = El->Zone;
            Net = El->Net;
            Node = El->Node;
            Point = El->Point;
            strcpy (Domain, El->Domain);
            SendOnly = El->SendOnly;
            ReceiveOnly = El->ReceiveOnly;
            PersonalOnly = El->PersonalOnly;
            Passive = El->Passive;
            Skip = El->Skip;
            if (Point == 0)
               sprintf (Address, "%u:%u/%u", Zone, Net, Node);
            else
               sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
            if (Domain[0] != '\0') {
               strcat (Address, "@");
               strcat (Address, Domain);
            }
            RetVal = TRUE;
            break;
         }
      } while ((El = (ECHOLINK *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TFilechoLink::Clear (VOID)
{
   Data.Clear ();
   New ();
}

VOID TFilechoLink::Delete (VOID)
{
   if (Data.Value () != NULL)
      Data.Remove ();
}

USHORT TFilechoLink::First (VOID)
{
   USHORT RetVal = FALSE;
   ECHOLINK *El;

   if ((El = (ECHOLINK *)Data.First ()) != NULL) {
      EchoTag = El->EchoTag;
      Zone = El->Zone;
      Net = El->Net;
      Node = El->Node;
      Point = El->Point;
      strcpy (Domain, El->Domain);
      SendOnly = El->SendOnly;
      ReceiveOnly = El->ReceiveOnly;
      PersonalOnly = El->PersonalOnly;
      Passive = El->Passive;
      Skip = El->Skip;
      if (Point == 0)
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      else
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      if (Domain[0] != '\0') {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
      if (Skip4D == TRUE && Point != 0)
         ShortAddress[0] = '\0';
      else
         strcpy (ShortAddress, Address);
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TFilechoLink::Load (PSZ pszEchoTag)
{
   int fd, i, Count;
   CHAR Temp[64];
   ULONG Crc;
   ECHOLINK *Buffer;

   Data.Clear ();
   strcpy (Temp, pszEchoTag);
   strupr (Temp);
   Crc = StringCrc32 (Temp, 0xFFFFFFFFL);
   EchoTag = Crc;

   if ((fd = sopen (DataFile, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if ((Buffer = (ECHOLINK *)malloc (sizeof (ECHOLINK) * ECHOLINK_INDEX)) != NULL) {
         do {
            Count = read (fd, Buffer, sizeof (ECHOLINK) * ECHOLINK_INDEX) / sizeof (ECHOLINK);
            for (i = 0; i < Count; i++) {
               if (Buffer[i].Free == FALSE && Buffer[i].EchoTag == Crc)
                  Data.Add (&Buffer[i], sizeof (ECHOLINK));
            }
         } while (Count == ECHOLINK_INDEX);
         free (Buffer);
      }
      close (fd);
   }

   First ();
}

VOID TFilechoLink::New (VOID)
{
   Zone = 0;
   Net = 0;
   Node = 0;
   Point = 0;
   Address[0] = Domain[0] = '\0';
   SendOnly = FALSE;
   ReceiveOnly = FALSE;
   PersonalOnly = FALSE;
   Passive = FALSE;
   Skip = FALSE;
}

USHORT TFilechoLink::Next (VOID)
{
   USHORT RetVal = FALSE;
   ECHOLINK *El;

   if ((El = (ECHOLINK *)Data.Next ()) != NULL) {
      if (Skip4D == FALSE || El->Point == 0) {
         if (Zone != El->Zone) {
            if (El->Point == 0)
               sprintf (ShortAddress, "%u:%u/%u", El->Zone, El->Net, El->Node);
            else
               sprintf (ShortAddress, "%u:%u/%u.%u", El->Zone, El->Net, El->Node, El->Point);
         }
         else if (Net != El->Net) {
            if (El->Point == 0)
               sprintf (ShortAddress, "%u/%u", El->Net, El->Node);
            else
               sprintf (ShortAddress, "%u/%u.%u", El->Net, El->Node, El->Point);
         }
         else if (Node != El->Node) {
            if (El->Point == 0)
               sprintf (ShortAddress, "%u", El->Node);
            else
               sprintf (ShortAddress, "%u.%u", El->Node, El->Point);
         }
         else
            sprintf (ShortAddress, ".%u", El->Point);
      }
      else
         ShortAddress[0] = '\0';

      EchoTag = El->EchoTag;
      Zone = El->Zone;
      Net = El->Net;
      Node = El->Node;
      Point = El->Point;
      strcpy (Domain, El->Domain);
      SendOnly = El->SendOnly;
      ReceiveOnly = El->ReceiveOnly;
      PersonalOnly = El->PersonalOnly;
      Passive = El->Passive;
      Skip = El->Skip;
      if (Point == 0)
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      else
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      if (Domain[0] != '\0') {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TFilechoLink::Previous (VOID)
{
   USHORT RetVal = FALSE;
   ECHOLINK *El;

   if ((El = (ECHOLINK *)Data.Previous ()) != NULL) {
      EchoTag = El->EchoTag;
      Zone = El->Zone;
      Net = El->Net;
      Node = El->Node;
      Point = El->Point;
      strcpy (Domain, El->Domain);
      SendOnly = El->SendOnly;
      ReceiveOnly = El->ReceiveOnly;
      PersonalOnly = El->PersonalOnly;
      Passive = El->Passive;
      Skip = El->Skip;
      if (Point == 0)
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      else
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      if (Domain[0] != '\0') {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TFilechoLink::Save (VOID)
{
   int fd, i, Count, Changed;
   ULONG Position;
   ECHOLINK *Buffer, *Record;

   if ((fd = sopen (DataFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      Record = (ECHOLINK *)Data.First ();

      if ((Buffer = (ECHOLINK *)malloc (sizeof (ECHOLINK) * ECHOLINK_INDEX)) != NULL) {
         do {
            Changed = FALSE;

            Position = tell (fd);
            Count = read (fd, Buffer, sizeof (ECHOLINK) * ECHOLINK_INDEX) / sizeof (ECHOLINK);
            for (i = 0; i < Count && Record != NULL; i++) {
               if (Buffer[i].EchoTag == Record->EchoTag || Buffer[i].Free == TRUE) {
                  memcpy (&Buffer[i], Record, sizeof (ECHOLINK));
                  Record = (ECHOLINK *)Data.Next ();
                  Changed = TRUE;
               }
            }

            for (; i < Count; i++) {
               if (Buffer[i].EchoTag == EchoTag) {
                  memset (&Buffer[i], 0, sizeof (ECHOLINK));
                  Buffer[i].Free = TRUE;
                  Changed = TRUE;
               }
            }

            if (Changed == TRUE) {
               lseek (fd, Position, SEEK_SET);
               write (fd, Buffer, sizeof (ECHOLINK) * Count);
            }
         } while (Count == ECHOLINK_INDEX);
         free (Buffer);
      }

      if (Record != NULL) {
         do {
            write (fd, Record, sizeof (ECHOLINK));
         } while ((Record = (ECHOLINK *)Data.Next ()) != NULL);
      }

      close (fd);
   }
}

VOID TFilechoLink::Update (VOID)
{
   ECHOLINK *Buffer;

   if ((Buffer = (ECHOLINK *)Data.Value ()) != NULL) {
      Buffer->EchoTag = EchoTag;
      Buffer->Zone = Zone;
      Buffer->Net = Net;
      Buffer->Node = Node;
      Buffer->Point = Point;
      strcpy (Buffer->Domain, Domain);
      Buffer->SendOnly = SendOnly;
      Buffer->ReceiveOnly = ReceiveOnly;
      Buffer->PersonalOnly = PersonalOnly;
      Buffer->Passive = Passive;
      Buffer->Skip = Skip;
      if (Point == 0)
         sprintf (Address, "%u:%u/%u", Zone, Net, Node);
      else
         sprintf (Address, "%u:%u/%u.%u", Zone, Net, Node, Point);
      if (Domain[0] != '\0') {
         strcat (Address, "@");
         strcat (Address, Domain);
      }
   }
}

