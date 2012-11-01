
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.19
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// FTP Server Module
//
// History:
//    11/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "combase.h"
#include "config.h"
#include "filebase.h"
#include "files.h"
#include "nodes.h"
#include "outbound.h"
#include "user.h"
#include "version.h"

class TFTP
{
public:
   TFTP (void);
   ~TFTP (void);

   CHAR   ClientIP[16];
   class  TConfig *Cfg;
   class  TTcpip  *Tcp;

   USHORT FileExist (PSZ pszFile);
   VOID   GetCommand (VOID);
   USHORT Login (VOID);
   VOID   ReadFile (PSZ pszCode, PSZ pszFile);
   VOID   Server (VOID);

private:
   USHORT Valid, Anonymous, Binary, DataPort;
   USHORT Known, Protected;
   PSZ    Month[12];
   CHAR   Response[128], Temp[4096];
#if defined(__OS2__)
   HEV    hEvent;
#endif
   struct dosdate_t date;
   class  TAddress  Addr;
   class  TTcpip    *Data;
   class  TUser     *User;
   class  TNodes    *Nodes;
   class  FileData  *Area;
   class  TFile     *Files;
   class  TOutbound *Outbound;
};

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
   Month[0] = "Jan";
   Month[1] = "Feb";
   Month[2] = "Mar";
   Month[3] = "Apr";
   Month[4] = "Maj";
   Month[5] = "Jun";
   Month[6] = "Jul";
   Month[7] = "Aug";
   Month[8] = "Sep";
   Month[9] = "Oct";
   Month[10] = "Nov";
   Month[11] = "Dec";

#if defined(__OS2__)
   DosCreateEventSem (NULL, &hEvent, 0L, FALSE);
#endif
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

#if defined(__OS2__)
   DosCloseEventSem (hEvent);
#endif
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
      DosWaitEventSem (hEvent, 200L);
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

