#define UNICODE
#define _UNICODE

#include "structs.hpp"
#include "functions.hpp"

LPCSTR IDNull::BirthDay()
{
    static char buffer[10];
    sprintf(buffer, "%02X%02X.%02X.%02X", birthday[0], birthday[1], birthday[2], birthday[3]);
    return buffer;
}

LPCSTR IDNull::Date(uint32_t& variable)
{
    static char buffer[10];
    DateStamp(_byteswap_ulong(variable), buffer, '.');
    return buffer;
}

IDNull::IDNull(BYTE* ptr)
{
    memcpy(cardNumber, ptr, 18); cardNumber[17] = '\0';
    memcpy(issuer, ptr + 18, 35); issuer[35] = '\0';
    memcpy(&dateissued, ptr + 53, 4);
    memcpy(&startdate, ptr + 57, 4);
    memcpy(&expirydate, ptr + 61, 4);
    memcpy(surname, ptr + 66, 36); surname[35] = '\0';
    memcpy(name, ptr + 102, 36); name[35] = '\0';
    memcpy(birthday, ptr + 137, 4); birthday[4] = '\0';
    memcpy(country, ptr + 141, 2); country[2] = '\0';
}


DriverLicenseNULL::DriverLicenseNULL(BYTE* ptr)
{
    memcpy(&delimiter1, ptr, 1);
    memcpy(&country, ptr+1, 35); country[35] = '\0';
    memcpy(&delimiter2, ptr+36, 1);
    memcpy(&license, ptr+37, 16); license[16] = '\0';
}

DailyWrapper::DailyWrapper(BYTE* block) {
    memcpy(&header, block, 14);
    ptr = block + 14;
}

Iterator1023::Iterator1023(int len) {
    no = len / 1023;
    rem = len % 1023;
}

uint16_t Iterator1023::next() {
    if (no > 0) {
        no--;
        return 0x03FF;
    } else if (rem) {
        uint16_t temp = rem;
        rem = 0x0000;
        return temp;
    } else {
        return 0x0000;
    }
}

Activities::Activities(BYTE* ptr, int end, int start) {
    readActivities(ptr, end, start);
}

void Activities::readActivities(BYTE* ptr, int end, int start) {
    // pointers must be able to go into minus INT, thats why uint16_t is invalid
    int st = 0;
    while (start != end) {
        ptrWrp[st] = new DailyWrapper(ptr + start);
        start -= _byteswap_ushort(ptrWrp[st]->header.prevLength);
        st++;
        if (start < 0){
            start += 13776;
        }
    }
    lastIndex = st;
}

Activities::~Activities() {
    for (int i = 0; i < 365; i++) {
        if (ptrWrp[i]) {
            delete ptrWrp[i];
            ptrWrp[i] = nullptr;
        }
    }
}

DailyWrapper* Activities::GetNextPtrWrp() {
    index = (index < 0 ? 0 : index);
    index = (index > lastIndex ? lastIndex : index);
    return ptrWrp[index];
}

DrawingBrush::DrawingBrush(HDC &hDC, HWND &hWindow) {
    hdc = hDC;
    window = hWindow;
    CreateColor();
}

void DrawingBrush::CreateColor(uint8_t a, uint8_t b, uint8_t c) {
    if (color) DeleteObject(color);
    color = CreateSolidBrush(RGB(a,b,c));
}

