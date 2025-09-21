#include "functions.hpp"
#include "TahoReader.hpp"
#include "mainwndproc.hpp"
#include "readtachocardfunc.hpp"
#include <commdlg.h>

HMENU MainMenu;
ID idData{};
IDNull idDataNull{};
DriverLicenseNULL licenseDataNull{};
byte* ICDataptr = nullptr;
byte* ICCDataptr = nullptr;
byte* DIR = nullptr;
byte* ATR = nullptr;
byte* EL = nullptr;
BYTE* driverLicenseDATAptr = nullptr;
BYTE* activitiesDATAptr = nullptr;
BYTE* vehiclesDATAptr = nullptr;
BYTE* cardCertDATAptr = nullptr;
BYTE* CACertDATAptr = nullptr;
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
    delete[] DIR;
    DIR = nullptr;
    delete[] ATR;
    ATR = nullptr;
    delete[] EL;
    EL = nullptr;

    memset(&idData, 0, sizeof(ID));
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
    // DIR
    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x2F, 0x00});
    DIR = reader.ReadData(20);
    // ATR
    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x2F, 0x01});
    ATR = reader.ReadData(11);
    // Extended length
    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x00, 0x06});
    EL = reader.ReadData(3);
    
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

void WriteDDD(HWND& hWindow)
{
    OPENFILENAME file;
    wchar_t SizeOfFile[260] = L"";

    ZeroMemory(&file, sizeof(file));
    file.lStructSize = sizeof(file);
    file.hwndOwner = hWindow;
    file.lpstrFile = SizeOfFile;
    file.nMaxFile = sizeof(SizeOfFile) / sizeof(wchar_t);
    file.lpstrFilter = L"DDD Files (*.ddd)\0*.ddd\0All Files (*.*)\0*.*\0";
    file.nFilterIndex = 1;
    file.lpstrDefExt = L"ddd";
    file.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&file))
    {
        FILE* fp = _wfopen(file.lpstrFile, L"wb");
        if (fp) // serious function is needed here!!!!!! u cant just write data as it is
        // NAREDI DVA ID STRUCTA en ki iam original variable memberje drugi ki ima dodatne
        // in samo referencira original
        {
            BYTE arr[5] = {0x05, 0x20, 0x00, 0x00, 0x8F};
            fwrite(&arr, sizeof(arr), 1, fp);
            fwrite(&idData, sizeof(idData), 1, fp);
            fclose(fp);
        }
    }
}
