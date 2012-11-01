
#include "_ldefs.h"
#include "config.h"
#include "files.h"
#include "menu.h"
#include "msgbase.h"
#include "msgdata.h"
#include "schedule.h"
#include "user.h"
#include "version.h"

typedef struct {
   PSZ    Text;
   USHORT Id;
} ITEMDATA;

CHAR Temp[512], Translate[128], File[32];
USHORT FirstMsg = TRUE, FirstFile = TRUE, FirstChannel = TRUE, FirstLimit = TRUE;
USHORT FirstEvent = TRUE;
ULONG Line;

ITEMDATA Action[] = {
   "Chg_Alias", MNU_CHGALIAS,
   "Chg_Hotkeys", MNU_CHGHOTKEYS,
   "Chg_Length", MNU_CHGLENGTH,
   "Chg_More", MNU_CHGMORE,
   "Chg_Password", MNU_CHGPASSWORD,
   "Chg_Phone", MNU_CHGPHONE,
   "Chg_RIP", MNU_CHGRIP,
   "Chg_Video", MNU_CHGVIDEO,
   "Display_File", MNU_DISPLAY,
   "Display_Menu", MNU_GOTO,
   "Edit_Abort", MNU_EDITABORT,
   "Edit_Continue", MNU_EDITCONTINUE,
   "Edit_Delete", MNU_EDITDELETE,
   "Edit_Edit", MNU_EDITEDIT,
   "Edit_Insert", MNU_EDITINSERT,
   "Edit_List", MNU_EDITLIST,
   "Edit_Save", MNU_EDITSAVE,
   "Edit_Subj", MNU_EDITSUBJECT,
   "Edit_To", MNU_EDITTO,
   "File_Area", MNU_FILEAREA,
   "File_Download", MNU_DOWNLOAD,
   "File_Kill", MNU_FILEKILL,
   "File_Locate", MNU_FILENAMELOCATE,
   "File_Newfiles", MNU_NEWFILES,
   "File_NextArea", MNU_FILENEXTAREA,
   "File_PrevArea", MNU_FILEPREVAREA,
   "File_Tag", MNU_FILETAG,
   "File_Titles", MNU_FILETITLES,
   "File_Upload", MNU_UPLOAD,
   "Finger", MNU_FINGER,
   "Ftp", MNU_FTP,
   "Goodbye", MNU_LOGOFF,
   "Gosub_Menu", MNU_GOSUB,
   "Goto_Menu", MNU_GOTO,
   "Link_Menu", MNU_GOSUB,
   "Mail_Read", MNU_MAILREAD,
   "Msg_Area", MNU_MSGAREA,
   "Msg_Current", MNU_READCURRENT,
   "Msg_Download_BW", MNU_DOWNLOADBW,
   "Msg_Download_QWK", MNU_DOWNLOADQWK,
   "Msg_Enter", MNU_MSGENTER,
   "Msg_Forward", MNU_MSGFORWARD,
   "Msg_Kill", MNU_MSGDELETE,
   "Msg_Kludges", MNU_MSGKLUDGES,
   "Msg_List", MNU_MSGLIST,
   "Msg_ListTag", MNU_MSGVIEWTAG,
   "Msg_NextArea", MNU_MSGNEXTAREA,
   "Msg_PrevArea", MNU_MSGPREVAREA,
   "Msg_Tag", MNU_MSGTAG,
   "Press_Enter", MNU_PRESSENTER,
   "Read_Individual", MNU_MSGINDIVIDUAL,
   "Read_Msg", MNU_READMSG,
   "Read_Next", MNU_READNEXT,
   "Read_Nonstop", MNU_READNONSTOP,
   "Read_Previous", MNU_READPREVIOUS,
   "Return", MNU_RETURN,
   "Same_Direction", MNU_SAMEDIRECTION,
   "Telnet", MNU_TELNET,
   "Version", MNU_VERSION,
   "Who_Is_On", MNU_WHOISON,
   "Xtern_Run", MNU_RUN,
   NULL, 0
};

