// MIDListDlg.hpp --- "ID List" Dialog
//////////////////////////////////////////////////////////////////////////////
// RisohEditor --- Another free Win32 resource editor
// Copyright (C) 2017-2018 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "MWindowBase.hpp"
#include "MTextToText.hpp"
#include "MIdOrString.hpp"
#include "MAddResIDDlg.hpp"
#include "MModifyResIDDlg.hpp"
#include "MResizable.hpp"

class MSubclassedListView;
class MIDListDlg;

#define MYWM_IDJUMPBANG (WM_USER + 238)

MString Res_GetEntityIDText(const MString& name, INT nIDTYPE_);
std::vector<INT> GetPrefixIndexes(const MString& prefix);
MString GetAssoc(const MString& name);

//////////////////////////////////////////////////////////////////////////////

// Let the listview subclassed to get Enter key
class MSubclassedListView : public MWindowBase
{
public:
    virtual LRESULT CALLBACK
    WindowProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_GETDLGCODE)
        {
            LPMSG pMsg = (LPMSG)lParam;
            if (pMsg && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
            {
                INT iItem = ListView_GetNextItem(m_hwnd, -1, LVNI_ALL | LVNI_SELECTED);
                TCHAR szText[128];
                ListView_GetItemText(m_hwnd, iItem, 2, szText, _countof(szText));
                if (szText[0] != TEXT('L') && szText[0] != TEXT('"'))
                {
                    SendMessage(GetParent(hwnd), WM_COMMAND, ID_MODIFYRESID, (LPARAM)hwnd);
                }
            }
        }
        return DefaultProcDx();
    }
};

class MIDListDlg : public MDialogBase
{
public:
    typedef std::map<MString, MString>      assoc_map_type;
    typedef std::map<MStringA, MStringA>    id_map_type;
    HWND m_hMainWnd;
    LPWSTR m_pszResH;
    INT m_nBase;
    HWND m_hCmb1;
    HWND m_hLst1;
    BOOL m_bChanging;
    HICON m_hIconDiamond;
    MSubclassedListView m_lv;
    MResizable m_resizable;

    MIDListDlg()
        : MDialogBase(IDD_IDLIST), m_hMainWnd(NULL), m_pszResH(NULL),
          m_nBase(10), m_hLst1(NULL), m_bChanging(FALSE)
    {
        m_hIconDiamond = LoadSmallIconDx(IDI_DIAMOND);
    }

    ~MIDListDlg()
    {
        DestroyIcon(m_hIconDiamond);
    }

    static int CALLBACK
    CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        MIDListDlg *this_ = (MIDListDlg *)lParamSort;
        HWND m_hLst1 = this_->m_hLst1;

        LV_FINDINFO find;

        ZeroMemory(&find, sizeof(find));
        find.flags = LVFI_PARAM;
        find.lParam = lParam1;
        INT i1 = ListView_FindItem(m_hLst1, -1, &find);

        ZeroMemory(&find, sizeof(find));
        find.flags = LVFI_PARAM;
        find.lParam = lParam2;
        INT i2 = ListView_FindItem(m_hLst1, -1, &find);

