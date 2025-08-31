#include "structs.hpp"
#include "functions.hpp"

LPCSTR ID::BirthDay()
{
    static char buffer[10];
    sprintf(buffer, "%02X%02X.%02X.%02X", birthday[0], birthday[1], birthday[2], birthday[3]);
    return buffer;
}

LPCSTR ID::Date(uint32_t& variable)
{
    static char buffer[10];
    int epoch = _byteswap_ulong(variable), year = 1970, month = 1, day = 1;
    DateStamp(epoch, year, month, day);
    sprintf(buffer, "%04i.%02i.%02i", year, month, day);
    return buffer;
}

void ID::nullterminator()
{
    cardNumber[17] = '\0';
    issuer[34] = '\0';
    date0 = '\0';
    surname[35] = '\0';
    name[34] = '\0';
    country[1] = country[0];
    country[0] = end1;
    end1 = '\0';
    end2 = '\0';
    //birthday[0] -= 6 * (birthday[0] % 10); // 6*firstdigit number
    //birthday[1] -= 6 * (birthday[1] % 10);
    //birthday[2] -= 6 * (birthday[2] % 10);
    //birthday[3] -= 6 * (birthday[3] % 10);
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

Activities::Activities(BYTE* ptr, uint16_t end, uint16_t start) {
    readActivities(ptr, end, start);
}

void Activities::readActivities(BYTE* ptr, uint16_t end, uint16_t start) {
    int st = 0;
    while (start != end) {
        ptrWrp[st] = new DailyWrapper(ptr + start);
        start -= _byteswap_ushort(ptrWrp[st]->header.prevLength);
        st++;
        if (start < 0) start += 13776;
    }
    lastIndex = st;
}

Activities::~Activities() {
    for (int i = 0; i < 365; i++) {
        if (ptrWrp[i]) {
            delete ptrWrp[i];
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
    if (!counter)
    {
        return;
    }
    int prevTime = 0, activity, activityType, activityTime, duration;
    RECT rect = {0, 0, 0, 0}, hWindowRect;
    GetClientRect(window, &hWindowRect);
    activity = (ptr[0] << 8) | ptr[1];
    activityType = (activity >> 11) & 0b11;
    activityTime = ((activity & 0b0000011111111111) % 1440);
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
        activityTime = ((activity & 0b0000011111111111) % 1440);
        duration = activityTime - prevTime;
        rect.bottom = activityTime * 10; // FIRST
        switch (activityType)
        {
            case 0: // REST
            {
                rect.left = (hWindowRect.right / 16) * 3;
                rect.right = (hWindowRect.right / 16) * 4;
                pData.rest += duration;
                
                if (!pData.RecordingRest && (duration < 45))
                {
                    pData.RecordingRest += 15;
                }
                else if (pData.RecordingRest && (duration <= 45) && (duration >= 30))
                {
                    pData.RecordingRest += 30;
                }
                else if (pData.RecordingRest && (duration >= 45))
                {
                    pData.RecordingRest += 30;
                }
                else if (!pData.RecordingRest && (duration >= 45))
                {
                    pData.RecordingRest += 45;
                }
                FillRect(hdc, &rect, color);                    
                if (pData.RecordingRest == 45) pData.RecordingRest = 0, pData.Overdrive = 0;
                break;
            }
            case 1: // ADMINISTRATION
            {
                rect.left = (hWindowRect.right / 16) * 6;
                rect.right = (hWindowRect.right / 16) * 7;
                pData.administration += duration;
                FillRect(hdc, &rect, color);
                break;
            }
            case 2: // WORK
            {
                rect.left = (hWindowRect.right / 16) * 9;
                rect.right = (hWindowRect.right / 16) * 10;
                pData.work += duration;
                FillRect(hdc, &rect, color);
                break;
            }
            case 3: // DRIVING
            {
                rect.left = (hWindowRect.right / 16) * 12;
                rect.right = (hWindowRect.right / 16) * 13;
                pData.driving += duration;
                pData.Overdrive += duration;
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