PSZ TranslateIn (PSZ pszText)
{
   UCHAR c, a;
   PSZ Src, Dst;

   Src = pszText;
   Dst = Translate;
   while ((c = *Src++) != '\0') {
      if (c == '\\') {
         a = *Src++;
         if (a == '\\')
            *Dst++ = (CHAR)a;
         else if (a == 'n')
            *Dst++ = '\n';
         else if (a == 'r')
            *Dst++ = '\r';
         else if (a == 'a')
            *Dst++ = '\a';
         else if (a == 'x') {
            a = *Src++;
            a -= 0x30;
            if (a > 9)
               a -= 7;
            c = *Src++;
            c -= 0x30;
            if (c > 9)
               c -= 7;
            c |= (UCHAR)(a << 4);
            *Dst++ = (CHAR)c;
         }
      }
      else
         *Dst++ = (CHAR)c;
   }
   *Dst = '\0';
   return (Translate);
}

PSZ GetNextKeyword (PSZ pszFrom, PSZ pszSeparator)
{
   PSZ RetVal;

   if ((RetVal = strtok (pszFrom, pszSeparator)) == NULL)
      cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, "");

   return (RetVal);
}

VOID DefineMenu (FILE *fp, PSZ pszName)
{
   int fd;
   USHORT i, HdrWritten = FALSE, NoDsp, More;
   PSZ p, p1, s;
   UCHAR ItemColor, ItemHilight;
   MENUHDR Hdr;
   ITEM Item;
   class TConfig *Cfg;
   class TLimits *Limits;

   if ((Cfg = new TConfig (".\\")) != NULL)
      Cfg->Read (0);

   Limits = new TLimits (Cfg->SystemPath);

   memset (&Hdr, 0, sizeof (Hdr));
   Hdr.cbSize = sizeof (Hdr);

   ItemColor = Hdr.ucColor = 0x0A;
   ItemHilight = Hdr.ucHilight = 0x0B;

   s = pszName;
   while ((p = strchr (s, '\\')) != NULL)
      s = p + 1;
   strcpy (Hdr.szMenuName, s);
   cprintf ("\rCompiling Menu: %-32s", strupr (Hdr.szMenuName));

   if (Cfg == NULL || strchr (pszName, '\\') != NULL)
      sprintf (Temp, "%s.Mnu", pszName);
   else
      sprintf (Temp, "%s%s.Mnu", Cfg->MenuPath, Hdr.szMenuName);

   if ((fd = open (Temp, O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE)) != -1) {
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Line++;
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (*p == '%' || *p == ';')
               continue;
            else if (!stricmp (p, "End")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Menu"))
                     break;
               }
            }
            else if (!stricmp (p, "MenuColour") || !stricmp (p, "MenuColor")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  ItemColor = Hdr.ucColor = (UCHAR)atoi (p);
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     ItemHilight = Hdr.ucHilight = (UCHAR)atoi (p);
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     ItemColor = (UCHAR)atoi (p);
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     ItemHilight = (UCHAR)atoi (p);
               }
            }
            else if (!stricmp (p, "Prompt")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (Hdr.szPrompt, TranslateIn (p));
               }
            }
            else {
               if (HdrWritten == FALSE) {
                  write (fd, &Hdr, sizeof (Hdr));
                  HdrWritten = TRUE;
               }

               memset (&Item, 0, sizeof (Item));
               Item.cbSize = sizeof (Item);
               Item.ucColor = ItemColor;
               Item.ucHilight = ItemHilight;

               NoDsp = FALSE;

               do {
                  More = FALSE;
                  if (!stricmp (p, "NoDsp")) {
                     NoDsp = TRUE;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
                  else if (!stricmp (p, "NoRIP")) {
                     Item.Flags |= MF_DSPNORIP;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
                  else if (!stricmp (p, "RIP")) {
                     Item.Flags |= MF_DSPRIP;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
                  else if (!stricmp (p, "Local")) {
                     Item.Flags |= MF_DSPLOCAL;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
                  else if (!stricmp (p, "Echo")) {
                     Item.Flags |= MF_DSPECHOMAIL;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
                  else if (!stricmp (p, "Matrix") || !stricmp (p, "NetMail")) {
                     Item.Flags |= MF_DSPNETMAIL;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
                  else if (!stricmp (p, "UsrRemote")) {
                     Item.Flags |= MF_DSPUSRREMOTE;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
                  else if (!stricmp (p, "UsrLocal")) {
                     Item.Flags |= MF_DSPUSRLOCAL;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
                  else if (!stricmp (p, "Exec")) {
                     Item.Flags |= MF_AUTOEXEC;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
                  else if (!stricmp (p, "OnEnter")) {
                     Item.Flags |= MF_ONENTER;
                     p = GetNextKeyword (NULL, " ");
                     More = TRUE;
                  }
               } while (More == TRUE && p != NULL);

               if (p != NULL) {
                  i = 0;
                  while (Action[i].Text != NULL) {
                     if (!stricmp (Action[i].Text, p)) {
                        Item.usCommand = Action[i].Id;
                        break;
                     }
                     i++;
                  }

                  if ((p1 = GetNextKeyword (NULL, "")) != NULL) {
                     while (*p1 == ' ')
                        p1++;

                     if ((s = strchr (p1, '\"')) != NULL) {
                        *s++ = '\0';
                        if ((p = strchr (s, '\"')) != NULL)
                           *p = '\0';
                        strcpy (Item.szDisplay, TranslateIn (s));

                        s -= 2;
                        while (*s == ' ')
                           *s-- = '\0';
                        while (*s != ' ')
                           s--;
                        *s++ = '\0';

                        if (isdigit (*s) || Limits == NULL)
                           Item.usLevel = (USHORT)atoi (s);
                        else {
                           if (Limits->Read (s) == TRUE) {
                              Item.usLevel = Limits->Level;
                              Item.ulAccessFlags = Limits->Flags;
                              Item.ulDenyFlags = Limits->DenyFlags;
                           }
                        }

                        s -= 2;
                        while (s > p1 && *s == ' ')
                           *s-- = '\0';
                        if (s > p1)
                           strcpy (Item.szArgument, p1);

                        if ((p = strchr (Item.szDisplay, '^')) != NULL) {
                           p++;
                           s = Item.szKey;
                           if (*p == '^')
                              *s++ = *p++;
                           while (*p != '^')
                              *s++ = *p++;
                           *s = '\0';
                        }

                        if (NoDsp == TRUE)
                           memset (Item.szDisplay, 0, sizeof (Item.szDisplay));

                        write (fd, &Item, sizeof (Item));
                        Hdr.usItems++;
                     }
                  }
               }
            }
         }
      }

      if (HdrWritten == TRUE) {
         lseek (fd, 0L, SEEK_SET);
         write (fd, &Hdr, sizeof (Hdr));
      }

      close (fd);
   }

   if (Limits != NULL)
      delete Limits;

   if (Cfg != NULL)
      delete Cfg;
}

VOID DefineAccess (FILE *fp, PSZ pszKey)
{
   PSZ p;
   class TLimits *Limit;

   cprintf ("\rCompiling Access: %-32s", pszKey);

   if (FirstLimit == TRUE) {
      unlink ("limits.dat");
      FirstLimit = FALSE;
   }

   if ((Limit = new TLimits (".\\")) != NULL) {
      Limit->Clear ();
      strcpy (Limit->Key, pszKey);
      strcpy (Limit->Description, pszKey);
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Line++;
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (*p == '%' || *p == ';')
               continue;
            else if (!stricmp (p, "End")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Access")) {
                     Limit->Add ();
                     break;
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Desc")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (Limit->Description, p);
               }
            }
            else if (!stricmp (p, "Level")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL)
                  Limit->Level = (USHORT)atoi (p);
            }
            else if (!stricmp (p, "Time")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL)
                  Limit->CallTimeLimit = (USHORT)atoi (p);
            }
            else if (!stricmp (p, "Cume")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL)
                  Limit->DayTimeLimit = (USHORT)atoi (p);
            }
            else if (!stricmp (p, "FileLimit")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL)
                  Limit->CallDownloadLimit = (USHORT)atoi (p);
            }
            else
               cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
         }
      }
      delete Limit;
   }
}

