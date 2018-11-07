// MMessagesDlg.hpp --- "String Table" Dialog
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

#ifndef MZC4_MMESSAGESDLG_HPP_
#define MZC4_MMESSAGESDLG_HPP_

#include "MWindowBase.hpp"
#include "RisohSettings.hpp"
#include "ConstantsDB.hpp"
#include "Res.hpp"
#include "MResizable.hpp"
#include "MComboBoxAutoComplete.hpp"
#include "resource.h"

#include "MessageRes.hpp"

class MAddMsgDlg;
class MModifyMsgDlg;
class MMessagesDlg;

void InitMessageComboBox(HWND hCmb, MString strString);
BOOL MsgDlg_GetEntry(HWND hwnd, MESSAGE_ENTRY& entry);
void MsgDlg_SetEntry(HWND hwnd, MESSAGE_ENTRY& entry);

//////////////////////////////////////////////////////////////////////////////

class MAddMsgDlg : public MDialogBase
{
public:
    MESSAGE_ENTRY& m_entry;
    MessageRes& m_msg_res;
    MComboBoxAutoComplete m_cmb1;

    MAddMsgDlg(MESSAGE_ENTRY& entry, MessageRes& msg_res)
        : MDialogBase(IDD_ADDMSG), m_entry(entry), m_msg_res(msg_res)
    {
        m_cmb1.m_bAcceptSpace = TRUE;
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        HWND hCmb1 = GetDlgItem(hwnd, cmb1);
        InitMessageComboBox(hCmb1, L"");
        SubclassChildDx(m_cmb1, cmb1);

        CenterWindowDx();
        return TRUE;
    }

    void OnOK(HWND hwnd)
    {
        HWND hCmb1 = GetDlgItem(hwnd, cmb1);
        if (!MsgDlg_GetEntry(hwnd, m_entry))
        {
            ComboBox_SetEditSel(hCmb1, 0, -1);
            SetFocus(hCmb1);
            ErrorBoxDx(IDS_NOSUCHID);
            return;
        }
        INT value;
        if (g_db.HasResID(m_entry.MessageID))
        {
            value = g_db.GetResIDValue(m_entry.MessageID);
        }
        else
        {
            value = mstr_parse_int(m_entry.MessageID);
        }
        if (m_msg_res.map().find(value) != m_msg_res.map().end())
        {
            ComboBox_SetEditSel(hCmb1, 0, -1);
            SetFocus(hCmb1);
            ErrorBoxDx(IDS_ALREADYEXISTS);
            return;
        }

        EndDialog(IDOK);
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case IDOK:
            OnOK(hwnd);
            break;
        case IDCANCEL:
            EndDialog(IDCANCEL);
            break;
        case psh1:
            OnPsh1(hwnd);
            break;
        case cmb1:
            if (codeNotify == CBN_EDITCHANGE)
            {
                m_cmb1.OnEditChange();
            }
            break;
        }
    }

    void OnPsh1(HWND hwnd)
    {
        SendMessage(GetParent(GetParent(hwnd)), WM_COMMAND, ID_IDLIST, 0);
    }

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
            HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        }
        return 0;
    }
};

//////////////////////////////////////////////////////////////////////////////

class MModifyMsgDlg : public MDialogBase
{
public:
    MESSAGE_ENTRY& m_entry;
    MessageRes& m_msg_res;
    MComboBoxAutoComplete m_cmb1;

    MModifyMsgDlg(MESSAGE_ENTRY& entry, MessageRes& msg_res)
        : MDialogBase(IDD_MODIFYMSG), m_entry(entry), m_msg_res(msg_res)
    {
        m_cmb1.m_bAcceptSpace = TRUE;
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        HWND hCmb1 = GetDlgItem(hwnd, cmb1);
        InitMessageComboBox(hCmb1, L"");
        SubclassChildDx(m_cmb1, cmb1);

        MsgDlg_SetEntry(hwnd, m_entry);
        CenterWindowDx();
        return TRUE;
    }

