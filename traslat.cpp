
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.35
// Copyright (c) 1997 by Marco Maccaferri. All rights reserved.
//
// History:
//    02/02/97 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"

TTranslation::TTranslation (void)
{
   fdDat = -1;
   strcpy (DataFile, "cost.dat");
}

TTranslation::TTranslation (PSZ pszDataPath)
{
   fdDat = -1;
   strcpy (DataFile, pszDataPath);
   strcat (DataFile, "cost.dat");
   AdjustPath (strlwr (DataFile));
}

TTranslation::~TTranslation (void)
{
   if (fdDat != -1)
      close (fdDat);
}

VOID TTranslation::Class2Struct (VOID)
{
   int i;

   memset (&table, 0, sizeof (table));
   table.Size = sizeof (table);
   strcpy (table.Location, Location);
   strcpy (table.Search, Search);
   strcpy (table.Traslate, Traslate);

   for (i = 0; i < MAXCOST; i++) {
      table.Cost[i].Days = Cost[i].Days;
      table.Cost[i].Start = Cost[i].Start;
      table.Cost[i].Stop = Cost[i].Stop;
      table.Cost[i].CostFirst = Cost[i].CostFirst;
      table.Cost[i].TimeFirst = Cost[i].TimeFirst;
      table.Cost[i].Cost = Cost[i].Cost;
      table.Cost[i].Time = Cost[i].Time;
   }
}

VOID TTranslation::Struct2Class (VOID)
{
   int i;

   strcpy (Location, table.Location);
   strcpy (Search, table.Search);
   strcpy (Traslate, table.Traslate);

   for (i = 0; i < MAXCOST; i++) {
      Cost[i].Days = table.Cost[i].Days;
      Cost[i].Start = table.Cost[i].Start;
      Cost[i].Stop = table.Cost[i].Stop;
      Cost[i].CostFirst = table.Cost[i].CostFirst;
      Cost[i].TimeFirst = table.Cost[i].TimeFirst;
      Cost[i].Cost = table.Cost[i].Cost;
      Cost[i].Time = table.Cost[i].Time;
   }
}

VOID TTranslation::Add (VOID)
{
   USHORT DoClose = FALSE;

   if (fdDat == -1) {
      fdDat = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1) {
      Class2Struct ();
      lseek (fdDat, 0L, SEEK_END);
      write (fdDat, &table, sizeof (table));
   }

   if (DoClose == TRUE && fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }
}

VOID TTranslation::DeleteAll (VOID)
{
   if (fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }

   unlink (DataFile);
}

USHORT TTranslation::First (VOID)
{
   if (fdDat == -1)
      fdDat = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);

   if (fdDat != -1)
      lseek (fdDat, 0L, SEEK_SET);

   return (Next ());
}

VOID TTranslation::Export (PSZ pszFile)
{
   FILE *fp;
   int i;
   CHAR string[64];

   if ((fp = fopen (pszFile, "wt")) != NULL) {
      if (First () == TRUE)
         do {
            if (Search[0] == '\0')
               strcpy (Search, "/");
            if (Traslate[0] == '\0')
               strcpy (Traslate, "/");
            fprintf (fp, "\nPrefix %s %s \"%s\"\n", Search, Traslate, Location);

            for (i = 0; i < MAXCOST; i++) {
               if (Cost[i].Days == 0)
                  continue;
               strcpy (string, "-------");
               if (Cost[i].Days & DAY_SUNDAY)
                  string[0] = 'S';
               if (Cost[i].Days & DAY_MONDAY)
                  string[1] = 'M';
               if (Cost[i].Days & DAY_TUESDAY)
                  string[2] = 'T';
               if (Cost[i].Days & DAY_WEDNESDAY)
                  string[3] = 'W';
               if (Cost[i].Days & DAY_THURSDAY)
                  string[4] = 'T';
               if (Cost[i].Days & DAY_FRIDAY)
                  string[5] = 'F';
               if (Cost[i].Days & DAY_SATURDAY)
                  string[6] = 'S';
               fprintf (fp, "    %s %2d:%02d-%2d:%02d %4d %3d.%d %4d %3d.%d\n",
                           string,
                           Cost[i].Start / 60, Cost[i].Start % 60,
                           Cost[i].Stop / 60, Cost[i].Stop % 60,
                           Cost[i].CostFirst, Cost[i].TimeFirst / 10, Cost[i].TimeFirst % 10,
                           Cost[i].Cost, Cost[i].Time / 10, Cost[i].Time % 10);
            }
         } while (Next () == TRUE);
      fclose (fp);
   }
}

