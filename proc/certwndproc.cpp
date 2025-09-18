#include "certwndproc.hpp"
#include "mainwndproc.hpp"
#include "functions.hpp"

void TextFillWnd(HWND& wnd, BYTE* ptr)
{
    char fullBuffer[1024]{};
    char buffer[10];
    for (int i = 0; i < 194; i++)
    {
        sprintf(buffer, "%02X ", ptr[i]);
        strcat(fullBuffer, buffer);
    }
    SetWindowTextA(wnd, (LPCSTR)fullBuffer);
}

LRESULT CALLBACK CertificatesWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND DecorationCard, DecorationCA, DecorationICC, DecorationIC, CardCWnd, CACWnd, ICSerialNumber, ICMR;
    char buffer[60]{};
    switch (msg)
    {
        case WM_CREATE:
        {
            DecorationCard = CreateWindowExA(0, "BUTTON", "Card Certificate", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL); // decorative window
            DecorationCA = CreateWindowExA(0, "BUTTON", "CA Certificate", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL); // decorative window
            DecorationICC = CreateWindowExA(0, "BUTTON", "ICC", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL); // decorative window
            DecorationIC = CreateWindowExA(0, "BUTTON", "IC", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL); // decorative window
            CardCWnd = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            CACWnd = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            ICSerialNumber = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            ICMR = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            break;
        }
        case WM_SIZE:
        {
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            MoveWindow(DecorationCard, 20, 20, hParentWindowSize.right - 40, 120, TRUE);
            MoveWindow(DecorationCA, 20, 180, hParentWindowSize.right - 40, 120, TRUE);
            MoveWindow(DecorationICC, 20, 360, (hParentWindowSize.right / 2) - 40, 200, TRUE);
            MoveWindow(DecorationIC, hParentWindowSize.right / 2, 360, (hParentWindowSize.right / 2) - 20, 200, TRUE);
            MoveWindow(CardCWnd, 40, 40, hParentWindowSize.right - 80, 80, TRUE);
            MoveWindow(CACWnd, 40, 200, hParentWindowSize.right - 80, 80, TRUE);
            MoveWindow(ICSerialNumber, hParentWindowSize.right / 2 + 10, 390, 400, 20, TRUE);
            MoveWindow(ICMR, hParentWindowSize.right / 2 + 10, 420, 400, 20, TRUE);
            break;
        }
        case ID_CERTSTAB_UPDATE:
        {
            if ((cardCertDATAptr) && (CACertDATAptr))
            {
                TextFillWnd(CardCWnd, cardCertDATAptr);
                TextFillWnd(CACWnd, CACertDATAptr);
                sprintf(buffer, "Serial number: %u", _byteswap_ulong(ICData.icSerialNumber));
                SetWindowTextA(ICSerialNumber, buffer);
                sprintf(buffer, "Manufacturing references: %02X %02X %02X %02X",
                    ICData.icManufacturingReference[0],
                    ICData.icManufacturingReference[1],
                    ICData.icManufacturingReference[2],
                    ICData.icManufacturingReference[3]);
                SetWindowTextA(ICMR, buffer);
            } else {
                SetWindowTextA(CardCWnd, "");
                SetWindowTextA(CACWnd, "");
                SetWindowTextA(ICSerialNumber, "");
                SetWindowTextA(ICMR, "");
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