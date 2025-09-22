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
extern BYTE* idData;
extern IDNull idDataNull;
extern BYTE* ICDataptr;
extern BYTE* ICCDataptr;
extern BYTE* driverLicenseDATAptr;
extern DriverLicenseNULL licenseDataNull;
extern BYTE* activitiesDATAptr;
extern BYTE* vehiclesDATAptr;
extern BYTE* cardCertDATAptr;
extern BYTE* CACertDATAptr;
extern Vehicles vehicles;

extern BYTE* appIdentification;
extern BYTE* cardDownload;
extern BYTE* eventsData;
extern BYTE* faultsData;
extern BYTE* places;
extern BYTE* currentUsage;
extern BYTE* controlActivityData;
extern BYTE* specificConditions;

extern int UTC;

void DateStamp(uint32_t epoch, char* buffer, char mark = '-');
void CreateMainMenu();
int ReadTachographCard();
void SetTabNames(TCITEM tci, HWND TabControl, LPWSTR text, int noTab);
void SetWindowAttr(WNDCLASS& wc, HBRUSH color, LPCWSTR cursor, LPCWSTR icon, HINSTANCE hInstance, WNDPROC func, LPCWSTR classname, LPCWSTR menu, UINT style);
void RedrawBitMap(int left, int top, int right, int bottom, HDC& memDC, HWND& Window);
void FlushMemory();
void WriteDDD(HWND& hWindow);