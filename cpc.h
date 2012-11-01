
// ----------------------------------------------------------------------
// Common Presentation Classes - Version 1.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/08/96 - Initial coding.
// ----------------------------------------------------------------------

#if defined(__OS2__)
#define IDOK            1
#define IDCANCEL        2
#define IDHELP          3

#define IDYES           MBID_YES
#endif

#if defined(__NT__)
#include <commctrl.h>

#define WM_CONTROL      WM_COMMAND

#define CN_ENTER        LBN_DBLCLK
#define LN_SELECT       LBN_SELCHANGE
#endif

#define LVC_LEFT        0
#define LVC_CENTER      1
#define LVC_RIGHT       2

#if defined(__OS2__)

typedef struct {
   PSZ    pszData;
   CHAR   Data[256];
} COLUMNDATA;

typedef struct {
   RECORDCORE RecordCore;
   COLUMNDATA Column[1];
} LV_LISTDATA, *LV_PLISTDATA;

#endif

struct CPC_MSGMAP_ENTRY;

#define DECLARE_MESSAGE_MAP() \
    static CPC_MSGMAP_ENTRY _messageEntries[]; \
    virtual CPC_MSGMAP_ENTRY *GetMessageMap (VOID); \

class CWnd
{
public:
   virtual USHORT OnCreate (VOID);

   HWND   m_hWnd;
   UINT   m_msg;
#if defined(__OS2__)
   MPARAM m_mp1;
   MPARAM m_mp2;
#elif defined(__NT__)
   WPARAM m_wParam;
   LPARAM m_lParam;
#endif

   USHORT Create (int x, int y, int dx, int dy, PSZ title = NULL);
   int    MessageBox (PSZ text, PSZ caption, int button);
   VOID   SetWindowTitle (PSZ title);
   VOID   ShowWindow (int nCmdShow);

private:
   DECLARE_MESSAGE_MAP ()

#if defined(__OS2__)
   friend MRESULT EXPENTRY CFrameWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#elif defined(__NT__)
   friend LRESULT CALLBACK CFrameWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
};

class CWinApp
{
public:
   CWinApp (void);
   ~CWinApp (void);

#if defined(__NT__)
   int nCmdShow;
   HINSTANCE hinstCurrent;
   HINSTANCE hinstPrev;
#endif
   class  CWnd *m_pMainWnd;

   virtual USHORT InitInstance (VOID);

   VOID   Run (VOID);

private:
};

class CDialog : public CWnd
{
public:
   CDialog (PSZ dialogTemplate, HWND p_hWnd);
   virtual ~CDialog (void);

   virtual LONG   DoModal (VOID);
   virtual VOID   OnCancel (VOID);
   virtual VOID   OnHelp (VOID);
   virtual USHORT OnInitDialog (VOID);
   virtual VOID   OnOK (VOID);

protected:
   VOID   Center (VOID);
   VOID   EndDialog (USHORT Result);
   VOID   GetDlgItemText (int id, PSZ text, USHORT size);
   VOID   GetDlgItemText (int id, USHORT size, PSZ text);
   USHORT GetDlgItemTextLength (int id);
   VOID   SetFocus (int id);
   VOID   SetDlgItemText (int id, PSZ text);
   VOID   WinHelp (PSZ help_file, int topic_id, PSZ title = "Application Help");

   USHORT BM_QueryCheck (int id);
   VOID   BM_SetCheck (int id, USHORT value);
   VOID   CB_AddString (int id, PSZ value);
   VOID   CB_SelectItem (int id, USHORT value);
   USHORT CB_QuerySelection (int id);
   VOID   EM_SetTextLimit (int id, USHORT value);
   VOID   LM_AddString (int id, PSZ value);
   VOID   LM_DeleteAll (int id);
   VOID   LM_DeleteItem (int id, USHORT value);
   VOID   LM_SelectItem (int id, USHORT value);
   USHORT LM_QueryItemCount (int id);
   VOID   LM_QueryItemText (int id, USHORT value, USHORT size, PSZ text);
   USHORT LM_QuerySelection (int id);
   VOID   LM_SetItemText (int id, USHORT value, PSZ text);
   VOID   LVM_AllocateColumns (int id, int value);
   VOID   LVM_DeleteAll (int id);
   VOID   LVM_DeleteItem (int id, int item);
   VOID   LVM_InsertColumn (int id, PSZ text, int align);
   VOID   LVM_InsertItem (int id);
   VOID   LVM_InvalidateView (int id);
   int    LVM_QuerySelectedItem (int id);
   VOID   LVM_SelectItem (int id, int item);
   VOID   LVM_SetItemText (int id, int column, PSZ text);
   VOID   LVM_SetItemText (int id, int item, int column, PSZ text);
   int    LVM_QueryItemCount (int id);
   VOID   LVM_QueryItemText (int id, int item, int column, PSZ text);
   LONG   SPBM_QueryValue (int id);
   VOID   SPBM_SetCurrentValue (int id, LONG value);
   VOID   SPBM_SetLimits (int id, LONG highest, LONG lowest);

private:
   SHORT  lv_Selected;
   CHAR   dlgTemplate[64];
   HWND   owner_hWnd;
   HWND   help_hWnd;

   DECLARE_MESSAGE_MAP ()

#if defined(__OS2__)
   friend MRESULT EXPENTRY CDialogDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#elif defined(__NT__)
   friend BOOL CALLBACK CDialogDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
};

typedef void (CWnd::*CPC_PMSG)(void);   // routine to call (or special value)

struct CPC_MSGMAP_ENTRY {
   UINT nMessage;    // windows message
   UINT nCode;       // control code or WM_NOTIFY code
   UINT nID;         // control ID (or 0 for windows messages)
   UINT nLastID;     // used for entries specifying a range of control id's
   UINT nSig;        // signature type (action) or pointer to message #
   CPC_PMSG pfn;     // routine to call (or special value)
};

#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
    CPC_MSGMAP_ENTRY *theClass::GetMessageMap (VOID) \
        { return (&theClass::_messageEntries[0]); } \
    CPC_MSGMAP_ENTRY theClass::_messageEntries[] = \
    { \

#define END_MESSAGE_MAP() \
        {0, 0, 0, 0, 0, NULL } \
    }; \

#define ON_COMMAND(id, memberFxn) \
    { WM_COMMAND, 0, (UINT)id, (UINT)id, 0, (CPC_PMSG)memberFxn },

#define ON_CONTROL(wNotifyCode, id, memberFxn) \
    { WM_CONTROL, (UINT)wNotifyCode, (UINT)id, (UINT)id, 0, (CPC_PMSG)memberFxn },

#define ON_MESSAGE(message, memberFxn) \
    { message, 0, 0, 0, 0, (CPC_PMSG)memberFxn },

#define ON_NOTIFY(wNotifyCode, id, memberFxn) \
    { WM_NOTIFY, (UINT)wNotifyCode, (UINT)id, (UINT)id, 0, (CPC_PMSG)memberFxn }, \
    { WM_COMMAND, (UINT)wNotifyCode, (UINT)id, (UINT)id, 0, (CPC_PMSG)memberFxn },

