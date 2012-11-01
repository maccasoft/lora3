
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

#include "combase.h"
#include "lora.h"
#if defined(__OS2__) || defined(__NT__)
#include "cpc.h"
#endif

extern USHORT Daemon;
extern class  TConfig *Cfg;
extern class  TPMLog  *Log;

// ----------------------------------------------------------------------------

#define NO_RESPONSE        0
#if !defined(OK)
#define OK                 1
#endif
#if defined(__OS2__) || defined(__DOS__) || defined(__LINUX__)
#define ERROR              2
#endif
#define RING               3
#define CONNECT            4
#define NO_CARRIER         5
#define FAX                6
#define FDCS               7
#define FHNG               8
#define FPTS               9
#define FTSI               10
#define FET                11
#define FHS                12
#define FCS                13
#define FPS                14
#define FTI                15

class TModem
{
public:
   TModem ();
   ~TModem ();

   CHAR   Device[32];
   CHAR   NodelistPath[64], DialCmd[64];
   CHAR   Ring[32];
   USHORT LockSpeed, Terminal;
   ULONG  Speed;
   CHAR   Response[64];
#if defined(__OS2__) || defined(__NT__)
   HWND   hwndWindow;
#elif defined(__DOS__) || defined(__LINUX__)
   CXLWIN window;
#endif
   class  TLog *Log;
   class  TSerial *Serial;

   USHORT GetResponse ();
   USHORT Initialize (ULONG comhandle = 0L);
   VOID   Poll (PSZ pszNode);
   VOID   SendCommand (PSZ pszCmd);

private:
   USHORT Position;
};

/* A T.30 DIS frame, as sent by the remote fax machine */

struct T30Params {
   int vr;
   int br;        /* BR = Bit Rate :    br * 2400BPS */
   int wd;        /* Page Width :                    */
   int ln;        /* Page Length :                   */
   int df;        /* Data compression format :       */
   int ec;        /* Error Correction :              */
   int bf;        /* Binary File Transfer            */
   int st;        /* Scan Time (ms) :                */
};

struct faxmodem_response
{
   char remote_id[50];              /* +FCSI remote id      */
   int  hangup_code;                /* +FHNG code           */
   int  post_page_response_code;    /* +FPTS code           */
   int  post_page_message_code;     /* +FET code            */
   int  fcon;                       /* Boolean; TRUE if +FCON  seen */
   int  connect;                    /* Boolean; TRUE if CONNECT msg seen */
   int  ok;                         /* Boolean; TRUE if OK seen */
   int  error;                      /* Boolean; TRUE if ERROR or NO CARRIER seen */

   /* Session params; parsed from +FDCS */
   struct T30Params T30;
};

class TFax
{
public:
   TFax ();
   ~TFax ();

   USHORT Format;
   CHAR   DataPath[64];
   class  TSerial *Com;
   class  TLog *Log;

   int    faxreceive ();

private:
   int    gEnd_of_document, swaptableinit;
   int    opage;
   UCHAR  swaptable[256];
   ULONG  faxsize;
   struct faxmodem_response response;

   int    get_fax_file (int page);
   int    read_g3_stream (FILE * fp);
   void   get_faxline (char *p, int nbytes, unsigned int wtime);
   void   init_swaptable ();
   void   init_modem_response ();
   void   get_modem_result_code ();
   void   fax_status (char *str);
   void   parse_text_response (char *str);
   int    faxmodem_receive_page ();
};

// ----------------------------------------------------------------------------

class TPMList
{
public:
#if defined(__OS2__) || defined(__NT__)
   TPMList (HWND hwnd);
#elif defined(__LINUX__) || defined(__DOS__)
   TPMList ();
#endif
   ~TPMList ();

   VOID   Add (PSZ Text);
   VOID   Clear ();
   VOID   Update (PSZ Text);

private:
   FILE   *fp;
   USHORT DoWrite;
   CHAR   Line[128];
#if defined(__OS2__) || defined(__NT__)
   HWND   hwndList;
#elif defined(__DOS__) || defined(__LINUX__)
   USHORT First;
   CXLWIN window;
#endif
};

