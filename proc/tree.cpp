#include "functions.hpp"
#include "structs.hpp"
#include <unordered_map>

struct ActivitiesTree
{
    HTREEITEM Root;
    HTREEITEM* days;
    std::unordered_map<uint32_t, HTREEITEM*> treemap;
    HWND& hWindow;
    Activities& activities;
    TVINSERTSTRUCT tvsi{};
    char buffer[11];
    wchar_t bufferW[11];
    ActivitiesTree(HWND& hTreeWindow, Activities& ActAdd) : hWindow(hTreeWindow), activities(ActAdd)
    {
        days = new HTREEITEM[activities.lastIndex+1];
        tvsi.hParent = TVI_ROOT;
        tvsi.item.mask = TVIF_TEXT;
        tvsi.item.pszText = TEXT("Daily Activities");
        Root = TreeView_InsertItem(hWindow, &tvsi);
    }

    void CreateTree()
    {
        uint32_t time;
        tvsi.hParent = Root;
        for (int i = 0; i < activities.lastIndex+1; i++)
        {
            if (activities.ptrWrp[i])
            {
                time = _byteswap_ulong(activities.ptrWrp[i]->header.time);
                DateStamp(time, buffer);
                MultiByteToWideChar(CP_ACP, 0, buffer, -1, bufferW, 11);
                tvsi.item.pszText = bufferW;
                tvsi.item.lParam = i;
                days[i] = TreeView_InsertItem(hWindow, &tvsi);
                treemap.insert({time, &days[i]});

                tvsi.hParent = days[i];
                wsprintf(bufferW, L"km: %i", _byteswap_ushort(activities.ptrWrp[i]->header.km));
                tvsi.item.pszText = bufferW;
                TreeView_InsertItem(hWindow, &tvsi);

                tvsi.hParent = Root;
            }
        }
        return;
    }
};