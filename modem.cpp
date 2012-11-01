
// ----------------------------------------------------------------------
// LoraBBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    03/10/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "msgbase.h"
#include "lorawin.h"

TModem::TModem (void)
{
   Serial = NULL;
   Log = NULL;
   Position = 0;
   strcpy (Ring, "RING");

#if defined(__LINUX__)
   strcpy (NodelistPath, "./nodes");
#else
   strcpy (NodelistPath, ".\\nodes");
#endif
   strcpy (DialCmd, "ATDT%s");
   Terminal = FALSE;
#if defined(__DOS__) || defined(__LINUX__)
   if (Daemon == FALSE) {
      if ((window = wopen (2, 49, 11, 78, 5, CYAN|_BLACK, CYAN|_BLACK)) != 0)
         wprintf ("\n\n\n\n\n\n\n\n\n\n\n\n");
      videoupdate ();
   }
#endif
}

TModem::~TModem (void)
{
   if (Serial != NULL) {
      Serial->SetDTR (FALSE);
      Serial->SetRTS (FALSE);
      delete Serial;
   }
#if defined(__DOS__) || defined(__LINUX__)
   if (Daemon == FALSE) {
      if (window != 0) {
         wactiv (window);
         wclose ();
      }
      videoupdate ();
   }
#endif
}

