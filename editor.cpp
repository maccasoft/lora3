
// --------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History list:
//    03/06/95 - Initial coding
// --------------------------------------------------------------------

#include "_ldefs.h"
#include "lora.h"

TFullEditor::TFullEditor (void)
{
   StartCol = StartRow = 1;
   Width = 79;
   Height = 25;
   From[0] = To[0] = '\0';
   Subject[0] = '\0';
   FromAddress[0] = ToAddress[0] = '\0';
   Msgn = Number = 0L;
   AreaTitle[0] = '\0';
   Log = NULL;

   _dos_getdate (&d_date);
   _dos_gettime (&d_time);
}

TFullEditor::~TFullEditor (void)
{
}

VOID TFullEditor::Pause (VOID)
{
#if defined(__OS2__)
   DosSleep (1L);
#elif defined(__NT__)
   Sleep (1L);
#endif
}

VOID TFullEditor::GotoXY (USHORT x, USHORT y)
{
   Embedded->Printf ("\x16\x08%c%c", y + StartRow - 1, x + StartCol - 1);
}

VOID TFullEditor::Display (USHORT start)
{
   USHORT y, nCol, curLine;
   CHAR *p = Buffer, Line[128];
   ULONG Crc;

   nCol = 0;
   curLine = 1;

   for (y = 1; y <= Height + 1; ) {
      if (*p == '\0') {
         Crc = StringCrc32 ("", 0xFFFFFFFFL);
         if (Crc != LineCrc[y] && y <= Height) {
            LineCrc[y] = Crc;
            GotoXY (1, y);
            Embedded->ClrEol ();
         }
         y++;
      }
      while (*p != '\0') {
         if (Cursor == p) {
            cx = (USHORT)(nCol + 1);
            cy = y;
         }
         if (*p == '\n') {
            p++;
            Line[nCol] = '\0';
            if (curLine >= start) {
               Crc = StringCrc32 (Line, 0xFFFFFFFFL);
               if (Crc != LineCrc[y] && y <= Height) {
                  LineCrc[y] = Crc;
                  GotoXY (1, y);
                  Embedded->BufferedPrintf ("\x16\x07%s", Line);
               }
               y++;
            }
            curLine++;
            nCol = 0;
            break;
         }
         else {
            Line[nCol++] = *p++;
            if (nCol >= Width) {
               Line[nCol] = '\0';
               while (nCol > 1 && Line[nCol] != ' ') {
                  nCol--;
                  p--;
               }
               if (nCol > 1) {
                  while (Line[nCol] == ' ') {
                     nCol++;
                     p++;
                  }
               }
               Line[nCol] = '\0';

               if (curLine >= start) {
                  Crc = StringCrc32 (Line, 0xFFFFFFFFL);
                  if (Crc != LineCrc[y] && y <= Height) {
                     LineCrc[y] = Crc;
                     GotoXY (1, y);
                     Embedded->BufferedPrintf ("\x16\x07%s", Line);
                  }
                  y++;
               }
               curLine++;

               nCol = 0;
               break;
            }
         }
      }
   }

   if (cy <= Height)
      GotoXY (cx, cy);
}

VOID TFullEditor::MoveCursor (USHORT start)
{
   USHORT y, nCol, curLine;
   CHAR *p = Buffer, Line[128];

   nCol = 0;
   curLine = 1;

   for (y = 1; y <= Height; ) {
      for (;;) {
         if (Cursor == p) {
            cx = (USHORT)(nCol + 1);
            cy = y;
         }
         if (*p == '\n') {
            p++;
            Line[nCol] = '\0';
            if (curLine >= start)
               y++;
            curLine++;
            nCol = 0;
            break;
         }
         else {
            Line[nCol++] = *p++;
            if (nCol >= Width) {
               Line[nCol] = '\0';
               while (nCol > 1 && Line[nCol] != ' ') {
                  nCol--;
                  p--;
               }
               if (nCol > 1) {
                  while (Line[nCol] == ' ') {
                     nCol++;
                     p++;
                  }
               }
               Line[nCol] = '\0';

               if (curLine >= start)
                  y++;
               curLine++;

               nCol = 0;
               break;
            }
         }
      }
   }

   GotoXY (cx, cy);
}

VOID TFullEditor::SetCursor (USHORT start)
{
   USHORT y, nCol, curLine;
   CHAR *p = Buffer, Line[128];

   nCol = 0;
   curLine = 1;

   for (y = 1; y <= Height; ) {
      for (;;) {
         if (cy == y && cx == (nCol + 1))
            Cursor = p;
         if (*p == '\n') {
            p++;
            Line[nCol] = '\0';
            if (cy == y && cx > (nCol + 1)) {
               Cursor = p - 1;
               cx = (USHORT)(nCol + 1);
            }
            if (curLine >= start)
               y++;
            curLine++;
            nCol = 0;
            break;
         }
         else {
            Line[nCol++] = *p++;
            if (nCol >= Width) {
               Line[nCol] = '\0';
               while (nCol > 1 && Line[nCol] != ' ') {
                  nCol--;
                  p--;
               }
               if (nCol > 1) {
                  while (Line[nCol] == ' ') {
                     nCol++;
                     p++;
                  }
               }
               Line[nCol] = '\0';
               if (cy == y && cx > (nCol + 1)) {
                  Cursor = p - 1;
                  cx = (USHORT)(nCol + 1);
               }
               if (curLine >= start)
                  y++;
               curLine++;
               nCol = 0;
               break;
            }
         }
      }
   }

   GotoXY (cx, cy);
}

PSZ TFullEditor::GetFirstChar (USHORT start, USHORT line)
{
   USHORT y, nCol, curLine;
   CHAR *p = Buffer, Line[128];
   PSZ RetVal = NULL;

   nCol = 0;
   curLine = 1;

   for (y = 1; y <= Height + 1; ) {
      for (;;) {
         if (line == y && nCol == 0)
            RetVal = p;
         if (*p == '\n') {
            p++;
            Line[nCol] = '\0';
            if (curLine >= start)
               y++;
            curLine++;
            nCol = 0;
            break;
         }
         else {
            Line[nCol++] = *p++;
            if (nCol >= Width) {
               Line[nCol] = '\0';
               while (nCol > 1 && Line[nCol] != ' ') {
                  nCol--;
                  p--;
               }
               if (nCol > 1) {
                  while (Line[nCol] == ' ') {
                     nCol++;
                     p++;
                  }
               }
               Line[nCol] = '\0';
               if (curLine >= start)
                  y++;
               curLine++;
               nCol = 0;
               break;
            }
         }
      }
   }

   if (*RetVal == '\0')
      RetVal = NULL;

   return (RetVal);
}

VOID TFullEditor::DisplayScreen (VOID)
{
   CHAR Temp[96];

   strcpy (Temp, "===============================================================================");
   Temp[79 - strlen (AreaTitle) - 3] = '\0';
   Embedded->BufferedPrintf ("\x16\x01\x07\x0C\x16\x01\x09= \x16\x01\x0E%s \x16\x01\x09%s\n", AreaTitle, Temp);

   Embedded->BufferedPrintf ("\x16\x01\x0A    Msg: \x16\x01\x0E%lu of %lu\n", Msgn, Number);
   sprintf (Temp, "%02d %3.3s %d %2d:%02d", Msg->Written.Day, Lang->Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute);
   Embedded->BufferedPrintf ("\x16\x01\x0A   From: \x16\x01\x0E%-35.35s \x16\x01\x0F%-16.16s \x16\x01\x07%s\n", From, FromAddress, Temp);
   Embedded->BufferedPrintf ("\x16\x01\x0A     To: \x16\x01\x0E%-35.35s \x16\x01\x0F%-16.16s \x16\x01\x07%s\n", To, ToAddress, "");
   Embedded->BufferedPrintf ("\x16\x01\x0ASubject: \x16\x01\x0E%s\n", Subject);
   Embedded->BufferedPrintf ("\x16\x01\x09===================================================================\x16\x01\x0E ^K?=Help \x16\x01\x09==\n");
   Embedded->BufferedPrintf ("\x16\x01\x0A");

   StartRow = 7;
   StartCol = 1;
   Width = 79;
   Height = 18;
}