// ----------------------------------------------------------------------------

class TTicProcessor
{
public:
   TTicProcessor ();
   ~TTicProcessor ();

   CHAR   Inbound[128];
   CHAR   Area[64], Name[32], Complete[128];
   CHAR   Password[32], Replace[32];
   ULONG  Size, Crc;
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TCollection *Description;
   class  TKludges *SeenBy;
   class  TCollection *Path;
   class  TAddress From;
   class  TAddress Origin;
   class  TPMList *Output;

   USHORT Check ();
   USHORT CheckEchoList (PSZ pszFile, PSZ pszEchoTag);
   VOID   Delete ();
   VOID   Hatch (class TAddress *Dest);
   VOID   Hatch (class TAddress &Dest);
   VOID   Hatch (PSZ address);
   VOID   Import ();
   USHORT ImportTic ();
   USHORT Open (PSZ pszFile);
   USHORT OpenNext ();

private:
   CHAR   PktName[32];
   CHAR   CurrentFile[128];
   CHAR   Temp[256], Display[128];
   class  TFileBase *File;
   class  TFileData *Data;
   class  TNodes *Nodes;
};

// --------------------------------------------------------------------------

typedef struct {
   USHORT Zone;
   USHORT Net;
   USHORT Node;
   USHORT Point;
} KLUDGES;

class TKludges
{
public:
   TKludges ();
   ~TKludges ();

   USHORT Sort, KeepPoint;
   USHORT Zone, Net, Node, Point;
   CHAR   Address[32], ShortAddress[32];

   USHORT Add ();
   USHORT AddString (PSZ pszString);
   USHORT Check (PSZ pszName);
   VOID   Clear ();
   VOID   Delete ();
   USHORT First ();
   VOID   New ();
   USHORT Next ();

private:
   class  TCollection Data;
};

// ----------------------------------------------------------------------------

class TMailProcessor
{
public:
   TMailProcessor ();
   ~TMailProcessor ();

   USHORT Packets;
   CHAR   Inbound[64], Outbound[64];
   ULONG  Bad, NetMail, MsgTossed, Duplicate;
   ULONG  MsgSent;
   class  TConfig *Cfg;
   class  TLog *Log;
   class  TPMList *Output;
   class  TStatus *Status;

   VOID   Change ();
   USHORT CheckEchoList (PSZ pszFile, PSZ pszEchoTag);
   VOID   Export ();
   VOID   ExportNetMail ();
   VOID   Import ();
   VOID   ImportBad ();
   USHORT IsArcmail ();
   VOID   News ();
   VOID   Pack (PSZ pszFile, PSZ pszRoute = NULL);
   VOID   Mail ();
   USHORT DoRescan ();
   USHORT UnpackArcmail ();

private:
   CHAR   PktName[32];
   CHAR   Display[128];
   CHAR   LastTag[64];
   CHAR   Temp[1024];
   CHAR   ArcMailName[128];
   CHAR   Line[128], Name[128];
   USHORT IsAreasBBS;
   USHORT BadArea;
   ULONG  TossedArea;
   ULONG  Started, BytesProcessed;
   class  PACKET *Packet;
   class  TNodes *Nodes;
   class  TMsgData *Data;
   class  TEchoLink *Forward;
   class  TDupes *Dupes;
   class  TMsgBase *Msg;
   class  TEchotoss *Echo;
   class  TKludges *SeenBy;
   class  TKludges *Path;

   USHORT ExportEchoMail (ULONG Number, PSZ pszEchoTag);
   ULONG  ImportEchoMail (PSZ EchoTag, class TMsgBase *InBase = NULL);
   VOID   MakeArcMailName (PSZ pszAddress, CHAR Flag);
   USHORT OpenArea (PSZ pszEchoTag);
   USHORT OpenNextPacket ();
   VOID   RouteTo ();
   VOID   SendTo ();
   VOID   Poll ();
};

// --------------------------------------------------------------------------

