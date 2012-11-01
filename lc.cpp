
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.12
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/17/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lora_api.h"
#include "menu.h"

USHORT MsgPending = FALSE, MsgUpdate = FALSE, FilePending = FALSE, FileUpdate;
USHORT PackerPending = FALSE, PackerUpdate, NodesPending = FALSE, NodesUpdate;
USHORT LastOp, ChannelPending = FALSE, MenuPending = FALSE;
CHAR MenuName[128];
class TConfig *Cfg = NULL;
class TMsgData *Msg = NULL;
class TFileData *File = NULL;
class TPacker *Packer = NULL;
class TNodes *Nodes = NULL;
class TMenu *Menu = NULL;

void copyPath (char *dst, char *src)
{
   strcpy (dst, src);
   if (dst[strlen (dst) - 1] == '\\' || dst[strlen (dst) - 1] == '/')
      dst[strlen (dst) - 1] = '\0';
#if defined(__LINUX__)
   strcat (dst, "/");
#else
   strcat (dst, "\\");
#endif
}

int checkSystemConfiguration (char *p)
{
   int retval = FALSE;

   if (!stricmp (p, "system")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "path")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               copyPath (Cfg->SystemPath, p);
         }
         else if (!stricmp (p, "name")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->SystemName, p);
            }
         }
      }
   }
   else if (!stricmp (p, "sysop")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "name")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->SysopName, p);
            }
         }
      }
   }
   else if (!stricmp (p, "normal")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "inbound")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               copyPath (Cfg->NormalInbound, p);
         }
      }
   }
   else if (!stricmp (p, "protected")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "inbound")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               copyPath (Cfg->ProtectedInbound, p);
         }
      }
   }
   else if (!stricmp (p, "known")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "inbound")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               copyPath (Cfg->KnownInbound, p);
         }
      }
   }
   else if (!stricmp (p, "outbound")) {
      retval = TRUE;
      if ((p = strtok (NULL, " ")) != NULL)
         copyPath (Cfg->Outbound, p);
   }
   else if (!stricmp (p, "log")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "file")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->LogFile, p);
         }
      }
   }
   else if (!stricmp (p, "main")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "menu")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->MainMenu, p);
         }
      }
   }
   else if (!stricmp (p, "nodelist")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "flags")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->NodelistFlags, p);
            }
         }
         else if (!stricmp (p, "path")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               copyPath (Cfg->NodelistPath, p);
         }
      }
   }
   else if (!stricmp (p, "location")) {
      retval = TRUE;
      if ((p = strtok (NULL, "")) != NULL) {
         while (*p == ' ')
            p++;
         strcpy (Cfg->Location, p);
      }
   }
   else if (!stricmp (p, "phone")) {
      retval = TRUE;
      if ((p = strtok (NULL, "")) != NULL) {
         while (*p == ' ')
            p++;
         strcpy (Cfg->Phone, p);
      }
   }
   else if (!stricmp (p, "menu")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "path")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               copyPath (Cfg->MenuPath, p);
         }
      }
   }
   else if (!stricmp (p, "text")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "files")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "path")) {
                  retval = TRUE;
                  if ((p = strtok (NULL, " ")) != NULL)
                     copyPath (Cfg->TextFiles, p);
               }
            }
         }
      }
   }
   else if (!stricmp (p, "temp")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "path")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               copyPath (Cfg->TempPath, p);
         }
      }
   }
   else if (!stricmp (p, "olr")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "packet")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               copyPath (Cfg->OLRPacketName, p);
         }
      }
   }
   else if (!stricmp (p, "olr")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "max")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "messages")) {
                  retval = TRUE;
                  Cfg->OLRMaxMessages = (USHORT)atoi (p);
               }
            }
         }
      }
   }
   else if (!stricmp (p, "users")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "file")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->UserFile, p);
         }
         else if (!stricmp (p, "home")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               copyPath (Cfg->UsersHomePath, p);
         }
      }
   }
   else if (!stricmp (p, "address")) {
      retval = TRUE;
      if ((p = strtok (NULL, " ")) != NULL)
         Cfg->MailAddress.Add (p);
   }
   else if (!stricmp (p, "scheduler")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "file")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->SchedulerFile, p);
         }
      }
   }
   else if (!stricmp (p, "new")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "user")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "level")) {
                  retval = TRUE;
                  if ((p = strtok (NULL, " ")) != NULL)
                     Cfg->NewUserLevel = (USHORT)atoi (p);
               }
               else if (!stricmp (p, "limits")) {
                  retval = TRUE;
                  if ((p = strtok (NULL, " ")) != NULL)
                     strcpy (Cfg->NewUserLimits, p);
               }
            }
         }
         else if (!stricmp (p, "areas")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "path")) {
                  retval = TRUE;
                  if ((p = strtok (NULL, " ")) != NULL)
                     copyPath (Cfg->NewAreasPath, p);
               }
               else if (!stricmp (p, "type")) {
                  retval = TRUE;
                  if ((p = strtok (NULL, " ")) != NULL) {
                     if (!stricmp (p, "squish"))
                        Cfg->NewAreasStorage = ST_SQUISH;
                     else if (!stricmp (p, "fido"))
                        Cfg->NewAreasStorage = ST_FIDO;
                     else if (!stricmp (p, "jam"))
                        Cfg->NewAreasStorage = ST_JAM;
                     else if (!stricmp (p, "adept"))
                        Cfg->NewAreasStorage = ST_ADEPT;
                     else if (!stricmp (p, "usenet"))
                        Cfg->NewAreasStorage = ST_USENET;
                  }
               }
            }
         }
      }
   }
   else if (!stricmp (p, "ask")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "ansi")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "yes")) {
                  retval = TRUE;
                  Cfg->CheckAnsi = YES;
               }
               else if (!stricmp (p, "no")) {
                  retval = TRUE;
                  Cfg->CheckAnsi = NO;
               }
               else if (!stricmp (p, "required")) {
                  retval = TRUE;
                  Cfg->CheckAnsi = REQUIRED;
               }
            }
         }
         else if (!stricmp (p, "realname")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "yes")) {
                  retval = TRUE;
                  Cfg->RealName = YES;
               }
               else if (!stricmp (p, "no")) {
                  retval = TRUE;
                  Cfg->RealName = NO;
               }
               else if (!stricmp (p, "required")) {
                  retval = TRUE;
                  Cfg->RealName = REQUIRED;
               }
            }
         }
         else if (!stricmp (p, "company")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "yes")) {
                  retval = TRUE;
                  Cfg->CompanyName = YES;
               }
               else if (!stricmp (p, "no")) {
                  retval = TRUE;
                  Cfg->CompanyName = NO;
               }
               else if (!stricmp (p, "required")) {
                  retval = TRUE;
                  Cfg->CompanyName = REQUIRED;
               }
            }
         }
         else if (!stricmp (p, "address")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "yes")) {
                  retval = TRUE;
                  Cfg->Address = YES;
               }
               else if (!stricmp (p, "no")) {
                  retval = TRUE;
                  Cfg->Address = NO;
               }
               else if (!stricmp (p, "required")) {
                  retval = TRUE;
                  Cfg->Address = REQUIRED;
               }
            }
         }
         else if (!stricmp (p, "city")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "yes")) {
                  retval = TRUE;
                  Cfg->City = YES;
               }
               else if (!stricmp (p, "no")) {
                  retval = TRUE;
                  Cfg->City = NO;
               }
               else if (!stricmp (p, "required")) {
                  retval = TRUE;
                  Cfg->City = REQUIRED;
               }
            }
         }
         else if (!stricmp (p, "phone")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "yes")) {
                  retval = TRUE;
                  Cfg->PhoneNumber = YES;
               }
               else if (!stricmp (p, "no")) {
                  retval = TRUE;
                  Cfg->PhoneNumber = NO;
               }
               else if (!stricmp (p, "required")) {
                  retval = TRUE;
                  Cfg->PhoneNumber = REQUIRED;
               }
            }
         }
         else if (!stricmp (p, "gender")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "yes")) {
                  retval = TRUE;
                  Cfg->Gender = YES;
               }
               else if (!stricmp (p, "no")) {
                  retval = TRUE;
                  Cfg->Gender = NO;
               }
               else if (!stricmp (p, "required")) {
                  retval = TRUE;
                  Cfg->Gender = REQUIRED;
               }
            }
         }
      }
   }
   else if (!stricmp (p, "ansi")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "login")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "yes")) {
                  retval = TRUE;
                  Cfg->Ansi = YES;
               }
               else if (!stricmp (p, "no")) {
                  retval = TRUE;
                  Cfg->Ansi = NO;
               }
               else if (!stricmp (p, "detect") || !stricmp (p, "auto")) {
                  retval = TRUE;
                  Cfg->Ansi = AUTO;
               }
            }
         }
      }
   }
   else if (!stricmp (p, "iemsi")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "login")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "yes")) {
                  retval = TRUE;
                  Cfg->IEMSI = YES;
               }
               else if (!stricmp (p, "no")) {
                  retval = TRUE;
                  Cfg->IEMSI = NO;
               }
            }
         }
      }
   }
   else if (!stricmp (p, "netmail")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "path")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->NetMailPath, p);
         }
         else if (!stricmp (p, "type")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "squish"))
                  Cfg->NetMailStorage = ST_SQUISH;
               else if (!stricmp (p, "fido"))
                  Cfg->NetMailStorage = ST_FIDO;
               else if (!stricmp (p, "jam"))
                  Cfg->NetMailStorage = ST_JAM;
               else if (!stricmp (p, "adept"))
                  Cfg->NetMailStorage = ST_ADEPT;
               else if (!stricmp (p, "usenet"))
                  Cfg->NetMailStorage = ST_USENET;
            }
         }
      }
   }
   else if (!stricmp (p, "mail")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "path")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->MailPath, p);
         }
         else if (!stricmp (p, "type")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "squish"))
                  Cfg->MailStorage = ST_SQUISH;
               else if (!stricmp (p, "fido"))
                  Cfg->MailStorage = ST_FIDO;
               else if (!stricmp (p, "jam"))
                  Cfg->MailStorage = ST_JAM;
               else if (!stricmp (p, "adept"))
                  Cfg->MailStorage = ST_ADEPT;
               else if (!stricmp (p, "usenet"))
                  Cfg->MailStorage = ST_USENET;
            }
         }
         else if (!stricmp (p, "server")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->MailServer, p);
         }
      }
   }
   else if (!stricmp (p, "duplicate")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "path")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->DupePath, p);
         }
         else if (!stricmp (p, "type")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "squish"))
                  Cfg->DupeStorage = ST_SQUISH;
               else if (!stricmp (p, "fido"))
                  Cfg->DupeStorage = ST_FIDO;
               else if (!stricmp (p, "jam"))
                  Cfg->DupeStorage = ST_JAM;
               else if (!stricmp (p, "adept"))
                  Cfg->DupeStorage = ST_ADEPT;
               else if (!stricmp (p, "usenet"))
                  Cfg->DupeStorage = ST_USENET;
            }
         }
      }
   }
   else if (!stricmp (p, "bad")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "path")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->BadPath, p);
         }
         else if (!stricmp (p, "type")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "squish"))
                  Cfg->BadStorage = ST_SQUISH;
               else if (!stricmp (p, "fido"))
                  Cfg->BadStorage = ST_FIDO;
               else if (!stricmp (p, "jam"))
                  Cfg->BadStorage = ST_JAM;
               else if (!stricmp (p, "adept"))
                  Cfg->BadStorage = ST_ADEPT;
               else if (!stricmp (p, "usenet"))
                  Cfg->BadStorage = ST_USENET;
            }
         }
      }
   }
   else if (!stricmp (p, "news")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "server")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->NewsServer, p);
         }
      }
   }
   else if (!stricmp (p, "host")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "name")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->HostName, p);
         }
      }
   }
   else if (!stricmp (p, "keep")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "netmail")) {
            retval = TRUE;
            Cfg->KeepNetMail = TRUE;
         }
      }
   }
   else if (!stricmp (p, "secure")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "mail")) {
            retval = TRUE;
            Cfg->Secure = TRUE;
         }
      }
   }
   else if (!stricmp (p, "wazoo")) {
      retval = TRUE;
      Cfg->WaZoo = TRUE;
   }
   else if (!stricmp (p, "EMSI")) {
      retval = TRUE;
      Cfg->EMSI = TRUE;
   }
   else if (!stricmp (p, "Janus")) {
      retval = TRUE;
      Cfg->Janus = TRUE;
   }
   else if (!stricmp (p, "mail")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "only")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->MailOnly, p);
            }
         }
      }
   }
   else if (!stricmp (p, "enter")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "bbs")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->EnterBBS, p);
            }
         }
      }
   }
   else if (!stricmp (p, "tear")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "line")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->TearLine, p);
            }
         }
      }
   }
   else if (!stricmp (p, "force")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "intl")) {
            retval = TRUE;
            Cfg->ForceIntl = TRUE;
         }
      }
   }
   else if (!stricmp (p, "import")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "empty")) {
            retval = TRUE;
            Cfg->ImportEmpty = TRUE;
         }
      }
   }
   else if (!stricmp (p, "replace")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "tear")) {
            retval = TRUE;
            Cfg->ReplaceTear = TRUE;
         }
      }
   }
   else if (!stricmp (p, "use")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "areasbbs")) {
            retval = TRUE;
            Cfg->UseAreasBBS = TRUE;
         }
      }
   }
   else if (!stricmp (p, "update")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "areasbbs")) {
            retval = TRUE;
            Cfg->UpdateAreasBBS = TRUE;
         }
      }
   }
   else if (!stricmp (p, "separate")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "netmail")) {
            retval = TRUE;
            Cfg->SeparateNetMail = TRUE;
         }
      }
   }
   else if (!stricmp (p, "zmodem")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "telnet")) {
            retval = TRUE;
            Cfg->ZModemTelnet = TRUE;
         }
      }
   }
   else if (!stricmp (p, "detect")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "ppp")) {
            retval = TRUE;
            Cfg->EnablePPP = TRUE;
         }
      }
   }
   else if (!stricmp (p, "ppp")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "command")) {
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->PPPCmd, p);
               retval = TRUE;
            }
         }
         else if (!stricmp (p, "limit")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               Cfg->PPPTimeLimit = (USHORT)atoi (p);
               retval = TRUE;
            }
         }
      }
   }
   else if (!stricmp (p, "areas")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "bbs")) {
            retval = TRUE;
            if ((p = strtok (NULL, " ")) != NULL)
               strcpy (Cfg->AreasBBS, p);
         }
      }
   }
   else if (!stricmp (p, "after")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "caller")) {
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->AfterCallerCmd, p);
               retval = TRUE;
            }
         }
      }
   }
   else if (!stricmp (p, "after")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "mail")) {
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->AfterMailCmd, p);
               retval = TRUE;
            }
         }
      }
   }
   else if (!stricmp (p, "import")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "command")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->ImportCmd, p);
            }
         }
      }
   }
   else if (!stricmp (p, "export")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "command")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->ExportCmd, p);
            }
         }
      }
   }
   else if (!stricmp (p, "pack")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "command")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->PackCmd, p);
            }
         }
      }
   }
   else if (!stricmp (p, "single")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "pass")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->SinglePassCmd, p);
            }
         }
      }
   }
   else if (!stricmp (p, "newsgroup")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "command")) {
            retval = TRUE;
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
               strcpy (Cfg->NewsgroupCmd, p);
            }
         }
      }
   }

   return (retval);
}