VOID DefineArea (FILE *fp, PSZ pszKey)
{
   USHORT isFile = FALSE, isMsg = FALSE;
   PSZ p;
   class TMsgData *Msg;
   class FileData *File;
   class MsgBase *Base;
   class TFile *Data;

   cprintf ("\rCompiling Area: %-15s", strupr (pszKey));

   Msg = new TMsgData (".\\");
   File = new FileData (".\\");

   if (Msg != NULL && File != NULL) {
      Msg->New ();
      File->Reset ();
      strcpy (Msg->Key, pszKey);
      strcpy (File->Key, pszKey);
      Msg->ShowGlobal = File->ShowGlobal = TRUE;
      Msg->Offline = TRUE;

      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Line++;
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (*p == '%' || *p == ';')
               continue;
            else if (!stricmp (p, "End")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Area")) {
                     if (isMsg == TRUE && FirstMsg == TRUE) {
                        unlink ("msg.dat");
                        unlink ("msg.idx");
                        FirstMsg = FALSE;
                     }
                     if (isFile == TRUE && FirstFile == TRUE) {
                        unlink ("file.dat");
                        unlink ("file.idx");
                        FirstFile = FALSE;
                     }
                     if (isFile == TRUE) {
                        if ((Data = new TFile (".\\", File->Key)) != NULL) {
                           if (Data->First () == TRUE)
                              do {
                                 File->ActiveFiles++;
                              } while (Data->Next () == TRUE);
                           delete Data;
                        }
                        File->Add ();
                     }
                     if (isMsg == TRUE) {
                        Base = NULL;
                        if (Msg->Storage == ST_JAM)
                           Base = new JAM (Msg->Path);
                        else if (Msg->Storage == ST_SQUISH)
                           Base = new SQUISH (Msg->Path);
                        if (Base != NULL) {
                           Msg->ActiveMsgs = Base->Number ();
                           delete Base;
                        }
                        Msg->Add ();
                     }
                     break;
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "MsgInfo")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (Msg->Display, p);
                  isMsg = TRUE;
               }
            }
            else if (!stricmp (p, "FileInfo")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (File->Display, p);
                  isFile = TRUE;
               }
            }
            else if (!stricmp (p, "MsgAccess")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  Msg->Level = (USHORT)atoi (p);
                  isMsg = TRUE;
               }
            }
            else if (!stricmp (p, "FileAccess")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  File->Level = (USHORT)atoi (p);
                  isFile = TRUE;
               }
            }
            else if (!stricmp (p, "Access")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  Msg->Level = File->Level = (USHORT)atoi (p);
               }
            }
            else if (!stricmp (p, "Conference") || !stricmp (p, "Local") || !stricmp (p, "EchoMail") || !stricmp (p, "Matrix")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  strcpy (Msg->Path, p);
                  isMsg = TRUE;
                  if (!stricmp (p, "EchoMail"))
                     Msg->EchoMail = TRUE;
               }
            }
            else if (!stricmp (p, "Origin")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  strcpy (Msg->Origin, p);
                  isMsg = TRUE;
               }
            }
            else if (!stricmp (p, "MsgName")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  strcpy (Msg->EchoTag, p);
                  isMsg = TRUE;
               }
            }
            else if (!stricmp (p, "Upload")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (p[strlen (p) - 1] != '\\')
                     strcat (p, "\\");
                  strcpy (File->Upload, p);
                  isFile = TRUE;
               }
            }
            else if (!stricmp (p, "Download")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (p[strlen (p) - 1] != '\\')
                     strcat (p, "\\");
                  strcpy (File->Download, p);
                  isFile = TRUE;
               }
            }
            else if (!stricmp (p, "Type")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Jam"))
                     Msg->Storage = ST_JAM;
                  else if (!stricmp (p, "Squish"))
                     Msg->Storage = ST_SQUISH;
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "EchoMail")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Feeder")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        Msg->Feeder.Clear ();
                        Msg->Feeder.Add (p);
                     }
                  }
                  else if (!stricmp (p, "Link")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        do {
                           Msg->Forward.Add (p);
                        } while ((p = strtok (NULL, " ")) != NULL);
                     }
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
               }
            }
            else
               cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
         }
      }
   }

   if (Msg != NULL)
      delete Msg;
   if (File != NULL)
      delete File;
}

