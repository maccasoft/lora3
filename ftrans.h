
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 3.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#ifndef _FTRANS_H
#define _FTRANS_H

#include "combase.h"
#include "lora_api.h"

#define CAN          ('X'&037)
#define XOFF         ('s'&037)
#define XON          ('q'&037)
#if !defined(OK)
#define OK           1
#endif
#define ZERROR       (-1)
#define TIMEOUT      (-2)
#define RCDO         (-3)
#define LZCONV       0
#define LZMANAG      0
#define LZTRANS      0
#define KSIZE        8192

#define ZPAD         '*'            /* 052 Padding character begins frames */
#define ZDLE         030            /* Ctrl-X Zmodem escape - `ala BISYNC DLE */
#define ZDLEE        (ZDLE^0100)    /* Escaped ZDLE as transmitted */
#define ZBIN         'A'            /* Binary frame indicator */
#define ZHEX         'B'            /* HEX frame indicator */
#define ZBIN32       'C'            /* Binary frame with 32 bit FCS */

/* Frame types (see array "frametypes" in zm.c) */
#define ZRQINIT      0              /* Request receive init */
#define ZRINIT       1              /* Receive init */
#define ZSINIT       2              /* Send init sequence (optional) */
#define ZACK         3              /* ACK to above */
#define ZFILE        4              /* File name from sender */
#define ZSKIP        5              /* To sender: skip this file */
#define ZNAK         6              /* Last packet was garbled */
#define ZABORT       7              /* Abort batch transfers */
#define ZFIN         8              /* Finish session */
#define ZRPOS        9              /* Resume data trans at this position */
#define ZDATA        10             /* Data packet(s) follow */
#define ZEOF         11             /* End of file */
#define ZFERR        12             /* Fatal Read or Write error Detected */
#define ZCRC         13             /* Request for file CRC and response */
#define ZCHALLENGE   14             /* Receiver's Challenge */
#define ZCOMPL       15             /* Request is complete */
#define ZCAN         16             /* Other end canned session with CAN*5 */
#define ZFREECNT     17             /* Request for free bytes on filesystem */
#define ZCOMMAND     18             /* Command from sending program */
#define ZSTDERR      19             /* Output to standard error, data follows */

/* ZDLE sequences */
#define ZCRCE        'h'            /* CRC next, frame ends, header packet follows */
#define ZCRCG        'i'            /* CRC next, frame continues nonstop */
#define ZCRCQ        'j'            /* CRC next, frame continues, ZACK expected */
#define ZCRCW        'k'            /* CRC next, ZACK expected, end of frame */
#define ZRUB0        'l'            /* Translate to rubout 0177 */
#define ZRUB1        'm'            /* Translate to rubout 0377 */

/* zdlread return values (internal) */
/* -1 is general error, -2 is timeout */
#define GOTOR        0400
#define GOTCRCE      (ZCRCE|GOTOR)  /* ZDLE-ZCRCE received */
#define GOTCRCG      (ZCRCG|GOTOR)  /* ZDLE-ZCRCG received */
#define GOTCRCQ      (ZCRCQ|GOTOR)  /* ZDLE-ZCRCQ received */
#define GOTCRCW      (ZCRCW|GOTOR)  /* ZDLE-ZCRCW received */
#define GOTCAN       (GOTOR|030)    /* CAN*5 seen */

/* Byte positions within header array */
#define ZF0          3              /* First flags byte */
#define ZF1          2
#define ZF2          1
#define ZF3          0
#define ZP0          0              /* Low order 8 bits of position */
#define ZP1          1
#define ZP2          2
#define ZP3          3              /* High order 8 bits of file position */

/* Bit Masks for ZRINIT flags byte ZF0 */
#define CANFDX       01             /* Rx can send and receive true FDX */
#define CANOVIO      02             /* Rx can receive data during disk I/O */
#define CANBRK       04             /* Rx can send a break signal */
#define CANCRY       010            /* Receiver can decrypt */
#define CANLZW       020            /* Receiver can uncompress */
#define CANFC32      040            /* Receiver can use 32 bit Frame Check */

/* Parameters for ZFILE frame */
/* Conversion options one of these in ZF0 */
#define ZCBIN        1              /* Binary transfer - inhibit conversion */
#define ZCNL         2              /* Convert NL to local end of line convention */
#define ZCRESUM      3              /* Resume interrupted file transfer */