VOID TFTP::Server (VOID)
{
   int fd;
   USHORT EndRun, Readed, Ok, GotA;
   CHAR *p;

   EndRun = FALSE;

   while (Tcp->Carrier () == TRUE && EndRun == FALSE) {
      GetCommand ();
      printf ("  %s\n", Response);
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
                              sprintf (Temp, "%cr--r--r--   1 ftp      lorabbs  %8ld %s %2d  %4d %s\r\n", '-', Outbound->Size, Month[date.month - 1], date.day, date.year, strlwr (Outbound->Name));
                              Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           } while (Outbound->Next () == TRUE);
                     }
                  }
                  else if (Files == NULL) {
                     if (p != NULL) {
                        if ((Area = new FileData (".\\")) != NULL) {
                           if (Area->First () == TRUE)
                              do {
                                 if (!stricmp (p, Area->Key)) {
                                    if ((Files = new TFile (".\\", Area->Key)) != NULL) {
                                       Files->SortByName ();
                                       if (Files->First () == TRUE)
                                          do {
                                             if (Files->UplDate.Year == date.year)
                                                sprintf (Temp, "%cr--r--r--   1 ftp      lorabbs  %8ld %s %2d %2d:%2d %s\r\n", '-', Files->Size, Month[Files->UplDate.Month - 1], Files->UplDate.Day, Files->UplDate.Hour, Files->UplDate.Minute, strlwr (Files->Name), ' ');
                                             else
                                                sprintf (Temp, "%cr--r--r--   1 ftp      lorabbs  %8ld %s %2d  %4d %s\r\n", '-', Files->Size, Month[Files->UplDate.Month - 1], Files->UplDate.Day, Files->UplDate.Year, strlwr (Files->Name), ' ');
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
                        if ((Area = new FileData (".\\")) != NULL) {
                           if (Area->First () == TRUE)
                              do {
                                 sprintf (Temp, "%cr--r--r--   1 ftp      lorabbs  %8ld %s %2d  %4d %s\r\n", 'd', 0L, Month[date.month - 1], date.day, date.year, strlwr (Area->Key));
                                 Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                              } while (Area->Next () == TRUE);
                           delete Area;
                        }
                     }
                  }
                  else {
                     if (Files->First () == TRUE)
                        do {
                           if (Files->UplDate.Year == date.year)
                              sprintf (Temp, "%cr--r--r--   1 ftp      lorabbs  %8ld %s %2d %2d:%2d %s\r\n", '-', Files->Size, Month[Files->UplDate.Month - 1], Files->UplDate.Day, Files->UplDate.Hour, Files->UplDate.Minute, strlwr (Files->Name), ' ');
                           else
                              sprintf (Temp, "%cr--r--r--   1 ftp      lorabbs  %8ld %s %2d  %4d %s\r\n", '-', Files->Size, Month[Files->UplDate.Month - 1], Files->UplDate.Day, Files->UplDate.Year, strlwr (Files->Name), ' ');
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
                     if ((Area = new FileData (".\\")) != NULL) {
                        if (Area->First () == TRUE)
                           do {
                              sprintf (Temp, "%s/\r\n", strlwr (Area->Key));
                              Data->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
                           } while (Area->Next () == TRUE);
                        delete Area;
                     }
                  }
                  else {
                     if (Files->First () == TRUE)
                        do {
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
         else if (!stricmp (p, "RETR")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (*p == '/')
                  p++;
               if ((User != NULL || Anonymous == TRUE) && strchr (p, '/') != NULL) {
                  if ((Area = new FileData (".\\")) != NULL) {
                     if (Area->First () == TRUE)
                        do {
                           if (!strnicmp (p, Area->Key, strlen (Area->Key)) && p[strlen (Area->Key)] == '/') {
                              if ((Files = new TFile (".\\", Area->Key)) != NULL) {
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
         else if (!stricmp (p, "STOR")) {
            if ((p = strtok (NULL, " ")) != NULL) {
               if (Nodes != NULL) {
                  if ((Data = new TTcpip) != NULL) {
                     sprintf (Temp, "150 Opening %s mode data connection for %s.\r\n", (Binary == TRUE) ? "BINARY" : "ASCII", p);
                     Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));

                     if (Data->ConnectServer (ClientIP, DataPort) == TRUE) {
                        if (Protected == TRUE)
                           sprintf (Temp, "%s%s", Cfg->InboundProt, p);
                        else if (Known == TRUE)
                           sprintf (Temp, "%s%s", Cfg->InboundKnown, p);
                        else
                           sprintf (Temp, "%s%s", Cfg->Inbound, p);

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
                              DosWaitEventSem (hEvent, 200L);
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
                     }
                     else
                        sprintf (Temp, "550 %s: no such file or directory.\r\n", p);
                  }
                  else {
                     if ((Area = new FileData (".\\")) != NULL) {
                        if (Area->Read (p) == TRUE) {
                           if (Files != NULL)
                              delete Files;
                           if ((Files = new TFile (".\\", Area->Key)) != NULL)
                              Files->SortByName ();
                           sprintf (Temp, "250 CWD command successfull.\r\n");
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
                  sprintf (Temp, "257 \"/lorabbs\" is current directory.\r\n");
               else {
                  Files->First ();
                  sprintf (Temp, "257 \"/lorabbs/%s\" is current directory.\r\n", strlwr (Files->Area));
               }
            }
            else
               sprintf (Temp, "257 \"/lorabbs/%s\" is current directory.\r\n", "outbound");
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
      DosWaitEventSem (hEvent, 200L);
#endif
   }
}

USHORT TFTP::Login (VOID)
{
   USHORT EndRun;
   CHAR *p, Name[48];

   Valid = FALSE;
   EndRun = FALSE;
   Known = Protected = FALSE;

   if (FileExist ("logo.ftp")) {
      sprintf (Temp, "220-%s Professional Edition for OS/2 FTP Server - Version %s\r\n", NAME, VERSION);
      Tcp->BufferBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
      ReadFile ("220", "logo.ftp");
   }
   else {
      sprintf (Temp, "220 %s Professional Edition for OS/2 FTP Server - Version %s\r\n", NAME, VERSION);
      Tcp->SendBytes ((UCHAR *)Temp, (USHORT)strlen (Temp));
   }

   while (Tcp->Carrier () == TRUE && EndRun == FALSE) {
      GetCommand ();
      printf ("  %s\n", Response);
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
                  if ((Nodes = new TNodes (".\\")) != NULL) {
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
                  if ((User = new TUser (".\\")) != NULL)
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
      DosWaitEventSem (hEvent, 200L);
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

VOID FtpThread (VOID *arg)
{
   class TFTP *Ftp = (class TFTP *)arg;

   if (Ftp->Login () == TRUE)
      Ftp->Server ();
   delete Ftp;

   _endthread ();
}

// ----------------------------------------------------------------------
// Entry point per il server FTP.
//
//    argv[1] = Port da utilizzare (21 = ftp)
// ----------------------------------------------------------------------

void main (int argc, char *argv[])
{
   USHORT Socket, Port;
#if defined(__OS2__)
   HEV hEvent;
#endif
   class TTcpip *Tcp;
   class TFTP *Ftp;

#if defined(__OS2__)
   printf ("\nLoraBBS Professional Edition for OS/2 PM. Version %s\n", VERSION);
#elif defined(__NT__)
   printf ("\nLoraBBS Professional Edition for Windows NT/95. Version %s\n", VERSION);
#endif
   printf ("Copyright (c) 1995 by Marco Maccaferri. All rights reserved.\n\n");

   Port = 21;
   if (argc > 1)
      Port = (USHORT)atoi (argv[1]);

   if ((Tcp = new TTcpip) != NULL) {
      if (Tcp->Initialize (Port) == TRUE) {
#if defined(__OS2__)
         DosCreateEventSem (NULL, &hEvent, 0L, FALSE);
#endif

         printf ("FTP Server ready on port %d\n\n", Port);

         for (;;) {
            if ((Socket = Tcp->WaitClient ()) != 0) {
               if (!stricmp (Tcp->ClientName, Tcp->ClientIP))
                  printf ("Incoming request from %s\n", Tcp->ClientIP);
               else
                  printf ("Incoming request from %s (%s)\n", Tcp->ClientName, Tcp->ClientIP);

               if ((Ftp = new TFTP) != NULL) {
                  if ((Ftp->Cfg = new TConfig (".\\")) != NULL)
                     Ftp->Cfg->Read (0);
                  if ((Ftp->Tcp = new TTcpip) != NULL) {
                     strcpy (Ftp->ClientIP, Tcp->ClientIP);
                     if (Ftp->Tcp->Initialize (0, Socket) == TRUE)
                        _beginthread (FtpThread, NULL, 8192, Ftp);
                  }
               }
            }
#if defined(__OS2__)
            DosWaitEventSem (hEvent, 200L);
#endif
         }
      }
      delete Tcp;
   }
}