int checkMessageAreas (char *p)
{
   int retval = FALSE;
   ULONG HWM, Highest;
   class TEchoLink *Link;

   if (!stricmp (p, "message")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "area")) {
            retval = TRUE;
            if (Msg != NULL && MsgPending == TRUE) {
               if (MsgUpdate == TRUE)
                  Msg->Update ();
               else
                  Msg->Add ();
               MsgUpdate = MsgPending = FALSE;
            }
            if (Msg == NULL)
               Msg = new TMsgData (Cfg->SystemPath);
            if ((p = strtok (NULL, " ")) != NULL) {
               if ((MsgUpdate = Msg->Read (p)) == TRUE) {
                  HWM = Msg->HighWaterMark;
                  Highest = Msg->Highest;
               }
               Msg->New ();
               strcpy (Msg->Key, p);
               if (MsgUpdate == TRUE) {
                  Msg->HighWaterMark = HWM;
                  Msg->Highest = Highest;
               }
               MsgPending = TRUE;

               if (LastOp != 1 && LastOp != 0)
                  cprintf ("\r\n");
               cprintf ("\r +-- Message area '%s' ", Msg->Key);
               LastOp = 1;
            }
         }
      }
   }

   if (MsgPending == TRUE) {
      if (!stricmp (p, "display")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            strcpy (Msg->Display, p);
         }
      }
      else if (!stricmp (p, "type")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "squish"))
               Msg->Storage = ST_SQUISH;
            else if (!stricmp (p, "fido"))
               Msg->Storage = ST_FIDO;
            else if (!stricmp (p, "jam"))
               Msg->Storage = ST_JAM;
            else if (!stricmp (p, "adept"))
               Msg->Storage = ST_ADEPT;
            else if (!stricmp (p, "usenet"))
               Msg->Storage = ST_USENET;
         }
      }
      else if (!stricmp (p, "path")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (Msg->Path, p);
      }
      else if (!stricmp (p, "echotag")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (Msg->EchoTag, p);
         Msg->EchoMail = TRUE;
      }
      else if (!stricmp (p, "newsgroup")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (Msg->NewsGroup, p);
      }
      else if (!stricmp (p, "level")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            Msg->Level = (USHORT)atoi (p);
      }
      else if (!stricmp (p, "write")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "level")) {
               retval = TRUE;
               if ((p = strtok (NULL, " ")) != NULL)
                  Msg->WriteLevel = (USHORT)atoi (p);
            }
         }
      }
      else if (!stricmp (p, "forward")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "to")) {
               retval = TRUE;
               if ((p = strtok (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  if ((Link = new TEchoLink (Cfg->SystemPath)) != NULL) {
                     Link->Load (Msg->EchoTag);
                     Link->AddString (p);
                     Link->Save ();
                     delete Link;
                  }
               }
            }
         }
      }
      else if (!stricmp (p, "update")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "from")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (!stricmp (p, "newsgroup"))
                     Msg->UpdateNews = TRUE;
               }
            }
         }
      }
      else if (!stricmp (p, "max")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "messages")) {
               if ((p = strtok (NULL, " ")) != NULL)
                  Msg->MaxMessages = (USHORT)atoi (p);
            }
            else if (!stricmp (p, "days")) {
               if ((p = strtok (NULL, " ")) != NULL)
                  Msg->DaysOld = (USHORT)atoi (p);
            }
         }
      }
      else if (!stricmp (p, "show")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "global")) {
               if ((p = strtok (NULL, " ")) != NULL)
                  Msg->ShowGlobal = TRUE;
            }
         }
      }
      else if (!stricmp (p, "use")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "offline")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (!stricmp (p, "reader"))
                     Msg->Offline = TRUE;
               }
            }
         }
      }

      if (retval == FALSE) {
         if (Msg != NULL && MsgPending == TRUE) {
            if (MsgUpdate == TRUE)
               Msg->Update ();
            else
               Msg->Add ();
            MsgUpdate = MsgPending = FALSE;
         }
      }
   }

   return (retval);
}

