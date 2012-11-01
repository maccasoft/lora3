
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.20
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "filebase.h"

#define MAX_INDEX       256

typedef struct {
   CHAR  Name[32];
   ULONG Date;
   ULONG Download;
   ULONG Position;
} NAMESORT;

TFile::TFile (void)
{
   fdIdx = fdDat = -1;
   fUploader = FALSE;
   Description = new TCollection;
   Clear ();
   List = NULL;
}

TFile::TFile (PSZ pszPath, PSZ pszArea)
{
   fdIdx = fdDat = -1;
   fUploader = FALSE;
   Description = new TCollection;
   Clear ();
   Open (pszPath, pszArea);
   List = NULL;
}

TFile::~TFile (void)
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

USHORT TFile::Add (VOID)
{
   PSZ pszTemp;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm ftm;

   fUploader = FALSE;

   memset (&fileIndex, 0, sizeof (fileIndex));
   fileIndex.Area = StringCrc32 (Area);
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

   lseek (fdIdx, 0L, SEEK_END);
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

VOID TFile::Clear (VOID)
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
}

VOID TFile::Close (VOID)
{
   if (fdIdx != -1)
      close (fdIdx);
   if (fdDat != -1)
      close (fdDat);
   fdIdx = fdDat = -1;
}

VOID TFile::Delete (VOID)
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

USHORT TFile::First (VOID)
{
   USHORT RetVal = FALSE, i, r, w;
   CHAR szTemp[80], szLine[80];
   NAMESORT *ns;
   FILEDATA fileData;
   struct tm *ftm;

   if (List == NULL) {
      lseek (fdIdx, 0L, SEEK_SET);
      RetVal = Next ();
   }
   else if ((ns = (NAMESORT *)List->First ()) != NULL) {
      RetVal = TRUE;

      lseek (fdDat, ns->Position, SEEK_SET);
      read (fdDat, &fileData, sizeof (fileData));

      Clear ();
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
   }

   return (RetVal);
}

USHORT TFile::Next (VOID)
{
   USHORT fRet = FALSE, i, r, w;
   CHAR szTemp[80], szLine[80];
   ULONG ulCrc;
   NAMESORT *ns;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm *ftm;

   if (List == NULL && fdDat != -1) {
      ulCrc = StringCrc32 (szArea);

      while (read (fdIdx, &fileIndex, sizeof (fileIndex)) == sizeof (fileIndex)) {
         if (fileIndex.Flags & FILE_DELETED)
            continue;
         if (szArea[0] == '\0' || fileIndex.Area == ulCrc) {
            fRet = TRUE;
            lseek (fdDat, fileIndex.Offset, SEEK_SET);
            break;
         }
      }
   }
   else if (List != NULL && (ns = (NAMESORT *)List->Next ()) != NULL) {
      lseek (fdDat, ns->Position, SEEK_SET);
      fRet = TRUE;
   }

   if (fRet == TRUE) {
      read (fdDat, &fileData, sizeof (fileData));

      Clear ();
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
   }

   return (fRet);
}

USHORT TFile::Open (PSZ pszPath, PSZ pszArea)
{
   CHAR szFile[128];

   sprintf (szFile, "%s%s", pszPath, "FileBase.Idx");
   if ((fdIdx = open (szFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) == -1)
      return (FALSE);

   sprintf (szFile, "%s%s", pszPath, "FileBase.Dat");
   if ((fdDat = open (szFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE)) == -1) {
      close (fdDat);
      fdDat = -1;
      return (FALSE);
   }

   strcpy (szArea, pszArea);

   return (TRUE);
}

USHORT TFile::Previous (VOID)
{
   USHORT fRet = FALSE, i, r, w;
   CHAR szTemp[80], szLine[80];
   ULONG ulCrc;
   NAMESORT *ns;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm *ftm;

   if (List == NULL && fdDat != -1) {
      ulCrc = StringCrc32 (szArea);

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

      Clear ();
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
   }

   return (fRet);
}

USHORT TFile::Read (PSZ pszFile)
{
   USHORT fRet = FALSE, i, r, w;
   CHAR szTemp[80], szLine[80];
   ULONG ulCrc;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm *ftm;

   ulCrc = StringCrc32 (szArea);
   lseek (fdIdx, 0L, SEEK_SET);

   if (fdDat != -1) {
      while (fRet == FALSE && read (fdIdx, &fileIndex, sizeof (fileIndex)) == sizeof (fileIndex)) {
         if (!(fileIndex.Flags & FILE_DELETED)) {
            if (szArea[0] == '\0' || fileIndex.Area == ulCrc) {
               if (!stricmp (fileIndex.Name, pszFile))
                  fRet = TRUE;
            }
         }
      }
      if (fRet == TRUE) {
         lseek (fdDat, fileIndex.Offset, SEEK_SET);
         read (fdDat, &fileData, sizeof (fileData));

         Clear ();
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
      }
   }

   return (fRet);
}

USHORT TFile::Replace (VOID)
{
   PSZ pszTemp;
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm ftm;

   fUploader = FALSE;

   lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex), SEEK_SET);
   read (fdIdx, &fileIndex, sizeof (fileIndex));

   lseek (fdDat, fileIndex.Offset, SEEK_SET);
   read (fdDat, &fileData, sizeof (fileData));
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

