#include "functions.hpp"
#include "TahoReader.hpp"
#include "mainwndproc.hpp"
#include "readtachocardfunc.hpp"
#include <commdlg.h>
#include <initializer_list>

HMENU MainMenu;
BYTE* ICCDataptr = nullptr;             // 00 02
BYTE* ICDataptr = nullptr;              // 00 05
BYTE* appIdentification = nullptr;      // 05 01
BYTE* cardCertDATAptr = nullptr;        // C1 00
BYTE* CACertDATAptr = nullptr;          // C1 08
BYTE* idData = nullptr;                 // 05 20
BYTE* cardDownload = nullptr;           // 05 0E
BYTE* driverLicenseDATAptr = nullptr;   // 05 21
BYTE* eventsData = nullptr;             // 05 02
BYTE* faultsData = nullptr;             // 05 03
BYTE* activitiesDATAptr = nullptr;      // 05 04
BYTE* vehiclesDATAptr = nullptr;        // 05 05
BYTE* places = nullptr;                 // 05 06
BYTE* currentUsage = nullptr;           // 05 07
BYTE* controlActivityData = nullptr;    // 05 08
BYTE* specificConditions = nullptr;     // 05 22
IDNull idDataNull{};
DriverLicenseNULL licenseDataNull{};
Vehicles vehicles(nullptr);

int UTC = 0;

void DateStamp(uint32_t epoch, char* buffer, char mark)
{
    int year = 1970, month = 1, day = 1;
    int sec = epoch % 86400; // HH:MM:SS
    int i = 0;
    int months[12] = {2678400,2419200,2678400,2592000,2678400,2592000,2678400,2678400,2592000,2678400,2592000,2678400};
    epoch -= sec;
    while (epoch >= 31536000)
    {
        epoch -= ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0))) ? 31622400 : 31536000;
        year += 1;
    }

    if ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))
    {
        months[1] = 2505600;
    }
    while (epoch >= months[i])
    {
        epoch -= months[i++];
    }
    month += i;
    day += epoch / 86400;
    sprintf(buffer, "%04d%c%02d%c%02d", year, mark, month, mark, day);
}

void CreateMainMenu(){
    // setting up main menu
    MainMenu = CreateMenu();
    HMENU Program = CreateMenu();
    HMENU Card = CreateMenu();
    HMENU Help = CreateMenu();
    
    AppendMenu(Program, MF_STRING, 101, TEXT("Save As"));
    AppendMenu(Program, MF_STRING, 102, TEXT("Exit\tAlt+F4"));
    AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT_PTR)(Program), TEXT("Program"));

    AppendMenu(Card, MF_STRING, 201, TEXT("Read card\tF5"));
    AppendMenu(Card, MF_STRING, 202, TEXT("Flush memory"));
    AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT_PTR)(Card), TEXT("Card"));

    AppendMenu(Help, MF_STRING, 301, TEXT("About"));
    AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT_PTR)(Help), TEXT("Help"));
}

void SetTabNames(TCITEM tci, HWND TabControl, LPWSTR text, int noTab)
{
    memset(&tci, 0, sizeof(TCITEM));
    tci.mask = TCIF_TEXT; // TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM | TCIF_STATE
    tci.pszText = text;
    TabCtrl_InsertItem(TabControl, noTab, &tci);
}

void SetWindowAttr(WNDCLASS& wc, HBRUSH color, LPCWSTR cursor, LPCWSTR icon, HINSTANCE hInstance, WNDPROC func, LPCWSTR classname, LPCWSTR menu, UINT style)
{
    wc.hbrBackground = color;
    wc.hCursor = LoadCursor(NULL, cursor);
    wc.hIcon = LoadIcon(NULL, icon);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = func;
    wc.lpszClassName = classname;
    wc.lpszMenuName = menu;
    wc.style = style;
}

