
// ----------------------------------------------------------------------
// Common Presentation Classes - Version 1.00.1
// Copyright (c) 1996 by Marco Maccaferri. All rights reserved.
//
// History:
//    10/08/96 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "cpc.h"

#if defined(__OS2__)
MRESULT EXPENTRY CWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
#elif defined(__NT__)
LRESULT CALLBACK CWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
{
   int i;
   class CWnd *wndClass;
   CPC_MSGMAP_ENTRY *msgMap = NULL;

#if defined(__OS2__)
   wndClass = (class CWnd *)WinQueryWindowULong (hwnd, QWL_USER);
#elif defined(__NT__)
   wndClass = (class CWnd *)GetWindowLong (hwnd, GWL_USERDATA);
#endif

   if (wndClass != NULL)
      msgMap = wndClass->GetMessageMap ();

   switch (msg) {
      case WM_CREATE:
         break;

#if defined(__OS2__)
      case WM_ERASEBACKGROUND:
         return ((MRESULT)TRUE);
#endif

      case WM_COMMAND:
         for (i = 0; msgMap[i].pfn != NULL; i++) {
#if defined(__OS2__)
            if (msgMap[i].nMessage == msg) {
               if (msgMap[i].nID >= SHORT1FROMMP (mp1) && msgMap[i].nLastID <= SHORT1FROMMP (mp1)) {
#elif defined(__NT__)
            if (msgMap[i].nMessage == msg && msgMap[i].nCode == HIWORD (wParam)) {
               if (msgMap[i].nID >= LOWORD (wParam) && msgMap[i].nLastID <= LOWORD (wParam)) {
#endif
                  (wndClass->*msgMap[i].pfnw) ();
                  break;
               }
            }
         }
         return (0);
   }

#if defined(__OS2__)
   return (WinDefWindowProc (hwnd, msg, mp1, mp2));
#elif defined(__NT__)
   return (DefWindowProc (hwnd, msg, wParam, lParam));
#endif
}

BEGIN_MESSAGE_MAP (CWnd, CWnd)
END_MESSAGE_MAP ()

CWnd::CWnd ()
{
}

CWnd::~CWnd ()
{
}

USHORT CWnd::OnCreate ()
{
   return (TRUE);
}

// ----------------------------------------------------------------------

CFrameWnd::CFrameWnd ()
{
   int x, y, dx, dy;
   ULONG flFrame;

#if defined(__OS2__)
   dy = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN) / 2;
   dx = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN) / 2;
   y = (WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN) - dy) / 2;
   x = (WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN) - dx) / 2;
#elif defined(__NT__)
   dy = GetSystemMetrics (SM_CYSCREEN) / 2;
   dx = GetSystemMetrics (SM_CXSCREEN) / 2;
   y = (GetSystemMetrics (SM_CYSCREEN) - dy) / 2;
   x = (GetSystemMetrics (SM_CXSCREEN) - dx) / 2;
#endif

   cWnd.cbSize = sizeof (CWND_DATA);
   cWnd.Wnd = this;

   flFrame = (FCF_TASKLIST|FCF_TITLEBAR|FCF_SYSMENU|FCF_MINMAX|FCF_SIZEBORDER|FCF_NOBYTEALIGN);
   if ((hwndMainFrame = WinCreateStdWindow (HWND_DESKTOP, 0, &flFrame, "CWINAPP_WINDOW", NULL, 0, NULLHANDLE, 256, &hwndMainClient)) != NULLHANDLE) {
      WinSetWindowULong (hwndMainClient, QWL_USER, (ULONG)this);

      WinSetWindowText (hwndMainFrame, "Common Presentation Classes Window");

      if (OnCreate () == TRUE)
         WinSetWindowPos (hwndMainFrame, NULLHANDLE, x, y, dx, dy, SWP_SIZE|SWP_MOVE|SWP_SHOW|SWP_ACTIVATE);
      else {
         WinDestroyWindow (hwndMainFrame);
         hwndMainFrame = NULL;
      }
   }
}

CFrameWnd::~CFrameWnd ()
{
   if (hwndMainFrame != NULLHANDLE)
      WinDestroyWindow (hwndMainFrame);
}

// ----------------------------------------------------------------------

CWinApp::CWinApp ()
{
   m_pMainWnd = NULL;

   if ((hab = WinInitialize (0)) != 0) {
      if ((hmq = WinCreateMsgQueue (hab, 0)) != 0)
         WinRegisterClass (hab, "CWINAPP_WINDOW", CWndProc, CS_CLIPCHILDREN|CS_SIZEREDRAW|CS_MOVENOTIFY, 0);
   }
}

CWinApp::~CWinApp ()
{
   if (m_pMainWnd != NULL)
      delete m_pMainWnd;

   if (hab != 0) {
      if (hmq != 0)
         WinDestroyMsgQueue (hmq);
      WinTerminate (hab);
   }
}

USHORT CWinApp::InitInstance ()
{
   m_pMainWnd = new CFrameWnd;

   return (TRUE);
}

VOID CWinApp::Run ()
{
   InitInstance ();

   while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
      WinDispatchMsg (hab, &qmsg);
}

