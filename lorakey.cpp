
#include "_ldefs.h"
#include "lora_api.h"

//#define PRE_KEY      "BT"        // Beta testers

//#define PRE_KEY      "IT"        // Italy, Marco Maccaferri
//#define PRE_KEY      "US"        // USA, Robert Butler
#define PRE_KEY      "AU"        // Australia, Michael Doyle

CHAR KeyLines[4][64];

VOID GenerateKey (PSZ product, USHORT type, PSZ name, PSZ serial)
{
   FILE *fp;
   CHAR Check[64];
   ULONG Crc;

   if ((fp = fopen ("lora.key", "at")) != NULL) {
      sprintf (Check, "[%s]", product);
      fprintf (fp, "%s\n", Check);
      fprintf (fp, "name=%s\n", name);
      fprintf (fp, "serial=%s\n", serial);

      if (type == KEY_BASIC)
         Crc = StringCrc32 ("30BASIC", 0xFFFFFFFFL);
      else if (type == KEY_ADVANCED)
         Crc = StringCrc32 ("30ADVANCED", 0xFFFFFFFFL);
      else if (type == KEY_PROFESSIONAL)
         Crc = StringCrc32 ("30PROFESSIONAL", 0xFFFFFFFFL);

      Crc = StringCrc32 (Check, Crc);
      Crc = StringCrc32 (name, Crc);
      Crc = StringCrc32 (serial, Crc);

      fprintf (fp, "validation=%lu\n\n", Crc);

      fclose (fp);

      sprintf (Check, "[%s]", product);
      sprintf (KeyLines[0], "%s", Check);
      sprintf (KeyLines[1], "name=%s", name);
      sprintf (KeyLines[2], "serial=%s", serial);
      sprintf (KeyLines[3], "validation=%lu", Crc);
   }
}

void main (void)
{
   FILE *fp;
   int fd, num;
   USHORT Key;
   CHAR Name[64], OS[16], Address1[64], Address2[64], City[64];
   CHAR Version[32], Serial[32], Temp[64];
   time_t t;
   struct tm *ltm;

	printf ("\nLoraBBS 2.99 - Registration Key Generator\n");
   printf ("CopyRight (c) 1996-97 by Marco Maccaferri. All Rights Reserved\n\n");

   unlink ("lora.key");

   printf ("Name..........................: ");
   gets (Name);
   printf ("Address 1.....................: ");
   gets (Address1);
   printf ("Address 2.....................: ");
   gets (Address2);
   printf ("ZIP / City / State / Country..: ");
   gets (City);

   printf ("\nSerial number..........................: %s", PRE_KEY);
   if ((fd = open ("lorakey.dat", O_RDWR|O_BINARY)) == -1) {
      gets (Serial);
      num = atoi (Serial);
      sprintf (Serial, "%s%04d", PRE_KEY, num);

      fd = open ("lorakey.dat", O_WRONLY|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
      write (fd, &num, sizeof (int));
      close (fd);
   }
   else {
      read (fd, &num, sizeof (int));
      sprintf (Serial, "%s%04d", PRE_KEY, ++num);
      printf ("%04d\n", num);
      lseek (fd, 0L, SEEK_SET);
      write (fd, &num, sizeof (int));
      close (fd);
   }

loop:
   printf ("O.S. (dos, windows, os/2, linux).......: ");
   gets (OS);
   printf ("Version (basic, advanced, professional): ");
   gets (Version);

   sprintf (Temp, "bbs-%s", OS);
   if (!stricmp (Version, "basic"))
      Key = KEY_BASIC;
   else if (!stricmp (Version, "advanced"))
      Key = KEY_ADVANCED;
   else if (!stricmp (Version, "professional"))
      Key = KEY_PROFESSIONAL;

   GenerateKey (Temp, Key, Name, Serial);

   printf ("\nPlease confirm the generation (Y/N) ? ");
   gets (Temp);
   if (!stricmp (Temp, "y") || !stricmp (Temp, "Y")) {
      if ((fp = fopen ("key.log", "at")) != NULL) {
         t = time (NULL);
         ltm = localtime (&t);
         fprintf (fp, "Date.......: %s", asctime (ltm));
         fprintf (fp, "Name.......: %s\n", Name);
         fprintf (fp, "Address....: %s\n", Address1);
         if (Address2[0] != '\0')
            fprintf (fp, "             %s\n", Address2);
         fprintf (fp, "City/State.: %-31.31s %s\n", City, KeyLines[0]);
         fprintf (fp, "O.S. ......: %-31.31s %s\n", OS, KeyLines[1]);
         fprintf (fp, "Version....: %-31.31s %s\n", Version, KeyLines[2]);
         fprintf (fp, "Serial.....: %-31.31s %s\n", Serial, KeyLines[3]);
         fprintf (fp, "----------------------------------------\n");
         fclose (fp);
      }

      printf ("Make one more key to the same user (Y/N) ? ");
      gets (Temp);
      if (!stricmp (Temp, "y") || !stricmp (Temp, "Y")) {
         printf ("\nSerial number..........................: %s%04d\n", PRE_KEY, num);
         goto loop;
      }
   }
}