void DrawingBrush::DrawOneDay(BYTE* ptr, int counter, ActivityData& pData) {
    /* Recursive method */
    if (!counter)
    {
        return;
    }
    int prevTime = 0, activity, activityType, activityTime, duration;
    RECT rect = {0, 0, 0, 0}, hWindowRect;
    GetClientRect(window, &hWindowRect);
    activity = (ptr[0] << 8) | ptr[1];
    activityType = (activity >> 11) & 0b11;
    activityTime = ((activity & 0b0000011111111111) % 1440) + (UTC * 60);
    prevTime = activityTime;
    rect.top = activityTime * 10; // FIRST
    // we draw first starting event as a straight line on timeline
    // that will mark begining of the insertion card although that could be dubious
    MoveToEx(hdc, hWindowRect.left + 45, rect.top, NULL);
    LineTo(hdc, hWindowRect.right - 45, rect.top);
    // we substract first event and move pointer forward
    ptr += 2;
    counter -=2;
    while (counter > 0)
    {
        activity = (ptr[0] << 8) | ptr[1];
        activityTime = ((activity & 0b0000011111111111) % 1440) + (UTC * 60);
        duration = activityTime - prevTime;
        rect.bottom = activityTime * 10; // FIRST
        switch (activityType)
        {
            case 0: // REST
            {
                rect.left = (hWindowRect.right / 16) * 3;
                rect.right = (hWindowRect.right / 16) * 4;
                pData.rest += duration;
                
                RestingFunc(pData.RecordingRest, duration);
                CreateColor(0x1F, 0xFF, 0x1F);
                FillRect(hdc, &rect, color);
                if (pData.RecordingRest == 45) pData.RecordingRest = 0, pData.Overdrive = 0;
                break;
            }
            case 1: // ADMINISTRATION
            {
                rect.left = (hWindowRect.right / 16) * 6;
                rect.right = (hWindowRect.right / 16) * 7;
                pData.administration += duration;
                RestingFunc(pData.RecordingRest, duration);
                CreateColor(0x6B, 0x6B, 0x6B);
                FillRect(hdc, &rect, color);
                if (pData.RecordingRest == 45) pData.RecordingRest = 0, pData.Overdrive = 0;
                break;
            }
            case 2: // WORK
            {
                rect.left = (hWindowRect.right / 16) * 9;
                rect.right = (hWindowRect.right / 16) * 10;
                pData.work += duration;
                CreateColor(0xFF, 0x9D, 0x00);
                FillRect(hdc, &rect, color);
                break;
            }
            case 3: // DRIVING
            {
                rect.left = (hWindowRect.right / 16) * 12;
                rect.right = (hWindowRect.right / 16) * 13;
                pData.driving += duration;
                pData.Overdrive += duration;
                CreateColor(0x00, 0xA5, 0xFF);
                FillRect(hdc, &rect, color);
                if ((pData.RecordingRest < 45) && (pData.Overdrive > 270))
                {
                    CreateColor(0xFF, 0x00, 0x00);
                    rect.top = rect.bottom - (pData.Overdrive - 270) * 10;
                    FillRect(hdc, &rect, color);
                    pData.Overdrive = 270;
                    CreateColor();
                }
                break;
            }
        }
        activityType = (activity >> 11) & 0b11; // we update activityType now as first event marks first type till...
        prevTime = activityTime;
        rect.top = activityTime * 10; // SECOND becomes FIRST
        ptr += 2;
        counter -=2;
        if (((activity >> 13) & 0b1) && (counter > 2))
        {
            DrawOneDay(ptr, counter, pData);
            break;
        }
    }
    MoveToEx(hdc, hWindowRect.left + 45, rect.bottom, NULL);
    LineTo(hdc, hWindowRect.right - 45, rect.bottom);
}

DrawingBrush::~DrawingBrush() {
    if (color) DeleteObject(color);
}

void Vehicles::readVehicles(BYTE* ptr)
{
    int no = 6200, i = 0; // generation 1 card VehiclePointerNewestRecord non-existent
    while (no)
    {
        memcpy(&ptrWrp[i], ptr, 31);
        ptrWrp[i].registration.chr[13] = '\0';
        i++;
        ptr += 31;
        no -= 31;
    }
}

Vehicles::Vehicles(BYTE* ptr)
{
    if (ptr) readVehicles(ptr);
}

Vehicles::~Vehicles()
{
    delete[] ptrWrp;
}

ActivitiesTree::ActivitiesTree(HWND& hTreeWindow, Activities& ActAdd) : hWindow(hTreeWindow), activities(ActAdd)
{
    days = new HTREEITEM[activities.lastIndex+1];
    tvis.hParent = TVI_ROOT;
    tvis.item.mask = TVIF_TEXT;
    tvis.item.pszText = TEXT("Daily Activities");
    Root = TreeView_InsertItem(hWindow, &tvis);
}

