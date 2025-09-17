#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <stdio.h>
#include <stdint.h>
#include <unordered_map>

#pragma pack(push, 1)
struct ID {
    char cardNumber[18];
    char issuer[35];
    uint32_t dateissued;
    uint32_t startdate;
    uint32_t expirydate;
    byte datex01;
    char surname[36];
    char name[35];
    byte birthday[4];
    char country[2];
};
#pragma pack(pop)

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
    IDNull(const ID& id);

    LPCSTR BirthDay();
    LPCSTR Date(uint32_t& variable);
};

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

struct uint24_t
{
    uint8_t bytes[3];
};

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
    Vehicle* ptrWrp = new Vehicle[197];
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