int checkFileAreas (char *p)
{
   int retval = FALSE;
   class TFilechoLink *Link;

   if (!stricmp (p, "file")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         if (!stricmp (p, "area")) {
            retval = TRUE;
            if (File != NULL && FilePending == TRUE) {
               if (FileUpdate == TRUE)
                  File->Update ();
               else
                  File->Add ();
               FileUpdate = FilePending = FALSE;
            }
            if (File == NULL)
               File = new TFileData (Cfg->SystemPath);
            if ((p = strtok (NULL, " ")) != NULL) {
               FileUpdate = File->Read (p);
               File->New ();
               strcpy (File->Key, p);
               FilePending = TRUE;

               if (LastOp != 2 && LastOp != 0)
                  cprintf ("\r\n");
               cprintf ("\r +-- File area '%s' ", File->Key);
               LastOp = 2;
            }
         }
      }
   }

   if (FilePending == TRUE) {
      if (!stricmp (p, "display")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            strcpy (File->Display, p);
         }
      }
      else if (!stricmp (p, "level")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            File->Level = (USHORT)atoi (p);
      }
      else if (!stricmp (p, "download")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "level")) {
               retval = TRUE;
               if ((p = strtok (NULL, " ")) != NULL)
                  File->DownloadLevel = (USHORT)atoi (p);
            }
            else if (!stricmp (p, "path")) {
               retval = TRUE;
               if ((p = strtok (NULL, " ")) != NULL)
                  copyPath (File->Download, p);
            }
         }
      }
      else if (!stricmp (p, "upload")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "level")) {
               retval = TRUE;
               if ((p = strtok (NULL, " ")) != NULL)
                  File->UploadLevel = (USHORT)atoi (p);
            }
            else if (!stricmp (p, "path")) {
               retval = TRUE;
               if ((p = strtok (NULL, " ")) != NULL)
                  copyPath (File->Upload, p);
            }
         }
      }
      else if (!stricmp (p, "echotag")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (File->EchoTag, p);
      }
      else if (!stricmp (p, "forward")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "to")) {
               retval = TRUE;
               if ((p = strtok (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  if ((Link = new TFilechoLink (Cfg->SystemPath)) != NULL) {
                     Link->Load (File->EchoTag);
                     Link->AddString (p);
                     Link->Save ();
                     delete Link;
                  }
               }
            }
         }
      }
      else if (!stricmp (p, "show")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "global")) {
               if ((p = strtok (NULL, " ")) != NULL)
                  File->ShowGlobal = TRUE;
            }
         }
      }
      else if (!stricmp (p, "cdrom")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            File->CdRom = TRUE;
      }

      if (retval == FALSE) {
         if (File != NULL && FilePending == TRUE) {
            if (FileUpdate == TRUE)
               File->Update ();
            else
               File->Add ();
            FileUpdate = FilePending = FALSE;
         }
      }
   }

   return (retval);
}

