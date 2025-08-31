#include "vehicleswndproc.hpp"

LRESULT CALLBACK VehiclesWndProc(HWND hParentWindow, UINT msg, WPARAM wParam, LPARAM lParam)
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