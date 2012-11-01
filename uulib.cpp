
#include "_ldefs.h"
#include "uulib.h"

#define ENC(c) ((c) ? ((c) & 077) + ' ': '`')
#define DEC(c) (((c) - ' ') & 077)

TUULib::TUULib (void)
{
   MaxLines = 0;
}

TUULib::~TUULib (void)
{
}

USHORT TUULib::Decode (PSZ pszBuffer)
{
   USHORT c1, c2, c3;
   PSZ src = pszBuffer;
   BYTE *dest = Buffer;

   Size = 0;
   memset (Buffer, 0, sizeof (Buffer));
   src++;

   while (*src != '\0' && src[1] != '\0' && src[2] != '\0') {
      c1 = (USHORT)((DEC (*src) << 2) | (DEC (src[1]) >> 4));
      c2 = (USHORT)((DEC (src[1]) << 4) | (DEC (src[2]) >> 2));
      c3 = (USHORT)((DEC (src[2]) << 6) | (DEC (src[3])));
      *dest++ = (BYTE)c1;
      *dest++ = (BYTE)c2;
      *dest++ = (BYTE)c3;
      src += 4;
      Size += 3;
   }

   return (Size);
}

USHORT TUULib::Decode (class TCollection &Text)
{
   return (Decode (&Text));
}

USHORT TUULib::Decode (class TCollection *Text)
{
   FILE *fpd = NULL;
   USHORT i, RetVal = FALSE, Began = FALSE;
   USHORT Checksum, CheckFile;
   ULONG CodeSize, FileSize;
   CHAR Destination[64], *Temp;

   CodeSize = FileSize = 0L;
   Checksum = CheckFile = 0;

   if ((Temp = (CHAR *)Text->First ()) != NULL)
      do {
         if (!strncmp (Temp, "end", 3)) {
            Began = FALSE;
            for (i = 0; i < strlen (Temp); i++) {
               Checksum += Temp[i];
               CodeSize++;
            }
            Checksum += 13;
            CodeSize++;
         }

         if (Began == TRUE) {
            for (i = 0; i < strlen (Temp); i++) {
               Checksum += Temp[i];
               CodeSize++;
            }
            Checksum += 13;
            CodeSize++;
            Decode (Temp);
            for (i = 0; i < Size; i++) {
               CheckFile += Buffer[i];
               FileSize++;
            }
            if (fpd != NULL)
               fwrite (Buffer, 1, Size, fpd);
         }

         if (!strncmp (Temp, "begin ", 6)) {
            strcpy (Destination, &Temp[10]);
            if ((fpd = fopen (Destination, "wb")) != NULL)
               RetVal = TRUE;
            Began = TRUE;
            for (i = 0; i < strlen (Temp); i++) {
               Checksum += Temp[i];
               CodeSize++;
            }
            Checksum += 13;
            CodeSize++;
         }
      } while ((Temp = (CHAR *)Text->Next ()) != NULL);

   if (fpd != NULL)
      fclose (fpd);

   return (RetVal);
}

USHORT TUULib::DecodeFile (PSZ pszSource, PSZ pszDestination)
{
   FILE *fps, *fpd;
   USHORT i, RetVal = FALSE, Began = FALSE;
   USHORT Checksum, CheckFile;
   ULONG CodeSize, FileSize;
   CHAR Temp[128];

   CodeSize = FileSize = 0L;
   Checksum = CheckFile = 0;

   if ((fps = fopen (pszSource, "rt")) != NULL) {
      if ((fpd = fopen (pszDestination, "wb")) != NULL) {
         RetVal = TRUE;

         fgets (Temp, sizeof (Temp) - 1, fps);
         while (fgets (Temp, sizeof (Temp) - 1, fps) != NULL) {
            Temp[strlen (Temp) - 1] = '\0';
            if (!strncmp (Temp, "end", 3)) {
               Began = FALSE;
               for (i = 0; i < strlen (Temp); i++) {
                  Checksum += Temp[i];
                  CodeSize++;
               }
               Checksum += 13;
               CodeSize++;
            }

            if (Began == TRUE) {
               for (i = 0; i < strlen (Temp); i++) {
                  Checksum += Temp[i];
                  CodeSize++;
               }
               Checksum += 13;
               CodeSize++;
               Decode (Temp);
               for (i = 0; i < Size; i++) {
                  CheckFile += Buffer[i];
                  FileSize++;
               }
               fwrite (Buffer, 1, Size, fpd);
            }

            if (!strncmp (Temp, "begin ", 6)) {
               Began = TRUE;
               for (i = 0; i < strlen (Temp); i++) {
                  Checksum += Temp[i];
                  CodeSize++;
               }
               Checksum += 13;
               CodeSize++;
            }
         }

         fclose (fpd);
      }
      fclose (fps);
   }

   return (RetVal);
}

VOID TUULib::Encode (BYTE *lpBuffer, USHORT usSize)
{
   USHORT c1, c2, c3, c4;
   BYTE *src = lpBuffer, *dest = Buffer;

   *dest++ = (BYTE)ENC (usSize);
   Size = 1;

   while (usSize > 0) {
      c1 = (USHORT)(*src >> 2);
      if (usSize >= 2)
         c2 = (USHORT)(((*src << 4) & 060) | ((src[1] >> 4) & 017));
      else
         c2 = 0;
      if (usSize >= 3) {
         c3 = (USHORT)(((src[1] << 2) & 074) | ((src[2] >> 6) & 03));
         c4 = (USHORT)(src[2] & 077);
      }
      else
         c3 = c4 = 0;
      *dest++ = (BYTE)ENC (c1);
      *dest++ = (BYTE)ENC (c2);
      *dest++ = (BYTE)ENC (c3);
      *dest++ = (BYTE)ENC (c4);
      Size += 4;
      src += (usSize >= 3) ? 3 : usSize;
      usSize -= (usSize >= 3) ? 3 : usSize;
   }
}

USHORT TUULib::EncodeFile (PSZ pszSource, PSZ pszDestination, PSZ pszRemote)
{
   FILE *fps, *fpd;
   USHORT RetVal = FALSE, Readed, Count = 0;
   BYTE Temp[128];
   CHAR TempFile[64], *p;

   strcpy (TempFile, pszDestination);

   if ((fps = fopen (pszSource, "rb")) != NULL) {
      if ((fpd = fopen (pszDestination, "wt")) != NULL) {
         RetVal = TRUE;

         if (pszRemote != NULL)
            fprintf (fpd, "begin 644 %s\n", pszRemote);
         else
            fprintf (fpd, "begin 644 %s\n", pszSource);

         do {
            if ((Readed = (USHORT)fread (Temp, 1, 45, fps)) != 0) {
               Encode (Temp, Readed);
               fwrite (Buffer, 1, Size, fpd);
               fwrite ("\n", 1, 1, fpd);

               if (MaxLines != 0 && ++Count >= MaxLines) {
                  fclose (fpd);
                  p = strchr (TempFile, '\0') - 1;
                  if (isdigit (*p)) {
                     if (*p == '9') {
                        *p = '0';
                        p--;
                        if (isdigit (*p)) {
                           if (*p == '9') {
                              p--;
                           }
                           else
                             (*p)++;
                        }
                        else
                           *p = '1';
                     }
                     else
                        (*p)++;
                  }
                  else
                     *p = '0';
                  fpd = fopen (TempFile, "wt");
                  Count = 0;
               }
            }
         } while (Readed == 45);

         fprintf (fpd, "'\nend\n");
         fclose (fpd);
      }
      fclose (fps);
   }

   return (RetVal);
}

