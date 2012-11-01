
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

#define MAX_INDEX       256

typedef struct {
   CHAR  Name[32];
   ULONG Date;
   ULONG Download;
   ULONG Position;
   ULONG IdxPosition;
} NAMESORT;

TFileBase::TFileBase (void)
{
   fdIdx = fdDat = -1;
   fUploader = FALSE;
   Description = new TCollection;
   Clear ();
   List = NULL;
}

TFileBase::TFileBase (PSZ pszPath, PSZ pszArea)
{
   fdIdx = fdDat = -1;
   fUploader = FALSE;
   Description = new TCollection;
   Clear ();
   Open (pszPath, pszArea);
   List = NULL;
}

TFileBase::~TFileBase (void)
{
   Clear ();
   Close ();
   if (Description != NULL)
      delete Description;
   if (List != NULL) {
      List->Clear ();
      delete List;
   }
}

USHORT TFileBase::Add (VOID)
{
   PSZ pszTemp;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm ftm;

   fUploader = FALSE;

   memset (&fileIndex, 0, sizeof (fileIndex));
   fileIndex.Area = StringCrc32 (Area, 0xFFFFFFFFL);
   strcpy (fileIndex.Name, Name);
   memset (&ftm, 0, sizeof (ftm));
   ftm.tm_min = UplDate.Minute;
   ftm.tm_hour = UplDate.Hour;
   ftm.tm_mday = UplDate.Day;
   ftm.tm_mon = UplDate.Month - 1;
   ftm.tm_year = UplDate.Year - 1900;
   fileIndex.UploadDate = mktime (&ftm);
   lseek (fdDat, 0L, SEEK_END);
   fileIndex.Offset = tell (fdDat);
   if (Unapproved == TRUE)
      fileIndex.Flags |= FILE_UNAPPROVED;

   lseek (fdIdx, 0L, SEEK_END);
   write (fdIdx, &fileIndex, sizeof (fileIndex));

   memset (&fileData, 0, sizeof (fileData));
   fileData.Id = FILEBASE_ID;
   strcpy (fileData.Area, Area);
   strcpy (fileData.Name, Name);
   strcpy (fileData.Complete, Complete);
   strcpy (fileData.Keyword, Keyword);
   fileData.Size = Size;
   fileData.DlTimes = DlTimes;
   memset (&ftm, 0, sizeof (ftm));
   ftm.tm_min = Date.Minute;
   ftm.tm_hour = Date.Hour;
   ftm.tm_mday = Date.Day;
   ftm.tm_mon = Date.Month - 1;
   ftm.tm_year = Date.Year - 1900;
   FileDate = fileData.FileDate = mktime (&ftm);
   memset (&ftm, 0, sizeof (ftm));
   ftm.tm_min = UplDate.Minute;
   ftm.tm_hour = UplDate.Hour;
   ftm.tm_mday = UplDate.Day;
   ftm.tm_mon = UplDate.Month - 1;
   ftm.tm_year = UplDate.Year - 1900;
   UploadDate = fileData.UploadDate = mktime (&ftm);
   fileData.Cost = Cost;
   fileData.Password = Password;
   fileData.Level = Level;
   fileData.AccessFlags = AccessFlags;
   fileData.DenyFlags = DenyFlags;
   if (Unapproved == TRUE)
      fileData.Flags |= FILE_UNAPPROVED;
   if (CdRom == TRUE)
      fileData.Flags |= FILE_CDROM;
   if ((pszTemp = (PSZ)Description->First ()) != NULL) {
      do {
         fileData.Description += strlen (pszTemp) + 2;
      } while ((pszTemp = (PSZ)Description->Next ()) != NULL);
   }
   if (Uploader != NULL)
     fileData.Uploader = (USHORT)(strlen (Uploader) + 1);

   lseek (fdDat, 0L, SEEK_END);
   write (fdDat, &fileData, sizeof (fileData));

   if ((pszTemp = (PSZ)Description->First ()) != NULL) {
      do {
         write (fdDat, pszTemp, strlen (pszTemp));
         write (fdDat, "\r\n", 2);
      } while ((pszTemp = (PSZ)Description->Next ()) != NULL);
   }

   if (Uploader != NULL)
     write (fdDat, Uploader, fileData.Uploader);

   return (TRUE);
}

ULONG TFileBase::ChangeLibrary (PSZ pszFrom, PSZ pszTo)
{
   ULONG CrcFrom, Number;
   FILEDATA fileData;
   FILEINDEX fileIndex;

   CrcFrom = StringCrc32 (pszFrom, 0xFFFFFFFFL);
   Number = 0L;

   lseek (fdIdx, 0L, SEEK_SET);

   while (read (fdIdx, &fileIndex, sizeof (fileIndex)) == sizeof (fileIndex)) {
      if (fileIndex.Flags & FILE_DELETED)
         continue;
      if (fileIndex.Area == CrcFrom) {
         lseek (fdDat, fileIndex.Offset, SEEK_SET);
         read (fdDat, &fileData, sizeof (fileData));
         strcpy (fileData.Area, pszTo);
         lseek (fdDat, fileIndex.Offset, SEEK_SET);
         write (fdDat, &fileData, sizeof (fileData));

         fileIndex.Area = StringCrc32 (pszTo, 0xFFFFFFFFL);
         lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex), SEEK_SET);
         write (fdIdx, &fileIndex, sizeof (fileIndex));

         Number++;
      }
   }

   return (Number);
}

VOID TFileBase::Clear (VOID)
{
   memset (Area, 0, sizeof (Area));
   memset (Name, 0, sizeof (Name));
   memset (Complete, 0, sizeof (Complete));
   memset (Keyword, 0, sizeof (Keyword));
   memset (&Date, 0, sizeof (Date));
   memset (&UplDate, 0, sizeof (UplDate));
   Description->Clear ();
   if (fUploader == TRUE)
      free (pszMemUploader);
   fUploader = FALSE;
   Uploader = NULL;
   pszMemUploader = NULL;
   Size = 0;
   DlTimes = 0;
   Cost = 0;
   Password = 0;
   Level = 0;
   AccessFlags = DenyFlags = 0;
   FileDate = UploadDate = 0L;
   Unapproved = FALSE;
   CdRom = FALSE;
}