/* Management options, one of these in ZF1 */
#define ZMNEW        1              /* Transfer if source newer or longer */
#define ZMCRC        2              /* Transfer if different file CRC or length */
#define ZMAPND       3              /* Append contents to existing file (if any) */
#define ZMCLOB       4              /* Replace existing file */
#define ZMSPARS      5              /* Encoding for sparse file */
#define ZMDIFF       6              /* Transfer if dates or lengths different */
#define ZMPROT       7              /* Protect destination file */

/* Transport options, one of these in ZF2 */
#define ZTLZW        1              /* Lempel-Ziv compression */
#define ZTCRYPT      2              /* Encryption */
#define ZTRLE        3              /* Run Length encoding */

/* Parameters for ZCOMMAND frame ZF0 (otherwise 0) */
#define ZCACK1       1              /* Acknowledge, then do command */

/* Parameters for ZSINIT frame */
#define ZATTNLEN     32             /* Max length of attention string */

class DLL_EXPORT TZModem
{
public:
   TZModem (void);
   ~TZModem (void);

   USHORT EndRun, Hangup;
   USHORT FileSent, Maxblklen, Telnet;
   CHAR   Pathname[128];
   ULONG  Speed;
   class  TCom *Com;
   class  TLog *Log;
   class  TProgress *Progress;

   USHORT AbortSession (VOID);
   SHORT  TimedRead (LONG hSec);
   SHORT  ZInitReceiver (VOID);
   SHORT  ZInitSender (SHORT NothingToDo);
   SHORT  ZReceiveFile (PSZ pszPath);
   SHORT  ZSendFile (PSZ pszFile, PSZ pszName = NULL);
   VOID   ZEndSender (VOID);

private:
   SHORT Wantfcs32, Txfcs32, Znulls, lastsent, ZCtlesc;
   SHORT Rxframeind, Rxtype, Rxflags, Rxbuflen, Rxcount;
   SHORT Tframlen, TryZHdrType, RxTempSize;
   CHAR  Txhdr[4], Rxhdr[4], Attn[ZATTNLEN];
   CHAR  RxBuffer[KSIZE], *TxBuffer;
   CHAR  RxTemp[256], *RxTempPos;
   LONG  Rxtimeout, Rxpos, Txpos, Rxbytes;

   VOID  ZAckBiBi (VOID);
   SHORT ZDLRead (VOID);
   SHORT ZGetByte (VOID);
   SHORT ZGetHeader (CHAR *hdr);
   SHORT ZGetHex (VOID);
   LONG  ZGetLong (char *hdr);
   VOID  ZPutHex (SHORT c);
   VOID  ZPutLong (CHAR *hdr, LONG pos);
   SHORT ZReceiveBinaryHeader (CHAR *hdr);
   SHORT ZReceiveBinaryHeader32 (CHAR *hdr);
   SHORT ZReceiveData (char *buf, short length);
   SHORT ZReceiveHexHeader (CHAR *hdr);
   VOID  ZSendBinaryHeader (SHORT type, CHAR *hdr);
   VOID  ZSendData (CHAR *buf, SHORT length, SHORT frameend);
   VOID  ZSendHexHeader (SHORT type, CHAR *hdr);
   VOID  ZSendLine (UCHAR c);
};

// ----------------------------------------------------------------------

class TFileQueue
{
public:
   TFileQueue (void);
   ~TFileQueue (void);

   USHORT Sent;
   CHAR   Name[32], Complete[128];
   ULONG  Size;
   UCHAR  DeleteAfter;
   UCHAR  TruncateAfter;
   ULONG  TotalFiles;

   USHORT Add (VOID);
   VOID   Clear (VOID);
   USHORT First (VOID);
   VOID   New (VOID);
   USHORT Next (VOID);
   USHORT Previous (VOID);
   VOID   Remove (PSZ pszName = NULL);
   VOID   Update (VOID);

private:
   class  TCollection Data;
};

// ----------------------------------------------------------------------

class DLL_EXPORT TTransfer : public TZModem
{
public:
   TTransfer (void);
   ~TTransfer (void);

   USHORT Task;
   CHAR   Device[16];
   class  TFileQueue RxQueue;
   class  TFileQueue TxQueue;