int checkCompressor (char *p)
{
   int retval = FALSE;

   if (!stricmp (p, "compressor")) {
      retval = TRUE;
      if (Packer != NULL && PackerPending == TRUE) {
         if (PackerUpdate == TRUE)
            Packer->Update ();
         else
            Packer->Add ();
         PackerUpdate = PackerPending = FALSE;
      }
      if (Packer == NULL)
         Packer = new TPacker (Cfg->SystemPath);
      if ((p = strtok (NULL, " ")) != NULL) {
         PackerUpdate = Packer->Read (p, FALSE);
         Packer->New ();
         strcpy (Packer->Key, p);
         PackerPending = TRUE;

         if (LastOp != 3 && LastOp != 0)
            cprintf ("\r\n");
         cprintf ("\r +-- Compressor '%s' ", Packer->Key);
         LastOp = 3;
      }
   }

   if (PackerPending == TRUE) {
      if (!stricmp (p, "display")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            strcpy (Packer->Display, p);
         }
      }
      else if (!stricmp (p, "pack")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "command")) {
               retval = TRUE;
               if ((p = strtok (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (Packer->PackCmd, p);
               }
            }
         }
      }
      else if (!stricmp (p, "unpack")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "command")) {
               retval = TRUE;
               if ((p = strtok (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (Packer->UnpackCmd, p);
               }
            }
         }
      }
      else if (!stricmp (p, "position")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            retval = TRUE;
            Packer->Position = atol (p);
         }
      }
      else if (!stricmp (p, "id")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            retval = TRUE;
            strcpy (Packer->Id, p);
         }
      }
      else if (!stricmp (p, "dos")) {
         retval = TRUE;
         Packer->Dos = TRUE;
      }
      else if (!stricmp (p, "os/2")) {
         retval = TRUE;
         Packer->OS2 = TRUE;
      }
      else if (!stricmp (p, "windows")) {
         retval = TRUE;
         Packer->Windows = TRUE;
      }
      else if (!stricmp (p, "linux")) {
         retval = TRUE;
         Packer->Linux = TRUE;
      }

      if (retval == FALSE) {
         if (Packer != NULL && PackerPending == TRUE) {
            if (PackerUpdate == TRUE)
               Packer->Update ();
            else
               Packer->Add ();
            PackerUpdate = PackerPending = FALSE;
         }
      }
   }

   return (retval);
}

