#include "drawwndproc.hpp"
#include "mainwndproc.hpp"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

LRESULT CALLBACK DrawWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HDC memDC;
    static HBITMAP bitmap;
    static HFONT hFont;
    static SCROLLINFO si;
    static DailyWrapper* CurrentDay = nullptr;
    static ActivityData pData{};
    static DrawingBrush* painter;
    switch (msg)
    {
        case WM_CREATE:
        {
            HDC hdc = GetDC(hParentWindow);

            memDC = CreateCompatibleDC(hdc);
            bitmap = CreateCompatibleBitmap(hdc, 800, 14400);
            SelectObject(memDC, bitmap);
            ReleaseDC(hParentWindow, hdc);

            hFont = CreateFont(
                10,
                5, 0, 0, FW_THIN,
                FALSE, FALSE, FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE,
                TEXT("Arial")
            );

            painter = new DrawingBrush(memDC, hParentWindow);

            SelectObject(memDC, hFont); // we set font of our memDC

            RedrawBitMap(0, 0, 800, 14400, memDC, hParentWindow); // one time static width

            si = {0};
            si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
            si.nMax = 14400;
            si.nMin = 0;
            si.nPage = 800;
            si.nPos = 0;
            si.cbSize = sizeof(si);
            SetScrollInfo(hParentWindow, SB_VERT, &si, TRUE);
            
            break;
        }
        case WM_VSCROLL:
        {
            GetScrollInfo(hParentWindow, SB_VERT, &si);
            int prevPos = si.nPos;
            switch (LOWORD(wParam))
            {
                case SB_THUMBTRACK: si.nPos = (HIWORD(wParam) / 800) * 800; break;
                case SB_LINEUP: si.nPos -= 10; break;
                case SB_LINEDOWN: si.nPos += 10; break;
            }

            SetScrollInfo(hParentWindow, SB_VERT, &si, TRUE);

            if (prevPos != si.nPos)
            {
                InvalidateRect(hParentWindow, NULL, TRUE);
            }

            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hParentWindow, &ps);
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            
            int scrollOffset = MIN(si.nPos, 14400 - hParentWindowSize.bottom);

            BitBlt(hdc, 0, 0, hParentWindowSize.right, hParentWindowSize.bottom, memDC, 0, scrollOffset, SRCCOPY);

            EndPaint(hParentWindow, &ps);
            break;
        }
        case WM_SIZE:
        {
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            int width = hParentWindowSize.right;
            int height = hParentWindowSize.bottom;

            DeleteObject(bitmap); // so it doesnt pollute memory

            HDC hdc = GetDC(hParentWindow);
            bitmap = CreateCompatibleBitmap(hdc, width, 14400);
            SelectObject(memDC, bitmap);
            ReleaseDC(hParentWindow, hdc);

            RedrawBitMap(0, 0, width, 14400, memDC, hParentWindow);
            HWND hParent = GetParent(hParentWindow);
            pData = {0,0,0,0,0,0};

            if (CurrentDay && _byteswap_ushort(CurrentDay->header.currLength) > 14)
            {
                painter->DrawOneDay(CurrentDay->ptr, _byteswap_ushort(CurrentDay->header.currLength) - 14, pData);
                SendMessage(hParent, ID_ACTIVITIESTAB_UPDATE_ACTIVITIES, 0, (LPARAM)&pData);
            }
            SendMessage(hParent, ID_ACTIVITIESTAB_UPDATE_ACTIVITIES, 0, (LPARAM)&pData);
            InvalidateRect(hParentWindow, NULL, TRUE);
            break;
        }
        case WM_DESTROY:
        {
            // for correctly releasing/deleting memory
            DeleteObject(bitmap);
            DeleteDC(memDC);
            DeleteObject(hFont);
            delete painter;
            break;
        }
        case ID_ACTIVITIESTAB_DRAW_DAY:
        {
            CurrentDay = (DailyWrapper*)lParam;
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            int width = hParentWindowSize.right - hParentWindowSize.left;
            LPARAM sizeParam = MAKELPARAM(width, 14400);
            SendMessage(hParentWindow, WM_SIZE, 0, sizeParam);
            break;
        }
        case ID_ACTIVITIESTAB_RESET:
        {
            CurrentDay = nullptr;
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            LPARAM sizeParam = MAKELPARAM(hParentWindowSize.right, 14400);
            SendMessage(hParentWindow, WM_SIZE, 0, sizeParam);
            break;
        }
        default:
        {
            return DefWindowProc(hParentWindow, msg, wParam, lParam);
        }
    }
    return 0;
}