VOID SystemSection (FILE *fp)
{
   USHORT i, FirstAddress = TRUE;
   PSZ p, p1;
   class TConfig *Cfg;

   if ((Cfg = new TConfig (".\\")) != NULL) {
      Cfg->Clear ();
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Line++;
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (*p == '%' || *p == ';')
               continue;
            else if (!stricmp (p, "End")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "System")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (!stricmp (p, "Section"))
                           break;
                        else
                           cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
                     }
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Sysop")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (Cfg->SysopName, p);
               }
            }
            else if (!stricmp (p, "Name")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (Cfg->SystemName, p);
               }
            }
            else if (!stricmp (p, "Location")) {
               if ((p = GetNextKeyword (NULL, "")) != NULL) {
                  while (*p == ' ')
                     p++;
                  strcpy (Cfg->CityState, p);
               }
            }
            else if (!stricmp (p, "Path")) {
               if ((p1 = GetNextKeyword (NULL, " ")) != NULL) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                     if (stricmp (p1, "Sent")) {
                        if (p[strlen (p) - 1] != '\\')
                           strcat (p, "\\");
                     }
                     if (!stricmp (p1, "Home"))
                        strcpy (Cfg->HomePath, p);
                     else if (!stricmp (p1, "EMail"))
                        strcpy (Cfg->MailSpool, p);
                     else if (!stricmp (p1, "Misc"))
                        strcpy (Cfg->MiscPath, p);
                     else if (!stricmp (p1, "Outbound"))
                        strcpy (Cfg->Outbound, p);
                     else if (!stricmp (p1, "Inbound")) {
                        strcpy (Cfg->Inbound, p);
                        strcpy (Cfg->InboundProt, p);
                        strcpy (Cfg->InboundKnown, p);
                     }
                     else if (!stricmp (p1, "System"))
                        strcpy (Cfg->SystemPath, p);
                     else if (!stricmp (p1, "Sent") && !stricmp (p, "Mail")) {
                        if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                           if (p[strlen (p) - 1] != '\\')
                              strcat (p, "\\");
                           strcpy (Cfg->SentMail, p);
                        }
                     }
                  }
               }
            }
            else if (!stricmp (p, "Menu")) {
               if ((p1 = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p1, "Path")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (p[strlen (p) - 1] != '\\')
                           strcat (p, "\\");
                        strcpy (Cfg->MenuPath, p);
                     }
                  }
               }
            }
            else if (!stricmp (p, "First")) {
               if ((p1 = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p1, "Menu")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL)
                        strcpy (Cfg->FirstMenu, p);
                  }
               }
            }
            else if (!stricmp (p, "File")) {
               if ((p1 = GetNextKeyword (NULL, " ")) != NULL) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                     if (!stricmp (p1, "Password"))
                        strcpy (Cfg->UserFile, p);
                  }
               }
            }
            else if (!stricmp (p, "Log")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Path")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (p[strlen (p) - 1] != '\\')
                           strcat (p, "\\");
                        strcpy (Cfg->LogPath, p);
                     }
                  }
                  else if (!stricmp (p, "Mode")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (!stricmp (p, "Debug"))
                           Cfg->LogLevel = 0xFF;
                        else if (!stricmp (p, "Verbose"))
                           ;
                        else if (!stricmp (p, "Terse"))
                           ;
                        else
                           cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
                     }
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Address")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (FirstAddress == TRUE)
                     Cfg->Address.Clear ();
                  Cfg->Address.Add (p);
                  FirstAddress = FALSE;
               }
            }
            else if (!stricmp (p, "Start")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Ftp")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (!stricmp (p, "Server"))
                           Cfg->FtpServer = TRUE;
                        else
                           cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
                     }
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Event")) {
               class TScheduler *Event;

               if (FirstEvent == TRUE) {
                  unlink ("Events.Dat");
                  FirstEvent = FALSE;
               }

               if ((Event = new TScheduler (".\\")) != NULL) {
                  GetNextKeyword (NULL, " ");
                  if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                     Event->Hour = (UCHAR)atoi (p);
                     if ((p = strchr (p, ':')) != NULL)
                        Event->Minute = (UCHAR)atoi (p + 1);
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        i = (USHORT)(atoi (p) * 60);
                        if ((p = strchr (p, ':')) != NULL)
                           i += (USHORT)atoi (p + 1);
                        Event->Length = (USHORT)(i - (Event->Hour * 60 + Event->Minute));

                        while ((p = GetNextKeyword (NULL, " ")) != NULL) {
                           if (!strnicmp (p, "A=", 2))
                              Event->Delay = (USHORT)atoi (&p[2]);
                           else if (!strnicmp (p, "C=", 2)) {
                              Event->CallNode = TRUE;
                              strcpy (Event->Address, &p[2]);
                           }
                           else if (!stricmp (p, "F"))
                              Event->ForceCall = TRUE;
                           else if (!strnicmp (p, "T=", 2)) {
                              Event->Failures = (USHORT)atoi (&p[2]);
                              if ((p = strchr (p, ',')) != NULL)
                                 Event->Retries = (USHORT)atoi (p + 1);
                           }
                        }

                        Event->Add ();
                     }
                  }
                  delete Event;
               }
            }
            else
               cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
         }
      }
      Cfg->Write (0);
      delete Cfg;
   }
}

