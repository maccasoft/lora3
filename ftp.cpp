
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.6
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    13/06/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "lserver.h"

TFTP::TFTP (void)
{
   Cfg = NULL;
   Tcp = Data = NULL;
   User = NULL;
   Nodes = NULL;
   Files = NULL;
   Outbound = NULL;

   Binary = TRUE;
   Valid = Anonymous = FALSE;
   Known = Protected = FALSE;
   DataPort = 0;

   _dos_getdate (&date);
   _dos_gettime (&d_time);

   Month[0] = "Jan";
   Month[1] = "Feb";
   Month[2] = "Mar";
   Month[3] = "Apr";
   Month[4] = "May";
   Month[5] = "Jun";
   Month[6] = "Jul";
   Month[7] = "Aug";
   Month[8] = "Sep";
   Month[9] = "Oct";
   Month[10] = "Nov";
   Month[11] = "Dec";
}

TFTP::~TFTP (void)
{
   if (Tcp != NULL)
      delete Tcp;
   if (User != NULL)
      delete User;
   if (Files != NULL)
      delete Files;
   if (Nodes != NULL)
      delete Nodes;
   if (Outbound != NULL)
      delete Outbound;
   if (Cfg != NULL)
      delete Cfg;
}

VOID TFTP::GetCommand (VOID)
{
   USHORT len = 0, MaxLen = sizeof (Response);
   CHAR c, *pszResp = Response;

   do {
      if (Tcp->BytesReady () == TRUE) {
         do {
            if ((c = (CHAR)Tcp->ReadByte ()) != '\r') {
               if (c != '\n') {
                  *pszResp++ = c;
                  if (++len >= MaxLen)
                     c = '\r';
               }
            }
         } while (Tcp->RxBytes > 0 && c != '\r');
      }
#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   } while (c != '\r' && Tcp->Carrier () == TRUE);

   *pszResp = '\0';
}

VOID TFTP::ReadFile (PSZ pszCode, PSZ pszFile)
{
   FILE *fp;

   if ((fp = fopen (pszFile, "rt")) != NULL) {
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         Tcp->BufferBytes ((UCHAR *)pszCode, (USHORT)strlen (pszCode));
         Tcp->BufferByte ('-');
         Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)(strlen (Temp) - 1));
         Tcp->BufferBytes ((UCHAR *)"\r\n", 2);
      }
      fclose (fp);

      Tcp->BufferBytes ((UCHAR *)pszCode, (USHORT)strlen (pszCode));
      Tcp->BufferByte (' ');
      Tcp->BufferBytes ((UCHAR *)"\r\n", 2);

      Tcp->UnbufferBytes ();
   }
}

USHORT TFTP::FileExist (PSZ pszFile)
{
   int fd;

   if ((fd = open (pszFile, O_RDONLY)) != -1) {
      close (fd);
      return (TRUE);
   }

   return (FALSE);
}

