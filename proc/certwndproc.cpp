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

void PrintClockStop(HWND& wnd, byte num)
{
    if (num == 1)
    {
        SetWindowTextA(wnd, "Clockstop allowed, no preferred level.");
    } else if (num == 3) {
        SetWindowTextA(wnd, "Clockstop allowed, high level preferred.");
    } else if (num == 5) {
        SetWindowTextA(wnd, "Clockstop allowed, low level preferred.");
    } else if (num == 0) {
        SetWindowTextA(wnd, "Clockstop not allowed.");
    } else if (num == 2) {
        SetWindowTextA(wnd, "Clockstop only allowed on high level.");
    } else if (num == 4) {
        SetWindowTextA(wnd, "Clockstop only allowed on low level.");
    }
}

LRESULT CALLBACK CertificatesWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND DecorationCard, DecorationCA, DecorationICC,
        DecorationIC, CardCWnd, CACWnd, ICSerialNumber, ICMR, ClockStop,
        ExtendedSerialNumber, CardApprovalNumber, PersonaliserID,
        ICAssemblerID, ICIdentifier;
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
            ClockStop = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            ExtendedSerialNumber = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            CardApprovalNumber = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            PersonaliserID = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            ICAssemblerID = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            ICIdentifier = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            break;
        }
        case WM_SIZE:
        {
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            MoveWindow(DecorationCard, 20, 20, hParentWindowSize.right - 40, 120, TRUE);
            MoveWindow(DecorationCA, 20, 180, hParentWindowSize.right - 40, 120, TRUE);
            MoveWindow(DecorationICC, 20, 360, (hParentWindowSize.right / 2) - 40, 220, TRUE);
            MoveWindow(DecorationIC, hParentWindowSize.right / 2, 360, (hParentWindowSize.right / 2) - 20, 220, TRUE);
            MoveWindow(CardCWnd, 40, 40, hParentWindowSize.right - 80, 80, TRUE);
            MoveWindow(CACWnd, 40, 200, hParentWindowSize.right - 80, 80, TRUE);
            MoveWindow(ICSerialNumber, hParentWindowSize.right / 2 + 10, 390, 400, 20, TRUE);
            MoveWindow(ICMR, hParentWindowSize.right / 2 + 10, 420, 400, 20, TRUE);
            MoveWindow(ClockStop, 30, 390, 400, 20, TRUE);
            MoveWindow(ExtendedSerialNumber, 30, 420, 400, 20, TRUE);
            MoveWindow(CardApprovalNumber, 30, 450, 400, 20, TRUE);
            MoveWindow(PersonaliserID, 30, 480, 400, 20, TRUE);
            MoveWindow(ICAssemblerID, 30, 510, 400, 20, TRUE);
            MoveWindow(ICIdentifier, 30, 540, 400, 20, TRUE);
            break;
        }
        case ID_CERTSTAB_UPDATE:
        {
            if ((gen1card.cardCertDATAptr) && (gen1card.CACertDATAptr))
            {
                TextFillWnd(CardCWnd, gen1card.cardCertDATAptr);
                TextFillWnd(CACWnd, gen1card.CACertDATAptr);
                sprintf(buffer, "Serial number: %u", (gen1card.ICDataptr[0] << 24) | (gen1card.ICDataptr[1] << 16) | (gen1card.ICDataptr[2] << 8) | gen1card.ICDataptr[3]);
                SetWindowTextA(ICSerialNumber, buffer);
                sprintf(buffer, "Manufacturing references: %u", (gen1card.ICDataptr[4] << 24) | (gen1card.ICDataptr[5] << 16) | (gen1card.ICDataptr[6] << 8) | gen1card.ICDataptr[7]);
                SetWindowTextA(ICMR, buffer);
                PrintClockStop(ClockStop, gen1card.ICCDataptr[0]);
                sprintf(buffer, "Extended serial number: %02X %02X %02X %02X %02X %02X %02X %02X",
                    gen1card.ICCDataptr[1], gen1card.ICCDataptr[2], gen1card.ICCDataptr[3], gen1card.ICCDataptr[4],
                    gen1card.ICCDataptr[5], gen1card.ICCDataptr[6], gen1card.ICCDataptr[7], gen1card.ICCDataptr[8]);
                SetWindowTextA(ExtendedSerialNumber, buffer);
                sprintf(buffer, "Card approval number: %02X %02X %02X %02X %02X %02X %02X %02X",
                    gen1card.ICCDataptr[9], gen1card.ICCDataptr[10], gen1card.ICCDataptr[11], gen1card.ICCDataptr[12],
                    gen1card.ICCDataptr[13], gen1card.ICCDataptr[14], gen1card.ICCDataptr[15], gen1card.ICCDataptr[16]);
                SetWindowTextA(CardApprovalNumber, buffer);
                sprintf(buffer, "Personaliser ID: %02X", gen1card.ICCDataptr[17]);
                SetWindowTextA(PersonaliserID, buffer);
                sprintf(buffer, "IC Assembler ID: %02X %02X %02X %02X %02X",
                    gen1card.ICCDataptr[18], gen1card.ICCDataptr[19], gen1card.ICCDataptr[20], gen1card.ICCDataptr[21], gen1card.ICCDataptr[22]);
                SetWindowTextA(ICAssemblerID, buffer);
                sprintf(buffer, "IC identifier: %02X %02X", gen1card.ICCDataptr[23], gen1card.ICCDataptr[24]);
                SetWindowTextA(ICIdentifier, buffer);
            } else {
                SetWindowTextA(CardCWnd, NULL);
                SetWindowTextA(CACWnd, NULL);
                SetWindowTextA(ICSerialNumber, NULL);
                SetWindowTextA(ICMR, NULL);
                SetWindowTextA(ClockStop, NULL);
                SetWindowTextA(ExtendedSerialNumber, NULL);
                SetWindowTextA(CardApprovalNumber, NULL);
                SetWindowTextA(PersonaliserID, NULL);
                SetWindowTextA(ICAssemblerID, NULL);
                SetWindowTextA(ICIdentifier, NULL);
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