void FlushMemory()
{
    delete[] idData;
    idData = nullptr;
    delete[] activitiesDATAptr;
    activitiesDATAptr = nullptr;
    delete[] vehiclesDATAptr;
    vehiclesDATAptr = nullptr;
    delete[] cardCertDATAptr;
    cardCertDATAptr = nullptr;
    delete[] CACertDATAptr;
    CACertDATAptr = nullptr;
    delete[] ICDataptr;
    ICDataptr = nullptr;
    delete[] ICCDataptr;
    ICCDataptr = nullptr;
    delete[] driverLicenseDATAptr;
    driverLicenseDATAptr = nullptr;

    delete[] appIdentification;
    appIdentification = nullptr;
    delete[] cardDownload;
    cardDownload = nullptr;
    delete[] eventsData;
    eventsData = nullptr;
    delete[] faultsData;
    faultsData = nullptr;
    delete[] places;
    places = nullptr;
    delete[] currentUsage;
    currentUsage = nullptr;
    delete[] controlActivityData;
    controlActivityData = nullptr;
    delete[] specificConditions;
    specificConditions = nullptr;

    memset(&idDataNull, 0, sizeof(IDNull));
    memset(&licenseDataNull, 0, sizeof(DriverLicenseNULL));

    SendMessage(IDTab, ID_IDTAB_UPDATE, 0, 0);
    HWND hWndtoDrawProc = FindWindowEx(ActivitiesTab, NULL, TEXT("DrawWindow"), NULL);
    SendMessage(ActivitiesTab, ID_ACTIVITIESTAB_RESET, 0, 0);
    SendMessage(ActivitiesTab, ID_ACTIVITIESTAB_UPDATE_TIME, 0, 0);
    SendMessage(hWndtoDrawProc, ID_ACTIVITIESTAB_RESET, 0, 0);
    SendMessage(CertTab, ID_CERTSTAB_UPDATE, 0, 0);
}

int ReadTachographCard()
{
    TahoReader reader;
    
    if (reader.CheckStatus()) return -1;
    
    FlushMemory();

    reader.SelectFile({0x00, 0xA4, 0x00, 0x0C, 0x02, 0xFF, 0x54, 0x41, 0x43, 0x48, 0x4F}); // selecting TAHO app 2 bytes
    
    /*EF*/
    // ICC Data
    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x00, 0x02});
    ICCDataptr = reader.ReadData(25);
    // IC Data
    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x00, 0x05});
    ICDataptr = reader.ReadData(8);
    // DIR ATR AND EL are not included in standard .DDD format therefore are not needed
    // EL can be used for protocol
    
    /*DF*/
    DFG1(reader);

    return 0;
}

void RedrawBitMap(int left, int top, int right, int bottom, HDC& memDC, HWND& Window)
{
    HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
    RECT memDCSize = {left, top, right, bottom};
    FillRect(memDC, &memDCSize, white);
    DeleteObject(white);
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(Window, &ps);
    
    // left
    MoveToEx(memDC, memDCSize.left + 45, 0, NULL);
    LineTo(memDC, memDCSize.left + 45, 14400);
    // right
    MoveToEx(memDC, memDCSize.right - 45, 0, NULL);
    LineTo(memDC, memDCSize.right - 45, 14400);

    char buffer[5];
    int i = 0;

    for (int h = 0; h < 24; h++)
    {
        for (int m = 0; m < 60; m++)
        {
            sprintf(buffer, "%02i:%02i", h, m);
            TextOutA(memDC, memDCSize.left + 10, i, buffer, 5);
            TextOutA(memDC, memDCSize.right - 35, i, buffer, 5);
            i += 10;
        }
    }

    EndPaint(Window, &ps);
}


void WriteToFile(FILE* fp, std::initializer_list<BYTE> arr, BYTE* data, int size)
{
    fwrite(arr.begin(), arr.size(), 1, fp);
    fwrite(data, size, 1, fp);
}

void WriteDDD(HWND& hWindow)
{
    OPENFILENAME file;
    wchar_t StrFile[260] = L"";

    ZeroMemory(&file, sizeof(file));
    file.lStructSize = sizeof(file);
    file.hwndOwner = hWindow;
    file.lpstrFile = StrFile;
    file.nMaxFile = sizeof(StrFile) / sizeof(wchar_t);
    file.lpstrFilter = L"DDD Files (*.ddd)\0*.ddd\0All Files (*.*)\0*.*\0";
    file.nFilterIndex = 1;
    file.lpstrDefExt = L"ddd";
    file.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&file))
    {
        FILE* fp = _wfopen(file.lpstrFile, L"wb");
        if (fp)
        {
            WriteToFile(fp, {0x00, 0x02, 0x00, 0x00, 0x19}, ICCDataptr, 25);
            WriteToFile(fp, {0x00, 0x05, 0x00, 0x00, 0x08}, ICDataptr, 8);
            WriteToFile(fp, {0x05, 0x01, 0x00, 0x00, 0x0A}, appIdentification, 10);
            WriteToFile(fp, {0xC1, 0x00, 0x00, 0x00, 0xC2}, cardCertDATAptr, 194);
            WriteToFile(fp, {0xC1, 0x08, 0x00, 0x00, 0xC2}, CACertDATAptr, 194);
            WriteToFile(fp, {0x05, 0x20, 0x00, 0x00, 0x8F}, idData, 143);
            WriteToFile(fp, {0x05, 0x0E, 0x00, 0x00, 0x04}, cardDownload, 4);
            fclose(fp);
        }
    }
}
