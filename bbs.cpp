
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.16
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "bbs.h"
#include "version.h"

TBbs::TBbs (void)
{
   EndRun = Hangup = FALSE;
   Ansi = Avatar = FALSE;
   Color = FALSE;
   HotKey = FALSE;
   Rip = FALSE;
   TimeWarning = FALSE;
   More = TRUE;
   Limits = NULL;
   Snoop = NULL;
   Remote = TRUE;

   Cfg = NULL;
}

TBbs::~TBbs (void)
{
}

USHORT TBbs::AbortSession (VOID)
{
   USHORT RetVal = FALSE;

   if (EndRun == TRUE || Hangup == TRUE)
      RetVal = TRUE;
   if (Com != NULL && Com->Carrier () == FALSE)
      RetVal = TRUE;
   if (Snoop != NULL && Snoop->Carrier () == FALSE)
      RetVal = TRUE;

   return (RetVal);
}

VOID TBbs::ClrEol (VOID)
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

VOID TBbs::DisplayBanner (VOID)
{
#if defined(__DOS__)
   Printf ("\nLoraBBS Standard Ed. for DOS/32 v%s (2 Lines) - Unregistered\n", VERSION);
#elif defined(__OS2__)
   Printf ("\nLoraBBS Professional for OS/2 v%s (256 Lines) - Unregistered\n", VERSION);
#elif defined(__NT__)
   Printf ("\nLoraBBS Professional for Win/NT v%s (256 Lines) - Unregistered\n", VERSION);
#endif
   Printf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\n\n");
}

USHORT TBbs::GetAnswer (USHORT flQuestion)
{
   CHAR answer[10];

   if (flQuestion & ASK_HELP) {
      if (flQuestion & ASK_DEFYES)
         Printf (Lang->DefYesNoHelp);
      else if (flQuestion & ASK_DEFNO)
         Printf (Lang->YesDefNoHelp);
   }
   else {
      if (flQuestion & ASK_DEFYES)
         Printf (Lang->DefYesNo);
      else if (flQuestion & ASK_DEFNO)
         Printf (Lang->YesDefNo);
   }

   while (AbortSession () == FALSE) {
      GetString (answer, 1, INP_NOCRLF|INP_HOTKEY);

      if (answer[0] == '\0') {
         Printf ("\n");
         if (flQuestion & ASK_DEFYES)
            return (ANSWER_YES);
         else if (flQuestion & ASK_DEFNO)
            return (ANSWER_NO);
      }

      answer[0] = (CHAR)toupper (answer[0]);

      if (answer[0] == Lang->Yes) {
         Printf ("\n");
         return (ANSWER_YES);
      }
      if (answer[0] == Lang->No) {
         Printf ("\n");
         return (ANSWER_NO);
      }
      if (answer[0] == Lang->Help) {
         if (flQuestion & ASK_HELP) {
            Printf ("\n");
            return (ANSWER_YES);
         }
      }

      Printf ("\x08 \x08");
   }

   return (0);
}

