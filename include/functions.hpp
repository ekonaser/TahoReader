#pragma once

#define UNICODE
#define _UNICODE

// some general includes
#include <Windows.h>
#include <CommCtrl.h>
#include "structs.hpp"

// global handles
extern HWND TabControl, IDTab, ActivitiesTab, CertTab;

// menu
extern HMENU MainMenu;

// main driver card data/sections
// if we wanna copy data in one shot 2 structs are needed original && with nullterms
extern ID idData;
extern IDNull idDataNull;
extern byte* ICDataptr;
extern byte* ICCDataptr;
extern byte* DIR;
extern byte* ATR;
extern byte* EL; // extended length
extern BYTE* driverLicenseDATAptr;
extern DriverLicenseNULL licenseDataNull;
extern BYTE* activitiesDATAptr;
extern BYTE* vehiclesDATAptr;
extern BYTE* cardCertDATAptr;
extern BYTE* CACertDATAptr;
extern Vehicles vehicles;

extern int UTC;

void DateStamp(uint32_t epoch, char* buffer, char mark = '-');
void CreateMainMenu();
int ReadTachographCard();
void SetTabNames(TCITEM tci, HWND TabControl, LPWSTR text, int noTab);
void SetWindowAttr(WNDCLASS& wc, HBRUSH color, LPCWSTR cursor, LPCWSTR icon, HINSTANCE hInstance, WNDPROC func, LPCWSTR classname, LPCWSTR menu, UINT style);
void RedrawBitMap(int left, int top, int right, int bottom, HDC& memDC, HWND& Window);
void FlushMemory();
void WriteDDD(HWND& hWindow);