USHORT TFullEditor::Run (VOID)
{
   int i;
   unsigned int bytes;
   USHORT c, lineStart = 1, EndRun = FALSE, RetVal = FALSE;
   CHAR *a, *p;

   if (Text.First () == NULL)
      Text.Add ("");

   DisplayScreen ();

   bytes = 0;
   if ((p = (CHAR *)Text.First ()) != NULL)
      do {
         bytes += strlen (p) + 1;
      } while ((p = (CHAR *)Text.Next ()) != NULL);

   if (bytes > 32767)
      bytes += 10240;
   else
      bytes = 32767;
   Buffer = (CHAR *)malloc (bytes);

   strcpy (Buffer, "\n");

   a = Buffer;
   if ((p = (CHAR *)Text.First ()) != NULL)
      do {
         strcpy (a, p);
         a = strchr (a, '\0');
         strcpy (a, "\n");
         a = strchr (a, '\0');
      } while ((p = (CHAR *)Text.Next ()) != NULL);

   Text.Clear ();
   for (i = 0; i < 51; i++)
      LineCrc[i] = 0L;

   Cursor = Buffer;
   Display (1);

   while (EndRun == FALSE && Embedded->AbortSession () == FALSE) {
      if (Embedded->KBHit () == TRUE) {
         if ((c = Embedded->Getch ()) == 0)
            c = (USHORT)(Embedded->Getch () << 8);

         if (c == ESC) {
            while (Embedded->KBHit () == FALSE && Embedded->AbortSession () == FALSE)
               Pause ();
            if (Embedded->KBHit () == TRUE) {
               if ((c = Embedded->Getch ()) == '[')
                  do {
                     while (Embedded->KBHit () == FALSE && Embedded->AbortSession () == FALSE)
                        Pause ();
                     if (Embedded->KBHit () == TRUE)
                        c = Embedded->Getch ();
                  } while ((c == ';' || isdigit (c)) && Embedded->AbortSession () == FALSE);
                  if (c == 'A')
                     c = CTRLE;
                  else if (c == 'B')
                     c = CTRLX;
                  else if (c == 'C')
                     c = CTRLD;
                  else if (c == 'D')
                     c = CTRLS;
                  else if (c == 'H')
                     c = 0x4700;
                  else if (c == 'K')
                     c = 0x4F00;
            }
         }
         else if (c == CTRLK) {
            Embedded->PrintfAt (6, 2, "\x16\x01\x0E ^K \008");
            while (Embedded->KBHit () == FALSE && Embedded->AbortSession () == FALSE)
               Pause ();
            if (Embedded->KBHit () == TRUE) {
               c = Embedded->Getch ();
               if (c == CTRLQ || toupper (c) == 'Q') {
                  Embedded->PrintfAt (6, 2, "\x16\x01\x0E ^KQ ");
                  EndRun = TRUE;
               }
               else if (c == CTRLD || toupper (c) == 'D') {
                  Embedded->PrintfAt (6, 2, "\x16\x01\x0E ^KD ");
                  EndRun = TRUE;
                  RetVal = TRUE;
               }
               else if (c == '?') {
                  Embedded->PrintfAt (6, 2, "\x16\x01\x0E ^K? ");
                  Embedded->DisplayFile ("fshelp");
                  for (i = 0; i < 51; i++)
                     LineCrc[i] = 0L;
                  DisplayScreen ();
                  c = 0;
               }
            }
            Embedded->PrintfAt (6, 2, "\x16\x01\x09======\x16\x01\x0A");
            Display (lineStart);
         }
         else if (c == CTRLQ) {
            Embedded->PrintfAt (6, 2, "\x16\x01\x0E ^Q \008");
            while (Embedded->KBHit () == FALSE && Embedded->AbortSession () == FALSE)
               Pause ();
            if (Embedded->KBHit () == TRUE) {
               c = Embedded->Getch ();
               if (c == CTRLS || toupper (c) == 'S') {
                  Embedded->PrintfAt (6, 2, "\x16\x01\x0E ^QS ");
                  c = 0x4700;
               }
               else if (c == CTRLD || toupper (c) == 'D') {
                  Embedded->PrintfAt (6, 2, "\x16\x01\x0E ^QD ");
                  c = 0x4F00;
               }
            }
            Embedded->PrintfAt (6, 2, "\x16\x01\x09======\x16\x01\x0A");
         }
         else if (c == CTRLL) {  // ^L = Redraw
            for (i = 0; i < 51; i++)
               LineCrc[i] = 0L;
            DisplayScreen ();
            Display (lineStart);
         }
         else if (c == CTRLZ) {  // ^Z = Save
            EndRun = TRUE;
            RetVal = TRUE;
         }

         switch (c) {
            case 0x08:     // Backspace
               if (Cursor > Buffer) {
                  Cursor--;
                  memmove (Cursor, Cursor + 1, strlen (&Cursor[1]) + 1);
                  Display (lineStart);
               }
               break;

            case 0x19:     // CTRL-Y
               if ((p = GetFirstChar (lineStart, cy)) != NULL) {
                  if ((a = GetFirstChar (lineStart, (USHORT)(cy + 1))) != NULL)
                     memmove (p, a, strlen (a) + 1);
                  else
                     strcpy (p, "\n");
                  SetCursor (lineStart);
                  Display (lineStart);
               }
               break;

            case CTRLE:
            case 0x4800:   // Up Arrow
               if (cy > 1) {
                  cy--;
                  SetCursor (lineStart);
               }
               else if (lineStart > 1) {
                  lineStart--;
                  SetCursor (lineStart);
                  Display (lineStart);
               }
               break;

            case CTRLV:    // Page Down
               break;

            case CTRLX:
            case 0x5000:   // Down Arrow
               if (GetFirstChar (lineStart, (USHORT)(cy + 1)) != NULL) {
                  cy++;
                  if (cy > Height) {
                     cy--;
                     lineStart++;
                     SetCursor (lineStart);
                     Display (lineStart);
                  }
                  else
                     SetCursor (lineStart);
               }
               break;

            case CTRLD:
            case 0x4D00:   // Right Arrow
               Cursor++;
               if (*Cursor != '\0')
                  MoveCursor (lineStart);
               else
                  Cursor--;
               break;

            case CTRLS:
            case 0x4B00:   // Left Arrow
               if (Cursor > Buffer) {
                  Cursor--;
                  MoveCursor (lineStart);
               }
               break;

            case 0x7F:
            case 0x5300:   // Delete
               memmove (Cursor, Cursor + 1, strlen (&Cursor[1]) + 1);
               Display (lineStart);
               break;

            case 0x4700:   // Home
               Cursor = GetFirstChar (lineStart, cy);
               MoveCursor (lineStart);
               break;

            case 0x4F00:   // End
               if ((p = GetFirstChar (lineStart, (USHORT)(cy + 1))) == NULL)
                  p = strchr (Cursor, '\0');
               Cursor = p - 1;
               MoveCursor (lineStart);
               break;

            default:
               if (c >= 32 || c == 0x0D) {
                  if (c == 0x0D)
                     c = '\n';
                  memmove (Cursor + 1, Cursor, strlen (Cursor) + 1);
                  *Cursor++ = (CHAR)c;
                  Display (lineStart);
               }
               break;
         }

         if (cy > 25) {
            lineStart++;
            SetCursor (lineStart);
            Display (lineStart);
         }
         else if (cy < 1 && lineStart > 1) {
            lineStart--;
            cy++;
            Display (lineStart);
         }
      }

      Pause ();
   }

   Text.Clear ();

   if (RetVal == TRUE && Buffer != NULL) {
      a = Buffer;
      while ((p = strchr (a, '\n')) != NULL) {
         if (p > a) {
            p--;
            if (*p == '\r')
               *p = '\0';
            p++;
         }
         *p = '\0';
         Text.Add (a);
         a = p + 1;
      }
      Text.Add (a);
   }

   if (Buffer != NULL)
      free (Buffer);

   Embedded->Printf ("\x0C");

   return (RetVal);
}

