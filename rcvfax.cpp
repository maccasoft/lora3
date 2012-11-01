
// LoraBBS Version 2.99 Free Edition
// Copyright (C) 1987-98 Marco Maccaferri
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "_ldefs.h"
#include "lorawin.h"

#define ETX          0x03
#define DLE          0x10
#define DC2          0x12

/* Constants for Class 2 commands */

/* exit codes  */

#define FAXSENT      0
#define FAXINSYNC    0
#define FAXNOSYNC    1
#define FAXNODIAL    2
#define FAXBUSY      3
#define FAXHANG      4
#define FAXERROR     5

/* My own page reception codes */

#define PAGE_GOOD             0
#define ANOTHER_DOCUMENT      1
#define END_OF_DOCUMENT       2
#define PAGE_HANGUP           4
#define PAGE_ERROR            5

/********************************************************/
/* Class 2 session parameters                           */

/* Set desired transmission params with +FDT=DF,VR,WD,LN
 * DF = Data Format :   0  1-d huffman
 *                      *1 2-d modified Read
 *                      *2 2-d uncompressed mode
 *                      *3 2-d modified modified Read
 *
 * VR = Vertical Res :  0 Normal, 98 lpi
 *                      1 Fine, 196 lpi
 *
 * WD = width :         0  1728 pixels in 215 mm
 *                      *1 2048 pixels in 255 mm
 *
 * LN = page length :   0 A4, 297 mm
 *                      1 B4, 364 mm
 *                      2  Unlimited
 *
 * EC = error correction :      0 disable ECM
 *
 * BF = binary file transfer :  0 disable BFT
 *
 * ST = scan time/line :        VR = normal     VR = fine
 *                         0    0 ms            0 ms
 *
 */

/* data format */

#define DF_1DHUFFMAN    0
#define DF_2DMREAD      1
#define DF_2DUNCOMP     2
#define DF_2DMMREAD     3

/* vertical resolution */

#define VR_NORMAL       0
#define VR_FINE         1

/* width */

#define WD_1728         0
#define WD_2048         1

/* page length */

#define LN_A4           0
#define LN_B4           1
#define LN_UNLIMITED    2

/* Baud rate */

#define BR_2400         0
#define BR_4800         1
#define BR_7200         2
#define BR_9600         3

TFax::TFax ()
{
   Format = 0;
   DataPath[0] = '\0';
   swaptableinit = FALSE;
   faxsize = 0L;
   opage = 0;
}

TFax::~TFax ()
{
}

/*--------------------------------------------------------------------------*/
/* FAX RECEIVE Routines                                                     */
/*--------------------------------------------------------------------------*/

/* receive fax files into basefilename */

int TFax::faxreceive ()
{
   int result, page;

   if (!swaptableinit)
      init_swaptable ();

   Com->SetParameters (19200, 8, 'N', 1);

   init_modem_response ();
   gEnd_of_document = FALSE;
   response.fcon = TRUE;            /* we already connected */

   result = 0;

   for (page = 0; gEnd_of_document == FALSE; page++) {
      result = get_fax_file (page);
      Log->Write (">FAX get_fax_file returns = %d", result);
      switch ((int) result) {
         case PAGE_GOOD:
            continue;

         case PAGE_HANGUP:
            Log->Write (" FAX Received %d pages", page);
            result = 1;
            gEnd_of_document = TRUE;
            break;

         default:
            Log->Write (" FAX Error during transmission");
            result = page;
            gEnd_of_document = TRUE;
            break;
      }
   }

   return (result);
}

/* This executes the +FDR receive page command, and looks for
 * the proper CONNECT response, or, if the document is finished,
 * looks for the FHNG/FHS code.
 *
 * returns:
 *  PAGE_GOOD                no error conditions occured during reception
 *  PAGE_HANGUP              normal end of transmission
 *  PAGE_ERROR               something's wrong
 */