VOID SessionSection (FILE *fp)
{
   PSZ p;
   class TConfig *Cfg;

   if ((Cfg = new TConfig (".\\")) != NULL) {
      if (Cfg->Read (0) == FALSE)
         Cfg->Clear ();
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Line++;
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (*p == '%' || *p == ';')
               continue;
            else if (!stricmp (p, "End")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Session")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (!stricmp (p, "Section"))
                           break;
                        else
                           cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
                     }
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "First")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Message")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (!stricmp (p, "Area")) {
                           if ((p = GetNextKeyword (NULL, " ")) != NULL)
                              strcpy (Cfg->FirstMessageArea, p);
                        }
                     }
                  }
                  else if (!stricmp (p, "File")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (!stricmp (p, "Area")) {
                           if ((p = GetNextKeyword (NULL, " ")) != NULL)
                              strcpy (Cfg->FirstFileArea, p);
                        }
                     }
                  }
               }
            }
            else
               cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
         }
      }
      Cfg->Write (0);
      delete Cfg;
   }
}

VOID ReaderSection (FILE *fp)
{
   PSZ p;
   class TConfig *Cfg;

   if ((Cfg = new TConfig (".\\")) != NULL) {
      if (Cfg->Read (0) == 0)
         Cfg->Clear ();
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Line++;
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (*p == '%' || *p == ';')
               continue;
            else if (!stricmp (p, "End")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Reader")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (!stricmp (p, "Section"))
                           break;
                        else
                           cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
                     }
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Phone")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Number")) {
                     if ((p = GetNextKeyword (NULL, "")) != NULL) {
                        while (*p == ' ')
                           p++;
                        strcpy (Cfg->PhoneNumber, p);
                     }
                  }
               }
            }
            else if (!stricmp (p, "Packet")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Name")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL)
                        strcpy (Cfg->BbsId, p);
                  }
               }
            }
            else if (!stricmp (p, "Max")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Messages")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL)
                        Cfg->MaxOfflineMsgs = (USHORT)atoi (p);
                  }
               }
            }
            else if (!stricmp (p, "Work")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Directory")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                        if (p[strlen (p) - 1] != '\\')
                           strcat (p, "\\");
                        strcpy (Cfg->TempPath, p);
                     }
                  }
               }
            }
            else
               cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
         }
      }
      Cfg->Write (0);
      delete Cfg;
   }
}

