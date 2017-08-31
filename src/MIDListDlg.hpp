#ifndef MZC4_MIDLISTDLG_HPP_
#define MZC4_MIDLISTDLG_HPP_

#include "RisohEditor.hpp"
#include "MWindowBase.hpp"
#include "MTextToText.hpp"
#include "id_string.hpp"
#include "resource.h"
#include "MAddResIDDlg.hpp"
#include "MModifyResIDDlg.hpp"

class MIDListDlg : public MDialogBase
{
public:
    typedef std::map<MString, MString>      assoc_map_type;
    typedef std::map<MStringA, MStringA>    id_map_type;
    RisohSettings& m_settings;
    ConstantsDB& m_db;

    MIDListDlg(ConstantsDB& db, RisohSettings& settings)
        : MDialogBase(IDD_IDLIST), m_db(db), m_settings(settings)
    {
    }

    MString GetAssoc(const MString& name)
    {
        MString str;
        assoc_map_type::const_iterator it, end = m_settings.assoc_map.end();
        for (it = m_settings.assoc_map.begin(); it != end; ++it)
        {
            if (name.find(it->second) == 0)
            {
                if (str.empty())
                {
                    str = it->first;
                }
                else
                {
                    str += TEXT("/");
                    str += it->first;
                }
            }
        }
        return str;
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
        }
        return 0;
    }

    void SetItems()
    {
        ListView_DeleteAllItems(m_hLst1);

        INT iItem = 0;
        id_map_type::const_iterator it, end = m_settings.id_map.end();
        for (it = m_settings.id_map.begin(); it != end; ++it)
        {
            LV_ITEM item;

            MString text1 = MAnsiToText(it->first.c_str()).c_str();
            MString text2 = GetAssoc(text1);
            MString text3 = MAnsiToText(it->second.c_str()).c_str();
            if (text2.empty())
                continue;

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

            ZeroMemory(&item, sizeof(item));
            item.iItem = iItem;
            item.mask = LVIF_TEXT;
            item.iSubItem = 2;
            item.pszText = &text3[0];
            ListView_SetItem(m_hLst1, &item);

            ++iItem;
        }

        ListView_SortItems(m_hLst1, CompareFunc, (LPARAM)this);
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        m_hLst1 = GetDlgItem(hwnd, lst1);
        ListView_SetExtendedListViewStyle(m_hLst1, LVS_EX_FULLROWSELECT);

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
        column.cx = 90;
        column.pszText = LoadStringDx(IDS_IDTYPE);
        column.iSubItem = 1;
        ListView_InsertColumn(m_hLst1, 1, &column);

        column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        column.fmt = LVCFMT_LEFT;
        column.cx = 80;
        column.pszText = LoadStringDx(IDS_VALUE);
        column.iSubItem = 2;
        ListView_InsertColumn(m_hLst1, 2, &column);

        if (m_settings.bResumeWindowPos)
        {
            if (m_settings.nIDListLeft != CW_USEDEFAULT)
            {
                POINT pt = { m_settings.nIDListLeft, m_settings.nIDListTop };
                SetWindowPosDx(hwnd, &pt);
            }
            if (m_settings.nIDListWidth != CW_USEDEFAULT)
            {
                SIZE siz = { m_settings.nIDListWidth, m_settings.nIDListHeight };
                SetWindowPosDx(hwnd, NULL, &siz);
            }
        }

        return TRUE;
    }

    BOOL CopyText(HWND hwnd, const MString& text)
    {
#ifdef UNICODE
        UINT CF_ = CF_UNICODETEXT;
#else
        UINT CF_ = CF_TEXT;
#endif
        DWORD size = (text.size() + 1) * sizeof(TCHAR);
        LPTSTR psz = (LPTSTR)GlobalAllocPtr(GMEM_SHARE | GMEM_MOVEABLE, size);
        if (psz)
        {
            HGLOBAL hGlobal = GlobalPtrHandle(psz);
            CopyMemory(psz, text.c_str(), size);
            GlobalUnlockPtr(psz);

            if (OpenClipboard(hwnd))
            {
                EmptyClipboard();
                SetClipboardData(CF_, hGlobal);
                return CloseClipboard();
            }
        }
        return FALSE;
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        INT iItem;
        TCHAR szText[64];
        MString str1, str2;
        switch (id)
        {
        case IDCANCEL:
            ::DestroyWindow(hwnd);
            break;
        case CMDID_ADDRESID:
            {
                MAddResIDDlg dialog(m_settings, m_db);
                if (dialog.DialogBoxDx(hwnd) == IDOK)
                {
                    ConstantsDB::TableType& table = m_db.m_map[L"RESOURCE.ID"];
                    INT value = wcstol(dialog.m_str2.c_str(), NULL, 0);
                    ConstantsDB::EntryType entry(dialog.m_str1, value);
                    table.push_back(entry);

                    MStringA stra1 = MTextToAnsi(dialog.m_str1).c_str();
                    MStringA stra2 = MTextToAnsi(dialog.m_str2).c_str();
                    m_settings.id_map.insert(std::make_pair(stra1, stra2));

                    m_settings.added_ids.insert(std::make_pair(stra1, stra2));
                    m_settings.removed_ids.erase(stra1);

                    SetItems();
                }
            }
            break;
        case CMDID_MODIFYRESID:
            iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
            ListView_GetItemText(m_hLst1, iItem, 0, szText, _countof(szText));
            str1 = szText;
            ListView_GetItemText(m_hLst1, iItem, 2, szText, _countof(szText));
            str2 = szText;
            {
                MModifyResIDDlg dialog(m_db, str1, str2);
                if (dialog.DialogBoxDx(hwnd) == IDOK)
                {
                    ConstantsDB::TableType& table = m_db.m_map[L"RESOURCE.ID"];
                    ConstantsDB::TableType::iterator it, end = table.end();
                    for (it = table.begin(); it != end; ++it)
                    {
                        if (it->name == str1)
                        {
                            table.erase(it);
                            break;
                        }
                    }
                    INT value = wcstol(dialog.m_str2.c_str(), NULL, 0);
                    ConstantsDB::EntryType entry(dialog.m_str1, value);
                    table.push_back(entry);

                    MStringA stra1old = MTextToAnsi(str1).c_str();
                    MStringA stra2old = MTextToAnsi(str2).c_str();
                    MStringA stra1 = MTextToAnsi(dialog.m_str1).c_str();
                    MStringA stra2 = MTextToAnsi(dialog.m_str2).c_str();
                    m_settings.id_map.erase(stra1old);
                    m_settings.id_map.insert(std::make_pair(stra1, stra2));
                    m_settings.added_ids.erase(stra1old);
                    m_settings.added_ids.insert(std::make_pair(stra1, stra2));
                    m_settings.removed_ids.erase(stra1old);
                    m_settings.removed_ids.insert(std::make_pair(stra1old, stra2old));

                    SetItems();
                }
            }
            break;
        case CMDID_DELETERESID:
            {
                iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
                ListView_GetItemText(m_hLst1, iItem, 0, szText, _countof(szText));
                MString str1 = szText;
                MStringA astr1 = MTextToAnsi(szText).c_str();
                ListView_GetItemText(m_hLst1, iItem, 2, szText, _countof(szText));
                MStringA astr2 = MTextToAnsi(szText).c_str();

                ConstantsDB::TableType& table = m_db.m_map[L"RESOURCE.ID"];
                ConstantsDB::TableType::iterator it, end = table.end();
                for (it = table.begin(); it != end; ++it)
                {
                    if (it->name == str1)
                    {
                        table.erase(it);
                        break;
                    }
                }

                m_settings.id_map.erase(astr1);
                if (!m_settings.added_ids.erase(astr1))
                {
                    m_settings.removed_ids.insert(std::make_pair(astr1, astr2));
                }
                SetItems();
            }
            break;
        case CMDID_COPYRESIDNAME:
            {
                iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
                ListView_GetItemText(m_hLst1, iItem, 0, szText, _countof(szText));
                CopyText(hwnd, szText);
            }
            break;
        case CMDID_COPYIDDEF:
            {
                iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
                ListView_GetItemText(m_hLst1, iItem, 0, szText, _countof(szText));
                MString text1 = szText;
                ListView_GetItemText(m_hLst1, iItem, 2, szText, _countof(szText));
                MString text2 = szText;
                MString text = TEXT("#define ");
                text += text1;
                text += TEXT(" ");
                text += text2;
                text += TEXT("\r\n");
                CopyText(hwnd, text);
            }
            break;
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
        default:
            return DefaultProcDx();
        }
    }

    void OnMove(HWND hwnd, int x, int y)
    {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        m_settings.nIDListLeft = rc.left;
        m_settings.nIDListTop = rc.top;
    }

    void OnSize(HWND hwnd, UINT state, int cx, int cy)
    {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        m_settings.nIDListWidth = rc.right - rc.left;
        m_settings.nIDListHeight = rc.bottom - rc.top;

        MoveWindow(m_hLst1, 0, 0, cx, cy, TRUE);
    }

    void OnContextMenu(HWND hwnd, HWND hwndContext, UINT xPos, UINT yPos)
    {
        if (hwndContext == m_hLst1)
        {
            HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(2));
            HMENU hSubMenu = GetSubMenu(hMenu, 3);

            SetForegroundWindow(hwnd);
            TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                xPos, yPos, 0, hwnd, NULL);
            PostMessage(hwnd, WM_NULL, 0, 0);
        }
    }

    LRESULT OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr)
    {
        if (idFrom == lst1)
        {
            if (pnmhdr->code == NM_DBLCLK || pnmhdr->code == NM_RETURN)
            {
                PostMessageDx(WM_COMMAND, CMDID_MODIFYRESID);
                return 1;
            }
            if (pnmhdr->code == LVN_KEYDOWN)
            {
                LV_KEYDOWN *down = (LV_KEYDOWN *)pnmhdr;
                if (down->wVKey == VK_DELETE)
                {
                    PostMessageDx(WM_COMMAND, CMDID_DELETERESID);
                    return 1;
                }
            }
        }
        return 0;
    }

protected:
    HWND m_hLst1;
};

#endif  // ndef MZC4_MIDLISTDLG_HPP_