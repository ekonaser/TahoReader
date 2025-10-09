#include "gnsswndproc.hpp"

LRESULT CALLBACK GNSSWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
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