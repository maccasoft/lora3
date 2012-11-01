
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.11
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora.h"
#include <errno.h>

typedef struct {
   PSZ    Key;
   PSZ    Text;
   USHORT Len;
} KEYWORDS;

KEYWORDS MeccaKeywords[] = {
   // Color Tokens
   { "black",        "\026\001\000", 3 },
   { "blue",         "\026\001\001", 3 },
   { "green",        "\026\001\002", 3 },
   { "cyan",         "\026\001\003", 3 },
   { "red",          "\026\001\004", 3 },
   { "magenta",      "\026\001\005", 3 },
   { "brown",        "\026\001\006", 3 },
   { "gray",         "\026\001\007", 3 },
   { "darkgray",     "\026\001\010", 3 },
   { "lightblue",    "\026\001\011", 3 },
   { "lightgreen",   "\026\001\012", 3 },
   { "lightcyan",    "\026\001\013", 3 },
   { "lightred",     "\026\001\014", 3 },
   { "lightmagenta", "\026\001\015", 3 },
   { "yellow",       "\026\001\016", 3 },
   { "white",        "\026\001\017", 3 },

   // Cursor Control and Video Tokens
   { "bell",         "\007",         1 },
   { "bs",           "\010",         1 },
   { "clreol",       "\026\007",     2 },
   { "cls",          "\014",         1 },
   { "cr",           "\015",         1 },
   { "down",         "\026\004",     2 },
   { "left",         "\026\005",     2 },
   { "locate",       "\026\008",     2 },
   { "lf",           "\012",         1 },
   { "tab",          "\011",         1 },
   { "right",        "\026\006",     2 },
   { "up",           "\026\003",     2 },

   // Informational Tokens
   { "age",          "\0062",        2 },
   { "city",         "\006\003",     2 },
   { "date",         "\006\004",     2 },
   { "dl",           "\006\030",     2 },
   { "fname",        "\006\006",     2 },
   { "file_carea",   "\027\006A",    3 },
   { "file_cname",   "\027\006N",    3 },
   { "first",        "\006\006",     2 },
   { "length",       "\006\014",     2 },
   { "minutes",      "\006\013",     2 },
   { "msg_carea",    "\027\015A",    3 },
   { "msg_cmsg",     "\027\015L",    3 },
   { "msg_cname",    "\027\015N",    3 },
   { "msg_hmsg",     "\027\015H",    3 },
   { "msg_nummsg",   "\027\015#",    3 },
   { "node_num",     "\027jN",       3 },
   { "phone",        "\027P",        2 },
   { "ratio",        "\006\031",     2 },
   { "realname",     "\027R",        2 },
   { "remain",       "\006\017",     2 },
   { "syscall",      "\006\021",     2 },
   { "sys_name",     "\022\003",     2 },
   { "sysop_name",   "\022\004",     2 },
   { "time",         "\006\024",     2 },
   { "timeoff",      "\006\020",     2 },
   { "ul",           "\006R",        2 },
   { "user",         "\006\002",     2 },
   { "usercall",     "\006\005",     2 },

   // Questionnaire Tokens
   { "choice",       "\017U",        2 },
   { "menu",         "\017R",        2 },
   { "open",         "\017O",        2 },

   // Flow control
   { "color",        "\017E",        2 },
   { "colour",       "\017E",        2 },
   { "endcolor",     "\017e",        2 },
   { "endcolour",    "\017e",        2 },
   { "endrip",       "\017I",        2 },
   { "iftask",       "\027b",        2 },
   { "rip",          "\017G",        2 },

   { "enter",        "\001",         1 },
   { "pause",        "\006\007",     2 },
   { "quit",         "\017Q",        2 },
   { "quote",        "\006\001",     2 },
   { "repeat",       "\031",         1 },
   { "repeatseq",    "\026\031",     2 },

   { NULL, NULL }
};

#define NO_DROPFILE        0
#define DOOR_SYS           1
#define DOORX_SYS          2
#define DORINFO1_DEF       3
#define DORINFOX_DEF       4

#define SYSTEM_TERMQ       "\\QUEUES\\TELTERM.QUE"

TEmbedded::TEmbedded (void)
{
   Com = NULL;
   Snoop = NULL;
   User = NULL;
   Log = NULL;
   Cfg = NULL;
   Language = NULL;

   MsgArea = NULL;
   FileArea = NULL;

   Task = 1;
   EndRun = Hangup = FALSE;
   Ansi = Color = TRUE;
   Rip = Avatar = FALSE;
   HotKey = More = TRUE;
   strcpy (Path, ".\\");
   AltPath[0] = '\0';
   ScreenHeight = 24;
   TimeLimit = 0;
   StartCall = 0L;
   last_time = 0L;
   IsDown = FALSE;

   fp = NULL;
   AnswerFile = NULL;
   Position = NULL;
   StopNested = Stop = FALSE;
   Nested = 0;
   Response = '\0';
   Traslate[0] = '\0';
   TrasPtr = Traslate;
   TrasLen = 0;
   IsMec = FALSE;
   Required = FALSE;
}

TEmbedded::~TEmbedded (void)
{
   if (fp != NULL)
      fclose (fp);
}

USHORT TEmbedded::AbortSession (VOID)
{
   USHORT RetVal = FALSE;
   ULONG Len, Timeout;

   if (Com != NULL) {
      if (Com->Carrier () == FALSE)
         RetVal = TRUE;

      if (RetVal == TRUE && IsDown == FALSE) {
         Timeout = TimerSet (Cfg->CarrierDropTimeout * 100L);
         do {
            if (Com != NULL) {
               if (Com->Carrier () == TRUE)
                  RetVal = FALSE;
            }
         } while (TimeUp (Timeout) == FALSE && RetVal == TRUE);
      }
   }

   if (Snoop != NULL) {
      if (Snoop->Carrier () == FALSE)
         RetVal = TRUE;
   }

   if (RetVal == FALSE && TimeLimit != 0) {
      Len = (time (NULL) - StartCall) / 60L;
      if (Len >= TimeLimit)
         RetVal = TRUE;
   }

   IsDown = RetVal;

   return (RetVal);
}

VOID TEmbedded::ClrEol (VOID)
{
   if (Com != NULL) {
      if (Avatar == TRUE) {
         Com->BufferByte (CTRLV);
         Com->BufferByte (CTRLG);
      }
      else if (Ansi == TRUE)
         Com->BufferBytes ((UCHAR *)"\x1B[K", 3);
   }

   if (Snoop != NULL && (Ansi == TRUE || Avatar == TRUE))
      Snoop->BufferBytes ((UCHAR *)"\x1B[K", 3);
}

USHORT TEmbedded::DisplayFile (PSZ pszFile)
{
   SHORT c;
   USHORT RetVal = FALSE;
   CHAR TempName[128];

   LastChar = EOF;
   Line = 1;
   strcpy (TempName, pszFile);
   OnExit[0] = '\0';
   IsMec = FALSE;

   if ((fp = OpenFile (TempName)) != NULL) {
      RetVal = TRUE;
      if (Log != NULL)
         Log->Write (":Display File %s", TempName);
      while ((c = GetNextChar ()) != EOF && AbortSession () == FALSE) {
         if (c < 32)
            ProcessControl ((UCHAR)c);
         else {
            if (Com != NULL)
               Com->BufferByte ((UCHAR)c);
            if (Snoop != NULL)
               Snoop->BufferByte ((UCHAR)c);
         }
      }

      fclose (fp);
      fp = NULL;
   }

   if (AnswerFile != NULL) {
      fclose (AnswerFile);
      AnswerFile = NULL;
      if (Log != NULL)
         Log->Write (":Answer file closed");
   }

   if (Com != NULL)
      Com->UnbufferBytes ();
   if (Snoop != NULL)
      Snoop->UnbufferBytes ();

   if (OnExit[0] != '\0')
      DisplayFile (OnExit);

   Stop = FALSE;
   if (Nested == 0)
      StopNested = FALSE;

   return (RetVal);
}

