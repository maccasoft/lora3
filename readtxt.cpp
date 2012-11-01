
#include "_ldefs.h"
#include "lorawin.h"
#include "msgbase.h"

PSZ Months[] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

USHORT TotalLines, DoRescan = TRUE, ShowKludges = FALSE;
USHORT NetMail, EMail, BadMsgs, Dupes;
CHAR   AreaKey[16];
ULONG  Number;
class  TConfig *Cfg;
class  TMsgBase *Msg;
class  TUser *User;
class  TMsgData *Data;
class  TScan *Scan;

// ----------------------------------------------------------------------

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

VOID GetTextField (USHORT y, USHORT x, CHAR *Text, USHORT FieldSize, USHORT Size)
{
   CHAR Temp[128], String[128];

   if (FieldSize < Size)
      Size = FieldSize;
   memset (Temp, '?', Size);
   Temp[Size] = '\0';

   strcpy (String, Text);
   winpbeg (WHITE|_BLUE, WHITE|_BLUE);
   winpdef (y, x, String, Temp, 0, 2, NULL, 0);
   if (winpread () != W_ESCPRESS)
      strcpy (Text, strbtrim (String));
   hidecur ();
}

VOID DisplayTextField (USHORT y, USHORT x, CHAR *Text, USHORT FieldSize, USHORT Size)
{
   CHAR Temp[128];

   if (FieldSize < Size)
      Size = FieldSize;

   memset (Temp, ' ', Size);
   Temp[Size] = '\0';
   wprints (y, x, WHITE|_BLUE, Temp);
   wprints (y, x, WHITE|_BLUE, Text);
}

