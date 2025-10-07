#include "functions.hpp"
#include "TahoReader.hpp"
#include "mainwndproc.hpp"
#include "readtachocardfunc.hpp"
#include <commdlg.h>
#include <initializer_list>

HMENU MainMenu;
G1Card gen1card;
G2Card gen2card;
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
    AppendMenu(Program, MF_STRING, 102, TEXT("Open"));
    AppendMenu(Program, MF_STRING, 103, TEXT("Exit\tAlt+F4"));
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
    gen1card = G1Card();
    gen2card = G2Card();
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
    gen1card.ICCDataptr = reader.ReadData(25);
    // IC Data
    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x00, 0x05});
    gen1card.ICDataptr = reader.ReadData(8);
    // DIR ATR AND EL are not included in standard .DDD format therefore are not needed
    // EL can be used for protocol
    
    /*DF*/
    DFG1(reader);
    DFG2(reader);

    return 0;
}

int ReadTachographFile(LPSTR filePath)
{
    FlushMemory();
    static TahoFileReader reader;
    reader.ReadFile(filePath);

    // GEN 1
    gen1card.ICCDataptr = reader.FindFile(0x00, 0x02, 0x00);

    gen1card.ICDataptr = reader.FindFile(0x00, 0x05, 0x00);

    gen1card.cardCertDATAptr = reader.FindFile(0xC1, 0x00, 0x00);

    gen1card.CACertDATAptr = reader.FindFile(0xC1, 0x08, 0x00);

    gen1card.idData = reader.FindFile(0x05, 0x20, 0x00);
    idDataNull = IDNull(gen1card.idData);

    gen1card.driverLicenseDATAptr = reader.FindFile(0x05, 0x21, 0x00);
    licenseDataNull = DriverLicenseNULL(gen1card.driverLicenseDATAptr);

    gen1card.activitiesDATAptr = reader.FindFile(0x05, 0x04, 0x00);

    gen1card.vehiclesDATAptr = reader.FindFile(0x05, 0x05, 0x00);
    vehicles.readVehicles(gen1card.vehiclesDATAptr);

    gen1card.appIdentification = reader.FindFile(0x05, 0x01, 0x00);

    gen1card.cardDownload = reader.FindFile(0x05, 0x0E, 0x00);

    gen1card.eventsData = reader.FindFile(0x05, 0x02, 0x00);

    gen1card.faultsData = reader.FindFile(0x05, 0x03, 0x00);

    gen1card.places = reader.FindFile(0x05, 0x06, 0x00);

    gen1card.currentUsage = reader.FindFile(0x05, 0x07, 0x00);

    gen1card.controlActivityData = reader.FindFile(0x05, 0x08, 0x00);

    gen1card.specificConditions = reader.FindFile(0x05, 0x22, 0x00);
    
    // GEN 2
    gen2card.cardCertDATAptr = reader.FindFile(0xC1, 0x00, 0x02);

    gen2card.CACertDATAptr = reader.FindFile(0xC1, 0x08, 0x02);

    gen2card.idData = reader.FindFile(0x05, 0x20, 0x02);

    gen2card.driverLicenseDATAptr = reader.FindFile(0x05, 0x21, 0x02);

    gen2card.activitiesDATAptr = reader.FindFile(0x05, 0x04, 0x02);

    gen2card.vehiclesDATAptr = reader.FindFile(0x05, 0x05, 0x02);

    gen2card.appIdentification = reader.FindFile(0x05, 0x01, 0x02);

    gen2card.cardDownload = reader.FindFile(0x05, 0x0E, 0x02);

    gen2card.eventsData = reader.FindFile(0x05, 0x02, 0x02);

    gen2card.faultsData = reader.FindFile(0x05, 0x03, 0x02);

    gen2card.places = reader.FindFile(0x05, 0x06, 0x02);

    gen2card.currentUsage = reader.FindFile(0x05, 0x07, 0x02);

    gen2card.controlActivityData = reader.FindFile(0x05, 0x08, 0x02);

    gen2card.specificConditions = reader.FindFile(0x05, 0x22, 0x02);

    gen2card.cardSignCertificate = reader.FindFile(0xC1, 0x01, 0x02);

    gen2card.linkCertificate = reader.FindFile(0xC1, 0x09, 0x02);

    gen2card.vehicleUnitsUsed = reader.FindFile(0x05, 0x23, 0x02);

    gen2card.GNSS = reader.FindFile(0x05, 0x24, 0x02);
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
            // [2bytes, 1byte, 2bytes]
            // [section, card generation, length of section]
            WriteToFile(fp, {0x00, 0x02, 0x00, 0x00, 0x19}, gen1card.ICCDataptr, 25);
            WriteToFile(fp, {0x00, 0x05, 0x00, 0x00, 0x08}, gen1card.ICDataptr, 8);
            WriteToFile(fp, {0x05, 0x01, 0x00, 0x00, 0x0A}, gen1card.appIdentification, 10);
            WriteToFile(fp, {0xC1, 0x00, 0x00, 0x00, 0xC2}, gen1card.cardCertDATAptr, 194);
            WriteToFile(fp, {0xC1, 0x08, 0x00, 0x00, 0xC2}, gen1card.CACertDATAptr, 194);
            WriteToFile(fp, {0x05, 0x20, 0x00, 0x00, 0x8F}, gen1card.idData, 143);
            WriteToFile(fp, {0x05, 0x0E, 0x00, 0x00, 0x04}, gen1card.cardDownload, 4);
            WriteToFile(fp, {0x05, 0x21, 0x00, 0x00, 0x35}, gen1card.driverLicenseDATAptr, 53);
            WriteToFile(fp, {0x05, 0x02, 0x00, 0x06, 0xC0}, gen1card.eventsData, 1728);
            WriteToFile(fp, {0x05, 0x03, 0x00, 0x04, 0x80}, gen1card.faultsData, 1152);
            WriteToFile(fp, {0x05, 0x04, 0x00, 0x35, 0xD4}, gen1card.activitiesDATAptr, 13780);
            WriteToFile(fp, {0x05, 0x05, 0x00, 0x18, 0x3A}, gen1card.vehiclesDATAptr, 6202);
            WriteToFile(fp, {0x05, 0x06, 0x00, 0x04, 0x61}, gen1card.places, 1121);
            WriteToFile(fp, {0x05, 0x07, 0x00, 0x00, 0x13}, gen1card.currentUsage, 19);
            WriteToFile(fp, {0x05, 0x08, 0x00, 0x00, 0x2E}, gen1card.controlActivityData, 46);
            WriteToFile(fp, {0x05, 0x22, 0x00, 0x01, 0x18}, gen1card.specificConditions, 280);
            // 24926 + 25 + 8
            WriteToFile(fp, {0x05, 0x01, 0x02, 0x00, 0x0F}, gen2card.appIdentification, 15);
            WriteToFile(fp, {0xC1, 0x00, 0x02, 0x00, 0xCD}, gen2card.cardCertDATAptr, 205);
            WriteToFile(fp, {0xC1, 0x01, 0x02, 0x00, 0xCD}, gen2card.cardSignCertificate, 205);
            WriteToFile(fp, {0xC1, 0x08, 0x02, 0x00, 0xCD}, gen2card.CACertDATAptr, 205);
            WriteToFile(fp, {0xC1, 0x09, 0x02, 0x00, 0xCD}, gen2card.linkCertificate, 205);
            WriteToFile(fp, {0x05, 0x20, 0x02, 0x00, 0x8F}, gen2card.idData, 143);
            WriteToFile(fp, {0x05, 0x0E, 0x02, 0x00, 0x04}, gen2card.cardDownload, 4);
            WriteToFile(fp, {0x05, 0x21, 0x02, 0x00, 0x35}, gen2card.driverLicenseDATAptr, 53);
            WriteToFile(fp, {0x05, 0x02, 0x02, 0x0C, 0x60}, gen2card.eventsData, 3168);
            WriteToFile(fp, {0x05, 0x03, 0x02, 0x04, 0x80}, gen2card.faultsData, 1152);
            WriteToFile(fp, {0x05, 0x04, 0x02, 0x35, 0xD4}, gen2card.activitiesDATAptr, 13780);
            WriteToFile(fp, {0x05, 0x05, 0x02, 0x25, 0x82}, gen2card.vehiclesDATAptr, 9602);
            WriteToFile(fp, {0x05, 0x06, 0x02, 0x09, 0x32}, gen2card.places, 2354);
            WriteToFile(fp, {0x05, 0x07, 0x02, 0x00, 0x13}, gen2card.currentUsage, 19);
            WriteToFile(fp, {0x05, 0x08, 0x02, 0x00, 0x2E}, gen2card.controlActivityData, 46);
            WriteToFile(fp, {0x05, 0x22, 0x02, 0x02, 0x32}, gen2card.specificConditions, 562);
            WriteToFile(fp, {0x05, 0x23, 0x02, 0x07, 0xD2}, gen2card.vehicleUnitsUsed, 2002);
            WriteToFile(fp, {0x05, 0x24, 0x02, 0x13, 0xB2}, gen2card.GNSS, 5042);
            // 39306
            fclose(fp);
        }
    }
}