SHORT TBbs::Getch (VOID)
{
   if (Com != NULL || Snoop != NULL) {
      while (AbortSession () == FALSE) {
         if (Com != NULL && Com->BytesReady () == TRUE)
            return (Com->ReadByte ());
         if (Snoop != NULL && Snoop->BytesReady () == TRUE)
            return (Snoop->ReadByte ());
      }
   }

   return (-1);
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

PSZ TBbs::GetString (CHAR *pszBuffer, USHORT usMaxlen, USHORT flAttrib)
{
   short i, c, len, dofancy;
   char *p;

   p = pszBuffer;
   len = 0;
   dofancy = TRUE;

   if ((flAttrib & INP_FIELD) && (Ansi == TRUE || Avatar == TRUE)) {
      for (i = 0; i < usMaxlen; i++) {
         Com->BufferByte (' ');
         if (Snoop != NULL)
            Snoop->BufferByte (' ');
      }
      for (i = 0; i < usMaxlen; i++) {
         Com->BufferByte (8);
         if (Snoop != NULL)
            Snoop->BufferByte (8);
      }
      Com->UnbufferBytes ();
      if (Snoop != NULL)
         Snoop->UnbufferBytes ();
   }

   while (AbortSession () == FALSE) {
      if (KBHit ()) {
         LastActivity = time (NULL);
         if ((c = Getch ()) == 0)
            c = (short)(Getch () << 8);

         if (c == 13)
            break;
         else if (c == 8 || c == 127) {
            if (len > 0) {
               Printf ("%c %c", 8, 8);
               p--;
               len--;
               if (flAttrib & INP_FANCY) {
                  if (len == 0)
                     dofancy = TRUE;
                  else if (len > 0) {
                     if (*(p - 1) == ' ' || *(p - 1) == '_' || *(p - 1) == '-' || *(p - 1) == '.')
                        dofancy = TRUE;
                  }
               }
            }
         }
         else if (c >= 32 && c < 127) {
            if (len < usMaxlen) {
               if (flAttrib & INP_FANCY) {
                  if (dofancy == TRUE) {
                     if (c == ' ' || c == '_' || c == '-' || c == '.')
                        continue;
                     c = (short)toupper (c);
                     dofancy = FALSE;
                  }
                  else {
                     if (c == ' ' || c == '_' || c == '-' || c == '.')
                        dofancy = TRUE;
                     else
                        c = (short)tolower (c);
                  }
               }
               *p++ = (char)c;
               len++;
               if (flAttrib & INP_PWD)
                  Putch ('*');
               else
                  Putch ((unsigned char)c);

               if (!(flAttrib & INP_NUMERIC) || !isdigit (c)) {
                  if (len == 1 && flAttrib & INP_HOTKEY) {
                     if (HotKey == TRUE)
                        break;
                  }
               }
            }
         }
      }
      else {
         if ((i = TimerExpired ()) != TE_NONE) {
            if (i == TE_CALL)
               Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for this call! Please feel free to call\nback again later...\n");
            else if (i == TE_DAY)
               Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for today! Please feel free to call back\nagain tomorrow...\n");
            else if (i == TE_WEEK)
               Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for this week! Please feel free to call\nback again next week...\n");
            else if (i == TE_MONTH)
               Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for this month! Please feel free to call\nback again next month...\n");
            else if (i == TE_YEAR)
               Printf ("\x16\x01\x0D\n\n\x07Sorry, you're all out of time for this year! Please feel free to call\nback again next year...\n");
            Hangup = TRUE;
         }
         else
            ReleaseTimeSlice ();
      }
   }

   if (!(flAttrib & INP_NOCOLOR))
      Printf ("\x16\x01\x07");
   if (!(flAttrib & INP_NOCRLF))
      Printf ("\n");

   *p = '\0';

   return (pszBuffer);
}

SHORT TBbs::KBHit (VOID)
{
   SHORT RetVal = FALSE;

   if (Com != NULL && Com->BytesReady () == TRUE)
      RetVal = TRUE;
   if (Snoop != NULL && Snoop->BytesReady () == TRUE)
      RetVal = TRUE;

   return (RetVal);
}

/*
   Descrizione:
      Ferma lo scroll del testo e chiede all'utente se vuole proseguire
      con la lettura.

   Parametri:
      nLine = Numero della linea di schermo corrente. Viene incrementata
              automaticamente da questa funzione.

   Valori di ritorno:
      >0 = Lo scroll prosegue normalmente.
       0 = L'utente ha scelto di non voler proseguire la visualizzazione.
      -1 = Lo scroll deve avvenire senza ulteriori interruzioni.
           Normalmente l'applicazione dovrebbe verificare solamente
           il valore di ritorno zero, gli altri sono gestiti automaticamente.
*/

SHORT TBbs::MoreQuestion (SHORT nLine)
{
   CHAR szTemp[2];

   if (nLine == -1 || nLine == 0 || More == FALSE)
      return (nLine);

   if (++nLine >= (ScreenHeight - 1)) {
      for (;;) {
         if (AbortSession () == TRUE)
            return (0);
         Printf (Lang->MoreQuestion);
         GetString (szTemp, 1, INP_NOCRLF|INP_NOCOLOR|INP_HOTKEY);
         if (toupper (szTemp[0]) == Lang->QuitKey) {
            Printf (Lang->DeleteMoreQuestion);
            return (0);
         }
         if (toupper (szTemp[0]) == Lang->ContinueKey || szTemp[0] == '\0') {
            Printf (Lang->DeleteMoreQuestion);
            return (1);
         }
         if (toupper (szTemp[0]) == Lang->NonStopKey) {
            Printf (Lang->DeleteMoreQuestion);
            return (-1);
         }
         Printf (Lang->PleaseAnswer3, Lang->NonStopKey, Lang->QuitKey, Lang->ContinueKey);
      }
   }

   return (nLine);
}

VOID TBbs::Putch (UCHAR ucByte)
{
   if (Com != NULL)
      Com->SendByte (ucByte);
   if (Snoop != NULL)
      Snoop->SendByte (ucByte);
}

VOID TBbs::SetColor (USHORT usColor)
{
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

LONG TBbs::TimeLeft (VOID)
{
   LONG Temp, RetVal = 1440;

   if (Limits != NULL) {
      Temp = (time (NULL) - StartCall) / 60L;
      if (Limits->CallTimeLimit != 0) {
         if ((Limits->CallTimeLimit - Temp) < RetVal)
            RetVal = Limits->CallTimeLimit - Temp;
      }
      if (Limits->DayTimeLimit != 0) {
         Temp += User->TodayTime;
         if ((Limits->DayTimeLimit - Temp) < RetVal)
            RetVal = Limits->DayTimeLimit - Temp;
         Temp -= User->TodayTime;
      }
      if (Limits->WeekTimeLimit != 0) {
         Temp += User->WeekTime;
         if ((Limits->WeekTimeLimit - Temp) < RetVal)
            RetVal = Limits->WeekTimeLimit - Temp;
         Temp -= User->WeekTime;
      }
      if (Limits->MonthTimeLimit != 0) {
         Temp += User->MonthTime;
         if ((Limits->MonthTimeLimit - Temp) < RetVal)
            RetVal = Limits->MonthTimeLimit - Temp;
         Temp -= User->MonthTime;
      }
      if (Limits->YearTimeLimit != 0) {
         Temp += User->YearTime;
         if ((Limits->YearTimeLimit - Temp) < RetVal)
            RetVal = Limits->YearTimeLimit - Temp;
         Temp -= User->YearTime;
      }

   }

   return (RetVal);
}

USHORT TBbs::TimerExpired (VOID)
{
   USHORT RetVal = TE_NONE;
   LONG Temp;

   if (Limits != NULL) {
      Temp = (time (NULL) - StartCall) / 60L;
      if (Limits->CallTimeLimit != 0 && Temp >= Limits->CallTimeLimit)
         RetVal = TE_CALL;
      else if (Limits->DayTimeLimit != 0 && (Temp + User->TodayTime) >= Limits->DayTimeLimit)
         RetVal = TE_DAY;
      else if (Limits->WeekTimeLimit != 0 && (Temp + User->WeekTime) >= Limits->WeekTimeLimit)
         RetVal = TE_WEEK;
      else if (Limits->MonthTimeLimit != 0 && (Temp + User->MonthTime) >= Limits->MonthTimeLimit)
         RetVal = TE_MONTH;
      else if (Limits->YearTimeLimit != 0 && (Temp + User->YearTime) >= Limits->YearTimeLimit)
         RetVal = TE_YEAR;
      else if (Limits->InactivityTime != 0) {
         Temp = time (NULL) - LastActivity;
         if (Temp >= Limits->InactivityTime)
            RetVal = TE_INACTIVITY;
      }
   }

   return (RetVal);
}