VOID TFileBase::Close (VOID)
{
   if (fdIdx != -1)
      close (fdIdx);
   if (fdDat != -1)
      close (fdDat);
   fdIdx = fdDat = -1;
}

VOID TFileBase::Delete (VOID)
{
   FILEDATA fileData;
   FILEINDEX fileIndex;

   if (tell (fdIdx) > 0) {
      lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex), SEEK_SET);
      read (fdIdx, &fileIndex, sizeof (fileIndex));
      fileIndex.Flags |= FILE_DELETED;
      lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex), SEEK_SET);
      write (fdIdx, &fileIndex, sizeof (fileIndex));

      lseek (fdDat, fileIndex.Offset, SEEK_SET);
      read (fdDat, &fileData, sizeof (fileData));
      fileData.Flags = fileIndex.Flags;
      lseek (fdDat, fileIndex.Offset, SEEK_SET);
      write (fdDat, &fileData, sizeof (fileData));

      if (List != NULL) {
         List->Clear ();
         delete List;
         List = NULL;
      }
   }
}

USHORT TFileBase::First (PSZ pszSearch)
{
   USHORT RetVal = FALSE, i, r, w;
   CHAR szTemp[80], szLine[80];
   NAMESORT *ns;
   FILEDATA fileData;
   struct tm *ftm;

   if (List == NULL) {
      lseek (fdIdx, 0L, SEEK_SET);
      RetVal = Next (pszSearch);
   }
   else if ((ns = (NAMESORT *)List->First ()) != NULL) {
      lseek (fdDat, ns->Position, SEEK_SET);
      read (fdDat, &fileData, sizeof (fileData));

      if (fileData.Id == FILEBASE_ID) {
         Clear ();
         fileData.Area[sizeof (fileData.Area) - 1] = '\0';
         fileData.Name[sizeof (fileData.Name) - 1] = '\0';
         fileData.Complete[sizeof (fileData.Complete) - 1] = '\0';
         fileData.Keyword[sizeof (fileData.Keyword) - 1] = '\0';

         strcpy (Area, fileData.Area);
         strcpy (Name, fileData.Name);
         strcpy (Complete, fileData.Complete);
         strcpy (Keyword, fileData.Keyword);
         if (fileData.Description != 0) {
            w = 0;
            do {
               if ((r = (USHORT)sizeof (szTemp)) > fileData.Description)
                  r = fileData.Description;
               r = (USHORT)read (fdDat, szTemp, r);
               for (i = 0; i < r; i++) {
                  if (szTemp[i] == '\r') {
                     szLine[w++] = '\0';
                     Description->Add (szLine, w);
                     w = 0;
                  }
                  else if (szTemp[i] != '\n')
                     szLine[w++] = szTemp[i];
               }
               fileData.Description -= r;
            } while (fileData.Description > 0);
            if (w > 0) {
               szLine[w++] = '\0';
               Description->Add (szLine, w);
            }
         }
         if (fileData.Uploader != 0) {
            fUploader = TRUE;
            pszMemUploader = Uploader = (PSZ)malloc (fileData.Uploader);
            read (fdDat, Uploader, fileData.Uploader);
         }
         Size = fileData.Size;
         DlTimes = fileData.DlTimes;
         FileDate = fileData.FileDate;
         ftm = localtime ((time_t *)&fileData.FileDate);
         Date.Day = (UCHAR)ftm->tm_mday;
         Date.Month = (UCHAR)(ftm->tm_mon + 1);
         Date.Year = (USHORT)(ftm->tm_year + 1900);
         Date.Hour = (UCHAR)ftm->tm_hour;
         Date.Minute = (UCHAR)ftm->tm_min;
         UploadDate = fileData.UploadDate;
         ftm = localtime ((time_t *)&fileData.UploadDate);
         UplDate.Day = (UCHAR)ftm->tm_mday;
         UplDate.Month = (UCHAR)(ftm->tm_mon + 1);
         UplDate.Year = (USHORT)(ftm->tm_year + 1900);
         UplDate.Hour = (UCHAR)ftm->tm_hour;
         UplDate.Minute = (UCHAR)ftm->tm_min;
         Cost = fileData.Cost;
         Password = fileData.Password;
         Level = fileData.Level;
         AccessFlags = fileData.AccessFlags;
         DenyFlags = fileData.DenyFlags;
         Unapproved = (fileData.Flags & FILE_UNAPPROVED) ? TRUE : FALSE;
         CdRom = (fileData.Flags & FILE_CDROM) ? TRUE : FALSE;
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

USHORT TFileBase::MatchName (PSZ pszName, PSZ pszSearch)
{
   USHORT Match = TRUE;

   while (*pszName != '\0' && *pszSearch != '\0' && Match == TRUE) {
      if (*pszSearch == '*') {
         pszSearch++;
         while (*pszName != '\0' && toupper (*pszName) != toupper (*pszSearch))
            pszName++;
         if (toupper (*pszName) != toupper (*pszSearch))
            Match = FALSE;
      }
      else if (*pszSearch != '?' && toupper (*pszName) != toupper (*pszSearch))
         Match = FALSE;
      else {
         pszSearch++;
         pszName++;
      }
   }

   return (Match);
}

USHORT TFileBase::Next (PSZ pszSearch)
{
   USHORT fRet = FALSE, i, r, w;
   CHAR szTemp[80], szLine[80];
   ULONG ulCrc;
   NAMESORT *ns;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm *ftm;

   if (List == NULL && fdDat != -1) {
      ulCrc = StringCrc32 (szArea, 0xFFFFFFFFL);

      while (read (fdIdx, &fileIndex, sizeof (fileIndex)) == sizeof (fileIndex)) {
         if (fileIndex.Flags & FILE_DELETED)
            continue;
         if (szArea[0] == '\0' || fileIndex.Area == ulCrc) {
            fRet = TRUE;
            if (pszSearch != NULL)
               fRet = MatchName (fileIndex.Name, pszSearch);
            if (fRet == TRUE) {
               lseek (fdDat, fileIndex.Offset, SEEK_SET);
               break;
            }
         }
      }
   }
   else if (List != NULL && (ns = (NAMESORT *)List->Next ()) != NULL) {
      lseek (fdDat, ns->Position, SEEK_SET);
      fRet = TRUE;
   }

   if (fRet == TRUE) {
      read (fdDat, &fileData, sizeof (fileData));

      fRet = FALSE;
      if (fileData.Id == FILEBASE_ID) {
         Clear ();
         fileData.Area[sizeof (fileData.Area) - 1] = '\0';
         fileData.Name[sizeof (fileData.Name) - 1] = '\0';
         fileData.Complete[sizeof (fileData.Complete) - 1] = '\0';
         fileData.Keyword[sizeof (fileData.Keyword) - 1] = '\0';

         strcpy (Area, fileData.Area);
         strcpy (Name, fileData.Name);
         strcpy (Complete, fileData.Complete);
         strcpy (Keyword, fileData.Keyword);
         if (fileData.Description != 0) {
            w = 0;
            do {
               if ((r = (USHORT)sizeof (szTemp)) > fileData.Description)
                  r = fileData.Description;
               r = (USHORT)read (fdDat, szTemp, r);
               for (i = 0; i < r; i++) {
                  if (szTemp[i] == '\r') {
                     szLine[w++] = '\0';
                     Description->Add (szLine, w);
                     w = 0;
                  }
                  else if (szTemp[i] != '\n')
                     szLine[w++] = szTemp[i];
               }
               fileData.Description -= r;
            } while (fileData.Description > 0);
            if (w > 0) {
               szLine[w++] = '\0';
               Description->Add (szLine, w);
            }
         }
         if (fileData.Uploader != 0) {
            fUploader = TRUE;
            pszMemUploader = Uploader = (PSZ)malloc (fileData.Uploader);
            read (fdDat, Uploader, fileData.Uploader);
         }
         Size = fileData.Size;
         DlTimes = fileData.DlTimes;
         FileDate = fileData.FileDate;
         ftm = localtime ((time_t *)&fileData.FileDate);
         Date.Day = (UCHAR)ftm->tm_mday;
         Date.Month = (UCHAR)(ftm->tm_mon + 1);
         Date.Year = (USHORT)(ftm->tm_year + 1900);
         Date.Hour = (UCHAR)ftm->tm_hour;
         Date.Minute = (UCHAR)ftm->tm_min;
         UploadDate = fileData.UploadDate;
         ftm = localtime ((time_t *)&fileData.UploadDate);
         UplDate.Day = (UCHAR)ftm->tm_mday;
         UplDate.Month = (UCHAR)(ftm->tm_mon + 1);
         UplDate.Year = (USHORT)(ftm->tm_year + 1900);
         UplDate.Hour = (UCHAR)ftm->tm_hour;
         UplDate.Minute = (UCHAR)ftm->tm_min;
         Cost = fileData.Cost;
         Password = fileData.Password;
         Level = fileData.Level;
         AccessFlags = fileData.AccessFlags;
         DenyFlags = fileData.DenyFlags;
         Unapproved = (fileData.Flags & FILE_UNAPPROVED) ? TRUE : FALSE;
         CdRom = (fileData.Flags & FILE_CDROM) ? TRUE : FALSE;
         fRet = TRUE;
      }
   }

   return (fRet);
}

USHORT TFileBase::Open (PSZ pszDataPath, PSZ pszArea)
{
   CHAR szFile[128];

   strcpy (DataPath, pszDataPath);
   if (DataPath[0] != '\0') {
#if defined(__LINUX__)
      if (DataPath[strlen (DataPath) - 1] != '/')
         strcat (DataPath, "/");
#else
      if (DataPath[strlen (DataPath) - 1] != '\\')
         strcat (DataPath, "\\");
#endif
   }

   sprintf (szFile, "%s%s", DataPath, "filebase.idx");
   if ((fdIdx = sopen (AdjustPath (szFile), O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) == -1)
      return (FALSE);

   sprintf (szFile, "%s%s", DataPath, "filebase.dat");
   if ((fdDat = sopen (AdjustPath (szFile), O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) == -1) {
      close (fdDat);
      fdDat = -1;
      return (FALSE);
   }

   strcpy (szArea, pszArea);

   return (TRUE);
}

VOID TFileBase::Pack (VOID)
{
   int fdNdx, fdNdat, Readed;
   CHAR File[128], NewFile[128], *Buffer;
   FILEDATA fileData;
   FILEINDEX fileIndex;

   sprintf (File, "%s%s", DataPath, "filebase.$dx");
   fdNdx = sopen (AdjustPath (File), O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);
   sprintf (File, "%s%s", DataPath, "filebase.$at");
   fdNdat = sopen (AdjustPath (File), O_RDWR|O_BINARY|O_CREAT|O_TRUNC, SH_DENYNO, S_IREAD|S_IWRITE);
   Buffer = (CHAR *)malloc (2048);

   if (fdIdx != -1 && fdDat != -1 && fdNdx != -1 && fdNdat != -1 && Buffer != NULL) {
      while (read (fdIdx, &fileIndex, sizeof (FILEINDEX)) == sizeof (FILEINDEX)) {
         if (!(fileIndex.Flags & FILE_DELETED)) {
            lseek (fdDat, fileIndex.Offset, SEEK_SET);

            fileIndex.Offset = tell (fdNdat);
            write (fdNdx, &fileIndex, sizeof (FILEINDEX));

            read (fdDat, &fileData, sizeof (FILEDATA));
            write (fdNdat, &fileData, sizeof (FILEDATA));

            while (fileData.Description > 0) {
               Readed = read (fdDat, Buffer, 2048);
               write (fdNdat, Buffer, Readed);
               fileData.Description -= (USHORT)Readed;
            }
            while (fileData.Uploader > 0) {
               Readed = read (fdDat, Buffer, 2048);
               write (fdNdat, Buffer, Readed);
               fileData.Uploader -= (USHORT)Readed;
            }
         }
      }

      close (fdNdat);
      close (fdDat);
      fdNdat = -1;
      sprintf (File, "%s%s", DataPath, "filebase.$at");
      sprintf (NewFile, "%s%s", DataPath, "filebase.dat");
      unlink (NewFile);
      rename (File, NewFile);
      fdDat = sopen (NewFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);

      close (fdNdx);
      close (fdIdx);
      fdNdx = -1;
      sprintf (File, "%s%s", DataPath, "filebase.$dx");
      sprintf (NewFile, "%s%s", DataPath, "filebase.idx");
      unlink (NewFile);
      rename (File, NewFile);
      fdIdx = sopen (NewFile, O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE);
   }

   if (Buffer != NULL)
      delete Buffer;

   if (fdNdat != -1)
      close (fdNdat);
   sprintf (File, "%s%s", DataPath, "filebase.$at");
   unlink (File);

   if (fdNdx != -1)
      close (fdNdx);
   sprintf (File, "%s%s", DataPath, "filebase.$dx");
   unlink (File);
}

USHORT TFileBase::Previous (VOID)
{
   USHORT fRet = FALSE, i, r, w;
   CHAR szTemp[80], szLine[80];
   ULONG ulCrc;
   NAMESORT *ns;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm *ftm;

   if (List == NULL && fdDat != -1) {
      ulCrc = StringCrc32 (szArea, 0xFFFFFFFFL);

      if (fdDat != -1) {
         if (tell (fdIdx) >= sizeof (fileIndex) * 2)
            do {
               lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex) * 2, SEEK_SET);
               read (fdIdx, &fileIndex, sizeof (fileIndex));
               if (fileIndex.Flags & FILE_DELETED)
                  continue;
               if (szArea[0] == '\0' || fileIndex.Area == ulCrc) {
                  fRet = TRUE;
                  lseek (fdDat, fileIndex.Offset, SEEK_SET);
                  break;
               }
            } while (tell (fdIdx) >= sizeof (fileIndex) * 2);
      }
   }
   else if (List != NULL && (ns = (NAMESORT *)List->Previous ()) != NULL) {
      lseek (fdDat, ns->Position, SEEK_SET);
      fRet = TRUE;
   }

   if (fRet == TRUE) {
      read (fdDat, &fileData, sizeof (fileData));

      fRet = FALSE;
      if (fileData.Id == FILEBASE_ID) {
         Clear ();
         fileData.Area[sizeof (fileData.Area) - 1] = '\0';
         fileData.Name[sizeof (fileData.Name) - 1] = '\0';
         fileData.Complete[sizeof (fileData.Complete) - 1] = '\0';
         fileData.Keyword[sizeof (fileData.Keyword) - 1] = '\0';

         strcpy (Area, fileData.Area);
         strcpy (Name, fileData.Name);
         strcpy (Complete, fileData.Complete);
         strcpy (Keyword, fileData.Keyword);
         if (fileData.Description != 0) {
            w = 0;
            do {
               if ((r = (USHORT)sizeof (szTemp)) > fileData.Description)
                  r = fileData.Description;
               r = (USHORT)read (fdDat, szTemp, r);
               for (i = 0; i < r; i++) {
                  if (szTemp[i] == '\r') {
                     szLine[w++] = '\0';
                     Description->Add (szLine, w);
                     w = 0;
                  }
                  else if (szTemp[i] != '\n')
                     szLine[w++] = szTemp[i];
               }
               fileData.Description -= r;
            } while (fileData.Description > 0);
            if (w > 0) {
               szLine[w++] = '\0';
               Description->Add (szLine, w);
            }
         }
         if (fileData.Uploader != 0) {
            fUploader = TRUE;
            pszMemUploader = Uploader = (PSZ)malloc (fileData.Uploader);
            read (fdDat, Uploader, fileData.Uploader);
         }
         Size = fileData.Size;
         DlTimes = fileData.DlTimes;
         FileDate = fileData.FileDate;
         ftm = localtime ((time_t *)&fileData.FileDate);
         Date.Day = (UCHAR)ftm->tm_mday;
         Date.Month = (UCHAR)(ftm->tm_mon + 1);
         Date.Year = (USHORT)(ftm->tm_year + 1900);
         Date.Hour = (UCHAR)ftm->tm_hour;
         Date.Minute = (UCHAR)ftm->tm_min;
         UploadDate = fileData.UploadDate;
         ftm = localtime ((time_t *)&fileData.UploadDate);
         UplDate.Day = (UCHAR)ftm->tm_mday;
         UplDate.Month = (UCHAR)(ftm->tm_mon + 1);
         UplDate.Year = (USHORT)(ftm->tm_year + 1900);
         UplDate.Hour = (UCHAR)ftm->tm_hour;
         UplDate.Minute = (UCHAR)ftm->tm_min;
         Cost = fileData.Cost;
         Password = fileData.Password;
         Level = fileData.Level;
         AccessFlags = fileData.AccessFlags;
         DenyFlags = fileData.DenyFlags;
         Unapproved = (fileData.Flags & FILE_UNAPPROVED) ? TRUE : FALSE;
         CdRom = (fileData.Flags & FILE_CDROM) ? TRUE : FALSE;
         fRet = TRUE;
      }
   }

   return (fRet);
}

USHORT TFileBase::Read (PSZ pszFile)
{
   USHORT fRet = FALSE, i, r, w;
   CHAR szTemp[80], szLine[80];
   ULONG ulCrc;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm *ftm;

   ulCrc = StringCrc32 (szArea, 0xFFFFFFFFL);

   if (fdDat != -1 && fdIdx != -1) {
      lseek (fdIdx, 0L, SEEK_SET);
      while (read (fdIdx, &fileIndex, sizeof (fileIndex)) == sizeof (fileIndex)) {
         if (!(fileIndex.Flags & FILE_DELETED)) {
            if (szArea[0] == '\0' || fileIndex.Area == ulCrc) {
               if (!stricmp (fileIndex.Name, pszFile)) {
                  fRet = TRUE;
                  break;
               }
            }
         }
      }
      if (fRet == TRUE) {
         lseek (fdDat, fileIndex.Offset, SEEK_SET);
         read (fdDat, &fileData, sizeof (fileData));

         fRet = FALSE;
         if (fileData.Id == FILEBASE_ID) {
            Clear ();
            fileData.Area[sizeof (fileData.Area) - 1] = '\0';
            fileData.Name[sizeof (fileData.Name) - 1] = '\0';
            fileData.Complete[sizeof (fileData.Complete) - 1] = '\0';
            fileData.Keyword[sizeof (fileData.Keyword) - 1] = '\0';

            strcpy (Area, fileData.Area);
            strcpy (Name, fileData.Name);
            strcpy (Complete, fileData.Complete);
            strcpy (Keyword, fileData.Keyword);
            if (fileData.Description != 0) {
               w = 0;
               do {
                  if ((r = (USHORT)sizeof (szTemp)) > fileData.Description)
                     r = fileData.Description;
                  r = (USHORT)read (fdDat, szTemp, r);
                  for (i = 0; i < r; i++) {
                     if (szTemp[i] == '\r') {
                        szLine[w++] = '\0';
                        Description->Add (szLine, w);
                        w = 0;
                     }
                     else if (szTemp[i] != '\n')
                        szLine[w++] = szTemp[i];
                  }
                  fileData.Description -= r;
               } while (fileData.Description > 0);
               if (w > 0) {
                  szLine[w++] = '\0';
                  Description->Add (szLine, w);
               }
            }
            if (fileData.Uploader != 0) {
               fUploader = TRUE;
               pszMemUploader = Uploader = (PSZ)malloc (fileData.Uploader);
               read (fdDat, Uploader, fileData.Uploader);
            }
            Size = fileData.Size;
            DlTimes = fileData.DlTimes;
            FileDate = fileData.FileDate;
            ftm = localtime ((time_t *)&fileData.FileDate);
            Date.Day = (UCHAR)ftm->tm_mday;
            Date.Month = (UCHAR)(ftm->tm_mon + 1);
            Date.Year = (USHORT)(ftm->tm_year + 1900);
            Date.Hour = (UCHAR)ftm->tm_hour;
            Date.Minute = (UCHAR)ftm->tm_min;
            UploadDate = fileData.UploadDate;
            ftm = localtime ((time_t *)&fileData.UploadDate);
            UplDate.Day = (UCHAR)ftm->tm_mday;
            UplDate.Month = (UCHAR)(ftm->tm_mon + 1);
            UplDate.Year = (USHORT)(ftm->tm_year + 1900);
            UplDate.Hour = (UCHAR)ftm->tm_hour;
            UplDate.Minute = (UCHAR)ftm->tm_min;
            Cost = fileData.Cost;
            Password = fileData.Password;
            Level = fileData.Level;
            AccessFlags = fileData.AccessFlags;
            DenyFlags = fileData.DenyFlags;
            Unapproved = (fileData.Flags & FILE_UNAPPROVED) ? TRUE : FALSE;
            CdRom = (fileData.Flags & FILE_CDROM) ? TRUE : FALSE;
            fRet = TRUE;
         }
      }
   }

   return (fRet);
}

USHORT TFileBase::Replace (VOID)
{
   PSZ pszTemp;
   USHORT RetVal = FALSE;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm ftm;

   fUploader = FALSE;

   lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex), SEEK_SET);
   read (fdIdx, &fileIndex, sizeof (fileIndex));

   lseek (fdDat, fileIndex.Offset, SEEK_SET);
   read (fdDat, &fileData, sizeof (fileData));

   if (fileData.Id == FILEBASE_ID) {
      fileData.Flags |= FILE_DELETED;
      lseek (fdDat, fileIndex.Offset, SEEK_SET);
      write (fdDat, &fileData, sizeof (fileData));

      lseek (fdDat, 0L, SEEK_END);

      fileIndex.Offset = tell (fdDat);
      memset (&ftm, 0, sizeof (ftm));
      ftm.tm_min = UplDate.Minute;
      ftm.tm_hour = UplDate.Hour;
      ftm.tm_mday = UplDate.Day;
      ftm.tm_mon = UplDate.Month - 1;
      ftm.tm_year = UplDate.Year - 1900;
      fileIndex.UploadDate = mktime (&ftm);
      if (Unapproved == TRUE)
         fileIndex.Flags |= FILE_UNAPPROVED;
      else
         fileIndex.Flags &= ~FILE_UNAPPROVED;
      lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex), SEEK_SET);
      write (fdIdx, &fileIndex, sizeof (fileIndex));

      memset (&fileData, 0, sizeof (fileData));
      strcpy (fileData.Area, Area);
      strcpy (fileData.Name, Name);
      strcpy (fileData.Complete, Complete);
      strcpy (fileData.Keyword, Keyword);
      fileData.Size = Size;
      fileData.DlTimes = DlTimes;
      memset (&ftm, 0, sizeof (ftm));
      ftm.tm_min = Date.Minute;
      ftm.tm_hour = Date.Hour;
      ftm.tm_mday = Date.Day;
      ftm.tm_mon = Date.Month - 1;
      ftm.tm_year = Date.Year - 1900;
      FileDate = fileData.FileDate = mktime (&ftm);
      memset (&ftm, 0, sizeof (ftm));
      ftm.tm_min = UplDate.Minute;
      ftm.tm_hour = UplDate.Hour;
      ftm.tm_mday = UplDate.Day;
      ftm.tm_mon = UplDate.Month - 1;
      ftm.tm_year = UplDate.Year - 1900;
      UploadDate = fileData.UploadDate = mktime (&ftm);
      fileData.Cost = Cost;
      fileData.Password = Password;
      fileData.Level = Level;
      fileData.AccessFlags = AccessFlags;
      fileData.DenyFlags = DenyFlags;
      if (Unapproved == TRUE)
         fileData.Flags |= FILE_UNAPPROVED;
      if (CdRom == TRUE)
         fileData.Flags |= FILE_CDROM;
      if ((pszTemp = (PSZ)Description->First ()) != NULL) {
         do {
            fileData.Description += strlen (pszTemp) + 2;
         } while ((pszTemp = (PSZ)Description->Next ()) != NULL);
      }
      if (Uploader != NULL)
        fileData.Uploader = (USHORT)(strlen (Uploader) + 1);

      lseek (fdDat, 0L, SEEK_END);
      write (fdDat, &fileData, sizeof (fileData));

      if ((pszTemp = (PSZ)Description->First ()) != NULL) {
         do {
            write (fdDat, pszTemp, strlen (pszTemp));
            write (fdDat, "\r\n", 2);
         } while ((pszTemp = (PSZ)Description->Next ()) != NULL);
      }

      if (Uploader != NULL)
        write (fdDat, Uploader, fileData.Uploader);

      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TFileBase::ReplaceHeader (VOID)
{
   USHORT RetVal = FALSE;
   ULONG ulCrc;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   NAMESORT *ns;
   struct tm ftm;

   if (List == NULL) {
      ulCrc = StringCrc32 (szArea, 0xFFFFFFFFL);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while (read (fdIdx, &fileIndex, sizeof (fileIndex)) == sizeof (fileIndex)) {
            if (!(fileIndex.Flags & FILE_DELETED)) {
               if (szArea[0] == '\0' || fileIndex.Area == ulCrc) {
                  if (!stricmp (fileIndex.Name, Name)) {
                     lseek (fdDat, fileIndex.Offset, SEEK_SET);
                     read (fdDat, &fileData, sizeof (fileData));
                     if (fileData.Id == FILEBASE_ID) {
                        RetVal = TRUE;
                        break;
                     }
                  }
               }
            }
         }
      }
   }
   else {
      if ((ns = (NAMESORT *)List->Value ()) != NULL) {
         lseek (fdIdx, ns->IdxPosition, SEEK_SET);
         read (fdIdx, &fileIndex, sizeof (fileIndex));
         lseek (fdDat, fileIndex.Offset, SEEK_SET);
         read (fdDat, &fileData, sizeof (fileData));
         if (fileData.Id == FILEBASE_ID && !strcmp (fileData.Name, Name))
            RetVal = TRUE;
      }

      if (RetVal == FALSE) {
         if ((ns = (NAMESORT *)List->First ()) != NULL)
            do {
               if (!stricmp (ns->Name, Name)) {
                  lseek (fdIdx, ns->IdxPosition, SEEK_SET);
                  read (fdIdx, &fileIndex, sizeof (fileIndex));
                  lseek (fdDat, fileIndex.Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));
                  if (fileData.Id == FILEBASE_ID) {
                     RetVal = TRUE;
                     break;
                  }
               }
            } while ((ns = (NAMESORT *)List->Next ()) != NULL);
      }
   }

   if (RetVal == TRUE) {
      // Aggiornamento dell'indice
      fileIndex.Area = StringCrc32 (Area, 0xFFFFFFFFL);
      strcpy (fileIndex.Name, Name);
      memset (&ftm, 0, sizeof (ftm));
      ftm.tm_min = UplDate.Minute;
      ftm.tm_hour = UplDate.Hour;
      ftm.tm_mday = UplDate.Day;
      ftm.tm_mon = UplDate.Month - 1;
      ftm.tm_year = UplDate.Year - 1900;
      fileIndex.UploadDate = mktime (&ftm);
      if (Unapproved == TRUE)
         fileIndex.Flags |= FILE_UNAPPROVED;
      else
         fileIndex.Flags &= ~FILE_UNAPPROVED;

      lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex), SEEK_SET);
      write (fdIdx, &fileIndex, sizeof (fileIndex));

      // Aggiornamento della struttura dati principale
      strcpy (fileData.Area, Area);
      strcpy (fileData.Name, Name);
      strcpy (fileData.Complete, Complete);
      strcpy (fileData.Keyword, Keyword);
      fileData.Size = Size;
      fileData.DlTimes = DlTimes;
      memset (&ftm, 0, sizeof (ftm));
      ftm.tm_min = Date.Minute;
      ftm.tm_hour = Date.Hour;
      ftm.tm_mday = Date.Day;
      ftm.tm_mon = Date.Month - 1;
      ftm.tm_year = Date.Year - 1900;
      FileDate = fileData.FileDate = mktime (&ftm);
      memset (&ftm, 0, sizeof (ftm));
      ftm.tm_min = UplDate.Minute;
      ftm.tm_hour = UplDate.Hour;
      ftm.tm_mday = UplDate.Day;
      ftm.tm_mon = UplDate.Month - 1;
      ftm.tm_year = UplDate.Year - 1900;
      UploadDate = fileData.UploadDate = mktime (&ftm);
      fileData.Cost = Cost;
      fileData.Password = Password;
      fileData.Level = Level;
      fileData.AccessFlags = AccessFlags;
      fileData.DenyFlags = DenyFlags;
      if (Unapproved == TRUE)
         fileData.Flags |= FILE_UNAPPROVED;
      else
         fileData.Flags &= ~FILE_UNAPPROVED;
      if (CdRom == TRUE)
         fileData.Flags |= FILE_CDROM;
      else
         fileData.Flags &= ~FILE_CDROM;

      lseek (fdDat, fileIndex.Offset, SEEK_SET);
      write (fdDat, &fileData, sizeof (fileData));
   }

   return (TRUE);
}