int checkNodes (char *p)
{
   int retval = FALSE;

   if (!stricmp (p, "node")) {
      retval = TRUE;
      if (Nodes != NULL && NodesPending == TRUE) {
         if (NodesUpdate == TRUE)
            Nodes->Update ();
         else
            Nodes->Add ();
         NodesUpdate = NodesPending = FALSE;
      }
      if (Nodes == NULL)
         Nodes = new TNodes (Cfg->NodelistPath);
      if ((p = strtok (NULL, " ")) != NULL) {
         NodesUpdate = Nodes->Read (p, FALSE);
         Nodes->New ();
         strcpy (Nodes->Address, p);
         NodesPending = TRUE;

         if (LastOp != 4 && LastOp != 0)
            cprintf ("\r\n");
         cprintf ("\r +-- Node '%s' ", Nodes->Address);
         LastOp = 4;
      }
   }

   if (NodesPending == TRUE) {
      if (!stricmp (p, "name")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            strcpy (Nodes->SystemName, p);
         }
      }
      else if (!stricmp (p, "sysop")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            strcpy (Nodes->SysopName, p);
         }
      }
      else if (!stricmp (p, "location")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            strcpy (Nodes->Location, p);
         }
      }
      else if (!stricmp (p, "phone")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            strcpy (Nodes->Phone, p);
         }
      }
      else if (!stricmp (p, "flags")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            strcpy (Nodes->Flags, p);
         }
      }
      else if (!stricmp (p, "remap")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "netmail")) {
               retval = TRUE;
               Nodes->RemapMail = TRUE;
            }
         }
      }
      else if (!stricmp (p, "compressor")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (Nodes->Packer, p);
      }
      else if (!stricmp (p, "session")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "password") || !stricmp (p, "pwd")) {
               retval = TRUE;
               if ((p = strtok (NULL, " ")) != NULL)
                  strcpy (Nodes->SessionPwd, p);
            }
         }
      }
      else if (!stricmp (p, "areamgr") || !stricmp (p, "areafix")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "password") || !stricmp (p, "pwd")) {
               retval = TRUE;
               if ((p = strtok (NULL, " ")) != NULL)
                  strcpy (Nodes->AreaMgrPwd, p);
            }
         }
      }
      else if (!stricmp (p, "in")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "packet")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (!stricmp (p, "password") || !stricmp (p, "pwd")) {
                     retval = TRUE;
                     if ((p = strtok (NULL, " ")) != NULL)
                        strcpy (Nodes->InPktPwd, p);
                  }
               }
            }
         }
      }
      else if (!stricmp (p, "out")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "packet")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (!stricmp (p, "password") || !stricmp (p, "pwd")) {
                     retval = TRUE;
                     if ((p = strtok (NULL, " ")) != NULL)
                        strcpy (Nodes->OutPktPwd, p);
                  }
               }
            }
         }
      }
      else if (!stricmp (p, "create")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "new")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (!stricmp (p, "areas")) {
                     retval = TRUE;
                     Nodes->CreateNewAreas = TRUE;
                  }
               }
            }
         }
      }
      else if (!stricmp (p, "new")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "areas")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (!stricmp (p, "filter")) {
                     retval = TRUE;
                     if ((p = strtok (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (Nodes->NewAreasFilter, p);
                     }
                  }
               }
            }
         }
      }

      if (retval == FALSE) {
         if (Nodes != NULL && NodesPending == TRUE) {
            if (NodesUpdate == TRUE)
               Nodes->Update ();
            else
               Nodes->Add ();
            NodesUpdate = NodesPending = FALSE;
         }
      }
   }

   return (retval);
}