        TCHAR sz1[64], sz2[64];
        if (i1 != -1 && i2 != -1)
        {
            ListView_GetItemText(m_hLst1, i1, 1, sz1, _countof(sz1));
            ListView_GetItemText(m_hLst1, i2, 1, sz2, _countof(sz2));
            int cmp = lstrcmp(sz1, sz2);
            if (cmp != 0)
                return cmp;

            ListView_GetItemText(m_hLst1, i1, 2, sz1, _countof(sz1));
            ListView_GetItemText(m_hLst1, i2, 2, sz2, _countof(sz2));
            MIdOrString id1(sz1);
            MIdOrString id2(sz2);
            if (id1 < id2)
                return -1;
            if (id1 > id2)
                return 1;

            ListView_GetItemText(m_hLst1, i1, 0, sz1, _countof(sz1));
            ListView_GetItemText(m_hLst1, i2, 0, sz2, _countof(sz2));
            cmp = lstrcmp(sz1, sz2);
            if (cmp != 0)
                return cmp;
        }
        return 0;
    }

    void OnCmb1(HWND hwnd)
    {
        INT iItem = ComboBox_GetCurSel(m_hCmb1);
        TCHAR szText[256];
        ComboBox_GetLBText(m_hCmb1, iItem, szText);
        SetItems(szText);
    }

    void SetItem(LPCTSTR pszIDType, const MStringA& first, const MStringA& second, const EntryBase *entry = NULL)
    {
        if (entry && entry->m_et == ET_LANG)
        {
            if (entry->m_type == RT_ICON || entry->m_type == RT_CURSOR ||
                entry->m_type == RT_STRING || entry->m_type == RT_MESSAGETABLE)
            {
                // ignore
                return;
            }
        }

        LV_ITEM item;

        MString text1 = MAnsiToText(CP_ACP, first.c_str()).c_str();
        MString text2 = GetAssoc(text1);
        MString text3 = MAnsiToText(CP_ACP, second.c_str()).c_str();
        if (text2.empty() || text2 == L"Resource.ID" || text2 == L"Unknown.ID")
        {
            if (entry)
            {
                auto nIDTYPE_ = g_db.IDTypeFromResType(entry->m_type);
                if (nIDTYPE_ != IDTYPE_UNKNOWN)
                {
                    text2 = g_db.GetName(L"RESOURCE.ID.TYPE", nIDTYPE_);
                }
                else
                {
                    if (entry->m_type.is_int())
                    {
                        text2 = g_db.GetName(L"RESOURCE", entry->m_type.m_id);
                        if (text2.empty())
                        {
                            if (m_nBase == 10)
                                text2 = mstr_dec(entry->m_type.m_id);
                            else if (m_nBase == 16)
                                text2 = mstr_hex(entry->m_type.m_id);
                            else
                                assert(0);
                        }
                    }
                    else
                    {
                        text2 = entry->m_type.c_str();
                    }
                }
            }
        }
        if (text2.empty())
            text2 = L"Unknown.ID";
        if (text2 == L"Resource.ID")
            return;

        if (pszIDType && text2.find(pszIDType) == MString::npos &&
            lstrcmp(pszIDType, LoadStringDx(IDS_ALL)) != 0)
        {
            return;
        }

        INT iItem = ListView_GetItemCount(m_hLst1);

        for (INT i = 0; i < iItem; ++i)
        {
            WCHAR szText[64];
            ZeroMemory(&item, sizeof(item));
            item.iItem = i;
            item.iSubItem = 0;
            item.mask = LVIF_TEXT;
            item.pszText = szText;
            item.cchTextMax = _countof(szText);
            ListView_GetItem(m_hLst1, &item);

            if (lstrcmpW(item.pszText, text1.c_str()) != 0)
                continue;

            item.iSubItem = 1;
            ListView_GetItem(m_hLst1, &item);
            if (wcsstr(item.pszText, text2.c_str()) == NULL)
                continue;

            item.iSubItem = 2;
            ListView_GetItem(m_hLst1, &item);
            if (lstrcmpW(item.pszText, text3.c_str()) != 0)
                continue;

            return; // there is the same item
        }

        ZeroMemory(&item, sizeof(item));
        item.iItem = iItem;
        item.mask = LVIF_TEXT | LVIF_PARAM;
        item.iSubItem = 0;
        item.pszText = &text1[0];
        item.lParam = iItem;
        ListView_InsertItem(m_hLst1, &item);

        ZeroMemory(&item, sizeof(item));
        item.iItem = iItem;
        item.mask = LVIF_TEXT;
        item.iSubItem = 1;
        item.pszText = &text2[0];
        ListView_SetItem(m_hLst1, &item);

        if (text3[0] != TEXT('"') && text3[0] != TEXT('L'))
        {
            int value = mstr_parse_int(text3.c_str(), true);

            TCHAR szText[32];
            if (m_nBase == 10)
                StringCchPrintf(szText, _countof(szText), TEXT("%d"), value);
            else if (m_nBase == 16)
                StringCchPrintf(szText, _countof(szText), TEXT("0x%X"), value);
            else
                assert(0);

            text3 = szText;
        }

        ZeroMemory(&item, sizeof(item));
        item.iItem = iItem;
        item.mask = LVIF_TEXT;
        item.iSubItem = 2;
        item.pszText = &text3[0];
        ListView_SetItem(m_hLst1, &item);
    }

    MStringW GetItemText(INT i)
    {
        MStringW ret;

        WCHAR szText[64];
        LV_ITEM item;
        ZeroMemory(&item, sizeof(item));
        item.iItem = i;
        item.iSubItem = 0;
        item.mask = LVIF_TEXT;
        item.pszText = szText;
        item.cchTextMax = _countof(szText);
        ListView_GetItem(m_hLst1, &item);

        ret = item.pszText;
        ret += L"\x7F";

        item.iSubItem = 1;
        ListView_GetItem(m_hLst1, &item);

        ret += item.pszText;
        ret += L"\x7F";

        item.iSubItem = 2;
        ListView_GetItem(m_hLst1, &item);

        ret += item.pszText;
        return ret;
    }

    void MakeUnique()
    {
        WCHAR szText[128];
        INT k = ComboBox_GetCurSel(m_hCmb1);
        ComboBox_GetLBText(m_hCmb1, k, szText);
        MStringW strText = szText;

        BOOL bAll = (strText == LoadStringDx(IDS_ALL));

        INT iItem = ListView_GetItemCount(m_hLst1);
        for (INT i = 0; i < iItem; ++i)
        {
            MStringW str = GetItemText(i);
            std::vector<MStringW> vec;
            mstr_split(vec, str, L"\x7F");
            str = vec[1];
            if (!bAll)
            {
                str = strText;
            }
            else
            {
                mstr_split(vec, str, L"/");
                std::sort(vec.begin(), vec.end());
                vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
                str = mstr_join(vec, L"/");
            }

            LV_ITEM item;
            ZeroMemory(&item, sizeof(item));
            item.mask = LVIF_TEXT;
            item.iItem = i;
            item.iSubItem = 1;
            item.pszText = &str[0];
            ListView_SetItem(m_hLst1, &item);
        }
        iItem = ListView_GetItemCount(m_hLst1);
        for (INT i = 0; i < iItem - 1; ++i)
        {
            MStringW str0 = GetItemText(i);
            MStringW str1 = GetItemText(i + 1);
            if (str0 == str1)
            {
                ListView_DeleteItem(m_hLst1, i);
                --i;
                --iItem;
            }
        }

        INT i = ComboBox_FindStringExact(m_hCmb1, -1, L"Resource.ID");
        if (i != CB_ERR)
            ComboBox_DeleteString(m_hCmb1, i);
    }

    void SetItems(LPCTSTR pszIDType = NULL)
    {
        ListView_DeleteAllItems(m_hLst1);

        if (!g_settings.bHideID)
        {
            for (auto& pair : g_settings.id_map)
            {
                SetItem(pszIDType, pair.first, pair.second);
            }
        }

        EntrySet found;
        g_res.search(found, ET_LANG);

        for (auto entry : found)
        {
            auto nIDTYPE_ = g_db.IDTypeFromResType(entry->m_type);
            if (entry->m_name.m_id)
            {
                auto strName = g_db.GetNameOfIDTypeValue(nIDTYPE_, entry->m_name.m_id);
                if (strName.empty() || g_settings.bHideID)
                {
                    if (m_nBase == 10)
                        strName = mstr_dec(entry->m_name.m_id);
                    else if (m_nBase == 16)
                        strName = mstr_hex(entry->m_name.m_id);
                    else
                        assert(0);
                }

                auto strValue = mstr_dec(entry->m_name.m_id);
                if (m_nBase == 16)
                    strValue = mstr_hex(entry->m_name.m_id);

                MWideToAnsi strNameA(CP_ACP, strName);
                MWideToAnsi strValueA(CP_ACP, strValue);
                SetItem(pszIDType, strNameA.c_str(), strValueA.c_str(), entry);
            }
            else
            {
                MWideToAnsi strNameA(CP_ACP, entry->m_name.quoted_wstr());
                SetItem(pszIDType, strNameA.c_str(), strNameA.c_str(), entry);
            }
        }

        ListView_SortItems(m_hLst1, CompareFunc, (LPARAM)this);

        if (pszIDType == NULL && !m_bChanging)
        {
            m_bChanging = TRUE;
            if (pszIDType == NULL)
            {
                ComboBox_ResetContent(m_hCmb1);
                ComboBox_AddString(m_hCmb1, LoadStringDx(IDS_ALL));
                INT i, nCount = ListView_GetItemCount(m_hLst1);
                for (i = 0; i < nCount; ++i)
                {
                    TCHAR szText[256];
                    ListView_GetItemText(m_hLst1, i, 1, szText, _countof(szText));
                    std::vector<MString> types;
                    mstr_split(types, szText, TEXT("/"));
                    for (size_t k = 0; k < types.size(); ++k)
                    {
                        INT ret = ComboBox_FindStringExact(m_hCmb1, -1, types[k].c_str());
                        if (ret == CB_ERR)
                        {
                            if (!types[k].empty())
                                ComboBox_AddString(m_hCmb1, types[k].c_str());
                        }
                    }
                }
            }
            if (pszIDType == NULL)
                ComboBox_SelectString(m_hCmb1, -1, LoadStringDx(IDS_ALL));
            m_bChanging = FALSE;
        }

        MakeUnique();
    }

    void OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
    {
        RECT rc;
        SetRect(&rc, 0, 0, 200, 15);
        MapDialogRect(hwnd, &rc);

        lpMeasureItem->itemHeight = rc.bottom - rc.top;

        GetClientRect(hwnd, &rc);
        lpMeasureItem->itemWidth = rc.right - rc.left;
    }

    void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
    {
        const INT CX_ICON_SMALL = 16;
        const INT CY_ICON_SMALL = 16;

        RECT rc = lpDrawItem->rcItem;

        SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOW));
        FillRect(lpDrawItem->hDC, &rc, (HBRUSH)(COLOR_WINDOW + 1));
        SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));

        INT x = 0;
        INT y = ((rc.top + rc.bottom) - CY_ICON_SMALL) / 2 - 1;
        if (lpDrawItem->itemState & ODS_COMBOBOXEDIT)
        {
            x += 3;
        }
        DrawIconEx(lpDrawItem->hDC, x, y, m_hIconDiamond, CX_ICON_SMALL, CY_ICON_SMALL, 0, NULL, DI_NORMAL);

        rc.left += CX_ICON_SMALL;

        SetBkMode(lpDrawItem->hDC, OPAQUE);
        if (lpDrawItem->itemState & ODS_SELECTED)
        {
            SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHT));
            FillRect(lpDrawItem->hDC, &rc, (HBRUSH)(COLOR_HIGHLIGHT + 1));
            SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
        }

        TCHAR szText[128];
        ComboBox_GetLBText(lpDrawItem->hwndItem, lpDrawItem->itemID, szText);

        InflateRect(&rc, -2, -2);
        DrawText(lpDrawItem->hDC, szText, -1, &rc, 
            DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);
        InflateRect(&rc, 2, 2);

        if (lpDrawItem->itemState & ODS_FOCUS)
        {
            DrawFocusRect(lpDrawItem->hDC, &rc);
        }
    }

    int OnCompareItem(HWND hwnd, const COMPAREITEMSTRUCT * lpCompareItem)
    {
        TCHAR szText1[128], szText2[128];
        ComboBox_GetLBText(lpCompareItem->hwndItem, lpCompareItem->itemID1, szText1);
        ComboBox_GetLBText(lpCompareItem->hwndItem, lpCompareItem->itemID2, szText2);
        return lstrcmpi(szText1, szText2);
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        m_hCmb1 = GetDlgItem(hwnd, cmb1);
        m_hLst1 = GetDlgItem(hwnd, lst1);

        m_resizable.OnParentCreate(hwnd);

        m_resizable.SetLayoutAnchor(cmb1, mzcLA_TOP_LEFT, mzcLA_TOP_RIGHT);
        m_resizable.SetLayoutAnchor(lst1, mzcLA_TOP_LEFT, mzcLA_BOTTOM_RIGHT);

        ListView_SetExtendedListViewStyle(m_hLst1, LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_lv.SubclassDx(m_hLst1);

        LV_COLUMN column;
        ZeroMemory(&column, sizeof(column));

        column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        column.fmt = LVCFMT_LEFT;
        column.cx = 160;
        column.pszText = LoadStringDx(IDS_NAME);
        column.iSubItem = 0;
        ListView_InsertColumn(m_hLst1, 0, &column);

        column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        column.fmt = LVCFMT_LEFT;
        column.cx = 120;
        column.pszText = LoadStringDx(IDS_IDTYPE);
        column.iSubItem = 1;
        ListView_InsertColumn(m_hLst1, 1, &column);

        column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        column.fmt = LVCFMT_LEFT;
        column.cx = 80;
        column.pszText = LoadStringDx(IDS_VALUE);
        column.iSubItem = 2;
        ListView_InsertColumn(m_hLst1, 2, &column);

        if (g_settings.bResumeWindowPos)
        {
            if (g_settings.nIDListLeft != CW_USEDEFAULT)
            {
                POINT pt = { g_settings.nIDListLeft, g_settings.nIDListTop };
                SetWindowPosDx(&pt);
            }
            if (g_settings.nIDListWidth != CW_USEDEFAULT)
            {
                SIZE siz = { g_settings.nIDListWidth, g_settings.nIDListHeight };
                SetWindowPosDx(NULL, &siz);
            }
        }

        SetItems();

        return TRUE;
    }

    void UpdateResHIfAsk()
    {
        if (g_settings.bAskUpdateResH)
            PostMessage(m_hMainWnd, WM_COMMAND, ID_UPDATERESHBANG, 0);
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        INT iItem;
        TCHAR szText[64];
        MString str1, str2;
        switch (id)
        {
        case cmb1:
            if (codeNotify == CBN_SELCHANGE && !m_bChanging)
            {
                m_bChanging = TRUE;
                OnCmb1(hwnd);
                m_bChanging = FALSE;
            }
            break;
        case IDCANCEL:
            DestroyWindow(hwnd);
            break;
        case ID_ADDRESID:
            {
                MAddResIDDlg dialog;
                if (dialog.DialogBoxDx(hwnd) == IDOK)
                {
                    ConstantsDB::TableType& table = g_db.m_map[L"RESOURCE.ID"];
                    INT value = mstr_parse_int(dialog.m_str2.c_str());
                    ConstantsDB::EntryType entry(dialog.m_str1, value);
                    table.push_back(entry);

                    MStringA stra1 = MTextToAnsi(CP_ACP, dialog.m_str1).c_str();
                    MStringA stra2 = MTextToAnsi(CP_ACP, dialog.m_str2).c_str();
                    g_settings.id_map.insert(std::make_pair(stra1, stra2));

                    g_settings.added_ids.insert(std::make_pair(stra1, stra2));
                    g_settings.removed_ids.erase(stra1);

                    SetItems();
                    SendMessage(m_hMainWnd, WM_COMMAND, ID_UPDATEID, 0);

                    UpdateResHIfAsk();
                }
            }
            break;
        case ID_MODIFYRESID:
            iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
            if (iItem == -1)
                break;
            ListView_GetItemText(m_hLst1, iItem, 0, szText, _countof(szText));
            str1 = szText;
            ListView_GetItemText(m_hLst1, iItem, 2, szText, _countof(szText));
            str2 = szText;
            if (szText[0] != TEXT('L') && szText[0] != TEXT('"'))
            {
                MModifyResIDDlg dialog(str1, str2);
                if (dialog.DialogBoxDx(hwnd) == IDOK)
                {
                    ConstantsDB::TableType& table = g_db.m_map[L"RESOURCE.ID"];
                    auto end = table.end();
                    for (auto it = table.begin(); it != end; ++it)
                    {
                        if (it->name == str1)
                        {
                            table.erase(it);
                            break;
                        }
                    }
                    INT value = mstr_parse_int(dialog.m_str2.c_str());
                    ConstantsDB::EntryType entry(dialog.m_str1, value);
                    table.push_back(entry);

                    MStringA stra1old = MTextToAnsi(CP_ACP, str1).c_str();
                    MStringA stra2old = MTextToAnsi(CP_ACP, str2).c_str();
                    MStringA stra1 = MTextToAnsi(CP_ACP, dialog.m_str1).c_str();
                    MStringA stra2 = MTextToAnsi(CP_ACP, dialog.m_str2).c_str();
                    g_settings.id_map.erase(stra1old);
                    g_settings.id_map.insert(std::make_pair(stra1, stra2));
                    g_settings.added_ids.erase(stra1old);
                    g_settings.added_ids.insert(std::make_pair(stra1, stra2));
                    g_settings.removed_ids.erase(stra1old);
                    g_settings.removed_ids.insert(std::make_pair(stra1old, stra2old));

                    ListView_SetItemText(m_hLst1, iItem, 0, &str1[0]);
                    MString assoc = GetAssoc(str1);
                    ListView_SetItemText(m_hLst1, iItem, 1, &assoc[0]);
                    ListView_SetItemText(m_hLst1, iItem, 2, &str2[0]);

                    SetItems();

                    SendMessage(m_hMainWnd, WM_COMMAND, ID_UPDATEID, 0);
                    UpdateResHIfAsk();
                }
            }
            break;
        case ID_DELETERESID:
            for (;;)
            {
                iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
                if (iItem == -1)
                    break;

                ListView_GetItemText(m_hLst1, iItem, 0, szText, _countof(szText));
                MString str1 = szText;
                MStringA astr1 = MTextToAnsi(CP_ACP, szText).c_str();
                ListView_GetItemText(m_hLst1, iItem, 2, szText, _countof(szText));
                MStringA astr2 = MTextToAnsi(CP_ACP, szText).c_str();

                ConstantsDB::TableType& table = g_db.m_map[L"RESOURCE.ID"];
                auto end = table.end();
                for (auto it = table.begin(); it != end; ++it)
                {
                    if (it->name == str1)
                    {
                        table.erase(it);
                        break;
                    }
                }

                g_settings.id_map.erase(astr1);
                if (!g_settings.added_ids.erase(astr1))
                {
                    g_settings.removed_ids.insert(std::make_pair(astr1, astr2));
                }

                ListView_DeleteItem(m_hLst1, iItem);
            }
            SendMessage(m_hMainWnd, WM_COMMAND, ID_UPDATEID, 0);
            UpdateResHIfAsk();
            break;
        case ID_COPYRESIDNAME:
            {
                iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
                if (iItem == -1)
                    break;
                ListView_GetItemText(m_hLst1, iItem, 0, szText, _countof(szText));
                CopyTextDx(hwnd, szText);
            }
            break;
        case ID_COPYRESIDVALUE:
            {
                iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
                if (iItem == -1)
                    break;
                ListView_GetItemText(m_hLst1, iItem, 2, szText, _countof(szText));
                MString text = szText;
                CopyTextDx(hwnd, text);
            }
            break;
        case ID_COPYIDDEF:
            {
                iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
                if (iItem == -1)
                    break;
                ListView_GetItemText(m_hLst1, iItem, 0, szText, _countof(szText));
                MString text1 = szText;
                ListView_GetItemText(m_hLst1, iItem, 2, szText, _countof(szText));
                MString text2 = szText;
                MString text = TEXT("#define ");
                text += text1;
                text += TEXT(" ");
                text += text2;
                text += TEXT("\r\n");
                CopyTextDx(hwnd, text);
            }
            break;
        case ID_LOADRESH:
            PostMessage(m_hMainWnd, WM_COMMAND, ID_LOADRESHBANG, 0);
            break;
        case ID_IDJUMP:
            OnIdJump(hwnd);
            break;
        case ID_BASE10:
            {
                m_nBase = 10;
                ComboBox_GetText(m_hCmb1, szText, _countof(szText));
                SetItems(szText);
            }
            break;
        case ID_BASE16:
            {
                m_nBase = 16;
                ComboBox_GetText(m_hCmb1, szText, _countof(szText));
                SetItems(szText);
            }
            break;
        case ID_IDJUMP00:
            OnIdJump(hwnd, 0);
            break;
        case ID_IDJUMP01:
            OnIdJump(hwnd, 1);
            break;
        case ID_IDJUMP02:
            OnIdJump(hwnd, 2);
            break;
        case ID_IDJUMP03:
            OnIdJump(hwnd, 3);
            break;
        case ID_IDJUMP04:
            OnIdJump(hwnd, 4);
            break;
        case ID_IDJUMP05:
            OnIdJump(hwnd, 5);
            break;
        case ID_IDJUMP06:
            OnIdJump(hwnd, 6);
            break;
        case ID_IDJUMP07:
            OnIdJump(hwnd, 7);
            break;
        case ID_IDJUMP08:
            OnIdJump(hwnd, 8);
            break;
        case ID_IDJUMP09:
            OnIdJump(hwnd, 9);
            break;
        }
    }

    void OnIdJump(HWND hwnd, INT nIndex = -1)
    {
        INT iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
        if (iItem == -1)
            return;

        TCHAR szText[128];
        ListView_GetItemText(m_hLst1, iItem, 1, szText, _countof(szText));
        MString str = szText;
        if (str.find(TEXT('/')) == MString::npos || nIndex == 0)
        {
            PostMessage(m_hMainWnd, MYWM_IDJUMPBANG, iItem, 0);
            return;
        }

        std::vector<MString> vecItems;
        mstr_split(vecItems, str, TEXT("/"));

        if (nIndex == -1)
        {
            HMENU hMenu = CreatePopupMenu();
            const size_t max_count = 10;
            for (size_t i = 0; i < vecItems.size() && i < max_count; ++i)
            {
                INT k = ID_IDJUMP00 + INT(i);
                InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING | MF_ENABLED, 
                    k, vecItems[i].c_str());
            }

            // get the cursor position
            POINT pt;
            GetCursorPos(&pt);

            // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms648002.aspx
            SetForegroundWindow(hwnd);

            TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
                pt.x, pt.y, 0, hwnd, NULL);

            // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms648002.aspx
            PostMessage(hwnd, WM_NULL, 0, 0);

            // destroy the menu
            DestroyMenu(hMenu);
        }
        else
        {
            PostMessage(m_hMainWnd, MYWM_IDJUMPBANG, iItem, nIndex);
        }
    }

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_MOVE, OnMove);
        HANDLE_MSG(hwnd, WM_SIZE, OnSize);
        HANDLE_MSG(hwnd, WM_CONTEXTMENU, OnContextMenu);
        HANDLE_MSG(hwnd, WM_NOTIFY, OnNotify);
        HANDLE_MSG(hwnd, WM_INITMENUPOPUP, OnInitMenuPopup);
        HANDLE_MSG(hwnd, WM_MEASUREITEM, OnMeasureItem);
        HANDLE_MSG(hwnd, WM_DRAWITEM, OnDrawItem);
        HANDLE_MSG(hwnd, WM_COMPAREITEM, OnCompareItem);
        default:
            return DefaultProcDx();
        }
    }

    void OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
    {
        if (m_nBase == 10)
        {
            CheckMenuRadioItem(hMenu, ID_BASE10, ID_BASE16, 
                ID_BASE10, MF_BYCOMMAND);
        }
        else if (m_nBase == 16)
        {
            CheckMenuRadioItem(hMenu, ID_BASE10, ID_BASE16, 
                ID_BASE16, MF_BYCOMMAND);
        }
        INT iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);

        TCHAR szText[128];
        ListView_GetItemText(m_hLst1, iItem, 2, szText, _countof(szText));
        if (iItem == -1 || szText[0] == TEXT('L') || szText[0] == TEXT('"'))
        {
            EnableMenuItem(hMenu, ID_MODIFYRESID, MF_GRAYED);
            EnableMenuItem(hMenu, ID_COPYRESIDNAME, MF_GRAYED);
            EnableMenuItem(hMenu, ID_COPYRESIDVALUE, MF_GRAYED);
            EnableMenuItem(hMenu, ID_COPYIDDEF, MF_GRAYED);
            EnableMenuItem(hMenu, ID_DELETERESID, MF_GRAYED);
        }
        else
        {
            ListView_GetItemText(m_hLst1, iItem, 0, szText, _countof(szText));
            if (mchr_is_digit(szText[0]) || szText[0] == L'-' ||
                lstrcmpW(szText, L"IDC_STATIC") == 0)
            {
                EnableMenuItem(hMenu, ID_MODIFYRESID, MF_GRAYED);
                EnableMenuItem(hMenu, ID_COPYRESIDNAME, MF_ENABLED);
                EnableMenuItem(hMenu, ID_COPYRESIDVALUE, MF_ENABLED);
                EnableMenuItem(hMenu, ID_COPYIDDEF, MF_ENABLED);
                EnableMenuItem(hMenu, ID_DELETERESID, MF_GRAYED);
            }
            else
            {
                EnableMenuItem(hMenu, ID_MODIFYRESID, MF_ENABLED);
                EnableMenuItem(hMenu, ID_COPYRESIDNAME, MF_ENABLED);
                EnableMenuItem(hMenu, ID_COPYRESIDVALUE, MF_ENABLED);
                EnableMenuItem(hMenu, ID_COPYIDDEF, MF_ENABLED);
                EnableMenuItem(hMenu, ID_DELETERESID, MF_ENABLED);
            }
        }
    }

    void OnMove(HWND hwnd, int x, int y)
    {
        assert(m_hwnd);

        if (!IsZoomed(hwnd) && !IsIconic(hwnd))
        {
            RECT rc;
            GetWindowRect(hwnd, &rc);
            g_settings.nIDListLeft = rc.left;
            g_settings.nIDListTop = rc.top;
        }
    }

    void OnSize(HWND hwnd, UINT state, int cx, int cy)
    {
        assert(m_hwnd);

        if (!IsZoomed(hwnd) && !IsIconic(hwnd))
        {
            RECT rc;
            GetWindowRect(hwnd, &rc);
            g_settings.nIDListWidth = rc.right - rc.left;
            g_settings.nIDListHeight = rc.bottom - rc.top;
        }

        m_resizable.OnSize();
    }

    void OnContextMenu(HWND hwnd, HWND hwndContext, UINT xPos, UINT yPos)
    {
        if (hwndContext == m_hLst1)
        {
            PopupMenuDx(hwnd, m_hLst1, IDR_POPUPMENUS, 3, xPos, yPos);
        }
    }

    LRESULT OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr)
    {
        if (idFrom == lst1)
        {
            if (pnmhdr->code == NM_DBLCLK)
            {
                PostMessageDx(WM_COMMAND, ID_IDJUMP);
                return 1;
            }
            if (pnmhdr->code == LVN_KEYDOWN)
            {
                LV_KEYDOWN *down = (LV_KEYDOWN *)pnmhdr;
                if (down->wVKey == VK_DELETE)
                {
                    PostMessageDx(WM_COMMAND, ID_DELETERESID);
                    return 1;
                }
                if (down->wVKey == 'C' && GetKeyState(VK_CONTROL) < 0)
                {
                    PostMessageDx(WM_COMMAND, ID_COPYIDDEF);
                    return 1;
                }
            }
            if (pnmhdr->code == LVN_GETINFOTIP)
            {
                NMLVGETINFOTIP *pGetInfoTip = (NMLVGETINFOTIP *)pnmhdr;
                INT iItem = pGetInfoTip->iItem;
                INT iSubItem = pGetInfoTip->iSubItem;
                TCHAR szText[128];
                ListView_GetItemText(m_hLst1, iItem, iSubItem, szText, _countof(szText));
                StringCchCopy(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, szText);
                return 1;
            }
        }
        return 0;
    }
};
