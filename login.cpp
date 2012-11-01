
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.8
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    01/08/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora.h"

void main (int argc, char *argv[])
{
   int i;
   USHORT Task;
   CHAR Temp[128], *Config = NULL, *Channel = NULL;
   class TBbs *Bbs;
   class TStdio *Com;

/*
   printf ("\n\n%s Version %s\n", NAME, VERSION);
   printf ("Copyright (c) 1996 by Marco Maccaferri. All rights reserved.\n\n");

   if (ValidateKey ("bbs", NULL, NULL) == KEY_UNREGISTERED) {
      printf ("* * *     WARNING: No license key found    * * *\n");
      if ((i = CheckExpiration ()) == 0) {
         printf ("* * *   This evaluation copy has expired   * * *\n\a\n");
         exit (0);
      }
      else
         printf ("* * * You have %2d days left for evaluation * * * \n\n\a", i);
   }
*/

   for (i = 1; i < argc; i++) {
      if (!stricmp (argv[i], "/LINE")) {
         i++;
         Task = (USHORT)atoi (argv[i]);
      }
      else if (Config == NULL)
         Config = argv[i];
      else if (Channel == NULL)
         Channel = argv[i];
   }

   if ((Bbs = new TBbs) != NULL) {
      if ((Bbs->Cfg = new TConfig) != NULL) {
         Bbs->Cfg->TaskNumber = Task;
         if (Bbs->Cfg->Load (Config, Channel) == FALSE)
            Bbs->Cfg->Default ();
      }
      if ((Bbs->Log = new TLog) != NULL) {
#if defined(__LINUX__)
         if (Bbs->Cfg->LogFile[0] == '\0')
            sprintf (Temp, "lora%u.log", Bbs->Cfg->TaskNumber);
         else
            sprintf (Temp, Bbs->Cfg->LogFile, Bbs->Cfg->TaskNumber);
         Bbs->Log->Open (Temp);
#else
         Bbs->Log->Open ("local.log");
#endif
         Bbs->Log->Write ("+Connect Local");
      }
      if ((Com = new TStdio) != NULL)
         Com->Initialize ();
      Bbs->Com = Com;
      Bbs->Speed = 57600L;

      Bbs->Run ();

      if (Bbs->Com != NULL)
         delete Bbs->Com;
      if (Bbs->Cfg != NULL)
         delete Bbs->Cfg;
      if (Bbs->Log != NULL) {
         Bbs->Log->WriteBlank ();
         delete Bbs->Log;
      }

      delete Bbs;
   }
}