VOID TFTP::Run (VOID)
{
   int fd;
   USHORT Readed, Ok, GotA;
   CHAR *p, CurPath[32];

   strcpy (CurPath, "/");
   EndRun = FALSE;

   while (Tcp->Carrier () == TRUE && EndRun == FALSE) {
//   while (EndRun == FALSE) {
      GetCommand ();
      if ((p = strtok (Response, " ")) != NULL) {
         if (!stricmp (p, "QUIT")) {
            if (FileExist ("logoff.ftp")) {
               sprintf (Temp, "221-Goodbye.\r\n");
               Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               ReadFile ("221", "logoff.ftp");
            }
            else {
               sprintf (Temp, "221 Goodbye.\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
            EndRun = TRUE;
         }
         else if (!stricmp (p, "TYPE")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (!stricmp (p, "A")) {
                  sprintf (Temp, "200 Type set to A\r\n");
                  Binary = FALSE;
               }
               else if (!stricmp (p, "I")) {
                  sprintf (Temp, "200 Type set to I\r\n");
                  Binary = TRUE;
               }
               else
                  sprintf (Temp, "503 Command not implemented.\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            }
         }
         else if (!stricmp (p, "PORT")) {
            ClientIP[0] = '\0';
            if ((p = strtok (NULL, ",")) != NULL) {
               strcat (ClientIP, p);
               strcat (ClientIP, ".");
            }
            if ((p = strtok (NULL, ",")) != NULL) {
               strcat (ClientIP, p);
               strcat (ClientIP, ".");
            }
            if ((p = strtok (NULL, ",")) != NULL) {
               strcat (ClientIP, p);
               strcat (ClientIP, ".");
            }
            if ((p = strtok (NULL, ",")) != NULL)
               strcat (ClientIP, p);

            DataPort = 0;
            if ((p = strtok (NULL, ",")) != NULL)
               DataPort = (USHORT)(atoi (p) << 8);
            if ((p = strtok (NULL, ",")) != NULL)
               DataPort |= (USHORT)atoi (p);

            sprintf (Temp, "200 Port command successfull.\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (!stricmp (p, "LIST")) {
            p = strtok (NULL, " ");
            if ((Data = new TTcpip) != NULL) {
               sprintf (Temp, "150 Opening ASCII mode data connection for /bin/ls.\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

               if (Data->ConnectServer (ClientIP, DataPort) == TRUE) {
                  if (Nodes != NULL) {
                     if (Outbound != NULL) {
                        if (Outbound->First () == TRUE)
                           do {
                              sprintf (Temp, "-rw-a--      0 0        0        %8lu %s %2d %02d:%02d %s\r\n", Outbound->Size, Month[date.month - 1], date.day, d_time.hour, d_time.minute, strlwr (Outbound->Name));
                              Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           } while (Outbound->Next () == TRUE);
                     }
                  }
                  else if (Files == NULL) {
                     if (p != NULL) {
                        if ((Area = new TFileData (Cfg->SystemPath)) != NULL) {
                           if (Area->First () == TRUE)
                              do {
                                 if (!stricmp (p, Area->Key)) {
                                    if ((Files = new TFileBase (Cfg->SystemPath, Area->Key)) != NULL) {
                                       Files->SortByName ();
                                       if (Files->First () == TRUE)
                                          do {
                                             sprintf (Temp, "-rw-a--      0 0        0               0 %s %2d %02d:%02d %s\r\n", Files->Size, Month[Files->UplDate.Month - 1], Files->UplDate.Day, Files->UplDate.Hour, Files->UplDate.Minute, strlwr (Files->Name));
                                             Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                                          } while (Files->Next () == TRUE);
                                       delete Files;
                                       Files = NULL;
                                    }
                                 }
                              } while (Area->Next () == TRUE);
                           delete Area;
                        }
                     }
                     else {
                        if ((Area = new TFileData (Cfg->SystemPath)) != NULL) {
                           if (Area->First () == TRUE)
                              do {
                                 sprintf (Temp, "drw----      0 0        0               0 %s %2d %02d:%02d %s\r\n", Month[date.month - 1], date.day, d_time.hour, d_time.minute, strlwr (Area->Key));
                                 Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              } while (Area->Next () == TRUE);
                           delete Area;
                        }
                     }
                  }
                  else {
                     if (Files->First () == TRUE)
                        do {
                           sprintf (Temp, "-rw-a--      0 0        0        %8lu %s %2d %02d:%02d %s\r\n", Files->Size, Month[Files->UplDate.Month - 1], Files->UplDate.Day, Files->UplDate.Hour, Files->UplDate.Minute, strlwr (Files->Name));
                           Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                        } while (Files->Next () == TRUE);
                  }
                  Data->UnbufferBytes ();
               }

               sprintf (Temp, "226 Transfer complete.\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               delete Data;
            }
         }
         else if (!stricmp (p, "NLST")) {
            p = strtok (NULL, " ");
            if ((Data = new TTcpip) != NULL) {
               sprintf (Temp, "150 Opening ASCII mode data connection for /bin/ls.\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

               if (Data->ConnectServer (ClientIP, DataPort) == TRUE) {
                  if (Nodes != NULL) {
                     if (Outbound != NULL) {
                        if (Outbound->First () == TRUE)
                           do {
                              sprintf (Temp, "%s\r\n", strlwr (Outbound->Name));
                              Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           } while (Outbound->Next () == TRUE);
                     }
                  }
                  else if (Files == NULL) {
                     if ((Area = new TFileData (Cfg->SystemPath)) != NULL) {
                        if (Area->First () == TRUE)
                           do {
                              if (p != NULL)
                                 sprintf (Temp, "drw----      0 0        0               0 %s %2d %02d:%02d %s\r\n", Month[date.month - 1], date.day, d_time.hour, d_time.minute, strlwr (Area->Key));
                              else
                                 sprintf (Temp, "%s/\r\n", strlwr (Area->Key));
                              Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           } while (Area->Next () == TRUE);
                        delete Area;
                     }
                  }
                  else {
                     if (Files->First () == TRUE)
                        do {
                           if (p != NULL)
                              sprintf (Temp, "-rw-a--      0 0        0        %lu %s %2d %02d:%02d %s\r\n", Files->Size, Month[Files->UplDate.Month - 1], Files->UplDate.Day, Files->UplDate.Hour, Files->UplDate.Minute, strlwr (Files->Name));
                           else
                              sprintf (Temp, "%s\r\n", strlwr (Files->Name));
                           Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                        } while (Files->Next () == TRUE);
                  }
                  Data->UnbufferBytes ();
               }

               sprintf (Temp, "226 Transfer complete.\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               delete Data;
            }
         }
         else if (!stricmp (p, "NOOP")) {
            sprintf (Temp, "200 NOOP command successfull\r\n", p);
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (!stricmp (p, "RETR")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (*p == '/')
                  p++;
               if ((User != NULL || Anonymous == TRUE) && strchr (p, '/') != NULL) {
                  if ((Area = new TFileData (Cfg->SystemPath)) != NULL) {
                     if (Area->First () == TRUE)
                        do {
                           if (!strnicmp (p, Area->Key, strlen (Area->Key)) && p[strlen (Area->Key)] == '/') {
                              if ((Files = new TFileBase (Cfg->SystemPath, Area->Key)) != NULL) {
                                 if (Files->Read (&p[strlen (Area->Key) + 1]) == TRUE) {
                                    if ((Data = new TTcpip) != NULL) {
                                       sprintf (Temp, "150 Opening %s mode data connection for %s (%ld bytes).\r\n", (Binary == TRUE) ? "BINARY" : "ASCII", Files->Name, Files->Size);
                                       Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

                                       if (Data->ConnectServer (ClientIP, DataPort) == TRUE) {
                                          if ((fd = open (Files->Complete, (Binary == TRUE) ? (O_RDONLY|O_BINARY) : (O_RDONLY|O_TEXT))) != -1) {
                                             do {
                                                Readed = (USHORT)read (fd, Temp, sizeof (Temp));
                                                Data->BufferBytes ((UCHAR *)Temp, Readed);
                                             } while (Readed == sizeof (Temp));
                                             close (fd);

                                             Files->DlTimes++;
                                             Files->ReplaceHeader ();
                                          }
                                          Data->UnbufferBytes ();
                                       }

                                       sprintf (Temp, "226 Transfer complete.\r\n");
                                       Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                                       delete Data;
                                    }
                                 }
                                 else {
                                    sprintf (Temp, "550 %s: no such file or directory.\r\n", p);
                                    Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                                 }
                                 delete Files;
                                 Files = NULL;
                              }
                           }
                        } while (Area->Next () == TRUE);
                     delete Area;
                  }
               }
               else if ((User != NULL || Anonymous == TRUE) && Files != NULL) {
                  if (Files->Read (p) == TRUE) {
                     if ((Data = new TTcpip) != NULL) {
                        sprintf (Temp, "150 Opening %s mode data connection for %s (%ld bytes).\r\n", (Binary == TRUE) ? "BINARY" : "ASCII", Files->Name, Files->Size);
                        Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

                        if (Data->ConnectServer (ClientIP, DataPort) == TRUE) {
                           sprintf (Temp, "%s%s", Area->Download, Files->Name);
                           if ((fd = open (Temp, (Binary == TRUE) ? (O_RDONLY|O_BINARY) : (O_RDONLY|O_TEXT))) != -1) {
                              do {
                                 Readed = (USHORT)read (fd, Temp, sizeof (Temp));
                                 Data->BufferBytes ((UCHAR *)Temp, Readed);
                              } while (Readed == sizeof (Temp));
                              close (fd);

                              Files->DlTimes++;
                              Files->ReplaceHeader ();
                           }
                           Data->UnbufferBytes ();
                        }

                        sprintf (Temp, "226 Transfer complete.\r\n");
                        Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                        delete Data;
                     }
                  }
                  else {
                     sprintf (Temp, "550 %s: no such file or directory.\r\n", p);
                     Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  }
               }
               else if (Nodes != NULL && Outbound != NULL) {
                  Ok = FALSE;
                  if (Outbound->First () == TRUE)
                     do {
                        if (!stricmp (Outbound->Name, p)) {
                           if ((Data = new TTcpip) != NULL) {
                              sprintf (Temp, "150 Opening %s mode data connection for %s (%ld bytes).\r\n", (Binary == TRUE) ? "BINARY" : "ASCII", Outbound->Name, Outbound->Size);
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

                              if (Data->ConnectServer (ClientIP, DataPort) == TRUE) {
                                 if ((fd = open (Outbound->Complete, (Binary == TRUE) ? (O_RDONLY|O_BINARY) : (O_RDONLY|O_TEXT))) != -1) {
                                    do {
                                       Readed = (USHORT)read (fd, Temp, sizeof (Temp));
                                       Data->BufferBytes ((UCHAR *)Temp, Readed);
                                    } while (Readed == sizeof (Temp));
                                    close (fd);
                                 }
                                 Data->UnbufferBytes ();
                              }

                              Ok = TRUE;

                              sprintf (Temp, "226 Transfer complete.\r\n");
                              Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              delete Data;
                           }
                        }
                     } while (Outbound->Next () == TRUE);

                  if (Ok == FALSE) {
                     sprintf (Temp, "550 %s: no such file or directory.\r\n", p);
                     Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                  }
               }
               else {
                  sprintf (Temp, "550 %s: no such file or directory.\r\n", p);
                  Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               }
            }
         }
         else if (!stricmp (p, "SYST")) {
            sprintf (Temp, "215 UNIX type:OS/2\r\n", p);
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (!stricmp (p, "STOR")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (Nodes != NULL) {
                  if ((Data = new TTcpip) != NULL) {
                     sprintf (Temp, "150 Opening %s mode data connection for %s.\r\n", (Binary == TRUE) ? "BINARY" : "ASCII", p);
                     Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

                     if (Data->ConnectServer (ClientIP, DataPort) == TRUE) {
                        if (Protected == TRUE)
                           sprintf (Temp, "%s%s", Cfg->ProtectedInbound, p);
                        else if (Known == TRUE)
                           sprintf (Temp, "%s%s", Cfg->KnownInbound, p);
                        else
                           sprintf (Temp, "%s%s", Cfg->NormalInbound, p);

                        if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1) {
                           close (fd);
                           if ((p = strchr (Temp, '\0')) != NULL) {
                              p--;
                              GotA = FALSE;
                              do {
                                 if (isdigit (*p) || GotA == TRUE) {
                                    if (*p == '9') {
                                       GotA = TRUE;
                                       *p = 'A';
                                    }
                                    else
                                       *p = (char)(*p + 1);
                                 }
                                 else
                                    *p = '0';
                                 if ((fd = open (Temp, O_RDONLY|O_BINARY)) != -1)
                                    close (fd);
                              } while (fd != -1);
                           }
                        }

                        if ((fd = open (Temp, (Binary == TRUE) ? (O_WRONLY|O_BINARY|O_CREAT|O_TRUNC) : (O_WRONLY|O_TEXT|O_CREAT|O_TRUNC), S_IREAD|S_IWRITE)) != -1) {
                           do {
                              while (Data->BytesReady ()) {
                                 Readed = Data->ReadBytes ((UCHAR *)Temp, (USHORT)sizeof (Temp));
                                 write (fd, Temp, Readed);
                              }
#if defined(__OS2__)
                              DosSleep (1L);
#elif defined(__NT__)
                              Sleep (1L);
#endif
                           } while (Data->Carrier () == TRUE);
                           close (fd);
                        }
                     }

                     sprintf (Temp, "226 Transfer complete.\r\n");
                     Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                     delete Data;
                  }
               }
               else {
                  sprintf (Temp, "503 Command not implemented.\r\n");
                  Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               }
            }
         }
         else if (!stricmp (p, "CWD")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (User != NULL || Anonymous == TRUE) {
                  if (Files != NULL) {
                     if (!stricmp (p, "..")) {
                        delete Files;
                        Files = NULL;
                        sprintf (Temp, "250 CWD command successfull.\r\n");
                        strcpy (CurPath, "/");
                     }
                     else
                        sprintf (Temp, "550 %s: no such file or directory.\r\n", p);
                  }
                  else {
                     if ((Area = new TFileData (Cfg->SystemPath)) != NULL) {
                        if (Area->Read (p) == TRUE) {
                           if (Files != NULL)
                              delete Files;
                           if ((Files = new TFileBase (Cfg->SystemPath, Area->Key)) != NULL)
                              Files->SortByName ();
                           sprintf (Temp, "250 CWD command successfull.\r\n");
                           strcpy (CurPath, p);
                        }
                        else
                           sprintf (Temp, "550 %s: no such file or directory.\r\n", p);
                     }
                  }
                  Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               }
               else {
                  sprintf (Temp, "550 %s: no such file or directory.\r\n", p);
                  Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               }
            }
         }
         else if (!stricmp (p, "CDUP")) {
            if (User != NULL || Anonymous == TRUE) {
               if (Files != NULL) {
                  delete Files;
                  Files = NULL;
                  sprintf (Temp, "250 CDUP command successfull.\r\n");
                  strcpy (CurPath, "/");
               }
               else
                  sprintf (Temp, "550 %s: no such file or directory.\r\n", "..");
            }
            else
               sprintf (Temp, "550 %s: no such file or directory.\r\n", "..");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (!stricmp (p, "PWD")) {
            if (User != NULL || Anonymous == TRUE) {
               if (Files == NULL)
                  sprintf (Temp, "257 \"/\" is current directory.\r\n");
               else {
                  Files->First ();
                  sprintf (Temp, "257 \"%s\" is current directory.\r\n", strlwr (CurPath));
               }
            }
            else
               sprintf (Temp, "257 \"/%s\" is current directory.\r\n", "outbound");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (!stricmp (p, "DELE")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (Nodes != NULL && Outbound != NULL) {
                  if (Outbound->First () == TRUE)
                     do {
                        if (!stricmp (Outbound->Name, p)) {
                           Outbound->Remove ();
                           sprintf (Temp, "250 DELE command successfull.\r\n");
                           Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                        }
                     } while (Outbound->Next () == TRUE);
                  Outbound->Update ();
               }
            }
         }
         else {
            sprintf (Temp, "503 Command not implemented.\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
      }

#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   }
}

USHORT TFTP::Login (VOID)
{
   CHAR *p, Name[48];

   Valid = FALSE;
   EndRun = FALSE;
   Known = Protected = FALSE;

   if (FileExist ("logo.ftp")) {
#if defined(__OS2__)
      sprintf (Temp, "220-%s Professional Edition for OS/2 FTP Server - Version %s\r\n", NAME, VERSION);
#elif defined(__NT__)
      sprintf (Temp, "220-%s Professional Edition for Win/NT FTP Server - Version %s\r\n", NAME, VERSION);
#endif
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
      ReadFile ("220", "logo.ftp");
   }
   else {
#if defined(__OS2__)
      sprintf (Temp, "220 %s Professional Edition for OS/2 FTP Server - Version %s\r\n", NAME, VERSION);
#elif defined(__NT__)
      sprintf (Temp, "220 %s Professional Edition for Win/NT FTP Server - Version %s\r\n", NAME, VERSION);
#endif
      Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   }

   while (Tcp->Carrier () == TRUE && EndRun == FALSE) {
//   while (EndRun == FALSE) {
      GetCommand ();
      if ((p = strtok (Response, " ")) != NULL) {
         if (!stricmp (p, "USER")) {
            if ((p = strtok (NULL, "")) != NULL) {
               while (*p == ' ')
                  p++;
            }
            else
               p = "anonymous";

            strcpy (Name, p);
            if (!stricmp (p, "anonymous")) {
               sprintf (Temp, "331 Enter your e-mail address as password for anonymous\r\n");
               Anonymous = TRUE;
            }
            else {
               if (strchr (p, ':') != NULL && strchr (p, '/') != NULL) {
                  Known = Protected = FALSE;
                  if ((Nodes = new TNodes (Cfg->NodelistPath)) != NULL) {
                     Valid = TRUE;
                     Addr.Parse (p);
                     if (Nodes->Read (Addr) == TRUE) {
                        Known = TRUE;
                        if (Nodes->SessionPwd[0] != '\0') {
                           sprintf (Temp, "331 Password required for %s.\r\n", Name);
                           Valid = FALSE;
                        }
                     }
                     if (Valid == TRUE)
                        EndRun = TRUE;
                  }
               }
               else {
                  if ((User = new TUser (Cfg->UserFile)) != NULL)
                     User->GetData (p);
                  sprintf (Temp, "331 Password required for %s.\r\n", p);
               }
            }
            if (Valid == FALSE)
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
         else if (!stricmp (p, "PASS")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (User != NULL) {
                  if (User->CheckPassword (p) == TRUE)
                     Valid = TRUE;
               }
               else if (Nodes != NULL) {
                  if (!stricmp (p, Nodes->SessionPwd)) {
                     if ((Outbound = new TOutbound (Cfg->Outbound)) != NULL)
                        Outbound->Add (Addr.Zone, Addr.Net, Addr.Node, Addr.Point, Addr.Domain);
                     Valid = TRUE;
                     Protected = TRUE;
                  }
               }
               else if (Anonymous == TRUE)
                  Valid = TRUE;
            }

            if (Valid == TRUE)
               EndRun = TRUE;
            else {
               sprintf (Temp, "530 User not logged in.\r\n");
               Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
               if (User != NULL)
                  delete User;
               if (Nodes != NULL)
                  delete Nodes;
               Nodes = NULL;
               User = NULL;
            }
         }
         else if (!stricmp (p, "QUIT")) {
            sprintf (Temp, "221 Goodbye.\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
            EndRun = TRUE;
         }
         else {
            sprintf (Temp, "503 Command not implemented.\r\n");
            Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         }
      }

#if defined(__OS2__)
      DosSleep (1L);
#elif defined(__NT__)
      Sleep (1L);
#endif
   }

   if (Valid == TRUE) {
      if (FileExist ("welcome.ftp")) {
         sprintf (Temp, "230-User %s logged in.\r\n", Name);
         Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
         ReadFile ("230", "welcome.ftp");
      }
      else {
         sprintf (Temp, "230 User %s logged in.\r\n", Name);
         Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
      }
   }

   return (Valid);
}