VOID ChannelSection (FILE *fp, USHORT usChannel)
{
   USHORT Init = 0;
   UCHAR Ask = Q_YES;
   PSZ p, p1;
   class TConfig *Cfg;

   if ((Cfg = new TConfig (".\\")) != NULL) {
      Cfg->Clear ();
      Cfg->Read (0);
      if (FirstChannel == TRUE) {
         unlink ("channels.dat");
         Cfg->Write (0);
         FirstChannel = FALSE;
      }
      if (Cfg->Read (usChannel) == FALSE) {
         Cfg->ClearChannel ();
         Cfg->Write (usChannel);
         Cfg->Read (usChannel);
      }
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Line++;
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (*p == '%' || *p == ';')
               continue;
            else if (!stricmp (p, "End")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Channel"))
                     break;
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Type")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Modem")) {
                     Cfg->Type = CH_MODEM;
                     Cfg->MaxChannels = 1;
                  }
                  else if (!stricmp (p, "Telnet")) {
                     Cfg->Type = CH_TELNET;
                     Cfg->MaxChannels = 16;
                  }
                  else if (!stricmp (p, "Local")) {
                     Cfg->Type = CH_LOCAL;
                     Cfg->MaxChannels = 1;
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Max")) {
               if (Cfg->Type == CH_TELNET) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                     if (!stricmp (p, "Connections")) {
                        if ((p = GetNextKeyword (NULL, " ")) != NULL)
                           Cfg->MaxChannels = (USHORT)atoi (p);
                     }
                     else
                        cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
                  }
               }
               else
                  cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
            }
            else if (!stricmp (p, "Output")) {
               if (Cfg->Type == CH_MODEM) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     strcpy (Cfg->Device, p);
               }
               else
                  cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
            }
            else if (!stricmp (p, "Baud")) {
               if (Cfg->Type == CH_MODEM) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                     if (!stricmp (p, "Maximum")) {
                        if ((p = GetNextKeyword (NULL, " ")) != NULL)
                           Cfg->Speed = atol (p);
                     }
                     else
                        cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
                  }
               }
               else
                  cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
            }
            else if (!stricmp (p, "Init")) {
               if (Cfg->Type == CH_MODEM && Init < 3) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     strcpy (Cfg->Initialize[Init++], p);
               }
               else
                  cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
            }
            else if (!stricmp (p, "Answer")) {
               if (Cfg->Type == CH_MODEM) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     strcpy (Cfg->Answer, p);
               }
               else
                  cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
            }
            else if (!stricmp (p, "Hangup")) {
               if (Cfg->Type == CH_MODEM) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     strcpy (Cfg->Hangup, p);
               }
               else
                  cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
            }
            else if (!stricmp (p, "Busy")) {
               if (Cfg->Type == CH_MODEM) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     strcpy (Cfg->OffHook, p);
               }
               else
                  cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
            }
            else if (!stricmp (p, "Dial")) {
               if (Cfg->Type == CH_MODEM) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     strcpy (Cfg->Dial, p);
               }
               else
                  cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
            }
            else if (!stricmp (p, "Port")) {
               if (Cfg->Type == CH_TELNET) {
                  if ((p = GetNextKeyword (NULL, " ")) != NULL)
                     Cfg->Port = (USHORT)atoi (p);
               }
               else
                  cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
            }
            else if (!stricmp (p, "Logon")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Level")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL)
                        Cfg->NewLevel = (USHORT)atoi (p);
                  }
                  else if (!stricmp (p, "Class")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL)
                        strcpy (Cfg->NewLimitClass, strupr (p));
                  }
                  else if (!stricmp (p, "TimeLimit")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL)
                        Cfg->LoginTime = (USHORT)atoi (p);
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Input")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Timeout")) {
                     if ((p = GetNextKeyword (NULL, " ")) != NULL)
                        Cfg->InactivityTime = (USHORT)atoi (p);
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Ask")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  Ask = Q_YES;
                  if ((p1 = strtok (NULL, " ")) != NULL) {
                     if (!stricmp (p1, "Required"))
                        Ask = Q_REQUIRED;
                     else
                        cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p1);
                  }
                  if (!stricmp (p, "RealName"))
                     Cfg->AskRealName = Ask;
                  else if (!stricmp (p, "Company"))
                     Cfg->AskCompany = Ask;
                  else if (!stricmp (p, "Address"))
                     Cfg->AskAddress = Ask;
                  else if (!stricmp (p, "City"))
                     Cfg->AskZipCity = Ask;
                  else if (!stricmp (p, "State"))
                     Cfg->AskState = Ask;
                  else if (!stricmp (p, "Phone"))
                     Cfg->AskPhone = Ask;
                  else if (!stricmp (p, "Sex"))
                     Cfg->AskSex = Ask;
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
               }
            }
            else
               cprintf ("Unknown statement in line %lu of '%s': '%s'\a\r\n", Line, File, p);
         }
      }
      Cfg->Write (usChannel);
      delete Cfg;
   }
}

