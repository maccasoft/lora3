
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/05/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "user.h"

TLastRead::TLastRead (void)
{
   Data.Clear ();
}

TLastRead::~TLastRead (void)
{
   Data.Clear ();
}

VOID TLastRead::Add (VOID)
{
   LASTREAD lr;

   memset (&lr, 0, sizeof (lr));
   strcpy (lr.Key, Key);
   lr.Number = Number;
   lr.QuickScan = QuickScan;
   lr.PersonalOnly = PersonalOnly;
   lr.ExcludeOwn = ExcludeOwn;
   Data.Add (&lr, sizeof (lr));
}

VOID TLastRead::Clear (VOID)
{
   Data.Clear ();
}

USHORT TLastRead::First (VOID)
{
   USHORT RetVal = FALSE;
   LASTREAD *lr;

   if ((lr = (LASTREAD *)Data.First ()) != NULL) {
      strcpy (Key, lr->Key);
      Number = lr->Number;
      QuickScan = lr->QuickScan;
      PersonalOnly = lr->PersonalOnly;
      ExcludeOwn = lr->ExcludeOwn;
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TLastRead::Next (VOID)
{
   USHORT RetVal = FALSE;
   LASTREAD *lr;

   if ((lr = (LASTREAD *)Data.Next ()) != NULL) {
      strcpy (Key, lr->Key);
      Number = lr->Number;
      QuickScan = lr->QuickScan;
      PersonalOnly = lr->PersonalOnly;
      ExcludeOwn = lr->ExcludeOwn;
      RetVal = TRUE;
   }

   return (RetVal);
}

VOID TLastRead::New (VOID)
{
   memset (Key, 0, sizeof (Key));
   Number = 0L;
   QuickScan = FALSE;
   PersonalOnly = FALSE;
   ExcludeOwn = FALSE;
}

USHORT TLastRead::Read (PSZ pszKey)
{
   USHORT RetVal = FALSE;
   LASTREAD *lr;

   if ((lr = (LASTREAD *)Data.First ()) != NULL)
      do {
         if (!stricmp (pszKey, lr->Key)) {
            strcpy (Key, lr->Key);
            Number = lr->Number;
            QuickScan = lr->QuickScan;
            PersonalOnly = lr->PersonalOnly;
            ExcludeOwn = lr->ExcludeOwn;
            RetVal = TRUE;
         }
      } while (RetVal == FALSE && (lr = (LASTREAD *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TLastRead::Remove (VOID)
{
   USHORT RetVal = FALSE;
   LASTREAD *lr;

   if ((lr = (LASTREAD *)Data.First ()) != NULL)
      do {
         if (!stricmp (Key, lr->Key)) {
            strcpy (Key, lr->Key);
            Number = lr->Number;
            QuickScan = lr->QuickScan;
            PersonalOnly = lr->PersonalOnly;
            ExcludeOwn = lr->ExcludeOwn;
            RetVal = TRUE;
         }
      } while (RetVal == FALSE && (lr = (LASTREAD *)Data.Next ()) != NULL);

   if (RetVal == TRUE)
      Data.Remove ();
}

VOID TLastRead::Update (VOID)
{
   USHORT RetVal = FALSE;
   LASTREAD *lr;

   if ((lr = (LASTREAD *)Data.First ()) != NULL)
      do {
         if (!stricmp (Key, lr->Key))
            RetVal = TRUE;
      } while (RetVal == FALSE && (lr = (LASTREAD *)Data.Next ()) != NULL);

   if (RetVal == TRUE) {
      strcpy (lr->Key, Key);
      lr->Number = Number;
      lr->QuickScan = QuickScan;
      lr->PersonalOnly = PersonalOnly;
      lr->ExcludeOwn = ExcludeOwn;
   }
}



