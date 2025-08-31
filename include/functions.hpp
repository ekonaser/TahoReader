#pragma once

#define UNICODE
#define _UNICODE

// some general includes
#include <Windows.h>
#include <CommCtrl.h>
#include "structs.hpp"

extern HMENU MainMenu;

// main driver card data/sections
extern ID idData;
extern BYTE* activitiesDATAptr;
extern BYTE* vehiclesDATAptr;

extern int UTC;

const SCARD_IO_REQUEST* getProtocol(DWORD activeProtocol);
void DateStamp(int epoch, int& year, int& month, int& day);
void CreateMainMenu();
void ReadTachographCard();
void SetTabNames(TCITEM tci, HWND TabControl, LPWSTR text, int noTab);
void SetWindowAttr(WNDCLASS& wc, HBRUSH color, LPCWSTR cursor, LPCWSTR icon, HINSTANCE hInstance, WNDPROC func, LPCWSTR classname, LPCWSTR menu, UINT style);
void RedrawBitMap(int left, int top, int right, int bottom, HDC& memDC, HWND& Window);