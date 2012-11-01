
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.14
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "bbs.h"

USHORT TBbs::LoginUser (PSZ pszName)
{
   USHORT RetVal, Count, NewLogin;
   CHAR Temp[36], Pwd[16], Verify[16], *p;

   RetVal = FALSE;
   NewLogin = FALSE;

   Log->Write (Log->IsCalling, pszName);
   if (Status != NULL) {
      strcpy (Status->User, pszName);
      strcpy (Status->Status, "Login...");
      Status->Update ();
   }

   if (User->GetData (pszName) == FALSE) {
      Printf (Lang->NameNotFound);
      if (GetAnswer (ASK_DEFNO) == ANSWER_YES) {
         HotKey = TRUE;

         if (Status != NULL) {
            strcpy (Status->Status, "New user login");
            Status->Update ();
         }

         User->Clear ();
         strcpy (User->Name, pszName);
         User->ScreenHeight = 24;
         User->ScreenWidth = 80;
         User->Level = Cfg->NewLevel;
         User->HotKey = TRUE;
         strcpy (User->LimitClass, Cfg->NewLimitClass);
         User->CreationDate = time (NULL);
         Limits->Read (User->LimitClass);

         Log->Write (Log->NotInList, User->Name);

         ReadFile ("NEWUSER1");

         if (AbortSession () == FALSE) {
            Printf (Lang->AskAnsi);
            if (GetAnswer (ASK_DEFYES) == ANSWER_YES) {
               User->Ansi = TRUE;
               Ansi = TRUE;
            }
         }

         if (AbortSession () == FALSE && Cfg->AskRealName != Q_NO)
            do {
               Printf (Lang->AskRealName);
               GetString (User->RealName, (SHORT)(sizeof (User->RealName) - 1), INP_FIELD|INP_FANCY);
            } while (User->RealName[0] == '\0' && AbortSession () == FALSE && Cfg->AskRealName == Q_REQUIRED);

         if (AbortSession () == FALSE && Cfg->AskCompany != Q_NO)
            do {
               Printf (Lang->EnterCompanyName);
               GetString (User->Company, (SHORT)(sizeof (User->Company) - 1), INP_FIELD);
            } while (User->Company[0] == '\0' && AbortSession () == FALSE && Cfg->AskCompany == Q_REQUIRED);

         if (AbortSession () == FALSE && Cfg->AskAddress != Q_NO)
            do {
               Printf (Lang->StreetAddress);
               GetString (User->Address1, (SHORT)(sizeof (User->Address1) - 1), INP_FIELD);
            } while (User->Address1[0] == '\0' && AbortSession () == FALSE && Cfg->AskAddress == Q_REQUIRED);

         if (AbortSession () == FALSE && Cfg->AskZipCity != Q_NO)
            do {
               Printf (Lang->City);
               GetString (User->Address2, (SHORT)(sizeof (User->Address2) - 1), INP_FIELD);
            } while (User->Address2[0] == '\0' && AbortSession () == FALSE && Cfg->AskZipCity == Q_REQUIRED);

         if (AbortSession () == FALSE && Cfg->AskState != Q_NO)
            do {
               Printf (Lang->Country);
               GetString (User->Address3, (SHORT)(sizeof (User->Address3) - 1), INP_FIELD);
            } while (User->Address3[0] == '\0' && AbortSession () == FALSE && Cfg->AskState == Q_REQUIRED);

         if (AbortSession () == FALSE && Cfg->AskPhone != Q_NO)
            do {
               Printf (Lang->PhoneNumber);
               GetString (User->DayPhone, (SHORT)(sizeof (User->DayPhone) - 1), INP_FIELD);
            } while (User->DayPhone[0] == '\0' && AbortSession () == FALSE && Cfg->AskPhone == Q_REQUIRED);

         if (AbortSession () == FALSE && Cfg->AskSex != Q_NO)
            do {
               Printf (Lang->AskSex);
               GetString (Temp, 1, INP_FIELD);
               User->Sex = (UCHAR)toupper (Temp[0]);
            } while (User->Sex != 'M' && User->Sex != 'F' && AbortSession () == FALSE && Cfg->AskSex == Q_REQUIRED);

         strcpy (Temp, User->Name);
         p = strtok (Temp, " ");
         User->MailBox[0] = *p;                 // La prima lettera della mailbox e'
         User->MailBox[1] = '\0';               // la prima lettera del nome
         if ((p = strtok (NULL, " ")) == NULL)  // Cerca il cognome
            p = &Temp[1];
         if (strlen (p) > 7)                    // Se la mailbox risultasse piu' di
            p[7] = '\0';                        // otto caratteri, forza gli otto caratteri
         strcat (User->MailBox, p);
         strlwr (User->MailBox);

         ReadFile ("USERID");
         Printf ("\n\x16\x01\013Do you want to be known as '\x16\x01\016%s\x16\x01\013' on the system", User->MailBox);
         if (GetAnswer (ASK_DEFYES) == ANSWER_NO && AbortSession () == FALSE)
            do {
               Printf ("\n\x16\x01\013Enter the User-ID you want to be known as: \x16\x01\x1E");
               GetString (User->MailBox, 8, INP_FIELD);
               if (AbortSession () == FALSE)
                  Printf ("\n\x16\x01\013Do you want to be known as '\x16\x01\016%s\x16\x01\013' on the system", strlwr (User->MailBox));
            } while (AbortSession () == FALSE && GetAnswer (ASK_DEFYES) == ANSWER_NO);

         ReadFile ("NEWUSER2");

         do {
            do {
               Printf (Lang->SelectPassword);
               GetString (Pwd, sizeof (Pwd) - 1, INP_FIELD|INP_PWD);
            } while (AbortSession () == FALSE && strlen (Pwd) < 4);

            Printf (Lang->PleaseReenter);
            GetString (Verify, sizeof (Verify) - 1, INP_FIELD|INP_PWD);

            if (stricmp (Pwd, Verify))
               Printf (Lang->WrongPassword, Pwd, Verify);
         } while (AbortSession () == FALSE && stricmp (Pwd, Verify));

         if (AbortSession () == FALSE) {
            User->SetPassword (strlwr (Pwd));
            User->Add ((USHORT)(sizeof (LASTREAD) * 20));
            User->Validate = FALSE;
            RetVal = TRUE;
         }
      }
      else
         Log->Write (Log->BrainLapsed, Temp);
   }
   else {
      if (Status != NULL) {
         strcpy (Status->User, User->Name);
         strcpy (Status->Location, User->Address2);
         strcpy (Status->Status, "Login...");
         Status->Update ();
      }

      Count = 0;

      do {
         Printf (Lang->Password);
         GetString (Pwd, sizeof (Pwd) - 1, INP_FIELD|INP_PWD);
         if (User->CheckPassword (Pwd) == FALSE)
            Printf (Lang->Wrong);
      } while (AbortSession () == FALSE && User->CheckPassword (Pwd) == FALSE && ++Count < 4);

      if (AbortSession () == FALSE && User->CheckPassword (Pwd) == TRUE)
         RetVal = TRUE;
   }

   if (RetVal == TRUE) {
      Ansi = User->Ansi;
      Avatar = User->Avatar;
      HotKey = User->HotKey;
      ScreenHeight = User->ScreenHeight;
      ScreenWidth = User->ScreenWidth;

      if (Status != NULL) {
         sprintf (Status->User, "%s (%s)", User->Name, User->Address2);
         Status->Update ();
      }
   }

   return (RetVal);
}

