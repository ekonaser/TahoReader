#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <stdio.h>
#include <stdint.h>
#include <unordered_map>

#pragma pack(push, 1)
struct IDNull {
    char cardNumber[18];
    char issuer[36];
    uint32_t dateissued;
    uint32_t startdate;
    uint32_t expirydate;
    char surname[36]; // bcs last byte is 0x01 36th byte same doesnt apply for name!!!!!
    char name[36];
    byte birthday[5];
    char country[3];

    IDNull()
    {
        memset(this, 0, sizeof(IDNull));
    }
    IDNull(BYTE* ptr);

    LPCSTR BirthDay();
    LPCSTR Date(uint32_t& variable);
};
#pragma pack(pop)

#pragma pack(push, 1)
struct DriverLicenseNULL {
    byte delimiter1;
    char country[36];
    byte delimiter2;
    char license[17];
    DriverLicenseNULL()
    {
        memset(this, 0, sizeof(DriverLicenseNULL));
    }
    DriverLicenseNULL(BYTE* ptr);
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ICC {
    uint8_t clockstop;
    byte cardExtendedSerialNumber[8];
    byte cardApprovalNumber[8]; // default values 20 20 20 .. 20 - empty 'char'
    byte cardPersonalizerID;
    byte embedderIcAssemblerId[5];
    byte icIdentifier[2];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Header {
    uint16_t prevLength;
    uint16_t currLength;
    uint32_t time;
    uint16_t noActivity;
    uint16_t km;
    uint16_t empty;
};
#pragma pack(pop)

struct DailyWrapper
{
    Header header;
    BYTE* ptr;

    DailyWrapper(BYTE* block);
};

struct Iterator1023
{
    int no;
    uint16_t rem;
    Iterator1023(int len);

    uint16_t next();
};

struct Activities
/*
Struct holds list of activities by day wrapped in sub-struct DailyWrapper.
*/
{
    int index = 0, lastIndex;
    DailyWrapper* ptrWrp[365] = {}; // pointer to wrapper currently all nullptr's

    Activities(BYTE* ptr, int end, int start);

    void readActivities(BYTE* ptr, int end, int start);

    DailyWrapper* GetNextPtrWrp();

    ~Activities();
};

struct ActivityData
{
    int rest;
    int administration;
    int work;
    int driving;
    int RecordingRest;
    int Overdrive;
};

struct DrawingBrush
{
    HDC hdc;
    HWND window;
    HBRUSH color;
    DrawingBrush(HDC &hDC, HWND &hWindow);
    void CreateColor(uint8_t a = 0x00, uint8_t b = 0x00, uint8_t c = 0x00);
    void DrawOneDay(BYTE* ptr, int counter, ActivityData& pData);
    ~DrawingBrush();
};

#pragma pack(push, 1)
struct uint24_t
{
    uint8_t bytes[3];
};
#pragma pack(pop)

struct Registration
{
    char chr[14]; // the last is reserved for '\0'
};

#pragma pack(push, 1)
struct Vehicle
{
    uint16_t no;
    uint24_t startKM;
    uint24_t endKM;
    uint32_t startTime;
    uint32_t endTime;
    uint16_t delimiter;
    Registration registration;
};
#pragma pack(pop)

struct Vehicles
{
    Vehicle* ptrWrp = new Vehicle[200]; // 6200 / 31 = 200 records
    Vehicles(BYTE* ptr);
    void readVehicles(BYTE* ptr);
    ~Vehicles();
};

struct ActivitiesTree
{
    HTREEITEM Root;
    HTREEITEM* days;
    std::unordered_map<uint32_t, HTREEITEM*> treemap;
    TVITEM tv{};
    HWND& hWindow;
    Activities& activities;
    TVINSERTSTRUCT tvis{}; // tree view insert struct
    char buffer[11];
    wchar_t bufferW[20];
    ActivitiesTree(HWND& hTreeWindow, Activities& ActAdd);
    void CreateTree();
    void UpdateTreeVehicles(Vehicle* ptr);
};

struct G1Card
{
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
    G1Card() = default;
    ~G1Card();
};

struct G2Card : G1Card
{
    BYTE* cardSignCertificate = nullptr;    // C1 01
    BYTE* linkCertificate = nullptr;        // C1 09
    BYTE* vehicleUnitsUsed = nullptr;       // 05 23
    BYTE* GNSS = nullptr;                   // 05 24 Global Navigation Satellite System
    G2Card() = default;
    ~G2Card();
};