VOID TFullEditor::InputSubject (VOID)
{
   do {
      Embedded->Printf ("\n\x16\x01\013Enter the subject of this message (%d chars.): ", sizeof (Subject) - 1);
      Embedded->Input (Subject, (USHORT)(sizeof (Subject) - 1), 0);
   } while (Embedded->AbortSession () == FALSE && Subject[0] == '\0');
}

VOID TFullEditor::InputTo (VOID)
{
   do {
      Embedded->Printf ("\n\x16\x01\x0AWho do you wish to send this message to?\n\x16\x01\013Enter User-Name, ? for help, or RETURN for '\x16\x01\013All\x16\x01\013': ");
      Embedded->Input (To, (USHORT)(sizeof (To) - 1), INP_FANCY);
      if (To[0] == '\0')
         strcpy (To, "All");
      else if (To[0] == '?') {
         Embedded->DisplayFile ("WRITEHLP");
         To[0] = '\0';
      }
   } while (Embedded->AbortSession () == FALSE && To[0] == '\0');

   if (strchr (To, '@'))
      strlwr (To);
}

USHORT TFullEditor::Write (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[96], String[64], *p;

   To[0] = '\0';
   InputTo ();
   if (Embedded->AbortSession () == FALSE)
      InputSubject ();

   Msgn = Number = Msg->Highest () + 1L;

   if (Embedded->AbortSession () == FALSE) {
      Msg->Written.Day = d_date.day;
      Msg->Written.Month = d_date.month;
      Msg->Written.Year = (USHORT)d_date.year;
      Msg->Written.Hour = d_time.hour;
      Msg->Written.Minute = d_time.minute;
      Msg->Written.Second = d_time.second;

      Text.Clear ();

      strcpy (String, To);
      if ((p = strtok (String, " ")) != NULL)
         sprintf (Temp, "Hello, %s!", p);
      else
         sprintf (Temp, "Hello!");
      Text.Add (Temp);
      Text.Add ("");

      if (Run () == TRUE)
         Save ();
   }

   return (RetVal);
}

USHORT TFullEditor::Reply (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], Init[8], *p;

   strcpy (To, Msg->From);
   strcpy (Subject, Msg->Subject);
   Msgn = Number = Msg->Highest () + 1L;

   Msg->Read (Msg->Current, 72);

   Msg->Written.Day = d_date.day;
   Msg->Written.Month = d_date.month;
   Msg->Written.Year = (USHORT)d_date.year;
   Msg->Written.Hour = d_time.hour;
   Msg->Written.Minute = d_time.minute;
   Msg->Written.Second = d_time.second;

   // This section build the quote string. The first character
   // of the first two words of the name are used to build the
   // string. If the user has a single word name, then only one
   // character is taken.
   Init[0] = ' ';
   Init[1] = (CHAR)toupper (To[0]);
   if ((p = strchr (To, ' ')) != NULL) {
      Init[2] = (CHAR)toupper (p[1]);
      Init[3] = '>';
      Init[4] = ' ';
      Init[5] = '\0';
   }
   else {
      Init[2] = '>';
      Init[3] = ' ';
      Init[4] = '\0';
   }

   // The first loop removes all kludge lines (lines that begin with
   // a ^A character and the SEEN-BY: word).
   if ((p = (CHAR *)Msg->Text.First ()) != NULL)
      do {
         if (!strncmp (p, "SEEN-BY: ", 9) || *p == 0x01) {
            Msg->Text.Remove ();
            p = (CHAR *)Msg->Text.Value ();
         }
         else
            p = (CHAR *)Msg->Text.Next ();
      } while (p != NULL);

   // The second loop add the quote string in the front of each of the
   // remaning lines.
   if ((p = (CHAR *)Msg->Text.First ()) != NULL)
      do {
         sprintf (Temp, "%s%s", Init, p);
         Msg->Text.Replace (Temp, (USHORT)(strlen (Temp) + 1));
      } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

   Text.Clear ();

   if ((p = strtok (Msg->From, " ")) != NULL)
      sprintf (Temp, "Hello, %s!", p);
   else
      sprintf (Temp, "Hello!");
   Text.Add (Temp);
   Text.Add ("");
   while ((p = (CHAR *)Msg->Text.First ()) != NULL) {
      Text.Add (p);
      Msg->Text.Remove ();
   }

   if (Run () == TRUE)
      Save ();

   return (RetVal);
}

VOID TFullEditor::Save (VOID)
{
   CHAR Temp[128], *p;

   if (Msg != NULL) {
      Msg->New ();

      Msg->Local = TRUE;
      strcpy (Msg->From, From);
      strcpy (Msg->To, To);
      strcpy (Msg->Subject, Subject);

      Msg->Arrived.Day = Msg->Written.Day = d_date.day;
      Msg->Arrived.Month = Msg->Written.Month = d_date.month;
      Msg->Arrived.Year = Msg->Written.Year = (USHORT)d_date.year;
      Msg->Arrived.Hour = Msg->Written.Hour = d_time.hour;
      Msg->Arrived.Minute = Msg->Written.Minute = d_time.minute;
      Msg->Arrived.Second = Msg->Written.Second = d_time.second;

      if (EchoMail == TRUE) {
         sprintf (Temp, "\001MSGID: %s %08lx", FromAddress, time (NULL));
         p = (PSZ)Text.First ();
         Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
         if (p != NULL) {
            Text.Insert (p, (USHORT)(strlen (p) + 1));
            Text.First ();
            Text.Remove ();
         }

         sprintf (Temp, "\001PID: %s", NAME_OS);
         Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));

         sprintf (Temp, "--- %s v%s", NAME, VERSION);
         Text.Add (Temp);
         sprintf (Temp, " * Origin: %s (%s)", Origin, FromAddress);
         Text.Add (Temp);
      }

      Msg->Add (Text);
      Number = Msg->Highest ();

      if (Log != NULL)
         Log->Write (":Written message #%lu", Number);
      Embedded->Printf ("\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%ld WRITTEN TO DISK >>>\n\006\007\006\007", Number);
   }
}

// --------------------------------------------------------------------