int checkChannel (char *p)
{
   static int init = 0;
   int retval = FALSE;

   if (!stricmp (p, "channel")) {
      if ((p = strtok (NULL, " ")) != NULL) {
         retval = TRUE;
         if (ChannelPending == TRUE)
            Cfg->Save ();
         Cfg->NewChannel ();
         Cfg->TaskNumber = (USHORT)atoi (p);
         ChannelPending = TRUE;
         init = 0;
      }
   }

   if (ChannelPending == TRUE) {
      if (!stricmp (p, "port")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (Cfg->Device, p);
      }
      else if (!stricmp (p, "speed")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            Cfg->Speed = atol (p);
      }
      else if (!stricmp (p, "lock")) {
         retval = TRUE;
         Cfg->LockSpeed = TRUE;
      }
      else if (!stricmp (p, "initialize")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            if (init < 3)
               strcpy (Cfg->Initialize[init++], p);
         }
      }
      else if (!stricmp (p, "answer")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (Cfg->Answer, p);
      }
      else if (!stricmp (p, "ring")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (Cfg->Ring, p);
      }
      else if (!stricmp (p, "dial")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "timeout")) {
               if ((p = strtok (NULL, " ")) != NULL)
                  Cfg->DialTimeout = (USHORT)atoi (p);
            }
            else
               strcpy (Cfg->Dial, p);
         }
      }
      else if (!stricmp (p, "hangup")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (Cfg->Hangup, p);
      }
      else if (!stricmp (p, "offhook")) {
         retval = TRUE;
         if ((p = strtok (NULL, " ")) != NULL)
            strcpy (Cfg->OffHook, p);
      }
      else if (!stricmp (p, "fax")) {
         if ((p = strtok (NULL, " ")) != NULL) {
            if (!stricmp (p, "message")) {
               retval = TRUE;
               if ((p = strtok (NULL, " ")) != NULL)
                  strcpy (Cfg->FaxMessage, p);
            }
            else if (!stricmp (p, "command")) {
               retval = TRUE;
               if ((p = strtok (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (Cfg->FaxCommand, p);
               }
            }
         }
      }

      if (retval == FALSE) {
         if (ChannelPending == TRUE) {
            Cfg->Save ();
            ChannelPending = FALSE;
         }
      }
   }

   return (retval);
}