   PSZ    ReceiveXModem (PSZ pszPath);
   PSZ    ReceiveASCIIDump (PSZ pszPath);
   PSZ    ReceiveFTPHost (PSZ pszFile);
   PSZ    Receive1kXModem (PSZ pszPath);
   PSZ    ReceiveYModem (PSZ pszPath);
   PSZ    ReceiveYModemG (PSZ pszPath);
   PSZ    ReceiveZModem (PSZ pszPath);
   VOID   RunExternalProtocol (USHORT Download, PSZ Cmd, class TProtocol *Protocol);
   VOID   Janus (PSZ pszPath);
   USHORT Send1kXModem (PSZ pszFile);
   USHORT SendASCIIDump (PSZ pszFile);
   USHORT SendFTPHost (PSZ pszFile);
   USHORT SendXModem (PSZ pszFile);
   USHORT SendYModem (PSZ pszFile);
   USHORT SendYModemG (PSZ pszFile);
   USHORT SendZModem (PSZ pszFile, PSZ pszName = NULL);
   USHORT SendZModem8K (PSZ pszFile, PSZ pszName = NULL);

private:
   USHORT PktSize;
   UCHAR  Soh;
   USHORT DoCrc;
   USHORT UseAck;
   UCHAR  PktNumber;
   CHAR   FinalName[128];

   USHORT ReceivePacket (UCHAR *lpBuffer);
   PSZ    ReceiveXFile (PSZ pszPath);
   SHORT  SendPacket (UCHAR *lpBuffer);
   USHORT SendXFile (PSZ pszFile);
};

// ----------------------------------------------------------------------

/* Misc. Constants */
#define BUFMAX 2048          /* Max packet contents length                   */
#define JANUS_EFFICIENCY 95  /* Estimate Janus xfers at 95% throughput       */

class TJanus
{
public:
   TJanus (void);
   ~TJanus (void);

   USHORT TimeoutSecs;
   CHAR   RxPath[64];
   ULONG  Speed;
   class  TCom *Com;
   class  TLog *Log;
   class  TFileQueue *TxQueue;
   class  TFileQueue *RxQueue;
   class  TOutbound *Outbound;

   VOID   Transfer (VOID);

private:
   int    RxFile, TxFile;
   USHORT Rxblklen, IsOutbound;
   SHORT  CanCrc32, WaitFlag, RxCrc32, RxTempSize;
   CHAR   RxFileName[128], TxFileName[128];
   CHAR   RxTemp[256], *RxTempPos;
   UCHAR  LastSent, *Rxbufptr, *Rxbufmax;
   UCHAR  RxBuffer[BUFMAX + 8], TxBuffer[BUFMAX + 8];
   ULONG  RxPktCrc32;
   USHORT RxPktCrc16;
   ULONG  LastPktName;
   LONG   Rxpos, RxFilesize, RxFiletime;

   UCHAR pkttype, SharedCap, Done;
   USHORT xstate, rstate, rpos_count;
   USHORT blklen, txblklen, txblkmax;
   LONG rxstpos, length, xmit_retry, txpos, txstpos, timeout;
   LONG txlength, lasttx, last_blkpos, rpos_retry, rpos_sttime;
   struct utimbuf utimes;
#if defined(__NT__)
   HANDLE hBlock;
#elif defined(__OS2__)
#endif

   friend VOID SendThread (PVOID Args);

   SHORT  GetByte (VOID);
   VOID   GetNextFile (VOID);
   UCHAR  GetPacket (VOID);
   SHORT  GetRawByte (VOID);
   LONG   ProcessFileName (VOID);
   VOID   SendByte (UCHAR Byte);
   VOID   SendPacket (UCHAR *Buffer, USHORT Len, USHORT Type);
};

// ----------------------------------------------------------------------

#define FILE_RECEIVING        1
#define FILE_SENDING          2
#define FILE_BIDIRECTIONAL    3

class DLL_EXPORT TProgress
{
public:
   TProgress (void);
   virtual ~TProgress (void);

   USHORT Type;
   USHORT RxBlockSize, TxBlockSize;
   CHAR   RxFileName[128], TxFileName[128];
   ULONG  RxSize, RxPosition;
   ULONG  TxSize, TxPosition;

   virtual VOID   Begin (VOID);
   virtual VOID   End (VOID);
   virtual VOID   Update (VOID);
};

#endif