USHORT MessageBox (PSZ Caption, PSZ Text)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (9, 24, 15, 55, 1, WHITE|_RED, WHITE|_RED);
   wshadow (DGREY|_BLACK);
   wtitle (Caption, TCENTER, WHITE|_RED);

   DisplayButton (3, 6, "   Ok   ", BLACK|_RED);
   DisplayButton (3, 16, " Cancel ", BLACK|_RED);

   wprints (1, 2, WHITE|_RED, Text);

   do {
      wmenubegc ();
      wmenuitem (3, 6, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (3, 16, " Cancel ", 'C', 997, 0, NULL, 0, 0);
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

typedef struct {
   CHAR   Key[16];
   CHAR   Description[64];
   ULONG  Messages;
   ULONG  New;
   CHAR   Tag[64];
} SCANDATA;

class TScan
{
public:
   TScan (void);
   ~TScan (void);

   CHAR   Key[16];
   CHAR   Description[64];
   ULONG  Messages;
   ULONG  New;
   CHAR   Tag[64];

   VOID   Add (VOID);
   VOID   Clear (VOID);
   USHORT First (VOID);
   USHORT Next (VOID);
   USHORT Read (PSZ key);
   VOID   Update (VOID);

private:
   class  TCollection Data;
};

TScan::TScan (void)
{
   Data.Clear ();
}

TScan::~TScan (void)
{
   Data.Clear ();
}

VOID TScan::Add (VOID)
{
   SCANDATA sd;

   strcpy (sd.Key, Key);
   strcpy (sd.Description, Description);
   sd.Messages = Messages;
   sd.New = New;
   strcpy (sd.Tag, Tag);

   Data.Add (&sd, sizeof (SCANDATA));
}

VOID TScan::Clear (VOID)
{
   Data.Clear ();
}

USHORT TScan::First (VOID)
{
   USHORT RetVal = FALSE;
   SCANDATA *sd;

   if ((sd = (SCANDATA *)Data.First ()) != NULL) {
      strcpy (Key, sd->Key);
      strcpy (Description, sd->Description);
      Messages = sd->Messages;
      New = sd->New;
      strcpy (Tag, sd->Tag);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TScan::Next (VOID)
{
   USHORT RetVal = FALSE;
   SCANDATA *sd;

   if ((sd = (SCANDATA *)Data.Next ()) != NULL) {
      strcpy (Key, sd->Key);
      strcpy (Description, sd->Description);
      Messages = sd->Messages;
      New = sd->New;
      strcpy (Tag, sd->Tag);
      RetVal = TRUE;
   }

   return (RetVal);
}

USHORT TScan::Read (PSZ key)
{
   USHORT RetVal = FALSE;
   SCANDATA *sd;

   if ((sd = (SCANDATA *)Data.First ()) != NULL)
      do {
         if (!stricmp (sd->Key, key)) {
            strcpy (Key, sd->Key);
            strcpy (Description, sd->Description);
            Messages = sd->Messages;
            New = sd->New;
            strcpy (Tag, sd->Tag);
            RetVal = TRUE;
            break;
         }
      } while ((sd = (SCANDATA *)Data.Next ()) != NULL);

   return (RetVal);
}

VOID TScan::Update (VOID)
{
   SCANDATA *sd;

   if ((sd = (SCANDATA *)Data.Value ()) != NULL) {
      sd->Messages = Messages;
      sd->New = New;
   }
}

// ----------------------------------------------------------------------

class TTextEditor
{
public:
   TTextEditor (void);
   ~TTextEditor (void);

   USHORT StartCol, StartRow;
   USHORT Width, Height;
   class  TCollection Text;

   USHORT Run (VOID);

private:
   USHORT cx, cy;
   CHAR   *Buffer, *Cursor;
   ULONG  LineCrc[51];

   VOID   Display (USHORT line);
   VOID   DisplayScreen (VOID);
   PSZ    GetFirstChar (USHORT start, USHORT line);
   VOID   GotoXY (USHORT x, USHORT y);
   VOID   MoveCursor (USHORT start);
   VOID   SetCursor (USHORT start);
};

TTextEditor::TTextEditor (void)
{
   StartCol = 0;
   StartRow = 7;
   Width = 79;
   Height = 17;
}

TTextEditor::~TTextEditor (void)
{
}

VOID TTextEditor::GotoXY (USHORT x, USHORT y)
{
   gotoxy_ ((short)(y + StartRow - 1), (short)(x + StartCol - 1));
}

VOID TTextEditor::Display (USHORT start)
{
   USHORT y, nCol, curLine;
   CHAR *p = Buffer, Line[128], Temp[128];
   ULONG Crc;

   nCol = 0;
   curLine = 1;

   for (y = 1; y <= Height + 1; ) {
      if (*p == '\0') {
         Crc = StringCrc32 ("", 0xFFFFFFFFL);
         if (Crc != LineCrc[y] && y <= Height) {
            LineCrc[y] = Crc;
            memset (Temp, ' ', Width);
            Temp[Width] = '\0';
            prints ((short)(y + StartRow - 1), StartCol, LGREY|_BLACK, Temp);
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
                  memset (Temp, ' ', Width);
                  Temp[Width] = '\0';
                  prints ((short)(y + StartRow - 1), StartCol, LGREY|_BLACK, Temp);
                  if (!strncmp (Line, "---", 3) || !strncmp (Line, " * Origin:", 10))
                     prints ((short)(y + StartRow - 1), StartCol, WHITE|_BLACK, Line);
                  else if (strchr (Line, '>') != NULL)
                     prints ((short)(y + StartRow - 1), StartCol, YELLOW|_BLACK, Line);
                  else if (*p == 0x01 || !strncmp (p, "SEEN-BY:", 8))
                     prints ((short)(y + StartRow - 1), StartCol, CYAN|_BLACK, Line);
                  else
                     prints ((short)(y + StartRow - 1), StartCol, LGREY|_BLACK, Line);
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
                     memset (Temp, ' ', Width);
                     Temp[Width] = '\0';
                     prints ((short)(y + StartRow - 1), StartCol, LGREY|_BLACK, Temp);
                     if (!strncmp (Line, "---", 3) || !strncmp (Line, " * Origin:", 10))
                        prints ((short)(y + StartRow - 1), StartCol, WHITE|_BLACK, Line);
                     else if (strchr (Line, '>') != NULL)
                        prints ((short)(y + StartRow - 1), StartCol, YELLOW|_BLACK, Line);
                     else if (*p == 0x01 || !strncmp (p, "SEEN-BY:", 8))
                        prints ((short)(y + StartRow - 1), StartCol, CYAN|_BLACK, Line);
                     else
                        prints ((short)(y + StartRow - 1), StartCol, LGREY|_BLACK, Line);
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

VOID TTextEditor::MoveCursor (USHORT start)
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

VOID TTextEditor::SetCursor (USHORT start)
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

PSZ TTextEditor::GetFirstChar (USHORT start, USHORT line)
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

VOID TTextEditor::DisplayScreen (VOID)
{
}

USHORT TTextEditor::Run (VOID)
{
   int i, c;
   unsigned int bytes;
   USHORT RetVal = FALSE, lineStart = 1;
   CHAR *a, *p;

   if (Text.First () == NULL)
      Text.Add ("");

   showcur ();
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

   a = Buffer;
   if ((p = (CHAR *)Text.First ()) != NULL)
      do {
         strcpy (a, p);
         a = strchr (a, '\0');
         strcpy (a, "\n");
         a = strchr (a, '\0');
      } while ((p = (CHAR *)Text.Next ()) != NULL);

   for (i = 0; i < 51; i++)
      LineCrc[i] = 0L;

   Cursor = Buffer;
   Display (1);

   while (RetVal == FALSE) {
      videoupdate ();
      while (!kbmhit ())
         ;
      c = getxch ();
      if (c == 0x1B) {
         hidecur ();
         if (MessageBox (" Abort Message ", "Are you really sure ?") == TRUE)
            break;
         MoveCursor (lineStart);
         showcur ();
      }
      else if ((c >= 32 && c <= 255 && c != 127) || c == 0x0D) {
         if (c == 0x0D)
            c = '\n';
         memmove (Cursor + 1, Cursor, strlen (Cursor) + 1);
         *Cursor++ = (CHAR)c;
         Display (lineStart);
      }
      else if (c == 0x08 || c == 127) {
         if (Cursor > Buffer) {
            Cursor--;
            memmove (Cursor, Cursor + 1, strlen (&Cursor[1]) + 1);
            Display (lineStart);
         }
      }
      else if (c == 0x19) { // CTRL-Y
         if ((p = GetFirstChar (lineStart, cy)) != NULL) {
            if ((a = GetFirstChar (lineStart, (USHORT)(cy + 1))) != NULL)
               memmove (p, a, strlen (a) + 1);
            else
               strcpy (p, "\n");
            SetCursor (lineStart);
            Display (lineStart);
         }
      }
      else {
         switch (c) {
            case 0x4D00:  // Freccia a destra
               Cursor++;
               if (*Cursor != '\0')
                  MoveCursor (lineStart);
               else
                  Cursor--;
               break;

            case 0x4B00:  // Freccia a sinistra
               if (Cursor > Buffer) {
                  Cursor--;
                  MoveCursor (lineStart);
               }
               break;

            case 0x4800:  // Freccia su'
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

            case 0x5000:  // Freccia giu'
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

            case 0x5300:  // Delete
               memmove (Cursor, Cursor + 1, strlen (&Cursor[1]) + 1);
               Display (lineStart);
               break;

            case 0x4700:  // Home
               Cursor = GetFirstChar (lineStart, cy);
               MoveCursor (lineStart);
               break;

            case 0x4F00:  // End
               if ((p = GetFirstChar (lineStart, (USHORT)(cy + 1))) == NULL)
                  p = strchr (Cursor, '\0');
               Cursor = p - 1;
               MoveCursor (lineStart);
               break;

            case 0x1F00:  // Alt-S = Save
               hidecur ();
               if (MessageBox (" Save Message ", "Are you really sure ?") == TRUE)
                  RetVal = TRUE;
               showcur ();
               break;
         }
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

   hidecur ();

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

   return (RetVal);
}

// ----------------------------------------------------------------------

VOID CScanDlg (VOID)
{
   USHORT IsUser = FALSE, EscPressed = FALSE;
   ULONG LastRead;
   class TMsgBase *Msg;
   class TMsgData *Data;

   if (Scan != NULL)
      Scan->Clear ();

   if ((User = new TUser (Cfg->UserFile)) != NULL) {
     if (User->GetData (Cfg->SysopName) == TRUE)
        IsUser = TRUE;
   }

   Msg = NULL;
   switch (Cfg->NetMailStorage) {
      case ST_JAM:
         Msg = new JAM (Cfg->NetMailPath);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Cfg->NetMailPath);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Cfg->NetMailPath);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Cfg->NetMailPath);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->NetMailBoard);
         break;
   }
   if (Msg != NULL) {
      LastRead = 0L;
      if (IsUser == TRUE && User != NULL) {
         if (User->MsgTag->Read ("NetMail") == TRUE)
            LastRead = User->MsgTag->LastRead;
      }

      if (Scan != NULL) {
         strcpy (Scan->Key, "NetMail");
         strcpy (Scan->Description, "FidoNet E-Mail");
         Scan->Messages = Msg->Number ();
         Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
         Scan->Tag[0] = '\0';
         Scan->Add ();
      }

      delete Msg;
   }

   Msg = NULL;
   switch (Cfg->MailStorage) {
      case ST_JAM:
         Msg = new JAM (Cfg->MailPath);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Cfg->MailPath);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Cfg->MailPath);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Cfg->MailPath);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->MailBoard);
         break;
   }
   if (Msg != NULL) {
      LastRead = 0L;
      if (IsUser == TRUE && User != NULL) {
         if (User->MsgTag->Read ("EMail") == TRUE)
            LastRead = User->MsgTag->LastRead;
      }

      if (Scan != NULL) {
         strcpy (Scan->Key, "EMail");
         strcpy (Scan->Description, "Personal E-Mail");
         Scan->Messages = Msg->Number ();
         Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
         Scan->Tag[0] = '\0';
         Scan->Add ();
      }

      delete Msg;
   }

   Msg = NULL;
   switch (Cfg->BadStorage) {
      case ST_JAM:
         Msg = new JAM (Cfg->BadPath);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Cfg->BadPath);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Cfg->BadPath);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Cfg->BadPath);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->BadBoard);
         break;
   }
   if (Msg != NULL) {
      LastRead = 0L;
      if (IsUser == TRUE && User != NULL) {
         if (User->MsgTag->Read ("BadMsgs") == TRUE)
            LastRead = User->MsgTag->LastRead;
      }

      if (Scan != NULL) {
         strcpy (Scan->Key, "BadMsgs");
         strcpy (Scan->Description, "Bad Messages");
         Scan->Messages = Msg->Number ();
         Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
         Scan->Tag[0] = '\0';
         Scan->Add ();
      }

      delete Msg;
   }

   Msg = NULL;
   switch (Cfg->DupeStorage) {
      case ST_JAM:
         Msg = new JAM (Cfg->DupePath);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Cfg->DupePath);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Cfg->DupePath);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Cfg->DupePath);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->DupeBoard);
         break;
   }
   if (Msg != NULL) {
      LastRead = 0L;
      if (IsUser == TRUE && User != NULL) {
         if (User->MsgTag->Read ("Dupes") == TRUE)
            LastRead = User->MsgTag->LastRead;
      }

      if (Scan != NULL) {
         strcpy (Scan->Key, "Dupes");
         strcpy (Scan->Description, "Duplicate Messages");
         Scan->Messages = Msg->Number ();
         Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
         Scan->Tag[0] = '\0';
         Scan->Add ();
      }

      delete Msg;
   }

   wopen (11, 19, 13, 61, 0, LRED|_BLACK, YELLOW|_BLACK);
   wshadow (DGREY|_BLACK);

   if ((Data = new TMsgData (Cfg->SystemPath)) != NULL) {
      if (Data->First () == TRUE)
         do {
            if (kbmhit ()) {
               if ((getxch () & 0xff) == 0x1b)
                  EscPressed = TRUE;
            }
            if (EscPressed == FALSE) {
               wcclear (YELLOW|_BLACK);
               Data->Display[41] = '\0';
               wprints (0, (short)((41 - strlen (Data->Display)) / 2), YELLOW|_BLACK, Data->Display);
               videoupdate ();

               Msg = NULL;
               switch (Data->Storage) {
                 case ST_JAM:
                    Msg = new JAM (Data->Path);
                    break;
                 case ST_SQUISH:
                    Msg = new SQUISH (Data->Path);
                    break;
                 case ST_FIDO:
                    Msg = new FIDOSDM (Data->Path);
                    break;
                 case ST_ADEPT:
                    Msg = new ADEPT (Data->Path);
                    break;
                 case ST_HUDSON:
                    Msg = new HUDSON (Data->Path, (UCHAR)Data->Board);
                    break;
               }

               if (Msg != NULL) {
                  LastRead = 0L;
                  if (IsUser == TRUE && User != NULL) {
                     if (User->MsgTag->Read (Data->Key) == TRUE)
                        LastRead = User->MsgTag->LastRead;
                  }

                  if (Scan != NULL) {
                     strcpy (Scan->Key, Data->Key);
                     strcpy (Scan->Description, Data->Display);
                     Scan->Messages = Msg->Number ();
                     Scan->New = Msg->Number () - Msg->UidToMsgn (LastRead);
                     strcpy (Scan->Tag, Data->EchoTag);
                     Scan->Add ();
                  }

                  delete Msg;
               }
            }
         } while (Data->Next () == TRUE);
   }

   wclose ();
}

