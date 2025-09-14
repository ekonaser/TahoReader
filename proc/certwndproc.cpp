#include "certwndproc.hpp"
#include "mainwndproc.hpp"
#include "functions.hpp"

LRESULT CALLBACK CertificatesWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND CardCViewer;
    switch (msg)
    {
        case WM_CREATE:
        {
            CardCViewer = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            break;
        }
        case WM_SIZE:
        {
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            MoveWindow(CardCViewer, 20, 20, hParentWindowSize.right - 40, (hParentWindowSize.bottom / 2) - 40, TRUE);
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
                SetWindowTextA(CardCViewer, (LPCSTR)fullBuffer);
                break;
            } else {
                SetWindowTextA(CardCViewer, "");
            }
        }
        default:
        {
            return DefWindowProc(hParentWindow, msg, wParam, lParam);
        }
    }
    return 0;
}