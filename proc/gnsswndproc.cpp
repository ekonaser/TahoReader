#include "gnsswndproc.hpp"
#include "functions.hpp"
#include "structs.hpp"

LRESULT CALLBACK GNSSWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            break;
        }
        case WM_SIZE:
        {
            RECT hParentWindowSize;
            GetClientRect(hParentWindow, &hParentWindowSize);
            if (webviewGNSS.webviewController)
            {
                webviewGNSS.webviewController->put_Bounds(hParentWindowSize);
            }
            break;
        }
        case WM_DESTROY:
        {
            webviewGNSS.webviewController->Close();
            // u have to delete some files manually
            // remove(TahoReader.exe.WebView2);
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hParentWindow, msg, wParam, lParam);
        }
    }
    return 0;
}