    void OnOK(HWND hwnd)
    {
        if (!MsgDlg_GetEntry(hwnd, m_entry))
        {
            ErrorBoxDx(IDS_NOSUCHID);
            return;
        }
        EndDialog(IDOK);
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case IDOK:
            OnOK(hwnd);
            break;
        case IDCANCEL:
            EndDialog(IDCANCEL);
            break;
        case psh1:
            OnPsh1(hwnd);
            break;
        case cmb1:
            if (codeNotify == CBN_EDITCHANGE)
            {
                m_cmb1.OnEditChange();
            }
            break;
        }
    }

    void OnPsh1(HWND hwnd)
    {
        SendMessage(GetParent(GetParent(hwnd)), WM_COMMAND, ID_IDLIST, 0);
    }

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
            HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        }
        return DefaultProcDx();
    }
};

//////////////////////////////////////////////////////////////////////////////

class MMessagesDlg : public MDialogBase
{
public:
    MessageRes m_msg_res;
    MResizable m_resizable;
    HICON m_hIcon;
    HICON m_hIconSm;
    HWND m_hLst1;

    MMessagesDlg(MessageRes& msg_res)
        : MDialogBase(IDD_MESSAGES), m_msg_res(msg_res)
    {
        m_hIcon = LoadIconDx(IDI_SMILY);
        m_hIconSm = LoadSmallIconDx(IDI_SMILY);
    }

    ~MMessagesDlg()
    {
        DestroyIcon(m_hIcon);
        DestroyIcon(m_hIconSm);
    }

