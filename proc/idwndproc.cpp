#include "idwndproc.hpp"
#include "mainwndproc.hpp"

LRESULT CALLBACK IdCardProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND G2, Name, Surname, Birthday, StartDate, EndDate, Issuer, CardNumber;
    switch (msg)
    {
        case WM_CREATE:
        {
            G2 = CreateWindow(TEXT("STATIC"), TEXT("G2"), WS_CHILD | WS_VISIBLE,
                68, 190, 20, 20, hParentWindow, NULL, NULL, NULL);
            Surname = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE,
                160, 50, 100, 20, hParentWindow, NULL, NULL, NULL);
            Name = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE,
                160, 70, 100, 20, hParentWindow, NULL, NULL, NULL);
            Birthday = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE,
                160, 90, 80, 20, hParentWindow, NULL, NULL, NULL);
            StartDate = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE,
                160, 110, 80, 20, hParentWindow, NULL, NULL, NULL);
            EndDate = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE,
                300, 110, 80, 20, hParentWindow, NULL, NULL, NULL);
            Issuer = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE,
                160, 130, 100, 20, hParentWindow, NULL, NULL, NULL);
            CardNumber = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE,
                160, 170, 130, 20, hParentWindow, NULL, NULL, NULL);
            break;
        }
        case WM_PAINT:
        {
            HICON hIcon = (HICON)LoadImage(NULL, L"user.ico",
                IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hParentWindow, &ps);
            DrawIconEx(hdc, 30, 90, hIcon, 96, 96, 0, NULL, DI_NORMAL);

            EndPaint(hParentWindow, &ps);
            break;
        }
        case ID_IDTAB_UPDATE:
        {
            SetWindowTextA(Surname, idDataNull.surname);
            SetWindowTextA(Name, idDataNull.name);
            SetWindowTextA(Birthday, idDataNull.BirthDay());
            SetWindowTextA(StartDate, idDataNull.Date(idData.startdate));
            SetWindowTextA(EndDate, idDataNull.Date(idData.expirydate));
            SetWindowTextA(Issuer, idDataNull.issuer);
            SetWindowTextA(CardNumber, &idDataNull.cardNumber[1]);
            break;
        }
        default:
        {
            return DefWindowProc(hParentWindow, msg, wParam, lParam);
        }
    }
    return 0;
}

LRESULT CALLBACK IdWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND IDCard; // persists during multiple calls, thats why it has to be static
    switch (msg)
    {
        case WM_CREATE:
        {
            WNDCLASS IDCardWindowClass = {0};
            SetWindowAttr(IDCardWindowClass, (HBRUSH)(COLOR_WINDOW), IDC_ARROW, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, IdCardProc, L"IDCard", NULL, CS_HREDRAW | CS_VREDRAW);
            RegisterClass(&IDCardWindowClass);

            IDCard = CreateWindow(TEXT("IDCard"), TEXT("IDCard"), WS_VISIBLE | WS_CHILD,
                0, 0, 420, 220, hParentWindow, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            HRGN hIDCardRgn = CreateRoundRectRgn(0, 0, 420, 220, 30, 30);
            SetWindowRgn(IDCard, hIDCardRgn, TRUE);
            break;
        }
        case WM_SIZE:
        {
            RECT parentWindow;
            GetClientRect(hParentWindow, &parentWindow);
            MoveWindow(IDCard, (parentWindow.right - 420) / 2, (parentWindow.bottom - 220) / 2, 420, 220, TRUE);
            break;
        }
        case ID_IDTAB_UPDATE:
        {
            SendMessage(IDCard, ID_IDTAB_UPDATE, wParam, lParam); // passing down the message
            break;
        }
        default:
        {
            return DefWindowProc(hParentWindow, msg, wParam, lParam);
        }
    }
    return 0;
}