class TAreaManager
{
public:
   TAreaManager ();
   ~TAreaManager ();

   class  TConfig *Cfg;
   class  TLog *Log;
   class  TMsgBase *Msg;
   class  TStatus *Status;

   USHORT AddArea (PSZ address, PSZ area);
   VOID   DoAreaListings (PSZ Address, USHORT Type, USHORT Level, ULONG AccessFlags, ULONG DenyFlags);
   USHORT FilePassive (PSZ address, USHORT flag);
   USHORT FileRemoveAll (PSZ address);
   VOID   ImportAreasBBS ();
   VOID   ImportDescriptions (PSZ pszFile);
   VOID   ExportDescriptions (PSZ pszFile);
   VOID   MsgFooter ();
   VOID   MsgHeader ();
   USHORT Passive (PSZ address, USHORT flag);
   VOID   ProcessAreafix ();
   VOID   ProcessRaid ();
   USHORT RemoveAll (PSZ address);
   USHORT RemoveArea (PSZ address, PSZ area);
   VOID   Rescan (PSZ pszEchoTag, PSZ pszAddress, USHORT MaxMsgs = 0);
   USHORT SetInPacketPwd (PSZ address, PSZ pwd);
   USHORT SetOutPacketPwd (PSZ address, PSZ pwd);
   USHORT SetPacker (PSZ Cmd);
   USHORT SetPacketPwd (PSZ address, PSZ pwd);
   USHORT SetPwd (PSZ address, PSZ pwd);
   USHORT SetSessionPwd (PSZ address, PSZ pwd);
   VOID   UpdateAreasBBS ();

private:
   class  TNodes *Nodes;
   class  TEchoLink *EchoLink;
   class  TFilechoLink *FileEchoLink;
   class  TMsgData *Data;
   class  TFileData *FileData;
   class  TCollection Text;
};

// --------------------------------------------------------------------------

VOID CompileNodelist (USHORT force);

// ----------------------------------------------------------------------------

class TPMStatus : public TStatus
{
public:
#if defined(__OS2__) || defined(__NT__)
   TPMStatus (HWND hwnd);
#else
   TPMStatus ();
#endif
   ~TPMStatus ();

   VOID   Clear ();
   VOID   SetLine (USHORT line, PSZ text, ...);

private:
#if defined(__OS2__) || defined(__NT__)
   HWND   hwndList;
#endif
};

class TPMLog : public TLog
{
public:
#if defined(__OS2__) || defined(__NT__)
   TPMLog (HWND hwnd);
#else
   TPMLog ();
#endif
   ~TPMLog ();

   USHORT First;
   VOID   Write (PSZ pszFormat, ...);

private:
#if defined(__OS2__) || defined(__NT__)
   HWND   hwndList;
#elif defined(__DOS__) || defined(__LINUX__)
   CXLWIN window;
#endif
};

#if defined(__OS2__) || defined(__NT__)
#define WMU_ADDLOGITEM              1
#define WMU_ADDMODEMITEM            2
#define WMU_SETSTATUSLINE           3
#define WMU_SETSTATUSLINE0          4
#define WMU_SETSTATUSLINE1          5
#define WMU_REFRESHOUTBOUND         6
#define WMU_ADDOUTBOUNDLINE         7
#define WMU_CLEAROUTBOUND           8
#define WMU_REPLACEOUTBOUNDLINE     9

#if defined(__OS2__)
extern HAB  hab;
extern HWND hwndMainFrame, hwndMainClient;
extern FILEDLG fild;
extern HEV  hCfgEvent;
#elif defined(__NT__)
extern HINSTANCE hinst;
extern HWND hwndMainClient, hwndMainList;
extern HWND hwndModemClient, hwndModemList;
extern HWND hwndProgressDlg;
#endif

class CAddressDlg : public CDialog
{
public:
   CAddressDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Change ();
   VOID   Remove ();
   VOID   SelChanged ();
};

