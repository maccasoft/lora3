
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "files.h"

FileData::FileData (void)
{
   fdDat = -1;
   fdIdx = -1;
   IsValid = FALSE;
   Deleted = FALSE;
   Forward = new TCollection;
}

FileData::FileData (PSZ pszDataPath)
{
   fdDat = -1;
   fdIdx = -1;
   IsValid = FALSE;
   Deleted = FALSE;
   Forward = new TCollection;
   strcpy (DataPath, pszDataPath);
   if (DataPath[0] != '\0' && DataPath[strlen (DataPath) - 1] != '\\')
      strcat (DataPath, "\\");
}

FileData::~FileData (void)
{
   if (fdDat != -1)
      close (fdDat);
   if (fdIdx != -1)
      close (fdIdx);
}

USHORT FileData::Add (VOID)
{
   USHORT retVal = FALSE, Size, Fwd;
   CHAR FileName[128];
   FILES *File;
   INDEX Idx;

   if (fdIdx == -1) {
      sprintf (FileName, "%sFile.Idx", DataPath);
      fdIdx = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }
   if (fdDat == -1) {
      sprintf (FileName, "%sFile.Dat", DataPath);
      fdDat = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdDat, 0L, SEEK_END);
      lseek (fdIdx, 0L, SEEK_END);

      Fwd = 0;
      if (Forward->First () != NULL)
         do {
            Fwd++;
         } while (Forward->Next () != NULL);
      Size = sizeof (FILES);
      if (Fwd > 1)
         Size += sizeof (ADDR) * (Fwd - 1);

      if ((File = (FILES *)malloc (Size)) != NULL) {
         memset (File, 0, Size);
         File->Size = Size;
         File->Forwards = Fwd;
         Class2Struct (File);

         memset (&Idx, 0, sizeof (Idx));
         strcpy (Idx.Key, Key);
         Idx.Level = Level;
         Idx.AccessFlags = AccessFlags;
         Idx.DenyFlags = DenyFlags;
         Idx.Position = tell (fdDat);
         Idx.Size = Size;

         write (fdDat, File, Size);
         write (fdIdx, &Idx, sizeof (Idx));
         retVal = TRUE;

         free (File);
      }
   }

   return (retVal);
}

VOID FileData::Class2Struct (FILES *File)
{
   USHORT Fwd;

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
   memcpy (&File->Feeder, &Feeder, sizeof (Feeder));

   Fwd = 0;
   if (Forward->First () != NULL)
      do {
         memcpy (&File->Forward[Fwd], Forward->Value (), sizeof (ADDR));
         Fwd++;
      } while (Forward->Next () != NULL);
}

VOID FileData::Delete (VOID)
{
   INDEX Idx;

   if (fdDat != -1 && fdIdx != -1) {
      if (tell (fdIdx) >= sizeof (INDEX)) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         Idx.Flags |= IDX_DELETED;
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         write (fdIdx, &Idx, sizeof (Idx));
      }
   }
}

USHORT FileData::First (VOID)
{
   USHORT retVal = FALSE;
   CHAR FileName[128];

   if (fdIdx == -1) {
      sprintf (FileName, "%sFile.Idx", DataPath);
      fdIdx = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }
   if (fdDat == -1) {
      sprintf (FileName, "%sFile.Dat", DataPath);
      fdDat = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);
      retVal = Next ();
   }

   return (retVal);
}

USHORT FileData::IsNext (VOID)
{
   USHORT retVal = FALSE;
   ULONG ulPosIdx;
   INDEX Idx;

   ulPosIdx = tell (fdIdx);

   while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
      if (!(Idx.Flags & IDX_DELETED))
         retVal = TRUE;
   }

   lseek (fdIdx, ulPosIdx, SEEK_SET);

   return (retVal);
}

USHORT FileData::IsPrevious (VOID)
{
   USHORT retVal = FALSE;
   ULONG ulPosIdx;
   INDEX Idx;

   ulPosIdx = tell (fdIdx);

   while (retVal == FALSE && tell (fdIdx) >= sizeof (Idx) * 2) {
      lseek (fdIdx, tell (fdIdx) - sizeof (Idx) * 2, SEEK_SET);
      read (fdIdx, &Idx, sizeof (Idx));
      if (!(Idx.Flags & IDX_DELETED))
         retVal = TRUE;
   }

   lseek (fdIdx, ulPosIdx, SEEK_SET);

   return (retVal);
}

