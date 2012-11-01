
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 2.99.20
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"
#include "lorawin.h"

VOID DisplayButton (USHORT y, USHORT x, CHAR *Text, USHORT Shadow)
{
   USHORT ButtonSize = 8;
   CHAR Temp[48];

   if (strlen (Text) > ButtonSize)
      ButtonSize = (USHORT)(strlen (Text) + 1);
   Temp[ButtonSize + 1] = '\0';

   memset (Temp, ' ', ButtonSize + 1);
   Temp[ButtonSize] = 'Ü';
   wprints (y, x, Shadow, Temp);

   memset (&Temp[1], 'ß', ButtonSize);
   wprints ((short)(y + 1), x, Shadow, Temp);

   Temp[ButtonSize] = '\0';
   memset (Temp, ' ', ButtonSize);
   memcpy (&Temp[(ButtonSize - strlen (Text)) / 2], Text, strlen (Text));
   wprints (y, x, BLACK|_GREEN, Temp);
}

VOID DisplayTextField (USHORT y, USHORT x, CHAR *Text, USHORT FieldSize, USHORT Size)
{
   CHAR Temp[128];

   if (FieldSize < Size)
      Size = FieldSize;

   memset (Temp, ' ', Size);
   Temp[Size] = '\0';
   wprints (y, x, WHITE|_BLUE, Temp);
   strcpy (Temp, Text);
   if (strlen (Temp) > Size)
      Temp[Size] = '\0';
   wprints (y, x, WHITE|_BLUE, Temp);
}

VOID GetTextField (USHORT y, USHORT x, CHAR *Text, USHORT FieldSize, USHORT Size)
{
   CHAR Temp[128], String[128];

   if (FieldSize < Size)
      Size = FieldSize;
   memset (Temp, '?', Size);
   Temp[Size] = '\0';

   strcpy (String, Text);
   if (strlen (String) > Size)
      String[Size] = '\0';
   winpbeg (WHITE|_BLUE, WHITE|_BLUE);
   winpdef (y, x, String, Temp, 0, 2, NULL, 0);
   if (winpread () != W_ESCPRESS)
      strcpy (Text, strtrim (String));
   hidecur ();
}

