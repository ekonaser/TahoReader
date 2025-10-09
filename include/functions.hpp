#pragma once

#define UNICODE
#define _UNICODE

// some general includes
#include <Windows.h>
#include <CommCtrl.h>
#include "structs.hpp"

// global handles
extern HWND TabControl, IDTab, ActivitiesTab, GNSSTab, CertTab;

// menu
extern HMENU MainMenu;

// main driver card data/sections
// if we wanna copy data in one shot 2 structs are needed original && with nullterms
extern G1Card gen1card;
extern G2Card gen2card;
extern IDNull idDataNull;
extern DriverLicenseNULL licenseDataNull;
extern Activities activities;
extern Vehicles vehicles;

extern int UTC;

void DateStamp(uint32_t epoch, char* buffer, char mark = '-');
void CreateMainMenu();
int ReadTachographCard();
int ReadTachographFile(LPSTR filePath);
void SetTabNames(TCITEM tci, HWND TabControl, LPWSTR text, int noTab);
void SetWindowAttr(WNDCLASS& wc, HBRUSH color, LPCWSTR cursor, LPCWSTR icon, HINSTANCE hInstance, WNDPROC func, LPCWSTR classname, LPCWSTR menu, UINT style);
void RedrawBitMap(int left, int top, int right, int bottom, HDC& memDC, HWND& Window);
void RestingFunc(int& recordingrest, int& duration);
void FlushMemory();
void WriteDDD(HWND& hWindow);