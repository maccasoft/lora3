
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.01 - Rev. 1
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    07/17/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_defs.h"

HANDLE hInstance;
HWND hwndFrame, hwndContainer;

long FAR PASCAL MainWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message) {
      case WM_CREATE: {
         RECT rc;

         GetWindowRect (hWnd, &rc);
         hwndContainer = CreateWindow ("LISTBOX", NULL, WS_CHILD|WS_VISIBLE, 0, 0, rc.right - rc.left + 1, rc.bottom - rc.top + 1, hWnd, NULL, hInstance, NULL);
         break;
      }

      case WM_DESTROY:
         PostQuitMessage (0);
         break;

      default:
         return (DefWindowProc (hWnd, message, wParam, lParam));
   }

   return (0L);
}

int PASCAL WinMain (HANDLE hInst, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   MSG msg;
   WNDCLASS wc;

   hInstance = hInst;

   if (!hPrevInstance) {
      wc.style = NULL;
      wc.lpfnWndProc = MainWndProc;
      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;
      wc.hInstance = hInstance;
      wc.hIcon = NULL;
      wc.hCursor = NULL;
      wc.hbrBackground = GetStockObject (WHITE_BRUSH);
      wc.lpszMenuName = "IDW_MAIN";
      wc.lpszClassName = "CONSOLE";

      if (!RegisterClass (&wc))
         return (FALSE);
   }

   if ((hwndFrame = CreateWindow ("CONSOLE", "LoraBBS/NT - System Console", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 240, NULL, NULL, hInstance, NULL)) != NULL) {
      ShowWindow (hwndFrame, nCmdShow);
      UpdateWindow (hwndFrame);

      SendMessage (hwndContainer, LB_ADDSTRING, 0, (LPARAM)"0\tLocal\t\t\tLocal Session");
      SendMessage (hwndContainer, LB_ADDSTRING, 0, (LPARAM)"1\tSerial\t\t\tSerial port ready to use");
   }

   while (GetMessage (&msg, NULL, NULL, NULL)) {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
   }

   return (msg.wParam);
}