class CAnswerDlg : public CDialog
{
public:
   CAnswerDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CAreafixDlg : public CDialog
{
public:
   CAreafixDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CBBSGeneralDlg : public CDialog
{
public:
   CBBSGeneralDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CCommandsDlg : public CDialog
{
public:
   CCommandsDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CDirectoriesDlg : public CDialog
{
public:
   CDirectoriesDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CEventsDlg : public CDialog
{
public:
   CEventsDlg (HWND p_hWnd);
   ~CEventsDlg ();

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   class  TEvents *Data;
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Delete ();
   VOID   DisplayData ();
   VOID   List ();
   VOID   Next ();
   VOID   Previous ();
   VOID   ReadData ();
};

class CExternalDlg : public CDialog
{
public:
   CExternalDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CFaxOptDlg : public CDialog
{
public:
   CFaxOptDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CFileDlg : public CDialog
{
public:
   CFileDlg (HWND p_hWnd);
   ~CFileDlg ();

   VOID   OnAdd ();
   VOID   OnDelete ();
   VOID   OnHelp ();
   USHORT OnInitDialog ();
   VOID   OnInsert ();
   VOID   OnNext ();
   VOID   OnOK ();
   VOID   OnPrevious ();
   VOID   Security ();

private:
   class  TFileData *Data;
   DECLARE_MESSAGE_MAP()

   VOID   DisplayData ();
   VOID   Links ();
   VOID   List ();
   VOID   Move ();
   VOID   ReadData ();
   VOID   Search ();
};

class CGeneralDlg : public CDialog
{
public:
   CGeneralDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CHardwareDlg : public CDialog
{
public:
   CHardwareDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CInternetDlg : public CDialog
{
public:
   CInternetDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CLimitsDlg : public CDialog
{
public:
   CLimitsDlg (HWND p_hWnd);
   virtual ~CLimitsDlg ();

   class  TLimits *Limits;

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   DECLARE_MESSAGE_MAP ()

   VOID   DisplayData ();
   VOID   Delete ();
   VOID   Next ();
   VOID   Previous ();
   VOID   Add ();
};

class CMailprocDlg : public CDialog
{
public:
   CMailprocDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CMenuDlg : public CDialog
{
public:
   CMenuDlg (HWND p_hWnd);
   ~CMenuDlg ();

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   CHAR   FullFile[256];
#if defined(__OS2__)
   FILEDLG fild;
#elif defined(__NT__)
   OPENFILENAME OpenFileName;
#endif
   class  TMenu *Menu;

   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Color ();
   VOID   Command ();
   VOID   Delete ();
   VOID   DisplayData ();
   VOID   HilightColor ();
   VOID   Insert ();
   VOID   List ();
   VOID   Next ();
   VOID   Previous ();
   VOID   Prompt ();
   VOID   ReadData ();
   VOID   Security ();
};

class CMessageDlg : public CDialog
{
public:
   CMessageDlg (HWND p_hWnd);
   ~CMessageDlg ();

   VOID   OnAdd ();
   VOID   OnDelete ();
   VOID   OnHelp ();
   USHORT OnInitDialog ();
   VOID   OnInsert ();
   VOID   OnNext ();
   VOID   OnOK ();
   VOID   OnPrevious ();

private:
   class  TMsgData *Data;
   DECLARE_MESSAGE_MAP()

   VOID   DisplayData ();
   VOID   Links ();
   VOID   List ();
   VOID   Move ();
   VOID   ReadData ();
   VOID   Search ();
   VOID   Security ();
};

class CMiscDlg : public CDialog
{
public:
   CMiscDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CNewUserDlg : public CDialog
{
public:
   CNewUserDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
   VOID   NewUserSecurity ();

private:
   DECLARE_MESSAGE_MAP();

   UCHAR  GetSelection (USHORT id, USHORT Three);
};

class CNodelistDlg : public CDialog
{
public:
   CNodelistDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Edit ();
   VOID   Remove ();
};

class CNodesDlg : public CDialog
{
public:
   CNodesDlg (HWND p_hWnd);
   ~CNodesDlg ();

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   class  TNodes *Data;
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Copy ();
   VOID   Delete ();
   VOID   DisplayData ();
   VOID   List ();
   VOID   Next ();
   VOID   Other ();
   VOID   Previous ();
   VOID   ReadData ();
   VOID   Security ();
   VOID   NodeTic ();
   VOID   NodeEcho ();
};

class CNodeFlagsDlg : public CDialog
{
public:
   CNodeFlagsDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Change ();
   VOID   Remove ();
};

class COfflineDlg : public CDialog
{
public:
   COfflineDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class COkFileDlg : public CDialog
{
public:
   COkFileDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Change ();
   VOID   Remove ();
   VOID   SelChanged ();
};

class COriginDlg : public CDialog
{
public:
   COriginDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   class  TCollection Text;
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   ItemSelected ();
   VOID   Remove ();
   VOID   Replace ();
};

class CPackerDlg : public CDialog
{
public:
   CPackerDlg (HWND p_hWnd);
   ~CPackerDlg ();

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   class  TPacker *Data;
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Delete ();
   VOID   DisplayData ();
   VOID   List ();
   VOID   Next ();
   VOID   Previous ();
   VOID   ReadData ();
};

class CProtocolDlg : public CDialog
{
public:
   CProtocolDlg (HWND p_hWnd);
   ~CProtocolDlg ();

   VOID   OnAdd ();
   VOID   OnDelete ();
   VOID   OnHelp ();
   USHORT OnInitDialog ();
   VOID   OnNext ();
   VOID   OnOK ();
   VOID   OnPrevious ();

private:
   class  TProtocol *Data;
   DECLARE_MESSAGE_MAP()

   VOID   DisplayData ();
   VOID   List ();
   VOID   ReadData ();
};

class CRaidDlg : public CDialog
{
public:
   CRaidDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CSiteInfoDlg : public CDialog
{
public:
   CSiteInfoDlg (HWND p_hWnd);

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();
};

class CTranslationDlg : public CDialog
{
public:
   CTranslationDlg (HWND p_hWnd);
   ~CTranslationDlg ();

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   void *Data;
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Copy ();
   VOID   Delete ();
   VOID   DisplayData ();
   VOID   List ();
   VOID   Next ();
   VOID   Previous ();
   VOID   ReadData ();
};

class CUserDlg : public CDialog
{
public:
   CUserDlg (HWND p_hWnd);
   ~CUserDlg ();

   USHORT OnInitDialog ();
   VOID   OnHelp ();
   VOID   OnOK ();

private:
   class  TUser *Data;
   DECLARE_MESSAGE_MAP ()

   VOID   Add ();
   VOID   Delete ();
   VOID   DisplayData ();
   VOID   List ();
   VOID   Next ();
   VOID   Other ();
   VOID   Password ();
   VOID   Previous ();
   VOID   ReadData ();
   VOID   Search ();
   VOID   Security ();
};

#elif defined(__DOS__) || defined(__LINUX__)

VOID DisplayButton (USHORT y, USHORT x, CHAR *Text, USHORT Shadow = BLACK|_LGREY);
USHORT MessageBox (PSZ Caption, PSZ Text);

USHORT CAddressDlg ();
USHORT CAnswerDlg ();
USHORT CAreafixDlg ();
USHORT CBBSGeneralDlg ();
USHORT CCommandsDlg ();
USHORT CCompressorDlg ();
USHORT CDirectoriesDlg ();
USHORT CEventDlg ();
USHORT CExternalProcDlg ();
USHORT CFaxDlg ();
USHORT CFileDlg ();
USHORT CGeneralOptDlg ();
USHORT CHardwareDlg ();
USHORT CInternetDlg ();
USHORT CMailerMiscDlg ();
USHORT CMailProcessingDlg ();
USHORT CMenuEditorDlg (PSZ pszFile);
USHORT CMessageDlg ();
USHORT CNewUsersDlg ();
USHORT CNodelistDlg ();
VOID   CNodeFlagsDlg ();
USHORT CNodesDlg ();
USHORT COfflineDlg ();
USHORT CSiteInfoDlg ();
USHORT CUserDlg ();

#endif