VOID TFileBase::SearchFile (PSZ pszFile)
{
   USHORT i, Readed, RetVal = FALSE;
   ULONG Crc;
   NAMESORT ns, *pns;
   FILEDATA fileData;
   FILEINDEX *fileIndex;

   if (List == NULL)
      List = new TCollection;
   fileIndex = (FILEINDEX *)malloc (sizeof (FILEINDEX) * MAX_INDEX);
   strlwr (pszFile);

   if (List != NULL && fileIndex != NULL) {
      List->Clear ();
      Crc = StringCrc32 (szArea, 0xFFFFFFFFL);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (i = 0; i < Readed; i++) {
               if (!(fileIndex[i].Flags & (FILE_DELETED|FILE_UNAPPROVED)) && (szArea[0] == '\0' || fileIndex[i].Area == Crc)) {
                  lseek (fdDat, fileIndex[i].Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));

                  if (fileData.Id == FILEBASE_ID) {
                     RetVal = FALSE;
                     if (MatchName (fileIndex[i].Name, pszFile) == TRUE) {
                        fileIndex[i].Name[sizeof (fileIndex[i].Name) - 1] = '\0';
                        strcpy (ns.Name, fileIndex[i].Name);
                        ns.Position = fileIndex[i].Offset;
                        if ((pns = (NAMESORT *)List->First ()) != NULL) {
                           if (strcmp (pns->Name, ns.Name) > 0) {
                              List->Insert (&ns, sizeof (ns));
                              List->Insert (pns, sizeof (ns));
                              List->First ();
                              List->Remove ();
                              RetVal = TRUE;
                           }
                           if (RetVal == FALSE)
                              do {
                                 if (strcmp (pns->Name, ns.Name) > 0) {
                                    List->Previous ();
                                    List->Insert (&ns, sizeof (ns));
                                    RetVal = TRUE;
                                 }
                              } while (RetVal == FALSE && (pns = (NAMESORT *)List->Next ()) != NULL);
                        }

                        if (RetVal == FALSE)
                           List->Add (&ns, sizeof (ns));
                     }
                  }
               }
            }
#if defined(__OS2__)
            DosSleep (1L);
#elif defined(__NT__)
            Sleep (1L);
#endif
         }
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFileBase::SearchKeyword (PSZ pszKeyword)
{
   USHORT x, Readed, RetVal = FALSE;
   ULONG Crc;
   NAMESORT ns, *pns;
   FILEDATA fileData;
   FILEINDEX *fileIndex;

   if (List == NULL)
      List = new TCollection;
   fileIndex = (FILEINDEX *)malloc (sizeof (FILEINDEX) * MAX_INDEX);
   strlwr (pszKeyword);

   if (List != NULL && fileIndex != NULL) {
      List->Clear ();
      Crc = StringCrc32 (szArea, 0xFFFFFFFFL);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (x = 0; x < Readed; x++) {
               if (!(fileIndex[x].Flags & (FILE_DELETED|FILE_UNAPPROVED)) && (szArea[0] == '\0' || fileIndex[x].Area == Crc)) {
                  lseek (fdDat, fileIndex[x].Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));

                  if (fileData.Id == FILEBASE_ID) {
                     if (strstr (strlwr (fileData.Keyword), pszKeyword) != NULL) {
                        fileIndex[x].Name[sizeof (fileIndex[x].Name) - 1] = '\0';
                        strcpy (ns.Name, fileIndex[x].Name);
                        ns.Position = fileIndex[x].Offset;

                        RetVal = FALSE;
                        if ((pns = (NAMESORT *)List->First ()) != NULL) {
                           if (strcmp (pns->Name, ns.Name) > 0) {
                              List->Insert (&ns, sizeof (ns));
                              List->Insert (pns, sizeof (ns));
                              List->First ();
                              List->Remove ();
                              RetVal = TRUE;
                           }
                           if (RetVal == FALSE)
                              do {
                                 if (strcmp (pns->Name, ns.Name) > 0) {
                                    List->Previous ();
                                    List->Insert (&ns, sizeof (ns));
                                    RetVal = TRUE;
                                 }
                              } while (RetVal == FALSE && (pns = (NAMESORT *)List->Next ()) != NULL);
                        }

                        if (RetVal == FALSE)
                           List->Add (&ns, sizeof (ns));
                     }
                  }
               }
            }
#if defined(__OS2__)
            DosSleep (1L);
#elif defined(__NT__)
            Sleep (1L);
#endif
         }
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFileBase::SearchText (PSZ pszText)
{
   USHORT x, i, r, w, Readed, RetVal = FALSE, AddThis;
   CHAR szTemp[80], szLine[80];
   ULONG Crc;
   NAMESORT ns, *pns;
   FILEDATA fileData;
   FILEINDEX *fileIndex;

   if (List == NULL)
      List = new TCollection;
   fileIndex = (FILEINDEX *)malloc (sizeof (FILEINDEX) * MAX_INDEX);
   strlwr (pszText);

   if (List != NULL && fileIndex != NULL) {
      List->Clear ();
      Crc = StringCrc32 (szArea, 0xFFFFFFFFL);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (x = 0; x < Readed; x++) {
               if (!(fileIndex[x].Flags & (FILE_DELETED|FILE_UNAPPROVED)) && (szArea[0] == '\0' || fileIndex[x].Area == Crc)) {
                  lseek (fdDat, fileIndex[x].Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));

                  if (fileData.Id == FILEBASE_ID) {
                     AddThis = FALSE;
                     fileData.Name[sizeof (fileData.Name) - 1] = '\0';
                     fileData.Keyword[sizeof (fileData.Keyword) - 1] = '\0';

                     if (strstr (strlwr (fileData.Name), pszText) != NULL)
                        AddThis = TRUE;
                     if (strstr (strlwr (fileData.Keyword), pszText) != NULL)
                        AddThis = TRUE;

                     if (fileData.Description != 0 && AddThis == FALSE) {
                        w = 0;
                        do {
                           if ((r = (USHORT)sizeof (szTemp)) > fileData.Description)
                              r = fileData.Description;
                           r = (USHORT)read (fdDat, szTemp, r);
                           for (i = 0; i < r && AddThis == FALSE; i++) {
                              if (szTemp[i] == '\r') {
                                 szLine[w++] = '\0';
                                 if (strstr (strlwr (szLine), pszText) != NULL)
                                    AddThis = TRUE;
                                 w = 0;
                              }
                              else if (szTemp[i] != '\n')
                                 szLine[w++] = szTemp[i];
                           }
                           fileData.Description -= r;
                        } while (AddThis == FALSE && fileData.Description > 0);
                        if (w > 0) {
                           szLine[w++] = '\0';
                           if (strstr (strlwr (szLine), pszText) != NULL)
                              AddThis = TRUE;
                        }
                     }

                     if (fileData.Uploader != 0 && AddThis == FALSE) {
                        fUploader = TRUE;
                        pszMemUploader = Uploader = (PSZ)malloc (fileData.Uploader);
                        read (fdDat, Uploader, fileData.Uploader);
                     }

                     if (AddThis == TRUE) {
                        fileIndex[x].Name[sizeof (fileIndex[x].Name) - 1] = '\0';
                        strcpy (ns.Name, fileIndex[x].Name);
                        ns.Position = fileIndex[x].Offset;

                        RetVal = FALSE;
                        if ((pns = (NAMESORT *)List->First ()) != NULL) {
                           if (strcmp (pns->Name, ns.Name) > 0) {
                              List->Insert (&ns, sizeof (ns));
                              List->Insert (pns, sizeof (ns));
                              List->First ();
                              List->Remove ();
                              RetVal = TRUE;
                           }
                           if (RetVal == FALSE)
                              do {
                                 if (strcmp (pns->Name, ns.Name) > 0) {
                                    List->Previous ();
                                    List->Insert (&ns, sizeof (ns));
                                    RetVal = TRUE;
                                 }
                              } while (RetVal == FALSE && (pns = (NAMESORT *)List->Next ()) != NULL);
                        }

                        if (RetVal == FALSE)
                           List->Add (&ns, sizeof (ns));
                     }
                  }
               }
            }
#if defined(__OS2__)
            DosSleep (1L);
#elif defined(__NT__)
            Sleep (1L);
#endif
         }
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFileBase::SortByDate (ULONG ulDate)
{
   USHORT i, Readed, RetVal = FALSE;
   ULONG Crc;
   NAMESORT ns, *pns;
   FILEDATA fileData;
   FILEINDEX *fileIndex;

   if (List == NULL)
      List = new TCollection;
   fileIndex = (FILEINDEX *)malloc (sizeof (FILEINDEX) * MAX_INDEX);

   if (List != NULL && fileIndex != NULL) {
      List->Clear ();
      Crc = StringCrc32 (szArea, 0xFFFFFFFFL);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (i = 0; i < Readed; i++) {
               if (!(fileIndex[i].Flags & (FILE_DELETED|FILE_UNAPPROVED)) && (szArea[0] == '\0' || fileIndex[i].Area == Crc)) {
                  lseek (fdDat, fileIndex[i].Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));

                  if (fileData.Id == FILEBASE_ID) {
                     fileIndex[i].Name[sizeof (fileIndex[i].Name) - 1] = '\0';
                     strcpy (ns.Name, fileIndex[i].Name);
                     ns.Date = fileIndex[i].UploadDate;
                     ns.Position = fileIndex[i].Offset;

                     if (ns.Date > ulDate) {
                        RetVal = FALSE;
                        if ((pns = (NAMESORT *)List->First ()) != NULL) {
                           if (pns->Date < ns.Date) {
                              List->Insert (&ns, sizeof (ns));
                              List->Insert (pns, sizeof (ns));
                              List->First ();
                              List->Remove ();
                              List->First ();
                              RetVal = TRUE;
                           }
                           if (RetVal == FALSE)
                              do {
                                 if (pns->Date < ns.Date) {
                                    List->Previous ();
                                    List->Insert (&ns, sizeof (ns));
                                    RetVal = TRUE;
                                 }
                              } while (RetVal == FALSE && (pns = (NAMESORT *)List->Next ()) != NULL);
                        }

                        if (RetVal == FALSE)
                           List->Add (&ns, sizeof (ns));
                     }
                  }
               }
            }
#if defined(__OS2__)
            DosSleep (1L);
#elif defined(__NT__)
            Sleep (1L);
#endif
         }
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFileBase::SortByDownload (VOID)
{
   USHORT x, Readed, RetVal = FALSE;
   ULONG Crc;
   NAMESORT ns, *pns;
   FILEDATA fileData;
   FILEINDEX *fileIndex;

   if (List == NULL)
      List = new TCollection;
   fileIndex = (FILEINDEX *)malloc (sizeof (FILEINDEX) * MAX_INDEX);

   if (List != NULL && fileIndex != NULL) {
      List->Clear ();
      Crc = StringCrc32 (szArea, 0xFFFFFFFFL);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (x = 0; x < Readed; x++) {
               if (!(fileIndex[x].Flags & (FILE_DELETED|FILE_UNAPPROVED)) && (szArea[0] == '\0' || fileIndex[x].Area == Crc)) {
                  lseek (fdDat, fileIndex[x].Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));

                  if (fileData.Id == FILEBASE_ID) {
                     fileIndex[x].Name[sizeof (fileIndex[x].Name) - 1] = '\0';
                     strcpy (ns.Name, fileIndex[x].Name);
                     ns.Download = fileData.DlTimes;
                     ns.Position = fileIndex[x].Offset;

                     RetVal = FALSE;
                     if ((pns = (NAMESORT *)List->First ()) != NULL) {
                        if (pns->Download < ns.Download) {
                           List->Insert (&ns, sizeof (ns));
                           List->Insert (pns, sizeof (ns));
                           List->First ();
                           List->Remove ();
                           RetVal = TRUE;
                        }
                        if (RetVal == FALSE)
                           do {
                              if (pns->Download < ns.Download) {
                                 List->Previous ();
                                 List->Insert (&ns, sizeof (ns));
                                 RetVal = TRUE;
                              }
                           } while (RetVal == FALSE && (pns = (NAMESORT *)List->Next ()) != NULL);
                     }

                     if (RetVal == FALSE)
                        List->Add (&ns, sizeof (ns));
                  }
               }
            }
#if defined(__OS2__)
            DosSleep (1L);
#elif defined(__NT__)
            Sleep (1L);
#endif
         }
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFileBase::SortByName (VOID)
{
   USHORT i, Readed, RetVal = FALSE;
   ULONG Crc, Position;
   NAMESORT ns, *pns;
   FILEDATA fileData;
   FILEINDEX *fileIndex;

   if (List == NULL)
      List = new TCollection;
   fileIndex = (FILEINDEX *)malloc (sizeof (FILEINDEX) * MAX_INDEX);

   if (List != NULL && fileIndex != NULL) {
      List->Clear ();
      Crc = StringCrc32 (szArea, 0xFFFFFFFFL);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         Position = 0L;
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (i = 0; i < Readed; i++, Position += sizeof (FILEINDEX)) {
               if (!(fileIndex[i].Flags & (FILE_DELETED|FILE_UNAPPROVED)) && (szArea[0] == '\0' || fileIndex[i].Area == Crc)) {
                  lseek (fdDat, fileIndex[i].Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));

                  if (fileData.Id == FILEBASE_ID) {
                     fileIndex[i].Name[sizeof (fileIndex[i].Name) - 1] = '\0';
                     strcpy (ns.Name, fileIndex[i].Name);
                     ns.Position = fileIndex[i].Offset;
                     ns.IdxPosition = Position;

                     RetVal = FALSE;
                     if ((pns = (NAMESORT *)List->First ()) != NULL) {
                        if (strcmp (pns->Name, ns.Name) > 0) {
                           List->Insert (&ns, sizeof (ns));
                           List->Insert (pns, sizeof (ns));
                           List->First ();
                           List->Remove ();
                           RetVal = TRUE;
                        }
                        if (RetVal == FALSE)
                           do {
                              if (strcmp (pns->Name, ns.Name) > 0) {
                                 List->Previous ();
                                 List->Insert (&ns, sizeof (ns));
                                 RetVal = TRUE;
                              }
                           } while (RetVal == FALSE && (pns = (NAMESORT *)List->Next ()) != NULL);
                     }

                     if (RetVal == FALSE)
                        List->Add (&ns, sizeof (ns));
                  }
               }
            }
#if defined(__OS2__)
            DosSleep (1L);
#elif defined(__NT__)
            Sleep (1L);
#endif
         }
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}