// ----------------------------------------------------------------------

VOID DisplayScreen (VOID)
{
   CHAR Temp[128];

   videoinit ();

   hidecur ();
   if (wopen (0, 0, 24, 79, 5, LGREY|_BLACK, LGREY|_BLACK) != 0) {
//      box_ (1, 0, 24, 79, 0, LGREY|_BLACK);
      box_ (1, 0, 6, 79, 0, LGREY|_BLACK);
//      whline (6, 0, 80, 0, LGREY|_BLACK);
      whline (24, 0, 80, 0, LGREY|_BLACK);
      if (Cfg->MailAddress.First () == TRUE)
         prints (0, 1, LGREEN|_BLACK, Cfg->MailAddress.String);
#if !defined(__POINT__)
      sprintf (Temp, "%s Mail Reader v%s", NAME, VERSION);
#else
      sprintf (Temp, "%s v%s", NAME, VERSION);
#endif
      prints (0, (USHORT)(78 - strlen (Temp)), LGREEN|_BLACK, Temp);

      prints (2, 1, YELLOW|_BLACK, "    Msg:");
      prints (3, 1, YELLOW|_BLACK, "   From:");
      prints (4, 1, YELLOW|_BLACK, "     To:");
      prints (5, 1, YELLOW|_BLACK, "Subject:");
   }

   videoupdate ();
}

VOID ParseAddress (PSZ text, PSZ name, PSZ address)
{
   CHAR Temp[128], *p, *a;

   strcpy (Temp, text);
   if (strchr (Temp, '(') != NULL) {
      if ((p = strtok (Temp, " ")) != NULL) {
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            if (*p == '(') {
               strcpy (Temp, ++p);
               p = strchr (Temp, '\0');
               while (--p > Temp) {
                  if (*p == ')') {
                     *p = '\0';
                     break;
                  }
               }
               strcpy (name, Temp);
               strcpy (Temp, text);
               if ((p = strtok (Temp, " ")) != NULL)
                  strcpy (address, p);
            }
            else {
               strcpy (Temp, text);
               if ((p = strtok (Temp, " ")) != NULL)
                  strcpy (name, p);
            }
         }
      }
   }
   else if ((p = strchr (Temp, '<')) != NULL) {
      *p++ = '\0';
      if ((a = strchr (p, '>')) != NULL)
         *a = '\0';
      strcpy (address, p);
      p = Temp;
      if (*p == '"')
         strcpy (Temp, ++p);
      p = strchr (Temp, '\0');
      while (--p > Temp) {
         if (*p != ' ' && *p != '"')
            break;
         *p = '\0';
      }
      strcpy (name, Temp);
   }
}