USHORT TModem::GetResponse (VOID)
{
   USHORT RetVal = NO_RESPONSE, IsUpper;
   CHAR c, *p;

   while (Serial->BytesReady () == TRUE && RetVal == NO_RESPONSE) {
      c = (CHAR)Serial->ReadByte ();
      if (Terminal == TRUE)
         Serial->SendByte (c);
      if (c == '\r' || Position >= sizeof (Response) - 1) {
         Response[Position] = '\0';
         if (Position > 0) {
            if (!stricmp (Response, "OK"))
               RetVal = OK;
            else if (!stricmp (Response, "ERROR"))
               RetVal = ERROR;
            else if (!stricmp (Response, Ring))
               RetVal = RING;
            else if (!stricmp (Response, "FAX") || !stricmp (Response, "+FCON"))
               RetVal = FAX;
            else if (!strnicmp (Response, "CONNECT", 7)) {
               if ((Speed = atol (&Response[7])) == 0L)
                  Speed = 300L;
               RetVal = CONNECT;
            }
            else if (!stricmp (Response, "NO CARRIER") || !stricmp (Response, "NO ANSWER") || !stricmp (Response, "BUSY"))
               RetVal = NO_CARRIER;

            if (Terminal == TRUE) {
               if (!strnicmp (Response, "ATD", 3) || !stricmp (Response, "ATA")) {
#if defined(__OS2__) || defined(__NT__)
                  if (hwndWindow != NULL) {
#elif defined(__DOS__) || defined(__LINUX__)
                  if (window != 0 && Daemon == FALSE) {
                     wactiv (window);
#endif
#if defined(__OS2__)
                     WinSendMsg (hwndWindow, WM_USER, MPFROMSHORT (WMU_ADDMODEMITEM), MPFROMP (Response));
#elif defined(__NT__)
                     SendMessage (hwndWindow, WM_USER, (WPARAM)WMU_ADDMODEMITEM, (LPARAM)Response);
#elif defined(__DOS__) || defined(__LINUX__)
                     wprintf ("\n%.28s", Response);
                     videoupdate ();
#endif
                  }

                  Pause (10);
                  Serial->SendBytes ((UCHAR *)"\r\n", 2);
                  sprintf (Response, "CONNECT %lu", Serial->Speed);
                  Serial->SendBytes ((UCHAR *)Response, (USHORT)strlen (Response));
                  Serial->SendBytes ((UCHAR *)"\r\n", 2);
                  Serial->SetDTR (TRUE);
                  Speed = Serial->Speed;
                  RetVal = CONNECT;
               }
               else if (!strnicmp (Response, "AT", 2)) {
                  Serial->SendBytes ((UCHAR *)"\r\n", 2);
                  Serial->SendBytes ((UCHAR *)"OK\r\n", 4);
               }
            }

#if defined(__OS2__) || defined(__NT__)
            if (hwndWindow != NULL) {
#elif defined(__DOS__) || defined(__LINUX__)
            if (window != 0 && Daemon == FALSE) {
               wactiv (window);
#endif
#if defined(__OS2__)
               WinSendMsg (hwndWindow, WM_USER, MPFROMSHORT (WMU_ADDMODEMITEM), MPFROMP (Response));
#elif defined(__NT__)
               SendMessage (hwndWindow, WM_USER, (WPARAM)WMU_ADDMODEMITEM, (LPARAM)Response);
#elif defined(__DOS__) || defined(__LINUX__)
               wprintf ("\n%.28s", Response);
               videoupdate ();
#endif
            }

            if (Log != NULL && RetVal != NO_RESPONSE && RetVal != OK && RetVal != ERROR) {
//            if (Log != NULL && RetVal != NO_RESPONSE) {
               p = Response;
               IsUpper = TRUE;
               while (*p != '\0') {
                  if (IsUpper == TRUE) {
                     *p = (CHAR)toupper (*p);
                     IsUpper = FALSE;
                  }
                  else if (*p == ' ' || *p == '/')
                     IsUpper = TRUE;
                  else if (IsUpper == FALSE) {
                     *p = (CHAR)tolower (*p);
                     IsUpper = FALSE;
                  }
                  p++;
               }
               Log->Write ("+%s", Response);
            }
         }
         Position = 0;
      }
      else if (c >= ' ')
         Response[Position++] = c;
   }

   if (RetVal == CONNECT && LockSpeed == FALSE)
      Serial->SetParameters (Speed, 8, 'N', 1);

   return (RetVal);
}

USHORT TModem::Initialize (ULONG comHandle)
{
   USHORT RetVal = FALSE;

   if (Serial != NULL) {
      delete Serial;
      Serial = NULL;
   }
   if (Serial == NULL)
      Serial = new TSerial;

   if (Serial != NULL) {
#if defined(__OS2__) || defined(__NT__) || defined(__LINUX__)
      strcpy (Serial->Device, Device);
#else
      Serial->Com = (USHORT)atoi (&Device[3]);
#endif
      Serial->Speed = Speed;
      if (comHandle == 0L) {
         if (Serial->Initialize () == TRUE) {
            if (Serial->Carrier () == FALSE) {
               Serial->SetDTR (FALSE);
               Serial->SetRTS (TRUE);
            }
            RetVal = TRUE;
         }
      }
      else {
#if defined(__OS2__)
         Serial->hFile = (HFILE)comHandle;
#elif defined(__NT__)
         Serial->hFile = (HANDLE)comHandle;
#endif
         Serial->SetParameters (Serial->Speed, Serial->DataBits, Serial->Parity, Serial->StopBits);
         if (Serial->Carrier () == FALSE) {
            Serial->SetDTR (FALSE);
            Serial->SetRTS (TRUE);
         }
         RetVal = TRUE;
      }
   }

   return (RetVal);
}

VOID TModem::Poll (PSZ pszNode)
{
   FILE *fp;
   CHAR Number[64], Temp[128], Traslated[128], *p;
   class TAddress Addr;
   class TNodes *Nodes;
   class TNodeFlags *Flags;

   strcpy (DialCmd, Cfg->Dial);
   strcpy (Number, pszNode);

   if (strchr (pszNode, '/') != NULL || strchr (pszNode, ':') != NULL) {
      if ((Nodes = new TNodes (NodelistPath)) != NULL) {
         Addr.Parse (pszNode);
         if (Nodes->Read (Addr) == TRUE) {
            if (Log != NULL)
               Log->Write ("*Processing %s - %s", Nodes->Address, Nodes->SystemName);
            strcpy (Number, Nodes->Phone);
            if (Nodes->DialCmd[0] != '\0')
               strcpy (DialCmd, Nodes->DialCmd);
            else {
               if ((Flags = new TNodeFlags (Cfg->SystemPath)) != NULL) {
                  if (Flags->Read (Nodes->Flags) == TRUE) {
                     if (Flags->Cmd[0] != '\0')
                        strcpy (DialCmd, Flags->Cmd);
                  }
                  delete Flags;
               }
            }
         }
         delete Nodes;
      }
   }

   if (Number[0] == '+')
      strcpy (Number, &Number[1]);

   sprintf (Temp, "%scost.cfg", NodelistPath);
   if ((fp = _fsopen (Temp, "rt", SH_DENYNO)) != NULL) {
      while (fgets (Temp, sizeof (Temp) - 1, fp) != NULL) {
         if ((p = strchr (Temp, '\n')) != NULL)
            *p = '\0';
         if ((p = strchr (Temp, '\r')) != NULL)
            *p = '\0';
         if ((p = strtok (Temp, " ")) != NULL) {
            if (!strcmp (p, "Prefix")) {
               if ((p = strtok (NULL, " ")) != NULL) {
                  if (!strncmp (Number, p, strlen (p)) || !strcmp (p, "-")) {
                     if (!strcmp (p, "-"))
                        p = "";
                     strcpy (Traslated, &Number[strlen (p)]);
                     if ((p = strtok (NULL, " ")) != NULL) {
                        if (!strcmp (p, "/"))
                           p = "";
                        strcpy (Number, p);
                        strcat (Number, Traslated);
                     }
                     break;
                  }
               }
            }
         }
      }
      fclose (fp);
   }

   if (Log != NULL)
      Log->Write (":Dialing %s", Number);

   if (Terminal == TRUE)
      SendCommand ("RING");
   else {
      sprintf (Temp, DialCmd, Number);
      SendCommand (Temp);
   }
}

VOID TModem::SendCommand (PSZ pszCmd)
{
   if (Terminal == FALSE) {
      Serial->SetDTR (TRUE);
      Pause (10);
   }

   while (*pszCmd) {
      switch (*pszCmd) {
         case '|':
            Serial->SendByte ((char)13);
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
            Serial->SendByte (*pszCmd);
            break;
      }
      pszCmd++;
   }

   Serial->SendByte ((char)13);
   Pause (10);
}

/*
USHORT TModem::ReceiveFax (PSZ path)
{
   int i, j;

   for (i = 0; i < 256; i++)
      j = (((i & 0x01) << 7) |
           ((i & 0x02) << 5) |
           ((i & 0x04) << 3) |
           ((i & 0x08) << 1) |
           ((i & 0x10) >> 1) |
           ((i & 0x20) >> 3) |
           ((i & 0x40) >> 5) |
           ((i & 0x80) >> 7));
      swaptable[i] = (unsigned char)j;
   }
}

#define ETX    0x03
#define DLE    0x10
#define DC2    0x12

USHORT TModem::ReadG3Stream (VOID)
{
   USHORT RetVal = FALSE, c, faxsize = 0;
   CHAR e_input_buf[11];
   UCHAR *secbuf, *p;
   ULONG ltimer = 0L;

   Serial->ClearInbound ();

   if ((secbuf = (UCHAR *)malloc (1024)) != NULL) {
      p = secbuf;
      Serial->SendByte (DC2);

      while (Serial->Carrier () == TRUE) {
         if (Serial->BytesReady () == TRUE) {
            c = Serial->ReadByte ();
            if (c == DLE) {
               while (Serial->BytesReady () == FALSE)
                  ;
               if ((c = Serial->ReadByte ()) == ETX)
                  break;
            }

            *p++ = swaptable[(unsigned char)c];
            faxsize++;

            if ((faxsize % 1024) == 0) {
               if (fax_fp != NULL)
                  fwrite (secbuf, 1, 1024, fax_fp);
               p = secbuf;
            }
         }
      }

      if ((faxsize % 1024) != 0)
         fwrite (secbuf, 1, faxsize % 1024, fp);

      free (secbuf);

      c = 0;
      post_page_code = -1;
      RetVal = TRUE;

      do {
         switch (GetResponse ()) {
            case NO_CARRIER:
            case ERROR:
            case FHNG:
            case FHS:
               RetVal = FALSE;
               break;
            case
         }
      } while (post_page_code == -1);
   }

   return (RetVal);
}
*/