int checkMenu (char *p)
{
   int retval = FALSE;
   char *a;

   if (!stricmp (p, "menu")) {
      if (Menu == NULL)
         Menu = new TMenu;
      if ((p = strtok (NULL, " ")) != NULL) {
         retval = TRUE;
         if (MenuPending == TRUE)
            Menu->Save (MenuName);
         delete Menu;
         Menu = new TMenu;
         Menu->New (TRUE);
         strcpy (MenuName, p);
         MenuPending = TRUE;

         if (LastOp != 5 && LastOp != 0)
            cprintf ("\r\n");
         cprintf ("\r +-- Menu '%s' ", MenuName);
         LastOp = 5;
      }
   }

   if (MenuPending == TRUE) {
      if (!stricmp (p, "prompt")) {
         retval = TRUE;
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ' || *p == '"')
               p++;
            a = p;
            while (*p != '"') {
               if (*p == '\\' && p[1] == '"')
                  p++;
               p++;
            }
            *p++ = '\0';
            strcpy (Menu->Prompt, a);
            if ((p = strtok (p, " ")) != NULL)
               Menu->PromptColor = (UCHAR)atoi (p);
            if ((p = strtok (NULL, " ")) != NULL)
               Menu->PromptHilight = (UCHAR)atoi (p);
         }
      }
      else if (!stricmp (p, "item")) {
         retval = TRUE;
         Menu->New ();

         p = strtok (NULL, "");
         while (*p == ' ')
            p++;
         if (*p != '"') {
            a = p;
            while (*p != ' ')
               p++;
            *p++ = '\0';
            strcpy (Menu->Key, a);
            while (*p == ' ')
               p++;
         }
         if (*p == '"') {
            a = ++p;
            while (*p != '"') {
               if (*p == '\\' && p[1] == '"')
                  p++;
               p++;
            }
            *p++ = '\0';
            strcpy (Menu->Display, a);
         }
         if ((p = strtok (p, " ")) != NULL)
            Menu->Level = (UCHAR)atoi (p);
         if ((p = strtok (NULL, " ")) != NULL)
            Menu->Color = (UCHAR)atoi (p);
         if ((p = strtok (NULL, " ")) != NULL)
            Menu->Hilight = (UCHAR)atoi (p);
         Menu->Command = 0;
         Menu->Argument[0] = '\0';
         if ((p = strtok (NULL, " ")) != NULL)
            Menu->Command = (USHORT)atoi (p);
         if ((p = strtok (NULL, "")) != NULL) {
            while (*p == ' ')
               p++;
            strcpy (Menu->Argument, p);
         }
         Menu->Add ();
      }

      if (retval == FALSE) {
         if (MenuPending == TRUE) {
            Menu->Save (MenuName);
            MenuPending = FALSE;
         }
      }
   }

   return (retval);
}