int TFax::get_fax_file (int page)
{
   char buf[256], j[100];
   int result, TaskNumber = 1;
   FILE *fp = NULL;
   struct stat statbuf;

   Log->Write (">FAX [get_fax_file]");

   if (page == 0) {
      do {
         sprintf (buf, "%sfax%03d%02d.fax", DataPath, opage++, page + 1);
      } while (!stat (buf, &statbuf) && (opage < 256));
   }
   else
      sprintf (buf, "%sfax%03d%02d.fax", DataPath, opage, page + 1);

   if (opage == 1000) {
      Log->Write ("!FAX Couldn't create output file");
      return (PAGE_ERROR);
   }

   if ((result = faxmodem_receive_page ()) == 0) {
      /* filename to create for this page of document */
      if ((fp = fopen (buf, "ab")) == NULL) {
         Log->Write ("!FAX Couldn't create output file %s", buf);
         return (PAGE_ERROR);
      }

      if (!page)
         Log->Write (" FAX Connect with %s", response.remote_id);

      sprintf (j, "%s %s; page %02x", "FAX Rcv", buf, page);
      result = read_g3_stream (fp);
   }

   if (fp != NULL) {
      fclose (fp);
      if (faxsize <= 256L)
         unlink (buf);
      else
         Log->Write (" FAX File received %s (%lub)", buf, faxsize);
   }

   return (result);
}

/* Reads a data stream from the faxmodem, unstuffing DLE characters.
 * Returns the +FET value (2 if no more pages) or 4 if hangup.
 */

int TFax::read_g3_stream (FILE * fp)
{
   short c;
   char e_input_buf[11];
   unsigned char *secbuf, *p;
   long ltimer = 0L;                  /* MB 94-01-01 */
   int pseudo_carrier;                /* MB 94-01-01 */

   Log->Write (">FAX [read_g3_stream]");

   response.post_page_response_code = -1;    /* reset page codes         */
   response.post_page_message_code = -1;

   Com->ClearInbound ();

   if ((secbuf = (unsigned char *) calloc (1, 1024)) == NULL)
      goto fax_error;

   p = secbuf;

   sprintf (e_input_buf, "%lu", faxsize);
   fax_status (e_input_buf);

   pseudo_carrier = !(Com->Carrier ());        /* test if modem sets DCD */
   if (pseudo_carrier)
      Log->Write (">FAX modem doesn't assert DCD [read_g3_stream]");

   Log->Write (">FAX DC2  [read_g3_stream]");

   /* Send DC2 to start phase C data stream */

   Com->SendByte ((UCHAR) DC2);

   while (pseudo_carrier || Com->Carrier () == TRUE) {
      if (Com->BytesReady () == FALSE) {
         if (pseudo_carrier) {
            if (!ltimer)
               ltimer = TimerSet (1500);    /* 15 secs timeout */
            else if (TimeUp(ltimer))
               goto fax_error;   /* Houston, we lost the downlink   */
         }
         continue;            /* process timeouts */
      }
      else
         ltimer = 0L;         /* reset no char waiting timer */

      c = (short)(Com->ReadByte () & 0xff);    /* get a character  */

      if (c == DLE) {
         long ltimer2 = 0L;

         while (Com->BytesReady () == FALSE) {
            if (!ltimer2)
               ltimer2 = TimerSet (400);
            else if (TimeUp (ltimer2)) {
               faxsize = 0L;
               goto fax_error;      /* give up */
            }
         }

         c = Com->ReadByte ();

         if (c == ETX)     /* end of stream */
            goto end_page;
      }

      *p++ = swaptable[(unsigned char) c];
      faxsize++;

      if (!(faxsize % 1024)) {
         sprintf (e_input_buf, "%lu", faxsize);
         fax_status (e_input_buf);
         if (fwrite (secbuf, 1, 1024, fp) != 1024)
            goto fax_error;   /* hoppala */
         p = secbuf;
      }
   }

end_page:

   if (faxsize % 1024) {
      if (fwrite (secbuf, 1, (size_t) (faxsize % 1024), fp) != (size_t) (faxsize % 1024))
         goto fax_error;      /* hoppala */
      sprintf (e_input_buf, "%lu", faxsize);
      fax_status (e_input_buf);
   }

   free (secbuf);

   Log->Write (">FAX Waiting for +FET/+FHNG  [read_g3_stream]");
   c = 0;
   while (response.post_page_message_code == -1) {
      get_modem_result_code ();
      c++;
      if ((!response.post_page_response_code) || (c > 5) || (response.error))
         return (PAGE_ERROR);
      if (response.hangup_code != -1)
         return (PAGE_HANGUP);
   }
   return (PAGE_GOOD);

fax_error:
   if (secbuf != NULL)
      free (secbuf);
   Log->Write ("!FAX Error receiving page");
   get_modem_result_code ();
   return (PAGE_ERROR);
}