USHORT TBbs::VerifyAccount (VOID)
{
   USHORT retVal = FALSE, Day, Month, Year, WDay;
//   CHAR Temp[64];
   time_t t;
   struct tm *ltm;

   t = time (NULL);
   if ((ltm = localtime (&t)) != NULL) {
      Day = (USHORT)ltm->tm_mday;
      Month = (USHORT)ltm->tm_mon;
      Year = (USHORT)ltm->tm_year;
      WDay = (USHORT)ltm->tm_wday;
      if ((ltm = localtime ((time_t *)&User->LastCall)) != NULL) {
         if (Year != ltm->tm_year) {
            User->YearTime = 0;
            User->MonthTime = 0;
            User->WeekTime = 0;
            User->TodayTime = 0;
         }
         else if (Month != ltm->tm_mon) {
            User->MonthTime = 0;
            User->WeekTime = 0;
            User->TodayTime = 0;
         }
         else if (Day != ltm->tm_mday) {
            User->TodayTime = 0;
            if (WDay <= ltm->tm_wday)
               User->WeekTime = 0;
         }
      }
   }

   if (Limits != NULL)
      Limits->Read (User->LimitClass, TRUE);
   Log->Write (Log->GivenLevel, TimeLeft (), User->Level, User->LimitClass);

   if (User->Level == 0) {
      ReadFile ("LOCKOUT");
      retVal = FALSE;
   }
   else {
      User->TotalCalls++;
      User->Update ();
//      if (Cfg->Language.Check (User->Language) == TRUE) {
//         sprintf (Temp, "%s%s", Cfg->SystemPath, Cfg->Language.File);
//         Lang->Read (Temp);
//      }
      ReadFile ("WELCOME");
      if (ReadFile ("BULLETIN") == FALSE)
         ReadFile ("NEWS");
      retVal = TRUE;
   }

   return (TRUE);
}