VOID DisplayText (USHORT start)
{
   USHORT y;
   CHAR *p;

   if (Msg != NULL) {
      fill_ (7, 0, 23, 79, ' ', LGREY|_BLACK);

      y = 7;
      if ((p = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if ((*p != 0x01 && strncmp (p, "SEEN-BY:", 8)) || ShowKludges == TRUE) {
               if (start > 0)
                  start--;
               if (start == 0) {
                  if (!strcmp (p, "---") || !strncmp (p, "--- ", 4) || !strncmp (p, " * Origin: ", 11))
                     prints (y, 0, WHITE|_BLACK, p);
                  else if (strchr (p, '>') != NULL && (strchr (p, '>') - p) <= 6)
                     prints (y, 0, YELLOW|_BLACK, p);
                  else if (*p == 0x01 || !strncmp (p, "SEEN-BY:", 8))
                     prints (y, 0, CYAN|_BLACK, p);
                  else
                     prints (y, 0, LGREY|_BLACK, p);
                  y++;
               }
            }
         } while ((p = (CHAR *)Msg->Text.Next ()) != NULL && y < 24);
   }
}

VOID DisplayMessage (VOID)
{
   CHAR Temp[128], *p;
   USHORT gotFrom = FALSE, gotTo = FALSE;
   ULONG Msgn;

   TotalLines = 0;

   if (Msg != NULL) {
      if (Msg->Read (Number, 80) == FALSE) {
         Msg->New ();
         Msgn = 0L;
      }
      else
         Msgn = Msg->UidToMsgn (Number);

      if (Scan != NULL) {
         Scan->New = Msg->Number () - Msgn;
         Scan->Update ();
      }

      if ((p = (CHAR *)Msg->Text.First ()) != NULL)
         do {
            if ((*p != 0x01 && strncmp (p, "SEEN-BY:", 8)) || ShowKludges == TRUE)
               TotalLines++;
            if (!strncmp (p, " * Origin: ", 11)) {
               Msg->ToAddress[0] = '\0';

               strcpy (Temp, &p[11]);
               p = strchr (Temp, '\0');
               while (--p > Temp) {
                  if (*p != ' ' && *p != ')')
                     break;
                  *p = '\0';
               }
               if (p > Temp) {
                  while (--p > Temp) {
                     if (*p == '(' || *p == ' ')
                        break;
                  }
               }
               if (*p == '(' || *p == ' ')
                  p++;
               strcpy (Msg->FromAddress, p);

               if (strchr (p, '@') == NULL && strchr (p, ':') == NULL && strchr (p, '/') == NULL) {
                  if ((p = (CHAR *)Msg->Text.Next ()) != NULL) {
                     Msg->ToAddress[0] = '\0';

                     strcpy (Temp, p);
                     p = strchr (Temp, '\0');
                     while (--p > Temp) {
                        if (*p != ' ' && *p != ')')
                           break;
                        *p = '\0';
                     }
                     if (p > Temp) {
                        while (--p > Temp) {
                           if (*p == '(' || *p == ' ')
                              break;
                        }
                     }
                     if (*p == '(' || *p == ' ')
                        p++;
                     strcpy (Msg->FromAddress, p);
                  }
               }
            }
            else if (!strncmp (p, "\001MSGID: ", 8)) {
               strcpy (Temp, &p[8]);
               if ((p = strtok (Temp, " ")) != NULL) {
                  if (strchr (p, ':') != NULL && strchr (p, '/') != NULL) {
                     Msg->ToAddress[0] = '\0';
                     strcpy (Msg->FromAddress, p);
                  }
               }
            }
            else if (!strncmp (p, "\001From: ", 7)) {
               Msg->FromAddress[0] = '\0';
               if (gotTo == FALSE)
                  Msg->ToAddress[0] = '\0';
               ParseAddress (&p[7], Msg->From, Msg->FromAddress);
               gotFrom = TRUE;
            }
            else if (!strncmp (p, "\001To: ", 5)) {
               if (gotFrom == FALSE)
                  Msg->FromAddress[0] = '\0';
               Msg->ToAddress[0] = '\0';
               ParseAddress (&p[5], Msg->To, Msg->ToAddress);
               gotTo = TRUE;
            }
         } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

      sprintf (Temp, "%lu of %lu (%lu left)                        ", Msgn, Msg->Number (), Msg->Number () - Msgn);
      prints (2, 10, LGREY|_BLACK, Temp);
      sprintf (Temp, "%-30.30s", Msg->From);
      if (!stricmp (Msg->From, Cfg->SysopName))
         prints (3, 10, WHITE|_BLACK, Temp);
      else
         prints (3, 10, LGREY|_BLACK, Temp);
      sprintf (Temp, "%-16.16s", Msg->FromAddress);
      prints (3, 41, LGREY|_BLACK, Temp);
      if (Msgn != 0L) {
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Written.Day, Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Minute);
         prints (3, 59, LGREY|_BLACK, Temp);
      }
      else
         prints (3, 59, LGREY|_BLACK, "                    ");

      sprintf (Temp, "%-30.30s", Msg->To);
      if (!stricmp (Msg->To, Cfg->SysopName))
         prints (4, 10, WHITE|_BLACK, Temp);
      else
         prints (4, 10, LGREY|_BLACK, Temp);
      sprintf (Temp, "%-16.16s", Msg->ToAddress);
      prints (4, 41, LGREY|_BLACK, Temp);
      if (Msgn != 0L) {
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Arrived.Day, Months[Msg->Arrived.Month - 1], Msg->Arrived.Year, Msg->Arrived.Hour, Msg->Arrived.Minute, Msg->Arrived.Second);
         prints (4, 59, LGREY|_BLACK, Temp);
      }
      else
         prints (4, 59, LGREY|_BLACK, "                    ");

      sprintf (Temp, "%-69.69s", Msg->Subject);
      prints (5, 10, LGREY|_BLACK, Temp);

      DisplayText (1);
   }
}

VOID ExportMessage (VOID)
{
   FILE *fp;
   CHAR *p, Temp[128], Key[32];
   ULONG Msgn;

   TotalLines = 0;

   if (Msg != NULL) {
      Msgn = Msg->UidToMsgn (Number);

      if ((fp = fopen ("reader.out", "at")) != NULL) {
         strcpy (Temp, "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ");
         sprintf (Key, " %s ", AreaKey);
         memcpy (&Temp[1], Key, strlen (Key));
         fprintf (fp, "%s\n", Temp);
         fprintf (fp, "     Msg: %lu of %lu\n", Msgn, Msg->Number ());
         fprintf (fp, "    From: %-30.30s %-16.16s %02d %s %d %2d:%02d:%02d\n", Msg->From, Msg->FromAddress, Msg->Written.Day, Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Minute);
         fprintf (fp, "      To: %-30.30s %-16.16s %02d %s %d %2d:%02d:%02d\n", Msg->To, Msg->ToAddress, Msg->Arrived.Day, Months[Msg->Arrived.Month - 1], Msg->Arrived.Year, Msg->Arrived.Hour, Msg->Arrived.Minute, Msg->Arrived.Minute);
         fprintf (fp, " Subject: %.69s\n", Msg->Subject);
         strcpy (Temp, "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ");
         fprintf (fp, "%s\n", Temp);

         if ((p = (CHAR *)Msg->Text.First ()) != NULL)
            do {
               if ((*p != 0x01 && strncmp (p, "SEEN-BY:", 8)) || ShowKludges == TRUE) {
                  if (*p == 0x01)
                     *p = '@';
                  fprintf (fp, "%s\n", p);
               }
            } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

         fprintf (fp, "\n");

         fclose (fp);
      }
   }
}