USHORT TFile::ReplaceHeader (VOID)
{
   FILEDATA fileData;
   FILEINDEX fileIndex;
   struct tm ftm;

   if (tell (fdIdx) > 0) {
      lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex), SEEK_SET);
      read (fdIdx, &fileIndex, sizeof (fileIndex));

      fileIndex.Area = StringCrc32 (Area);
      strcpy (fileIndex.Name, Name);
      memset (&ftm, 0, sizeof (ftm));
      ftm.tm_min = UplDate.Minute;
      ftm.tm_hour = UplDate.Hour;
      ftm.tm_mday = UplDate.Day;
      ftm.tm_mon = UplDate.Month - 1;
      ftm.tm_year = UplDate.Year - 1900;
      fileIndex.UploadDate = mktime (&ftm);

      lseek (fdIdx, tell (fdIdx) - sizeof (fileIndex), SEEK_SET);
      write (fdIdx, &fileIndex, sizeof (fileIndex));

      lseek (fdDat, fileIndex.Offset, SEEK_SET);
      read (fdDat, &fileData, sizeof (fileData));

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

      lseek (fdDat, fileIndex.Offset, SEEK_SET);
      write (fdDat, &fileData, sizeof (fileData));
   }

   return (TRUE);
}

VOID TFile::SearchFile (PSZ pszFile)
{
   USHORT i, Readed, RetVal = FALSE;
   ULONG Crc;
   NAMESORT ns, *pns;
   FILEINDEX *fileIndex;

   if (List == NULL)
      List = new TCollection;
   fileIndex = (FILEINDEX *)malloc (sizeof (FILEINDEX) * MAX_INDEX);
   strlwr (pszFile);

   if (List != NULL && fileIndex != NULL) {
      List->Clear ();
      Crc = StringCrc32 (szArea);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (i = 0; i < Readed; i++) {
               if (!(fileIndex[i].Flags & FILE_DELETED) && (szArea[0] == '\0' || fileIndex[i].Area == Crc)) {
                  RetVal = FALSE;
                  if (strstr (strlwr (fileIndex[i].Name), pszFile) != NULL) {
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
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFile::SearchKeyword (PSZ pszKeyword)
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
      Crc = StringCrc32 (szArea);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (x = 0; x < Readed; x++) {
               if (!(fileIndex[x].Flags & FILE_DELETED) && (szArea[0] == '\0' || fileIndex[x].Area == Crc)) {
                  lseek (fdDat, fileIndex[x].Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));

                  if (strstr (strlwr (fileData.Keyword), pszKeyword) != NULL) {
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
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFile::SearchText (PSZ pszText)
{
   USHORT x, i, r, w, Readed, RetVal = FALSE, AddThis;
   CHAR szTemp[80], szLine[80], *p;
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
      Crc = StringCrc32 (szArea);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (x = 0; x < Readed; x++) {
               if (!(fileIndex[x].Flags & FILE_DELETED) && (szArea[0] == '\0' || fileIndex[x].Area == Crc)) {
                  lseek (fdDat, fileIndex[x].Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));

                  AddThis = FALSE;

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
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFile::SortByDate (ULONG ulDate)
{
   USHORT i, Readed, RetVal = FALSE;
   ULONG Crc;
   NAMESORT ns, *pns;
   FILEINDEX *fileIndex;

   if (List == NULL)
      List = new TCollection;
   fileIndex = (FILEINDEX *)malloc (sizeof (FILEINDEX) * MAX_INDEX);

   if (List != NULL && fileIndex != NULL) {
      List->Clear ();
      Crc = StringCrc32 (szArea);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (i = 0; i < Readed; i++) {
               if (!(fileIndex[i].Flags & FILE_DELETED) && (szArea[0] == '\0' || fileIndex[i].Area == Crc)) {
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
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFile::SortByDownload (VOID)
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
      Crc = StringCrc32 (szArea);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (x = 0; x < Readed; x++) {
               if (!(fileIndex[x].Flags & FILE_DELETED) && (szArea[0] == '\0' || fileIndex[x].Area == Crc)) {
                  lseek (fdDat, fileIndex[x].Offset, SEEK_SET);
                  read (fdDat, &fileData, sizeof (fileData));

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
      }
   }

   if (fileIndex != NULL)
      free (fileIndex);
}

VOID TFile::SortByName (VOID)
{
   USHORT i, Readed, RetVal = FALSE;
   ULONG Crc;
   NAMESORT ns, *pns;
   FILEINDEX *fileIndex;

   if (List == NULL)
      List = new TCollection;
   fileIndex = (FILEINDEX *)malloc (sizeof (FILEINDEX) * MAX_INDEX);

   if (List != NULL && fileIndex != NULL) {
      List->Clear ();
      Crc = StringCrc32 (szArea);

      if (fdDat != -1 && fdIdx != -1) {
         lseek (fdIdx, 0L, SEEK_SET);
         while ((Readed = (USHORT)read (fdIdx, fileIndex, sizeof (FILEINDEX) * MAX_INDEX)) > 0) {
            Readed /= sizeof (FILEINDEX);
            for (i = 0; i < Readed; i++) {
               if (!(fileIndex[i].Flags & FILE_DELETED) && (szArea[0] == '\0' || fileIndex[i].Area == Crc)) {
                  strcpy (ns.Name, fileIndex[i].Name);
                  ns.Position = fileIndex[i].Offset;

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
   }

   if (fileIndex != NULL)
      free (fileIndex);
}


