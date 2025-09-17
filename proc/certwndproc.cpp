#include "certwndproc.hpp"
#include "mainwndproc.hpp"
#include "functions.hpp"

LRESULT CALLBACK CertificatesWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND ICCViewer, ICCWnd;
    switch (msg)
    {
        case WM_CREATE:
        {
            ICCViewer = CreateWindowExA(0, "BUTTON", "ICC", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL); // decorative window
            ICCWnd = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            break;
        }
        case WM_SIZE:
        {
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            MoveWindow(ICCViewer, 20, 20, hParentWindowSize.right - 40, (hParentWindowSize.bottom / 2) - 40, TRUE);
            MoveWindow(ICCWnd, 40, 40, hParentWindowSize.right - 80, (hParentWindowSize.bottom / 2) - 80, TRUE);
            break;
        }
        case ID_CERTSTAB_UPDATE:
        {
            if (cardCertDATAptr)
            {
                char fullBuffer[1024]{};
                char buffer[10];
                for (int i = 0; i < 194; i++)
                {
                    sprintf(buffer, "%02X ", cardCertDATAptr[i]);
                    strcat(fullBuffer, buffer);
                }
                SetWindowTextA(ICCWnd, (LPCSTR)fullBuffer);
                break;
            } else {
                SetWindowTextA(ICCWnd, "");
            }
            break;
        }
        default:
        {
            return DefWindowProc(hParentWindow, msg, wParam, lParam);
        }
    }
    return 0;
}