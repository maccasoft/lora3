
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
#include "cpc.h"

#if defined(__NT__)
extern HINSTANCE hinst;
#endif

typedef struct {
   USHORT cbSize;
   class CWnd *Wnd;
} CWND_DATA;

// ----------------------------------------------------------------------
// Frame window procedure
// ----------------------------------------------------------------------

#if defined(__OS2__)
MRESULT EXPENTRY CFrameWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
#elif defined(__NT__)
LRESULT CALLBACK CFrameWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

   if (wndClass != NULL) {
      wndClass->m_msg = msg;
#if defined(__OS2__)
      wndClass->m_mp1 = mp1;
      wndClass->m_mp2 = mp2;
#elif defined(__NT__)
      wndClass->m_wParam = wParam;
      wndClass->m_lParam = lParam;
#endif

      msgMap = wndClass->GetMessageMap ();
   }

   switch (msg) {
      case WM_CREATE: {
#if defined(__OS2__)
         CWND_DATA *cData = (CWND_DATA *)mp2;
#elif defined(__NT__)
         CWND_DATA *cData = (CWND_DATA *)lParam;
#endif

         wndClass = cData->Wnd;
         wndClass->m_hWnd = hwnd;
#if defined(__OS2__)
         WinSetWindowULong (hwnd, QWL_USER, (ULONG)wndClass);
#elif defined(__NT__)
         SetWindowLong (hwnd, GWL_USERDATA, (LONG)wndClass);
#endif

         wndClass->OnCreate ();
#if defined(__OS2__)
         break;
#elif defined(__NT__)
         return ((BOOL)TRUE);
#endif
      }

#if defined(__OS2__)
      case WM_CONTROL:
         for (i = 0; msgMap[i].pfn != NULL; i++) {
            if (msgMap[i].nMessage == msg) {
               if (msgMap[i].nID >= SHORT1FROMMP (mp1) && msgMap[i].nLastID <= SHORT1FROMMP (mp1)) {
                  if (msgMap[i].nCode == SHORT2FROMMP (mp1)) {
                     (wndClass->*msgMap[i].pfn) ();
                     break;
                  }
               }
            }
         }
         return (0);
#endif

#if defined(__NT__)
      case WM_NOTIFY: {
         LPNMHDR pnmh = (LPNMHDR)lParam;

         for (i = 0; msgMap[i].pfn != NULL; i++) {
            if (msgMap[i].nMessage == msg && msgMap[i].nCode == pnmh->code) {
               if (msgMap[i].nID >= pnmh->idFrom && msgMap[i].nLastID <= pnmh->idFrom) {
                  (wndClass->*msgMap[i].pfn) ();
                  break;
               }
            }
         }
         return (0);
      }
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
                  (wndClass->*msgMap[i].pfn) ();
                  break;
               }
            }
         }
         return (0);

#if defined(__NT__)
      case WM_CLOSE:
         EndDialog (hwnd, FALSE);
         break;
#endif

      default:
         if (msgMap != NULL) {
            for (i = 0; msgMap[i].pfn != NULL; i++) {
               if (msgMap[i].nMessage == msg) {
                  (wndClass->*msgMap[i].pfn) ();
                  return (0);
               }
            }
         }
         break;
   }

#if defined(__OS2__)
   return (WinDefWindowProc (hwnd, msg, mp1, mp2));
#elif defined(__NT__)
   return (DefWindowProc (hwnd, msg, wParam, lParam));
#endif
}

USHORT CWnd::OnCreate (VOID)
{
   return (TRUE);
}

// ----------------------------------------------------------------------
// Windowing API replacement
// ----------------------------------------------------------------------

USHORT CWnd::Create (int x, int y, int dx, int dy, PSZ title)
{
   USHORT RetVal = FALSE;
   CWND_DATA cWnd;
#if defined(__OS2__)
   FRAMECDATA fd;
   HWND hwndFrame;
#endif

   if (title == NULL)
      title = "CPC Main Window";

   cWnd.cbSize = sizeof (CWND_DATA);
   cWnd.Wnd = this;

#if defined(__OS2__)
   fd.cb = sizeof (fd);
   fd.flCreateFlags = FCF_TASKLIST|FCF_TITLEBAR|FCF_SYSMENU|FCF_MINMAX|FCF_SIZEBORDER|FCF_NOBYTEALIGN;
   fd.hmodResources = NULL;
   fd.idResources = 256;

   if ((hwndFrame = WinCreateWindow (HWND_DESKTOP, WC_FRAME, NULL, 0, 0, 0, 0, 0, NULL, HWND_TOP, fd.idResources, &fd, NULL)) != NULLHANDLE) {
      if ((m_hWnd = WinCreateWindow (hwndFrame, "CPC_MAIN_WINDOW", title, 0, 0, 0, 10, 10, hwndFrame, HWND_BOTTOM, FID_CLIENT, NULL, NULL)) != NULL) {
         WinSetWindowPos (hwndFrame, NULLHANDLE, x, y, dx, dy, SWP_SIZE|SWP_MOVE|SWP_SHOW|SWP_ACTIVATE);
         RetVal = TRUE;
      }
   }
   if ((m_hWnd = WinCreateWindow (HWND_DESKTOP, WC_FRAME, title, WS_VISIBLE, x, y, dx, dy, NULL, HWND_TOP, 1001, NULL, NULL)) != NULL)
      RetVal = TRUE;
#elif defined(__NT__)
   if ((m_hWnd = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW, "CPC_MAIN_WINDOW", title, WS_OVERLAPPEDWINDOW, x, y, dx, dy, NULL, NULL, hinst, &cWnd)) != NULL)
      RetVal = TRUE;
#endif

   return (RetVal);
}

int CWnd::MessageBox (PSZ text, PSZ caption, int button)
{
#if defined(__OS2__)
   return ((int)WinMessageBox (HWND_DESKTOP, m_hWnd, text, caption, 0, button|MB_MOVEABLE));
#elif defined(__NT__)
   return (::MessageBox (m_hWnd, text, caption, button));
#endif
}

VOID CWnd::SetWindowTitle (PSZ title)
{
#if defined(__OS2__)
   WinSetWindowText (m_hWnd, title);
#elif defined(__NT__)
   ::SetWindowText (m_hWnd, title);
#endif
}

VOID CWnd::ShowWindow (int nCmdShow)
{
#if defined(__OS2__)
   WinSetWindowPos (m_hWnd, NULLHANDLE, 0, 0, 0, 0, SWP_SHOW|SWP_ACTIVATE);
   nCmdShow = nCmdShow;
#elif defined(__NT__)
   ::ShowWindow (m_hWnd, nCmdShow);
#endif
}

BEGIN_MESSAGE_MAP (CWnd, CWnd)
END_MESSAGE_MAP ()

