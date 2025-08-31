#include "activitieswndproc.hpp"
#include "structs.hpp"
#include "mainwndproc.hpp"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

LRESULT CALLBACK TreeWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        default:
        {
            return DefWindowProc(hParentWindow, msg, wParam, lParam);
        }
    }
    return 0;
}

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
        default:
        {
            return DefWindowProc(hParentWindow, msg, wParam, lParam);
        }
    }
    return 0;
}

LRESULT CALLBACK ActivitiesWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hDraw, hTree, Day, Rest, Administration, Work, Driving, hUTC;
    static Activities activities(nullptr, 0, 0);
    static LPCSTR Time;
    static LPCWSTR RestTime, AdministrationTime, WorkTime, DrivingTime;
    char timeBuffer[17];
    wchar_t timeBufferW[20];
    switch (msg)
    {
        case WM_CREATE:
        {
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);

            WNDCLASS hDrawAttr = {0};
            SetWindowAttr(hDrawAttr, (HBRUSH)(COLOR_WINDOW+1), IDC_ARROW, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, DrawWndProc, TEXT("DrawWindow"), NULL, CS_HREDRAW | CS_VREDRAW);
            RegisterClass(&hDrawAttr);

            hDraw = CreateWindow(TEXT("DrawWindow"), TEXT("DrawWindow"), WS_CHILD | WS_VISIBLE | WS_BORDER,
                0, 0, 0, 0, hParentWindow, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            WNDCLASS hTreeAttr = {0};
            SetWindowAttr(hTreeAttr, (HBRUSH)(COLOR_WINDOW+1), IDC_ARROW, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, TreeWndProc, TEXT("TreeWindow"), NULL, CS_HREDRAW | CS_VREDRAW);
            RegisterClass(&hTreeAttr);

            hTree = CreateWindow(TEXT("TreeWindow"), TEXT("TreeWindow"), WS_CHILD | WS_VISIBLE | WS_BORDER,
                0, 0, 0, 0, hParentWindow, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            Day = CreateWindowA("STATIC", Time, WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0, hParentWindow, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            Rest = CreateWindowW(L"STATIC", RestTime, WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0, hParentWindow, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            Administration = CreateWindowW(L"STATIC", AdministrationTime, WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0, hParentWindow, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            Work = CreateWindowW(L"STATIC", WorkTime, WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0, hParentWindow, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            Driving = CreateWindowW(L"STATIC", DrivingTime, WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0, hParentWindow, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            hUTC = CreateWindow(UPDOWN_CLASS, NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_NUMBER,
                0, 0, 0, 0, hParentWindow, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            break;
        }
        case WM_SIZE:
        {
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            int width = hParentWindowSize.right;
            int height = hParentWindowSize.bottom;
            MoveWindow(Day, width / 2 + 10, height - 80, 100, 20, TRUE);
            MoveWindow(hUTC, width - 40, height - 80, 30, 50, TRUE);
            MoveWindow(Driving, width / 2 + 160, height - 40, 60, 20, TRUE);
            MoveWindow(Work, width / 2 + 160, height - 60, 60, 20, TRUE);
            MoveWindow(Administration, width / 2 + 30, height - 40, 60, 20, TRUE);
            MoveWindow(Rest, width / 2 + 30, height - 60, 60, 20, TRUE);
            MoveWindow(hTree, 10, 10, width / 2 - 20, height - 20, TRUE);
            MoveWindow(hDraw, width / 2 + 10, 10, width / 2 - 20, height - 100, TRUE);
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hParentWindow, &ps);
            HICON hRestIcon = (HICON)LoadImage(NULL, L"rest.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
            HICON hAdministrationIcon = (HICON)LoadImage(NULL, L"administration.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
            HICON hWorkIcon = (HICON)LoadImage(NULL, L"work.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
            HICON hDrivingIcon = (HICON)LoadImage(NULL, L"driving.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            DrawIconEx(hdc, hParentWindowSize.right / 2 + 10, hParentWindowSize.bottom - 60, hRestIcon, 16, 16, 0, NULL, DI_IMAGE | DI_MASK);
            DrawIconEx(hdc, hParentWindowSize.right / 2 + 10, hParentWindowSize.bottom - 40, hAdministrationIcon, 16, 16, 0, NULL, DI_IMAGE | DI_MASK);
            DrawIconEx(hdc, hParentWindowSize.right / 2 + 140, hParentWindowSize.bottom - 60, hWorkIcon, 16, 16, 0, NULL, DI_IMAGE | DI_MASK);
            DrawIconEx(hdc, hParentWindowSize.right / 2 + 140, hParentWindowSize.bottom - 40, hDrivingIcon, 16, 16, 0, NULL, DI_IMAGE | DI_MASK);
            EndPaint(hParentWindow, &ps);
            break;
        }
        case ID_ACTIVITIESTAB_UPDATE:
        {
            uint16_t end = (activitiesDATAptr[0] << 8) | activitiesDATAptr[1];
            uint16_t start = (activitiesDATAptr[2] << 8) | activitiesDATAptr[3];
            activities.readActivities(activitiesDATAptr+4, end, start);
            SendMessage(hParentWindow, ID_ACTIVITIESTAB_UPDATE_TIME, 0, 0);
            SendMessage(hDraw, ID_ACTIVITIESTAB_DRAW_DAY, 0, (LPARAM)activities.GetNextPtrWrp()); // we send pointer to the data that will be drawn
            break;
        }
        case ID_ACTIVITIESTAB_UPDATE_TIME:
        {
            if (activities.GetNextPtrWrp())
            {
                int epoch = _byteswap_ulong(activities.GetNextPtrWrp()->header.time), year = 1970, month = 1, day = 1;
                DateStamp(epoch, year, month, day);
                sprintf(timeBuffer, "%04i-%02i-%02i", year, month, day);
                Time = timeBuffer;
                SetWindowTextA(Day, Time);
            }
            break;
        }
        case ID_ACTIVITIESTAB_UPDATE_ACTIVITIES:
        {
            ActivityData* ptrToActivitiesCounter = (ActivityData*)lParam;
            wsprintfW(timeBufferW, L"%02i:%02i", ptrToActivitiesCounter->rest / 60, ptrToActivitiesCounter->rest % 60);
            RestTime = timeBufferW;
            SetWindowTextW(Rest, RestTime);
            wsprintfW(timeBufferW, L"%02i:%02i", ptrToActivitiesCounter->administration / 60, ptrToActivitiesCounter->administration % 60);
            AdministrationTime = timeBufferW;
            SetWindowTextW(Administration, AdministrationTime);
            wsprintfW(timeBufferW, L"%02i:%02i", ptrToActivitiesCounter->work / 60, ptrToActivitiesCounter->work % 60);
            WorkTime = timeBufferW;
            SetWindowTextW(Work, WorkTime);
            wsprintfW(timeBufferW, L"%02i:%02i", ptrToActivitiesCounter->driving / 60, ptrToActivitiesCounter->driving % 60);
            DrivingTime = timeBufferW;
            SetWindowTextW(Driving, DrivingTime);
            break;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_PRIOR:
                {
                    activities.index--;
                    SendMessage(hDraw, ID_ACTIVITIESTAB_DRAW_DAY, 0, (LPARAM)activities.GetNextPtrWrp());
                    SendMessage(hParentWindow, ID_ACTIVITIESTAB_UPDATE_TIME, 0, 0);
                    break;
                }
                case VK_NEXT:
                {
                    activities.index++;
                    SendMessage(hDraw, ID_ACTIVITIESTAB_DRAW_DAY, 0, (LPARAM)activities.GetNextPtrWrp());
                    SendMessage(hParentWindow, ID_ACTIVITIESTAB_UPDATE_TIME, 0, 0);
                    break;
                }
            }
            break;
        }
        case WM_NOTIFY:
        {
            NMHDR* nmhdr = (NMHDR*)lParam;

            if (nmhdr->code == UDN_DELTAPOS)
            {
                NMUPDOWN* nmUpDown = (NMUPDOWN*)lParam;
                UTC += nmUpDown->iDelta;
                if (UTC < -12) UTC = 14;
                if (UTC > 14) UTC = -12;
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