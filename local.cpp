
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.16
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"
#include "mailer.h"
#include "menu.h"
#include "version.h"

class TLocal : public TBbs
{
public:
   TLocal (USHORT usChannel);
   ~TLocal (void);

   LONG   CheckResponse (VOID);
   VOID   ReleaseTimeSlice (VOID);
   VOID   Run (VOID);
   VOID   SendCommand (PSZ cmd);

private:
#if defined(__OS2__)
   HEV    hEvent;
#endif
   class  TScreen *Screen;
};

TLocal::TLocal (USHORT usChannel) : TBbs ()
{
   User = NULL;
   if ((Cfg = new TConfig (".\\")) != NULL)
      Cfg->Read (usChannel);
   Remote = FALSE;
}

TLocal::~TLocal (void)
{
   if (Cfg != NULL)
      delete Cfg;
}

VOID TLocal::Run (VOID)
{
   CHAR temp[64];
   ULONG CallLen;
   class TDetect *Detect;
   class TEMail *Mail;

   if ((Status = new TStatus (Cfg->SystemPath)) != NULL) {
      Status->Activate (Cfg->Channel);
      strcpy (Status->User, "Answering call...");
      Status->Speed = 0L;
      Status->Update ();
   }

   if ((Log = new TLog) != NULL) {
      sprintf (temp, "%sCH%d.LOG", Cfg->LogPath, Cfg->Channel);
      if (Log->Open (temp) == TRUE) {
         Log->Level = Cfg->LogLevel;
         Log->Write (Log->Begin, VERSION, Cfg->Channel);

         if ((Screen = new TScreen) != NULL) {
            if (Screen->Initialize () == TRUE) {
               Com = Screen;

#if defined(__OS2__)
               DosCreateEventSem (NULL, &hEvent, 0L, FALSE);
#endif

               Hangup = FALSE;
               Ansi = TRUE;
               Lang = new TLanguage;
               User = new TUser (Cfg->UserFile);
               Limits = new TLimits (Cfg->SystemPath);

               LastActivity = StartCall = time (NULL);
               CarrierSpeed = 57600L;

               DisplayBanner ();

               if ((Detect = new TDetect (this)) != NULL) {
                  Detect->SelectLanguage ();
                  delete Detect;
               }

               if (LoginUser () == TRUE) {
                  if (VerifyAccount () == TRUE) {
                     if ((Mail = new TEMail (this)) != NULL) {
                        Mail->Check ();
                        delete Mail;
                     }
                     class TMenu *Menu = new TMenu (this);
                     Menu->Run ((Cfg->FirstMenu[0] == '\0') ? "TOP" : Cfg->FirstMenu);
                     delete Menu;
                  }

                  CallLen = (time (NULL) - StartCall) / 60L;

                  User->LastCall = time (NULL);
                  User->TodayTime += CallLen;
                  User->WeekTime += CallLen;
                  User->MonthTime += CallLen;
                  User->YearTime += CallLen;
                  if (User->Update () == FALSE)
                     Log->Write ("!Unable to update user");

                  Log->Write (Log->UserOffline, User->Name, User->TotalCalls, CallLen);
                  Log->WriteBlank ();
               }

               delete Limits;
               Limits = NULL;
               delete User;
               User = NULL;
               delete Lang;
               Lang = NULL;

               Pause (200);
               Com->SendBytes ((UCHAR *)"\r\n\x1B[0;37m\x1B[2J\x1B[1;1f", 19);

#if defined(__OS2__)
               DosCloseEventSem (hEvent);
#endif
            }

            delete Screen;
         }

         Log->Write (Log->End);
         Log->Close ();
      }

      delete Log;
   }

   if (Status != NULL) {
      Status->Deactivate ();
      delete Status;
   }
}

VOID TLocal::ReleaseTimeSlice (VOID)
{
#if defined(__OS2__)
   DosWaitEventSem (hEvent, 10L);
#endif
}

// ----------------------------------------------------------------------

void main (int argc, char *argv[])
{
   class TLocal *Local;

   if (argc < 2) {
      printf ("\nLoraBBS Professional Edition for OS/2 PM. Version %s\n", VERSION);
      printf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\n\n");
   }
   else {
      if ((Local = new TLocal ((USHORT)atoi (argv[1]))) != NULL) {
         Local->Run ();
         delete Local;
      }
   }
}



