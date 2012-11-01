
#include "_ldefs.h"
#include "lora_api.h"
#include "msgbase.h"

class TConfig *Cfg;

VOID UpdateLastread (PSZ Area, ULONG TotalMsgs, ULONG *Active)
{
   int fd, i, Count, Changed, m;
   ULONG Position, LastRead;
   MSGTAGS *Buffer;

   if ((fd = sopen ("msgtags.dat", O_RDWR|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
      if ((Buffer = (MSGTAGS *)malloc (sizeof (MSGTAGS) * MSGTAGS_INDEX)) != NULL) {
         do {
            Changed = FALSE;

            Position = tell (fd);
            Count = read (fd, Buffer, sizeof (MSGTAGS) * MSGTAGS_INDEX) / sizeof (MSGTAGS);
            for (i = 0; i < Count; i++) {
               if (Buffer[i].Free == FALSE && !strcmp (Buffer[i].Area, Area)) {
                  LastRead = 0L;
                  for (m = 0; m < TotalMsgs; m++) {
                     if (Active[m] != 0L && Active[m] <= Buffer[i].LastRead)
                        LastRead = Active[m];
                  }
                  if (Buffer[i].LastRead != LastRead) {
                     Buffer[i].LastRead = LastRead;
                     Changed = TRUE;
                  }
               }
            }
            if (Changed == TRUE) {
               lseek (fd, Position, SEEK_SET);
               write (fd, Buffer, sizeof (MSGTAGS) * Count);
            }
         } while (Count == MSGTAGS_INDEX);
         free (Buffer);
      }

      close (fd);
   }
}

VOID PurgeMessages (CHAR *Area, USHORT WriteDate)
{
   int i;
   USHORT Deleted, Done;
   ULONG Number, Highest, TotalMsgs, *Active, Counter;
   time_t Today, MsgDate;
   struct tm ltm;
   class TMsgData *Data;
   class TMsgBase *Msg;

   cprintf (" * Purging Messages\r\n");
   Today = time (NULL) / 86400L;

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (Area != NULL) {
               if (Data->Read (Area) == FALSE)
                  break;
            }

            Msg = NULL;
            if (Data->Storage == ST_JAM)
               Msg = new JAM (Data->Path);
            else if (Data->Storage == ST_SQUISH)
               Msg = new SQUISH (Data->Path);
            else if (Data->Storage == ST_FIDO)
               Msg = new FIDOSDM (Data->Path);
            else if (Data->Storage == ST_ADEPT)
               Msg = new ADEPT (Data->Path);

            if (Msg != NULL) {
               cprintf (" +-- %-15.15s %-29.29s ", Data->Key, Data->Display);
               Msg->Lock (0L);

               Counter = 0;
               Deleted = 0;
               TotalMsgs = Msg->Number ();
               if (TotalMsgs > 0L) {
                  if ((Active = (ULONG *)malloc ((size_t)((TotalMsgs + 100L) * sizeof (ULONG)))) != NULL) {
                     i = 0;
                     Number = Msg->Lowest ();
                     do {
                        Active[i++] = Number;
                     } while (Msg->Next (Number) == TRUE);
                  }
               }
               else
                  Active = NULL;

               if (Data->DaysOld != 0 || Data->MaxMessages != 0) {
                  Number = Msg->Lowest ();
                  Highest = Msg->Highest ();

                  do {
                     if ((Counter % 10L) == 0L)
                        cprintf ("%6lu / %6lu\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", Counter, TotalMsgs);
                     Counter++;

                     if (Msg->ReadHeader (Number) == TRUE) {
                        Done = FALSE;
                        if (Data->DaysOld != 0) {
                           if (WriteDate == TRUE) {
                              ltm.tm_mday = Msg->Written.Day;
                              ltm.tm_mon = Msg->Written.Month - 1;
                              ltm.tm_year = Msg->Written.Year - 1900;
                              ltm.tm_hour = Msg->Written.Hour;
                              ltm.tm_min = Msg->Written.Minute;
                              ltm.tm_sec = Msg->Written.Second;
                           }
                           else {
                              ltm.tm_mday = Msg->Arrived.Day;
                              ltm.tm_mon = Msg->Arrived.Month - 1;
                              ltm.tm_year = Msg->Arrived.Year - 1900;
                              ltm.tm_hour = Msg->Arrived.Hour;
                              ltm.tm_min = Msg->Arrived.Minute;
                              ltm.tm_sec = Msg->Arrived.Second;
                           }
                           MsgDate = mktime (&ltm) / 86400L;

                           if ((Today - MsgDate) > Data->DaysOld) {
                              Msg->Delete (Number);
                              Done = TRUE;
                              Deleted++;
                              if (Active != NULL) {
                                 for (i = 0; i < TotalMsgs; i++) {
                                    if (Active[i] == Number)
                                       Active[i] = 0L;
                                 }
                              }
                           }
                        }
                        if (Done == FALSE && Data->MaxMessages != 0 && Msg->Number () > Data->MaxMessages) {
                           Msg->Delete (Number);
                           Deleted++;
                           if (Active != NULL) {
                              for (i = 0; i < TotalMsgs; i++) {
                                 if (Active[i] == Number)
                                    Active[i] = 0L;
                              }
                           }
                        }
                     }
                  } while (Msg->Next (Number) == TRUE);
                  cprintf ("%6lu / %6lu\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", Counter, TotalMsgs);
               }

               cprintf ("Total: %5lu, Deleted: %5u\r\n", TotalMsgs, Deleted);
               Msg->UnLock ();

               if (Deleted > 0) {
                  Data->ActiveMsgs = Msg->Number ();
                  Data->FirstMessage = Msg->Lowest ();
                  Data->LastMessage = Msg->Highest ();
                  Data->Update ();
                  if (Active != NULL)
                     UpdateLastread (Data->Key, TotalMsgs, Active);
               }
               if (Active != NULL)
                  free (Active);
            }
            if (Msg != NULL)
               delete Msg;

            if (Area != NULL)
               break;
         } while (Data->Next () == TRUE);
      delete Data;
   }
}

