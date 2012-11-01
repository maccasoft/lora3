
#include "_ldefs.h"
#include "lora_api.h"

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

      fprintf (fp, "validation=%lu\n", Crc);

      fclose (fp);
   }
}

void main (void)
{
   FILE *fp;
   USHORT Key;
   CHAR Name[64], OS[16], Address1[64], Address2[64], City[64];
   CHAR Version[32], Serial[32], Temp[64];
   time_t t;
   struct tm *ltm;

   unlink ("lora.key");

   printf ("\n");
   printf ("Name..........................: ");
   gets (Name);
   printf ("Address 1.....................: ");
   gets (Address1);
   printf ("Address 2.....................: ");
   gets (Address2);
   printf ("ZIP / City / State / Country..: ");
   gets (City);

loop:
   printf ("\n");

   printf ("O.S. (dos, windows, os/2, linux).......: ");
   gets (OS);
   printf ("Version (basic, advanced, professional): ");
   gets (Version);
   printf ("Serial number..........................: ");
   gets (Serial);

   sprintf (Temp, "bbs-%s", OS);
   if (!stricmp (Version, "basic"))
      Key = KEY_BASIC;
   else if (!stricmp (Version, "advanced"))
      Key = KEY_ADVANCED;
   else if (!stricmp (Version, "professional"))
      Key = KEY_PROFESSIONAL;

   GenerateKey (Temp, Key, Name, Serial);

   if ((fp = fopen ("key.log", "at")) != NULL) {
      t = time (NULL);
      ltm = localtime (&t);
      fprintf (fp, "Date.......: %s", asctime (ltm));
      fprintf (fp, "Name.......: %s\n", Name);
      fprintf (fp, "Address....: %s\n", Address1);
      if (Address2[0] != '\0')
         fprintf (fp, "             %s\n", Address2);
      fprintf (fp, "City/State.: %s\n", City);
      fprintf (fp, "O.S. ......: %s\n", OS);
      fprintf (fp, "Version....: %s\n", Version);
      fprintf (fp, "Serial.....: %s\n", Serial);
      fprintf (fp, "----------------------------------------\n");
      fclose (fp);
   }

   printf ("Make one more key to the same user (S/N) ? ");
   gets (Temp);
   if (!stricmp (Temp, "s"))
      goto loop;
}