USHORT FileData::Next (VOID)
{
   USHORT retVal = FALSE;
   FILES *File;
   INDEX Idx;

   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((File = (FILES *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, File, Idx.Size) == Idx.Size) {
                  retVal = TRUE;
                  Reset ();
                  Struct2Class (File);
                  IsValid = TRUE;
               }

               free (File);
            }
         }
      }
   }

   return (retVal);
}

USHORT FileData::Last (VOID)
{
   USHORT retVal = FALSE;
   FILES *File;
   INDEX Idx;

   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_END);
      if (tell (fdIdx) >= sizeof (INDEX)) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((File = (FILES *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, File, Idx.Size) == Idx.Size) {
                  retVal = TRUE;
                  Reset ();
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
            if ((File = (FILES *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, File, Idx.Size) == Idx.Size) {
                  retVal = TRUE;
                  Reset ();
                  Struct2Class (File);
                  IsValid = TRUE;
               }

               free (File);
            }
         }
      }
   }

   return (retVal);
}

VOID FileData::Pack (VOID)
{
   int fdNewIdx, fdNewDat;
   CHAR FileName[128], New[128];
   USHORT MaxSize;
   INDEX Idx;
   FILES *File;

   if (fdIdx == -1) {
      sprintf (FileName, "%sFile.Idx", DataPath);
      fdIdx = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }
   if (fdDat == -1) {
      sprintf (FileName, "%sFile.Dat", DataPath);
      fdDat = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fdIdx != -1 && fdDat != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      MaxSize = 0;
      while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && MaxSize < Idx.Size)
            MaxSize = Idx.Size;
      }
      lseek (fdIdx, 0L, SEEK_SET);

      if (MaxSize != 0 && (File = (FILES *)malloc (MaxSize)) != NULL) {
         sprintf (FileName, "%sFile-New.Idx", DataPath);
         fdNewIdx = open (FileName, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
         sprintf (FileName, "%sFile-New.Dat", DataPath);
         fdNewDat = open (FileName, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

         if (fdNewIdx != -1 && fdNewDat != -1) {
            while (read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
               if (!(Idx.Flags & IDX_DELETED)) {
                  lseek (fdDat, Idx.Position, SEEK_SET);
                  read (fdDat, File, Idx.Size);
                  Idx.Position = tell (fdNewDat);
                  write (fdNewDat, File, Idx.Size);
                  write (fdNewIdx, &Idx, sizeof (Idx));
               }
            }

            close (fdNewIdx);
            close (fdNewDat);

            sprintf (FileName, "%sMsg.Dat", DataPath);
            unlink (FileName);
            sprintf (New, "%sMsg-New.Dat", DataPath);
            rename (New, FileName);
            sprintf (FileName, "%sMsg.Idx", DataPath);
            unlink (FileName);
            sprintf (New, "%sMsg-New.Idx", DataPath);
            rename (New, FileName);
         }

         sprintf (FileName, "%sMsg-New.Dat", DataPath);
         unlink (FileName);
         sprintf (FileName, "%sMsg-New.Idx", DataPath);
         unlink (FileName);
         free (FileName);
      }

      close (fdIdx);
      close (fdDat);
      fdIdx = fdDat = -1;
   }
}

USHORT FileData::Previous (VOID)
{
   USHORT retVal = FALSE;
   FILES *File;
   INDEX Idx;

   IsValid = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      while (IsValid == FALSE && tell (fdIdx) >= sizeof (INDEX) * 2) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX) * 2, SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));
         if (!(Idx.Flags & IDX_DELETED)) {
            lseek (fdDat, Idx.Position, SEEK_SET);
            if ((File = (FILES *)malloc (Idx.Size)) != NULL) {
               if (read (fdDat, File, Idx.Size) == Idx.Size) {
                  retVal = TRUE;
                  Reset ();
                  Struct2Class (File);
                  IsValid = TRUE;
               }

               free (File);
            }
         }
      }
   }

   return (retVal);
}

