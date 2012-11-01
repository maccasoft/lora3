
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_defs.h"
#include "menu.h"

void set_top_mnu_ita (void)
{
   int fd;
   MENUHDR menuhdr;
   ITEM item;

   fd = open ("top.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "TOP");
   menuhdr.usItems = 11;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nMenu' Principale (TOP)\n^Fai la tua scelta (T,I,F,E,C,L,A,D,O,? per aiuto, o X per uscire): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nScegli uno dei seguenti:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Teleconferenza\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^I^ ... Centro Informazioni\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "I");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Posta Elettronica\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = MNU_GOEMAIL;
   strcpy (item.szArgument, "EMAIL");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... Conferenze (Aree Messaggi Pubbliche)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_GOMSG;
   strcpy (item.szArgument, "MSG");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... Librerie di Programmi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = MNU_GOFILE;
   strcpy (item.szArgument, "FILE");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^A^ ... Visualizza/Modifica Dati Personali\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Servizi Internet\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "INTERNET");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Giochi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "DOORS");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^O^ ... Lettore di Posta Off-Line\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "O");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "OFFLINE");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Fine Collegamento (Logoff)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_LOGOFF;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);
}

void set_menu_ita (void)
{
   int fd;
   MENUHDR menuhdr;
   ITEM item;

   printf ("Menu System\n");

   set_top_mnu_ita ();

   fd = open ("msg.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "MSG");
   menuhdr.usItems = 11;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nConferenze\n^Fai la tua scelta (R,W,T,S,M,E,A,C,O o X per uscire): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nLa Conferenza corrente e' : Y\nCi sono 9 messaggi.\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Leggi Messaggi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = MNU_READMAIL;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^W^ ... Scrivi un Messaggio\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "W");
   item.usCommand = MNU_WRITEMAIL;
   strcpy (item.szArgument, "EDITOR");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^Q^ ... Configurazione del Lettore Off-Line\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "Q");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "OLRCONF");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... Elenco Messaggi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "LISTING");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Teleconferenza\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Seleziona un'altra Conferenza\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_SELECTAREA;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^M^ ... Modifica un Messaggio\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "M");
   item.usCommand = MNU_MODIFYMAIL;
   strcpy (item.szArgument, "EDITOR");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Cancella un Messaggio\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = MNU_DELETEMAIL;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^O^ ... Menu' Operazioni\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "O");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // -----------------

   fd = open ("olrconf.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "OLRCONF");
   menuhdr.usItems = 4;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nLettore Offline\n^Fai la tua scelta (+,-,V, o X per uscire): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^+^ ... Aggiungi una Conferenza al tuo Lettore Offline\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "+");
   item.usCommand = MNU_QUICKADD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^-^ ... Togli una Conferenza dal tuo Lettore Offline\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "-");
   item.usCommand = MNU_QUICKREMOVE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^V^ ... Guarda le Conferenza presenti nel tuo Lettore Offline\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "V");
   item.usCommand = MNU_QUICKDISPLAY;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // -----------------

   fd = open ("listing.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "LISTING");
   menuhdr.usItems = 9;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nScegli una lettera dalla lista, oppure ? per l'aiuto: ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nPuoi elencare i messaggi nei seguenti modi:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^B^ ... Titolo breve      (1 linea per messagio)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "B");
   item.usCommand = MNU_LISTBRIEF;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "B");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Titolo            (4 linee di sommario per messaggio)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = MNU_LISTTITLE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... Tutto il testo    (sommario piu' tutto il testo)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_LISTTEXT;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "|");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // -----------------

   fd = open ("file.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "FILE");
   menuhdr.usItems = 13;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nLibrerie di Programmi\n^Fai la tua scelta (R,W,F,T,S,M,E,A,C,O o X per uscire): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nBenvenuto nella Libreria 2\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "Descrizione: Z\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "Ci sono 8 archivi disponibili per il prelievo.\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nSeleziona uno dei seguenti:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... Cerca i Programmi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "FILELIST");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Preleva un Programma\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_DOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^U^ ... Mandaci un Programma\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "U");
   item.usCommand = MNU_UPLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Gestisci i Programmi Selezionati (7 selezionati)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = MNU_MANAGETAGGED;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... Preleva l'Elenco dei Programmi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Seleziona un'altra Libreria\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_SELECTLIBRARY;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^P^ ... Imposta le tue Preferenze\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "P");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^O^ ... Menu Operazioni\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "O");
   item.usCommand = MNU_GOSUB;
   strcpy (item.szArgument, "LIBOP");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Esci dalla Libreria Programmi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("libop.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "LIBOP");
   menuhdr.usItems = 14;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nLibrary Operations\n^Make your selection (C,D,E,L,J,M,I,A,U,R,V,F,S or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nLibrary Operations Menu for the <???> Library.\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^C^ ... Create a Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "C");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Delete a Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Edit this Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... Add files to this Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = MNU_ADDFILES;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^J^ ... Join/Unjoin other Libraries to this Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "J");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^M^ ... Library maintenance\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "M");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^I^ ... List Libraries with files awaiting approval\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "I");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^A^ ... Approve files (0 waiting)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^U^ ... Unapprove files\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "V");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Remove or rename files\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = MNU_REMOVEFILES;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^V^ ... Move or copy files in this Library to another Library\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "V");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... Edit file information\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_EDITFILE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Exit the operations menu\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("filelist.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "FILELIST");
   menuhdr.usItems = 17;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\n^Sceglii un'opzione, oppure X per uscire: ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nElenco dei Programmi:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... Per nome, in ordine alfabetico\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_FILELIST;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Per data, dai piu' recenti aggiunti alla Libreria\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_SEARCHDATE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^Q^ ... Elenco veloce dei nuovi Programmi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "Q");
   item.usCommand = MNU_SEARCHNEW;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "Q");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^N^ ... Per quante volte il Programma e' stato Prelevato\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "N");
   item.usCommand = MNU_SEARCHDOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "N");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nRicerca dei Programmi:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^W^ ... Per nome del Programma, o nome parziale\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "W");
   item.usCommand = MNU_SEARCHNAME;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "W");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^K^ ... Per parole chiave\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "K");
   item.usCommand = MNU_SEARCHKEYWORD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "K");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Per parole chiave nel nome e nella descrizione\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_SEARCHTEXT;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("email.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "EMAIL");
   menuhdr.usItems = 6;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nPosta Elettronica\n^Fai la tua scelta (R,W,E,P, o X per uscire): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nSono disponibili i seguenti servizi di Posta Elettronica:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Lettura dei Messaggi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = MNU_READMAIL;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^W^ ... Scrittura di un Messaggio\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "W");
   item.usCommand = MNU_WRITEMAIL;
   strcpy (item.szArgument, "EDITOR");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Cancellazione di un Messaggio\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = MNU_DELETEMAIL;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^P^ ... Preferenze Personali\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "P");
   item.usCommand = MNU_READPREFERENCE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Uscita dalla Posta Elettronica\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("editor.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "EDITOR");
   menuhdr.usItems = 13;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nEditor\n^Fai la tua scelta (S,R,A,D,L,I,C,N,H,B,U o X per uscire): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nOPZIONI PER L'EDITOR:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^S^ ... Salva il messaggio  ");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "S");
   item.usCommand = MNU_SAVEMSG;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Riscrivi una linea\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = MNU_RETYPELINE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^A^ ... Aggiungi al messaggio");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = MNU_APPENDMSG;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Cancella una linea\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_DELETELINE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... Visualizza il messaggio  ");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = MNU_LISTMSG;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^I^ ... Inserisci una o piu' linee\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "I");
   item.usCommand = MNU_INSERTLINE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^C^ ... Cambia il testo   ");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "C");
   item.usCommand = MNU_CHANGETEXT;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^N^ ... Nuovo messaggio\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "N");
   item.usCommand = MNU_NEWMESSAGE;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^H^ ... Aiuto          ");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "H");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^B^ ... Cambia l'argomento del messaggio\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "B");
   item.usCommand = MNU_CHANGESUBJ;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^U^ ... Manda un testo\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "U");
   item.usCommand = MNU_UPLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("offline.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "OFFLINE");
   menuhdr.usItems = 7;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nLettore Offline (QWK/BlueWave)\n^Scegli un'opzione (Q,B,A,U,R,? per aiuto o X per uscire): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nBenvenuto al Lettore Offline!\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^Q^ ... Preleva l'archivio in formato ^QWK^\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "Q");
   item.usCommand = MNU_QUICKDOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Preleva l'archivio in formato ^BlueWave^\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = MNU_BWDOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^A^ ... Preleva l'archivio in formato ^ASCII^\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = MNU_ASCIIDOWNLOAD;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^U^ ... Manda i tuoi messaggi\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "U");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^R^ ... Imposta gli ultimi messaggi letti\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "R");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^X^ ... Uscita dal Lettore Offline\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("doors.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "DOORS");
   menuhdr.usItems = 7;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nDoors\n^Select a letter from the list (I,E,L, ? for help or X to exit): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nThe following Doors services are available:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^E^ ... Enter a Door\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "E");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^L^ ... List available Doors\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "L");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^A^ ... Add a Door\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "A");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000U;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^M^ ... Modify a Door\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "M");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000U;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^D^ ... Delete a Door\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "D");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000U;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^X^ ... Exit Doors\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

   // ----------------

   fd = open ("internet.mnu", O_WRONLY|O_BINARY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);

   memset (&menuhdr, 0, sizeof (menuhdr));
   menuhdr.cbSize = sizeof (menuhdr);
   strcpy (menuhdr.szMenuName, "INTERNET");
   menuhdr.usItems = 7;
   menuhdr.ucColor = 0x0A;
   menuhdr.ucHilight = 0x0B;
   strcpy (menuhdr.szPrompt, "\nServizi Internet\n^Fai la tua scelta (T,F,I, ? per aiuto o X per uscire): ^\x16\x01\x07");
   write (fd, &menuhdr, sizeof (menuhdr));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\nSono disponibili i seguenti Servizi Internet:\n\n");
   item.ucColor = 0x0A;
   item.ucHilight = 0x0A;
   strcpy (item.szKey, "");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^T^ ... Telnet\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "T");
   item.usCommand = MNU_TELNET;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^F^ ... FTP (Trasferimento Programmi)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "F");
   item.usCommand = MNU_FTP;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^G^ ... Gopher\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "G");
   item.usCommand = 0;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^I^ ... Finger (Informazioni sugli Utenti)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "I");
   item.usCommand = MNU_FINGER;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "  ^V^ ... VModem (Virtual Modem Protocol)\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "V");
   item.usCommand = MNU_VMODEM;
   strcpy (item.szArgument, "");
   item.usLevel = 65000;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   memset (&item, 0, sizeof (item));
   item.cbSize = sizeof (item);
   strcpy (item.szDisplay, "\n  ^X^ ... Esci da Internet\n");
   item.ucColor = 0x0E;
   item.ucHilight = 0x0B;
   strcpy (item.szKey, "X");
   item.usCommand = MNU_RETURN;
   strcpy (item.szArgument, "");
   item.usLevel = 0;
   item.ulAccessFlags = 0;
   item.ulDenyFlags = 0;
   write (fd, &item, sizeof (item));

   close (fd);

}