VOID PackMessages (CHAR *Area)
{
   class TMsgData *Data;
   class TMsgBase *Msg;

   printf (" * Pack (Compressing) Messages\n");

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (Area != NULL) {
               if (Data->Read (Area) == FALSE)
                  break;
            }

            Msg = NULL;
            if (Data->Storage == ST_JAM)
               Msg = new JAM (Data->Path);
            else if (Data->Storage == ST_SQUISH)
               Msg = new SQUISH (Data->Path);
            else if (Data->Storage == ST_FIDO)
               Msg = new FIDOSDM (Data->Path);
            else if (Data->Storage == ST_ADEPT)
               Msg = new ADEPT (Data->Path);
            if (Msg != NULL) {
               cprintf (" +-- %-15.15s %-49.49s ", Data->Key, Data->Display);
               fflush (stdout);
               Msg->Pack ();
               Data->ActiveMsgs = Msg->Number ();
               Data->FirstMessage = Msg->Lowest ();
               Data->LastMessage = Msg->Highest ();
               Data->Update ();
               cprintf ("\r\n");
            }
            if (Msg != NULL)
               delete Msg;

            if (Area != NULL)
               break;
         } while (Data->Next () == TRUE);
      delete Data;
   }
}

VOID ReindexMessages (VOID)
{
   class TMsgData *Data;
   class TMsgBase *Msg;

   printf (" * Indexing Messages\n");

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            Msg = NULL;
            if (Data->Storage == ST_JAM)
               Msg = new JAM (Data->Path);
            else if (Data->Storage == ST_SQUISH)
               Msg = new SQUISH (Data->Path);
            else if (Data->Storage == ST_FIDO)
               Msg = new FIDOSDM (Data->Path);
            else if (Data->Storage == ST_ADEPT)
               Msg = new ADEPT (Data->Path);
            if (Msg != NULL) {
               printf (" +-- %-15.15s %-29.29s ", Data->Key, Data->Display);
               Data->ActiveMsgs = Msg->Number ();
               Data->FirstMessage = Msg->Lowest ();
               Data->LastMessage = Msg->Highest ();
               Data->Update ();
               printf ("Total: %5lu, First: %5u\n", Data->ActiveMsgs, Data->FirstMessage);
            }
            if (Msg != NULL)
               delete Msg;
         } while (Data->Next () == TRUE);
      delete Data;
   }
}

void main (int argc, char *argv[])
{
   int i;
   USHORT Purge, Pack, Reindex, Link, WriteDate;
   CHAR *Area = NULL, *Config = NULL;

   Purge = Pack = FALSE;
   Reindex = Link = FALSE;
   WriteDate = FALSE;

   printf ("\nLMSG; %s v%s - Message maintenance utility\n", NAME, VERSION);
   printf ("      Copyright (c) 1991-96 by Marco Maccaferri. All Rights Reserved.\n\n");

   if (ValidateKey ("bbs", NULL, NULL) == KEY_UNREGISTERED) {
      printf ("* * *     WARNING: No license key found    * * *\n");
      if ((i = CheckExpiration ()) == 0) {
         printf ("* * *   This evaluation copy has expired   * * *\n\a\n");
          exit (0);
      }
      else
         printf ("* * * You have %2d days left for evaluation * * * \n\a\n", i);
   }

   if (argc <= 1) {
      printf (" * Command-line parameters:\n\n");

      printf ("        -A<key>   Process the area <key> only\n");
      printf ("        -I        Recreate index files\n");
      printf ("        -P[K]     Pack (compress) message base\n");
      printf ("                  K=Purge\n");
      printf ("        -K[W]     Purge messages from info in MSG.DAT\n");
      printf ("                  W=Use write date\n");
      printf ("        -L        Link messages by subject\n");

      printf ("\n * Please refer to the documentation for a more complete command summary\n\n");
   }
   else {
      for (i = 1; i < argc; i++) {
         if (argv[i][0] == '-' || argv[i][0] == '/') {
            switch (toupper (argv[i][1])) {
               case 'A':
                  Area = &argv[i][2];
                  break;
               case 'K':
                  Purge = TRUE;
                  if (toupper (argv[i][2]) == 'W')
                     WriteDate = TRUE;
                  break;
               case 'P':
                  Pack = TRUE;
                  if (toupper (argv[i][2]) == 'K')
                     Purge = TRUE;
                  break;
               case 'I':
                  Reindex = TRUE;
                  break;
               case 'L':
                  Link = TRUE;
                  break;
            }
         }
         else if (Config == NULL)
            Config = argv[i];
      }

      if ((Cfg = new TConfig) != NULL) {
         if (Cfg->Load (Config, NULL) == FALSE)
            Cfg->Default ();
      }

      if (Reindex == TRUE)
         ReindexMessages ();
      if (Purge == TRUE)
         PurgeMessages (Area, WriteDate);
      if (Pack == TRUE)
         PackMessages (Area);
      if (Link == TRUE)
         ;

      if (Purge == TRUE || Pack == TRUE || Reindex == TRUE)
         printf (" * Done\n\n");
      else
         printf (" * Nothing to do\n\n");

      if (Cfg != NULL)
         delete Cfg;
   }
}


