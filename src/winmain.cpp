#include "mainwndproc.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR filePath, int cmdshow)
{
    // main window attributes
    WNDCLASS MainWndAttr = {0};
    MainWndAttr.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    MainWndAttr.hCursor = LoadCursor(NULL, IDC_ARROW);
    MainWndAttr.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    MainWndAttr.hInstance = hInstance;
    MainWndAttr.lpfnWndProc = MainWndProc;
    MainWndAttr.lpszClassName = TEXT("TahoReader");
    MainWndAttr.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&MainWndAttr);
    
    CreateMainMenu();
    
    // getting x and y center positions
    int x = GetSystemMetrics(SM_CXSCREEN) / 2 - 500, y = GetSystemMetrics(SM_CYSCREEN) / 2 - 400;

    HWND MainWindow = CreateWindow(TEXT("TahoReader"), TEXT("TahoReader"), WS_OVERLAPPEDWINDOW,
        x, y, 1000, 800, NULL, MainMenu, hInstance, (LPVOID)filePath);
    
    ShowWindow(MainWindow, cmdshow);
    UpdateWindow(MainWindow);

    MSG messages;

    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    return 0;
}