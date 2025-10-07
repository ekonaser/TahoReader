#include "activitieswndproc.hpp"
#include "structs.hpp"
#include "mainwndproc.hpp"
#include "drawwndproc.hpp"

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

LRESULT CALLBACK ActivitiesWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hDraw, hTree, Day, Rest, Administration, Work, Driving, hUTCLButton, hUTCRButton, hUTCString;
    static Activities activities(nullptr, 0, 0);
    static LPCSTR Time;
    static LPCWSTR RestTime, AdministrationTime, WorkTime, DrivingTime, hUTCTime;
    static ActivitiesTree* Tree;
    char timeBuffer[10];
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

            hTree = CreateWindowEx(0, WC_TREEVIEW, L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | WS_TABSTOP | TVS_SHOWSELALWAYS,
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
            
            hUTCLButton = CreateWindow(TEXT("BUTTON"), TEXT("<"), WS_CHILD | WS_BORDER | WS_VISIBLE | ES_NUMBER,
                0, 0, 0, 0, hParentWindow, (HMENU)ID_ACTIVITIESTAB_LBUTTON, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            hUTCRButton = CreateWindow(TEXT("BUTTON"), TEXT(">"), WS_CHILD | WS_BORDER | WS_VISIBLE | ES_NUMBER,
                0, 0, 0, 0, hParentWindow, (HMENU)ID_ACTIVITIESTAB_RBUTTON, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            hUTCString = CreateWindowW(L"STATIC", hUTCTime, WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0, hParentWindow, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            break;
        }
        case WM_SIZE:
        {
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            int width = hParentWindowSize.right;
            int height = hParentWindowSize.bottom;
            MoveWindow(Day, width / 2 + 10, height - 80, 90, 20, TRUE);
            MoveWindow(hUTCLButton, width / 2 + 100, height - 80, 20, 20, TRUE);
            MoveWindow(hUTCString, width / 2 + 130, height - 80, 60, 20, TRUE);
            MoveWindow(hUTCRButton, width / 2 + 190, height - 80, 20, 20, TRUE);
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
            activities.~Activities();
            int end = (gen1card.activitiesDATAptr[0] << 8) | gen1card.activitiesDATAptr[1];
            int start = (gen1card.activitiesDATAptr[2] << 8) | gen1card.activitiesDATAptr[3];
            activities.readActivities(gen1card.activitiesDATAptr+4, end, start);

            if (Tree) delete Tree;
            Tree = new ActivitiesTree(hTree, activities);
            Tree->CreateTree();
            Tree->UpdateTreeVehicles(vehicles.ptrWrp);
            
            SendMessage(hParentWindow, ID_ACTIVITIESTAB_UPDATE_TIME, 0, 0);
            SendMessage(hDraw, ID_ACTIVITIESTAB_DRAW_DAY, 0, (LPARAM)activities.GetNextPtrWrp()); // we send pointer to the data that will be drawn
            break;
        }
        case ID_ACTIVITIESTAB_RESET:
        {
            activities.~Activities();
            SendMessage(hTree, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
            break;
        }
        case ID_ACTIVITIESTAB_UPDATE_TIME:
        {
            if (activities.GetNextPtrWrp())
            {
                DateStamp(_byteswap_ulong(activities.GetNextPtrWrp()->header.time), timeBuffer);
                Time = timeBuffer;
                SetWindowTextA(Day, Time);
            } else {
                Time = {};
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
            if (UTC < 0) {
                wsprintfW(timeBufferW, L"UTC %i", UTC);
            } else {
                wsprintfW(timeBufferW, L"UTC +%i", UTC);
            }
            hUTCTime = timeBufferW;
            SetWindowTextW(hUTCString, hUTCTime);
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
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case ID_ACTIVITIESTAB_LBUTTON:
                {
                    UTC -= 1;
                    break;
                }
                case ID_ACTIVITIESTAB_RBUTTON:
                {
                    UTC += 1;
                    break;
                }
            }
            if (UTC < -12) UTC = 14;
            if (UTC > 14) UTC = -12;
            SendMessage(hDraw, ID_ACTIVITIESTAB_DRAW_DAY, 0, (LPARAM)activities.GetNextPtrWrp());
            break;
        }
        case WM_NOTIFY:
        {
            LPNMHDR nmhdr = (LPNMHDR)lParam;
            if (nmhdr->hwndFrom == hTree) // if change came from hTree window
            {
                switch (nmhdr->code)
                {
                    case TVN_SELCHANGED:
                    {
                        NMTREEVIEW* ptv = (NMTREEVIEW*)lParam; // pointer to the notification message
                        activities.index = (int)ptv->itemNew.lParam;
                        SendMessage(hDraw, ID_ACTIVITIESTAB_DRAW_DAY, 0, (LPARAM)activities.GetNextPtrWrp());
                        SendMessage(hParentWindow, ID_ACTIVITIESTAB_UPDATE_TIME, 0, 0);
                        break;
                    }
                }
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