USHORT MessageBox (PSZ Caption, PSZ Text)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (9, 24, 15, 55, 1, WHITE|_RED, WHITE|_RED);
   wshadow (DGREY|_BLACK);
   wtitle (Caption, TCENTER, WHITE|_LGREY);

   DisplayButton (3, 2, "   Ok   ", BLACK|_RED);
   DisplayButton (3, 12, " Cancel ", BLACK|_RED);

   wprints (1, 2, WHITE|_RED, Text);

   do {
      wmenubegc ();
      wmenuitem (3, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (3, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      switch (menu_sel = wmenuget ()) {
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CFlagsDlg (PSZ title, ULONG *flags)
{
   int menu_sel = 1;
   USHORT RetVal = FALSE;
   ULONG Flags;

   Flags = *flags;

   wopen (7, 20, 19, 59, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (title, TCENTER, WHITE|_LGREY);

   DisplayButton (9,  2, "   Ok   ");
   DisplayButton (9, 12, " Cancel ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " A1 ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " A2 ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " A3 ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " A4 ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " A5 ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " A6 ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " A7 ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (7, 1, " A8 ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (0, 10, " B1 ", 0, 9, 0, NULL, 0, 0);
      wmenuitem (1, 10, " B2 ", 0, 10, 0, NULL, 0, 0);
      wmenuitem (2, 10, " B3 ", 0, 11, 0, NULL, 0, 0);
      wmenuitem (3, 10, " B4 ", 0, 12, 0, NULL, 0, 0);
      wmenuitem (4, 10, " B5 ", 0, 13, 0, NULL, 0, 0);
      wmenuitem (5, 10, " B6 ", 0, 14, 0, NULL, 0, 0);
      wmenuitem (6, 10, " B7 ", 0, 15, 0, NULL, 0, 0);
      wmenuitem (7, 10, " B8 ", 0, 16, 0, NULL, 0, 0);
      wmenuitem (0, 19, " C1 ", 0, 17, 0, NULL, 0, 0);
      wmenuitem (1, 19, " C2 ", 0, 18, 0, NULL, 0, 0);
      wmenuitem (2, 19, " C3 ", 0, 19, 0, NULL, 0, 0);
      wmenuitem (3, 19, " C4 ", 0, 20, 0, NULL, 0, 0);
      wmenuitem (4, 19, " C5 ", 0, 21, 0, NULL, 0, 0);
      wmenuitem (5, 19, " C6 ", 0, 22, 0, NULL, 0, 0);
      wmenuitem (6, 19, " C7 ", 0, 23, 0, NULL, 0, 0);
      wmenuitem (7, 19, " C8 ", 0, 24, 0, NULL, 0, 0);
      wmenuitem (0, 28, " D1 ", 0, 25, 0, NULL, 0, 0);
      wmenuitem (1, 28, " D2 ", 0, 26, 0, NULL, 0, 0);
      wmenuitem (2, 28, " D3 ", 0, 27, 0, NULL, 0, 0);
      wmenuitem (3, 28, " D4 ", 0, 28, 0, NULL, 0, 0);
      wmenuitem (4, 28, " D5 ", 0, 29, 0, NULL, 0, 0);
      wmenuitem (5, 28, " D6 ", 0, 30, 0, NULL, 0, 0);
      wmenuitem (6, 28, " D7 ", 0, 31, 0, NULL, 0, 0);
      wmenuitem (7, 28, " D8 ", 0, 32, 0, NULL, 0, 0);

      wmenuitem (9,  2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (9, 12, " Cancel ", 'C', 995, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      wprints (0, 6, WHITE|_BLUE, (Flags & 0x80000000L) ? "Yes" : "No ");
      wprints (1, 6, WHITE|_BLUE, (Flags & 0x40000000L) ? "Yes" : "No ");
      wprints (2, 6, WHITE|_BLUE, (Flags & 0x20000000L) ? "Yes" : "No ");
      wprints (3, 6, WHITE|_BLUE, (Flags & 0x10000000L) ? "Yes" : "No ");
      wprints (4, 6, WHITE|_BLUE, (Flags & 0x08000000L) ? "Yes" : "No ");
      wprints (5, 6, WHITE|_BLUE, (Flags & 0x04000000L) ? "Yes" : "No ");
      wprints (6, 6, WHITE|_BLUE, (Flags & 0x02000000L) ? "Yes" : "No ");
      wprints (7, 6, WHITE|_BLUE, (Flags & 0x01000000L) ? "Yes" : "No ");
      wprints (0, 15, WHITE|_BLUE, (Flags & 0x00800000L) ? "Yes" : "No ");
      wprints (1, 15, WHITE|_BLUE, (Flags & 0x00400000L) ? "Yes" : "No ");
      wprints (2, 15, WHITE|_BLUE, (Flags & 0x00200000L) ? "Yes" : "No ");
      wprints (3, 15, WHITE|_BLUE, (Flags & 0x00100000L) ? "Yes" : "No ");
      wprints (4, 15, WHITE|_BLUE, (Flags & 0x00080000L) ? "Yes" : "No ");
      wprints (5, 15, WHITE|_BLUE, (Flags & 0x00040000L) ? "Yes" : "No ");
      wprints (6, 15, WHITE|_BLUE, (Flags & 0x00020000L) ? "Yes" : "No ");
      wprints (7, 15, WHITE|_BLUE, (Flags & 0x00010000L) ? "Yes" : "No ");
      wprints (0, 24, WHITE|_BLUE, (Flags & 0x00008000L) ? "Yes" : "No ");
      wprints (1, 24, WHITE|_BLUE, (Flags & 0x00004000L) ? "Yes" : "No ");
      wprints (2, 24, WHITE|_BLUE, (Flags & 0x00002000L) ? "Yes" : "No ");
      wprints (3, 24, WHITE|_BLUE, (Flags & 0x00001000L) ? "Yes" : "No ");
      wprints (4, 24, WHITE|_BLUE, (Flags & 0x00000800L) ? "Yes" : "No ");
      wprints (5, 24, WHITE|_BLUE, (Flags & 0x00000400L) ? "Yes" : "No ");
      wprints (6, 24, WHITE|_BLUE, (Flags & 0x00000200L) ? "Yes" : "No ");
      wprints (7, 24, WHITE|_BLUE, (Flags & 0x00000100L) ? "Yes" : "No ");
      wprints (0, 33, WHITE|_BLUE, (Flags & 0x00000080L) ? "Yes" : "No ");
      wprints (1, 33, WHITE|_BLUE, (Flags & 0x00000040L) ? "Yes" : "No ");
      wprints (2, 33, WHITE|_BLUE, (Flags & 0x00000020L) ? "Yes" : "No ");
      wprints (3, 33, WHITE|_BLUE, (Flags & 0x00000010L) ? "Yes" : "No ");
      wprints (4, 33, WHITE|_BLUE, (Flags & 0x00000008L) ? "Yes" : "No ");
      wprints (5, 33, WHITE|_BLUE, (Flags & 0x00000004L) ? "Yes" : "No ");
      wprints (6, 33, WHITE|_BLUE, (Flags & 0x00000002L) ? "Yes" : "No ");
      wprints (7, 33, WHITE|_BLUE, (Flags & 0x00000001L) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            Flags ^= 0x80000000L;
            break;
         case 2:
            Flags ^= 0x40000000L;
            break;
         case 3:
            Flags ^= 0x20000000L;
            break;
         case 4:
            Flags ^= 0x10000000L;
            break;
         case 5:
            Flags ^= 0x08000000L;
            break;
         case 6:
            Flags ^= 0x04000000L;
            break;
         case 7:
            Flags ^= 0x02000000L;
            break;
         case 8:
            Flags ^= 0x01000000L;
            break;
         case 9:
            Flags ^= 0x00800000L;
            break;
         case 10:
            Flags ^= 0x00400000L;
            break;
         case 11:
            Flags ^= 0x00200000L;
            break;
         case 12:
            Flags ^= 0x00100000L;
            break;
         case 13:
            Flags ^= 0x00080000L;
            break;
         case 14:
            Flags ^= 0x00040000L;
            break;
         case 15:
            Flags ^= 0x00020000L;
            break;
         case 16:
            Flags ^= 0x00010000L;
            break;
         case 17:
            Flags ^= 0x00008000L;
            break;
         case 18:
            Flags ^= 0x00004000L;
            break;
         case 19:
            Flags ^= 0x00002000L;
            break;
         case 20:
            Flags ^= 0x00001000L;
            break;
         case 21:
            Flags ^= 0x00000800L;
            break;
         case 22:
            Flags ^= 0x00000400L;
            break;
         case 23:
            Flags ^= 0x00000200L;
            break;
         case 24:
            Flags ^= 0x00000100L;
            break;
         case 25:
            Flags ^= 0x00000080L;
            break;
         case 26:
            Flags ^= 0x00000040L;
            break;
         case 27:
            Flags ^= 0x00000020L;
            break;
         case 28:
            Flags ^= 0x00000010L;
            break;
         case 29:
            Flags ^= 0x00000008L;
            break;
         case 30:
            Flags ^= 0x00000004L;
            break;
         case 31:
            Flags ^= 0x00000002L;
            break;
         case 32:
            Flags ^= 0x00000001L;
            break;
         case 996:
            *flags = Flags;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

VOID DisplayFlags (USHORT y, USHORT x, ULONG Flags)
{
   int i;
   CHAR Temp[48];
   ULONG Test;

   Test = 0x80000000L;

   strcpy (Temp, "--------");
   for (i = 0; i < 8; i++, Test >>= 1) {
      if (Flags & Test)
         Temp[i] = 'X';
   }
   wprints (y, x, WHITE|_BLUE, Temp);
   strcpy (Temp, "--------");
   for (i = 0; i < 8; i++, Test >>= 1) {
      if (Flags & Test)
         Temp[i] = 'X';
   }
   wprints (y, (SHORT)(x + 9), WHITE|_BLUE, Temp);
   strcpy (Temp, "--------");
   for (i = 0; i < 8; i++, Test >>= 1) {
      if (Flags & Test)
         Temp[i] = 'X';
   }
   wprints (y, (SHORT)(x + 18), WHITE|_BLUE, Temp);
   strcpy (Temp, "--------");
   for (i = 0; i < 8; i++, Test >>= 1) {
      if (Flags & Test)
         Temp[i] = 'X';
   }
   wprints (y, (SHORT)(x + 27), WHITE|_BLUE, Temp);
}

USHORT CSecurityDlg (PSZ title, USHORT *level, ULONG *flags, ULONG *deny_flags)
{
   int menu_sel = 1;
   CHAR Temp[48];
   USHORT RetVal = FALSE, Level;
   ULONG AccessFlags, DenyFlags;

   Level = *level;
   AccessFlags = *flags;
   DenyFlags = *deny_flags;

   wopen (9, 14, 16, 65, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (title, TCENTER, WHITE|_LGREY);

   DisplayButton (4,  2, "   Ok   ");
   DisplayButton (4, 12, " Cancel ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Level      ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Flags      ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Deny Flags ", 0, 3, 0, NULL, 0, 0);

      wmenuitem (4,  2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (4, 12, " Cancel ", 'C', 995, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      sprintf (Temp, "%u", Level);
      DisplayTextField (0, 14, Temp, 5, 5);
      DisplayFlags (1, 14, AccessFlags);
      DisplayFlags (2, 14, DenyFlags);

      switch (menu_sel = wmenuget ()) {
         case 1:
            sprintf (Temp, "%u", Level);
            GetTextField (0, 14, Temp, 5, 5);
            Level = (USHORT)atoi (Temp);
            break;
         case 2:
            CFlagsDlg (" Access Flags ", &AccessFlags);
            break;
         case 3:
            CFlagsDlg (" Deny Flags ", &DenyFlags);
            break;
         case 996:
            *level = Level;
            *flags = AccessFlags;
            *deny_flags = DenyFlags;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CRemoveAddressDlg (VOID)
{
   short i, start = 0;
   CHAR Temp[128], *p, **Array;
   USHORT RetVal = FALSE;
   class TCollection List;

   if (Cfg->MailAddress.First () == TRUE) {
      i = 0;
      List.Clear ();
      do {
         sprintf (Temp, " %-17.17s ", Cfg->MailAddress.String);
         List.Add (Temp);
         i++;
      } while (Cfg->MailAddress.Next () == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i = wpickstr (0 + 4 + 1, 1 + 22 + 1, 9 + 4 + 1, 19 + 22 + 1, 5, BLACK|_CYAN, BLACK|_CYAN, YELLOW|_CYAN, Array, start, NULL)) != -1) {
            if (Cfg->MailAddress.First () == TRUE)
               do {
                  sprintf (Temp, " %-17.17s ", Cfg->MailAddress.String);
                  if (!strcmp (Temp, Array[i])) {
                     Cfg->MailAddress.Delete ();
                     break;
                  }
               } while (Cfg->MailAddress.Next () == TRUE);
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();
   }
   else
      getxch ();

   return (RetVal);
}

USHORT CAddressDlg (VOID)
{
   short i, menu_sel = 996;
   CHAR Temp[64];
   USHORT RetVal = FALSE;

   wopen (4, 22, 20, 56, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Addresses ", TCENTER, WHITE|_LGREY);

   DisplayButton (1, 22, "  Add   ");
   DisplayButton (3, 22, " Remove ");

   DisplayButton (13, 2, "   Ok   ");
   DisplayButton (13, 12, " Cancel ");
   DisplayButton (13, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (1, 22, "  Add   ", 'A', 102, 0, NULL, 0, 0);
      wmenuitem (3, 22, " Remove ", 'R', 103, 0, NULL, 0, 0);

      wmenuitem (11, 1, " Point Fake Net ", 0, 1, 0, NULL, 0, 0);

      wmenuitem (13, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (13, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (13, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      for (i = 0; i <= 9; i++)
         wprints (i, 1, BLACK|_CYAN, "                   ");

      i = 0;
      if (Cfg->MailAddress.First () == TRUE)
         do {
            if (i <= 9) {
               sprintf (Temp, "%.17s", Cfg->MailAddress.String);
               wprints (i++, 2, BLACK|_CYAN, Temp);
            }
         } while (Cfg->MailAddress.Next () == TRUE);

      sprintf (Temp, "%u", Cfg->FakeNet);
      DisplayTextField (11, 18, Temp, 5, 5);

      switch (menu_sel = wmenuget ()) {
         case 1:
            sprintf (Temp, "%u", Cfg->FakeNet);
            GetTextField (11, 18, Temp, 5, 5);
            Cfg->FakeNet = (USHORT)atoi (Temp);
            break;
         case 102:
            if (wopen (10, 20, 12, 59, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
               wshadow (DGREY|_BLACK);
               wtitle (" Add Address ", TCENTER, WHITE|_LGREY);

               wprints (0, 1, WHITE|_GREEN, " Address ");
               Temp[0] = '\0';
               winpbeg (WHITE|_BLUE, WHITE|_BLUE);
               winpdef (0, 11, Temp, "??????????????????????????", 0, 2, NULL, 0);
               if (winpread () != W_ESCPRESS && Temp[0] != '\0')
                  Cfg->MailAddress.Add (Temp);
               hidecur ();
               wclose ();
            }
            break;
         case 103:
            CRemoveAddressDlg ();
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CAnswerDlg (VOID)
{
   short menu_sel = 996;
   CHAR Temp[32], *p;
   USHORT RetVal = FALSE;

   wopen (7, 12, 16, 66, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Answer Control ", TCENTER, WHITE|_LGREY);

   DisplayButton (6, 2, "   Ok   ");
   DisplayButton (6, 12, " Cancel ");
   DisplayButton (6, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Manual Answer  ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Answer Command ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Limited Hours  ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Starting Time  ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Ending Time    ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (6, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (6, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (6, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      wprints (0, 18, WHITE|_BLUE, (Cfg->ManualAnswer == TRUE) ? "Yes" : "No ");
      DisplayTextField (1, 18, Cfg->Answer, sizeof (Cfg->Answer), 34);
      wprints (2, 18, WHITE|_BLUE, (Cfg->LimitedHours == TRUE) ? "Yes" : "No ");
      sprintf (Temp, "%02d:%02d", Cfg->StartTime / 60, Cfg->StartTime % 60);
      DisplayTextField (3, 18, Temp, 5, 5);
      sprintf (Temp, "%02d:%02d", Cfg->EndTime / 60, Cfg->EndTime % 60);
      DisplayTextField (4, 18, Temp, 5, 5);

      switch (menu_sel = wmenuget ()) {
         case 1:
            Cfg->ManualAnswer = (Cfg->ManualAnswer == TRUE) ? FALSE : TRUE;
            break;
         case 2:
            GetTextField (1, 18, Cfg->Answer, sizeof (Cfg->Answer), 34);
            break;
         case 3:
            Cfg->LimitedHours = (Cfg->LimitedHours == TRUE) ? FALSE : TRUE;
            break;
         case 4:
            sprintf (Temp, "%02d:%02d", Cfg->StartTime / 60, Cfg->StartTime % 60);
            GetTextField (3, 12, Temp, 5, 5);
            if ((p = strtok (Temp, ":")) != NULL) {
               Cfg->StartTime = (USHORT)(atoi (p) * 60);
               if ((p = strtok (Temp, ":")) != NULL)
                  Cfg->StartTime += (USHORT)atoi (p);
            }
            break;
         case 5:
            sprintf (Temp, "%02d:%02d", Cfg->EndTime / 60, Cfg->EndTime % 60);
            GetTextField (4, 12, Temp, 5, 5);
            if ((p = strtok (Temp, ":")) != NULL) {
               Cfg->EndTime = (USHORT)(atoi (p) * 60);
               if ((p = strtok (Temp, ":")) != NULL)
                  Cfg->EndTime += (USHORT)atoi (p);
            }
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CAreafixDlg (VOID)
{
   short menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (8, 5, 17, 73, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Areafix ", TCENTER, WHITE|_LGREY);

   DisplayButton (6, 2, "   Ok   ");
   DisplayButton (6, 12, " Cancel ");
   DisplayButton (6, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " AREAS.BBS        ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Use AREAS.BBS    ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Update AREAS.BBS ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " New Areas Base   ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " New Areas Path   ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (6, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (6, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (6, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 20, Cfg->AreasBBS, sizeof (Cfg->AreasBBS), 66);
      wprints (1, 20, WHITE|_BLUE, (Cfg->UseAreasBBS == TRUE) ? "Yes" : "No ");
      wprints (2, 20, WHITE|_BLUE, (Cfg->UpdateAreasBBS == TRUE) ? "Yes" : "No ");
      switch (Cfg->NewAreasStorage) {
         case ST_JAM:
            wprints (3, 20, WHITE|_BLUE, "JAM             ");
            break;
         case ST_SQUISH:
            wprints (3, 20, WHITE|_BLUE, "Squish          ");
            break;
         case ST_FIDO:
            wprints (3, 20, WHITE|_BLUE, "Fido *.MSG      ");
            break;
         case ST_ADEPT:
            wprints (3, 20, WHITE|_BLUE, "AdeptXBBS       ");
            break;
         case ST_HUDSON:
            wprints (3, 20, WHITE|_BLUE, "Hudson (QBBS)   ");
            break;
         case ST_PASSTHROUGH:
            wprints (3, 20, WHITE|_BLUE, "Passthrough     ");
            break;
      }
      DisplayTextField (4, 20, Cfg->NewAreasPath, sizeof (Cfg->NewAreasPath), 66);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 20, Cfg->AreasBBS, sizeof (Cfg->AreasBBS), 66);
            break;
         case 2:
            Cfg->UseAreasBBS = (Cfg->UseAreasBBS == TRUE) ? FALSE : TRUE;
            break;
         case 3:
            Cfg->UpdateAreasBBS = (Cfg->UpdateAreasBBS == TRUE) ? FALSE : TRUE;
            break;
         case 4:
            switch (Cfg->NewAreasStorage) {
               case ST_JAM:
                  Cfg->NewAreasStorage = ST_SQUISH;
                  break;
               case ST_SQUISH:
                  Cfg->NewAreasStorage = ST_FIDO;
                  break;
               case ST_FIDO:
                  Cfg->NewAreasStorage = ST_ADEPT;
                  break;
               case ST_ADEPT:
                  Cfg->NewAreasStorage = ST_HUDSON;
                  break;
               case ST_HUDSON:
                  Cfg->NewAreasStorage = ST_PASSTHROUGH;
                  break;
               case ST_PASSTHROUGH:
                  Cfg->NewAreasStorage = ST_JAM;
                  break;
            }
            break;
         case 5:
            GetTextField (4, 20, Cfg->NewAreasPath, sizeof (Cfg->NewAreasPath), 66);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CCommandsDlg (VOID)
{
   short menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (7, 12, 17, 66, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Command Strings ", TCENTER, WHITE|_LGREY);

   DisplayButton (7, 2, "   Ok   ");
   DisplayButton (7, 12, " Cancel ");
   DisplayButton (7, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Init. 1  ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Init. 2  ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Init. 3  ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Dialing  ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Hangup   ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Off Hook ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (7, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (7, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (7, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 12, Cfg->Initialize[0], sizeof (Cfg->Initialize[0]), 40);
      DisplayTextField (1, 12, Cfg->Initialize[1], sizeof (Cfg->Initialize[1]), 40);
      DisplayTextField (2, 12, Cfg->Initialize[2], sizeof (Cfg->Initialize[2]), 40);
      DisplayTextField (3, 12, Cfg->Dial, sizeof (Cfg->Dial), 40);
      DisplayTextField (4, 12, Cfg->Hangup, sizeof (Cfg->Hangup), 40);
      DisplayTextField (5, 12, Cfg->OffHook, sizeof (Cfg->OffHook), 40);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 12, Cfg->Initialize[0], sizeof (Cfg->Initialize[0]), 40);
            break;
         case 2:
            GetTextField (1, 12, Cfg->Initialize[1], sizeof (Cfg->Initialize[1]), 40);
            break;
         case 3:
            GetTextField (2, 12, Cfg->Initialize[2], sizeof (Cfg->Initialize[2]), 40);
            break;
         case 4:
            GetTextField (3, 12, Cfg->Dial, sizeof (Cfg->Dial), 40);
            break;
         case 5:
            GetTextField (4, 12, Cfg->Hangup, sizeof (Cfg->Hangup), 40);
            break;
         case 6:
            GetTextField (5, 12, Cfg->OffHook, sizeof (Cfg->OffHook), 40);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CDirectoriesDlg (VOID)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (8, 5, 19, 73, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Directory / Paths ", TCENTER, WHITE|_LGREY);

   DisplayButton (8, 2, "   Ok   ");
   DisplayButton (8, 12, " Cancel ");
   DisplayButton (8, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " System Path   ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Outbound      ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Norm. Inbound ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Known Inbound ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Prot. Inbound ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Nodelist      ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Hudson Base   ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (8, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (8, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (8, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 17, Cfg->SystemPath, sizeof (Cfg->SystemPath), 49);
      DisplayTextField (1, 17, Cfg->Outbound, sizeof (Cfg->Outbound), 49);
      DisplayTextField (2, 17, Cfg->NormalInbound, sizeof (Cfg->NormalInbound), 49);
      DisplayTextField (3, 17, Cfg->KnownInbound, sizeof (Cfg->KnownInbound), 49);
      DisplayTextField (4, 17, Cfg->ProtectedInbound, sizeof (Cfg->ProtectedInbound), 49);
      DisplayTextField (5, 17, Cfg->NodelistPath, sizeof (Cfg->NodelistPath), 49);
      DisplayTextField (6, 17, Cfg->HudsonPath, sizeof (Cfg->HudsonPath), 49);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 17, Cfg->SystemPath, sizeof (Cfg->SystemPath), 49);
            break;
         case 2:
            GetTextField (1, 17, Cfg->Outbound, sizeof (Cfg->Outbound), 49);
            break;
         case 3:
            GetTextField (2, 17, Cfg->NormalInbound, sizeof (Cfg->NormalInbound), 49);
            break;
         case 4:
            GetTextField (3, 17, Cfg->KnownInbound, sizeof (Cfg->KnownInbound), 49);
            break;
         case 5:
            GetTextField (4, 17, Cfg->ProtectedInbound, sizeof (Cfg->ProtectedInbound), 49);
            break;
         case 6:
            GetTextField (5, 17, Cfg->NodelistPath, sizeof (Cfg->NodelistPath), 49);
            break;
         case 7:
            GetTextField (6, 17, Cfg->HudsonPath, sizeof (Cfg->HudsonPath), 49);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

// ----------------------------------------------------------------------
#define DAY_SUN      0x01
#define DAY_MON      0x02
#define DAY_TUE      0x04
#define DAY_WED      0x08
#define DAY_THU      0x10
#define DAY_FRI      0x20
#define DAY_SAT      0x40

USHORT CWeekDaysDlg (USHORT *Days)
{
   int menu_sel = 996;
   UCHAR Sun, Mon, Tue, Wed, Thu, Fri, Sat;
   USHORT RetVal = FALSE;

   Sun = (*Days & DAY_SUN) ? TRUE : FALSE;
   Mon = (*Days & DAY_MON) ? TRUE : FALSE;
   Tue = (*Days & DAY_TUE) ? TRUE : FALSE;
   Wed = (*Days & DAY_WED) ? TRUE : FALSE;
   Thu = (*Days & DAY_THU) ? TRUE : FALSE;
   Fri = (*Days & DAY_FRI) ? TRUE : FALSE;
   Sat = (*Days & DAY_SAT) ? TRUE : FALSE;

   wopen (7, 28, 18, 52, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Week Days ", TCENTER, WHITE|_LGREY);

   DisplayButton (8, 2, "   Ok   ");
   DisplayButton (8, 12, " Cancel ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Sunday    ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Monday    ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Tuesday   ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Wednesday ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Thursday  ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Friday    ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Saturday  ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (8, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (8, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      wprints (0, 13, WHITE|_BLUE, (Sun == TRUE) ? "Yes" : "No ");
      wprints (1, 13, WHITE|_BLUE, (Mon == TRUE) ? "Yes" : "No ");
      wprints (2, 13, WHITE|_BLUE, (Tue == TRUE) ? "Yes" : "No ");
      wprints (3, 13, WHITE|_BLUE, (Wed == TRUE) ? "Yes" : "No ");
      wprints (4, 13, WHITE|_BLUE, (Thu == TRUE) ? "Yes" : "No ");
      wprints (5, 13, WHITE|_BLUE, (Fri == TRUE) ? "Yes" : "No ");
      wprints (6, 13, WHITE|_BLUE, (Sat == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            Sun = (Sun == TRUE) ? FALSE : TRUE;
            break;
         case 2:
            Mon = (Mon == TRUE) ? FALSE : TRUE;
            break;
         case 3:
            Tue = (Tue == TRUE) ? FALSE : TRUE;
            break;
         case 4:
            Wed = (Wed == TRUE) ? FALSE : TRUE;
            break;
         case 5:
            Thu = (Thu == TRUE) ? FALSE : TRUE;
            break;
         case 6:
            Fri = (Fri == TRUE) ? FALSE : TRUE;
            break;
         case 7:
            Sat = (Sat == TRUE) ? FALSE : TRUE;
            break;
         case 996:
            RetVal = TRUE;
            *Days = 0;
            *Days |= (Sun == TRUE) ? DAY_SUN : 0;
            *Days |= (Mon == TRUE) ? DAY_MON : 0;
            *Days |= (Tue == TRUE) ? DAY_TUE : 0;
            *Days |= (Wed == TRUE) ? DAY_WED : 0;
            *Days |= (Thu == TRUE) ? DAY_THU : 0;
            *Days |= (Fri == TRUE) ? DAY_FRI : 0;
            *Days |= (Sat == TRUE) ? DAY_SAT : 0;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CAddEventDlg (USHORT *Start, USHORT *Length, USHORT *WeekDays)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE, Hour, Minute, Len, Days;
   CHAR Temp[16], *p;
   UCHAR Sun, Mon, Tue, Wed, Thu, Fri, Sat;

   Days = 0;
   Hour = Minute = Len = 0;

   Sun = (Days & DAY_SUN) ? TRUE : FALSE;
   Mon = (Days & DAY_MON) ? TRUE : FALSE;
   Tue = (Days & DAY_TUE) ? TRUE : FALSE;
   Wed = (Days & DAY_WED) ? TRUE : FALSE;
   Thu = (Days & DAY_THU) ? TRUE : FALSE;
   Fri = (Days & DAY_FRI) ? TRUE : FALSE;
   Sat = (Days & DAY_SAT) ? TRUE : FALSE;

   wopen (9, 28, 16, 52, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Add Event ", TCENTER, WHITE|_LGREY);

   DisplayButton (4, 2, "   Ok   ");
   DisplayButton (4, 12, " Cancel ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Week Days  ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Start Time ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Length     ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (4, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (4, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      strcpy (Temp, "-------");
      if (Sun == TRUE)
         Temp[0] = 'S';
      if (Mon == TRUE)
         Temp[1] = 'M';
      if (Tue == TRUE)
         Temp[2] = 'T';
      if (Wed == TRUE)
         Temp[3] = 'W';
      if (Thu == TRUE)
         Temp[4] = 'T';
      if (Fri == TRUE)
         Temp[5] = 'F';
      if (Sat == TRUE)
         Temp[6] = 'S';
      wprints (0, 14, WHITE|_BLUE, Temp);
      sprintf (Temp, "%02d:%02d", Hour, Minute);
      DisplayTextField (1, 14, Temp, 5, 5);
      sprintf (Temp, "%d", Len);
      DisplayTextField (2, 14, Temp, 5, 5);

      switch (menu_sel = wmenuget ()) {
         case 1:
            Days = 0;
            Days |= (Sun == TRUE) ? DAY_SUN : 0;
            Days |= (Mon == TRUE) ? DAY_MON : 0;
            Days |= (Tue == TRUE) ? DAY_TUE : 0;
            Days |= (Wed == TRUE) ? DAY_WED : 0;
            Days |= (Thu == TRUE) ? DAY_THU : 0;
            Days |= (Fri == TRUE) ? DAY_FRI : 0;
            Days |= (Sat == TRUE) ? DAY_SAT : 0;
            if (CWeekDaysDlg (&Days) == TRUE) {
               Sun = (Days & DAY_SUN) ? TRUE : FALSE;
               Mon = (Days & DAY_MON) ? TRUE : FALSE;
               Tue = (Days & DAY_TUE) ? TRUE : FALSE;
               Wed = (Days & DAY_WED) ? TRUE : FALSE;
               Thu = (Days & DAY_THU) ? TRUE : FALSE;
               Fri = (Days & DAY_FRI) ? TRUE : FALSE;
               Sat = (Days & DAY_SAT) ? TRUE : FALSE;
            }
            break;
         case 2:
            sprintf (Temp, "%02d:%02d", Hour, Minute);
            GetTextField (1, 14, Temp, 5, 5);
            if ((p = strtok (Temp, ":")) != NULL) {
               Hour = (USHORT)atoi (p);
               if ((p = strtok (NULL, ":")) != NULL)
                  Minute = (USHORT)atoi (p);
            }
            break;
         case 3:
            sprintf (Temp, "%d", Len);
            GetTextField (2, 14, Temp, 5, 5);
            Len = (USHORT)atoi (Temp);
            break;
         case 996:
            RetVal = TRUE;
            *Start = (USHORT)(Hour * 60 + Minute);
            *Length = Len;
            *WeekDays = 0;
            *WeekDays |= (Sun == TRUE) ? DAY_SUN : 0;
            *WeekDays |= (Mon == TRUE) ? DAY_MON : 0;
            *WeekDays |= (Tue == TRUE) ? DAY_TUE : 0;
            *WeekDays |= (Wed == TRUE) ? DAY_WED : 0;
            *WeekDays |= (Thu == TRUE) ? DAY_THU : 0;
            *WeekDays |= (Fri == TRUE) ? DAY_FRI : 0;
            *WeekDays |= (Sat == TRUE) ? DAY_SAT : 0;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CEventDlg (VOID)
{
   int menu_sel = 1;
   USHORT RetVal = FALSE, Days;
   CHAR Temp[64], *p;
   class TEvents *Data;

   if ((Data = new TEvents (Cfg->SchedulerFile)) != NULL)
      Data->Load ();
   if (Data->First () == FALSE)
      Data->New ();

   wopen (4, 5, 21, 74, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Event Scheduler ", TCENTER, WHITE|_LGREY);

   DisplayButton (14,  2, "  Save  ");
   DisplayButton (14, 12, "  Add   ");
   DisplayButton (14, 22, " Delete ");
   DisplayButton (14, 32, "  List  ");
   DisplayButton (14, 47, "  Prev. ");
   DisplayButton (14, 57, "  Next  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Title         ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Week Days     ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Start Time    ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (2, 23, " Length        ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Dynamic       ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (3, 23, " Mail Only     ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (3, 45, " Import Normal ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Forced        ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (4, 23, " Allow F.Req.  ", 0, 9, 0, NULL, 0, 0);
      wmenuitem (4, 45, " Import Prot.  ", 0, 10, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Clock Adjust. ", 0, 11, 0, NULL, 0, 0);
      wmenuitem (5, 23, " Make F.Req.   ", 0, 12, 0, NULL, 0, 0);
      wmenuitem (5, 45, " Import Known  ", 0, 13, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Max Try       ", 0, 14, 0, NULL, 0, 0);
      wmenuitem (6, 23, " Send Normal   ", 0, 15, 0, NULL, 0, 0);
      wmenuitem (6, 45, " Export Mail   ", 0, 16, 0, NULL, 0, 0);
      wmenuitem (7, 1, " Max Failed    ", 0, 17, 0, NULL, 0, 0);
      wmenuitem (7, 23, " Send Direct   ", 0, 18, 0, NULL, 0, 0);
      wmenuitem (7, 45, " Start Import  ", 0, 19, 0, NULL, 0, 0);
      wmenuitem (8, 1, " Call Delay    ", 0, 20, 0, NULL, 0, 0);
      wmenuitem (8, 23, " Send Crash    ", 0, 21, 0, NULL, 0, 0);
      wmenuitem (8, 45, " Start Export  ", 0, 22, 0, NULL, 0, 0);
      wmenuitem (9, 1, " Forced Poll   ", 0, 23, 0, NULL, 0, 0);
      wmenuitem (9, 23, " Send Immed.   ", 0, 24, 0, NULL, 0, 0);
      wmenuitem (9, 45, " Process TIC   ", 0, 25, 0, NULL, 0, 0);
      wmenuitem (10, 1, " Reserved For  ", 0, 26, 0, NULL, 0, 0);
      wmenuitem (11, 1, " Route Tag     ", 0, 27, 0, NULL, 0, 0);
      wmenuitem (12, 1, " Command       ", 0, 28, 0, NULL, 0, 0);

      wmenuitem (14,  2, "  Save  ", 'S', 996, 0, NULL, 0, 0);
      wmenuitem (14, 12, "  Add   ", 'A', 115, 0, NULL, 0, 0);
      wmenuitem (14, 22, " Delete ", 'D', 116, 0, NULL, 0, 0);
      wmenuitem (14, 32, "  List  ", 'L', 117, 0, NULL, 0, 0);
      wmenuitem (14, 47, "  Prev. ", 'P', 118, 0, NULL, 0, 0);
      wmenuitem (14, 57, "  Next  ", 'N', 119, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 17, Data->Label, sizeof (Data->Label), 31);
      strcpy (Temp, "-------");
      if (Data->Sunday == TRUE)
         Temp[0] = 'S';
      if (Data->Monday == TRUE)
         Temp[1] = 'M';
      if (Data->Tuesday == TRUE)
         Temp[2] = 'T';
      if (Data->Wednesday == TRUE)
         Temp[3] = 'W';
      if (Data->Thursday == TRUE)
         Temp[4] = 'T';
      if (Data->Friday == TRUE)
         Temp[5] = 'F';
      if (Data->Saturday == TRUE)
         Temp[6] = 'S';
      wprints (1, 17, WHITE|_BLUE, Temp);
      sprintf (Temp, "%02d:%02d", Data->Hour, Data->Minute);
      DisplayTextField (2, 17, Temp, 5, 5);
      sprintf (Temp, "%d", Data->Length);
      DisplayTextField (2, 39, Temp, 5, 5);
      wprints (3, 17, WHITE|_BLUE, (Data->Dynamic == TRUE) ? "Yes" : "No ");
      wprints (3, 39, WHITE|_BLUE, (Data->MailOnly == TRUE) ? "Yes" : "No ");
      wprints (3, 61, WHITE|_BLUE, (Data->ImportNormal == TRUE) ? "Yes" : "No ");
      wprints (4, 17, WHITE|_BLUE, (Data->Force == TRUE) ? "Yes" : "No ");
      wprints (4, 39, WHITE|_BLUE, (Data->AllowRequests == TRUE) ? "Yes" : "No ");
      wprints (4, 61, WHITE|_BLUE, (Data->ImportProtected == TRUE) ? "Yes" : "No ");
      wprints (5, 39, WHITE|_BLUE, (Data->MakeRequests == TRUE) ? "Yes" : "No ");
      wprints (5, 61, WHITE|_BLUE, (Data->ImportKnown == TRUE) ? "Yes" : "No ");
      sprintf (Temp, "%d", Data->MaxCalls);
      DisplayTextField (6, 17, Temp, 5, 5);
      wprints (6, 39, WHITE|_BLUE, (Data->SendNormal == TRUE) ? "Yes" : "No ");
      wprints (6, 61, WHITE|_BLUE, (Data->ExportMail == TRUE) ? "Yes" : "No ");
      sprintf (Temp, "%d", Data->MaxConnects);
      DisplayTextField (7, 17, Temp, 5, 5);
      wprints (7, 39, WHITE|_BLUE, (Data->SendDirect == TRUE) ? "Yes" : "No ");
      wprints (7, 61, WHITE|_BLUE, (Data->StartImport == TRUE) ? "Yes" : "No ");
      sprintf (Temp, "%d", Data->CallDelay);
      DisplayTextField (8, 17, Temp, 5, 5);
      wprints (8, 39, WHITE|_BLUE, (Data->SendCrash == TRUE) ? "Yes" : "No ");
      wprints (8, 61, WHITE|_BLUE, (Data->StartExport == TRUE) ? "Yes" : "No ");
      wprints (9, 17, WHITE|_BLUE, (Data->ForceCall == TRUE) ? "Yes" : "No ");
      wprints (9, 39, WHITE|_BLUE, (Data->SendImmediate == TRUE) ? "Yes" : "No ");
      wprints (9, 61, WHITE|_BLUE, (Data->ProcessTIC == TRUE) ? "Yes" : "No ");
      DisplayTextField (10, 17, Data->Address, sizeof (Data->Address), 32);
      DisplayTextField (11, 17, Data->RouteCmd, sizeof (Data->RouteCmd), 32);
      DisplayTextField (12, 17, Data->Command, sizeof (Data->Command), 50);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 17, Data->Label, sizeof (Data->Label), 31);
            break;
         case 2:
            Days = 0;
            Days |= (Data->Sunday == TRUE) ? DAY_SUN : 0;
            Days |= (Data->Monday == TRUE) ? DAY_MON : 0;
            Days |= (Data->Tuesday == TRUE) ? DAY_TUE : 0;
            Days |= (Data->Wednesday == TRUE) ? DAY_WED : 0;
            Days |= (Data->Thursday == TRUE) ? DAY_THU : 0;
            Days |= (Data->Friday == TRUE) ? DAY_FRI : 0;
            Days |= (Data->Saturday == TRUE) ? DAY_SAT : 0;
            if (CWeekDaysDlg (&Days) == TRUE) {
               Data->Sunday = (Days & DAY_SUN) ? TRUE : FALSE;
               Data->Monday = (Days & DAY_MON) ? TRUE : FALSE;
               Data->Tuesday = (Days & DAY_TUE) ? TRUE : FALSE;
               Data->Wednesday = (Days & DAY_WED) ? TRUE : FALSE;
               Data->Thursday = (Days & DAY_THU) ? TRUE : FALSE;
               Data->Friday = (Days & DAY_FRI) ? TRUE : FALSE;
               Data->Saturday = (Days & DAY_SAT) ? TRUE : FALSE;
            }
            break;
         case 3:
            sprintf (Temp, "%02d:%02d", Data->Hour, Data->Minute);
            GetTextField (2, 17, Temp, 5, 5);
            if ((p = strtok (Temp, ":")) != NULL) {
               Data->Hour = (UCHAR)atoi (p);
               if ((p = strtok (NULL, ":")) != NULL)
                  Data->Minute = (UCHAR)atoi (p);
            }
            break;
         case 4:
            sprintf (Temp, "%d", Data->Length);
            GetTextField (2, 39, Temp, 5, 5);
            Data->Length = (USHORT)atoi (Temp);
            break;
         case 5:
            Data->Dynamic = (Data->Dynamic == TRUE) ? FALSE : TRUE;
            break;
         case 6:
            Data->MailOnly = (Data->MailOnly == TRUE) ? FALSE : TRUE;
            break;
         case 7:
            Data->ImportNormal = (Data->ImportNormal == TRUE) ? FALSE : TRUE;
            break;
         case 8:
            Data->Force = (Data->Force == TRUE) ? FALSE : TRUE;
            break;
         case 9:
            Data->AllowRequests = (Data->AllowRequests == TRUE) ? FALSE : TRUE;
            break;
         case 10:
            Data->ImportProtected = (Data->ImportProtected == TRUE) ? FALSE : TRUE;
            break;
         case 12:
            Data->MakeRequests = (Data->MakeRequests == TRUE) ? FALSE : TRUE;
            break;
         case 13:
            Data->ImportKnown = (Data->ImportKnown == TRUE) ? FALSE : TRUE;
            break;
         case 14:
            sprintf (Temp, "%d", Data->MaxCalls);
            GetTextField (6, 17, Temp, 5, 5);
            Data->MaxCalls = (USHORT)atoi (Temp);
            break;
         case 15:
            Data->SendNormal = (Data->SendNormal == TRUE) ? FALSE : TRUE;
            break;
         case 16:
            Data->ExportMail = (Data->ExportMail == TRUE) ? FALSE : TRUE;
            break;
         case 17:
            sprintf (Temp, "%d", Data->MaxConnects);
            GetTextField (7, 17, Temp, 5, 5);
            Data->MaxConnects = (USHORT)atoi (Temp);
            break;
         case 18:
            Data->SendDirect = (Data->SendDirect == TRUE) ? FALSE : TRUE;
            break;
         case 19:
            Data->StartImport = (Data->StartImport == TRUE) ? FALSE : TRUE;
            break;
         case 20:
            sprintf (Temp, "%d", Data->CallDelay);
            GetTextField (8, 17, Temp, 5, 5);
            Data->CallDelay = (USHORT)atoi (Temp);
            break;
         case 21:
            Data->SendCrash = (Data->SendCrash == TRUE) ? FALSE : TRUE;
            break;
         case 22:
            Data->StartExport = (Data->StartExport == TRUE) ? FALSE : TRUE;
            break;
         case 23:
            Data->ForceCall = (Data->ForceCall == TRUE) ? FALSE : TRUE;
            break;
         case 24:
            Data->SendImmediate = (Data->SendImmediate == TRUE) ? FALSE : TRUE;
            break;
         case 25:
            Data->ProcessTIC = (Data->ProcessTIC == TRUE) ? FALSE : TRUE;
            break;
         case 26:
            GetTextField (10, 17, Data->Address, sizeof (Data->Address), 32);
            break;
         case 27:
            GetTextField (11, 17, Data->RouteCmd, sizeof (Data->RouteCmd), 32);
            break;
         case 28:
            GetTextField (12, 17, Data->Command, sizeof (Data->Command), 50);
            break;
         case 115: {
            USHORT NewStart, NewLength, NewDays;

            if (CAddEventDlg (&NewStart, &NewLength, &NewDays) == TRUE) {
               Data->New ();
               Data->Sunday = (NewDays & DAY_SUN) ? TRUE : FALSE;
               Data->Monday = (NewDays & DAY_MON) ? TRUE : FALSE;
               Data->Tuesday = (NewDays & DAY_TUE) ? TRUE : FALSE;
               Data->Wednesday = (NewDays & DAY_WED) ? TRUE : FALSE;
               Data->Thursday = (NewDays & DAY_THU) ? TRUE : FALSE;
               Data->Friday = (NewDays & DAY_FRI) ? TRUE : FALSE;
               Data->Saturday = (NewDays & DAY_SAT) ? TRUE : FALSE;
               Data->Hour = (UCHAR)(NewStart / 60);
               Data->Minute = (UCHAR)(NewStart % 60);
               Data->Length = NewLength;
               Data->Add ();
            }
            break;
         }
         case 116:
            if (MessageBox ("Delete Event", "Are you really sure ?") == TRUE)
               Data->Delete ();
            break;
         case 118:
            Data->Previous ();
            break;
         case 119:
            Data->Next ();
            break;
         case 996:
            Data->Update ();
            Data->Save ();
            break;
      }
   } while (menu_sel != -1);

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CExternalProcDlg (VOID)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (8, 5, 17, 73, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" External Processing ", TCENTER, WHITE|_LGREY);

   DisplayButton (6, 2, "   Ok   ");
   DisplayButton (6, 12, " Cancel ");
   DisplayButton (6, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Import      ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Export      ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Pack        ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Single Pass ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Newsgroups  ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (6, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (6, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (6, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 15, Cfg->ImportCmd, sizeof (Cfg->ImportCmd), 71);
      DisplayTextField (1, 15, Cfg->ExportCmd, sizeof (Cfg->ExportCmd), 71);
      DisplayTextField (2, 15, Cfg->PackCmd, sizeof (Cfg->PackCmd), 71);
      DisplayTextField (3, 15, Cfg->SinglePassCmd, sizeof (Cfg->SinglePassCmd), 71);
      DisplayTextField (4, 15, Cfg->NewsgroupCmd, sizeof (Cfg->NewsgroupCmd), 71);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 15, Cfg->ImportCmd, sizeof (Cfg->ImportCmd), 71);
            break;
         case 2:
            GetTextField (1, 15, Cfg->ExportCmd, sizeof (Cfg->ExportCmd), 71);
            break;
         case 3:
            GetTextField (2, 15, Cfg->PackCmd, sizeof (Cfg->PackCmd), 71);
            break;
         case 4:
            GetTextField (3, 15, Cfg->SinglePassCmd, sizeof (Cfg->SinglePassCmd), 71);
            break;
         case 5:
            GetTextField (4, 15, Cfg->NewsgroupCmd, sizeof (Cfg->NewsgroupCmd), 71);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CGeneralOptDlg (VOID)
{
   int menu_sel = 996;
   CHAR Temp[64];
   USHORT RetVal = FALSE;

   wopen (7, 5, 18, 73, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" General Options ", TCENTER, WHITE|_LGREY);

   DisplayButton (8, 2, "   Ok   ");
   DisplayButton (8, 12, " Cancel ");
   DisplayButton (8, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Task Number   ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Log File      ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Events File   ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Fax Command   ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " After Caller  ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " After Mail    ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " ZModem Telnet ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (8, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (8, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (8, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      sprintf (Temp, "%u", Cfg->TaskNumber);
      DisplayTextField (0, 17, Temp, 5, 5);
      DisplayTextField (1, 17, Cfg->LogFile, sizeof (Cfg->LogFile), 49);
      DisplayTextField (2, 17, Cfg->SchedulerFile, sizeof (Cfg->SchedulerFile), 49);
      DisplayTextField (3, 17, Cfg->FaxCommand, sizeof (Cfg->FaxCommand), 49);
      DisplayTextField (4, 17, Cfg->AfterCallerCmd, sizeof (Cfg->AfterCallerCmd), 49);
      DisplayTextField (5, 17, Cfg->AfterMailCmd, sizeof (Cfg->AfterMailCmd), 49);
      wprints (6, 17, WHITE|_BLUE, (Cfg->ZModemTelnet == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            sprintf (Temp, "%u", Cfg->TaskNumber);
            GetTextField (0, 17, Temp, 5, 5);
            Cfg->TaskNumber = (USHORT)atoi (Temp);
            break;
         case 2:
            GetTextField (1, 17, Cfg->LogFile, sizeof (Cfg->LogFile), 49);
            break;
         case 3:
            GetTextField (2, 17, Cfg->SchedulerFile, sizeof (Cfg->SchedulerFile), 49);
            break;
         case 4:
            GetTextField (3, 17, Cfg->FaxCommand, sizeof (Cfg->FaxCommand), 49);
            break;
         case 5:
            GetTextField (4, 17, Cfg->AfterCallerCmd, sizeof (Cfg->AfterCallerCmd), 49);
            break;
         case 6:
            GetTextField (5, 17, Cfg->AfterMailCmd, sizeof (Cfg->AfterMailCmd), 49);
            break;
         case 7:
            Cfg->ZModemTelnet = (Cfg->ZModemTelnet == TRUE) ? FALSE : TRUE;
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CHardwareDlg (VOID)
{
   int menu_sel = 996;
   CHAR Temp[64];
   USHORT RetVal = FALSE;

   wopen (6, 12, 18, 66, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Modem Hardware ", TCENTER, WHITE|_LGREY);

   DisplayButton (9, 2, "   Ok   ");
   DisplayButton (9, 12, " Cancel ");
   DisplayButton (9, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Modem Port       ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Baud Rate        ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Lock Speed       ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Strip Dashes     ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Fax Message      ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Ring Message     ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Dial Timeout     ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (7, 1, " DCD Drop Timeout ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (9, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (9, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (9, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 20, Cfg->Device, sizeof (Cfg->Device), 32);
      sprintf (Temp, "%lu", Cfg->Speed);
      DisplayTextField (1, 20, Temp, 6, 6);
      wprints (2, 20, WHITE|_BLUE, (Cfg->LockSpeed == TRUE) ? "Yes" : "No ");
      wprints (3, 20, WHITE|_BLUE, (Cfg->StripDashes == TRUE) ? "Yes" : "No ");
      DisplayTextField (4, 20, Cfg->FaxMessage, sizeof (Cfg->FaxMessage), 32);
      DisplayTextField (5, 20, Cfg->Ring, sizeof (Cfg->Ring), 32);
      sprintf (Temp, "%u", Cfg->DialTimeout);
      DisplayTextField (6, 20, Temp, 6, 6);
      sprintf (Temp, "%lu", Cfg->CarrierDropTimeout);
      DisplayTextField (7, 20, Temp, 6, 6);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 20, Cfg->Device, sizeof (Cfg->Device), 32);
            break;
         case 2:
            sprintf (Temp, "%lu", Cfg->Speed);
            GetTextField (1, 20, Temp, 6, 6);
            Cfg->Speed = atol (Temp);
            break;
         case 3:
            Cfg->LockSpeed = (Cfg->LockSpeed == TRUE) ? FALSE : TRUE;
            break;
         case 4:
            Cfg->StripDashes = (Cfg->StripDashes == TRUE) ? FALSE : TRUE;
            break;
         case 5:
            GetTextField (4, 20, Cfg->FaxMessage, sizeof (Cfg->FaxMessage), 32);
            break;
         case 6:
            GetTextField (5, 20, Cfg->Ring, sizeof (Cfg->Ring), 32);
            break;
         case 7:
            sprintf (Temp, "%u", Cfg->DialTimeout);
            GetTextField (6, 20, Temp, 6, 6);
            Cfg->DialTimeout = (USHORT)atoi (Temp);
            break;
         case 8:
            sprintf (Temp, "%lu", Cfg->CarrierDropTimeout);
            GetTextField (7, 20, Temp, 6, 6);
            Cfg->CarrierDropTimeout = (USHORT)atoi (Temp);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CFaxDlg (VOID)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (8, 2, 16, 76, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Fax Options ", TCENTER, WHITE|_LGREY);

   DisplayButton (5, 2, "   Ok   ");
   DisplayButton (5, 12, " Cancel ");
   DisplayButton (5, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Use external recv. ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " External receiver  ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Receive path       ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " After receive cmd. ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (5, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (5, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (5, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      wprints (0, 22, WHITE|_BLUE, (Cfg->ExternalFax == TRUE) ? "Yes" : "No ");
      DisplayTextField (1, 22, Cfg->FaxCommand, sizeof (Cfg->FaxCommand), 50);
      DisplayTextField (2, 22, Cfg->FaxPath, sizeof (Cfg->FaxPath), 50);
      DisplayTextField (3, 22, Cfg->AfterFaxCmd, sizeof (Cfg->AfterFaxCmd), 50);

      switch (menu_sel = wmenuget ()) {
         case 1:
            Cfg->ExternalFax = (Cfg->ExternalFax == TRUE) ? FALSE : TRUE;
            break;
         case 2:
            GetTextField (1, 22, Cfg->FaxCommand, sizeof (Cfg->FaxCommand), 50);
            break;
         case 3:
            GetTextField (2, 22, Cfg->FaxPath, sizeof (Cfg->FaxPath), 50);
            break;
         case 4:
            GetTextField (3, 22, Cfg->AfterFaxCmd, sizeof (Cfg->AfterFaxCmd), 50);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CInternetDlg (VOID)
{
   short menu_sel = 996;
   CHAR Temp[16];
   USHORT RetVal = FALSE;

   wopen (8, 6, 19, 71, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Internet Options ", TCENTER, WHITE|_LGREY);

   DisplayButton (8, 2, "   Ok   ");
   DisplayButton (8, 12, " Cancel ");
   DisplayButton (8, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Host Name   ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " News Server ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Mail Server ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Enable PPP  ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Time Limit  ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Command     ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Max Msgs.   ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (8, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (8, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (8, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend (menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 15, Cfg->HostName, sizeof (Cfg->HostName), 48);
      DisplayTextField (1, 15, Cfg->NewsServer, sizeof (Cfg->NewsServer), 48);
      DisplayTextField (2, 15, Cfg->MailServer, sizeof (Cfg->MailServer), 48);
      wprints (3, 15, WHITE|_BLUE, (Cfg->EnablePPP == TRUE) ? "Yes" : "No ");
      sprintf (Temp, "%u", Cfg->PPPTimeLimit);
      DisplayTextField (4, 15, Temp, 5, 5);
      DisplayTextField (5, 15, Cfg->PPPCmd, sizeof (Cfg->PPPCmd), 48);
      sprintf (Temp, "%u", Cfg->RetriveMaxMessages);
      DisplayTextField (6, 15, Temp, 5, 5);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 15, Cfg->HostName, sizeof (Cfg->HostName), 49);
            break;
         case 2:
            GetTextField (1, 15, Cfg->NewsServer, sizeof (Cfg->NewsServer), 49);
            break;
         case 3:
            GetTextField (2, 15, Cfg->MailServer, sizeof (Cfg->MailServer), 49);
            break;
         case 4:
            Cfg->EnablePPP = (Cfg->EnablePPP == TRUE) ? FALSE : TRUE;
            break;
         case 5:
            sprintf (Temp, "%u", Cfg->PPPTimeLimit);
            GetTextField (4, 15, Temp, 5, 5);
            Cfg->PPPTimeLimit = (USHORT)atoi (Temp);
            break;
         case 6:
            GetTextField (5, 15, Cfg->PPPCmd, sizeof (Cfg->PPPCmd), 49);
            break;
         case 7:
            sprintf (Temp, "%u", Cfg->RetriveMaxMessages);
            GetTextField (6, 15, Temp, 5, 5);
            Cfg->RetriveMaxMessages = (USHORT)atoi (Temp);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CMailerMiscDlg (VOID)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (8, 5, 17, 73, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Miscellaneous ", TCENTER, WHITE|_LGREY);

   DisplayButton (6, 2, "   Ok   ");
   DisplayButton (6, 12, " Cancel ");
   DisplayButton (6, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Mail-only ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Enter-BBS ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " WaZOO     ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (2, 17, " Secure Mail   ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (3, 1, " EMSI      ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (3, 17, " Keep Netmail  ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Janus     ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (4, 17, " Track Netmail ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (6, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (6, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (6, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 13, Cfg->MailOnly, sizeof (Cfg->MailOnly), 53);
      DisplayTextField (1, 13, Cfg->EnterBBS, sizeof (Cfg->EnterBBS), 53);
      wprints (2, 13, WHITE|_BLUE, (Cfg->WaZoo == TRUE) ? "Yes" : "No ");
      wprints (2, 33, WHITE|_BLUE, (Cfg->Secure == TRUE) ? "Yes" : "No ");
      wprints (3, 13, WHITE|_BLUE, (Cfg->EMSI == TRUE) ? "Yes" : "No ");
      wprints (3, 33, WHITE|_BLUE, (Cfg->KeepNetMail == TRUE) ? "Yes" : "No ");
      wprints (4, 13, WHITE|_BLUE, (Cfg->Janus == TRUE) ? "Yes" : "No ");
      wprints (4, 33, WHITE|_BLUE, (Cfg->TrackNetMail == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 13, Cfg->MailOnly, sizeof (Cfg->MailOnly), 53);
            break;
         case 2:
            GetTextField (1, 13, Cfg->EnterBBS, sizeof (Cfg->EnterBBS), 53);
            break;
         case 3:
            Cfg->WaZoo = (Cfg->WaZoo == TRUE) ? FALSE : TRUE;
            break;
         case 4:
            Cfg->Secure = (Cfg->Secure == TRUE) ? FALSE : TRUE;
            break;
         case 5:
            Cfg->EMSI = (Cfg->EMSI == TRUE) ? FALSE : TRUE;
            break;
         case 6:
            Cfg->KeepNetMail = (Cfg->KeepNetMail == TRUE) ? FALSE : TRUE;
            break;
         case 7:
            Cfg->Janus = (Cfg->Janus == TRUE) ? FALSE : TRUE;
            break;
         case 8:
            Cfg->TrackNetMail = (Cfg->TrackNetMail == TRUE) ? FALSE : TRUE;
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CMailProcessingDlg (VOID)
{
   int menu_sel = 996;
   CHAR Temp[16];
   USHORT RetVal = FALSE;

   wopen (6, 4, 20, 75, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Mail Processing ", TCENTER, WHITE|_LGREY);

   DisplayButton (11, 2, "   Ok   ");
   DisplayButton (11, 12, " Cancel ");
   DisplayButton (11, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " NetMail Storage  ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (0, 37, " Board ", 0, 13, 0, NULL, 0, 0);
      wmenuitem (1, 1, " NetMail Path     ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Dupes Storage    ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (2, 37, " Board ", 0, 14, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Dupes Path       ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Bad Storage      ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (4, 37, " Board ", 0, 15, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Bad Path         ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " Import Empty     ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (6, 24, " Separate NetMail ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (7, 1, " Force INTL       ", 0, 9, 0, NULL, 0, 0);
      wmenuitem (7, 24, " Single Pass      ", 0, 10, 0, NULL, 0, 0);
      wmenuitem (8, 1, " Replace Tearline ", 0, 11, 0, NULL, 0, 0);
      wmenuitem (9, 1, " Tearline         ", 0, 12, 0, NULL, 0, 0);
      wmenuitem (11, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (11, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (11, 22, "  Help  ", 'H', 998, M_NOSEL, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      switch (Cfg->NetMailStorage) {
         case ST_JAM:
            wprints (0, 20, WHITE|_BLUE, "JAM             ");
            break;
         case ST_SQUISH:
            wprints (0, 20, WHITE|_BLUE, "Squish          ");
            break;
         case ST_FIDO:
            wprints (0, 20, WHITE|_BLUE, "Fido *.MSG      ");
            break;
         case ST_ADEPT:
            wprints (0, 20, WHITE|_BLUE, "AdeptXBBS       ");
            break;
         case ST_HUDSON:
            wprints (0, 20, WHITE|_BLUE, "Hudson (QBBS)   ");
            break;
      }
      sprintf (Temp, "%u", Cfg->NetMailBoard);
      DisplayTextField (0, 45, Temp, 5, 5);
      DisplayTextField (1, 20, Cfg->NetMailPath, sizeof (Cfg->NetMailPath), 49);
      switch (Cfg->DupeStorage) {
         case ST_JAM:
            wprints (2, 20, WHITE|_BLUE, "JAM             ");
            break;
         case ST_SQUISH:
            wprints (2, 20, WHITE|_BLUE, "Squish          ");
            break;
         case ST_FIDO:
            wprints (2, 20, WHITE|_BLUE, "Fido *.MSG      ");
            break;
         case ST_ADEPT:
            wprints (2, 20, WHITE|_BLUE, "AdeptXBBS       ");
            break;
         case ST_HUDSON:
            wprints (2, 20, WHITE|_BLUE, "Hudson (QBBS)   ");
            break;
      }
      sprintf (Temp, "%u", Cfg->DupeBoard);
      DisplayTextField (2, 45, Temp, 5, 5);
      DisplayTextField (3, 20, Cfg->DupePath, sizeof (Cfg->DupePath), 49);
      switch (Cfg->BadStorage) {
         case ST_JAM:
            wprints (4, 20, WHITE|_BLUE, "JAM             ");
            break;
         case ST_SQUISH:
            wprints (4, 20, WHITE|_BLUE, "Squish          ");
            break;
         case ST_FIDO:
            wprints (4, 20, WHITE|_BLUE, "Fido *.MSG      ");
            break;
         case ST_ADEPT:
            wprints (4, 20, WHITE|_BLUE, "AdeptXBBS       ");
            break;
         case ST_HUDSON:
            wprints (4, 20, WHITE|_BLUE, "Hudson (QBBS)   ");
            break;
      }
      sprintf (Temp, "%u", Cfg->BadBoard);
      DisplayTextField (4, 45, Temp, 5, 5);
      DisplayTextField (5, 20, Cfg->BadPath, sizeof (Cfg->BadPath), 49);
      wprints (6, 20, WHITE|_BLUE, (Cfg->ImportEmpty == TRUE) ? "Yes" : "No ");
      wprints (6, 43, WHITE|_BLUE, (Cfg->SeparateNetMail == TRUE) ? "Yes" : "No ");
      wprints (7, 20, WHITE|_BLUE, (Cfg->ForceIntl == TRUE) ? "Yes" : "No ");
      wprints (7, 43, WHITE|_BLUE, (Cfg->UseSinglePass == TRUE) ? "Yes" : "No ");
      wprints (8, 20, WHITE|_BLUE, (Cfg->ReplaceTear == TRUE) ? "Yes" : "No ");
      DisplayTextField (9, 20, Cfg->TearLine, sizeof (Cfg->TearLine), 49);

      switch (menu_sel = wmenuget ()) {
         case 1:
            switch (Cfg->NetMailStorage) {
               case ST_JAM:
                  Cfg->NetMailStorage = ST_SQUISH;
                  break;
               case ST_SQUISH:
                  Cfg->NetMailStorage = ST_FIDO;
                  break;
               case ST_FIDO:
                  Cfg->NetMailStorage = ST_ADEPT;
                  break;
               case ST_ADEPT:
                  Cfg->NetMailStorage = ST_HUDSON;
                  break;
               case ST_HUDSON:
                  Cfg->NetMailStorage = ST_JAM;
                  break;
            }
            break;
         case 2:
            GetTextField (1, 20, Cfg->NetMailPath, sizeof (Cfg->NetMailPath), 49);
            break;
         case 3:
            switch (Cfg->DupeStorage) {
               case ST_JAM:
                  Cfg->DupeStorage = ST_SQUISH;
                  break;
               case ST_SQUISH:
                  Cfg->DupeStorage = ST_FIDO;
                  break;
               case ST_FIDO:
                  Cfg->DupeStorage = ST_ADEPT;
                  break;
               case ST_ADEPT:
                  Cfg->DupeStorage = ST_HUDSON;
                  break;
               case ST_HUDSON:
                  Cfg->DupeStorage = ST_JAM;
                  break;
            }
            break;
         case 4:
            GetTextField (3, 20, Cfg->DupePath, sizeof (Cfg->DupePath), 49);
            break;
         case 5:
            switch (Cfg->BadStorage) {
               case ST_JAM:
                  Cfg->BadStorage = ST_SQUISH;
                  break;
               case ST_SQUISH:
                  Cfg->BadStorage = ST_FIDO;
                  break;
               case ST_FIDO:
                  Cfg->BadStorage = ST_ADEPT;
                  break;
               case ST_ADEPT:
                  Cfg->BadStorage = ST_HUDSON;
                  break;
               case ST_HUDSON:
                  Cfg->BadStorage = ST_JAM;
                  break;
            }
            break;
         case 6:
            GetTextField (5, 20, Cfg->BadPath, sizeof (Cfg->BadPath), 49);
            break;
         case 7:
            Cfg->ImportEmpty = (Cfg->ImportEmpty == TRUE) ? FALSE : TRUE;
            break;
         case 8:
            Cfg->SeparateNetMail = (Cfg->SeparateNetMail == TRUE) ? FALSE : TRUE;
            break;
         case 9:
            Cfg->ForceIntl = (Cfg->ForceIntl == TRUE) ? FALSE : TRUE;
            break;
         case 10:
            Cfg->UseSinglePass = (Cfg->UseSinglePass == TRUE) ? FALSE : TRUE;
            break;
         case 11:
            Cfg->ReplaceTear = (Cfg->ReplaceTear == TRUE) ? FALSE : TRUE;
            break;
         case 12:
            GetTextField (9, 20, Cfg->TearLine, sizeof (Cfg->TearLine), 49);
            break;
         case 13:
            sprintf (Temp, "%u", Cfg->NetMailBoard);
            GetTextField (0, 45, Temp, 5, 5);
            Cfg->NetMailBoard = (USHORT)atoi (Temp);
            break;
         case 14:
            sprintf (Temp, "%u", Cfg->DupeBoard);
            GetTextField (2, 45, Temp, 5, 5);
            Cfg->DupeBoard = (USHORT)atoi (Temp);
            break;
         case 15:
            sprintf (Temp, "%u", Cfg->BadBoard);
            GetTextField (4, 45, Temp, 5, 5);
            Cfg->BadBoard = (USHORT)atoi (Temp);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CAddNodeDlg (PSZ Address, PSZ System, PSZ Sysop)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (9, 6, 16, 72, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Add Node ", TCENTER, WHITE|_LGREY);

   DisplayButton (4, 2, "   Ok   ");
   DisplayButton (4, 12, " Cancel ");
   DisplayButton (4, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Address     ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " System Name ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Sysop Name  ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (4, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (4, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (4, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 15, Address, 31, 31);
      DisplayTextField (1, 15, System, 54, 54);
      DisplayTextField (2, 15, Sysop, 54, 54);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 15, Address, 31, 31);
            break;
         case 2:
            GetTextField (1, 15, System, 49, 49);
            break;
         case 3:
            GetTextField (2, 15, Sysop, 31, 31);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT COtherNodeDlg (class TNodes *Data)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (8, 8, 17, 70, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Other Node Options ", TCENTER, WHITE|_LGREY);

   DisplayButton (6, 2, "   Ok   ");
   DisplayButton (6, 12, " Cancel ");
   DisplayButton (6, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Dial Command       ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " E-Mail Address     ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Import E-Mail      ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " POP3 Password      ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Use E-Mail Address ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (6, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (6, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (6, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 22, Data->DialCmd, sizeof (Data->DialCmd), 36);
      DisplayTextField (1, 22, Data->InetAddress, sizeof (Data->InetAddress), 38);
      wprints (2, 22, WHITE|_BLUE, (Data->ImportPOP3Mail == TRUE) ? "Yes" : "No ");
      DisplayTextField (3, 22, Data->Pop3Pwd, sizeof (Data->Pop3Pwd), 36);
      wprints (4, 22, WHITE|_BLUE, (Data->UseInetAddress == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 22, Data->DialCmd, sizeof (Data->DialCmd), 36);
            break;
         case 2:
            GetTextField (1, 22, Data->InetAddress, sizeof (Data->InetAddress), 38);
            break;
         case 3:
            Data->ImportPOP3Mail = (Data->ImportPOP3Mail == TRUE) ? FALSE : TRUE;
            break;
         case 4:
            GetTextField (3, 22, Data->Pop3Pwd, sizeof (Data->Pop3Pwd), 36);
            break;
         case 5:
            Data->UseInetAddress = (Data->UseInetAddress == TRUE) ? FALSE : TRUE;
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CNodeSecurityDlg (class TNodes *Data)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (8, 13, 17, 65, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Node Security ", TCENTER, WHITE|_LGREY);

   DisplayButton (6, 2, "   Ok   ");
   DisplayButton (6, 12, " Cancel ");
   DisplayButton (6, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Session Pwd.    ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " In Packet Pwd.  ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Out Packet Pwd. ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " AreaMgr Pwd.    ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " TIC Pwd.        ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (6, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (6, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (6, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 19, Data->SessionPwd, sizeof (Data->SessionPwd), 31);
      DisplayTextField (1, 19, Data->InPktPwd, sizeof (Data->InPktPwd), 8);
      DisplayTextField (2, 19, Data->OutPktPwd, sizeof (Data->OutPktPwd), 8);
      DisplayTextField (3, 19, Data->AreaMgrPwd, sizeof (Data->AreaMgrPwd), 31);
      DisplayTextField (4, 19, Data->TicPwd, sizeof (Data->TicPwd), 31);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 19, Data->SessionPwd, sizeof (Data->SessionPwd), 31);
            break;
         case 2:
            GetTextField (1, 19, Data->InPktPwd, sizeof (Data->InPktPwd), 8);
            break;
         case 3:
            GetTextField (2, 19, Data->OutPktPwd, sizeof (Data->OutPktPwd), 8);
            break;
         case 4:
            GetTextField (3, 19, Data->AreaMgrPwd, sizeof (Data->AreaMgrPwd), 31);
            break;
         case 5:
            GetTextField (4, 19, Data->TicPwd, sizeof (Data->TicPwd), 31);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CNodesListDlg (PSZ address)
{
   short i, start = 0;
   CHAR Temp[128], *p, **Array;
   USHORT RetVal = FALSE;
   class TNodes *Data;
   class TCollection List;

   wopen (7, 2, 23, 77, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Nodes List ", TCENTER, WHITE|_LGREY);

   sprintf (Temp, " %-18.18s %-26.26s %-26.26s ", "Address", "System Name", "Sysop Name");
   wprints (0, 0, YELLOW|_LGREY, Temp);

   Data = new TNodes (Cfg->NodelistPath);
   if (Data->First () == TRUE) {
      i = 0;
      List.Clear ();
      do {
         sprintf (Temp, " %-18.18s %-26.26s %-26.26s ", Data->Address, Data->SystemName, Data->SysopName);
         List.Add (Temp);
         if (!strcmp (Data->Address, address))
            start = i;
         i++;
      } while (Data->Next () == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i = wpickstr (9, 3, 22, 76, 5, WHITE|_LGREY, WHITE|_LGREY, WHITE|_BLUE, Array, start, NULL)) != -1) {
            p = Array[i] + 1;
            p[18] = '\0';
            strcpy (address, strtrim (p));
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();
   }
   else
      getxch ();

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

USHORT CNodesDlg (VOID)
{
   int menu_sel = 1;
   USHORT RetVal = FALSE;
   class TNodes *Data;

   Data = new TNodes (Cfg->NodelistPath);
   if (Data->First () == FALSE)
      Data->New ();

   wopen (5, 5, 20, 74, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Nodes ", TCENTER, WHITE|_LGREY);

   DisplayButton (10, 32, "  Other ");
   DisplayButton (10, 47, "    Security     ");

   DisplayButton (12,  2, " Change ");
   DisplayButton (12, 12, "  Add   ");
   DisplayButton (12, 22, " Delete ");
   DisplayButton (12, 32, "  List  ");
   DisplayButton (12, 47, "  Prev. ");
   DisplayButton (12, 57, "  Next  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Address        ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " System Name    ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Sysop Name     ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Location       ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Phone Number   ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Nodelist Flags ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " New EchoMail   ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (7, 1, " Areas Filter   ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (8, 1, " Compressor     ", 0, 9, 0, NULL, 0, 0);

      wmenuitem (2, 50, " Remap Mail  ", 0, 10, 0, NULL, 0, 0);

      wmenuitem (10, 32, "  Other ", 'O', 122, 0, NULL, 0, 0);
      wmenuitem (10, 47, "    Security      ", 'S', 123, 0, NULL, 0, 0);

      wmenuitem (12,  2, " Change ", 'C', 996, 0, NULL, 0, 0);
      wmenuitem (12, 12, "  Add   ", 'A', 115, 0, NULL, 0, 0);
      wmenuitem (12, 22, " Delete ", 'D', 116, 0, NULL, 0, 0);
      wmenuitem (12, 32, "  List  ", 'L', 117, 0, NULL, 0, 0);
      wmenuitem (12, 47, "  Prev. ", 'P', 118, 0, NULL, 0, 0);
      wmenuitem (12, 57, "  Next  ", 'N', 119, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 18, Data->Address, sizeof (Data->Address), 31);
      DisplayTextField (1, 18, Data->SystemName, sizeof (Data->SystemName), 49);
      DisplayTextField (2, 18, Data->SysopName, sizeof (Data->SysopName), 31);
      DisplayTextField (3, 18, Data->Location, sizeof (Data->Location), 31);
      DisplayTextField (4, 18, Data->Phone, sizeof (Data->Phone), 31);
      DisplayTextField (5, 18, Data->Flags, sizeof (Data->Flags), 49);
      wprints (6, 18, WHITE|_BLUE, (Data->CreateNewAreas == TRUE) ? "Yes" : "No ");
      DisplayTextField (7, 18, Data->NewAreasFilter, sizeof (Data->NewAreasFilter), 49);
      DisplayTextField (8, 18, Data->Packer, sizeof (Data->Packer), 31);

      wprints (2, 64, WHITE|_BLUE, (Data->RemapMail == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 18, Data->Address, sizeof (Data->Address), 31);
            break;
         case 2:
            GetTextField (1, 18, Data->SystemName, sizeof (Data->SystemName), 49);
            break;
         case 3:
            GetTextField (2, 18, Data->SysopName, sizeof (Data->SysopName), 31);
            break;
         case 4:
            GetTextField (3, 18, Data->Location, sizeof (Data->Location), 31);
            break;
         case 5:
            GetTextField (4, 18, Data->Phone, sizeof (Data->Phone), 31);
            break;
         case 6:
            GetTextField (5, 18, Data->Flags, sizeof (Data->Flags), 49);
            break;
         case 7:
            Data->CreateNewAreas = (Data->CreateNewAreas == TRUE) ? FALSE : TRUE;
            break;
         case 8:
            GetTextField (7, 18, Data->NewAreasFilter, sizeof (Data->NewAreasFilter), 49);
            break;
         case 9:
            GetTextField (8, 18, Data->Packer, sizeof (Data->Packer), 31);
            break;
         case 10:
            Data->RemapMail = (Data->RemapMail == TRUE) ? FALSE : TRUE;
            break;
         case 115: {
            CHAR Address[32], System[50], Sysop[32];

            Address[0] = System[0] = Sysop[0] = '\0';
            if (CAddNodeDlg (Address, System, Sysop) == TRUE) {
               Data->New ();
               strcpy (Data->Address, Address);
               strcpy (Data->SystemName, System);
               strcpy (Data->SysopName, Sysop);
               Data->Add ();
            }
            break;
         }
         case 116:
            if (MessageBox ("Delete Node", "Are you really sure ?") == TRUE)
               Data->Delete ();
            break;
         case 117: {
            CHAR Address[32];

            strcpy (Address, Data->Address);
            if (CNodesListDlg (Address) == TRUE)
               Data->Read (Address);
            break;
         }
         case 118:
            Data->Previous ();
            break;
         case 119:
            Data->Next ();
            break;
         case 122:
            COtherNodeDlg (Data);
            break;
         case 123:
            CNodeSecurityDlg (Data);
            break;
         case 996:
            Data->Update ();
            break;
      }
   } while (menu_sel != -1);

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CSiteInfoDlg (VOID)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (9, 5, 18, 73, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Site Informations ", TCENTER, WHITE|_LGREY);

   DisplayButton (6, 2, "   Ok   ");
   DisplayButton (6, 12, " Cancel ");
   DisplayButton (6, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " System Name  ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Sysop        ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Location     ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Phone Number ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Flags        ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (6, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (6, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (6, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 16, Cfg->SystemName, sizeof (Cfg->SystemName), 50);
      DisplayTextField (1, 16, Cfg->SysopName, sizeof (Cfg->SysopName), 50);
      DisplayTextField (2, 16, Cfg->Location, sizeof (Cfg->Location), 50);
      DisplayTextField (3, 16, Cfg->Phone, sizeof (Cfg->Phone), 40);
      DisplayTextField (4, 16, Cfg->NodelistFlags, sizeof (Cfg->NodelistFlags), 50);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 16, Cfg->SystemName, sizeof (Cfg->SystemName), 50);
            break;
         case 2:
            GetTextField (1, 16, Cfg->SysopName, sizeof (Cfg->SysopName), 50);
            break;
         case 3:
            GetTextField (2, 16, Cfg->Location, sizeof (Cfg->Location), 50);
            break;
         case 4:
            GetTextField (3, 16, Cfg->Phone, sizeof (Cfg->Phone), 40);
            break;
         case 5:
            GetTextField (4, 16, Cfg->NodelistFlags, sizeof (Cfg->NodelistFlags), 50);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CCompressorListDlg (PSZ Key)
{
   short i, start = 0;
   CHAR Temp[128], *p, **Array;
   USHORT RetVal = FALSE;
   class TPacker *Data;
   class TCollection List;

   wopen (10, 4, 23, 75, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Compressors List ", TCENTER, WHITE|_LGREY);

   sprintf (Temp, " %-12.12s %-31.31s %s  %s %s %s  ", "Key", "Display", "DOS", "OS/2", "Windows", "Linux");
   wprints (0, 0, YELLOW|_LGREY, Temp);

   Data = new TPacker (Cfg->SystemPath);
   if (Data->First () == TRUE) {
      i = 0;
      List.Clear ();
      do {
         sprintf (Temp, " %-12.12s %-31.31s %s  %s    %s    %s  ", Data->Key, Data->Display, (Data->Dos == TRUE) ? "Yes" : "No ", (Data->OS2 == TRUE) ? "Yes" : "No ", (Data->Windows == TRUE) ? "Yes" : "No ", (Data->Linux == TRUE) ? "Yes" : "No ");
         List.Add (Temp);
         if (!strcmp (Data->Key, Key))
            start = i;
         i++;
      } while (Data->Next () == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i = wpickstr (12, 5, 22, 74, 5, WHITE|_LGREY, WHITE|_LGREY, WHITE|_BLUE, Array, start, NULL)) != -1) {
            p = Array[i] + 1;
            p[27] = '\0';
            strcpy (Key, strtrim (p));
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();
   }
   else
      getxch ();

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

USHORT CAddCompressorDlg (PSZ Key, PSZ Display)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (9, 17, 15, 62, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Add Compressor ", TCENTER, WHITE|_LGREY);

   DisplayButton (3, 2, "   Ok   ");
   DisplayButton (3, 12, " Cancel ");
   DisplayButton (3, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Key     ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Display ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (3, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (3, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (3, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 11, Key, 15, 15);
      DisplayTextField (1, 11, Display, 31, 31);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 11, Key, 15, 15);
            break;
         case 2:
            GetTextField (1, 11, Display, 31, 31);
            break;
         case 996:
            RetVal = TRUE;
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   return (RetVal);
}

USHORT CCompressorDlg (VOID)
{
   int menu_sel = 1;
   USHORT RetVal = FALSE;
   CHAR Temp[32];
   class TPacker *Data;

   Data = new TPacker (Cfg->SystemPath);
   if (Data->First (FALSE) == FALSE)
      Data->New ();

   wopen (7, 5, 18, 74, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Compressors ", TCENTER, WHITE|_LGREY);

   DisplayButton (8,  2, " Change ");
   DisplayButton (8, 12, "  Add   ");
   DisplayButton (8, 22, " Delete ");
   DisplayButton (8, 32, "  List  ");
   DisplayButton (8, 47, "  Prev. ");
   DisplayButton (8, 57, "  Next  ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " Key         ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (1, 1, " Display     ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Position    ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (3, 1, " Id          ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (4, 1, " Pack Cmd.   ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (5, 1, " Unpack Cmd. ", 0, 6, 0, NULL, 0, 0);
      wmenuitem (6, 1, " DOS         ", 0, 7, 0, NULL, 0, 0);
      wmenuitem (6, 19, " OS/2    ", 0, 8, 0, NULL, 0, 0);
      wmenuitem (6, 33, " Windows ", 0, 9, 0, NULL, 0, 0);
      wmenuitem (6, 47, " Linux   ", 0, 10, 0, NULL, 0, 0);

      wmenuitem (8,  2, " Change ", 'C', 996, 0, NULL, 0, 0);
      wmenuitem (8, 12, "  Add   ", 'A', 115, 0, NULL, 0, 0);
      wmenuitem (8, 22, " Delete ", 'D', 116, 0, NULL, 0, 0);
      wmenuitem (8, 32, "  List  ", 'L', 117, 0, NULL, 0, 0);
      wmenuitem (8, 47, "  Prev. ", 'P', 118, 0, NULL, 0, 0);
      wmenuitem (8, 57, "  Next  ", 'N', 119, 0, NULL, 0, 0);

      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 15, Data->Key, sizeof (Data->Key), 31);
      DisplayTextField (1, 15, Data->Display, sizeof (Data->Display), 52);
      sprintf (Temp, "%ld", Data->Position);
      DisplayTextField (2, 15, Temp, 5, 5);
      DisplayTextField (3, 15, Data->Id, sizeof (Data->Id), 52);
      DisplayTextField (4, 15, Data->PackCmd, sizeof (Data->PackCmd), 52);
      DisplayTextField (5, 15, Data->UnpackCmd, sizeof (Data->UnpackCmd), 52);
      wprints (6, 15, WHITE|_BLUE, (Data->Dos == TRUE) ? "Yes" : "No ");
      wprints (6, 29, WHITE|_BLUE, (Data->OS2 == TRUE) ? "Yes" : "No ");
      wprints (6, 43, WHITE|_BLUE, (Data->Windows == TRUE) ? "Yes" : "No ");
      wprints (6, 57, WHITE|_BLUE, (Data->Linux == TRUE) ? "Yes" : "No ");

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 15, Data->Key, sizeof (Data->Key), 31);
            break;
         case 2:
            GetTextField (1, 15, Data->Display, sizeof (Data->Display), 52);
            break;
         case 3:
            sprintf (Temp, "%ld", Data->Position);
            GetTextField (2, 15, Temp, 5, 5);
            Data->Position = atol (Temp);
            break;
         case 4:
            GetTextField (3, 15, Data->Id, sizeof (Data->Id), 52);
            break;
         case 5:
            GetTextField (4, 15, Data->PackCmd, sizeof (Data->PackCmd), 52);
            break;
         case 6:
            GetTextField (5, 15, Data->UnpackCmd, sizeof (Data->UnpackCmd), 52);
            break;
         case 7:
            Data->Dos = (Data->Dos == TRUE) ? FALSE : TRUE;
            break;
         case 8:
            Data->OS2 = (Data->OS2 == TRUE) ? FALSE : TRUE;
            break;
         case 9:
            Data->Windows = (Data->Windows == TRUE) ? FALSE : TRUE;
            break;
         case 10:
            Data->Linux = (Data->Linux == TRUE) ? FALSE : TRUE;
            break;
         case 115: {
            CHAR Key[16], Display[32];

            Key[0] = Display[0] = '\0';
            if (CAddCompressorDlg (Key, Display) == TRUE) {
               Data->New ();
               strcpy (Data->Key, Key);
               strcpy (Data->Display, Display);
               Data->Add ();
            }
            break;
         }
         case 116:
            if (MessageBox ("Delete Record", "Are you really sure ?") == TRUE)
               Data->Delete ();
            break;
         case 117:
            CCompressorListDlg ("");
            break;
         case 118:
            Data->Previous (FALSE);
            break;
         case 119:
            Data->Next (FALSE);
            break;
         case 996:
            Data->Update ();
            break;
      }
   } while (menu_sel != -1);

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

// ----------------------------------------------------------------------

USHORT CSelectNodelistDlg (class TNodes *Data)
{
   short i, a, start = 0;
   CHAR Temp[128], *p, **Array;
   USHORT RetVal = FALSE;
   class TCollection List;

   if (Data->FirstNodelist () == TRUE) {
      i = 1;
      List.Clear ();
      do {
         sprintf (Temp, " %5d ³ %-12.12s ³ %-12.12s ", Data->DefaultZone, Data->Nodelist, Data->Nodediff);
         List.Add (Temp);
         i++;
      } while (Data->NextNodelist () == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i += 5) > 15)
            i = 15;
         if ((i = wpickstr (6, 15, i, 51, 5, BLACK|_CYAN, BLACK|_CYAN, YELLOW|_CYAN, Array, start, NULL)) != -1) {
            a = 0;
            if (Data->FirstNodelist () == TRUE)
               do {
                  if (a++ == i)
                     break;
               } while (Data->NextNodelist () == TRUE);
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();
   }
   else
      getxch ();

   return (RetVal);
}

USHORT CNodelistDlg (VOID)
{
   short i, menu_sel = 996, sub_menu_sel = 996;
   CHAR Temp[64], Zone[16], Nodelist[32], Nodediff[32];
   USHORT RetVal = FALSE;
   class TNodes *Data;

   if ((Data = new TNodes (Cfg->NodelistPath)) != NULL)
      Data->LoadNodelist ();
   if (Cfg->MailAddress.First () == TRUE)
      Data->DefaultZone = Cfg->MailAddress.Zone;

   wopen (4, 13, 19, 65, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" Nodelist ", TCENTER, WHITE|_LGREY);

   DisplayButton (1, 40, "  Add   ");
   DisplayButton (3, 40, "  Edit  ");
   DisplayButton (5, 40, " Remove ");

   DisplayButton (12, 2, "   Ok   ");
   DisplayButton (12, 12, " Cancel ");
   DisplayButton (12, 22, "  Help  ");

   do {
      wmenubegc ();
      wmenuitem (1, 40, "  Add   ", 'A', 102, 0, NULL, 0, 0);
      wmenuitem (3, 40, "  Edit  ", 'E', 103, 0, NULL, 0, 0);
      wmenuitem (5, 40, " Remove ", 'R', 104, 0, NULL, 0, 0);

      wmenuitem (12, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (12, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuitem (12, 22, "  Help  ", 'H', 998, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      sprintf (Temp, "%5s   %-12.12s   %-12.12s", "Zone", "Nodelist", "Nodediff");
      wprints (0, 2, WHITE|_LGREY, Temp);
      for (i = 1; i <= 10; i++) {
         sprintf (Temp, " %5s ³ %-12.12s ³ %-12.12s ", "", "", "");
         wprints (i, 1, BLACK|_CYAN, Temp);
      }

      i = 1;
      if (Data->FirstNodelist () == TRUE)
         do {
            if (i <= 10) {
               sprintf (Temp, "%5d ³ %-12.12s ³ %-12.12s", Data->DefaultZone, Data->Nodelist, Data->Nodediff);
               wprints (i, 2, BLACK|_CYAN, Temp);
               i++;
            }
         } while (Data->NextNodelist () == TRUE);

      switch (menu_sel = wmenuget ()) {
         case 102:
            if (wopen (8, 20, 15, 59, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
               wshadow (DGREY|_BLACK);
               wtitle (" Add Entry ", TCENTER, WHITE|_LGREY);

               DisplayButton (4, 2, "   Ok   ");
               DisplayButton (4, 12, " Cancel ");

               Zone[0] = '\0';
               Nodelist[0] = '\0';
               Nodediff[0] = '\0';
               sub_menu_sel = 996;

               do {
                  wmenubegc ();
                  wmenuitem (0, 1, " Zone     ", 0, 1, 0, NULL, 0, 0);
                  wmenuitem (1, 1, " Nodelist ", 0, 2, 0, NULL, 0, 0);
                  wmenuitem (2, 1, " Nodediff ", 0, 3, 0, NULL, 0, 0);

                  wmenuitem (4, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
                  wmenuitem (4, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
                  wmenuend ((short)sub_menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

                  DisplayTextField (0, 12, Zone, sizeof (Zone), 5);
                  DisplayTextField (1, 12, Nodelist, sizeof (Nodelist), 25);
                  DisplayTextField (2, 12, Nodediff, sizeof (Nodediff), 25);

                  switch (sub_menu_sel = wmenuget ()) {
                     case 1:
                        GetTextField (0, 12, Zone, sizeof (Zone), 5);
                        break;
                     case 2:
                        GetTextField (1, 12, Nodelist, sizeof (Nodelist), 25);
                        break;
                     case 3:
                        GetTextField (2, 12, Nodediff, sizeof (Nodediff), 25);
                        break;
                  }
               } while (sub_menu_sel != -1 && sub_menu_sel != 996 && sub_menu_sel != 997);

               if (sub_menu_sel == 996)
                  Data->AddNodelist (Nodelist, Nodediff, (USHORT)atoi (Zone));

               wclose ();
            }
            break;
         case 103:
            if (CSelectNodelistDlg (Data) == TRUE) {
               if (wopen (8, 20, 15, 59, 1, WHITE|_LGREY, WHITE|_LGREY) > 0) {
                  wshadow (DGREY|_BLACK);
                  wtitle (" Edit Entry ", TCENTER, WHITE|_LGREY);

                  DisplayButton (4, 2, "   Ok   ");
                  DisplayButton (4, 12, " Cancel ");

                  sprintf (Zone, "%d", Data->DefaultZone);
                  strcpy (Nodelist, Data->Nodelist);
                  strcpy (Nodediff, Data->Nodediff);
                  sub_menu_sel = 996;

                  do {
                     wmenubegc ();
                     wmenuitem (0, 1, " Zone     ", 0, 1, 0, NULL, 0, 0);
                     wmenuitem (1, 1, " Nodelist ", 0, 2, 0, NULL, 0, 0);
                     wmenuitem (2, 1, " Nodediff ", 0, 3, 0, NULL, 0, 0);

                     wmenuitem (4, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
                     wmenuitem (4, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
                     wmenuend ((short)sub_menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

                     DisplayTextField (0, 12, Zone, sizeof (Zone), 5);
                     DisplayTextField (1, 12, Nodelist, sizeof (Nodelist), 25);
                     DisplayTextField (2, 12, Nodediff, sizeof (Nodediff), 25);

                     switch (sub_menu_sel = wmenuget ()) {
                        case 1:
                           GetTextField (0, 12, Zone, sizeof (Zone), 5);
                           break;
                        case 2:
                           GetTextField (1, 12, Nodelist, sizeof (Nodelist), 25);
                           break;
                        case 3:
                           GetTextField (2, 12, Nodediff, sizeof (Nodediff), 25);
                           break;
                     }
                  } while (sub_menu_sel != -1 && sub_menu_sel != 996 && sub_menu_sel != 997);

                  if (sub_menu_sel == 996)
                     ;

                  wclose ();
               }
            }
            break;
         case 104:
            if (CSelectNodelistDlg (Data) == TRUE)
               Data->DeleteNodelist ();
            break;
         case 996:
            RetVal = TRUE;
            Data->SaveNodelist ();
            break;
         case 997:
            RetVal = FALSE;
            break;
      }
   } while (menu_sel != -1 && menu_sel != 996 && menu_sel != 997);

   wclose ();

   if (Data != NULL)
      delete Data;

   return (RetVal);
}