TEditor::TEditor (void)
{
   Embedded = NULL;
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
   Embedded->Printf ("\n\x16\x01\012Continue entering text. Type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself when you are\ndone. (Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
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
      if (Embedded->AbortSession () == TRUE)
         return;
      Embedded->Printf ("\n\x16\x01\013Change text in what line (1-%d)? ", usRows);
      Embedded->GetString (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
      if ((usEditLine = (USHORT)atoi (szTemp)) == 0)
         return;
      if (usEditLine > usRows)
         Embedded->Printf ("\n\x16\x01\x0DSorry, that line number is out of range!\n");
   } while (usEditLine > usRows);

   Text.First ();
   for (i = 1; i < usEditLine; i++)
      Text.Next ();

   Embedded->Printf ("\n\x16\x01\012The current line reads:\n\n\x16\x01\x0E%3d: \x16\x01\013%s\n", usEditLine, (PSZ)Text.Value ());

   if ((szText = (CHAR *)malloc (ScreenWidth)) == NULL)
      return;
   Embedded->Printf ("\n\x16\x01\013Change what text?\n: ");
   Embedded->Input (szText, (USHORT)(ScreenWidth - 10), INP_NOCOLOR);
   if (szText[0] == '\0' || Embedded->AbortSession () == TRUE) {
      free (szText);
      return;
   }

   if ((szReplace = (CHAR *)malloc (ScreenWidth)) == NULL) {
      free (szText);
      return;
   }

   Embedded->Printf ("\n\x16\x01\013Enter new text now (just RETURN to delete)\n: ");
   Embedded->GetString (szReplace, (USHORT)(ScreenWidth - 10), INP_NOCOLOR);
   if (szText[0] == '\0' || Embedded->AbortSession () == TRUE) {
      free (szReplace);
      free (szText);
      return;
   }

   if ((szLine = (CHAR *)malloc (ScreenWidth * 2)) == NULL) {
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

   Embedded->Printf ("\n\x16\x01\012New line now reads:\n\n\x16\x01\016%3d: \x16\x01\013%s\n", usEditLine, Text.Value ());
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
      if (Embedded->AbortSession () == TRUE)
         return;
      Embedded->Printf ("\n\x16\x01\013Delete what line (1-%d)? ", usRows);
      Embedded->Input (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
      if ((usEditLine = (USHORT)atoi (szTemp)) == 0)
         return;
      if (usEditLine > usRows)
         Embedded->Printf ("\n\x16\x01\x0DSorry, that line number is out of range!\n");
   } while (usEditLine > usRows);

   Text.First ();
   for (i = 1; i < usEditLine; i++)
      Text.Next ();

   Embedded->Printf ("\n\x16\x01\012The current line reads:\n\n\x16\x01\x0E%3d: \x16\x01\013%s\n", usEditLine, (PSZ)Text.Value ());

   do {
      Embedded->Printf ("\n\x16\x01\013Okay to delete this line (Y/N)? ");
      Embedded->GetString (szTemp, 1, INP_HOTKEY);
      szTemp[0] = (CHAR)toupper (szTemp[0]);
   } while (szTemp[0] != 'Y' && szTemp[0] != 'N');

   if (szTemp[0] == 'Y')
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
   Embedded->Printf ("%s", Wrap);
   Wrap[0] = '\0';

   c = 0;
   while (Embedded->AbortSession () == FALSE && c != '\r') {
      if (Embedded->KBHit ()) {
         if ((c = Embedded->Getch ()) == 0)
            c = (SHORT)(Embedded->Getch () << 8);

         if (c != '\r') {
            if (c == 8 || c == 127) {
               if (len > 0) {
                  Embedded->Printf ("%c %c", 8, 8);
                  p--;
                  len--;
               }
            }
            else if (c >= 32 && c < 256) {
               if (len < usMaxlen) {
                  *p++ = (char)c;
                  len++;
                  Embedded->Putch ((UCHAR)c);
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
                              Embedded->Printf ("%c %c", 8, 8);
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
      Embedded->Idle ();
   }

   *p = '\0';
   Embedded->Printf ("\n");

   return (pszBuffer);
}

USHORT TEditor::InputText (VOID)
{
   USHORT RetVal = FALSE, Number;
   CHAR *Line;

   Number = 1;
   if ((Line = (CHAR *)Text.Value ()) != NULL) {
      Text.First ();
      while ((CHAR *)Text.Value () != Line && Text.Value () != NULL) {
         Number++;
         Text.Next ();
      }
      Number++;
   }

   if ((Line = (CHAR *)malloc (ScreenWidth)) != NULL) {
      do {
         Embedded->Printf ("%3u: ", Number);
         GetString (Line, (USHORT)(ScreenWidth - 10));
         if (!strcmp (Line, "/?"))
            Embedded->Printf ("\n\x16\x01\012Continue entering text. Type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself when you are\ndone. (Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
         else if (stricmp (Line, "/OK") && stricmp (Line, "/SAVE")) {
            Text.Insert (Line, (SHORT)(strlen (Line) + 1));
            Number++;
         }
      } while (Embedded->AbortSession () == FALSE && stricmp (Line, "/OK") && stricmp (Line, "/SAVE"));

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
      if (Embedded->AbortSession () == TRUE)
         return (FALSE);
      Embedded->Printf ("\n\x16\x01\013Insert after which line (1-%d)? ", usRows);
      Embedded->GetString (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
      if ((usEditLine = (USHORT)atoi (szTemp)) == 0)
         return (FALSE);
      if (usEditLine > usRows)
         Embedded->Printf ("\n\x16\x01\x0DSorry, that line number is out of range!\n");
   } while (usEditLine > usRows);

   Text.First ();
   for (i = 1; i < usEditLine; i++)
      Text.Next ();

   Embedded->Printf ("\n\x16\x01\012Continue entering text. Type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself when you are\ndone. (Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");

   return (InputText ());
}

VOID TEditor::ListText (VOID)
{
   USHORT usLine, usRow;
   PSZ pszLine;

   Embedded->Printf ("\n");

   if ((pszLine = (PSZ)Text.First ()) != NULL) {
      usRow = usLine = 1;
      do {
         Embedded->Printf ("\x16\x01\x0E%3d: %s\n", usLine++, pszLine);
         usRow = Embedded->MoreQuestion (usRow);
      } while ((pszLine = (PSZ)Text.Next ()) != NULL && usRow != 0 && Embedded->AbortSession () == FALSE);
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
      if (Embedded->AbortSession () == TRUE)
         return;
      Embedded->Printf ("\n\x16\x01\013Retype what line (1-%d)? ", usRows);
      Embedded->GetString (szTemp, (USHORT)(sizeof (szTemp) - 1), 0);
      if ((usEditLine = (USHORT)atoi (szTemp)) == 0)
         return;
      if (usEditLine > usRows)
         Embedded->Printf ("\n\x16\x01\x0DSorry, that line number is out of range!\n");
   } while (usEditLine > usRows);

   Text.First ();
   for (i = 1; i < usEditLine; i++)
      Text.Next ();

   Embedded->Printf ("\n\x16\x01\012The current line reads:\n\n\x16\x01\x0E%3d: \x16\x01\013%s\n", usEditLine, (PSZ)Text.Value ());

   if ((szLine = (CHAR *)malloc (ScreenWidth)) == NULL)
      return;
   Embedded->Printf ("\n\x16\x01\013Enter new line\n: ");
   Embedded->GetString (szLine, (USHORT)(ScreenWidth - 10), INP_NOCOLOR);
   if (szLine[0] == '\0' || Embedded->AbortSession () == TRUE) {
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

// --------------------------------------------------------------------

TMsgEditor::TMsgEditor (void)
{
   Log = NULL;
   Embedded = NULL;
   Number = 0L;
   strcpy (Origin, "Default Origin");
   strcpy (Address, "0:0/0");
}

TMsgEditor::~TMsgEditor (void)
{
}

VOID TMsgEditor::Forward (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128];
   PSZ Line;

   do {
      Embedded->Printf ("\n\x16\x01\x0AWho do you wish to send this message to?\n\x16\x01\013Enter User-Name, ? for help, or RETURN for '\x16\x01\013All\x16\x01\013': ");
      Embedded->Input (To, (USHORT)(sizeof (To) - 1), INP_FANCY);
      if (To[0] == Lang->Help)
         Embedded->DisplayFile ("FORWDHLP");
      else if (To[0] != '\0')
         RetVal = TRUE;
   } while (Embedded->AbortSession () == FALSE && RetVal == FALSE && To[0] != '\0');

   if (strchr (To, '@'))
      strlwr (To);

   if (RetVal == TRUE) {
      sprintf (Subject, "%s (Fwd)", Msg->Subject);

      Text.Clear ();
      sprintf (Temp, " * Originally for %s", Msg->To);
      Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      sprintf (Temp, " * Forwarded by %s", UserName);
      Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      Text.Add ("", 1);

      if ((Line = (PSZ)Msg->Text.First ()) != NULL)
         do {
            if (Line[0] != 0x01 && strnicmp (Line, "SEEN-BY: ", 9))
               Text.Add (Line, (USHORT)(strlen (Line) + 1));
         } while ((Line = (PSZ)Msg->Text.Next ()) != NULL);

      Save ();
   }
}

VOID TMsgEditor::InputSubject (VOID)
{
   do {
      Embedded->Printf ("\n\x16\x01\013Enter the subject of this message (%d chars.): ", sizeof (Subject) - 1);
      Embedded->Input (Subject, (USHORT)(sizeof (Subject) - 1), 0);
   } while (Embedded->AbortSession () == FALSE && Subject[0] == '\0');
}

VOID TMsgEditor::InputTo (VOID)
{
   do {
      Embedded->Printf ("\n\x16\x01\x0AWho do you wish to send this message to?\n\x16\x01\013Enter User-Name, ? for help, or RETURN for '\x16\x01\013All\x16\x01\013': ");
      Embedded->Input (To, (USHORT)(sizeof (To) - 1), INP_FANCY);
      if (To[0] == '\0')
         strcpy (To, "All");
      else if (To[0] == '?') {
         Embedded->DisplayFile ("WRITEHLP");
         To[0] = '\0';
      }
   } while (Embedded->AbortSession () == FALSE && To[0] == '\0');

   if (strchr (To, '@'))
      strlwr (To);
}

VOID TMsgEditor::Menu (VOID)
{
   USHORT Stop = FALSE;
   CHAR Cmd[2];

   while (Stop == FALSE && Embedded->AbortSession () == FALSE) {
      Embedded->Printf ("\n\026\001\012EDITOR OPTIONS:\n\n");
      Embedded->Printf ("  \026\001\013S\026\001\016 ... Save message    \026\001\013R\026\001\016 ... Retype a line\n");
      Embedded->Printf ("  \026\001\013A\026\001\016 ... Append message  \026\001\013D\026\001\016 ... Delete line\n");
      Embedded->Printf ("  \026\001\013L\026\001\016 ... List message    \026\001\013I\026\001\016 ... Insert line(s)\n");
      Embedded->Printf ("  \026\001\013C\026\001\016 ... Change text     \026\001\013N\026\001\016 ... New message\n");
      Embedded->Printf ("  \026\001\013H\026\001\016 ... Help            \026\001\013B\026\001\016 ... Change subject\n");
      Embedded->Printf ("  \026\001\013Q\026\001\016 ... Quote text\n");
      Embedded->Printf ("\n\026\001\012Editor\n\026\001\013Make your selection (S,R,A,D,L,I,C,N,H,B,Q or X to exit): \026\001\007");

      Embedded->Input (Cmd, (USHORT)(sizeof (Cmd) - 1), INP_HOTKEY);

      if (Embedded->AbortSession () == FALSE) {
         switch (toupper (Cmd[0])) {
            case 'A':
               if (AppendText () == FALSE) {
                  Save ();
                  Stop = TRUE;
               }
               break;
            case 'C':
               ChangeText ();
               break;
            case 'B':
               InputSubject ();
               break;
            case 'D':
               DeleteLine ();
               break;
            case 'I':
               InsertLines ();
               break;
            case 'L':
               ListText ();
               break;
            case 'N':
               Text.Clear ();
               Embedded->Printf ("\n\x16\x01\012Type your message now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
               if (InputText () == FALSE) {
                  Save ();
                  Stop = TRUE;
               }
               break;
            case 'Q':
               QuoteText ();
               break;
            case 'R':
               RetypeLine ();
               break;
            case 'S':
               Save ();
               Stop = TRUE;
               break;
            case 'X':
               Embedded->Printf ("\n\026\001\016<<< EDITOR EXITED, MESSAGE NOT SAVED >>>\n\n\006\007\006\007");
               Stop = TRUE;
               break;
         }
      }
   }
}

USHORT TMsgEditor::Modify (VOID)
{
   USHORT RetVal = FALSE;
   PSZ Line;

   if ((Line = (PSZ)Msg->Text.First ()) != NULL) {
      do {
         if (Line[0] != 0x01 && strnicmp (Line, "SEEN-BY: ", 9))
            Text.Add (Line, (USHORT)(strlen (Line) + 1));
      } while ((Line = (PSZ)Msg->Text.Next ()) != NULL);

      strcpy (To, Msg->To);
      strcpy (Subject, Msg->Subject);

      Embedded->Printf ("\n\x16\x01\012Type your message now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
      if ((RetVal = InputText ()) == FALSE)
         Save ();
   }

   return (RetVal);
}

VOID TMsgEditor::QuoteText (VOID)
{
   USHORT Line, Current;
   USHORT Start, End;
   CHAR Temp[16], *p;

   if (Msg->Text.First () != NULL) {
      while (Embedded->AbortSession () == FALSE) {
         Embedded->Printf ("\n\026\001\013Start quoting from line number (or ? displays message): ");
         Embedded->Input (Temp, (USHORT)(sizeof (Temp) - 1), 0);

         if (Embedded->AbortSession () == FALSE) {
            if (Temp[0] == '?') {
               if ((p = (PSZ)Msg->Text.First ()) != NULL) {
                  Line = 1;
                  Current = 0;
                  do {
                     Embedded->Printf ("\026\001\016%3d: %s\n", ++Current, p);
                     if ((Line = Embedded->MoreQuestion (Line)) == 0)
                        break;
                  } while ((p = (PSZ)Msg->Text.Next ()) != NULL);
               }
            }
            else if (Temp[0] == '\0')
               break;
            else {
               Start = (USHORT)atoi (Temp);

               Embedded->Printf ("\026\001\013End quoting at line number: ");
               Embedded->Input (Temp, (USHORT)(sizeof (Temp) - 1), 0);
               End = (USHORT)atoi (Temp);
               break;
            }
         }
      }

      if ((p = (PSZ)Msg->Text.First ()) != NULL) {
         Current = 0;
         do {
            Current++;
            if (Current >= Start && Current <= End)
               Text.Add (p, (USHORT)(strlen (p) + 1));
         } while ((p = (PSZ)Msg->Text.Next ()) != NULL);
      }
   }
}

USHORT TMsgEditor::Reply (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], Init[8], *p;
   class TFullEditor *Full;

   strcpy (To, Msg->From);
   strcpy (Subject, Msg->Subject);

   Msg->Read (Msg->Current, 72);
   Init[0] = ' ';
   Init[1] = (CHAR)toupper (To[0]);
   if ((p = strchr (To, ' ')) != NULL) {
      Init[2] = (CHAR)toupper (p[1]);
      Init[3] = '>';
      Init[4] = ' ';
      Init[5] = '\0';
   }
   else {
      Init[2] = '>';
      Init[3] = ' ';
      Init[4] = '\0';
   }

   // The first loop removes all kludge lines (lines that begin with
   // a ^A character and the SEEN-BY: word).
   if ((p = (CHAR *)Msg->Text.First ()) != NULL)
      do {
         if (!strncmp (p, "SEEN-BY: ", 9) || *p == 0x01) {
            Msg->Text.Remove ();
            p = (CHAR *)Msg->Text.Value ();
         }
         else
            p = (CHAR *)Msg->Text.Next ();
      } while (p != NULL);

   // The second loop add the quote string in the front of each of the
   // remaning lines.
   if ((p = (CHAR *)Msg->Text.First ()) != NULL)
      do {
         sprintf (Temp, "%s%s", Init, p);
         Msg->Text.Replace (Temp, (USHORT)(strlen (Temp) + 1));
      } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

   Text.Clear ();

   if ((Full = new TFullEditor) != NULL) {
      strcpy (Full->From, UserName);
      strcpy (Full->To, To);
      strcpy (Full->Subject, Subject);
      Full->Msgn = Full->Number = Msg->Number () + 1L;
      Full->Embedded = Embedded;

      if ((p = strtok (Msg->From, " ")) != NULL)
         sprintf (Temp, "Hello, %s!", p);
      else
         sprintf (Temp, "Hello!");
      Full->Text.Add (Temp);
      Full->Text.Add ("");
      while ((p = (CHAR *)Msg->Text.First ()) != NULL) {
         Full->Text.Add (p);
         Msg->Text.Remove ();
      }
      Full->Text.Add ("");

      Full->Run ();

      delete Full;
   }
   else {
      Embedded->Printf ("\n\x16\x01\012Type your message now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
      if ((RetVal = InputText ()) == FALSE)
         Save ();
   }

   return (RetVal);
}

VOID TMsgEditor::Save (VOID)
{
   CHAR Temp[128], *p;
   struct dosdate_t d_date;
   struct dostime_t d_time;

   if (Msg != NULL) {
      Msg->New ();

      Msg->Local = TRUE;
      strcpy (Msg->From, UserName);
      strcpy (Msg->To, To);
      strcpy (Msg->Subject, Subject);

      _dos_getdate (&d_date);
      _dos_gettime (&d_time);

      Msg->Arrived.Day = Msg->Written.Day = d_date.day;
      Msg->Arrived.Month = Msg->Written.Month = d_date.month;
      Msg->Arrived.Year = Msg->Written.Year = (USHORT)d_date.year;
      Msg->Arrived.Hour = Msg->Written.Hour = d_time.hour;
      Msg->Arrived.Minute = Msg->Written.Minute = d_time.minute;
      Msg->Arrived.Second = Msg->Written.Second = d_time.second;

      if (EchoMail == TRUE) {
         Cfg->MailAddress.First ();
         sprintf (Temp, "\001MSGID: %s %08lx", Cfg->MailAddress.String, time (NULL));
         p = (PSZ)Text.First ();
         Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
         if (p != NULL) {
            Text.Insert (p, (USHORT)(strlen (p) + 1));
            Text.First ();
            Text.Remove ();
         }

         sprintf (Temp, "\001PID: %s", NAME_OS);
         Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));

         Text.Add ("", 1);
         sprintf (Temp, "--- %s v%s", NAME, VERSION);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
         sprintf (Temp, " * Origin: %s (%s)", Origin, Address);
         Text.Add (Temp, (USHORT)(strlen (Temp) + 1));
      }

      Msg->Add (Text);
      Number = Msg->Highest ();

      Log->Write (":Written message #%lu", Number);
      Embedded->Printf ("\n\x16\x01\x0E<<< CONFIRMED: MESSAGE #%ld WRITTEN TO DISK >>>\n\006\007\006\007", Number);
   }
}

USHORT TMsgEditor::Write (VOID)
{
   USHORT RetVal = FALSE;

   To[0] = '\0';
   InputTo ();
   if (Embedded->AbortSession () == FALSE)
      InputSubject ();

   if (Embedded->AbortSession () == FALSE) {
      Text.Clear ();
      Embedded->Printf ("\n\x16\x01\012Type your message now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
      if ((RetVal = InputText ()) == FALSE)
         Save ();
   }

   return (RetVal);
}

// --------------------------------------------------------------------

TCommentEditor::TCommentEditor (void)
{
   Embedded = NULL;
   File = NULL;
}

TCommentEditor::~TCommentEditor (void)
{
}

VOID TCommentEditor::Menu (VOID)
{
   USHORT Stop = FALSE;
   CHAR Cmd[2];

   while (Stop == FALSE && Embedded->AbortSession () == FALSE) {
      Embedded->Printf ("\n\026\001\012EDITOR OPTIONS:\n\n");
      Embedded->Printf ("  \026\001\013S\026\001\016 ... Save comment    \026\001\013R\026\001\016 ... Retype a line\n");
      Embedded->Printf ("  \026\001\013A\026\001\016 ... Append text     \026\001\013D\026\001\016 ... Delete line\n");
      Embedded->Printf ("  \026\001\013L\026\001\016 ... List text       \026\001\013I\026\001\016 ... Insert line(s)\n");
      Embedded->Printf ("  \026\001\013C\026\001\016 ... Change text     \026\001\013N\026\001\016 ... New comment\n");
      Embedded->Printf ("  \026\001\013H\026\001\016 ... Help\n");
      Embedded->Printf ("\n\026\001\012Editor\n\026\001\013Make your selection (S,R,A,D,L,I,C,N,H or X to exit): \026\001\007");

      Embedded->Input (Cmd, (USHORT)(sizeof (Cmd) - 1), INP_HOTKEY);

      if (Embedded->AbortSession () == FALSE) {
         switch (toupper (Cmd[0])) {
            case 'A':
               if (AppendText () == FALSE) {
                  Save ();
                  Stop = TRUE;
               }
               break;
            case 'C':
               ChangeText ();
               break;
            case 'D':
               DeleteLine ();
               break;
            case 'I':
               InsertLines ();
               break;
            case 'L':
               ListText ();
               break;
            case 'N':
               Text.Clear ();
               Embedded->Printf ("\n\x16\x01\012Type your comment now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
               if (InputText () == FALSE) {
                  Save ();
                  Stop = TRUE;
               }
               break;
            case 'R':
               RetypeLine ();
               break;
            case 'S':
               Save ();
               Stop = TRUE;
               break;
            case 'X':
               Embedded->Printf ("\n\026\001\016<<< EDITOR EXITED, COMMENT NOT SAVED >>>\n\n\006\007\006\007");
               Stop = TRUE;
               break;
         }
      }
   }
}

VOID TCommentEditor::Save (VOID)
{
   if (File != NULL) {
      if (Text.First () != NULL)
         do {
            File->Description->Add (Text.Value (), (USHORT)(strlen ((PSZ)Text.Value ()) + 1));
         } while (Text.Next () != NULL);
   }
}

USHORT TCommentEditor::Write (VOID)
{
   USHORT RetVal;

   Text.Clear ();
   Embedded->Printf ("\n\x16\x01\012Type your comment now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
   if ((RetVal = InputText ()) == FALSE)
      Save ();

   return (RetVal);
}

// --------------------------------------------------------------------

TMailEditor::TMailEditor (void)
{
   Log = NULL;
   Embedded = NULL;
   Number = 0L;
   strcpy (Origin, "Default Origin");
   strcpy (Address, "0:0/0");
   Msg = NULL;

   Storage = ST_SQUISH;
   strcpy (BasePath, "email");
}

TMailEditor::~TMailEditor (void)
{
}

USHORT TMailEditor::InputSubject (VOID)
{
   USHORT RetVal = FALSE;

   Embedded->Printf ("\n\x16\x01\013Enter the subject of this message (%d chars.): ", sizeof (Subject) - 1);
   Embedded->Input (Subject, (USHORT)(sizeof (Subject) - 1), 0);

   if (Subject[0] != '\0')
      RetVal = TRUE;

   return (RetVal);
}

USHORT TMailEditor::InputTo (VOID)
{
   USHORT RetVal = FALSE;

   do {
      Embedded->Printf ("\n\x16\x01\x0AWho do you wish to send this message to?\n\x16\x01\013Enter User-Name, e-mail address, or ? for help: ");
      Embedded->Input (To, (USHORT)(sizeof (To) - 1), 0);
      if (To[0] == '?')
         Embedded->DisplayFile ("EDITMAIL");
   } while (Embedded->AbortSession () == FALSE && To[0] == '?');

   if (To[0] != '\0')
      RetVal = TRUE;

   return (RetVal);
}

VOID TMailEditor::Menu (VOID)
{
   USHORT Stop = FALSE;
   CHAR Cmd[2];

   while (Stop == FALSE && Embedded->AbortSession () == FALSE) {
      Embedded->Printf ("\n\026\001\012EDITOR OPTIONS:\n\n");
      Embedded->Printf ("  \026\001\013S\026\001\016 ... Save message    \026\001\013R\026\001\016 ... Retype a line\n");
      Embedded->Printf ("  \026\001\013A\026\001\016 ... Append message  \026\001\013D\026\001\016 ... Delete line\n");
      Embedded->Printf ("  \026\001\013L\026\001\016 ... List message    \026\001\013I\026\001\016 ... Insert line(s)\n");
      Embedded->Printf ("  \026\001\013C\026\001\016 ... Change text     \026\001\013N\026\001\016 ... New message\n");
      Embedded->Printf ("  \026\001\013H\026\001\016 ... Help            \026\001\013B\026\001\016 ... Change subject\n");
      Embedded->Printf ("  \026\001\013Q\026\001\016 ... Quote text\n");
      Embedded->Printf ("\n\026\001\012Editor\n\026\001\013Make your selection (S,R,A,D,L,I,C,N,H,B,Q or X to exit): \026\001\007");

      Embedded->Input (Cmd, (USHORT)(sizeof (Cmd) - 1), INP_HOTKEY);

      if (Embedded->AbortSession () == FALSE) {
         switch (toupper (Cmd[0])) {
            case 'A':
               if (AppendText () == FALSE) {
                  Save ();
                  Stop = TRUE;
               }
               break;
            case 'C':
               ChangeText ();
               break;
            case 'B':
               InputSubject ();
               break;
            case 'D':
               DeleteLine ();
               break;
            case 'I':
               InsertLines ();
               break;
            case 'L':
               ListText ();
               break;
            case 'N':
               Text.Clear ();
               Embedded->Printf ("\n\x16\x01\012Type your message now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
               if (InputText () == FALSE) {
                  Save ();
                  Stop = TRUE;
               }
               break;
            case 'R':
               RetypeLine ();
               break;
            case 'Q':
               QuoteText ();
               break;
            case 'S':
               Save ();
               Stop = TRUE;
               break;
            case 'X':
               Embedded->Printf ("\n\026\001\016<<< EDITOR EXITED, MESSAGE NOT SAVED >>>\n\n\006\007\006\007");
               Stop = TRUE;
               break;
         }
      }
   }
}

USHORT TMailEditor::Modify (VOID)
{
   USHORT RetVal = FALSE;
   PSZ Line;

   if ((Line = (PSZ)Msg->Text.First ()) != NULL) {
      do {
         if (Line[0] != 0x01 && strnicmp (Line, "SEEN-BY: ", 9))
            Text.Add (Line, (USHORT)(strlen (Line) + 1));
      } while ((Line = (PSZ)Msg->Text.Next ()) != NULL);

      strcpy (To, Msg->To);
      strcpy (Subject, Msg->Subject);

      Embedded->Printf ("\n\x16\x01\012Type your message now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
      if ((RetVal = InputText ()) == FALSE)
         Save ();
   }

   return (RetVal);
}

VOID TMailEditor::QuoteText (VOID)
{
   USHORT Line, Current;
   USHORT Start, End;
   CHAR Temp[16], *p;

   if (Msg->Text.First () != NULL) {
      while (Embedded->AbortSession () == FALSE) {
         Embedded->Printf ("\n\026\001\013Start quoting from line number (or ? displays message): ");
         Embedded->Input (Temp, (USHORT)(sizeof (Temp) - 1), 0);

         if (Embedded->AbortSession () == FALSE) {
            if (Temp[0] == '?') {
               if ((p = (PSZ)Msg->Text.First ()) != NULL) {
                  Line = 1;
                  Current = 0;
                  do {
                     Embedded->Printf ("\026\001\016%3d: %s\n", ++Current, p);
                     if ((Line = Embedded->MoreQuestion (Line)) == 0)
                        break;
                  } while ((p = (PSZ)Msg->Text.Next ()) != NULL);
               }
            }
            else if (Temp[0] == '\0')
               break;
            else {
               Start = (USHORT)atoi (Temp);

               Embedded->Printf ("\026\001\013End quoting at line number: ");
               Embedded->Input (Temp, (USHORT)(sizeof (Temp) - 1), 0);
               End = (USHORT)atoi (Temp);
               break;
            }
         }
      }

      if ((p = (PSZ)Msg->Text.First ()) != NULL) {
         Current = 0;
         do {
            Current++;
            if (Current >= Start && Current <= End)
               Text.Add (p, (USHORT)(strlen (p) + 1));
         } while ((p = (PSZ)Msg->Text.Next ()) != NULL);
      }
   }
}

USHORT TMailEditor::Reply (VOID)
{
   USHORT RetVal = FALSE;
   CHAR Temp[128], Init[16], *p;

   strcpy (To, Msg->From);
   if (Msg->FromAddress[0] != '\0') {
      strcat (To, " ");
      strcat (To, Msg->FromAddress);
   }
   strcpy (Subject, Msg->Subject);

   Msg->Read (Msg->Current, 68);
   Init[0] = (CHAR)toupper (To[0]);
   if ((p = strchr (To, ' ')) != NULL) {
      Init[1] = (CHAR)toupper (p[1]);
      Init[2] = '>';
      Init[3] = '\0';
   }
   else {
      Init[1] = '>';
      Init[2] = '\0';
   }

   // The first loop removes all kludge lines (lines that begin with
   // a ^A character and the SEEN-BY: word).
   if ((p = (CHAR *)Msg->Text.First ()) != NULL)
      do {
         if (!strncmp (p, "SEEN-BY: ", 9) || *p == 0x01) {
            Msg->Text.Remove ();
            p = (CHAR *)Msg->Text.Value ();
         }
         else
            p = (CHAR *)Msg->Text.Next ();
      } while (p != NULL);

   // The second loop add the quote string in the front of each of the
   // remaning lines.
   if ((p = (CHAR *)Msg->Text.First ()) != NULL)
      do {
         sprintf (Temp, "%s%s", Init, p);
         Msg->Text.Replace (Temp, (USHORT)(strlen (Temp) + 1));
      } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

   Text.Clear ();

   Embedded->Printf ("\n\x16\x01\012Type your message now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
   if ((RetVal = InputText ()) == FALSE)
      Save ();

   return (RetVal);
}

VOID TMailEditor::Save (VOID)
{
   CHAR *p, Temp[64];
   USHORT CloseBase = FALSE, IsFidoNet, First, Mapped, IsInternet;
   struct dosdate_t d_date;
   struct dostime_t d_time;
   class TMsgBase *NetMail;
   class TAddress Address;

   if (Msg == NULL) {
      CloseBase = TRUE;

      if (Cfg->MailStorage == ST_JAM)
         Msg = new JAM (Cfg->MailPath);
      else if (Cfg->MailStorage == ST_SQUISH)
         Msg = new SQUISH (Cfg->MailPath);
      else if (Cfg->MailStorage == ST_FIDO)
         Msg = new FIDOSDM (Cfg->MailPath);
      else if (Cfg->MailStorage == ST_ADEPT)
         Msg = new ADEPT (Cfg->MailPath);
      else
         Log->Write ("!Invalid e-mail storage type");
   }


   if (Msg != NULL) {
      Msg->New ();
      Msg->Local = TRUE;

      IsInternet = IsFidoNet = FALSE;

      if ((p = strchr (To, ':')) != NULL) {
         p--;
         while (p > To && isdigit (*p))
            p--;
         if (p > To) {
            *p++ = '\0';
            Address.Parse (p);
            IsFidoNet = TRUE;
         }

         if (Cfg->MailAddress.First () == TRUE) {
            strcpy (Msg->FromAddress, Cfg->MailAddress.String);
            if (Address.Zone == 0)
               Address.Zone = Cfg->MailAddress.Zone;
            if (Address.Net == 0)
               Address.Net = Cfg->MailAddress.Net;

            Mapped = FALSE;
            do {
               if (Address.Zone == Cfg->MailAddress.Zone) {
                  strcpy (Msg->FromAddress, Cfg->MailAddress.String);
                  Mapped = TRUE;
                  break;
               }
            } while (Cfg->MailAddress.Next () == TRUE);
            if (Mapped == FALSE)
               Cfg->MailAddress.First ();

            Address.Add ();
            Address.First ();
            strcpy (Msg->ToAddress, Address.String);

            First = TRUE;
            if (Address.Zone != Cfg->MailAddress.Zone) {
               sprintf (Temp, "\001INTL %u:%u/%u %u:%u/%u", Address.Zone, Address.Net, Address.Node, Cfg->MailAddress.Zone, Cfg->MailAddress.Net, Cfg->MailAddress.Node);
               if (First == TRUE) {
                  p = (PSZ)Text.First ();
                  Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
                  if (p != NULL) {
                     Text.Insert (p, (USHORT)(strlen (p) + 1));
                     Text.First ();
                     Text.Remove ();
                  }
                  First = FALSE;
               }
               else
                  Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
            }
            if (Cfg->MailAddress.Point != 0) {
               sprintf (Temp, "\001FMPT %u", Cfg->MailAddress.Point);
               if (First == TRUE) {
                  p = (PSZ)Text.First ();
                  Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
                  if (p != NULL) {
                     Text.Insert (p, (USHORT)(strlen (p) + 1));
                     Text.First ();
                     Text.Remove ();
                  }
                  First = FALSE;
               }
               else
                  Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
            }
            if (Address.Point != 0) {
               sprintf (Temp, "\001TOPT %u", Address.Point);
               if (First == TRUE) {
                  p = (PSZ)Text.First ();
                  Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
                  if (p != NULL) {
                     Text.Insert (p, (USHORT)(strlen (p) + 1));
                     Text.First ();
                     Text.Remove ();
                  }
                  First = FALSE;
               }
               else
                  Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
            }
            sprintf (Temp, "\001MSGID: %s %08lx", Cfg->MailAddress.String, time (NULL));
            if (First == TRUE) {
               p = (PSZ)Text.First ();
               Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
               if (p != NULL) {
                  Text.Insert (p, (USHORT)(strlen (p) + 1));
                  Text.First ();
                  Text.Remove ();
               }
               First = FALSE;
            }
            else
               Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));

            sprintf (Temp, "\001PID: %s", NAME_OS);
            Text.Insert (Temp, (USHORT)(strlen (Temp) + 1));
         }
      }
      else if (strchr (To, '@') != NULL)
         IsInternet = TRUE;

      strcpy (Msg->From, UserName);
      strcpy (Msg->To, To);
      strcpy (Msg->Subject, Subject);

      if (IsInternet == FALSE) {
         strlwr (Msg->To);
         Msg->To[0] = (CHAR)toupper (Msg->To[0]);
         p = Msg->To;
         while ((p = strchr (p, ' ')) != NULL) {
            p++;
            *p = (CHAR)toupper (*p);
         }
         p = Msg->To;
         while ((p = strchr (p, '-')) != NULL) {
            p++;
            *p = (CHAR)toupper (*p);
         }
         p = Msg->To;
         while ((p = strchr (p, '.')) != NULL) {
            p++;
            *p = (CHAR)toupper (*p);
         }
      }

      _dos_getdate (&d_date);
      _dos_gettime (&d_time);

      Msg->Arrived.Day = Msg->Written.Day = d_date.day;
      Msg->Arrived.Month = Msg->Written.Month = d_date.month;
      Msg->Arrived.Year = Msg->Written.Year = (USHORT)d_date.year;
      Msg->Arrived.Hour = Msg->Written.Hour = d_time.hour;
      Msg->Arrived.Minute = Msg->Written.Minute = d_time.minute;
      Msg->Arrived.Second = Msg->Written.Second = d_time.second;

      Msg->Add (Text);
      Number = Msg->Highest ();

      if (IsFidoNet == TRUE) {
         NetMail = NULL;
         if (Cfg->NetMailStorage == ST_JAM)
            NetMail = new JAM (Cfg->NetMailPath);
         else if (Cfg->NetMailStorage == ST_SQUISH)
            NetMail = new SQUISH (Cfg->NetMailPath);
         else if (Cfg->NetMailStorage == ST_FIDO)
            NetMail = new FIDOSDM (Cfg->NetMailPath);
         else if (Cfg->NetMailStorage == ST_ADEPT)
            NetMail = new ADEPT (Cfg->NetMailPath);
         if (NetMail != NULL) {
            if (Msg->Read (Number) == TRUE)
               NetMail->Add (Msg);
            delete NetMail;
         }
      }

      Log->Write (":Written e-mail message #%lu", Number);
      Embedded->Printf ("\n\x16\x01\x0E<<< CONFIRMED: E-MAIL MESSAGE #%ld WRITTEN TO DISK >>>\n\006\007\006\007", Number);
   }

   if (CloseBase == TRUE && Msg != NULL)
      delete Msg;
}

USHORT TMailEditor::Write (VOID)
{
   USHORT RetVal = FALSE;

   To[0] = '\0';
   if (InputTo () == TRUE) {
      if (Embedded->AbortSession () == FALSE) {
         if (InputSubject () == TRUE) {
            if (Embedded->AbortSession () == FALSE) {
               Text.Clear ();
               Embedded->Printf ("\n\x16\x01\012Type your message now. When done, type '\x16\x01\x0B/OK\x16\x01\012' on a line by itself.\n(Or, type '\x16\x01\x0B/SAVE\x16\x01\012' to save and proceed, without editing).\n\n");
               if ((RetVal = InputText ()) == FALSE)
                  Save ();
            }
         }
      }
   }

   return (RetVal);
}