void ActivitiesTree::CreateTree()
{
    uint32_t time;
    tvis.hParent = Root;
    for (int i = 0; i < activities.lastIndex+1; i++)
    {
        if (activities.ptrWrp[i])
        {
            time = _byteswap_ulong(activities.ptrWrp[i]->header.time);
            DateStamp(time, buffer);
            MultiByteToWideChar(CP_ACP, 0, buffer, -1, bufferW, 11);
            tvis.item.mask = TVIF_TEXT | TVIF_PARAM; // text label | we are providing additional data like lParam
            tvis.item.pszText = bufferW;
            tvis.item.lParam = i;
            days[i] = TreeView_InsertItem(hWindow, &tvis);

            treemap.insert({time, &days[i]});

            tvis.hParent = days[i];
            wsprintf(bufferW, L"km: %i", _byteswap_ushort(activities.ptrWrp[i]->header.km));
            tvis.item.pszText = bufferW;
            TreeView_InsertItem(hWindow, &tvis);
            
            tvis.hParent = Root;
        }
    }
    TreeView_Expand(hWindow, Root, TVE_EXPAND);
}

void ActivitiesTree::UpdateTreeVehicles(Vehicle* ptr)
{
    /* method is specifically designed to take in vehicles DATA Wrapper Ptr */
    if (ptr)
    {
        for (int i = 0; i < 197; i++)
        {
            int num = _byteswap_ulong(ptr[i].startTime);
            auto it = treemap.find(num - (num % 86400));
            if (it != treemap.end())
            {
                tvis.hParent = *(it->second);

                tv.mask = TVIF_PARAM;
                tv.hItem = *(it->second);
                TreeView_GetItem(hWindow, &tv);
                tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
                tvis.item.lParam = tv.lParam;

                MultiByteToWideChar(CP_ACP, 0, ptr[i].registration.chr, -1, bufferW, 20);
                tvis.item.pszText = bufferW;
                HTREEITEM hVehicleNode = TreeView_InsertItem(hWindow, &tvis);
                
                tvis.hParent = hVehicleNode;

                wsprintf(bufferW, L"Start km: %d",
                    (ptr[i].startKM.bytes[0] << 16) | (ptr[i].startKM.bytes[1] << 8) | ptr[i].startKM.bytes[2]);
                tvis.item.pszText = bufferW;
                TreeView_InsertItem(hWindow, &tvis);
                
                wsprintf(bufferW, L"End km: %d",
                    (ptr[i].endKM.bytes[0] << 16) | (ptr[i].endKM.bytes[1] << 8) | ptr[i].endKM.bytes[2]);
                tvis.item.pszText = bufferW;
                TreeView_InsertItem(hWindow, &tvis);
            }
        }
    }
}

G1Card::~G1Card() {
    delete[] ICCDataptr;
    delete[] ICDataptr;
    delete[] appIdentification;
    delete[] cardCertDATAptr;
    delete[] CACertDATAptr;
    delete[] idData;
    delete[] cardDownload;
    delete[] driverLicenseDATAptr;
    delete[] eventsData;
    delete[] faultsData;
    delete[] activitiesDATAptr;
    delete[] vehiclesDATAptr;
    delete[] places;
    delete[] currentUsage;
    delete[] controlActivityData;
    delete[] specificConditions;
}

G2Card::~G2Card() {
    delete[] ICCDataptr;
    delete[] ICDataptr;
    delete[] appIdentification;
    delete[] cardCertDATAptr;
    delete[] CACertDATAptr;
    delete[] idData;
    delete[] cardDownload;
    delete[] driverLicenseDATAptr;
    delete[] eventsData;
    delete[] faultsData;
    delete[] activitiesDATAptr;
    delete[] vehiclesDATAptr;
    delete[] places;
    delete[] currentUsage;
    delete[] controlActivityData;
    delete[] specificConditions;

    delete[] cardSignCertificate;
    delete[] linkCertificate;
    delete[] vehicleUnitsUsed;
    delete[] GNSS;
}