void processFile (char *file)
{
   FILE *fp;
   CHAR Temp[512], *p;

   printf (" * Compiling '%s'\n", file);
   if ((fp = fopen (file, "rt")) != NULL) {
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         if ((p = strchr (Temp, '\r')) != NULL)
            *p = '\0';
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (*p == '%' || *p == ';' || *p == '\0')
               continue;
            if (!stricmp (p, "include")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (LastOp != 0)
                     printf ("\n");
                  LastOp = 0;
                  processFile (p);
               }
            }
            else if (!stricmp (p, "define")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (checkCompressor (p) == FALSE) {
                     if (checkNodes (p) == FALSE)
                        checkMenu (p);
                  }
               }
            }
            else if (NodesPending == TRUE)
               checkNodes (p);
            else if (PackerPending == TRUE)
               checkCompressor (p);
            else if (MenuPending == TRUE)
               checkMenu (p);
            else if (checkSystemConfiguration (p) == FALSE) {
               if (checkChannel (p) == FALSE) {
                  if (checkMessageAreas (p) == FALSE)
                     checkFileAreas (p);
               }
            }
         }
      }
      fclose (fp);
   }
}

void main (void)
{
   int i;
   FILE *fp;

   if ((Cfg = new TConfig) != NULL)
      Cfg->New ();
//      Cfg->Default ();
   Msg = NULL;
   File = NULL;
   LastOp = 0;

   printf ("\nLC; %s v%s - Configuration file compiler\n", NAME, VERSION);
   printf ("    Copyright (c) 1991-96 by Marco Maccaferri. All Rights Reserved.\n\n");

   if (ValidateKey ("bbs", NULL, NULL) == KEY_UNREGISTERED) {
      printf ("* * *     WARNING: No license key found    * * *\n");
      if ((i = CheckExpiration ()) == 0) {
         printf ("* * *   This evaluation copy has expired   * * *\n\a\n");
          exit (0);
      }
      else
         printf ("* * * You have %2d days left for evaluation * * * \n\a\n", i);
   }

   processFile ("lora.cfg");

   if (Msg != NULL && MsgPending == TRUE) {
      if (MsgUpdate == TRUE)
         Msg->Update ();
      else
         Msg->Add ();
   }
   if (File != NULL && FilePending == TRUE) {
      if (FileUpdate == TRUE)
         File->Update ();
      else
         File->Add ();
   }
   if (Packer != NULL && PackerPending == TRUE) {
      if (PackerUpdate == TRUE)
         Packer->Update ();
      else
         Packer->Add ();
   }
   if (Nodes != NULL && NodesPending == TRUE) {
      if (NodesUpdate == TRUE)
         Nodes->Update ();
      else
         Nodes->Add ();
   }
   if (Menu != NULL && MenuPending == TRUE)
      Menu->Save (MenuName);
   if (Cfg != NULL)
      Cfg->Save ();

   printf ("\r\n * Done\n\n");
}

/*
void main (int argc, char *argv[])
{
   FILE *fp;
   class TMenu *Menu;

   if ((Menu = new TMenu) != NULL) {
      Menu->Load (argv[1]);
      if ((fp = fopen ("m.cfg", "at")) != NULL) {
         fprintf (fp, "\ndefine menu %s\n", argv[1]);
         fprintf (fp, "   text color    %d\n", Menu->PromptColor);
         fprintf (fp, "   hilight color %d\n", Menu->PromptHilight);
         fprintf (fp, "   prompt        \"%s\"\n\n", Menu->Prompt);

         if (Menu->First () == TRUE)
            do {
               fprintf (fp, "   item   %s \"%s\" %d %d %d %s\n", Menu->Key, Menu->Display, Menu->Color, Menu->Hilight, Menu->Command, Menu->Argument);
            } while (Menu->Next () == TRUE);
         fclose (fp);
      }
      delete Menu;
   }
}
*/