VOID ClearScreen (VOID)
{
   wcloseall ();
   showcur ();
   videoupdate ();
   closevideo ();
}

USHORT ChangeArea (VOID)
{
   int i;
   USHORT start, RetVal = FALSE;
   CHAR Temp[128], **Array, *p;
   struct stat statbuf;
   class TCollection List;

   if (wopen (4, 2, 22, 76, 0, LCYAN|_BLACK, BLUE|_BLACK) != 0) {
      wtitle (" Area List ", TCENTER, LCYAN|_BLACK);
      wshadow (DGREY|_BLACK);
      wprints (0, 0, YELLOW|_BLACK, " Key             Msgs.    New  Description                               ");

      sprintf (Temp, "%sarealist.rsn", Cfg->SystemPath);
      if (stat (Temp, &statbuf) == 0) {
         DoRescan = TRUE;
         unlink (Temp);
      }

      if (Scan == NULL)
         Scan = new TScan;
      if (Scan != NULL && DoRescan == TRUE) {
         CScanDlg ();
         DoRescan = FALSE;
      }

      start = 0;
      i = 0;
      if (Scan->First () == TRUE)
         do {
            sprintf (Temp, " %-15.15s %5lu  %5lu  %-41.41s ", Scan->Key, Scan->Messages, Scan->New, Scan->Description);
            List.Add (Temp);
            if (!stricmp (Scan->Key, AreaKey))
               start = (short)i;
            i++;
         } while (Scan->Next () == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i = wpickstr (6, 3, 21, 75, 5, LGREY|_BLACK, LGREY|_BLACK, WHITE|_BLUE, Array, start, NULL)) != -1) {
            Temp[0] = '\0';
            if ((p = strtok (Array[i], " ")) != NULL)
               strcpy (Temp, p);
            NetMail = BadMsgs = Dupes = EMail = FALSE;

            if (Scan != NULL)
               Scan->Read (Temp);
            if (!stricmp (Temp, "NetMail"))
               NetMail = TRUE;
            else if (!stricmp (Temp, "EMail"))
               EMail = TRUE;
            else if (!stricmp (Temp, "BadMsgs"))
               BadMsgs = TRUE;
            else if (!stricmp (Temp, "Dupes"))
               Dupes = TRUE;
            else
               Data->Read (Temp, FALSE);
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();

      wclose ();
   }

   return (RetVal);
}

USHORT CMessageListDlg (VOID)
{
   int i;
   USHORT start, RetVal = FALSE;
   CHAR Temp[128], **Array, *p;
   ULONG ListNumber;
   class TCollection List;

   if (wopen (4, 2, 22, 76, 0, LCYAN|_BLACK, BLUE|_BLACK) != 0) {
      wtitle (" Message List ", TCENTER, LCYAN|_BLACK);
      wshadow (DGREY|_BLACK);
      wprints (0, 0, YELLOW|_BLACK, " Num. From              To                Subject");
      videoupdate ();

      start = 0;
      i = 0;
      ListNumber = Msg->Lowest ();
      do {
         if (Msg->ReadHeader (ListNumber) == TRUE) {
            sprintf (Temp, " %4lu %-16.16s  %-16.16s  %.30s ", Msg->UidToMsgn (ListNumber), Msg->From, Msg->To, Msg->Subject);
            List.Add (Temp);
            if (Number == ListNumber)
               start = (short)i;
            i++;
         }
      } while (Msg->Next (ListNumber) == TRUE);

      if (List.Elements > 0) {
         i = 0;
         Array = (CHAR **)malloc ((List.Elements + 1) * sizeof (CHAR *));
         if ((p = (CHAR *)List.First ()) != NULL)
            do {
               Array[i++] = p;
            } while ((p = (CHAR *)List.Next ()) != NULL);
         Array[i] = NULL;
         if ((i = wpickstr (6, 3, 21, 75, 5, LGREY|_BLACK, LGREY|_BLACK, WHITE|_BLUE, Array, start, NULL)) != -1) {
            Temp[0] = '\0';
            if ((p = strtok (Array[i], " ")) != NULL)
               strcpy (Temp, p);
            Number = Msg->MsgnToUid (atol (Temp));
            RetVal = TRUE;
         }
         if (Array != NULL)
            free (Array);
      }
      else
         getxch ();

      wclose ();
   }

   return (RetVal);
}

