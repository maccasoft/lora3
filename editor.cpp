
// --------------------------------------------------------------------
// LoraBBS Professional Edition - Version 0.21
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History list:
//    03/06/95 - Initial coding
// --------------------------------------------------------------------

#include "_ldefs.h"
#include "editor.h"

TEditor::TEditor (class TBbs *bbs)
{
   Bbs = bbs;
   Lang = bbs->Lang;
   User = bbs->User;
   Text.Clear ();
   Wrap[0] = '\0';
}

TEditor::~TEditor (void)
{
   Clear ();
}

USHORT TEditor::AppendText (VOID)
{
   Text.Last ();
   Bbs->Printf (Lang->ContinueEntering);
   return (InputText ());
}

VOID TEditor::ChangeText (VOID)
{
   USHORT i, usRows, usEditLine;
   CHAR szTemp[20], *szText, *szLine, *szReplace;

   usRows = 0;

   if (Text.First () != NULL)
      do {
         usRows++;
      } while (Text.Next () != NULL);

   do {
      if (Bbs->AbortSession () == TRUE)
         return;
      Bbs->Printf (Lang->ChangeLine, usRows);
      Bbs->GetString (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
      if ((usEditLine = (USHORT)atoi (szTemp)) == 0)
         return;
      if (usEditLine > usRows)
         Bbs->Printf (Lang->LineOutOfRange);
   } while (usEditLine > usRows);

   Text.First ();
   for (i = 1; i < usEditLine; i++)
      Text.Next ();

   Bbs->Printf (Lang->CurrentLineReads, usEditLine, (PSZ)Text.Value ());

   if ((szText = (CHAR *)malloc (User->ScreenWidth)) == NULL)
      return;
   Bbs->Printf (Lang->TextToChange);
   Bbs->GetString (szText, (USHORT)(User->ScreenWidth - 10), INP_NOCOLOR);
   if (szText[0] == '\0' || Bbs->AbortSession () == TRUE) {
      free (szText);
      return;
   }

   if ((szReplace = (CHAR *)malloc (User->ScreenWidth)) == NULL) {
      free (szText);
      return;
   }

   Bbs->Printf (Lang->NewText);
   Bbs->GetString (szReplace, (USHORT)(User->ScreenWidth - 10), INP_NOCOLOR);
   if (szText[0] == '\0' || Bbs->AbortSession () == TRUE) {
      free (szReplace);
      free (szText);
      return;
   }

   if ((szLine = (CHAR *)malloc (User->ScreenWidth * 2)) == NULL) {
      free (szReplace);
      free (szText);
      return;
   }

   strcpy (szLine, (PSZ)Text.Value ());
   StringReplace (szLine, szText, szReplace);

   free (szReplace);
   free (szText);

   Text.Replace (szLine, (SHORT)(strlen (szLine) + 1));
   free (szLine);

   Bbs->Printf (Lang->LineNowReads, usEditLine, Text.Value ());
}

VOID TEditor::Clear (VOID)
{
   Text.Clear ();
}

VOID TEditor::DeleteLine (VOID)
{
   USHORT i, usRows, usEditLine;
   CHAR szTemp[10];

   usRows = 0;
   if (Text.First () != NULL)
      do {
         usRows++;
      } while (Text.Next () != NULL);

   do {
      if (Bbs->AbortSession () == TRUE)
         return;
      Bbs->Printf (Lang->DeleteLine, usRows);
      Bbs->GetString (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
      if ((usEditLine = (USHORT)atoi (szTemp)) == 0)
         return;
      if (usEditLine > usRows)
         Bbs->Printf (Lang->LineOutOfRange);
   } while (usEditLine > usRows);

   Text.First ();
   for (i = 1; i < usEditLine; i++)
      Text.Next ();

   Bbs->Printf (Lang->CurrentLineReads, usEditLine, (PSZ)Text.Value ());

   do {
      Bbs->Printf (Lang->OkToDelete);
      Bbs->GetString (szTemp, 1, INP_HOTKEY);
      szTemp[0] = (CHAR)toupper (szTemp[0]);
   } while (szTemp[0] != Lang->Yes && szTemp[0] != Lang->No);

   if (szTemp[0] == Lang->Yes)
      Text.Remove ();
}

PSZ TEditor::GetString (CHAR *pszBuffer, USHORT usMaxlen)
{
   SHORT c, len, count, i;
   PSZ p, mp;

   p = pszBuffer;
   strcpy (p, Wrap);
   len = (SHORT)strlen (Wrap);
   p += len;
   Bbs->Printf ("%s", Wrap);
   Wrap[0] = '\0';

   while (Bbs->AbortSession () == FALSE && c != '\r') {
      if (Bbs->KBHit ()) {
         Bbs->LastActivity = time (NULL);
         if ((c = Bbs->Getch ()) == 0)
            c = (SHORT)(Bbs->Getch () << 8);

         if (c != '\r') {
            if (c == 8 || c == 127) {
               if (len > 0) {
                  Bbs->Printf ("%c %c", 8, 8);
                  p--;
                  len--;
               }
            }
            else if (c >= 32 && c < 256) {
               if (len < usMaxlen) {
                  *p++ = (char)c;
                  len++;
                  Bbs->Putch ((UCHAR)c);
                  if (len >= usMaxlen) {
                     *p = '\0';
                     if (c != ' ') {
                        mp = p;
                        p--;
                        count = 1;
                        while (p > pszBuffer && *p != ' ') {
                           p--;
                           count++;
                        }
                        if (p > pszBuffer) {
                           *p++ = '\0';
                           strcpy (Wrap, p);
                           for (i = 0; i < count; i++)
                              Bbs->Printf ("%c %c", 8, 8);
                        }
                        else
                           p = mp;
                     }
                     c = '\r';
                  }
               }
            }
         }
      }
      else
         Bbs->ReleaseTimeSlice ();
   }

   *p = '\0';
   Bbs->Printf ("\n");

   return (pszBuffer);
}

USHORT TEditor::InputText (VOID)
{
   USHORT RetVal = FALSE;
   CHAR *Line;

   if ((Line = (CHAR *)malloc (User->ScreenWidth)) != NULL) {
      do {
         GetString (Line, (USHORT)(User->ScreenWidth - 10));
         if (Line[0] == '\0')
            Bbs->Printf (Lang->ContinueEntering);
         else if (stricmp (Line, "/OK") && stricmp (Line, "/SAVE"))
            Text.Insert (Line, (SHORT)(strlen (Line) + 1));
      } while (Bbs->AbortSession () == FALSE && stricmp (Line, "/OK") && stricmp (Line, "/SAVE"));

      if (!stricmp (Line, "/OK"))
         RetVal = TRUE;

      free (Line);
   }

   return (RetVal);
}

USHORT TEditor::InsertLines (VOID)
{
   USHORT i, usRows, usEditLine;
   CHAR szTemp[20];

   usRows = 0;

   if (Text.First () != NULL)
   do {
      usRows++;
   } while (Text.Next () != NULL);

   do {
      if (Bbs->AbortSession () == TRUE)
         return (FALSE);
      Bbs->Printf (Lang->InsertAfterLine, usRows);
      Bbs->GetString (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
      if ((usEditLine = (USHORT)atoi (szTemp)) == 0)
         return (FALSE);
      if (usEditLine > usRows)
         Bbs->Printf (Lang->LineOutOfRange);
   } while (usEditLine > usRows);

   Text.First ();
   for (i = 1; i < usEditLine; i++)
      Text.Next ();

   Bbs->Printf (Lang->ContinueEntering);

   return (InputText ());
}

VOID TEditor::ListText (VOID)
{
   USHORT usLine, usRow;
   PSZ pszLine;

   Bbs->Printf ("\n");

   if ((pszLine = (PSZ)Text.First ()) != NULL) {
      usRow = usLine = 1;
      do {
         Bbs->Printf (Lang->TextList, usLine, pszLine);
         usRow = Bbs->MoreQuestion (usRow);
      } while ((pszLine = (PSZ)Text.Next ()) != NULL && usRow != 0 && Bbs->AbortSession () == FALSE);
   }
}

VOID TEditor::RetypeLine (VOID)
{
   USHORT i, usRows, usEditLine;
   CHAR szTemp[10], *szLine;

   usRows = 0;
   if (Text.First () != NULL)
      do {
         usRows++;
      } while (Text.Next () != NULL);

   do {
      if (Bbs->AbortSession () == TRUE)
         return;
      Bbs->Printf (Lang->RetypeLine, usRows);
      Bbs->GetString (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
      if ((usEditLine = (USHORT)atoi (szTemp)) == 0)
         return;
      if (usEditLine > usRows)
         Bbs->Printf (Lang->LineOutOfRange);
   } while (usEditLine > usRows);

   Text.First ();
   for (i = 1; i < usEditLine; i++)
      Text.Next ();

   Bbs->Printf (Lang->CurrentLineReads, usEditLine, (PSZ)Text.Value ());

   if ((szLine = (CHAR *)malloc (User->ScreenWidth)) == NULL)
      return;
   Bbs->Printf (Lang->EnterNewLine);
   Bbs->GetString (szLine, (USHORT)(User->ScreenWidth - 10), INP_NOCOLOR);
   if (szLine[0] == '\0' || Bbs->AbortSession () == TRUE) {
      free (szLine);
      return;
   }

   Text.Replace (szLine, (SHORT)(strlen (szLine) + 1));
}

PSZ TEditor::StringReplace (PSZ str, PSZ search, PSZ replace)
{
   SHORT i, max, leninstr, st_pos;
   PSZ p, src;

   max = (SHORT)strlen (search);
   leninstr = (SHORT)strlen (replace);

   for (p = str; *p; p++) {
      if (!strncmp (search, p, max))
         break;
   }

   if (*p) {
      src = (PSZ)(p + strlen (search));
      strcpy (p, src);

      st_pos = (SHORT)(p - str);

      for (i = (SHORT)strlen (str); i >= st_pos; i--)
         *(str + leninstr + i) = *(str + i);
      for (i = 0; i < leninstr; i++)
         *(str + st_pos + i) = *(replace + i);
   }

   return (str);
}