/*--------------------------------------------------------------------------*/
/* Class 2 Faxmodem Protocol Functions                                      */
/*                                                                          */
/* Taken from EIA Standards Proposal No. 2388: Proposed New Standard        */
/* "Asynchronous Facsimile DCE Control Standard" (if approved,              */
/* to be published as EIA/TIA-592)                                          */
/*--------------------------------------------------------------------------*/

/* reads a line of characters, terminated by a newline */

void TFax::get_faxline (char *p, int nbytes, unsigned int wtime)
{
   short c;             /* current modem character   */
   int count = 1;          /* character count (+null)   */
   long t;
   char *resp;

   t = TimerSet (wtime);

   resp = p;

   while ((count < nbytes) && (!TimeUp (t))) {
      if (Com->BytesReady () == FALSE) {
         continue;
      }
      c = (short)(Com->ReadByte () & 0xff); /* get a character           */
      if (c == '\n')
         continue;
      if (c == '\r')
         if (count > 1)
            break;         /* get out                   */
         else
            continue;      /* otherwise just keep going */
      *p++ = (char) c;     /* store the character       */
      ++count;          /* increment the counter     */
   }

   *p = '\0';              /* terminate the new string  */
}

void TFax::init_swaptable ()
{
   int i, j;

   for (i = 0; i < 256; i++) {
      j = (((i & 0x01) << 7) |
         ((i & 0x02) << 5) |
         ((i & 0x04) << 3) |
         ((i & 0x08) << 1) |
         ((i & 0x10) >> 1) |
         ((i & 0x20) >> 3) |
         ((i & 0x40) >> 5) |
         ((i & 0x80) >> 7));
      swaptable[i] = (unsigned char) j;
   }
   swaptableinit = TRUE;
}

/****************************************************************
 * Initialize a faxmodem_response struct
 */

void TFax::init_modem_response ()
{
   response.remote_id[0] = '\0';
   response.fcon = FALSE;
   response.connect = FALSE;
   response.ok = FALSE;
   response.error = FALSE;
   response.hangup_code = -1;
   response.post_page_response_code = -1;
   response.post_page_message_code = -1;
   response.T30.ec = response.T30.bf = 0;
}

/* This function parses numeric responses from the faxmodem.
 * It fills in any relevant slots of the faxmodem_response structure.
 */

void TFax::get_modem_result_code ()
{
   char buf[256];
   long t;

   Log->Write (">FAX [get_modem_result_code]");

   t = TimerSet (400);

   while (!TimeUp (t)) {
      buf[0] = '\0';
      get_faxline (buf, 255, 100);
      if (buf[0]) {
         parse_text_response (buf);
         return;
      }
   }
   return;
}

void TFax::fax_status (char *str)
{
   str = str;
}

