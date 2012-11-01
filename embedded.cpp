
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.05
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "bbs.h"
#include "menu.h"

TEmbedded::TEmbedded (class TBbs *pBbs)
{
   Bbs  = pBbs;
   Com  = pBbs->Com;
   Cfg  = pBbs->Cfg;
   Snoop = pBbs->Snoop;
   User = pBbs->User;

   fp = NULL;
   Position = NULL;
   StopNested = Stop = FALSE;
   Nested = 0;
   Response = '\0';
}

TEmbedded::~TEmbedded (void)
{
   if (fp != NULL)
      fclose (fp);
}

VOID TEmbedded::BasicControl (UCHAR ucControl)
{
   SHORT i, c;
   CHAR Temp[128], *p;

   switch (ucControl) {
      case CTRLA:
         if ((c = PeekNextChar ()) == CTRLA) {
            Bbs->Printf ("\n\026\001\015Press [Enter] to continue: ");
            GetNextChar ();
            Bbs->GetString (Temp, 0, INP_NOCRLF);
            Bbs->Printf ("\r\026\001\007\026\007\n");
         }
         else
         break;
      case CTRLF:
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
               case CTRLB:    // ^F^B = Nome e cognome dell'utente
                  OutString ("%s", User->Name);
                  break;
               case CTRLE:    // ^F^E = Numero di chiamate fatte dall'utente
                  OutString ("%ld", User->TotalCalls);
                  break;
               case CTRLF: {  // ^F^F = Solo il nome dell'utente
                  PSZ Temp, p;

                  if ((Temp = (PSZ)malloc (sizeof (User->Name))) != NULL) {
                     strcpy (Temp, User->Name);
                     if ((p = strtok (Temp, " ")) != NULL)
                        OutString ("%s", p);
                  }
                  break;
               }
               case CTRLL:    // ^F^L = Tempo trascorso in linea
                  OutString ("%lu", (time (NULL) - Bbs->StartCall + 59L) / 60L);
                  break;
               default:
                  ProcessControlF ((UCHAR)c);
                  break;
            }
         }
         break;
      case CTRLL:             // ^L = Cancella lo schermo
         if (Bbs->Ansi == TRUE)
            OutString ("\x1B[2J\x1B[1;1f");
         else {
            if (Com != NULL)
               Com->BufferByte (CTRLL);
            if (Snoop != NULL)
               Snoop->BufferByte (CTRLL);
         }
         break;
      case CTRLO:
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
               case 'E':      // ^OE = Salta la linea se non c'e' ANSI o AVATAR
                  if (Bbs->Ansi == FALSE && Bbs->Avatar == FALSE)
                     do {
                        if ((c = GetNextChar ()) == 0x0D) {
                           if (PeekNextChar () == 0x0A)
                              GetNextChar ();
                           else
                              c = 0;
                        }
                     } while (c != EOF && c != 0x0D);
                  break;
               case 'Q':      // ^OQ = Fine immediata del file
                  Stop = TRUE;
                  break;
               case 'R':      // ^OR[valid] = Attende una risposta dall'utente
                  p = Temp;
                  do {
                     if ((c = GetNextChar ()) > ' ')
                        *p++ = (CHAR)c;
                  } while (c != EOF && c > ' ');
                  if (c == 0x0D && PeekNextChar () == 0x0A)
                     GetNextChar ();
                  *p = '\0';

                  if (Temp[0] != '\0') {
                     CHAR Resp[2];

                     if (Com != NULL) {
                        Com->BufferByte (0x20);
                        Com->UnbufferBytes ();
                     }
                     if (Snoop != NULL) {
                        Snoop->BufferByte (0x20);
                        Snoop->UnbufferBytes ();
                     }
                     strupr (Temp);

                     do {
                        if (Com != NULL)
                           Com->SendBytes ((UCHAR *)"\x08 \x08", 3);
                        if (Snoop != NULL)
                           Snoop->SendBytes ((UCHAR *)"\x08 \x08", 3);
                        Bbs->GetString (Resp, 1, INP_HOTKEY|INP_NOCRLF);
                        if ((Response = Resp[0]) == '\0')
                           Response = '|';
                     } while (strchr (Temp, toupper (Response)) == NULL && Bbs->AbortSession () == FALSE);

                     if (Com != NULL)
                        Com->SendBytes ((UCHAR *)"\x0D\x0A", 2);
                     if (Snoop != NULL)
                        Snoop->SendBytes ((UCHAR *)"\x0D\x0A", 2);
                  }
                  break;
               case 'S':      // ^OS = Link con un'altro file
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
                     if ((fp = fopen (Temp, "rb")) == NULL) {
                        strcpy (p, ".Bbs");
                        fp = fopen (Temp, "rb");
                     }
                  }
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
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
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
                     Bbs->SetColor (c);
                  }
                  break;
               case CTRLG:    // ^V^G = Cancella fino alla fine della linea
                  Bbs->ClrEol ();
                  break;
               case CTRLH:    // ^V^H[y][x] = Posiziona il cursore in [x],[y]
                  if ((c = GetNextChar ()) != EOF) {
                     if ((i = GetNextChar ()) != EOF) {
                        if (Bbs->Ansi == TRUE)
                           OutString ("\x1B[%d;%df", c, i);
                        else if (Bbs->Avatar == TRUE) {
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
                     }
                  }
                  break;
            }
         }
         break;
      case CTRLW:
         if ((c = GetNextChar ()) != EOF) {
            switch (c) {
               case CTRLC:    // ^W^C = Nome del sistema
                  OutString ("%s", Cfg->SystemName);
                  break;
               case CTRLD:    // ^W^D = Nome del Sysop
                  OutString ("%s", Cfg->SysopName);
                  break;
               case 'E':
                  StopNested = TRUE;
                  break;
               case 'L': {    // ^WL = Link con un'altro file
                  FILE *SaveFP;

                  p = Temp;
                  do {
                     if ((c = GetNextChar ()) > ' ')
                        *p++ = (CHAR)c;
                  } while (c != EOF && c > ' ');
                  if (c == 0x0D && PeekNextChar () == 0x0A)
                     GetNextChar ();
                  *p = '\0';

                  if (Temp[0] != '\0') {
                     SaveFP = fp;
                     Nested++;
                     if (DisplayFile (Temp) == FALSE) {
                        strcpy (p, ".Bbs");
                        DisplayFile (Temp);
                     }
                     if (--Nested == 0) {
                        if (StopNested == TRUE)
                           Stop = TRUE;
                        StopNested = FALSE;
                     }
                     fp = SaveFP;
                  }
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
      default:
         ProcessControl (ucControl);
         break;
   }
}

USHORT TEmbedded::DisplayFile (PSZ pszFile)
{
   SHORT c;
   USHORT retVal = FALSE;

   LastChar = EOF;

   if ((fp = fopen (pszFile, "rb")) != NULL) {
      retVal = TRUE;
      while (Bbs->AbortSession () == FALSE && (c = GetNextChar ()) != EOF) {
         if (c < 32)
            BasicControl ((UCHAR)c);
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

   Stop = FALSE;
   if (Nested == 0)
      StopNested = FALSE;

   return (retVal);
}

USHORT TEmbedded::DisplayPrompt (PSZ pszString, UCHAR ucColor, UCHAR ucHilight)
{
   SHORT c, Hilight;
   USHORT retVal = FALSE;

   LastChar = EOF;
   Hilight = FALSE;

   if ((Position = pszString) != NULL) {
      Bbs->SetColor (ucColor);

      retVal = TRUE;
      while (Bbs->AbortSession () == FALSE && (c = GetNextChar ()) != EOF) {
         if (c < 32) {
            if (c == LF) {
               if (Com != NULL)
                  Com->BufferByte (CR);
               if (Snoop != NULL)
                  Snoop->BufferByte (CR);
            }
            BasicControl ((UCHAR)c);
         }
         else if (c == '^') {
            if (Hilight == FALSE) {
               Bbs->SetColor (ucHilight);
               Hilight = TRUE;
            }
            else {
               Bbs->SetColor (ucColor);
               Hilight = FALSE;
            }
            if (PeekNextChar () == '^') {
               c = GetNextChar ();
               if (Com != NULL)
                  Com->BufferByte ((UCHAR)c);
               if (Snoop != NULL)
                  Snoop->BufferByte ((UCHAR)c);
            }
         }
         else if (c == '%') {
            switch (PeekNextChar ()) {
               case 't':
                  GetNextChar ();
                  OutString ("%ld", Bbs->TimeLeft ());
                  break;
               default:
                  if (Com != NULL)
                     Com->BufferByte ((unsigned char)c);
                  if (Snoop != NULL)
                     Snoop->BufferByte ((unsigned char)c);
                  break;
            }
         }
         else {
            if (c == ';') {
               if (PeekNextChar () != EOF) {
                  if (Com != NULL)
                     Com->BufferByte ((unsigned char)c);
                  if (Snoop != NULL)
                     Snoop->BufferByte ((unsigned char)c);
               }
            }
            else {
               if (Com != NULL)
                  Com->BufferByte ((unsigned char)c);
               if (Snoop != NULL)
                  Snoop->BufferByte ((unsigned char)c);
            }
         }
      }
   }

   return (retVal);
}

USHORT TEmbedded::DisplayString (PSZ pszString)
{
   SHORT c;
   USHORT retVal = FALSE;

   LastChar = EOF;

   if ((Position = pszString) != NULL) {
      retVal = TRUE;
      while (Bbs->AbortSession () == FALSE && (c = GetNextChar ()) != EOF) {
         if (c < 32) {
            if (c == LF) {
               if (Com != NULL)
                  Com->BufferByte (CR);
               if (Snoop != NULL)
                  Snoop->BufferByte (CR);
            }
            BasicControl ((UCHAR)c);
         }
         else {
            if (Com != NULL)
               Com->BufferByte ((unsigned char)c);
            if (Snoop != NULL)
               Snoop->BufferByte ((unsigned char)c);
         }
      }
   }

   return (retVal);
}

SHORT TEmbedded::GetNextChar (VOID)
{
   SHORT retVal = EOF;

   if (Stop == FALSE && (Nested == 0 || StopNested == FALSE)) {
      if (fp != NULL) {
         if (LastChar == EOF) {
            if (fp != NULL)
               retVal = (SHORT)fgetc (fp);
         }
         else {
            retVal = LastChar;
            LastChar = EOF;
         }
      }
      else if (Position != NULL) {
         if (LastChar == EOF) {
            if (Position != NULL) {
               if ((retVal = Position[0]) == '\0')
                  retVal = EOF;
               else
                  Position++;
            }
         }
         else {
            retVal = LastChar;
            LastChar = EOF;
         }
      }
   }

   return (retVal);
}

VOID TEmbedded::OutString (PSZ pszFormat, ...)
{
   va_list arglist;

   va_start (arglist, pszFormat);
   vsprintf (Temp, pszFormat, arglist);
   va_end (arglist);

   if (Com != NULL)
      Com->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   if (Snoop != NULL)
      Snoop->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
}

SHORT TEmbedded::PeekNextChar (VOID)
{
   SHORT retVal = EOF;

   if (Stop == FALSE && (Nested == 0 || StopNested == FALSE)) {
      if (fp != NULL) {
         if (LastChar == EOF) {
            if (fp != NULL) {
               retVal = (SHORT)fgetc (fp);
               LastChar = retVal;
            }
         }
         else
            retVal = LastChar;
      }
      else if (Position != NULL) {
         if (LastChar == EOF) {
            if (Position != NULL) {
               if ((retVal = Position[0]) == '\0')
                  retVal = EOF;
               else
                  Position++;
               LastChar = retVal;
            }
         }
         else
            retVal = LastChar;
      }
   }

   return (retVal);
}

VOID TEmbedded::ProcessControl (UCHAR ucControl)
{
   switch (ucControl) {
      case CTRLG:
      case CTRLH:
      case LF:
      case CR:
      case ESC:
      case DEL:
         if (Com != NULL)
            Com->BufferByte (ucControl);
         if (Snoop != NULL)
            Snoop->BufferByte (ucControl);
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

// --------------------------------------------------------------------

VOID TBbs::Printf (PSZ pszFormat, ...)
{
   va_list arglist;
   PSZ Temp;
   class TEmbedded *PS;

   if ((Temp = (PSZ)malloc (1024)) != NULL) {
      va_start (arglist, pszFormat);
      vsprintf (Temp, pszFormat, arglist);
      va_end (arglist);

      if ((PS = new TEmbedded (this)) != NULL) {
         PS->DisplayString (Temp);
         if (Com != NULL)
            Com->UnbufferBytes ();
         if (Snoop != NULL)
            Snoop->UnbufferBytes ();
         delete PS;
      }

      free (Temp);
   }
}

VOID TBbs::PrintfAt (USHORT usRow, USHORT usColumn, PSZ pszFormat, ...)
{
   va_list arglist;
   CHAR Position[20];
   PSZ Temp;
   class TEmbedded *PS;

   if ((Temp = (PSZ)malloc (1024)) != NULL) {
      va_start (arglist, pszFormat);
      vsprintf (Temp, pszFormat, arglist);
      va_end (arglist);

      sprintf (Position, "\x1B[%d;%df", usRow, usColumn);
      if (Com != NULL)
         Com->BufferBytes ((UCHAR *)Position, (USHORT)strlen (Position));
      if (Snoop != NULL)
         Snoop->BufferBytes ((UCHAR *)Position, (USHORT)strlen (Position));

      if ((PS = new TEmbedded (this)) != NULL) {
         PS->DisplayString (Temp);
         if (Com != NULL)
            Com->UnbufferBytes ();
         if (Snoop != NULL)
            Snoop->UnbufferBytes ();
         delete PS;
      }

      free (Temp);
   }
}

USHORT TBbs::ReadFile (PSZ pszName, PSZ pszPath)
{
   CHAR FileName[128];
   USHORT retVal = FALSE;
   class TEmbedded *RF;

   if (pszPath == NULL)
      pszPath = Cfg->MiscPath;

   if ((RF = new TEmbedded (this)) != NULL) {
      sprintf (FileName, "%s%s", pszPath, pszName);
      if ((retVal = RF->DisplayFile (FileName)) == FALSE) {
         sprintf (FileName, "%s%s.BBS", pszPath, pszName);
         retVal = RF->DisplayFile (FileName);
         if (Com != NULL)
            Com->UnbufferBytes ();
         if (Snoop != NULL)
            Snoop->UnbufferBytes ();
      }
      delete RF;
   }

   return (retVal);
}