USHORT FileData::Read (PSZ pszName, USHORT fCloseFile)
{
   USHORT retVal = FALSE;
   CHAR FileName[128];
   FILES *File;
   INDEX Idx;

   Reset ();
   IsValid = TRUE;

   if (fdIdx == -1) {
      sprintf (FileName, "%sFile.Idx", DataPath);
      fdIdx = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }
   if (fdDat == -1) {
      sprintf (FileName, "%sFile.Dat", DataPath);
      fdDat = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      lseek (fdDat, 0L, SEEK_SET);

      while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
         if (!(Idx.Flags & IDX_DELETED) && !stricmp (pszName, Idx.Key)) {
            if ((File = (FILES *)malloc (Idx.Size)) != NULL) {
               lseek (fdDat, Idx.Position, SEEK_SET);
               read (fdDat, File, Idx.Size);

               retVal = TRUE;
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

   return (retVal);
}

USHORT FileData::ReRead (VOID)
{
   USHORT retVal = FALSE;

   if (fdDat != -1 && fdIdx != -1) {
      if (tell (fdIdx) >= sizeof (INDEX)) {
         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         retVal = Next ();
      }
   }

   return (retVal);
}

VOID FileData::Reset (VOID)
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
   ShowGlobal = FALSE;
   memset (MenuName, 0, sizeof (MenuName));
   memset (Moderator, 0, sizeof (Moderator));
   Cost = 0L;
   ActiveFiles = UnapprovedFiles = 0L;
   memset (EchoTag, 0, sizeof (EchoTag));
   memset (&Feeder, 0, sizeof (Feeder));
   Forward->Clear ();

   IsValid = FALSE;
   Deleted = FALSE;
}

VOID FileData::Struct2Class (FILES *File)
{
   USHORT i;
   ADDR Addr;

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
   strcpy (Upload, File->Upload);
   CdRom = File->CdRom;
   FreeDownload = File->FreeDownload;
   ShowGlobal = File->ShowGlobal;
   strcpy (MenuName, File->MenuName);
   strcpy (Moderator, File->Moderator);
   Cost = File->Cost;
   ActiveFiles = File->ActiveFiles;
   UnapprovedFiles = File->UnapprovedFiles;
   strcpy (EchoTag, File->EchoTag);
   memcpy (&Feeder, &File->Feeder, sizeof (Feeder));

   Forward->Clear ();
   for (i = 0; i < File->Forwards; i++)
      Forward->Add (&File->Forward[i], sizeof (Addr));
}

USHORT FileData::Update (VOID)
{
   USHORT retVal = FALSE, Size, Fwd, DoClose = FALSE;
   CHAR FileName[128];
   FILES *File;
   INDEX Idx;

   if (fdIdx == -1) {
      sprintf (FileName, "%sFile.Idx", DataPath);
      fdIdx = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }
   if (fdDat == -1) {
      sprintf (FileName, "%sFile.Dat", DataPath);
      fdDat = sopen (FileName, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1 && fdIdx != -1) {
      if (DoClose == TRUE) {
         lseek (fdIdx, 0L, SEEK_SET);
         while (retVal == FALSE && read (fdIdx, &Idx, sizeof (Idx)) == sizeof (Idx)) {
            if (!(Idx.Flags & IDX_DELETED) && !stricmp (Key, Idx.Key))
               retVal = TRUE;
         }
      }
      else
         retVal = TRUE;

      if (retVal == TRUE && tell (fdIdx) >= sizeof (INDEX)) {
         retVal = FALSE;

         lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
         read (fdIdx, &Idx, sizeof (Idx));

         Fwd = 0;
         if (Forward->First () != NULL)
            do {
               Fwd++;
            } while (Forward->Next () != NULL);
         Size = sizeof (FILES);
         if (Fwd > 1)
            Size += sizeof (ADDR) * (Fwd - 1);

         if ((File = (FILES *)malloc (Size)) != NULL) {
            memset (File, 0, Size);
            File->Size = Size;
            File->Forwards = Fwd;
            Class2Struct (File);

            if (Idx.Size < Size)
               Idx.Position = filelength (fdDat);

            Idx.Size = Size;
            strcpy (Idx.Key, Key);
            if (Deleted == TRUE)
               Idx.Flags |= IDX_DELETED;
            else
               Idx.Flags &= ~IDX_DELETED;
            lseek (fdIdx, tell (fdIdx) - sizeof (INDEX), SEEK_SET);
            write (fdIdx, &Idx, sizeof (Idx));

            lseek (fdDat, Idx.Position, SEEK_SET);
            write (fdDat, File, Idx.Size);
            free (File);

            retVal = TRUE;
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

   return (retVal);
}