    void InitCtl1(HWND hLst1)
    {
        ListView_DeleteAllItems(m_hLst1);

        typedef MessageRes::map_type map_type;
        const map_type& map = m_msg_res.map();

        INT i = 0;
        for (auto& pair : map)
        {
            if (pair.second.empty())
                continue;

            MStringW str = g_db.GetNameOfResID(IDTYPE_MESSAGE, pair.first);

            LV_ITEM item;
            ZeroMemory(&item, sizeof(item));
            item.iItem = i;
            item.mask = LVIF_TEXT;
            item.iSubItem = 0;
            item.pszText = &str[0];
            ListView_InsertItem(m_hLst1, &item);

            str = mstr_quote(pair.second);

            ZeroMemory(&item, sizeof(item));
            item.iItem = i;
            item.mask = LVIF_TEXT;
            item.iSubItem = 1;
            item.pszText = &str[0];
            ListView_SetItem(m_hLst1, &item);

            ++i;
        }
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        m_hLst1 = GetDlgItem(hwnd, lst1);
        ListView_SetExtendedListViewStyle(m_hLst1, LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

        LV_COLUMN column;
        ZeroMemory(&column, sizeof(column));

        column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        column.fmt = LVCFMT_LEFT;
        column.cx = 140;
        column.pszText = LoadStringDx(IDS_MESSAGEID);
        column.iSubItem = 0;
        ListView_InsertColumn(m_hLst1, 0, &column);

        column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        column.fmt = LVCFMT_LEFT;
        column.cx = 500;
        column.pszText = LoadStringDx(IDS_MESSAGEVALUE);
        column.iSubItem = 1;
        ListView_InsertColumn(m_hLst1, 1, &column);

        InitCtl1(m_hLst1);

        UINT state = LVIS_SELECTED | LVIS_FOCUSED;
        ListView_SetItemState(m_hLst1, 0, state, state);
        SetFocus(m_hLst1);

        m_resizable.OnParentCreate(hwnd);

        m_resizable.SetLayoutAnchor(lst1, mzcLA_TOP_LEFT, mzcLA_BOTTOM_RIGHT);
        m_resizable.SetLayoutAnchor(psh1, mzcLA_TOP_RIGHT);
        m_resizable.SetLayoutAnchor(psh2, mzcLA_TOP_RIGHT);
        m_resizable.SetLayoutAnchor(psh3, mzcLA_TOP_RIGHT);
        m_resizable.SetLayoutAnchor(psh4, mzcLA_BOTTOM_LEFT);
        m_resizable.SetLayoutAnchor(IDOK, mzcLA_BOTTOM_RIGHT);
        m_resizable.SetLayoutAnchor(IDCANCEL, mzcLA_BOTTOM_RIGHT);

        SendMessageDx(WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);
        SendMessageDx(WM_SETICON, ICON_SMALL, (LPARAM)m_hIconSm);

        CenterWindowDx();
        return TRUE;
    }

    void OnAdd(HWND hwnd)
    {
        MESSAGE_ENTRY me;
        ZeroMemory(&me, sizeof(me));
        MAddMsgDlg dialog(me, m_msg_res);
        if (dialog.DialogBoxDx(hwnd) != IDOK)
        {
            return;
        }

        INT iItem;

        LV_FINDINFO find;
        TCHAR sz[128];
        StringCchCopy(sz, _countof(sz), me.MessageID);
        ZeroMemory(&find, sizeof(find));
        find.flags = LVFI_STRING;
        find.psz = sz;
        iItem = ListView_FindItem(m_hLst1, -1, &find);
        if (iItem != -1)
        {
            ListView_DeleteItem(m_hLst1, iItem);
        }

        LV_ITEM item;
        iItem = ListView_GetItemCount(m_hLst1);

        ZeroMemory(&item, sizeof(item));
        item.iItem = iItem;
        item.mask = LVIF_TEXT;
        item.iSubItem = 0;
        item.pszText = me.MessageID;
        ListView_InsertItem(m_hLst1, &item);

        std::wstring str = me.MessageValue;
        str = mstr_quote(str);

        ZeroMemory(&item, sizeof(item));
        item.iItem = iItem;
        item.mask = LVIF_TEXT;
        item.iSubItem = 1;
        item.pszText = &str[0];
        ListView_SetItem(m_hLst1, &item);

        UINT state = LVIS_SELECTED | LVIS_FOCUSED;
        ListView_SetItemState(m_hLst1, iItem, state, state);
        ListView_EnsureVisible(m_hLst1, iItem, FALSE);

        ULONG dwValue = g_db.GetResIDValue(me.MessageID);
        m_msg_res.m_map[dwValue] = me.MessageValue;
    }

    void GetEntry(HWND hwnd, INT iItem, MESSAGE_ENTRY& entry)
    {
        ListView_GetItemText(m_hLst1, iItem, 0, entry.MessageID, _countof(entry.MessageID));
        mstr_trim(entry.MessageID);

        ListView_GetItemText(m_hLst1, iItem, 1, entry.MessageValue, _countof(entry.MessageValue));
        mstr_trim(entry.MessageValue);
        if (entry.MessageValue[0] == L'"')
        {
            mstr_unquote(entry.MessageValue);
        }
    }

    void OnDelete(HWND hwnd)
    {
        INT iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
        if (iItem >= 0)
        {
            MESSAGE_ENTRY me;
            GetEntry(hwnd, iItem, me);

            ULONG dwValue = g_db.GetResIDValue(me.MessageID);
            m_msg_res.m_map.erase(dwValue);

            ListView_DeleteItem(m_hLst1, iItem);
        }
    }

    void OnDeleteAll(HWND hwnd)
    {
        m_msg_res.m_map.clear();
        ListView_DeleteAllItems(m_hLst1);
    }

    void OnModify(HWND hwnd)
    {
        INT iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
        if (iItem < 0)
        {
            return;
        }

        MESSAGE_ENTRY me;
        GetEntry(hwnd, iItem, me);

        MModifyMsgDlg dialog(me, m_msg_res);
        if (IDOK == dialog.DialogBoxDx(hwnd))
        {
            MString str = mstr_quote(me.MessageValue);
            ListView_SetItemText(m_hLst1, iItem, 1, &str[0]);

            ULONG dwValue = g_db.GetResIDValue(me.MessageID);
            m_msg_res.m_map[dwValue] = str;
        }
    }

    void OnOK(HWND hwnd)
    {
        INT iItem, nCount = ListView_GetItemCount(m_hLst1);
        if (nCount == 0)
        {
            ErrorBoxDx(IDS_DATAISEMPTY);
            return;
        }

        m_msg_res.map().clear();

        MESSAGE_ENTRY me;
        for (iItem = 0; iItem < nCount; ++iItem)
        {
            GetEntry(hwnd, iItem, me);

            DWORD dwID = g_db.GetResIDValue(me.MessageID);
            std::wstring str = me.MessageValue;

            m_msg_res.map().insert(std::make_pair(dwID, str));
        }

        EndDialog(IDOK);
    }

    void OnContextMenu(HWND hwnd, HWND hwndContext, UINT xPos, UINT yPos)
    {
        if (hwndContext == m_hLst1)
        {
            PopupMenuDx(hwnd, m_hLst1, IDR_POPUPMENUS, 4, xPos, yPos);
        }
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case psh1:
        case ID_ADD:
            OnAdd(hwnd);
            break;
        case psh2:
        case ID_MODIFY:
            OnModify(hwnd);
            break;
        case psh3:
        case ID_DELETE:
            OnDelete(hwnd);
            break;
        case psh4:
            OnDeleteAll(hwnd);
            break;
        case IDOK:
            OnOK(hwnd);
            break;
        case IDCANCEL:
            EndDialog(IDCANCEL);
            break;
        }
    }

    LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnmhdr)
    {
        if (idFrom == lst1)
        {
            if (pnmhdr->code == LVN_KEYDOWN)
            {
                LV_KEYDOWN *KeyDown = (LV_KEYDOWN *)pnmhdr;
                if (KeyDown->wVKey == VK_DELETE)
                {
                    OnDelete(hwnd);
                    return 0;
                }
            }
            if (pnmhdr->code == NM_DBLCLK)
            {
                OnModify(hwnd);
                return 0;
            }
            if (pnmhdr->code == LVN_GETINFOTIP)
            {
                NMLVGETINFOTIP *pGetInfoTip = (NMLVGETINFOTIP *)pnmhdr;
                INT iItem = pGetInfoTip->iItem;
                INT iSubItem = pGetInfoTip->iSubItem;
                TCHAR szText[128];
                ListView_GetItemText(m_hLst1, iItem, iSubItem, szText, _countof(szText));
                StringCchCopy(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, szText);
            }
            if (pnmhdr->code == LVN_ITEMCHANGED)
            {
                //NM_LISTVIEW *pListView = (NM_LISTVIEW *)pnmhdr;
                OnItemChanged(hwnd);
            }
        }
        return 0;
    }

    void OnSize(HWND hwnd, UINT state, int cx, int cy)
    {
        m_resizable.OnSize();
    }

    void OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
    {
        INT iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
        if (iItem >= 0)
        {
            EnableMenuItem(hMenu, ID_MODIFY, MF_BYCOMMAND | MF_ENABLED);
            EnableMenuItem(hMenu, ID_DELETE, MF_BYCOMMAND | MF_ENABLED);
        }
        else
        {
            EnableMenuItem(hMenu, ID_MODIFY, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenu, ID_DELETE, MF_BYCOMMAND | MF_GRAYED);
        }
        EnableMenuItem(hMenu, ID_RENAME, MF_BYCOMMAND | MF_GRAYED);
    }

    void OnItemChanged(HWND hwnd)
    {
        INT iItem = ListView_GetNextItem(m_hLst1, -1, LVNI_ALL | LVNI_SELECTED);
        BOOL bSelected = (iItem != -1);
        EnableWindow(GetDlgItem(hwnd, psh2), bSelected);
        EnableWindow(GetDlgItem(hwnd, psh3), bSelected);
    }

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
            HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
            HANDLE_MSG(hwnd, WM_NOTIFY, OnNotify);
            HANDLE_MSG(hwnd, WM_SIZE, OnSize);
            HANDLE_MSG(hwnd, WM_CONTEXTMENU, OnContextMenu);
            HANDLE_MSG(hwnd, WM_INITMENUPOPUP, OnInitMenuPopup);
        }
        return DefaultProcDx();
    }
};

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef MZC4_MMESSAGESDLG_HPP_