USHORT TEmbedded::DisplayPrompt (PSZ pszString, USHORT usColor, USHORT usHilight, USHORT usUnbuffer)
{
   SHORT c;
   USHORT RetVal = FALSE, Hilight = FALSE;
   CHAR Temp[16];

   LastChar = EOF;
   Line = 1;
   IsMec = FALSE;

   if (LastColor != usColor)
      SetColor (usColor);

   if ((Position = pszString) != NULL) {
      RetVal = TRUE;
      while ((c = GetNextChar ()) != EOF && AbortSession () == FALSE) {
         if (c < 32) {
            if (c == '\n') {
               if (Com != NULL)
                  Com->BufferByte ('\r');
               if (Snoop != NULL)
                  Snoop->BufferByte ('\r');
            }
            ProcessControl ((UCHAR)c);
         }
         else if (c == '^') {
            if (PeekNextChar () != '^') {
               Hilight = (USHORT)((Hilight == TRUE) ? FALSE : TRUE);
               SetColor ((Hilight == TRUE) ? usHilight : usColor);
            }
            else {
               GetNextChar ();
               if (Com != NULL)
                  Com->BufferByte ((unsigned char)c);
               if (Snoop != NULL)
                  Snoop->BufferByte ((unsigned char)c);
            }
         }
         else if (c == '\x7E') {
            sprintf (Temp, "%lu", TimeRemain ());
            if (Com != NULL)
               Com->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            if (Snoop != NULL)
               Snoop->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else {
            if (Com != NULL)
               Com->BufferByte ((unsigned char)c);
            if (Snoop != NULL)
               Snoop->BufferByte ((unsigned char)c);
         }
      }
   }

   if (AnswerFile != NULL) {
      fclose (AnswerFile);
      AnswerFile = NULL;
      if (Log != NULL)
         Log->Write (":Answer file closed");
   }

   if (usUnbuffer == TRUE) {
      if (Com != NULL)
         Com->UnbufferBytes ();
      if (Snoop != NULL)
         Snoop->UnbufferBytes ();
   }

   return (RetVal);
}

USHORT TEmbedded::DisplayString (PSZ pszString)
{
   SHORT c;
   USHORT RetVal = FALSE;

   LastChar = EOF;
   Line = 1;
   IsMec = FALSE;

   if ((Position = pszString) != NULL) {
      RetVal = TRUE;
      while ((c = GetNextChar ()) != EOF && AbortSession () == FALSE) {
         if (c < 32) {
            if (c == '\n') {
               if (Com != NULL)
                  Com->BufferByte ('\r');
               if (Snoop != NULL)
                  Snoop->BufferByte ('\r');
            }
            ProcessControl ((UCHAR)c);
         }
         else {
            if (Com != NULL)
               Com->BufferByte ((unsigned char)c);
            if (Snoop != NULL)
               Snoop->BufferByte ((unsigned char)c);
         }
      }
   }

   if (AnswerFile != NULL) {
      fclose (AnswerFile);
      AnswerFile = NULL;
      if (Log != NULL)
         Log->Write (":Answer file closed");
   }

   if (Com != NULL)
      Com->UnbufferBytes ();
   if (Snoop != NULL)
      Snoop->UnbufferBytes ();

   return (RetVal);
}

USHORT TEmbedded::GetAnswer (USHORT flQuestion)
{
   CHAR answer[10];

   if (flQuestion & ASK_HELP) {
      if (flQuestion & ASK_DEFYES)
         Printf (Language->Text (LNG_DEFYESNOHELP));
      else if (flQuestion & ASK_DEFNO)
         Printf (Language->Text (LNG_YESDEFNOHELP));
   }
   else {
      if (flQuestion & ASK_DEFYES)
         Printf (Language->Text (LNG_DEFYESNO));
      else if (flQuestion & ASK_DEFNO)
         Printf (Language->Text (LNG_YESDEFNO));
   }

   while (AbortSession () == FALSE) {
      Input (answer, 1, INP_NOCRLF|INP_HOTKEY);

      if (AbortSession () == FALSE) {
         if (answer[0] == '\0') {
            Printf ("\n");
            if (flQuestion & ASK_DEFYES)
               return (ANSWER_YES);
            else if (flQuestion & ASK_DEFNO)
               return (ANSWER_NO);
         }

         answer[0] = (CHAR)toupper (answer[0]);
         if (answer[0] == Language->Yes) {
            Printf ("\n");
            return (ANSWER_YES);
         }
         if (answer[0] == Language->No) {
            Printf ("\n");
            return (ANSWER_NO);
         }
         if (answer[0] == Language->Help) {
            if (flQuestion & ASK_HELP) {
               Printf ("\n");
               return (ANSWER_HELP);
            }
         }

         Printf ("\x08 \x08");
      }
   }

   return (0);
}

USHORT TEmbedded::Getch (VOID)
{
   USHORT RetVal = 0;

   if (Com != NULL || Snoop != NULL) {
      while (AbortSession () == FALSE && RetVal == 0) {
         if (Com != NULL) {
            if (Com->BytesReady () == TRUE)
               RetVal = Com->ReadByte ();
         }
         else if (Snoop != NULL) {
            if (Snoop->BytesReady () == TRUE)
               RetVal = Snoop->ReadByte ();
         }
      }
   }

   return (RetVal);
}

VOID TEmbedded::Idle (VOID)
{
   time_t t;

   if ((t = time (NULL)) != last_time) {
      last_time = t;
      if (Snoop != NULL) {
         Snoop->SetTime (t);
         Snoop->SetTimeLeft (TimeRemain (TRUE));
      }
      else if (Com != NULL) {
         Com->SetTime (t);
         Com->SetTimeLeft (TimeRemain (TRUE));
      }
   }
}

// ----------------------------------------------------------------------
// Descrizione:
//    Accetta una stringa in input dall'utente.
//
// Argomenti:
//    pszBuffer = Puntatore ad un buffer di caratteri per la stringa
//                digitata dall'utente.
//    usMaxlen  = Lunghezza massima della stringa
//    flAttrib  = Attributi di input. Puo' essere uno dei seguenti
//                valori:
//
//                INP_FIELD   = Traccia uno sfondo pari alla dimensione
//                              massima della stringa.
//                INP_FANCY   = Forza le maiuscole (per i nomi).
//                INP_NOCRLF  = Non visualizza il CR/LF alla fine della
//                              stringa di input.
//                INP_PWD     = Visualizza un asterisco al posto delle
//                              lettere digitate (per l'inserimento di password).
//                INP_NOCOLOR = Non effettua il reset del colore alla fine
//                              dell'inserimento.
//                INP_HOTKEY  = Attiva gli hotkey se richiesto anche
//                              dall'utente.
//                INP_NUMERIC = Indica un input numerico, tutto il resto
//                              viene ignorato.
//
// Valori di ritorno:
//    pszBuffer = Stringa digitata dall'utente.
// ----------------------------------------------------------------------

PSZ TEmbedded::Input (PSZ pszBuffer, USHORT usMaxlen, USHORT flAttrib)
{
   USHORT i, c, Len, DoFancy, Enter;
   PSZ p;

   p = pszBuffer;
   Len = 0;
   DoFancy = TRUE;
   Enter = FALSE;

   if (Com != NULL)
      Com->UnbufferBytes ();
   if (Snoop != NULL)
      Snoop->UnbufferBytes ();

   if ((flAttrib & INP_FIELD) && (Ansi == TRUE || Avatar == TRUE) && Color == TRUE) {
      for (i = 0; i < usMaxlen; i++) {
         if (Com != NULL)
            Com->BufferByte (' ');
         if (Snoop != NULL)
            Snoop->BufferByte (' ');
      }
      for (i = 0; i < usMaxlen; i++) {
         if (Com != NULL)
            Com->BufferByte (8);
         if (Snoop != NULL)
            Snoop->BufferByte (8);
      }
      if (Com != NULL)
         Com->UnbufferBytes ();
      if (Snoop != NULL)
         Snoop->UnbufferBytes ();
   }

   LastActivity = time (NULL);

   while (Enter == FALSE && AbortSession () == FALSE) {
      if (KBHit ()) {
         LastActivity = time (NULL);
         if ((c = Getch ()) == 0)
            c = (short)(Getch () << 8);

         if (c == 13)
            Enter = TRUE;
         else if (c == 8 || c == 127) {
            if (Len > 0) {
               if (Com != NULL)
                  Com->SendBytes ((UCHAR *)"\x08 \x08", 3);
               if (Snoop != NULL)
                  Snoop->SendBytes ((UCHAR *)"\x08 \x08", 3);
               p--;
               Len--;
            }
         }
         else if (c >= 32 && c < 127) {
            if (Len < usMaxlen) {
               *p++ = (CHAR)c;
               Len++;
               if (flAttrib & INP_PWD)
                  Putch ('*');
               else
                  Putch ((UCHAR)c);

               if (!(flAttrib & INP_NUMERIC) || !isdigit (c)) {
                  if (Len == 1 && flAttrib & INP_HOTKEY && !isdigit (c)) {
                     if (HotKey == TRUE)
                        Enter = TRUE;
                  }
               }
            }
         }
      }
      else
         Idle ();
   }

   *p = '\0';

   if (flAttrib & INP_FANCY) {
      strlwr (pszBuffer);
      p = pszBuffer;
      *p = (CHAR)toupper (*p);
      while ((p = strchr (p, ' ')) != NULL) {
         p[1] = (CHAR)toupper (p[1]);
         p++;
      }
   }

   if (AbortSession () == FALSE) {
      SetColor (7);
      if (!(flAttrib & INP_NOCRLF)) {
         if (Com != NULL)
            Com->BufferBytes ((UCHAR *)"\r\n", 2);
         if (Snoop != NULL)
            Snoop->BufferBytes ((UCHAR *)"\r\n", 2);
      }
      if (Com != NULL)
         Com->UnbufferBytes ();
      if (Snoop != NULL)
         Snoop->UnbufferBytes ();
   }
   else {
      if (Com != NULL)
         Com->ClearOutbound ();
      if (Snoop != NULL)
         Snoop->ClearOutbound ();
   }

   return (pszBuffer);
}

PSZ TEmbedded::GetString (PSZ pszBuffer, USHORT usMaxlen, USHORT flAttrib)
{
   return (Input (pszBuffer, usMaxlen, flAttrib));
}

SHORT TEmbedded::GetNextChar (VOID)
{
   SHORT RetVal = EOF, c;
   UCHAR Byte;

   if (Stop == FALSE && (Nested == 0 || StopNested == FALSE)) {
      if (TrasLen != 0) {
         RetVal = *TrasPtr++;
         TrasLen--;
      }
      else if (fp != NULL) {
         if (LastChar == EOF) {
            if (fp != NULL)
               RetVal = (SHORT)fgetc (fp);
         }
         else {
            RetVal = LastChar;
            LastChar = EOF;
         }
      }
      else if (Position != NULL) {
         if (LastChar == EOF) {
            if ((RetVal = Position[0]) == '\0')
               RetVal = EOF;
            else
               Position++;
         }
         else {
            RetVal = LastChar;
            LastChar = EOF;
         }
      }

      if (RetVal == '[' && IsMec == TRUE) {
         if (PeekNextChar () != '[') {
            TranslateKeyword ();
            if (TrasLen != 0) {
               RetVal = *TrasPtr++;
               TrasLen--;
            }
            else
               RetVal = GetNextChar ();
         }
         else
            RetVal = GetNextChar ();
      }

      if (RetVal == '\\') {
         if (fp != NULL)
            c = (SHORT)fgetc (fp);
         else if (Position != NULL) {
            if ((c = Position[0]) == '\0')
               c = EOF;
            else
               Position++;
         }

         if (isdigit (c)) {
            Byte = (UCHAR)((c - '0') * 64);
            if (fp != NULL)
               c = (SHORT)fgetc (fp);
            else if (Position != NULL) {
               if ((c = Position[0]) == '\0')
                  c = EOF;
               else
                  Position++;
            }
            Byte += (UCHAR)((c - '0') * 8);
            if (fp != NULL)
               c = (SHORT)fgetc (fp);
            else if (Position != NULL) {
               if ((c = Position[0]) == '\0')
                  c = EOF;
               else
                  Position++;
            }
            Byte += (UCHAR)(c - '0');
            RetVal = Byte;
         }
         else if (c == 'x') {
            if (fp != NULL)
               c = (SHORT)fgetc (fp);
            else if (Position != NULL) {
               if ((c = Position[0]) == '\0')
                  c = EOF;
               else
                  Position++;
            }
            if ((Byte = (UCHAR)(c - '0')) > 9)
               Byte -= 7;
            Byte <<= 4;
            if (fp != NULL)
               c = (SHORT)fgetc (fp);
            else if (Position != NULL) {
               if ((c = Position[0]) == '\0')
                  c = EOF;
               else
                  Position++;
            }
            c -= '0';
            if (c > 9)
               c -= 7;
            Byte |= (UCHAR)c;
            RetVal = Byte;
         }
         else if (c == 'n')
            RetVal = '\n';
         else if (c == 'r')
            RetVal = '\r';
         else if (c == 'a')
            RetVal = '\a';
         else if (c == 't')
            RetVal = '\t';
         else if (c == '\\')
            RetVal = '\\';
         else {
            LastChar = c;
            RetVal = '\\';
         }
      }
   }

   return (RetVal);
}

USHORT TEmbedded::KBHit (VOID)
{
   USHORT RetVal = FALSE;

   if (Com != NULL) {
      if (Com->BytesReady () == TRUE)
         RetVal = TRUE;
   }
   else if (Snoop != NULL) {
      if (Snoop->BytesReady () == TRUE)
         RetVal = TRUE;
   }

   return (RetVal);
}

SHORT TEmbedded::MoreQuestion (SHORT nLine)
{
   USHORT SaveColor;
   CHAR Temp[2];

   if (nLine == -1 || nLine == 0 || More == FALSE)
      return (nLine);

   if (++nLine >= (SHORT)(ScreenHeight - 1)) {
      SaveColor = LastColor;
      while (AbortSession () == FALSE) {
         OutString (Language->Text(LNG_MOREQUESTION));
         GetString (Temp, 1, INP_NOCRLF|INP_NOCOLOR|INP_HOTKEY);
         if (toupper (Temp[0]) == Language->Text(LNG_QUIT)[0]) {
            OutString (Language->Text(LNG_DELETEMOREQUESTION));
            SetColor (SaveColor);
            return (0);
         }
         else if (toupper (Temp[0]) == Language->Text(LNG_NONSTOP)[0]) {
            OutString (Language->Text(LNG_DELETEMOREQUESTION));
            SetColor (SaveColor);
            return (-1);
         }
         else if (toupper (Temp[0]) == Language->Text(LNG_CONTINUE)[0] || Temp[0] == '\0') {
            OutString (Language->Text(LNG_DELETEMOREQUESTION));
            SetColor (SaveColor);
            return (1);
         }
         else
            OutString ("\r");
      }
   }

   return (nLine);
}

FILE *TEmbedded::OpenFile (PSZ pszName, PSZ pszAccess)
{
   FILE *fp;
   CHAR Temp[128], *p;

   strcpy (Temp, pszName);
   strlwr (Temp);
   IsMec = FALSE;

   if ((fp = _fsopen (AdjustPath (Temp), pszAccess, SH_DENYNO)) == NULL) {
      if ((p = strchr (Temp, '.')) != NULL)
         *p = '\0';
      strcat (Temp, ".bbs");
      if ((fp = _fsopen (Temp, pszAccess, SH_DENYNO)) == NULL) {
         if (fp == NULL && Rip == TRUE) {
            if ((p = strchr (Temp, '.')) != NULL)
               *p = '\0';
            strcat (Temp, ".rip");
            fp = _fsopen (Temp, pszAccess, SH_DENYNO);
         }
         if (fp == NULL && Avatar == TRUE) {
            if ((p = strchr (Temp, '.')) != NULL)
               *p = '\0';
            strcat (Temp, ".avt");
            fp = _fsopen (Temp, pszAccess, SH_DENYNO);
         }
         if (fp == NULL && Ansi == TRUE) {
            if ((p = strchr (Temp, '.')) != NULL)
               *p = '\0';
            strcat (Temp, ".ans");
            fp = _fsopen (Temp, pszAccess, SH_DENYNO);
         }
         if (fp == NULL) {
            if ((p = strchr (Temp, '.')) != NULL)
               *p = '\0';
            strcat (Temp, ".mec");
            if ((fp = _fsopen (Temp, pszAccess, SH_DENYNO)) != NULL)
               IsMec = TRUE;
         }
      }
      if (fp == NULL && AltPath[0] != '\0') {
         strcpy (Temp, AltPath);
         strcat (Temp, pszName);
         strlwr (Temp);
         if ((fp = _fsopen (AdjustPath (Temp), pszAccess, SH_DENYNO)) == NULL) {
            if ((p = strchr (Temp, '.')) != NULL)
               *p = '\0';
            strcat (Temp, ".bbs");
            if ((fp = _fsopen (Temp, pszAccess, SH_DENYNO)) == NULL) {
               if (fp == NULL && Rip == TRUE) {
                  if ((p = strchr (Temp, '.')) != NULL)
                     *p = '\0';
                  strcat (Temp, ".rip");
                  fp = _fsopen (Temp, pszAccess, SH_DENYNO);
               }
               if (fp == NULL && Avatar == TRUE) {
                  if ((p = strchr (Temp, '.')) != NULL)
                     *p = '\0';
                  strcat (Temp, ".avt");
                  fp = _fsopen (Temp, pszAccess, SH_DENYNO);
               }
               if (fp == NULL && Ansi == TRUE) {
                  if ((p = strchr (Temp, '.')) != NULL)
                     *p = '\0';
                  strcat (Temp, ".ans");
                  fp = _fsopen (Temp, pszAccess, SH_DENYNO);
               }
               if (fp == NULL) {
                  if ((p = strchr (Temp, '.')) != NULL)
                     *p = '\0';
                  strcat (Temp, ".mec");
                  if ((fp = _fsopen (Temp, pszAccess, SH_DENYNO)) != NULL)
                     IsMec = TRUE;
               }
            }
         }
      }
      if (fp == NULL) {
         strcpy (Temp, Path);
         strcat (Temp, pszName);
         strlwr (Temp);
         if ((fp = _fsopen (AdjustPath (Temp), pszAccess, SH_DENYNO)) == NULL) {
            if ((p = strchr (Temp, '.')) != NULL)
               *p = '\0';
            strcat (Temp, ".bbs");
            if ((fp = _fsopen (Temp, pszAccess, SH_DENYNO)) == NULL) {
               if (fp == NULL && Rip == TRUE) {
                  if ((p = strchr (Temp, '.')) != NULL)
                     *p = '\0';
                  strcat (Temp, ".rip");
                  fp = _fsopen (Temp, pszAccess, SH_DENYNO);
               }
               if (fp == NULL && Avatar == TRUE) {
                  if ((p = strchr (Temp, '.')) != NULL)
                     *p = '\0';
                  strcat (Temp, ".avt");
                  fp = _fsopen (Temp, pszAccess, SH_DENYNO);
               }
               if (fp == NULL && Ansi == TRUE) {
                  if ((p = strchr (Temp, '.')) != NULL)
                     *p = '\0';
                  strcat (Temp, ".ans");
                  fp = _fsopen (Temp, pszAccess, SH_DENYNO);
               }
               if (fp == NULL) {
                  if ((p = strchr (Temp, '.')) != NULL)
                     *p = '\0';
                  strcat (Temp, ".mec");
                  if ((fp = _fsopen (Temp, pszAccess, SH_DENYNO)) != NULL)
                     IsMec = TRUE;
               }
            }
         }
      }
   }

   return (fp);
}

VOID TEmbedded::OutString (PSZ pszFormat, ...)
{
   va_list arglist;
   PSZ p;

   va_start (arglist, pszFormat);
   vsprintf (Temp2, pszFormat, arglist);
   va_end (arglist);

   p = Temp2;
   while (*p != '\0') {
      if (*p == CTRLV && p[1] == CTRLA) {
         SetColor (p[2]);
         p += 3;
      }
      else {
         if (Com != NULL)
            Com->BufferByte ((UCHAR)*p);
         if (Snoop != NULL)
            Snoop->BufferByte ((UCHAR)*p);
         p++;
      }
   }
}

SHORT TEmbedded::PeekNextChar (VOID)
{
   SHORT RetVal = EOF, c;
   UCHAR Byte;

   if (Stop == FALSE && (Nested == 0 || StopNested == FALSE)) {
      if (fp != NULL) {
         if (LastChar == EOF) {
            if (fp != NULL)
               RetVal = (SHORT)fgetc (fp);
         }
         else
            RetVal = LastChar;
      }
      else if (Position != NULL) {
         if (LastChar == EOF) {
            if (Position != NULL) {
               if ((RetVal = Position[0]) == '\0')
                  RetVal = EOF;
               else
                  Position++;
            }
         }
         else
            RetVal = LastChar;
      }

      if (RetVal == '\\' && LastChar == EOF) {
         if (fp != NULL)
            c = (SHORT)fgetc (fp);
         else if (Position != NULL) {
            if ((c = Position[0]) == '\0')
               c = EOF;
            else
               Position++;
         }

         if (isdigit (c)) {
            Byte = (UCHAR)((c - '0') * 64);
            if (fp != NULL)
               c = (SHORT)fgetc (fp);
            else if (Position != NULL) {
               if ((c = Position[0]) == '\0')
                  c = EOF;
               else
                  Position++;
            }
            Byte += (UCHAR)((c - '0') * 8);
            if (fp != NULL)
               c = (SHORT)fgetc (fp);
            else if (Position != NULL) {
               if ((c = Position[0]) == '\0')
                  c = EOF;
               else
                  Position++;
            }
            Byte += (UCHAR)(c - '0');
            RetVal = Byte;
         }
         else if (c == 'x') {
            if (fp != NULL)
               c = (SHORT)fgetc (fp);
            else if (Position != NULL) {
               if ((c = Position[0]) == '\0')
                  c = EOF;
               else
                  Position++;
            }
            if ((Byte = (UCHAR)(c - '0')) > 9)
               Byte -= 7;
            Byte <<= 4;
            if (fp != NULL)
               c = (SHORT)fgetc (fp);
            else if (Position != NULL) {
               if ((c = Position[0]) == '\0')
                  c = EOF;
               else
                  Position++;
            }
            c -= '0';
            if (c > 9)
               c -= 7;
            Byte |= (UCHAR)c;
            RetVal = Byte;
         }
         else if (c == 'n')
            RetVal = '\n';
         else if (c == 'r')
            RetVal = '\r';
         else if (c == 'a')
            RetVal = '\a';
         else if (c == 't')
            RetVal = '\t';
         else if (c == '\\')
            RetVal = '\\';
         else
            RetVal = c;
      }
   }

   LastChar = RetVal;

   return (RetVal);
}

VOID TEmbedded::PressEnter (VOID)
{
   USHORT SaveColor;
   CHAR Temp[2];

   if (Language != NULL) {
      SaveColor = LastColor;
      OutString (Language->Text(LNG_PRESSENTER));
      GetString (Temp, 1, INP_NOCRLF|INP_NOCOLOR|INP_HOTKEY);
      OutString ("\r                             \r");
      SetColor (SaveColor);
   }
}

VOID TEmbedded::Printf (PSZ pszFormat, ...)
{
   va_list arglist;

   va_start (arglist, pszFormat);
   vsprintf (Temp, pszFormat, arglist);
   va_end (arglist);

   DisplayString (Temp);

   if (Com != NULL)
      Com->UnbufferBytes ();
   if (Snoop != NULL)
      Snoop->UnbufferBytes ();
}

VOID TEmbedded::BufferedPrintf (PSZ pszFormat, ...)
{
   va_list arglist;

   va_start (arglist, pszFormat);
   vsprintf (Temp, pszFormat, arglist);
   va_end (arglist);

   DisplayString (Temp);
}

VOID TEmbedded::PrintfAt (USHORT usRow, USHORT usColumn, PSZ pszFormat, ...)
{
   va_list arglist;
   CHAR Position[20];

   va_start (arglist, pszFormat);
   vsprintf (Temp, pszFormat, arglist);
   va_end (arglist);

   if (Avatar == TRUE)
      sprintf (Position, "\026\010%c%c", usRow, usColumn);
   else if (Ansi == TRUE)
      sprintf (Position, "\x1B[%d;%df", usRow, usColumn);
   if (Com != NULL)
      Com->BufferBytes ((UCHAR *)Position, (USHORT)strlen (Position));
   if (Snoop != NULL)
      Snoop->BufferBytes ((UCHAR *)Position, (USHORT)strlen (Position));

   DisplayString (Temp);

   if (Com != NULL)
      Com->UnbufferBytes ();
   if (Snoop != NULL)
      Snoop->UnbufferBytes ();
}

VOID TEmbedded::BufferedPrintfAt (USHORT usRow, USHORT usColumn, PSZ pszFormat, ...)
{
   va_list arglist;
   CHAR Position[20];

   va_start (arglist, pszFormat);
   vsprintf (Temp, pszFormat, arglist);
   va_end (arglist);

   if (Avatar == TRUE || Ansi == TRUE) {
      if (Avatar == TRUE)
         sprintf (Position, "\026\010%c%c", usRow, usColumn);
      else if (Ansi == TRUE)
         sprintf (Position, "\x1B[%d;%df", usRow, usColumn);
      if (Com != NULL)
         Com->BufferBytes ((UCHAR *)Position, (USHORT)strlen (Position));
      if (Snoop != NULL)
         Snoop->BufferBytes ((UCHAR *)Position, (USHORT)strlen (Position));
   }

   DisplayString (Temp);
}

VOID TEmbedded::ProcessControl (UCHAR ucControl)
{
   SHORT i, c;
   CHAR Temp[128], *p;

   switch (ucControl) {
      case CTRLA:             // ^A = Premi enter per continuare
         if (PeekNextChar () != CTRLA)
            GetString (Temp, 1, INP_HOTKEY);
         else {
            GetNextChar ();
            PressEnter ();
         }
         break;
      case CTRLD:             // ^D = Abilita la pausa a fine pagina
         More = TRUE;
         Line = 1;
         break;
      case CTRLE:             // ^E = Disabilita la pausa a fine pagina
         More = FALSE;
         Line = 1;
         break;
      case CTRLF:
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
               case CTRLA: {  // ^F^A = Show next quote
                  FILE *fp;
                  int fd;
                  CHAR Temp[128], *p;
                  ULONG Position = 0L;

                  sprintf (Temp, "%squotes.dat", Cfg->SystemPath);
                  if ((fd = sopen (Temp, O_RDONLY|O_BINARY, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                     read (fd, &Position, sizeof (Position));
                     close (fd);
                  }

                  if ((fp = OpenFile ("quotes", "rt")) != NULL) {
                     if (Position >= filelength (fileno (fp)))
                        Position = 0L;
                     fseek (fp, Position, SEEK_SET);
                     while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
                        if ((p = strchr (Temp, '\r')) != NULL)
                           *p = '\0';
                        if ((p = strchr (Temp, '\n')) != NULL)
                           *p = '\0';
                        if (Temp[0] == '\0')
                           break;
                        if (Temp[strlen (Temp) - 1] == '\n')
                           Temp[strlen (Temp) - 1] = '\0';
                        OutString ("%s\r\n", Temp);
                     }
                     Position = ftell (fp);
                     fclose (fp);
                  }

                  sprintf (Temp, "%squotes.dat", Cfg->SystemPath);
                  if ((fd = sopen (Temp, O_WRONLY|O_BINARY|O_CREAT, SH_DENYNO, S_IREAD|S_IWRITE)) != -1) {
                     write (fd, &Position, sizeof (Position));
                     close (fd);
                  }
                  break;
               }
               case 'A':      // ^FA
               case CTRLB:    // ^F^B = Nome e cognome dell'utente
                  if (User != NULL)
                     OutString ("%s", User->Name);
                  break;
               case 'B':      // ^FB = New files check
                  OutString ("%s", (User->NewFileCheck == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case CTRLC:    // ^F^C = City
                  if (User != NULL)
                     OutString ("%s", User->City);
                  break;
               case CTRLD: {  // ^F^D = Today's date (dd mmm yy)
                  time_t t;
                  struct tm *ltm;

                  t = time (NULL);
                  ltm = localtime (&t);
                  OutString ("%d %3.3s %d", ltm->tm_mday, Language->Months[ltm->tm_mon], ltm->tm_year + 1900);
                  break;
               }
               case 'P':      // ^FP
               case CTRLE:    // ^F^E = Numero di chiamate fatte dall'utente
                  if (User != NULL)
                     OutString ("%ld", User->TotalCalls);
                  break;
               case CTRLP: {  // ^F^P = Time off (hh:mm:ss)
                  time_t t;
                  struct tm *ltm;

                  t = time (NULL) + TimeRemain (TRUE);
                  ltm = localtime (&t);
                  OutString ("%2d:%02d:%02d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
                  break;
               }
               case CTRLT: {  // ^F^T = Current time (hh:mm:ss)
                  time_t t;
                  struct tm *ltm;

                  t = time (NULL);
                  ltm = localtime (&t);
                  OutString ("%2d:%02d:%02d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
                  break;
               }
               case 'W':      // ^FW
               case CTRLF: {  // ^F^F = Solo il nome dell'utente
                  PSZ Temp, p;

                  if (User != NULL) {
                     if ((Temp = (PSZ)malloc (sizeof (User->Name))) != NULL) {
                        strcpy (Temp, User->Name);
                        if ((p = strtok (Temp, " ")) != NULL)
                           OutString ("%s", p);
                     }
                  }
                  break;
               }
               case '?':      // ^F? = Video mode
                  if (Rip == TRUE)
                     OutString ("%s", "RIP   ");
                  else if (Avatar == TRUE)
                     OutString ("%s", "AVATAR");
                  else if (Ansi == TRUE)
                     OutString ("%s", "ANSI  ");
                  else
                     OutString ("%s", "TTY   ");
                  break;
               case CTRLG:    // ^F^G = Pausa di 1 secondo
                  if (Com != NULL)
                     Com->UnbufferBytes ();
                  if (Snoop != NULL)
                     Snoop->UnbufferBytes ();
                  Pause (100);
                  break;
               case CTRLK:    // ^F^K = Time of previous calls today
                  if (User != NULL)
                     OutString ("%lu", User->TodayTime);
                  break;
               case 'U':
               case CTRLL:    // Time online, this call
                  OutString ("%lu", (time (NULL) - StartCall) / 60L);
                  break;
               case CTRLO:    // ^F^O = Tempo rimasto
                  OutString ("%lu", TimeRemain ());
                  break;
               case CTRLQ: {  // ^F^Q = Total calls
                  class TStatistics *Stats;

                  if ((Stats = new TStatistics) != NULL) {
                     Stats->Read (Task);
                     OutString ("%lu", Stats->TotalCalls);
                     delete Stats;
                  }
                  break;
               }
               case CTRLU:    // ^F^U = Answers are required
                  Required = TRUE;
                  break;
               case CTRLV:    // ^F^V = Answers are not required
                  Required = FALSE;
                  break;
               case CTRLW:    // ^F^W = Upload KBytes
                  if (User != NULL)
                     OutString ("%lu", (User->UploadBytes + 1023L) / 1024L);
                  break;
               case CTRLX:    // ^F^X = Download KBytes
                  if (User != NULL)
                     OutString ("%lu", (User->DownloadBytes + 1023L) / 1024L);
                  break;
               case ':':    // ^F:  = Fullscreen enhancements
                  OutString ("%s", (User->FullScreen == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case CTRLY:    // ^F^Y = DL/UL Bytes Ratio
                  if (User != NULL) {
                     if (User->UploadBytes != 0)
                        OutString ("%lu:1", User->DownloadBytes / User->UploadBytes);
                     else
                        OutString ("%lu:0", (User->DownloadBytes + 1023L) / 1024L);
                  }
                  break;
               case '0':    // ^F0 = Full Screen Editor YES/NO
                  OutString ("%s", (User->FullEd == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case '2':    // ^F2 = User's age
                  if (User != NULL)
                     OutString ("%u", User->Age ());
                  break;
               case '3':    // ^F3 = Hotkey YES/NO
                  OutString ("%s", (HotKey == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case '5':    // ^F5 = Birthdate
                  OutString ("%d-%02d-%04d", User->BirthDay, User->BirthMonth, User->BirthYear);
                  break;
               case '6':    // ^F6 = Mailcheck YES/NO
                  OutString ("%s", (User->MailCheck == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case '8':    // ^F8 = Avatar YES/NO
                  OutString ("%s", (Avatar == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case '9':      // ^F9  = DL/UL Files Ratio
                  if (User != NULL) {
                     if (User->UploadFiles != 0)
                        OutString ("%lu:1", User->DownloadFiles / User->UploadFiles);
                     else
                        OutString ("%lu:0", User->DownloadFiles);
                  }
                  break;
               case '!':    // ^F! = Color YES/NO
                  OutString ("%s", (Color == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case '=':    // ^F= = Rip graphics YES/NO
                  OutString ("%s", (Rip == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case '$':    // ^F$ = In User list YES/NO
                  OutString ("%s", (User->InUserList == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case 'D':    // ^FD = E-Mail address
                  if (User != NULL)
                     OutString ("%s", User->InetAddress);
                  break;
               case 'E':    // ^FE = Phone number
                  if (User != NULL)
                     OutString ("%s", User->DayPhone);
                  break;
               case 'N':    // ^FN = Last message read
                  if (User != NULL && MsgArea != NULL)
                     OutString ("%lu", MsgArea->LastReaded);
                  break;
               case 'I':    // ^FI = IBM characters YES/NO
                  OutString ("%s", (User->IBMChars == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case 'K':    // ^F0 = Full Screen Editor YES/NO
                  OutString ("%s", (User->Kludges == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case 'Q':    // ^FR = Upload Files
                  if (User != NULL)
                     OutString ("%u", User->UploadFiles);
                  break;
               case 'R':    // ^FR = Upload Bytes
                  if (User != NULL)
                     OutString ("%lu", User->UploadBytes);
                  break;
               case 'S':    // ^FS = Download Files
                  if (User != NULL)
                     OutString ("%lu", User->DownloadFiles);
                  break;
               case 'T':    // ^FT = Download Bytes
                  if (User != NULL)
                     OutString ("%lu", User->DownloadBytes);
                  break;
               case 'V':    // ^FV = Screen Length
                  if (User != NULL)
                     OutString ("%u", User->ScreenHeight);
                  break;
               case 'X':    // ^FX = Ansi YES/NO
                  OutString ("%s", (Ansi == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case 'Y':    // ^FY = More? YES/NO
                  OutString ("%s", (User->MorePrompt == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case 'Z':    // ^FZ = Screen Clear YES/NO
                  OutString ("%s", (User->ScreenClear == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               case '\\':  // ^F\ = Language Name
                  if (Language != NULL)
                     OutString ("%s", Language->Comment);
                  break;
               case ';':    // ^F; = Full Screen Reader YES/NO
                  OutString ("%s", (User->FullReader == TRUE) ? Language->Text (LNG_YES) : Language->Text (LNG_NO));
                  break;
               default:
                  ProcessControlF ((UCHAR)c);
                  break;
            }
         }
         break;
      case CTRLK:
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
               case '1':    // ^K1 = Name of current message area
                  if (MsgArea != NULL)
                     OutString ("%s", MsgArea->Key);
                  break;
               case '2':    // ^K2 = Name of current file area
                  if (FileArea != NULL)
                     OutString ("%s", FileArea->Key);
                  break;
               case '7':    // ^K7 = Number of tagged files
                  if (User != NULL) {
                     if (User->FileTag != NULL)
                        OutString ("%u", User->FileTag->TotalFiles);
                  }
                  break;
               case '8':    // ^K8 = Number of files in current area
                  if (FileArea != NULL)
                     OutString ("%lu", FileArea->ActiveFiles);
                  break;
               case '9':    // ^K9 = Number of messages in current area
                  if (MsgArea != NULL)
                     OutString ("%lu", MsgArea->ActiveMsgs);
                  break;
               case 'A': {  // ^KA = Total calls
                  class TStatistics *Stats;

                  if ((Stats = new TStatistics) != NULL) {
                     Stats->Read (Task);
                     OutString ("%lu", Stats->TotalCalls);
                     delete Stats;
                  }
                  break;
               }
               case 'D':    // ^KD = First message in area
                  if (MsgArea != NULL)
                     OutString ("%lu", MsgArea->FirstMessage);
                  break;
               case 'E':    // ^KE = Last message in area
                  if (MsgArea != NULL)
                     OutString ("%lu", MsgArea->LastMessage);
                  break;
               case 'W':    // ^KW = Numero di linea
                  OutString ("%u", Task);
                  break;
               case 'Y':    // ^KY = Title of current message area
                  if (MsgArea != NULL)
                     OutString ("%s", MsgArea->Display);
                  break;
               case 'Z':    // ^KZ = Title of current file area
                  if (FileArea != NULL)
                     OutString ("%s", FileArea->Display);
                  break;
            }
         }
         break;
      case CTRLL:             // ^L = Cancella lo schermo
         if (Ansi == TRUE || Avatar == TRUE) {
            if (Ansi == TRUE && User->ScreenClear == FALSE)
               OutString ("\x1B[2J\x1B[1;1f");
            else
               OutString ("\x0C");
         }
         else
            OutString ("\n");
         Line = 1;
         break;
      case CTRLO:
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
               case 'C': {    // ^OC[cmd] = Run external program
                  CHAR File[64], *p;

                  p = File;
                  while ((c = GetNextChar ()) != EOF) {
                     if (c <= ' ')
                        break;
                     *p++ = (CHAR)c;
                  }
                  *p = '\0';

                  if (c == '\r' && PeekNextChar () == '\n')
                     GetNextChar ();

                  RunExternal (File);
                  break;
               }
               case 'E':      // ^OE = Display only if ANSI/Avatar enabled
                  if (Ansi == FALSE && Avatar == FALSE) {
                     while ((c = GetNextChar ()) != EOF) {
                        if (c == CTRLO) {
                           if ((c = GetNextChar ()) == 'e')
                              break;
                        }
                     }
                  }
                  break;
               case 'e':      // ^Oe = End of ANSI/Avatar only text
                  break;
               case 'F':      // ^OF = Set On Exit filename
                  p = OnExit;
                  while ((c = GetNextChar ()) != EOF) {
                     if (c <= ' ')
                        break;
                     *p++ = (CHAR)c;
                  }
                  *p = '\0';
                  if (c == '\r' && PeekNextChar () == '\n')
                     GetNextChar ();
                  break;
               case 'G':      // ^OG = Display only if RIPscript enabled
                  if (Rip == FALSE) {
                     while ((c = GetNextChar ()) != EOF) {
                        if (c == CTRLO) {
                           if ((c = GetNextChar ()) == 'I')
                              break;
                        }
                     }
                  }
                  break;
               case 'I':      // ^OI = End of Rip only text
                  break;
               case 'L': {    // ^OL[file] = Change language
                  CHAR File[64], *p;

                  p = File;
                  while ((c = GetNextChar ()) != EOF) {
                     if (c <= ' ')
                        break;
                     *p++ = (CHAR)c;
                  }
                  *p = '\0';

                  if (c == '\r' && PeekNextChar () == '\n')
                     GetNextChar ();

                  Language->Load (File);
                  strcpy (AltPath, Language->TextFiles);
                  break;
               }
               case 'M': {    // ^OM[comment] = Memorizza l'ultima risposta al ^OR
                  CHAR Comment[64], *p;

                  p = Comment;
                  do {
                     if ((c = GetNextChar ()) > ' ')
                        *p++ = (CHAR)c;
                  } while (c != EOF && c > ' ');
                  *p = '\0';
                  if (c == 0x0D && PeekNextChar () == 0x0A)
                     GetNextChar ();

                  if (AnswerFile != NULL)
                     fprintf (AnswerFile, "  %s: %c\n", Comment, Response);
                  break;
               }
               case 'N': {    // ^ON[comment] = Attende una stringa dall'utente e la memorizza
                  CHAR Comment[64], Answer[64], *p;

                  p = Comment;
                  do {
                     if ((c = GetNextChar ()) > ' ')
                        *p++ = (CHAR)c;
                  } while (c != EOF && c > ' ');
                  *p = '\0';
                  if (c == 0x0D && PeekNextChar () == 0x0A)
                     GetNextChar ();

                  do {
                     GetString (Answer, (USHORT)(sizeof (Answer) - 1), 0);
                  } while (AbortSession () == FALSE && Answer[0] == '\0' && Required == TRUE);
                  if (AnswerFile != NULL)
                     fprintf (AnswerFile, "  %s: %s\n", Comment, Answer);
                  break;
               }
               case 'O': {    // ^OO[file] = Open answer file
                  CHAR File[64], *p;

                  p = File;
                  while ((c = GetNextChar ()) != EOF) {
                     if (c <= ' ')
                        break;
                     *p++ = (CHAR)c;
                  }
                  *p = '\0';
                  if (c == '\r' && PeekNextChar () == '\n')
                     GetNextChar ();

                  if (AnswerFile != NULL)
                     fclose (AnswerFile);
                  if ((AnswerFile = fopen (File, "at")) != NULL) {
                     if (Log != NULL)
                        Log->Write ("+Answer file %s opened", File);
                  }
                  else {
                     if (Log != NULL)
                        Log->Write ("!Failed to open answer file %s", File);
                  }
                  break;
               }
               case 'P':      // ^OP = Write user's data to answer file
                  if (AnswerFile != NULL)
                     fprintf (AnswerFile, "* %s\t%s\t%s\t\n", User->Name, User->City, "");
                  break;
               case 'Q':      // ^OQ = Fine immediata del file
                  Stop = TRUE;
                  break;
               case 'R': {    // ^OR = Attende un comando dall'utente
                  USHORT Good = FALSE;
                  CHAR Valid[128], *p;

                  p = Valid;
                  do {
                     if ((c = GetNextChar ()) > ' ')
                        *p++ = (CHAR)c;
                  } while (c != EOF && c > ' ');
                  if (c == 0x0D && PeekNextChar () == 0x0A)
                     GetNextChar ();
                  *p = '\0';
                  strupr (Valid);

                  while (AbortSession () == FALSE && Good == FALSE) {
                     GetString (Temp, 1, INP_HOTKEY|INP_NOCRLF);
                     if ((Response = (CHAR)toupper (Temp[0])) == '\0')
                        Response = '|';
                     if (strchr (Valid, Response) != NULL)
                        Good = TRUE;
                     else if (Response != '|')
                        OutString ("\x08 \x08");
                  }

                  OutString ("\n");
                  break;
               }
               case 'S': {    // ^OS = Passa il controllo ad un'altro file
                  p = Temp;
                  do {
                     if ((c = GetNextChar ()) > ' ')
                        *p++ = (CHAR)c;
                  } while (c != EOF && c > ' ');
                  if (c == 0x0D && PeekNextChar () == 0x0A)
                     GetNextChar ();
                  *p = '\0';

                  if (Temp[0] != '\0') {
                     if (fp != NULL)
                        fclose (fp);
                     fp = OpenFile (Temp);
                     Line = 1;
                  }
                  break;
               }
               case 'T':      // ^OT = Salta all'inizio del file
                  if (fp != NULL)
                     fseek (fp, 0L, SEEK_SET);
                  break;
               case 'U':      // ^OU[char] = La riga prosegue se l'utente ha risposto [char]
                  if ((c = GetNextChar ()) != EOF) {
                     if (toupper (c) != toupper (Response))
                        do {
                           if ((c = GetNextChar ()) == 0x0D) {
                              if (PeekNextChar () == 0x0A)
                                 GetNextChar ();
                              else
                                 c = 0;
                           }
                        } while (c != EOF && c != 0x0D);
                  }
                  break;
               case 'V': {    // ^OV[pos] = Si posiziona a [pos] nel file
                  ULONG Pos = 0L;

                  while (isdigit (PeekNextChar ())) {
                     c = GetNextChar ();
                     Pos *= 10L;
                     Pos += c - 0x30;
                  }

                  if (PeekNextChar () == 0x0D) {
                     GetNextChar ();
                     if (PeekNextChar () == 0x0A)
                        GetNextChar ();
                  }

                  if (fp != NULL)
                     fseek (fp, Pos, SEEK_SET);
                  break;
               }
            }
         }
         break;
      case CTRLP:
         switch (PeekNextChar ()) {
            case 'L':
               GetNextChar ();
               do {
                  if ((c = GetNextChar ()) == 0x0D) {
                     if (PeekNextChar () == 0x0A)
                        GetNextChar ();
                     else
                        c = 0;
                  }
               } while (c != EOF && c != 0x0D);
               break;
            default:
               if (Com != NULL)
                  Com->BufferByte (ucControl);
               if (Snoop != NULL)
                  Snoop->BufferByte (ucControl);
               break;
         }
         break;
      case CTRLR:
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
               case CTRLC:    // ^R^C = System Name
                  if (Cfg != NULL)
                     OutString ("%s", Cfg->SystemName);
                  break;
               case CTRLD:    // ^R^D = Sysop Name
                  if (Cfg != NULL)
                     OutString ("%s", Cfg->SysopName);
                  break;
            }
         }
         break;
      case CTRLV:
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
               case CTRLA:    // ^V^A[^P][col] = Cambia il colore in [col]
                  if ((c = GetNextChar ()) != EOF) {
                     if (c == CTRLP) {
                        if ((c = GetNextChar ()) != EOF)
                           c &= 0x7F;
                     }
                     SetColor (c);
                  }
                  break;
               case CTRLG:    // ^V^G = Cancella fino alla fine della linea
                  ClrEol ();
                  break;
               case CTRLH:    // ^V^H[y][x] = Posiziona il cursore in [x],[y]
                  if ((c = GetNextChar ()) != EOF) {
                     if ((i = GetNextChar ()) != EOF) {
                        if (Avatar == TRUE) {
                           if (Com != NULL) {
                              Com->BufferByte (CTRLV);
                              Com->BufferByte (CTRLA);
                              Com->BufferByte ((UCHAR)c);
                              Com->BufferByte ((UCHAR)i);
                           }
                           if (Snoop != NULL) {
                              Snoop->BufferByte (CTRLV);
                              Snoop->BufferByte (CTRLA);
                              Snoop->BufferByte ((UCHAR)c);
                              Snoop->BufferByte ((UCHAR)i);
                           }
                        }
                        else if (Ansi == TRUE)
                           OutString ("\x1B[%d;%df", c, i);
                        Line = 1;
                     }
                  }
                  break;
               case CTRLY:    // ^V^Y[len][s][n] = Ripete la sequenza
                  c = GetNextChar ();
                  p = Temp;
                  for (i = 0; i < c; i++)
                     *p++ = (CHAR)GetNextChar ();
                  *p = '\0';
                  c = GetNextChar ();
                  for (i = 0; i < c; i++)
                     OutString ("%s", Temp);
                  break;
            }
         }
         break;
      case CTRLW:
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
               case 'b': {    // ^Wb[n] = If Task == [n]
                  CHAR Temp[64], *p;

                  p = Temp;
                  while ((c = GetNextChar ()) != EOF) {
                     if (c <= ' ')
                        break;
                     *p++ = (CHAR)c;
                  }
                  *p = '\0';

                  if (c == '\r' && PeekNextChar () == '\n')
                     GetNextChar ();

                  if (Task != atoi (Temp))
                     do {
                        if ((c = GetNextChar ()) == 0x0D) {
                           if (PeekNextChar () == 0x0A)
                              GetNextChar ();
                           else
                              c = 0;
                        }
                     } while (c != EOF && c != 0x0D);
                  break;
               }
               case CTRLA: {  // ^W^A = User's last call date (dd mmm yy)
                  struct tm *ltm;

                  if (User != NULL) {
                     ltm = localtime ((time_t *)&User->LastCall);
                     OutString ("%d %3.3s %d", ltm->tm_mday, Language->Months[ltm->tm_mon], ltm->tm_year + 1900);
                  }
                  break;
               }
               case CTRLC:    // ^W^C = System Name
                  if (Cfg != NULL)
                     OutString ("%s", Cfg->SystemName);
                  break;
               case CTRLD:    // ^W^D = Sysop Name
                  if (Cfg != NULL)
                     OutString ("%s", Cfg->SysopName);
                  break;
               case 'E':
                  StopNested = TRUE;
                  break;
               case CTRLF:
               case 'G':
                  switch (c = GetNextChar ()) {
                     case 'A':      // ^W^FA = Name of current file area
                        if (FileArea != NULL)
                           OutString ("%s", FileArea->Key);
                        break;
                     case 'N':      // ^W^FN = Description of current file area
                        if (FileArea != NULL)
                           OutString ("%s", FileArea->Display);
                        break;
                  }
                  break;
               case 'j':
                  switch (c = GetNextChar ()) {
                     case 'N':      // ^WjN = Node number
                        OutString ("%d", Task);
                        break;
                  }
                  break;
               case 'L': {    // ^WL = Link con un'altro file
                  FILE *SaveFP;
                  CHAR SaveResponse;

                  p = Temp;
                  do {
                     if ((c = GetNextChar ()) > ' ')
                        *p++ = (CHAR)c;
                  } while (c != EOF && c > ' ');
                  if (c == 0x0D && PeekNextChar () == 0x0A)
                     GetNextChar ();
                  *p = '\0';

                  if (Temp[0] != '\0') {
                     strlwr (Temp);

                     SaveFP = fp;
                     SaveResponse = Response;
                     if (strstr (Temp, ".cmd") == NULL) {
                        Nested++;
                        DisplayFile (Temp);
                        if (--Nested == 0) {
                           if (StopNested == TRUE)
                              Stop = TRUE;
                           StopNested = FALSE;
                        }
                     }
#if defined(__OS2__)
                     else {
                        fp = NULL;
                        CallRexx (this, Temp);
                     }
#endif

                     Response = SaveResponse;
                     fp = SaveFP;
                  }
                  break;
               }
               case CTRLM: {
                  switch (c = GetNextChar ()) {
                     case '#':      // ^W^M# = Number of messages in current area
                        if (MsgArea != NULL)
                           OutString ("%lu", MsgArea->ActiveMsgs);
                        break;
                     case 'A':      // ^W^MA = Name of current message area
                        if (MsgArea != NULL)
                           OutString ("%s", MsgArea->Key);
                        break;
                     case 'L':      // ^W^MH = Highest message in area
                        if (MsgArea != NULL)
                           OutString ("%lu", MsgArea->LastMessage);
                        break;
                     case 'N':      // ^W^MN = Description of current message area
                        if (MsgArea != NULL)
                           OutString ("%s", MsgArea->Display);
                        break;
                  }
                  break;
               }
               case 'P':
                  if (User != NULL)
                     OutString ("%s", User->DayPhone);
                  break;
               case 'R':
                  if (User != NULL)
                     OutString ("%s", User->RealName);
                  break;
               case 'W': {    // ^WW[comment] = Scrive la stringa nell'answer file
                  CHAR Comment[64], *p;

                  p = Comment;
                  do {
                     if ((c = GetNextChar ()) > ' ')
                        *p++ = (CHAR)c;
                  } while (c != EOF && c > ' ');
                  *p = '\0';
                  if (c == 0x0D && PeekNextChar () == 0x0A)
                     GetNextChar ();

                  if (AnswerFile != NULL)
                     fprintf (AnswerFile, "  %s\n", Comment);
                  break;
               }
               default:
                  ProcessControlW ((UCHAR)c);
                  break;
            }
         }
         break;
      case CTRLY:             // ^Y[char][rep] = Ripete il carattere [char] per [rep] volte
         if ((c = GetNextChar ()) != EOF) {
            if ((i = GetNextChar ()) != EOF) {
               while (i-- > 0) {
                  if (Com != NULL)
                     Com->BufferByte ((UCHAR)c);
                  if (Snoop != NULL)
                     Snoop->BufferByte ((UCHAR)c);
               }
            }
         }
         break;
      case CTRLG:
      case CTRLH:
      case '\n':
      case '\r':
      case ESC:
      case DEL:
         if (Com != NULL)
            Com->BufferByte (ucControl);
         if (Snoop != NULL)
            Snoop->BufferByte (ucControl);
         if (ucControl == '\n') {
            if ((Line = MoreQuestion (Line)) == 0)
               Stop = TRUE;
         }
         break;
      default:
         if (ucControl >= 128) {
            if (Com != NULL)
               Com->BufferByte (ucControl);
            if (Snoop != NULL)
               Snoop->BufferByte (ucControl);
         }
         break;
   }
}

VOID TEmbedded::ProcessControlF (UCHAR ucControl)
{
   ucControl = ucControl;
}

VOID TEmbedded::ProcessControlW (UCHAR ucControl)
{
   ucControl = ucControl;
}

VOID TEmbedded::RunExternal (PSZ Command)
{
   FILE *fp;
   USHORT DropFile;
   CHAR Cmd[128], Temp[64], *p, *a;
   struct tm *ltm;

   DropFile = NO_DROPFILE;

   a = Cmd;
   p = Command;
   do {
      if (*p == '%') {
         p++;
         switch (*p) {
            case '%':
               *a++ = '%';
               break;
            case 'b':      // Baud rate
               sprintf (a, "%lu", Cfg->Speed);
               a += strlen (a);
               break;
            case 'c':
               strcpy (a, User->City);
               a += strlen (a);
               break;
            case 'K':      // Task number (hex)
               sprintf (a, "%02x", Task);
               a += strlen (a);
               break;
            case 'k':      // Task number
               sprintf (a, "%u", Task);
               a += strlen (a);
               break;
            case 'L':      // Com port number and baud rate (Opus style)
               sprintf (a, "-p%d -b%u", atoi (&Cfg->Device[3]), Cfg->Speed);
               a += strlen (a);
               break;
            case 'p': {    // Com port handle
#if defined(__OS2__) || defined(__NT__)
               class TSerial *Serial = (class TSerial *)Com;

               sprintf (a, "%lu", Serial->hFile);
#else
               sprintf (a, "%d", atoi (&Cfg->Device[3]));
#endif
               a += strlen (a);
               break;
            }
            case 'P':      // Com port number
               sprintf (a, "%d", atoi (&Cfg->Device[3]));
               a += strlen (a);
               break;
            case 'T':      // Time left (seconds)
               sprintf (a, "%lu", TimeRemain (TRUE));
               a += strlen (a);
               break;
            case 't':      // Time left (minutes)
               sprintf (a, "%lu", TimeRemain ());
               a += strlen (a);
               break;
            case 'U':
               *a++ = '_';
               break;
            case 'W':      // Speed
               sprintf (a, "%lu", Cfg->Speed);
               a += strlen (a);
               break;
            case 'Z':      // User name (uppercase)
               strcpy (a, User->Name);
               a += strlen (strupr (a));
               break;
            default:
               *a++ = '%';
               *a++ = *p;
               break;
         }
      }
      else if (*p == '*') {
         p++;
         switch (toupper (*p)) {
            case '*':
               *a++ = '*';
               break;
            case 'B':      // Baud rate
               sprintf (a, "%lu", Cfg->Speed);
               a += strlen (a);
               break;
            case 'C':      // Location of the command processor
               if (getenv ("COMSPEC") != NULL) {
                  strcpy (a, getenv ("COMSPEC"));
                  a += strlen (a);
               }
               break;
            case 'D':      // Dropfile type
               p++;
               if (*p == 'D')
                  DropFile = DOOR_SYS;
               else if (*p == 'F')
                  DropFile = DOORX_SYS;
               else if (*p == 'I')
                  DropFile = DORINFO1_DEF;
               else if (*p == 'J')
                  DropFile = DORINFOX_DEF;
               break;
            case 'N':      // Task number
               sprintf (a, "%u", Task);
               a += strlen (a);
               break;
            case 'P':      // Com port number
               sprintf (a, "%d", atoi (&Cfg->Device[3]));
               a += strlen (a);
               break;
            case 'T':      // Time left
               sprintf (a, "%lu", TimeRemain ());
               a += strlen (a);
               break;
            default:
               *a++ = '*';
               *a++ = *p;
               break;
         }
      }
      else
         *a++ = *p;
   } while (*p++ != '\0');

   switch (DropFile) {
      case DOOR_SYS:
      case DOORX_SYS:
         if (DropFile == DOOR_SYS)
            sprintf (Temp, "%sdoor.sys", Cfg->SystemPath);
         else
            sprintf (Temp, "%sdoor%u.sys", Cfg->SystemPath, Task);

         Log->Write (":Creating %s", Temp);
         if ((fp = fopen (Temp, "wt")) != NULL) {
            fprintf (fp, "%s\n", Cfg->Device);
            fprintf (fp, "%lu\n", Cfg->Speed);
            fprintf (fp, "8\n");
            fprintf (fp, "%u\n", Task);
            fprintf (fp, "%lu\n", Cfg->Speed);
            fprintf (fp, "Y\n");
            fprintf (fp, "Y\n");
            fprintf (fp, "Y\n");
            fprintf (fp, "Y\n");
            fprintf (fp, "%s\n", User->Name);
            fprintf (fp, "%s\n", User->City);
            fprintf (fp, "%s\n", User->DayPhone);
            fprintf (fp, "\n");
            fprintf (fp, "\n");
            fprintf (fp, "%u\n", User->Level);
            fprintf (fp, "%lu\n", User->TotalCalls);
            ltm = localtime ((time_t *)&User->LastCall);
            fprintf (fp, "%02d/%02d/%04d\n", ltm->tm_mon + 1, ltm->tm_mday, ltm->tm_year);
            fprintf (fp, "%lu\n", TimeRemain (TRUE));
            fprintf (fp, "%lu\n", TimeRemain ());
            fprintf (fp, "%s\n", (Ansi == TRUE || Avatar == TRUE) ? "GR" : "NG");
            fprintf (fp, "%u\n", ScreenHeight);
            fprintf (fp, "Y\n");
            fprintf (fp, "\n");
            fprintf (fp, "\n");
            fprintf (fp, "01/01/99\n");
            fprintf (fp, "1\n");
            fprintf (fp, "N\n");
            fprintf (fp, "%d\n", User->UploadFiles);
            fprintf (fp, "%d\n", User->DownloadFiles);
            fprintf (fp, "%d\n", User->FilesToday);
            fprintf (fp, "%d\n", 2000);
            fclose (fp);
         }
         break;

      case DORINFO1_DEF:
      case DORINFOX_DEF:
         if (DropFile == DOOR_SYS)
            sprintf (Temp, "%sdorinfo1.def", Cfg->SystemPath);
         else
            sprintf (Temp, "%sdorinfo%u.def", Cfg->SystemPath, Task);

         Log->Write (":Creating %s", Temp);
         if ((fp = fopen (Temp, "wt")) != NULL) {
            fprintf (fp, "%s\n", Cfg->SystemName);
            strcpy (Temp, Cfg->SysopName);
            if ((p = strtok (Temp, " ")) != NULL) {
               fprintf (fp, "%s\n", p);
               if ((p = strtok (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  fprintf (fp, "%s\n", p);
               }
               else
                  fprintf (fp, "\n");
            }
            else
               fprintf (fp, "\n\n");
            fprintf (fp, "%s\n", Cfg->Device);
            fprintf (fp, "%u BAUD,N,8,1\n", Cfg->Speed);
            fprintf (fp, "%u\n", Task);
            strcpy (Temp, User->Name);
            if ((p = strtok (Temp, " ")) != NULL) {
               fprintf (fp, "%s\n", p);
               if ((p = strtok (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  fprintf (fp, "%s\n", p);
               }
               else
                  fprintf (fp, "\n");
            }
            else
               fprintf (fp, "\n\n");
            fprintf (fp, "%s\n", User->City);
            fprintf (fp, "%s\n", (Ansi == TRUE || Avatar == TRUE) ? "1" : "0");
            fprintf (fp, "%u\n", User->Level);
            fprintf (fp, "%lu\n", TimeRemain ());
            fprintf (fp, "-1\n");
            fclose (fp);
         }
         break;
   }

   DisplayFile ("leaving");
   if (Log != NULL) {
      Log->Write (":Running: %s", Cmd);
      Log->Suspend ();
   }

   ::RunExternal (Cmd);

   if (Log != NULL) {
      Log->Resume ();
      Log->Write (":Returned from door");
   }
   DisplayFile ("return");
}

VOID TEmbedded::SetColor (USHORT usColor)
{
   LastColor = usColor;

   if ((Ansi == TRUE || Avatar == TRUE) && Color == TRUE) {
      if (Com != NULL) {
         if (Avatar == TRUE) {
            Com->BufferByte (CTRLV);
            Com->BufferByte (CTRLA);
            Com->BufferByte ((UCHAR)usColor);
         }
         else if (Ansi == TRUE) {
            Com->BufferBytes ((UCHAR *)"\x1B[0", 3);

            if (usColor & BLINK)
               Com->BufferBytes ((UCHAR *)";5", 2);

            switch (usColor & 0x0F) {
               case BLACK:
                  Com->BufferBytes ((UCHAR *)";30", 3);
                  break;
               case BLUE:
                  Com->BufferBytes ((UCHAR *)";34", 3);
                  break;
               case GREEN:
                  Com->BufferBytes ((UCHAR *)";32", 3);
                  break;
               case CYAN:
                  Com->BufferBytes ((UCHAR *)";36", 3);
                  break;
               case RED:
                  Com->BufferBytes ((UCHAR *)";31", 3);
                  break;
               case MAGENTA:
                  Com->BufferBytes ((UCHAR *)";35", 3);
                  break;
               case BROWN:
                  Com->BufferBytes ((UCHAR *)";33", 3);
                  break;
               case LGREY:
                  Com->BufferBytes ((UCHAR *)";37", 3);
                  break;
               case DGREY:
                  Com->BufferBytes ((UCHAR *)";1;30", 5);
                  break;
               case LBLUE:
                  Com->BufferBytes ((UCHAR *)";1;34", 5);
                  break;
               case LGREEN:
                  Com->BufferBytes ((UCHAR *)";1;32", 5);
                  break;
               case LCYAN:
                  Com->BufferBytes ((UCHAR *)";1;36", 5);
                  break;
               case LRED:
                  Com->BufferBytes ((UCHAR *)";1;31", 5);
                  break;
               case LMAGENTA:
                  Com->BufferBytes ((UCHAR *)";1;35", 5);
                  break;
               case YELLOW:
                  Com->BufferBytes ((UCHAR *)";1;33", 5);
                  break;
               case WHITE:
                  Com->BufferBytes ((UCHAR *)";1;37", 5);
                  break;
            }

            switch (usColor & 0xF0) {
               case _BLACK:
                  Com->BufferBytes ((UCHAR *)";40", 3);
                  break;
               case _BLUE:
                  Com->BufferBytes ((UCHAR *)";44", 3);
                  break;
               case _GREEN:
                  Com->BufferBytes ((UCHAR *)";42", 3);
                  break;
               case _CYAN:
                  Com->BufferBytes ((UCHAR *)";46", 3);
                  break;
               case _RED:
                  Com->BufferBytes ((UCHAR *)";41", 3);
                  break;
               case _MAGENTA:
                  Com->BufferBytes ((UCHAR *)";45", 3);
                  break;
               case _BROWN:
                  Com->BufferBytes ((UCHAR *)";43", 3);
                  break;
               case _LGREY:
                  Com->BufferBytes ((UCHAR *)";47", 3);
                  break;
            }

            Com->BufferBytes ((UCHAR *)"m", 1);
         }
      }

      if (Snoop != NULL && (Ansi == TRUE || Avatar == TRUE)) {
         Snoop->BufferBytes ((UCHAR *)"\x1B[0", 3);

         if (usColor & BLINK)
            Snoop->BufferBytes ((UCHAR *)";5", 2);

         switch (usColor & 0x0F) {
            case BLACK:
               Snoop->BufferBytes ((UCHAR *)";30", 3);
               break;
            case BLUE:
               Snoop->BufferBytes ((UCHAR *)";34", 3);
               break;
            case GREEN:
               Snoop->BufferBytes ((UCHAR *)";32", 3);
               break;
            case CYAN:
               Snoop->BufferBytes ((UCHAR *)";36", 3);
               break;
            case RED:
               Snoop->BufferBytes ((UCHAR *)";31", 3);
               break;
            case MAGENTA:
               Snoop->BufferBytes ((UCHAR *)";35", 3);
               break;
            case BROWN:
               Snoop->BufferBytes ((UCHAR *)";33", 3);
               break;
            case LGREY:
               Snoop->BufferBytes ((UCHAR *)";37", 3);
               break;
            case DGREY:
               Snoop->BufferBytes ((UCHAR *)";1;30", 5);
               break;
            case LBLUE:
               Snoop->BufferBytes ((UCHAR *)";1;34", 5);
               break;
            case LGREEN:
               Snoop->BufferBytes ((UCHAR *)";1;32", 5);
               break;
            case LCYAN:
               Snoop->BufferBytes ((UCHAR *)";1;36", 5);
               break;
            case LRED:
               Snoop->BufferBytes ((UCHAR *)";1;31", 5);
               break;
            case LMAGENTA:
               Snoop->BufferBytes ((UCHAR *)";1;35", 5);
               break;
            case YELLOW:
               Snoop->BufferBytes ((UCHAR *)";1;33", 5);
               break;
            case WHITE:
               Snoop->BufferBytes ((UCHAR *)";1;37", 5);
               break;
         }

         switch (usColor & 0xF0) {
            case _BLACK:
               Snoop->BufferBytes ((UCHAR *)";40", 3);
               break;
            case _BLUE:
               Snoop->BufferBytes ((UCHAR *)";44", 3);
               break;
            case _GREEN:
               Snoop->BufferBytes ((UCHAR *)";42", 3);
               break;
            case _CYAN:
               Snoop->BufferBytes ((UCHAR *)";46", 3);
               break;
            case _RED:
               Snoop->BufferBytes ((UCHAR *)";41", 3);
               break;
            case _MAGENTA:
               Snoop->BufferBytes ((UCHAR *)";45", 3);
               break;
            case _BROWN:
               Snoop->BufferBytes ((UCHAR *)";43", 3);
               break;
            case _LGREY:
               Snoop->BufferBytes ((UCHAR *)";47", 3);
               break;
         }

         Snoop->BufferBytes ((UCHAR *)"m", 1);
      }
   }
}

VOID TEmbedded::Putch (UCHAR ucByte)
{
   if (Com != NULL)
      Com->SendByte (ucByte);
   if (Snoop != NULL)
      Snoop->SendByte (ucByte);
}

ULONG TEmbedded::TimeRemain (USHORT seconds)
{
   ULONG RetVal = 1440L * 60L, Len;

   if (TimeLimit != 0) {
      Len = time (NULL) - StartCall;
      RetVal = (TimeLimit * 60L) - Len;
   }

   if (seconds == FALSE)
      RetVal /= 60L;

   return (RetVal);
}

VOID TEmbedded::UnbufferBytes (VOID)
{
   if (Com != NULL)
      Com->UnbufferBytes ();
   if (Snoop != NULL)
      Snoop->UnbufferBytes ();
}

VOID TEmbedded::TranslateKeyword (VOID)
{
   USHORT Len;
   SHORT i, c, PutOn = FALSE, WasColor = FALSE;
   CHAR Key[64], *p, Color = 0;

   Traslate[0] = '\0';
   TrasPtr = Traslate;
   TrasLen = Len = 0;

   while ((c = GetNextChar ()) != EOF) {
      if (c > ' ')
         break;
   }

   for (;;) {
      p = Key;
      if (c != EOF) {
         *p++ = (CHAR)c;
         while ((c = GetNextChar ()) != EOF) {
            if (c == ']' || c <= ' ')
               break;
            *p++ = (CHAR)c;
         }
         *p++ = '\0';

         if (!stricmp (Key, "on"))
            PutOn = TRUE;
         else if (WasColor == TRUE && !stricmp (Key, "blink"))
            Traslate[Len - 1] |= 0x80;
         else if (isdigit (Key[0]))
            Traslate[Len++] = (CHAR)atoi (Key);
         else {
            for (i = 0; MeccaKeywords[i].Key != NULL; i++) {
               if (!stricmp (Key, MeccaKeywords[i].Key)) {
                  if (PutOn == FALSE) {
                     memcpy (&Traslate[Len], MeccaKeywords[i].Text, MeccaKeywords[i].Len);
                     Len += MeccaKeywords[i].Len;
                  }
                  WasColor = FALSE;
                  if (MeccaKeywords[i].Text[0] == '\026' && MeccaKeywords[i].Text[1] == '\001') {
                     WasColor = TRUE;
                     if (PutOn == TRUE) {
                        Color = (CHAR)(MeccaKeywords[i].Text[2] << 4);
                        Traslate[Len - 1] |= Color;
                        PutOn = FALSE;
                     }
                  }
                  break;
               }
            }
         }

         if (c != ']' && c != EOF) {
            while ((c = GetNextChar ()) != EOF && c != ']') {
               if (c > ' ')
                  break;
            }
         }
      }
      if (c == ']' || c == EOF)
         break;
   }

   TrasLen = Len;
}