void TFax::parse_text_response (char *str)
{
   /* Look for +FCON, +FDCS, +FDIS, +FHNG, +FHS, +FPTS, +FK, +FTSI */
   if (!strnicmp ("+FCON", str, 5)) {
      response.fcon = TRUE;
      fax_status ("+FCON     ");
      return;
   }

   if (!strnicmp (str, "OK", 2)) {
      response.ok = TRUE;
      return;
   }

   if (!strnicmp (str, "CONNECT", 7)) {
      response.connect = TRUE;
      return;
   }

   if (!strnicmp (str, "NO CARRIER", 10) || !strnicmp (str, "ERROR", 5)) {
      response.error = TRUE;
      response.hangup_code = 0;
      return;
   }

   if (!strnicmp (str, "+FDCS", 5)) {
      sscanf (str + 6, "%d,%d,%d,%d,%d,%d,%d,%d",
         &response.T30.vr, &response.T30.br, &response.T30.wd,
         &response.T30.ln, &response.T30.df, &response.T30.ec,
         &response.T30.bf, &response.T30.st);
      fax_status ("+FDCS     ");
      return;
   }

   if (!strnicmp (str, "+FHNG", 5)) {
      sscanf (str + 6, "%d", &response.hangup_code);
      fax_status ("+FHNG     ");
      return;
   }

   if (!strnicmp (str, "+FPTS", 5)) {
      sscanf (str + 6, "%d", &response.post_page_response_code);
      fax_status ("+FPTS     ");
      return;
   }

   if (!strnicmp (str, "+FTSI", 5)) {
      strcpy (response.remote_id, str + 6);
      fax_status ("+FTSI     ");
      return;
   }

   if (!strnicmp (str, "+FET", 4)) {
      sscanf (str + 5, "%d", &response.post_page_message_code);
      fax_status ("+FET      ");
      return;
   }

   if (!strnicmp (str, "+FHS", 4)) {
      sscanf (str + 5, "%d", &response.hangup_code);
      fax_status ("+FHS      ");
      return;
   }

   if (!strnicmp (str, "+FCS", 4)) {
      sscanf (str + 5, "%d,%d,%d,%d,%d,%d,%d,%d",
         &response.T30.vr, &response.T30.br, &response.T30.wd,
         &response.T30.ln, &response.T30.df, &response.T30.ec,
         &response.T30.bf, &response.T30.st);
      fax_status ("+FCS      ");
      return;
   }

   if (!strnicmp (str, "+FPS", 4)) {
      sscanf (str + 5, "%d", &response.post_page_response_code);
      fax_status ("+FPS      ");
      return;
   }

   if (!strnicmp (str, "+FTI", 4)) {
      strcpy (response.remote_id, str + 5);
      fax_status ("+FTI      ");
      return;
   }

}

/****************************************************************
 * Action Commands
 */

/* Receive a page
 * after receiving OK,
 * send +FDR
 * This is somewhat ugly, because the FDR command can return
 * a couple of possible results;
 * If the connection is valid, it returns something like
 *  +FCFR
 *  +FDCS: <params>
 *  CONNECT
 *
 * If, on the other hand, the other machine has hung up, it returns
 * +FHNG: <code>  or
 * +FHS: <code>
 *
 * and if the connection was never made at all, it returns ERROR (actually numeric
 * code 4)
 *
 * faxmodem_receive_page returns values:
 * PAGE_GOOD     page reception OK, data coming
 * PAGE_HANGUP   normal hangup
 * PAGE_ERROR    page error
 */

int TFax::faxmodem_receive_page ()
{
   long t;
   char buf[100];

   faxsize = 0L;
   response.connect = response.ok = FALSE;

  /* We wait until a string "OK" is seen
   * or a "+FHNG"
   * or a "ERROR" or "NO CARRIER"
   * or until 10 seconds for a response.
   */

   t = TimerSet (1000);

   Log->Write (">FAX Waiting for OK  [faxmodem_receive_page]");

   while (!TimeUp (t) && (!response.ok)) {
      get_faxline (buf, 100, 500);
      if (buf[0] != '\0')
         Log->Write ("> Response from peer: %s", buf);
      parse_text_response (buf);

      if (response.hangup_code != -1)
         return (PAGE_HANGUP);

      if (response.error)
         return (PAGE_ERROR);
   }

   if (!response.ok)
      return (PAGE_ERROR);

   Com->SendBytes ((UCHAR *)"AT+FDR\r", 7);
   Log->Write (">FAX AT+FDR  [faxmodem_receive_page]");

   /* We wait until either a string "CONNECT" is seen
    * or a "+FHNG"
    * or until 10 seconds for a response.
    */

   t = TimerSet (1000);

   Log->Write (">FAX Waiting for CONNECT  [faxmodem_receive_page]");

   while (!TimeUp (t)) {
      get_faxline (buf, 100, 500);
      if (buf[0] != '\0')
         Log->Write ("> Response from peer: %s", buf);
      parse_text_response (buf);

      if (response.connect == TRUE)
         return (PAGE_GOOD);

      if (response.hangup_code != -1)
         return (PAGE_HANGUP);

      if (response.error)
         return (PAGE_ERROR);
   }

   return (PAGE_ERROR);
}

