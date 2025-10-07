#include "mainwndproc.hpp"
#include "idwndproc.hpp"
#include "certwndproc.hpp"
#include "activitieswndproc.hpp"
#include "functions.hpp"
#include <thread>

HWND TabControl, IDTab, ActivitiesTab, CertTab;

LRESULT CALLBACK MainWndProc(HWND hMainWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    const static WCHAR Tab1[] = L"ID", Tab2[] = L"Activities", Tab3[] = L"Certificates";
    static TCITEM tci = {};
    switch (msg)
    {
        case WM_CREATE:
        {
            RECT TabRect;
            TabControl = CreateWindowEx(0, WC_TABCONTROL, TEXT("TabC"),
                WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 0, 0,
                hMainWindow, (HMENU)ID_TABCTRL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            SetTabNames(tci, TabControl, (LPWSTR)Tab1, 0);
            SetTabNames(tci, TabControl, (LPWSTR)Tab2, 1);
            SetTabNames(tci, TabControl, (LPWSTR)Tab3, 2);

            WNDCLASS TabWindowsCl = {0};
            SetWindowAttr(TabWindowsCl, (HBRUSH)(COLOR_WINDOW+1), IDC_ARROW, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, IdWndProc, Tab1, NULL, CS_HREDRAW | CS_VREDRAW);
            RegisterClass(&TabWindowsCl);

            SetWindowAttr(TabWindowsCl, (HBRUSH)(COLOR_WINDOW), IDC_ARROW, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, ActivitiesWndProc, Tab2, NULL, CS_HREDRAW | CS_VREDRAW);
            RegisterClass(&TabWindowsCl);

            SetWindowAttr(TabWindowsCl, (HBRUSH)(COLOR_WINDOW), IDC_ARROW, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, CertificatesWndProc, Tab3, NULL, CS_HREDRAW | CS_VREDRAW);
            RegisterClass(&TabWindowsCl);

            GetClientRect(TabControl, &TabRect);

            IDTab = CreateWindowEx(0, Tab1, NULL, WS_CHILD | WS_VISIBLE,
                TabRect.left, TabRect.right + 40, TabRect.right - TabRect.left, TabRect.bottom - (TabRect.right + 40),
                TabControl, (HMENU)ID_IDTAB, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            ActivitiesTab = CreateWindowEx(0, Tab2, NULL, WS_CHILD,
                TabRect.left, TabRect.right + 40, TabRect.right - TabRect.left, TabRect.bottom - (TabRect.right + 40),
                TabControl, (HMENU)ID_ACTIVITIESTAB, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            CertTab = CreateWindowEx(0, Tab3, NULL, WS_CHILD,
                TabRect.left, TabRect.right + 40, TabRect.right - TabRect.left, TabRect.bottom - (TabRect.right + 40),
                TabControl, (HMENU)ID_CERTSTAB, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // default open file
            LPCREATESTRUCT s = (LPCREATESTRUCT)lParam;
            LPSTR filePath = (LPSTR)s->lpCreateParams;

            /*if (filePath)
            {
                ReadTachographFile(filePath);
                PostMessage(IDTab, ID_IDTAB_UPDATE, 0, 0);
            }*/
            break;
        }
        case WM_NOTIFY:
        {
            if (((NMHDR*)lParam)->code == TCN_SELCHANGE)
            {
                int selectedtab = TabCtrl_GetCurSel(TabControl);
                ShowWindow(IDTab, selectedtab == 0 ? SW_SHOW : SW_HIDE);
                ShowWindow(ActivitiesTab, selectedtab == 1 ? SW_SHOW : SW_HIDE);
                ShowWindow(CertTab, selectedtab == 2 ? SW_SHOW : SW_HIDE);
            }
            break;
        }
        case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            MoveWindow(TabControl, 0, 0, width, height, TRUE);

            RECT rc;
            GetClientRect(TabControl, &rc);
            TabCtrl_AdjustRect(TabControl, FALSE, &rc);

            MoveWindow(IDTab, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
            MoveWindow(ActivitiesTab, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
            MoveWindow(CertTab, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

            break;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_F5:
                {
                    std::thread([]()
                    {
                        if (!ReadTachographCard()) {
                            SendMessage(IDTab, ID_IDTAB_UPDATE, 0, 0);
                            SendMessage(ActivitiesTab, ID_ACTIVITIESTAB_UPDATE, 0, 0);
                            SendMessage(CertTab, ID_CERTSTAB_UPDATE, 0, 0);
                        }
                    }).detach();
                    break;
                }
                case VK_PRIOR:
                {
                    SendMessage(ActivitiesTab, WM_KEYDOWN, VK_PRIOR, 0);
                    break;
                }
                case VK_NEXT:
                {
                    SendMessage(ActivitiesTab, WM_KEYDOWN, VK_NEXT, 0);
                    break;
                }
            }
            break;
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case 101:
                {
                    WriteDDD(hMainWindow);
                    break;
                }
                case 102:
                {
                    OPENFILENAMEA ofn = {0};
                    char filePath[MAX_PATH] = {0};

                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hMainWindow;
                    ofn.lpstrFile = filePath;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrFilter = "DDD Files\0*.ddd\0All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if (GetOpenFileNameA(&ofn)) {
                        ReadTachographFile(filePath);
                        SendMessage(IDTab, ID_IDTAB_UPDATE, 0, 0);
                        SendMessage(ActivitiesTab, ID_ACTIVITIESTAB_UPDATE, 0, 0);
                        SendMessage(CertTab, ID_CERTSTAB_UPDATE, 0, 0);
                    }
                    break;
                }

                case 103:
                {
                    FlushMemory();
                    PostQuitMessage(0);
                    break;
                }
                case 201:
                {
                    std::thread([]()
                    {
                        if (!ReadTachographCard()) {
                            SendMessage(IDTab, ID_IDTAB_UPDATE, 0, 0);
                            SendMessage(ActivitiesTab, ID_ACTIVITIESTAB_UPDATE, 0, 0);
                            SendMessage(CertTab, ID_CERTSTAB_UPDATE, 0, 0);
                        }
                    }).detach();
                    break;
                }
                case 202:
                {
                    FlushMemory();
                    break;
                }
                case 301:
                {
                    break; // remove this line afterwards!
                }
            }
            break;
        }
        case WM_DESTROY:
        {
            FlushMemory();
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hMainWindow, msg, wParam, lParam);
        }
    }
    return 0;
}