USHORT CMsgHeaderDlg (VOID)
{
   int menu_sel = 996;
   USHORT RetVal = FALSE;

   wopen (7, 2, 14, 76, 1, WHITE|_LGREY, WHITE|_LGREY);
   wshadow (DGREY|_BLACK);
   wtitle (" New Message ", TCENTER, WHITE|_LGREY);

   DisplayButton (4, 2, "   Ok   ");
   DisplayButton (4, 12, " Cancel ");

   do {
      wmenubegc ();
      wmenuitem (0, 1, " From    ", 0, 1, 0, NULL, 0, 0);
      wmenuitem (0, 42, " From Address ", 0, 2, 0, NULL, 0, 0);
      wmenuitem (1, 1, " To      ", 0, 3, 0, NULL, 0, 0);
      wmenuitem (1, 42, " To Address   ", 0, 4, 0, NULL, 0, 0);
      wmenuitem (2, 1, " Subject ", 0, 5, 0, NULL, 0, 0);
      wmenuitem (4, 2, "   Ok   ", 'O', 996, 0, NULL, 0, 0);
      wmenuitem (4, 12, " Cancel ", 'C', 997, 0, NULL, 0, 0);
      wmenuend ((short)menu_sel, M_OMNI|M_SAVE, 0, 0, BLACK|_GREEN, YELLOW|_GREEN, DGREY|_GREEN, WHITE|_GREEN);

      DisplayTextField (0, 11, Msg->From, 30, 30);
      DisplayTextField (0, 57, Msg->FromAddress, 15, 15);
      DisplayTextField (1, 11, Msg->To, 30, 30);
      DisplayTextField (1, 57, Msg->ToAddress, 15, 15);
      DisplayTextField (2, 11, Msg->Subject, 61, 61);

      switch (menu_sel = wmenuget ()) {
         case 1:
            GetTextField (0, 11, Msg->From, 30, 30);
            break;
         case 2:
            GetTextField (0, 57, Msg->FromAddress, 15, 15);
            break;
         case 3:
            GetTextField (1, 11, Msg->To, 30, 30);
            break;
         case 4:
            GetTextField (1, 57, Msg->ToAddress, 15, 15);
            break;
         case 5:
            GetTextField (2, 11, Msg->Subject, 61, 61);
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

VOID EditMessage (USHORT Reply, USHORT DoQuote)
{
   int i;
   CHAR Temp[128], Quote[16], FidoAddress[64], *p;
   CHAR Header[128], Footer[128];
   class TTextEditor *Editor;
   struct dostime_t d_time;
   struct dosdate_t d_date;

   if (Reply == TRUE) {
      if (Msg->Read (Number, 72) == FALSE) {
         Msg->New ();
         Reply = FALSE;
      }
   }

   _dos_getdate (&d_date);
   _dos_gettime (&d_time);

   if (Msg != NULL) {
      if (Reply == TRUE) {
         strcpy (FidoAddress, Msg->FromAddress);

         if ((p = (CHAR *)Msg->Text.First ()) != NULL)
            do {
               if (!strncmp (p, " * Origin: ", 11)) {
                  Msg->ToAddress[0] = '\0';

                  strcpy (Temp, &p[11]);
                  p = strchr (Temp, '\0');
                  while (--p > Temp) {
                     if (*p != ' ' && *p != ')')
                        break;
                     *p = '\0';
                  }
                  if (p > Temp) {
                     while (--p > Temp) {
                        if (*p == '(' || *p == ' ')
                           break;
                     }
                  }
                  if (*p == '(' || *p == ' ')
                     p++;
                  strcpy (Msg->FromAddress, p);
                  strcpy (FidoAddress, p);
                  break;
               }
               else if (!strncmp (p, "\001MSGID: ", 8)) {
                  strcpy (Temp, &p[8]);
                  if ((p = strtok (Temp, " ")) != NULL) {
                     if (strchr (p, ':') != NULL && strchr (p, '/') != NULL) {
                        Msg->ToAddress[0] = '\0';
                        strcpy (Msg->FromAddress, p);
                        strcpy (FidoAddress, p);
                     }
                  }
                  break;
               }
               else if (!strncmp (p, "\001From: ", 7)) {
                  Msg->FromAddress[0] = '\0';
                  Msg->ToAddress[0] = '\0';
                  ParseAddress (&p[7], Msg->From, Msg->FromAddress);
                  break;
               }
            } while ((p = (CHAR *)Msg->Text.Next ()) != NULL);

         if (DoQuote == TRUE) {
            Quote[0] = ' ';
            Quote[1] = '\0';
            i = 1;

            strcpy (Temp, Msg->From);
            if ((p = strtok (Temp, " ")) != NULL)
               do {
                  Quote[i++] = *p;
               } while ((p = strtok (NULL, " ")) != NULL);

            Quote[i++] = '>';
            Quote[i++] = ' ';
            Quote[i] = '\0';
         }

         Cfg->MailAddress.First ();

         strcpy (Msg->To, Msg->From);
         strcpy (Msg->ToAddress, Msg->FromAddress);
         strcpy (Msg->From, Cfg->SysopName);
         strcpy (Msg->FromAddress, Cfg->MailAddress.String);
      }
      else {
         Msg->New ();
         Cfg->MailAddress.First ();

         strcpy (Msg->To, "All");
         strcpy (Msg->From, Cfg->SysopName);
         strcpy (Msg->FromAddress, Cfg->MailAddress.String);
      }

      if (CMsgHeaderDlg () == TRUE) {
         strcpy (Temp, Msg->To);
         if ((p = strtok (Temp, " ")) != NULL)
            sprintf (Header, "Hello, %s!\n", p);
         else
            sprintf (Header, "Hello!\n");

         if (Scan != NULL && Scan->Tag[0] != '\0') {
            Cfg->MailAddress.First ();
            sprintf (Footer, " \n--- %s v%s\n * Origin: %s (%s)", NAME, VERSION, Cfg->SystemName, Cfg->MailAddress.String);
         }
         else
            sprintf (Footer, " \n--- %s v%s", NAME, VERSION);

         Msg->Written.Day = Msg->Arrived.Day = d_date.day;
         Msg->Written.Month = Msg->Arrived.Month = d_date.month;
         Msg->Written.Year = Msg->Arrived.Year = d_date.year;
         Msg->Written.Hour = Msg->Arrived.Hour = d_time.hour;
         Msg->Written.Minute = Msg->Arrived.Minute = d_time.minute;
         Msg->Written.Second = Msg->Arrived.Second = d_time.second;

         sprintf (Temp, "%lu of %lu                                   ", Msg->Number () + 1L, Msg->Number () + 1L);
         prints (2, 10, LGREY|_BLACK, Temp);
         sprintf (Temp, "%-30.30s", Msg->From);
         prints (3, 10, LGREY|_BLACK, Temp);
         sprintf (Temp, "%-16.16s", Msg->FromAddress);
         prints (3, 41, LGREY|_BLACK, Temp);
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Written.Day, Months[Msg->Written.Month - 1], Msg->Written.Year, Msg->Written.Hour, Msg->Written.Minute, Msg->Written.Minute);
         prints (3, 59, LGREY|_BLACK, Temp);

         sprintf (Temp, "%-30.30s", Msg->To);
         prints (4, 10, LGREY|_BLACK, Temp);
         sprintf (Temp, "%-16.16s", Msg->ToAddress);
         prints (4, 41, LGREY|_BLACK, Temp);
         sprintf (Temp, "%02d %s %d %2d:%02d:%02d", Msg->Arrived.Day, Months[Msg->Arrived.Month - 1], Msg->Arrived.Year, Msg->Arrived.Hour, Msg->Arrived.Minute, Msg->Arrived.Second);
         prints (4, 59, LGREY|_BLACK, Temp);

         sprintf (Temp, "%-68.68s", Msg->Subject);
         prints (5, 10, LGREY|_BLACK, Temp);

         if ((Editor = new TTextEditor) != NULL) {
            Editor->Text.Add (Header);
            while ((p = (CHAR *)Msg->Text.First ()) != NULL) {
               if (((*p != 0x01 && strncmp (p, "SEEN-BY:", 8)) || ShowKludges == TRUE) && DoQuote == TRUE) {
                  sprintf (Temp, "%s%s", Quote, p);
                  Editor->Text.Add (Temp);
               }
               Msg->Text.Remove ();
            }
            Editor->Text.Add (Footer);

            if (Editor->Run () == TRUE)
               Msg->Add (Editor->Text);

            delete Editor;
         }
      }
   }
}

VOID CHelpDlg (VOID)
{
   if (wopen (4, 14, 22, 76, 0, YELLOW|_BLACK, BLUE|_BLACK) != 0) {
      wtitle (" Help ", TCENTER, YELLOW|_BLACK);
      wshadow (DGREY|_BLACK);
      prints (22, 16, YELLOW|_BLACK, " Press any key to close ");

      wprints (0,  1, LGREY|_BLACK, "A, Alt-A        Change message area");
      wprints (1,  1, LGREY|_BLACK, "E, Alt-E, Ins   Write new message");
      wprints (2,  1, LGREY|_BLACK, "L, Alt-L        List messages");
      wprints (3,  1, LGREY|_BLACK, "R, Alt-R        Reply to current message");
      wprints (4,  1, LGREY|_BLACK, "Q, Alt-Q        Reply to current message (quoted)");
      wprints (5,  1, LGREY|_BLACK, "V, Alt-V        Show/hide kludge lines");
      wprints (6,  1, LGREY|_BLACK, "W, Alt-W        Write current message to disk (READER.OUT)");
      wprints (7,  1, LGREY|_BLACK, "X, Alt-X        Exit");
      wprints (8,  1, LGREY|_BLACK, "Right           Next message");
      wprints (9,  1, LGREY|_BLACK, "Left            Previous message");
      wprints (10, 1, LGREY|_BLACK, "End             Display last part of current message");
      wprints (11, 1, LGREY|_BLACK, "Home            Display first part of current message");
      wprints (12, 1, LGREY|_BLACK, "Down            Scroll message display");
      wprints (13, 1, LGREY|_BLACK, "Up              Scroll message display");
      wprints (14, 1, LGREY|_BLACK, "PgDn            Display next page of message");
      wprints (15, 1, LGREY|_BLACK, "PgUp            Display previous page of message");

      getxch ();
      wclose ();
   }
}

void main (int argc, char *argv[])
{
   int i;
   USHORT Line, EndRun;
   CHAR Temp[128], *Config, *Channel;
   time_t t, last_time = 0;
   struct tm *ltm;

   Config = Channel = NULL;

   for (i = 1; i < argc; i++) {
      if (Config == NULL)
         Config = argv[i];
      else if (Channel == NULL)
         Channel = argv[i];
   }

   if (Config == NULL)
      Config = getenv ("LORA_CONFIG");
   if (Channel == NULL)
      Channel = getenv ("LORA_CHANNEL");

   if ((Cfg = new TConfig) != NULL) {
      Cfg->TaskNumber = 1;
      if (Cfg->Load (Config, Channel) == FALSE)
         Cfg->Default ();
   }

   Data = new TMsgData (Cfg->SystemPath);
   Scan = NULL;

   DisplayScreen ();

   Msg = NULL;
   switch (Cfg->NetMailStorage) {
      case ST_JAM:
         Msg = new JAM (Cfg->NetMailPath);
         break;
      case ST_SQUISH:
         Msg = new SQUISH (Cfg->NetMailPath);
         break;
      case ST_FIDO:
         Msg = new FIDOSDM (Cfg->NetMailPath);
         break;
      case ST_ADEPT:
         Msg = new ADEPT (Cfg->NetMailPath);
         break;
      case ST_HUDSON:
         Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->NetMailBoard);
         break;
   }

   NetMail = TRUE;
   strcpy (AreaKey, "NetMail");
   Number = 0L;
   if ((User = new TUser (Cfg->UserFile)) != NULL) {
      if (User->GetData (Cfg->SysopName) == TRUE) {
         if (User->MsgTag->Read (AreaKey) == TRUE)
            Number = User->MsgTag->LastRead;
      }
      delete User;
   }
   if (Msg->ReadHeader (Number) == FALSE) {
      if (Msg->Next (Number) == FALSE)
         Msg->Previous (Number);
   }

   DisplayMessage ();
   Line = 1;
   prints (1, 1, LCYAN|_BLACK, " NetMail ");
   videoupdate ();

   kbput (0x1E00);

   EndRun = FALSE;
   while (EndRun == FALSE) {
      if (kbmhit ()) {
         switch (getxch ()) {
            case 0x3B00:
               CHelpDlg ();
               break;
            case 0x5000:   // Freccia giu'
               if ((TotalLines - Line + 1) > 17)
                  DisplayText (++Line);
               break;
            case 0x5100:   // Pagina giu'
               if ((TotalLines - Line + 1) > 17) {
                  Line += 17;
                  DisplayText (Line);
               }
               break;
            case 0x4800:   // Freccia su
               if (Line > 1)
                  DisplayText (--Line);
               break;
            case 0x4900:   // Pagina su
               if (Line > 17) {
                  Line -= 17;
                  DisplayText (Line);
               }
               else if (Line > 1) {
                  Line = 1;
                  DisplayText (Line);
               }
               break;
            case 0x4D00:   // Freccia destra
               if (Msg->Next (Number) == TRUE) {
                  DisplayMessage ();
                  Line = 1;
               }
               break;
            case 0x4B00:   // Freccia sinistra
               if (Msg->Previous (Number) == TRUE) {
                  DisplayMessage ();
                  Line = 1;
               }
               break;
            case 0x7500:   // Ctrl-End
               Number = Msg->Highest ();
               DisplayMessage ();
               Line = 1;
               break;
            case 0x7700:   // Ctrl-Home
               Number = Msg->Lowest ();
               DisplayMessage ();
               Line = 1;
               break;
            case 'A':
            case 0x1E00:   // Alt-A  - Cambio Area
               if ((User = new TUser (Cfg->UserFile)) != NULL) {
                  if (User->GetData (Cfg->SysopName) == TRUE) {
                     if (User->MsgTag->Read (AreaKey) == TRUE) {
                        User->MsgTag->LastRead = Number;
                        User->MsgTag->Update ();
                     }
                     else {
                        User->MsgTag->New ();
                        strcpy (User->MsgTag->Area, AreaKey);
                        User->MsgTag->Tagged = FALSE;
                        User->MsgTag->LastRead = Number;
                        User->MsgTag->Add ();
                     }
                     User->Update ();
                  }
                  delete User;
               }

               Data = new TMsgData (Cfg->SystemPath);

               if (ChangeArea () == TRUE) {
                  if (NetMail == FALSE && Dupes == FALSE && BadMsgs == FALSE && EMail == FALSE) {
                     strcpy (AreaKey, Data->Key);
                     whline (1, 1, 77, 0, LGREY|_BLACK);
                     sprintf (Temp, " %.70s ", Data->Display);
                     prints (1, 1, LCYAN|_BLACK, Temp);
                     switch (Data->Storage) {
                        case ST_JAM:
                           Msg = new JAM (Data->Path);
                           break;
                        case ST_SQUISH:
                           Msg = new SQUISH (Data->Path);
                           break;
                        case ST_FIDO:
                           Msg = new FIDOSDM (Data->Path);
                           break;
                        case ST_ADEPT:
                           Msg = new ADEPT (Data->Path);
                           break;
                        case ST_HUDSON:
                           Msg = new HUDSON (Data->Path, (UCHAR)Data->Board);
                           break;
                        case ST_USENET:
                           Msg = new USENET (Cfg->NewsServer, Data->NewsGroup);
                           break;
                     }
                  }
                  else if (NetMail == TRUE) {
                     strcpy (AreaKey, "NetMail");
                     whline (1, 1, 77, 0, LGREY|_BLACK);
                     sprintf (Temp, " %.70s ", AreaKey);
                     prints (1, 1, LCYAN|_BLACK, Temp);
                     switch (Cfg->NetMailStorage) {
                        case ST_JAM:
                           Msg = new JAM (Cfg->NetMailPath);
                           break;
                        case ST_SQUISH:
                           Msg = new SQUISH (Cfg->NetMailPath);
                           break;
                        case ST_FIDO:
                           Msg = new FIDOSDM (Cfg->NetMailPath);
                           break;
                        case ST_ADEPT:
                           Msg = new ADEPT (Cfg->NetMailPath);
                           break;
                        case ST_HUDSON:
                           Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->NetMailBoard);
                           break;
                     }
                  }
                  else if (EMail == TRUE) {
                     strcpy (AreaKey, "EMail");
                     whline (1, 1, 77, 0, LGREY|_BLACK);
                     sprintf (Temp, " %.70s ", AreaKey);
                     prints (1, 1, LCYAN|_BLACK, Temp);
                     switch (Cfg->MailStorage) {
                        case ST_JAM:
                           Msg = new JAM (Cfg->MailPath);
                           break;
                        case ST_SQUISH:
                           Msg = new SQUISH (Cfg->MailPath);
                           break;
                        case ST_FIDO:
                           Msg = new FIDOSDM (Cfg->MailPath);
                           break;
                        case ST_ADEPT:
                           Msg = new ADEPT (Cfg->MailPath);
                           break;
                        case ST_HUDSON:
                           Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->MailBoard);
                           break;
                     }
                  }
                  else if (Dupes == TRUE) {
                     strcpy (AreaKey, "Dupes");
                     whline (1, 1, 77, 0, LGREY|_BLACK);
                     sprintf (Temp, " %.70s ", AreaKey);
                     prints (1, 1, LCYAN|_BLACK, Temp);
                     switch (Cfg->DupeStorage) {
                        case ST_JAM:
                           Msg = new JAM (Cfg->DupePath);
                           break;
                        case ST_SQUISH:
                           Msg = new SQUISH (Cfg->DupePath);
                           break;
                        case ST_FIDO:
                           Msg = new FIDOSDM (Cfg->DupePath);
                           break;
                        case ST_ADEPT:
                           Msg = new ADEPT (Cfg->DupePath);
                           break;
                        case ST_HUDSON:
                           Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->DupeBoard);
                           break;
                     }
                  }
                  else if (BadMsgs == TRUE) {
                     strcpy (AreaKey, "BadMsgs");
                     whline (1, 1, 77, 0, LGREY|_BLACK);
                     sprintf (Temp, " %.70s ", AreaKey);
                     prints (1, 1, LCYAN|_BLACK, Temp);
                     switch (Cfg->BadStorage) {
                        case ST_JAM:
                           Msg = new JAM (Cfg->BadPath);
                           break;
                        case ST_SQUISH:
                           Msg = new SQUISH (Cfg->BadPath);
                           break;
                        case ST_FIDO:
                           Msg = new FIDOSDM (Cfg->BadPath);
                           break;
                        case ST_ADEPT:
                           Msg = new ADEPT (Cfg->BadPath);
                           break;
                        case ST_HUDSON:
                           Msg = new HUDSON (Cfg->HudsonPath, (UCHAR)Cfg->BadBoard);
                           break;
                     }
                  }

                  if (Msg != NULL && Scan != NULL) {
                     Scan->Messages = Msg->Number ();
                     Scan->Update ();
                  }

                  Number = 0L;
                  if ((User = new TUser (Cfg->UserFile)) != NULL) {
                     if (User->GetData (Cfg->SysopName) == TRUE) {
                        if (User->MsgTag->Read (AreaKey) == TRUE)
                           Number = User->MsgTag->LastRead;
                     }
                     delete User;
                  }

                  if (Msg != NULL)
                     Msg->Next (Number);
                  DisplayMessage ();
                  Line = 1;
               }
               break;
            case 'E':
            case 0x1200:   // Alt-E = Edit new message
            case 0x5200:   // Ins   = Edit new message
               EditMessage (FALSE, FALSE);
               if (Number == 0L)
                  Msg->Next (Number);
               DisplayMessage ();
               break;
            case 'L':
            case 0x2600:   // Alt-L = List messages
               if (CMessageListDlg () == TRUE) {
                  DisplayMessage ();
                  Line = 1;
               }
               break;
            case 'R':
            case 0x1300:   // Alt-R = Reply
               EditMessage (TRUE, FALSE);
               if (Number == 0L)
                  Msg->Next (Number);
               DisplayMessage ();
               break;
            case 'Q':
            case 0x1000:   // Alt-Q = Reply + Quote
               EditMessage (TRUE, TRUE);
               if (Number == 0L)
                  Msg->Next (Number);
               DisplayMessage ();
               break;
            case 'V':
            case 0x2F00:   // Alt-V  - Show Kludges
               ShowKludges = (ShowKludges == TRUE) ? FALSE : TRUE;
               DisplayMessage ();
               Line = 1;
               break;
            case 'W':
            case 0x1100:   // Alt-W  - Export Message
               ExportMessage ();
               break;
            case 'X':
            case 0x2D00:   // Alt-X  - Uscita
               if (MessageBox (" Quit ", "Are you really sure ?") == TRUE)
                  EndRun = TRUE;
               break;
         }
      }
      if ((t = time (NULL)) != last_time) {
         last_time = t;
         ltm = localtime (&last_time);
         sprintf (Temp, " %02d:%02d:%02d ", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
         prints (24, (short)(79 - strlen (Temp)), LCYAN|_BLACK, Temp);
         sprintf (Temp, " %02d %s %04d ", ltm->tm_mday, Months[ltm->tm_mon], ltm->tm_year + 1900);
         prints (24, 1, LCYAN|_BLACK, Temp);
      }
   }

   if ((User = new TUser (Cfg->UserFile)) != NULL) {
      if (User->GetData (Cfg->SysopName) == TRUE) {
         if (User->MsgTag->Read (AreaKey) == TRUE) {
            User->MsgTag->LastRead = Number;
            User->MsgTag->Update ();
         }
         else {
            User->MsgTag->New ();
            strcpy (User->MsgTag->Area, AreaKey);
            User->MsgTag->Tagged = FALSE;
            User->MsgTag->LastRead = Number;
            User->MsgTag->Add ();
         }
         User->Update ();
      }
      delete User;
   }

   if (Data != NULL)
      delete Data;
   if (Cfg != NULL)
      delete Cfg;

   ClearScreen ();
}