USHORT TTranslation::Import (PSZ pszFile)
{
   FILE *fp;
   int i, t1, t2;
   USHORT RetVal = FALSE;
   CHAR Temp[128], *p;

   if ((fp = fopen (pszFile, "rt")) != NULL) {
      DeleteAll ();
      RetVal = TRUE;
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Temp[strlen (Temp) - 1] = '\0';
         if ((p = strtok (Temp, " ")) == NULL)
            continue;
         if (stricmp (p, "Prefix"))
            continue;

         New ();
         if ((p = strtok (NULL, " ")) != NULL) {
            strcpy (Search, p);
            if ((p = strtok (NULL, " ")) != NULL) {
               if (strcmp (p, "/"))
                  strcpy (Traslate, p);
            }
            if ((p = strtok (NULL, "\"")) != NULL)
               strcpy (Location, p);

            for (i = 0; i < MAXCOST; i++) {
               fgets (Temp, sizeof (Temp) - 1, fp);
               Temp[strlen (Temp) - 1] = '\0';
               if ((p = strtok (Temp, " ")) == NULL)
                  break;
               if (toupper (p[0]) == 'S')
                  Cost[i].Days |= DAY_SUNDAY;
               if (toupper (p[1]) == 'M')
                  Cost[i].Days |= DAY_MONDAY;
               if (toupper (p[2]) == 'T')
                  Cost[i].Days |= DAY_TUESDAY;
               if (toupper (p[3]) == 'W')
                  Cost[i].Days |= DAY_WEDNESDAY;
               if (toupper (p[4]) == 'T')
                  Cost[i].Days |= DAY_THURSDAY;
               if (toupper (p[5]) == 'F')
                  Cost[i].Days |= DAY_FRIDAY;
               if (toupper (p[6]) == 'S')
                  Cost[i].Days |= DAY_SATURDAY;

               if ((p = strtok (NULL, " -")) == NULL)
                  break;
               sscanf (p, "%d:%d", &t1, &t2);
               Cost[i].Start = (USHORT)(t1 * 60 + t2);

               if ((p = strtok (NULL, " ")) == NULL)
                  break;
               sscanf (p, "%d:%d", &t1, &t2);
               Cost[i].Stop = (USHORT)(t1 * 60 + t2);

               if ((p = strtok (NULL, " ")) == NULL)
                  break;
               Cost[i].CostFirst = (USHORT)atoi (p);
               if ((p = strtok (NULL, " ")) == NULL)
                  break;
               if (sscanf (p, "%d.%d", &t1, &t2) == 1)
                  Cost[i].TimeFirst = (USHORT)(t1 * 10);
               else
                  Cost[i].TimeFirst = (USHORT)(t1 * 10 + t2);

               if ((p = strtok (NULL, " ")) == NULL)
                  break;
               Cost[i].Cost = (USHORT)atoi (p);
               if ((p = strtok (NULL, " ")) == NULL)
                  break;
               if (sscanf (p, "%d.%d", &t1, &t2) == 1)
                  Cost[i].Time = (USHORT)(t1 * 10);
               else
                  Cost[i].Time = (USHORT)(t1 * 10 + t2);

               Add ();
            }
         }
      }
      fclose (fp);
   }

   return (RetVal);
}

USHORT TTranslation::Next (VOID)
{
   USHORT RetVal = FALSE;

   if (fdDat != -1) {
      if (read (fdDat, &table, sizeof (table)) == sizeof (table)) {
         Struct2Class ();
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

VOID TTranslation::New (VOID)
{
   int i;

   Location[0] = '\0';
   Search[0] = '\0';
   Traslate[0] = '\0';

   for (i = 0; i < MAXCOST; i++) {
      Cost[i].Days = 0;
      Cost[i].Start = 0;
      Cost[i].Stop = 0;
      Cost[i].CostFirst = 0;
      Cost[i].TimeFirst = 0;
      Cost[i].Cost = 0;
      Cost[i].Time = 0;
   }
}

USHORT TTranslation::Read (PSZ pszName)
{
   USHORT RetVal = FALSE, DoClose = FALSE;

   if (fdDat == -1) {
      fdDat = open (DataFile, O_RDWR|O_BINARY|O_CREAT, S_IREAD|S_IWRITE);
      DoClose = TRUE;
   }

   if (fdDat != -1) {
      lseek (fdDat, 0L, SEEK_SET);
      while (read (fdDat, &table, sizeof (table)) == sizeof (table)) {
         if (!strncmp (table.Search, pszName, strlen (table.Search))) {
            Struct2Class ();
            RetVal = TRUE;
            break;
         }
      }
   }

   if (DoClose == TRUE && fdDat != -1) {
      close (fdDat);
      fdDat = -1;
   }

   return (RetVal);
}

VOID TTranslation::Update (VOID)
{
   if (fdDat != -1) {
      lseek (fdDat, tell (fdDat) - sizeof (table), SEEK_SET);
      Class2Struct ();
      write (fdDat, &table, sizeof (table));
   }
}


