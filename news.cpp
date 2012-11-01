
#include "_ldefs.h"
#include "msgbase.h"

void main (int argc, char *argv[])
{
   FILE *fp;
   USHORT i, UseLoraJam = FALSE, Found;
   CHAR Temp[128], *p, *Server, *Newsgroup, *Path;
   ULONG Num, Highest;
   struct dosdate_t date;
   struct dostime_t time;
   class MsgBase *Local, *News;
   class TCollection *Last;

   cprintf ("\r\nNEWS; USENET Newsgroups Mirror for LoraBBS Professional Edition\r\n");
   cprintf ("        Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\r\n");

   if (argc < 2) {
      cprintf ("\r\nUsage:\r\n       NEWS <Server> <Newsgroup> <Path> [-J|-S]\r\n\r\n");
      cprintf ("or     NEWS <Server> [-J|-L]\r\n\r\n");
      cprintf ("Where:\r\n");
      cprintf ("  <Server>    - NNTP server from which the newsgroups are mirrored.\r\n");
      cprintf ("  <Newsgroup> - USENET Newsgroup to mirror.\r\n");
      cprintf ("  <Path>      - Path for the local message base (either JAM or SQUISH).\r\n");
      cprintf ("  -J          - Selects the JAM message base as the local storage (default)\r\n");
      cprintf ("  -S          - Selects the SQUISH message base as the local storage\r\n");
      cprintf ("\r\n");
   }
   else {
      Server = Newsgroup = Path = NULL;

      for (i = 1; i < argc; i++) {
         if (!stricmp (argv[i], "-J"))
            UseLoraJam = FALSE;
         else if (!stricmp (argv[i], "-S"))
            UseLoraJam = TRUE;
         else if (Server == NULL)
            Server = argv[i];
         else if (Newsgroup == NULL)
            Newsgroup = argv[i];
         else if (Path == NULL)
            Path = argv[i];
      }

      if ((Last = new TCollection) != 0) {
         if ((fp = fopen ("news.rc", "rt")) != NULL) {
            while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
               if ((p = strchr (Temp, '\n')) != NULL)
                  *p = '\0';
               Last->Add (Temp, (USHORT)(strlen (Temp) + 1));
            }
            fclose (fp);
         }
      }

      cprintf ("\r\nConnecting to server ...");

      if ((News = new USENET (Server, Newsgroup)) != 0) {
         if (UseLoraJam == TRUE)
            Local = new SQUISH (Path);
         else
            Local = new JAM (Path);
         if (Local != 0) {
            cprintf ("\r\nUpdating newsgroup %s\r\n", Newsgroup);
            Highest = News->Highest ();
            if ((Num = News->Lowest ()) != 0L)
               Num--;

            if (Last != 0) {
               if ((p = (CHAR *)Last->First ()) != NULL)
                  do {
                     strcpy (Temp, p);
                     if ((p = strtok (Temp, ",")) != NULL) {
                        if (!stricmp (p, Newsgroup)) {
                           if (strtok (NULL, ",") != NULL) {
                              if ((p = strtok (NULL, "")) != NULL)
                                 Num = atol (p);
                           }
                        }
                     }
                  } while ((p = (CHAR *)Last->Next ()) != NULL);
            }

            if (News->Next (Num) == TRUE)
               do {
                  cprintf ("\rCopying %lu / %lu", Num, Highest);
                  if (News->Read (Num) == TRUE) {
                     Local->New ();
                     strcpy (Local->From, News->From);
                     strcpy (Local->To, News->To);
                     strcpy (Local->Subject, News->Subject);
                     Local->Written.Day = News->Written.Day;
                     Local->Written.Month = News->Written.Month;
                     Local->Written.Year = News->Written.Year;
                     Local->Written.Hour = News->Written.Hour;
                     Local->Written.Minute = News->Written.Minute;
                     _dos_getdate (&date);
                     _dos_gettime (&time);
                     Local->Arrived.Day = date.day;
                     Local->Arrived.Month = date.month;
                     Local->Arrived.Year = (USHORT)date.year;
                     Local->Arrived.Hour = time.hour;
                     Local->Arrived.Minute = time.minute;
                     Local->Arrived.Second = time.second;
                     Local->Write (Local->Highest () + 1L, News->Text);
                  }
               } while (News->Next (Num) == TRUE);

            cprintf ("\r\n");

            if (Last != 0) {
               Found = FALSE;
               if ((p = (CHAR *)Last->First ()) != NULL)
                  do {
                     strcpy (Temp, p);
                     if ((p = strtok (Temp, ",")) != NULL) {
                        if (!stricmp (p, Newsgroup)) {
                           sprintf (Temp, "%s,%s,%lu", Newsgroup, Path, Num);
                           Last->Replace (Temp, (USHORT)(strlen (Temp) + 1));
                        }
                     }
                  } while ((p = (CHAR *)Last->Next ()) != NULL);

               if (Found == FALSE) {
                  sprintf (Temp, "%s,%s,%lu", Newsgroup, Path, Num);
                  Last->Add (Temp, (USHORT)(strlen (Temp) + 1));
               }
            }

            delete Local;
         }

         delete News;
      }

      if (Last != 0) {
         if ((fp = fopen ("news.rc", "wt")) != NULL) {
            if ((p = (CHAR *)Last->First ()) != NULL)
               do {
                  fprintf (fp, "%s\n", p);
               } while ((p = (CHAR *)Last->Next ()) != NULL);
            fclose (fp);
         }
         delete Last;
      }
   }
}



