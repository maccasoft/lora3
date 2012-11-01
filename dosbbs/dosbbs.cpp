
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.06
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/13/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_defs.h"
#include "bbs.h"
#include "combase.h"
#include "cxl.h"
#include "mailer.h"
#include "menu.h"
#include "version.h"

PSZ Months[] = {
   "January", "February", "March", "April", "May", "June",
   "July", "August", "September", "October", "November", "December"
};

PSZ WeekDays[] = {
   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

class TScreen : public TCom
{
public:
   TScreen (void);
   ~TScreen (void);

   SHORT  BytesReady (VOID);
   VOID   BufferByte (UCHAR byte);
   VOID   BufferBytes (UCHAR *bytes, USHORT len);
   SHORT  Carrier (VOID);
   VOID   ClearOutbound (VOID);
   VOID   ClearInbound (VOID);
   SHORT  Initialize (USHORT usRows);
   UCHAR  ReadByte (VOID);
   VOID   SendByte (UCHAR byte);
   VOID   SendBytes (UCHAR *bytes, USHORT len);
   VOID   UnbufferBytes (VOID);

private:
   WINDOW wh;
   USHORT Attr, Count, Params[10];
   CHAR   Prec, Ansi;
};

TScreen::TScreen (void)
{
   wh = -1;
   Ansi = FALSE;
   Prec = 0;
   Attr = BLACK|_LGREY;
}

TScreen::~TScreen (void)
{
   if (wh != -1) {
      wactiv (wh);
      wclose ();
   }
}

SHORT TScreen::BytesReady (VOID)
{
   SHORT RetVal = FALSE;

   if (kbhit ())
      RetVal = TRUE;

   return (RetVal);
}

VOID TScreen::BufferByte (UCHAR byte)
{
   USHORT i;

   if (byte == '[' && Prec == ESC) {
      Ansi = TRUE;
      Count = 0;
      Params[Count] = 0;
   }
   else {
      if (Ansi == TRUE) {
         if (isalpha (byte)) {
            if (byte == 'm') {
               for (i = 0; i <= Count; i++) {
                  if (Params[i] >= 30 && Params[i] <= 39)
                     Attr &= 0xF8;
                  else if (Params[i] >= 40 && Params[i] <= 49)
                     Attr &= 0x8F;

                  switch (Params[i]) {
                     case 0:
                        Attr = 0;
                        break;
                     case 1:
                        Attr |= 0x08;
                        break;
                     case 5:
                        Attr |= BLINK;
                        break;
                     case 30:
                        Attr |= BLACK;
                        break;
                     case 34:
                        Attr |= BLUE;
                        break;
                     case 32:
                        Attr |= GREEN;
                        break;
                     case 36:
                        Attr |= CYAN;
                        break;
                     case 31:
                        Attr |= RED;
                        break;
                     case 35:
                        Attr |= MAGENTA;
                        break;
                     case 39:
                        Attr |= BROWN;
                        break;
                     case 37:
                        Attr |= LGREY;
                        break;
                     case 33:
                        Attr |= YELLOW;
                        break;
                     case 40:
                        Attr |= _BLACK;
                        break;
                     case 44:
                        Attr |= _BLUE;
                        break;
                     case 42:
                        Attr |= _GREEN;
                        break;
                     case 46:
                        Attr |= _CYAN;
                        break;
                     case 41:
                        Attr |= _RED;
                        break;
                     case 45:
                        Attr |= _MAGENTA;
                        break;
                     case 49:
                        Attr |= _BROWN;
                        break;
                     case 47:
                        Attr |= _LGREY;
                        break;
                  }
               }
               wtextattr (Attr);
            }
            else if (byte == 'f')
               wgotoxy ((SHORT)(Params[0] - 1), (SHORT)(Params[1] - 1));
            else if (byte == 'J' && Params[0] == 2)
               wclear ();
            Ansi = FALSE;
         }
         else if (byte == ';') {
            Count++;
            Params[Count] = 0;
         }
         else if (isdigit (byte)) {
            Params[Count] *= 10;
            Params[Count] += (USHORT)(byte - '0');
         }
      }
      else if (byte == CTRLL)
         wclear ();
      else if (byte != ESC)
         wputc (byte);
   }

   Prec = byte;
}

VOID TScreen::BufferBytes (UCHAR *bytes, USHORT len)
{
   while (len-- > 0)
      BufferByte (*bytes++);
}

SHORT TScreen::Carrier (VOID)
{
   return (TRUE);
}

VOID TScreen::ClearOutbound (VOID)
{
}

VOID TScreen::ClearInbound (VOID)
{
}

SHORT TScreen::Initialize (USHORT usRows)
{
   SHORT RetVal = FALSE;

   if ((wh = wopen (0, 0, (short)(usRows - 1), 79, 5, LGREY|_BLACK, LGREY|_BLACK)) != -1)
      RetVal = TRUE;

   return (RetVal);
}

UCHAR TScreen::ReadByte (VOID)
{
   return ((UCHAR)getch ());
}

VOID TScreen::SendByte (UCHAR byte)
{
   BufferByte (byte);
}

VOID TScreen::SendBytes (UCHAR *bytes, USHORT len)
{
   while (len-- > 0)
      BufferByte (*bytes++);
}

VOID TScreen::UnbufferBytes (VOID)
{
}

// ----------------------------------------------------------------------

class TLocal : public TBbs
{
public:
   TLocal (void);
   ~TLocal (void);
   
   VOID   ReleaseTimeSlice (VOID);
   VOID   Run (VOID);
   VOID   SetStatus (PSZ pszAction);
   VOID   SetBrowsingStatus (PSZ pszMenu);

private:
   ULONG  Time;
   WINDOW whStatus;
   struct tm *tp;
};

TLocal::TLocal (void) : TBbs ()
{
   Log = 0;
   whStatus = 0;
}

TLocal::~TLocal (void)
{
}

VOID TLocal::ReleaseTimeSlice (VOID)
{
   if (whStatus != 0) {
      if (time (NULL) != Time) {
         Time = time (NULL);
         tp = localtime (&Time);
         printsf (24, 71, WHITE|_BLUE, "%02d:%02d:%02d", tp->tm_hour % 24, tp->tm_min % 60, tp->tm_sec % 60);
      }
   }
}

VOID TLocal::Run (VOID)
{
   USHORT DisplayLog;
   CHAR Temp[64];
   ULONG CallLen;
   class TScreen *Screen;

   Time = 0L;

   if (Log == 0) {
      if ((Log = new TLog) != 0) {
         sprintf (Temp, "%sCH%d.LOG", Cfg->LogPath, Cfg->Channel);
         if (Log->Open (Temp) == TRUE) {
            Log->Display = TRUE;
            Log->Level = Cfg->LogLevel;
            Log->Write (Log->Begin, VERSION, Cfg->Channel);
         }
      }
   }

   Log->Write ("+Connect Local");
   DisplayLog = Log->Display;
   Log->Display = FALSE;

   if ((Screen = new TScreen) != 0) {
      if ((whStatus = wopen (24, 0, 24, 79, 5, LGREY|_BLUE, LGREY|_BLUE)) != 0) {
         wprintc (0, 34, WHITE|_BLUE, '�');
         wprintc (0, 63, WHITE|_BLUE, '�');
         wprintc (0, 69, WHITE|_BLUE, '�');
      }
      if (Screen->Initialize (24) == TRUE) {
         Com = Screen;
         showcur ();

         Hangup = FALSE;
         Ansi = TRUE;
         Lang = new TLanguage;
         User = new TUser (Cfg->UserPath);
         Limits = new TLimits (Cfg->UserPath);

         SetStatus ("Channel has activity");
         LastActivity = StartCall = time (NULL);
         CarrierSpeed = 57600L;

         DisplayBanner ();

         if (LoginUser () == TRUE) {
            if (whStatus != 0)
               printsf (24, 1, WHITE|_BLUE, "%.32s", User->Name);

            if (VerifyAccount () == TRUE) {
               class TMenu *Menu = new TMenu (this);
               Menu->Run ("TOP");
               delete Menu;
            }

            CallLen = (time (NULL) - StartCall) / 60L;
            User->LastCall = time (NULL);
            User->TodayTime += CallLen;
            User->WeekTime += CallLen;
            User->MonthTime += CallLen;
            User->YearTime += CallLen;
            Log->Write (Log->UserOffline, User->Name, User->TotalCalls, CallLen);
            User->Update ();
         }

         delete Limits;
         Limits = 0;
         delete User;
         User = 0;
         delete Lang;
         Lang = 0;

         Pause (200);
      }

      delete Screen;

      if (whStatus != 0) {
         wactiv (whStatus);
         wclose ();
         whStatus = 0;
      }
   }

   Log->Write (Log->End);
   Log->Display = DisplayLog;
}

VOID TLocal::SetStatus (PSZ pszAction)
{
   pszAction = pszAction;
}

VOID TLocal::SetBrowsingStatus (PSZ pszMenu)
{
   pszMenu = pszMenu;
}

// ----------------------------------------------------------------------

class TModem : public TBbs
{
public:
   TModem (void);
   ~TModem (void);
   
   VOID   ReleaseTimeSlice (VOID);
   VOID   Run (VOID);
   VOID   SetStatus (PSZ pszAction);
   VOID   SetBrowsingStatus (PSZ pszMenu);

private:
   CHAR   Response[64];
   PSZ    Ptr;
   ULONG  Time;
   WINDOW whStatus;
   struct tm *tp;
   class  TSerial *Serial;
   class  TOutbound *Outbound;

   LONG   CheckResponse (VOID);
   VOID   HandleMenu (VOID);
   VOID   SendCommand (PSZ pszCmd);
};

#define NO_RESPONSE      0L
#define BUSY            -1L
#define ERROR           -2L
#define NO_ANSWER       -3L
#define NO_CARRIER      -4L
#define NO_DIALTONE     -5L
#define OK              -6L
#define RING            -7L
#define RINGING         -8L
#define VOICE           -9L

#define AT_FHNG         -10
#define AT_FCON         -11
#define AT_FPOLL        -12
#define AT_FVO          -13
#define AT_FDIS         -14
#define AT_FNSF         -15
#define AT_FCSI         -16
#define AT_FPTS         -17
#define AT_FDCS         -18
#define AT_FNSS         -19
#define AT_FTSI         -20
#define AT_FET          -21

TModem::TModem (void) : TBbs ()
{
   Ptr = Response;
   whStatus = 0;
   Time = 0L;
}

TModem::~TModem (void)
{
}

VOID TModem::ReleaseTimeSlice (VOID)
{
   if (whStatus != 0) {
      if (time (NULL) != Time) {
         Time = time (NULL);
         tp = localtime (&Time);
         printsf (24, 71, WHITE|_BLUE, "%02d:%02d:%02d", tp->tm_hour % 24, tp->tm_min % 60, tp->tm_sec % 60);
      }
   }
}

VOID TModem::Run (VOID)
{
   USHORT i, EndRun = FALSE, Key, Port, Retry;
   CHAR Temp[64], isBusy, isNoAnswer, Flags[10];
   ULONG Elapsed = 0L, Init_tout, Pause_tout, Answer_tout;
   LONG Response;
   WINDOW wh1;
   class TDetect *Detect;
   class TScreen *Screen;

   cclrscrn (LGREY|_BLACK);
   hidecur ();
   wh1 = wopen (1, 0, 24, 79, 0, LGREY|_BLACK, LGREY|_BLACK);
   Cfg->Address.First ();
   prints (0, 1, LGREEN|_BLACK, Cfg->Address.String);
   prints (0, (SHORT)(78 - strlen (STD_COMPLETE)), LGREEN|_BLACK, STD_COMPLETE);
   whline (10, 0, 78, 0, LGREY|_BLACK);
   whline (20, 0, 78, 0, LGREY|_BLACK);
   wvline (0, 51, 22, 0, LGREY|_BLACK);
   prints (1, 2, LCYAN|_BLACK, "LOG");
   prints (1, 54, LCYAN|_BLACK, "SYSTEM");
   prints (12, 2, LCYAN|_BLACK, "OUTBOUND");
   prints (12, 54, LCYAN|_BLACK, "MODEM");
   prints (23, 17, YELLOW|_BLACK, "Press ESC for menu");

   prints (5, 54, LCYAN|_BLACK, "   Status:");
   prints (6, 54, LCYAN|_BLACK, "  Elapsed:");
   prints (7, 54, LCYAN|_BLACK, "     Port:");

   if ((Log = new TLog) != 0) {
      sprintf (Temp, "%sCH%d.LOG", Cfg->LogPath, Cfg->Channel);
      if (Log->Open (Temp) == TRUE) {
         Log->Display = TRUE;
         Log->Level = Cfg->LogLevel;
         Log->Write (Log->Begin, VERSION, Cfg->Channel);
      }
   }

   Elapsed = time (NULL);
   Time = time (NULL);
   tp = localtime (&Time);
   sprintf (Temp, "%s, %s %d %d", WeekDays[tp->tm_wday], Months[tp->tm_mon], tp->tm_mday, tp->tm_year + 1900);
   prints (2, (SHORT)(54 + ((25 - strlen (Temp)) / 2)), YELLOW|_BLACK, Temp);
   sprintf (Temp, "%02d:%02d:%02d", tp->tm_hour % 24, tp->tm_min % 60, tp->tm_sec % 60);
   prints (3, (SHORT)(54 + ((25 - strlen (Temp)) / 2)), YELLOW|_BLACK, Temp);

   sprintf (Temp, "%02ld:%02ld", (Time - Elapsed) / 60L, (Time - Elapsed) % 60L);
   prints (6, 65, YELLOW|_BLACK, Temp);

   prints (5, 65, YELLOW|_BLACK, "Initializing");

   if ((Outbound = new TOutbound (Cfg->Outbound)) != 0) {
      if (Cfg->Address.First () == TRUE)
         Outbound->DefaultZone = Cfg->Address.Zone;
      else
         Outbound->DefaultZone = 2;
      Outbound->BuildQueue (Cfg->Outbound);
      if (Outbound->FirstNode () == TRUE) {
         prints (13, 2, YELLOW|_BLACK, "Node              Try/Con  Type   Size    Status");
         i = 14;
         do {
            Flags[0] = (Outbound->Crash == TRUE) ? 'C' : ' ';
            Flags[1] = (Outbound->Direct == TRUE) ? 'D' : ' ';
            Flags[2] = (Outbound->Hold == TRUE) ? 'H' : ' ';
            Flags[3] = (Outbound->Immediate == TRUE) ? 'I' : ' ';
            Flags[4] = (Outbound->Normal == TRUE) ? 'N' : ' ';
            Flags[5] = '\0';
            sprintf (Temp, "%-17.17s %3d %-3d  %s %5lu    ------", Outbound->Address, Outbound->Attempts, Outbound->Failed, Flags, Outbound->Size);
            prints (i++, 2, LCYAN|_BLACK, Temp);
         } while (Outbound->NextNode () == TRUE);
      }
      else
         prints (17, 13, LCYAN|_BLACK, "Nothing in outbound area");
   }

   Init_tout = 0L;
   Pause_tout = 0L;
   Answer_tout = 0L;

   isBusy = FALSE;
   isNoAnswer = FALSE;

   if ((Serial = new TSerial) != 0) {
      Com = Serial;

      if (!strnicmp (Cfg->Device, "COM", 3))
         Port = (USHORT)atoi (&Cfg->Device[3]);
      else
         Port = (USHORT)atoi (Cfg->Device);

      sprintf (Temp, "%-6lu Com%d", Cfg->Speed, Port);
      prints (7, 65, YELLOW|_BLACK, Temp);

      if (Serial->Initialize (Port, Cfg->Speed, 8, 'N', 1) == TRUE) {
         Serial->SetDTR (TRUE);
         SendCommand (Cfg->Initialize[0]);
         Init_tout = TimerSet (1500);
         Retry = 0;
      }
   }

   while (EndRun == FALSE) {
      if ((Response = CheckResponse ()) == OK) {
         prints (5, 65, YELLOW|_BLACK, "Idle        ");
         Init_tout = 0L;
         Pause_tout = TimerSet (60000L);
         Retry = 0;
      }

      if (Init_tout != 0L) {
         if (TimeUp (Init_tout)) {
            Log->Write ("!Modem doesn't report OK");
            prints (5, 65, YELLOW|_BLACK, "Idle        ");
            Pause_tout = TimerSet (200);
            Init_tout = 0L;
            if (++Retry >= 3)
               EndRun = TRUE;
         }
      }
      else if (Pause_tout != 0L) {
         if (TimeUp (Pause_tout)) {
            prints (5, 65, YELLOW|_BLACK, "Initializing");
            SendCommand (Cfg->Initialize[0]);
            Init_tout = TimerSet (1500);
            Pause_tout = 0L;
         }
      }
      else if (Answer_tout) {
         if (TimeUp (Answer_tout)) {
            prints (5, 65, YELLOW|_BLACK, "Initializing");
            SendCommand (Cfg->Initialize[0]);
            Init_tout = TimerSet (500);
         }
      }

      if (Response < 0) {
         switch (Response) {
            case RING:
               if (isNoAnswer == FALSE) {
                  prints (5, 65, YELLOW|_BLACK, "Answering   ");
                  SendCommand (Cfg->Answer);
                  Answer_tout = TimerSet (4500);
               }
               break;

            case BUSY:
            case ERROR:
            case NO_ANSWER:
            case NO_CARRIER:
            case NO_DIALTONE:
            case VOICE:
               prints (5, 65, YELLOW|_BLACK, "Initializing");
               SendCommand (Cfg->Initialize[0]);
               Init_tout = TimerSet (500);
               Answer_tout = 0L;
               break;
         }
      }
      else if (Response > 0) {
         Answer_tout = 0;
         CarrierSpeed = Response;
         Pause (100);

         Lang = new TLanguage;
         User = new TUser (Cfg->UserPath);
         Limits = new TLimits (Cfg->UserPath);

         StartCall = time (NULL);

         Ansi = Avatar = Color = HotKey = Rip = FALSE;
         DisplayBanner ();

         if ((Detect = new TDetect (this)) != 0) {
            Detect->DetectTerminal ();
            delete Detect;
         }

         if (AbortSession () == FALSE) {
            if ((Screen = new TScreen) != 0) {
               if ((whStatus = wopen (24, 0, 24, 79, 5, LGREY|_BLUE, LGREY|_BLUE)) != 0) {
                  wprintc (0, 34, WHITE|_BLUE, '�');
                  wprintc (0, 63, WHITE|_BLUE, '�');
                  wprintc (0, 69, WHITE|_BLUE, '�');
               }
               if (Screen->Initialize (24) == TRUE) {
                  Snoop = Screen;
                  showcur ();
                  Time = 0L;
               }
               Log->Display = FALSE;
            }

            if (LoginUser () == TRUE) {
               if (whStatus != 0)
                  printsf (24, 1, WHITE|_BLUE, "%.32s", User->Name);

               if (VerifyAccount () == TRUE) {
                  class TMenu *Menu = new TMenu (this);
                  Menu->Run ("TOP");
                  delete Menu;
               }

               User->LastCall = time (NULL);
               User->Update ();
            }
         }

         delete Limits;
         Limits = 0;
         delete User;
         User = 0;
         delete Lang;
         Lang = 0;

         Pause (200);

         if (Snoop != 0) {
            hidecur ();
            delete Snoop;
            Snoop = 0;
            if (whStatus != 0) {
               wactiv (whStatus);
               wclose ();
               whStatus = 0;
            }
            Log->Display = TRUE;
         }

         prints (5, 65, YELLOW|_BLACK, "Hangup      ");
         SendCommand (Cfg->Hangup);

         prints (5, 65, YELLOW|_BLACK, "Initializing");
         SendCommand (Cfg->Initialize[0]);
         Init_tout = TimerSet (500);
      }

      if (kbhit ()) {
         if ((Key = (USHORT)getch ()) == 0)
            Key = (USHORT)(getch () << 8);
         switch (Key) {
// ----------------------------------------------------------------------
// Alt-X = Exit to DOS
// ----------------------------------------------------------------------
            case 0x2D00:
               EndRun = TRUE;
               break;

// ----------------------------------------------------------------------
// Alt-K = Login in local mode
// ----------------------------------------------------------------------
            case 0x2500: {
               class TLocal *Local;

               if ((Local = new TLocal) != 0) {
                  Local->Cfg = Cfg;
                  Local->Log = Log;
                  Local->Run ();
                  delete Local;

                  Log->WriteBlank ();
                  wfill (0, 0, 9, 49, ' ', LGREY|_BLACK);
                  Log->Write (Log->Begin, VERSION, Cfg->Channel);

                  prints (5, 65, YELLOW|_BLACK, "Initializing");
                  SendCommand (Cfg->Initialize[0]);
                  Init_tout = TimerSet (1500);
                  Pause_tout = 0L;
               }

               hidecur ();
               break;
            }

            case ESC:
               HandleMenu ();
               Elapsed = time (NULL);
               if (Init_tout != 0L)
                  prints (5, 65, YELLOW|_BLACK, "Initializing");
               else
                  prints (5, 65, YELLOW|_BLACK, "Idle        ");
               break;
         }
      }

      if (time (NULL) != Time) {
         Time = time (NULL);
         tp = localtime (&Time);
         sprintf (Temp, "%s, %s %d %d", WeekDays[tp->tm_wday], Months[tp->tm_mon], tp->tm_mday, tp->tm_year + 1900);
         prints (2, (SHORT)(54 + ((25 - strlen (Temp)) / 2)), YELLOW|_BLACK, Temp);
         sprintf (Temp, "%02d:%02d:%02d", tp->tm_hour % 24, tp->tm_min % 60, tp->tm_sec % 60);
         prints (3, (SHORT)(54 + ((25 - strlen (Temp)) / 2)), YELLOW|_BLACK, Temp);

         sprintf (Temp, "%02ld:%02ld", (Time - Elapsed) / 60L, (Time - Elapsed) % 60L);
         prints (6, 65, YELLOW|_BLACK, Temp);
      }
   }

   if (Log != 0) {
      Log->Write (Log->End);
      delete Log;
   }

   if (Outbound != 0)
      delete Outbound;
   if (Serial != 0)
      delete Serial;

   wcloseall ();
   showcur ();
}

VOID TModem::SetStatus (PSZ pszAction)
{
   pszAction = pszAction;
}

VOID TModem::SetBrowsingStatus (PSZ pszMenu)
{
   pszMenu = pszMenu;
}

VOID TModem::SendCommand (PSZ pszCmd)
{
   while (*pszCmd) {
      switch (*pszCmd) {
         case '|':
            Serial->BufferByte ((char)13);
            Pause (10);
            break;

         case 'v':
            Serial->SetDTR (FALSE);
            Pause (10);
            break;

         case '^':
            Serial->SetDTR (TRUE);
            Pause (10);
            break;

         case '~':
            Pause (50);
            break;

         case '`':
            Pause (10);
            break;

         default:
            Serial->BufferByte (*pszCmd);
            break;
      }
      pszCmd++;
   }

   Serial->BufferByte ((char)13);
   Pause (10);
}

LONG TModem::CheckResponse (VOID)
{
   USHORT DoCheck = FALSE;
   CHAR c, Temp[27];
   LONG RetVal = NO_RESPONSE;

   if (Com->BytesReady () == TRUE) {
      c = (char)Com->ReadByte ();
      if (c == '\r') {
         *Ptr = '\0';
         Ptr = Response;
         DoCheck = TRUE;

         if (Response[0] != '\0') {
            sprintf (Temp, "%.*s", sizeof (Temp) - 1, Response);
            wscrollbox (11, 52, 19, 77, 1, D_UP);
            prints (21, (SHORT)(53 + ((26 - strlen (Temp)) / 2)), CYAN|_BLACK, Temp);
         }
      }
      else if (c != '\n')
         *Ptr++ = c;
   }

   if (DoCheck == TRUE) {
      if (Response[0] == '+' && Response[1] == 'F') {
         if (!strncmp (Response, "+FHS:", 5))
            RetVal = AT_FHNG;
         if (!strncmp (Response, "+FCO:", 5))
            RetVal = AT_FCON;
         if (!strncmp (Response, "+FPO", 4))
            RetVal = AT_FPOLL;
         if (!strncmp (Response, "+FVO", 4))
            RetVal = AT_FVO;
         if (!strncmp (Response, "+FIS:", 5))
            RetVal = AT_FDIS;
         if (!strncmp (Response, "+FNF:", 5))
            RetVal = AT_FNSF;
         if (!strncmp (Response, "+FCI:", 5))
            RetVal = AT_FCSI;
         if (!strncmp (Response, "+FPS:", 5))
            RetVal = AT_FPTS;
         if (!strncmp (Response, "+FCS:", 5))
            RetVal = AT_FDCS;
         if (!strncmp (Response, "+FNS:", 5))
            RetVal = AT_FNSS;
         if (!strncmp (Response, "+FTI:", 5))
            RetVal = AT_FTSI;
         if (!strncmp (Response, "+FET:", 5))
            RetVal = AT_FET;
      }
      else {
         if (!stricmp (Response, "OK"))
            RetVal = OK;
         if (!stricmp (Response, "BUSY"))
            RetVal = BUSY;
         if (!stricmp (Response, "ERROR"))
            RetVal = ERROR;
         if (!stricmp (Response, "NO ANSWER"))
            RetVal = NO_ANSWER;
         if (!stricmp (Response, "NO CARRIER"))
            RetVal = NO_CARRIER;
         if (!stricmp (Response, "NO DIALTONE"))
            RetVal = NO_DIALTONE;
         if (!stricmp (Response, "RING"))
            RetVal = RING;
         if (!stricmp (Response, "RINGING"))
            RetVal = RINGING;
         if (!stricmp (Response, "VOICE"))
            RetVal = VOICE;

         if (!strnicmp (Response, "CONNECT", 7) || !strnicmp (Response, "CARRIER", 7)) {
            if (Response[7] == ' ')
               RetVal = atol (&Response[8]);
            else
               RetVal = 300L;
         }
      }
   }

   if (RetVal != NO_RESPONSE && RetVal != OK)
      Log->Write (":%s", Response);

   return (RetVal);
}

PSZ MenuEdit[] = {
   " File areas    ",
   " Message areas ",
   " Forums        ",
   " Users         ",
   " Limit classes "
};

PSZ MenuMailer[] = {
   " Rescan Outbound      ",
   "컴컴컴컴컴컴컴컴컴컴컴",
   " Initialize Modem     ",
   " Answer Call          ",
   " Toggle DTR           ",
   "컴컴컴컴컴컴컴컴컴컴컴",
   " Clear Calls          ",
   "컴컴컴컴컴컴컴컴컴컴컴",
   " Inactive No-Answer   ",
   " Inactive Busy        ",
   "컴컴컴컴컴컴컴컴컴컴컴",
   " Controls           > "
};

PSZ MenuControls[] = {
   "   No BBS         ",
   "   No Incoming    ",
   "   No Outgoing    ",
   "컴컴컴컴컴컴컴컴컴",
   "   Send Freqs.    ",
   "   Receive Freqs. ",
   "컴컴컴컴컴컴컴컴컴",
   "   FTS-0001 only  ",
   "   No EMSI        ",
   "   No Janus       "
};

PSZ MenuLog[] = {
   " View log ",
   "컴컴컴컴컴",
   " Kill log "
};

PSZ MenuConfig[] = {
   " General Settings   ",
   " Mailer Settings    ",
   " Events             ",
   " Mailer Addresses   ",
   " Logon Strings      ",
   " Dial Strings       ",
   " BBS              > ",
   "컴컴컴컴컴컴컴컴컴컴",
   " Register LoraBBS   "
};

PSZ MenuBBS[] = {
   " General Settings    ",
   " Misc. Paths / Files ",
   " Languages           ",
   " BBS Action Flags    ",
   " New User Defaults   ",
   " Internet Options    "
};

PSZ MenuHelp[] = {
   " Help index...          ",
   " General help...        ",
   " Using help...          ",
   " Keys help...           ",
   "컴컴컴컴컴컴컴컴컴컴컴컴",
   " Product information... "
};

VOID TModem::HandleMenu (VOID)
{
   USHORT i, EndRun = FALSE, Key, Items, CurrentItem, Position, Y;
   CHAR Temp[40], **CurrentMenu;
   ULONG Elapsed;
   WINDOW wh1, wh2, wh3;

   Time = 0L;
   CurrentMenu = MenuEdit;
   Items = 4;
   Position = 1;
   Y = 1;
   wh3 = 0;

   Elapsed = time (NULL);
   prints (5, 65, YELLOW|_BLACK, "Interactive ");

   if ((wh1 = wopen (0, 0, 0, 79, 5, BLACK|_LGREY, BLACK|_LGREY)) != 0) {
      wprints (0, 0, BLUE|_LGREY, "  Edit  Mailer  Log  Config  Help                                               ");
      wprintc (0, 2, WHITE|_LGREY, 'E');
      wprintc (0, 8, WHITE|_LGREY, 'M');
      wprintc (0, 16, WHITE|_LGREY, 'L');
      wprintc (0, 21, WHITE|_LGREY, 'C');
      wprintc (0, 29, WHITE|_LGREY, 'H');

      wprints (0, 1, WHITE|_BLACK, " Edit ");
      if ((wh2 = wopen (Y, Position, (SHORT)(1 + Items + 2), (SHORT)(Position + strlen (CurrentMenu[0]) + 1), 0, BLUE|_LGREY, BLUE|_LGREY)) != 0) {
         for (i = 0; i <= Items; i++)
            wprints (i, 0, BLUE|_LGREY, CurrentMenu[i]);
         CurrentItem = 0;
         wprints (CurrentItem, 0, WHITE|_BLACK, CurrentMenu[CurrentItem]);
      }

      while (EndRun == FALSE) {
         if (kbhit ()) {
            if ((Key = (USHORT)getch ()) == 0)
               Key = (USHORT)(getch () << 8);
            switch (Key) {
               case CR:
                  if (CurrentMenu == MenuConfig && CurrentItem == 6) {
                     Items = 5;
                     CurrentMenu = MenuBBS;
                     if ((wh3 = wopen (6, 40, (SHORT)(6 + Items + 2), (SHORT)(40 + strlen (CurrentMenu[0]) + 1), 0, BLUE|_LGREY, BLUE|_LGREY)) != 0) {
                        for (i = 0; i <= Items; i++)
                           wprints (i, 0, BLUE|_LGREY, CurrentMenu[i]);
                        CurrentItem = 0;
                        wprints (CurrentItem, 0, WHITE|_BLACK, CurrentMenu[CurrentItem]);
                     }
                  }
                  else if (CurrentMenu == MenuMailer && CurrentItem == 11) {
                     Items = 9;
                     CurrentMenu = MenuControls;
                     if ((wh3 = wopen (6, 29, (SHORT)(6 + Items + 2), (SHORT)(29 + strlen (CurrentMenu[0]) + 1), 0, BLUE|_LGREY, BLUE|_LGREY)) != 0) {
                        for (i = 0; i <= Items; i++)
                           wprints (i, 0, BLUE|_LGREY, CurrentMenu[i]);
                        CurrentItem = 0;
                        wprints (CurrentItem, 0, WHITE|_BLACK, CurrentMenu[CurrentItem]);
                     }
                  }
                  break;

               case 0x4800:
                  if (CurrentItem > 0) {
                     wprints (CurrentItem, 0, BLUE|_LGREY, CurrentMenu[CurrentItem]);
                     CurrentItem--;
                     if (CurrentMenu[CurrentItem][0] == '�')
                        CurrentItem--;
                     wprints (CurrentItem, 0, WHITE|_BLACK, CurrentMenu[CurrentItem]);
                  }
                  break;

               case 0x5000:
                  if (CurrentItem < Items) {
                     wprints (CurrentItem, 0, BLUE|_LGREY, CurrentMenu[CurrentItem]);
                     CurrentItem++;
                     if (CurrentMenu[CurrentItem][0] == '�')
                        CurrentItem++;
                     wprints (CurrentItem, 0, WHITE|_BLACK, CurrentMenu[CurrentItem]);
                  }
                  break;

               case 0x4D00:
                  if (wh2 != 0 && wh3 == 0) {
                     wclose ();
                     if (CurrentMenu == MenuEdit) {
                        wprints (0, 1, BLUE|_LGREY, " Edit ");
                        wprints (0, 7, WHITE|_BLACK, " Mailer ");
                        CurrentMenu = MenuMailer;
                        Position = 7;
                        Items = 11;
                     }
                     else if (CurrentMenu == MenuMailer) {
                        wprints (0, 7, BLUE|_LGREY, " Mailer ");
                        wprints (0, 15, WHITE|_BLACK, " Log ");
                        CurrentMenu = MenuLog;
                        Position = 15;
                        Items = 2;
                     }
                     else if (CurrentMenu == MenuLog) {
                        wprints (0, 15, BLUE|_LGREY, " Log ");
                        wprints (0, 20, WHITE|_BLACK, " Config ");
                        CurrentMenu = MenuConfig;
                        Position = 20;
                        Items = 8;
                     }
                     else if (CurrentMenu == MenuConfig) {
                        wprints (0, 20, BLUE|_LGREY, " Config ");
                        wprints (0, 28, WHITE|_BLACK, " Help ");
                        CurrentMenu = MenuHelp;
                        Position = 28;
                        Items = 5;
                     }
                     else if (CurrentMenu == MenuHelp) {
                        wprints (0, 28, BLUE|_LGREY, " Help ");
                        wprints (0, 1, WHITE|_BLACK, " Edit ");
                        CurrentMenu = MenuEdit;
                        Position = 1;
                        Items = 4;
                     }
                     if ((wh2 = wopen (Y, Position, (SHORT)(1 + Items + 2), (SHORT)(Position + strlen (CurrentMenu[0]) + 1), 0, BLUE|_LGREY, BLUE|_LGREY)) != 0) {
                        for (i = 0; i <= Items; i++)
                           wprints (i, 0, BLUE|_LGREY, CurrentMenu[i]);
                        CurrentItem = 0;
                        wprints (CurrentItem, 0, WHITE|_BLACK, CurrentMenu[CurrentItem]);
                     }
                  }
                  break;

               case 0x4B00:
                  if (wh2 != 0 && wh3 == 0) {
                     wclose ();
                     if (CurrentMenu == MenuEdit) {
                        wprints (0, 1, BLUE|_LGREY, " Edit ");
                        wprints (0, 28, WHITE|_BLACK, " Help ");
                        CurrentMenu = MenuHelp;
                        Position = 28;
                        Items = 5;
                     }
                     else if (CurrentMenu == MenuHelp) {
                        wprints (0, 28, BLUE|_LGREY, " Help ");
                        wprints (0, 20, WHITE|_BLACK, " Config ");
                        CurrentMenu = MenuConfig;
                        Position = 20;
                        Items = 8;
                     }
                     else if (CurrentMenu == MenuConfig) {
                        wprints (0, 20, BLUE|_LGREY, " Config ");
                        wprints (0, 15, WHITE|_BLACK, " Log ");
                        CurrentMenu = MenuLog;
                        Position = 15;
                        Items = 2;
                     }
                     else if (CurrentMenu == MenuLog) {
                        wprints (0, 15, BLUE|_LGREY, " Log ");
                        wprints (0, 7, WHITE|_BLACK, " Mailer ");
                        CurrentMenu = MenuMailer;
                        Position = 7;
                        Items = 11;
                     }
                     else if (CurrentMenu == MenuMailer) {
                        wprints (0, 7, BLUE|_LGREY, " Mailer ");
                        wprints (0, 1, WHITE|_BLACK, " Edit ");
                        CurrentMenu = MenuEdit;
                        Position = 1;
                        Items = 4;
                     }
                     if ((wh2 = wopen (Y, Position, (SHORT)(1 + Items + 2), (SHORT)(Position + strlen (CurrentMenu[0]) + 1), 0, BLUE|_LGREY, BLUE|_LGREY)) != 0) {
                        for (i = 0; i <= Items; i++)
                           wprints (i, 0, BLUE|_LGREY, CurrentMenu[i]);
                        CurrentItem = 0;
                        wprints (CurrentItem, 0, WHITE|_BLACK, CurrentMenu[CurrentItem]);
                     }
                  }
                  break;

               case ESC:
                  if (wh3 != 0) {
                     wclose ();
                     wh3 = 0;
                     if (CurrentMenu == MenuBBS) {
                        CurrentMenu = MenuConfig;
                        CurrentItem = 6;
                        Items = 8;
                     }
                     else if (CurrentMenu == MenuControls) {
                        CurrentMenu = MenuMailer;
                        CurrentItem = 11;
                        Items = 11;
                     }
                  }
                  else {
                     if (wh2 != 0)
                        wclose ();
                     EndRun = TRUE;
                  }
                  break;
            }
         }

         if (time (NULL) != Time) {
            Time = time (NULL);
            tp = localtime (&Time);
            sprintf (Temp, "%s, %s %d %d", WeekDays[tp->tm_wday], Months[tp->tm_mon], tp->tm_mday, tp->tm_year + 1900);
            prints (2, (SHORT)(54 + ((25 - strlen (Temp)) / 2)), YELLOW|_BLACK, Temp);
            sprintf (Temp, "%02d:%02d:%02d", tp->tm_hour % 24, tp->tm_min % 60, tp->tm_sec % 60);
            prints (3, (SHORT)(54 + ((25 - strlen (Temp)) / 2)), YELLOW|_BLACK, Temp);

            sprintf (Temp, "%02ld:%02ld", (Time - Elapsed) / 60L, (Time - Elapsed) % 60L);
            prints (6, 65, YELLOW|_BLACK, Temp);
         }
      }

      wclose ();
   }
}

// ----------------------------------------------------------------------

void main (int argc, char *argv[])
{
   USHORT i, Quit, DoLocal, Port, Task;
   PSZ LogFile, CfgPath;
   ULONG Speed;
   class TConfig *Cfg;
   class TModem *Modem;
   class TLocal *Local;

   Quit = DoLocal = FALSE;
   LogFile = NULL;
   CfgPath = "";
   Port = 0;
   Speed = 0L;
   Task = 1;

   for (i = 1; i < argc; i++) {
      if (!stricmp (argv[i], "?") || !stricmp (argv[i], "-?") || !strnicmp (argv[i], "-h", 2)) {
         cprintf ("\r\nLoraBBS Standard Edition for DOS - Version %s\r\n", VERSION);
         cprintf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\r\n\r\n");
         cprintf ("Usage:\r\n        DOSBBS [switches] [TERM|IMPORT|EXPORT|PACK]\r\n\r\n");
         cprintf ("Where [switches] are:\r\n\r\n");
         cprintf ("   -c<path>  = Sets the path where to find the configuration files\r\n");
         cprintf ("   -l        = Run in local connection mode\r\n");
         cprintf ("   -n<num>   = Sets the task number\r\n");
         cprintf ("   -p<port>  = Overrides the COM port number\r\n");
         cprintf ("   -r<file>  = Sets the name of the log file\r\n");
         cprintf ("   -s<speed> = Overrides the DTE<>DCE speed\r\n");
         Quit = TRUE;
      }
      else if (!strnicmp (argv[i], "-c", 2))
         CfgPath = &argv[i][2];
      else if (!stricmp (argv[i], "-l"))
         DoLocal = TRUE;
      else if (!strnicmp (argv[i], "-n", 2))
         Task = (USHORT)atoi (&argv[i][2]);
      else if (!strnicmp (argv[i], "-p", 2))
         Port = (USHORT)atoi (&argv[i][2]);
      else if (!strnicmp (argv[i], "-r", 2))
         LogFile = &argv[i][2];
      else if (!strnicmp (argv[i], "-s", 2))
         Speed = atol (&argv[i][2]);
   }

   if (Quit == FALSE) {
      if ((Cfg = new TConfig (CfgPath)) != 0) {
         if (Cfg->Read (Task) == TRUE) {
            if (Port != 0)
               sprintf (Cfg->Device, "COM%d", Port);
            if (Speed != 0L)
               Cfg->Speed = Speed;

            if (DoLocal == TRUE) {
               if ((Local = new TLocal) != 0) {
                  Local->Cfg = Cfg;
                  Local->Run ();
                  if (Local->Log != 0)
                     delete Local->Log;
                  delete Local;
               }
            }
            else {
               if ((Modem = new TModem) != 0) {
                  Modem->Cfg = Cfg;
                  Modem->Run ();
                  delete Modem;
               }
            }

            wcloseall ();
            cclrscrn (LGREY|_BLACK);
         }
      }
   }
}