USHORT CompileFile (PSZ pszFile)
{
   FILE *fp;
   USHORT RetVal = FALSE, NewLine = FALSE;
   ULONG MyCount = 0;
   PSZ p;

   strcpy (File, pszFile);
   Line = 0L;

   if ((fp = fopen (pszFile, "rt")) != NULL) {
      cprintf ("Parsing '%s':\r\n", pszFile);
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Line++;
         MyCount++;
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (*p == '%' || *p == ';')
               continue;
            else if (!stricmp (p, "System")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Section")) {
                     cprintf ("Compiling Section: System\r\n");
                     SystemSection (fp);
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Session")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Section")) {
                     cprintf ("Compiling Section: Session\r\n");
                     SessionSection (fp);
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Reader")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  if (!stricmp (p, "Section")) {
                     cprintf ("Compiling Section: Reader\r\n");
                     ReaderSection (fp);
                  }
                  else
                     cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
               }
            }
            else if (!stricmp (p, "Channel")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL)
                  ChannelSection (fp, (USHORT)atoi (p));
            }
            else if (!stricmp (p, "Area")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  DefineArea (fp, p);
                  NewLine = TRUE;
               }
            }
            else if (!stricmp (p, "Access")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  DefineAccess (fp, p);
                  NewLine = TRUE;
               }
            }
            else if (!stricmp (p, "Menu")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  DefineMenu (fp, p);
                  NewLine = TRUE;
               }
            }
            else if (!stricmp (p, "Include")) {
               if ((p = GetNextKeyword (NULL, " ")) != NULL) {
                  CompileFile (p);
                  Line = MyCount;
               }
            }
            else
               cprintf ("Unknown statement in line %lu of '%s': '%s'\r\n", Line, File, p);
         }
      }
      fclose (fp);
      RetVal = TRUE;
   }
   else
      cprintf ("Parsing '%s': Fatal error opening '%s' for read!\a\r\n", pszFile, pszFile);

   if (NewLine == TRUE)
      cprintf ("\r\n");

   return (RetVal);
}

void main (void)
{
   cprintf ("\r\nLoraBBS Professional Edition Configuration File Compiler - Version %s.\r\n", VERSION);
   cprintf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\r\n\r\n");

   Line = 0;
   if (CompileFile ("Lora.Ctl") == TRUE)
      cprintf ("\r\nDone!\r\n");
}



